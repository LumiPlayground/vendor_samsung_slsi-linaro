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

public class StrokeAddRequest extends StrokeRequestMessage {
    private static final int CERT_SEND_IF_ASKED = 1;
    private static final int CERT_NEVER_SEND = 2;
    // options
    private static final int OPT_USE_ORIGINAL_TS  = 0x00000001;
    private static final int OPT_DO_REKEY_ON_ROAM = 0x00000002;
    private static final int OPT_DO_PMTU_DISCOVERY = 0x00000004;

    StrokeAddRequest(ConnectionParameters parameters) {
        super(RequestMessageType.STR_REQ_ADD_CONN);
        align(8);
        putString("name", parameters.mName);
        putInt("version", parameters.mIkeVersion);
        putString("eap_identity", parameters.mEapId);
        putString("aaa_identity", null);
        putString("xauth_identity", null);
        putInt("mode", parameters.mChannelMode);
        putInt("mobike", parameters.mMobikeSupported ? 1 : 0);
        putInt("aggressive", 0);
        putInt("pushmode", 0);
        putInt("force_encap", parameters.mForceUdpEncapsulation ? 1 : 0);
        putInt("fragmentation", parameters.mIkeFragmentationEnabled ? 1 : 0);
        putInt("ipcomp", 0);
        putTime("inactivity", 0);
        putInt("proxy_mode", 0);
        putInt("install_policy", parameters.mInstallPolicy);
        putInt("close_action", 0);
        putInt("reqid", parameters.mRequestId);
        putInt("tfc", 0);
        putByte("ikedscp", parameters.mIkeDscp);
        putInt("crl_policy", 0);
        putInt("unique", parameters.mIsFirstConnection ? 1 : 4); /* 1: yes, 4: never */
        putString("algorithms.ike", parameters.mIkeAlgorithms);
        putString("algorithms.esp", parameters.mEspAlgorithms);
        putString("algorithms.ah", null);
        align();
        putInt("rekey.reauth", parameters.mIkeReauth);
        putTime("rekey.ipsec_lifetime", parameters.mEspLifetime);
        putTime("rekey.ike_lifetime", parameters.mIkeLifetime);
        putTime("rekey.margin", parameters.mRekeyMargin);
        putLong("rekey.life_bytes", 0);
        putLong("rekey.margin_bytes", 0);
        putLong("rekey.life_packets", 0);
        putLong("rekey.margin_packets", 0);
        putLongOrInt("rekey.tries", parameters.mRekeyRetries);
        putLongOrInt("rekey.fuzz", parameters.mRekeyFuzz);
        align();
        putTime("dpd.delay", parameters.mDpdInterval);
        putTime("dpd.timeout", 0);
        putInt("dpd.action", parameters.mDpdAction);
        align();
        putInt("ikeme.mediation", 0);
        putString("ikeme.mediated_by", null);
        putString("ikeme.peerid", null);
        align();
        putInt("mark_in.value", 0);
        putInt("mark_in.mask", 0);
        align();
        putInt("mark_out.value", 0);
        putInt("mark_out.mask", 0);
        align();
        // ME
        putString("me.auth", parameters.mMeAuthType);
        putString("me.auth2", null);
        putString("me.id", parameters.mMeId);
        putString("me.id2", null);
        putString("me.eap_id", null);
        putString("me.rsakey", null);
        putString("me.cert", null);
        putString("me.cert2", null);
        putString("me.ca", null);
        putString("me.ca2", null);
        putString("me.groups", null);
        putString("me.groups2", null);
        putString("me.cert_policy", null);
        putString("me.updown", null);
        putString("me.address", parameters.mMeAddress);
        putShort("me.ikeport", parameters.mMeIkePort);
        putString("me.sourceip", parameters.mMeSourceIP);
        putString("me.dns", parameters.mMeDns);
        putString("me.subnets", parameters.mMeSubnet);
        int sendcert = (parameters.mCertificateSupported ? CERT_SEND_IF_ASKED : CERT_NEVER_SEND);
        putInt("me.sendcert", sendcert);
        putInt("me.hostaccess", 0);
        putInt("me.tohost", 0);
        putInt("me.allow_any", 0);
        putByte("me.protocol", 0);
        putShort("me.from_port", 0);
        putShort("me.to_port", parameters.mMeToPort);
        align();
        // OTHER
        putString("other.auth", parameters.mOtherAuthType);
        putString("other.auth2", null);
        putString("other.id", parameters.mOtherId);
        putString("other.id2", null);
        putString("other.eap_id", null);
        putString("other.rsakey", null);
        putString("other.cert", null);
        putString("other.cert2", null);
        putString("other.ca", null);
        putString("other.ca2", null);
        putString("other.groups", null);
        putString("other.groups2", null);
        putString("other.cert_policy", null);
        putString("other.updown", null);
        putString("other.address", parameters.mOtherAddress);
        putShort("other.ikeport", parameters.mOtherIkePort);
        putString("other.sourceip", null);
        putString("other.dns", null);
        putString("other.subnets", parameters.mOtherSubnets);
        putInt("other.sendcert", sendcert);
        putInt("other.hostaccess", 0);
        putInt("other.tohost", 0);
        putInt("other.allow_any", 0);
        putByte("other.protocol", 0);
        putShort("other.from_port", 0);
        putShort("other.to_port", parameters.mOtherToPort);
        align();
        // end
        putInt("replay_window", 0);
        putInt("sha256_96", 0);
        // IWLAN
        putString("iwlan.interface", parameters.mInterface);
        putBytes("iwlan.request.attributes", (parameters.mIkeConfiguration != null) ?
                parameters.mIkeConfiguration.getOutgoingAttributesBytes() : null);
        putBytes("iwlan.request.notifies", (parameters.mIkeNotifyProcessor != null) ?
                parameters.mIkeNotifyProcessor.getOutgoingNotifyBytes() : null);
        putBytes("iwlan.response.attributes", (parameters.mIkeConfiguration != null) ?
                parameters.mIkeConfiguration.getExpectedAttributesBytes() : null);
        putBytes("iwlan.response.notifies", (parameters.mIkeNotifyProcessor != null) ?
                parameters.mIkeNotifyProcessor.getExpectedNotifyBytes() : null);
        putFloat("iwlan.retransmit.timeout", parameters.mRetransmitTimeout);
        putFloat("iwlan.retransmit.base", parameters.mRetransmitBase);
        putInt("iwlan.retransmit.tries", parameters.mRetransmitTries);
        putFloat("iwlan.retransmit.timeout_handover", parameters.mRetransmitTimeoutHandover);
        putFloat("iwlan.retransmit.base_handover", parameters.mRetransmitBaseHandover);
        putInt("iwlan.retransmit.tries_handover", parameters.mRetransmitTriesHandover);
        putInt("iwlan.keepalive_interval", parameters.mKeepAliveInterval);
        int options = (parameters.mUseOriginalTsi ? OPT_USE_ORIGINAL_TS : 0);
        options |= (parameters.mRekeyOnRoam ? OPT_DO_REKEY_ON_ROAM : 0);
        options |= (parameters.mPmtuDiscovery ? OPT_DO_PMTU_DISCOVERY : 0);
        putInt("iwlan.options", options);
        align(8);
    }
}
