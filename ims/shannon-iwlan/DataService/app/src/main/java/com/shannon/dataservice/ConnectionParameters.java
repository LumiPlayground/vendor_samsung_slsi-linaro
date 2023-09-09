/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import java.net.InetAddress;

public class ConnectionParameters {
    public static final String PROTO_IPv4 = "%config4";
    public static final String PROTO_IPv6 = "%config6";
    public static final String PROTO_IPv4v6 = "%config4, %config6";
    public static final String SUBNET_IPv4 = "0.0.0.0/0";
    public static final String SUBNET_IPv6 = "::/0";
    public static final String SUBNET_IPv4v6 = "0.0.0.0/0, ::/0";

    public String mName = null;
    public int mIkeVersion = 2;
    public int mChannelMode = 2;
    public int mInstallPolicy = 1;
    public int mIkeReauth = 0;
    public int mRekeyMargin = 10;   /* 10sec */
    public int mRekeyFuzz = 0;      /* 0% of margin */
    public int mRekeyRetries = 1;
    public String mMeId = null;
    public String mEapId = null;
    public String mMeAddress = null;
    public String mMeSourceIP = null;
    public String mMeSubnet = null;
    public String mMeDns = null;
    public String mMeAuthType = null;
    public int mMeIkePort = 500;
    public int mMeToPort = 65535;
    public String mOtherId = null;
    public String mOtherAuthType = null;
    public String mOtherAddress = null;
    public String mOtherSubnets = null;
    public int mOtherIkePort = 500;
    public int mOtherToPort = 65535;
    public String mIkeAlgorithms = null;
    public String mEspAlgorithms = null;
    public int mIkeDscp = 0;
    public int mIkeLifetime = 0;
    public int mEspLifetime = 0;
    public int mDpdInterval = 0;
    public int mDpdAction = 1;
    public int mKeepAliveInterval= 0;
    public boolean mIsHandover = false;
    public boolean mIsFirstConnection = false;
    public boolean mMobikeSupported = false;
    public boolean mCertificateSupported = false;
    public boolean mForceUdpEncapsulation = false;
    public boolean mUseOriginalTsi = false;
    public boolean mIkeFragmentationEnabled = false;
    public boolean mRekeyOnRoam = true;
    public int mRequestId = 21;
    public String mInterface = null;
    /* retransmit */
    public float mRetransmitTimeout = (float) 1.0;
    public float mRetransmitBase = (float) 2.0;
    public int mRetransmitTries = 3;
    public float mRetransmitTimeoutHandover = (float) 1.0;
    public float mRetransmitBaseHandover = (float) 1.0;
    public int mRetransmitTriesHandover = 1;
    /* internal for service */
    public InetAddress mLocalAddress;
    public InetAddress mRemoteAddress;
    /* IKE configuration attributes */
    public IkeConfiguration mIkeConfiguration = null;
    /* vendor specific notifies */
    public IkeNotifyProcessor mIkeNotifyProcessor = null;
    /* PMTU discovery */
    public boolean mPmtuDiscovery = true;

    public ConnectionParameters() {}

    public ConnectionParameters(ConfigurationParameters configurationParameters) {
        mMeSubnet = "%dynamic";
        mOtherAuthType = "any";

        mMeAuthType = configurationParameters.mMeAuthType;
        mMeIkePort = configurationParameters.mMeIkePort;
        mOtherIkePort = configurationParameters.mOtherIkePort;
        mIkeAlgorithms = configurationParameters.mIkeAlgorithms;
        mEspAlgorithms = configurationParameters.mEspAlgorithms;
        mIkeDscp = configurationParameters.mIkeDscp;
        mIkeLifetime = configurationParameters.mIkeLifetime;
        mEspLifetime = configurationParameters.mEspLifetime;
        mDpdInterval = configurationParameters.mDpdInterval;
        mKeepAliveInterval = configurationParameters.mKeepAliveInterval;
        mMobikeSupported = configurationParameters.mMobikeEnabled;
        mCertificateSupported = configurationParameters.mUseCertificate;
        mForceUdpEncapsulation = configurationParameters.mForceUdpEncapsulation;
        mUseOriginalTsi = configurationParameters.mUseInitialTs;
        mIkeFragmentationEnabled = configurationParameters.mUseFragmentation;
        mRetransmitTimeout = (float)configurationParameters.mRetransmitTimeout;
        mRetransmitBase = (float)configurationParameters.mRetransmitBase;
        mRetransmitTries = configurationParameters.mRetransmitTries;
        mRetransmitTriesHandover = configurationParameters.mRetransmitTriesHandover;
        mPmtuDiscovery = configurationParameters.mPmtuDiscovery;
    }

    public void setIkeConfiguration(IkeConfiguration configuration) {
        mIkeConfiguration = configuration;
    }

    public void setIkeNotifyProcessor(IkeNotifyProcessor processor) {
        mIkeNotifyProcessor = processor;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(1024);
        sb.append("Connection parameters ePDG name: ").append(mName);
        sb.append(" adapter: ").append(mInterface);
        sb.append(" local IP: ").append(mMeAddress);
        sb.append(" remote IP: ").append(mOtherAddress);
        sb.append(" tunnel IP: ").append(mMeSourceIP);
        sb.append(" IKE dscp: ").append(mIkeDscp);
        sb.append(" IKE lifetime: ").append(mIkeLifetime);
        sb.append(" ESP lifetime: ").append(mEspLifetime);
        sb.append(" DPD interval: ").append(mDpdInterval);
        sb.append(" Keep-alive: ").append(mKeepAliveInterval);
        sb.append(" IKE algorithms: ").append(mIkeAlgorithms);
        sb.append(" ESP algorithms: ").append(mEspAlgorithms);
        sb.append(" PmtuDiscovery: ").append(mPmtuDiscovery);
        return sb.toString();
    }
}
