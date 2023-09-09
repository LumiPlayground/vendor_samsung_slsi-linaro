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

import android.telephony.TelephonyManager;
import android.text.TextUtils;

public class ConfigurationParameters {
    /* Vendor prefix */
    public static final String KEY_PREFIX = "vendor.";

    /* ePDG keys */
    public static final String KEY_EPDG = "epdg.";
    public static final String KEY_EPDG_SERVERS_LIST = KEY_PREFIX + KEY_EPDG + "servers";
    public static final String KEY_EPDG_SERVERS_LIST_ROAMING = KEY_PREFIX + KEY_EPDG + "servers_roaming";
    public static final String KEY_EPDG_EMERGENCY_SERVERS_LIST = KEY_PREFIX + KEY_EPDG + "emergency_servers";
    public static final String KEY_EPDG_EMERGENCY_SERVERS_LIST_ROAMING = KEY_PREFIX + KEY_EPDG + "emergency_servers_roaming";
    public static final String KEY_EPDG_SERVERS_BLACKLIST = KEY_PREFIX + KEY_EPDG + "servers_blacklist";
    public static final String KEY_EPDG_USE_BLACKLISTED_SERVERS = KEY_PREFIX + KEY_EPDG + "use_blacklisted_servers";
    public static final String KEY_EPDG_RESPONDER_ID_FORMAT = KEY_PREFIX + KEY_EPDG + "responder_id_format";
    public static final String KEY_EPDG_RESPONDER_ID_FORMAT_FOR_EMERGENCY = KEY_PREFIX + KEY_EPDG + "responder_id_format_for_emergency";
    public static final String KEY_EPDG_RESPONDER_ID_FORMAT_FOR_MMS = KEY_PREFIX + KEY_EPDG + "responder_id_format_for_mms";
    public static final String KEY_EPDG_RESPONDER_ID_FORMAT_FOR_XCAP = KEY_PREFIX + KEY_EPDG + "responder_id_format_for_xcap";
    public static final String KEY_EPDG_EMERGENCY_KEYID = KEY_PREFIX + KEY_EPDG + "emergency_keyid";
    public static final String KEY_EPDG_MMS_KEYID = KEY_PREFIX + KEY_EPDG + "mms_keyid";
    public static final String KEY_EPDG_XCAP_KEYID = KEY_PREFIX + KEY_EPDG + "xcap_keyid";
    public static final String KEY_EPDG_IKE_ATTRIBUTE_PCSCFv4 = KEY_PREFIX + KEY_EPDG + "ike_attr_pcscf4";
    public static final String KEY_EPDG_IKE_ATTRIBUTE_PCSCFv6 = KEY_PREFIX + KEY_EPDG + "ike_attr_pcscf6";
    public static final String KEY_EPDG_IKE_DEVICE_IDENTITY = KEY_PREFIX + KEY_EPDG + "ike_device_identity";
    public static final String KEY_EPDG_IKE_KEEP_ALIVE_INTERVAL = KEY_PREFIX + KEY_EPDG + "ike_keep_alive_interval";
    public static final String KEY_EPDG_IKE_KEEP_ALIVE_OFFLOADED = KEY_PREFIX + KEY_EPDG + "ike_keep_alive_offloaded";
    public static final String KEY_EPDG_IKE_DPD_INTERVAL = KEY_PREFIX + KEY_EPDG + "ike_dpd_interval";
    public static final String KEY_EPDG_IKE_DSCP = KEY_PREFIX + KEY_EPDG + "ike_dscp";
    public static final String KEY_EPDG_IKE_LIFETIME = KEY_PREFIX + KEY_EPDG + "ike_lifetime";
    public static final String KEY_EPDG_IKE_ALGORITHM_LIST = KEY_PREFIX + KEY_EPDG + "ike_algorithm_list";
    public static final String KEY_EPDG_IKE_CERTIFICATE_ENABLED = KEY_PREFIX + KEY_EPDG + "ike_certificate_enabled";
    public static final String KEY_EPDG_IKE_FRAGMENTATION_ENABLED = KEY_PREFIX + KEY_EPDG + "ike_fragmentation_enabled";
    public static final String KEY_EPDG_PMTU_DISCOVERY = KEY_PREFIX + KEY_EPDG + "pmtu_discovery";
    public static final String KEY_EPDG_FORCE_UDP_ENCAPSULATION = KEY_PREFIX + KEY_EPDG + "force_udp_encapsulation";
    public static final String KEY_EPDG_IKE_MOBIKE_ENABLED = KEY_PREFIX + KEY_EPDG + "ike_mobike_enabled";
    public static final String KEY_EPDG_IKE_INITIATOR_ID_HAS_MAC_ADDR = KEY_PREFIX + KEY_EPDG + "ike_initiator_id_has_mac_addr";
    public static final String KEY_EPDG_IKE_ORIGINAL_TSI_FOR_REKEY = KEY_PREFIX + KEY_EPDG + "ike_original_tsi_for_rekey";
    public static final String KEY_EPDG_ESP_LIFETIME = KEY_PREFIX + KEY_EPDG + "esp_lifetime";
    public static final String KEY_EPDG_ESP_ALGORITHM_LIST = KEY_PREFIX + KEY_EPDG + "esp_algorithm_list";
    public static final String KEY_EPDG_REQUEST_PCSCF_FOR_ALL_PDN = KEY_PREFIX + KEY_EPDG + "request_pcscf_for_all_pdn";
    public static final String KEY_EPDG_RETRANSMIT_TIMEOUT = KEY_PREFIX + KEY_EPDG + "retransmit_timeout";
    public static final String KEY_EPDG_RETRANSMIT_BASE = KEY_PREFIX + KEY_EPDG + "retransmit_base";
    public static final String KEY_EPDG_RETRANSMIT_TRIES = KEY_PREFIX + KEY_EPDG + "retransmit_tries";
    public static final String KEY_EPDG_RETRANSMIT_TRIES_HANDOVER = KEY_PREFIX + KEY_EPDG + "retransmit_tries_handover";
    public static final String KEY_EPDG_DEFAULT_RETRY_TIMEOUT = KEY_PREFIX + KEY_EPDG + "default_retry_timeout";
    public static final String KEY_EPDG_SUPPORTED_PCO_ID_LIST = KEY_PREFIX + KEY_EPDG + "supported_pco_id_list";
    public static final String KEY_EPDG_CONNECTION_RETRY_PARAMETERS = KEY_PREFIX + KEY_EPDG + "connection_retry_parameters";

    /* SIM related keys */
    public static final String KEY_SIM_USIM_ONLY = KEY_PREFIX + "sim_usim_only";

    /* VoMobile keys */
    public static final String KEY_VOMOBILE_ENABLED = KEY_PREFIX + "vomobile_enabled";
    public static final String KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "vomobile_supported_radio_technology";
    public static final String KEY_VOMOBILE_ONROAMING_ENABLED = KEY_PREFIX + "vomobile_onroaming_enabled";

    /* WiFi */
    public static final String KEY_WIFI_ACCESS_POINTS_HANDOVER_SUPPORTED = KEY_PREFIX + "wifi_access_points_handover_supported";
    public static final String KEY_WIFI_ACCESS_POINTS_HANDOVER_TIMEOUT = KEY_PREFIX + "wifi_access_points_handover_timeout";

    /* FQDN type */
    public static final String SERVER_3GPP_OPERATOR = "3gpp-operator";
    public static final String SERVER_3GPP_LOCATION = "3gpp-location";
    public static final String SERVER_3GPP_5G_LOCATION = "3gpp-location-5g";
    public static final String SERVER_3GPP_VISITED = "3gpp-visited";
    public static final String SERVER_IMS_OPERATOR = "ims-operator";
    public static final String SERVER_SIM = "sim";
    /* MCC/MNC source type */
    public static final String MCCMNC_SIM = "sim";
    public static final String MCCMNC_PLMN = "plmn";
    public static final String MCCMNC_EHPLMN = "ehplmn";
    public static final String MCCMNC_WPLMN = "wlanplmn";
    public static final String MCCMNC_OPLMNACT = "oplmnact";

    /* device identity */
    public static final String IDENTITY_AS_ATTRIBUTE = "attribute";
    public static final String IDENTITY_AS_NOTIFY = "notify";
    public static final String ATTR_DEVICE_IDENTITY = "device_identity";
    public static final String ATTR_AT_DEVICE_IDENTITY = "at_device_identity";

    /* responder ID */
    public static final String ID_KEY_ID = "keyid";
    public static final String ID_FQDN = "fqdn";

    /* default configurable parameters */
    public String mHomeFqdnIms = SERVER_3GPP_OPERATOR + ":" + MCCMNC_SIM;    /* 3gpp-operator:sim */
    public String mRoamingFqdnIms = "";
    public String mHomeFqdnEmc = "";
    public String mRoamingFqdnEmc = "";
    public String mServersBlacklist = "";
    public boolean mUseBlacklistedServers = true;
    public int mIkeAttrPcscf4 = 20;
    public int mIkeAttrPcscf6 = 21;
    public int mKeepAliveInterval = 20;
    public int mDpdInterval = 60;
    public int mIkeDscp = 0;
    public int mIkeLifetime = 64800;
    public int mEspLifetime = 64800;
    public String mIkeAlgorithms = "aes128-aes256-aes128ctr-aes256ctr-des-3des-null-md5-sha1-aesxcbc-sha256-prfmd5-prfsha1-prfaesxcbc-prfsha256-modp1024-modp1536-modp2048!";
    public String mEspAlgorithms = "aes128-aes192-aes256-aes128ctr-aes192ctr-aes256ctr-des-3des-null-md5-sha1-aesxcbc-sha256!";
    public boolean mMobikeEnabled = true;
    public boolean mUseMacForNai = false;
    public boolean mUseInitialTs = false;
    public boolean mUseFragmentation = false;
    public String mIkeDeviceIdentity = null;
    public boolean mForceUdpEncapsulation = false;
    public boolean mUseCertificate = false;
    public boolean mUseUsim = false;
    public String mDefaultRetryTimeout = null;
    public String mResponderIdFormat = "keyid:%s";
    public String mResponderIdFormatForEmergency = null;
    public String mResponderIdFormatForMms = null;
    public String mResponderIdFormatForXcap = null;
    /* retransmit */
    public int mRetransmitTimeout = 1;
    public int mRetransmitBase = 2;
    public int mRetransmitTries = 3;
    public int mRetransmitTriesHandover = 1;
    /* offloaded keep alive flag */
    public boolean mUseOffloadedKeepalive = false;
    /* MMS configurable parameters*/
    public String mMmsEpdgName = "";
    /* EMC configurable parameters */
    public String mEmcEpdgName = "sos";
    /* XCAP configurable parameters*/
    public String mXcapEpdgName = "";
    /* not configurable now */
    public String mMeAuthType = "eap-aka";
    public int mMeIkePort = 500;
    public int mOtherIkePort = 500;
    /* TEST */
    public boolean mRequestPcscfForAllPdn = false;
    /* PMTU discovery */
    public boolean mPmtuDiscovery = true;

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(2048);
        sb.append("Configuration parameters");
        sb.append(" homeFQDNs: ").append(mHomeFqdnIms);
        if (!TextUtils.isEmpty(mRoamingFqdnIms)) {
            sb.append(" roamFQDNs: ").append(mRoamingFqdnIms);
        }
        if (!TextUtils.isEmpty(mHomeFqdnEmc)) {
            sb.append(" homeEFQDNs: ").append(mHomeFqdnEmc);
        }
        if (!TextUtils.isEmpty(mRoamingFqdnEmc)) {
            sb.append(" roamEFQDNs: ").append(mRoamingFqdnEmc);
        }
        sb.append(" IkeAttrPcscf4: ").append(mIkeAttrPcscf4);
        sb.append(" IkeAttrPcscf6: ").append(mIkeAttrPcscf6);
        sb.append(" IkeDscp: ").append(mIkeDscp);
        sb.append(" IKE lifetime: ").append(mIkeLifetime);
        sb.append(" ESP lifetime: ").append(mEspLifetime);
        sb.append(" DPD interval: ").append(mDpdInterval);
        sb.append(" Keep-alive: ").append(mKeepAliveInterval);
        sb.append(" IKE retrans timeout: ").append(mRetransmitTimeout);
        sb.append(" IKE retrans base: ").append(mRetransmitBase);
        sb.append(" IKE algorithms: ").append(mIkeAlgorithms);
        sb.append(" ESP algorithms: ").append(mEspAlgorithms);
        if (!TextUtils.isEmpty(mMmsEpdgName)) {
            sb.append(" MMS APN name: ").append(mMmsEpdgName);
        }
        if (!TextUtils.isEmpty(mEmcEpdgName)) {
            sb.append(" EMC APN name: ").append(mEmcEpdgName);
        }
        if (!TextUtils.isEmpty(mXcapEpdgName)) {
            sb.append(" XCAP APN name: ").append(mXcapEpdgName);
        }
        if (mUseOffloadedKeepalive) {
            sb.append(" Keepalive offload: ").append(mUseOffloadedKeepalive);
        }
        if (mRequestPcscfForAllPdn) {
            sb.append(" Request P-CSCF for all pdn: ").append(mRequestPcscfForAllPdn);
        }
        if (mPmtuDiscovery) {
            sb.append(" PMTU Discovery: ").append(mPmtuDiscovery);
        }
        return sb.toString();
    }
}
