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
package com.shannon.vendorcarrierconfig;

import static android.content.Intent.FLAG_RECEIVER_FOREGROUND;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.PersistableBundle;
import android.telephony.CarrierConfigManager;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.SparseArray;
import android.util.Xml;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Map;

public class ShannonVendorCarrierConfig {
    private static final boolean DBG = true;
    private static ShannonVendorCarrierConfig sMe = null;
    private static final String TAG = "ShannonVCC";
    private static String getTag() { return TAG; }
    public static String getServiceTAG(String s) { return TAG.concat(s); }

    /* Vendor prefix */
    public static final String KEY_PREFIX = "vendor.";
    /* WiFi */
    public static final String KEY_WIFI_NETWORK_VALIDATION_REQUIRED = KEY_PREFIX + "wifi_network_validation_required";
    public static final String KEY_WIFI_CAN_USE_IPV6 = KEY_PREFIX + "wifi_can_use_ipv6";
    public static final String KEY_WIFI_ACCESS_POINTS_HANDOVER_SUPPORTED = KEY_PREFIX + "wifi_access_points_handover_supported";
    public static final String KEY_WIFI_ACCESS_POINTS_HANDOVER_TIMEOUT = KEY_PREFIX + "wifi_access_points_handover_timeout";
    public static final String KEY_WIFI_PING_SERVER = KEY_PREFIX + "wifi_ping_server";
    public static final String KEY_WIFI_PING_PERIOD = KEY_PREFIX + "wifi_ping_period";
    public static final String KEY_WIFI_PING_INTERVAL = KEY_PREFIX + "wifi_ping_interval";
    public static final String KEY_WIFI_PING_COUNT = KEY_PREFIX + "wifi_ping_count";
    public static final String KEY_WIFI_PING_PACKET_SIZE = KEY_PREFIX + "wifi_ping_size";
    /* VoLTE */
    public static final String KEY_VOLTE_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "volte_supported_radio_technology";
    public static final String KEY_VOLTE_VOPS_REQUIRED = KEY_PREFIX + "volte_vops_required";
    public static final String KEY_VOLTE_VOPS_NOT_REQUIRED_ACTIVE_CALL = KEY_PREFIX + "volte_vops_not_required_active_call";
    // international roaming keys
    public static final String KEY_VOLTE_ROAMING_ENABLED = KEY_PREFIX + "volte_roaming_enabled";
    public static final String KEY_VOLTE_VOPS_REQUIRED_ROAMING = KEY_PREFIX + "volte_vops_required_roaming";
    /* *
    * This key contains list of all allowed international roaming networks to use for IMS over cellular, including VoLTE and VoNR.
    * Format: MCC or MCCMNC (PLMN) separated by comma (e.g. 123,45678)
    */
    public static final String KEY_VOLTE_ROAMING_ALLOWED_NETWORKS_LIST = KEY_PREFIX + "volte_roaming_allowed_networks";
    /* *
    * This key contains list of all allowed domestic roaming networks to use for IMS over cellular, including VoLTE and VoNR.
    * Format: MCCMNC (PLMN) separated by comma (e.g. 12345,678900)
    */
    public static final String KEY_VOLTE_DOMESTIC_ROAMING_ALLOWED_NETWORKS_LIST = KEY_PREFIX + "volte_domestic_roaming_allowed_networks";
    /* VoWiFi */
    public static final String KEY_VOWIFI_OEM_PROVISIONING_ENABLED = KEY_PREFIX + "vowifi_oem_provisioning_enabled";
    /* *
    * Key has similar purpose as KEY_CARRIER_WFC_IMS_AVAILABLE_BOOL but for roaming case
    * WFC enabled/disabled decision uses this flag AND user settings to provide combined value to service
    * For devices without WFC roaming UI it might be left untouched and keep default value, in this case,
    * WFC enabled/disabled will be controlled by KEY_CARRIER_DEFAULT_WFC_IMS_ROAMING_ENABLED_BOOL
    * */
    public static final String KEY_VOWIFI_ROAMING_ENABLED = KEY_PREFIX + "vowifi_roaming_enabled";
    /* *
    * Android native settings UI does not support WFC roaming
    * There is a way to receive UI change via ProvisioningManager.KEY_VOICE_OVER_WIFI_ROAMING_ENABLED_OVERRIDE
    * but it does not work if CarrierConfigManager.KEY_CARRIER_DEFAULT_WFC_IMS_ROAMING_ENABLED_BOOL is set to false
    * Add this configuration to ignore roaming and use home settings
    * */
    public static final String KEY_VOWIFI_USE_ROAMING_SETTINGS = KEY_PREFIX + "vowifi_use_roaming_settings";
    /* *
    * In case Location is unknown, it can be regarded as HOME or ROAM as per configuration. default is HOME.
    * */
    public static final String KEY_VOWIFI_UNKNOWN_LOCATION_AS_ROAMING = KEY_PREFIX + "vowifi_unknown_location_as_roaming";
    public static final String KEY_VOWIFI_SUPPORTED_IN_AIRPLANE_MODE = KEY_PREFIX + "vowifi_supported_in_airplane_mode";
    /* VoMobile */
    public static final String KEY_VOMOBILE_ENABLED = KEY_PREFIX + "vomobile_enabled";
    public static final String KEY_VOMOBILE_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "vomobile_supported_radio_technology";
    public static final String KEY_VOMOBILE_ONROAMING_ENABLED = KEY_PREFIX + "vomobile_onroaming_enabled";
    public static final String KEY_VOMOBILE_CURRENT_CELLULAR_SIGNAL_IN = KEY_PREFIX + "vomobile_current_cellular_signal_in";
    public static final String KEY_VOMOBILE_CURRENT_CELLULAR_SIGNAL_OUT = KEY_PREFIX + "vomobile_current_cellular_signal_out";
    public static final String KEY_VOMOBILE_ROAMING_OVERRIDE_SIGNAL_STRENGTH = KEY_PREFIX + "vomobile_roaming_override_signal_strength";
    public static final String KEY_VOMOBILE_OTHER_CELLULAR_SIGNAL_IN = KEY_PREFIX + "vomobile_other_cellular_signal_in";
    public static final String KEY_VOMOBILE_OTHER_CELLULAR_SIGNAL_OUT = KEY_PREFIX + "vomobile_other_cellular_signal_out";
    public static final String KEY_VOMOBILE_IGNORE_CS_STATE = KEY_PREFIX + "vomobile_ignore_cs_state";
    public static final String KEY_VOMOBILE_PREFERRED = KEY_PREFIX +"vomobile_preferred";
    // Forbidden networks in roaming. Possible values MCC or MCCMNC (PLMN) separated by comma (123,45678)
    public static final String KEY_VOMOBILE_FORBIDDEN_NETWORKS_IN_ROAMING = KEY_PREFIX + "vomobile_forbidden_networks_in_roaming";
    public static final String KEY_VOMOBILE_INTERSTACK_HANDOVER_SUPPORTED = KEY_PREFIX + "vomobile_interstack_handover_supported";
    /* IMS PDN handover */
    public static final String KEY_IMS_HANDOVER_SUPPORTED = KEY_PREFIX + "ims_handover_supported";
    public static final String KEY_IMS_HANDOVER_SUPPORTED_IN_CALL = KEY_PREFIX + "ims_handover_supported_in_call";
    public static final String KEY_IMS_HANDOVER_SUPPORTED_ROAMING = KEY_PREFIX + "ims_handover_supported_roaming";
    public static final String KEY_IMS_HANDOVER_SUPPORTED_IN_CALL_ROAMING = KEY_PREFIX + "ims_handover_supported_in_call_roaming";
    /* *
    * List of allowed cellular radio technologies available for handover
    * RAT is used as source or destination depending on handover direction: cell->IWLAN or IWLAN->cell
    * If RAT is excluded from list both directions for handover are prohibited
    * */
    public static final String KEY_IMS_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "ims_handover_supported_radio_technology";
    /* *
     * List of restricted radio technologies for handover. List can contain all framework supported RATs, including non cellular
     * Format: source|source>destination, >destination, source>, >destination|destination
     * */
    public static final String KEY_IMS_HANDOVER_RESTRICTED_RADIO_TECHNOLOGY = KEY_PREFIX + "ims_handover_restricted_radio_technology";
    public static final String KEY_IMS_HANDOVER_USE_PROFILE_DEFAULT_NETWORK = KEY_PREFIX + "ims_handover_use_profile_default_network";
    public static final String KEY_IMS_HANDOVER_DEFAULT_IN = KEY_PREFIX + "ims_handover_default_in";
    public static final String KEY_IMS_HANDOVER_ROVE_IN = KEY_PREFIX + "ims_handover_rove_in";
    public static final String KEY_IMS_HANDOVER_ROVE_IN_ROAMING = KEY_PREFIX + "ims_handover_rove_in_roaming";
    public static final String KEY_IMS_HANDOVER_ROVE_IN_HYSTERESIS_ON = KEY_PREFIX + "ims_handover_rove_in_hysteresis_on";
    public static final String KEY_IMS_HANDOVER_ROVE_IN_WIFI_PREFERRED = KEY_PREFIX + "ims_handover_rove_in_wifi_preferred";
    public static final String KEY_IMS_HANDOVER_ROVE_IN_CELL_PREFERRED = KEY_PREFIX + "ims_handover_rove_in_cell_preferred";
    public static final String KEY_IMS_HANDOVER_HAND_IN = KEY_PREFIX + "ims_handover_hand_in";
    public static final String KEY_IMS_HANDOVER_HAND_IN_ROAMING = KEY_PREFIX + "ims_handover_hand_in_roaming";
    public static final String KEY_IMS_HANDOVER_HAND_IN_HYSTERESIS_ON = KEY_PREFIX + "ims_handover_hand_in_hysteresis_on";
    public static final String KEY_IMS_HANDOVER_HAND_IN_WIFI_PREFERRED = KEY_PREFIX + "ims_handover_hand_in_wifi_preferred";
    public static final String KEY_IMS_HANDOVER_HAND_IN_CELL_PREFERRED = KEY_PREFIX + "ims_handover_hand_in_cell_preferred";
    public static final String KEY_IMS_HANDOVER_DEFAULT_OUT = KEY_PREFIX + "ims_handover_default_out";
    public static final String KEY_IMS_HANDOVER_ROVE_OUT = KEY_PREFIX + "ims_handover_rove_out";
    public static final String KEY_IMS_HANDOVER_ROVE_OUT_HYSTERESIS_ON = KEY_PREFIX + "ims_handover_rove_out_hysteresis_on";
    public static final String KEY_IMS_HANDOVER_ROVE_OUT_WIFI_PREFERRED = KEY_PREFIX + "ims_handover_rove_out_wifi_preferred";
    public static final String KEY_IMS_HANDOVER_ROVE_OUT_CELL_PREFERRED = KEY_PREFIX + "ims_handover_rove_out_cell_preferred";
    public static final String KEY_IMS_HANDOVER_HAND_OUT = KEY_PREFIX + "ims_handover_hand_out";
    public static final String KEY_IMS_HANDOVER_HAND_OUT_HYSTERESIS_ON = KEY_PREFIX + "ims_handover_hand_out_hysteresis_on";
    public static final String KEY_IMS_HANDOVER_HAND_OUT_WIFI_PREFERRED = KEY_PREFIX + "ims_handover_hand_out_wifi_preferred";
    public static final String KEY_IMS_HANDOVER_HAND_OUT_CELL_PREFERRED = KEY_PREFIX + "ims_handover_hand_out_cell_preferred";
    public static final String KEY_IMS_HANDOVER_CELL_MID = KEY_PREFIX + "ims_handover_cell_mid";
    public static final String KEY_IMS_HANDOVER_WIFI_PING_RTT = KEY_PREFIX + "ims_handover_wifi_ping_rtt";
    public static final String KEY_IMS_HANDOVER_RTP_DOWNLINK_CHECK_INTERVAL = KEY_PREFIX + "ims_handover_rtp_downlink_interval";
    public static final String KEY_IMS_HANDOVER_RTP_DOWNLINK_LOST_RATE = KEY_PREFIX + "ims_handover_rtp_downlink_lost_rate";
    public static final String KEY_IMS_HANDOVER_RTP_JITTER_MAX = KEY_PREFIX + "ims_handover_rtp_jitter_max";
    public static final String KEY_IMS_HANDOVER_NO_RTP_INTERVAL = KEY_PREFIX + "ims_handover_no_rtp_interval";
    public static final String KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE = KEY_PREFIX + "ims_handover_hysteresis_timer_volte";
    public static final String KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOWIFI = KEY_PREFIX + "ims_handover_hysteresis_timer_vowifi";
    public static final String KEY_IMS_HANDOVER_REGISTRATION_TIMER = KEY_PREFIX + "ims_handover_registration_timer";
    public static final String KEY_IMS_HANDOVER_IGNORE_CALL_DOWNGRADE = KEY_PREFIX + "ims_handover_ignore_call_downgrade";
    public static final String KEY_IMS_HANDOVER_IGNORE_CALL_UPGRADE = KEY_PREFIX + "ims_handover_ignore_call_upgrade";
    /* Emergency PDN keys */
    public static final String KEY_EMERGENCY_HANDOVER_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "emergency_handover_supported_radio_technology";
    public static final String KEY_EMERGENCY_OVER_WIFI_SUPPORTED = KEY_PREFIX + "emergency_over_wifi_supported";
    public static final String KEY_EMERGENCY_OVER_WIFI_SUPPORTED_ROAMING = KEY_PREFIX + "emergency_over_wifi_supported_roaming";
    public static final String KEY_EMERGENCY_OVER_WIFI_PROVISIONING_ENABLED = KEY_PREFIX + "emergency_over_wifi_provisioning_enabled";
    public static final String KEY_EMERGENCY_OVER_WIFI_IGNORE_USER_SETTING = KEY_PREFIX + "emergency_over_wifi_ignore_user_setting";
    public static final String KEY_EMERGENCY_OVER_WIFI_IGNORE_USER_SETTING_HANDOVER = KEY_PREFIX + "emergency_over_wifi_ignore_user_setting_for_handover";
    public static final String KEY_EMERGENCY_HANDOVER_ENABLED = KEY_PREFIX + "emergency_handover_enabled";
    public static final String KEY_EMERGENCY_HANDOVER_ENABLED_ROAMING = KEY_PREFIX + "emergency_handover_enabled_roaming";
    public static final String KEY_EMERGENCY_HANDOVER_WIFI_RSSI_IN = KEY_PREFIX + "emergency_handover_wifi_rssi_in";
    public static final String KEY_EMERGENCY_HANDOVER_WIFI_RSSI_OUT = KEY_PREFIX + "emergency_handover_wifi_rssi_out";
    public static final String KEY_EMERGENCY_HANDOVER_EUTRAN_RSRP_IN = KEY_PREFIX + "emergency_handover_eutran_rsrp_in";
    public static final String KEY_EMERGENCY_HANDOVER_EUTRAN_RSRP_OUT = KEY_PREFIX + "emergency_handover_eutran_rsrp_out";
    public static final String KEY_EMERGENCY_HANDOVER_EUTRAN_RSRP_MID = KEY_PREFIX + "emergency_handover_eutran_rsrp_mid";
    /* CBS PDN keys */
    public static final String KEY_CBS_OVER_WIFI_SUPPORTED = KEY_PREFIX + "cbs_over_wifi_supported";
    /* MMS PDN keys */
    public static final String KEY_MMS_OVER_WIFI_SUPPORTED = KEY_PREFIX + "mms_over_wifi_supported";
    public static final String KEY_MMS_OVER_WIFI_PREFERRED  = KEY_PREFIX + "mms_over_wifi_preferred";
    public static final String KEY_MMS_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "mms_supported_radio_technology";
    /* XCAP PDN keys */
    public static final String KEY_XCAP_OVER_WIFI_SUPPORTED = KEY_PREFIX + "xcap_over_wifi_supported";
    public static final String KEY_XCAP_OVER_WIFI_PREFERRED  = KEY_PREFIX + "xcap_over_wifi_preferred";
    public static final String KEY_XCAP_SUPPORTED_RADIO_TECHNOLOGY = KEY_PREFIX + "xcap_supported_radio_technology";
    public static final String KEY_XCAP_HANDOVER_ENABLED = KEY_PREFIX + "xcap_handover_enabled";
    public static final String KEY_XCAP_USE_IMS_PRECONDITION_FOR_WIFI = KEY_PREFIX + "xcap_use_ims_precondition_for_wifi";
    /* Hysteresis timer for APN other than IMS */
    public static final String KEY_HYSTERESIS_TIMER_CELLULAR = KEY_PREFIX + "hysteresis_timer_cellular";
    public static final String KEY_HYSTERESIS_TIMER_WIFI = KEY_PREFIX + "hysteresis_timer_wifi";
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
    /* PDN retry parameters string. Format: name=<algorithm name or operator name>, [variable parameters specific for algorithm separated by comma] */
    public static final String KEY_EPDG_CONNECTION_RETRY_PARAMETERS = KEY_PREFIX + KEY_EPDG + "connection_retry_parameters";

    /* SIM related keys */
    public static final String KEY_SIM_USIM_ONLY = KEY_PREFIX + "sim_usim_only";

    /* Network Interfaces */
    public static final String KEY_NETWORK_IWLAN_INTERFACE_NAME = KEY_PREFIX + "network_iwlan_interface_name";
    public static final String KEY_NETWORK_CELLULAR_INTERFACE_NAME = KEY_PREFIX + "network_cellular_interface_name";

    /* Transport related */
    public static final String KEY_TRANSPORT_CELLULAR_PERMANENT_ERROR_RETRY = KEY_PREFIX + "transport_cellular_permanent_error_retry";
    public static final String KEY_TRANSPORT_CELLULAR_PERMANENT_ERROR_TIMEOUT = KEY_PREFIX + "transport_cellular_permanent_error_timeout";
    public static final String KEY_TRANSPORT_CELLULAR_PERMANENT_ERROR_HANDOVER_TIMEOUT = KEY_PREFIX + "transport_cellular_permanent_error_handover_timeout";
    public static final String KEY_TRANSPORT_IWLAN_PERMANENT_ERROR_RETRY = KEY_PREFIX + "transport_iwlan_permanent_error_retry";
    public static final String KEY_TRANSPORT_IWLAN_PERMANENT_ERROR_TIMEOUT = KEY_PREFIX + "transport_iwlan_permanent_error_timeout";
    public static final String KEY_TRANSPORT_IWLAN_PERMANENT_ERROR_HANDOVER_TIMEOUT = KEY_PREFIX + "transport_iwlan_permanent_error_handover_timeout";
    public static final String KEY_TRANSPORT_INITIAL_NON_PERMANENT_ERROR_MAX_RETRY = KEY_PREFIX +
            "transport_initial_non_permanent_error_max_retry";
    public static final String KEY_TRANSPORT_INITIAL_NON_PERMANENT_ERROR_TIMEOUT = KEY_PREFIX +
            "transport_initial_non_permanent_error_timeout";
    public static final String KEY_TRANSPORT_HANDOVER_NON_PERMANENT_ERROR_MAX_RETRY = KEY_PREFIX +
            "transport_handover_non_permanent_error_max_retry";
    public static final String KEY_TRANSPORT_HANDOVER_NON_PERMANENT_ERROR_TIMEOUT = KEY_PREFIX +
            "transport_handover_non_permanent_error_timeout";
    public static final String KEY_TRANSPORT_INITIAL_ATTACH_TIMEOUT = KEY_PREFIX + "transport_initial_attach_timeout";

    /* custom broadcasts */
    public static final String BROADCAST_ACTION_VENDOR_CONFIG_CHANGED = "com.shannon.ACTION.VCC.vendor_config_changed";
    public static final String BROADCAST_EXTRA_VENDOR_CONFIG = "VENDOR_CONFIG";
    public static final String BROADCAST_ACTION_REQUEST_VENDOR_CONFIG = "com.shannon.ACTION.VCC.request_vendor_config";
    public static final String BROADCAST_EXTRA_PACKAGE_NAME = "PACKAGE_NAME";
    public static final String BROADCAST_ACTION_VENDOR_CONFIG_MEDIA_MOUNTED = "com.shannon.ACTION.VCC.vendor_config_media_mounted";
    protected static final String BROADCAST_ACTION_VENDOR_CONFIG_DATA_CHANGED = "com.shannon.ACTION.VCC.vendor_config_data_changed";

    /* broadcast permission*/
    private static final String IWLAN_CONFIGURATION_PERMISSION = "com.shannon.permission.IWLAN_CONFIGURATION";

    /* messages */
    private static final int SIM_INSERTED = 1;
    private static final int CARRIER_CONFIG_CHANGED = 2;
    private static final int VENDOR_CONFIG_CHANGED = 3;
    private static final int USER_CONFIG_CHANGED = 4;
    private static final int MEDIA_MOUNTED = 5;
    private static final int REQUEST_CONFIG = 6;

    /*
    * NOTE: Android carrier ID list used to differentiate operators, see
    * <a href="https://android.googlesource.com/platform/packages/providers/TelephonyProvider/+/master/assets/carrier_list.textpb">carrier id list</a>
    */
    /* file names */
    private static final String PREFERENCES_FILE_PACKAGE = "com.shannon.";
    private static final String CARRIER_ID_PREFIX = "vendor_config_carrierid_";
    private static final String MCCMNC_PREFIX = "vendor_config_mccmnc_";

    private final Context mContext;
    private final SimStateBroadcastReceiver mReceiver;
    private final SparseArray<ShannonVendorCarrierConfigProvider> mProviders;

    @VisibleForTesting
    protected class SimStateBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (action == null) {
                loge("NULL action for intent " + intent);
                return;
            }
            log("Got intent " + intent);
            switch (action) {
                case TelephonyIntents.ACTION_SIM_STATE_CHANGED: {
                    int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
                    if (phoneId < 0) {
                        loge("Invalid phone ID: " + phoneId);
                        return;
                    }

                    int state = SubscriptionManager.getSimStateForSlotIndex(phoneId);
                    log("Slot " + phoneId + " SIM state " + getSimStateString(state));
                    switch (state) {
                        case TelephonyManager.SIM_STATE_READY:
                        case TelephonyManager.SIM_STATE_LOADED:
                        case TelephonyManager.SIM_STATE_PRESENT: {
                            int subId = intent.getIntExtra(PhoneConstants.SUBSCRIPTION_KEY, -1);
                            if (subId < 0) {
                                return;
                            }
                            ShannonVendorCarrierConfigProvider provider = mProviders.get(phoneId);
                            if (provider != null) {
                                if (provider.getSubId() != subId) {
                                    log("SIM subId changed " + provider.getSubId() + "->" + subId +
                                            ".Remove configuration provider for " + phoneId);
                                    mProviders.delete(phoneId);
                                    provider.destroy();
                                } else {
                                    log("VendorConfig provider was already created for slot " + phoneId);
                                    return;
                                }
                            }
                            log("Create configuration provider for " + phoneId);
                            provider = new ShannonVendorCarrierConfigProvider(phoneId, subId);
                            mProviders.put(phoneId, provider);

                            Message message = Message.obtain(provider.getMessageHandler(), SIM_INSERTED);
                            message.sendToTarget();
                            break;
                        }
                        default: {
                            ShannonVendorCarrierConfigProvider provider = mProviders.get(phoneId);
                            if (provider != null) {
                                log("Remove configuration provider for " + phoneId);
                                mProviders.delete(phoneId);

                                provider.destroy();
                            }
                        }
                    }
                    break;
                }
            }
        }

        private String getSimStateString(int state) {
            switch (state) {
                case TelephonyManager.SIM_STATE_ABSENT: return "ABSENT";
                case TelephonyManager.SIM_STATE_PIN_REQUIRED: return "PIN_REQUIRED";
                case TelephonyManager.SIM_STATE_PUK_REQUIRED: return "PUK_REQUIRED";
                case TelephonyManager.SIM_STATE_NETWORK_LOCKED: return "LOCKED";
                case TelephonyManager.SIM_STATE_READY: return "READY";
                case TelephonyManager.SIM_STATE_NOT_READY: return "NOT_READY";
                case TelephonyManager.SIM_STATE_PERM_DISABLED: return "DISABLED";
                case TelephonyManager.SIM_STATE_CARD_IO_ERROR: return "IO_ERROR";
                case TelephonyManager.SIM_STATE_CARD_RESTRICTED: return "RESTRICTED";
                case TelephonyManager.SIM_STATE_LOADED: return "LOADED";
                case TelephonyManager.SIM_STATE_PRESENT: return "PRESENT";
                default:
                    return "UNKNOWN";
            }
        }

        public void onServiceLoaded() {
            TelephonyManager tm = mContext.getSystemService(TelephonyManager.class);
            if (tm == null) return;

            for (int phoneId = 0; phoneId < tm.getActiveModemCount(); phoneId++) {
                int state = SubscriptionManager.getSimStateForSlotIndex(phoneId);
                log("Slot " + phoneId + " SIM state " + getSimStateString(state));
                switch (state) {
                    case TelephonyManager.SIM_STATE_READY:
                    case TelephonyManager.SIM_STATE_LOADED:
                    case TelephonyManager.SIM_STATE_PRESENT: {
                        if (mProviders.get(phoneId) != null) {
                            log("VendorConfig provider was already created for slot " + phoneId);
                            return;
                        }
                        log("Create configuration provider for " + phoneId);

                        int[] subIds = SubscriptionManager.getSubId(phoneId);
                        int subId = (subIds != null && subIds.length > 0) ? subIds[0] : -1;
                        if (SubscriptionManager.isValidSubscriptionId(subId)) {
                            ShannonVendorCarrierConfigProvider provider = new ShannonVendorCarrierConfigProvider(phoneId, subId);
                            mProviders.put(phoneId, provider);

                            Message message = Message.obtain(provider.getMessageHandler(), SIM_INSERTED);
                            message.sendToTarget();
                        }
                        break;
                    }
                    default: ;
                }
            }
        }
    }

    public class ShannonVendorCarrierConfigProvider {
        private final String TAG;
        private final int mSlotId;
        private final int mSubId;
        private final HandlerThread mHandlerThread;
        private final Handler mHandler;
        private final VendorConfigBroadcastReceiver mReceiver;
        private TelephonyManager mTelephonyManager;
        private Bundle mBundle;

        protected class VendorConfigBroadcastReceiver extends BroadcastReceiver {
            @Override
            public void onReceive(Context context, Intent intent) {
                final String action = intent.getAction();
                if (action == null) {
                    loge("NULL action for intent " + intent);
                    return;
                }
                log("Got intent " + intent);
                switch (action) {
                    case CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED: {
                        int slotId = intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1);
                        if ((slotId < 0) || (slotId != mSlotId)) {
                            return;
                        }
                        log("Carrier config changed for slot " + slotId);

                        Message message = Message.obtain(mHandler, CARRIER_CONFIG_CHANGED);
                        message.sendToTarget();
                        break;
                    }
                    case BROADCAST_ACTION_VENDOR_CONFIG_DATA_CHANGED: {
                        log("Vendor config changed, slot " + mSlotId);

                        Message message = Message.obtain(mHandler, VENDOR_CONFIG_CHANGED);
                        message.sendToTarget();
                        break;
                    }
                    case BROADCAST_ACTION_VENDOR_CONFIG_MEDIA_MOUNTED: {
                        log("External media mounted");

                        Message message = Message.obtain(mHandler, MEDIA_MOUNTED);
                        message.sendToTarget();
                        break;
                    }
                    case BROADCAST_ACTION_REQUEST_VENDOR_CONFIG: {
                        int slotId = intent.getIntExtra(PhoneConstants.SLOT_KEY, -1);
                        if ((slotId < 0) || (slotId != mSlotId)) {
                            return;
                        }
                        log("Vendor config request from " + intent.getStringExtra(BROADCAST_EXTRA_PACKAGE_NAME));

                        Message message = Message.obtain(mHandler, REQUEST_CONFIG);
                        message.sendToTarget();
                        break;
                    }
                }
            }
        }

        @VisibleForTesting
        VendorConfigBroadcastReceiver getBroadcastReceiver() { return mReceiver; }

        public ShannonVendorCarrierConfigProvider(int slotId, int subId) {
            TAG = ShannonVendorCarrierConfig.getTag() + "-" + slotId + "-P";

            mSlotId = slotId;
            mSubId = subId;
            mTelephonyManager = null;
            mBundle = null;
            mReceiver = new VendorConfigBroadcastReceiver();

            mHandlerThread = new HandlerThread(ShannonVendorCarrierConfigProvider.class.getSimpleName() + slotId);
            mHandlerThread.start();
            mHandler = new Handler(mHandlerThread.getLooper()) {
                @Override
                public void handleMessage(Message message) {
                    switch (message.what) {
                        case SIM_INSERTED: {
                            mTelephonyManager = mContext.getSystemService(TelephonyManager.class);
                            if (mTelephonyManager != null) {
                                mTelephonyManager = mTelephonyManager.createForSubscriptionId(mSubId);
                            }
                            /* pass thru to next */
                        }
                        case CARRIER_CONFIG_CHANGED: {
                            if (mBundle == null) {
                                mBundle = getConfigBundle();

                                log("Vendor configuration loaded");
                                printBundle(mBundle);

                                /* one time send full list */
                                sendVendorConfigBroadcast(mBundle);
                            } else {
                                Bundle newConfigBundle = getConfigBundle();
                                if (!newConfigBundle.isEmpty()) {
                                    Bundle bundle = updateBundleNonOverlap(mBundle, newConfigBundle);
                                    if (!bundle.isEmpty()) {
                                        log("Vendor configuration re-loaded");
                                        printBundle(bundle);

                                        sendVendorConfigBroadcast(bundle);
                                    }
                                }
                            }
                            Bundle bundle = updateBundleNonOverlap(mBundle, getCarrierConfigBundle());
                            if (!bundle.isEmpty()) {
                                log("Vendor configuration updated by OTA");
                                printBundle(bundle);

                                sendVendorConfigBroadcast(bundle);
                                saveBundleToSharedPreferences(bundle);
                            }
                            break;
                        }
                        case MEDIA_MOUNTED:
                        case VENDOR_CONFIG_CHANGED: {
                            Bundle newConfigBundle = getConfigBundle();
                            if (!newConfigBundle.isEmpty()) {
                                Bundle bundle = updateBundleNonOverlap(mBundle, newConfigBundle);
                                if (bundle.isEmpty()) break;

                                log("Vendor configuration updated by file");
                                printBundle(bundle);

                                sendVendorConfigBroadcast(bundle);
                            }
                            break;
                        }
                        case USER_CONFIG_CHANGED: {
                            Bundle userBundle = (Bundle)message.obj;
                            if (userBundle == null) {
                                return;
                            }
                            Bundle bundle = updateBundleNonOverlap(mBundle, userBundle);
                            if (!bundle.isEmpty()) {
                                log("Vendor configuration updated by user");
                                printBundle(bundle);

                                sendVendorConfigBroadcast(bundle);
                                saveBundleToSharedPreferences(bundle);
                            }
                            break;
                        }
                        case REQUEST_CONFIG: {
                            /* send full config as requested */
                            if (mBundle != null && !mBundle.isEmpty()) {
                                sendVendorConfigBroadcast(mBundle);
                            }
                        }
                    }
                }
            };

            IntentFilter filter = new IntentFilter();
            filter.addAction(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
            filter.addAction(BROADCAST_ACTION_REQUEST_VENDOR_CONFIG);
            filter.addAction(BROADCAST_ACTION_VENDOR_CONFIG_DATA_CHANGED);
            filter.addAction(BROADCAST_ACTION_VENDOR_CONFIG_MEDIA_MOUNTED);
            mContext.registerReceiver(mReceiver, filter);
        }

        public Handler getMessageHandler() { return mHandler; }
        public int getSubId() { return mSubId; }

        public void destroy() {
            mContext.unregisterReceiver(mReceiver);
            mHandlerThread.quit();
        }

        public Bundle getBundle() {
            if (mBundle != null) {
                return new Bundle(mBundle);
            }
            return new Bundle();
        }

        private Bundle updateBundleNonOverlap(Bundle to, Bundle from) {
            if (to == null || from == null || to.isEmpty() || from.isEmpty()) {
                return new Bundle();
            }
            Bundle bundle = new Bundle(from);
            for (String key : from.keySet()) {
                if (to.containsKey(key)) {
                    Object objTo = to.get(key);
                    Object objFrom = bundle.get(key);

                    if ((objTo == null) || !objTo.equals(objFrom)) {
                        to.remove(key);
                    } else {
                        bundle.remove(key);
                    }
                } else {
                    bundle.remove(key);
                }
            }
            if (!bundle.isEmpty()) {
                /* overwrite all changed values */
                to.putAll(bundle);
            }
            return bundle;
        }

        private Bundle getCarrierConfigBundle() {
            CarrierConfigManager ccm = mContext.getSystemService(CarrierConfigManager.class);
            if (ccm != null) {
                // If an invalid subId is used, this bundle will contain default values.
                PersistableBundle bundle = ccm.getConfigForSubId(mSubId);
                if ((bundle != null) &&
                        CarrierConfigManager.isConfigForIdentifiedCarrier(bundle)) {
                    CarrierConfigUtils utils = new CarrierConfigUtils(bundle);
                    return utils.convertToLocal();
                }
            }
            return new Bundle();
        }

        @VisibleForTesting
        protected Bundle getSharedPreferencesBundle(int carrierId) {
            Bundle bundle = new Bundle();

            String preferencesFileName = PREFERENCES_FILE_PACKAGE + CARRIER_ID_PREFIX + carrierId;
            File file = mContext.getSharedPreferencesPath(preferencesFileName);
            if (!file.exists()) {
                return bundle;
            }
            log("Get preferences from " + preferencesFileName);

            SharedPreferences preferences = mContext.getSharedPreferences(preferencesFileName, Context.MODE_PRIVATE);
            if (preferences != null) {
                Map<String, ?> map = preferences.getAll();
                if (!map.isEmpty()) {
                    for (String key : map.keySet()) {
                        Object value = map.get(key);
                        if (value instanceof Integer) {
                            bundle.putInt(key, (Integer) value);
                        } else if (value instanceof Boolean) {
                            bundle.putBoolean(key, (Boolean) value);
                        } else if (value instanceof String) {
                            bundle.putString(key, (String) value);
                        } else if (value instanceof Float) {
                            bundle.putFloat(key, (Float) value);
                        }
                    }
                }
            }
            return bundle;
        }

        private Bundle getSharedPreferencesBundle() {
            Bundle bundle = new Bundle();

            try {
                int carrierId = mTelephonyManager.getSimSpecificCarrierId();
                if (carrierId != TelephonyManager.UNKNOWN_CARRIER_ID) {
                    bundle = getSharedPreferencesBundle(carrierId);
                    if (!bundle.isEmpty()) {
                        log("Preferences from SIM specific carrierId");
                        return bundle;
                    }
                }
                carrierId = mTelephonyManager.getSimCarrierId();
                if (carrierId != TelephonyManager.UNKNOWN_CARRIER_ID) {
                    bundle = getSharedPreferencesBundle(carrierId);
                    if (!bundle.isEmpty()) {
                        log("Preferences from SIM carrierId");
                        return bundle;
                    }
                }
                carrierId = mTelephonyManager.getCarrierIdFromSimMccMnc();
                if (carrierId != TelephonyManager.UNKNOWN_CARRIER_ID) {
                    bundle = getSharedPreferencesBundle(carrierId);
                    if (!bundle.isEmpty()) {
                        log("Preferences from MCC/MNC carrierId");
                        return bundle;
                    }
                }
            } catch (Exception e) {
                loge("Failed to read shared preferences: " + e);
            }
            return bundle;
        }

        @VisibleForTesting
        protected void saveBundleToSharedPreferences(Bundle bundle) {
            if (bundle == null || bundle.isEmpty()) {
                return;
            }
            int carrierId = mTelephonyManager.getSimSpecificCarrierId();
            if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                carrierId = mTelephonyManager.getSimCarrierId();
                if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                    carrierId = mTelephonyManager.getCarrierIdFromSimMccMnc();
                }
            }
            if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                loge("Carrier ID is unknown, saving to shared preferences not supported");
                return;
            }
            String preferencesFileName = PREFERENCES_FILE_PACKAGE + CARRIER_ID_PREFIX + carrierId;
            log("Save preference to " + preferencesFileName);

            SharedPreferences preferences = mContext.getSharedPreferences(preferencesFileName, Context.MODE_PRIVATE);
            if (preferences != null) {
                SharedPreferences.Editor editor = preferences.edit();
                for (String key : bundle.keySet()) {
                    Object value = bundle.get(key);
                    if (value instanceof Integer) {
                        editor.putInt(key, (Integer)value);
                    } else if (value instanceof Boolean) {
                        editor.putBoolean(key, (Boolean)value);
                    } else if (value instanceof String) {
                        editor.putString(key, (String)value);
                    } else if (value instanceof Float) {
                        editor.putFloat(key, (Float)value);
                    }
                }
                editor.apply();
            }
        }

        private boolean isExternalStorageReadable() {
            String state = Environment.getExternalStorageState();
            return (Environment.MEDIA_MOUNTED.equals(state) ||
                    Environment.MEDIA_MOUNTED_READ_ONLY.equals(state));
        }

        private Bundle readConfigFromXml(XmlPullParser parser) throws IOException, XmlPullParserException {
            if (parser == null) {
                return new Bundle();
            }
            PersistableBundle bundle = new PersistableBundle();

            int event;
            while (((event = parser.next()) != XmlPullParser.END_DOCUMENT)) {
                if (event == XmlPullParser.START_TAG && "vendor_config".equals(parser.getName())) {
                    PersistableBundle configFragment = PersistableBundle.restoreFromXml(parser);
                    bundle.putAll(configFragment);
                }
            }
            return new Bundle(bundle);
        }

        private Bundle getConfigFromAssets(String fileName) {
            Bundle bundle = new Bundle();
            try {
                String[] names = mContext.getAssets().list("");
                if (names == null) {
                    loge("Assets list is null");
                    return bundle;
                }
                for (String name : names) {
                    if (name.startsWith(fileName)) {
                        log("Assets file name : " + name);

                        XmlPullParser parser = Xml.newPullParser();
                        parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false);

                        InputStream input = mContext.getAssets().open(name);
                        parser.setInput(input, "utf-8");
                        bundle = readConfigFromXml(parser);
                        input.close();
                        break;
                    }
                }
            } catch (Exception e) {
                loge("Failed to read configuration from assets: " + e);
            }
            return bundle;
        }

        @VisibleForTesting
        protected Bundle getConfigFromExternal(String fileName) {
            Bundle bundle = new Bundle();
            try {
                if (!isExternalStorageReadable()) {
                    loge("External storage not mounted");
                    return bundle;
                }
                File externalDir = mContext.getExternalFilesDir(null);
                if (externalDir == null) {
                    loge("No external directory");
                    return bundle;
                }
                log("External files location is " + externalDir.getAbsolutePath());

                File[] files = externalDir.listFiles();
                if (files == null) {
                    loge("File list is null");
                    return bundle;
                }
                for (File file : files) {
                    String name = file.getName();
                    if (name.startsWith(fileName)) {
                        log("External file name : " + name);

                        XmlPullParser parser = Xml.newPullParser();
                        parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, false);

                        InputStream input = new FileInputStream(file);
                        parser.setInput(input, "utf-8");
                        bundle = readConfigFromXml(parser);
                        input.close();
                        break;
                    }
                }
            } catch (Exception e) {
                loge("Failed to read configuration from external: " + e);
            }
            return bundle;
        }

        private Bundle getCarrierConfigBundle(int carrierId) {
            Bundle bundle = new Bundle();

            if (carrierId != TelephonyManager.UNKNOWN_CARRIER_ID) {
                /* external files always preferable */
                bundle = getConfigFromExternal(CARRIER_ID_PREFIX + carrierId + "_");
                if (bundle.isEmpty()) {
                    bundle = getConfigFromAssets(CARRIER_ID_PREFIX + carrierId + "_");
                }
            }
            return bundle;
        }

        private Bundle getConfigBundle() {
            Bundle bundle = new Bundle();
            try {
                /* external files always preferable */
                bundle = getConfigFromExternal("default");
                if (bundle.isEmpty()) {
                    bundle = getConfigFromAssets("default");
                }
                /* carrier ID */
                int carrierId = mTelephonyManager.getSimSpecificCarrierId();
                log("Try SIM specific carrierId = " + carrierId);
                Bundle carrierBundle = getCarrierConfigBundle(carrierId);
                if (carrierBundle.isEmpty()) {
                    carrierId = mTelephonyManager.getSimCarrierId();
                    log("Try SIM carrierId = " + carrierId);
                    carrierBundle = getCarrierConfigBundle(carrierId);
                    if (carrierBundle.isEmpty()) {
                        carrierId = mTelephonyManager.getCarrierIdFromSimMccMnc();
                        log("Try carrierId from MCC/MNC = " + carrierId);
                        carrierBundle = getCarrierConfigBundle(carrierId);
                    }
                }
                if (carrierBundle.isEmpty()) {
                    String op = mTelephonyManager.getSimOperator();
                    if (!TextUtils.isEmpty(op)) {
                        log("Try MCC/MNC = " + op);
                        /* external files always preferable */
                        carrierBundle = getConfigFromExternal(MCCMNC_PREFIX + op);
                        if (carrierBundle.isEmpty()) {
                            carrierBundle = getConfigFromAssets(MCCMNC_PREFIX + op);
                        }
                    }
                }
                if (!carrierBundle.isEmpty()) {
                    log("Default vendor config updated by carrier bundle");
                    updateBundleNonOverlap(bundle, carrierBundle);
                }
                Bundle prefBundle = getSharedPreferencesBundle();
                if (!prefBundle.isEmpty()) {
                    log("Default vendor carrier config updated by shared preferences");
                    updateBundleNonOverlap(bundle, prefBundle);
                }
            } catch (Exception e) {
                loge("Failed to read configuration");
            }
            return bundle;
        }

        private void sendVendorConfigBroadcast(Bundle bundle) {
            Intent intent = new Intent(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);
            intent.putExtra(PhoneConstants.SLOT_KEY, mSlotId);
            intent.putExtra(BROADCAST_EXTRA_VENDOR_CONFIG, bundle);
            intent.setFlags(FLAG_RECEIVER_FOREGROUND);
            try {
                mContext.sendBroadcast(intent, IWLAN_CONFIGURATION_PERMISSION);
                log("Send intent=" + intent);
            } catch (Exception e) {
                loge("Failed to send broadcast " + e);
            }
        }

        private void printBundle(Bundle bundle) {
            for (String key : bundle.keySet()) {
                log("key=" + key + " value=" + bundle.get(key));
            }
        }

        @VisibleForTesting
        protected void setTelephonyManager(TelephonyManager tm) { mTelephonyManager = tm; }

        private void log(String s) {
            if (DBG) {
                Rlog.d(TAG, s);
            }
        }

        private void loge(String s) {
            Rlog.e(TAG, s);
        }
    }

    public ShannonVendorCarrierConfig(Context context) {
        mContext = context.createDeviceProtectedStorageContext();
        mProviders = new SparseArray<>();
        mReceiver = new SimStateBroadcastReceiver();
        mReceiver.onServiceLoaded();

        sMe = this;

        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        mContext.registerReceiver(mReceiver, filter);
    }

    public void destroy() {
        for (int i = 0; i < mProviders.size(); i++) {
            ShannonVendorCarrierConfigProvider provider = mProviders.valueAt(i);
            provider.destroy();
        }
        mProviders.clear();
        mContext.unregisterReceiver(mReceiver);

        sMe = null;
    }

    public static Bundle getVendorCarrierConfigForSlot(int slotId) {
        if (sMe != null) {
            ShannonVendorCarrierConfigProvider provider = sMe.mProviders.get(slotId);
            if (provider != null) {
                return provider.getBundle();
            }
        }
        return new Bundle();
    }

    public static void updateVendorCarrierConfigForSlot(int slotId, Bundle bundle) {
        if (sMe != null) {
            ShannonVendorCarrierConfigProvider provider = sMe.mProviders.get(slotId);
            if (provider != null && !bundle.isEmpty()) {
                Message message = Message.obtain(provider.getMessageHandler(), USER_CONFIG_CHANGED, new Bundle(bundle));
                message.sendToTarget();
            }
        }
    }

    public static void deleteSharedPreferencesFiles() {
        if (sMe == null) {
            return;
        }
        String preferencesFileName = PREFERENCES_FILE_PACKAGE + CARRIER_ID_PREFIX;
        File file = sMe.mContext.getSharedPreferencesPath(preferencesFileName);
        File dir = file.getParentFile();

        if (dir == null) {
            return;
        }
        File[] dirFilesList = dir.listFiles();
        if (dirFilesList == null) {
            return;
        }
        for (File preference : dirFilesList) {
            String name = preference.getName();
            if (name.startsWith(preferencesFileName)) {
                boolean result = sMe.mContext.deleteSharedPreferences(name);
                result &= preference.delete();
                if (result) {
                    sMe.log("Delete Successfully - carrier id: " + name);
                }
            }
        }
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
