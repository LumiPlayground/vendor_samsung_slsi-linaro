/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.oem;

import android.util.Log;

public class OemRilConstants {

    public static final int RILC_SOCKET_ID_1            = 0;
    public static final int RILC_SOCKET_ID_2            = 1;

    public static final int SOLICITED                   = 0;
    public static final int UNSOLICITED                 = 1;

    public static final int RILC_TRANSACTION_MAX        = 255;
    public static final int RILC_TRANSACTION_NONE       = 0xFFFFFFFF;
    public static final int RILC_REQUEST_HEADER_SIZE    = 13;
    public static final int RILC_RESPONSE_HEADER_SIZE   = 17;

    /** Error Status */
    public static final int RILC_STATUS_SUCCESS         = 0;
    public static final int RILC_STATUS_FAIL            = 1;
    public static final int RILC_STATUS_NOT_CONNECTED   = 2;
    public static final int RILC_STATUS_INVALID_PARAM   = 3;
    public static final int RILC_STATUS_IO_ERROR        = 4;
    public static final int RILC_STATUS_NO_RESOURCES    = 5;

    /** Request ID */
    public static final int RILC_REQ_SYSTEM_MODEM_DUMP          = 1;
    public static final int RILC_REQ_MISC_SET_ENG_MODE          = 2;
    public static final int RILC_REQ_MISC_SCREEN_LINE           = 3;
    public static final int RILC_REQ_MISC_DEBUG_TRACE           = 4;
    public static final int RILC_REQ_MISC_SET_CARRIER_CONFIG    = 5;
    public static final int RILC_REQ_MISC_SET_ENG_STRING_INPUT  = 6;
    public static final int RILC_REQ_MISC_GET_MSL_CODE          = 8;
    public static final int RILC_REQ_MISC_SET_PIN_CONTROL       = 9;
    public static final int RILC_REQ_AUDIO_SET_MUTE             = 10;
    public static final int RILC_REQ_AUDIO_GET_MUTE             = 11;
    public static final int RILC_REQ_AUDIO_SET_VOLUME           = 12;
    public static final int RILC_REQ_AUDIO_GET_VOLUME           = 13;
    public static final int RILC_REQ_AUDIO_SET_PATH             = 14;
    public static final int RILC_REQ_AUDIO_GET_PATH             = 15;
    public static final int RILC_REQ_AUDIO_SET_MIC              = 16;
    public static final int RILC_REQ_AUDIO_GET_MIC              = 17;
    public static final int RILC_REQ_AUDIO_SET_AUDIO_CLOCK      = 18;
    public static final int RILC_REQ_AUDIO_SET_AUDIO_LOOPBACK   = 19;
    public static final int RILC_REQ_MISC_SET_PREFERRED_CALL_CAPA = 20;
    public static final int RILC_REQ_MISC_GET_PREFERRED_CALL_CAPA = 21;
    public static final int RILC_REQ_MODEM_RESET                = 22;
    public static final int RILC_REQ_GET_MANUAL_RAT_MODE        = 23;
    public static final int RILC_REQ_SET_MANUAL_RAT_MODE        = 24;
    public static final int RILC_REQ_GET_FREQUENCY_LOCK         = 25;
    public static final int RILC_REQ_SET_FREQUENCY_LOCK         = 26;
    public static final int RILC_REQ_SET_ENDC_MODE              = 27;
    public static final int RILC_REQ_GET_ENDC_MODE              = 28;
    public static final int RILC_REQ_DETACH                     = 29;
    public static final int RILC_REQ_SET_IMS_TEST_MODE          = 38;
    public static final int RILC_REQ_SET_UICC_SUBSCRIPTION      = 40;
    public static final int RILC_REQ_AUDIO_SET_TTY_MODE         = 41;
    public static final int RILC_REQ_SET_AP_SYSTEM_TIME         = 42;
    public static final int RILC_REQ_SET_NR_MODE                = 43;
    public static final int RILC_REQ_GET_NR_MODE                = 44;
    public static final int RILC_REQ_SET_VOICE_OPERATION        = 45;
    public static final int RILC_REQ_GET_VOICE_OPERATION        = 46;
    public static final int RILC_REQ_GET_SIGNAL_STRENGTH        = 47;
    public static final int RILC_REQ_SET_IMS_REGISTRATION_STATE = 48;
    public static final int RILC_REQ_SET_NR_SILENCE_MODE        = 49;
    public static final int RILC_REQ_SET_URSP                   = 50;
    public static final int RILC_REQ_SET_EMC_LAB_TEST_MODE      = 51;
    public static final int RILC_REQ_SET_VONR_CAPA              = 57;
    public static final int RILC_REQ_GET_VONR_CAPA              = 58;
    public static final int RILC_REQ_SET_MOBILE_DATA_STATE      = 59;
    public static final int RILC_REQ_SET_PLMN_LIST              = 71;
    public static final int RILC_REQ_GET_PLMN_LIST              = 72;

    public static final int RILC_REQ_MISC_GET_MANUAL_BAND_MODE  = 90;
    public static final int RILC_REQ_MISC_SET_MANUAL_BAND_MODE  = 91;
    public static final int RILC_REQ_MISC_GET_RF_DESENSE_MODE   = 92;
    public static final int RILC_REQ_MISC_SET_RF_DESENSE_MODE   = 93;
    public static final int RILC_REQ_SCAN_RSSI                  = 96;
    public static final int RILC_REQ_FORWARDING_AT_COMMAND      = 97;
    public static final int RILC_REQ_SET_MODEM_LOG_DUMP         = 98;
    public static final int RILC_REQ_SET_ELEVATOR_SENSOR        = 99;

    public static final int RILC_REQ_PSENSOR_SET_STATUS         = 401;
    public static final int RILC_REQ_VSIM_NOTIFICATION          = 451;
    public static final int RILC_REQ_VSIM_OPERATION             = 452;

    public static final int RILC_REQ_SAR_SET_SAR_STATE          = 501;
    public static final int RILC_REQ_SAR_GET_SAR_STATE          = 502;
    public static final int RILC_REQ_SET_SELFLOG                = 600;
    public static final int RILC_REQ_GET_SELFLOG_STATUS         = 601;
    public static final int RILC_REQ_MODEM_INFO                 = 602;
    public static final int RILC_REQ_SWITCH_MODEM_FUNCTION      = 604;
    public static final int RILC_REQ_SET_PDCP_DISCARD_TIMER     = 605;
    public static final int RILC_REQ_GET_CQI_INFO               = 606;
    public static final int RILC_REQ_SET_SAR_SETTING            = 607;
    public static final int RILC_REQ_SET_GMO_SWITCH             = 608;
    public static final int RILC_REQ_SET_TCS_FCI                = 609;
    public static final int RILC_REQ_GET_TCS_FCI                = 610;
    public static final int RILC_REQ_SET_SELFLOG_PROFILE        = 611;
    public static final int RILC_REQ_SET_FORBID_LTE_CELL        = 612;
    public static final int RILC_REQ_GET_FREQUENCY_INFO         = 613;
    public static final int RILC_REQ_GET_MODEM_ACTIVITY_INFO    = 614;
    public static final int RILC_REQ_SET_SVN                    = 615;
    public static final int RILC_REQ_SET_FORBID_FREQUENCY       = 616;
    public static final int RILC_REQ_SET_FORBID_LTE_BAND        = 617;
    public static final int RILC_REQ_SET_FORBID_LTE_TAC         = 618;
    public static final int RILC_REQ_SET_SELFLOG_TCP_DUMP_MODE  = 619;
    public static final int RILC_REQ_SET_ALL_DIV_SETTING        = 620;
    public static final int RILC_REQ_SET_BSR_PARA               = 621;
    public static final int RILC_REQ_SET_DRDV                   = 622;
    public static final int RILC_REQ_SET_AHEAD_RRE              = 623;
    public static final int RILC_REQ_SET_FORBID_NR_CELL         = 624;
    public static final int RILC_REQ_GET_CDMA_CAPABILITY_INFO   = 625;
    public static final int RILC_REQ_SET_UE_OPERATION_MODE      = 630;
    public static final int RILC_REQ_GET_UE_OPERATION_MODE      = 631;
    public static final int RILC_REQ_SET_NRRRM_MIN_REQ_ENABLE   = 634;
    public static final int RILC_REQ_GET_NRRRM_MIN_REQ_ENABLE   = 635;
    public static final int RILC_REQ_SET_PWR_DETECT_THR         = 636;
    public static final int RILC_REQ_SET_VCRT                   = 637;
    public static final int RILC_REQ_SET_ENABLE_N28             = 638;
    public static final int RILC_REQ_SET_UL_MIMO                = 639;
    public static final int RILC_REQ_GET_BINARY_VERSION_INFO    = 640;
    public static final int RILC_REQ_GET_VCRT                   = 642;
    public static final int RILC_REQ_SET_NR_DISABLE_BAND        = 643;

    public static final int RILC_REQ_CHANGE_BARRING_PASSWORD_OVER_MMI  = 700;

    public static final int RILC_REQ_GET_RADIO_NODE                    = 800;
    public static final int RILC_REQ_SET_RADIO_NODE                    = 801;
    public static final int RILC_REQ_GET_PROVISION_UPDATE_REQUEST      = 802;
    public static final int RILC_REQ_SET_PROVISION_UPDATE_DONE_REQUEST = 803;
    public static final int RILC_REQ_RADIO_CONFIG_RESET                = 804;
    public static final int RILC_REQ_VERIFY_MSL                        = 805;
    public static final int RILC_REQ_GET_PLMN_NAME_FROM_SE13TABLE      = 806;
    public static final int RILC_REQ_TS25TABLE_DUMP                    = 807;
    public static final int RILC_REQ_SET_CA_BANDWIDTH_FILTER           = 808;
    public static final int RILC_REQ_ICC_DEPERSONALIZATION             = 809;
    public static final int RILC_REQ_CANCEL_GET_AVAILABLE_NETWORK      = 810;
    public static final int RILC_REQ_MISC_SEND_TEST_JSON_DATA          = 811;

    public static final int RILC_REQ_MISC_SET_TELEPHONY_STATUS_LOGGING = 989;

    /** Unsolicited Response ID */
    public static final int RILC_UNSOL_SOCKET_ERROR             = 1000;
    public static final int RILC_UNSOL_RADIO_STATE_CHANGED      = 1002;
    public static final int RILC_UNSOL_MODEM_STATE_CHANGED      = 1004;
    public static final int RILC_UNSOL_DISPLAY_ENG_MODE         = 2002;
    public static final int RILC_UNSOL_PIN_CONTROL              = 2004;
    public static final int RILC_UNSOL_AM                       = 2005;
    public static final int RILC_UNSOL_SCAN_RSSI_RESULT         = 2006;
    public static final int RILC_UNSOL_FORWARDING_AT_COMMAND    = 2007;
    public static final int RILC_UNSOL_LTE_RRC_STATE_CHANGED    = 2010;
    public static final int RILC_UNSOL_TOTAL_OOS                = 2011;
    public static final int RILC_UNSOL_DEVICE_RESET             = 2012;
    public static final int RILC_UNSOL_IMS_SRVCC_HO             = 3003;
    public static final int RILC_UNSOL_AIMS_SIP_MSG_INFO        = 3050;
    public static final int RILC_UNSOL_SELFLOG_STATUS           = 5100;
    public static final int RILC_UNSOL_MODEM_INFO               = 5101;
    public static final int RILC_UNSOL_FREQUENCY_INFO           = 5103;
    public static final int RILC_UNSOL_AMBR_REPORT              = 5104;
    public static final int RILC_UNSOL_B2_B1_CONFIG_INFO        = 5105;
    public static final int RILC_UNSOL_BASESTATION_INFO         = 5106;
    public static final int RILC_UNSOL_DROPPED_MT_CALL_INFO     = 5107;
    public static final int RILC_UNSOL_CA_BANDWIDTH_FILTER      = 6002;
    public static final int RILC_UNSOL_VSIM_OPERATION           = 6501;
    public static final int RILC_UNSOL_SAR_RF_CONNECTION        = 7001;

    public static String requestToString(int request) {
        switch (request) {
        case RILC_REQ_SYSTEM_MODEM_DUMP:
            return "RILC_REQ_SYSTEM_MODEM_DUMP";
        case RILC_REQ_MISC_SET_ENG_MODE:
            return "RILC_REQ_MISC_SET_ENG_MODE";
        case RILC_REQ_MISC_SCREEN_LINE:
            return "RILC_REQ_MISC_SCREEN_LINE";
        case RILC_REQ_MISC_DEBUG_TRACE:
            return "RILC_REQ_MISC_DEBUG_TRACE";
        case RILC_REQ_MISC_SET_CARRIER_CONFIG:
            return "RILC_REQ_MISC_SET_CARRIER_CONFIG";
        case RILC_REQ_MISC_SET_ENG_STRING_INPUT:
            return "RILC_REQ_MISC_SET_ENG_STRING_INPUT";
        case RILC_REQ_MISC_GET_MSL_CODE:
            return "RILC_REQ_MISC_GET_MSL_CODE";
        case RILC_REQ_MISC_SET_PIN_CONTROL:
            return "RILC_REQ_MISC_SET_PIN_CONTROL";
        case RILC_REQ_AUDIO_SET_MUTE:
            return "RILC_REQ_AUDIO_SET_MUTE";
        case RILC_REQ_AUDIO_GET_MUTE:
            return "RILC_REQ_AUDIO_GET_MUTE";
        case RILC_REQ_AUDIO_SET_VOLUME:
            return "RILC_REQ_AUDIO_SET_VOLUME";
        case RILC_REQ_AUDIO_GET_VOLUME:
            return "RILC_REQ_AUDIO_GET_VOLUME";
        case RILC_REQ_AUDIO_SET_PATH:
            return "RILC_REQ_AUDIO_SET_PATH";
        case RILC_REQ_AUDIO_GET_PATH:
            return "RILC_REQ_AUDIO_GET_PATH";
        case RILC_REQ_AUDIO_SET_MIC:
            return "RILC_REQ_AUDIO_SET_MIC";
        case RILC_REQ_AUDIO_GET_MIC:
            return "RILC_REQ_AUDIO_GET_MIC";
        case RILC_REQ_AUDIO_SET_AUDIO_CLOCK:
            return "RILC_REQ_AUDIO_SET_AUDIO_CLOCK";
        case RILC_REQ_AUDIO_SET_AUDIO_LOOPBACK:
            return "RILC_REQ_AUDIO_SET_AUDIO_LOOPBACK";
        case RILC_REQ_MISC_SET_PREFERRED_CALL_CAPA:
            return "RILC_REQ_MISC_SET_PREFERRED_CALL_CAPA";
        case RILC_REQ_MISC_GET_PREFERRED_CALL_CAPA:
            return "RILC_REQ_MISC_GET_PREFERRED_CALL_CAPA";
        case RILC_REQ_SET_VOICE_OPERATION:
            return "RILC_REQ_SET_VOICE_OPERATION";
        case RILC_REQ_GET_VOICE_OPERATION:
            return "RILC_REQ_GET_VOICE_OPERATION";
        case RILC_REQ_MISC_GET_MANUAL_BAND_MODE:
            return "RILC_REQ_MISC_GET_MANUAL_BAND_MODE";
        case RILC_REQ_MISC_SET_MANUAL_BAND_MODE:
            return "RILC_REQ_MISC_SET_MANUAL_BAND_MODE";
        case RILC_REQ_MISC_GET_RF_DESENSE_MODE:
            return "RILC_REQ_MISC_GET_RF_DESENSE_MODE";
        case RILC_REQ_MISC_SET_RF_DESENSE_MODE:
            return "RILC_REQ_MISC_SET_RF_DESENSE_MODE";
        case RILC_REQ_SCAN_RSSI:
            return "RILC_REQ_SCAN_RSSI";
        case RILC_REQ_FORWARDING_AT_COMMAND:
            return "RILC_REQ_FORWARDING_AT_COMMAND";
        case RILC_REQ_PSENSOR_SET_STATUS:
            return "RILC_REQ_PSENSOR_SET_STATUS";
        case RILC_REQ_VSIM_NOTIFICATION:
            return "RILC_REQ_VSIM_NOTIFICATION";
        case RILC_REQ_VSIM_OPERATION:
            return "RILC_REQ_VSIM_OPERATION";
        case RILC_REQ_SAR_SET_SAR_STATE:
            return "RILC_REQ_SAR_SET_SAR_STATE";
        case RILC_REQ_SAR_GET_SAR_STATE:
            return "RILC_REQ_SAR_GET_SAR_STATE";
        case RILC_REQ_GET_RADIO_NODE:
            return "RILC_REQ_GET_RADIO_NODE";
        case RILC_REQ_SET_RADIO_NODE:
            return "RILC_REQ_SET_RADIO_NODE";
        case RILC_REQ_GET_PROVISION_UPDATE_REQUEST:
            return "RILC_REQ_GET_PROVISION_UPDATE_REQUEST";
        case RILC_REQ_SET_PROVISION_UPDATE_DONE_REQUEST:
            return "RILC_REQ_SET_PROVISION_UPDATE_DONE_REQUEST";
        case RILC_REQ_RADIO_CONFIG_RESET:
            return "RILC_REQ_RADIO_CONFIG_RESET";
        case RILC_REQ_VERIFY_MSL:
            return "RILC_REQ_VERIFY_MSL";
        case RILC_REQ_GET_PLMN_NAME_FROM_SE13TABLE:
            return "RILC_REQ_GET_PLMN_NAME_FROM_SE13TABLE";
        case RILC_REQ_TS25TABLE_DUMP:
            return "RILC_REQ_TS25_TABLE_DUMP";
        case RILC_UNSOL_MODEM_STATE_CHANGED:
            return "RILC_UNSOL_MODEM_STATE_CHANGED";
        case RILC_UNSOL_DISPLAY_ENG_MODE:
            return "RILC_UNSOL_DISPLAY_ENG_MODE";
        case RILC_UNSOL_AM:
            return "RILC_UNSOL_AM";
        case RILC_UNSOL_DEVICE_RESET:
            return "RILC_UNSOL_DEVICE_RESET";
        case RILC_UNSOL_SCAN_RSSI_RESULT:
            return "RILC_UNSOL_SCAN_RSSI_RESULT";
        case RILC_UNSOL_FORWARDING_AT_COMMAND:
            return "RILC_UNSOL_FORWARDING_AT_COMMAND";
        case RILC_UNSOL_VSIM_OPERATION:
            return "RILC_UNSOL_VSIM_OPERATION";
        case RILC_UNSOL_SAR_RF_CONNECTION:
            return "RILC_UNSOL_SAR_RF_CONNECTION";
        case RILC_REQ_MODEM_INFO:
            return "RILC_REQ_MODEM_INFO";
        case RILC_REQ_MODEM_RESET:
            return "RILC_REQ_MODEM_RESET";
        case RILC_REQ_GET_MANUAL_RAT_MODE:
            return "RILC_REQ_GET_MANUAL_RAT_MODE";
        case RILC_REQ_SET_MANUAL_RAT_MODE:
            return "RILC_REQ_SET_MANUAL_RAT_MODE";
        case RILC_REQ_GET_FREQUENCY_LOCK:
            return "RILC_REQ_GET_FREQUENCY_LOCK";
        case RILC_REQ_SET_FREQUENCY_LOCK:
            return "RILC_REQ_SET_FREQUENCY_LOCK";
        case RILC_REQ_SET_SELFLOG:
            return "RILC_REQ_SET_SELFLOG";
        case RILC_REQ_GET_SELFLOG_STATUS:
            return "RILC_REQ_GET_SELFLOG_STATUS";
        case RILC_UNSOL_MODEM_INFO:
            return "RILC_UNSOL_MODEM_INFO";
        case RILC_REQ_SWITCH_MODEM_FUNCTION:
            return "RILC_REQ_SWITCH_MODEM_FUNCTION";
        case RILC_REQ_SET_PDCP_DISCARD_TIMER:
            return "RILC_REQ_SET_PDCP_DISCARD_TIMER";
        case RILC_REQ_SET_ENDC_MODE:
            return "RILC_REQ_SET_ENDC_MODE";
        case RILC_REQ_GET_ENDC_MODE:
            return "RILC_REQ_GET_ENDC_MODE";
        case RILC_UNSOL_SELFLOG_STATUS:
            return "RILC_UNSOL_SELFLOG_STATUS";
        case RILC_REQ_SET_CA_BANDWIDTH_FILTER:
            return "RILC_REQ_SET_CA_BANDWIDTH_FILTER";
        case RILC_REQ_ICC_DEPERSONALIZATION:
            return "RILC_REQ_ICC_DEPERSONALIZATION";
        case RILC_UNSOL_CA_BANDWIDTH_FILTER:
            return "RILC_UNSOL_CA_BANDWIDTH_FILTER";
        case RILC_REQ_CANCEL_GET_AVAILABLE_NETWORK:
            return "RILC_REQ_CANCEL_GET_AVAILABLE_NETWORK";
        case RILC_REQ_MISC_SEND_TEST_JSON_DATA:
            return "RILC_REQ_MISC_SEND_TEST_JSON_DATA";
        case RILC_REQ_SET_MODEM_LOG_DUMP:
            return"RILC_REQ_SET_MODEM_LOG_DUMP";
        case RILC_REQ_SET_UICC_SUBSCRIPTION:
            return "RILC_REQ_SET_UICC_SUBSCRIPTION";
        case RILC_REQ_SET_SELFLOG_PROFILE:
            return "RILC_REQ_SET_SELFLOG_PROFILE";
        case RILC_UNSOL_IMS_SRVCC_HO:
            return "RILC_UNSOL_IMS_SRVCC_HO";
        case RILC_UNSOL_AIMS_SIP_MSG_INFO:
            return "RILC_UNSOL_AIMS_SIP_MSG_INFO";
        case RILC_UNSOL_AMBR_REPORT:
            return "RILC_UNSOL_AMBR_REPORT";
        case RILC_REQ_SET_FORBID_LTE_CELL:
            return "RILC_REQ_SET_FORBID_LTE_CELL";
        case RILC_REQ_GET_FREQUENCY_INFO:
            return "RILC_REQ_GET_FREQUENCY_INFO";
        case RILC_UNSOL_B2_B1_CONFIG_INFO:
            return "RILC_UNSOL_B2_B1_CONFIG_INFO";
        case RILC_REQ_SET_ELEVATOR_SENSOR:
            return "RILC_REQ_SET_ELEVATOR_SENSOR";
        case RILC_REQ_GET_CQI_INFO:
            return "RILC_REQ_GET_CQI_INFO";
        case RILC_UNSOL_FREQUENCY_INFO:
            return "RILC_UNSOL_FREQUENCY_INFO";
        case RILC_REQ_DETACH:
            return "RILC_REQ_DETACH";
        case RILC_REQ_SET_SAR_SETTING:
            return "RILC_REQ_SET_SAR_SETTING";
        case RILC_REQ_SET_IMS_TEST_MODE:
            return "RILC_REQ_SET_IMS_TEST_MODE";
        case RILC_REQ_SET_GMO_SWITCH:
            return "RILC_REQ_SET_GMO_SWITCH";
        case RILC_REQ_SET_TCS_FCI:
            return "RILC_REQ_SET_TCS_FCI";
        case RILC_REQ_GET_TCS_FCI:
            return "RILC_REQ_GET_TCS_FCI";
        case RILC_UNSOL_RADIO_STATE_CHANGED:
            return "RILC_UNSOL_RADIO_STATE_CHANGED";
        case RILC_UNSOL_LTE_RRC_STATE_CHANGED:
            return "RILC_UNSOL_LTE_RRC_STATE_CHANGED";
        case RILC_UNSOL_TOTAL_OOS:
            return "RILC_UNSOL_TOTAL_OOS";
        case RILC_UNSOL_BASESTATION_INFO:
            return "RILC_UNSOL_BASESTATION_INFO";
        case RILC_REQ_GET_MODEM_ACTIVITY_INFO:
            return "RILC_REQ_GET_MODEM_ACTIVITY_INFO";
        case RILC_REQ_AUDIO_SET_TTY_MODE:
            return "RILC_REQ_AUDIO_SET_TTY_MODE";
        case RILC_REQ_SET_AP_SYSTEM_TIME:
            return "RILC_REQ_SET_AP_SYSTEM_TIME";
        case RILC_REQ_SET_NR_MODE:
            return "RILC_REQ_SET_NR_MODE";
        case RILC_REQ_GET_NR_MODE:
            return "RILC_REQ_GET_NR_MODE";
        case RILC_REQ_SET_SVN:
            return "RILC_REQ_SET_SVN";
        case RILC_REQ_SET_FORBID_FREQUENCY:
            return "RILC_REQ_SET_FORBID_FREQUENCY";
        case RILC_REQ_SET_FORBID_LTE_BAND:
            return "RILC_REQ_SET_FORBID_LTE_BAND";
        case RILC_REQ_SET_FORBID_LTE_TAC:
            return "RILC_REQ_SET_FORBID_LTE_TAC";
        case RILC_UNSOL_DROPPED_MT_CALL_INFO:
            return "RILC_UNSOL_DROPPED_MT_CALL_INFO";
        case RILC_REQ_SET_SELFLOG_TCP_DUMP_MODE:
            return "RILC_REQ_SET_SELFLOG_TCP_DUMP_MODE";
        case RILC_REQ_SET_ALL_DIV_SETTING:
            return "RILC_REQ_SET_ALL_DIV_SETTING";
        case RILC_REQ_SET_BSR_PARA:
            return "RILC_REQ_SET_BSR_PARA";
        case RILC_REQ_SET_AHEAD_RRE:
            return "RILC_REQ_SET_AHEAD_RRE";
        case RILC_REQ_SET_DRDV:
            return "RILC_REQ_SET_DRDV";
        case RILC_REQ_SET_FORBID_NR_CELL:
            return "RILC_REQ_SET_FORBID_NR_CELL";
        case RILC_REQ_GET_CDMA_CAPABILITY_INFO:
            return "RILC_REQ_GET_CDMA_CAPABILITY_INFO";
        case RILC_REQ_SET_UE_OPERATION_MODE:
            return "RILC_REQ_SET_UE_OPERATION_MODE";
        case RILC_REQ_GET_UE_OPERATION_MODE:
            return "RILC_REQ_GET_UE_OPERATION_MODE";
        case RILC_REQ_SET_NRRRM_MIN_REQ_ENABLE:
            return "RILC_REQ_SET_NRRRM_MIN_REQ_ENABLE";
        case RILC_REQ_GET_NRRRM_MIN_REQ_ENABLE:
            return "RILC_REQ_GET_NRRRM_MIN_REQ_ENABLE";
        case RILC_REQ_SET_PWR_DETECT_THR:
            return "RILC_REQ_SET_PWR_DETECT_THR";
        case RILC_REQ_SET_VCRT:
            return "RILC_REQ_SET_VCRT";
        case RILC_REQ_SET_ENABLE_N28:
            return "RILC_REQ_SET_ENABLE_N28";
        case RILC_REQ_SET_UL_MIMO:
            return "RILC_REQ_SET_UL_MIMO";
        case RILC_REQ_GET_BINARY_VERSION_INFO:
            return "RILC_REQ_GET_BINARY_VERSION_INFO";
        case RILC_REQ_GET_VCRT:
            return "RILC_REQ_GET_VCRT";
        case RILC_REQ_SET_NR_DISABLE_BAND:
            return "RILC_REQ_SET_NR_DISABLE_BAND";

        case RILC_REQ_SET_URSP:
            return "RILC_REQ_SET_URSP";
        case RILC_REQ_SET_EMC_LAB_TEST_MODE:
            return "RILC_REQ_SET_EMC_LAB_TEST_MODE";
        case RILC_REQ_SET_VONR_CAPA:
            return "RILC_REQ_SET_VONR_CAPA";
        case RILC_REQ_GET_VONR_CAPA:
            return "RILC_REQ_GET_VONR_CAPA";
        case RILC_REQ_SET_MOBILE_DATA_STATE:
            return "RILC_REQ_SET_MOBILE_DATA_STATE";
        case RILC_REQ_SET_PLMN_LIST:
            return "RILC_REQ_SET_PLMN_LIST";
        case RILC_REQ_GET_PLMN_LIST:
            return "RILC_REQ_GET_PLMN_LIST";
        case RILC_REQ_CHANGE_BARRING_PASSWORD_OVER_MMI:
            return "RILC_REQ_CHANGE_BARRING_PASSWORD_OVER_MMI";
        case RILC_REQ_MISC_SET_TELEPHONY_STATUS_LOGGING:
            return "RILC_REQ_MISC_SET_TELEPHONY_STATUS_LOGGING";
        }
        return ("unsupported request. id="+request);
    }

    static Exception fromRilErrno(int errno) {
        switch (errno) {
        case RILC_STATUS_SUCCESS:  return null;
        case RILC_STATUS_FAIL:
            return new Exception("RILC_STATUS_FAIL");
        case RILC_STATUS_NOT_CONNECTED:
            return new Exception("RILC_STATUS_NOT_CONNECTED");
        case RILC_STATUS_INVALID_PARAM:
            return new Exception("RILC_STATUS_INVALID_PARAM");
        case RILC_STATUS_IO_ERROR:
            return new Exception("RILC_STATUS_IO_ERROR");
        case RILC_STATUS_NO_RESOURCES:
            return new Exception("RILC_STATUS_NO_RESOURCES");
        default:
            Log.e("OemRil", "Unrecognized errno " + errno);
            return new Exception("INVALID_RESPONSE");
        }
    }
}
