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

import static com.shannon.dataservice.DnsResolver.DNS_HEADER_LENGTH;
import static com.shannon.dataservice.DnsResolver.DNS_QUERY_LENGTH;
import static com.shannon.dataservice.DnsResolver.DNS_TYPE_NAPTR;

public class DnsQuery {
    private static int mCounter = 1;
    private final int mId;
    private int mType;
    private String mDomain;

    DnsQuery() {
        mId = mCounter++;
        mCounter = mCounter & 0xFFFF;
        mDomain = "";
    }

    public byte[] getNaptrQuery(String domain) {
        mDomain = domain;
        mType = DNS_TYPE_NAPTR;
        try {
            byte[] bytes = new byte[DNS_HEADER_LENGTH + DNS_QUERY_LENGTH + domain.length() + 2];
            bytes[0]  = (byte) ((mId & 0xFF00) >> 8);
            bytes[1]  = (byte) (mId & 0xFF);
            bytes[5]  = 1; /* 1 query */
            System.arraycopy(domain.getBytes("US-ASCII"), 0, bytes, DNS_HEADER_LENGTH + 1, domain.length());
            int ofs = DNS_HEADER_LENGTH + domain.length() + 2;
            bytes[ofs + 1] = (byte)mType;
            bytes[ofs + 3] = 1;  /* IN */

            ofs = DNS_HEADER_LENGTH;
            String[] labels = domain.split("\\.");
            for (String label : labels) {
                int length = label.length();
                bytes[ofs] = (byte)length;
                ofs += (length + 1);
            }
            return bytes;
        } catch (Exception e) {
            return null;
        }
    }

    public int getRequestId() { return mId; }
    public int getType() { return mType; }
    public String getDomain() { return mDomain; }
}
