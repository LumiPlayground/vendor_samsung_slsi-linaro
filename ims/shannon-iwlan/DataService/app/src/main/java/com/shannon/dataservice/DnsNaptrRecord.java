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

public class DnsNaptrRecord extends DnsResourceRecord implements Comparable<DnsNaptrRecord> {
    private int mOrder;
    private int mPreference;
    private String mFlags;
    private String mRegexp;
    private String mDomain;

    DnsNaptrRecord(DnsResourceCallback callback, int ttl, byte[] input) {
        super(callback, ttl);

        mOrder = StringUtils.getShortAsInt(input, 0);
        mPreference = StringUtils.getShortAsInt(input, 2);
        /* flags */
        mFlags = "";
        int length = StringUtils.getByteAsInt(input, 4);
        if (length > 0) {
            try {
                StringBuilder sb = new StringBuilder(128);
                for (int i = 0; i < length; i++) {
                    sb.append((char)input[5 + i]);
                }
                mFlags = sb.toString();
            } catch (Exception e) {
                // nothing
            }
        }
        /* services */
        int ofs = 5 + length;
        length = StringUtils.getByteAsInt(input, ofs);
        /* regexp */
        mRegexp = "";
        ofs += (length + 1);
        length = StringUtils.getByteAsInt(input, ofs);
        if (length > 0) {
            try {
                StringBuilder sb = new StringBuilder(128);
                for (int i = 0; i < length; i++) {
                    sb.append((char)input[ofs + 1 + i]);
                }
                mRegexp = sb.toString();
            } catch (Exception e) {
                // nothing
            }
        }
        /* domain */
        ofs += (length + 1);
        length = StringUtils.getByteAsInt(input, ofs);
        try {
            StringBuilder sb = new StringBuilder(512);
            while (length > 0) {
                for (int i = 0; i < length; i++) {
                    sb.append((char)input[++ofs]);
                }
                length = StringUtils.getByteAsInt(input, ++ofs);
                if (length > 0) {
                    sb.append('.');
                }
            }
            mDomain = sb.toString();
        } catch (Exception e) {
            mDomain = "";
        }
    }

    @Override
    public int compareTo(DnsNaptrRecord other) {
        if (mOrder == other.mOrder) {
            return (mPreference - other.mPreference);
        }
        return (mOrder - other.mOrder);
    }

    public int getOrder() { return mOrder; }
    public int getPreference() { return mPreference; }
    public String getReplacement() { return mDomain; }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(512);
        sb.append("NAPTR. Order=").append(mOrder);
        sb.append(" Pref=").append(mPreference);
        sb.append(" Flags=").append(mFlags);
        sb.append(" Regexp=").append(mRegexp);
        sb.append(" Replace=").append(mDomain);
        return sb.toString();
    }
}
