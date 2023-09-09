/*
 *  SIT RIL network Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */
#include <android/log.h>
#include "sitrilTestSim.h"
#include "testUtil.h"
#include <librilutils/systemproperty.h>
#include <slsi/radio_v2_0.h>

using namespace std;

#define DBG true
#define LOG_TAG    "SIM_TEST"
#define LOGI(...)  if(DBG) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#define TEST_SIM_DEPERSONALIZATION 1
#define TEST_GET_SIM_STATUS 1
#define TEST_STK_MODULE 1
#define TEST_SIM_AUTH 1
#define TEST_RADIOCONFIG 1
#define TEST_VERIFY_PIN_PUK 1
#define TEST_SIM_LOCK 1
#define TEST_CHANGE_PIN 1
#define TEST_INDICATION_FOR_SIM 1
#define TEST_SECURE_ELEMENT 1
#define TEST_OEM_DEPERSONALIZATION 1
#define TEST_BIP_OVER_WIFI 1
#define TEST_MISC 1
#define TEST_UICC_PB 1

#define PROPERTY_NAME_LEN           (40)

namespace V2_0 = android::hardware::radio::impl::sim::V2_0;

////////////////////////////////////////////////////////////////////////////////////////
// hard coded variables

////////////////////////////////////////////////////////////////////////////////////////
// Build Ril Response
static UINT8* buildEnanbleModemRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetImsiForAppRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_id_get_imsi_rp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    if (rspData == nullptr) return nullptr;
    *datalen = len;

    const char imsi[] = {
        0x34, 0x35, 0x30, 0x30, 0x35, 0x30, 0x32, 0x31, 0x30, 0x33, 0x34, 0x31, 0x30, 0x36, 0x38
    };

    // need to make verifying data
    sit_id_get_imsi_rp *rsp = (sit_id_get_imsi_rp*)rspData;
    rsp->imsi_len = 0x0f;
    memcpy(rsp->imsi, imsi, 0x0f);

/*
    typedef struct
    {
        RCM_HEADER hdr;
        BYTE imsi_len;
        BYTE imsi[MAX_IMSI_LEN];
    }sit_id_get_imsi_rp;
 */

    return rspData;
}

static UINT8* buildGetModemStackStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pwr_get_stack_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_pwr_get_stack_status_rsp *rsp = (sit_pwr_get_stack_status_rsp*)rspData;
    rsp->mode = TEST_GET_STACK_ENABLED;

    return rspData;
}

static UINT8* buildGetSimStatusErrorRsp(INT16 sitId, UINT sitToken, UINT32 *datalen,
        tSimStatusParams *pParams) {
    UINT8 *rspData = nullptr;
    if (pParams == nullptr) return nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_status_rsp);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, RIL_E_RADIO_NOT_AVAILABLE);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetSimStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen,
        tSimStatusParams *pParams) {
    UINT8 *rspData = nullptr;
    if (pParams == nullptr) return nullptr;

    UINT16 simStatusLen = sizeof(sit_sim_get_sim_status_rsp);
    simStatusLen -= (sizeof(sit_sim_apps_status_info) * MAX_SIM_APPS_INFO_COUNT);
    simStatusLen += (sizeof(sit_sim_apps_status_info) * pParams->application_num);
    LOGI("simStatusLen : %d", simStatusLen);
    UINT16 len = simStatusLen + sizeof(sit_sim_get_sim_status_rsp_ext);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;
    LOGI("Total Length : %d, ext length : %lu", len,
            (unsigned long)sizeof(sit_sim_get_sim_status_rsp_ext));
    LOGI("application_num : %d", pParams->application_num);

    // need to make verifying data
    sit_sim_get_sim_status_rsp *rsp = (sit_sim_get_sim_status_rsp*)rspData;
    rsp->card_state = pParams->card_state;

    if (rsp->card_state == TEST_GET_SIM_CARD_STATE_PRESENT) {
        rsp->universal_pin_state = pParams->universal_pin_state;
        rsp->application_num = pParams->application_num;

        if (rsp->application_num > 0) {
            rsp->apps_status_info[0].apps_type = 2;
            rsp->apps_status_info[0].apps_state = pParams->apps_state;
            rsp->apps_status_info[0].perso_substate = pParams->perso_substate;
            rsp->apps_status_info[0].pin1_state = pParams->pin1_state;
            rsp->apps_status_info[0].aid_len = 16;
            memcpy(rsp->apps_status_info[0].AID, USIM_AIDData, 16);
        }

        if (rsp->application_num > 1) {
            rsp->apps_status_info[1].apps_type = 4;
            rsp->apps_status_info[1].apps_state = pParams->apps_state;
            rsp->apps_status_info[1].perso_substate = pParams->perso_substate;
            rsp->apps_status_info[1].pin1_state = pParams->pin1_state;
            rsp->apps_status_info[1].aid_len = 16;
            memcpy(rsp->apps_status_info[1].AID, CSIM_AIDData, 16);
        }

        if (rsp->application_num > 2) {
            rsp->apps_status_info[2].apps_type = 5;
            rsp->apps_status_info[2].apps_state = pParams->apps_state;
            rsp->apps_status_info[2].perso_substate = pParams->perso_substate;
            rsp->apps_status_info[2].pin1_state = pParams->pin1_state;
            rsp->apps_status_info[2].aid_len = 16;
            memcpy(rsp->apps_status_info[2].AID, ISIM_AIDData, 16);
        }
    }

    sit_sim_get_sim_status_rsp_ext *rsp_ext =
            (sit_sim_get_sim_status_rsp_ext*)(rspData + simStatusLen);
    rsp_ext->esim_no_profile = 1;
    rsp_ext->physical_slot_id = 0xff;
    rsp_ext->atr_length = sizeof(USIM_ATRData);
    memcpy(rsp_ext->atr, USIM_ATRData, rsp_ext->atr_length);
    rsp_ext->iccid_length = sizeof(USIM_ICCIDData);
    memcpy(rsp_ext->iccid, USIM_ICCIDData, rsp_ext->iccid_length);
    rsp_ext->eid_length = sizeof(ESIM_EIDData);
    memcpy(rsp_ext->eid, ESIM_EIDData, rsp_ext->eid_length);

    return rspData;
}

/*
static UINT8* buildGetSimStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    tSimStatusParams param;
    return buildGetSimStatusRsp(sitId, sitToken, datalen, &param);
}
*/

static UINT8* buildGetSlotStatusValidRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_slot_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_slot_status_rsp *rsp = (sit_sim_get_slot_status_rsp*)rspData;
    rsp->num_of_info = 2;

    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].atr_len = 0x14;
    memcpy(rsp->info[0].atr, USIM_ATRData, rsp->info[0].atr_len);

    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].atr_len = 0x17;
    memcpy(rsp->info[1].atr, ESIM_ATRData, rsp->info[1].atr_len);
    rsp->info[1].eid_len = 0x10;
    memcpy(rsp->info[1].eid, ESIM_EIDData, rsp->info[1].eid_len);

    return rspData;
}

static UINT8* buildGetSlotStatusValidLegacyRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_slot_status_rsp_v1_2);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_slot_status_rsp_v1_2 *rsp = (sit_sim_get_slot_status_rsp_v1_2*)rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].slot_State = TEST_GET_SLOT_STATE_ACTIVE;
    rsp->info[0].atr_len = 0x14;
    memcpy(rsp->info[0].atr, USIM_ATRData, rsp->info[0].atr_len);
    rsp->info[0].logicalSlotId = 0;
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].slot_State = TEST_GET_SLOT_STATE_ACTIVE;
    rsp->info[1].atr_len = 0x17;
    memcpy(rsp->info[1].atr, ESIM_ATRData, rsp->info[1].atr_len);
    rsp->info[1].logicalSlotId = 1;
    rsp->info[1].eid_len = 0x10;
    memcpy(rsp->info[1].eid, ESIM_EIDData, rsp->info[1].eid_len);


    return rspData;
}

static UINT8* buildGetSlotStatusInvalidRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_slot_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_slot_status_rsp *rsp = (sit_sim_get_slot_status_rsp*)rspData;
    rsp->num_of_info = 2;

    rsp->info[0].card_state = TEST_GET_SLOT_CARD_ABSENT;
    rsp->info[0].atr_len = 0;
    memset(rsp->info[0].atr, 0, 33);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_ABSENT;
    rsp->info[1].atr_len = 0;
    memset(rsp->info[1].atr, 0, 33);
    rsp->info[1].eid_len = 0;
    memset(rsp->info[1].eid, 0, 16);

    return rspData;
}

static UINT8* buildGetSlotStatusOnlyAtrIccIdEidNullRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_slot_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_slot_status_rsp *rsp = (sit_sim_get_slot_status_rsp*)rspData;
    rsp->num_of_info = 2;

    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].atr_len = 0;
    memset(rsp->info[0].atr, 0, 33);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].atr_len = 0;
    memset(rsp->info[1].atr, 0, 33);
    rsp->info[1].eid_len = 0;
    memset(rsp->info[1].eid, 0, 16);

    return rspData;
}

static UINT8* buildSlotStatusAidlRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_slot_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_slot_status_rsp *rsp = (sit_sim_get_slot_status_rsp*) rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].atr_len = 0x14;
    memcpy(rsp->info[0].atr, USIM_ATRData, rsp->info[0].atr_len);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);
    rsp->info[0].port_len = 1;
    rsp->info[0].port_info[0].iccid_len = 10;
    memcpy(rsp->info[0].port_info[0].iccid, ESIM_ICCIDData, rsp->info[0].port_info[0].iccid_len);
    rsp->info[0].port_info[0].logicalSlotId = 0;
    rsp->info[0].port_info[0].port_state = TEST_PORT_STATE_ACTIVE;

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].atr_len = 0x17;
    memcpy(rsp->info[1].atr, ESIM_ATRData, rsp->info[1].atr_len);
    rsp->info[1].eid_len = 0x10;
    memcpy(rsp->info[1].eid, ESIM_EIDData, rsp->info[1].eid_len);
    rsp->info[1].port_len = 1;
    rsp->info[1].port_info[0].iccid_len = 10;
    memcpy(rsp->info[1].port_info[0].iccid, ESIM_ICCIDData, rsp->info[1].port_info[0].iccid_len);
    rsp->info[1].port_info[0].logicalSlotId = 0;
    rsp->info[1].port_info[0].port_state = TEST_PORT_STATE_ACTIVE;

    return rspData;
}

static UINT8* buildIccIOForAppRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE error=0) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_sim_io_rsp);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    *datalen = len;

    const char response[] = {
        0x00, 0xff, 0xff, 0xf2
    };

    // need to make verifying data
    sit_sim_sim_io_rsp *rsp = (sit_sim_sim_io_rsp*)rspData;
    rsp->sw1 = 0x90;
    rsp->sw2 = 0x00;
    rsp->response_len = 0x04;
    memcpy(rsp->response, response, 0x04);
/*
typedef struct
{
    RCM_HEADER hdr;
    BYTE sw1;
    BYTE sw2;
    INT16 response_len;
    BYTE response[MAX_SIM_IO_DATA_LEN];
}sit_sim_sim_io_rsp;
*/
    return rspData;
}

/*
static UINT8* buildIccIOForOEMRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int error) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(null_data_format);
    if (error > 0) {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    } else {
        rspData = allocRspModemMem(len, sitId, sitToken);
    }
    *datalen = len;

    return rspData;
}
*/

static UINT8* buildIccCloseLogicalChannelRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_close_sim_channel_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildIccOpenLogicalChannelRsp(INT16 sitId, UINT sitToken, UINT32 *datalen,
        bool error=0) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_open_channel_rsp);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    *datalen = len;

    const char response[] = {
        0x6f, 0x0b, 0x84, 0x09, 0xa0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4c, 0x00
    };

    // need to make verifying data
    sit_sim_open_channel_rsp *rsp = (sit_sim_open_channel_rsp*)rspData;
    rsp->session_id = 0x02;
    if (error == 0) {
        rsp->sw1 = 0x90;
        rsp->sw2 = 0x00;
    } else {
        rsp->sw1 = 0x69;
        rsp->sw2 = 0x99;
    }
    rsp->response_len = 0x0d;
    memcpy(rsp->response, response, rsp->response_len);
    return rspData;
}

static UINT8* buildIccTransmitApduBasicChannelRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 headerLen = (UINT16)sizeof(RCM_HEADER);
    rspData = allocRspModemMem(headerLen, sitId, sitToken);
    if (rspData == nullptr) return nullptr;

    UINT8 rawLen = 57;
    const UINT8 raw[] = {
        0x37, 0x00, 0x62, 0x33, 0x82, 0x02, 0x78, 0x21, 0x83, 0x02, 0x3f, 0x00, // response
        0xa5, 0x0c, 0x80, 0x01, 0x61, 0x87, 0x01, 0x01, 0x83, 0x04, 0x00, 0x07,
        0x16, 0xe8, 0x8a, 0x01, 0x05, 0x8b, 0x06, 0x2f, 0x06, 0x01, 0x02, 0x00,
        0x02, 0xc6, 0x0c, 0x90, 0x01, 0x60, 0x83, 0x01, 0x01, 0x83, 0x01, 0x0a,
        0x83, 0x01, 0x0d, 0x81, 0x02, 0xff, 0xff, 0x90, 0x00
    };

    *datalen = (headerLen + rawLen);

    UINT8 *ApduBasicChannelRsp = new UINT8[*datalen];
    memset(ApduBasicChannelRsp, 0x00, *datalen);
    memcpy(ApduBasicChannelRsp, rspData, headerLen);
    memcpy(ApduBasicChannelRsp+headerLen, raw, rawLen);
    free(rspData);

    return ApduBasicChannelRsp;
}

static UINT8* buildIccTransmitApduLogicalChannelRsp(INT16 sitId, UINT sitToken, UINT32 *datalen,
        bool success) {
    UINT8 *rspData = nullptr;

    UINT16 headerLen = (UINT16)sizeof(RCM_HEADER);
    rspData = allocRspModemMem(headerLen, sitId, sitToken);
    if (rspData == nullptr) return nullptr;

    // because response[0];
    int rawLen = 11;
    const UINT8 raw_success[] = {
        0x90, 0x00, 0x05, 0x00, 0xbf, 0x2d, 0x02, 0xa0, 0x00, 0x00, 0x00        // response data
    };

    const UINT8 raw_failed[] = {
        0x6c, 0x00, 0x05, 0x00, 0xbf, 0x2d, 0x02, 0xa0, 0x00, 0x00, 0x00        // response data
    };

    *datalen = (headerLen + rawLen);

    UINT8 *ApduLogicalChannelRsp = new UINT8[*datalen];
    memset(ApduLogicalChannelRsp, 0x00, *datalen);
    memcpy(ApduLogicalChannelRsp, rspData, headerLen);
    if (success) {
        memcpy(ApduLogicalChannelRsp+headerLen, raw_success, rawLen);
    } else {
        memcpy(ApduLogicalChannelRsp+headerLen, raw_failed, rawLen);
    }
    free(rspData);

    return ApduLogicalChannelRsp;

/*
    typedef struct
    {
        RCM_HEADER hdr;
        BYTE sw1;
        BYTE sw2;
        INT16 response_len;
        BYTE response[0];
    }sit_sim_transmit_sim_apdu_channel_rsp;
*/
}

static UINT8* BuildSetLogicalSlotMappingResponse(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* BuildSetLogicalSlotMappingErrorResponse(INT16 sitId, UINT sitToken, UINT32 *datalen,
        int error = 0) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(null_data_format);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    *datalen = len;
    return rspData;
}


static UINT8* buildSetSimCardPowerRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildStkSendEnvelopeCommandRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_stk_send_stk_envelope_cmd_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_stk_send_stk_envelope_cmd_rsp *rsp = (sit_stk_send_stk_envelope_cmd_rsp*)rspData;
    rsp->envelope_rsp_len = 240;
    memset(rsp->envelope_rsp, 0, rsp->envelope_rsp_len);
    return rspData;
}

static UINT8* buildStkSendEnvelopeStatusRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_stk_send_stk_envelope_with_status_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_stk_send_stk_envelope_with_status_rsp *rsp =
            (sit_stk_send_stk_envelope_with_status_rsp*)rspData;
    rsp->sw1 = 90;
    rsp->sw2 = 00;
    rsp->envelope_rsp_len = 240;
    memset(rsp->envelope_rsp, 0, rsp->envelope_rsp_len);
    return rspData;
}

/*
static UINT8* buildStkSendTerminalRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_stk_send_stk_terminal_rsp_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_stk_send_stk_terminal_rsp_rsp *rsp = (sit_stk_send_stk_terminal_rsp_rsp*)rspData;
    rsp->sw1 = 0;
    rsp->sw2 = 1;
    return rspData;
}
*/

static UINT8* buildStkProactiveCmdInd(INT16 sitId, UINT32 *datalen, int ProactiveCmdtype) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_stk_stk_proactive_cmd_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_stk_stk_proactive_cmd_ind *rsp = (sit_stk_stk_proactive_cmd_ind *)rspData;
    switch (ProactiveCmdtype) {
        case DISPLAY_TEXT_NORMAL: {
            rsp->proactive_cmd_len = 0x1c;
            const UINT8 raw[] = {
                0xD0, 0x1A, 0x81, 0x03, 0x01, 0x21, 0x80, 0x82, 0x02, 0x81, 0x02, 0x8D,
                0x0F, 0x04, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x54, 0x65,
                0x73, 0x74, 0x20, 0x31
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case GET_INKEY_NORNAL: {
            rsp->proactive_cmd_len = 0x17;
            const UINT8 raw[] = {
                0xD0, 0x15, 0x81, 0x03, 0x01, 0x22, 0x00, 0x82, 0x02, 0x81, 0x82, 0x8D,
                0x0A, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x22, 0x2B, 0x22
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case GET_INPUT_NORMAL: {
            rsp->proactive_cmd_len = 0x1d;
            const UINT8 raw[] = {
                0xD0, 0x1B, 0x81, 0x03, 0x01, 0x23, 0x00, 0x82, 0x02, 0x81, 0x82, 0x8D,
                0x0C, 0x04, 0x45, 0x6E, 0x74, 0x65, 0x72, 0x20, 0x31, 0x32, 0x33, 0x34,
                0x35, 0x91, 0x02, 0x05, 0x05
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case MORE_TIME: {
            rsp->proactive_cmd_len = 0xb;
            const UINT8 raw[] = {
                0xD0, 0x09, 0x81, 0x03, 0x01, 0x02, 0x00, 0x82, 0x02, 0x81, 0x82
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case PLAY_TONE_NORMAL: {
            rsp->proactive_cmd_len = 0x1d;
            const UINT8 raw[] = {
                0xD0, 0x1B, 0x81, 0x03, 0x01, 0x20, 0x00, 0x82, 0x02, 0x81, 0x03, 0x85,
                0x09, 0x44, 0x69, 0x61, 0x6C, 0x20, 0x54, 0x6F, 0x6E, 0x65, 0x8E, 0x01,
                0x01, 0x84, 0x02, 0x01, 0x05
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case POLL_INTERVAL: {
            rsp->proactive_cmd_len = 0xf;
            const UINT8 raw[] = {
                0xD0, 0x0D, 0x81, 0x03, 0x01, 0x03, 0x00, 0x82, 0x02, 0x81, 0x82, 0x84,
                0x02, 0x01, 0x14
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case REFRESH_NORMAL: {
            rsp->proactive_cmd_len = 0x12;
            const UINT8 raw[] = {
                0xD0, 0x10, 0x81, 0x03, 0x01, 0x01, 0x01, 0x82, 0x02, 0x81, 0x82, 0x92,
                0x05, 0x01, 0x3F, 0x00, 0x2F, 0xE2
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SET_UP_MENU_NORMAL: {
            rsp->proactive_cmd_len = 0x3d;
            const UINT8 raw[] = {
                0xD0, 0x3B, 0x81, 0x03, 0x01, 0x25, 0x00, 0x82, 0x02, 0x81, 0x82, 0x85,
                0x0C, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x4D, 0x65, 0x6E,
                0x75, 0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31, 0x8F, 0x07,
                0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, 0x8F, 0x07, 0x03, 0x49, 0x74,
                0x65, 0x6D, 0x20, 0x33, 0x8F, 0x07, 0x04, 0x49, 0x74, 0x65, 0x6D, 0x20,
                0x34
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SELECT_ITEM: {
            rsp->proactive_cmd_len = 0x3f;
            const UINT8 raw[] = {
                0xD0, 0x3D, 0x81, 0x03, 0x01, 0x24, 0x00, 0x82, 0x02, 0x81, 0x82, 0x85,
                0x0E, 0x54, 0x6F, 0x6F, 0x6C, 0x6B, 0x69, 0x74, 0x20, 0x53, 0x65, 0x6C,
                0x65, 0x63, 0x74, 0x8F, 0x07, 0x01, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x31,
                0x8F, 0x07, 0x02, 0x49, 0x74, 0x65, 0x6D, 0x20, 0x32, 0x8F, 0x07, 0x03,
                0x49, 0x74, 0x65, 0x6D, 0x20, 0x33, 0x8F, 0x07, 0x04, 0x49, 0x74, 0x65,
                0x6D, 0x20, 0x34
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case PROVIDE_LOCAL_INFORMATION: {
            rsp->proactive_cmd_len = 0xb;
            const UINT8 raw[] = {
                0xD0, 0x09, 0x81, 0x03, 0x01, 0x26, 0x01, 0x82, 0x02, 0x81, 0x82
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SET_UP_EVENT_LIST_NORMAL: {
            rsp->proactive_cmd_len = 0xe;
            const UINT8 raw[] = {
                0xD0, 0x0C, 0x81, 0x03, 0x01, 0x05, 0x00, 0x82, 0x02, 0x81, 0x82, 0x99,
                0x01, 0x04
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case PERFORM_CARD_APDU: {
            rsp->proactive_cmd_len = 0xb;
            const UINT8 raw[] = {
                0xD0, 0x09, 0x81, 0x03, 0x01, 0x31, 0x00, 0x82, 0x02, 0x81, 0x11
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case POWER_OFF_CARD: {
            rsp->proactive_cmd_len = 0xb;
            const UINT8 raw[] = {
                0xD0, 0x09, 0x81, 0x03, 0x01, 0x32, 0x00, 0x82, 0x02, 0x81, 0x11
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case POWER_ON_CARD: {
            rsp->proactive_cmd_len = 0xb;
            const UINT8 raw[] = {
               0xD0, 0x09, 0x81, 0x03, 0x01, 0x31, 0x00, 0x82, 0x02, 0x81, 0x11
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case TIMER_MANAGEMENT_NORMAL: {
            rsp->proactive_cmd_len = 0x13;
            const UINT8 raw[] = {
                0xD0, 0x11, 0x81, 0x03, 0x01, 0x27, 0x00, 0x82, 0x02, 0x81, 0x82, 0xA4,
                0x01, 0x01, 0xA5, 0x03, 0x00, 0x50, 0x00
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SET_UP_IDLE_MODE_TEXT: {
            rsp->proactive_cmd_len = 0x1c;
            const UINT8 raw[] = {
                0xD0, 0x1A, 0x81, 0x03, 0x01, 0x28, 0x00, 0x82, 0x02, 0x81, 0x82, 0x8D,
                0x0F, 0x04, 0x49, 0x64, 0x6C, 0x65, 0x20, 0x4D, 0x6F, 0x64, 0x65, 0x20,
                0x54, 0x65, 0x78, 0x74
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case RUN_AT_COMMAND: {
            rsp->proactive_cmd_len = 0x14;
            const UINT8 raw[] = {
                0xD0, 0x12, 0x81, 0x03, 0x01, 0x34, 0x00, 0x82, 0x02, 0x81, 0x82, 0xA8,
                0x07, 0x41, 0x54, 0x2B, 0x43, 0x47, 0x4D, 0x49
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case LANGUAGE_NOTIFICATION: {
            rsp->proactive_cmd_len = 0xf;
            const UINT8 raw[] = {
                0xD0, 0x0D, 0x81, 0x03, 0x01, 0x35, 0x01, 0x82, 0x02, 0x81, 0x82, 0xAD,
                0x02, 0x73, 0x65
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case LAUNCH_BROWSER: {
            rsp->proactive_cmd_len = 0x16;
            const UINT8 raw[] = {
                0xD0, 0x14, 0x81, 0x03, 0x01, 0x40, 0x00, 0x82, 0x02, 0x81, 0x82, 0x05,
                0x00, 0x39, 0x02, 0x05, 0x78, 0x3C, 0x03, 0x03, 0x0D, 0xBC
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case GET_CHANNEL_STATUS: {
            rsp->proactive_cmd_len = 0x16;
            const UINT8 raw[] = {
                0xD0, 0x14, 0x81, 0x03, 0x01, 0x40, 0x00, 0x82, 0x02, 0x81, 0x82, 0x05,
                0x00, 0x39, 0x02, 0x05, 0x78, 0x3C, 0x03, 0x03, 0x0D, 0xBC
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case ACTIVATE: {
            rsp->proactive_cmd_len = 0xe;
            const UINT8 raw[] = {
                0xD0, 0x0C, 0x81, 0x03, 0x01, 0x70, 0x00, 0x82, 0x02, 0x81, 0x82, 0xFB,
                0x01, 0x01
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case CONTACTLESS_STATE_CHANGED: {
            rsp->proactive_cmd_len = 0xe;
            const UINT8 raw[] = {
                0xD0, 0x0C, 0x81, 0x03, 0x01, 0x71, 0x00, 0x82, 0x02, 0x81, 0x82, 0xD4,
                0x01, 0x00
            };
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SIM_REFRESH_FILE_UPDATE: {
            const UINT8 raw[] = {
                0xD0, 0x24, 0x81, 0x03, 0x01, 0x01, 0x01, 0x82, 0x02, 0x81, 0x82,
                0x92, 0x07, 0x01, 0x3F, 0x00, 0x7F, 0xFF, 0x6F, 0x7B, 0x2F, 0x10,
                0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x02, 0x12, 0x34, 0x56, 0x78,
                0x90, 0x12, 0x34, 0x56, 0x78
            };
            rsp->proactive_cmd_len = sizeof(raw);
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SIM_REFRESH_INIT: {
            const UINT8 raw[] = {
                0xD0, 0x09, 0x81, 0x03, 0x01, 0x01, 0x00, 0x82, 0x02, 0x81, 0x82
            };
            rsp->proactive_cmd_len = sizeof(raw);
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
        case SIM_REFRESH_RESET: {
            const UINT8 raw[] = {
                0xD0, 0x09, 0x81, 0x03, 0x01, 0x01, 0x04, 0x82, 0x02, 0x81, 0x82,
            };
            rsp->proactive_cmd_len = sizeof(raw);
            memcpy(rsp->proactive_cmd, raw, rsp->proactive_cmd_len);
            break;
        }
    }
    return rspData;
}

static UINT8* buildStkSimRefreshInd(INT16 sitId, UINT32 *datalen, int result) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_stk_sim_refresh_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_stk_sim_refresh_ind *rsp = (sit_stk_sim_refresh_ind *)rspData;
    rsp->result = result;

    // need to make data
    return rspData;
}

static UINT8* buildStkSessionEndInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;
   // need to make data
   return rspData;
}

/*
static UINT8* BuildStkHandleCallSetupReqFromSimRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int remainCnt) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(sit_stk_setup_call_user_operation_e_type);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_stk_setup_call_user_operation_e_type *rspPin = (sit_stk_setup_call_user_operation_e_type *)rspData;
    return rspData;
}
*/

static UINT8* buildSsReturnResult(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_ss_return_result_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;
    sit_ss_return_result_ind *rsp = (sit_ss_return_result_ind *)rspData;

    rsp->return_result_len = 4;
    const UINT8 raw[] = {
        0xd0, 0x35, 0x81, 0x03
    };
    memcpy(rsp->return_result, raw, rsp->return_result_len);
    return rspData;
}

static UINT8* buildStkCcAlphaNtf(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_stk_cc_alpha_notify_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;
    sit_stk_cc_alpha_notify_ind *rsp = (sit_stk_cc_alpha_notify_ind *)rspData;

    rsp->alpha_len = 4;
    const UINT8 raw[] = {
        0xd0, 0x35, 0x81, 0x03
    };
    memcpy(rsp->alpha_buf, raw, rsp->alpha_len);
    return rspData;
}

#if TEST_SIM_AUTH
static UINT8* buildIsimAuthenticationRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int error) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_auth_rsp);

    if (error > 0) {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
        *datalen = len;
        return rspData;
    }

    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;
    // Refer  testEapAkaAuthentication() in cts/CarrierApiTest.java and TS 123 108
    //   Format: [DB][Length][RES][Length][CK][Length][IK]
    /*   String EXPECTED_EAP_AKA_RESULT =
            "DB10111013121514171619181B1A1D1C1F1E"
                    + "101013121514171619181B1A1D1C1F1E11"
                    + "1013121514171619181B1A1D1C1F1E1110"; */

    // need to make verifying data
    const char auth_rsp[] = {
        0xDB, 0x10, 0x11, 0x10, 0x13, 0x12, 0x15, 0x14, 0x17, 0x16,
        0x19, 0x18, 0x1B, 0x1A, 0x1D, 0x1C, 0x1F, 0x1E, 0x10, 0x10,
        0x13, 0x12, 0x15, 0x14, 0x17, 0x16, 0x19, 0x18, 0x1B, 0x1A,
        0x1D, 0x1C, 0x1F, 0x1E, 0x11, 0x10, 0x13, 0x12, 0x15, 0x14,
        0x17, 0x16, 0x19, 0x18, 0x1B, 0x1A, 0x1D, 0x1C, 0x1F, 0x1E,
        0x11, 0x10
    };

    sit_sim_get_sim_auth_rsp *rsp = (sit_sim_get_sim_auth_rsp*)rspData;
    rsp->auth_type = 0; //IMS auth
    rsp->auth_len = sizeof(auth_rsp);
    memcpy(rsp->auth, auth_rsp, rsp->auth_len);
    return rspData;
}


static UINT8* buildSimAuthenticationRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, int error) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_auth_rsp);

    if (error > 0) {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
        *datalen = len;
        return rspData;
    }

    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // Refer  testEapSimAuthentication() in cts/CarrierApiTest.java and TS 123 108
    //     Format: [Length][SRES][Length][Kc]
    /* String EXPECTED_EAP_SIM_RESULT = "0400000000080000000000000000"; */


    // need to make verifying data
    const char auth_rsp[] = {
        0x04, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    sit_sim_get_sim_auth_rsp *rsp = (sit_sim_get_sim_auth_rsp*)rspData;
    rsp->auth_type = 1; //GSM auth
    rsp->auth_len = sizeof(auth_rsp);
    memcpy(rsp->auth, auth_rsp, rsp->auth_len);
    return rspData;
}
#endif

static UINT8* BuildSimGetFacilityLockRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, bool success) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_auth_rsp);
    if (success) {
        rspData = allocRspModemMem(len, sitId, sitToken);
        *datalen = len;
    } else {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, RIL_E_GENERIC_FAILURE);
        *datalen = len;
    }

    // need to make verifying data
    sit_sim_get_facility_lock_rsp *rsp = (sit_sim_get_facility_lock_rsp*)rspData;
    rsp->service_class = 1;
    return rspData;
}

static UINT8* BuildSimGetSimLockInfoRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_get_sim_lock_info_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // need to make verifying data
    sit_sim_get_sim_lock_info_rsp *rsp = (sit_sim_get_sim_lock_info_rsp*)rspData;
    rsp->policy = 0;
    rsp->status = 0; // UNLOCK
    rsp->lockType = 0;
    rsp->maxRetryCount = 0;
    rsp->reaminCount = 0;
    rsp->lockCodeCount = 0;
    memcpy(rsp->lockCode, TEST_SIMLOCK_RSP, 4);
    return rspData;
}

static UINT8* BuildSimDepersonalizationResponse(INT16 sitId, UINT sitToken, UINT32 *datalen,
        int/*  persoType */, int remainCnt) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(sit_sim_verify_network_lock_rsp) + sizeof(UINT16);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sim_verify_network_lock_rsp *rsp = (sit_sim_verify_network_lock_rsp *)rspData;
    rsp->remain_count = remainCnt;

    return rspData;
}

static UINT8* BuildVerifyNetworkLockResponse(INT16 sitId, UINT sitToken, UINT32 *datalen,
        int /*persoType*/, int remainCnt) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(sit_sim_verify_network_lock_rsp) + sizeof(UINT16);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sim_verify_network_lock_rsp *rsp = (sit_sim_verify_network_lock_rsp *)rspData;
    rsp->remain_count = remainCnt;

    return rspData;
}


static UINT8* BuildSimPinPukResponse(INT16 sitId, UINT sitToken, UINT32 *datalen, int remainCnt,
        int error = 0) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(sit_sim_verify_sim_pin_rsp);
    rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
    *datalen = len;

    sit_sim_verify_sim_pin_rsp *rspPin = (sit_sim_verify_sim_pin_rsp *)rspData;
    rspPin->remain_count = remainCnt;

    return rspData;
}

static UINT8* buildGetAtrRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, BYTE result, int error) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16) sizeof(sit_id_get_atr_rsp);

    if (error > 0) {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, error);
        *datalen = len;
        return rspData;
    }

    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_id_get_atr_rsp *rsp = (sit_id_get_atr_rsp *)rspData;
    rsp->result = result;
    if (rsp->result == true) {
        rsp->atr_len = 0x14;
        memcpy(rsp->atr, USIM_ATRData, rsp->atr_len);
    }

    return rspData;
}

static UINT8* buildSetCarrierRestrictionRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildGetCarrierRestrictionRsp(INT16 sitId, UINT sitToken, UINT32 *datalen,
        bool success) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)(sizeof(sit_sim_set_carrier_restriction_req) +
                    sizeof(SIT_CARRIER_INFO) * 2 + sizeof(uint8_t) * 2);

    if (success) {
        rspData = allocRspModemMem(len, sitId, sitToken);
        *datalen = len;

        const char mcc[] = {0x34, 0x35, 0x30};
        const char mnc[] = {0x30, 0x38, 0x38};
        const char match_data[32] = {};

        sit_sim_get_carrier_restriction_rsp *rsp = (sit_sim_get_carrier_restriction_rsp *)rspData;
        rsp->allowed_carriers_len = 1;

        memcpy(rsp->carrier_list[0].mcc, mcc, 3);
        memcpy(rsp->carrier_list[0].mnc, mnc, 3);
        rsp->carrier_list[0].match_type = 0;
        rsp->carrier_list[0].match_len = 0;
        memcpy(rsp->carrier_list[0].match_data, match_data, 32);

        rsp->excluded_carriers_len = 1;
        memcpy(rsp->carrier_list[1].mcc, mcc, 3);
        memcpy(rsp->carrier_list[1].mnc, mnc, 3);
        rsp->carrier_list[1].match_type = 0;
        rsp->carrier_list[1].match_len = 0;
        memcpy(rsp->carrier_list[1].match_data, match_data, 32);

        BYTE *pData = (BYTE *) rspData;
        pData[len - 2] = 1;
        pData[len - 1] = 0;
    } else {
        rspData = allocErrorRspModemMem(len, sitId, sitToken, RIL_E_REQUEST_NOT_SUPPORTED);
        *datalen = len;
    }

    return rspData;
}

static UINT8* buildSimTrayStatusChangedInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_sim_tray_status_changed_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sim_tray_status_changed_ind *rsp = (sit_sim_tray_status_changed_ind *)rspData;
    rsp->state = 1;

    return rspData;
}

static UINT8* buildSimPbStorageInfo(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_pb_storage_info_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_sim_pb_storage_info_rsp *rsp = (sit_sim_pb_storage_info_rsp *)rspData;
    rsp->pb_type = 0x09;
    rsp->total_count = 0x01f4; // 500
    rsp->used_count = 1;

    return rspData;
}

static UINT8* buildSimPb3GPbCapa(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_pb_capa_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    BYTE response[5][7] = {
        {0x01, 0xf4, 0x01, 0x10, 0x00, 0x0a, 0x00},
        {0x02, 0xf4, 0x01, 0x28, 0x00, 0x0a, 0x00},
        {0x03, 0xf4, 0x01, 0x28, 0x00, 0x0a, 0x00},
        {0x04, 0x64, 0x00, 0x26, 0x00, 0x00, 0x00},
        {0x06, 0xf4, 0x01, 0x04, 0x00, 0x0a, 0x00}
    };

    sit_sim_pb_capa_rsp *rsp = (sit_sim_pb_capa_rsp *)rspData;
    rsp->entry_num = 0x05;
    for (int i = 0; i < rsp->entry_num; i++) {
        memcpy(&(rsp->pb_list[i]), response[i], 7);
    }

    return rspData;
}
/*
static UINT8* buildReadSimPbRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_read_pb_resp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    const char response[] = {
        0x01, 0x05, 0x00, 0x02, 0x54, 0x65, 0x73, 0x74,//name
        0x02, 0x09, 0x00, 0x81, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,//number
        0x04, 0x09, 0x00, 0x02, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x63, 0x6f, 0x6d,//email
        0x03, 0x07, 0x00, 0x81, 0x00, 0x32, 0x32, 0x32, 0x32, 0x32, 0xff//anr
    };

    sit_read_pb_resp *rsp = (sit_read_pb_resp *) rspData;
    rsp->pb_type = 0x09;
    rsp->recordId = 1;
    rsp->data_len = 0x2b;
    memcpy(rsp->entry_data, response, rsp->data_len);

    return rspData;
}
*/
static UINT8* buildUpdateSimPbRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_update_pb_entry_resp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_update_pb_entry_resp *rsp = (sit_update_pb_entry_resp *)rspData;
    rsp->mode = 1;
    rsp->pb_type = 0x09;
    rsp->recordId = 1;

    return rspData;
}

////////////////////////////////////////////////////////////////////////////////////////
// Build Indication
static UINT8* buildGetSlotStatusValidLegacyInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_slot_status_changed_ind_v1_2);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    // need to make verifying data
    sit_sim_slot_status_changed_ind_v1_2 *rsp = (sit_sim_slot_status_changed_ind_v1_2*)rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].slot_State = TEST_GET_SLOT_STATE_ACTIVE;
    rsp->info[0].atr_len = 0x14;
    memcpy(rsp->info[0].atr, USIM_ATRData, rsp->info[0].atr_len);
    rsp->info[0].logicalSlotId = 0;
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].slot_State = TEST_GET_SLOT_STATE_ACTIVE;
    rsp->info[1].atr_len = 0x17;
    memcpy(rsp->info[1].atr, ESIM_ATRData, rsp->info[1].atr_len);
    rsp->info[1].logicalSlotId = 1;
    rsp->info[1].eid_len = 0x10;
    memcpy(rsp->info[1].eid, ESIM_EIDData, rsp->info[1].eid_len);

    return rspData;
}


static UINT8* buildGetSlotStatusValidInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_slot_status_changed_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    // need to make verifying data
    sit_sim_slot_status_changed_ind *rsp = (sit_sim_slot_status_changed_ind*)rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].atr_len = 0x14;
    memcpy(rsp->info[0].atr, USIM_ATRData, rsp->info[0].atr_len);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].atr_len = 0x17;
    memcpy(rsp->info[1].atr, ESIM_ATRData, rsp->info[1].atr_len);
    rsp->info[1].eid_len = 0x10;
    memcpy(rsp->info[1].eid, ESIM_EIDData, rsp->info[1].eid_len);

    return rspData;
}

static UINT8* buildGetSlotStatusInvalidInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_slot_status_changed_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    // need to make verifying data
    sit_sim_slot_status_changed_ind *rsp = (sit_sim_slot_status_changed_ind*)rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_ABSENT;
    rsp->info[0].atr_len = 0;
    memset(rsp->info[0].atr, 0, 33);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_ABSENT;
    rsp->info[1].atr_len = 0;
    memset(rsp->info[1].atr, 0, 33);
    rsp->info[1].eid_len = 0;
    memset(rsp->info[1].eid, 0, 16);

    return rspData;
}

static UINT8* buildGetSlotStatusOnlyAtrIccIdEidNullInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_slot_status_changed_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    // need to make verifying data
    sit_sim_slot_status_changed_ind *rsp = (sit_sim_slot_status_changed_ind*)rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].atr_len = 0;
    memset(rsp->info[0].atr, 0, 33);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].atr_len = 0;
    memset(rsp->info[1].atr, 0, 33);
    rsp->info[1].eid_len = 0;
    memset(rsp->info[1].eid, 0, 16);

    return rspData;
}

static UINT8* buildSlotStatusAidlInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_slot_status_changed_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    // need to make verifying data
    sit_sim_slot_status_changed_ind *rsp = (sit_sim_slot_status_changed_ind*)rspData;
    rsp->num_of_info = 2;

    // USIM
    rsp->info[0].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[0].atr_len = 0x14;
    memcpy(rsp->info[0].atr, USIM_ATRData, rsp->info[0].atr_len);
    rsp->info[0].eid_len = 0;
    memset(rsp->info[0].eid, 0, 16);
    rsp->info[0].port_len = 1;
    rsp->info[0].port_info[0].iccid_len = 10;
    memcpy(rsp->info[0].port_info[0].iccid, ESIM_ICCIDData, rsp->info[0].port_info[0].iccid_len);
    rsp->info[0].port_info[0].logicalSlotId = 0;
    rsp->info[0].port_info[0].port_state = TEST_PORT_STATE_ACTIVE;

    rsp->info[1].card_state = TEST_GET_SLOT_CARD_PRESENT;
    rsp->info[1].atr_len = 0x17;
    memcpy(rsp->info[1].atr, ESIM_ATRData, rsp->info[1].atr_len);
    rsp->info[1].eid_len = 0x10;
    memcpy(rsp->info[1].eid, ESIM_EIDData, rsp->info[1].eid_len);
    rsp->info[1].port_len = 1;
    rsp->info[1].port_info[0].iccid_len = 10;
    memcpy(rsp->info[1].port_info[0].iccid, ESIM_ICCIDData, rsp->info[1].port_info[0].iccid_len);
    rsp->info[1].port_info[0].logicalSlotId = 0;
    rsp->info[1].port_info[0].port_state = TEST_PORT_STATE_ACTIVE;

    return rspData;
}

static UINT8* buildSimStatusChangedInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_sim_sim_status_changed_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    return rspData;
}

static UINT8* buildUiccSubsStateChangedInd(INT16 sitId, UINT32 *datalen, int state) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_sim_uicc_sub_state_changed_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sim_uicc_sub_state_changed_ind *rsp = (sit_sim_uicc_sub_state_changed_ind *)rspData;
    rsp->state = state;

    return rspData;
}

static UINT8* buildNullDataFormatInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *indData = nullptr;
    UINT16 len = (UINT16) sizeof(null_ind_data_format);

    indData = allocIndModemMem(len, sitId);
    *datalen = len;

    return indData;
}

static UINT8* buildNullDataFormatRsp(INT16 sitId, UINT sitToken, UINT32 *datalen, UINT32 rilError=0) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);

    RCM_HEADER *header = (RCM_HEADER *) rspData;
    header->ext.rsp.error = rilError;

    *datalen = len;

    return rspData;
}

static UINT8* buildBowOpenChannelReqInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_stk_wifi_open_channel_req_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    const UINT8 ipv4[4] = {0x01, 0x02, 0x03, 0x04};
    const UINT8 ipv6[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
        0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };

    sit_stk_wifi_open_channel_req_ind *rsp = (sit_stk_wifi_open_channel_req_ind*)rspData;

    rsp->transport = BIP_TRANSPORT_TCP;
    rsp->port = 8080;
    rsp->ip_type = BIP_IPV4V6;
    memcpy(rsp->ipv4, ipv4, 4);
    memcpy(rsp->ipv6, ipv6, 16);
    return rspData;
}

static UINT8* buildBowSendDataReqInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_stk_wifi_send_data_req_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    const UINT8 data[16] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
        0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
    };

    sit_stk_wifi_send_data_req_ind *rsp = (sit_stk_wifi_send_data_req_ind*)rspData;

    //rsp->cid = 0x01;
    rsp->length = 16;
    memcpy(rsp->data, data, 16);
    return rspData;
}

static UINT8* buildSecureElementOpenLogicalChannelRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_sim_open_channel_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    const char response[] = {
        0x6f, 0x0b, 0x84, 0x09, 0xa0, 0x00, 0x00, 0x01, 0x51, 0x41, 0x43, 0x4c, 0x00
    };

    // need to make verifying data
    sit_sim_open_channel_rsp *rsp = (sit_sim_open_channel_rsp*)rspData;
    rsp->session_id = 0x02;
    rsp->sw1 = 0x90;
    rsp->sw2 = 0x00;
    rsp->response_len = 0x0d;
    memcpy(rsp->response, response, rsp->response_len);
    return rspData;
}

static UINT8* buildSecureElementTransmitApduLogicalRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 headerLen = (UINT16)sizeof(RCM_HEADER);
    rspData = allocRspModemMem(headerLen, sitId, sitToken);
    if (rspData == nullptr) return nullptr;

    // because response[0];
    int rawLen = 17;
    const UINT8 raw[] = {
        0x90, 0x00, 0x0b, 0x00, 0xdf, 0x20, 0x08, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00
    };

    *datalen = (headerLen + rawLen);

    UINT8 *TransmitApduLogicalRsp = new UINT8[*datalen];
    memset(TransmitApduLogicalRsp, 0x00, *datalen);
    memcpy(TransmitApduLogicalRsp, rspData, headerLen);
    memcpy(TransmitApduLogicalRsp+headerLen, raw, rawLen);
    free(rspData);

    return TransmitApduLogicalRsp;
}

static UINT8* buildSecureElementTransmitApduLogicalWith0x6cRsp(INT16 sitId, UINT sitToken,
        UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 headerLen = (UINT16)sizeof(RCM_HEADER);
    rspData = allocRspModemMem(headerLen, sitId, sitToken);
    if (rspData == nullptr) return nullptr;

    // because response[0];
    int rawLen = 2;
    const UINT8 raw[] = {
        0x6C, 0x14
    };

    *datalen = (headerLen + rawLen);

    UINT8 *TransmitApduLogicalRsp = new UINT8[*datalen];
    memset(TransmitApduLogicalRsp, 0x00, *datalen);
    memcpy(TransmitApduLogicalRsp, rspData, headerLen);
    memcpy(TransmitApduLogicalRsp+headerLen, raw, rawLen);
    free(rspData);

    return TransmitApduLogicalRsp;
}


static UINT8* buildSecureElementResetRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    return rspData;
}

static UINT8* buildSimPbChangedInd(INT16 sitId, UINT32 *datalen, int status) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16) sizeof(sit_sim_pb_ready_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_sim_pb_ready_ind * rsp = (sit_sim_pb_ready_ind *) rspData;
    rsp->pb_ready = status;

    return rspData;
}

static UINT8* buildEnableUiccApplicationsRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;
    return rspData;
}

// sitrilTestSim class
void SitrilTestSim::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    // require that disabled_iccid is already saved.
    SystemProperty::Set("persist.vendor.ril.sim1.disabled_iccid", "");
    SystemProperty::Set("persist.vendor.ril.sim2.disabled_iccid", "");
}
void SitrilTestSim::TearDown() {
    mSitRilTest = nullptr;
}

void SitrilTestSim::ChangeSimStatus(tSimStatusParams *pParam)
{
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->resetTestData();
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SIM_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, pParam);
    psitRilTest->processRsp(RIL_REQUEST_GET_SIM_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    psitRilTest->resetTestData();
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);

}

void SitrilTestSim::SetDefaultSimStatus()
{
    tSimStatusParams pParam;
    ChangeSimStatus(&pParam);
}

////////////////////////////////////////////////////////////////////////////////////////
// SET reqeust|response //

#if TEST_SIM_DEPERSONALIZATION
TEST_P(SitrilTestSim, simVerifyNetworkLock) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);
    // from RIL_PERSOSUBSTATE_SIM_NETWORK(3) to RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER(6)
    for (int i = RIL_PERSOSUBSTATE_SIM_NETWORK; i <= RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER ; i++) {
        // set specific network lock
        tSimStatusParams param = {};
        param.apps_state = RIL_APPSTATE_SUBSCRIPTION_PERSO;
        param.perso_substate = i;
        ChangeSimStatus(&param);

        const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
        ASSERT_NE(nullptr, prilFuncs);

        int rilVer = prilFuncs->version;
        EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

        UINT16 sitId = SIT_VERIFY_NETWORK_LOCK;
        UINT sitToken = TOKEN_INVALID;
        UINT8* rspData = nullptr;
        UINT32 rspDataLen = 0;
        char *pStrings[2];

        char persotype[5];
        sprintf(persotype, "%d", i);
        pStrings[0] = (char *) persotype;
        pStrings[1] = (char *)"0000";

        psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION,
                pStrings, 2 * sizeof(char *), mPhoneIndex);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        UINT32 sizeOfModemData = sizeof(sit_sim_verify_network_lock_req);
        UINT32 txIpcRawDataLen = 0;
        char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
        ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

        ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
        EXPECT_EQ(pModemData->GetMessageId(), sitId);

        /* test RSP */
        sitToken = pModemData->GetToken();
        rspData = BuildVerifyNetworkLockResponse(sitId, sitToken, &rspDataLen, i, 3);
        psitRilTest->processRsp(RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION, rspData, rspDataLen);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        UINT32 rilRspDataLen = 0;
        void *rilRspData= nullptr;
        rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
        ASSERT_NE((UINT32)0, rilRspDataLen);
        ASSERT_NE(nullptr, rilRspData);

        delete pModemData;
        free(rspData);

        psitRilTest->resetTestData();
    }

    SetDefaultSimStatus();
}

TEST_P(SitrilTestSim, getSimStatusForSpecificPersoType) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SIM_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    tSimStatusParams param;
    param.perso_substate = RIL_PERSOSUBSTATE_SIM_NETWORK;
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);

    psitRilTest->processRsp(RIL_REQUEST_GET_SIM_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, simDepersonalizationBeforeGetSimStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_NETWORK_LOCK;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    std::string persoType = std::to_string(50);
    char *pStrings[2] = {
        (char *)persoType.c_str(),  // persoType
        (char *)"0000"      // control key
    };

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION,
            pStrings, 2 * sizeof(char *), mPhoneIndex);

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    free(rspData);
}

TEST_P(SitrilTestSim, simDepersonalization) {
     SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);
    // from RIL_PERSOSUBSTATE_SIM_NETWORK(3) to RIL_PERSOSUBSTATE_SIM_NS_SP_PUK(34)
    bool bMatching_persotype;
    for (int i = RIL_PERSOSUBSTATE_SIM_NETWORK; i <= RIL_PERSOSUBSTATE_SIM_NS_SP_PUK ; i++)  {
        bMatching_persotype = false;
        switch (i) {
        case RIL_PERSOSUBSTATE_SIM_SIM:
        case RIL_PERSOSUBSTATE_SIM_SIM_PUK:
        case RIL_PERSOSUBSTATE_RUIM_HRPD:
        case RIL_PERSOSUBSTATE_RUIM_RUIM:
        case RIL_PERSOSUBSTATE_RUIM_HRPD_PUK:
        case RIL_PERSOSUBSTATE_RUIM_RUIM_PUK:
            LOGI("====== No match PersoType ======");
            break;
        default:
              bMatching_persotype = true;
            break;
        }

        // set specific network lock
        tSimStatusParams param = {};
        param.apps_state = RIL_APPSTATE_SUBSCRIPTION_PERSO;
        param.perso_substate = i;
        ChangeSimStatus(&param);

        const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
        ASSERT_NE(nullptr, prilFuncs);

        int rilVer = prilFuncs->version;
        EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

        UINT16 sitId = SIT_VERIFY_NETWORK_LOCK;
        UINT sitToken = TOKEN_INVALID;
        UINT8* rspData = nullptr;
        UINT32 rspDataLen = 0;
        char *pStrings[2];

        char persotype[5];
        sprintf(persotype, "%d", i);
        pStrings[0] = (char *) persotype;   // persoType
        pStrings[1] = (char *)"0000";   // control key

        if (bMatching_persotype) {
            psitRilTest->processReq(GetRandomSerialNumber(),
                    RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION, pStrings, 2 * sizeof(char *),
                    mPhoneIndex);
            ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

            UINT32 sizeOfModemData = sizeof(sit_sim_verify_network_lock_req);
            UINT32 txIpcRawDataLen = 0;
            char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
            ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

            ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
            EXPECT_EQ(pModemData->GetMessageId(), sitId);

            /* test RSP */
            sitToken = pModemData->GetToken();
            rspData = BuildVerifyNetworkLockResponse(sitId, sitToken, &rspDataLen, i, 3);
            psitRilTest->processRsp(RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION, rspData, rspDataLen);

            ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

            UINT32 rilRspDataLen = 0;
            void *rilRspData= nullptr;
            rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
            ASSERT_NE((UINT32)0, rilRspDataLen);
            ASSERT_NE(nullptr, rilRspData);

            delete pModemData;
            free(rspData);

            psitRilTest->resetTestData();
        } else {
            psitRilTest->processReqNoIpc(GetRandomSerialNumber(),
                    RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION, pStrings, 2 * sizeof(char *),
                    mPhoneIndex);

            UINT32 rilRspDataLen = 0;
            void *rilRspData= nullptr;
            rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
            EXPECT_EQ((UINT32)0, rilRspDataLen);

            free(rspData);
        }
    }

    SetDefaultSimStatus();
}

TEST_P(SitrilTestSim, simDepersonalizationWithNullControlKey) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_NETWORK_LOCK;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    std::string persoType = std::to_string(3);
    char *pStrings[2] = {
        (char *)persoType.c_str(),  // persoType
        nullptr,
    };

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION,
            pStrings, 2 * sizeof(char *), mPhoneIndex);

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    free(rspData);
}
#endif

#if TEST_GET_SIM_STATUS
TEST_P(SitrilTestSim, getIccCardStatusWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_SIM_STATUS;
    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    tSimStatusParams param;
    param.card_state = TEST_GET_SIM_CARD_STATE_PRESENT;
    rspData = buildGetSimStatusErrorRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getIccCardStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_SIM_STATUS;
    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

#ifdef TIMEOUT_SUPPORT
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sleep(35);

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    psitRilTest->resetTestData();

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    tSimStatusParams param;
    param.card_state = TEST_GET_SIM_CARD_STATE_PRESENT;
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
#else
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    tSimStatusParams param;
    param.card_state = TEST_GET_SIM_CARD_STATE_PRESENT;
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
#endif
}

TEST_P(SitrilTestSim, getImsiForApp) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_IMSI;
    UINT16 sitId = SIT_GET_IMSI;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    LOGI("====== GSM AID ======");
    char *pStrings[1];
    pStrings[0] = (char *)"A0000000871002FFFFFFFF89040300FF";

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, sizeof(char *),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_id_get_imsi_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetImsiForAppRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    free(rspData);
    rspData = nullptr;
    delete pModemData;

    LOGI("====== CDMA AID ======");
    pStrings[0] = (char *)"A0000003431002F310FFFF8906110000";

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pStrings, sizeof(char *),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetImsiForAppRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = psitRilTest->getRilRspData(1, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    free(rspData);
    delete pModemData;

}

TEST_P(SitrilTestSim, iccIOForApp) {
    LOGI("====== iccIOForApp ======");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_IO;
    UINT16 sitId = SIT_SIM_IO;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string path = "3F007FFF";
    string aidPtr = "A0000000871002FFFFFFFF89040300FF";
    RIL_SIM_IO_v6 rilIccIo = {};
    rilIccIo.command = 0xb0;
    rilIccIo.fileid = 0x6fad;
    rilIccIo.path = (char *)calloc(path.size(), sizeof(char *));
    strncpy(rilIccIo.path, path.c_str(), path.size());
    rilIccIo.p1 = 0x00;
    rilIccIo.p2 = 0x00;
    rilIccIo.p3 = 0x04;
    rilIccIo.data = NULL;
    rilIccIo.pin2 = NULL;
    rilIccIo.aidPtr = (char *)calloc(aidPtr.size(), sizeof(char *));
    strncpy(rilIccIo.aidPtr, aidPtr.c_str(), aidPtr.size());

    LOGI("iccIOForApp NORMAL SCENARIO");
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&rilIccIo,
            sizeof(RIL_SIM_IO_v6), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_sim_io_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccIOForAppRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);

    LOGI("iccIOForApp COMMAND_STATUS SCENARIO");

    rilIccIo.command = 0xf2;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&rilIccIo,
            sizeof(RIL_SIM_IO_v6), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccIOForAppRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = psitRilTest->getRilRspData(1, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);

    LOGI("iccIOForApp COMMAND_STATUS SCENARIO");

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&rilIccIo,
            sizeof(RIL_SIM_IO_v6), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(2, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccIOForAppRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = psitRilTest->getRilRspData(2, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);
}

TEST_P(SitrilTestSim, iccIOForFDN) {
    LOGI("====== iccIOForFDN ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT sitToken = TOKEN_INVALID;
    UINT8* rspDataPin2 = nullptr;
    UINT8* rspDataSimIo = nullptr;
    UINT32 rspDataLen = 0;
    UINT32 sizeOfModemData = 0;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;

    string path = "3F007FFF";
    string aidPtr = "A0000000871002FFFFFFFF89040300FF";
    string pin2 = "1234";
    RIL_SIM_IO_v6 rilIccIo = {};
    rilIccIo.command = 0xD6;
    rilIccIo.fileid = 0x6F3B;
    rilIccIo.path = (char *)calloc(path.size(), sizeof(char *));
    strncpy(rilIccIo.path, path.c_str(), path.size());
    rilIccIo.p1 = 0x00;
    rilIccIo.p2 = 0x00;
    rilIccIo.p3 = 0x04;
    rilIccIo.data = NULL;
    rilIccIo.pin2 = (char *)calloc(pin2.size(), sizeof(char *));
    strncpy(rilIccIo.pin2, pin2.c_str(), pin2.size());
    rilIccIo.aidPtr = (char *)calloc(aidPtr.size(), sizeof(char *));
    strncpy(rilIccIo.aidPtr, aidPtr.c_str(), aidPtr.size());

    LOGI("iccIOForFDN NORMAL SCENARIO");
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_SIM_IO, (char *)&rilIccIo
            , sizeof(RIL_SIM_IO_v6), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());


    UINT16 sitId = SIT_VERIFY_SIM_PIN2;
    sizeOfModemData = sizeof(sit_sim_verify_sim_pin2_req);
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemDataForPin2 = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemDataForPin2->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemDataForPin2->GetToken();
    rspDataPin2 = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PIN2, rspDataPin2, rspDataLen);
    usleep(150*1000);   // 150ms

    sitId = SIT_SIM_IO;
    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    sizeOfModemData = sizeof(sit_sim_sim_io_req);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemDataForSimIo = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemDataForSimIo->GetMessageId(), sitId);

    sitToken = pModemDataForSimIo->GetToken();
    rspDataSimIo = buildIccIOForAppRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(RIL_REQUEST_SIM_IO, rspDataSimIo, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    if (rspDataPin2 != nullptr) free(rspDataPin2);
    if (pModemDataForPin2 != nullptr) delete pModemDataForPin2;

    psitRilTest->resetTestData();

    LOGI("========== iccIOForFDN ERROR SCENARIO (RIL_E_SIM_PIN2) ==========");

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_SIM_IO, (char *)&rilIccIo
            , sizeof(RIL_SIM_IO_v6), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sitId = SIT_VERIFY_SIM_PIN2;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    sizeOfModemData = sizeof(sit_sim_verify_sim_pin2_req);
    pModemDataForPin2 = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    /* test RSP */
    sitToken = pModemDataForPin2->GetToken();
    rspDataPin2 = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3, 4 /* RIL_E_SIM_PIN2 */);
    psitRilTest->processRsp(RIL_REQUEST_SIM_IO, rspDataPin2, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);

    if (rilIccIo.path != nullptr) free(rilIccIo.path);
    if (rilIccIo.pin2 != nullptr) free(rilIccIo.pin2);
    if (rilIccIo.aidPtr != nullptr) free(rilIccIo.aidPtr);

    if (rspDataPin2 != nullptr) free(rspDataPin2);
    if (rspDataSimIo != nullptr) free(rspDataSimIo);

    if (pModemDataForSimIo != nullptr) delete pModemDataForSimIo;
    if (pModemDataForPin2 != nullptr) delete pModemDataForPin2;
}

TEST_P(SitrilTestSim, iccIOForOEM) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_IMS_SIM_IO;
    UINT16 sitId = SIT_SIM_IO;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const char rawData[] = {
        0xb0, 0x02, 0xad, 0x6f, 0x04, 0x00, 0x3f, 0xff, 0x7f, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x10, 0xa0, 0x00, 0x00, 0x00, 0x87, 0x10,
        0x02, 0xff, 0xff, 0xff, 0xff, 0x89, 0x04, 0x03, 0x00, 0xff
    };

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&rawData,
            sizeof(rawData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_sim_io_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccIOForAppRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, iccTransmitApduBasicChannel) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC;
    UINT16 sitId = SIT_TRANSMIT_SIM_APDU_BASIC;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    RIL_SIM_APDU apdu = {};
    apdu.sessionid = 0x00;
    apdu.cla = 0x80;
    apdu.instruction = 0xF2;
    apdu.p1 = 0x00;
    apdu.p2 = 0x00;
    apdu.p3 = 0x00;
    apdu.data = (char *)"1234";

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&apdu,
            sizeof(RIL_SIM_APDU), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_transmit_sim_apdu_basic_req) + strlen(apdu.data);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccTransmitApduBasicChannelRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, iccOpenLogicalChannelWithP2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SIM_OPEN_CHANNEL;
    UINT16 sitId = SIT_OPEN_SIM_CHANNEL_WITH_P2;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    string aid = "A00000015141434C";
    RIL_OpenChannelParams params = {};

    params.aidPtr = (char *)aid.c_str();
    params.p2 = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&params,
            sizeof(RIL_OpenChannelParams), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_open_sim_channel_with_p2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccOpenLogicalChannelRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, iccOpenLogicalChannel) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SIM_OPEN_CHANNEL;
    UINT16 sitId = SIT_OPEN_SIM_CHANNEL;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    string aid = "A00000015141434C";
    RIL_OpenChannelParams params = {};

    params.aidPtr = (char *)aid.c_str();
    params.p2 = -1;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&params,
            sizeof(RIL_OpenChannelParams), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_open_sim_channel_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccOpenLogicalChannelRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, iccOpenLogicalChannelWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SIM_OPEN_CHANNEL;
    UINT16 sitId = SIT_OPEN_SIM_CHANNEL_WITH_P2;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    string aid = "A00000015141434C";
    RIL_OpenChannelParams params = {};

    params.aidPtr = (char *)aid.c_str();
    params.p2 = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&params,
            sizeof(RIL_OpenChannelParams), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_open_sim_channel_with_p2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccOpenLogicalChannelRsp(sitId, sitToken, &rspDataLen, 2);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, iccCloseLogicalChannel) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_CLOSE_CHANNEL;
    UINT16 sitId = SIT_CLOSE_SIM_CHANNEL;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int pInts[1] = { 0x02 };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)pInts, sizeof(int),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_close_sim_channel_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccCloseLogicalChannelRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, iccCloseLogicalChannelWithInvalidSessionId) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_CLOSE_CHANNEL;
    //UINT16 sitId = SIT_CLOSE_SIM_CHANNEL;
    //UINT sitToken = TOKEN_INVALID;
    //UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    int pInts[1] = { 0x00 };
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, (char *)pInts, sizeof(int),
            mPhoneIndex);

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);

}

TEST_P(SitrilTestSim, iccTransmitApduLogicalChannel) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL;
    UINT16 sitId = SIT_TRANSMIT_SIM_APDU_CHANNEL;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string data = "BF2D0D5C0B5A909192B79F709599BF76";
    RIL_SIM_APDU apdu = {};
    apdu.sessionid = 0x01;
    apdu.cla = 0x81;
    apdu.instruction = 0xE2;
    apdu.p1 = 0x91;
    apdu.p2 = 0x00;
    apdu.p3 = 0x10;
    apdu.data = (char *)data.c_str();

    UINT32 sizeOfModemData = sizeof(sit_sim_transmit_sim_apdu_channel_req) + strlen(apdu.data);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&apdu,
            sizeof(RIL_SIM_APDU), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildIccTransmitApduLogicalChannelRsp(sitId, sitToken, &rspDataLen, true);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

#endif

#if TEST_STK_MODULE

TEST_P(SitrilTestSim, stkSendEnvelopeCommand) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND;
    UINT16 sitId = SIT_SEND_STK_ENVELOPE_CMD;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string testEnvelopCmd = "D30782020181900101";
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *) testEnvelopCmd.c_str(),
            sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_send_stk_envelope_cmd_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildStkSendEnvelopeCommandRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, stkSendTerminalRsp) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE;
    UINT16 sitId = SIT_SEND_STK_TERMINAL_RSP;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string testTerminalRsp = "810301150302028281830110";
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *) testTerminalRsp.c_str(),
            sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_send_stk_envelope_cmd_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildStkSendEnvelopeCommandRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, stkSendEnvelopWithStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS;
    UINT16 sitId = SIT_SEND_STK_ENVELOPE_WITH_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string testEnvelopCmd = "D30782020181900101";
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *) testEnvelopCmd.c_str(),
            sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_send_stk_envelope_with_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildStkSendEnvelopeStatusRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

//int RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING = 103;
// In case that there is no SIT command
TEST_P(SitrilTestSim, stkIsRunning) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestSim, stkProactiveCommand) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_PROACTIVE_COMMAND;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    for(int i = 0; i < PROACTIVE_UICC_COMMANDS_NUM; ++i) {
        // These test vectors cause heap-buffer-overflow.
        if (i == SET_UP_MENU_NORMAL || i == SELECT_ITEM ||
            i == LAUNCH_BROWSER || i == GET_CHANNEL_STATUS) {
            continue;
        }

        rspData = buildStkProactiveCmdInd(sitId, &rspDataLen, i);
        psitRilTest->processInd(RIL_UNSOL_STK_PROACTIVE_COMMAND, rspData, rspDataLen);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    }

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);
    free(rspData);
}

TEST_P(SitrilTestSim, stkSimRefresh) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    UINT8* rspRefreshData = nullptr;
    UINT32 rspRefreshDataLen = 0;

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;

    // Proactive command for refresh
    for (int i = SIM_REFRESH_FILE_UPDATE; i <= SIM_REFRESH_RESET; ++i) {
        rspData = buildStkProactiveCmdInd(SIT_IND_STK_PROACTIVE_COMMAND, &rspDataLen, i);
        psitRilTest->processInd(RIL_UNSOL_STK_PROACTIVE_COMMAND, rspData, rspDataLen);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        /* test IND */
        rspRefreshData = buildStkSimRefreshInd(SIT_IND_SIM_REFRESH, &rspRefreshDataLen,
                (i - SIM_REFRESH_FILE_UPDATE));
        psitRilTest->processInd(RIL_UNSOL_SIM_REFRESH, rspRefreshData, rspRefreshDataLen);
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

        rilIndData = psitRilTest->getRilIndData(0, &indLen);
        ASSERT_NE((UINT32)0, indLen);
    }
    free(rspData);
    free(rspRefreshData);
}

TEST_P(SitrilTestSim, stkSessionEnd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_SESSION_END;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildStkSessionEndInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_STK_SESSION_END, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_EQ((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, stkHandleCallSetupReqFromSim) {

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_STK_CALL_SETUP;
    int testRilCmdId = RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int nUserOper = 0x00; //SIT_STK_CALL_SETUP_REJECT
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &nUserOper, sizeof(nUserOper),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_call_setup_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    tSimStatusParams param;
    param.card_state = TEST_GET_SIM_CARD_STATE_PRESENT;
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, ssReturnResult) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SS_RETURN_RESULT;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSsReturnResult(sitId,&rspDataLen);

    psitRilTest->processInd(RIL_UNSOL_SUPP_SVC_RETURN_RESULT, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, stkCcAlphaNtf) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_CC_ALPHA_NOTIFY;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildStkCcAlphaNtf(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_STK_CC_ALPHA_NOTIFY, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);
    free(rspData);
}

#endif

#if TEST_SIM_AUTH

TEST_P(SitrilTestSim, requestIsimAuthentication) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_ISIM_AUTHENTICATION;
    UINT16 sitId = SIT_GET_SIM_AUTH;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Refer  testEapAkaAuthentication() in cts/CarrierApiTest.java and TS 123 108
    // String EAP_SIM_AKA_RAND = "11111111111111111111111111111111"; // MAX 16 size
    // String EAP_AKA_AUTN = "12351417161900001130131215141716"; // MAX 16 size
    // String challenge = "10" + EAP_SIM_AKA_RAND + "10" + EAP_AKA_AUTN;

    string base64Challenge = "MTAxMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTEwMTIzNTE0MTcxNjE5MDAwMDExMzAxMzEyMTUxNDE3MTY=";
    int len = base64Challenge.size();
    char *pAuthData = new char[len + 1];
    memset(pAuthData, 0, len + 1);
    strncpy(pAuthData, base64Challenge.c_str(), len);

    /* test RSP */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pAuthData, sizeof(pAuthData),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildIsimAuthenticationRsp(sitId, sitToken, &rspDataLen, RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete[] pAuthData;
    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, requestIsimAuthenticationWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_ISIM_AUTHENTICATION;
    UINT16 sitId = SIT_GET_SIM_AUTH;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Refer  testEapAkaAuthentication() in cts/CarrierApiTest.java and TS 123 108
    // String EAP_SIM_AKA_RAND = "11111111111111111111111111111111"; // MAX 16 size
    // String EAP_AKA_AUTN = "12351417161900001130131215141716"; // MAX 16 size
    // String challenge = "10" + EAP_SIM_AKA_RAND + "10" + EAP_AKA_AUTN;

    string base64Challenge = "MTAxMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMTEwMTIzNTE0MTcxNjE5MDAwMDExMzAxMzEyMTUxNDE3MTY=";
    int len = base64Challenge.size();
    char *pAuthData = new char[len + 1];
    memset(pAuthData, 0, len + 1);
    strncpy(pAuthData, base64Challenge.c_str(), len);

    /* test RSP */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, pAuthData, sizeof(pAuthData),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildIsimAuthenticationRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete[] pAuthData;
    delete pModemData;
    free(rspData);
}

//int RIL_REQUEST_SIM_AUTHENTICATION = 125;
TEST_P(SitrilTestSim, requestIccSimAuthentication) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_AUTHENTICATION;
    UINT16 sitId = SIT_GET_SIM_AUTH;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    RIL_SimAuthentication pf = {};
    // per 3GPP TS 31.102 (Section 7.1.2)
    /* about auth_type(authContext)
    public static final int AUTH_CONTEXT_EAP_SIM = 128;
    public static final int AUTH_CONTEXT_EAP_AKA = 129;
    */

    pf.authContext = 0x80; //AUTH_CONTEXT_EAP_SIM

    // Refer   testEapSimAuthentication() in cts/CarrierApiTest.java and TS 123 108
    // String EAP_SIM_AKA_RAND = "11111111111111111111111111111111"; // MAX 16 size
    // String challenge = "10" + EAP_SIM_AKA_RAND;

    string base64Challenge = "MTAxMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMQ==";
    int len = base64Challenge.size();
    char *pAuthData = new char[len + 1];
    memset(pAuthData, 0, len + 1);
    strncpy(pAuthData, base64Challenge.c_str(), len);
    pf.authData = pAuthData;

    string AID = "A0000000871002FFFFFFFF89040300FF"; //USIM_AID
    len = AID.size();
    char *pAID = new char[len + 1];
    memset(pAID, 0, len + 1);
    strncpy(pAID, AID.c_str(), len);
    pf.aid = pAID;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &pf,
            sizeof(RIL_SimAuthentication), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSimAuthenticationRsp(sitId, sitToken, &rspDataLen, RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete [] pAuthData;
    delete [] pAID;
    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, requestIccSimAuthenticationWithGenericFailure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_AUTHENTICATION;
    UINT16 sitId = SIT_GET_SIM_AUTH;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

        RIL_SimAuthentication pf = {};
    // per 3GPP TS 31.102 (Section 7.1.2)
    /* about auth_type(authContext)
    public static final int AUTH_CONTEXT_EAP_SIM = 128;
    public static final int AUTH_CONTEXT_EAP_AKA = 129;
    */

    pf.authContext = 0x80; //AUTH_CONTEXT_EAP_SIM

    // Refer   testEapSimAuthentication() in cts/CarrierApiTest.java and TS 123 108
    // String EAP_SIM_AKA_RAND = "11111111111111111111111111111111"; // MAX 16 size
    // String challenge = "10" + EAP_SIM_AKA_RAND;

    string base64Challenge = "MTAxMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMQ==";
    int len = base64Challenge.size();
    char *pAuthData = new char[len + 1];
    memset(pAuthData, 0, len + 1);
    strncpy(pAuthData, base64Challenge.c_str(), len);
    pf.authData = pAuthData;

    string AID = "A0000000871002FFFFFFFF89040300FF"; //USIM_AID
    len = AID.size();
    char *pAID = new char[len + 1];
    memset(pAID, 0, len + 1);
    strncpy(pAID, AID.c_str(), len);
    pf.aid = pAID;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &pf,
            sizeof(RIL_SimAuthentication), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSimAuthenticationRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete [] pAuthData;
    delete [] pAID;
    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, requestIccSimAuthenticationWithAnotherFailure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_AUTHENTICATION;
    UINT16 sitId = SIT_GET_SIM_AUTH;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

       RIL_SimAuthentication pf = {};
    // per 3GPP TS 31.102 (Section 7.1.2)
    /* about auth_type(authContext)
    public static final int AUTH_CONTEXT_EAP_SIM = 128;
    public static final int AUTH_CONTEXT_EAP_AKA = 129;
    */

    pf.authContext = 0x80; //AUTH_CONTEXT_EAP_SIM

    // Refer   testEapSimAuthentication() in cts/CarrierApiTest.java and TS 123 108
    // String EAP_SIM_AKA_RAND = "11111111111111111111111111111111"; // MAX 16 size
    // String challenge = "10" + EAP_SIM_AKA_RAND;

    string base64Challenge = "MTAxMTExMTExMTExMTExMTExMTExMTExMTExMTExMTExMQ==";
    int len = base64Challenge.size();
    char *pAuthData = new char[len + 1];
    memset(pAuthData, 0, len + 1);
    strncpy(pAuthData, base64Challenge.c_str(), len);
    pf.authData = pAuthData;

    string AID = "A0000000871002FFFFFFFF89040300FF"; //USIM_AID
    len = AID.size();
    char *pAID = new char[len + 1];
    memset(pAID, 0, len + 1);
    strncpy(pAID, AID.c_str(), len);
    pf.aid = pAID;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &pf,
            sizeof(RIL_SimAuthentication), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_auth_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSimAuthenticationRsp(sitId, sitToken, &rspDataLen, RIL_E_REQUEST_NOT_SUPPORTED);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete [] pAuthData;
    delete [] pAID;
    delete pModemData;
    free(rspData);
}
#endif

//int RIL_REQUEST_SET_SIM_CARD_POWER = 140;
TEST_P(SitrilTestSim, togglingSimCardPower_1_6) {
    LOGI("====== START togglingSimCardPower_1_6 ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 6));
    UINT16 sitId = SIT_SET_SIM_CARD_POWER;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    unsigned int simCardPower = SIM_CARD_POWERSTATE_POWER_DOWN; //SIM_CARD_POWERSTATE_POWER_DOWN
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &simCardPower,
            sizeof(simCardPower), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_sim_card_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    EXPECT_EQ(((sit_sim_set_sim_card_power_req *)txIpcRawData)->mode,
        SIM_CARD_POWERSTATE_POWER_DOWN_PRESENT);

    ModemData *pModemData_down = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_down->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData_down->GetToken();
    rspData = buildSetSimCardPowerRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData_down;
    free(rspData);

    psitRilTest->resetTestData();

    sitToken = TOKEN_INVALID;
    rspData = nullptr;
    rspDataLen = 0;

    simCardPower = SIM_CARD_POWERSTATE_POWER_UP; //SIM_CARD_POWERSTATE_POWER_UP
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &simCardPower,
            sizeof(simCardPower), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sizeOfModemData = sizeof(sit_sim_set_sim_card_power_req);
    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    EXPECT_EQ(((sit_sim_set_sim_card_power_req *)txIpcRawData)->mode,
        SIM_CARD_POWERSTATE_POWER_UP);

    ModemData *pModemData_up = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_up->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData_up->GetToken();
    rspData = buildSetSimCardPowerRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData_up;
    free(rspData);
}

TEST_P(SitrilTestSim, SimCardPowerInvalidInput) {
    LOGI("====== START SimCardPowerInvalidInput ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 1));

    // HAL_VERSION_CODE(1, 1) nSimCardPowerState - POWER_DOWN / UP / PASS_THROUGH
    unsigned int simCardPower = SIM_CARD_POWERSTATE_POWER_RESET;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &simCardPower,
            sizeof(simCardPower), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_EQ(RIL_E_INVALID_ARGUMENTS, psitRilTest->getRilRspErrno());

    psitRilTest->resetTestData();

    testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 0));

    // HAL_VERSION_CODE(1, 0) nSimCardPowerState - POWER_DOWN / UP
    simCardPower = SIM_CARD_POWERSTATE_POWER_UP_PASS_THROUGH;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &simCardPower,
           sizeof(simCardPower), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_EQ(RIL_E_INVALID_ARGUMENTS, psitRilTest->getRilRspErrno());

}

//misc
//int RIL_REQUEST_ENABLE_MODEM = 146;
TEST_P(SitrilTestSim, enableModem) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_ENABLE_MODEM;
    UINT16 sitId = SIT_SET_STACK_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    unsigned int enablemodem = 1; //enable
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &enablemodem,
            sizeof(enablemodem), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pwr_set_stack_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildEnanbleModemRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}


//misc
//int RIL_REQUEST_GET_MODEM_STATUS = 147;
TEST_P(SitrilTestSim, getModemStackStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_MODEM_STATUS;
    UINT16 sitId = SIT_GET_STACK_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetModemStackStatusRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, togglingUiccApplicationsSimAbsent) {

    LOGI("====== START togglingUiccApplicationsSimAbsent ======");

    //  require SIM ABSENT
    tSimStatusParams param = {};
    param.card_state = TEST_GET_SIM_CARD_STATE_ABSENT;
    ChangeSimStatus(&param);

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // for req
    int testRilCmdId = RIL_REQUEST_ENABLE_UICC_APPLICATIONS;
    // for rsp
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;

    unsigned int enableuiccapplication = UICCAPPLICATION_DISABLE;
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId,
           &enableuiccapplication, sizeof(unsigned int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    // As SIM is absent, SIM_ABSENT should be returned
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_EQ(RCM_E_SIM_ABSENT, psitRilTest->getRilRspErrno());

    // Query uiccapplication state
    testRilCmdId = RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT;

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    rilRspDataLen = 0;
    rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(1, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_EQ(RCM_E_SIM_ABSENT, psitRilTest->getRilRspErrno());
}

//int RIL_REQUEST_ENABLE_UICC_APPLICATIONS = 208;
TEST_P(SitrilTestSim, togglingUiccApplicationsSimPresent) {

    LOGI("====== START togglingUiccApplicationsSimPresent ======");

    // require SIM Present
    SetDefaultSimStatus();

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_ENABLE_UICC_APPLICATIONS;
    UINT16 sitId = SIT_SET_UICC_SUBSCRIPTION;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

   // disable uiccapplication
    unsigned int enableuiccapplication = UICCAPPLICATION_DISABLE;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId,
           &enableuiccapplication, sizeof(unsigned int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_uicc_sub_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData_disable = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_disable->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData_disable->GetToken();
    rspData = buildEnableUiccApplicationsRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData_disable;
    free(rspData);

    psitRilTest->resetTestData();

    // Query uiccapplication state and check whether uiccapplication state is disable
    testRilCmdId = RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    rilRspDataLen = 0;
    rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(int), rilRspDataLen);
    EXPECT_EQ(UICCAPPLICATION_DISABLE, *((int *)rilRspData));

    psitRilTest->resetTestData();

    // enable uiccapplication
    testRilCmdId = RIL_REQUEST_ENABLE_UICC_APPLICATIONS;
    sitId = SIT_SET_UICC_SUBSCRIPTION;
    sitToken = TOKEN_INVALID;
    rspData = nullptr;
    rspDataLen = 0;

    enableuiccapplication = UICCAPPLICATION_ENABLE;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId,
           &enableuiccapplication, sizeof(unsigned int), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    sizeOfModemData = sizeof(sit_sim_set_uicc_sub_req);
    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData_enable = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_enable->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData_enable->GetToken();
    rspData = buildEnableUiccApplicationsRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspDataLen = 0;
    rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData_enable;
    free(rspData);

    psitRilTest->resetTestData();

    // Query uiccapplication state and check whether uiccapplication state is enable
    testRilCmdId = RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT;
    //UINT16 sitId = TOKEN_INVALID;    // no IPC definition

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // check response because NO IPC build case.
    rilRspDataLen = 0;
    rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ(sizeof(int), rilRspDataLen);
    EXPECT_EQ(UICCAPPLICATION_ENABLE, *((int *)rilRspData));
}

TEST_P(SitrilTestSim, autoDisableUiccApplication) {

    LOGI("====== START autoDisableUiccApplication ======");

    // require that disabled_iccid is already saved.
    SystemProperty::Set("persist.vendor.ril.sim1.disabled_iccid", "8982300215013945185F");
    SystemProperty::Set("persist.vendor.ril.sim2.disabled_iccid", "8982300215013945185F");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    //  require SIM ABSENT to init "m_bUiccApplicationsEnablementChanged = true"
    tSimStatusParams param = {};
    param.card_state = TEST_GET_SIM_CARD_STATE_ABSENT;
    ChangeSimStatus(&param);

    psitRilTest->resetTestData();

    // 1. GET_SIM_STATUS (RIL_APPSTATE_READY)
    int testRilCmdId = RIL_REQUEST_GET_SIM_STATUS;
    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId,
         nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData_ready = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_ready->GetMessageId(), sitId);

    tSimStatusParams param_ready = {};
    param_ready.card_state = TEST_GET_SIM_CARD_STATE_PRESENT;
    param_ready.application_num = 1;
    param_ready.apps_state = RIL_APPSTATE_READY;

    /* test RSP */
    sitToken = pModemData_ready->GetToken();
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param_ready);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    usleep(150*1000);   // 150ms

    delete pModemData_ready;
    free(rspData);

    // 2. SET_UICC_SUBSCRIPTION for disable
    testRilCmdId = RIL_REQUEST_ENABLE_UICC_APPLICATIONS;
    sitId = SIT_SET_UICC_SUBSCRIPTION;
    sitToken = TOKEN_INVALID;
    rspData = nullptr;
    rspDataLen = 0;

    sizeOfModemData = sizeof(sit_sim_set_uicc_sub_req);
    txIpcRawDataLen = 0;
    txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData_disable = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_disable->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData_disable->GetToken();
    rspData = buildEnableUiccApplicationsRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    usleep(150*1000);   // 150ms

    delete pModemData_disable;
    free(rspData);

    testRilCmdId = RIL_REQUEST_GET_SIM_STATUS;
    sitId = SIT_GET_SIM_STATUS;
    sitToken = TOKEN_INVALID;
    rspData = nullptr;
    rspDataLen = 0;

    sizeOfModemData = sizeof(null_data_format);
    txIpcRawDataLen = 0;

    txIpcRawData = psitRilTest->getTxIpcRawData(2, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData_detected = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData_detected->GetMessageId(), sitId);

    /* test RSP */

    tSimStatusParams param_detected = {};
    param_detected.card_state = TEST_GET_SIM_CARD_STATE_PRESENT;
    param_detected.application_num = 1;
    param_detected.apps_state = RIL_APPSTATE_DETECTED;

    sitToken = pModemData_detected->GetToken();
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param_detected);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    V2_0::RIL_CardStatus& info = *(V2_0::RIL_CardStatus *)rilRspData;
    // Todo
    EXPECT_EQ(RIL_APPSTATE_DETECTED, info.applications[0].app_state);

    delete pModemData_detected;
    free(rspData);
}


#if TEST_RADIOCONFIG
TEST_P(SitrilTestSim, getSlotStatusConvertLegacyToAidl) {
    LOGI("====== START getSlotStatusConvertLegacyToAidl ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_GET_SLOT_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SLOT_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSlotStatusValidLegacyRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(RIL_REQUEST_GET_SLOT_STATUS, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, getSlotStatusWithValidData) {
    LOGI("====== START getSlotStatusWithValidData ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_GET_SLOT_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SLOT_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSlotStatusValidRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(RIL_REQUEST_GET_SLOT_STATUS, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getSlotStatusWithInValidData) {
    LOGI("====== START getSlotStatusWithInValidData ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_GET_SLOT_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SLOT_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSlotStatusInvalidRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(RIL_REQUEST_GET_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getSlotStatusWithPresentButMemberInvalidData) {
    LOGI("====== START getSlotStatusWithPresentButMemberInvalidData ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_GET_SLOT_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SLOT_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetSlotStatusOnlyAtrIccIdEidNullRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(RIL_REQUEST_GET_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getSlotStatusToAidl) {
    LOGI("====== START getSlotStatusToAidl ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_GET_SLOT_STATUS, HAL_VERSION_CODE(2, 0));
    UINT16 sitId = SIT_GET_SLOT_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSlotStatusAidlRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, setLogicalSlotMappingHidl) {
    LOGI("====== START setLogicalSlotMappingHidl ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING,
            HAL_VERSION_CODE(1, 6));
    UINT16 sitId = SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* { physicalSlotId, physicalSlotId } */
    int uData[2] = {0, 1};

    psitRilTest->processReq(GetRandomSerialNumber(),
            testRilCmdId, uData, sizeof(uData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_logical_to_physical_slot_mapping_req_v1_2);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSetLogicalSlotMappingResponse(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, setLogicalSlotMappingAidl) {
    LOGI("====== START setLogicalSlotMappingAidl ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING, HAL_VERSION_CODE(2, 0));
    UINT16 sitId = SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /*
        typedef struct {
            int32_t physicalSlotId;
            int32_t portId;
        } RIL_SlotPortMapping;

        typedef struct {
            int32_t num_slots;
            RIL_SlotPortMapping slotPortMap[MAX_PORT_NUM];
        } RIL_SlotPortMappingReq;
    */
    int uData[5] = {2, 0, 0, 1, 0};

    psitRilTest->processReq(GetRandomSerialNumber(),
            testRilCmdId, uData, sizeof(uData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_logical_to_physical_slot_mapping_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSetLogicalSlotMappingResponse(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, setLogicalSlotMappingAidlInvalidInput) {
    LOGI("====== START setLogicalSlotMappingAidlInvalidInput ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING, HAL_VERSION_CODE(2, 0));
    UINT16 sitId = SIT_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /*
        typedef struct {
            int32_t physicalSlotId;
            int32_t portId;
        } RIL_SlotPortMapping;

        typedef struct {
            int32_t num_slots;
            RIL_SlotPortMapping slotPortMap[MAX_PORT_NUM];
        } RIL_SlotPortMappingReq;
    */
    int uData[5] = {2, 0, 0, 1, 0};

    psitRilTest->processReq(GetRandomSerialNumber(),
            testRilCmdId, uData, sizeof(uData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_logical_to_physical_slot_mapping_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSetLogicalSlotMappingErrorResponse(sitId, sitToken, &rspDataLen, RIL_E_CANCELLED);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    // from RIL_E_CANCELLED to RIL_E_SUCCESS in simservice
    EXPECT_EQ(RIL_E_SUCCESS, psitRilTest->getRilRspErrno());

    delete pModemData;
    free(rspData);

}

#endif
#if TEST_VERIFY_PIN_PUK
TEST_P(SitrilTestSim, autoVerifyPinForKernelPanic) {
    // Set kernel panic conditions
    SystemProperty::Set("sys.boot.reason", "kernel_panic");
    char szPropertyName[PROPERTY_NAME_LEN] = {0, };
    snprintf(szPropertyName, (PROPERTY_NAME_LEN - 1), "persist.vendor.ril.encrypted.pin.%d", 0);
    SystemProperty::Set((const char*)szPropertyName, "43BD3E75F2B2121E6F2968E7A612");

    // SIT_GET_SIM_STATUS, REQ, RSP
    tSimStatusParams param = {};
    param.apps_state = RIL_APPSTATE_PIN;
    param.pin1_state = RIL_PINSTATE_ENABLED_NOT_VERIFIED;

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_GET_SIM_STATUS,
            nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(RIL_REQUEST_GET_SIM_STATUS, rspData, rspDataLen);

    usleep(150*1000);   // 150ms

    delete pModemData;
    free(rspData);

    // SIT_VERIFY_SIM_PIN, RSP
    sitId = SIT_VERIFY_SIM_PIN;
    sitToken = TOKEN_INVALID;
    rspData = nullptr;
    rspDataLen = 0;

    sizeOfModemData = sizeof(sit_sim_verify_sim_pin_req);
    txIpcRawDataLen = 0;
    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PIN, rspData, rspDataLen);

    usleep(150*1000);   // 150ms

    delete pModemData;
    free(rspData);

    // SIT_GET_SIM_STATUS, RSP
    sitId = SIT_GET_SIM_STATUS;
    sitToken = TOKEN_INVALID;
    rspData = nullptr;
    rspDataLen = 0;

    sizeOfModemData = sizeof(null_data_format);
    txIpcRawDataLen = 0;
    txIpcRawData = psitRilTest->getTxIpcRawData(2, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    param.apps_state = RIL_APPSTATE_READY;
    param.pin1_state = RIL_PINSTATE_ENABLED_VERIFIED;
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(RIL_REQUEST_GET_SIM_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    delete pModemData;
    free(rspData);

    SetDefaultSimStatus();
}

TEST_P(SitrilTestSim, verifyPin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_VERIFY_SIM_PIN;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[2];

    pStrings[0] = (char *) "1234";       // pin
    pStrings[1] = (char *) "a0000000871002ffffffff89040300ff"; // aid

    LOGI("===== Normal Case =====");
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN,
            pStrings, 2 * sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_verify_sim_pin_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PIN, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);

    psitRilTest->resetTestData();

    LOGI("===== Error Case =====");

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN,
            pStrings, 2 * sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3, 2);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PIN, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);

}

TEST_P(SitrilTestSim, verifyInvalidPin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PIN;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    const char *pStrings[2];

    pStrings[0] = (char *)"1234569761";  // pin
    pStrings[1] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN,
            pStrings, 2 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, verifyNullPin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PIN;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char *pStrings[2];

    pStrings[0] = NULL; // pin
    pStrings[1] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN,
            pStrings, 2 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}


TEST_P(SitrilTestSim, verifyPuk) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_VERIFY_SIM_PUK;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[3];

    pStrings[0] = (char *)"12345678";   // puk
    pStrings[1] = (char *)"1234";   // pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PUK,
            pStrings, 3 * sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_verify_sim_puk_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PUK, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, verifyInvalidPuk) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PUK;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char *pStrings[3];

    pStrings[0] = (char *)"12345678901112";  // puk
    pStrings[1] = (char *)"12";  // pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PUK,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, verifyNullPuk) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PUK;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char *pStrings[3];

    pStrings[0] = NULL; // puk
    pStrings[1] = (char *)"1234";  // pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PUK,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, verifyPin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_VERIFY_SIM_PIN2;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[2];

    pStrings[0] = (char *)"1234";  // pin2
    pStrings[1] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN2,
            pStrings, 2 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_verify_sim_pin2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PIN2, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, verifyInvalidPin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PIN2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[2];

    pStrings[0] = (char *)"7861781582436578263";  // pin2
    pStrings[1] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN2,
            pStrings, 2 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

     free(rspData);
}

TEST_P(SitrilTestSim, verifyNullPin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PIN2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[2];

    pStrings[0] = NULL; // pin2
    pStrings[1] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PIN2,
            pStrings, 2 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

     free(rspData);
}

TEST_P(SitrilTestSim, verifyPuk2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_VERIFY_SIM_PUK2;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[3];

    pStrings[0] = (char *)"12345678";   // puk2
    pStrings[1] = (char *)"1324";   // pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PUK2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_verify_sim_puk2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PUK2, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, verifyInvalidPuk2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PUK2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    const char *pStrings[3];

    pStrings[0] = "1234567890182";  // puk
    pStrings[1] = "31";  // pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PUK2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, verifyNullPuk2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_VERIFY_SIM_PUK2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    const char *pStrings[3];

    pStrings[0] = NULL; // puk
    pStrings[1] = "1234";   // pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff";  // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_ENTER_SIM_PUK2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}
#endif

#if TEST_SIM_LOCK

TEST_P(SitrilTestSim, setFacilityLock) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_SET_FACILITY_LOCK;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    const char *pStrings[6];

    pStrings[0] = "SC"; // code
    pStrings[1] = "1";  // lock state
    pStrings[2] = "0000";   // password
    pStrings[3] = "7";  // service class
    pStrings[4] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    LOGI("====== NORMAL SCENARIO ======");
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            pStrings, 5 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_facility_lock_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_SET_FACILITY_LOCK, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);

    psitRilTest->resetTestData();

    LOGI("====== GENERIC FAILURE SCENARIO ======");
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            pStrings, 5 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(RIL_REQUEST_SET_FACILITY_LOCK, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);

    if (pModemData != nullptr) delete pModemData;
    if (rspData != nullptr) free(rspData);

    psitRilTest->resetTestData();

    LOGI("====== INVALID_ARGUMENTS 1 SCENARIO ======");
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            NULL, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    LOGI("====== INVALID_ARGUMENTS 2 SCENARIO ======");
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            pStrings, 6 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

TEST_P(SitrilTestSim, getFacilityLock) {

    LOGI("====== START getFacilityLock ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_QUERY_FACILITY_LOCK;
    UINT16 sitId = SIT_GET_FACILITY_LOCK;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[4];

    pStrings[0] = (char*)FACILITY;
    pStrings[1] = (char*)PWD;
    pStrings[2] = (char*)SERVICE_CLASS;
    pStrings[3] = (char*)APPID;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId,  pStrings, 4 * sizeof(char*),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_facility_lock_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimGetFacilityLockRsp(sitId, sitToken, &rspDataLen, true);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getFacilityLockWithErrorResp) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_QUERY_FACILITY_LOCK;
    UINT16 sitId = SIT_GET_FACILITY_LOCK;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[4];

    pStrings[0] = (char*)FACILITY;
    pStrings[1] = (char*)PWD;
    pStrings[2] = (char*)SERVICE_CLASS;
    pStrings[3] = (char*)APPID;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId,  pStrings, 4 * sizeof(char*),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_facility_lock_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimGetFacilityLockRsp(sitId, sitToken, &rspDataLen, false);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getSimLockStatus) {

    LOGI("====== START getSimLockStatus ======");

    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_SIM_LOCK_STATUS;
    UINT16 sitId = SIT_OEM_GET_SIM_LOCK_INFO;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_get_sim_lock_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimGetSimLockInfoRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

// cannot verify below sequence
//
// RIL_REQUEST_SET_FACILITY_LOCK
//               |
//          VERIFY_PIN2
//               |
//       SET_FACILITY_LOCK
TEST_P(SitrilTestSim, setFacilityLockForFDN) {
    LOGI("===== setFacilityLockForFDN =====");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT sitToken = TOKEN_INVALID;
    char *pStrings[5];

    pStrings[0] = (char *)"FD";         // code
    pStrings[1] = (char *)"1";          // lock state
    pStrings[2] = (char *)"1234";       // password
    pStrings[3] = (char *)"7";          // service class
    pStrings[4] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    LOGI("===== Normal case =====");
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            pStrings, 5 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT16 sitId = SIT_VERIFY_SIM_PIN2;
    UINT32 sizeOfModemData = sizeof(sit_sim_verify_sim_pin2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData1 = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData1->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData1->GetToken();
    UINT32 rspDataLen1 = 0;
    UINT8* rspData1 = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen1, 3);
    psitRilTest->processRsp(RIL_REQUEST_ENTER_SIM_PIN2, rspData1, rspDataLen1);
    usleep(150*100);

    sitId = SIT_SET_FACILITY_LOCK;
    sizeOfModemData = sizeof(sit_sim_set_facility_lock_req);
    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData2 = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData2->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData2->GetToken();
    UINT32 rspDataLen2 = 0;
    UINT8* rspData2 = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen2, 3);
    psitRilTest->processRsp(RIL_REQUEST_SET_FACILITY_LOCK, rspData2, rspDataLen2);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    if (pModemData1 != nullptr) delete pModemData1;
    if (pModemData2 != nullptr) delete pModemData2;
    if (rspData1 != nullptr) free(rspData1);
    if (rspData2 != nullptr) free(rspData2);
}

TEST_P(SitrilTestSim, setFacilityLockForFDNButInvalidPassword) {
    LOGI("===== setFacilityLockForFDNButInvalidPassword =====");
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_VERIFY_SIM_PIN2;
    UINT sitToken = TOKEN_INVALID;
    char *pStrings[5];

    pStrings[0] = (char *)"FD";         // code
    pStrings[1] = (char *)"1";          // lock state
    pStrings[2] = (char *)"1234";       // password
    pStrings[3] = (char *)"7";          // service class
    pStrings[4] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    LOGI("========== Normal case ==========");
    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            pStrings, 5 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_verify_sim_pin2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData1 = new ModemData(txIpcRawData, sizeOfModemData);

    /* test RSP */
    sitToken = pModemData1->GetToken();
    UINT32 rspDataLen1 = 0;
    UINT8* rspData1 = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen1, 2, 3);
    psitRilTest->processRsp(RIL_REQUEST_SET_FACILITY_LOCK, rspData1, rspDataLen1);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    if (pModemData1 != nullptr) delete pModemData1;
    if (rspData1 != nullptr) free(rspData1);
}

TEST_P(SitrilTestSim, setFacilityLockWithNullReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_SET_FACILITY_LOCK;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_SET_FACILITY_LOCK,
            NULL, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    free(rspData);
}

#endif

#if TEST_CHANGE_PIN

TEST_P(SitrilTestSim, changePin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_CHG_SIM_PIN;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[3];

    pStrings[0] = (char *)"0000";       // old pin
    pStrings[1] = (char *)"1234";       // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_change_sim_pin_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_CHANGE_SIM_PIN, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, changeInvalidOldPin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_CHG_SIM_PIN;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[3];

    pStrings[0] = (char *)"00";       // old pin
    pStrings[1] = (char *)"1234";       // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, changeNullOldPin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_CHG_SIM_PIN;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[3];

    pStrings[0] = NULL;       // old pin
    pStrings[1] = (char *)"1234";       // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, changeInvalidNewPin) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_CHG_SIM_PIN;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[3];

    pStrings[0] = (char *)"0000";       // old pin
    pStrings[1] = (char *)"13248719867982457";      // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, changePin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_CHG_SIM_PIN2;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    char *pStrings[3];

    pStrings[0] = (char *)"0000";       // old pin
    pStrings[1] = (char *)"1234";      // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReq(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_change_sim_pin2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimPinPukResponse(sitId, sitToken, &rspDataLen, 3);
    psitRilTest->processRsp(RIL_REQUEST_CHANGE_SIM_PIN2, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, changeInvalidOldPin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_CHG_SIM_PIN2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[3];

    pStrings[0] = (char *)"1";       // old pin
    pStrings[1] = (char *)"1234";      // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, changeNullOldPin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_CHG_SIM_PIN2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[3];

    pStrings[0] = NULL;                 // old pin
    pStrings[1] = (char *)"1234";       // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

TEST_P(SitrilTestSim, changeInvalidNewPin2) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //UINT16 sitId = SIT_CHG_SIM_PIN2;
    //UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    //UINT32 rspDataLen = 0;
    char* pStrings[3];

    pStrings[0] = (char *)"1234";       // old pin
    pStrings[1] = (char *)"87";      // new pin
    pStrings[2] = (char *)"a0000000871002ffffffff89040300ff"; // aid

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), RIL_REQUEST_CHANGE_SIM_PIN2,
            pStrings, 3 * sizeof(char *), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    free(rspData);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////
// Indication //
#if TEST_INDICATION_FOR_SIM

//Indication test
TEST_P(SitrilTestSim, simStatusChanged) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_STATUS_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSimStatusChangedInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_EQ((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, uiccSubsStateChangedWithDeActivated) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildUiccSubsStateChangedInd(sitId, &rspDataLen, UICC_SUB_DEACTIVATED);
    psitRilTest->processInd(RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, uiccSubsStateChangedWithActivated) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildUiccSubsStateChangedInd(sitId, &rspDataLen, UICC_SUB_ACTIVATED);
    psitRilTest->processInd(RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, slotStatusChangedConvertLegacytoAidl) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    LOGI("====== START slotStatusChangedConvertLegacytoAidl ======");

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_SLOT_STATUS_CHANGED;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildGetSlotStatusValidLegacyInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ICC_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}


TEST_P(SitrilTestSim, slotStatusChangedWithValidData) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_SLOT_STATUS_CHANGED;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildGetSlotStatusValidInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ICC_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, slotStatusChangedWithInValidData) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_SLOT_STATUS_CHANGED;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildGetSlotStatusInvalidInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ICC_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, slotStatusChangedWithCardPresentButMemberInvalidData) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_SLOT_STATUS_CHANGED;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildGetSlotStatusOnlyAtrIccIdEidNullInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ICC_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, slotStatusChangedAidl) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    LOGI("====== START slotStatusChangedAidl ======");

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_SLOT_STATUS_CHANGED;
    // UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSlotStatusAidlInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_ICC_SLOT_STATUS, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

#endif

#if TEST_SECURE_ELEMENT
TEST_P(SitrilTestSim, SeOpenChannel) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SIM_OPEN_CHANNEL;
    UINT16 sitId = SIT_OPEN_SIM_CHANNEL_WITH_P2;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    SitSimOpenChannelReq params = {};

    string aid = "A00000015141434C";
    params.aid_len = aid.size();
    strncpy((char *)params.aid, aid.c_str(), aid.size());
    params.p2 = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&params,
            sizeof(SitSimOpenChannelReq), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_open_sim_channel_with_p2_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildSecureElementOpenLogicalChannelRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, SeTransmitApduLogical) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_LOGICAL;
    UINT16 sitId = SIT_TRANSMIT_SIM_APDU_CHANNEL;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    // 82cadf2000
    const UINT8 transmitComand[] = {
        0x02, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0xca, 0x00,
        0x00, 0x00, 0xdf, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    SitSimTransmitApduChannelReq *apdu = (SitSimTransmitApduChannelReq *) transmitComand;
    UINT32 sizeOfModemData = sizeof(sit_sim_transmit_sim_apdu_channel_req);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)apdu
            , sizeof(SitSimTransmitApduChannelReq), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildSecureElementTransmitApduLogicalRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, SeTransmitApduLogicalWith0x6C) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_LOGICAL;
    UINT16 sitId = SIT_TRANSMIT_SIM_APDU_CHANNEL;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData1 = nullptr;
    UINT8* rspData2 = nullptr;
    UINT32 rspDataLen = 0;

    // 82cadf2000
    const UINT8 transmitComand[] = {
        0x02, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0xca, 0x00,
        0x00, 0x00, 0xdf, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    SitSimTransmitApduChannelReq *apdu = (SitSimTransmitApduChannelReq *) transmitComand;
    UINT32 sizeOfModemData = sizeof(sit_sim_transmit_sim_apdu_channel_req);

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, (char *)apdu
            , sizeof(SitSimTransmitApduChannelReq), mPhoneIndex);
    usleep(150*100);

    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemData1 = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData1->GetMessageId(), sitId);

    sitToken = pModemData1->GetToken();
    rspData1 = buildSecureElementTransmitApduLogicalWith0x6cRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData1, rspDataLen);
    usleep(150*100);

    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemData2 = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData2->GetMessageId(), sitId);

    sitToken = pModemData2->GetToken();
    rspData2 = buildSecureElementTransmitApduLogicalRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData2, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);

    if (pModemData1 != nullptr) delete pModemData1;
    if (pModemData2 != nullptr) delete pModemData2;
    if (rspData1 != nullptr) free(rspData1);
    if (rspData2 != nullptr) free(rspData2);
}


TEST_P(SitrilTestSim, SeTransmitApduLogicalWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_LOGICAL;
    UINT16 sitId = SIT_TRANSMIT_SIM_APDU_CHANNEL;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    // 82cadf2000
    const UINT8 transmitComand[] = {
        0x02, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0xca, 0x00,
        0x00, 0x00, 0xdf, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    SitSimTransmitApduChannelReq *apdu = (SitSimTransmitApduChannelReq *) transmitComand;
    UINT32 sizeOfModemData = sizeof(sit_sim_transmit_sim_apdu_channel_req);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)apdu
            , sizeof(SitSimTransmitApduChannelReq), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);
    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, SecureElementReset) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_SIM_RESET;
    UINT16 sitId = SIT_SET_SIM_CARD_POWER;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_sim_card_power_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildSecureElementResetRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, GetUiccCardPresent) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_SIM_PRESENT;

    // PRESENT
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)4, rilRspDataLen);

    // ABSENT
    tSimStatusParams param = {};
    param.card_state = TEST_GET_SIM_CARD_STATE_ABSENT;
    ChangeSimStatus(&param);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    UINT16 sitId = SIT_GET_SIM_STATUS;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    UINT sitToken = pModemData->GetToken();
    rspData = buildGetSimStatusRsp(sitId, sitToken, &rspDataLen, &param);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)4, rilRspDataLen);

    delete pModemData;
    free(rspData);

    SetDefaultSimStatus();
}

#endif

#if TEST_OEM_DEPERSONALIZATION
TEST_P(SitrilTestSim, OemSimDepersonalization) {

    // Test OemSimDepersonalization
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_ICC_DEPERSONALIZATION;
    UINT16 sitId = SIT_VERIFY_NETWORK_LOCK;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

   /* Refer
    API:  public void supplyIccDepersonalization(int lockType, String pin, int subId, IMotoDepersoCB callback)
      Covert to DataWriter as below
       1. lockType = 17 => dw.writeInt(17)
       2. Pin len = 4  => dw.writeInt(pin.length())
       3. Pin value = "1234" => dw.writeBytes(pin.getBytes());
    */
    char value[12] = {
       0x11, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04
    };

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, value, sizeof(value), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_verify_network_lock_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = BuildSimDepersonalizationResponse(sitId, sitToken, &rspDataLen, 0, 3);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);

    delete pModemData;
    free(rspData);

}
#endif

#if TEST_BIP_OVER_WIFI

TEST_P(SitrilTestSim, WifiChannelStatusReport) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_CHANNEL_STATUS_REPORT;
    UINT16 sitId = SIT_SEND_STK_TCP_STATUS_EVENT;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse data;
    data.cid = 0x1;
    data.status = 0x01; // WIFI_CHANNEL_EVENT_RECEIVED

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&data,
            sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_channel_status_report_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, WifiStatusResponse) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_STATUS_RESPONSE;
    UINT16 sitId = SIT_SEND_STK_WIFI_STATUS_REPORT;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    const UINT8 data = 1; // MEDIA_CONNECTED

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&data, sizeof(UINT8),
            mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_status_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, OpenChannelResponse) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_OPEN_CHANNEL_RESPONSE;
    UINT16 sitId = SIT_SEND_STK_WIFI_OPEN_CHANNEL_RSP;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse openChannelData;
    openChannelData.cid = 0x01;     // Context Id assigned for the particular BIP session
    openChannelData.status = 0x00;  // SUCCESS

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&openChannelData
            , sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_close_channel_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, OpenChannelResponseWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_OPEN_CHANNEL_RESPONSE;
    UINT16 sitId = SIT_SEND_STK_WIFI_OPEN_CHANNEL_RSP;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse openChannelData;
    openChannelData.cid = 0x01;     // Context Id assigned for the particular BIP session
    openChannelData.status = 0x00;  // SUCCESS

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&openChannelData
            , sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_close_channel_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, SendDataResponse) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_SEND_DATA_RESPONSE;
    UINT16 sitId = SIT_STK_SEND_WIFI_DATA_RESPONSE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse sendDataResponseData;
    sendDataResponseData.cid = 0x01;     // Context Id assigned for the particular BIP session
    sendDataResponseData.status = 0x00; // SUCCESS

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&sendDataResponseData
            , sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_send_data_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, SendDataResponseWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_SEND_DATA_RESPONSE;
    UINT16 sitId = SIT_STK_SEND_WIFI_DATA_RESPONSE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse sendDataResponseData;
    sendDataResponseData.cid = 0x01;     // Context Id assigned for the particular BIP session
    sendDataResponseData.status = 0x00; // SUCCESS

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&sendDataResponseData
            , sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_send_data_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, ReceiveDataResponse) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_RECEIVE_DATA_RESPONSE;
    UINT16 sitId = SIT_STK_SEND_BIP_DATA_TO_SIM;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string receivedData = "1234567890123456";
    BipDataInfo bipDataInfo;
    bipDataInfo.cid = 0x01;    // Context Id of that particular BIP session
    bipDataInfo.length = receivedData.size();
    memcpy(bipDataInfo.data, receivedData.c_str(), receivedData.size());

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&bipDataInfo
            , sizeof(BipDataInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_receive_data_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, ReceiveDataResponseWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_RECEIVE_DATA_RESPONSE;
    UINT16 sitId = SIT_STK_SEND_BIP_DATA_TO_SIM;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string receivedData = "1234567890123456";
    BipDataInfo bipDataInfo;
    bipDataInfo.cid = 0x01;    // Context Id of that particular BIP session
    bipDataInfo.length = receivedData.size();
    memcpy(bipDataInfo.data, receivedData.c_str(), receivedData.size());

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&bipDataInfo
            , sizeof(BipDataInfo), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_receive_data_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, CloseChannelResponse) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_CLOSE_CHANNEL_RESPONSE;
    UINT16 sitId = SIT_STK_SEND_CLOSE_CHANNEL_RESPONSE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse closeChannelData;
    closeChannelData.cid = 0x01;     // Context Id assigned for the particular BIP session
    closeChannelData.status = 0x00; // SUCCESS

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&closeChannelData
            , sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_close_channel_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, CloseChannelResponseWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_OEM_WIFI_CLOSE_CHANNEL_RESPONSE;
    UINT16 sitId = SIT_STK_SEND_CLOSE_CHANNEL_RESPONSE;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    BipStatusResponse closeChannelData;
    closeChannelData.cid = 0x01;     // Context Id assigned for the particular BIP session
    closeChannelData.status = 0x00; // SUCCESS

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, (char *)&closeChannelData
            , sizeof(BipStatusResponse), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_stk_wifi_close_channel_rsp_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildNullDataFormatRsp(sitId, sitToken, &rspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, WiFiStatusRequest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_WIFI_STATUS_REQUEST;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildNullDataFormatInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_WIFI_STATUS_REQUEST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_EQ((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, OpenChannelRequest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_OPEN_CHANNEL_REQUEST;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildBowOpenChannelReqInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_WIFI_OPEN_CHANNEL_REQUEST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, SendDataRequest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_SEND_DATA_TO_WIFI;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildBowSendDataReqInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_WIFI_SEND_DATA_REQUEST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, ReceiveDataRequest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_RECEIVE_DATA_FROM_WIFI;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildNullDataFormatInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_WIFI_RECEIVE_DATA_REQUEST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_EQ((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, CloseChannelRequest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_STK_CLOSE_CHANNEL_REQUEST;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildNullDataFormatInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_WIFI_CLOSE_CHANNEL_REQUEST, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_EQ((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, getCarrierRestrictionsNotSupported) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_GET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4));
    UINT16 sitId = SIT_GET_CARRIER_RESTRICTIONS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetCarrierRestrictionRsp(sitId, sitToken, &rspDataLen, false);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}
#endif

#if TEST_MISC

TEST_P(SitrilTestSim, getAtr) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_GET_ATR;
    UINT16 sitId = SIT_GET_ATR;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetAtrRsp(sitId, sitToken, &rspDataLen, true, RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, getAtrWithError) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_GET_ATR;
    UINT16 sitId = SIT_GET_ATR;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetAtrRsp(sitId, sitToken, &rspDataLen, true, RIL_E_REQUEST_NOT_SUPPORTED);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}


TEST_P(SitrilTestSim, getAtrFailedCase) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_SIM_GET_ATR;
    UINT16 sitId = SIT_GET_ATR;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetAtrRsp(sitId, sitToken, &rspDataLen, false, RIL_E_SUCCESS);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, setCarrierRestrictions) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4));
    UINT16 sitId = SIT_SET_CARRIER_RESTRICTIONS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    unsigned int crLength = 0;

    RIL_CarrierRestrictions_V1_4 cr = {};
    RIL_Carrier *allowedCarriers = NULL;
    RIL_Carrier *excludedCarriers = NULL;
    crLength = sizeof(RIL_CarrierRestrictions_V1_4) - (2 * sizeof(RIL_Carrier *));

    cr.carriers.len_allowed_carriers = 1;
    allowedCarriers = (RIL_Carrier *) calloc(cr.carriers.len_allowed_carriers, sizeof(RIL_Carrier));
    cr.carriers.allowed_carriers = allowedCarriers;
    allowedCarriers[0].mcc = "123";
    allowedCarriers[0].mnc = "456";
    allowedCarriers[0].match_type = (RIL_CarrierMatchType) 0;
    allowedCarriers[0].match_data = "1111";
    crLength += 4 + 4 + sizeof(RIL_CarrierMatchType) + 5;

    cr.carriers.len_excluded_carriers = 1;
    excludedCarriers = (RIL_Carrier *) calloc(cr.carriers.len_excluded_carriers, sizeof(RIL_Carrier));
    cr.carriers.excluded_carriers = excludedCarriers;
    excludedCarriers[0].mcc = "789";
    excludedCarriers[0].mnc = "012";
    excludedCarriers[0].match_type = (RIL_CarrierMatchType) 0;
    excludedCarriers[0].match_data = "1324";
    crLength += 4 + 4 + sizeof(RIL_CarrierMatchType) + 5;

    cr.carriers.allowedCarriersPrioritized = true;
    cr.multiSimPolicy = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &cr, crLength, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_sim_set_carrier_restriction_req)
            + (sizeof(SIT_CARRIER_INFO) * (cr.carriers.len_allowed_carriers
            + cr.carriers.len_excluded_carriers))
            + sizeof(BYTE) + sizeof(BYTE);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildSetCarrierRestrictionRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(allowedCarriers);
    free(excludedCarriers);
    free(rspData);
}

TEST_P(SitrilTestSim, getCarrierRestrictions) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_GET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1, 4));
    UINT16 sitId = SIT_GET_CARRIER_RESTRICTIONS;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildGetCarrierRestrictionRsp(sitId, sitToken, &rspDataLen, true);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, simTrayStatusChanged) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_TRAY_STATUS_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSimTrayStatusChangedInd(sitId, &rspDataLen);
    psitRilTest->processInd(RIL_UNSOL_IND_SIM_TRAY_STATUS_CHANGED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    free(rspData);
}
#endif

#if TEST_UICC_PB

TEST_P(SitrilTestSim, simPbChangedWithPBMDisabled) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_PB_READY;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSimPbChangedInd(sitId, &rspDataLen, 2); // pbm disabled

    free(rspData);
}

TEST_P(SitrilTestSim, simPbChanged) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    UINT16 sitId = SIT_IND_SIM_PB_READY;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSimPbChangedInd(sitId, &rspDataLen, 1);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData = nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);

    free(rspData);
}

TEST_P(SitrilTestSim, getSimPbCapa) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // SEND SIT_GET_PB_STORAGE_INFO
    UINT16 sitId = SIT_GET_PB_STORAGE_INFO;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData1 = nullptr;
    UINT8* rspData2 = nullptr;
    UINT32 rspDataLen = 0;

    UINT32 sizeOfModemData = sizeof(sit_sim_pb_storage_info);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemDataForPbStorageInfo = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemDataForPbStorageInfo->GetMessageId(), sitId);

    sitToken = pModemDataForPbStorageInfo->GetToken();
    rspData1 = buildSimPbStorageInfo(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData1, rspDataLen);
    usleep(150*1000);   // 150ms

    // SEND SIT_GET_3G_PB_CAPA
    sitId = SIT_GET_3G_PB_CAPA;
    sizeOfModemData = sizeof(null_data_format);
    txIpcRawDataLen = 0;
    txIpcRawData = psitRilTest->getTxIpcRawData(1, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemDataFor3GPbCapa = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemDataFor3GPbCapa->GetMessageId(), sitId);

    sitToken = pModemDataFor3GPbCapa->GetToken();
    rspData2 = buildSimPb3GPbCapa(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData2, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);

    delete pModemDataForPbStorageInfo;
    delete pModemDataFor3GPbCapa;
    free(rspData1);
    free(rspData2);
}

// TODO: cannot solve "Sending modem data to test unit fails because it is not expected by test unit"
/*
TEST_P(SitrilTestSim, getSimPbRec) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS;

    // RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    //UINT32 rilRspDataLen = 0;
    //void *rilRspData= nullptr;
    //rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    //ASSERT_NE((UINT32)4, rilRspDataLen);

    // RIL_REQUEST_READ_PB_ENTRY
    //testRilCmdId = RIL_REQUEST_READ_PB_ENTRY;
    //psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);
    //ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT16 sitId = SIT_READ_PB_ENTRY;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    UINT32 sizeOfModemData = sizeof(sit_read_pb_entry_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    sitToken = pModemData->GetToken();
    rspData = buildReadSimPbRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    usleep(150*1000);

    psitRilTest->processInd(RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_RECORDS_RECEIVED, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    ASSERT_NE((UINT32)0, indLen);

    delete pModemData;
    free(rspData);
}
*/

TEST_P(SitrilTestSim, updateSimPb) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS;
    UINT16 sitId = SIT_UPDATE_PB_ENTRY;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    string name = "test";
    string num = "1234";

    int length = 0, pb_length = 0;
    RIL_UpdatePbEntry updateReq = {};
    updateReq.mode = 1; // PB_INSERT
    updateReq.type = PB_ADN_3G;
    updateReq.recordId = 0;
    updateReq.pb3g = new RIL_3GPb[2];
    length = sizeof(RIL_UpdatePbEntry) - sizeof(RIL_3GPb *);

    updateReq.pb3g[0].type3g = 0x01;
    updateReq.pb3g[0].data_type = GSM7BIT;
    updateReq.pb3g[0].data_len = name.size();
    updateReq.pb3g[0].data = new char[updateReq.pb3g[0].data_len]();
    strncpy(updateReq.pb3g[0].data, name.c_str(), updateReq.pb3g[0].data_len);
    pb_length += (updateReq.pb3g[0].data_len + (sizeof(int) * 3));

    updateReq.pb3g[1].type3g = 0x02;
    updateReq.pb3g[1].data_len = num.size();
    updateReq.pb3g[1].data_type = 0x80 | (1 << 4) | 1;
    updateReq.pb3g[1].data = new char[updateReq.pb3g[1].data_len]();
    strncpy(updateReq.pb3g[1].data, num.c_str(), updateReq.pb3g[1].data_len);
    pb_length += (updateReq.pb3g[1].data_len + (sizeof(int) * 3));

    updateReq.pb3gCnt = 2;
    updateReq.length = pb_length;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &updateReq, length, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_update_pb_entry_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildUpdateSimPbRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);

    delete[] updateReq.pb3g[0].data;
    delete[] updateReq.pb3g[1].data;
    delete[] updateReq.pb3g;
    delete pModemData;
    free(rspData);
}

TEST_P(SitrilTestSim, deleteSimPb) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testRilCmdId = RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS;
    UINT16 sitId = SIT_UPDATE_PB_ENTRY;
    UINT sitToken = TOKEN_INVALID;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    RIL_UpdatePbEntry updateReq = {};
    updateReq.mode = 2; // PB_DELETE
    updateReq.type = PB_ADN_3G;
    updateReq.recordId = 1;
    updateReq.length = 0;
    updateReq.pb3g = NULL;
    int length = sizeof(RIL_UpdatePbEntry) - sizeof(RIL_3GPb *);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &updateReq, length, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_update_pb_entry_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    EXPECT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = pModemData->GetToken();
    rspData = buildUpdateSimPbRsp(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);

    delete pModemData;
    free(rspData);
}
#endif
