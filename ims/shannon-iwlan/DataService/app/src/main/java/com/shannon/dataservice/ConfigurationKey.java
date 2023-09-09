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

import java.lang.reflect.Field;

import static com.shannon.dataservice.ConfigurationParameters.*;

import com.android.internal.annotations.VisibleForTesting;

public enum ConfigurationKey {
    CFG_HOME_FQDN(KEY_EPDG_SERVERS_LIST, "mHomeFqdnIms"),
    CFG_ROAM_FQDN(KEY_EPDG_SERVERS_LIST_ROAMING, "mRoamingFqdnIms"),
    CFG_EHOME_FQDN(KEY_EPDG_EMERGENCY_SERVERS_LIST, "mHomeFqdnEmc"),
    CFG_EROAM_FQDN(KEY_EPDG_EMERGENCY_SERVERS_LIST_ROAMING, "mRoamingFqdnEmc"),
    CFG_IKE_ATTRIBUTE_PCSCF4(KEY_EPDG_IKE_ATTRIBUTE_PCSCFv4, "mIkeAttrPcscf4"),
    CFG_IKE_ATTRIBUTE_PCSCF6(KEY_EPDG_IKE_ATTRIBUTE_PCSCFv6, "mIkeAttrPcscf6"),
    CFG_KEEPALIVE_INTERVAL(KEY_EPDG_IKE_KEEP_ALIVE_INTERVAL, "mKeepAliveInterval"),
    CFG_DPD_INTERVAL(KEY_EPDG_IKE_DPD_INTERVAL, "mDpdInterval"),
    CFG_IKE_DSCP(KEY_EPDG_IKE_DSCP, "mIkeDscp"),
    CFG_IKE_LIFETIME(KEY_EPDG_IKE_LIFETIME, "mIkeLifetime"),
    CFG_ESP_LIFETIME(KEY_EPDG_ESP_LIFETIME, "mEspLifetime"),
    CFG_IKE_ALGORITHM(KEY_EPDG_IKE_ALGORITHM_LIST, "mIkeAlgorithms"),
    CFG_ESP_ALGORITHM(KEY_EPDG_ESP_ALGORITHM_LIST, "mEspAlgorithms"),
    CFG_MOBIKE(KEY_EPDG_IKE_MOBIKE_ENABLED, "mMobikeEnabled"),
    CFG_USE_MAC(KEY_EPDG_IKE_INITIATOR_ID_HAS_MAC_ADDR, "mUseMacForNai"),
    CFG_USE_TS(KEY_EPDG_IKE_ORIGINAL_TSI_FOR_REKEY, "mUseInitialTs"),
    CFG_USE_FRAGMENTATION(KEY_EPDG_IKE_FRAGMENTATION_ENABLED, "mUseFragmentation"),
    CFG_PMTU_DISCOVERY(KEY_EPDG_PMTU_DISCOVERY, "mPmtuDiscovery"),
    CFG_USE_IMEI(KEY_EPDG_IKE_DEVICE_IDENTITY, "mIkeDeviceIdentity"),
    CFG_UDP_ENCAP(KEY_EPDG_FORCE_UDP_ENCAPSULATION, "mForceUdpEncapsulation"),
    CFG_CERTIFICATE(KEY_EPDG_IKE_CERTIFICATE_ENABLED, "mUseCertificate"),
    CFG_USIM(KEY_SIM_USIM_ONLY, "mUseUsim"),
    CFG_MMS_EPDG_NAME(KEY_EPDG_MMS_KEYID, "mMmsEpdgName"),
    CFG_EMC_EPDG_NAME(KEY_EPDG_EMERGENCY_KEYID, "mEmcEpdgName"),
    CFG_XCAP_EPDG_NAME(KEY_EPDG_XCAP_KEYID, "mXcapEpdgName"),
    CFG_REQUEST_PCSCF_FOR_ALL_PDN(KEY_EPDG_REQUEST_PCSCF_FOR_ALL_PDN, "mRequestPcscfForAllPdn"),
    CFG_RETRANSMIT_TIMEOUT(KEY_EPDG_RETRANSMIT_TIMEOUT, "mRetransmitTimeout"),
    CFG_RETRANSMIT_BASE(KEY_EPDG_RETRANSMIT_BASE, "mRetransmitBase"),
    CFG_RETRANSMIT_TRIES(KEY_EPDG_RETRANSMIT_TRIES, "mRetransmitTries"),
    CFG_RETRANSMIT_TRIES_HANDOVER(KEY_EPDG_RETRANSMIT_TRIES_HANDOVER, "mRetransmitTriesHandover"),
    CFG_USE_OFFLOADED_KEEPALIVE(KEY_EPDG_IKE_KEEP_ALIVE_OFFLOADED, "mUseOffloadedKeepalive"),
    CFG_EPDG_BACKOFF_TIMER_FOR_ERROR_CODE(KEY_EPDG_DEFAULT_RETRY_TIMEOUT, "mDefaultRetryTimeout"),
    CFG_EPDG_SUPPORTED_PCO_ID_LIST(KEY_EPDG_SUPPORTED_PCO_ID_LIST, "mSupportedPcoIdList"),
    CFG_EPDG_SERVERS_BLACKLIST(KEY_EPDG_SERVERS_BLACKLIST, "mServersBlacklist"),
    CFG_EPDG_USE_BLACKLISTED_SERVERS(KEY_EPDG_USE_BLACKLISTED_SERVERS, "mUseBlacklistedServers"),
    CFG_EPDG_RESPONDER_ID_FORMAT(KEY_EPDG_RESPONDER_ID_FORMAT, "mResponderIdFormat"),
    CFG_EPDG_RESPONDER_ID_FORMAT_FOR_EMERGENCY(KEY_EPDG_RESPONDER_ID_FORMAT_FOR_EMERGENCY, "mResponderIdFormatForEmergency"),
    CFG_EPDG_RESPONDER_ID_FORMAT_FOR_MMS(KEY_EPDG_RESPONDER_ID_FORMAT_FOR_MMS, "mResponderIdFormatForMms"),
    CFG_EPDG_RESPONDER_ID_FORMAT_FOR_XCAP(KEY_EPDG_RESPONDER_ID_FORMAT_FOR_XCAP, "mResponderIdFormatForXcap");

    private String mKeyName;
    private String mFieldName;

    ConfigurationKey(String name, String field) {
        mKeyName = name;
        mFieldName = field;
    }

    public String getKeyName() { return mKeyName; }

    public void setToObject(ConfigurationParameters parameters, Object value) {
        try {
            Field field = ConfigurationParameters.class.getField(mFieldName);
            if (field != null) {
                field.set(parameters, value);
            }

        } catch (Exception e) {
            // failed to set
        }
    }
}
