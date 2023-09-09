/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __CONSTANT_DEF_H__
#define __CONSTANT_DEF_H__


#define RCM_ID                             255
#define TOKEN_INVALID                      0xFFFFFFFF

// Call & SS
#define MAX_DIAL_NUM                       82
#define MAX_DIAL_NAME                      82
#define MAX_UUS_DATA_LEN                   128
#define MAX_CALL_LIST_NUM                  9
#define MAX_USSD_DATA_LEN                  182
#define MAX_DTMF_LEN                       64
#define MAX_SS_NUM_LEN                     32
#define MAX_CALL_FORWARD_STATUS_NUM        8
#define MAX_SS_RETURN_RESULT               255
#define MAX_EMERGENCY_CALL_NUM_V1_1        10
#define MAX_EMERGENCY_CALL_NUM_V1_4        20
#define MAX_ENL_RECORD_NUM                 20
#define MAX_EENL_RECORD_NUM                50
#define MAX_SUB_SERVICE_LEN                255
#define MAX_EMERGENCY_NUMBER_LEN           32
#define MAX_EENL_EMERGENCY_NUMBER_LEN      20
#define MAX_SS_INFO_NUM                    (4)
#define MAX_EMERGENCY_NUMBER_LIST_COUNT    255
#define MAX_URN_LEN                        255
#define MAX_URN_COUNT                      4
#define MAX_CALL_PARTY_NUM_LEN             513
#define MAX_VALID_ICCID_LEN                8
#define MAX_TAG_LEN                        32

// SMS
#define MAX_GSM_SMS_SERVICE_CENTER_ADDR    12
#define MAX_GSM_SMS_TPDU_SIZE 244  // TODO: Should be updated as 176 correctly
#define MAX_CB_MSG                         88
#define MAX_CB_CHANNEL_NUM                 50
#define MAX_BCST_INFO_NUM                  50
    // MAX_BCST_MSG_LEN =
    //  1252 bytes(Max. 15 CBS Messae pages) + 2 bytes(WAC length) + 1024 bytes(Max. WAC)
    // Max. WAC size is
    //  1024 bytes = CMSP Defined Use
    //  or 321 bytes = WAC Geo-Fencing Maximum Wait Time(3 bytes) + 9 Circles(90 bytes)
    //  + 1 Polygon(2 bytes + 226 bytes(22 bits * 82)), See ATIS-0700041
#define MAX_BCST_MSG_LEN                   2278

//SIM & SAT
#define MAX_SIM_AID_LEN                    16
#define MAX_SIM_APP_LABEL_LEN              32
#define MAX_SIM_APPS_INFO_COUNT            100
#define MAX_SIM_PIN_LEN                    8
#define MAX_SIM_PUK_LEN                    8
#define MAX_SIM_FACILITY_PASSWORD_LEN      39
#define MAX_SIM_IO_PATH_LEN                12
#define MAX_SIM_IO_DATA_LEN                512
#define MAX_SIM_PATH_LEN                   50
#define MAX_SIM_DATA_LEN                   512
#define MAX_SIM_AUTH_REQ_LEN               68
#define MAX_SIM_AUTH_RSP_LEN               69
#define MAX_IMSI_LEN                       15
#define MAX_STK_MSG_LEN                    512
#define MAX_PASSWORD_LEN                   (8+1)
#define MAX_SIM_GBA_AUTH_REQ_LEN           512
#define MAX_SIM_GBA_AUTH_RSP_LEN           260
#define MAX_OPEN_CHANNEL_RSP_LEN           256
#define BASIC_APDU_LEN                     512
#define MAX_APDU_LEN                       (16384)      // 16K
#define MAX_ATR_LEN                        (33)
#define MAX_PB_ENTRY_NUM                   13
#define MAX_PB_FIELD_ENTRY                 32
#define MAX_PB_ENTRY_LEN                   (800)
#define MAX_VSIM_DATA_LEN                  261
#define MAX_FACILITY_CODE_LEN              3
#define MAX_ICCID_LEN                      (10)
#define MAX_ICCID_STR_LEN                  (MAX_ICCID_LEN * 2)
#define MAX_EID_LEN                      (16)
#define MAX_ICCID_STRING_LEN               20
#define MAX_CARRIER_INFO_NUM               (10)
#define MAX_CR_MATCH_DATA_SIZE             (32)
#define INVALID_SLOT_ID                                 (0xFF)
#define MAX_PORT_NUM                       4
#define MAX_SLOT_NUM                       4
#define MAX_ENCRYPTED_PIN_LEN               (32)
#define MAX_GBA_AUTH_REQ_DATA_LEN          255
#define MAX_GBA_AUTH_TYPE                  2
#define MAX_GBA_TYPE                       2

// SAP
#define MAX_SAP_TRANSFER_APDU_LEN          512
#define MAX_SAP_TRANSFER_ATR_LEN           33

// DEVICEID & MISC
#define MAX_IMEI_LEN                       15
#define MAX_IMEISV_LEN                     17
#define MAX_MEID_LEN                       14
#define MAX_ESN_LEN                        11
#define MAX_ME_SN_LEN                      (32+1)
#define MAX_BB_SW_VER_LEN                  32
#define MAX_BB_HW_VER_LEN                  32
#define MAX_BB_RF_CAL_DATE_LEN             32
#define MAX_BB_PRODUCT_CODE_LEN            32
#define MAX_BB_MODEL_ID_LEN                17
#define MAX_BB_PRL_ERI_VER_LEN             17
#define MAX_BB_CP_CHIPSET_LEN              16
#define MAX_MCC_LEN                        (3)
#define MAX_MNC_LEN                        (3)
#define MAX_IMSI_ENCRIPTION_KEY_LEN        (32)
#define MAX_MODEL_NAME_LEN                 32
#define MAX_AP_OS_VERSION_LEN              32
#define MAX_RSSI_SCAN_RESULT               2044 // TODO: As per CP JSON document size should be 4048
#define SIT_NUM_OF_RECIEVER                5

// PDP & NET
#define MAX_CID_NUM_LEN                    10
#define MAX_PLMN_LEN                       6
#define MIN_PLMN_LEN                       5
#define MAX_SHORT_NAME_LEN                 32
#define MAX_FULL_NAME_LEN                  64
#define MAX_PDP_APN_LEN                    101
#define MAX_PDP_ADDRESS_LEN                20
#define DEFAULT_CELL_INFO_VER              2
// This will be '60' for legacy format
#define MAX_PCSCF_ADDRESS_LEN              ((MAX_IPV4_ADDR_LEN * 3) + (MAX_IPV6_ADDR_LEN * 3))
#define MAX_PDP_USER_NAME_LEN              50
#define MAX_AUTH_USER_NAME_LEN             50
#define MAX_AUTH_PASSWORD_LEN              50
#define MAX_NET_INFO_COUNT                 16
#define MAX_IPV4_ADDR_LEN                  4
#define MAX_IPV6_ADDR_LEN                  16
#define MAX_IPV4_PREFIX_LEN                32
#define MAX_RTR_SOLICITATIONS              3
#define MAX_IFNAME_LEN                     256
#define MAX_IFPREFIX_LEN                   32
#define MAX_IOCHANNEL_NAME_LEN             32
#define MAX_DATA_CALL_SIZE                 (15)
#define MAX_ADDRESS_STRING_LEN             256
#define MAX_PREFERRED_PLMN_SIZE            150
#define NET_BAND_MAX                       32
#define MAX_PCO_CONTENT_LEN                (255)
#define MAX_DB_ECC_ID_LEN                  32
#define MAX_QOS_FILTERS                    (255)
#define MAX_ADDRESS_NUM                    (2)
#define EACH_PLMN_LEN                      3
#define MAX_ADDITIONAL_PLMN_LEN            10
#define HNB_NAME_LEN                       100
#define MAX_TD_LIST                        (15)
#define MAX_RSSI_SCAN_RESULT_SIZE          4088

// SND

//IMS
#define MAX_PDN_IP_LEN                     16
#define MAX_DELAYED_TIME_REF               8

//WLan SIM auth
#define MAX_AUTH_LEN                       (34)
#define MAX_AUTH_RSP_LEN                   (69)

//MISC
#define RIL_NUM_TX_POWER_LEVELS            5
#define MAX_ADB_SERIAL_NUMBER              16
#define MAX_FCI_LEN                        5
#define MAX_SW_VER_LEN                     32
#define MAX_HW_VER_LEN                     32
#define MAX_RF_CAL_DATE_LEN                32
#define MAX_PRODUCT_CODE_LEN               32
#define MAX_MODEL_ID_LEN                   17
#define MAX_PRL_VER_LEN                    51
#define MAX_ERI_VER_LEN                    51
#define MAX_CP_CHIPSET_LEN                 16
#define MAX_UL_DL_KBPS_LEN                 20
#define MAX_DBM_LEN                        10

#define MAX_FORBID_LTE_BAND                44

#define GID_DEFAULT_VALUE_ANY               "*"

// Call & SS
#define MAX_BURST_DTMF_LEN                 32
#define MAX_BURST_DTMF_ON_OFF_LEN          255
#define MAX_FLASH_LEN                      35
#define MAX_ALPHA_INFO_BUF_LEN             64
#define MAX_NUMBER_INFO_BUFFER_LEN         81
#define MAX_NUMBER_OF_INFO_RECS            10

// SMS
    // CDMA SMS MESSAGE SIZE
    // Max. 304 Bytes for MO
    //     TYPE(1) + Teleservice Id(4) + Destination Address(5 + RIL_CDMA_SMS_ADDRESS_MAX(36))
    //     + Bearer Reply Option(3)+ Bearer Data(RIL_CDMA_SMS_BEARER_DATA_MAX(255))
    // Max. 348 Bytes for MT
    //     TYPE(1) + Teleservice Id(4) + Service Category(4) + Originating Address(5 + RIL_CDMA_SMS_ADDRESS_MAX(36))
    //     Origination Subaddress(4 + RIL_CDMA_SMS_SUBADDRESS_MAX(36)) + Bearer Reply Option(3)
    //     + Bearer Data(RIL_CDMA_SMS_BEARER_DATA_MAX(255))
#define MAX_CDMA_SMS_MSG_SIZE              348
#define MAX_CDMA_SMS_RUIM_MSG_SIZE         253 // See 3GPP2 C.S0023 3.4.27
#define MAX_CDMA_BCST_INFO_NUM             50

// DEVICEID & MISC
#define MAX_CDMA_MDN_LEN                   15
#define MAX_CDMA_MIN_LEN                   10
#endif
