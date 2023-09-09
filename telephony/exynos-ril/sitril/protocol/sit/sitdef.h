/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _SIT_DEF_H_
#define _SIT_DEF_H_
/******************************************************************************
 *
 * File: sitdef.h
 *
 * Description: Define SIT(Shannon Interface for Telephony) Command Structure
 *
  *****************************************************************************/

/**********************************************************************************

                       S I T   M E S S A G E   F O R M A T

***********************************************************************************/
/* SIT HEADER
-----------------------------------------------------------------------------------
  | LENGTH(4) | SEQ_NUM(4) | CONFIG(1) | CH_ID(2) | RESERVED(1) | PAYLOAD(X)
  ----------------------------------------------------------------------------------
*/

/* SIT PAYLOAD
-----------------------------------------------------------------------------------
  | CMD_ID(2) | TRANS_ID(4) | LENGTH(4) | PARAMETER(X)
  ----------------------------------------------------------------------------------
*/

/*********************************************************************************/

#include <base/types.h>
#include "constdef.h"
#include "commondef.h"

#pragma pack(1)


#ifndef packed__
#define packed__ __attribute__ ((packed))
#endif

//-------------------------------------------------------------------------------------------------
// Codes below must be consistently updated all together. Be careful!!
//    : Each 'enum', 'rcm_name', and 'rcm_err_name' must be consistent for a proper log printing.
//
// [2014-07-18] Written by arkade (arkade.kang@samsung.com)
//-------------------------------------------------------------------------------------------------

#define SIT_MAX_UUID_LENGTH (64)
#define SIT_CELL_INFO_V12 (12)
#define SIT_CELL_INFO_V14 (14)
#define SIT_CELL_INFO_V15 (15)

// struct
#pragma pack(1)

typedef struct {
    UINT    token;
    BYTE    reserved[2];
} RCM_REQ_EXT;

typedef struct {
    UINT    token;
    BYTE    error;
    BYTE    reserved[1];
} RCM_RSP_EXT;

typedef struct {
    BYTE    reserved[2];
} RCM_IND_EXT;

typedef struct {
    BYTE    type;
    BYTE    reserved;
    UINT16    id;
    UINT16    length;
} RCM_GEN_HEADER;

typedef struct {
    BYTE    type;
    BYTE    reserved;
    UINT16    id;
    UINT16    length;
    union {
        RCM_REQ_EXT req;
        RCM_RSP_EXT rsp;
    } ext;
} RCM_HEADER;

typedef struct {
    BYTE    type;
    BYTE    reserved;
    UINT16    id;
    UINT16    length;
    RCM_IND_EXT ext;
} RCM_IND_HEADER;

typedef struct
{
    RCM_HEADER hdr;
}null_data_format;

typedef struct
{
    RCM_IND_HEADER hdr;
}null_ind_data_format;

typedef struct
{
    RCM_HEADER hdr;
    char data[0];
}sit_simple_data_format;

typedef struct
{
    RCM_IND_HEADER hdr;
    char data[0];
}sit_simple_ind_data_format;

/*
    SIT_GET_CURRENT_CALLS (RCM ID = 0x0000)
*/
typedef struct
{
    RCM_HEADER hdr;
}sit_call_get_current_calls_req ;

struct sit_call_info_type_v1_1
{
    BYTE state; // sit_call_state_e_type
    INT32 index;
    INT32 type_of_address;
    BYTE is_mpty; // sit_call_is_mtpy_e_type
    BYTE is_mt; // sit_call_is_mt_e_type
    BYTE als;
    BYTE call_type; // sit_call_call_type_e_type
    BYTE is_voice_privacy; // sit_call_is_voice_privacy_e_type
    BYTE num_len;
    BYTE num[ MAX_DIAL_NUM ];
    INT32 number_presentation; // sit_call_presentation_e_type
    BYTE name_len;
    BYTE name[ MAX_DIAL_NAME ];
    INT32 name_presentation; // sit_call_presentation_e_type
    BYTE name_dcs;
    INT32 uus_type; // sit_call_uus_type_e_type
    INT32 uus_dcs; // sit_call_uus_dcs_e_type
    BYTE uus_data_len;
    BYTE uus_data[ MAX_UUS_DATA_LEN ];
    BYTE SRVCCCall;
};

typedef struct
{
    RCM_HEADER hdr;
    INT32 number;
    sit_call_info_type_v1_1 record[MAX_CALL_LIST_NUM];
}sit_call_get_current_calls_rsp_v1_1;

struct sit_call_info_type_v1_2 : public sit_call_info_type_v1_1
{
    INT32 audioQuality; // sit_call_audio_quality_e_type
};

typedef struct
{
    RCM_HEADER hdr;
    INT32 number;
    sit_call_info_type_v1_2 record[MAX_CALL_LIST_NUM];
}sit_call_get_current_calls_rsp_v1_2;

typedef struct
{
    BYTE state; // sit_call_state_e_type
    INT32 index;
    INT32 type_of_address;
    BYTE is_mpty; // sit_call_is_mpty_e_type
    BYTE is_mt; // sit_call_is_mt_e_type
    BYTE als;
    BYTE call_type; // sit_call_call_type_e_type
    BYTE is_voice_privacy; // sit_call_is_voice_privacy_e_type
    BYTE num_len;
    BYTE num[MAX_DIAL_NUM];
    INT32 number_presentation; // sit_call_presentation_e_type
    BYTE name_len;
    BYTE name[MAX_DIAL_NAME];
    INT32 name_presentation; // sit_call_presentation_e_type
    BYTE name_dcs;
    INT32 uus_type; // sit_call_uus_type_e_type
    INT32 uus_dcs;  // sit_call_uus_dcs_e_type
    BYTE uus_data_len;
    BYTE uus_data[MAX_UUS_DATA_LEN];
    BYTE SRVCCCall;
    INT32 audioQuality; // sit_call_audio_quality_e_type
    BYTE forwarded_num_len;
    BYTE forwarded_num[MAX_SS_NUM_LEN];
}sit_call_info_type_v1_6;

typedef struct
{
    RCM_HEADER hdr;
    INT32 number;
    sit_call_info_type_v1_6 record[MAX_CALL_LIST_NUM];
}sit_call_get_current_calls_rsp_v1_6;

/*
    SIT_DIAL (RCM ID = 0x0001)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE call_type; // sit_call_call_type_e_type
    BYTE emergency_call_subtype;
    BYTE num_len;
    BYTE num[ MAX_DIAL_NUM ];
    BYTE num_type;
    BYTE num_plan;
    BYTE clir;
    BYTE cug_call;
    BYTE cug_info_index;
    BYTE cug_info;
    BYTE ecall_ui;
}sit_call_dial_req;

typedef null_data_format sit_call_dial_rsp;

/*
    SIT_GET_LAST_CALL_FAIL_CAUSE (RCM ID = 0x0002)
*/
typedef null_data_format sit_call_get_last_call_fail_cause_req ;

typedef struct
{
    RCM_HEADER hdr;
    INT32 last_call_fail_cause; // sit_call_last_call_fail_cause_e_type
}sit_call_get_last_call_fail_cause_rsp;

/*
    SIT_DTMF (RCM ID =0x0003)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE dtmf_len;
    BYTE dtmf_digit[MAX_DTMF_LEN];
}sit_call_dtmf_req ;

typedef null_data_format sit_call_dtmf_rsp;

/*
    SIT_ANSWER (RCM ID =0x0004)
*/
typedef null_data_format sit_call_answer_req ;
typedef null_data_format sit_call_answer_rsp ;


/*
    SIT_DTMF_START (RCM ID =0x0005)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE tone_type; // sit_call_dtmf_local_tone_e_type
    BYTE tone_len; // sit_call_dtmf_tone_duration_e_type
    BYTE dtmf_digit;
}sit_call_dtmf_start_req ;

/*
    SIT_DTMF_STOP (RCM ID =0x0006)

*/
typedef null_data_format sit_call_dtmf_stop_req ;
typedef null_data_format sit_call_dtmf_stop_rsp ;

/*
    SIT_SEND_EXPLICIT_CALL_TRANSFER (RCM ID =0x0007)
*/
typedef null_data_format sit_call_explicit_call_transfer_req ;
typedef null_data_format sit_call_explicit_call_transfer_rsp ;


/*
    SIT_HANGUP (RCM ID =0x0008)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 index;
    INT32 call_type;
}sit_call_hangup_req ;

typedef enum
{
    SIT_CALL_HANGUP_CALL_TYPE_SINGLE = 0x01,
    SIT_CALL_HANGUP_CALL_TYPE_MULTI,

    SIT_CALL_HANGUP_CALL_TYPE_MAX
}sit_call_hangup_call_type_e_type;

typedef null_data_format sit_call_hangup_rsp;

/*
    SIT_IND_CALL_STATE_CHANGED (RCM ID =0x0009)
*/
typedef null_ind_data_format sit_call_call_state_changed_ind;


/*
    SIT_IND_CALL_RING (RCM ID =0x000A)
*/
typedef null_ind_data_format sit_call_call_ring_ind;

/*
    SIT_IND_EMERGENCY_CALL_LIST (RCM ID =0x000D)
*/

typedef struct
{
    BYTE category;
    BYTE emc_number_len;
    BYTE emc_number[MAX_EMERGENCY_NUMBER_LEN]; //32
}sit_call_emergency_call_number_info_v1_1;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE mccmnc[6];
    BYTE num;
    sit_call_emergency_call_number_info_v1_1 number_info[MAX_EMERGENCY_CALL_NUM_V1_1]; //10
}sit_call_emergency_call_list_ind_v1_1;

typedef struct
{
    BYTE category;
    BYTE emc_number_len;
    BYTE emc_number[MAX_EMERGENCY_NUMBER_LEN]; //32
    BYTE source;
}sit_call_emergency_call_number_info_v1_4;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE mcc[MAX_MCC_LEN];
    BYTE mnc[MAX_MNC_LEN];
    BYTE num;
    sit_call_emergency_call_number_info_v1_4 number_info[MAX_EMERGENCY_CALL_NUM_V1_4]; //20
}sit_call_emergency_call_list_ind_v1_4;

typedef struct {
    BYTE emc_number_len;
    BYTE emc_number[MAX_EENL_EMERGENCY_NUMBER_LEN];  // 20
    BYTE sub_service_len;
    BYTE sub_service[MAX_SUB_SERVICE_LEN];  // 255
} sit_call_emergency_call_eenl_record;

typedef struct {
    BYTE eenlv;
    BYTE eenlNum;
    BYTE eenl_records[sizeof(sit_call_emergency_call_eenl_record) * MAX_EENL_RECORD_NUM];
} sit_call_emergency_call_eenl;

typedef struct {
    RCM_IND_HEADER hdr;
    BYTE mcc[MAX_MCC_LEN];
    BYTE mnc[MAX_MNC_LEN];
    BYTE enlNum;
    sit_call_emergency_call_number_info_v1_4 enl_records[MAX_ENL_RECORD_NUM];  // 20
    sit_call_emergency_call_eenl eenl;
    //sit_call_emergency_call_eenl_record eenl_records[MAX_EENL_RECORD_NUM];  // 50
} sit_call_emergency_call_list_ind_v1_6;

/*
    SIT_IND_ENTER_EMERGENCY_CB_MODE (RCM ID = 0x0014)
*/
typedef null_ind_data_format sit_call_enter_emergency_cb_ind;

/*
    SIT_IND_EXIT_EMERGENCY_CB_MODE (RCM ID = 0x0019)
*/
typedef null_ind_data_format sit_call_exit_emergency_cb_ind;

/*
    SIT_EXIT_EMERGENCY_CB_MODE (RCM ID = 0x001A)
*/
typedef null_data_format sit_call_exit_emergency_cb_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result; // 0x00: Fail, 0x01: Success
}sit_call_exit_emergency_cb_mode_rsp;

/*
    SIT_SEND_SMS (RCM ID = 0x0100)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE sms_domain;
    BYTE smsc_len;
    BYTE smsc[MAX_GSM_SMS_SERVICE_CENTER_ADDR];
    BYTE sms_len;
    BYTE sms_data[MAX_GSM_SMS_TPDU_SIZE];
}sit_sms_send_sms_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 msg_ref;
    BYTE ack_pdu_len;
    BYTE ack_pdu[MAX_GSM_SMS_TPDU_SIZE];
    INT32 error_code;
}sit_sms_send_sms_rsp;

/*
    SIT_SEND_EXPECT_MORE (RCM ID = 0x0101)
*/
// same as SIT_SEND_SMS

/*
    SIT_SEND_SMS_ACK (RCM ID = 0x0102)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 result;
    BYTE msg_tpid;
    INT32 error_code;
}sit_sms_send_sms_ack_req;

typedef null_data_format sit_sms_send_sms_ack_rsp;

/*
    SIT_WRITE_SMS_TO_SIM (RCM ID = 0x0103)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 status;
    INT32 index;
    BYTE pdu_len;
    BYTE pdu_data[MAX_GSM_SMS_TPDU_SIZE];
}sit_sms_write_sms_to_sim_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 index;
}sit_sms_write_sms_to_sim_rsp;

/*
    SIT_DELETE_SMS_ON_SIM (RCM ID = 0x0104)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 index;
}sit_sms_delete_sms_on_sim_req;

typedef null_data_format sit_sms_delete_sms_on_sim_rsp;

/*
    SIT_GET_BCST_SMS_CFG (RCM ID = 0x0105)
*/
typedef null_data_format sit_sms_get_bcst_sms_cfg_req;

typedef struct
{
    INT16 from_svc_id;
    INT16 to_svc_id;
    BYTE from_code_scheme;
    BYTE to_code_scheme;
    BYTE selected;
}sit_sms_bcst_sms_cfg_item;

typedef struct
{
    RCM_HEADER hdr;
    BYTE bcst_info_num;
    sit_sms_bcst_sms_cfg_item cfgitem[MAX_BCST_INFO_NUM];
}sit_sms_get_bcst_sms_cfg_rsp;

/*
    SIT_SET_BCST_SMS_CFG (RCM ID = 0x0106)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE bcst_info_num;
    sit_sms_bcst_sms_cfg_item cfgitem[MAX_BCST_INFO_NUM];
}sit_sms_set_bcst_sms_cfg_req;

typedef null_data_format sit_sms_set_bcst_sms_cfg_rsp;

/*
    SIT_ACT_BCST_SMS (RCM ID = 0x0107)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 bcst_act;
}sit_sms_act_bcst_sms_req;

typedef null_data_format sit_sms_act_bcst_sms_rsp;

/*
    SIT_GET_SMSC_ADDR (RCM ID = 0x0108)
*/
typedef null_data_format sit_sms_get_smsc_addr_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE sca_len;
    BYTE sca[MAX_GSM_SMS_SERVICE_CENTER_ADDR];
}sit_sms_get_smsc_addr_rsp;

/*
    SIT_SET_SMSC_ADDR (RCM ID = 0x0109)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE sca_len;
    BYTE sca[MAX_GSM_SMS_SERVICE_CENTER_ADDR];
}sit_sms_set_smsc_addr_req;

typedef null_data_format sit_sms_set_smsc_addr_rsp;


/*
    SIT_SEND_SMS_MEM_STATUS (RCM ID = 0x010A)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 mem_status;
}sit_sms_send_sms_mem_status_req;

typedef null_data_format sit_sms_send_sms_mem_status_rsp;

/*
    SIT_SEND_ACK_INCOMING_SMS (RCM ID = 0x010B)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 result;
    BYTE msg_tpid;
    BYTE tpdu_len;
    BYTE tpdu[MAX_GSM_SMS_TPDU_SIZE];
}sit_sms_send_ack_incoming_sms_req;

typedef null_data_format sit_sms_send_ack_incoming_sms_rsp;

/*
    SIT_IND_NEW_SMS (RCM ID = 0x010C)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE msg_tpid;
    BYTE tpdu_len;
    BYTE tpdu[MAX_GSM_SMS_TPDU_SIZE];
}sit_sms_new_sms_ind;

/*
    SIT_IND_NEW_SMS_STATUS_REPORT (RCM ID = 0x010D), This seems to be able to be replaced with the sit_sms_new_sms_ind above later.
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE msg_tpid;
    BYTE tpdu_len;
    BYTE tpdu[MAX_GSM_SMS_TPDU_SIZE];
}sit_sms_new_sms_status_report_ind;


/*
    SIT_IND_NEW_SMS_ON_SIM (RCM ID = 0x010E)
*/
// not in use as per CP
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 index;
}sit_sms_new_sms_on_sim_ind;

/*
SIT_IND_SIM_SMS_STORAGE_FULL (RCM ID = 0x010F)
*/
typedef null_ind_data_format sit_sms_sim_sms_storage_full_ind;

/*
    SIT_IND_NEW_BCST_SMS (RCM ID = 0x0110)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    UINT16 bcst_msg_len;
    BYTE bcst_msg[MAX_BCST_MSG_LEN];
}sit_sms_new_bcst_sms_ind;

/*
    SIT_GET_STORED_SMS_COUNT (RCM ID = 0x0121)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE sim_id;
}sit_sms_get_stored_sms_count_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE sim_id;
    INT16 total_num;
    INT16 used_num;
}sit_sms_get_stored_sms_count_rsp;

/*
    SIT_GET_SIM_STATUS (RCM ID = 0x0200)
*/
typedef null_data_format sit_sim_get_sim_status_req;

typedef struct
{
    BYTE apps_type;
    BYTE apps_state;
    BYTE perso_substate;
    BYTE aid_len;
    BYTE AID[MAX_SIM_AID_LEN];
    BYTE app_label_len;
    BYTE app_label[MAX_SIM_APP_LABEL_LEN];
    INT32 pin1_replaced;
    BYTE pin1_state;
    BYTE pin2_state;
    BYTE pin1_remain_count;
    BYTE puk1_remain_count;
    BYTE pin2_remain_count;
    BYTE puk2_remain_count;
}sit_sim_apps_status_info;

typedef struct
{
    RCM_HEADER hdr;
    BYTE card_state;
    BYTE universal_pin_state;
    BYTE application_num;
    sit_sim_apps_status_info apps_status_info[MAX_SIM_APPS_INFO_COUNT];
}sit_sim_get_sim_status_rsp;

typedef struct
{
    BYTE esim_no_profile;
    BYTE physical_slot_id;
    BYTE atr_length;
    BYTE atr[MAX_ATR_LEN];
    BYTE iccid_length;
    BYTE iccid[MAX_ICCID_LEN];
    BYTE eid_length;
    BYTE eid[MAX_EID_LEN];
    BYTE port_id;
}sit_sim_get_sim_status_rsp_ext;

/*
    SIT_VERIFY_SIM_PIN (RCM ID = 0x0201)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE pin_len;
    BYTE pin[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_verify_sim_pin_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_verify_sim_pin_rsp;


/*
    SIT_VERIFY_SIM_PUK (RCM ID = 0x0202)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE puk_len;
    BYTE puk[MAX_SIM_PUK_LEN];
    BYTE new_pin_len;
    BYTE new_pin[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_verify_sim_puk_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_verify_sim_puk_rsp;
*/

// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_verify_sim_puk_rsp;

/*
    SIT_VERIFY_SIM_PIN2 (RCM ID = 0x0203)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE pin2_len;
    BYTE pin2[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_verify_sim_pin2_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_verify_sim_pin2_rsp;
*/
// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_verify_sim_pin2_rsp;

/*
    SIT_VERIFY_SIM_PUK2 (RCM ID = 0x0204)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE puk2_len;
    BYTE puk2[MAX_SIM_PUK_LEN];
    BYTE new_pin2_len;
    BYTE new_pin2[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_verify_sim_puk2_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_verify_sim_puk2_rsp;
*/
// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_verify_sim_puk2_rsp;

/*
    SIT_CHG_SIM_PIN (RCM ID = 0x0205)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE fac;
    BYTE old_pin_len;
    BYTE old_pin[MAX_SIM_PIN_LEN];
    BYTE new_pin_len;
    BYTE new_pin[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_change_sim_pin_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_change_sim_pin_rsp;
*/
// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_change_sim_pin_rsp;

/*
    SIT_CHG_SIM_PIN2 (RCM ID = 0x0206)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE fac;
    BYTE old_pin2_len;
    BYTE old_pin2[MAX_SIM_PIN_LEN];
    BYTE new_pin2_len;
    BYTE new_pin2[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_change_sim_pin2_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_change_sim_pin2_rsp;
*/
// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_change_sim_pin2_rsp;

/*
    SIT_VERIFY_NETWORK_LOCK  (RCM ID = 0x0207)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE fac;
    BYTE password_len;
    BYTE password[MAX_SIM_FACILITY_PASSWORD_LEN];
    BYTE service_class;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_verify_network_lock_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_verify_network_lock_rsp;
*/
// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_verify_network_lock_rsp;

/*
    SIT_SIM_IO (RCM ID = 0x0208)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE command;
    BYTE app_type;
    INT16 file_id;
    BYTE path_len;
    BYTE path[MAX_SIM_IO_PATH_LEN];
    BYTE p1;
    BYTE p2;
    BYTE p3;
    WORD data_len;
    BYTE data[MAX_SIM_IO_DATA_LEN];
    BYTE pin2_len;
    BYTE pin2[MAX_SIM_PIN_LEN];
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_sim_io_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE sw1;
    BYTE sw2;
    INT16 response_len;
    BYTE response[MAX_SIM_IO_DATA_LEN];
}sit_sim_sim_io_rsp;

/*
    SIT_GET_FACILITY_LOCK (RCM ID = 0x0209)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE code;
    BYTE password_len;
    BYTE password[MAX_SIM_FACILITY_PASSWORD_LEN];
    BYTE service_class;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_get_facility_lock_req;

typedef struct
{
    RCM_HEADER hdr;
    //INT32 service_class;
    BYTE lock_mode;  // NOT USED by f/w anymore
    BYTE service_class;
}sit_sim_get_facility_lock_rsp;

/*
    SIT_SET_FACILITY_LOCK  (RCM ID = 0x020A)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE code;
    BYTE lock_mode;
    BYTE password_len;
    BYTE password[MAX_SIM_FACILITY_PASSWORD_LEN];
    BYTE service_class;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_set_facility_lock_req;

/*
typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
}sit_sim_set_facility_lock_rsp;
*/
// same as SIT_VERIFY_SIM_PIN
typedef sit_sim_verify_sim_pin_rsp sit_sim_set_facility_lock_rsp;

/*
    SIT_GET_SIM_AUTH  (RCM ID = 0x020B)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE auth_type;
    BYTE auth_len;
    BYTE auth[MAX_SIM_AUTH_REQ_LEN];
    /*
     * IMS Auth : RAND_LEN(1) + RAND(V) + AUTN_LEN(1) + AUTH(V)
     * GSM Auth : RAND_LEN(1) + RAND(V)
     * 3G Auth : RAND_LEN(1) + RAND(V) + AUTN_LEN(1) + AUTH(V)
     */
}sit_sim_get_sim_auth_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE auth_type;
    BYTE auth_len;
    BYTE auth[MAX_SIM_AUTH_RSP_LEN];
    /*
     * Result(1) + Auth Response
     * IMS Auth : RES_AUTS_len(1) + RES_AUTS(V) + CK_len(1) + CK(V) + IK_len(1) + IK(V) + KC_len(1) + KC(V)
     * GSM Auth : SRES_len(1) + SRES(V) + Kc_len(1) + Kc(V)
     * 3G Auth : RES_AUTS_len(1) + RES_AUTS(V) + CK_len(1) + CK(V) + IK_len(1) + IK(V) + KC_len(1) + KC(V)
     */
}sit_sim_get_sim_auth_rsp;

/*
    SIT_GET_GBA_CONTEXT  (RCM ID = 0x0211)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE auth_type;
    BYTE gba_type;
    BYTE gba_tag;
    BYTE data1_len;
    BYTE data1[MAX_GBA_AUTH_REQ_DATA_LEN];
    BYTE data2_len;
    BYTE data2[MAX_GBA_AUTH_REQ_DATA_LEN];
}sit_sim_get_sim_gba_auth_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE auth[MAX_SIM_GBA_AUTH_RSP_LEN];
}sit_sim_get_sim_gba_auth_rsp;

typedef struct
{
    BYTE cla;
    BYTE instruction;
    BYTE p1;
    BYTE p2;
    BYTE p3;
    BYTE data[0];
}sit_sim_apdu;

/*
    SIT_TRANSMIT_SIM_APDU_BASIC (RCM ID = 0x020C)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 session_id;
    UINT16 apdu_len;
    union {
        sit_sim_apdu sim_apdu;
        BYTE apdu[0];
    } entry;
}sit_sim_transmit_sim_apdu_basic_req;

typedef struct
{
    RCM_HEADER hdr;
    WORD apdu_len;
    BYTE apdu[0];      // MAX_APDU_LEN is 16K
}sit_sim_transmit_sim_apdu_basic_rsp;

/*
    SIT_OPEN_SIM_CHANNEL (RCM ID = 0x020D)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_sim_open_sim_channel_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 session_id;
    BYTE sw1;
    BYTE sw2;
    INT16 response_len;
    BYTE response[MAX_OPEN_CHANNEL_RSP_LEN];
}sit_sim_open_channel_rsp;


/*
    SIT_CLOSE_SIM_CHANNEL (RCM ID = 0x020E)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 session_id;
}sit_sim_close_sim_channel_req;

typedef null_data_format sit_sim_close_sim_channel_rsp;


/*
    SIT_TRANSMIT_SIM_APDU_CHANNEL (RCM ID = 0x020F)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 session_id;
    INT32 cla;
    INT32 instruction;
    INT32 p1;
    INT32 p2;
    INT32 p3;
    UINT16 data_len;
    BYTE data[0];
}sit_sim_transmit_sim_apdu_channel_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE sw1;
    BYTE sw2;
    INT16 response_len;
    BYTE response[0];
}sit_sim_transmit_sim_apdu_channel_rsp;


/*
    SIT_IND_SIM_STATUS_CHANGED (RCM ID = 0x0210)
*/
typedef null_ind_data_format sit_sim_sim_status_changed_ind;


/* PhoneBook */

/*
    SIT_READ_PB_ENTRY (RCM_ID = 0x0240)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE pb_type;
    UINT16 recordId;
}sit_read_pb_entry_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE pb_type;
    UINT16 recordId;
    UINT16 data_len;
    char entry_data[MAX_PB_ENTRY_LEN];
}sit_read_pb_resp;

/*
    SIT_UPDATE_PB_ENTRY (RCM_ID = 0x0241)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
    BYTE pb_type;
    UINT16 recordId;
    UINT16 entry_len;
    BYTE entry[MAX_PB_ENTRY_LEN];
} sit_update_pb_entry_req;

typedef struct{
    RCM_HEADER hdr;
    BYTE mode;
    BYTE pb_type;
    UINT16 recordId;
} sit_update_pb_entry_resp;

/*
    SIT_GET_PB_STORAGE_INFO (RCM_ID = 0x0242)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE pb_type;
}sit_sim_pb_storage_info;

typedef struct {
    RCM_HEADER hdr;
    BYTE pb_type;
    UINT16 total_count;
    UINT16 used_count;
}sit_sim_pb_storage_info_rsp;

/*
    SIT_GET_PB_STORAGE_LIST (RCM_ID = 0x0243)
*/
typedef struct {
    RCM_HEADER hdr;
    int pb_list;
}sit_sim_pb_storage_list_rsp;

/*
    SIT_GET_3G_PB_CAPA (RCM_ID = 0x0245)
*/
typedef struct {
    BYTE pb_type;
    UINT16 index_max;
    UINT16 entry_max;
    UINT16 used_count;
}sit_sim_pb_capa;

typedef struct
{
    RCM_HEADER hdr;
    BYTE entry_num;
    sit_sim_pb_capa pb_list[MAX_PB_ENTRY_NUM];
}sit_sim_pb_capa_rsp;

/*
    SIT_SIM_IND_PB_READY (RCM ID = 0x0246)
*/
typedef struct {
    RCM_IND_HEADER hdr;
    BYTE pb_ready;
}sit_sim_pb_ready_ind;

/*
    SIT_OPEN_SIM_CHANNEL_WITH_P2 (RCM ID = 0x0247)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
    BYTE p2;
}sit_sim_open_sim_channel_with_p2_req;

/*
    SIT_SET_UICC_SUBSCRIPTION (RCM ID = 0x0249)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE state;    // 0: deactivated, 1: activated which are same to RIL_UiccSubActStatus
}sit_sim_set_uicc_sub_req;

/*
    SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED (RCM ID = 0x024A)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 state;    // 0: deactivated, 1: activated which are same to RIL_UiccSubActStatus
}sit_sim_uicc_sub_state_changed_ind;

/*
    SIT_SET_SIM_CARD_POWER (RCM ID = 0x024C)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;    // 0: UP, 1: DOWON
}sit_sim_set_sim_card_power_req;

/*
    SIT_GET_SLOT_STATUS (RCM ID = 0x024D)
*/
typedef struct
{
    BYTE card_state;
    BYTE slot_State;
    BYTE atr_len;
    BYTE atr[MAX_ATR_LEN];
    BYTE logicalSlotId;
    BYTE iccid_len;
    BYTE iccid[MAX_ICCID_LEN];
    BYTE eid_len;           // for RadioConfigV1.2
    BYTE eid[MAX_EID_LEN];  // for RadioConfigV1.2
} sit_sim_get_slot_status_info_v1_2;

typedef struct
{
    BYTE iccid_len;
    BYTE iccid[MAX_ICCID_LEN];
    BYTE logicalSlotId;
    BYTE port_state;
} PORT_INFO;

typedef struct
{
    BYTE card_state;
    BYTE atr_len;
    BYTE atr[MAX_ATR_LEN];
    BYTE eid_len;
    BYTE eid[MAX_EID_LEN];
    BYTE port_len;
    PORT_INFO port_info[MAX_PORT_NUM];
} sit_sim_get_slot_status_info;

typedef struct
{
    RCM_HEADER hdr;
    BYTE num_of_info;
    sit_sim_get_slot_status_info_v1_2 info[MAX_SLOT_NUM];
} sit_sim_get_slot_status_rsp_v1_2;

typedef struct
{
    RCM_HEADER hdr;
    BYTE num_of_info;
    sit_sim_get_slot_status_info info[MAX_SLOT_NUM];
} sit_sim_get_slot_status_rsp;

/*
    SIT_IND_SIM_SLOT_STATUS_CHANGED (RCM ID = 0x024E)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE num_of_info;
    sit_sim_get_slot_status_info_v1_2 info[MAX_SLOT_NUM];
} sit_sim_slot_status_changed_ind_v1_2;


typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE num_of_info;
    sit_sim_get_slot_status_info info[MAX_SLOT_NUM];
} sit_sim_slot_status_changed_ind;

/*
   SIT_IND_SIM_DATA_INFO (RCM ID = 0x024F)
 */
typedef struct {
    RCM_IND_HEADER hdr;
    UINT16 sim_file_id;
    UINT16 record_len;
    BYTE num_of_records;
    BYTE data_info[2];    // size is variant.
}sit_sim_file_data_info_ind;

/*
    SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING (RCM ID = 0x0250)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE slotMaplen;
    BYTE slotMap[MAX_SLOT_NUM];
} sit_sim_set_logical_to_physical_slot_mapping_req_v1_2;

typedef struct
{
    BYTE physicalSlotId;
    BYTE portId;
} SLOT_PORT_MAPPING;

typedef struct
{
    RCM_HEADER hdr;
    BYTE slotMaplen;
    SLOT_PORT_MAPPING slotMap[MAX_SLOT_NUM];
} sit_sim_set_logical_to_physical_slot_mapping_req;

/*
    SIT_IND_SIM_TRAY_STATUS_CHANGED (RCM ID = 0x0251)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE state;    // 0:unknown, 1: inserted, 2: removed
} sit_sim_tray_status_changed_ind;

/*
    SIT_SEND_STK_ENVELOPE_CMD (RCM ID = 0x0300)
*/
typedef struct
{
    RCM_HEADER hdr;
    WORD envelope_cmd_len;
    BYTE envelope_cmd[MAX_SIM_IO_DATA_LEN];
}sit_stk_send_stk_envelope_cmd_req;

typedef struct
{
    RCM_HEADER hdr;
    WORD envelope_rsp_len;
    BYTE envelope_rsp[MAX_SIM_IO_DATA_LEN];
}sit_stk_send_stk_envelope_cmd_rsp;

/*
    SIT_SEND_STK_TERMINAL_RSP (RCM ID = 0x0301)
*/
typedef struct
{
    RCM_HEADER hdr;
    WORD terminal_rsp_len;
    BYTE terminal_rsp[MAX_SIM_IO_DATA_LEN];
}sit_stk_send_stk_terminal_rsp_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE sw1;
    BYTE sw2;
}sit_stk_send_stk_terminal_rsp_rsp;

/*
    SIT_SEND_STK_ENVELOPE_WITH_STATUS (RCM ID = 0x0302)
*/
typedef struct
{
    RCM_HEADER hdr;
    WORD envelope_cmd_len;
    BYTE envelope_cmd[MAX_SIM_IO_DATA_LEN];
}sit_stk_send_stk_envelope_with_status_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE sw1;
    BYTE sw2;
    WORD envelope_rsp_len;
    BYTE envelope_rsp[MAX_SIM_IO_DATA_LEN];
}sit_stk_send_stk_envelope_with_status_rsp;

/*
    SIT_IND_STK_PROACTIVE_COMMAND (RCM ID = 0x0303)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    WORD proactive_cmd_len;
    BYTE proactive_cmd[MAX_SIM_IO_DATA_LEN];
}sit_stk_stk_proactive_cmd_ind;

/*
    SIT_IND_SIM_REFRESH (RCM ID = 0x0304)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE result;
}sit_stk_sim_refresh_ind;

/*
    SIT_STK_CALL_SETUP (RCM ID = 0x0305)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE user_operation;
}sit_stk_call_setup_req;

typedef null_data_format sit_stk_call_setup_rsp;

typedef enum
{
    SIT_STK_CALL_SETUP_REJECT = 0x00,
    SIT_STK_CALL_SETUP_ACCEPT
}sit_stk_setup_call_user_operation_e_type;

/*
    SIT_IND_STK_CC_ALPHA_NOTIFY (RCM ID = 0x0307)
*/
// IPC NOT IN USE as per CP JSON document
typedef struct {
    RCM_IND_HEADER hdr;
    BYTE alpha_len;
    BYTE alpha_buf[MAX_ALPHA_INFO_BUF_LEN];
}sit_stk_cc_alpha_notify_ind;

/*
    SIT_GET_IMSI (RCM ID = 0x0400)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
}sit_id_get_imsi_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE imsi_len;
    BYTE imsi[MAX_IMSI_LEN];
}sit_id_get_imsi_rp;

/*
    SIT_GET_IMEI (RCM ID = 0x0401)
*/
typedef null_data_format sit_id_get_imei_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE imei_len;
    BYTE imei[MAX_IMEI_LEN];
}sit_id_get_imei_rsp;


/*
    SIT_GET_IMEISV (RCM ID = 0x0402)
*/
typedef null_data_format sit_id_get_imeisv_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE imeisv_len;
    BYTE imeisv[MAX_IMEISV_LEN];
}sit_id_get_imeisv_rsp;

/*
    SIT_GET_DEVICE_ID (RCM ID = 0x0403)
*/
typedef null_data_format sit_id_get_deviceid_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE imei_len;
    BYTE imei[MAX_IMEI_LEN];
    BYTE imeisv_len;
    BYTE imesv[MAX_IMEISV_LEN];
    BYTE esn_len;
    BYTE esn[MAX_ESN_LEN];
    BYTE meid_len;
    BYTE meid[MAX_MEID_LEN];
}sit_id_get_deviceid_rsp;

/*
    SIT_SET_SGC (RCM ID = 0x0404)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT32 SGC; // sit_id_sgc_op_idx_e_type
    UINT32 Rsv1;
    UINT32 Rsv2;
}sit_id_set_sgc_req;

typedef null_data_format sit_id_set_sgc_rsp;


/*
    SIT_SET_CARRIER_INFO_IMSI_ENCRYPTION (RCM ID = 0x0406)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mcc[MAX_MCC_LEN];
    BYTE mnc[MAX_MNC_LEN];
    INT32 carrier_len;
    BYTE carrier_key[MAX_IMSI_ENCRIPTION_KEY_LEN];
    BYTE key_id_len;
    BYTE key_id[MAX_IMSI_ENCRIPTION_KEY_LEN];
    LONG expire_time;
    INT32 public_key_type;
}sit_id_set_carrier_info_imsi_encription_req;

typedef null_data_format sit_id_set_carrier_info_imsi_encription_rsp;

/*
    SIT_IND_CARRIER_INFO_IMSI_ENCRYPTION (RCM ID = 0x0407)
*/
typedef null_ind_data_format sit_id_carrier_info_imsi_encription_ind;

/*
 * SIT_SET_CARRIER_CONFIG_MANIFEST (RCMID = 0x0408)
 */
#define MAX_CONFPACK_LEN 256
#define MAX_MANIFEST_LEN 256
typedef struct
{
    RCM_HEADER hdr;
    int confpack_path_len;
    char confpack_path[MAX_CONFPACK_LEN];
    int manifest_len;
    char manifest_path[MAX_MANIFEST_LEN];
}sit_set_carrier_config_manifest_req;

/*
 * SIT_IND_CARRIER_CONFIG_STATUS (RCMID = 0x0409)
 */
enum {
    SIT_CARRIER_CONFIG_STATUS_NOT_UPDATED, // Not updatd (already applied it same configurations)
    SIT_CARRIER_CONFIG_STATUS_UPDATED,     // configurations updated
};

enum {
    SIT_CARRIER_CONFIG_CAUSE_NONE = 0,
    SIT_CARRIER_CONFIG_CAUSE_TCS_FAIL = 1,
    SIT_CARRIER_CONFIG_CAUSE_TIMER_EXPIRED = 2,
};

typedef struct
{
    RCM_IND_HEADER hdr;
    uint32_t status;
    uint32_t cause;
}sit_carrier_config_status_ind;

/*
 * SIT_IND_CARRIER_CONFIG_SIM_INFO (RCMID = 0x0410)
 */
#define SIM_ICCID_ASCII_LEN     /*USIM_ICCID_LEN=*/10 * 2 + 1
#define SIM_IMSI_ASCII_LEN      /*SIT_ID_IMSI_LENGTH=*/16
#define SIM_SPN_ASCII_LEN       /*SIM_SPN_DATA_LEN=*/16
#define SIM_PNN_ASCII_LEN       /*SIT_MAX_OPERATOR_LONG_NAME_LEN=*/64
#define SIM_GID_ASCII_LEN       /*USIM_GID_LEN=*/8 * 2 + 1
#define PRIVILEGE_ACCESS_RULES_LEN 25

typedef struct
{
    RCM_IND_HEADER hdr;
    UINT32 simInfoLen;
    BYTE iccid[SIM_ICCID_ASCII_LEN];
    BYTE imsi[SIM_IMSI_ASCII_LEN];  // USIM_IMSI
    BYTE imsiM[SIM_IMSI_ASCII_LEN]; // CSIM_IMSI
    BYTE mncLen;
    BYTE spn[SIM_SPN_ASCII_LEN];
    BYTE pnn[SIM_PNN_ASCII_LEN];
    BYTE gid1[SIM_GID_ASCII_LEN];
    BYTE gid2[SIM_GID_ASCII_LEN];
    UINT16 privilegeAccessRulesLen;
    BYTE privilegeAccessRules[PRIVILEGE_ACCESS_RULES_LEN];
}sit_ind_carrier_config_sim_info;

/*
    SIT_HANGUP_WAITING_OR_BACKGROUND (RCM ID =0x0500)
*/
typedef null_data_format sit_ss_hangup_waiting_or_background_req;
typedef null_data_format sit_ss_hangup_waiting_or_background_rsp;

/*
    SIT_HANGUP_FORGROUND_RESUME_BACKGROUND (RCM ID =0x0501)
*/
typedef null_data_format sit_ss_hangup_foreground_resume_background_req;
typedef null_data_format sit_ss_hangup_foreground_resume_background_rsp;

/*
    SIT_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE (RCM ID =0x0502)
*/
typedef null_data_format sit_ss_switch_waiting_or_holding_and_active_req;
typedef null_data_format sit_ss_switch_waiting_or_holding_and_active_rsp;

/*
    SIT_CONFERENCE (RCM ID =0x0503)
*/
typedef null_data_format sit_ss_conference_req;
typedef null_data_format sit_ss_conference_rsp;

/*
    SIT_UDUB (RCM ID =0x0504)
*/
typedef null_data_format sit_ss_udub_req;
typedef null_data_format sit_ss_udub_rsp;

/*
    SIT_SEND_USSD (RCM ID =0x0505)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE ussd_type;
    BYTE dcs;
    BYTE ussd_len;
    BYTE ussd[MAX_USSD_DATA_LEN];
}sit_ss_send_ussd_req;

typedef null_data_format sit_ss_send_ussd_rsp;

/*
    SIT_CANCEL_USSD (RCM ID =0x0506)
*/
typedef null_data_format sit_ss_cancel_ussd_req;
typedef null_data_format sit_ss_cancel_ussd_rsp;

/*
    SIT_GET_CLIR (RCM ID =0x0507)
*/
typedef null_data_format sit_ss_get_clir_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 clir_status;    // INT->INT32 for compile, rkkwon 20140701
}sit_ss_get_clir_rsp;

/*
    SIT_GET_COLP (RCM ID =0x0508)
*/
typedef null_data_format sit_ss_get_colp_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 status;
}sit_ss_get_colp_rsp;

/*
    SIT_GET_CALL_FORWARD_STATUS (RCM ID =0x0509)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT32 status;
    INT32 reason;
    INT32 service_class;
    INT32 toa;
    BYTE num_len;
    BYTE number[MAX_SS_NUM_LEN];
    INT32 timeseconds;
}sit_ss_get_call_forward_status_req;

typedef struct
{
    INT32 status;
    INT32 reason;
    INT32 service_class;
    INT32 toa;
    BYTE num_len;
    BYTE number[MAX_SS_NUM_LEN];
    INT32 timeseconds;
}sit_ss_call_forward_item;

typedef struct
{
    RCM_HEADER hdr;
    INT32 call_forward_num;
    sit_ss_call_forward_item record[MAX_CALL_FORWARD_STATUS_NUM];
}sit_ss_get_call_forward_status_rsp;

//refer sit_ss_call_forward_status_e_type in set request

/*
    SIT_SET_CALL_FORWARD (RCM ID =0x050A)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 status;
    INT32 reason;
    INT32 service_class;
    INT32 toa;
    BYTE num_len;
    BYTE number[MAX_SS_NUM_LEN];
    INT32 timeseconds;
}sit_ss_set_call_forward_req;

typedef null_data_format sit_ss_set_call_forward_rsp;

/*
    SIT_GET_CALL_WAITING (RCM ID =0x050B)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 service_class;
}sit_ss_get_call_waiting_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 status;
    INT32 service_class;
}sit_ss_get_call_waiting_rsp;

/*
    SIT_SET_CALL_WAITING (RCM ID =0x050C)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 status;
    INT32 service_class;
}sit_ss_set_call_waiting_req;

typedef null_data_format sit_ss_set_call_waiting_rsp;

/*
    SIT_CHG_BARRING_PWD (RCM ID =0x050D)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE pwd[4];
    BYTE new_pwd[4];
    BYTE new_pwd_again[4];
}sit_ss_change_barring_pwd_req;

typedef null_data_format sit_ss_change_barring_pwd_rsp;

/*
    SIT_SEPARATE_CONNECTION (RCM ID =0x050E)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 index;
}sit_ss_separate_connection_req;

typedef null_data_format sit_ss_separate_connection_rsp;

/*
    SIT_GET_CLIP (RCM ID =0x050F)
*/
typedef null_data_format sit_ss_get_clip_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE status;
}sit_ss_get_clip_rsp;

/*
    SIT_IND_ON_USSD (RCM ID =0x0510)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE status;
    BYTE dcs;
    BYTE ussd_len;
    BYTE ussd[MAX_USSD_DATA_LEN];
}sit_ss_ussd_ind;

/*
    SIT_IND_SUPP_SVC_NOTIFICATION (RCM ID =0x0511)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 noti_type;
    INT32 code;
    INT32 index;
    UINT type;
    BYTE num_len;
    BYTE num[MAX_SS_NUM_LEN];
}sit_ss_supp_svc_notification_ind;

/*
    SIT_GET_COLR (RCM ID =0x0512)
*/
typedef null_data_format sit_ss_get_colr_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 status;
}sit_ss_get_colr_rsp;

/*
    SIT_IND_SS_RETURN_RESULT (RCM ID =0x0513)
*/
typedef struct
{
    RCM_IND_HEADER hdr;    // [arkade] 2014/10/29 RCM_HEADER is changed to RCM_IND_HEADER
    BYTE return_result_len;
    BYTE return_result[MAX_SS_RETURN_RESULT];
}sit_ss_return_result_ind;

/*
    SIT_IND_ON_SS (RCM ID =0x0516)
*/
typedef struct
{
    INT32 call_forward_num;
    sit_ss_call_forward_item record[MAX_CALL_FORWARD_STATUS_NUM];
}sit_ss_cf_info_type;

typedef struct
{
    RCM_IND_HEADER hdr;    // [arkade] 2014/10/29 RCM_HEADER is changed to RCM_IND_HEADER
    BYTE service_type;    // same to RIL_SsServiceType
    BYTE request_type;    // same to RIL_SsRequestType
    BYTE teleservice_type;   // same to RIL_SsTeleserviceType
    INT32 service_class;    // same to sit_ss_service_class_e_type
    INT32 result;    // RCM error
    BYTE data_type;  // sit_ss_on_ss_ind_data_type
    union {
        INT32 ss_info[MAX_SS_INFO_NUM];
        sit_ss_cf_info_type cf_info;
    }data;
}sit_ss_on_ss_ind;

typedef enum
{
    SIT_ON_SS_IND_SS_INFO = 0,
    SIT_ON_SS_IND_CF_INFO = 1,
} sit_ss_on_ss_ind_data_type;

/*
    SIT_SETUP_DATA_CALL (RCM ID = 0x0600)
*/
typedef struct
{
    INT16 status;
    BYTE cid;
    BYTE active;
    BYTE pdp_type;
    BYTE address[MAX_PDP_ADDRESS_LEN];
    BYTE dns_type;
    BYTE primary_dns[MAX_PDP_ADDRESS_LEN];
    BYTE secondary_dns[MAX_PDP_ADDRESS_LEN];
    BYTE pcscf_type;
    BYTE pcscf[MAX_PCSCF_ADDRESS_LEN];
    INT32 ipv4_mtu_size; // Invalid: 0
    BYTE pco;       // Invalid: 0xFF
}sit_pdp_data_call_item;

/*
   SIT_SETUP_DATA_CALL Extension 1
   Previous Payload should be checked by Size.
   From This Extension First Two byte works as Extension header
   This extension block will be added on each DataCall in DATA_CALL_LIST_CHANGED_IND

   IPC_version (1) is reserved for old Header. Will ignore extension Bytes
   IPC_version (2) is used for pcscf extension only.
                  and first extension header should be pcscf_ext
   IPC_version (3) will be used in the future, this will use NextHeaderType
                  and first extension header should be pcscf_ext
   IPC_version > (4) is not used. But inevitably if policy should be changed,
                  these value can be used to use fixed payload format.

   Future Use
    NextHeaderType will show next Header Type.
    Reserved Type List
        (0) means no nextheader
        (1) means PCSCF extension header
*/

typedef struct
{
    BYTE IPC_version;
    BYTE NextHeaderType;
    BYTE pcscf_ipv4_ext[MAX_IPV4_ADDR_LEN * 5];
    BYTE pcscf_ipv6_ext[MAX_IPV6_ADDR_LEN * 5];
}sit_pdp_data_call_item_pcscf_ext;

typedef struct
{
    BYTE low_priority;
    int32_t suggestedRetryTime;
} sit_pdp_data_call_item_ext_lp_retrytime;

typedef struct
{
    BYTE low_priority;
    int64_t suggestedRetryTime;
} sit_pdp_data_call_item_ext_lp_retrytime_1_6;

/*
 * TS 24.301 9.9.4.2 APN aggregate maximum bit rate
 *
 * octet1 : APN aggregate maximum bit rate IEI code, it's decoded by CP.
 * octet2 : Length of APN aggreate maximum bit rate contents
 * octet3 : APN-AMBR for downlink
 * octet4 : APN-AMBR for uplink
 * octet5 : APN-AMBR for downlink (extended)
 * octet6 : APN-AMBR for uplink (extended)
 * octet7 : APN-AMBR for downlink (extended2)
 * octet8 : APN-AMBR for uplink (extended2)
 */
typedef struct
{
    BYTE octet2;
    BYTE octet3;
    BYTE octet4;
    BYTE octet5;
    BYTE octet6;
    BYTE octet7;
    BYTE octet8;
} sit_pdp_data_call_item_ext_ambr;

typedef struct {
    /** Maximum bit rate possible on the bearer */
    int32_t maxBitrateKbps;
    /** Minimum bit rate that is guaranteed to be provided by the network */
    int32_t guaranteedBitrateKbps;
} sit_pdp_QosBandwidth;

/** LTE/EPS Quality of Service parameters as per 3gpp spec 24.301 sec 9.9.4.3. */
typedef struct {
    /**
     * Quality of Service Class Identifier (QCI), see 3GPP TS 23.203 and 29.212.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t qci;
    sit_pdp_QosBandwidth downlink;
    sit_pdp_QosBandwidth uplink;
} sit_pdp_EpsQos;

/** 5G Quality of Service parameters as per 3gpp spec 24.501 sec 9.11.4.12 */
typedef struct {
    /**
     * 5G QOS Identifier (5QI), see 3GPP TS 24.501 and 23.501.
     * The allowed values are standard values(1-9, 65-68, 69-70, 75, 79-80, 82-85)
     * defined in the spec and operator specific values in the range 128-254.
     */
    uint16_t fiveQi;
    sit_pdp_QosBandwidth downlink;
    sit_pdp_QosBandwidth uplink;
    /**
     * QOS flow identifier of the QOS flow description in the
     * range of QosFlowIdRange::MIN to QosFlowIdRange::MAX
     */
    uint8_t qfi;
    uint16_t averagingWindowMs;
} sit_pdp_NrQos;

typedef union
{
    sit_pdp_EpsQos eps;
    sit_pdp_NrQos nr;
} sit_pdp_Qos;


typedef struct
{
    int32_t ipv6_mtu_size;
    int32_t pdu_session_id;
    BYTE defaultQosType;
    sit_pdp_Qos defaultQos;
} sit_pdp_default_Qos_param;

typedef struct
{
    BYTE optionalSliceInfoType;
    uint8_t optionalSliceInfoSst;
    int32_t optionalSliceInfoSd;
    uint8_t optionalSliceMappedHplmnSst;
    int32_t optionalSliceMappedHplmnSd;
} sit_pdp_optional_slice_info_param;

typedef struct
{
   uint32_t tdBitMask;
   BYTE dnn[MAX_PDP_APN_LEN];
   BYTE osAppID[255];
   BYTE osID[16];
}sit_pdp_traffic_descriptor_param;

// IPC Version 2 and later for SLICING_CONFIG
// will use variable field and length
// when each field has 0 length, next byte will be next field length

/*
   Old definition
*/
typedef struct
{
    INT16 status;
    BYTE cid;
    BYTE active;
    BYTE pdp_type;
    BYTE address[MAX_PDP_ADDRESS_LEN];
    BYTE primary_dns[MAX_PDP_ADDRESS_LEN];
    BYTE secondary_dns[MAX_PDP_ADDRESS_LEN];
    BYTE pcscf[MAX_PCSCF_ADDRESS_LEN];
}sit_pdp_data_call_item_old;

enum dc_packet_length {
 LEN_DC_V11_IND_HDR =   (sizeof(RCM_IND_HEADER) + sizeof(BYTE)),
 LEN_DC_V10 =           (sizeof(sit_pdp_data_call_item_old)),
 LEN_DC_V11 =           (sizeof(sit_pdp_data_call_item)),
 LEN_DC_V11_PCSCF_EXT = (LEN_DC_V11 + sizeof(sit_pdp_data_call_item_pcscf_ext)),
 LEN_DC_V11_LP_RETRY  = (LEN_DC_V11_PCSCF_EXT + sizeof(sit_pdp_data_call_item_ext_lp_retrytime)),
 LEN_DC_V11_AMBR      = (LEN_DC_V11_LP_RETRY + sizeof(sit_pdp_data_call_item_ext_ambr)),
 LEN_DC_V11_LP_RETRY_1_6 = (LEN_DC_V11_PCSCF_EXT + sizeof(sit_pdp_data_call_item_ext_lp_retrytime_1_6)),
 LEN_DC_V11_AMBR_1_6     = (LEN_DC_V11_LP_RETRY_1_6 + sizeof(sit_pdp_data_call_item_ext_ambr)),
 LEN_DC_V11_DEFAULT_QOS_PARAM      = (LEN_DC_V11_AMBR_1_6 + sizeof(sit_pdp_default_Qos_param)),
 LEN_DC_V11_SLICE_INFO      = (LEN_DC_V11_DEFAULT_QOS_PARAM + sizeof(sit_pdp_optional_slice_info_param)),
};

enum connection_capabilities {
    CC_IMS  = (1 << 0),
    CC_MMS  = (1 << 1),
    CC_SUPL = (1 << 2),
    CC_INTERNET = (1 << 3),
};

/*
   SIT_SETUP_DATA_CALL REQUEST Extension

   IPC_version (0) is R version Radio HAL 1.5
   IPC_version (1) is S version Radio HAL 1.6 PDU Session ID, and NW slice info
   IPC_version (2) is extended definition for Nw Slice to support wider features of standard
 */

struct sit_pdp_setup_data_call_req
{
    RCM_HEADER hdr;
    BYTE cid;
    BYTE rat;
    BYTE data_profile;
    BYTE apn_type;
    char apn[MAX_PDP_APN_LEN];
    char username[MAX_PDP_USER_NAME_LEN];
    char password[MAX_PDP_USER_NAME_LEN];
    BYTE auth_type;
    BYTE pdp_type;
    BYTE pcscf_addr_req;
    BYTE IPv4Address[MAX_IPV4_ADDR_LEN];
    BYTE IPv6Address[MAX_IPV6_ADDR_LEN];
    BYTE lowPriority;
    INT32 apnTypeBitMask;
    BYTE reqIpcVersion;
};

typedef struct
{
    uint32_t pduSessionId;
    BYTE optionalSliceInfoType;
    BYTE optionalSliceInfoSst;
    int32_t optionalSliceInfoSD;
    BYTE optionalSliceInfoMappedHplmnSst;
    int32_t optionalSliceInfoMappedHplmnSD;
}sit_pdp_optional_sliceInfo_req;

typedef struct
{
    uint32_t infoBitmask ;
    BYTE optionalDnn[101];
    BYTE optionalOsAppID[255];
    BYTE optionalOsID[16];
}sit_pdp_optional_trafficDesc_req;

struct sit_pdp_ip3tuple {
    BYTE bitmap;
    BYTE ipv4RemoteAddress[4];
    BYTE ipv4RemoteAddressMask[4];
    BYTE ipv6RemoteAddress[16];
    BYTE prefixLength;
    BYTE protocolIdentifier;
    BYTE singleRemotePort[2];
    BYTE remotePortRangeLowLimit[2];
    BYTE remotePortRangeHighLimit[2];
};

struct sit_pdp_optional_trafficDesc_ext_req {
    BYTE ipv4RemoteAddress[4];
    BYTE ipv4RemoteAddressMask[4];
    BYTE ipv6RemoteAddress[16];
    BYTE prefixLength;
    BYTE protocolIdentifier;
    BYTE singleRemotePort[2];
    BYTE remotePortRangeLowLimit[2];
    BYTE remotePortRangeHighLimit[2];
    BYTE SecurityParameterIndex[4];
    BYTE typeOfServiceTrafficClass;
    BYTE typeOfServiceTrafficClassMask;
    BYTE flowLabel[3];
    BYTE destinationMacAddress[6];
    BYTE cTagVid[2];
    BYTE sTagVid[2];
    BYTE cTagPcpDei;
    BYTE sTagPcpDei;
    BYTE etherType[2];
    BYTE connectionCapabilities;
    BYTE destinationFQDN[255];
    sit_pdp_ip3tuple ip3Tuple;
};

struct sit_pdp_setup_data_call_v2_req : sit_pdp_setup_data_call_req {
    sit_pdp_optional_sliceInfo_req sliceInfoData;
    BYTE matchAllRuleAllowed;
    sit_pdp_optional_trafficDesc_req tdData;
};

struct sit_pdp_setup_data_call_v3_req : sit_pdp_setup_data_call_v2_req {
    sit_pdp_optional_trafficDesc_ext_req tdExtData;
    uint8_t alwaysOnPdu;
};

typedef enum {
    SIT_PDP_DATA_PROFILE_DISABLED  = 0,    // not-used DP for VZW cases
    SIT_PDP_DATA_PROFILE_ENABLED   = 1,    // DP for VZW cases + essential internet DP for non-VZW cases
    SIT_PDP_DATA_PROFILE_IGNORED  = 2   // useless DP for non-VZW cases
}sit_pdp_data_profile_enabled_type;

typedef struct
{
    RCM_HEADER hdr;
    sit_pdp_data_call_item data_call;
}sit_pdp_setup_data_call_rsp;

typedef struct
{
    RCM_HEADER hdr;
    sit_pdp_data_call_item_old data_call;
}sit_pdp_setup_data_call_rsp_old;

typedef enum
{
    NO_NEXT_HEADER = 0,
    EXT_PCSCF = 1,
    EXT_LP_RETRYTIME = 2,
} data_call_extension_header_type;

typedef enum
{
    SIT_PDP_DATA_PROFILE_DEFAULT = 0x00,
    SIT_PDP_DATA_PROFILE_TETHERED,
    SIT_PDP_DATA_PROFILE_OEM_BASE,

    SIT_PDP_DATA_PROFILE_MAX
}sit_pdp_data_profile_e_type;

typedef enum
{
    SIT_PDP_APN_TYPE_DEFAULT = 0x00,
    SIT_PDP_APN_TYPE_IMS,
    SIT_PDP_APN_TYPE_EMERGENCY,
    SIT_PDP_APN_TYPE_EMBMS,

    SIT_PDP_APN_TYPE_MAX
}sit_pdp_apn_type_e_type;

typedef enum
{
    SIT_PDP_AUTH_TYPE_NONE = 0x00,
    SIT_PDP_AUTH_TYPE_PAP,
    SIT_PDP_AUTH_TYPE_CHAP,
    SIT_PDP_AUTH_TYPE_PAP_CHAP,

    SIT_PDP_AUTH_TYPE_MAX
}sit_pdp_auth_type_e_type;

typedef enum
{
    SIT_PDP_PDP_TYPE_UNKNOWN = 0x00,
    SIT_PDP_PDP_TYPE_IPV4 = 0x01,
    SIT_PDP_PDP_TYPE_IPV6 = 0x02,
    SIT_PDP_PDP_TYPE_IPV4IPV6 = 0x03,
    SIT_PDP_PDP_TYPE_PPP,

    SIT_PDP_PDP_TYPE_MAX
}sit_pdp_pdp_type_e_type;

typedef enum
{
    SIT_PDP_PCSCF_REQ_NONE = 0x00,
    SIT_PDP_PCSCF_REQ_IPV4,
    SIT_PDP_PCSCF_REQ_IPV6,
    SIT_PDP_PCSCF_REQ_IPV4V6,

    SIT_PDP_PCSCF_REQ_MAX
}sit_pdp_pcscf_req_e_type;

typedef enum
{
    SIT_PDP_STATUS_FAIL_NONE = 0x00,
    SIT_PDP_STATUS_FAIL_OPERATOR_BARRED = 0x08,
    SIT_PDP_STATUS_FAIL_INSUFFICIENT_RESOURCES = 0x1A,
    SIT_PDP_STATUS_FAIL_MISSING_UKNOWN_APN = 0x1B,
    SIT_PDP_STATUS_FAIL_UNKNOWN_PDP_ADDRESS_TYPE = 0x1C,
    SIT_PDP_STATUS_FAIL_USER_AUTHENTICATION = 0x1D,
    SIT_PDP_STATUS_FAIL_ACTIVATION_REJECT_GGSN = 0x1E,
    SIT_PDP_STATUS_FAIL_ACTIVATION_REJECT_UNSPECIFIED = 0x1F,
    SIT_PDP_STATUS_FAIL_SERVICE_OPTION_NOT_SUPPORTED = 0x20,
    SIT_PDP_STATUS_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED = 0x21,
    SIT_PDP_STATUS_FAIL_SERVICE_OPTION_OUT_OF_ORDER = 0x22,
    SIT_PDP_STATUS_FAIL_NSAPI_IN_USE = 0x23,
    SIT_PDP_STATUS_FAIL_ONLY_IPV4_ALLOWED = 0x32,
    SIT_PDP_STATUS_FAIL_ONLY_IPV6_ALLOWED = 0x33,
    SIT_PDP_STATUS_FAIL_ONLY_SINGLE_BEARER_ALLOWED = 0x34,
    SIT_PDP_STATUS_FAIL_PROTOCOL_ERRORS = 0x6F,

    SIT_PDP_STATUS_FAIL_MAX,
}sit_pdp_status_e_type;

// Just symbolic duplicate definition
typedef enum
{
    SIT_PDP_ACTIVE_INACTIVE = 0x00,
    SIT_PDP_ACTIVE_ACTIVE = 0x01,
    SIT_PDP_ACTIVE_DORMANT = 0x02,
    SIT_PDP_ACTIVE_MAX
}sit_pdp_active_e_type;

/*
    SIT_DEACT_DATA_CALL (RCM ID = 0x0601)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE cid;
    BYTE deact_reason;
}sit_pdp_deact_data_call_req;

typedef enum
{
    SIT_PDP_DEACT_REASON_DEFAULT = 0x00,
    SIT_PDP_DEACT_REASON_RADIO_OFF,
    SIT_PDP_DEACT_REASON_PDP_RESET,

    SIT_PDP_DEACT_REASON_MAX
}sit_pdp_deact_reason_e_type;

typedef null_data_format sit_pdp_deact_data_call_rsp;

/*
    SIT_GET_DATA_CALL_LIST (RCM ID = 0x0602)
*/

typedef null_data_format sit_pdp_get_data_call_list_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE datacall_info_num;
    sit_pdp_data_call_item data_call[MAX_CID_NUM_LEN];
}sit_pdp_get_data_call_list_rsp;

typedef struct
{
    RCM_HEADER hdr;
    BYTE datacall_info_num;
    sit_pdp_data_call_item_old data_call[MAX_CID_NUM_LEN];
}sit_pdp_get_data_call_list_rsp_old;

/*
    SIT_SET_INITIAL_ATTACH_APN (RCM ID = 0x0603)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE cid;
    BYTE rat;
    BYTE data_profile;
    BYTE apn_type;
    char apn[MAX_PDP_APN_LEN];
    char username[MAX_PDP_USER_NAME_LEN];
    char password[MAX_PDP_USER_NAME_LEN];
    BYTE auth_type;
    BYTE pdp_type;
    BYTE pcscf_addr_req;
    BYTE IPv4Address[MAX_IPV4_ADDR_LEN];
    BYTE IPv6Address[MAX_IPV6_ADDR_LEN];
    BYTE profile_id;
    BYTE apn_disable_flag;
    UINT16 max_pdn_conn_per_block;
    UINT16 max_pdn_conn_timer;
    UINT16 pdn_req_wait_interval;
    BYTE roaming_pdp_type;
    BYTE roaming_pcscf_req_type;
}sit_pdp_set_initial_attach_apn_req;

typedef enum
{
    SIT_IA_PROFILE_ID_UNKNOWN = 0x00,
    SIT_IA_PROFILE_ID_VZWIMS = 0x01,
    SIT_IA_PROFILE_ID_VZWFOTA = 0x02,
    SIT_IA_PROFILE_ID_VZWDEFAULT = 0x03,
    SIT_IA_PROFILE_ID_VZWCBS = 0x04,
    SIT_IA_PROFILE_ID_VZWCAS = 0x05,
    SIT_IA_PROFILE_ID_VZWSUPL = 0x06,
    SIT_IA_PROFILE_ID_VZWMMS = 0x07,
    SIT_IA_PROFILE_ID_VZWTETHERED = 0x08,
    SIT_IA_PROFILE_ID_VZWE911 = 0x09,
    SIT_IA_PROFILE_ID_VZWEMBMS = 0x0A,
    SIT_IA_PROFILE_ID_VZWBIP = 0x0B,
    SIT_IA_PROFILE_ID_INTERNET = 0xFF,
}sit_initial_attach_profile_id_e_type;

typedef null_data_format sit_pdp_set_initial_attach_apn_rsp;

/*
    SIT_IND_DATA_CALL_LIST_CHANGED (RCM ID = 0x0604)
    This format will be still used for extension representation
    So if we need to process extension block,
    It's required to calculate index of each data_call item block.
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE datacall_info_num;
    sit_pdp_data_call_item data_call[MAX_CID_NUM_LEN];
}sit_pdp_data_call_list_changed_ind;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE datacall_info_num;
    sit_pdp_data_call_item_old data_call[MAX_CID_NUM_LEN];
}sit_pdp_data_call_list_changed_ind_old;

/*
    SIT_SET_FD_INFO (RCM ID = 0x0605)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE lcd_on;            // Set timer value for lcd on state
    BYTE lcd_off;            // Set timer value for lcd off state
    BYTE rel8_lcd_on;        // Set timer value for lcd on state when network support REL 8
    BYTE rel8_lcd_off;        // Set timer value for lcd off state when network support REL 8
}sit_pdp_set_fd_info_req;

typedef null_data_format sit_pdp_set_fd_info_rsp;

/*
    SIT_IND_DEDICATED_BEARER_INFO (RCM ID = 0x0606)
*/
enum {
   SIT_REL_7 = 0,
   SIT_DOWNLINK = 1,
   SIT_UPLINK = 2,
   SIT_BIDIRECTIONAL = 3,
}sit_qos_filter_direction;

typedef struct
{
    INT32 field_flag;
    BYTE local_address[MAX_PDP_ADDRESS_LEN];
    BYTE local_address_prefix_v6;
    BYTE local_address_prefix_v4;
    BYTE remote_address[MAX_PDP_ADDRESS_LEN];
    BYTE remote_address_prefix_v6;
    BYTE remote_address_prefix_v4;
    UINT16 local_port_low;
    UINT16 local_port_high;
    UINT16 remote_port_low;
    UINT16 remote_port_high;
    INT32 qos_protocol;
    BYTE type_of_service;
    INT32 ipv6_flow_label;
    INT32 ip_sec_spi;
    BYTE direction;
    INT32 precedence;
}sit_pdp_qos_filter_info;


typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE cid;
    BYTE status;
    INT32 qos_session_id;
    BYTE qos_rat_type;
    union
    {
        sit_pdp_EpsQos eps;
        sit_pdp_NrQos nr;
    }qos;
    BYTE num_of_qos_filter;
    sit_pdp_qos_filter_info qos_filter[MAX_QOS_FILTERS];
}sit_pdp_dedicated_bearer_info_ind;

/*
   SIT_IND_NAS_TIMER_STATUS (RCM ID = 0x0607)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE timer_type;
    BYTE timer_status;
    INT32 timer_value;
    char apn[MAX_PDP_APN_LEN];
}sit_pdp_nas_timer_status_ind;

/*
   SIT_DETACH (RCM ID = 0x0608)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE detach_reason;
}sit_pdp_detach_req;

/*
    SIT_IND_DATA_STATE_CHANGE (RCM ID = 0x0609)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE expected_state;
}sit_pdp_data_state_change_ind;

/*
   SIT_START_KEEPALIVE (RCM ID = 0x060A)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE keepalive_type;    // same to RIL_KeepaliveType
    BYTE src_addr[MAX_IPV6_ADDR_LEN];
    INT32 source_port;
    BYTE dst_addr[MAX_IPV6_ADDR_LEN];
    INT32 dst_port;
    INT32 max_interval;
    INT32 cid;
}sit_pdp_start_keepalive_req;

typedef struct
{
    RCM_HEADER hdr;
    UINT32 session_handle;
    INT32 status_code;    // same to RIL_KeepaliveStatusCode
}sit_pdp_start_keepalive_rsp;

/*
   SIT_STOP_KEEPALIVE (RCM ID = 0x060B)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT32 keepalive_handle;
}sit_pdp_stop_keepalive_req;

/*
    SIT_IND_KEEPALIVE_STATUS (RCM ID = 0x060C)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    UINT32 session_handle;
    INT32 status_code;
}sit_pdp_keepalive_status_ind;

/*
    SIT_IND_PCO_DATA (RCM ID = 0x060D)
*/
typedef struct
{
    INT32 pco_id;
    BYTE contents_len; // This will be embedded in contents, first byes is length
    char *contents; // This will be increased from modemData by 1byte which is length field
} sit_pdp_pco_data_entry;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE cid;
    BYTE pdp_type;
    BYTE pco_num;
}sit_pdp_pco_data_ind;

/*
   SIT_GET_SLICING_CONFIG (RCM ID = 0x0622)
   SIT_IND_SLICING_CONFIG (RCM ID = 0x0623)
   SIT_SET_SlICING_CONFIG (RCM ID = 0x0624)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE slicing_config_version;
}sit_pdp_get_slicing_config;

typedef struct
{
    BYTE DNN_length;
    BYTE * DNN;
}sit_pdp_dnn_info;

typedef struct
{
    BYTE Length;
    BYTE Data;
}sit_pdp_traffic_descriptor;

typedef struct
{
    BYTE nssaiType;
    uint8_t SliceInfoSst;
    int32_t SliceInfoSd;
    uint8_t SliceMappedHplmnSst;
    int32_t SliceMappedHplmnSd;
    BYTE status;
}sit_pdp_slice_info;

struct sit_pdp_location_criteria_typeinfo {
    uint16_t length;
    /* actual payload will be parsed dynamically */
};

typedef struct
{
    BYTE precedence;
    BYTE sessionType;
    BYTE sscMode;
    uint8_t preferredAccessType;                    // only in v2
    uint8_t multiAccessPreferenceType;              // only in v2
    uint8_t nonSeamlessNon3gppOffloadIndication;    // only in v2
    uint8_t timeWindowType[16];                     // only in v2
    sit_pdp_location_criteria_typeinfo lct;         // only in v2
    BYTE numOfSliceInfo;
    sit_pdp_slice_info sliceInfo;
    BYTE numOfDNN;
    sit_pdp_dnn_info dnnInfo;
}sit_pdp_route_selection_descriptor;

struct sit_pdp_ursp_rules
{
    BYTE precedence; // Just useful,
    // Below is variable area decoded in code dynamically
    // sit_pdp_traffic_descriptor trafficDescriptor; // -> Extended in v2
    // And variable area
    // BYTE num_of_rsd;
    // sit_pdp_route_selection_descriptor routeSelectionDescriptor;
};

// RSP v1
struct sit_pdp_slicing_config_item
{
    BYTE slicing_config_version;
    BYTE urspNum;
    sit_pdp_ursp_rules urspRules;
    BYTE sliceNum;
    sit_pdp_slice_info sliceInfo;
};

// RSP/IND v1
struct sit_pdp_slicing_config_res
{
    RCM_HEADER hdr;
    sit_pdp_slicing_config_item slicing_config;
};

// RSP/IND v2


/*
    SIT_START_LCE_INFO (RCM ID = 0x060E)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 interval;
    BYTE mode;    // 0: push, 1: pull  which is same to LceServiceMode
}sit_pdp_start_lce_info_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE status;    // 0: stopped, 1: active, 2: not supported
    INT32 interval;
}sit_pdp_start_lce_info_rsp;

/*
    SIT_STOP_LCE_INFO (RCM ID = 0x060F)
*/
typedef null_data_format sit_pdp_stop_lce_info_req;
typedef struct
{
    RCM_HEADER hdr;
    BYTE status;    // 0: stopped, 1: active, 2: not supported
    INT32 interval;
}sit_pdp_stop_lce_info_rsp;

/*
    SIT_GET_LCE_DATA (RCM ID = 0x0610)
*/
typedef null_data_format sit_pdp_get_lce_data_req;
typedef struct
{
    RCM_HEADER hdr;
    INT32 dl_lc;
    INT32 ul_lc;
    BYTE conf_lvl;
    BYTE is_suspended;    // 0: not suspended, 1: suspended
}sit_pdp_get_lce_data_rsp;

/*
    SIT_IND_LCE_DATA (RCM ID = 0x0611)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 dl_lc;
    INT32 ul_lc;
    BYTE conf_lvl;
    BYTE is_suspended;    // 0: not suspended, 1: suspended
}sit_pdp_lce_data_ind;

/*
    SIT_SET_DATA_PROFILE (RCM ID = 0x0613)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT32 profile_id;
    char apn[MAX_PDP_APN_LEN];
    BYTE pdp_type;
    BYTE roaming_pdp_type;
    BYTE auth_type;
    char username[MAX_PDP_USER_NAME_LEN];
    char password[MAX_PDP_USER_NAME_LEN];
    BYTE data_profile_info_type;
    UINT32 max_conns_time;
    UINT32 max_conns;
    UINT32 wait_time;
    BYTE enabled;
    UINT32 apn_type;
    UINT32 radio_access_family;
    UINT32 mtu;
}sit_pdp_set_data_profile_req;

/*
    SIT_SET_IMS_TEST_MODE (RCM ID = 0x0614)
*/

typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
} sit_pdp_set_ims_test_mode_req;

/*
    SIT_GET_PHONE_CAPABILITY (RCM ID = 0x0615)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE max_simultaneous_data_stack;
    BYTE max_simultaneous_internet_pdn;
    BYTE internet_lingering_support;
    BYTE max_supported_stack;
} sit_pdp_get_phone_capability_rsp;

/*
    SIT_SET_PDN_IP_ADDRESS (RCM ID = 0x0616)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE cid;
    BYTE pdp_type;
    BYTE IPv4Address[MAX_IPV4_ADDR_LEN];
    BYTE IPv6Address[MAX_IPV6_ADDR_LEN];
} sit_pdp_set_pdn_ip_address_req;

/*
    SIT_IND_IP_ADDR_STATUS (RCM ID = 0x0617)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE cid;
    BYTE status;
    BYTE IPv4Address[MAX_IPV4_ADDR_LEN];
    BYTE IPv6Address[MAX_IPV6_ADDR_LEN];
} sit_pdp_ip_addr_status_ind;

/*
   SIT_IND_PS_DATA_OFF_STATUS (RCM ID = 0x0619)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE ps_data_off_available_status;
    BYTE exempt_list_for_home;
    BYTE exempt_list_for_roam;
} sit_pdp_ps_data_off_status_ind;

/*
   SIT_ALLOC_PDU_SESSION (RCM ID = 0x061A)
 */
typedef struct
{
    RCM_HEADER hdr;
} sit_pdp_alloc_pdu_session_req;

typedef struct
{
    RCM_HEADER hdr;
    int session_id;
} sit_pdp_alloc_pdu_session_resp;

/*
   SIT_REL_PDU_SESSION (RCM ID = 0x061B)
 */
typedef struct
{
    RCM_HEADER hdr;
    int session_id;
} sit_pdp_rel_pdu_session_req;

/*
   SIT_START_HANDOVER (RCM ID = 0x061C)
 */
typedef struct
{
    RCM_HEADER hdr;
    int call_id;
} sit_pdp_start_handover_req;

/*
   SIT_CANCEL_HANDOVER (RCM ID = 0x061D)
 */
typedef struct
{
    RCM_HEADER hdr;
    int call_id;
} sit_pdp_cancel_handover_req;

/*
   SIT_IND_UNTHROTTLE_APN (RCM ID = 0x061E)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    char apn[MAX_PDP_APN_LEN];
} sit_pdp_unthrottle_apn_ind;

/*
   SIT_SET_MOBILE_DATA_STATE (RCM ID = 0x0625)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE mobile_data;
    BYTE roaming_data;
} sit_set_mobile_data_state_req;

/*
    SIT_GET_CS_REG_STATE (RCM ID = 0x0700)
*/
typedef null_data_format sit_net_get_cs_reg_state_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE reg_state;
    BYTE rej_cause;    // only available when reg_state == denied (0x03)
    BYTE rat;
    INT32 gw_lac;
    INT32 gw_cid;
    BYTE gw_psc;
    INT32 lte_tac;
    INT32 lte_pcid;
    INT32 lte_eci;
    INT32 lte_csgid;
    INT32 lte_tadv;
    INT32 basestationid;
    INT32 basestation_latitude;
    INT32 basestation_longitude;
    BYTE concurrent;
    INT16 sid;
    INT16 nid;
    BYTE roamingindicator;
    BYTE registered_prl;
    BYTE roaming_indi_prl;
}sit_net_get_cs_reg_state_rsp;

// a data structure to support radio@1.0
typedef struct {
    BYTE reg_state;
    BYTE rej_cause;    // only available when reg_state == denied (0x03)
    BYTE rat;
    INT32 gw_lac;
    INT32 gw_cid;
    BYTE gw_psc;
    INT32 lte_tac;
    INT32 lte_pcid;
    INT32 lte_eci;
    INT32 lte_csgid;
    INT32 lte_tadv;
    INT32 basestationid;
    INT32 basestation_latitude;
    INT32 basestation_longitude;
    BYTE concurrent;
    INT16 sid;
    INT16 nid;
    BYTE roamingindicator;
    BYTE registered_prl;
    BYTE roaming_indi_prl;
    INT32 channel;         // arfcn, uarfcn, earfcn
} sit_cs_reg_state_v1_0;

// a data structure to support radio@1.5
typedef struct __sit_cs_reg_state_v1_1 : sit_cs_reg_state_v1_0 {
    ULONG nr_cid;
    INT32 nr_pcid;
    INT32 nr_tac;
    INT32 nr_arfcn;
} sit_cs_reg_state_v1_1;

/*
    SIT_GET_PS_REG_STATE (RCM ID = 0x0701)
*/
typedef null_data_format sit_net_get_ps_reg_state_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE reg_state;
    BYTE rej_cause;    // only available when reg_state == denied (0x03)
    BYTE max_sdc;
    BYTE rat;
    INT32 gw_lac;
    INT32 gw_cid;
    BYTE gw_psc;
    INT32 lte_tac;
    INT32 lte_pcid;
    INT32 lte_eci;
    INT32 lte_csgid;
    INT32 lte_tadv;
    BYTE ims_vops;     // 0x0 : Not available 0x01 : available
    BYTE emc_service;       // 0x0 : Not available 0x01 : available
}sit_net_get_ps_reg_state_rsp;

// a data structure to support radio@1.0
typedef struct {
    BYTE reg_state;
    BYTE rej_cause;    // only available when reg_state == denied (0x03)
    BYTE max_sdc;
    BYTE rat;
    INT32 gw_lac;
    INT32 gw_cid;
    BYTE gw_psc;
    INT32 lte_tac;
    INT32 lte_pcid;
    INT32 lte_eci;
    INT32 lte_csgid;
    INT32 lte_tadv;
    BYTE ims_vops;     // 0x0 : Not available 0x01 : available
    BYTE emc_service;       // 0x0 : Not available 0x01 : available
    INT32 channel;         // arfcn, uarfcn, earfcn
} sit_ps_reg_state_v1_0;

// a data structure to support NR
typedef struct __sit_ps_reg_state_v1_1 : sit_ps_reg_state_v1_0 {
    BYTE endc;             // 0x00 : Not available, 0x01: available
    BYTE dcnr_restricted;  // 0x00 : Not restrict,  0x01: restrict
    BYTE nr_available;     // 0x00 : Not available, 0x01: available
} sit_ps_reg_state_v1_1;

// a data structure to support radio@1.5
typedef struct __sit_ps_reg_state_v1_2 : sit_ps_reg_state_v1_1 {
    ULONG nr_cid;
    INT32 nr_pcid;
    INT32 nr_tac;
    INT32 nr_arfcn;
} sit_ps_reg_state_v1_2;

// a data structure to support radio@1.6
typedef struct __sit_ps_reg_state_v1_3 : sit_ps_reg_state_v1_2 {
    INT32 emf;
} sit_ps_reg_state_v1_3;

/*
    SIT_GET_OPERATOR (RCM ID = 0x0702)
*/
typedef null_data_format sit_net_get_operator_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE plmn[MAX_PLMN_LEN];
    char short_name[MAX_SHORT_NAME_LEN];
    char long_name[MAX_FULL_NAME_LEN];
}sit_net_get_operator_rsp;

typedef struct
{
    BYTE plmn[MAX_PLMN_LEN];
    char short_name[MAX_SHORT_NAME_LEN];
    char long_name[MAX_FULL_NAME_LEN];
    BYTE reg_state;
}sit_net_operator_v1_0;

typedef struct
{
    BYTE plmn[MAX_PLMN_LEN];
    char short_name[MAX_SHORT_NAME_LEN];
    char long_name[MAX_FULL_NAME_LEN];
    BYTE reg_state;
    INT32 lac;
}sit_net_operator_v1_1;

/*
    SIT_GET_NTW_MODE (RCM ID = 0x0703)
*/
typedef null_data_format sit_net_get_network_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 network_mode;    // sit_net_network_mode_e_type
}sit_net_get_network_mode_rsp;

/*
    SIT_SET_NTW_MODE_AUTO (RCM ID = 0x0704)
*/
typedef null_data_format sit_net_set_network_mode_auto_req;

typedef null_data_format sit_net_set_network_mode_auto_rsp;

/*
    SIT_SET_NTW_MODE_MANUAL (RCM ID = 0x0705)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 rat;    // sit_rat_type_e_type
    BYTE plmn[MAX_PLMN_LEN];
}sit_net_set_metwork_mode_manual_req;

typedef null_data_format sit_net_set_metwork_mode_manual_rsp;

/*
    SIT_GET_AVAILABLE_NETWORKS (RCM ID = 0x0706)
*/
typedef struct {
    RCM_HEADER hdr;
    int ran;    // sit_net_ran_e_type
} sit_net_get_available_networks_wit_rat_req;

typedef struct
{
    INT32 rat;                // sit_rat_type_e_type
    BYTE plmn[MAX_PLMN_LEN];
    INT32 plmn_status;        // sit_net_plmn_status_e_type
}sit_net_network_info_item;

typedef struct
{
    RCM_HEADER hdr;
    INT32 network_info_num;
    sit_net_network_info_item network_info[MAX_NET_INFO_COUNT];
}sit_net_get_available_networks_rsp;

/*
    SIT_CANCEL_GET_AVAILABLE_NETWORKS (RCM ID = 0x0707)
*/
typedef null_data_format sit_net_cancel_get_available_networks_req;

typedef null_data_format sit_net_cancel_get_available_networks_rsp;

/*
    SIT_SET_BAND_MODE (RCM ID = 0x0708)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 band;    // sit_net_band_e_type
}sit_net_set_band_mode_req;

typedef null_data_format sit_net_set_band_mode_rsp;

/*
    SIT_GET_BAND_MODE (RCM ID = 0x0709)
*/
typedef null_data_format sit_net_get_band_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 band_info_num;
    INT32 band[MAX_NET_INFO_COUNT];
}sit_net_get_band_mode_rsp;


/*
    SIT_SET_PREFERRED_NTW_TYPE (RCM ID = 0x070A)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 pref_net_type;
}sit_net_set_pref_network_req;

/*
    SIT_GET_PREFERRED_NTW_TYPE (RCM ID = 0x070B)
*/
typedef null_data_format sit_net_get_pref_network_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 pref_net_type;
}sit_net_get_pref_network_rsp;

/*
    SIT_GET_CELL_INFO_LIST (RCM ID = 0x070C)
*/
typedef null_data_format sit_net_get_cell_info_list_req;

typedef struct
{
    BYTE plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 sig_str;
    INT32 sig_ber;
}cell_info_gsm;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 arfcn;
    BYTE bsic;
    INT32 sig_str;
    INT32 sig_ber;
    INT32 sig_ta;
}cell_info_gsm_v12;

typedef struct
{
    INT32 ntw_id;
    INT32 sys_id;
    INT32 bs_id;
    INT32 longitude;
    INT32 lat;
    INT32 sig_dbm;
    INT32 sig_ecio;
    INT32 sig_snr;
    INT32 evdo_sig_dbm;
    INT32 evdo_sig_ecio;
    INT32 evdo_sig_snr;
}cell_info_cdma;

typedef struct
{
    INT32 ntw_id;
    INT32 sys_id;
    INT32 bs_id;
    INT32 longitude;
    INT32 lat;
    INT32 sig_dbm;
    INT32 sig_ecio;
    INT32 evdo_sig_dbm;
    INT32 evdo_sig_ecio;
    INT32 evdo_sig_snr;
}cell_info_cdma_v14;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 cell_id;
    INT32 phy_cell_id;
    INT32 tac;
    INT32 sig_str;
    INT32 sig_rsrp;
    INT32 sig_rsrq;
    INT32 sig_rssnr;
    INT32 sig_cqi;
    INT32 ta;
}cell_info_lte;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 cell_id;
    INT32 phy_cell_id;
    INT32 tac;
    INT32 earfcn;
    INT32 sig_str;
    INT32 sig_rsrp;
    INT32 sig_rsrq;
    INT32 sig_rssnr;
    INT32 sig_cqi;
    INT32 ta;
}cell_info_lte_v12;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 cell_id;
    INT32 phy_cell_id;
    INT32 tac;
    INT32 earfcn;
    INT32 bandwidth;
    INT32 endc_available;
    INT32 sig_str;
    INT32 sig_rsrp;
    INT32 sig_rsrq;
    INT32 sig_rssnr;
    INT32 sig_cqi;
    INT32 ta;
}cell_info_lte_v14;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 psc;
    INT32 sig_str;
    INT32 sig_ber;
}cell_info_wcdma;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 psc;
    INT32 uarfcn;
    INT32 sig_str;
    INT32 sig_ber;
}cell_info_wcdma_v12;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 psc;
    INT32 uarfcn;
    INT32 sig_str;
    INT32 sig_ber;
    INT32 rscp;
    INT32 ecno;
}cell_info_wcdma_v14;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 cpid;
    INT32 rscp;
}cell_info_tdscdma;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 cpid;
    INT32 sig_str;
    INT32 ber;
    INT32 rscp;
}cell_info_tdscdma_v14;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    ULONG cell_id;
    UINT32 phy_cell_id;
    INT32 tac;
    INT32 arfcn;
    INT32 ss_rsrp;
    INT32 ss_rsrq;
    INT32 ss_sinr;
    INT32 csi_rsrp;
    INT32 csi_rsrq;
    INT32 csi_sinr;
}cell_info_nr;

typedef struct
{
    BYTE cell_info_type;
    BYTE reg_status;
    union
    {
        cell_info_gsm gsm;
        cell_info_cdma cdma;
        cell_info_lte lte;
        cell_info_wcdma wcdma;
        cell_info_tdscdma tdscdma;
    }cell_info;
}sit_net_cell_info_item;

typedef struct
{
    BYTE cell_info_type;
    BYTE reg_status;
    union
    {
        cell_info_gsm_v12 gsm;
        cell_info_cdma cdma;
        cell_info_lte_v12 lte;
        cell_info_wcdma_v12 wcdma;
        cell_info_tdscdma tdscdma;
    }cell_info;
}sit_net_cell_info_item_v12;

typedef struct
{
    BYTE cell_info_type;
    BYTE reg_status;
    BYTE cell_connection_status;
    union
    {
        cell_info_gsm_v12 gsm;
        cell_info_cdma_v14 cdma;
        cell_info_lte_v14 lte;
        cell_info_wcdma_v14 wcdma;
        cell_info_tdscdma_v14 tdscdma;
        cell_info_nr nr;
    }cell_info;
}sit_net_cell_info_item_v14;

// cell identity
typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 arfcn;
    BYTE bsic;
}cell_identity_gsm;

typedef struct
{
    INT32 ntw_id;
    INT32 sys_id;
    INT32 bs_id;
    INT32 longitude;
    INT32 lat;
}cell_identity_cdma;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 cell_id;
    INT32 phy_cell_id;
    INT32 tac;
    INT32 earfcn;
    INT32 bandwidth;
}cell_identity_lte;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 psc;
    INT32 uarfcn;
}cell_identity_wcdma;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 cpid;
}cell_identity_tdscdma;

typedef struct
{
    char plmn[MAX_PLMN_LEN];
    ULONG cell_id;
    UINT32 phy_cell_id;
    INT32 tac;
    INT32 arfcn;
}cell_identity_nr;

typedef struct
{
    RCM_HEADER hdr;
    INT32 cell_info_num;
    char cell_info_list[0];
}sit_net_get_cell_info_list_rsp;

// Cell Info Version 3
#define MAX_BANDS 8

typedef struct {
    char plmn_short_name[MAX_SHORT_NAME_LEN];
    char plmn_long_name[MAX_FULL_NAME_LEN];
    INT32 additional_plmn_len;
    char additional_plmn_info[(EACH_PLMN_LEN * MAX_ADDITIONAL_PLMN_LEN)];
}plmn_info_v3;

typedef struct {
    BYTE csg_indication;
    char hnb_name[HNB_NAME_LEN];
    int csg_identity;
}csg_info_v3;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 arfcn;
    BYTE bsic;
    plmn_info_v3 plmn_info;
}cell_identity_gsm_v3;

typedef struct {
    cell_identity_gsm_v3 cell_identity;
    INT32 sig_str;
    INT32 sig_ber;
    INT32 sig_ta;
}cell_info_type_gsm_v3;

typedef struct {
    INT32 ntw_id;
    INT32 sys_id;
    INT32 bs_id;
    INT32 longitude;
    INT32 latitude;
}cell_identity_cdma_v3;

typedef struct {
    cell_identity_cdma_v3 cell_identity;
    INT32 cdma_dbm;
    INT32 cdma_ecio;
    INT32 evdo_dbm;
    INT32 evdo_ecio;
    INT32 evdo_snr;
}cell_info_type_cdma_v3;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 cell_id;
    INT32 phy_cell_id;
    INT32 tac;
    INT32 earfcn;
    INT32 bandwidth;
    plmn_info_v3 plmn_info;
    csg_info_v3 csg_info;
    INT32 band_len;
    INT16 band_info[MAX_BANDS];
}cell_identity_lte_v3;

typedef struct {
    cell_identity_lte_v3 cell_identity;
    INT32 endc_available;
    INT32 sig_str;
    INT32 sig_rsrp;
    INT32 sig_rsrq;
    INT32 sig_rssnr;
    INT32 cqi;
    INT32 ta;
}cell_info_type_lte_v3;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 psc;
    INT32 uarfcn;
    plmn_info_v3 plmn_info;
    csg_info_v3 csg_info;
}cell_identity_wcdma_v3;

typedef struct {
    cell_identity_wcdma_v3 cell_identity;
    INT32 sig_str;
    INT32 sig_ber;
    INT32 rscp;
    INT32 ecno;
}cell_info_type_wcdma_v3;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 cpid;
    INT32 uarfcn;
    plmn_info_v3 plmn_info;
    csg_info_v3 csg_info;
}cell_identity_tdscdma_v3;

typedef struct {
    cell_identity_tdscdma_v3 cell_identity;
    INT32 sig_str;
    INT32 ber;
    INT32 rscp;
}cell_info_type_tdscdma_v3;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    ULONG cell_id;
    UINT32 phy_cell_id;
    INT32 tac;
    INT32 arfcn;
    plmn_info_v3 plmn_info;
    csg_info_v3 csg_info;
    INT32 band_len;
    INT16 band_info[MAX_BANDS];
}cell_identity_nr_v3;

typedef struct {
    cell_identity_nr_v3 cell_identity;
    INT32 ss_rsrp;
    INT32 ss_rsrq;
    INT32 ss_sinr;
    INT32 csi_rsrp;
    INT32 csi_rsrq;
    INT32 csi_sinr;
}cell_info_type_nr_v3;

typedef struct {
    BYTE cell_info_type;
    BYTE reg_status;
    BYTE cell_connection_status;
    union {
        cell_info_type_gsm_v3 gsm;
        cell_info_type_cdma_v3 cdma;
        cell_info_type_lte_v3 lte;
        cell_info_type_wcdma_v3 wcdma;
        cell_info_type_tdscdma_v3 tdscdma;
        cell_info_type_nr_v3 nr;
    }cell_info;
}cell_information_v3;

// Cell Info Version 4
typedef struct {
    char plmn_short_name[MAX_SHORT_NAME_LEN];
    char plmn_long_name[MAX_FULL_NAME_LEN];
    INT32 additional_plmn_len;
    char additional_plmn_info[(EACH_PLMN_LEN * MAX_ADDITIONAL_PLMN_LEN)];
}plmn_info_v4;

typedef struct {
    BYTE csg_indication;
    char hnb_name[HNB_NAME_LEN];
    INT32 csg_identity;
}csg_info_v4;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 arfcn;
    BYTE bsic;
    plmn_info_v4 plmn_info;
}cell_identity_gsm_v4;

typedef struct {
    cell_identity_gsm_v4 cell_identity;
    INT32 sig_str;
    INT32 sig_ber;
    INT32 sig_ta;
}cell_info_type_gsm_v4;

typedef struct {
    INT32 ntw_id;
    INT32 sys_id;
    INT32 bs_id;
    INT32 longitude;
    INT32 latitude;
}cell_identity_cdma_v4;

typedef struct {
    cell_identity_cdma_v4 cell_identity;
    INT32 cdma_dbm;
    INT32 cdma_ecio;
    INT32 evdo_dbm;
    INT32 evdo_ecio;
    INT32 evdo_snr;
}cell_info_type_cdma_v4;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 cell_id;
    INT32 phy_cell_id;
    INT32 tac;
    INT32 earfcn;
    INT32 bandwidth;
    plmn_info_v4 plmn_info;
    csg_info_v4 csg_info;
    INT32 band_len;
    INT16 band_info[MAX_BANDS];
}cell_identity_lte_v4;

typedef struct {
    cell_identity_lte_v4 cell_identity;
    INT32 endc_available;
    INT32 sig_str;
    INT32 sig_rsrp;
    INT32 sig_rsrq;
    INT32 sig_rssnr;
    INT32 cqi_table_index;
    INT32 cqi;
    INT32 ta;
}cell_info_type_lte_v4;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 psc;
    INT32 uarfcn;
    plmn_info_v4 plmn_info;
    csg_info_v4 csg_info;
}cell_identity_wcdma_v4;

typedef struct {
    cell_identity_wcdma_v4 cell_identity;
    INT32 sig_str;
    INT32 sig_ber;
    INT32 rscp;
    INT32 ecno;
}cell_info_type_wcdma_v4;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    INT32 lac;
    INT32 cid;
    INT32 cpid;
    INT32 uarfcn;
    plmn_info_v4 plmn_info;
    csg_info_v4 csg_info;
}cell_identity_tdscdma_v4;

typedef struct {
    cell_identity_tdscdma_v4 cell_identity;
    INT32 sig_str;
    INT32 ber;
    INT32 rscp;
}cell_info_type_tdscdma_v4;

typedef struct {
    char plmn[MAX_PLMN_LEN];
    ULONG cell_id;
    UINT32 phy_cell_id;
    INT32 tac;
    INT32 arfcn;
    plmn_info_v4 plmn_info;
    csg_info_v4 csg_info;
    INT32 band_len;
    INT16 band_info[MAX_BANDS];
}cell_identity_nr_v4;

#define SIT_MAX_NR_SUBBAND_INDEX 19
typedef struct {
    cell_identity_nr_v4 cell_identity;
    INT32 ss_rsrp;
    INT32 ss_rsrq;
    INT32 ss_sinr;
    INT32 csi_rsrp;
    INT32 csi_rsrq;
    INT32 csi_sinr;
    INT32 cqi_table_index;
    INT32 wb_cqi;
    INT32 sb_cqi[SIT_MAX_NR_SUBBAND_INDEX];
}cell_info_type_nr_v4;

typedef struct {
    BYTE cell_info_type;
    BYTE reg_status;
    BYTE cell_connection_status;
    union {
        cell_info_type_gsm_v4 gsm;
        cell_info_type_cdma_v4 cdma;
        cell_info_type_lte_v4 lte;
        cell_info_type_wcdma_v4 wcdma;
        cell_info_type_tdscdma_v4 tdscdma;
        cell_info_type_nr_v4 nr;
    }cell_info;
}cell_information_v4;

/*
    SIT_SET_CELL_INFO_LIST_REPORT_RATE (RCM ID = 0x070D)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 report_rate;
}sit_net_cell_info_list_report_rate_req;

typedef null_data_format sit_net_cell_info_list_report_rate_rsp;

/*
    SIT_IND_NTW_STATE_CHANGED (RCM ID = 0x070E)
*/
typedef null_ind_data_format sit_net_network_state_changed_ind;

/*
    SIT_IND_CELL_INFO_LIST (RCM ID = 0x070F)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 cell_info_num;
    char cell_info_list[0];
}sit_net_cell_info_list_ind;

/*
    SIT_SET_PS_SERVICE (RCM ID = 0x0710)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE state;    // sit_net_enable_e_type
}sit_net_set_ps_service_req;
typedef null_data_format sit_net_set_ps_service_rsp;

/*
    SIT_GET_PS_SERVICE (RCM ID = 0x0711)
*/
typedef null_data_format sit_net_get_ps_service_req;
typedef struct
{
    RCM_HEADER hdr;
    BYTE state;
}sit_net_get_ps_service_rsp;

/*
   SIT_SET_EMERGENCY_CALL_STATUS (RCM ID = 0x0712)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE status;     // EMERGENCY_CALL_STATUS_START, etc
    BYTE rat;        // sit_rat_type_e_type
}sit_net_set_emergency_call_status_req;
typedef null_data_format sit_net_set_emergency_call_status_rsp;

/*
   SIT_IND_EMGERGENCY_ACT_INFO (RCM ID = 0x0713)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE rat;
    BYTE act_status;
}sit_net_emergency_act_info_ind;

/*
    SIT_SET_UPLMN (RCM ID = 0x0714)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
    BYTE index;
    BYTE plmn[MAX_PLMN_LEN];
    BYTE act;
}sit_net_set_uplmn_req;
typedef null_data_format sit_net_set_uplmn_rsp;

/*
    SIT_GET_UPLMN (RCM ID = 0x0715)
*/
typedef null_data_format sit_net_get_uplmn_req;

typedef struct
{
    BYTE index;
    char plmn[MAX_PLMN_LEN];
    BYTE act;
}preferred_plmn_item;

typedef struct
{
    RCM_HEADER hdr;
    INT32 plmn_list_num;
    preferred_plmn_item preffered_plmn[0];
}sit_net_get_uplmn_rsp;

/*
    SIT_SET_DS_NTW_TYPE (RCM ID = 0x0716)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 pref_ds_net_type;    // sit_net_pref_ds_type_e_type
}sit_net_set_ds_network_req;

/*
    SIT_GET_RADIO_CAPABILITY (RCM ID = 0x0718)
*/
typedef null_data_format sit_net_get_rc_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 rc_raf;
}sit_net_get_rc_rsp;

/*
    SIT_SET_RADIO_CAPABILITY (RCM ID = 0x0719)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 version;
    INT32 session_id;
    INT32 phase;  // same to RadioCapabilityPhase
    INT32 rc_raf;
    BYTE uuid[SIT_MAX_UUID_LENGTH];
    INT32 status;  // same to RadioCapabilityStatus
}sit_net_set_rc_req;

typedef null_data_format sit_net_set_rc_rsp;

/*
    SIT_GET_DUPLEX_MODE (RCM ID = 0x071A)
*/
typedef null_data_format sit_net_get_duplex_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE duplex_mode_4g;    // sit_net_duplex_mode_e_type
    BYTE duplex_mode_3g;    // sit_net_duplex_mode_e_type
}sit_net_get_duplex_mode_rsp;

/*
    SIT_SET_DUPLEX_MODE (RCM ID = 0x071B)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE duplex_mode_4g;    // sit_net_duplex_mode_e_type
    BYTE duplex_mode_3g;    // // sit_net_duplex_mode_e_type
}sit_net_set_duplex_mode_req;

typedef null_data_format sit_net_set_duplex_mode_rsp;

/*
    SIT_SET_MICRO_CELL_SEARCH (RCM ID = 0x071F)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE srch_mode;    // sit_net_mc_srch_mode_type
}sit_net_set_micro_cell_search_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE srch_result;
    BYTE plmn[MAX_PLMN_LEN];
}sit_net_set_micro_cell_search_rsp;

/*
   SIT_IND_AC_BARRING_INFO (RCM ID = 0x0720)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE for_emc;
    BYTE for_mo_sig_factor;
    INT16 for_mo_sig_time;
    BYTE for_mo_sig_ac_list[SPECIAL_AC_LIST];
    BYTE for_mo_data_factor;
    INT16 for_mo_data_time;
    BYTE for_mo_data_ac_list[SPECIAL_AC_LIST];
    BYTE for_mmtel_voice_factor;
    INT16 for_mmtel_voice_time;
    BYTE for_mmtel_voice_ac_list[SPECIAL_AC_LIST];
    BYTE for_mmtel_video_factor;
    INT16 for_mmtel_video_time;
    BYTE for_mmtel_video_ac_list[SPECIAL_AC_LIST];
}sit_net_ac_barring_info_ind;

/*
   SIT_SET_CDMA_ROAMING_PREFERENCE (RCM ID = 0x0721)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT8 cdma_roaming_type;    // sit_net_cdma_roaming_type
}sit_net_set_cdma_roaming_req;

typedef null_data_format sit_net_set_cdma_roaming_rsp;

/*
   SIT_GET_CDMA_ROAMING_PREFERENCE (RCM ID = 0x0722)
*/
typedef null_data_format sit_net_query_cdma_roaming_rep;

typedef struct
{
    RCM_HEADER hdr;
    INT8 cdma_roaming_type;
}sit_net_query_cdma_roaming_rsp;

/*
    SIT_SET_CDMA_HYBRID_MODE (RCM ID = 0x723)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT8 hybrid_mode;    // RIL_CdmaHybridMode
}sit_net_set_cdma_hybrid_mode_req;

/*
    SIT_GET_CDMA_HYBRID_MODE (RCM ID = 0x724)
*/
typedef null_data_format sit_net_get_cdma_hybrid_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    INT8 hybrid_mode;
}sit_net_get_cdma_hybrid_mode_rsp;

/*
    SIT_SET_DUAL_NTW_AND_PS_TYPE (RCM ID = 0x72B)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 pref_net_type_for_primary;
    INT32 pref_net_type_for_secondary;
    INT32 allowed_for_primary;
    INT32 allowed_for_secondary;
}sit_net_set_dual_network_and_allow_data_req;

typedef null_data_format sit_net_set_pref_network_rsp;

/*
    SIT_IND_TOTAL_OOS (RCM ID = 0x072C)
*/

/*
    SIT_IND_MCC (RCM ID = 0x072D)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE pref_net_type;
    char mcc[3];
}sit_net_mcc_ind;

/*
    SIT_SET_CARRIER_RESTRICTIONS (RCM ID = 0x072E)
*/
typedef struct
{
    BYTE mcc[MAX_MCC_LEN];
    BYTE mnc[MAX_MNC_LEN];
    BYTE match_type;    // sit_carrier_match_type_e_type
    BYTE match_len;
    BYTE match_data[MAX_CR_MATCH_DATA_SIZE];
}SIT_CARRIER_INFO;

typedef struct
{
    RCM_HEADER hdr;
    INT32 allowed_carriers_len;
    INT32 excluded_carriers_len;
    SIT_CARRIER_INFO carrier_list[0];
    // Below members are added for IRADIO v1.4,
    // but not included to this structure
    // because of variable about CARRIER_INFO Lists.
    //BYTE allowed_carrier_prioritized;
    //BYTE multi_sim_lock_policy;    // sit_multi_sim_lock_policy_e_type
}sit_sim_set_carrier_restriction_req;

typedef null_data_format sit_sim_set_carrier_restriction_rsp;

/*
    SIT_GET_CARRIER_RESTRICTIONS (RCM ID = 0x072F)
*/
typedef null_data_format sit_sim_get_carrier_restriction_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 allowed_carriers_len;
    INT32 excluded_carriers_len;
    SIT_CARRIER_INFO carrier_list[0];
    // Below members are added for IRADIO v1.4,
    // but not included to this structure
    // because of variable about CARRIER_INFO Lists.
    //BYTE allowed_carrier_prioritized;
    //BYTE multi_sim_lock_policy;    // sit_multi_sim_lock_policy_e_type
}sit_sim_get_carrier_restriction_rsp;

/*
    SIT_GET_MANUAL_BAND_MODE (RCM ID = 0x0730)
*/
typedef null_data_format sit_misc_get_manual_band_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE mode_set_status;
    BYTE supported_lte[32];
    BYTE supported_wcdma[8];
    BYTE supported_gsm;
    BYTE supported_cdma[2];
    BYTE current_lte[32];
    BYTE current_wcdma[8];
    BYTE current_gsm;
    BYTE current_cdma[2];
}sit_misc_get_manual_band_mode_rsp;

/*
    SIT_SET_MANUAL_BAND_MODE (RCM ID = 0x0731)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode_set;
    BYTE lte[32];
    BYTE wcdma[8];
    BYTE gsm;
    BYTE cdma[2];
}sit_misc_set_manual_band_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE cause;
}sit_misc_set_manual_band_mode_rsp;

/*
    SIT_GET_RF_DESENSE_MODE (RCM ID = 0x0732)
*/
typedef null_data_format sit_misc_get_rf_desense_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE desense_status;
    BYTE rat;
}sit_misc_get_rf_desense_mode_rsp;

/*
    SIT_SET_RF_DESENSE_MODE (RCM ID = 0x0733)
*/
typedef struct
{
    RCM_HEADER hdr;
    sit_rf_desense_info_type rfDesenseInfos;
}sit_misc_set_rf_desense_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE cause;
}sit_misc_set_rf_desense_mode_rsp;

/*
    SIT_START_SCANNING_NETWORKS (RCM ID = 0x0734)
*/
#define MAX_NETWORK_SCAN_SPECIFIER      8
#define MAX_NETWORK_SCAN_BAND           8
#define MAX_NETWORK_PLMN_IDS            20
#define MAX_NETWORK_SCAN_CHANNEL        32

typedef struct {
    BYTE network_type;    // sit_net_ran_e_type
    UINT32 num_band;
    BYTE bands[MAX_NETWORK_SCAN_BAND];
    BYTE num_channel;
    UINT16 channels[MAX_NETWORK_SCAN_CHANNEL];
} SIT_NET_SCAN_SPECIFIER;

typedef struct {
    char mcc[3];
    char mnc[3];
} SIT_NET_SCAN_PLMN_ID;

#define MAX_NETWORK_SCAN_DATA ((sizeof(SIT_NET_SCAN_SPECIFIER) * MAX_NETWORK_SCAN_SPECIFIER) + (sizeof(SIT_NET_SCAN_PLMN_ID) * MAX_NETWORK_PLMN_IDS))

typedef struct
{
    RCM_HEADER hdr;
    BYTE scan_type;
    UINT16 interval;
    UINT16 max_search_time;
    BYTE incremental_results;
    UINT16 periodicity;
    BYTE num_record;
    BYTE num_plmn;
    char data[MAX_NETWORK_SCAN_DATA];
}sit_net_start_scanning_network;

/*
    SIT_STOP_SCANNING_NETWORKS (RCM ID = 0x0735)
*/
typedef null_data_format sit_net_stop_scanning_network;

/*
    SIT_IND_SCANNING_NETWORKS (RCM ID = 0x0736)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE scan_status;
    INT32 cell_info_num;
    char cell_info_list[0];
}sit_net_scanning_network_ind;

/*
    SIT_IND_RADIO_CAPABILITY (RCM ID = 0x0737)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 version;
    INT32 session_id;
    INT32 phase;  // same to RadioCapabilityPhase
    INT32 rc_raf;
    BYTE uuid[SIT_MAX_UUID_LENGTH];
    INT32 status;  // same to RadioCapabilityStatus
}sit_net_radio_capability_ind;

/*
    SIT_GET_MANUAL_RAT_MODE (RCM ID = 0x0738)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE manual_rat_mode_set;
    INT32 rat;
}sit_net_get_manual_rat_mode_rsp;

/*
    SIT_SET_MANUAL_RAT_MODE (RCM ID = 0x0739)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE rat_mode_set;
    INT32 rat;
}sit_net_set_manual_rat_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE cause;
}sit_net_set_manual_rat_mode_rsp;

/*
    SIT_GET_FREQUENCY_LOCK (RCM ID = 0x073A)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE freq_mode_set;     // sit_net_enable_e_type
    BYTE rat;               // sit_freq_lock_rat_e_type
    INT32 lte_pci;
    INT32 lte_earfcn;
    INT32 gsm_arfcn;
    INT32 wcdma_psc;
    INT32 wcdma_uarfcn;
}sit_net_get_freq_lock_rsp;

/*
    SIT_SET_FREQUENCY_LOCK (RCM ID = 0x073B)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE freq_mode_set;
    BYTE rat;
    INT32 lte_pci;
    INT32 lte_earfcn;
    INT32 gsm_arfcn;
    INT32 wcdma_psc;
    INT32 wcdma_uarfcn;
} sit_net_set_freq_lock_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_net_set_freq_lock_rsp;

/*
    SIT_IND_B2_B1_CONFIG (RCM ID = 0x073C)
*/
typedef struct
{
    BYTE event_type;
    BYTE rat_type;
    BYTE eutra_threshold_type;
    INT16 eutra_threshold;
    BYTE utra_threshold_type;
    INT16 utra_threshold;
    INT16 geran_threshold;
    INT16 cdma_threshold;
    INT16 hysteresis;
    INT16 time_to_trigger;
}b2_b1_configuration_type;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE config_list_num;
    b2_b1_configuration_type b2_b1_configuration[0];
}sit_net_b2_b1_config_ind;

/*
    SIT_SET_ENDC_MODE (RCM ID = 0x073D)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;   // sit_net_enable_e_type
    BYTE immediate_scg_rel;    // sit_endc_release_e_type
}sit_net_set_endc_mode_req;

/*
    SIT_GET_ENDC_MODE (RCM ID = 0x073E)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
}sit_net_get_endc_mode_rsp;

/*
   SIT_IND_SCG_BEARER_ALLOCATION (RCM ID = 0x073F)
     - deprecated: legacy before AOSP Q
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE rat;           // sit_rat_type_e_type
    BYTE scg_status;    // 0x00 : No SCG, 0x01: SCG added
}sit_net_sgc_bearer_allocation_ind;

/*
    SIT_SET_PREFERRED_DATA_MODEM (RCM ID = 0x0740)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE stackId;
}sit_net_set_preferred_data_modem_req;
typedef null_data_format sit_net_set_preferred_data_modem_rsp;

/*
   SIT_IND_FREQUENCY_INFO (RCM ID = 0x0741)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE primary_rat;
    UINT32 primary_band;
    UINT32 primary_frequency;
    BYTE secondary_rat;
    UINT32 secondary_band;
    UINT32 secondary_frequency;
}sit_net_frequency_info_ind;

/*
    SIT_IND_PHYSICAL_CHANNEL_CONFIG (RCM ID = 0x0742)
 */
#define MAX_SIT_PHYSICAL_CHANNEL_CONFIGS 16
#define MAX_SIT_CONTEXT_ID_LEN 9

typedef struct {
    BYTE cell_status;
    INT32 cell_bandwidth_downlink;
    BYTE rat;
    BYTE frequency_range;
    INT32 channel;
    BYTE context_len;
    int context_id[MAX_SIT_CONTEXT_ID_LEN];
    UINT32 physical_cellid;
} sit_physical_channel_config;

typedef struct
{
    RCM_IND_HEADER hdr;
    int config_len;
    sit_physical_channel_config configs[MAX_SIT_PHYSICAL_CHANNEL_CONFIGS];
}sit_net_physical_channel_config_ind;

typedef struct {
    BYTE cell_status;
    INT32 cell_bandwidth_downlink;
    BYTE rat;
    BYTE frequency_range;
    INT32 channel;
    BYTE context_len;
    int context_id[MAX_SIT_CONTEXT_ID_LEN];
    UINT32 physical_cellid;
    INT32 channel_uplink;
    INT32 cell_bandwidth_uplink;
    INT16 band_number;
} sit_physical_channel_config_v1_6;

typedef struct
{
    RCM_IND_HEADER hdr;
    int config_len;
    sit_physical_channel_config_v1_6 configs[MAX_SIT_PHYSICAL_CHANNEL_CONFIGS];
}sit_net_physical_channel_config_ind_v1_6;

/*
    SIT_SET_LOCATION_UPDATE_SETTING (RCM ID = 0x0744)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE update_setting;
}sit_set_location_update_setting_req;

/*
    SIT_GET_FREQUENCY_INFO (RCM ID = 0x0746)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE primary_rat;
    UINT32 primary_band;
    UINT32 primary_frequency;
    BYTE secondary_rat;
    UINT32 secondary_band;
    UINT32 secondary_frequency;
}sit_net_get_frequency_info_rsp;

/*
    SIT_IND_RRC_INFO (RCM ID = 0x0747)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE rat;
    BYTE state;
}sit_net_rrc_info_ind;

/*
    SIT_SET_NR_MODE (RCM ID = 0x0748)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;    // sit_nr_mode_e_type
}sit_net_set_nr_mode_req;

/*
    SIT_GET_ENDC_MODE (RCM ID = 0x0749)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;    // sit_nr_mode_e_type
}sit_net_get_nr_mode_rsp;

/*
    SIT_IND_REG_FAILED (RCM ID = 0x074A)
*/
typedef struct
{
    BYTE cell_info_type;    //sit_net_cell_info_type_e_type
    union
    {
        cell_identity_gsm gsm;
        cell_identity_cdma cdma;
        cell_identity_lte lte;
        cell_identity_wcdma wcdma;
        cell_identity_tdscdma tdscdma;
        cell_identity_nr nr;
    };
}sit_net_cell_identity;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE plmn[MAX_PLMN_LEN];
    BYTE service_domain;        // sit_service_domain_type
    BYTE rej_cause;
    BYTE rej_cause_additional;
    char cell_identity[0];      // variable according to the cellIdentityVer
}sit_net_reg_failed_ind;

/*
    SIT_IND_BARRING_INFO_CHANGED (RCM ID = 0x074B)
*/
typedef struct
{
    INT32 service_type;
    INT32 barring_type;
    INT32 barring_factor;
    INT32 time_second;
    BYTE is_barred;
}sit_barring_info;

#define MIN_BARRING_INFO_DATA (sizeof(sit_net_cell_identity) + 4 + (sizeof(sit_barring_info)))
typedef struct
{
    RCM_IND_HEADER hdr;
    // sit_net_cell_identity + number_of_barring_info(4 byte) + barring_info_list(variable)
    BYTE barring_info_data[MIN_BARRING_INFO_DATA];
}sit_net_barring_chagned_ind;

/*
    SIT_SET_SYSTEM_SELECTION_CHANNELS (RCM ID = 0x074C)
*/
#define SIT_MAX_NO_SSC_GERAN_BANDS (14)
#define SIT_MAX_NO_SSC_UTRAN_BANDS (26)
#define SIT_MAX_NO_SSC_EUTRAN_BANDS (59)
#define SIT_MAX_NO_SSC_NGRAN_BANDS (52)
#define SIT_MAX_NO_SSC_GERAN_CHANNELS (100)
#define SIT_MAX_NO_SSC_UTRAN_CHANNELS (32)
#define SIT_MAX_NO_SSC_EUTRAN_CHANNELS (100)
#define SIT_MAX_NO_SSC_NGRAN_CHANNELS (100)
typedef struct
{
    BYTE radio_access_network;    // sit_net_ran_e_type
    BYTE bands_len;
    union {
        UINT16 geran_bands[SIT_MAX_NO_SSC_GERAN_BANDS];
        UINT16 utran_bands[SIT_MAX_NO_SSC_UTRAN_BANDS];
        UINT16 eutran_bands[SIT_MAX_NO_SSC_EUTRAN_BANDS];
        UINT16 ngran_bands[SIT_MAX_NO_SSC_NGRAN_BANDS];
    } bands;
    BYTE channels_len;
    union {
        UINT32 geran_channels[SIT_MAX_NO_SSC_GERAN_CHANNELS];
        UINT32 utran_channels[SIT_MAX_NO_SSC_UTRAN_CHANNELS];
        UINT32 eutran_channels[SIT_MAX_NO_SSC_EUTRAN_CHANNELS];
        UINT32 ngran_channels[SIT_MAX_NO_SSC_NGRAN_CHANNELS];
    } channels;
} sit_net_radio_access_specifier;

typedef struct
{
    RCM_HEADER hdr;
    BYTE is_specific_channel;
    BYTE radio_access_specifier_len;
    sit_net_radio_access_specifier radio_access_specifier_array[SIT_MAX_NO_RAS];    // variable length field.
}sit_net_set_system_selection_channels_req;

typedef null_data_format sit_set_system_selection_channels_rsp;

/*
    SIT_GET_BARRING_INFOS (RCM ID = 0x074D)
*/
typedef null_data_format sit_net_get_barring_infos_req;

typedef struct
{
    RCM_HEADER hdr;
    // sit_net_cell_identity + number_of_barring_info(4 byte) + barring_info_list(variable)
    BYTE barring_info_data[MIN_BARRING_INFO_DATA];
}sit_net_get_barring_infos_rsp;

/*
    SIT_GET_SYSTEM_SELECTION_CHANNELS (RCM ID = 0x074E)
*/
typedef null_data_format sit_net_get_system_selection_channels_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE radio_access_specifier_len;
    sit_net_radio_access_specifier radio_access_specifier_array[SIT_MAX_NO_RAS];    // variable length field.
}sit_net_get_system_selection_channels_rsp;

/*
    SIT_SET_ALLOW_NETWORK (RCM ID = 0x074F)
*/
typedef struct
{
    RCM_HEADER hdr;
    int rat;
}sit_net_set_allow_network_req;

/*
    SIT_GET_ALLOW_NETWORK (RCM ID = 0x0750)
*/
typedef null_data_format sit_net_get_allow_network_req;

typedef struct
{
    RCM_HEADER hdr;
    int rat;
}sit_net_get_allow_network_rsp;

/*
    SIT_SET_PLMN_LIST (RCM ID = 0x0751)
*/

/*
    SIT_GET_PLMN_LIST (RCM ID = 0x0752)
*/

/*
    SIT_IND_DSDS_STATUS (RCM ID = 0x0753)
*/
enum {
    DSDS_STATUS_RESUME = 0,
    DSDS_STATUS_SUSPEND = 1,
};

typedef struct
{
    RCM_IND_HEADER hdr;
    uint8_t status;
}sit_net_dsds_status_ind;

/*
  SIT_SET_TRIGGER_EPSFB (RCM ID = 0x0756)
*/
typedef struct {
    RCM_HEADER hdr;
    unsigned char epsFallbackReason;
} sit_set_trigger_epsfb_req;

typedef struct {
    RCM_IND_HEADER hdr;
    uint8_t result;
    uint8_t rat;
} sit_set_trigger_epsfb_ind;

/*
    SIT_SET_RADIO_POWER (RCM ID = 0x0800)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 radio_state;        // sit_pwr_radio_state_e_type
    BYTE for_emc;             // true/false
    BYTE prefered_for_emc;    // true/false
}sit_pwr_set_radio_power_req;

typedef null_data_format sit_pwr_radio_power_rsp;

/*
    SIT_GET_RADIO_POWER (RCM ID = 0x0801)
*/
typedef struct
{
    RCM_HEADER hdr;
}sit_pwr_get_radio_power_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 radio_state;    // sit_pwr_radio_sim_state_e_type
}sit_pwr_get_radio_power_rsp;

/*
    SIT_IND_RADIO_STATE_CHANGED (RCM ID = 0x0802)
*/
//typedef null_ind_data_format sit_pwr_radio_state_changed_ind;
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 radio_state;    //sit_pwr_radio_state_e_type
}sit_pwr_radio_state_changed_ind;

/*
    SIT_IND_RADIO_READY (RCM ID = 0x0803)
*/
typedef null_ind_data_format sit_pwr_radio_ready_ind;

/*
    SIT_IND_PHONE_RESET (RCM ID = 0x0804)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE reset_type;
    BYTE reset_cause;
}sit_pwr_phone_reset_ind;

/*
    SIT_IND_MODEM_RESTART (RCM ID = 0x0805)
*/
#define SIT_PWR_MODEM_RESTART_REASON_SIZE (50)
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE reason[SIT_PWR_MODEM_RESTART_REASON_SIZE];
}sit_pwr_modem_restart_ind;   // this command is not used by CP anymore as per JSON doc

/*
    SIT_SET_STACK_STATUS (RCM ID = 0x080F)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
}sit_pwr_set_stack_status_req;

typedef null_data_format sit_pwr_set_stack_status_rsp;

/*
    SIT_GET_STACK_STATUS (RCM ID = 0x0810)
*/
typedef null_data_format sit_pwr_get_stack_status_req;
typedef sit_pwr_set_stack_status_req sit_pwr_get_stack_status_rsp;

/*
    SIT_GET_SIGNAL_STRENGTH (RCM ID = 0x0900)
*/
typedef null_data_format sit_misc_get_signal_strength_req;

typedef struct {
    INT32 sig_str;
    INT32 ber;
} GW_SIGNAL_STRENGTH;

typedef struct {
    INT32 dbm;
    INT32 ecio;
} CDMA_SIGNAL_STRENGTH;

typedef struct {
    INT32 dbm;
    INT32 ecio;
    INT32 snr;
} EVDO_SIGNAL_STRENGTH;

typedef struct {
    INT32 sig_str;
    INT32 rsrp;
    INT32 rsrq;
    INT32 rssnr;
    INT32 cqi;
    INT32 timing_adv;
} LTE_SIGNAL_STRENGTH;

typedef struct {
    INT32 sig_str;
    INT32 rsrp;
    INT32 rsrq;
    INT32 rssnr;
    UINT32 cqi_table_index;
    INT32 cqi;
    INT32 timing_adv;
} LTE_SIGNAL_STRENGTH_V4;

typedef struct {
    INT32 rscp;
} TD_SCDMA_SIGNAL_STRENGTH;

typedef struct {
    UINT32 sig_str;
    UINT32 ber;
    UINT32 rscp;
} TD_SCDMA_SIGNAL_STRENGTH_V2;

typedef struct {
    INT32 ecno;
} UMTS_SIGNAL_STRENGTH;

typedef struct {
    INT32 sig_str;
    INT32 ber;
    INT32 ta;
} GSM_SIGNAL_STRENGTH;

typedef struct {
    INT32 sig_str;
    INT32 ber;
    UINT32 rscp;
    UINT32 ecno;
} WCDMA_SIGNAL_STRENGTH;

typedef struct {
    INT32 ss_rsrp;
    INT32 ss_rsrq;
    INT32 ss_sinr;
    INT32 csi_rsrp;
    INT32 csi_rsrq;
    INT32 csi_sinr;
} NR_SIGNAL_STRENGTH;

typedef struct {
    INT32 ss_rsrp;
    INT32 ss_rsrq;
    INT32 ss_sinr;
    INT32 csi_rsrp;
    INT32 csi_rsrq;
    INT32 csi_sinr;
    UINT32 cqi_table_index;
    INT32 wb_cqi;
    UINT32 sb_cqi[SIT_MAX_NR_SUBBAND_INDEX];
} NR_SIGNAL_STRENGTH_V4;

typedef struct {
    GW_SIGNAL_STRENGTH GW_SignalStrength;
    CDMA_SIGNAL_STRENGTH CDMA_SignalStrength;
    EVDO_SIGNAL_STRENGTH EVDO_SignalStrength;
    LTE_SIGNAL_STRENGTH LTE_SignalStrength;
    TD_SCDMA_SIGNAL_STRENGTH TD_SCDMA_SignalStrength;
    UMTS_SIGNAL_STRENGTH UMTS_SignalStrength;
} SIGNAL_STRENGTH;

typedef struct {
    GSM_SIGNAL_STRENGTH GSM_SignalStrength;
    WCDMA_SIGNAL_STRENGTH WCDMA_SignalStrength;
    CDMA_SIGNAL_STRENGTH CDMA_SignalStrength;
    EVDO_SIGNAL_STRENGTH EVDO_SignalStrength;
    TD_SCDMA_SIGNAL_STRENGTH_V2 TD_SCDMA_SignalStrength;
    LTE_SIGNAL_STRENGTH LTE_SignalStrength;
    NR_SIGNAL_STRENGTH NR_SignalStrength;
} SIGNAL_STRENGTH_V2;

typedef struct {
    GSM_SIGNAL_STRENGTH GSM_SignalStrength;
    WCDMA_SIGNAL_STRENGTH WCDMA_SignalStrength;
    CDMA_SIGNAL_STRENGTH CDMA_SignalStrength;
    EVDO_SIGNAL_STRENGTH EVDO_SignalStrength;
    TD_SCDMA_SIGNAL_STRENGTH_V2 TD_SCDMA_SignalStrength;
    LTE_SIGNAL_STRENGTH_V4 LTE_SignalStrength;
    NR_SIGNAL_STRENGTH_V4 NR_SignalStrength;
} SIGNAL_STRENGTH_V4;

typedef struct
{
    RCM_HEADER hdr;
    INT16 valid_rat_sig_flag;
    SIGNAL_STRENGTH sig_strength;
}sit_misc_signal_strength_rsp;

typedef struct
{
    RCM_HEADER hdr;
    INT16 valid_rat_sig_flag;
    SIGNAL_STRENGTH_V2 sig_strength;
}sit_misc_signal_strength_rsp_v2;

typedef struct
{
    RCM_HEADER hdr;
    INT16 valid_rat_sig_flag;
    SIGNAL_STRENGTH_V4 sig_strength;
}sit_misc_signal_strength_rsp_v4;

typedef struct
{
    RCM_IND_HEADER hdr;
    INT16 valid_rat_sig_flag;
    SIGNAL_STRENGTH sig_strength;
}sit_misc_signal_strength_ind;

typedef struct
{
    RCM_IND_HEADER hdr;
    INT16 valid_rat_sig_flag;
    SIGNAL_STRENGTH_V2 sig_strength;
}sit_misc_signal_strength_ind_v2;

typedef struct
{
    RCM_IND_HEADER hdr;
    INT16 valid_rat_sig_flag;
    SIGNAL_STRENGTH_V4 sig_strength;
}sit_misc_signal_strength_ind_v4;

typedef enum
{
    SIT_MISC_SIG_RAT_SIG_GW = 0x01,
    SIT_MISC_SIG_RAT_SIG_CDMA = 0x02,
    SIT_MISC_SIG_RAT_SIG_EVDO = 0x04,
    SIT_MISC_SIG_RAT_SIG_LTE = 0x08,
    SIT_MISC_SIG_RAT_SIG_TD_SCDMA = 0x10,
    SIT_MISC_SIG_RAT_SIG_NR = 0x20, // TO DO:: SYNC with CP SIT difinition
}sit_misc_get_signal_strength_rat_sig_e_type;

/* RSSI  field */
/* Radio signal strength */
typedef enum
{
    DISP_RSSI_0,     /* 0x00 */
    DISP_RSSI_1,     /* 0x01 */
    DISP_RSSI_2,     /* 0x02 */
    DISP_RSSI_3,     /* 0x03 */
    DISP_RSSI_4,     /* 0x04 */
    DISP_RSSI_5,     /* 0x05 */
    DISP_RSSI_6,     /* 0x06 */
    DISP_RSSI_MAX
}sit_misc_disp_rssi_e_type;

/*
    SIT_GET_BASEBAND_VERSION (RCM ID = 0x0901)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE ver_mask;
}sit_misc_get_baseband_version_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE ver_mask;
    char sw_version[32];
    char hw_version[32];
    char rf_cal_date[32];
    char product_code[32];
    char model_id[17];
    BYTE prl_nam_num;
    BYTE prl_version[51];
    BYTE eri_nam_num;
    BYTE eri_version[51];
    BYTE cp_chipsetname[16];
}sit_misc_get_baseband_version_rsp;

typedef enum
{
    SIT_MISC_VER_MASK_SW_VERSION = 0x01,
    SIT_MISC_VER_MASK_HW_VERSION = 0x02,
    SIT_MISC_VER_MASK_RF_CAL_DATA = 0x04,
    SIT_MISC_VER_MASK_PRODUCT_CODE = 0x08,
    SIT_MISC_VER_MASK_MODEL_ID = 0x10,    /*CDMA only*/
    SIT_MISC_VER_MASK_PRL = 0x20,    /*CDMA only*/
    SIT_MISC_VER_MASK_ERI = 0x40,    /*CDMA only*/
    SIT_MISC_VER_MASK_CP_CHIPSET = 0x80,
    SIT_MISC_VER_MASK_ALL = 0xFF
}sit_misc_baseband_version_mask_e_type;

/*
    SIT_SET_SCREEN_STATE (RCM ID = 0x0902)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 screen_state; // sit_misc_screen_state_e_type
}sit_misc_set_screen_state_req;

typedef null_data_format sit_misc_set_screen_state_rsp;

/*
    SIT_SET_TTY_MODE (RCM ID = 0x0903)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 tty_mode; // sit_misc_tty_mode_e_type
}sit_misc_set_tty_mode_req;

typedef null_data_format sit_misc_set_tty_mode_rsp;

/*
    SIT_GET_TTY_MODE (RCM ID = 0x0904)
*/
typedef null_data_format sit_misc_get_tty_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 tty_mode; // sit_misc_tty_mode_e_type
}sit_misc_get_tty_mode_rsp;

/*
    SIT_IND_NITZ_TIME_RECEIVED (RCM ID = 0x0905)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE daylight_valid;
    BYTE year;
    BYTE month;
    BYTE day;
    BYTE hour;
    BYTE minute;
    BYTE second;
    BYTE time_zone;
    BYTE daylight_adjust; // sit_misc_nitz_daylight_adjust_e_type
    BYTE day_of_week; // sit_misc_nitz_day_of_week_e_type
    BYTE mminfo;
    BYTE plmn[MAX_PLMN_LEN];
}sit_misc_nitz_time_received_ind;

struct sit_nitz_time {
    BYTE daylight_valid;
    BYTE year;
    BYTE month;
    BYTE day;
    BYTE hour;
    BYTE minute;
    BYTE second;
    BYTE time_zone;
    BYTE daylight_adjust;
    BYTE day_of_week;
    BYTE mminfo;
    BYTE plmn[MAX_PLMN_LEN];
};

struct sit_nitz_time_with_age : sit_nitz_time {
    int64_t age;
};

typedef struct
{
    RCM_IND_HEADER hdr;
    sit_nitz_time_with_age nitzData;
}sit_misc_nits_time_received_with_age_ind;

/*
    SIT_SET_CFG_DEFAULT (RCM ID = 0x0907)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE type;
}sit_misc_set_cfg_default_req;
typedef null_data_format sit_misc_set_cfg_default_rsp;

typedef enum
{
    SIT_MISC_CFG_DEFAULT_REASON_FACTORY_RESET = 0,
    SIT_MISC_CFG_DEFAULT_REASON_NET_REGI_RESET = 1,
    SIT_MISC_CFG_DEFAULT_REASON_MODEM_RESET = 2,
}sit_misc_cfg_default_e_type;

/*
    SIT_SET_ENG_MODE (RCM ID = 0x0908)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE eng_mode;
}sit_misc_set_eng_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE eng_mode;
    BYTE sub_mode;
}sit_misc_set_eng_mode_ex_req;

/*
    SIT_SET_SCREEN_LINE (RCM ID = 0x0909)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE scr_line;
}sit_misc_set_scr_line_req;

/*
    SIT_SET_DEBUG_TRACE (RCM ID = 0x090B)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE debug_trace;
}sit_misc_set_debug_trace_req;

/*
    SIT_GET_ACTIVITY_INFO (RCM ID = 0x090C)
*/
typedef null_data_format sit_misc_get_activity_info_req;

typedef struct {
    RCM_HEADER hdr;
    UINT32 sleep_mode_time_ms;
    UINT32 idle_mode_time_ms;
    UINT32 tx_mode_time_ms[RIL_NUM_TX_POWER_LEVELS];
    UINT32 rx_mode_time_ms;
}sit_misc_get_activity_info_rsp;

enum {
    TX_MODE_RANGE_LESS_0,
    TX_MODE_RANGE_0_5,
    TX_MODE_RANGE_5_15,
    TX_MODE_RANGE_15_20,
    TX_MODE_RANGE_GREATER_20,
    TX_MODE_RANGE_MAX,
};

// Support IRadio 2.0
// RAT speicific stats
struct sit_rat_statiscts {
    int32_t txModeTime[TX_MODE_RANGE_MAX];
    int32_t rxModeTime;
};

enum {
    ACTIVITY_INFO_RAT_GSM,
    ACTIVITY_INFO_RAT_UMTS,
    ACTIVITY_INFO_RAT_LTE,
    ACTIVITY_INFO_RAT_NR_SUB6,
    ACTIVITY_INFO_RAT_NR_MMWAVE,
    ACTIVITY_INFO_RAT_MAX,
};

struct sit_modem_activity_info {
    int32_t sleepModeTime;
    int32_t idleModeTime;
    sit_rat_statiscts ratSpecificInfo[ACTIVITY_INFO_RAT_MAX];
};

typedef struct {
    RCM_HEADER hdr;
    sit_modem_activity_info activity_info;
}sit_misc_get_activity_info_rat_specific_rsp;

/*
    SIT_SET_OPERATOR_INFO = 0x090D
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE plmn[MAX_PLMN_LEN];
    UINT32 openCarrierIndex;
}sit_misc_set_operator_info_req;

/*
    SIT_SET_FEATURE_INFO (RCM ID = 0x090E)
*/
typedef struct
{
    RCM_HEADER hdr;
    int feature;
}sit_set_feature_info_req;

typedef null_data_format sit_set_feature_info_rsp;

/*
    SIT_GET_CDMA_SUBSCRIPTION (RCM ID = 0x090F)
*/
typedef null_data_format sit_misc_get_cdma_subscription_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE mdn_size;
    BYTE mdn[MAX_CDMA_MDN_LEN];
    BYTE min[MAX_CDMA_MIN_LEN];
    WORD sid;
    WORD nid;
    UINT32 prl_version;
}sit_misc_get_cdma_subscription_rsp;

/*
    SIT_SET_ENG_STRING_INPUT (RCM ID = 0x0910)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE len;
    char input[255];
}sit_misc_set_eng_string_input_req;

/*
    SIT_GET_MSL_CODE (RCM ID = 0x0911)
*/
typedef null_data_format sit_misc_get_msl_code_req;

typedef struct {
    RCM_HEADER hdr;
    char msl_code[6];
} sit_misc_get_msl_code_rsp;

/*
    SIT_SET_PREFERRED_CALL_CAPABILITY (RCM ID = 0x091A)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 mode;
}sit_misc_set_preferred_call_capability_req;

typedef null_data_format sit_misc_set_preferred_call_capability_rsp;

/*
    SIT_SET_PIN_CONTROL (RCM ID = 0x0920)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE signal;
    BYTE status;
}sit_misc_set_pin_control_req;

typedef null_data_format sit_misc_set_pin_control_rsp;

/*
    SIT_IND_PIN_CONTROL (RCM ID = 0x0921)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE signal;
    BYTE status;
}sit_misc_pin_control_ind;

/*
    SIT_SET_VOICE_OPERATION (RCM ID = 0x091A)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT32 mode;
}sit_misc_set_voice_operation_req;

typedef null_data_format sit_misc_set_voice_operation_rsp;

/*
    SIT_GET_VOICE_OPERATION (RCM ID = 0x091B)
*/
typedef null_data_format sit_misc_get_voice_operation_req;

typedef struct {
    RCM_HEADER hdr;
    UINT32 mode;
}sit_misc_get_voice_operation_rsp;


/*
    SIT_SET_DEVICE_INFO (RCM ID = 0x0922)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE model_name[MAX_MODEL_NAME_LEN];
    BYTE sw_version[MAX_BB_SW_VER_LEN];
    BYTE product_name[MAX_BB_PRODUCT_CODE_LEN];
    BYTE ap_os_version[MAX_AP_OS_VERSION_LEN];
}sit_misc_set_device_info_req;

typedef struct {
    RCM_HEADER hdr;
    char sit_version_info[0];
}sit_misc_set_device_info_rsp;

/*
    SIT_GET_HW_CONFIG (RCM ID = 0x0926)
*/
#define SIT_MAX_HW_CONFIG (2)
typedef null_data_format sit_misc_get_hw_config_req;

typedef struct
{
    INT32 ril_model;
    UINT32 rat;   // same to RIL_RadioAccessFamilyForCp
    INT32 max_voice;
    INT32 max_data;
    INT32 max_standby;
}SIT_HW_CFG_MODEM;

typedef struct
{
    char modem_uuid[SIT_MAX_UUID_LENGTH];
}SIT_HW_CFG_SIM;

typedef struct
{
    BYTE type;
    char uuid[SIT_MAX_UUID_LENGTH];
    BYTE state;
    union {
        SIT_HW_CFG_MODEM modem;
        SIT_HW_CFG_SIM sim;
    } cfg;
}SIT_HW_CONFIG;

typedef struct
{
    RCM_HEADER hdr;
    BYTE num_recodrs;
    SIT_HW_CONFIG hw_config[SIT_MAX_HW_CONFIG];
}sit_misc_get_hw_config_rsp;

/*
    SIT_IND_HW_CONFIG_CHANGED (RCM ID = 0x0927)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE num_recodrs;
    SIT_HW_CONFIG hw_config[SIT_MAX_HW_CONFIG];
}sit_misc_hw_config_change_ind;

/*
    SIT_SET_IND_CMD_FILTER (RCM ID = 0x0928)
*/
typedef enum
{
    SIT_MISC_IND_FILTER_SIGNAL_STRENGTH = (1 << 0),
    SIT_MISC_IND_FILTER_NTW_STATE_CHANGED = (1 << 1),
    SIT_MISC_IND_FILTER_DATA_CALL_LIST_CHANGED = (1 << 2),
} sit_misc_ind_cmd_filter_type;

typedef struct
{
    RCM_HEADER hdr;
    UINT ind_cmd_filter;    // same to RIL_IndicationFilter
}sit_misc_set_ind_cmd_filter_req;

/*
    SIT_IND_CDMA_PRL_CHANGED (RCM ID = 0x0929)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 prl_ver;
}sit_misc_cdma_prl_change_ind;

/*
 *   SIT_IND_SAR_CONTROL_STATE (RCM ID = 0x092A)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE device_state;
}sit_misc_psensor_sar_control_state_ind;

/*
 *   SIT_SET_SENSOR_STATUS (RCM ID = 0x092B)
 */
typedef struct
{
    RCM_HEADER hdr;
    INT32 psensor_status;
}sit_misc_psensor_set_psensor_status_req;

/*
    SIT_SET_RSSI_SCAN (RCM ID = 0x092D)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE rat;
    BYTE band;
    BYTE rbw;
    BYTE scan_mode;
    UINT16 start_frequency;
    UINT16 end_frequency;
    BYTE step;
    BYTE antenna_selection;
    UINT16 sampling_count;
    BYTE tx1;
    BYTE tx1_band;
    BYTE tx1_bw;
    UINT16 tx1_freq;
    UINT16 tx1_power;
    BYTE tx1_rb_num;
    BYTE tx1_rb_offset;
    BYTE tx1_mcs;
    BYTE tx2;
    BYTE tx2_band;
    BYTE tx2_bw;
    UINT16 tx2_freq;
    UINT16 tx2_power;
    BYTE tx2_rb_num;
    BYTE tx2_rb_offset;
    BYTE tx2_mcs;
    BYTE reserved[20];
}sit_misc_set_rssi_scan_req;
typedef null_data_format sit_misc_set_rssi_scan_rsp;

/*
    SIT_IND_RSSI_SCAN (RCM ID = 0x092E)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE total_page;
    BYTE current_page;
    UINT16 start_frequency;
    UINT16 end_frequency;
    BYTE step;
    INT16 result[MAX_RSSI_SCAN_RESULT];
}sit_misc_rssi_scan_ind;

/*
    SIT_GET_PREFERRED_CALL_CAPABILITY (RCM ID = 0x0930)
*/
typedef null_data_format sit_misc_get_preferred_call_capability_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 mode;
}sit_misc_get_preferred_call_capability_rsp;

typedef enum
{
    SIT_PREF_CALL_CAPABILITY_CS_ONLY = 0x00,
    SIT_PREF_CALL_CAPABILITY_PS_ONLY = 0x01,
    SIT_PREF_CALL_CAPABILITY_CS_PREFERRED = 0x02,
    SIT_PREF_CALL_CAPABILITY_PS_PREFERRED = 0x03,
}sit_misc_preferred_call_capability_e_type;

/*
    SIT_SET_FORWARDING_AT_COMMAND (RCM ID = 0x0931)
*/
typedef struct {
    RCM_HEADER hdr;
    unsigned short length;
    char data[MAX_SIT_AT_COMMAND_LENGTH + 1];
}sit_misc_forwarding_at_command_req;
typedef null_data_format sit_misc_forwarding_at_command_rsp;

/*
    SIT_IND_FORWARDING_AT_COMMAND (RCM ID = 0x0932)
*/
typedef struct {
    RCM_IND_HEADER hdr;
    unsigned short length;
    char data[MAX_SIT_AT_COMMAND_LENGTH + 1];
}sit_misc_forwarding_at_command_ind;

#define MAX_RADIO_NODE_DATA_LEN    256

/*
    SIT_GET_RADIO_NODE (RCM ID = 0x0936)
*/
typedef struct {
    RCM_HEADER hdr;
    char path[MAX_RADIO_NODE_DATA_LEN];
}sit_misc_get_radio_node_req;

typedef struct {
    RCM_HEADER hdr;
    char value[MAX_RADIO_NODE_DATA_LEN];
}sit_misc_get_radio_node_rsp;

/*
    SIT_SET_RADIO_NODE (RCM ID = 0x0937)
*/
typedef struct {
    RCM_HEADER hdr;
    char path[MAX_RADIO_NODE_DATA_LEN];
    char value[MAX_RADIO_NODE_DATA_LEN];
}sit_misc_set_radio_node_req;

typedef null_data_format sit_misc_set_radio_node_rsp;

/*
    SIT_GET_VOLTE_PROVISION_UPDATE (RCM ID = 0x0938)
*/
typedef null_data_format sit_misc_get_volte_provision_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE status;
}sit_misc_get_volte_provision_rsp;

/*
    SIT_SET_VOLTE_PROVISION_UPDATE (RCM ID = 0x0939)
*/
typedef null_data_format sit_misc_set_volte_provision_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE result;
}sit_misc_set_volte_provision_rsp;

/*
   SIT_SET_INTPS_SERVICE  (RCM ID = 0x0933)
   DEPRECATED
 */
typedef enum
{
    SIT_MISC_ENABLE_INTPS_SERVICE = 0x01,   // enable default internet service
    SIT_MISC_DISABLE_INTPS_SERVICE = 0x00   // disable default internet service
} sit_misc_intps_service_type;

typedef struct
{
    RCM_HEADER hdr;
    INT32 mode;
} sit_misc_set_intps_service_req;

/*
   SIT_SET_SELFLOG (RCM ID = 0x093A)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE mode;
    BYTE size;
}sit_misc_set_selflog_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE result;
}sit_misc_set_selflog_rsp;

/*
   SIT_GET_SELFLOG_STATUS (RCM ID = 0x093B)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE status;
}sit_misc_get_selflog_status_rsp;

/*
   SIT_IND_SELFLOG_STATUS (RCM ID = 0x093C)
*/
typedef struct {
    RCM_IND_HEADER hdr;
    BYTE status;
}sit_misc_selflog_status_ind;

/*
   SIT_SET_ELEVATOR_SENSOR  (RCM ID = 0x093D)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE enable; // 0x00: Disable, 0x01: Enable
}sit_set_elevator_sensor_req;

/*
   SIT_SET_MODEM_CONFIG (RCM ID = 0x093F)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE config;
}sit_misc_set_modems_config_req;

enum {
    SIT_MODEM_CONFIG_SINGLE_SIM = 0,    // ss
    SIT_MODEM_CONFIG_MULTI_SIM = 1,     // dsds
};

/*
    SIT_SET_MODEM_LOG_DUMP (RCM ID = 0x0940)
*/
typedef struct {
    RCM_HEADER hdr;
    INT32 type;
}sit_misc_set_modem_log_dump_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE result;
}sit_misc_set_modem_log_dump_rsp;

/*
    SIT_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA (RCM ID = 0x0943)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 ms;
    INT32 db;
    BYTE len;
    INT32 dbm[MAX_DBM_LEN];
    INT32 radio_acc_net;    // sit_net_ran_e_type
    BYTE measure_type;
    BYTE enable;
}sit_misc_set_signal_strength_report_criteria_req;

/*
    SIT_SET_LINK_CAPACITY_REPORTING_CRITERIA (RCM ID = 0x0944)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 h_ms;
    INT32 h_dl_kbps;
    INT32 h_ul_kbps;
    BYTE t_dl_len;
    INT32 t_dl_kbps[MAX_UL_DL_KBPS_LEN];
    BYTE t_ul_len;
    INT32 t_ul_kbps[MAX_UL_DL_KBPS_LEN];
    INT32 radio_acc_net;
}sit_misc_set_link_capacity_report_criteria_req;

/*
    SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE (RCM ID = 0x0945)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 dl_capa_kbps;
    INT32 ul_capa_kbps;
    INT32 dcnr_sec_dl_capa_kbps;
    INT32 dcnr_sec_ul_capa_kbps;
}sit_misc_current_link_capa_estimate_ind;

typedef struct {
    INT32 dl_capa_kbps;
    INT32 ul_capa_kbps;
    INT32 dcnr_sec_dl_capa_kbps;
    INT32 dcnr_sec_ul_capa_kbps;
} CURRENT_LINK_CAPA_ESTIMATE;

/*
    SIT_SET_SELFLOG_PROFILE (RCM ID = 0x0942)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE data[0];
}sit_set_selflog_profile_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_set_selflog_profile_rsp;

/*
    SIT_SET_AP_SYSTEM_TIME (RCM ID = 0x0949)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE year;
    BYTE month;
    BYTE day;
    BYTE hour;
    BYTE minute;
    BYTE second;
}sit_set_ap_system_time_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_set_ap_system_time_rsp;

/*
    SIT_SET_NR_SILENCE_MODE (RCM ID = 0x094C)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE nsa_mode;
    BYTE sa_mode;
}sit_set_nr_silence_mode_req;

typedef null_data_format sit_set_nr_silence_mode_rsp;

/*
   SIT_SET_DATA_THROTTLING (RCM ID = 0x094D)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE throttling_action;
    int64_t completion_duration_millis;
}sit_set_data_throttling_req;

/**
 * SIT_SET_LAB_TEST_CONFIG (RCM ID = 0x094E)
 */
typedef struct
{
    RCM_HEADER hdr;
    INT32 lab_test_config;
    BYTE value;
}sit_oem_set_lab_test_config_req;

/*
    SIT_GET_NR_SILENCE_MODE (RCM ID = 0x094F)
*/
typedef null_data_format sit_get_nr_silence_mode_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE nsa_mode;
    BYTE sa_mode;
}sit_get_nr_silence_mode_rsp;

/*
    SIT_IND_RF_CONNECTION (RCM ID = 0x0950)
*/
        typedef struct {
    RCM_IND_HEADER hdr;
    BYTE rf_cable_state;
}sit_rf_connection_ind;

/*
    SIT_GET_VONR_CAPABILITY (RCM ID = 0x0953)
*/
typedef struct {
    RCM_HEADER hdr;
    INT32 vonr_capa;
}sit_net_get_vonr_capa_rsp;

/*
    SIT_SET_VONR_CAPABILITY (RCM ID = 0x0954)
*/
typedef struct {
    RCM_HEADER hdr;
    INT32 vonr_capa;
}sit_net_set_vonr_capa_req;

/*
    SIT_SET_UL_MIMO (RCM ID = 0x0955)
*/
typedef struct {
    RCM_HEADER hdr;
    INT32 disable_ul_mimo;
}sit_set_ul_mimo_req;

/*
 * SIT_SET_DEVICE_SERVICE (RCM ID = 0x0956)
 */
typedef struct {
    RCM_HEADER hdr;
    INT32 usage_setting;
}sit_net_set_device_service_req;

/*
   SIT_GET_DEVICE_SERVICE (RCM ID = 0x0957)
 */
typedef struct {
    RCM_HEADER hdr;
    INT32 usage_setting;
}sit_net_get_device_service_rsp;

/*
   SIT_SET_BTL_BUFMAX_SIZE (RCM ID = 0x0961)
*/
typedef struct {
    RCM_HEADER hdr;
    INT16 size;
}sit_misc_set_btl_bufmax_size_req;

/*
    SIT_GET_NITZ_TIME (RCM ID = 0x0962)
*/
typedef struct {
    RCM_HEADER hdr;
    sit_nitz_time_with_age nitzData;
}sit_misc_get_nitz_time_rsp;


// ##############################
// #### RCM Commands for Audio/Sound ####
// ##############################

/*
    SIT_SET_MUTE (RCM ID = 0x0A00)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 mute_mode;
}sit_snd_set_mute_req;

typedef null_data_format sit_snd_set_mute_rsp;

typedef enum
{
    SIT_SND_MUTE_MODE_DISABLE = 0x00,
    SIT_SND_MUTE_MODE_ENABLE
}sit_snd_mute_mode_e_type;

/*
    SIT_GET_MUTE (RCM ID = 0x0A01)
*/
typedef null_data_format sit_snd_get_mute_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 mute_state;
}sit_snd_get_mute_rsp;

/*
    SIT_IND_RINGBACK_TONE (RCM ID = 0x0A02)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 ringback_state;
}sit_snd_ringback_tone_ind;

typedef struct
{
    RCM_IND_HEADER hdr;
    INT32 ringback_state;
    INT32 flag;
}sit_snd_ringback_tone_with_flag_ind;

/*
    SIT_SET_VOLUME (RCM ID = 0x0A03)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 volume;    // up to AP, ex) Samsung : 0 ~ 5
}sit_snd_set_volume_req;

typedef null_data_format sit_snd_set_volume_rsp;

/*
    SIT_GET_VOLUME (RCM ID = 0x0A04)
*/
typedef null_data_format sit_snd_get_volume_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 volume;
}sit_snd_get_volume_rsp;

/*
    SIT_SET_AUDIO_PATH (RCM ID = 0x0A05)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 audiopath; // refer sit_snd_audiopath_e_mode
}sit_snd_set_audiopath_req;

typedef null_data_format sit_snd_set_audiopath_rsp;

/*
    SIT_GET_AUDIO_PATH (RCM ID = 0x0A06)
*/
typedef null_data_format sit_snd_get_audiopath_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 audiopath;
}sit_snd_get_audiopath_rsp;


/*
    SIT_SET_MULTIMIC (RCM ID = 0x0A07)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 multiMICmode; // refer sit_snd_multiMIC_e_mode
}sit_snd_set_multimic_req;

typedef null_data_format sit_snd_set_multimic_rsp;

/*
    SIT_GET_AUDIO_PATH (RCM ID = 0x0A08)
*/
typedef null_data_format sit_snd_get_multimic_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 multiMICmode;
}sit_snd_get_multimic_rsp;

/*
    SIT_SWITCH_VOICE_CALL_AUDIO (RCM ID = 0x0A09)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE active_call_sim_info;
}sit_snd_switch_voice_call_audio_req;

typedef null_data_format sit_snd_switch_voice_call_audio_rsp;

typedef enum
{
    SIT_CALL_IS_ACTIVATED_BY_SIM1 = 0x00,
    SIT_CALL_IS_ACTIVATED_BY_SIM2
}sit_snd_active_call_sim_info_e_type;

/*
    SIT_SET_AUDIO_CLK (RCM ID = 0x0A0A)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE clock_mode;
}sit_snd_set_clock_mode_req;

typedef enum
{
    SITRIL_CLOCKMODE_TURNOFF_I2S = 0x00,    //Turn off I2S clock
    SITRIL_CLOCKMODE_TURNON_I2S = 0x01,     //Turn on I2S clock
} sit_snd_clock_mode_e_type;

/*
    SIT_SET_AUDIO_LOOPBACK (RCM ID = 0x0A0B)
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 on_off;
    INT32 audio_path;
}sit_snd_set_loopback_req;

typedef enum
{
    SITRIL_AUDIO_LOOPBACK_STOP = 0,    //0: stop loop back test
    SITRIL_AUDIO_LOOPBACK_START = 1,    //1: start loop back test
} sit_snd_loopback_onoff_e_type;

typedef enum
{
    SITRIL_AUDIO_LOOPBACK_PATH_NA = 0,    //0: N/A
    SITRIL_AUDIO_LOOPBACK_PATH_HANDSET = 1,    //1: handset
    SITRIL_AUDIO_LOOPBACK_PATH_HEADSET = 2,    //2: headset
    SITRIL_AUDIO_LOOPBACK_PATH_HANDSFREE = 3,    //3: handsfree
    SITRIL_AUDIO_LOOPBACK_PATH_BT = 4,    //4: Bluetooth
    SITRIL_AUDIO_LOOPBACK_PATH_STEREO_BT = 5,    //5: stereo Bluetooth
    SITRIL_AUDIO_LOOPBACK_PATH_SPK = 6,    //6: speaker phone
    SITRIL_AUDIO_LOOPBACK_PATH_35PI_HEADSET = 7,    //7: 3.5pi headset
    SITRIL_AUDIO_LOOPBACK_PATH_BT_NS_EC_OFF = 8,    //8: BT NS/EC off
    SITRIL_AUDIO_LOOPBACK_PATH_WB_BT = 9,    //9: WB Bluetooth
    SITRIL_AUDIO_LOOPBACK_PATH_WB_BT_NS_EC_OFF = 10,    //10: WB BT NS/EC OFF
    SITRIL_AUDIO_LOOPBACK_PATH_HANDSET_HAC = 11,    //11: handset HAC
    SITRIL_AUDIO_LOOPBACK_PATH_LINEOUT = 12,                    // Lineout

    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSET = 65,  //65: VOLTE handset
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HEADSET = 66,  //66: VOLTE headset
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_HANDSFREE = 67,    //67: VOLTE handsfree
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_BT = 68,   //68: VOLTE Bluetooth
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_STEREO_BT = 69,    //69: VOLTE stereo Bluetooth
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_SPK = 70,  //70: VOLTE speaker phone
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_35PI_HEADSET = 71, //71: VOLTE 3.5pi headset
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_BT_NS_EC_OFF = 72, //72: VOLTE BT NS/EC off
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_WB_BT = 73,    //73: VOLTE WB Bluetooth
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_WB_BT_NS_EC_OFF = 74,  //74: VOLTE WB BT NS/EC OFF
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_VOLTE_HANDSET_HAC = 75,    // VoLTE : Phone MIC -> RECEIVER for HAC(Hearing Aid Compatibility
    SITRIL_AUDIO_LOOPBACK_PATH_VOLTE_LINEOUT = 76,              // VoLTE Lineout

    SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC1 = 129,  //129: Headset ? MIC1
    SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC2 = 130,  //130: Headset ? MIC2
    SITRIL_AUDIO_LOOPBACK_PATH_HEADSET_MIC3 = 131,  //131: Headset ? MIC3
} sit_snd_loopback_path_e_type;

/*
    SIT_IND_WB_AMR_REPORT = 0x0A0C
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE status;
}sit_snd_wb_amr_report_ind;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE status;
    BYTE call_type;
}sit_snd_wb_amr_report_with_calltype_ind;

/*
    SIT_SET_WB_CAPABILITY = 0x0A0D
*/
typedef struct
{
    RCM_HEADER hdr;
    INT32 wbamr;
}sit_snd_set_wbmar_capability_req;

typedef null_data_format sit_snd_set_wbmar_capability_rsp;

/*
    SIT_SET_WB_CAPABILITY = 0x0A0E
*/
typedef null_data_format sit_snd_get_wbmar_capability_req;

typedef sit_snd_set_wbmar_capability_req sit_snd_get_wbmar_capability_rsp;

/*
    SIT_IND_RESEND_IN_CALL_MUTE (RCM ID = 0x0A0F)
*/
typedef null_ind_data_format sit_snd_resend_in_call_mute_ind;


//for get channel
typedef struct
{
    BYTE ChannelNum;
    BYTE Type;
}sit_ims_get_conf_codec;

//for get option
typedef struct
{
    BYTE ChannelNum;
}sit_ims_get_conf_option;

/*
    SIT_IMS_GEN_REASONE_RESPONSE
    Using SIT_SET_IMS_SETUP (RCM ID = 0x0B00)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    UINT16 aims_call_type;
    BYTE aims_call_id;
    //skip below
}sit_ind_aims_call_ring;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
    BYTE fail_reason;
}sit_ims_gen_reason_rsp;

/*
    SIT_IMS_GEN_REASONE_RESPONSE
    Using SIT_IMS_CHANNEL_STATUS = 0x0B01
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_ims_gen_rsp;

/*
    SIT_IND_AIMS_REGISTRATION
    Using SIT_IND_AIMS_REGISTRATION = 0x0D05
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE state;
    BYTE feature;
    BYTE ecmp_state;
}sit_ims_ind_reg;

typedef enum
{
    SITRIL_AIMS_IMSREG_STATE_NOT_REGISTERED = 0x01,   //0x01 : AIMS_NOT_REGISTERED
    SITRIL_AIMS_IMSREG_STATE_REGISTERED = 0x02, //0x02 : AIMS_REGISTERED
    SITRIL_AIMS_IMSREG_STATE_LIMITED_REGISTERED = 0x03, //0x03 : AIMS_LIMITED_REGISTERED
    SITRIL_AIMS_IMSREG_STATE_NOT_REGISTERED_E911 = 0x04,    //0x04 : AIMS_NOT_REGISTERED_E911,
    SITRIL_AIMS_IMSREG_STATE_REGISTERED_E911 = 0x05,    //0x05 : AIMS_REGISTERED_E911

    SITRIL_AIMS_IMSREG_STATE_MAX
} sit_aims_imsreg_state_e_type;

/*
   SIT_IMS_SET_REGISTRATION_STATUS (RCM ID = 0x0B14)
*/
typedef struct
{
    RCM_HEADER hdr;
    int32_t regStatus;
    int32_t capabilites;
    int32_t accessNetwork;
    int32_t suggestedAction;
} sit_ims_set_registration_status_req;

/*
   SIT_IND_IMS_RRC_CONNECTION_FAILURE (RCM ID = 0x0B17)
 */
typedef struct {
    RCM_IND_HEADER hdr;
    int32_t token;
    int32_t connectionFailureReason;
    int32_t causeCode;
    int32_t waitTimeMillis;
} sit_ims_rrc_connection_failure_ind;

/*
   SIT_IND_IMS_DEREGISTERTAION (RCM ID = 0x0B18)
 */
typedef struct {
    RCM_IND_HEADER hdr;
    int32_t reason;
} sit_ims_trigger_ims_deregistration_ind;

/*
  SIT_IND_MT_CALL_GUARDTMR_STATUS (RCM ID = 0x0B19)
*/
typedef struct {
    RCM_HEADER hdr;
    uint8_t status;
} sit_ims_mt_call_guardtmr_status_req;

/*
   SIT_IMS_START_TRAFFIC (RCM ID = 0x0B1E)
 */
typedef struct {
    RCM_HEADER hdr;
    int32_t token;
    int32_t trafficType;
    int32_t accessNetwork;
    int32_t direction;
} sit_ims_start_ims_traffic_req;

typedef struct {
    RCM_HEADER hdr;
    int32_t connectionFailureReason;
    int32_t causeCode;
    int32_t waitTimeMillis;
} sit_ims_start_ims_traffic_rsp;

/*
  SIT_IMS_STOP_TRAFFIC (RCM ID = 0x0B1F)
*/
typedef struct {
    RCM_HEADER hdr;
    int32_t token;
} sit_ims_stop_ims_traffic_req;

/*
    SIT_GET_ATR (RCM ID = 0x0212)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
    BYTE atr_len;
    BYTE atr[MAX_ATR_LEN];
}sit_id_get_atr_rsp;

/*
    SIT_SAP_CONNECT (RCM ID = 0x0220)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT16 max_msg_size;
}sit_sap_connect_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE status;
    UINT16 max_msg_size;
}sit_sap_connect_rsp;

/*
    SIT_SAP_DISCONNECT (RCM ID = 0x0221)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE status;
}sit_sap_disconnect_rsp;


/*
    SIT_IND_SAP_DISCONNECT (RCM ID = 0x0222)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE status;
}sit_sap_disconnect_ind;

/*
    SIT_SAP_TRANSFER_APDU (RCM ID = 0x0223)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT16 apdu_len;
    char apdu[MAX_SAP_TRANSFER_APDU_LEN];
}sit_sap_transfer_apdu_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
    UINT16 apdu_len;
    char apdu[MAX_SAP_TRANSFER_APDU_LEN];
}sit_sap_transfer_apdu_rsp;

/*
    SIT_SAP_TRANSFER_ATR (RCM ID = 0x0224)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
    BYTE atr_len;
    char atr[MAX_SAP_TRANSFER_ATR_LEN];
}sit_sap_transfer_atr_rsp;


/*
    SIT_SAP_POWER_SIM_OFF (RCM ID = 0x0225)
    SIT_SAP_POWER_SIM_ON (RCM ID = 0x0226)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_sap_power_sim_onoff_rsp;

/*
    SIT_SAP_RESET_SIM (RCM ID = 0x0227)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_sap_reset_sim_rsp;


/*
    SIT_IND_SAP_STATUS (RCM ID = 0x0228)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE card_status;
}sit_sap_status_ind;

/*
    SIT_SAP_TRANSFER_CARD_READER_STATUS (RCM ID = 0x0229)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_sap_transfer_card_reader_status_rsp;


/*
    SIT_SAP_SET_TRANPORT_PROTOCOL (RCM ID = 0x022A)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE protocol;
}sit_sap_tranport_protocol_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_sap_tranport_protocol_rsp;

/*
  SIT_SET_SRVCC_CALL_LIST (RCM ID = 0x000B)
*/
typedef struct {
    int32_t index;
    char callType;
    char state;
    char subState;
    char ringBackTone;
    bool isMpty;
    bool isMt;
    char numberLen;
    char number[MAX_DIAL_NUM];
    int32_t numberPresentation;
    char nameLen;
    char name[MAX_DIAL_NAME];
    int32_t namePresentation;
} current_call_list;

typedef struct {
    RCM_HEADER hdr;
    int32_t callListNum;
    current_call_list callList[0];
} sit_ims_set_srvcc_call_list_req;

/*
    SIT_CALL_CONFIRM_FEATURE_SET_REQ (RCM ID = 0x000E)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE enable; // 0x00: Disable, 0x01: Enable
}sit_call_set_call_confirm_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result; // 0x00: Fail, 0x01: Success
}sit_call_set_call_confirm_rsp;

/*
    SIT_CALL_CONFIRM (RCM ID = 0x000F)
*/

typedef struct
{
    RCM_HEADER hdr;
    BYTE result; // 0x00: Fail, 0x01: Success
}sit_call_send_call_confirm_rsp;

//AIMS support start ---------------------

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[523];
}sit_aims_dial_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[3];
}sit_aims_answer_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[3];
}sit_aims_hangup_req;

typedef struct
{
    BYTE aims_call_id;
    BYTE aims_call_type;
    BYTE hangup_reason;
}sit_aims_hangup_req_ex;

typedef struct
{
    RCM_IND_HEADER hdr;
    UINT16 aims_call_type;
    BYTE aims_call_id;
    BYTE aims_call_state;
    //skip below
}sit_ind_aims_call_status;

typedef struct
{
    RCM_HEADER hdr;
}sit_aims_deregistration_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[903];
}sit_set_aims_hidden_menu_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[23];
}sit_set_aims_ims_pdn_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[2];
}sit_aims_call_manage_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[3];
}sit_aims_send_dtmf_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[10];
}sit_set_aims_frame_time_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[2];
}sit_get_aims_frame_time_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[2];
}sit_aims_modify_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[4];
}sit_aims_response_modify_req;

typedef struct
{
    INT32 number_len;
    BYTE number[513];
}sit_aims_conf_call_user_info;

typedef struct
{
    RCM_HEADER hdr;
    BYTE conf_call_id;
    INT16 number_of_participant;
    sit_aims_conf_call_user_info *user_info;
}sit_aims_conf_call_add_user;

typedef struct
{
    RCM_HEADER hdr;
    INT16 number_of_participant;
    sit_aims_conf_call_user_info *user_info;
}sit_aims_enhanced_conf_call;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[67];
}sit_aims_get_call_forward_status;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[67];
}sit_aims_set_call_forward_status;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[14];
}sit_aims_get_call_waiting;

typedef struct
{
    int service_class;
    BYTE current_time_info[10];
}sit_aims_get_call_waiting_ex;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
    BYTE error_code;
    BYTE retry_timer;
    int service_status;
    int service_class;
}sit_aims_get_call_waiting_rsp;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[18];
}sit_aims_set_call_waiting;

typedef struct
{
    int service_status;
    int service_class;
    BYTE current_time_info[10];
}sit_aims_set_call_waiting_ex;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
    BYTE error_code;
    BYTE retry_timer;
}sit_aims_set_call_waiting_rsp;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[18];
}sit_aims_get_call_barring;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[27];
}sit_aims_set_call_barring;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[258];
}sit_aims_send_sms;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[258];
}sit_aims_send_expect_more;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[9];
}sit_aims_send_sms_ack;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[250];
}sit_aims_send_ack_incoming_sms;

typedef struct
{
    RCM_HEADER hdr;
    BYTE data[22];
}sit_aims_chg_barring_pwd;

//AIMS support end ---------------------

/*
    SIT_VSIM_NOTIFICATION (RCM ID = 0x0E00)
*/
typedef struct
{
    RCM_HEADER hdr;
    int tid;
    int event_id;
    int sim_type;
}sit_vsim_notification_req;

typedef null_data_format sit_vsim_notification_rsp;

/*
    SIT_IND_VSIM_OPERATION (RCM ID = 0x0E01)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    int tid;
    int event_id;
    int result;
    int datalength;
    BYTE data[MAX_VSIM_DATA_LEN];
}sit_vsim_opertaion_ind;

/*
    SIT_VSIM_OPERATION (RCM ID = 0x0E02)
*/
typedef struct
{
    RCM_HEADER hdr;
    int tid;
    int event_id;
    int result;
    int datalength;
    BYTE data[MAX_VSIM_DATA_LEN];
}sit_vsim_opertaion_req;

typedef null_data_format sit_vsim_opertaion_rsp;

// OEM Common

/*
    SIT_OEM_STORE_ADB_SERIAL_NUMBER_REQ (RCM ID = 0x4011)
*/
typedef struct
{
    RCM_HEADER hdr;
    char adbSerialNumber[MAX_ADB_SERIAL_NUMBER];
}sit_oem_store_adb_serial_number_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE rcmError;
}sit_oem_store_adb_serial_number_rsp;

/*
    SIT_OEM_READ_ADB_SERIAL_NUMBER_REQ (RCM ID = 0x4012)
*/
typedef null_data_format sit_oem_read_adb_serial_number_req;

typedef struct
{
    RCM_HEADER hdr;
    char adbSerialNumber[MAX_ADB_SERIAL_NUMBER];
}sit_oem_read_adb_serial_number_rsp;

/*
 *   SIT_OEM_GET_SAR_STATE (RCM ID = 0x4100)
 */
typedef null_data_format sit_misc_sar_get_sar_state_req;

typedef struct
{
    RCM_HEADER hdr;
    INT32 sar_state;
}sit_misc_sar_get_sar_state_rsp;

/*
 *   SIT_OEM_SET_SAR_STATE (RCM ID = 0x4101)
 */
typedef struct
{
    RCM_HEADER hdr;
    INT32 sar_status;
}sit_misc_sar_set_sar_state_req;

/*
 *   SIT_OEM_IND_RF_CONNECTION (RCM ID = 0x4102)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE rf_state;
}sit_misc_sar_rf_connection_ind;

/*
   SIT_OEM_SET_SVN (RCM ID = 0x4103)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE sv_number[2];
}sit_id_set_sv_number_req;

/*
 *   SIT_OEM_GET_SIM_LOCK_INFO (RCM ID = 0x4104)
 */
typedef null_data_format sit_sim_get_sim_lock_info_req;

enum { SIT_STATUS_UNLOCKED, SIT_STATUS_LOCKED, };
enum {
    SIT_LOCK_TYPE_UNKNOWN = -1,
    SIT_LOCK_TYPE_UNLOCKED,
    SIT_LOCK_TYPE_PN,
    SIT_LOCK_TYPE_PU,
    SIT_LOCK_TYPE_SP,
    SIT_LOCK_TYPE_CP,
};
enum {
    SIT_LOCK_CODE_DEFAULT_SIZE = 2,
    SIT_LOCK_CODE_PN_SIZE = 6,
    SIT_MAX_LOCK_CODE_DATA = 1008
};

typedef struct
{
    RCM_HEADER hdr;
    BYTE policy;
    BYTE status;
    BYTE lockType;
    BYTE maxRetryCount;
    BYTE reaminCount;
    UINT16 lockCodeCount;
    char lockCode[SIT_MAX_LOCK_CODE_DATA];
}sit_sim_get_sim_lock_info_rsp;

enum {
    SIT_NW_INFO_SCREEN_STATE = 1,
    SIT_NW_INFO_CELL_INFO = 2,
    SIT_NW_INFO_CA_MIMO_HORXD = 3,
    SIT_NW_INFO_VOLTE_INFO = 4,
    SIT_NW_INFO_CDRX = 5,
    SIT_NW_INFO_SET_BAND = 7,
    SIT_NW_INFO_HW_BAND = 8,
};

/*
 * SIT_OEM_SET_CA_BW_FILTER (RCM ID = 0x4105)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE enable;
}sit_oem_set_ca_bw_filter_req;

/*
 * SIT_OEM_IND_CA_BW_FILTER (RCM ID = 0x4106)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE ca_config;
    UINT16 num_resource_block;
}sit_oem_ca_bw_filter_ind;

/*
 * SIT_OEM_NW_INFO (RCM ID = 0x4400)
 */
typedef struct
{
    RCM_HEADER hdr;
    int command;
    unsigned int length;     // size of data
    char data[0];   // variable
}sit_oem_nw_info_req;

typedef struct
{
    RCM_HEADER hdr;
    int command;
    unsigned int length;     // size of data
    char data[0];   // variable
}sit_oem_nw_info_rsp;

enum {
    SIT_NW_INFO_MIMO_INFO = 1,
    SIT_NW_INFO_SCG_FAIL_INFO = 2,
    SIT_NW_INFO_NR_CELL_INFO = 3,
};

/*
 * SIT_OEM_IND_NW_INFO (RCM ID = 0x4401)
 */
typedef struct
{
    RCM_IND_HEADER hdr;
    int command;
    unsigned int length;     // size of data
    char data[0];   // variable
}sit_oem_nw_info_ind;

/*
 *   SIT_OEM_SET_ACTIVATE_VSIM (RCM ID = 0x4402)
 */

typedef struct
{
    RCM_HEADER hdr;
    BYTE simSlot;    // 1 = SLOT_1, 2 = SLOT_2, 3 = SLOT_3;
    BYTE iccidLen;
    BYTE iccid[MAX_ICCID_STRING_LEN];  // ASCII
    BYTE imsiLen;
    BYTE imsi[MAX_IMSI_LEN];           // ASCII
    BYTE hplmn[MAX_PLMN_LEN];          // ASCII
    BYTE vsimState;    // 0 = deactivate, 1 = activate;
    BYTE vsimCardType;    // 1 = CDMA vsim, 2 = 2G vsim, 3 = 3G vsim, 4 = 4G vsim
}sit_oem_set_activate_visim_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;    // 0 = activate_fail, 1 = activate_success
}sit_oem_set_activate_visim_rsp;

/*
 * SIT_OEM_SET_FORBID_LTE_CELL (RCM ID = 0x4403)
 */
typedef struct
{
    RCM_HEADER hdr;
    int mode;
    int cellid;
    int forbidden_timer;
    char plmn[6];
}sit_oem_set_forbid_lte_cell_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_forbid_lte_cell_rsp;

/*
 * SIT_OEM_SET_FUNC_SWITCH_REQ (RCM ID = 0x4405)
 */
typedef struct
{
    RCM_HEADER hdr;
    /*
    0x01 = CA
    0x02 = MIMO
    0x03 = HORXD
    0x04 = HPUE
    0x05 = VOLTE
    0x06 = CDRX
    */
    int feature;
    /*
    0x00 = Disable(off)
    0x01 = Enable(on)
    */
    BYTE enable;
}sit_oem_set_func_switch_req;

typedef struct
{
    RCM_HEADER hdr;
    /*
    0x01 : Success
    0x00 : Failure
    */
    BYTE result;
}sit_oem_set_func_switch_rsp;

/*
 * SIT_OEM_SET_FORBID_NR_CELL (RCM ID = 0x4406)
 */
typedef struct
{
    RCM_HEADER hdr;
    int mode;
    ULONG cellid;
    int forbidden_timer;
    char plmn[6];
}sit_oem_set_forbid_nr_cell_req;

typedef struct {
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_forbid_nr_cell_rsp;

/*
 * SIT_OEM_SET_PDCP_DISCARD_TIMER (RCM ID = 0x4408)
 */
typedef struct
{
    RCM_HEADER hdr;
    /*
    PDCP discard timer value (ms)
    if the value is 0, that means control off.
    */
    int discardTimer;
}sit_oem_set_pdcp_discard_timer_req;

/*
 * SIT_OEM_GET_CQI_INFO (RCM ID = 0x4409)
 */
typedef struct
{
    RCM_HEADER hdr;
    INT16 type;
    INT16 cqi_info0;
    INT16 cqi_info1;
    INT16 ri;
}sit_oem_get_cqi_info_rsp;

/*
 * SIT_OEM_SET_SAR_SETTING (RCM ID = 0x440A)
 */
typedef struct
{
    RCM_HEADER hdr;
    INT32 dsi;
}sit_oem_set_sar_setting_req;

/*
 * SIT_OEM_SET_GMO_SWITCH (RCM ID = 0x440B)
 */
typedef struct
{
    RCM_HEADER hdr;
    INT32 feature;
}sit_oem_set_gmo_switch_req;

/*
 * SIT_OEM_SET_TCS_FCI_REQ (RCM ID = 0x440C)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE state;
    BYTE fci[MAX_FCI_LEN];
}sit_oem_set_tcs_fci_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_tcs_fci_rsp;

/*
 * SIT_OEM_GET_TCS_FCI_INFO (RCM ID = 0x440D)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE fci[MAX_FCI_LEN];
}sit_oem_get_tcs_fci_info_rsp;

/*
    SIT_OEM_IND_ENDC_CAPABILITY (RCM ID = 0x440E)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE endc_capability;
    BYTE cause;
}sit_oem_endc_capability_ind;

/*
    SIT_OEM_GET_ACTIVITY_INFO (RCM ID = 0x440F)
*/
typedef null_data_format sit_oem_get_modem_activity_req;

/*
    SIT_OEM5_SET_SVN (RCM ID = 0x4412)
*/
typedef struct
{
    RCM_HEADER hdr;
    char svn[2];
}sit_oem5_set_svn_req;

/*
 * SIT_OEM_SET_FORBIDDEN_FREQUENCY (RCM ID = 0x4413)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
    int frequency;
    int forbidden_timer;
}sit_oem_set_forbidden_frequency_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_forbidden_frequency_rsp;

/*
 * SIT_OEM_SET_FORBIDDEN_LTE_BAND (RCM ID = 0x4414)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE numBands;
    INT16 band[MAX_FORBID_LTE_BAND];
}sit_oem_set_forbidden_let_band_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_forbidden_let_band_rsp;

/*
 * SIT_OEM_SET_FORBIDDEN_LTE_TAC (RCM ID = 0x4415)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE mode;
    INT16 tac;
}sit_oem_set_forbidden_let_tac_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_forbidden_let_tac_rsp;

/*
 * SIT_OEM_SET_SELFLOG_TCP_DUMP_MODE (RCM ID = 0x4416)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE tcp_dump_enable;
    BYTE header_dump_enable;
}sit_oem_set_selflog_tcp_dump_mode_req;

/*
    SIT_OEM_IND_DROPPED_MT_CALL_INFO (RCM ID = 0x4417)
*/

/*
    SIT_OEM_SET_BSR_PARA_REQ (RCM ID = 0x4418)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE switch_status;
    INT16 packet_period;
    INT32 packet_size;
}sit_oem_set_bsr_para_req;

/*
    SIT_OEM_SET_DRDV (RCM ID = 0x4419)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE drdv_mask;
}sit_oem_set_drdv_req;

/**
 * SIT_OEM_SET_AHEAD_RRE (RCM ID = 0x441B)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE searchType;
    INT32 earfcn;
    INT32 globalCellId;
}sit_oem_set_ahead_rre_req;

/*
 * SIT_OEM_SET_AHEAD_RRE (RCM ID = 0x441B)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE result;
}sit_oem_set_ahead_rre_rsp;

/*
    SIT_OEM_GET_CDMA_CAPABILITY_INFO (RCM ID = 0x441D)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE capability;
}sit_oem_get_cdma_capability_info_rsp;

/**
 * SIT_OEM_SET_SRS_PWR_LIMIT (RCM ID = 0x441F)
 */
typedef struct {
    RCM_HEADER hdr;
    UINT16 level;
}sit_oem_set_srs_pwr_limit_req;

/*
    SIT_OEM_SET_UE_OPERATION_MODE (RCM ID = 0x4420)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE mode;
}sit_oem_set_ue_operation_mode_req;

/*
    SIT_OEM_GET_UE_OPERATION_MODE (RCM ID = 0x4421)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE mode;
}sit_oem_get_ue_operation_mode_rsp;

/*
 * SIT_OEM_SET_TRACE_DUMP_CFG (RCM ID = 0x4422)
 */
typedef struct {
    RCM_HEADER hdr;
    BYTE config;
}sit_oem_set_trace_dump_cfg_req;

/*
 * SIT_OEM_GET_TRACE_DUMP_CFG (RCM ID = 0x4423)
 */
typedef struct {
    RCM_HEADER hdr;
    BYTE config;
}sit_oem_get_trace_dump_cfg_rsp;

/*
    SIT_OEM_SET_NRRRM_MIN_REQ_ENABLE (RCM ID = 0x4427)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE enable;
}sit_oem_set_nrrrm_min_req_enable_req;

/*
    SIT_OEM_GET_NRRRM_MIN_REQ_ENABLE (RCM ID = 0x4428)
*/
typedef struct {
    RCM_HEADER hdr;
    BYTE enable;
}sit_oem_get_nrrrm_min_req_enable_rsp;

/*
    SIT_OEM_SET_PWR_DETECT_THR_REQ (RCM ID = 0x442C)
*/
typedef struct {
    RCM_HEADER hdr;
    INT16 txPwrDetectThr;
    INT16 txPwrDifferenceThr;
}sit_oem_set_pwr_detect_thr_req;

/*
    SIT_OEM_SET_ENABLE_N28 (RCM ID = 0x442B)
*/
typedef struct {
    RCM_HEADER hdr;
    INT32 enable_n28;
}sit_oem_set_enable_n28_req;

/*
    SIT_OEM_GET_BINARY_VERSION_INFO (RCM ID = 0x442D)
*/
typedef struct {
    RCM_HEADER hdr;
    INT16 length;
    char data[0]; // variable
}sit_oem_get_binary_version_info_rsp;

/*
   SIT_OEM_SET_VCRT (RCM ID = 0x4432)
 */
typedef struct {
    RCM_HEADER hdr;
    BYTE vcrtMode;
}sit_oem_set_vcrt_req;

/*
   SIT_OEM_GET_VCRT (RCM ID = 0x4433)
 */
typedef struct {
    RCM_HEADER hdr;
    BYTE vcrtMode;
}sit_oem_get_vcrt_rsp;

// Call & SS
/*
    SIT_SET_CDMA_VOICE_PRIVACY_MODE (RCM ID =0x0011)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE vpMode;
}sit_call_set_cdma_voice_privacy_mode_req;

typedef null_data_format sit_call_set_cdma__voice_privacy_mode_rsp;
// "response" is NULL

/*
    SIT_GET_CDMA_VOICE_PRIVACY_MODE (RCM ID =0x0012)
*/
typedef null_data_format sit_call_get_preferred_voice_privacy_mode_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE status;
}sit_call_get_preferred_voice_privacy_mode_rsp;

/*
    SIT_CDMA_BURST_DTMF (RCM ID =0x0015)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE dtmf_len;
    BYTE dtmf_digit[MAX_BURST_DTMF_LEN];
    BYTE on_len;
    BYTE dtmf_on_length[MAX_BURST_DTMF_ON_OFF_LEN];
    BYTE off_len;
    BYTE dtmf_off_length[MAX_BURST_DTMF_ON_OFF_LEN];
}sit_call_cdma_burst_dtmf_req;

typedef null_ind_data_format smi_call_cdma_burst_dtmf_rsp;

/*
    SIT_IND_OTA_PROVISION_STATUS (RCM ID =0x0016)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE otaType; // sit_ota_e_type
    BYTE otaStatus; // sit_otasp_status_e_type & sit_otapa_status_e_type
}sit_ota_provision_status_ind;

/*
    SIT_IND_CDMA_INFO_REC (RCM ID =0x0017)
*/
typedef struct
{
    BYTE alpha_len;
    BYTE alpha_buf[MAX_ALPHA_INFO_BUF_LEN];
}sit_cdma_display_info;

typedef struct
{
    BYTE len;
    BYTE buf[MAX_NUMBER_INFO_BUFFER_LEN];
    BYTE number_type;
    BYTE number_plan;
    BYTE pi;
    BYTE si;
}sit_cdma_number_info;

typedef struct
{
    BYTE is_present;
    BYTE signal_type;
    BYTE alert_pitch;
    BYTE signal;
}sit_cdma_signal_info;

typedef struct
{
    sit_cdma_number_info redirecting_number;
    BYTE reason; // sit_cdma_redirecting_reason_e_type
}sit_cdma_redirecting_number_info;

typedef struct
{
    BYTE line_ctrl_polarity_included;
    BYTE line_ctrl_toggle;
    BYTE line_ctrl_reverse;
    BYTE line_ctrl_power_denial;
}sit_cdma_line_control_info;

typedef struct
{
    BYTE cdma_info_name; // sit_cdma_info_rec_name_e_type
    union
    {
        sit_cdma_display_info display;
        sit_cdma_number_info number;
        sit_cdma_signal_info signal;
        sit_cdma_redirecting_number_info redirecting_number;
        sit_cdma_line_control_info line_control;
    }cdma_info;
}sit_cdma_info_rec_ind;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE num_of_info_recs;
    sit_cdma_info_rec_ind info_rec[MAX_NUMBER_OF_INFO_RECS];
}sit_cdma_information_records;

/*
    SIT_IND_EMERGENCY_SUPPORT_RAT_MODE (RCM ID =0x0018)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE available_tech; // sit_emergency_support_rat_mode_e_type
}sit_call_emergency_support_rat_mode_ind;

/*
    SIT_SET_CDMA_FLASH_INFO (RCM ID =0x0514)
 */
typedef struct
{
    RCM_HEADER hdr;
    BYTE flash_len;
    BYTE flash[MAX_FLASH_LEN];
}sit_ss_set_cdma_flash_info_req;

/*
    SIT_IND_CDMA_CALL_WAITING (RCM ID =0x0515)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE num_len;
    BYTE number[MAX_DIAL_NUM];
    BYTE number_presentation;
    BYTE name_len;
    BYTE name[MAX_DIAL_NAME];
    sit_cdma_signal_info signal_info;
    BYTE number_type;
    BYTE number_plan;
}sit_ss_cdma_call_waiting_ind;

// SMS
/*
    SIT_CDMA_SEND_SMS (RCM ID = 0x0111)
*/
typedef struct
{
    RCM_HEADER hdr;
    UINT16 msg_len;
    BYTE msg[MAX_CDMA_SMS_MSG_SIZE];
}sit_sms_cdma_send_sms_req;

typedef struct
{
    RCM_HEADER hdr;
    INT16 msg_ref;
    BYTE error_class;
    BYTE error_code;
}sit_sms_cdma_send_sms_rsp;

/*
    SIT_CDMA_SEND_SMS_EXPECT_MORE (RCM ID = 0x0122)
*/
// same as SIT_CDMA_SEND_SMS

/*
    SIT_CDMA_SEND_SMS_ACK (RCM ID = 0x0112)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE msg_tpid;
    BYTE error_class;
    BYTE error_code;
}sit_sms_cdma_send_sms_ack_req;

typedef null_data_format sit_sms_cdma_send_sms_ack_rsp;

/*
    SIT_CDMA_GET_BCST_SMS_CFG (RCM ID = 0x0113)
*/
typedef null_data_format sit_sms_cdma_get_bcst_sms_cfg_req;

typedef struct
{
    INT16 svc_category;
    BYTE language;
}sit_sms_cdma_bcst_sms_cfg_item;

typedef struct
{
    RCM_HEADER hdr;
    BYTE bcst_info_num;
    sit_sms_cdma_bcst_sms_cfg_item cfgitem[MAX_CDMA_BCST_INFO_NUM];
}sit_sms_cdma_get_bcst_sms_cfg_rsp;

/*
    SIT_CDMA_SET_BCST_SMS_CFG (RCM ID = 0x0114)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE bcst_info_num;
    sit_sms_cdma_bcst_sms_cfg_item cfgitem[MAX_CDMA_BCST_INFO_NUM];
}sit_sms_cdma_set_bcst_sms_cfg_req;

typedef null_data_format sit_sms_cdma_set_bcst_sms_cfg_rsp;

/*
    SIT_CDMA_ACT_BCST_SMS (RCM ID = 0x0115)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE bcst_act;
}sit_sms_cdma_act_bcst_sms_req;

typedef null_data_format sit_sms_cdma_act_bcst_sms_rsp;

/*
    SIT_CDMA_WRITE_SMS_TO_RUIM (RCM ID = 0x0116)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE status;
    BYTE msg_len;
    BYTE msg[MAX_CDMA_SMS_RUIM_MSG_SIZE];
}sit_sms_cdma_write_sms_to_ruim_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE index;
}sit_sms_cdma_write_sms_to_ruim_rsp;

/*
    SIT_CDMA_DELETE_SMS_ON_RUIM (RCM ID = 0x0117)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE index;
}sit_sms_cdma_delete_sms_on_ruim_req;

typedef null_data_format sit_sms_cdma_delete_sms_on_ruim_rsp;

/*
    SIT_IND_CDMA_NEW_SMS (RCM ID = 0x0118)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE msg_tpid;
    UINT16 msg_len;
    BYTE msg[MAX_CDMA_SMS_MSG_SIZE];
}sit_sms_cdma_new_sms_ind;

/*
    SIT_IND_CDMA_RUIM_SMS_STORAGE_FULL (RCM ID = 0x0119)
*/
typedef null_ind_data_format sit_sms_cdma_ruim_sms_storage_full_ind;

/*
    SIT_IND_CDMA_VOICE_MSG_WAITING_INFO (RCM ID = 0x0120)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE msg_count;
}sit_sms_cdma_voice_msg_waiting_info_ind;


// ##############################
// #### RCM Commands for GPS####
// ##############################

#define LPP_MAX_AGNSS_CNT       2
#define LPP_MAX_SVs_CNT         16

/*----------------------------------------------------------------
    GPS Control Plane MO Location Message
*/

/* QoS Type */
typedef struct
{
    UINT32              qosFlag;
    UINT8               horizontalAccuracy;
    BYTE                verticalCoordinateRequest;
    UINT8               verticalAccuracy;
    UINT8               responseTime;
}__attribute__((packed))sit_gps_qos_type;

/* GPS Satellit info Type */
typedef struct
{
    UINT8   sat_id;
    UINT8   iode;
}__attribute__((packed))sit_gps_sat_info_type;

typedef struct
{
    UINT8   beginWeek;
    UINT8   endWeek;
    UINT8   beginTow;
    UINT8   endTow;
}__attribute__((packed))sit_gps_ext_ephe_chk_type;

/* GPS Assistance Data Type */
typedef struct
{
    UINT32                      assistanceFlag;
    UINT16                      gpsWeek;
    UINT8                       gpsToe;
    UINT8                       nSat;
    UINT8                       toeLimit;
    sit_gps_sat_info_type       satInfo[15];
    UINT8                       gpsExtendedEphemeris;
    sit_gps_ext_ephe_chk_type   extEphemerisChk;
}__attribute__((packed))sit_gps_assistance_data_type;

/* Ellipsoid Point Type */
typedef struct {
    INT32                       latitude;
    INT32                       longitude;
}__attribute__((packed))sit_gps_ellipsoid_po_type;

/* Point Uncertain Circle Type */
typedef struct
{
    sit_gps_ellipsoid_po_type   point;
    INT8                        uncertainRadius;
}__attribute__((packed))sit_gps_po_unc_circle_type;

/* PointUncertain Ellipse Type */
typedef struct
{
    sit_gps_ellipsoid_po_type   point;
    INT8    semiMajorAxis;
    INT8    semiMinorAxis;
    INT8    orientationAngle;
    INT8    confidence;
}__attribute__((packed))sit_gps_po_unc_ellipse_type;

/* Point Altitude Uncertain Ellipsoid Type */
typedef struct
{
    sit_gps_ellipsoid_po_type   point;
    INT16                       altitude;
    INT8                        semiMajorAxis;
    INT8                        semiMinorAxis;
    INT8                        orientationAngle;
    INT8                        uncertainAltitude;
    INT8                        confidence;
}__attribute__((packed))sit_gps_po_alt_unc_ellipse_type;

/* Ellipsoid Arc Type */
typedef struct
{
    sit_gps_ellipsoid_po_type   point;
    INT16                       innerRadius;
    INT8                        uncertainRadius;
    INT8                        offsetAngle;
    INT8                        includedAngle;
    INT8                        confidence;
}__attribute__((packed))sit_gps_ellipsoid_arc_type;

/* Point Altitude Type */
typedef struct  {
    sit_gps_ellipsoid_po_type   point;
    INT16                       altitude;
}__attribute__((packed))sit_gps_ellipsoid_alt_type;

/* Polygon Type */
typedef struct
{
    INT8                        noOfPoints;
    sit_gps_ellipsoid_po_type   points[15];
}__attribute__((packed))sit_gps_polygon_type;

/* GPS Location Info Type  */
typedef struct
{
    UINT8   shape_type;
    sit_gps_po_unc_circle_type  p_unc_clrcle;
    sit_gps_po_unc_ellipse_type p_unc_ellipse;
    sit_gps_po_alt_unc_ellipse_type p_alt_unc_ellipse;
    sit_gps_ellipsoid_arc_type  ellipsoid_arc;
    sit_gps_ellipsoid_po_type   ellipsoid_po;
    sit_gps_ellipsoid_alt_type  ellipsoid_alt;
    sit_gps_polygon_type        polygon;
}__attribute__((packed))sit_gps_loc_info_type;

/* Deciphering Keys Type */
typedef struct
{
    UINT8   cipherKeyFlag;
    UINT8   currentDecipherKey[7];
    UINT8   nextDecipherKey[7];
}__attribute__((packed))sit_gps_deciphering_keys_type;

/*----------------------------------------------------------------
      Enhanced LTE Cell Info Message
*/

typedef struct {
    UINT8                                           flag;

    #define OEM_LTE_ECID_REQ_CAP                    0x01
    #define OEM_LTE_ECID_REQ_LOC                    0x02

    UINT8                                           responseTime;
} __attribute__((packed)) oem_gps_get_lpp_supl_ecid_Info;

typedef struct {
    UINT16                                          mcc;
    UINT16                                          mnc;
    UINT32                                          cellId;
    UINT16                                          tac;
} __attribute__((packed)) sit_cgi_info;

typedef struct {
    UINT8                                           bitMask;
    #define BM_LTE_MRL_CGI_INFO                     0x01
    #define BM_LTE_MRL_RSRP                         0x02
    #define BM_LTE_MRL_RSRQ                         0x04
    #define BM_LTE_MRL_EARFCN                       0x08

    UINT16                                          physCellId;
    sit_cgi_info                                    cgiInfo;
    UINT8                                           rsrp;
    UINT8                                           rsrq;
    UINT16                                          earfcn;
} __attribute__((packed)) sit_mrl_eutra_info;

/*----------------------------------------------------------------
    GSM/WCDMA Ehanced CELL Information Message (RRLP)
*/

typedef struct {
    UINT8                                   flag;
    #define OEM_WCDMA_ECID_REQ_CAP              0x01
    #define OEM_WCDMA_ECID_REQ_LOC              0x02

    UINT8                                   responseTime;
}  __attribute__((packed)) oem_gps_get_rrlp_supl_ecid_Info;

typedef struct {
    UINT16                                  aRfcn;
    UINT8                                   bSic;
    UINT8                                   rxLev;
}  __attribute__((packed)) sit_gsm_nmr_element;

typedef struct {
    sit_gsm_nmr_element                     nmr[15];
    UINT8                                   sizeOf;
}  __attribute__((packed)) sit_gsm_network_measured_results;

typedef struct {
    UINT8   bitMask;
    #define BM_2G_NETWORK_MEAS_RESULT       0x01
    #define BM_2G_TA                        0x02

    UINT16                                  refmcc;
    UINT16                                  refmnc;
    UINT16                                  reflac;
    UINT16                                  refci;
    sit_gsm_network_measured_results        networkMeasResult;
    UINT8                                   timingAdvance;
}  __attribute__((packed)) sit_gsm_cell_info;

typedef struct {
    UINT16                                  tA;
    UINT8                                   tAResolution;
    UINT8                                   chipRate;
}  __attribute__((packed)) sit_wcdma_timing_advance;

typedef struct {
    UINT8                                   modeSpecificInfo;
    #define WCDMA_FREQ_INFO_FDD             0x01
    #define WCDMA_FREQ_INFO_TDD             0x02
    UINT16                                  uarfcnUl;
    UINT16                                  uarfcnDl;
    UINT16                                  uarfcnNt;
}  __attribute__((packed)) sit_wcdma_frequency_info;

typedef struct {
    UINT16                                  primarycpichinfo;
    UINT8                                   cpichecn0;
    UINT8                                   cpichrscp;
    UINT8                                   pathloss;
}  __attribute__((packed)) sit_wcdma_cell_measured_results_fdd;

typedef struct {
    UINT8                                   cellparametersid;
    UINT8                                   proposedtgsn;
    UINT8                                   primaryccpch_rscp;
    UINT8                                   pathloss;
    UINT8                                   timeslotiscplist[14];
    UINT8                                   sizeoftimeslotiscplist;
}  __attribute__((packed)) sit_wcdma_cell_measured_results_tdd;

typedef struct {
    UINT8                                   modeSpecificInfo;
    #define WCDMA_CELLMEAS_RESULT_FDD       0x01
    #define WCDMA_CELLMEAS_RESULT_TDD       0x02

    UINT32                                  cellIdentity;
    union{
        sit_wcdma_cell_measured_results_fdd   fdd;
        sit_wcdma_cell_measured_results_tdd   tdd;
    }u_SpecificInfo;

}  __attribute__((packed)) sit_wcdma_cell_measured_results;

typedef struct {
    sit_wcdma_frequency_info                frequencyinfo;
    UINT8                                   utraCarrierRssi;
    sit_wcdma_cell_measured_results         cellMeasuredResultsList[16];
    UINT8                                   sizeOfCellMeasResultsList;
}  __attribute__((packed)) sit_wcdma_measured_results;

typedef struct {
    UINT8                                   bitMask;
    #define BM_3G_FREQUENCY_INFO            0x01
    #define BM_3G_PRIMARY_SCRAMBLING_CODE   0x02
    #define BM_3G_MEASURED_RESULTS_LIST     0x04
    #define BM_3G_CELL_PARAMETERS_ID        0x08
    #define BM_3G_TIMING_ADVANCE            0x10

    UINT16                                  refMcc;
    UINT16                                  refMnc;
    UINT32                                  refUc;

    sit_wcdma_frequency_info                frequencyInfo;
    UINT16                                  primaryScramblingCode;
    sit_wcdma_measured_results              measuredResultsList[8];
    UINT8                                   sizeOfMeasurementResultList;
    UINT8                                   cellParametersId;
    sit_wcdma_timing_advance                timingAdvance;
}  __attribute__((packed)) sit_wcdma_cell_info;


/*----------------------------------------------------------------
     GPS Assist Data Message
*/

/* GSM Time Type */
typedef struct
{
    BYTE        valid;
    UINT16      bcchCarrier;
    UINT16      bsic;
    UINT32      frameNumber;
    UINT16      timeSlot;
    UINT16      bitNumber;
}__attribute__((packed))sit_gps_gsm_time_type;

/* UTRAN GPS Reference Time Type */
typedef struct
{
    BYTE                        valid;
    UINT32                      cellFrames;
    BYTE                        choice_mode;
    UINT32                      UtranFdd;
    UINT32                      UtranTdd;
    UINT32                      sfn;
}__attribute__((packed))sit_gps_utran_gps_ref_time_type;

/* Utran Gps Uncertainty Type */
typedef struct
{
    BYTE        valid;
    UINT32      gpsTimeUncertainty;
}__attribute__((packed))sit_gps_utran_gps_unc_type;

/* Utran Drift Rate Type */
typedef struct
{
    BYTE        valid;
    INT32       driftRate;
}__attribute__((packed))sit_gps_drift_rate_type;

/* Utran Time Type  */
typedef struct
{
    sit_gps_utran_gps_ref_time_type     UtranGpsRefTime;
    sit_gps_utran_gps_unc_type          UtranGpsUncertainty;
    BYTE                                UtranSfnUncertainty;
    sit_gps_drift_rate_type             UtranDriftRate;
}__attribute__((packed))sit_gps_utran_time_type;

/* Gps Tow Assist Type */
typedef struct
{
    UINT16      satID;
    UINT16      tlmWord;
    BYTE        antiSpoofFlag;
    BYTE        alertFlag;
    UINT8       tmlReservedBits;
}__attribute__((packed))sit_gps_gps_tow_assist_type;

/* Reference Time Type */
typedef struct
{
    UINT32                  gpsTow;
    UINT32                  gpsWeek;
    UINT8                   nrOfSats;
    union
    {
        sit_gps_gsm_time_type       gsm_time;
        sit_gps_utran_time_type     UtranTime;
    } networkTimeInfo;
    sit_gps_gps_tow_assist_type GpsTowAssist[12];
}__attribute__((packed))sit_gps_ref_time_type;


/* Dgps Sat List Type */
typedef struct
{
    UINT8   satId;
    UINT16  iode;
    UINT8   udre;
    INT16   pseudoRangeCor;
    INT16   rangeRateCor;
}__attribute__((packed))sit_gps_dgps_sat_list_type;

/* Dgps Corrections Type*/
typedef struct
{
    UINT32              gpsTow;
    BYTE                status;
    UINT32              numberOfSat;
    sit_gps_dgps_sat_list_type  seqOfSatElement[16];
}__attribute__((packed))sit_gps_dgps_correction_type;


/* Navigation SubFrame Rsv Type*/
typedef struct
{
    UINT32  rsv1; // 0~838860
    UINT32  rsv2; // 0~16777215
    UINT32  rsv3; // 0~16777215
    UINT32  rsv4; // 0~65535
}__attribute__((packed))sit_gps_navi_subframe_rsv_type;

/* Navigation Ephemeris Type */
typedef struct
{
    UINT8                           ephemCodeOnL2; // 0~3
    UINT8                           ephemUra; // 0~15
    UINT8                           ephemSvHealth; // 0~63
    UINT16                          ephemIodc; // 0~1023
    UINT8                           ephemL2PFlag; // 0~1
    sit_gps_navi_subframe_rsv_type  NavigationSubFrameRsv;
    INT8                            ephemTgd; // -128~127
    UINT16                          ephemToc; // 0~37799
    INT8                            ephemAf2; // -128~12
    INT16                           ephemAf1; // -32768~32767
    INT32                           ephemAf0; // -2097152~2097151
    INT16                           ephemCrs; // -32768~32767
    INT16                           ephemDeltaN; // -32768~32767
    INT32                           ephemM0; // -2147483648~2147483647
    INT16                           ephemCuc; // -32768~32767
    UINT32                          ephemE; // 0~4294967295
    INT16                           ephemCus; // -32768~32767
    UINT32                          ephemAPowrHalf; // 0~4294967295
    UINT16                          ephemToe; // 0~37799
    INT8                            ephemFitFlag; // 0~1
    UINT8                           ephemAoda; // 0~31
    INT16                           ephemCic; // -32768~32767
    INT32                           ephemOmegaA0; // -2147483648~2147483647
    INT16                           ephemCis; // -32768~32767
    INT32                           ephemI0; // -2147483648~2147483647
    INT16                           ephemCrc; // -32768~32767
    INT32                           ephemW; // -2147483648~2147483647
    INT32                           ephemOmegaADot; // -8388608~8388607
    INT16                           ephemIDot; // -8192~8191
}__attribute__((packed))sit_gps_navi_ephe_type;

/* Navigtion Sat Info Type */
typedef struct
{
    UINT8                       satId;
    BYTE                        NavigationSatStatus;
    sit_gps_navi_ephe_type      NavigationEphemeris;
}__attribute__((packed))sit_gps_navi_sat_info_type;

/* Navigation Model Type */
typedef struct
{
    UINT32                      numberOfSat;
    sit_gps_navi_sat_info_type  NavigationSatInfo[16];
}__attribute__((packed))sit_gps_navi_model_type;

/* Iono Model Type */
typedef struct
{
    INT8    alfa0; // -128~127
    INT8    alfa1; // -128~127
    INT8    alfa2; // -128~127
    INT8    alfa3; // -128~127
    INT8    beta0; // -128~127
    INT8    beta1; // -128~127
    INT8    beta2; // -128~127
    INT8    beta3; // -128~127
}__attribute__((packed))sit_gps_iono_model_type;

/* Utc Model Type */
typedef struct
{
    INT32   utcA1; // -8388608~8388607
    INT32   utcA0; // -2147483648~2147483647
    UINT8   utcTot; // 0~255
    UINT8   utcWNt; // 0~255
    INT8    utcDeltaTls; // -128~127
    UINT8   utcWNlsf; // 0~255
    INT8    utcDN; // -128~127
    INT8    utcDeltaTlsf; // -128~127
}__attribute__((packed))sit_gps_utc_model_type;

/* Almanac Sat Info Type */
typedef struct
{
    INT8        dataId; // only for 3G, 0~3, if this value is -1, it means this value is invalid
    UINT8       satId;
    UINT16      almanacE; // 0~65536
    UINT8       almanacToa; // 0~255
    INT16       almanacKsii; // -32768~3276
    INT16       almanacOemgaDot; // -32768~3276
    UINT8       almanacSvHealth; // 0~255
    UINT32      almanacAPowerHalf; // 0~16777215
    INT32       almanacOmega0; // -8388608~8388607
    INT32       almanacW; // -8388608~8388607
    INT32       almanacM0; // -8388608~8388607
    INT16       almanacAf0; // -1024~1023
    INT16       almanacAf1; // -1024~1023
}__attribute__((packed))sit_gps_almanac_sat_info_type;

/* Almanac Type */
typedef struct
{
  UINT8                         almanacWNa; // 0~255
  UINT32                        numberOfSat;
  sit_gps_almanac_sat_info_type AlmanacSatInfo[64];
}__attribute__((packed))sit_gps_almanac_model_type;

/* Acquision Utran Time Type */
typedef struct
{
    sit_gps_utran_gps_ref_time_type     AcqUtranGpsRefTime;
    sit_gps_utran_gps_unc_type          AcqUtranGpsUncertainty;
}__attribute__((packed))sit_gps_acq_utran_time_type;

/* Acquisition Sat Info Type */
typedef struct
{
    UINT8                           satId;
    INT16                           doppler0; // -2048~2047 (real value is from -5120 to 5117.5 by step of 2.5)
    UINT8                           doppler1; // 0~63 (real value is from -0.966 to 0.483 by step of 0.023)
    UINT8                           dopplerUncertainty; // 0~7 (12.5, 25, 50, 100, 200)
    UINT16                          codePhase; // 0~1022
    UINT8                           intCodePhase; // 0~19
    UINT8                           gpsBitNumber; // 0~3
    UINT8                           codePhaseSearchWindow; // 0~15 (1023, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192)
    UINT8                           azimuth; // 0~31, 11.25 degree resolution
    UINT8                           elevation; // 0~7, 11.25 degree resolution
}__attribute__((packed))sit_gps_acq_sat_info_type;

/* Acquisition Assist Type */
typedef struct
{
    UINT32      gpsTow;
    union
    {
        sit_gps_gsm_time_type   gsm_time;
        sit_gps_acq_utran_time_type AcqUtranTime;
    } acquisitionTimeInfo;
    UINT32      numberOfSat;
    sit_gps_acq_sat_info_type   lcsAcquisitionSatInfo[16];
}__attribute__((packed))sit_gps_acq_assist_type;

/* Real Time Integrity Type */
typedef struct
{
    UINT8       satId[16];
    UINT8       numOfSat;
}__attribute__((packed))sit_gps_r_time_int_type;

typedef struct
{
    UINT8 shapeType;
    UINT8 hemisphere;
    UINT16 altitude;
    UINT32 latitude;
    INT32 longitude;
    UINT8 directionOfAlt;
    UINT8 semiMajorUncert;
    UINT8 semiMinorUncert;
    UINT8 majorAxis;
    UINT8 altUncert;
    UINT8 confidence;
}__attribute__((packed))sit_gps_ref_loc_type;

/*----------------------------------------------------------------
    GPS Measure Position Message
*/

/* Accuracy Type */
typedef struct
{
    UINT32      flag;
    UINT8       horizontalAccuracy;
    UINT8       vertcalAccuracy;
}__attribute__((packed))sit_gps_m_accuracy_type;

/* GPS Measurement Element Type */
typedef struct
{
    UINT8               satId;
    UINT8               cno; // 0~63, unit of dB-Hz
    INT16               doppler; // -32768~32767, Hz and scale factor 0.2
    UINT16              wholeChips; // 0~1022
    UINT16              fracChips; // 0~1024
    BYTE                lcsMultiPath;
    UINT8               pseuRangeRmsErr; // 0~63
}__attribute__((packed))sit_gps_measuremet_element_type;

/* GPS Measure Type */
typedef struct
{
    UINT32              gpsTow; ///< GPS time of week [msec]
    UINT16              gpsWeek; ///< GPS week [0 .. 1023]
    UINT8               nrOfSats; ///< number of satellites [1 .. 16]
    sit_gps_measuremet_element_type GpsMeasure[16];
}__attribute__((packed))sit_gps_measure_type;

/* GPS Measure Location Info Type */
typedef struct
{
    UINT32              gpsTow; ///< GPS time of week [msec]
    UINT16              gpsWeek; ///< GPS week [0 .. 1023]
    BYTE                fixType; ///< Fix type. 2D(0x01) or 3D.
    sit_gps_loc_info_type   measured_loc_info;
}__attribute__((packed))sit_gps_measure_loc_info_type;

/*----------------------------------------------------------------
    GPS MTLR Notification Message
*/

/* MTLR Location Type */
typedef struct
{
    UINT16  deferredLocEventType;
    BYTE    locEstimateType;
}__attribute__((packed))sit_gps_mtlr_loc_type;

/* String type */
typedef struct
{
    UINT32  length;
    UINT8   val[63];
}__attribute__((packed))sit_gps_string_type;

/* DCS String Type */
typedef struct
{
    UINT8  dcs;
    sit_gps_string_type  str;
    BYTE  format_indicator;
}__attribute__((packed))sit_gps_dcs_string_type;

/* Code word Type */
typedef struct
{
    UINT8  dcs;
    sit_gps_string_type  str;
}__attribute__((packed))sit_gps_code_word_type;

/*----------------------------------------------------------------
    LPP Request Location Information
*/

/***CommonIEsRequestLocationInformation*/

typedef struct {
    UINT8 locationInformation;
    #define locationEstimateRequired        1
    #define locationMeasurementsRequired    2
    #define locationEstimatePreferred       3
    #define locationMeasurementsPreferred   4
}  __attribute__((packed)) sit_lpp_locationInformationType_Type;

typedef struct {
    UINT8                                   reportingAmount;
    #define ra1                         1
    #define ra2                         2
    #define ra4                         3
    #define ra8                         4
    #define ra16                        5
    #define ra32                        6
    #define ra64                        7
    #define ra_Infinity                 8
}  __attribute__((packed)) sit_lpp_reportingAmount_Type;

typedef struct {
    UINT8                                   reportingInterval;
    #define noPeriodicalReporting       1
    #define ri0_25                      2
    #define ri0_5                       3
    #define ri1                         4
    #define ri2                         5
    #define ri4                         6
    #define ri8                         7
    #define ri16                        8
    #define ri32                        9
    #define ri64                        10
}  __attribute__((packed)) sit_lpp_reportingInterval_Type;

typedef struct {
  sit_lpp_reportingAmount_Type              reportingAmount;
  sit_lpp_reportingInterval_Type            reportingInterval;
}  __attribute__((packed)) sit_lpp_periodicalReporting_Type;

typedef struct {
  UINT8                                     ReportingDuration;
}  __attribute__((packed)) sit_lpp_reporting_Duration_Type;


typedef struct {
  BOOL                                   cellChange;
  sit_lpp_reporting_Duration_Type           reportingDuration;
}  __attribute__((packed)) sit_lpp_triggeredReporting_Type;

typedef struct {
    UINT8                                       additionalInformation;
    #define OEM_onlyReturnInformationRequested  1
    #define OEM_mayReturnAditionalInformation   2
}  __attribute__((packed)) sit_lpp_additionalInformation_Type;

typedef struct {
  UINT8                                     accuracy;
  UINT8                                     confidence;
}  __attribute__((packed)) sit_lpp_horizontalAccuracy_Type;

typedef struct {
  UINT8                                     accuracy;
  UINT8                                     confidence;
}  __attribute__((packed)) sit_lpp_verticalAccuracy_Type;

typedef struct {
  UINT8                                     time;
}  __attribute__((packed)) sit_lpp_responseTime_Type;

typedef struct {
  UINT8         bitMask;
    #define BM_HORIZONTAL_ACCURACY          0x01
    #define BM_VERTICAL_ACCURACY            0x02
    #define BM_RESPONSE_TIME                0x04
  sit_lpp_horizontalAccuracy_Type           horizontalAccuracy;
  BOOL                                   verticalCoordinateRequest;
  sit_lpp_verticalAccuracy_Type             verticalAccuracy;
  sit_lpp_responseTime_Type                 responseTime;
  BOOL                                   velocityRequest;
}  __attribute__((packed)) sit_lpp_qos_Type;

typedef struct {
    UINT8                                   environment;
    #define badArea                     1
    #define notBadArea                  2
    #define mixedArea                   3
}  __attribute__((packed)) sit_lpp_environment_Type;

typedef struct {
    BOOL                                 ellipsoidPoint;
    BOOL                                 ellipsoidPointWithUncertaintyCircle;
    BOOL                                 ellipsoidPointWithUncertaintyEllipse;
    BOOL                                 polygon;
    BOOL                                 ellipsoidPointWithAltitude;
    BOOL                                 ellipsoidPointWithAltitudeAndUncertaintyEllipsoid;
    BOOL                                 ellipsoidArc;
}  __attribute__((packed)) sit_lpp_locationCoordinateTypes_Type;

typedef struct {
    UINT8                                                   velocityTypes;
    #define HORIZENTAL_VELOCITY                         1
    #define HORIZENTAL_WITH_VERTICAL_VELOCITY           2
    #define HORIZENTAL_VELOCITY_WITH_UNCERTAINTY        3
    #define HORIZENTAL_WITH_VERTICAL_VELOCITY_AND_UNC   4
} sit_lpp_velocityTypes;

typedef struct {
  UINT8   bitMask;

  #define  BM_TRIGGERED_REPORTING           0x01
  #define  BM_PERIODICAL_REPORTING          0x02
  #define  BM_ADDITIONAL_INFORMATION        0x04
  #define  BM_QOS                           0x08
  #define  BM_ENVIRONMENT                   0x10
  #define  BM_LOCATION_COORDINATE_TYPES     0x20
  #define  BM_VELOCITY_TYPES                0x40

  sit_lpp_locationInformationType_Type      locationInformationType;
  sit_lpp_triggeredReporting_Type           triggeredReporting;
  sit_lpp_periodicalReporting_Type          periodicalReporting;
  sit_lpp_additionalInformation_Type        additionalInformation;
  sit_lpp_qos_Type                          qos;
  sit_lpp_environment_Type                  environment;
  sit_lpp_locationCoordinateTypes_Type      locationCoordinateTypes;
  sit_lpp_velocityTypes                     velocityTypes;
}  __attribute__((packed)) sit_lpp_CommonIEsRequestLocationInformation;

typedef struct {
    UINT16                                  primaryCPICHInfo;
}  __attribute__ ((packed)) sit_lpp_fdd_Type;

typedef struct {
    UINT8                                   cellParameters;
}  __attribute__ ((packed)) sit_lpp_tdd_Type;

typedef struct {
    UINT8                                   modeType;
    #define FDD_TYPE                        0x01
    #define TDD_TYPE                        0x02
    union {
        sit_lpp_fdd_Type                    fdd;
        sit_lpp_tdd_Type                    tdd;
    } u_mode;
}  __attribute__((packed)) sit_lpp_mode_Type;

typedef struct {
    UINT8                                   cidType;
    #define EUTRA                           0x01
    #define UTRA                            0x02
    union {
        UINT32                              eutra;
        UINT32                              utra;
    } u_cid;
}  __attribute__((packed)) sit_lpp_cell_Identity_Type;

typedef struct {
    UINT8                                   mcc[3];
    UINT8                                   mnc[3];
    UINT8                                   sizeOfMcc;
    UINT8                                   sizeOfMnc;
}  __attribute__((packed)) sit_lpp_plmn_Identity_Type;

typedef struct {
    sit_lpp_plmn_Identity_Type              plmnIdentity;
    sit_lpp_cell_Identity_Type              cellIdentity;
}  __attribute__((packed)) sit_lpp_CellGlobalIdEUTRA_AndUTRA_Type;

typedef struct {
    UINT8                                   bitMask;
    #define            BM_CELL_GLOBAL_ID    0x01
    sit_lpp_mode_Type                       mode;
    sit_lpp_CellGlobalIdEUTRA_AndUTRA_Type  cellGlobalId;
    UINT16                                  referenceSystemFrameNumber;
}  __attribute__((packed)) sit_lpp_uTRA_Type;

typedef struct {
    UINT8                                   bitMask;
    #define     BM_REFRENCE_FNMSB           0x01
    UINT16                                  referenceFN;
    UINT8                                   referenceFNMSB;
}  __attribute__((packed)) sit_lpp_referenceFrame_Type;

typedef struct {
    sit_lpp_plmn_Identity_Type              plmnIdentity;
    UINT16                                  locationAreaCode;
    UINT16                                  cellIdentity;
}  __attribute__((packed)) sit_lpp_cellGlobalIdGERAN_Type;

typedef struct {
    UINT8                                   bitMask;

    #define        BM_CELL_GLOBAL_ID        0x01
    #define        BM_DELTA_GNSS_TOD        0x02

    UINT16                                  bcchCarrier;
    UINT8                                   bsic;
    sit_lpp_cellGlobalIdGERAN_Type          cellGlobalId;
    sit_lpp_referenceFrame_Type             referenceFrame;
    UINT16                                  delta_gnss_tod;
}  __attribute__((packed)) sit_lpp_gSM_Type;

typedef struct {
    UINT8                                   bitMask;
    #define         BM_CELL_GLOBAL_ID       0x01
    UINT16                                  physCellId;
    sit_lpp_CellGlobalIdEUTRA_AndUTRA_Type  cellGlobalId;
    UINT16                                  systemFrameNumber;
}  __attribute__((packed)) sit_lpp_eUTRA_Type;

typedef struct {
    UINT8                                   networkTimeType;
    #define EUTRA                           0x01
    #define UTRA                            0x02
    #define GSM                             0x04
    union {
        sit_lpp_eUTRA_Type                  eUTRA;
        sit_lpp_uTRA_Type                   uTRA;
        sit_lpp_gSM_Type                    gSM;
    } u_networkTime;
}  __attribute__((packed)) sit_lpp_MeasRefTime_NetworkTime_Type;

typedef struct {
    UINT8   gnss_id;
    #define GNSS_GPS                    0
    #define GNSS_SBASS                  1
    #define GNSS_QZSS                   2
    #define GNSS_GALILEO                3
    #define GNSS_GLONASS                4
}  __attribute__((packed)) sit_lpp_GNSS_ID_Type;

typedef struct {
    UINT8                                   bitMask;

    #define           BM_GNSS_TOD_FRAC      0x01
    #define           BM_GNSS_TOD_UNC       0x02
    #define           BM_NETWORK_TIME       0x04

    UINT32                                  gnssTODMsec;
    UINT16                                  gnssTODFrac;
    UINT16                                  gnssTODUnc;
    sit_lpp_GNSS_ID_Type                    gnssTimeID;
    sit_lpp_MeasRefTime_NetworkTime_Type    networkTime;
} __attribute__((packed)) sit_lpp_MeasurementReferenceTime_Type;

typedef struct {
    UINT16                                  gnssIds;
    #define GNSS_GPS_IDS                0x01
    #define GNSS_SBAS_IDS               0x02
    #define GNSS_QZSS_IDS               0x04
    #define GNSS_GALILEO_IDS            0x08
    #define GNSS_GLONASS_IDS            0x10
} __attribute__((packed)) sit_lpp_GNSS_ID_Bitmap_Type;

typedef struct {
    sit_lpp_MeasurementReferenceTime_Type   measurementReferenceTime;
    sit_lpp_GNSS_ID_Bitmap_Type             agnssList;
} __attribute__((packed)) sit_lpp_GNSS_LocationInformation_Type;

typedef struct {
    UINT8                                   mpathDet;
//    #define notMeasured                 0
//    #define low                         1
//    #define medium                      2
//    #define high                        3
} sit_lpp_mpathDet;

typedef struct {
    UINT8                                   satellite_id;
} __attribute__((packed)) sit_lpp_SV_ID_Type;

typedef struct {
    UINT8                                   bitMask;

    #define         BM_CARRIER_QUALITY_IND  0x01
    #define         BM_INTEGER_CODE_PHASE   0x02
    #define         BM_DOPPLER              0x04
    #define         BM_ADR                  0x08

    sit_lpp_SV_ID_Type                      sv_ID;
    UINT8                                   cNo;
    sit_lpp_mpathDet                        mpathDet;
    UINT8                                   carrierQuialityInd;
    UINT32                                  codePhase;
    UINT16                                  intergerCodePhase;
    UINT16                                  coePhaseRMSError;
    INT16                                   doppler;
    UINT32                                  adr;
} __attribute__((packed)) sit_lpp_GNSS_SatMeasElement_Type;

typedef struct {
    UINT8                                   signalID;
} __attribute__((packed)) sit_lpp_GNSS_SignalID_Type;

typedef struct {
    UINT8                                   bitMask;
    #define      BM_CODE_PHASE_AMBIGUITY    0x01
    sit_lpp_GNSS_SignalID_Type              signalId;
    UINT16                                  codePhaseAmbiguity;
    sit_lpp_GNSS_SatMeasElement_Type        satMeasList[LPP_MAX_SVs_CNT];
    UINT8                                   sizeOfMeasurementList;
} __attribute__((packed)) sit_lpp_GNSS_SgnMeasElement_Type;

typedef struct {
    sit_lpp_GNSS_ID_Type                    gnssId;
    sit_lpp_GNSS_SgnMeasElement_Type        gnssSgnMeasList;
    UINT8                                   sizeOfMeasurementList;
} __attribute__((packed)) sit_lpp_GNSS_MeasurementForOneGNSS_Type;

typedef struct {
    sit_lpp_MeasurementReferenceTime_Type   measureRefTime;
    sit_lpp_GNSS_MeasurementForOneGNSS_Type measurementList[LPP_MAX_AGNSS_CNT];
    UINT8                                   sizeOfMeasurementList;
} __attribute__((packed)) sit_lpp_GNSS_SignalMeasurementInformation_Type;

typedef struct {
  UINT8                                             bitMask;

  #define      BM_SIGNAL_MEASURE_INFO               0x01
  #define      BM_LOCATION_INFO                     0x02
  #define      BM_ERROR                             0x04

    sit_lpp_GNSS_SignalMeasurementInformation_Type  signalMeasurementInformation;
    sit_lpp_GNSS_LocationInformation_Type           locationInformation;
}  __attribute__((packed)) sit_lpp_A_GNSS_ProvideLocationInformation;


/*----------------------------------------------------------------
    GPS LPP Provide Assist Data
*/

typedef struct {
    UINT8                                   satelliteID;
    UINT16                                  tlmWord;
    UINT8                                   antiSpoof;
    UINT8                                   alert;
    UINT8                                   tlmRsvdBits;
} __attribute__((packed)) sit_lpp_GPS_TOW_AssistElement_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_TIME_OF_DAY_FRAC_MSEC        0x01
    #define BM_NOTI_OF_LEAP_SEC             0x02
    #define BM_TOW_ASSIST                   0x04
    sit_lpp_GNSS_ID_Type                    timeID;
    UINT16                                  dayNumber;
    UINT32                                  timeOfDay;
    UINT16                                  timeOfDayFrac_msec;
    UINT8                                   notiOfLeapSec;
    sit_lpp_GPS_TOW_AssistElement_Type      towAssist[64];
    UINT8                                   sizeOfTowAssist;
} __attribute__((packed)) sit_lpp_GNSS_SystemTime_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_CELL_GLOBAL_ID_EUTRA         0x01
    UINT16                                  physCellId;
    sit_lpp_CellGlobalIdEUTRA_AndUTRA_Type  cellGlobalIdEUTRA;
    UINT16                                  earfcn;
} __attribute__((packed)) sit_lpp_cellIDeUTRA_Type;

typedef struct {
    UINT8   bitMask;
    #define BM_CELL_GLOBAL_ID_UTRA          0x01
    sit_lpp_mode_Type                       mode;
    sit_lpp_CellGlobalIdEUTRA_AndUTRA_Type  cellGlobalIdEUTRA;
    UINT16                                  uarfcn;
} __attribute__((packed)) sit_lpp_cellIDuTRA_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_CELL_GLOBAL_ID_GSM           0x01
    UINT16                                  bcchCarrier;
    UINT8                                   bsic;
    sit_lpp_cellGlobalIdGERAN_Type          cellGlobalIdGERAN;
} __attribute__((packed)) sit_lpp_cellIDgSM_Type;

typedef struct {
    UINT8                                   cellIDType;
    #define CELL_ID_EUTRA                   0x01
    #define CELL_ID_UTRA                    0x02
    #define CELL_ID_GSM                     0x04
    union {
       sit_lpp_cellIDeUTRA_Type             eUTRA;
       sit_lpp_cellIDuTRA_Type              uTRA;
       sit_lpp_cellIDgSM_Type               gSM;
    } u_cellID;
} __attribute__((packed)) sit_lpp_cellID_Type ;

typedef struct {
    UINT8                                   bitMask;
    #define BM_FRAME_DRIFT                  0x01
    UINT16                                  secondsFromFrameStructureStart;
    UINT32                                  fractionalSecondsFromFrameStructureStart;
    UINT8                                   frameDrift;
    sit_lpp_cellID_Type                     cellId;
} __attribute__((packed)) sit_lpp_NetworkTime_Type;


typedef struct {
    UINT8                                   bitMask;
    #define BM_BS_ALIGN                     0x01
    sit_lpp_NetworkTime_Type                networkTime;
    UINT8                                   referenceTimeUnc;
    BOOL                                 bsAlign;
} __attribute__((packed)) sit_lpp_GNSS_ReferenceTimeForOneCell;

typedef struct {
    UINT8                                   bitMask;
    #define     BM_REF_TIME_UNC             0x01
    #define     BM_REF_TIME_FOR_CELLS       0x02
    sit_lpp_GNSS_SystemTime_Type            systemTime;
    UINT8                                   refTimeUnc;
    sit_lpp_GNSS_ReferenceTimeForOneCell    refTimeForCells[16];
    UINT8                                   sizeOfRefTimeForCells;
} __attribute__((packed)) sit_lpp_GNSS_ReferenceTime_Type;

typedef struct {
    UINT8                                   latitudeSign;
    #define north                       0
    #define south                       1
} sit_lpp_latitudeSign;

typedef struct {
    UINT8                                   altitudeDirection;
    #define alt_height                      0
    #define alt_depth                       1
} sit_lpp_AltitudeDirection;

typedef struct {
    sit_lpp_latitudeSign                    latitudeSign;
    UINT32                                  degreesLatitude;
    INT32                                   degreesLongitude;
    sit_lpp_AltitudeDirection               altitudeDirection;
    UINT16                                  altitude;
    UINT8                                   uncertaintySemiMajor;
    UINT8                                   uncertaintySemiMinor;
    UINT8                                   orientationMajorAxis;
    UINT8                                   uncertaintyAltitude;
    UINT8                                   confidence;
} __attribute__((packed)) sit_lpp_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_Type;

typedef struct {
    sit_lpp_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid_Type  threeDlocation;
} __attribute__((packed)) sit_lpp_GNSS_ReferenceLocation_Type;

typedef struct {
    UINT8                                   dataID;
    INT8                                    alfa0;
    INT8                                    alfa1;
    INT8                                    alfa2;
    INT8                                    alfa3;
    INT8                                    beta0;
    INT8                                    beta1;
    INT8                                    beta2;
    INT8                                    beta3;
} __attribute__((packed)) sit_lpp_KlobucharModelParameter_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_IONO_STORM_FLAG1             0x01
    #define BM_IONO_STORM_FLAG2             0x02
    #define BM_IONO_STORM_FLAG3             0x04
    #define BM_IONO_STORM_FLAG4             0x08
    #define BM_IONO_STORM_FLAG5             0x10
    UINT16                                  ai0;
    UINT16                                  ai1;
    UINT16                                  ai2;
    UINT8                                   ionoStormFlag1;
    UINT8                                   ionoStormFlag2;
    UINT8                                   ionoStormFlag3;
    UINT8                                   ionoStormFlag4;
    UINT8                                   ionoStormFlag5;
} __attribute__((packed)) sit_lpp_NeQuickModelParameter_Type;

typedef struct {
  UINT8                                     bitMask;
  #define           BM_KLOBUCHAR_MODEL      0x01
  #define           BM_NE_QUICK_MODEL       0x02
    sit_lpp_KlobucharModelParameter_Type    klobucharModel;
    sit_lpp_NeQuickModelParameter_Type      neQuickModel;
} __attribute__((packed)) sit_lpp_GNSS_IonosphericModel_Type;

typedef struct {
    UINT16                                  teop;
    INT32                                   pmX;
    INT16                                   pmXdot;
    INT32                                   pmY;
    INT16                                   pmYdot;
    INT32                                   deltaUT1;
    INT32                                   deltaUT1dot;
} __attribute__((packed)) sit_lpp_GNSS_EarthOrientationParameters_Type;

typedef struct {
    UINT8                                           bitmask;
    #define BM_REF_TIME                             0x01
    #define BM_REF_LOCATION                         0x02
    #define BM_IONO_MODEL                           0x04
    #define BM_EARTH_ORIENTATION_PARAMETERS         0x08
    sit_lpp_GNSS_ReferenceTime_Type                 refTime;
    sit_lpp_GNSS_ReferenceLocation_Type             refLocation;
    sit_lpp_GNSS_IonosphericModel_Type              ionoModel;
    sit_lpp_GNSS_EarthOrientationParameters_Type    earthOrientationParameters;
} __attribute__((packed)) sit_lpp_GNSS_Common_Assist_Data_Type;

typedef struct {
    UINT8   sbas_id;
    #define GNSS_WASS                   0
    #define GNSS_EGNOS                  1
    #define GNSS_MSAS                   2
    #define GNSS_GAGAN                  3
} __attribute__((packed)) sit_lpp_SBAS_ID_Type;

typedef struct {
    UINT8                                   bitMask;

    #define BM_TA1                          0x01
    #define BM_TA2                          0x02
    #define BM_WEEK_NUMBER                  0x04
    #define BM_DELTA_T                      0x08

    UINT16                                  gnss_TimeModelRefTime;
    INT32                                   tA0;
    INT16                                   tA1;
    INT8                                    tA2;
    UINT8                                   gnssToID;
    UINT16                                  weekNumber;
    INT8                                    deltaT;
}  __attribute__((packed)) sit_lpp_GNSS_TimeModelElement_Type;

typedef struct {
    UINT8                                                       statusHealth;
    #define _1_dot_0                                         0
    #define _0_dot_75                                        1
    #define _0_dot_5                                         2
    #define _0_dot_3                                         3
    #define _0_dot_2                                         4
    #define _0_dot_1                                         5
    #define _Reference_Station_Transmission_Not_Monitored    6
    #define _Data_is_invalid_disregard                       7
} sit_lpp_statusHealth;

typedef struct {
    UINT8                                   udre;
    #define UDRE_LESS_1M                0
    #define UDRE_MORE_1M_LESS_4M        1
    #define UDRE_MORE_4M_LESS_8M        2
    #define UDRE_MORE_8M0x11            3
} sit_lpp_udre;

typedef struct{
    UINT8                                   uderGrowthRate;
    #define __1_dot_5                     0
    #define __2                           1
    #define __4                           2
    #define __6                           3
    #define __8                           4
    #define __10                          5
    #define __12                          6
    #define __16                          7
} sit_lpp_udreGrowthRate;

typedef struct{
    UINT8                                   udreValidityTime;
    #define __20                          0
    #define __40                          1
    #define __80                          2
    #define __160                         3
    #define __320                         4
    #define __640                         5
    #define __1280                        6
    #define __2560                        7
} sit_lpp_udreValidityTime;

typedef struct {
  UINT8                                     bitMask;

  #define      BM_UDRE_GROWTH_RATE          0x01
  #define      BM_UDRE_VALIDITY_TIME        0x02

  sit_lpp_SV_ID_Type                        svID;
  UINT16                                    iod;
  sit_lpp_udre                              udre;
  INT16                                     pseudoRangeCor;
  INT8                                      rangeRateCor;
    sit_lpp_udreGrowthRate                  udreGrwothRate;
    sit_lpp_udreValidityTime                udreValidityTime;
}  __attribute__((packed)) sit_lpp_DGNSS_CorrectionsElement_Type;

typedef struct {
  sit_lpp_GNSS_SignalID_Type                signalID;
  sit_lpp_statusHealth                      statusHealth;
  sit_lpp_DGNSS_CorrectionsElement_Type     dgnssSatList[LPP_MAX_SVs_CNT];
    UINT8                                   sizeOfDgnssSatList;
}  __attribute__((packed)) sit_lpp_DGNSS_SgnTypeElement_Type;

typedef struct {
    UINT16                                  refTime;
    sit_lpp_DGNSS_SgnTypeElement_Type       sgnTypeList;
}  __attribute__((packed)) sit_lpp_GNSS_DifferentialCorrections_Type;

typedef struct {
    UINT8                                   stanModelId;
    #define I_NAV                       0
    #define F_NAV                       1
} sit_lpp_stanModelId;

typedef struct {
    UINT8                                   bitMask;

    #define    BM_STAN_CLOCK_TGD            0x01
    #define    BM_STAN_MODEL_ID             0x02

    UINT16                                  stanClockToc;
    INT16                                   stanClockAF2;
    INT32                                   stanClockAF1;
    INT32                                   stanClockAF0;
    INT16                                   stanClockTgd;
    sit_lpp_stanModelId                     stanModelID;
}  __attribute__((packed)) sit_lpp_StandardClockModelElement_Type;

typedef struct {
    UINT16                                  navToc;
    INT8                                    navaf2;
    INT16                                   navaf1;
    INT32                                   navaf0;
    INT8                                    navTgd;
}  __attribute__((packed)) sit_lpp_NAV_ClockModel_Type;

typedef struct {
    UINT8                                   bitMask;

    #define               BM_CNAV_ISCL1_CP  0x01
    #define               BM_CNAV_ISCL1_CD  0x02
    #define               BM_CNAV_ISCL1_CA  0x04
    #define               BM_CNAV_ISCL2_C   0x08
    #define               BM_CNAV_ISCL5_I5  0x10
    #define               BM_CNAV_ISCL5_Q5  0x20

    UINT16                                  cnavToc;
    UINT16                                  cnavTop;
    INT8                                    cnavURA0;
    UINT8                                   cnavURA1;
    UINT8                                   cnavURA2;
    INT16                                   cnavAf2;
    INT32                                   cnavAf1;
    INT32                                   cnavAf0;
    INT16                                   cnavTgd;
    INT16                                   cnavISCl1cp;
    INT16                                   cnavISCl1cd;
    INT16                                   cnavISCl1ca;
    INT16                                   cnavISCl2c;
    INT16                                   cnavISCl5i5;
    INT16                                   cnavISCl5q5;
}  __attribute__((packed)) sit_lpp_CNAV_ClockModel_Type;

typedef struct {
    INT8                                    bitMask;
    #define     BM_GLO_DELTA_TAU            0x01
    INT32                                   gloTau;
    INT16                                   gloGamma;
    INT8                                    gloDeltaTau;
}  __attribute__((packed)) sit_lpp_GLONASS_ClockModel_Type;

typedef struct {
    UINT16                                  sbasTo;
    INT16                                   sbasAgfo;
    INT8                                    sbasAgf1;
}  __attribute__((packed)) sit_lpp_SBAS_ClockModel_Type;

typedef struct {
  UINT8                                         clockModelType;
  #define STAND_CLK_MODEL                       0x01
    #define NAV_CLK_MODEL                       0x02
    #define CNAV_CLK_MODEL                      0x04
    #define GLONASS_CLK_MODEL                   0x08
    #define SBAS_CLK_MODEL                      0x10
    union {
      sit_lpp_StandardClockModelElement_Type    standardClockModelList[2];
      sit_lpp_NAV_ClockModel_Type               navClockModel;
      sit_lpp_CNAV_ClockModel_Type              cnavClockModel;
      sit_lpp_GLONASS_ClockModel_Type           glonassClockModel;
      sit_lpp_SBAS_ClockModel_Type              sbasClockModel;
    } u_clockModel;
    UINT8                                       sizeOfStandardClockModelList;
}   __attribute__((packed)) sit_lpp_GNSS_ClockModel_Type;

typedef struct {
    UINT16                                  keplerToe;
    INT32                                   keplerW;
    INT16                                   keplerDeltaN;
    INT32                                   keplerM0;
    INT32                                   keplerOmegaDot;
    UINT32                                  keplerE;
    INT16                                   keplerIDot;
    UINT32                                  keplerAPowerHalf;
    INT32                                   keplerI0;
    INT32                                   keplerOmega0;
    INT16                                   keplerCrs;
    INT16                                   keplerCis;
    INT16                                   keplerCus;
    INT16                                   keplerCrc;
    INT16                                   keplerCic;
    INT16                                   keplerCuc;
}  __attribute__((packed)) sit_lpp_NavModelKeplerianSet_Type;

typedef struct {
    UINT32                                  reserved1;
    UINT32                                  reserved2;
    UINT32                                  reserved3;
    UINT16                                  reserved4;
}  __attribute__((packed)) sit_lpp_ephemSF1Rsvd_Type;

typedef struct {
    UINT8                                   ephemCodeOnL2;
    UINT8                                   ephemL2Pflag;
    sit_lpp_ephemSF1Rsvd_Type               ephemSF1Rsvd;
    UINT8                                   ephemAODA;
}  __attribute__((packed)) sit_lpp_addNAVparam_Type;

typedef struct {
    UINT8                                   bitMask;
    #define ADD_NAV_PARAM                   0x01
    UINT8                                   navURA;
    UINT8                                   navFitFlag;
    UINT16                                  navToe;
    INT32                                   navOmega;
    INT16                                   navDeltaN;
    INT32                                   navM0;
    INT32                                   navOmegaADot;
    UINT32                                  navE;
    INT16                                   navIDot;
    UINT32                                  navAPowerHalf;
    INT32                                   navI0;
    INT32                                   navOmegaA0;
    INT16                                   navCrs;
    INT16                                   navCis;
    INT16                                   navCus;
    INT16                                   navCrc;
    INT16                                   navCic;
    INT16                                   navCuc;
  sit_lpp_addNAVparam_Type        addNAVparam;
}  __attribute__((packed)) sit_lpp_NavModelNAV_KeplerianSet_Type;

typedef struct {
    UINT16                                  cnavTop;
    INT8                                    cnavURAindex;
    INT32                                   cnavDeltaA;
    INT32                                   cnavAdot;
    INT32                                   cnavDeltaNo;
    INT32                                   cnavDeltaNoDot;
    LONG                                   cnavMo;
    ULONG                                  cnavE;
    LONG                                   cnavOmega;
    LONG                                   cnavOMEGA0;
    INT32                                   cnavDeltaOmegaDot;
    LONG                                   cnavIo;
    INT16                                   cnavIoDot;
    INT16                                   cnavCis;
    INT16                                   cnavCic;
    INT32                                   cnavCrs;
    INT32                                   cnavCrc;
    INT32                                   cnavCus;
    INT32                                   cnavCuc;
}  __attribute__((packed)) sit_lpp_NavModelCNAV_KeplerianSet_Type;

typedef struct {
    UINT8                                   gloEn;
    UINT8                                   gloP1;
    BOOL                                 gloP2;
    UINT8                                   gloM;
    INT32                                   gloX;
    INT32                                   gloXdot;
    INT8                                    gloXdotdot;
    INT32                                   gloY;
    INT32                                   gloYdot;
    INT8                                    gloYdotdot;
    INT32                                   gloZ;
    INT32                                   gloZdot;
    INT8                                    gloZdotdot;
}  __attribute__((packed)) sit_lpp_NavModel_GLONASS_ECEF_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_SBAS_TO                      0x01
    UINT16                                  sbasTo;
    UINT8                                   sbasAccuracy;
    INT32                                   sbasXg;
    INT32                                   sbasYg;
    INT32                                   sbasZg;
    INT32                                   sbasXgDot;
    INT32                                   sbasYgDot;
    INT32                                   sbasZgDot;
    INT16                                   sbasXgDotDot;
    INT16                                   sbagYgDotDot;
    INT16                                   sbasZgDotDot;

}  __attribute__((packed)) sit_lpp_NavModel_SBAS_ECEF_Type;

typedef struct {
  UINT8                                     orbitModelType;
  #define KELP_SET                          0x01
    #define NAV_KEPL_SET                    0x02
    #define CNAV_KEPL_SET                   0x04
    #define GLONASS_ECEF                    0x08
    #define SBAS_ECEF                       0x10
    union {
  sit_lpp_NavModelKeplerianSet_Type         keplerianSet;
  sit_lpp_NavModelNAV_KeplerianSet_Type     navKeplerianSet;
  sit_lpp_NavModelCNAV_KeplerianSet_Type    cnavKeplerianSet;
  sit_lpp_NavModel_GLONASS_ECEF_Type        glonassECEF;
  sit_lpp_NavModel_SBAS_ECEF_Type           sbasECEF ;
    }u_navModel;
}  __attribute__((packed)) sit_lpp_GNSS_OrbitModel_Type;

typedef struct {
    sit_lpp_SV_ID_Type                      svID;
  UINT8                                     svHealth;
  UINT16                                    iod;
  sit_lpp_GNSS_ClockModel_Type              gnssClockModel;
  sit_lpp_GNSS_OrbitModel_Type              gnssOrbitModel;
} __attribute__((packed)) sit_lpp_GNSS_NavModelSatelliteElement_Type;

typedef struct {
  UINT8                                         nonBroadcastIndFlag;
  sit_lpp_GNSS_NavModelSatelliteElement_Type    gnssSatelliteList[LPP_MAX_SVs_CNT];
    UINT8                                       sizeOfGnssSatelliteList;
} __attribute__((packed)) sit_lpp_GNSS_NavigationModel_Type;

typedef struct {
    UINT8                                   signalIDs;
} __attribute__((packed))sit_lpp_GNSS_SignalIDs_Type;

typedef struct {
  UINT8                                     bitMask;
  #define   BM_BAD_SIGNAL_ID                0x01
    sit_lpp_SV_ID_Type                      badSVID;
    sit_lpp_GNSS_SignalIDs_Type             badSignalID;
} __attribute__((packed)) sit_lpp_BadSignalElement_Type;

typedef struct {
    sit_lpp_GNSS_SignalID_Type              signalType;
    UINT8                                   dataBits[128];
} __attribute__((packed)) sit_lpp_GNSS_DataBitsSgnElement_Type;

typedef struct {
    sit_lpp_SV_ID_Type                      svID;
    sit_lpp_GNSS_DataBitsSgnElement_Type    dataBitsSgnList;
} __attribute__((packed)) sit_lpp_GNSS_DataBitsSatElement_Type;

typedef struct {
    UINT8                                       bitMask;
    #define BM_GNSS_TOD_FRAC                    0x01
    UINT16                                      gnssTOD;
    UINT16                                      gnssTODfrac;
    sit_lpp_GNSS_DataBitsSatElement_Type        dataBitsSatList[LPP_MAX_SVs_CNT];
    UINT8                                       sizeOfGnssDataBitsSatList;
} __attribute__((packed)) sit_lpp_GNSS_DataBitAssistance_Type;

typedef struct {
    UINT8                                   dopplerUncertaintyExt_r10;
    #define _d60                         0x00
    #define _d80                         0x01
    #define _d100                        0x10
    #define _d120                        0x11
    #define _noInformation               0x100
} sit_lpp_dopplerUncertaintyExt_r10;

typedef struct {
    UINT8                                   bitMask;
    #define BM_CODE_PHASE_1023              0x01
    #define BM_DOPPLER_UNCERTAINTY_EXT_R10  0x02
    sit_lpp_SV_ID_Type                      svID;
    INT16                                   doppler0;
    UINT8                                   doppler1;
    UINT8                                   dopplerUncertainty;
    UINT16                                  codePhase;
    UINT8                                   intCodePhase;
    UINT8                                   codePhaseSearchWindow;
    UINT16                                  azimuth;
    UINT8                                   elevation;
    BOOL                                 codePhase1023;
    sit_lpp_dopplerUncertaintyExt_r10       dopplerUncertaintyExt_r10;
} __attribute__((packed)) sit_lpp_GNSS_AcquisitionAssistElement_Type;

typedef struct {
    sit_lpp_GNSS_SignalID_Type                  signalID;
    sit_lpp_GNSS_AcquisitionAssistElement_Type  acqAssistList[LPP_MAX_SVs_CNT];
    UINT8                                       sizeOfAcqAssistList;
    UINT8                                       confidence;
} __attribute__((packed)) sit_lpp_GNSS_AcquisitionAssistance_Type;

typedef struct {

    sit_lpp_SV_ID_Type                      svID;
    UINT16                                  kepAlmanacE;
    INT16                                   kepAlmanacDeltaI;
    INT16                                   kepAlmanacOmegaDot;
    UINT8                                   kepSVHealth;
    INT32                                   kepAlmanacAPowerHalf;
    INT16                                   kepAlmanacOmega0;
    INT16                                   kepAlmanacW;
    INT16                                   kepAlmanacM0;
    INT16                                   kepAlmanacAF0;
    INT16                                   kepAlmanacAF1;
} __attribute__((packed)) sit_lpp_AlmanacKeplerianSet_Type;

typedef struct {

    sit_lpp_SV_ID_Type                      svID;
    UINT16                                  navAlmE;
    INT16                                   navAlmDeltaI;
    INT16                                   navAlmOMEGADOT;
    UINT8                                   navAlmSVHealth;
    UINT32                                  navAlmSqrtA;
    INT32                                   navAlmOMEGAo;
    INT32                                   navAlmOmega;
    INT32                                   navAlmMo;
    INT16                                   navAlmaf0;
    INT16                                   navAlmaf1;

} __attribute__((packed)) sit_lpp_AlmanacNAV_KeplerianSet_Type;

typedef struct {

    sit_lpp_SV_ID_Type                      svID;
    INT8                                    redAlmDeltaA;
    INT8                                    redAlmOmega0;
    INT8                                    redAlmPhi0;
    BOOL                                 redAlmL1Health;
    BOOL                                 redAlmL2Health;
    BOOL                                 redAlmL5Health;
} __attribute__((packed)) sit_lpp_AlmanacReducedKeplerianSet_Type;

typedef struct {
    sit_lpp_SV_ID_Type                      svID;
    UINT16                                  midiAlmE;
    INT16                                   midiAlmDeltaI;
    INT16                                   midiAlmOmegaDot;
    UINT32                                  midiAlmSqrtA;
    INT16                                   midiAlmOmega0;
    INT16                                   midiAlmOmega;
    INT16                                   midiAlmMo;
    INT16                                   midiAlmaf0;
    INT16                                   midiAlmaf1;
    BOOL                                 midiAlmL1Health;
    BOOL                                 midiAlmL2Health;
    BOOL                                 midiAlmL5Health;
} __attribute__((packed)) sit_lpp_AlmanacMidiAlmanacSet_Type;

typedef struct {
    UINT8                                   bitMask;

    #define      BM_GLOALM_MA               0x01

    UINT16                                  gloAlm_NA;
    UINT8                                   gloAlmnA;
    UINT8                                   gloAlmHA;
    INT32                                   gloAlmLambdaA;
    UINT32                                  gloAlmtlambdaA;
    INT32                                   gloAlmDeltaIa;
    INT32                                   gloAlmDeltaTA;
    INT8                                    gloAlmDeltaTdotA;
    UINT16                                  gloAlmEpsilonA;
    INT16                                   gloAlmOmegaA;
    INT16                                   gloAlmTauA;
    UINT8                                   gloAlmCA;
    UINT8                                   gloAlmMA;
} __attribute__((packed)) sit_lpp_AlmanacGLONASS_AlmanacSet_Type;
typedef struct {

    UINT8                                   sbasAlmDataID;
    sit_lpp_SV_ID_Type                      svID;
    UINT8                                   sbasAlmHealth;
    INT16                                   sbasAlmXg;
    INT16                                   sbasAlmYg;
    INT16                                   sbasAlmZg;
    INT8                                    sbasAlmXgdot;
    INT8                                    sbasAlmYgDot;
    INT8                                    sbasAlmZgDot;
    UINT16                                  sbasAlmTo;
} __attribute__((packed)) sit_lpp_AlmanacECEF_SBAS_AlmanacSet_Type;

typedef struct {

    UINT8                                           almType;
    #define ALM_SET                                 0x01
    #define ALM_NAV                                 0x02
    #define ALM_REDUCED                             0x04
    #define ALM_MIDI                                0x08
    #define ALM_GLONASS                             0x10
    #define ALM_SBAS                                0x20

    union {
        sit_lpp_AlmanacKeplerianSet_Type            keplerianAlmanacSet;
        sit_lpp_AlmanacNAV_KeplerianSet_Type        keplerianNAV_Almanac;
        sit_lpp_AlmanacReducedKeplerianSet_Type     keplerianReducedAlmanac;
        sit_lpp_AlmanacMidiAlmanacSet_Type          keplerianMidiAlmanac;
        sit_lpp_AlmanacGLONASS_AlmanacSet_Type      keplerianGLONASS;
        sit_lpp_AlmanacECEF_SBAS_AlmanacSet_Type    ecefSbasAlmanac;
    } u_alm;
} __attribute__((packed)) sit_lpp_GNSS_AlmanacElement_Type;

typedef struct {
  UINT8                                     bitMask;

  #define    BM_ALM_WEEK_NUMBER             0x01
  #define    BM_TOA                         0x02
  #define    BM_IODA                        0x04

    UINT8                                   weekNumber;
    UINT8                                   toa;
    UINT8                                   ioda;
    BOOL                                 completeAlmanacProvided;
    sit_lpp_GNSS_AlmanacElement_Type        almanacList[32];
    UINT8                                   sizeOfAlmanacList;
} __attribute__((packed)) sit_lpp_GNSS_Almanac_Type;

typedef struct {
    INT32                                   utcA1wnt;
    INT32                                   utcA0wnt;
    UINT8                                   utcTot;
    UINT8                                   utcWNt;
    INT8                                    utcDeltaTls;
    UINT8                                   utcWNlsf;
    INT8                                    utcDN;
    INT8                                    utcDeltaTlsf;
    UINT8                                   utcStandardID;

} __attribute__((packed)) sit_lpp_UTC_ModelSet4_Type;

typedef struct {
  UINT8                                     bitMask;

  #define     BM_B1                         0x01
  #define     BM_B2                         0x02
  #define     BM_KP                         0x04

    UINT16                                  nA;
    INT32                                   tauC;
    INT16                                   b1;
    INT16                                   b2;
    UINT8                                   kp;
} __attribute__((packed)) sit_lpp_UTC_ModelSet3_Type;

typedef struct {

    INT16                                   utcA0;
    INT16                                   utcA1;
    INT8                                    utcA2;
    INT8                                    utcDeltaTls;
    UINT16                                  utcTot;
    UINT16                                  utcWNot;
    UINT8                                   utcWNlsf;
    UINT8                                   utcDN;
    INT8                                    utcDeltaTlsf;
} __attribute__((packed)) sit_lpp_UTC_ModelSet2_Type;

typedef struct {

    INT32                                   gnssUtcA1;
    INT32                                   gnssUtcA0;
    UINT8                                   gnssUtcTot;
    UINT8                                   gnssUtcWNt;
    INT8                                    gnssUtcDeltaTls;
    UINT8                                   gnssUtcWNlsf;
    INT8                                    gnssUtcDN;
    INT8                                    gnssUtcDeltaTlsf;
} __attribute__((packed)) sit_lpp_UTC_ModelSet1_Type;

typedef struct {

    UINT8                                   utcType;
    #define UTC_M1                          0x01
    #define UTC_M2                          0x02
    #define UTC_M3                          0x04
    #define UTC_M4                          0x08
    union {
        sit_lpp_UTC_ModelSet1_Type          utcModel1;
        sit_lpp_UTC_ModelSet2_Type          utcModel2;
        sit_lpp_UTC_ModelSet3_Type          utcModel3;
        sit_lpp_UTC_ModelSet4_Type          utcModel4;
    } u_utc;
} __attribute__((packed)) sit_lpp_GNSS_UTC_Model_Type;

typedef struct {
    sit_lpp_SV_ID_Type                      svID;
  sit_lpp_GNSS_SignalIDs_Type               signalsAvailable;
}  __attribute__((packed)) sit_lpp_GNSS_ID_GPS_SatElement_Type;

typedef struct {
  UINT8                                     bitMask;
  #define      BM_CHANNEL_NUMBER            0x01
    sit_lpp_SV_ID_Type                      svID;
    sit_lpp_GNSS_SignalIDs_Type             signalsAvailable;
  UINT8                                     channelNumber;
}  __attribute__((packed)) sit_lpp_GNSS_ID_GLONASS_SatElement_Type;

typedef struct {
    UINT8                                       auxInfoType;
    #define ID_GPS                              0x01
    #define ID_GLONASS                          0x02
    union {
        struct {
            sit_lpp_GNSS_ID_GPS_SatElement_Type     idGps[LPP_MAX_SVs_CNT];
            UINT8                                   sizeOfIdGps;
        }gpsList;

        struct {
            sit_lpp_GNSS_ID_GLONASS_SatElement_Type idGlonass[LPP_MAX_SVs_CNT];
            UINT8                                   sizeOfIdGlonass;
        }glonassList;
    } u_auxInfo;

}  __attribute__((packed)) sit_lpp_GNSS_AuxiliaryInformation_Type;

typedef struct {
    UINT16                                       bitMask;

    #define         BM_SBAS_ID                  0x0001
    #define         BM_TIME_MODELS              0x0002
    #define         BM_DIFF_CORRECTIONS         0x0004
    #define         BM_NAV_MODEL                0x0008
    #define         BM_RTI                      0x0010
    #define         BM_DATA_BIT_ASSISTANCE      0x0020
    #define         BM_ACQ_ASSISTANCE           0x0040
    #define         BM_ALMANAC                  0x0080
    #define         BM_UTC_MODEL                0x0100
    #define         BM_AUXILIARY_INFO           0x0200

    sit_lpp_GNSS_ID_Type                        gnss_ID;
    sit_lpp_SBAS_ID_Type                        sbas_ID;
    sit_lpp_GNSS_TimeModelElement_Type          timeModels;
    sit_lpp_GNSS_DifferentialCorrections_Type   diffCorrections;
    sit_lpp_GNSS_NavigationModel_Type           navModel;
    sit_lpp_BadSignalElement_Type               rti[LPP_MAX_SVs_CNT/2];
    UINT8                                       sizeOfRti;
#if FEATURE_GNSS_LPP_ADITIONAL_SUPPORT
    sit_lpp_GNSS_DataBitAssistance_Type         dataBitAssistance;
#endif
    sit_lpp_GNSS_AcquisitionAssistance_Type     acqAssistance;
    sit_lpp_GNSS_Almanac_Type                   almanac;
    sit_lpp_GNSS_UTC_Model_Type                 utcModel;
  sit_lpp_GNSS_AuxiliaryInformation_Type        auxiliaryInfo;
} __attribute__((packed)) sit_lpp_GNSS_GenericAssistDataElement_Type;

typedef struct {
    sit_lpp_GNSS_ID_Bitmap_Type             gnssMethods;
    BOOL                                 ftaMeasReq;
    BOOL                                 adrMeasReq;
    BOOL                                 multiFreqMeasReq;
    BOOL                                 assistanceAvailability;
} __attribute__((packed)) sit_lpp_gnss_PositioningInstructions_Type;

typedef struct {
    sit_lpp_gnss_PositioningInstructions_Type       posInstructions;
} __attribute__((packed)) sit_lpp_A_GNSS_RequestLocationInformation;


typedef struct {
    UINT8                                       assistType;
    #define CMN_ASSIST_DATA                  0x01
    #define GPS_GENERIC_ASSIST_DATA          0x02
    #define GLONASS_GENERIC_ASSIST_DATA      0x04
    #define MISC_GENERIC_ASSIST_DATA         0x08
    union {
    sit_lpp_GNSS_Common_Assist_Data_Type        cmnAssistData;
    sit_lpp_GNSS_GenericAssistDataElement_Type  gps_genericAssistData;
    sit_lpp_GNSS_GenericAssistDataElement_Type  glonass_genericAssistData;
    sit_lpp_GNSS_GenericAssistDataElement_Type  misc_genericAssistData;
    }u_IEs_type;
} __attribute__((packed)) sit_lpp_A_GNSS_Provide_Assistance_Data;

/*----------------------------------------------------------------
    GPS AP Provide Location Information
*/

typedef struct {
    UINT16                                  bearing;
    UINT16                                  horiSpeed;
}  __attribute__((packed)) sit_lpp_HorizontalVelocityType;

typedef struct {
    UINT8                                   vertiDirection;
    #define OEM_upward                      0
    #define OEM_downward                    1
}__attribute__((packed)) sit_lpp_vertiDirection;

typedef struct {
    UINT16                                  bearing;
    UINT16                                  horiSpeed;
    sit_lpp_vertiDirection                  vertiDirection;
    UINT8                                   vertiSpeed;
}  __attribute__((packed)) sit_lpp_HorizontalWithVerticalVelocityType;


typedef struct {
    UINT16                                  bearing;
    UINT16                                  horiSpeed;
    UINT8                                   uncertaintySpeed;
}  __attribute__((packed)) sit_lpp_HorizontalVelocityWithUncertaintyType;

typedef struct {
    UINT16                                  bearing;
    UINT16                                  horiSpeed;
    sit_lpp_vertiDirection                  vertiDirection;
    UINT8                                   vertiSpeed;
    UINT8                                   horiUncertaintySpeed;
    UINT8                                   vertiUncertaintySpeed;
}  __attribute__((packed)) sit_lpp_HorizontalWithVerticalVelocityAndUncertaintyType;

typedef struct {
    sit_lpp_velocityTypes                                               velocityType;
    #define BM_HORI_VELO                                                0x01
    #define BM_HORI_WITH_VERTI_VELO                                     0x02
    #define BM_HORI_VELO_WITH_UNCERTAINTY                               0x04
    #define BM_HORI_WITH_VERTI_VELO_WITH_UNCERTAINTY                    0x08
    union {
        sit_lpp_HorizontalVelocityType                                  horiVelo;
        sit_lpp_HorizontalWithVerticalVelocityType                      horiWithVertiVelo;
        sit_lpp_HorizontalVelocityWithUncertaintyType                   horiVeloWithUncertainty;
        sit_lpp_HorizontalWithVerticalVelocityAndUncertaintyType        horiWithVertiVeloAndUncertainty;
    }u_velocity;
}  __attribute__((packed)) sit_lpp_Velocity;

typedef struct {
    UINT8                                                       locFailCause;
    #define LPP_LOC_ERROR_UNDEFINED                             0
    #define LPP_REQUESTED_METHOD_NOT_SUPPORTED                  1
    #define LPP_POSITION_METHOD_FAILURE                         2
    #define LPP_PERIODIC_LOCATION_MEASUREMENT_NOT_AVAILABLE     3
    #define LPP_NO_LOCATION_INFORMATION                         4
}  __attribute__((packed)) sit_lpp_LocationError;

typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
}  __attribute__((packed)) sit_lpp_Ellipsoid_Point;

typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
    UINT8                                   uncertainty;
}  __attribute__((packed)) sit_lpp_Ellipsoid_PointWithUncertaintyCircle;

typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
    UINT8                                   uncertainty;
    UINT8                                   uncertaintySemiMaj;
    UINT8                                   uncertaintySemiMin;
    UINT8                                   orientationMajorAxis;
    UINT8                                   confidence;
}  __attribute__((packed)) sit_lpp_EllipsoidPointWithUncertaintyEllipse;

typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
}  __attribute__((packed)) sit_lpp_Polygon;


typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
    sit_lpp_AltitudeDirection               altiDirection;
    UINT16                                  alti;
}  __attribute__((packed)) sit_lpp_EllipsoidPointWithAltitude;

typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
    sit_lpp_AltitudeDirection               altiDirection;
    UINT16                                  alti;
    UINT8                                   uncertaintySemiMajor;
    UINT8                                   uncertaintySemiMinor;
    UINT8                                   orientationMajorAxis;
    UINT8                                   uncertaintyAltitude;
    UINT8                                   confidence;
}  __attribute__((packed)) sit_lpp_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid;

typedef struct {
    sit_lpp_latitudeSign                    latSign;
    UINT32                                  degreeLat;
    INT32                                   degreeLong;
    UINT16                                  innerRadius;
    UINT8                                   uncertaintyRadius;
    UINT8                                   offsetAngle;
    UINT8                                   includedAngle;
    UINT8                                   confidence;
}  __attribute__((packed)) sit_lpp_EllipsoidArc;


typedef struct {
    UINT8   locCorrdinateType;
    #define ELLIPSOID_POINT                                             0x01
    #define ELLIPSOID_POINT_WITH_UNCERTAINTY_CIRCLE                     0x02
    #define ELLIPSOID_POINT_WITH_UNCERTAINTY_ELLIPSE                    0x04
    #define POLYGON                                                     0x08
    #define ELLIPSOID_POINT_WITH_ALTITUDE                               0x10
    #define ELLIPSOID_POINT_WITH_ALTITUDE_AND_UNCERTAINTY_ELLIPSOID     0x20
    #define ELLIPSOID_ARC                                               0x40
    union {
        sit_lpp_Ellipsoid_Point                                         ellipsoidPoint;
        sit_lpp_Ellipsoid_PointWithUncertaintyCircle                    ellipsoidPointWithUncertaintyCircle;
        sit_lpp_EllipsoidPointWithUncertaintyEllipse                    ellipsoidPointWithUncertaintyEllipse;
        sit_lpp_Polygon                                                 polygon;
        sit_lpp_EllipsoidPointWithAltitude                              ellipsoidPointWithAltitude;
        sit_lpp_EllipsoidPointWithAltitudeAndUncertaintyEllipsoid       ellipsoidPointWithAltitudeAndUncertaintyEllipsoid;
        sit_lpp_EllipsoidArc                                            ellipsoidArc;
    } u_locCorrdinateType;
}  __attribute__((packed)) sit_lpp_LocationCoordinates;

typedef struct {
    UINT8   bitMask;
    #define BM_LOCATION_ESTIMATE              0x01
    #define BM_LPP_VELOCITY_ESTIMATE          0x02
    #define BM_LOCATION_ERROR                 0x04
    sit_lpp_LocationCoordinates               locationEstimate;
    sit_lpp_Velocity                          velocityEstimate;
    sit_lpp_LocationError                     locationError;
}  __attribute__((packed)) sit_lpp_CommonIEsProvideLocationInformation;


/*----------------------------------------------------------------
    LPP Implementation
*/

typedef struct {
  UINT32                                    sid;
}__attribute__((packed)) sit_lpp_Header;

  #define  LPP_LOC_INFO_NONE                0x00
  #define  LPP_LOC_INFO_CMN                 0x01
  #define  LPP_LOC_INFO_A_GNSS              0x02
  #define  LPP_LOC_INFO_OTDOA               0x04
  #define  LPP_LOC_INFO_ECID                0x08
  #define  LPP_LOC_INFO_EPDU                0x10

typedef struct {
    UINT8                                   toIds;
    BOOL                                 deltaT;
}  __attribute__((packed)) sit_lpp_GNSS_TimeModelElementReq_Type;

typedef struct {
    sit_lpp_GNSS_SignalIDs_Type             signalsReq;
    BOOL                                 validityTimeReq;
}  __attribute__((packed)) sit_lpp_GNSS_Differential_Corrections_REQ_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_CLOCK_MODEL_ID_PREF_LIST     0x01
    #define BM_ORBIT_MODEL_ID_PREF_LIST     0x02
    #define BM_ADD_NAV_PARAM_REQ            0x04
    ULONG                                  svReqList;
    UINT8                                   clockModelIDPrefList[8];
    UINT8                                   sizeOfClockModelIDPrefList;
    UINT8                                   orbitModelIDPrefList[8];
    UINT8                                   sizeOforbitModelIDPrefList;
    BOOL                                 addNavparamReq;
}  __attribute__((packed)) sit_lpp_ReqNavListInfo_Type;

typedef struct {
    UINT8                                   bitmask;
    #define BM_CLOCK_MODEL_ID               0x01
    #define BM_ORBIT_MODEL_ID               0x02
    UINT8                                   svID;
    UINT16                                  iod;
    UINT8                                   clockModelID;
    UINT8                                   orbitModelID;
}  __attribute__((packed)) sit_lpp_Sat_List_Related_Data_Element_Type;

typedef struct {
    UINT8                                       bitMask;
    #define BM_SAT_LIST_RELATED_DATA_LIST       0x01
    UINT16                                      weekOrDay;
    UINT8                                       toe;
    UINT8                                       toeLimit;
    sit_lpp_Sat_List_Related_Data_Element_Type  satListRelatedDataList[32];
    UINT8                                       sizeOfSatListRelatedDataList;
}  __attribute__((packed)) sit_lpp_Stored_NAV_List_Info_Type;

typedef struct {
    UINT8   modelReqType;
    #define STORED_NAV_LIST                 0x01
    #define REQ_NAV_LIST                    0x02
    union {
        sit_lpp_Stored_NAV_List_Info_Type   storedNavListInfo;
        sit_lpp_ReqNavListInfo_Type         reqNavListInfo;
    } u_modelReq;
}  __attribute__((packed)) sit_lpp_GNSS_Navigation_Model_REQ_Type;

typedef struct {

}  __attribute__((packed)) sit_lpp_GNSS_Real_Time_Integrity_REQ_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_TOD_FRAC                     0x01
    #define BM_DATA_BITS_REQ                0x02
    UINT16                                  tod;
    UINT16                                  todFrac;
    UINT8                                   dataBitInterval;
    sit_lpp_GNSS_SignalIDs_Type             signalIDs;
    UINT8                                   dataBitsReq[64];
    UINT8                                   sizeOfDataBitsReq;
}  __attribute__((packed)) sit_lpp_GNSS_Data_Bit_Assistance_REQ_Type;

typedef struct {
     UINT8                                  signalID;
}  __attribute__((packed)) sit_lpp_GNSS_Acquisition_Assistance_REQ_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_MODEL_ID                     0x01
    UINT8                                   modelID;
}  __attribute__((packed)) sit_lpp_GNSS_Almanac_REQ_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_MODEL_ID                     0x01
    UINT8                                   modelID;
}  __attribute__((packed)) sit_lpp_GNSS_UTC_Model_REQ_Type;

typedef struct {

}  __attribute__((packed)) sit_lpp_GNSS_Auxiliary_Information_REQ_Type;

typedef struct {
    UINT16                                          bitMask;
    #define BM_SBAS_ID                              0x0001
    #define BM_TIME_MODELS                          0x0002
    #define BM_DIFF_CORREECTIONS                    0x0004
    #define BM_NAV_MODEL                            0x0008
    #define BM_RTI                                  0x0010
    #define BM_DATA_BIT_ASSISTANCE                  0x0020
    #define BM_ACQ_ASSISTANCE                       0x0040
    #define BM_ALM                                  0x0080
    #define BM_UTC                                  0x0100
    #define BM_AUXILIARY_INFO                       0x0200
    sit_lpp_GNSS_ID_Type                            gnssID;
    sit_lpp_SBAS_ID_Type                            sbasID;
    sit_lpp_GNSS_TimeModelElementReq_Type           timeModels;
    sit_lpp_GNSS_Differential_Corrections_REQ_Type  diffCorrections;
    sit_lpp_GNSS_Navigation_Model_REQ_Type          navModel;
    sit_lpp_GNSS_Real_Time_Integrity_REQ_Type       rti;
    sit_lpp_GNSS_Data_Bit_Assistance_REQ_Type       dataBitAssistance;
    sit_lpp_GNSS_Acquisition_Assistance_REQ_Type    acqAssistance;
    sit_lpp_GNSS_Almanac_REQ_Type                   alm;
    sit_lpp_GNSS_UTC_Model_REQ_Type                 utc;
    sit_lpp_GNSS_Auxiliary_Information_REQ_Type     auxiliaryInfo;

}  __attribute__((packed)) sit_lpp_GNSS_Generic_Assist_Data_REQ_Element_Type;

typedef struct {

}  __attribute__((packed)) sit_lpp_GNSS_Earth_Orientation_Parameters_REQ_Type;


typedef struct {
    UINT8                                   bitMask;
    #define BM_KLOBUCHAR_MODEL              0x01
    #define BM_NEQUICKMODEL                 0x02
    UINT8                                   klobucharModel;
    UINT8                                   nequickModel; //NULL
} __attribute__((packed)) sit_lpp_GNSS_Ionospheric_Model_REQ_Type;

typedef struct {
} __attribute__((packed)) sit_lpp_GNSS_Reference_Location_REQ_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_TOW                          0x01
    #define BM_NOTI_OF_LEAP_SEC             0x02
    sit_lpp_GNSS_ID_Type                    timeReqPrefList[8];
    UINT8                                   sizeOftimeReqPrefList;
    BOOL                                 tow;
    BOOL                                 notOfLeapSecReq;
} __attribute__((packed)) sit_lpp_GNSS_Reference_Time_REQ_Type;

typedef struct {
    UINT8                                               bitmask;
    #define BM_REF_TIME                                 0x01
    #define BM_REF_LOCATION                             0x02
    #define BM_IONO_MODEL                               0x04
    #define BM_EARTH_ORIENTATION                        0x08
    sit_lpp_GNSS_Reference_Time_REQ_Type                refTime;
    sit_lpp_GNSS_Reference_Location_REQ_Type            refLocation;
    sit_lpp_GNSS_Ionospheric_Model_REQ_Type             ionoModel;
    sit_lpp_GNSS_Earth_Orientation_Parameters_REQ_Type  earthOrientation;
} __attribute__((packed)) sit_lpp_GNSS_Common_Assist_Data_REQ_Type;


typedef struct {
    UINT8                                               bitmask;
    #define  BM_CMN_ASSIST                              0x01
    #define  BM_GEN_ASSIST                              0x02
    sit_lpp_GNSS_Common_Assist_Data_REQ_Type            cmnAssist;
    sit_lpp_GNSS_Generic_Assist_Data_REQ_Element_Type   genAssist[LPP_MAX_AGNSS_CNT];
    UINT8                                               sizeOfGenAssist;
} __attribute__((packed)) sit_lpp_A_GNSS_Request_Assistance_Data;

typedef struct {
    UINT8                                   cellTime;
    #define EUTRA                           0x01
    #define UTRA                            0x02
    #define GSM                             0x04
} sit_lpp_cell_time;

typedef struct {
    UINT8                                   positionModes;
    #define STANDALONE                      0x01
    #define UE_BASED                        0x02
    #define UE_ASSISTED                     0x04
}sit_lpp_position_modes;

typedef struct {
    sit_lpp_cell_time                       cellTime;
    sit_lpp_position_modes                  positionModes;
}  __attribute__((packed)) sit_lpp_FTA_MeasSupport_Type;

typedef struct {
    UINT8                                   sbasIds;
}  __attribute__((packed)) sit_lpp_GNSS_SBAS_IDs_Type;

typedef struct {
    UINT8                                   bitMask;

    #define BM_SBAS_IDS                     0x01
    #define BM_FTA_MEASURE_SUPPORT          0x02

    sit_lpp_GNSS_ID_Type                    gnssID;
    sit_lpp_GNSS_SBAS_IDs_Type              sbasIDs;
    UINT8                                   positionMode;
    sit_lpp_GNSS_SignalIDs_Type             signalIDs;
    sit_lpp_FTA_MeasSupport_Type            ftaMeasureSupport;
    BOOL                                 adrSupport;
    BOOL                                 velocityMeasure;
}  __attribute__((packed)) sit_lpp_GNSS_Support_Element_Type;

typedef struct {
    //NA
}  __attribute__((packed)) sit_lpp_GNSS_TimeModelListSupport_Type;

typedef struct {
    sit_lpp_GNSS_SignalIDs_Type             gnssSignalIDs;
    BOOL                                 validityTimeSup;
}  __attribute__((packed)) sit_lpp_GNSS_DifferentialCorrectionsSupport_Type;

typedef struct {
    UINT8                                   bitMask;

    #define  BM_CLOCK_MODEL                 0x01
    #define  BM_ORBIT_MODEL                 0x02

    UINT8                                   clockModel;
    UINT8                                   orbitModel;
}  __attribute__((packed)) sit_lpp_GNSS_NavigationModelSupport_Type;

typedef struct {
    //NA
}  __attribute__((packed)) sit_lpp_GNSS_RealTimeIntegritySupport_Type;

typedef struct {
    //NA
}  __attribute__((packed)) sit_lpp_GNSS_DataBitAssistanceSupport_Type;

typedef struct {
    UINT8                                   bitMask;

    #define BM_CONFIDENCE_SUPPORT           0x01
    #define BM_DOPPLER_UNC_EXT_SUPPORT      0x02

    BOOL                                 confidenceSupport;
    BOOL                                 dopplerUncExtSupport;
}  __attribute__((packed)) sit_lpp_GNSS_AcquisitionAssistanceSupport_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_ALM_MODEL                    0x01
    UINT8                                   almModel;
    #define MODEL_1                         0x01
    #define MODEL_2                         0x02
    #define MODEL_3                         0x04
    #define MODEL_4                         0x08
    #define MODEL_5                         0x10
    #define MODEL_6                         0x20

}  __attribute__((packed)) sit_lpp_GNSS_AlmanacSupport_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_UTC_MODE                     0x01
    UINT8                                   utcModel;
    #define MODEL_1                         0x01
    #define MODEL_2                         0x02
    #define MODEL_3                         0x04
    #define MODEL_4                         0x08

}  __attribute__((packed)) sit_lpp_GNSS_UTC_ModelSupport_Type;

typedef struct {
    //NA
}  __attribute__((packed)) sit_lpp_GNSS_AuxiliaryInformationSupport_Type;

typedef struct {
    UINT16                                              bitMask;

    #define   BM_SBAS_ID                                0x0001
    #define   BM_TIME_MODELS_SUPPORT                    0x0002
    #define   BM_DIFF_CORRECT_SUPPORT                   0x0004
    #define   BM_NAV_MODEL_SUPPORT                      0x0008
    #define   BM_RTI_SUPPORT                            0x0010
    #define   BM_DATA_BIT_ASSIST_SUPPORT                0x0020
    #define   BM_ACQ_ASSIST_SUPPORT                     0x0040
    #define   BM_ALM_SUPPORT                            0x0080
    #define   BM_UTC_MODEL_SUPPORT                      0x0100
    #define   BM_AUXILIARY_INFO_SUPPORT                 0x0200

    sit_lpp_GNSS_ID_Type                                gnssID;
    sit_lpp_SBAS_ID_Type                                sbasID;
    sit_lpp_GNSS_TimeModelListSupport_Type              timeModelsSupport;
    sit_lpp_GNSS_DifferentialCorrectionsSupport_Type    differentialCorrectionsSupport;
    sit_lpp_GNSS_NavigationModelSupport_Type            navModelSupport;
    sit_lpp_GNSS_RealTimeIntegritySupport_Type          rti_Support;
    sit_lpp_GNSS_DataBitAssistanceSupport_Type          dataBitAssistSupport;
    sit_lpp_GNSS_AcquisitionAssistanceSupport_Type      acqAssistSupport;
    sit_lpp_GNSS_AlmanacSupport_Type                    alm_Support;
    sit_lpp_GNSS_UTC_ModelSupport_Type                  utcModelSupport;
    sit_lpp_GNSS_AuxiliaryInformationSupport_Type       auxiliaryInfoSupport;
}  __attribute__((packed)) sit_lpp_GNSS_GenericAssistDataSupportElement_Type;

typedef struct {
    UINT8                                   accessTypes;
}  __attribute__((packed)) sit_lpp_AccessTypes_Type;

typedef struct {
    UINT8                                   bitMask;
    #define BM_FTA                          0x01
    sit_lpp_GNSS_ID_Bitmap_Type             gnssSystemTime;
    sit_lpp_AccessTypes_Type                fta;
}  __attribute__((packed)) sit_lpp_GNSS_ReferenceTimeSupport_Type;

typedef struct {

}  __attribute__((packed)) sit_lpp_GNSS_ReferenceLocationSupport_Type;

typedef struct {
    UINT8 ionoModel;
}  __attribute__((packed)) sit_lpp_GNSS_IonosphericModelSupport_Type;

typedef struct {

}  __attribute__((packed)) sit_lpp_GNSS_EarthOrientationParametersSupport_Type;


typedef struct {
    UINT8                                                 bitMask;
    #define BM_REF_TIME                                   0x01
    #define BM_REF_LOCATION                               0x02
    #define BM_IONO_MODEL                                 0x04
    #define BM_EARTH_ORIENTATION_PARAMETERS               0x08
    sit_lpp_GNSS_ReferenceTimeSupport_Type                refTime;
    sit_lpp_GNSS_ReferenceLocationSupport_Type            refLocation;
    sit_lpp_GNSS_IonosphericModelSupport_Type             ionoModel;
    sit_lpp_GNSS_EarthOrientationParametersSupport_Type   earthOrientationParameters;
}  __attribute__((packed)) sit_lpp_GNSS_CommonAssistanceDataSupport_Type;



typedef struct {
    sit_lpp_GNSS_CommonAssistanceDataSupport_Type     cmnAssistDataSupport;
    sit_lpp_GNSS_GenericAssistDataSupportElement_Type genAssistDataSupport[LPP_MAX_SVs_CNT];
    UINT8                                             sizeOfGenAssistDataSupport;
}  __attribute__((packed)) sit_lpp_AssistanceDataSupportList_Type;

typedef struct {
    UINT8                                   bitMask;

    #define  BM_SUPPORT_LIST                0x01
    #define  BM_ASSIST_SUPPORT              0x02
    #define  BM_LOCATION_COORDINATE_TYPE    0x04
    #define  BM_VELOCITY_TYPE               0x08

    sit_lpp_GNSS_Support_Element_Type       supportList[LPP_MAX_SVs_CNT];
    UINT8                                   sizeOfSupprotList;
    sit_lpp_AssistanceDataSupportList_Type  assistSupport;
    sit_lpp_locationCoordinateTypes_Type    locationCoordinateTypes;
    sit_lpp_velocityTypes                   velocityType;
}  __attribute__((packed)) sit_lpp_A_GNSS_Capabilities;

//OEM Data Definitions
typedef struct {
    UINT8                     molr_type;
    UINT8                                     location_method;
    sit_gps_qos_type                            qos;
    UINT8                                     client_id[82];
    UINT8                                     mlc_num[82];
    sit_gps_assistance_data_type            assistance_data;
    UINT8                                     gad_shape;
    UINT8                                     serviceTypeID;
    UINT8                                     pseudonymIndicator;
} __attribute__((packed)) oem_gps_mo_loc_req;

typedef struct {
    UINT16          port;
} __attribute__((packed)) oem_gps_supl_ni_ready_req;

typedef struct {
    UINT8 result;
} __attribute__((packed)) oem_gps_supl_ni_ready_rsp;

typedef struct {
    BYTE                                         result;  // 0x00 : SUCCESS, 0x01 : Fail
    UINT32                                       response_type;  //2
    sit_gps_measure_type                         gps_measure;
    sit_gps_measure_loc_info_type                 loc_info;
    sit_gps_assistance_data_type                 measured_assit_data;
    sit_gps_utran_gps_ref_time_type              UtranGpsRefTime; // only for 3G
} __attribute__((packed)) oem_gps_measure_position_rsp;

typedef struct {
    UINT8                 reqId;
    UINT8                 response;
} __attribute__((packed)) oem_gps_mtlr_notification;

typedef struct {
    sit_lpp_Header                          lpp_hdr;
    UINT8                                   flag;
    #define   OEM_LPP_CAPABILITIES_A_GNSS   0x01
    #define   OEM_LPP_CAPABILITIES_EPDU     0x02
    sit_lpp_A_GNSS_Capabilities                 a_gnss;
}__attribute__((packed))oem_gps_lpp_provide_capabilities;

typedef struct {
    sit_lpp_Header                          lpp_hdr;
    UINT8                                   flag;
    #define   OEM_LPP_ASSISTANCE_CMN        0x01
    #define   OEM_LPP_ASSISTANCE_A_GNSS     0x02
    #define   OEM_LPP_ASSISTANCE_OTDOA      0x04
    #define   OEM_LPP_ASSISTANCE_EPDU       0x08
    sit_lpp_A_GNSS_Request_Assistance_Data      a_gnss;
} __attribute__((packed)) oem_gps_lpp_request_assistance_data;

typedef struct {
    sit_lpp_Header                          lpp_hdr;
    UINT8                                   flag;
    #define   OEM_LPP_LOC_INFO_CMN          0x01
    #define   OEM_LPP_LOC_INFO_A_GNSS       0x02
    #define   OEM_LPP_LOC_INFO_OTDOA        0x04
    #define   OEM_LPP_LOC_INFO_ECID         0x08
    #define   OEM_LPP_OC_INFO_EPDU          0x10
    sit_lpp_CommonIEsProvideLocationInformation cmn;
    sit_lpp_A_GNSS_ProvideLocationInformation   a_gnss;
} __attribute__((packed)) oem_gps_lpp_provide_loc_info;

typedef struct {
    sit_lpp_Header                                              lpp_hdr;
    UINT8                                                       flag;
    #define OEM_LPP_ABORT                                       0x01
    #define OEM_LPP_ERROR                                       0x02

    UINT8                                                       statusCode;
    #define OEM_LPP_INVALIED_SESSION_ID                         0x01
    #define OEM_LPP_UNEXPECTED_MSG                              0x02
    #define OEM_LPP_NOT_ALL_REQ_MEASURE_POSSIBLE                0x03
    #define OEM_LPP_AUTH_NET_FAILURE                            0x04
    #define OEM_LPP_MEASURE_TIME_EXPIRED                        0x05

}__attribute__((packed)) oem_gps_lpp_error_info;

typedef struct {
    sit_lpp_Header                      lpp_hdr;
    UINT8                               flag;

    #define OEM_LPP_SUPL_OTODA_REQ_CAP                 0x01
    #define OEM_LPP_SUPL_OTDOA_PROV_ASSIST             0x02
    #define OEM_LPP_SUPL_OTDOA_REQ_LOC                 0x04
    #define OEM_LPP_SUPL_ECID_REQ_CAP                  0x08
    #define OEM_LPP_SUPL_ECID_REQ_LOC                  0x10
    #define OEM_LPP_SUPL_OTDOA_TRIGGER_REQ_ASSIST      0x20

    BYTE     prsInfo[512];
    UINT16   sizeOf;
    UINT8    responseTime;
} __attribute__((packed)) oem_lpp_supl_lppDataInfo;

typedef struct {
    UINT8           tid;
    UINT8               result;
} __attribute__((packed)) oem_gps_supl_ni_msg;

/* ========================================================
    Solicited Messages
// ========================================================*/

//     : Request GPS Frequency Aiding

typedef struct {
    RCM_HEADER                             hdr;
    UINT8                                        enable;
}__attribute__((packed))sit_gps_set_freq_aiding_req;

typedef struct {
    RCM_HEADER                             hdr;
    UINT8                                        lock_status;
    UINT8                                        afc_update;
}__attribute__((packed))sit_gps_set_freq_aiding_rsp;

//     : Request to Get LPP Enhanced Cell ID Information

typedef struct {
    RCM_HEADER  hdr;
    UINT8                                           flag;

    #define LPP_ECID_REQ_CAP                        0x01
    #define LPP_ECID_REQ_LOC                        0x02

    UINT8                                           responseTime;
} __attribute__((packed)) sit_gps_get_lpp_supl_ecid_Info_req;

typedef struct {
    RCM_HEADER          hdr;
    UINT8                                           flag;

    #define LTE_ECID_PROV_CAP                       0x01
    #define LTE_ECID_PROV_LOC                       0x02

    UINT8                                           bitMask;
    #define BM_LTE_RSRP                             0x01
    #define BM_LTE_RSRQ                             0x02
    #define BM_LTE_TA                               0x04
    #define BM_LTE_MRL_LIST                         0x08
    #define BM_LTE_EARFCN                           0x10

    UINT8                                           ecidSupported;
    sit_cgi_info                                    cgiInfo;
    UINT16                                          psyCellId;
    UINT8                                           rsrp;
    UINT8                                           rsrq;
    UINT16                                          ta;
    UINT16                                          earfcn;
    sit_mrl_eutra_info                              mrlList[8];
    UINT8                                           sizeOfMRL;
} __attribute__((packed)) sit_gps_get_lpp_supl_ecid_Info_rsp;

//     : Request to Get RRLP Enhanced Cell ID Information

typedef struct {
    RCM_HEADER                            hdr;

    UINT8                                   flag;
    #define RRLP_ECID_REQ_CAP              0x01
    #define RRLP_ECID_REQ_LOC              0x02

    UINT8                                   responseTime;
}  __attribute__((packed)) sit_gps_get_rrlp_supl_ecid_Info_req;

typedef struct {
    RCM_HEADER                            hdr;

    UINT8                                   flag;
    #define WCDMA_ECID_PROV_CAP             0x01
    #define WCDMA_ECID_PROV_LOC             0x02

    UINT8                                   ecidSupported;

    UINT8                                   cell_info_type;
    #define     GSM_CELL_INFO               0x01
    #define     WCDMA_CELL_INFO             0x02
    union{
        sit_gsm_cell_info                   gsm_cell;
        sit_wcdma_cell_info                 wcdma_cell;
    }u_cellinfo;
}  __attribute__((packed)) sit_gps_get_rrlp_supl_ecid_Info_rsp;

//     : Send MO Location Request

typedef struct {
    RCM_HEADER                  hdr;
    BYTE                          molr_type;
    BYTE                          location_method;
    sit_gps_qos_type              qos;
    BYTE                          client_id[82];
    BYTE                          mlc_num[82];
    sit_gps_assistance_data_type  assistance_data;
    BYTE                          gad_shape;
    BYTE                          serviceTypeID;
    BYTE                          pseudonymIndicator;
}__attribute__((packed))sit_gps_set_mo_loc_req;

typedef struct {
    RCM_HEADER                  hdr;
    sit_gps_loc_info_type         loc_info;
    BYTE                          no_loc;
    sit_gps_deciphering_keys_type decper_keys;
}__attribute__((packed))sit_gps_set_mo_loc_rsp;

//     : Get Serving Cell Information
typedef struct {
    RCM_HEADER                             hdr;
    UINT8                                       enable;
}__attribute__((packed))sit_gps_get_serving_cell_info_req;

typedef struct {
    RCM_HEADER                             hdr;
    BYTE                                       tac[2];
    UINT16                                       psc;
    UINT32                                       cid;
}__attribute__((packed))sit_gps_get_serving_cell_info_rsp;

//     : Request SUPL NI Ready

typedef struct {
    RCM_HEADER  hdr;
    UINT16        port;
} __attribute__((packed)) sit_gps_set_supl_ni_ready_req;

typedef struct {
    RCM_HEADER    hdr;
    UINT16          result;
} __attribute__((packed)) sit_gps_set_supl_ni_ready_rsp;


/* ========================================================
    Unsolicited Messages
// ========================================================*/

/*------------------------From AP to Network-----------------------*/

//     : GPS AP Provide Measure Position

typedef struct {
    RCM_IND_HEADER                                 hdr;
    BYTE                                         result;  // 0x00 : SUCCESS, 0x01 : Fail
    UINT32                                       response_type;
    sit_gps_measure_type                         gps_measure;
    sit_gps_measure_loc_info_type                 loc_info;
    sit_gps_assistance_data_type                 measured_assit_data;
    sit_gps_utran_gps_ref_time_type              UtranGpsRefTime; // only for 3G
}__attribute__((packed))sit_gps_ap_provide_measure_position_ind;

//     : GPS Release GPS
typedef struct {
    RCM_IND_HEADER                              hdr;
}  __attribute__((packed)) sit_gps_rel_gps_ind;

//     : GPS AP MT Location

typedef struct {
    RCM_IND_HEADER                             hdr;
    UINT8                                        reqId;
    BYTE                                         response;
}__attribute__((packed))sit_gps_ap_mtlr_ind;

//     : GPS AP Provide Capabilities

typedef struct {
    RCM_IND_HEADER                              hdr;
    sit_lpp_Header                            lpp_hdr;
    UINT8                                     flag;
    #define   LPP_CAPABILITIES_A_GNSS         0x01
    #define   LPP_CAPABILITIES_EPDU           0x02
    sit_lpp_A_GNSS_Capabilities               a_gnss;
}  __attribute__((packed)) sit_gps_ap_provide_capabilities_ind;

//     : Get LPP Request Assistance Data

typedef struct {
    RCM_IND_HEADER                              hdr;
    sit_lpp_Header                            lpp_hdr;
    UINT8                                     flag;
    #define   LPP_ASSISTANCE_CMN              0x01
    #define   LPP_ASSISTANCE_A_GNSS           0x02
    #define   LPP_ASSISTANCE_OTDOA            0x04
    #define   LPP_ASSISTANCE_EPDU             0x08
    sit_lpp_A_GNSS_Request_Assistance_Data    a_gnss;
} __attribute__((packed)) sit_gps_lpp_req_assist_data_ind;

//     : GPS AP Provide Location Information

typedef struct {
    RCM_IND_HEADER                                  hdr;
    sit_lpp_Header                                lpp_hdr;
    UINT8                                         flag;
    #define   LPP_LOC_INFO_CMN                    0x01
    #define   LPP_LOC_INFO_A_GNSS                 0x02
    #define   LPP_LOC_INFO_OTDOA                  0x04
    #define   LPP_LOC_INFO_ECID                   0x08
    #define   LPP_OC_INFO_EPDU                    0x10
    sit_lpp_CommonIEsProvideLocationInformation   cmn;
    sit_lpp_A_GNSS_ProvideLocationInformation     a_gnss;
} __attribute__((packed)) sit_gps_ap_provide_location_info_ind;

//     : GPS AP LPP Error

typedef struct {
    RCM_IND_HEADER                                            hdr;
    sit_lpp_Header                                          lpp_hdr;
    UINT8                                                   flag;
    #define LPP_ABORT                                       0x01
    #define LPP_ERROR                                       0x02
    UINT8                                                   statusCode;
    #define LPP_INVALIED_SESSION_ID                         0x01
    #define LPP_UNEXPECTED_MSG                              0x02
    #define LPP_NOT_ALL_REQ_MEASURE_POSSIBLE                0x03
    #define LPP_AUTH_NET_FAILURE                            0x04
    #define LPP_MEASURE_TIME_EXPIRED                        0x05
}__attribute__((packed)) sit_gps_ap_lpp_error_ind;

//     : GPS SUPL LPP Data Information

typedef struct {
    RCM_IND_HEADER                                    hdr;
    sit_lpp_Header                                  lpp_hdr;
    UINT8                                           flag;

    #define  LPP_SUPL_OTODA_REQ_CAP                 0x01
    #define  LPP_SUPL_OTDOA_PROV_ASSIST             0x02
    #define  LPP_SUPL_OTDOA_REQ_LOC                 0x04
    #define  LPP_SUPL_ECID_REQ_CAP                  0x08
    #define  LPP_SUPL_ECID_REQ_LOC                  0x10
    #define  LPP_SUPL_OTDOA_TRIGGER_REQ_ASSIST      0x20

    BYTE                                            prsInfo[512];
    UINT16                                          sizeOf;
    UINT8                                           responseTime;
}__attribute__((packed)) sit_gps_ap_supl_lpp_data_info_ind;

//     : GPS AP SUPL NI Message

typedef struct {
    RCM_IND_HEADER    hdr;
    UINT8           tid;
    UINT8           result;
} __attribute__((packed)) sit_gps_ap_supl_ni_message_ind;

/*------------------------From Network to AP-----------------------*/

//     : GPS Measure Position Message

typedef struct {
    RCM_IND_HEADER                          hdr;
    BYTE                                  method_type;
    sit_gps_m_accuracy_type               accuracy;
    UINT8                                 rsp_time;
    BYTE                                  use_multi_sets;
    BYTE                                  envronment_char;
    BYTE                                  cell_timing_wnt;
    BYTE                                  add_assist_req;
}__attribute__((packed))sit_gps_mea_position_ind;

//     : GPS Assist Data

typedef struct {
    RCM_IND_HEADER                          hdr;
    UINT32                                    flag;
    sit_gps_ref_time_type                     ref_time;
    sit_gps_ref_loc_type                      ref_loc;
    sit_gps_dgps_correction_type              dgps_corrections;
    sit_gps_navi_model_type                   navi_model;
    sit_gps_iono_model_type                   iono_model;
    sit_gps_utc_model_type                    utc_model;
    sit_gps_almanac_model_type                almanac;
    sit_gps_acq_assist_type                   acq_assist;
    sit_gps_r_time_int_type                   r_time_int;
}__attribute__((packed))sit_gps_assist_data_ind;

//     : Release GPS

//     : MT Location Request Message

typedef struct {
    RCM_IND_HEADER                             hdr;
    BYTE                          req_id;
    BYTE                                         notify_type;
    sit_gps_mtlr_loc_type                        loc;
    BYTE                                         client_id[82];
    sit_gps_dcs_string_type                      client_name;
    sit_gps_dcs_string_type                      requestor_id;
    sit_gps_code_word_type                       code_word;
    BYTE                                         svc_type_id;
}__attribute__((packed))sit_gps_mtlr_ind;

//     : Unsol GPS Reset Assist Data

//     : LPP Request Capabilities

typedef struct {
    RCM_IND_HEADER                              hdr;
    sit_lpp_Header                            lpp_hdr;
    BOOL                                   cmn;
    BOOL                                   a_gnss;
    BOOL                                   epdu;
}  __attribute__((packed)) sit_gps_lpp_request_capabilities_ind;

//     : GPS LPP Provide Assist Data

typedef struct {
    RCM_IND_HEADER                              hdr;
    sit_lpp_Header                            lpp_hdr;
    UINT8                                     flag;
    #define   LPP_ASSISTANCE_CMN              0x01
    #define   LPP_ASSISTANCE_A_GNSS           0x02
    #define   LPP_ASSISTANCE_OTDOA            0x04
    #define   LPP_ASSISTANCE_EPDU             0x08
    sit_lpp_A_GNSS_Provide_Assistance_Data    a_gnss;
} __attribute__((packed)) sit_gps_lpp_provide_assist_data_ind;

//     : LPP Request Location Information

typedef struct {
    RCM_IND_HEADER                                  hdr;
    sit_lpp_Header                                lpp_hdr;
    UINT8                                         flag;
    #define   LPP_LOC_INFO_CMN                    0x01
    #define   LPP_LOC_INFO_A_GNSS                 0x02
    #define   LPP_LOC_INFO_OTDOA                  0x04
    #define   LPP_LOC_INFO_ECID                   0x08
    #define   LPP_OC_INFO_EPDU                    0x10
    sit_lpp_CommonIEsRequestLocationInformation   cmn;
    sit_lpp_A_GNSS_RequestLocationInformation     a_gnss;
} __attribute__((packed)) sit_gps_lpp_request_location_info_ind;

//     : GPS LPP Error from Network

typedef struct {
    RCM_IND_HEADER                                                                hdr;
    sit_lpp_Header                                                              lpp_hdr;
    UINT8                                                                       flag;
    #define LPP_ABORT                                                           0x01
    #define LPP_ERROR                                                           0x02
    UINT8                                                                       statusCode;
    #define LPP_INVALIED_SESSION_ID                                             0x01
    #define LPP_UNEXPECTED_MSG                                                  0x02
    #define LPP_ASSIST_DATA_PARTLY_NO_SUPPORT_NO_AVAIL_SERVER                   0x03
    #define LPP_TIME_EXPIRED                                                    0x04
    #define LPP_METHOD_FAILURE                                                  0x05
    #define LPP_PARTIAL_REQ_MEASURE_POSSIBLE                                    0x06
    #define LPP_SERVER_DISCONNECTED                                             0x07
    #define LPP_UNDELIVERED_ASSIST_DATA_IS_NOT_SUPPORTED_BY_SERVER              0x08
    #define LPP_UNDELIVERED_ASSIST_DATA_IS_SUPPORTED_BUT_AVAILABLE_BY_SERVER    0x09
}__attribute__((packed)) sit_gps_network_lpp_error_ind;

//     : GPS SUPL LPP Data Information

typedef struct {
    RCM_IND_HEADER                                    hdr;
    sit_lpp_Header                                  lpp_hdr;
    UINT8                                           flag;

    #define  LPP_SUPL_OTDOA_PROV_CAP                0x01
    #define  LPP_SUPL_OTDOA_REQ_ASSIST              0x02
    #define  LPP_SUPL_OTDOA_PROV_LOC                0x04
    #define  LPP_SUPL_ECID_PROV_CAP                 0x08
    #define  LPP_SUPL_ECID_PROV_LOC                 0x10

    BYTE                                            lppInfo[512];
    UINT16                                          sizeOf;
} __attribute__((packed)) sit_gps_cp_supl_lpp_data_info_ind;

//     : GPS Network SUPL NI Message

typedef struct {
    RCM_IND_HEADER          hdr;
    UINT8               tid;
    UINT16               sizeOf;
    BYTE                suplMsg[256];
} __attribute__((packed)) sit_gps_network_supl_ni_msg_ind;

/*
SIT_SET_EMBMS_SESSION = 0x1001,
SIT_IND_EMBMS_COVERAGE = 0x1002,
SIT_GET_EMBMS_SESSION_LIST = 0x1003,
SIT_IND_EMBMS_SESSION_LIST = 0x1004,
SIT_GET_EMBMS_SIGNAL_STRENGTH = 0x1005,
SIT_IND_EMBMS_SIGNAL_STRENGTH = 0x1006,
SIT_GET_EMBMS_NETWORK_TIME = 0x1007,
SIT_IND_EMBMS_NETWORK_TIME = 0x1008,
SIT_IND_EMBMS_SAI_LIST = 0x1009,
SIT_IND_GLOBAL_CELL_ID = 0x100A,
*/

/*
    SIT_SET_EMBMS_SERVICE (RCM ID = 0x1000)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE service_state;
} sit_embms_set_service_req;

typedef null_data_format sit_embms_set_service_rsp;

/*
    SIT_SET_EMBMS_SESSION (RCM ID = 0x1001)
*/

#define MAX_SAI_LIST_LEN    (64)
#define MAX_FREQ_LIST_LEN    (8)

typedef struct
{
    RCM_HEADER hdr;
    BYTE session_state;
    BYTE TMGI[6];
    BYTE deActTMGI[6];
    UINT8 sai_list_len;
    UINT16 saiList[MAX_SAI_LIST_LEN];
    UINT8 freq_list_len;
    UINT32 freqList[MAX_FREQ_LIST_LEN];
} __attribute__((packed)) sit_embms_set_session_req;

/*
    SIT_IND_EMBMS_COVERAGE (RCM ID = 0x1002)
*/

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE coverage;
}sit_embms_coverage_rsp;

/*
    SIT_GET_EMBMS_SESSION_LIST (RCM ID = 0x1003)
*/

#define EMBMS_MAX_INTRA_SAILIST_NUMBER  (64)
#define EMBMS_TMGI_LEN (6)

typedef struct
{
    RCM_HEADER hdr;
    BYTE session_state;
} sit_embms_ssesion_list_req;

typedef struct
{
    RCM_HEADER hdr;
    BYTE session_state;
    BYTE oos_reason;
    BYTE number_record;
    BYTE tmgi[EMBMS_MAX_INTRA_SAILIST_NUMBER*EMBMS_TMGI_LEN];
} sit_embms_ssesion_list_rsp;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE session_state;
    BYTE oos_reason;
    BYTE number_record;
    BYTE tmgi[EMBMS_MAX_INTRA_SAILIST_NUMBER*EMBMS_TMGI_LEN];
} sit_embms_ssesion_list_ind;

/*
    SIT_GET_EMBMS_SIGNAL_STRENGTH (RCM ID = 0x1005)
*/

#define MAX_SIGNAL_RECORD_NUM   (8)
typedef null_data_format sit_embms_signal_strength_req;

typedef struct
{
    UINT32 SNR; // Signal Noise Radio [db]
    BYTE MBSFNAreaId;
    UINT32 ESNR;
    BYTE num_tmgi;
    BYTE tmgi[EMBMS_MAX_INTRA_SAILIST_NUMBER][EMBMS_TMGI_LEN];
} __attribute__((packed)) _signal_record_;

typedef struct
{
    RCM_HEADER hdr;
    BYTE number_record;
    _signal_record_ signal_record[MAX_SIGNAL_RECORD_NUM];
} __attribute__((packed)) sit_embms_signal_strength_rsp;

typedef struct
{
    RCM_HEADER hdr;
    BYTE number_record;
    _signal_record_ signal_record[MAX_SIGNAL_RECORD_NUM];
} __attribute__((packed)) sit_embms_signal_strength_ind;

/*
    SIT_GET_EMBMS_NETWORK_TIME (RCM ID = 0x1007)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE Sib16Acquired;
    BYTE TimeInfoType;
    BYTE daylight_valid;
    BYTE year;
    BYTE month;
    BYTE day;
    BYTE hour;
    BYTE minute;
    BYTE second;
    BYTE time_zone;
    BYTE daylight_adjust;
    BYTE day_of_week;
    BYTE PLMN[6]; // MCC(3), MNC(3)
    unsigned long long AbsoluteTime;
    BYTE LeapSecond;
} __attribute__((packed)) sit_embms_network_time_rsp;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE Sib16Acquired;
    BYTE TimeInfoType;
    BYTE daylight_valid;
    BYTE year;
    BYTE month;
    BYTE day;
    BYTE hour;
    BYTE minute;
    BYTE second;
    BYTE time_zone;
    BYTE daylight_adjust;
    BYTE day_of_week;
    BYTE PLMN[6]; // MCC(3), MNC(3)
    unsigned long long AbsoluteTime;
    BYTE LeapSecond;
} __attribute__((packed)) sit_embms_network_time_ind;

/*
    SIT_IND_EMBMS_SAI_LIST (RCM ID = 0x1009)
*/

#define MAX_INTER_SAI_NUMBER        (64)
#define MAX_MULTI_BAND_NUMBER       (8)
#define MAX_INTRA_SAILIST_NUMBER    (64)
#define MAX_INTER_SAILIST_NUMBER    (8)

typedef struct
{
    UINT32 Frequency;
    BYTE InterSaiNumber; // max 64
    BYTE MultiBandInfoNumber; // max 8
    WORD InterSaiInfo[MAX_INTER_SAI_NUMBER];
    BYTE MultiBandInfo[MAX_MULTI_BAND_NUMBER];
} __attribute__((packed)) sit_embms_inter_sailist;

typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE IntraSaiListNum; // max 64
    BYTE InterSaiListNum; // max 8
    WORD IntraSaiList[MAX_INTRA_SAILIST_NUMBER];
    sit_embms_inter_sailist InterSaiList[MAX_INTER_SAILIST_NUMBER];
} __attribute__((packed)) sit_embms_sailist;

/*
    SIT_IND_GLOBAL_CELL_ID (RCM ID = 0x100A)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE mcc[MAX_MCC_LEN];
    BYTE mnc[MAX_MNC_LEN];
    UINT32 cellId;
}sit_embms_global_cellid_rsp;

/*
    SIT_IND_STK_WIFI_STATUS_REQUEST (RCM ID = 0x0309)
*/
enum {
    WIFI_CONNECTED = 0x01,
    WIFI_DISCONNECTED = 0x02
};

typedef null_ind_data_format sit_stk_wifi_status_req_ind;

/*
    SIT_SEND_STK_WIFI_STATUS_REPORT (RCM ID = 0x030A)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE status;
} sit_stk_wifi_status_rsp_req;
typedef null_data_format sit_stk_wifi_status_rsp_rsp;

/*
    SIT_SEND_STK_TCP_STATUS_EVENT (RCM ID = 0x030B)
*/

typedef struct
{
    RCM_HEADER hdr;
    BYTE cid;
    BYTE status;
} sit_stk_wifi_channel_status_report_req;
typedef null_data_format sit_stk_wifi_channel_status_report_rsp;

/*
    SIT_IND_STK_OPEN_CHANNEL_REQUEST (RCM ID = 0x030C)
*/


typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE transport;
    UINT32 port;
    BYTE ip_type;
    BYTE ipv4[4];
    BYTE ipv6[16];
} sit_stk_wifi_open_channel_req_ind;

/*
    SIT_SEND_STK_WIFI_OPEN_CHANNEL_RSP (RCM ID = 0x030D)
*/

typedef struct
{
    RCM_HEADER hdr;
    BYTE cid;
    BYTE result;
} sit_stk_wifi_open_channel_rsp_req;
typedef null_data_format sit_stk_wifi_open_channel_rsp_rsp;

/*
    SIT_IND_STK_SEND_DATA_TO_WIFI (RCM ID = 0x030E)
*/
#define MAX_BIP_DATA        (1500)
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE cid;
    WORD length;
    BYTE data[MAX_BIP_DATA];
} sit_stk_wifi_send_data_req_ind;

/*
    SIT_STK_SEND_WIFI_DATA_RESPONSE (RCM ID = 0x0310)
*/
typedef sit_stk_wifi_open_channel_rsp_req sit_stk_wifi_send_data_rsp_req;
typedef null_data_format sit_stk_wifi_send_data_rsp_rsp;

/*
    SIT_IND_STK_RECEIVE_DATA_FROM_WIFI (RCM ID = 0x0311)
*/
typedef null_ind_data_format sit_stk_wifi_receive_data_req_ind;

/*
    SIT_STK_SEND_BIP_DATA_TO_SIM (RCM ID = 0x0312)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE cid;
    WORD length;
    BYTE data[MAX_BIP_DATA];
} sit_stk_receive_data_req;
typedef null_data_format sit_stk_receive_data_rsp;

/*
    SIT_IND_STK_CLOSE_CHANNEL_REQUEST (RCM ID = 0x0313)
*/
typedef null_ind_data_format sit_stk_wifi_close_channel_req_ind;

/*
    SIT_STK_SEND_CLOSE_CHANNEL_RESPONSE (RCM ID = 0x0314)
*/
typedef sit_stk_wifi_open_channel_rsp_req sit_stk_wifi_close_channel_rsp_req;
typedef null_data_format sit_stk_wifi_close_channel_rsp_rsp;

/*
    SIT_OEM_VERIFY_SIM_PIN_ENC (RCM ID = 0x4603)
*/
typedef struct
{
    RCM_HEADER hdr;
    BYTE encrypted_pin_len;
    BYTE encrypted_pin[MAX_ENCRYPTED_PIN_LEN];
    BYTE pin_len;
    BYTE aid_len;
    BYTE aid[MAX_SIM_AID_LEN];
} sit_sim_verify_sim_encrypted_pin_req;

/*
    SIT_IND_ENCRYPTIED_PASSWORD (RCM ID = 0x4604)
*/
typedef struct
{
    RCM_IND_HEADER hdr;
    BYTE pin_len;
    BYTE encrypted_pin_len;
    BYTE encrypted_pin[MAX_ENCRYPTED_PIN_LEN];
} sit_sim_encrypted_pin_ind;

typedef struct
{
    RCM_HEADER hdr;
    INT32 remain_count;
} sit_sim_verify_sim_encrypted_password_rsp;
#pragma pack()

#endif /*_SIT_DEF_H_*/
