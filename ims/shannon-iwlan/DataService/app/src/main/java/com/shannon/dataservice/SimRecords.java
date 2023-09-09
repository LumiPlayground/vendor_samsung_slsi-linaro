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
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.telephony.gsm.SimTlv;
import com.android.internal.telephony.uicc.IccUtils;
import com.android.internal.telephony.uicc.PlmnActRecord;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Arrays;


public class SimRecords {
    private static final boolean DBG = true;

    /* command */
    static private final int READ_BINARY = 0xB0;

    /* path */
    private static final String MF_SIM = "3F00";
    private static final String DF_ADF = "7FFF";
    private static final String DF_WLAN = "5F40";

    /* file */
    private static final int EF_OPLMN_W_ACT = 0x6F61;
    private static final int EF_EHPLMN = 0x6FD9;
    private static final int EF_EPDGID = 0x6FF3;
    private static final int EF_EPDGSEL = 0x6FF4;
    private static final int EF_EPDGIDEM = 0x6FF5;
    private static final int EF_EPDGSELEM = 0x6FF6;
    private static final int EF_UPLMNWLAN = 0x4F42;
    private static final int EF_OPLMNWLAN = 0x4F43;
    private static final int EF_WHPI = 0x4F49;
    private static final int EF_WLRPLMN = 0x4F4A;

    /* selection parameters */
    private static final int EPDG_TLV_TAG = 0x80;
    private static final int EPDG_TLV_TYPE_FQDN = 0;
    private static final int EPDG_TLV_TYPE_IPV4 = 1;
    private static final int EPDG_TLV_TYPE_IPV6 = 2;
    private static final int EPDG_TLV_FQDN_OPERATOR = 0;
    private static final int EPDG_TLV_FQDN_LOCATION = 1;
    private static final int WHPI_USE_RPMLMN = 0;
    private static final int WHPI_USE_HPMLMN = 1;
    private static final int WHPI_INVALID = Integer.MAX_VALUE;

    protected class PreferredString implements Comparable<PreferredString> {
        private final String mValue;
        private final int mPreference;

        PreferredString(String value, int preference) {
            mValue = value;
            mPreference = preference;
        }

        public String getValue() { return mValue; }

        @Override
        public int compareTo(PreferredString other) {
            return (mPreference - other.mPreference);
        }
    }

    private final String TAG;
    private final TelephonyManager mTelephonyManager;
    private String[] mEfEhplmns;
    private String[] mEfOplmnsAct;
    private String[] mEfEpdgIds;
    private String[] mEfEpdgSelections;
    private String[] mEfEpdgIdsEm;
    private String[] mEfEpdgSelectionsEm;
    private int mEfWhpi;
    private String[] mEfUplmns;
    private String[] mEfOplmns;
    private String mEfRplmn;

    public SimRecords(int slotId, TelephonyManager tm) {
        mTelephonyManager = tm;

        TAG = ShannonDataService.getServiceTAG("-SIMRecords-" + slotId);

        loadEfOplmn_w_act();
        loadEfEhplmn();
        loadEfEpdgId();
        loadEfEpdgSelection();
        loadEfEpdgIdEm();
        loadEfEpdgSelectionEm();
        loadEfWhpi();
        loadEfUplmn();
        loadEfOplmn();
        loadEfRplmn();
    }

    public String[] getEfEhplmn() { return mEfEhplmns; }
    public String[] getEfOplmn() { return mEfOplmnsAct; }
    public String[] getEfEpdgId() { return mEfEpdgIds; }
    public String[] getEfEpdgSelection() { return mEfEpdgSelections; }

    public String[] getEfEpdgIdEmergency() {
        if (mEfEpdgIdsEm != null) {
            return mEfEpdgIdsEm;
        }
        return mEfEpdgIds;
    }

    public String[] getEfEpdgSelectionEmergency() {
        if (mEfEpdgSelectionsEm != null) {
            return mEfEpdgSelectionsEm;
        }
        return mEfEpdgSelections;
    }

    public String getIwlanPlmn() {
        switch (mEfWhpi) {
            case WHPI_USE_HPMLMN:
                if (mEfEhplmns != null && mEfEhplmns.length > 0) {
                    return mEfEhplmns[0];
                }
                /* we can use SIM MCC/MNC */
                return null;
            case WHPI_USE_RPMLMN:
                return mEfRplmn;
            default:;
        }
        if (mEfUplmns != null && mEfUplmns.length > 0) {
            return mEfUplmns[0];
        }
        if (mEfOplmns != null && mEfOplmns.length > 0) {
            return mEfOplmns[0];
        }
        return null;
    }

    private void loadEfOplmn_w_act() {
        mEfOplmnsAct = null;
        log("Load EF_OPLMN_W_ACT");

        byte[] response = readFile(EF_OPLMN_W_ACT);
        if (response != null) {
            log("Received Operator PLMNs with AcT, raw=" + IccUtils.bytesToHexString(response));
            PlmnActRecord[] records = PlmnActRecord.getRecords(response);
            if (records != null && records.length > 0) {
                int numValidPlmns = 0;
                String[] parsed = new String[records.length];
                for (PlmnActRecord record : records) {
                    if (isValidPlmn(record.plmn)) {
                        parsed[numValidPlmns++] = new String(record.plmn);
                    }
                }
                if (numValidPlmns != 0) {
                    mEfOplmnsAct = Arrays.copyOf(parsed, numValidPlmns);
                    log("Operator_w_act PLMNs: " + Arrays.toString(mEfOplmnsAct));
                }
            }
        }
    }

    private void loadEfEhplmn() {
        mEfEhplmns = null;
        log("Load EF_EHPLMN");

        byte[] response = readFile(EF_EHPLMN);
        if (response != null) {
            mEfEhplmns = parseBcdPlmnList(response, "Equivalent Home");
        }
    }

    private void loadEfEpdgId() {
        mEfEpdgIds = null;
        log("Load EF_EPDGID");

        byte[] response = readFile(EF_EPDGID);
        if (response != null) {
            mEfEpdgIds = parseEpdgIds(response, "ePDG");
        }
    }

    private void loadEfEpdgSelection() {
        mEfEpdgSelections = null;
        log("Load EF_EPDGSEL");


        byte[] response = readFile(EF_EPDGSEL);
        if (response != null) {
            mEfEpdgSelections = parseEpdgSelections(response, "ePDG");
        }
    }

    private void loadEfEpdgIdEm() {
        mEfEpdgIdsEm = null;
        log("Load EF_EPDGIDEM");

        byte[] response = readFile(EF_EPDGIDEM);
        if (response != null) {
            mEfEpdgIdsEm = parseEpdgIds(response, "Emergency ePDG");
        }
    }

    private void loadEfEpdgSelectionEm() {
        mEfEpdgSelectionsEm = null;
        log("Load EF_EPDGIDEM");

        byte[] response = readFile(EF_EPDGIDEM);
        if (response != null) {
            mEfEpdgSelectionsEm = parseEpdgSelections(response, "Emergency ePDG");
        }
    }

    private void loadEfWhpi() {
        mEfWhpi = WHPI_INVALID;
        log("Load EF_WHPI");

        byte[] response = readFile(EF_WHPI);
        if (response != null) {
            mEfWhpi = StringUtils.getByteAsInt(response, 0);
            log("WHPI is set to " + mEfWhpi);
        }
    }

    private void loadEfUplmn() {
        mEfUplmns = null;
        log("Load EF_UPLMNWLAN");

        byte[] response = readFile(EF_UPLMNWLAN);
        if (response != null) {
            mEfUplmns = parseBcdPlmnList(response, "User Controlled");
        }
    }

    private void loadEfOplmn() {
        mEfOplmns = null;
        log("Load EF_OPLMNWLAN");

        byte[] response = readFile(EF_OPLMNWLAN);
        if (response != null) {
            mEfOplmns = parseBcdPlmnList(response, "Operator Controlled");
        }
    }

    private void loadEfRplmn() {
        mEfRplmn = null;
        log("Load EF_WLRPLMN");

        byte[] response = readFile(EF_WLRPLMN);
        if (response != null) {
            String[] result = parseBcdPlmnList(response, "Last Registered");
            if (result != null && result.length > 0) {
                mEfRplmn = result[0];
            }
        }
    }

    private boolean isValidPlmn(String plmn) {
        return (plmn != null && plmn.length() >= 5);
    }

    /**
     * From SIMRecords.java
     * convert a byte array of packed plmns to an array of strings
     */
    private String[] parseBcdPlmnList(byte[] data, String description) {
        final int packedBcdPlmnLenBytes = 3;
        log("Received " + description + " PLMNs, raw=" + IccUtils.bytesToHexString(data));
        if (data.length == 0 || (data.length % packedBcdPlmnLenBytes) != 0) {
            loge("Received invalid " + description + " PLMN list");
            return null;
        }
        int numPlmns = data.length / packedBcdPlmnLenBytes;
        int numValidPlmns = 0;
        String[] parsed = new String[numPlmns];
        for (int i = 0; i < numPlmns; i++) {
            parsed[numValidPlmns] = IccUtils.bcdPlmnToString(data, i * packedBcdPlmnLenBytes);
            // we count the valid (non empty) records and only increment if valid
            if (isValidPlmn(parsed[numValidPlmns])) numValidPlmns++;
        }
        if (numValidPlmns == 0) {
            return null;
        }
        String[] ret = Arrays.copyOf(parsed, numValidPlmns);
        log(description + " PLMNs: " + Arrays.toString(ret));
        return ret;
    }

    private String[] parseEpdgIds(byte[] data, String description) {
        log("Received " + description + " Ids, raw=" + IccUtils.bytesToHexString(data));
        if (data.length == 0 || data.length < 4) {
            loge("Received invalid " + description + " Ids");
            return null;
        }
        ArrayList<String> list = new ArrayList<>();

        SimTlv tlv = new SimTlv(data, 0, data.length);
        for (; tlv.isValidObject(); tlv.nextObject()) {
            if (tlv.getTag() == EPDG_TLV_TAG) {
                byte[] tlvData = tlv.getData();
                if (tlvData == null || tlvData.length == 0) {
                    continue;
                }
                int type = StringUtils.getByteAsInt(tlvData, 0);
                switch (type) {
                    case EPDG_TLV_TYPE_FQDN:
                        try {
                            String s = new String(tlvData, 1, tlvData.length - 1, "UTF-8");
                            list.add(s);
                        } catch (Exception e) {
                            // nothing
                        }
                        break;
                    case EPDG_TLV_TYPE_IPV4:
                        try {
                            InetAddress a = InetAddress.getByAddress(Arrays.copyOfRange(tlvData, 1, 5));
                            list.add(a.getHostAddress());
                        } catch (Exception e) {
                            // nothing
                        }
                        break;
                    case EPDG_TLV_TYPE_IPV6:
                        try {
                            InetAddress a = InetAddress.getByAddress(Arrays.copyOfRange(tlvData, 1, 17));
                            list.add(a.getHostAddress());
                        } catch (Exception e) {
                            // nothing
                        }
                        break;
                    default:
                        loge("Invalid TLV address type " + type);
                }
            }
        }

        if (list.size() == 0) {
            return null;
        }
        String[] ret = list.toArray(new String[0]);
        log(description + " Ids: " + Arrays.toString(ret));
        return ret;
    }

    private String[] parseEpdgSelections(byte[] data, String description) {
        log("Received " + description + " Ids, raw=" + IccUtils.bytesToHexString(data));
        if (data.length == 0 || data.length < 8) {
            loge("Received invalid " + description + " Ids");
            return null;
        }
        final int recordLength = 6;
        SimTlv tlv = new SimTlv(data, 0, data.length);
        if (tlv.getTag() != EPDG_TLV_TAG) {
            loge("Invalid TLV TAG " + String.format("%02X", tlv.getTag()));
            return null;
        }
        byte[] tlvData = tlv.getData();
        if (tlvData == null || tlvData.length == 0 || (tlvData.length % recordLength) != 0) {
            loge("Invalid TLV data");
            return null;
        }

        int numRecords = tlvData.length / recordLength;
        int numValidRecords = 0;
        PreferredString[] parsed = new PreferredString[numRecords];
        for (int i = 0; i < numRecords; i++) {
            String plmn = IccUtils.bcdPlmnToString(tlvData, i * recordLength);
            if (!isValidPlmn(plmn) || plmn.contains("D")) {
                continue;
            }
            int preference = StringUtils.getShortAsInt(tlvData, (i * recordLength) + 3);
            int format = StringUtils.getByteAsInt(tlvData, (i * recordLength) + 5);
            String formatted;
            switch (format) {
                case EPDG_TLV_FQDN_OPERATOR:
                    formatted = ConfigurationParameters.SERVER_3GPP_OPERATOR + ":" + plmn;
                    break;
                case EPDG_TLV_FQDN_LOCATION:
                    formatted = ConfigurationParameters.SERVER_3GPP_LOCATION + ":" + plmn;
                    break;
                default:
                    loge("Invalid TLV FQDN format " + format);
                    continue;
            }
            parsed[numValidRecords++] = new PreferredString(formatted, preference);
        }

        if (numValidRecords == 0) {
            return null;
        }
        Arrays.sort(parsed);
        String[] ret = new String[numValidRecords];
        for (int i = 0; i < numValidRecords; i++) {
            ret[i] = parsed[i].getValue();
        }
        log(description + " Ids: " + Arrays.toString(ret));
        return ret;
    }

    private byte[] readFile(int fileId) {
        byte[] bytes = mTelephonyManager.iccExchangeSimIO(fileId, READ_BINARY, 0, 0, 0, getFilePath(fileId));
        /* framework returns data as [response data][SW1][SW2] */
        if (bytes != null && bytes.length > 2) {
            /* truncate */
            return Arrays.copyOf(bytes, bytes.length - 2);
        }
        return null;
    }

    private String getFilePath(int fileId) {
        switch (fileId) {
            case EF_OPLMN_W_ACT:
            case EF_EHPLMN:
            case EF_EPDGID:
            case EF_EPDGSEL:
            case EF_EPDGIDEM:
            case EF_EPDGSELEM:
                return MF_SIM + DF_ADF;

            case EF_UPLMNWLAN:
            case EF_OPLMNWLAN:
            case EF_WHPI:
            case EF_WLRPLMN:
                return MF_SIM + DF_ADF + DF_WLAN;
        }
        return null;
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }
}
