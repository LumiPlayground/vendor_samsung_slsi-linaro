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

import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.LinkedList;

public class IkeConfiguration {
    /* source IANA <https://www.iana.org/assignments/ikev2-parameters/ikev2-parameters.xhtml> */
    public static final int INTERNAL_IP4_ADDRESS = 1;
    public static final int INTERNAL_IP4_NETMASK = 2;
    public static final int INTERNAL_IP4_DNS = 3;
    public static final int INTERNAL_IP4_NBNS = 4;
    public static final int INTERNAL_IP4_DHCP = 6;
    public static final int APPLICATION_VERSION = 7;
    public static final int INTERNAL_IP6_ADDRESS = 8;
    public static final int INTERNAL_IP6_DNS = 10;
    public static final int INTERNAL_IP6_DHCP = 12;
    public static final int INTERNAL_IP4_SUBNET = 13;
    public static final int SUPPORTED_ATTRIBUTES = 14;
    public static final int INTERNAL_IP6_SUBNET = 15;
    public static final int MIP6_HOME_PREFIX = 16;
    public static final int INTERNAL_IP6_LINK = 17;
    public static final int INTERNAL_IP6_PREFIX = 18;
    public static final int HOME_AGENT_ADDRESS = 19;
    public static final int P_CSCF_IP4_ADDRESS = 20;
    public static final int P_CSCF_IP6_ADDRESS = 21;
    public static final int FTT_KAT = 22;
    public static final int EXTERNAL_SOURCE_IP4_NAT_INFO = 23;
    public static final int TIMEOUT_PERIOD_FOR_LIVENESS_CHECK = 24;
    public static final int INTERNAL_DNS_DOMAIN = 25;
    public static final int INTERNAL_DNSSEC_TA = 26;

    private static final boolean DBG = true;

    private final String TAG;
    private final LinkedList<IkeConfigurationAttribute> mSendList;
    private final LinkedList<Integer> mExpectedList;
    private final LinkedList<IkeConfigurationAttribute> mReceiveList;

    public IkeConfiguration(String tag) {
        TAG = tag + "-cfg";

        mSendList = new LinkedList<>();
        mExpectedList = new LinkedList<>();
        mReceiveList = new LinkedList<>();
    }

    public void addPcscfAttribute(int type, InetAddress address) {
        IkeIpConfigurationAttribute attribute = new IkeIpConfigurationAttribute(type, address);
        mSendList.add(attribute);

        /* P-CSCF is always expected in response */
        mExpectedList.add(type);
    }

    public void addImeiAttribute(int type, String Imei) {
        IkeImeiConfigurationAttribute attribute = new IkeImeiConfigurationAttribute(type, Imei);
        mSendList.add(attribute);
    }

    public void addDeviceIdentityAttribute(String Imei) {
        IkeDeviceIdentityAttribute attribute = new IkeDeviceIdentityAttribute(Imei);
        mSendList.add(attribute);
    }

    public void addLivenessCheckAttribute() {
        IkeConfigurationAttribute attribute = new IkeConfigurationAttribute(TIMEOUT_PERIOD_FOR_LIVENESS_CHECK, null);
        mSendList.add(attribute);
    }

    public void addExpectedAttribute(int type) {
        mExpectedList.add(type);
    }

    public byte[] getOutgoingAttributesBytes() {
        if (mSendList.isEmpty()) {
            log("No configuration attributes");
            return null;
        }

        /* create last, empty one */
        mSendList.add(new IkeConfigurationAttribute(0, null));

        int total = 0;
        for (IkeConfigurationAttribute attribute : mSendList) {
            total += attribute.getBytesSize();
        }

        ByteBuffer buffer = ByteBuffer.allocate(total);
        for (IkeConfigurationAttribute attribute : mSendList) {
            log("Send " + attribute);
            buffer.put(attribute.getBytes());
        }
        return buffer.array();
    }

    public byte[] getExpectedAttributesBytes() {
        if (mExpectedList.isEmpty()) {
            log("No attributes expected");
            return null;
        }
        int count = mExpectedList.size();

        ByteBuffer buffer = ByteBuffer.allocate(count * Short.BYTES + Integer.BYTES);
        buffer.order(ByteOrder.nativeOrder());
        buffer.putInt(count);
        for (Integer value : mExpectedList) {
            log("Expected " + value + " attribute");
            buffer.putShort(value.shortValue());
        }
        return buffer.array();
    }

    public void processReceivedAttributes(byte[] bytes) {
        if (bytes == null) {
            return;
        }
        try {
            ByteBuffer buffer = ByteBuffer.wrap(bytes);
            buffer.order(ByteOrder.nativeOrder());

            IkeConfigurationAttribute attribute = new IkeConfigurationAttribute(buffer);
            while (attribute.getType() != 0) {
                log("Received " + attribute);
                mReceiveList.add(attribute);
                attribute = new IkeConfigurationAttribute(buffer);
            }
        } catch (Exception e) {
            loge("Failed to process received attributes: " + e);
        }
    }

    public LinkedList<IkeIpConfigurationAttribute> getIpConfigurationAttributes(int type) {
        LinkedList<IkeIpConfigurationAttribute> list = new LinkedList<>();
        if (mReceiveList.isEmpty()) {
            loge("No attributes received");
            return list;
        }
        for (IkeConfigurationAttribute attribute : mReceiveList) {
            if (attribute.getType() == type) {
                list.add(new IkeIpConfigurationAttribute(attribute));
            }
        }
        return list;
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
    protected LinkedList<IkeConfigurationAttribute> getSendList() {
        return mSendList;
    }

    @VisibleForTesting
    protected LinkedList<Integer> getExpectedList() {
        return mExpectedList;
    }

    @VisibleForTesting
    protected LinkedList<IkeConfigurationAttribute> getReceiveList() {
        return mReceiveList;
    }
}
