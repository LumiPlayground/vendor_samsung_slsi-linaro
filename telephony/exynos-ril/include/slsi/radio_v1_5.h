/* copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __RADIO_V1_5_H__
#define __RADIO_V1_5_H__

#include <slsi/radio_v1_4.h>
#include <vector>

#define MAX_CSG_HOME_NODEB_NAME 100
#define MAX_REG_PLMN_LEN 7    // 6 + 1 for null end

/**
 * Defining signal strength type.
 */
typedef enum {
    /**
     * Received Signal Strength Indication.
     * Range: -113 dBm and -51 dBm
     * Used RAN: GERAN, CDMA2000
     * Reference: 3GPP TS 27.007 section 8.5.
     */
    RSSI = 1,
    /**
     * Received Signal Code Power.
     * Range: -120 dBm to -25 dBm;
     * Used RAN: UTRAN
     * Reference: 3GPP TS 25.123, section 9.1.1.1
     */
    RSCP = 2,
    /**
     * Reference Signal Received Power.
     * Range: -140 dBm to -44 dBm;
     * Used RAN: EUTRAN
     * Reference: 3GPP TS 36.133 9.1.4
     */
    RSRP = 3,
    /**
     * Reference Signal Received Quality
     * Range: -34 dB to 3 dB;
     * Used RAN: EUTRAN
     * Reference: 3GPP TS 36.133 v12.6.0 section 9.1.7
     */
    RSRQ = 4,
    /**
     * Reference Signal Signal to Noise Ratio
     * Range: -20 dB to 30 dB;
     * Used RAN: EUTRAN
     * Note: this field is optional; how to support it can be decided by the
     * corresponding vendor. Though the response code is not enforced,
     * vendor's implementation must ensure this interface not crashing.
     */
    RSSNR = 5,
    /**
     * 5G SS reference signal received power.
     * Range: -140 dBm to -44 dBm.
     * Used RAN: NGRAN
     * Reference: 3GPP TS 38.215.
     */
    SSRSRP = 6,
    /**
     * 5G SS reference signal received quality.
     * Range: -20 dB to -3 dB.
     * Used RAN: NGRAN
     * Reference: 3GPP TS 38.215.
     */
    SSRSRQ = 7,
    /**
     * 5G SS signal-to-noise and interference ratio.
     * Range: -23 dB to 40 dB
     * Used RAN: NGRAN
     * Reference: 3GPP TS 38.215 section 5.1.*, 3GPP TS 38.133 section 10.1.16.1.
     */
    SSSINR = 8,
} RIL_SignalMeasurementType;

/**
 * Contains the threshold values of each signal measurement type.
 */
typedef struct {
    /** Signal Measurement Type */
    RIL_SignalMeasurementType signalMeasurement;

    /** A hysteresis time in milliseconds to prevent flapping. A value of 0 disables hysteresis */
    int32_t hysteresisMs;

    /**
     * An interval in dB defining the required magnitude change between reports.
     * hysteresisDb must be smaller than the smallest threshold delta.
     * An interval value of 0 disables hysteresis.
     */
    int32_t hysteresisDb;

    /**
     * List of threshold values.
     * Range and unit must reference specific @1.5::SignalMeasurementType.
     * The threshold values for which to apply criteria.
     * A vector size of 0 disables the use of thresholds for reporting.
     */
    uint32_t len_thresholds;
    int *thresholds;

    /**
     * Indicates whether the reporting criteria of the corresponding measurement is enabled
     * (isEnabled==true) or disabled (isEnabled==false).
     *
     * If enabled, modem must trigger the report based on the criteria.
     * If disabled, modem must not trigger the report based on the criteria.
     */
    bool isEnabled;
} RIL_SignalThresholdInfo;

typedef enum {
    /**
     *  Next-Generation Radio Access Network (NGRAN)
     */
    ACCESS_NETWORK_NGRAN = 6,
} RIL_AccessNetwork_V1_5;

typedef enum {
    /** Next Generation Radio Access Network */
    RIL_RAN_NGRAN = 4,
    /** CDMA 2000 Network */
    RIL_RAN_CDMA2000 = 5,
} RIL_RadioAccessNetworks_V1_5;

typedef enum {
    /** 3GPP TS 38.101-1, Table 5.2-1: FR1 bands */
    NGRAN_BAND_1 = 1,
    NGRAN_BAND_2 = 2,
    NGRAN_BAND_3 = 3,
    NGRAN_BAND_5 = 5,
    NGRAN_BAND_7 = 7,
    NGRAN_BAND_8 = 8,
    NGRAN_BAND_12 = 12,
    NGRAN_BAND_14 = 14,
    NGRAN_BAND_18 = 18,
    NGRAN_BAND_20 = 20,
    NGRAN_BAND_25 = 25,
    NGRAN_BAND_28 = 28,
    NGRAN_BAND_29 = 29,
    NGRAN_BAND_30 = 30,
    NGRAN_BAND_34 = 34,
    NGRAN_BAND_38 = 38,
    NGRAN_BAND_39 = 39,
    NGRAN_BAND_40 = 40,
    NGRAN_BAND_41 = 41,
    NGRAN_BAND_48 = 48,
    NGRAN_BAND_50 = 50,
    NGRAN_BAND_51 = 51,
    NGRAN_BAND_65 = 65,
    NGRAN_BAND_66 = 66,
    NGRAN_BAND_70 = 70,
    NGRAN_BAND_71 = 71,
    NGRAN_BAND_74 = 74,
    NGRAN_BAND_75 = 75,
    NGRAN_BAND_76 = 76,
    NGRAN_BAND_77 = 77,
    NGRAN_BAND_78 = 78,
    NGRAN_BAND_79 = 79,
    NGRAN_BAND_80 = 80,
    NGRAN_BAND_81 = 81,
    NGRAN_BAND_82 = 82,
    NGRAN_BAND_83 = 83,
    NGRAN_BAND_84 = 84,
    NGRAN_BAND_86 = 86,
    NGRAN_BAND_90 = 90,
    /** 3GPP TS 38.101-2, Table 5.2-1: FR2 bands */
    NGRAN_BAND_257 = 257,
    NGRAN_BAND_258 = 258,
    NGRAN_BAND_260 = 260,
    NGRAN_BAND_261 = 261,
} RIL_NgranBands;

typedef enum {
    /** TD-SCDMA bands. 3GPP TS 25.102, Table 5.2: Frequency bands */
    UTRAN_BAND_A = 101,
    UTRAN_BAND_B = 102,
    UTRAN_BAND_C = 103,
    UTRAN_BAND_D = 104,
    UTRAN_BAND_E = 105,
    UTRAN_BAND_F = 106,
} RIL_UtranBands_V1_5;

/**
 * Overwritten from @1.1::RadioAccessSpecifier to add NGRAN and NgranBands
 */
typedef struct {
    RIL_RadioAccessNetworks radio_access_network; // The type of network to scan.
    uint32_t bands_length;                        // Length of bands
    union {
        RIL_GeranBands geran_bands[MAX_BANDS];
        RIL_UtranBands utran_bands[MAX_BANDS];
        RIL_EutranBands eutran_bands[MAX_BANDS];
        RIL_NgranBands ngran_bands[MAX_BANDS];
    } bands;
    uint32_t channels_length;                     // Length of channels
    uint32_t channels[MAX_CHANNELS];              // Frequency channels to scan
} RIL_RadioAccessSpecifier_V1_5;

/**
 * Overwritten from @1.2::NetworkScanRequest to update
 * RadioAccessSpecifier to 1.5 version
 */
typedef struct {
    RIL_ScanType type;                        // Type of the scan
    int32_t interval;                         // Time interval in seconds
                                              // between periodic scans, only
                                              // valid when type=RIL_PERIODIC
    uint32_t specifiers_length;               // Length of specifiers
    RIL_RadioAccessSpecifier_V1_5 specifiers[MAX_RADIO_ACCESS_NETWORKS]; // Radio access networks
                                                                    // with EUTRAN_BANDs/channels.
    int32_t maxSearchTime;                    // Maximum duration of the periodic search (in seconds).
                                              // Expected range for the input is [MaxSearchTimeRange:MIN - MaxSearchTimeRange:MAX]
                                              //If the search lasts maxSearchTime, it must be terminated.
    bool incrementalResults;                  // Indicates whether the modem must report incremental results of the network scan to the client.
                                              // 0 Incremental results must not be reported.
                                              // 1  Incremental must be reported.
    int32_t incrementalResultsPeriodicity;    // Indicates the periodicity with which the modem must report incremental results to
                                              // the client (in seconds).
                                              // Expected range for the input is
                                              // [RIL_IncrementalResultsPeriodicityRange:MIN - RIL_IncrementalResultsPeriodicityRange:MAX]
                                              // This value must be less than or equal to maxSearchTime.
    int32_t numOfMccMncs;                     // A size of vector of mccMncs.
    char **mccMncs;                           // Describes the List of PLMN ids (MCC-MNC)
                                              // If any PLMN of this list is found, search must end at that point and results with all
                                              // PLMN found until that point should be sent as response.
                                              // If the list is not sent, search to be completed until end and all PLMNs found to be reported.
} RIL_NetworkScanRequest_V1_5;

typedef enum {
    /**
     * APN type for XCAP
     * NOTE: Due to the addition of this new value, the value ALL defined in
     * 1.0::ApnTypes is deprecated and should not be used.
     */
    XCAP = 1 << 11,
} RIL_ApnTypes_V1_5;

/**
 * The properties of the link address. This enum reflects the definition in
 * if_addr.h in Linux kernel.
 */
typedef enum {
    ADDRESSPROPERTY_NONE = 0,

    /** Indicates this address is deprecated */
    ADDRESSPROPERTY_DEPRECATED = 0x20,
} RIL_AddressProperty;

typedef struct {
    /**
     * The format is IP address with optional "/"
     * prefix length (The format is defined in RFC-4291 section 2.3). For example, "192.0.1.3",
     * "192.0.1.11/16", or "2001:db8::1/64". Typically one IPv4 or one IPv6 or one of each. If
     * the prefix length is absent, then the addresses are assumed to be point to point with
     * IPv4 with prefix length 32 or IPv6 with prefix length 128.
     */
    char *address;

    /**
     * The properties of the link address
     */
    int32_t properties;

    /**
     * The UTC time that this link address will be deprecated. 0 indicates this information is not
     * available.
     */
    uint64_t deprecationTime;

    /**
     * The UTC time that this link address will expire and no longer valid. 0 indicates this
     * information is not available.
     */
    uint64_t expirationTime;
} RIL_LinkAddress;

typedef enum {
    /**
     * For more information, refer to below
     *  hardware/interfaces/radio/1.4/types.hal
     *  enum DataCallFailCause
     */
    LLC_SNDCP = 0x19,
    ACTIVATION_REJECTED_BCM_VIOLATION = 0x30,
    COLLISION_WITH_NETWORK_INITIATED_REQUEST = 0x38,
    ONLY_IPV4V6_ALLOWED = 0x39,
    ONLY_NON_IP_ALLOWED = 0x3A,
    UNSUPPORTED_QCI_VALUE = 0x3B,
    BEARER_HANDLING_NOT_SUPPORTED = 0x3C,
    INVALID_DNS_ADDR = 0x7B,
    INVALID_PCSCF_OR_DNS_ADDRESS = 0x7C,
    CALL_PREEMPT_BY_EMERGENCY_APN = 0x7F,
    UE_INITIATED_DETACH_OR_DISCONNECT = 0x80,
    MIP_FA_REASON_UNSPECIFIED = 0x7D0,
    MIP_FA_ADMIN_PROHIBITED = 0x7D1,
    MIP_FA_INSUFFICIENT_RESOURCES = 0x7D2,
    MIP_FA_MOBILE_NODE_AUTHENTICATION_FAILURE = 0x7D3,
    MIP_FA_HOME_AGENT_AUTHENTICATION_FAILURE = 0x7D4,
    MIP_FA_REQUESTED_LIFETIME_TOO_LONG = 0x7D5,
    MIP_FA_MALFORMED_REQUEST = 0x7D6,
    MIP_FA_MALFORMED_REPLY = 0x7D7,
    MIP_FA_ENCAPSULATION_UNAVAILABLE = 0x7D8,
    MIP_FA_VJ_HEADER_COMPRESSION_UNAVAILABLE = 0x7D9,
    MIP_FA_REVERSE_TUNNEL_UNAVAILABLE = 0x7DA,
    MIP_FA_REVERSE_TUNNEL_IS_MANDATORY = 0x7DB,
    MIP_FA_DELIVERY_STYLE_NOT_SUPPORTED = 0x7DC,
    MIP_FA_MISSING_NAI = 0x7DD,
    MIP_FA_MISSING_HOME_AGENT = 0x7DE,
    MIP_FA_MISSING_HOME_ADDRESS = 0x7DF,
    MIP_FA_UNKNOWN_CHALLENGE = 0x7E0,
    MIP_FA_MISSING_CHALLENGE = 0x7E1,
    MIP_FA_STALE_CHALLENGE = 0x7E2,
    MIP_HA_REASON_UNSPECIFIED = 0x7E3,
    MIP_HA_ADMIN_PROHIBITED = 0x7E4,
    MIP_HA_INSUFFICIENT_RESOURCES = 0x7E5,
    MIP_HA_MOBILE_NODE_AUTHENTICATION_FAILURE = 0x7E6,
    MIP_HA_FOREIGN_AGENT_AUTHENTICATION_FAILURE = 0x7E7,
    MIP_HA_REGISTRATION_ID_MISMATCH = 0x7E8,
    MIP_HA_MALFORMED_REQUEST = 0x7E9,
    MIP_HA_UNKNOWN_HOME_AGENT_ADDRESS = 0x7EA,
    MIP_HA_REVERSE_TUNNEL_UNAVAILABLE = 0x7EB,
    MIP_HA_REVERSE_TUNNEL_IS_MANDATORY = 0x7EC,
    MIP_HA_ENCAPSULATION_UNAVAILABLE = 0x7ED,
    CLOSE_IN_PROGRESS = 0x7EE,
    NETWORK_INITIATED_TERMINATION = 0x7EF,
    MODEM_APP_PREEMPTED = 0x7F0,
    PDN_IPV4_CALL_DISALLOWED = 0x7F1,
    PDN_IPV4_CALL_THROTTLED = 0x7F2,
    PDN_IPV6_CALL_DISALLOWED = 0x7F3,
    PDN_IPV6_CALL_THROTTLED = 0x7F4,
    MODEM_RESTART = 0x7F5,
    PDP_PPP_NOT_SUPPORTED = 0x7F6,
    UNPREFERRED_RAT = 0x7F7,
    PHYSICAL_LINK_CLOSE_IN_PROGRESS = 0x7F8,
    APN_PENDING_HANDOVER = 0x7F9,
    PROFILE_BEARER_INCOMPATIBLE = 0x7FA,
    SIM_CARD_CHANGED = 0x7FB,
    LOW_POWER_MODE_OR_POWERING_DOWN = 0x7FC,
    APN_DISABLED = 0x7FD,
    MAX_PPP_INACTIVITY_TIMER_EXPIRED = 0x7FE,
    IPV6_ADDRESS_TRANSFER_FAILED = 0x7FF,
    TRAT_SWAP_FAILED = 0x800,
    EHRPD_TO_HRPD_FALLBACK = 0x801,
    MIP_CONFIG_FAILURE = 0x802,
    PDN_INACTIVITY_TIMER_EXPIRED = 0x803,
    MAX_IPV4_CONNECTIONS = 0x804,
    MAX_IPV6_CONNECTIONS = 0x805,
    APN_MISMATCH = 0x806,
    IP_VERSION_MISMATCH = 0x807,
    DUN_CALL_DISALLOWED = 0x808,
    INTERNAL_EPC_NONEPC_TRANSITION = 0x809,
    INTERFACE_IN_USE = 0x80A,
    APN_DISALLOWED_ON_ROAMING = 0x80B,
    APN_PARAMETERS_CHANGED = 0x80C,
    NULL_APN_DISALLOWED = 0x80D,
    THERMAL_MITIGATION = 0x80E,
    DATA_SETTINGS_DISABLED = 0x80F,
    DATA_ROAMING_SETTINGS_DISABLED = 0x810,
    DDS_SWITCHED = 0x811,
    FORBIDDEN_APN_NAME = 0x812,
    DDS_SWITCH_IN_PROGRESS = 0x813,
    CALL_DISALLOWED_IN_ROAMING = 0x814,
    NON_IP_NOT_SUPPORTED = 0x815,
    PDN_NON_IP_CALL_THROTTLED = 0x816,
    PDN_NON_IP_CALL_DISALLOWED = 0x817,
    CDMA_LOCK = 0x818,
    CDMA_INTERCEPT = 0x819,
    CDMA_REORDER = 0x81A,
    CDMA_RELEASE_DUE_TO_SO_REJECTION = 0x81B,
    CDMA_INCOMING_CALL = 0x81C,
    CDMA_ALERT_STOP = 0x81D,
    CHANNEL_ACQUISITION_FAILURE = 0x81E,
    MAX_ACCESS_PROBE = 0x81F,
    CONCURRENT_SERVICE_NOT_SUPPORTED_BY_BASE_STATION = 0x820,
    NO_RESPONSE_FROM_BASE_STATION = 0x821,
    REJECTED_BY_BASE_STATION = 0x822,
    CONCURRENT_SERVICES_INCOMPATIBLE = 0x823,
    NO_CDMA_SERVICE = 0x824,
    RUIM_NOT_PRESENT = 0x825,
    CDMA_RETRY_ORDER = 0x826,
    ACCESS_BLOCK = 0x827,
    ACCESS_BLOCK_ALL = 0x828,
    IS707B_MAX_ACCESS_PROBES = 0x829,
    THERMAL_EMERGENCY = 0x82A,
    CONCURRENT_SERVICES_NOT_ALLOWED = 0x82B,
    INCOMING_CALL_REJECTED = 0x82C,
    NO_SERVICE_ON_GATEWAY = 0x82D,
    NO_GPRS_CONTEXT = 0x82E,
    ILLEGAL_MS = 0x82F,
    ILLEGAL_ME = 0x830,
    GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 0x831,
    GPRS_SERVICES_NOT_ALLOWED = 0x832,
    MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 0x833,
    IMPLICITLY_DETACHED = 0x834,
    PLMN_NOT_ALLOWED = 0x835,
    LOCATION_AREA_NOT_ALLOWED = 0x836,
    GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN = 0x837,
    PDP_DUPLICATE = 0x838,
    UE_RAT_CHANGE = 0x839,
    CONGESTION = 0x83A,
    NO_PDP_CONTEXT_ACTIVATED = 0x83B,
    ACCESS_CLASS_DSAC_REJECTION = 0x83C,
    PDP_ACTIVATE_MAX_RETRY_FAILED = 0x83D,
    RADIO_ACCESS_BEARER_FAILURE = 0x83E,
    ESM_UNKNOWN_EPS_BEARER_CONTEXT = 0x83F,
    DRB_RELEASED_BY_RRC = 0x840,
    CONNECTION_RELEASED = 0x841,
    EMM_DETACHED = 0x842,
    EMM_ATTACH_FAILED = 0x843,
    EMM_ATTACH_STARTED = 0x844,
    LTE_NAS_SERVICE_REQUEST_FAILED = 0x845,
    DUPLICATE_BEARER_ID = 0x846,
    ESM_COLLISION_SCENARIOS = 0x847,
    ESM_BEARER_DEACTIVATED_TO_SYNC_WITH_NETWORK = 0x848,
    ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER = 0x849,
    ESM_BAD_OTA_MESSAGE = 0x84A,
    ESM_DOWNLOAD_SERVER_REJECTED_THE_CALL = 0x84B,
    ESM_CONTEXT_TRANSFERRED_DUE_TO_IRAT = 0x84C,
    DS_EXPLICIT_DEACTIVATION = 0x84D,
    ESM_LOCAL_CAUSE_NONE = 0x84E,
    LTE_THROTTLING_NOT_REQUIRED = 0x84F,
    ACCESS_CONTROL_LIST_CHECK_FAILURE = 0x850,
    SERVICE_NOT_ALLOWED_ON_PLMN = 0x851,
    EMM_T3417_EXPIRED = 0x852,
    EMM_T3417_EXT_EXPIRED = 0x853,
    RRC_UPLINK_DATA_TRANSMISSION_FAILURE = 0x854,
    RRC_UPLINK_DELIVERY_FAILED_DUE_TO_HANDOVER = 0x855,
    RRC_UPLINK_CONNECTION_RELEASE = 0x856,
    RRC_UPLINK_RADIO_LINK_FAILURE = 0x857,
    RRC_UPLINK_ERROR_REQUEST_FROM_NAS = 0x858,
    RRC_CONNECTION_ACCESS_STRATUM_FAILURE = 0x859,
    RRC_CONNECTION_ANOTHER_PROCEDURE_IN_PROGRESS = 0x85A,
    RRC_CONNECTION_ACCESS_BARRED = 0x85B,
    RRC_CONNECTION_CELL_RESELECTION = 0x85C,
    RRC_CONNECTION_CONFIG_FAILURE = 0x85D,
    RRC_CONNECTION_TIMER_EXPIRED = 0x85E,
    RRC_CONNECTION_LINK_FAILURE = 0x85F,
    RRC_CONNECTION_CELL_NOT_CAMPED = 0x860,
    RRC_CONNECTION_SYSTEM_INTERVAL_FAILURE = 0x861,
    RRC_CONNECTION_REJECT_BY_NETWORK = 0x862,
    RRC_CONNECTION_NORMAL_RELEASE = 0x863,
    RRC_CONNECTION_RADIO_LINK_FAILURE = 0x864,
    RRC_CONNECTION_REESTABLISHMENT_FAILURE = 0x865,
    RRC_CONNECTION_OUT_OF_SERVICE_DURING_CELL_REGISTER = 0x866,
    RRC_CONNECTION_ABORT_REQUEST = 0x867,
    RRC_CONNECTION_SYSTEM_INFORMATION_BLOCK_READ_ERROR = 0x868,
    NETWORK_INITIATED_DETACH_WITH_AUTO_REATTACH = 0x869,
    NETWORK_INITIATED_DETACH_NO_AUTO_REATTACH = 0x86A,
    ESM_PROCEDURE_TIME_OUT = 0x86B,
    INVALID_CONNECTION_ID = 0x86C,
    MAXIMIUM_NSAPIS_EXCEEDED = 0x86D,
    INVALID_PRIMARY_NSAPI = 0x86E,
    CANNOT_ENCODE_OTA_MESSAGE = 0x86F,
    RADIO_ACCESS_BEARER_SETUP_FAILURE = 0x870,
    PDP_ESTABLISH_TIMEOUT_EXPIRED = 0x871,
    PDP_MODIFY_TIMEOUT_EXPIRED = 0x872,
    PDP_INACTIVE_TIMEOUT_EXPIRED = 0x873,
    PDP_LOWERLAYER_ERROR = 0x874,
    PDP_MODIFY_COLLISION = 0x875,
    MAXINUM_SIZE_OF_L2_MESSAGE_EXCEEDED = 0x876,
    NAS_REQUEST_REJECTED_BY_NETWORK = 0x877,
    RRC_CONNECTION_INVALID_REQUEST = 0x878,
    RRC_CONNECTION_TRACKING_AREA_ID_CHANGED = 0x879,
    RRC_CONNECTION_RF_UNAVAILABLE = 0x87A,
    RRC_CONNECTION_ABORTED_DUE_TO_IRAT_CHANGE = 0x87B,
    RRC_CONNECTION_RELEASED_SECURITY_NOT_ACTIVE = 0x87C,
    RRC_CONNECTION_ABORTED_AFTER_HANDOVER = 0x87D,
    RRC_CONNECTION_ABORTED_AFTER_IRAT_CELL_CHANGE = 0x87E,
    RRC_CONNECTION_ABORTED_DURING_IRAT_CELL_CHANGE = 0x87F,
    IMSI_UNKNOWN_IN_HOME_SUBSCRIBER_SERVER = 0x880,
    IMEI_NOT_ACCEPTED = 0x881,
    EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED = 0x882,
    EPS_SERVICES_NOT_ALLOWED_IN_PLMN = 0x883,
    MSC_TEMPORARILY_NOT_REACHABLE = 0x884,
    CS_DOMAIN_NOT_AVAILABLE = 0x885,
    ESM_FAILURE = 0x886,
    MAC_FAILURE = 0x887,
    SYNCHRONIZATION_FAILURE = 0x888,
    UE_SECURITY_CAPABILITIES_MISMATCH = 0x889,
    SECURITY_MODE_REJECTED = 0x88A,
    UNACCEPTABLE_NON_EPS_AUTHENTICATION = 0x88B,
    CS_FALLBACK_CALL_ESTABLISHMENT_NOT_ALLOWED = 0x88C,
    NO_EPS_BEARER_CONTEXT_ACTIVATED = 0x88D,
    INVALID_EMM_STATE = 0x88E,
    NAS_LAYER_FAILURE = 0x88F,
    MULTIPLE_PDP_CALL_NOT_ALLOWED = 0x890,
    EMBMS_NOT_ENABLED = 0x891,
    IRAT_HANDOVER_FAILED = 0x892,
    EMBMS_REGULAR_DEACTIVATION = 0x893,
    TEST_LOOPBACK_REGULAR_DEACTIVATION = 0x894,
    LOWER_LAYER_REGISTRATION_FAILURE = 0x895,
    DATA_PLAN_EXPIRED = 0x896,
    UMTS_HANDOVER_TO_IWLAN = 0x897,
    EVDO_CONNECTION_DENY_BY_GENERAL_OR_NETWORK_BUSY = 0x898,
    EVDO_CONNECTION_DENY_BY_BILLING_OR_AUTHENTICATION_FAILURE = 0x899,
    EVDO_HDR_CHANGED = 0x89A,
    EVDO_HDR_EXITED = 0x89B,
    EVDO_HDR_NO_SESSION = 0x89C,
    EVDO_USING_GPS_FIX_INSTEAD_OF_HDR_CALL = 0x89D,
    EVDO_HDR_CONNECTION_SETUP_TIMEOUT = 0x89E,
    FAILED_TO_ACQUIRE_COLOCATED_HDR = 0x89F,
    OTASP_COMMIT_IN_PROGRESS = 0x8A0,
    NO_HYBRID_HDR_SERVICE = 0x8A1,
    HDR_NO_LOCK_GRANTED = 0x8A2,
    DBM_OR_SMS_IN_PROGRESS = 0x8A3,
    HDR_FADE = 0x8A4,
    HDR_ACCESS_FAILURE = 0x8A5,
    UNSUPPORTED_1X_PREV = 0x8A6,
    LOCAL_END = 0x8A7,
    NO_SERVICE = 0x8A8,
    FADE = 0x8A9,
    NORMAL_RELEASE = 0x8AA,
    ACCESS_ATTEMPT_ALREADY_IN_PROGRESS = 0x8AB,
    REDIRECTION_OR_HANDOFF_IN_PROGRESS = 0x8AC,
    EMERGENCY_MODE = 0x8AD,
    PHONE_IN_USE = 0x8AE,
    INVALID_MODE = 0x8AF,
    INVALID_SIM_STATE = 0x8B0,
    NO_COLLOCATED_HDR = 0x8B1,
    UE_IS_ENTERING_POWERSAVE_MODE = 0x8B2,
    DUAL_SWITCH = 0x8B3,
    PPP_TIMEOUT = 0x8B4,
    PPP_AUTH_FAILURE = 0x8B5,
    PPP_OPTION_MISMATCH = 0x8B6,
    PPP_PAP_FAILURE = 0x8B7,
    PPP_CHAP_FAILURE = 0x8B8,
    PPP_CLOSE_IN_PROGRESS = 0x8B9,
    LIMITED_TO_IPV4 = 0x8BA,
    LIMITED_TO_IPV6 = 0x8BB,
    VSNCP_TIMEOUT = 0x8BC,
    VSNCP_GEN_ERROR = 0x8BD,
    VSNCP_APN_UNATHORIZED = 0x8BE,
    VSNCP_PDN_LIMIT_EXCEEDED = 0x8BF,
    VSNCP_NO_PDN_GATEWAY_ADDRESS = 0x8C0,
    VSNCP_PDN_GATEWAY_UNREACHABLE = 0x8C1,
    VSNCP_PDN_GATEWAY_REJECT = 0x8C2,
    VSNCP_INSUFFICIENT_PARAMETERS = 0x8C3,
    VSNCP_RESOURCE_UNAVAILABLE = 0x8C4,
    VSNCP_ADMINISTRATIVELY_PROHIBITED = 0x8C5,
    VSNCP_PDN_ID_IN_USE = 0x8C6,
    VSNCP_SUBSCRIBER_LIMITATION = 0x8C7,
    VSNCP_PDN_EXISTS_FOR_THIS_APN = 0x8C8,
    VSNCP_RECONNECT_NOT_ALLOWED = 0x8C9,
    IPV6_PREFIX_UNAVAILABLE = 0x8CA,
    HANDOFF_PREFERENCE_CHANGED = 0x8CB,
} RIL_DataCallFailCause_V1_5;

/**
 * Data connection active status
 */
typedef enum {
    /**
     * Indicates the data connection is inactive.
     */
    DATA_STATE_INACTIVE = 0,
    /**
     * Indicates the data connection is active with physical link dormant.
     */
    DATA_STATE_DORMANT = 1,
    /**
     * Indicates the data connection is active with physical link up.
     */
    DATA_STATE_ACTIVE = 2,
} RIL_DataConnActiveStatus;

typedef struct {
    /** Data call fail cause. DataCallFailCause.NONE if no error. */
    RIL_DataCallFailCause cause;

    /**
     * If status != DataCallFailCause.NONE, this field indicates the suggested retry back-off timer
     * value RIL wants to override the one pre-configured in FW. The unit is milliseconds.
     * The value < 0 means no value is suggested.
     * The value 0 means retry must be done ASAP.
     * The value of INT_MAX(0x7fffffff) means no retry.
     */
    int32_t suggestedRetryTime;

    /** Context ID, uniquely identifies this call. */
    int32_t cid;

    /** Data connection active status. */
    RIL_DataConnActiveStatus active;

    /**
     * PDP_type values. If cause is DataCallFailCause.ONLY_SINGLE_BEARER_ALLOWED, this is the type
     * supported such as "IP" or "IPV6".
     */
    PdpProtocolType type;

    /** The network interface name. */
    char* ifname;

    /**
     * List of link address.
     */
    int32_t leng_addresses;
    RIL_LinkAddress* addresses;

    /**
     * List of DNS server addresses, e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1". Empty if no dns
     * server addresses returned.
     */
    int32_t leng_dnses;
    char **dnses;

    /**
     * List of default gateway addresses, e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
     * When empty, the addresses represent point to point connections.
     */
    int32_t leng_gateways;
    char **gateways;

    /**
     * List of P-CSCF(Proxy Call State Control Function) addresses via PCO(Protocol Configuration
     * Option), e.g., "2001:db8::1 2001:db8::2 2001:db8::3". Empty if not IMS client.
     */
    int32_t leng_pcscf;
    char **pcscf;

    /**
     * MTU received from network for IPv4.
     * Value <= 0 means network has either not sent a value or sent an invalid value.
     */
    int32_t mtuV4;

    /**
     * MTU received from network for IPv6.
     * Value <= 0 means network has either not sent a value or sent an invalid value.
     */
    int32_t mtuV6;
} RIL_SetupDataCallResult_V1_5;

typedef enum {
    /** Circuit-switched */
    DOMAIN_CS = 1 << 0,

    /** Packet-switched */
    DOMAIN_PS = 1 << 1,
} RIL_Domain;

typedef struct {
    /**
     * Indicates whether the cell is restricted to only CSG members. A cell not broadcasting the
     * CSG Indication but reporting CSG information is considered a Hybrid Cell.
     * Refer to the "csg-Indication" field in 3GPP TS 36.331 section 6.2.2
     * SystemInformationBlockType1.
     * Also refer to "CSG Indicator" in 3GPP TS 25.331 section 10.2.48.8.1 and TS 25.304.
     */
    bool csgIndication;

    /**
     * The human-readable name of the closed subscriber group operating this cell.
     * Refer to "hnb-Name" in TS 36.331 section 6.2.2 SystemInformationBlockType9.
     * Also refer to "HNB Name" in 3GPP TS25.331 section 10.2.48.8.23 and TS 23.003 section 4.8.
     */
    char homeNodebName[MAX_CSG_HOME_NODEB_NAME];

    /**
     * The identity of the closed subscriber group that the cell belongs to.
     * Refer to "CSG-Identity" in TS 36.336 section 6.3.4.
     * Also refer to "CSG Identity" in 3GPP TS 25.331 section 10.3.2.8 and TS 23.003 section 4.7.
     */
    int32_t csgIdentity;
} RIL_ClosedSubscriberGroupInfo;

typedef union {
    /**
     * If CSG info is provided by the cell, this structure shall be present.
     */
    RIL_ClosedSubscriberGroupInfo csgInfo;
} RIL_OptionalCsgInfo;

typedef struct {
    // @1.2::CellIdentityGsm base;
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 16-bit GSM Cell Identity described in TS 27.007, 0..65535, INT_MAX if unknown  */
    int arfcn;  /* 16-bit GSM Absolute RF channel number; this value must be reported */
    uint8_t bsic; /* 6-bit Base Station Identity Code; 0xFF if unknown */
    RIL_CellIdentityOperatorNames operatorNames;
    /** Additional PLMN-IDs beyond the primary PLMN broadcast for this cell */
    int leng_additionalPlmns;
    char **additionalPlmns;
} RIL_CellIdentityGsm_V1_5;

typedef struct {
    // @1.2::CellIdentityWcdma base;
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int psc;    /* 9-bit UMTS Primary Scrambling Code described in TS 25.331, 0..511; this value must be reported */
    int uarfcn; /* 16-bit UMTS Absolute RF Channel Number; this value must be reported */
    RIL_CellIdentityOperatorNames operatorNames;
    /** Additional PLMN-IDs beyond the primary PLMN broadcast for this cell */
    int leng_additionalPlmns;
    char **additionalPlmns;
    /** Information about any closed subscriber group ID for this cell */
    RIL_OptionalCsgInfo optionalCsgInfo;
} RIL_CellIdentityWcdma_V1_5;

typedef struct {
    // @1.2::CellIdentityTdscdma base;
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int lac;    /* 16-bit Location Area Code, 0..65535, INT_MAX if unknown  */
    int cid;    /* 28-bit UMTS Cell Identity described in TS 25.331, 0..268435455, INT_MAX if unknown  */
    int cpid;    /* 8-bit Cell Parameters ID described in TS 25.331, 0..127, INT_MAX if unknown */
    int uarfcn;
    RIL_CellIdentityOperatorNames operatorNames;
    /** Additional PLMN-IDs beyond the primary PLMN broadcast for this cell */
    int leng_additionalPlmns;
    char **additionalPlmns;
    /** Information about any closed subscriber group ID for this cell */
    RIL_OptionalCsgInfo optionalCsgInfo;
} RIL_CellIdentityTdscdma_V1_5;

typedef struct {
    // @1.2::CellIdentityLte base;
    int mcc;    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int mnc;    /* 2 or 3-digit Mobile Network Code, 0..999, INT_MAX if unknown  */
    int ci;     /* 28-bit Cell Identity described in TS ???, INT_MAX if unknown */
    int pci;    /* physical cell id 0..503; this value must be reported */
    int tac;    /* 16-bit tracking area code, INT_MAX if unknown  */
    int earfcn; /* 18-bit LTE Absolute RF Channel Number; this value must be reported */
    RIL_CellIdentityOperatorNames operatorNames;
    int bandwidth;
    /** Additional PLMN-IDs beyond the primary PLMN broadcast for this cell */
    int leng_additionalPlmns;
    char **additionalPlmns;
    /** Information about any closed subscriber group ID for this cell */
    RIL_OptionalCsgInfo optionalCsgInfo;
    /** Bands used by the cell. */
    int leng_bands;
    RIL_EutranBands bands[MAX_BANDS];
} RIL_CellIdentityLte_V1_5;

typedef struct {
    // @1.4::CellIdentityNr base;
    int mcc;
    int mnc;
    uint64_t nci;
    uint32_t pci;
    int32_t tac;
    int32_t nrarfcn;
    RIL_CellIdentityOperatorNames operatorNames;
    /** Additional PLMN-IDs beyond the primary PLMN broadcast for this cell */
    int leng_additionalPlmns;
    char **additionalPlmns;
    /** Bands used by the cell. */
    int leng_bands;
    RIL_NgranBands bands[MAX_BANDS];
} RIL_CellIdentityNr_V1_5;

typedef struct {
    RIL_CellIdentityGsm_V1_5 cellIdentityGsm;
    RIL_GSM_SignalStrength_v12 signalStrengthGsm;
} RIL_CellInfoGsm_V1_5;

typedef struct {
    RIL_CellIdentityWcdma_V1_5 cellIdentityWcdma;
    RIL_WCDMA_SignalStrength_V1_2 signalStrengthWcdma;
} RIL_CellInfoWcdma_V1_5;

typedef struct {
    RIL_CellIdentityTdscdma_V1_5 cellIdentityTdscdma;
    RIL_TD_SCDMA_SignalStrength_V1_2 signalStrengthTdscdma;
} RIL_CellInfoTdscdma_V1_5;

typedef struct {
    RIL_CellIdentityLte_V1_5 cellIdentityLte;
    RIL_LTE_SignalStrength_v8 signalStrengthLte;
    bool isEndcAvailable;
} RIL_CellInfoLte_V1_5;

typedef struct {
    RIL_CellIdentityNr_V1_5 cellIdentityNr;
    RIL_NR_SignalStrength_V1_4 signalStrengthNr;
} RIL_CellInfoNr_V1_5;

typedef struct {
    RIL_CellInfoType  cellInfoType;

    /**
     * True if this cell is registered false if not registered.
     */
    bool registered;
    /**
     * Type of time stamp represented by timeStamp.
     */
    RIL_TimeStampType timeStampType;
    /**
     * Time in nanos as returned by ril_nano_time.
     */
    uint64_t timeStamp;
    union {
        RIL_CellInfoGsm_V1_5 gsm;
        RIL_CellInfoWcdma_V1_5 wcdma;
        RIL_CellInfoTdscdma_V1_5 tdscdma;
        RIL_CellInfoLte_V1_5 lte;
        RIL_CellInfoNr_V1_5 nr;
        RIL_CellInfoCdma_V1_2 cdma;
    } CellInfo;
    /**
     * Connection status for the cell.
     */
    RIL_CellConnectionStatus connectionStatus;
} RIL_CellInfo_V1_5;

typedef struct {
    RIL_CellInfoType  cellInfoType;   /* cell type for selecting from union CellInfo: RIL_CellInfoType, RIL_CellInfoType_V1_4 */
    union {
        RIL_CellIdentityGsm_V1_5 gsm;
        RIL_CellIdentityWcdma_V1_5 wcdma;
        RIL_CellIdentityTdscdma_V1_5 tdscdma;
        RIL_CellIdentityCdma_V1_2 cdma;
        RIL_CellIdentityLte_V1_5 lte;
        RIL_CellIdentityNr_V1_5 nr;
    };
} RIL_CellIdentity_V1_5;

typedef enum {
    /** Applicable to UTRAN */
    /** Barring for all CS services, including registration */
    CS_SERVICE,
    /** Barring for all PS services, including registration */
    PS_SERVICE,
    /** Barring for mobile-originated circuit-switched voice calls */
    CS_VOICE,

    /** Applicable to EUTRAN, NGRAN */
    /** Barring for mobile-originated signalling for any purpose */
    MO_SIGNALLING,
    /** Barring for mobile-originated internet or other interactive data */
    MO_DATA,
    /** Barring for circuit-switched fallback calling */
    CS_FALLBACK,
    /** Barring for IMS voice calling */
    MMTEL_VOICE,
    /** Barring for IMS video calling */
    MMTEL_VIDEO,

    /** Applicable to UTRAN, EUTRAN, NGRAN */
    /** Barring for emergency services, either CS or emergency MMTEL */
    EMERGENCY,
    /** Barring for short message services */
    SMS,

    /** Operator-specific barring codes; applicable to NGRAN */
    OPERATOR_1 = 1001,
    OPERATOR_2 = 1002,
    OPERATOR_3 = 1003,
    OPERATOR_4 = 1004,
    OPERATOR_5 = 1005,
    OPERATOR_6 = 1006,
    OPERATOR_7 = 1007,
    OPERATOR_8 = 1008,
    OPERATOR_9 = 1009,
    OPERATOR_10 = 1010,
    OPERATOR_11 = 1011,
    OPERATOR_12 = 1012,
    OPERATOR_13 = 1013,
    OPERATOR_14 = 1014,
    OPERATOR_15 = 1015,
    OPERATOR_16 = 1016,
    OPERATOR_17 = 1017,
    OPERATOR_18 = 1018,
    OPERATOR_19 = 1019,
    OPERATOR_20 = 1020,
    OPERATOR_21 = 1021,
    OPERATOR_22 = 1022,
    OPERATOR_23 = 1023,
    OPERATOR_24 = 1024,
    OPERATOR_25 = 1025,
    OPERATOR_26 = 1026,
    OPERATOR_27 = 1027,
    OPERATOR_28 = 1028,
    OPERATOR_29 = 1029,
    OPERATOR_30 = 1030,
    OPERATOR_31 = 1031,
    OPERATOR_32 = 1032,
} RIL_BarringServiceType;

typedef enum {
    /** Device is not barred for the given service */
    BARRING_TYPE_NONE,
    /** Device may be barred based on time and probability factors */
    BARRING_TYPE_CONDITIONAL,
    /* Device is unconditionally barred */
    BARRING_TYPE_UNCONDITIONAL,
} RIL_BarringType;

typedef struct {
    /** The barring factor as a percentage 0-100 */
    int32_t barringFactor;

    /** The number of seconds between re-evaluations of barring */
    int32_t barringTimeSeconds;

    /**
     * Indicates whether barring is currently being applied.
     *
     * <p>True if the UE applies barring to a conditionally barred
     * service based on the conditional barring parameters.
     *
     * <p>False if the service is conditionally barred but barring
     * is not currently applied, which could be due to either the
     * barring criteria not having been evaluated (if the UE has not
     * attempted to use the service) or due to the criteria being
     * evaluated and the UE being permitted to use the service
     * despite conditional barring.
     */
    bool isBarred;
} RIL_ConditionalBarringInfo;

typedef union {
    /** Must be included if barring is conditional */
    RIL_ConditionalBarringInfo conditionalBarringInfo;
} RIL_BarringTypeSpecificInfo;

typedef struct {
    /** Barring service */
    RIL_BarringServiceType service;

    /** The type of barring applied to the service */
    RIL_BarringType type;

    /** Type-specific barring info if applicable */
    RIL_BarringTypeSpecificInfo typeSpecificInfo;
} RIL_BarringInfo;

typedef enum {
    /** 0 - None */
    REG_FAIL_CAUSE_NONE = 0,
    /** 2 - IMSI unknown in HLR */
    REG_FAIL_CAUSE_IMSI_UNKNOWN_IN_HLR = 2,
    /** 3 - Illegal MS */
    REG_FAIL_CAUSE_ILLEGAL_MS = 3,
    /** 4 - Illegal ME */
    REG_FAIL_CAUSE_IMSI_UNKNOWN_IN_VLR = 4,
    /** 5 - PLMN not allowed */
    REG_FAIL_CAUSE_IMEI_NOT_ACCEPTED = 5,
    /** 6 - Location area not allowed */
    REG_FAIL_CAUSE_ILLEGAL_ME = 6,
    /** 7 - Roaming not allowed */
    REG_FAIL_CAUSE_GPRS_SERVICES_NOT_ALLOWED = 7,
    /** 8 - No Suitable Cells in this Location Area */
    REG_FAIL_CAUSE_GPRS_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 8,
    /** 9 - Network failure */
    REG_FAIL_CAUSE_MS_IDENTITY_CANNOT_BE_DERIVED_BY_NETWORK = 9,
    /** 10 - Persistent location update reject */
    REG_FAIL_CAUSE_IMPLICITLY_DETACHED = 10,
    /** 11 - PLMN not allowed */
    REG_FAIL_CAUSE_PLMN_NOT_ALLOWED = 11,
    /** 12 - Location area not allowed */
    REG_FAIL_CAUSE_LOCATION_AREA_NOT_ALLOWED = 12,
    /** 13 - Roaming not allowed in this Location Area */
    REG_FAIL_CAUSE_ROAMING_NOT_ALLOWED = 13,
    /** 14 - GPRS Services not allowed in this PLMN */
    REG_FAIL_CAUSE_GPRS_SERVICES_NOT_ALLOWED_IN_PLMN = 14,
    /** 15 - No Suitable Cells in this Location Area */
    REG_FAIL_CAUSE_NO_SUITABLE_CELLS = 15,
    /** 16 - MSC temporarily not reachable */
    REG_FAIL_CAUSE_MSC_TEMPORARILY_NOT_REACHABLE = 15,
    /** 17 - Network Failure */
    REG_FAIL_CAUSE_NETWORK_FAILURE = 17,
    /** 20 - MAC Failure */
    REG_FAIL_CAUSE_MAC_FAILURE = 20,
    /** 21 - Sync Failure */
    REG_FAIL_CAUSE_SYNC_FAILURE = 21,
    /** 22 - Congestion */
    REG_FAIL_CAUSE_CONGESTION = 22,
    /** 23 - GSM Authentication unacceptable */
    REG_FAIL_CAUSE_GSM_AUTHENTICATION_UNACCEPTABLE = 23,
    /** 25 - Not Authorized for this CSG */
    REG_FAIL_CAUSE_NOT_AUTHORIZED_FOR_THIS_CSG = 25,
    /** 28 SMS provided via GPRS in this routing area */
    REG_FAIL_CAUSE_SMS_PROVIDED_BY_GPRS_IN_ROUTING_AREA,
    /** 32 - Service option not supported */
    REG_FAIL_CAUSE_SERVICE_OPTION_NOT_SUPPORTED = 32,
    /** 33 - Requested service option not subscribed */
    REG_FAIL_CAUSE_SERVICE_OPTION_NOT_SUBSCRIBED = 33,
    /** 34 - Service option temporarily out of order */
    REG_FAIL_CAUSE_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER = 34,
    /** 38 - Call cannot be identified */
    REG_FAIL_CAUSE_CALL_CANNOT_BE_IDENTIFIED = 38,
    /** 40 No PDP context activated */
    REG_FAIL_CAUSE_NO_PDP_CONTEXT_ACTIVATED = 40,
    /** 48-63 - Retry upon entry into a new cell */
    RETRY_UPON_ENTRY_INTO_NEW_CELL_1 = 48,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_2 = 49,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_3 = 50,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_4 = 51,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_5 = 52,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_6 = 53,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_7 = 54,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_8 = 55,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_9 = 56,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_10 = 57,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_11 = 58,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_12 = 59,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_13 = 60,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_14 = 61,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_15 = 62,
    RETRY_UPON_ENTRY_INTO_NEW_CELL_16 = 63,
    /** 95 - Semantically incorrect message */
    REG_FAIL_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE = 95,
    /** 96 - Invalid mandatory information */
    REG_FAIL_CAUSE_INVALID_MANDATORY_INFORMATION = 96,
    /** 97 - Message type non-existent or not implemented */
    REG_FAIL_CAUSE_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 97,
    /** 98 - Message type not compatible with protocol state */
    REG_FAIL_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 98,
    /** 99 - Information element non-existent or not implemented */
    REG_FAIL_CAUSE_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99,
    /** 100 - Conditional IE error */
    REG_FAIL_CAUSE_CONDITIONAL_IE_ERROR = 100,
    /** 101 - Message not compatible with protocol state */
    REG_FAIL_CAUSE_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE = 101,
    /** 111 - Protocol error, unspecified */
    REG_FAIL_CAUSE_PROTOCOL_ERROR_UNSPECIFIED = 111,
} RIL_RegistrationFailCause;

typedef enum {
    PRL_IND_NOT_REGISTERED = -1,
    PRL_IND_NOT_IN_PRL = 0,
    PRL_IND_IN_PRL = 1,
} RIL_PrlIndicator;

typedef struct {
    RIL_RegState regState;
    RIL_RadioTechnology rat;
    RIL_RegistrationFailCause reasonForDenial;
    RIL_CellIdentity_V1_5 cellIdentity;
    char registeredPlmn[MAX_REG_PLMN_LEN];
    union {
        struct {
            bool cssSupported;
            int32_t roamingIndicator;
            RIL_PrlIndicator systemIsInPrl;
            int32_t defaultRoamingIndicator;
        } cdmaInfo;
        struct {
            RIL_LteVopsInfo lteVopsInfo;
            RIL_NrIndicators nrIndicators;
        } eutranInfo;
    } accessTechnologySpecificInfo;
} RIL_RegStateResult;

typedef struct {
    RIL_ScanStatus status;
    uint32_t network_infos_length;
    RIL_CellInfo_V1_5* network_infos;
    RIL_Errno error;
} RIL_NetworkScanResult_V1_5;

typedef enum {
    RIL_PERSOSUBSTATE_SIM_SPN           = 25,
    RIL_PERSOSUBSTATE_SIM_SPN_PUK       = 26,
    /** Equivalent Home PLMN */
    RIL_PERSOSUBSTATE_SIM_SP_EHPLMN     = 27,
    RIL_PERSOSUBSTATE_SIM_SP_EHPLMN_PUK = 28,
    RIL_PERSOSUBSTATE_SIM_ICCID         = 29,
    RIL_PERSOSUBSTATE_SIM_ICCID_PUK     = 30,
    RIL_PERSOSUBSTATE_SIM_IMPI          = 31,
    RIL_PERSOSUBSTATE_SIM_IMPI_PUK      = 32,
    /** Network subset service provider */
    RIL_PERSOSUBSTATE_SIM_NS_SP         = 33,
    RIL_PERSOSUBSTATE_SIM_NS_SP_PUK     = 34,
} RIL_PersoSubstate_V1_5;

typedef enum {
    DEFAULT = 0,
    TETHERED = 1,
    IMS = 2,
    FOTA = 3,
    CBS = 4,
    OEM_BASE = 1000,
    INVALID = 0xFFFFFFFF,
    INTERNET_FOR_CP = 0xFF, // This is not a regular RADIO HAL definition,
                            // specific to Exynos CP
} RIL_DataProfileId;

typedef enum {
    NO_PAP_NO_CHAP,
    PAP_NO_CHAP,
    NO_PAP_CHAP,
    PAP_CHAP,
} RIL_ApnAuthType;

typedef enum {
    COMMON,
    THREE_GPP,
    THREE_GPP2
} RIL_DataProfileInfoType;

typedef struct {
    /** ID of the data profile. */
    RIL_DataProfileId profileId;

    /** The APN name. */
    char* apn;

    /** PDP_type values. */
    PdpProtocolType protocol;

    /** PDP_type values used on roaming network. */
    PdpProtocolType roamingProtocol;

    /** APN authentication type. */
    RIL_ApnAuthType authType;

    /** The username for APN, or empty string. */
    char* user;

    /** The password for APN, or empty string. */
    char* password;

    /** Data profile technology type. */
    RIL_DataProfileInfoType type;

    /** The period in seconds to limit the maximum connections. */
    int32_t maxConnsTime;

    /** The maximum connections during maxConnsTime. */
    int32_t maxConns;

    /**
     * The required wait time in seconds after a successful UE initiated disconnect of a given PDN
     * connection before the device can send a new PDN connection request for that given PDN.
     */
    int32_t waitTime;

    /** True to enable the profile, false to disable. */
    bool enabled;

    /** Supported APN types bitmap. See ApnTypes for the value of each bit. */
    uint32_t supportedApnTypesBitmap;

    /** The bearer bitmap. See RadioAccessFamily for the value of each bit. */
    uint32_t bearerBitmap;

    /** Maximum transmission unit (MTU) size in bytes for IPv4. */
    int32_t mtuV4;

    /** Maximum transmission unit (MTU) size in bytes for IPv6. */
    int32_t mtuV6;

    /**
     * True if this data profile was used to bring up the last default (i.e internet) data
     * connection successfully.
     */
    bool preferred;

    /**
     * If true, modem must persist this data profile and profileId must not be
     * set to DataProfileId.INVALID. If the same data profile exists, this data profile must
     * overwrite it.
     */
    bool persistent;
} RIL_DataProfileInfo_V1_5;

typedef struct {
    int accessNetwork;
    RIL_DataProfileInfo_V1_5 dataProfileInfo;
    bool roamingAllow;
    int reason;
    int32_t leng_addresses;
    RIL_LinkAddress* linkAddress;
    char *dnses;
} RIL_SetupDataCallInfo_V1_5;

/**
 * Overwritten from @1.0::SetupDataCallResult in order to update the DataCallFailCause to 1.4
 * version.
 */
typedef struct {
    /** Data call fail cause. DataCallFailCause.NONE if no error. */
    int cause;

    /**
     * If status != DataCallFailCause.NONE, this field indicates the suggested retry back-off timer
     * value RIL wants to override the one pre-configured in FW. The unit is milliseconds.
     * The value < 0 means no value is suggested.
     * The value 0 means retry must be done ASAP.
     * The value of INT_MAX(0x7fffffff) means no retry.
     */
    int suggestedRetryTime;

    /** Context ID, uniquely identifies this call. */
    int cid;

    /** Data connection active status. */
    int active;

    /**
     * PDP_type values. If cause is DataCallFailCause.ONLY_SINGLE_BEARER_ALLOWED, this is the type
     * supported such as "IP" or "IPV6".
     */
    int type;

    /** The network interface name. */
    char *ifname;

    /**
     * List of addresses with optional "/" prefix length, e.g., "192.0.1.3" or
     * "192.0.1.11/16 2001:db8::1/64".  Typically one IPv4 or one IPv6 or one of each. If the
     * prefix length is absent the addresses are assumed to be point to point with IPv4 having a
     * prefix length of 32 and IPv6 128.
     */
    int len_addresses;
    char **addresses;

    /**
     * List of DNS server addresses, e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1". Empty if no dns
     * server addresses returned.
     */
    int len_dnses;
    char **dnses;

    /**
     * List of default gateway addresses, e.g., "192.0.1.3" or "192.0.1.11 2001:db8::1".
     * When empty, the addresses represent point to point connections.
     */
    int len_gateways;
    char **gateways;

    /**
     * List of P-CSCF(Proxy Call State Control Function) addresses via PCO(Protocol Configuration
     * Option), e.g., "2001:db8::1 2001:db8::2 2001:db8::3". Empty if not IMS client.
     */
    int len_pcscf;
    char **pcscf;

    /**
     * MTU received from network. Value <= 0 means network has either not sent a value or sent an
     * invalid value.
     */
    int mtuV4;
    int mtuV6;
} RIL_Data_Call_Response_V1_5;

typedef enum  {
    /** Control the unsolicited sending of registration failure reports via onRegistrationFailed */
    RIL_INDFILTER_REGISTRATION_FAILURE = 1 << 5,
    /** Control the unsolicited sending of barring info updates via onBarringInfo */
    RIL_INDFILTER_BARRING_INFO = 1 << 6,
} RIL_IndicationFilter_V1_5;

typedef struct
{
    bool isSpecifyChannels;
    uint32_t specifiers_length;               // Length of specifiers
    RIL_RadioAccessSpecifier_V1_5 specifiers[MAX_RADIO_ACCESS_NETWORKS]; // Radio access networks
}RIL_SysSelChannelRequest_V1_5;

typedef struct
{
    int accessNetwork;                        // RIL_AccessNetwork, RIL_AccessNetwork_V1_5
    RIL_SignalThresholdInfo signalThresholdInfo;
}RIL_SignalStrengthReportingCriteria_V1_5;

typedef struct
{
    RIL_CellIdentity_V1_5 cellIdentity;

    /** 5 or 6 digit alphanumeric PLMN (MCC|MNC) among those broadcast by the
    *        cell that was chosen for the failed registration attempt
    */
    char *chosenPlmn;

    /** RIL_Domain */
    int domainBitmap;

    /** the primary failure cause code of the procedure.
    *        For GSM/UMTS (MM), values are in TS 24.008 Sec 10.5.95
    *        For GSM/UMTS (GMM), values are in TS 24.008 Sec 10.5.147
    *        For LTE (EMM), cause codes are TS 24.301 Sec 9.9.3.9
    *        For NR (5GMM), cause codes are TS 24.501 Sec 9.11.3.2
    *        MAX_INT if this value is unused.
    */
    int causeCode;

    /** the cause code of any secondary/combined procedure if appropriate.
    *        For UMTS, if a combined attach succeeds for PS only, then the GMM cause code shall be
    *        included as an additionalCauseCode.
    *        For LTE (ESM), cause codes are in TS 24.301 9.9.4.4
    *        MAX_INT if this value is unused.
    */
    int additionalCauseCode;
}RIL_RegistrationFailed;

typedef struct
{
    RIL_CellIdentity_V1_5 cellIdentity;
    uint32_t numOfrecord;
    RIL_BarringInfo* precords;
}RIL_CellIdAndBarringInfo;

typedef struct {
    uint32_t hysteresisMs;                // A hysteresis time in milliseconds to prevent flapping.
    // A value of 0 disables hysteresis.
    uint32_t hysteresisDlKbps;            // An interval in kbps defining the required magnitude change between DL reports.
    // hysteresisDlKbps must be smaller than the smallest threshold delta.
    // An interval value of 0 disables hysteresis.
    uint32_t hysteresisUlKbps;            // An interval in kbps defining the required magnitude change between UL reports.
    // hysteresisUlKbps must be smaller than the smallest threshold delta.
    // An interval value of 0 disables hysteresis.
    uint32_t numOfThresholdsDownlinkKbps; // A size of vector of trigger thresholdsDownlinkKbps.
    uint32_t* thresholdsDownlinkKbps;     // A vector of trigger thresholds in kbps for downlink reports.
    // A vector size of 0 disables the use of DL thresholds for reporting.
    uint32_t numOfThresholdsUplinkKbps;   // A size of vector of trigger thresholdsUplinkKbps.
    uint32_t* thresholdsUplinkKbps;       // A vector of trigger thresholds in kbps for uplink reports.
    // A vector size of 0 disables the use of UL thresholds for reporting.
    RIL_AccessNetwork_V1_5 accessNetwork;      // The type of network for which to apply these thresholds.
} RIL_LinkCapacityReportingCriteria_V1_5;

/**
 * Send an SMS message. Identical to sendCdmaSms,
 * except that more messages are expected to be sent soon.
 */
#define RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE   148

/**
 * Enable or disable UiccApplications on the SIM. If disabled:
 *  - Modem will not register on any network.
 *  - SIM must be PRESENT, and the IccId of the SIM must still be accessible.
 *  - The corresponding modem stack is still functional, e.g. able to make emergency calls or
 *    do network scan.
 * By default if this API is not called, the uiccApplications must be enabled automatically.
 * It must work for both single SIM and DSDS cases for UX consistency.
 * The preference is per SIM, and must be remembered over power cycle, modem reboot, or SIM
 * insertion / unplug.
 */
#define RIL_REQUEST_ENABLE_UICC_APPLICATIONS    208

/**
 * Whether uiccApplications are enabled, or disabled.
 *
 * By default uiccApplications must be enabled, unless enableUiccApplications() with enable
 * being false is called.
 */
#define RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT    209

/**
 * Specify which bands modem's background scan must act on.
 * If specifyChannels is true, it only scans bands specified in specifiers.
 * If specifyChannels is false, it scans all bands.
 *
 * For example, CBRS is only on LTE band 48. By specifying this band,
 * modem saves more power.
 *
 * @param specifyChannels whether to scan bands defined in specifiers.
 * @param specifiers which bands to scan. Only used if specifyChannels is true.
 *
 */
#define RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS 210


/**
 * Get all the barring info for the current camped cell applicable to the current user.
 *
 * @param serial Serial number of request.
 *
 */
#define RIL_REQUEST_GET_BARRING_INFO    211

/**
 * TODO: Below command definition is temporary, because they are not defined by AOSP.
 */
#define RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION    212

/**
 * Report change of whether uiccApplications are enabled, or disabled.
 */
#define RIL_UNSOL_UICC_APPLICATIONS_ENABLEMENT_CHANGED  1103

/**
 * Report that Registration or a Location/Routing/Tracking Area update has failed.
 *
 * Indicate whenever a registration procedure, including a location, routing, or tracking
 * area update fails. This includes procedures that do not necessarily result in a change of
 * the modem's registration status. If the modem's registration status changes, that is
 * reflected in the onNetworkStateChanged() and subsequent get{Voice/Data}RegistrationState().
 *
 */
#define RIL_UNSOL_REGISTRATION_FAILED   1104

/**
 * Indicate barring information for the user's access category / access class and PLMN.
 *
 * Provide information about the barring status of the cell for the user. The information
 * provided should describe all barring configurations that are applicable to the current user,
 * even if the user is not currently barred (due to conditional barring). This informs Android
 * of likely future (statistical) barring for specific services.
 *
 * This indication should be sent whenever the cell barring config changes for the current
 * user, or if the user conditional barring status changes due to re-evaluation of the
 * barring conditions. Barring status will likely change when the device camps for service,
 * when PLMN selection is completed, when the device attempts to access a conditionally barred
 * service, and when the System Information including barring info for a camped cell is updated.
 */
#define RIL_UNSOL_BARRING_INFO_CHANGED  1105

#endif // __RADIO_V1_5_H__
