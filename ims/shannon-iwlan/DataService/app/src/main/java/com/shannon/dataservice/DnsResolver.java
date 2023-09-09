/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import android.telephony.Rlog;

import com.android.internal.annotations.VisibleForTesting;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executor;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.TimeUnit;

import static android.net.DnsResolver.ERROR_PARSE;
import static android.net.DnsResolver.ERROR_SYSTEM;
import static android.net.DnsResolver.FLAG_EMPTY;

public class DnsResolver {
    private static final boolean DBG = true;

    public static final int DNS_HEADER_LENGTH = 12;
    public static final int DNS_QUERY_LENGTH = 4;
    public static final int DNS_TYPE_NAPTR = 35;

    private enum DnsResponseCode {
        DNS_RCODE_SUCCESS(0),
        DNS_RCODE_FORMAT_ERROR(1),
        DNS_RCODE_SERVER_FAILURE(2),
        DNS_RCODE_NAME_ERROR(3),
        DNS_RCODE_NOT_IMPLEMENTED(4),
        DNS_RCODE_REFUSED(5),
        DNS_RCODE_YXDOMAIN(6),
        DNS_RCODE_YXRRSET(7),
        DNS_RCODE_NXRRSET(8),
        DNS_RCODE_NOT_AUTH(9),
        DNS_RCODE_NOT_ZONE(10),
        DNS_RCODE_BAD_VERS(16),
        DNS_RCODE_BAD_KEY(17),
        DNS_RCODE_BAD_TIME(18),
        DNS_RCODE_BAD_MODE(19),
        DNS_RCODE_BAD_NAME(20),
        DNS_RCODE_BAD_ALG(21),
        DNS_RCODE_BAD_TRUNC(22),
        DNS_RCODE_UNKNOWN(0xFFFF);

        private final int mCode;

        DnsResponseCode(int code) { mCode = code; }

        public static DnsResponseCode getEnum(int code) {
            for (DnsResponseCode rcode : DnsResponseCode.values()) {
                if (rcode.mCode == code) {
                    return rcode;
                }
            }
            return DNS_RCODE_UNKNOWN;
        }
    }

    protected class DnsRecordExpiredCallback implements DnsResourceRecord.DnsResourceCallback {
        public void onResourceExpired(DnsResourceRecord record) {
            if (record instanceof DnsNaptrRecord) {
                mNaptrRecords.remove(record);
            }
        }
    }

    protected class DnsResolverCallback implements android.net.DnsResolver.Callback<byte[]> {
        private static final int ERROR_SUCCESS = 100;
        private static final int ERROR_RESPONSE = 101;
        private static final int ERROR_UNEXPECTED = 102;

        private final CountDownLatch mLatch;
        private final DnsQuery mQuery;
        private int mErrorCode;

        DnsResolverCallback(DnsQuery query, CountDownLatch latch) {
            super();

            mQuery = query;
            mLatch = latch;
            mErrorCode = ERROR_SUCCESS;
        }

        public void onAnswer(byte[] answer, int rcode) {
            DnsResponseCode responseCode = DnsResponseCode.getEnum(rcode);
            log("Query response: " + responseCode);

            if (responseCode == DnsResponseCode.DNS_RCODE_SUCCESS) {
                try {
                    DnsResponse response = new DnsResponse(answer);
                    if (response.isExpected(mQuery.getRequestId())) {
                        byte[] data = response.getNextAnswer();
                        while (data != null) {
                            String name = response.getDomainName(data, 0);
                            int offset = response.getTypeOffset(data, 0);

                            log("Response for " + name);
                            if (mQuery.getDomain().equalsIgnoreCase(name)) {
                                int type = StringUtils.getShortAsInt(data, offset);
                                if (type == mQuery.getType()) {
                                    switch (type) {
                                        case DNS_TYPE_NAPTR:
                                            processNaptrRecord(data, offset + 4);
                                            break;
                                        default:;
                                    }
                                } else {
                                    loge("Type not matched: " + type + " expected: " + mQuery.getType());
                                }
                            }
                            data = response.getNextAnswer();
                        }
                    } else {
                        loge("Unexpected DNS response");
                        mErrorCode = ERROR_UNEXPECTED;
                    }
                } catch (Exception e) {
                    loge("Failed to parse response: " + e);
                    mErrorCode = ERROR_PARSE;
                }
            } else {
                mErrorCode = ERROR_RESPONSE;
            }
            mLatch.countDown();
        }

        public void onError(android.net.DnsResolver.DnsException error) {
            mErrorCode = error.code;

            loge("Query failed. Response: " + mErrorCode);
            mLatch.countDown();
        }

        public boolean isSuccessful() {
            switch (mErrorCode) {
                case ERROR_PARSE:
                case ERROR_SYSTEM:
                case ERROR_RESPONSE:
                case ERROR_UNEXPECTED:
                    return false;
            }
            return true;
        }

        private void processNaptrRecord(byte[] input, int offset) {
            int ttl = StringUtils.getInt(input, offset);
            int length = StringUtils.getShortAsInt(input, offset + 4);

            DnsNaptrRecord record = new DnsNaptrRecord(new DnsRecordExpiredCallback(), ttl, Arrays.copyOfRange(input, offset + 6, input.length));
            log("Received: " + record);

            mNaptrRecords.add(record);
        }
    }

    protected class DnsResolverExecutor implements Executor {
        public void execute(Runnable r) {
            r.run();
        }
    }

    private final String TAG;
    private final PriorityBlockingQueue<DnsNaptrRecord> mNaptrRecords;

    public DnsResolver(int slotId) {
        TAG = ShannonDataService.getServiceTAG("-DNS-" + slotId);
        mNaptrRecords = new PriorityBlockingQueue<>();
    }

    public void destroy() {
        if (!mNaptrRecords.isEmpty()) {
            for (DnsNaptrRecord record : mNaptrRecords) {
                record.destroy();
            }
            mNaptrRecords.clear();
        }
    }

    public LinkedList<String> doReplacementLookup(String domain) {
        log("Find replacement for " + domain);

        LinkedList<String> replacement = getReplacement();
        if (replacement != null && !replacement.isEmpty()) {
            return replacement;
        }

        CountDownLatch latch = new CountDownLatch(1);
        DnsQuery query = new DnsQuery();
        DnsResolverCallback callback = new DnsResolverCallback(query, latch);

        android.net.DnsResolver resolver = android.net.DnsResolver.getInstance();
        resolver.rawQuery(null, query.getNaptrQuery(domain), FLAG_EMPTY,
                new DnsResolverExecutor(), null, callback);
        try {
            if (latch.await(1, TimeUnit.SECONDS)) {
                if (callback.isSuccessful()) {
                    replacement = getReplacement();
                }
            } else {
                loge("NAPTR query timeout");
            }
        } catch (Exception e) {
            loge("Failed to wait NAPTR query response");
        }
        return replacement;
    }

    private LinkedList<String> getReplacement() {
        if (mNaptrRecords.isEmpty()) {
            return null;
        }
        DnsNaptrRecord[] recordsArray = mNaptrRecords.toArray(new DnsNaptrRecord[0]);
        Arrays.sort(recordsArray);

        LinkedList<String> result = new LinkedList<>();
        for (DnsNaptrRecord r : recordsArray) {
            result.add(r.getReplacement());
        }
        return result;
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }

    @VisibleForTesting
    protected PriorityBlockingQueue<DnsNaptrRecord> getNaptrRecords() {
        return mNaptrRecords;
    }
}
