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

#include "sitrilTestNetwork.h"
#include "testUtil.h"
#include <string>

#define TEST_OP_PLMN "460010"
#define TEST_OP_LONG_NAME "PLMN 5G"
#define TEST_OP_SHORT_NAME "PLMN 5G"

#define NET_SIM_EF_PNN    (0x6fc5)
#define NET_SIM_EF_OPL    (0x6fc6)
#define NET_SIM_EF_OPL5G  (0x4f08)

/////////////////////////////////////////////////////////////////////////////////////////////////
// SET request|response //
static UINT8* buildSetManualRatMode(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_misc_set_manual_band_mode_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_misc_set_manual_band_mode_rsp *rsp = (sit_misc_set_manual_band_mode_rsp *)rspData;
    rsp->cause = (BYTE)1;

    return rspData;
}

static UINT8* buildSetFrequencyLock(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_net_set_freq_lock_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_net_set_freq_lock_rsp *rsp = (sit_net_set_freq_lock_rsp *)rspData;
    rsp->result = (BYTE)1;

    return rspData;
}

static UINT8* buildSetFemtoCellSrchRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_net_set_micro_cell_search_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_net_set_micro_cell_search_rsp *rsp = (sit_net_set_micro_cell_search_rsp *)rspData;
    rsp->srch_result = (BYTE)1;
    memcpy(rsp->plmn, "45005", 5);

    return rspData;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// GET request|response
static UINT8* buildGetOperatorRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)(sizeof(RCM_HEADER) + sizeof(sit_net_operator_v1_1));
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_net_operator_v1_1 *rsp = (sit_net_operator_v1_1*)(rspData + sizeof(RCM_HEADER));
    memcpy(rsp->plmn, TEST_OP_PLMN, MAX_PLMN_LEN);
    memcpy(rsp->long_name, TEST_OP_LONG_NAME, sizeof(TEST_OP_LONG_NAME));
    memcpy(rsp->short_name, TEST_OP_SHORT_NAME, sizeof(TEST_OP_SHORT_NAME));
    rsp->reg_state = 1;    //OPERATOR_REG_HOME
    rsp->lac = 1;

    return rspData;
}

static UINT8* buildGetVoiceRegistrationRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_cs_reg_state_v1_1)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_cs_reg_state_v1_1 *rsp = (sit_cs_reg_state_v1_1*)(rspData + sizeof(RCM_HEADER));
    // Todo
    rsp->reg_state = SIT_NET_REG_STATE_REGISTERED;
    rsp->rej_cause = SIT_NET_REJ_CAUSE_GENERAL;

    return rspData;
}

static UINT8* buildGetDataRegistrationRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_ps_reg_state_v1_1)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_ps_reg_state_v1_1 *rsp = (sit_ps_reg_state_v1_1*)(rspData + sizeof(RCM_HEADER));
    // Todo
    rsp->reg_state = SIT_NET_REG_STATE_REGISTERED;
    rsp->rej_cause = SIT_NET_REJ_CAUSE_GENERAL;

    return rspData;
}

static UINT8* buildQueryNetworkSelectionModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_network_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_network_mode_rsp *rsp = (sit_net_get_network_mode_rsp*)rspData;
    rsp->network_mode = SIT_NET_NETWORK_MODE_AUTOMATIC;

    return rspData;
}

static UINT8* buildQueryAvailableNetworksRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_available_networks_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_available_networks_rsp *rsp = (sit_net_get_available_networks_rsp*)rspData;
    rsp->network_info_num = 1;
    rsp->network_info[0].rat = 14;
    rsp->network_info[0].plmn_status = 1;
    memcpy(rsp->network_info[0].plmn, "45005", 5);

    return rspData;
}

static UINT8* buildQueryAvailableBandModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_band_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_band_mode_rsp *rsp = (sit_net_get_band_mode_rsp*)rspData;
    rsp->band_info_num = 2;
    rsp->band[0] = 100;
    rsp->band[1] = 100;

    return rspData;
}

static UINT8* buildGetPrefNetTypeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_pref_network_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_pref_network_rsp *rsp = (sit_net_get_pref_network_rsp*)rspData;
    rsp->pref_net_type = PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA;

    return rspData;
}

static UINT8* buildGetCellInfoListRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    BYTE data[57] =
    { 0x02, 0x01, 0x01, 0x34, 0x35, 0x30, 0x30, 0x35,
      0x23, 0x27, 0xd4, 0x12, 0x00, 0xd3, 0x00, 0x00,
      0x00, 0x33, 0x27, 0x00, 0x00, 0xc4, 0x09, 0x00,
      0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x3f, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00,
      0x00, 0x03, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
      0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00 };

    *datalen = (UINT16)sizeof(sit_net_get_cell_info_list_rsp) + sizeof(data);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_cell_info_list_rsp *rsp = (sit_net_get_cell_info_list_rsp*)rspData;
    rsp->cell_info_num = 1;
    memcpy(rsp->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildGetRadioCapabilityRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_rc_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_rc_rsp *rsp = (sit_net_get_rc_rsp*)rspData;
    rsp->rc_raf = RAF_CP_LTE;

    return rspData;
}

static UINT8* buildGetBarringInfoRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_barring_infos_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_barring_infos_rsp *rsp = (sit_net_get_barring_infos_rsp*)rspData;
    // Todo
    rsp->barring_info_data[0] = 0;

    return rspData;
}

static UINT8* buildGetDuplexModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_duplex_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_duplex_mode_rsp *rsp = (sit_net_get_duplex_mode_rsp*)rspData;
    rsp->duplex_mode_4g = SIT_NET_DUPLEX_MODE_TDD;
    rsp->duplex_mode_3g = SIT_NET_DUPLEX_MODE_TDD;

    return rspData;
}

static UINT8* buildGetCdmaHybridModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_cdma_hybrid_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_cdma_hybrid_mode_rsp *rsp = (sit_net_get_cdma_hybrid_mode_rsp*)rspData;
    rsp->hybrid_mode = HYBRID_MODE_EHRPD_ONLY;

    return rspData;
}

static UINT8* buildCdmaQueryRoamingPreferrenceRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_query_cdma_roaming_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_query_cdma_roaming_rsp *rsp = (sit_net_query_cdma_roaming_rsp*)rspData;
    rsp->cdma_roaming_type = SIT_CDMA_RM_HOME;

    return rspData;
}

static UINT8* buildGetNrModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_nr_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_nr_mode_rsp *rsp = (sit_net_get_nr_mode_rsp*)rspData;
    rsp->mode = 1;

    return rspData;
}

static UINT8* buildGetEndcModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_endc_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_endc_mode_rsp *rsp = (sit_net_get_endc_mode_rsp*)rspData;
    rsp->mode = 1;

    return rspData;
}

static UINT8* buildGetImsSupportServiceRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_ps_reg_state_v1_1) + sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_ps_reg_state_v1_1 *rsp = (sit_ps_reg_state_v1_1 *)(rspData + sizeof(RCM_HEADER));
    rsp->ims_vops = SERVICE_AVAILABLE;
    rsp->emc_service = SERVICE_AVAILABLE;
    rsp->rat = SIT_RAT_TYPE_LTE;
    rsp->reg_state = SIT_NET_REG_STATE_REGISTERED;

    return rspData;
}

static UINT8* buildGetManualRatModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_manual_rat_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_manual_rat_mode_rsp *rsp = (sit_net_get_manual_rat_mode_rsp *)rspData;
    rsp->manual_rat_mode_set = 1;
    rsp->rat = SIT_RAT_TYPE_LTE;

    return rspData;
}

static UINT8* buildGetFrequencyLockRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_freq_lock_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_freq_lock_rsp *rsp = (sit_net_get_freq_lock_rsp *)rspData;
    rsp->freq_mode_set = 1;
    rsp->rat = SIT_RAT_TYPE_LTE;
    rsp->lte_pci = 3;
    rsp->lte_earfcn = 4;
    rsp->gsm_arfcn = 5;
    rsp->wcdma_psc = 6;
    rsp->wcdma_uarfcn = 7;

    return rspData;
}

static UINT8* buildGetNrSilenceModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_get_nr_silence_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_get_nr_silence_mode_rsp *rsp = (sit_get_nr_silence_mode_rsp *)rspData;
    rsp->nsa_mode = 1;
    rsp->sa_mode = 0;

    return rspData;
}

static UINT8* buildGetFrequencyInfoRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_frequency_info_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_frequency_info_rsp *rsp = (sit_net_get_frequency_info_rsp *)rspData;
    rsp->primary_rat = 1;
    rsp->primary_band = 2;
    rsp->primary_frequency = 3;
    rsp->secondary_rat = 4;
    rsp->secondary_band = 5;
    rsp->secondary_frequency = 6;

    return rspData;
}

static UINT8* buildGetAllowDataStateRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_net_get_ps_service_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_net_get_ps_service_rsp *rsp = (sit_net_get_ps_service_rsp *)rspData;
    rsp->state = 1;

    return rspData;
}

static UINT8* buildGetCdmaQueryPreferredVoicePrivacyModeRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_call_get_preferred_voice_privacy_mode_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_call_get_preferred_voice_privacy_mode_rsp *rsp = (sit_call_get_preferred_voice_privacy_mode_rsp *)rspData;
    rsp->status = 1;

    return rspData;
}

static UINT8* buildGetPreferredCallCapabilityRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_misc_get_preferred_call_capability_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_misc_get_preferred_call_capability_rsp *rsp = (sit_misc_get_preferred_call_capability_rsp *)rspData;
    rsp->mode = 1;

    return rspData;
}

static UINT8* buildCancelAvailableNetworkstRsp(INT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(null_data_format);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    return rspData;
}


////////////////////////////////////////////////////////////////////////////////////////
// Indication //
static UINT8* buildNetworkStateChangeInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_network_state_changed_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    // sit_net_network_state_changed_ind *ind = (sit_net_network_state_changed_ind*)rspData;

    return rspData;
}

static UINT8* buildRrcInfoInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_rrc_info_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_net_rrc_info_ind *ind = (sit_net_rrc_info_ind*)rspData;
    ind->rat = SIT_RAT_TYPE_LTE;
    ind->state = RRC_CONNECT;

    return rspData;
}

static UINT8* buildEmergencyActInfoInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_emergency_act_info_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_net_emergency_act_info_ind *ind = (sit_net_emergency_act_info_ind *)rspData;
    ind->rat = (BYTE)RADIO_TECH_LTE;
    ind->act_status = (BYTE)EMERGENCY_CALL_AVAILABLE;

    return rspData;
}

static UINT8* buildFrequencyInfoInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_frequency_info_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_net_frequency_info_ind *ind = (sit_net_frequency_info_ind *)rspData;
    ind->primary_rat = 1;
    ind->primary_band = 2;
    ind->primary_frequency = 3;
    ind->secondary_rat = 4;
    ind->secondary_band = 5;
    ind->secondary_frequency = 6;

    return rspData;
}

static UINT8* buildSimDataInfoInd(INT16 sitId, UINT32 *datalen, int efValue) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_sim_file_data_info_ind);
    const BYTE *pData = nullptr;

    // sample PNN data
    // PLMN 5G, ABCD, CCCDDD
    BYTE rawDataPnn[73] = {
        0x02, 0x00, 0x4f, 0x02, 0x49, 0x00, 0x00, 0x00, 0xc5, 0x6f, 0x14, 0x00, 0x03, 0x43, 0x08, 0x86,
        0x50, 0x66, 0xd3, 0x09, 0xaa, 0x1d, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0x43, 0x05, 0x86, 0x41, 0xe1, 0x90, 0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x43, 0x07, 0x86, 0xc3, 0xe1, 0x90, 0x48, 0x24, 0x02, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };

    // sample OPL record
    BYTE rawDataOpl[77] = { 0x02, 0x00, 0x4f, 0x02, 0x4d, 0x00, 0x00, 0x00, 0xc6, 0x6f, 0x08, 0x00, 0x08, 0x64, 0xf0, 0x10,
        0x00, 0x00, 0xff, 0xfe, 0x01, 0x64, 0xf0, 0x90, 0x00, 0x00, 0xff, 0xfe, 0x02, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    };

    if (NET_SIM_EF_PNN == efValue) { len = 73; pData = rawDataPnn; }
    else if (NET_SIM_EF_OPL == efValue) { len = 77; pData = rawDataOpl; }

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    if (pData != nullptr) memcpy(rspData, pData, *datalen);
    else {
        sit_sim_file_data_info_ind *ind = (sit_sim_file_data_info_ind *)rspData;
        // Todo
        ind->sim_file_id = 1;
        ind->record_len = 0;
        ind->num_of_records = 0;
    }

    return rspData;
}

static UINT8* buildCellInfoInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_cell_info_list_ind);

    BYTE data[57] =
    { 0x02, 0x01, 0x01, 0x34, 0x35, 0x30, 0x30, 0x35,
      0x23, 0x27, 0xd4, 0x12, 0x00, 0xd3, 0x00, 0x00,
      0x00, 0x33, 0x27, 0x00, 0x00, 0xc4, 0x09, 0x00,
      0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x3f, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00,
      0x00, 0x03, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
      0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_cell_info_list_ind *ind = (sit_net_cell_info_list_ind *)rspData;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildScanningNetworksGsmInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_scanning_network_ind);

    BYTE data[57] = {
        0x00, 0x01, 0x00, 0x34, 0x35, 0x30, 0x30, 0x35,
        0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_scanning_network_ind *ind = (sit_net_scanning_network_ind *)rspData;
    ind->scan_status = (BYTE)SIT_NET_SCAN_STATUS_COMPLETE;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildScanningNetworksCdmaInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_scanning_network_ind);

    BYTE data[57] = {
        0x01, 0x01, 0x00, 0x34, 0x35, 0x30, 0x30, 0x35,
        0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_scanning_network_ind *ind = (sit_net_scanning_network_ind *)rspData;
    ind->scan_status = (BYTE)SIT_NET_SCAN_STATUS_COMPLETE;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildScanningNetworksLteInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_scanning_network_ind);

    BYTE data[57] = {
        0x02, 0x01, 0x00, 0x34, 0x35, 0x30, 0x30, 0x35,
        0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_scanning_network_ind *ind = (sit_net_scanning_network_ind *)rspData;
    ind->scan_status = (BYTE)SIT_NET_SCAN_STATUS_COMPLETE;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildScanningNetworksWcdmaInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_scanning_network_ind);

    BYTE data[57] = {
        0x03, 0x01, 0x00, 0x34, 0x35, 0x30, 0x30, 0x35,
        0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_scanning_network_ind *ind = (sit_net_scanning_network_ind *)rspData;
    ind->scan_status = (BYTE)SIT_NET_SCAN_STATUS_COMPLETE;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildScanningNetworksTdscdmaInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_scanning_network_ind);

    BYTE data[57] = {
        0x04, 0x01, 0x00, 0x34, 0x35, 0x30, 0x30, 0x35,
        0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_scanning_network_ind *ind = (sit_net_scanning_network_ind *)rspData;
    ind->scan_status = (BYTE)SIT_NET_SCAN_STATUS_COMPLETE;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildScanningNetworksNrInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_scanning_network_ind);

    BYTE data[57] = {
        0x05, 0x01, 0x00, 0x34, 0x35, 0x30, 0x30, 0x35,
        0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    rspData = allocIndModemMem(len + sizeof(data), sitId);
    *datalen = len + sizeof(data);

    sit_net_scanning_network_ind *ind = (sit_net_scanning_network_ind *)rspData;
    ind->scan_status = (BYTE)SIT_NET_SCAN_STATUS_COMPLETE;
    ind->cell_info_num = 1;
    memcpy(ind->cell_info_list, data, 57);

    return rspData;
}

static UINT8* buildB2b1ConfigInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(RCM_IND_HEADER) + sizeof(BYTE);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    return rspData;
}

static UINT8* buildAcBarringInfoInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_ac_barring_info_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    BYTE data[33] = {
        0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    sit_net_ac_barring_info_ind *ind = (sit_net_ac_barring_info_ind *)rspData;
    memcpy(&(ind->for_emc), data, 33);

    return rspData;
}

static UINT8* buildPhysicalChannelConfigInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_physical_channel_config_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    BYTE data[52] = {
        0x01, 0x10, 0x27, 0x00, 0x00, 0x0e, 0x01, 0xc4,
        0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xd3, 0x00, 0x00, 0x00 };

    sit_net_physical_channel_config_ind *ind = (sit_net_physical_channel_config_ind *)rspData;
    ind->config_len = 1;
    memcpy(ind->configs, data, 52);

    return rspData;
}

#if 0
// Path removed
static UINT8* buildScgBearerAllocationInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_sgc_bearer_allocation_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_net_sgc_bearer_allocation_ind *ind = (sit_net_sgc_bearer_allocation_ind *)rspData;
    ind->rat = (BYTE)RADIO_TECH_NR;
    ind->scg_status = (BYTE)PRIMARY_SERVING;

    return rspData;
}
#endif

static UINT8* buildRegFailedInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_reg_failed_ind);

    // test with an invalid cell identity type(none)
    len += 1;

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_net_reg_failed_ind *ind = (sit_net_reg_failed_ind *)rspData;
    memcpy(ind->plmn, TEST_OP_PLMN, MAX_PLMN_LEN);
    ind->service_domain = (BYTE)SERVICE_DOMAIN_PS;
    ind->rej_cause = (BYTE)1;
    ind->rej_cause_additional = (BYTE)2;

    // cell identity can be variable according to
    // the version of cell identity struct.
    // test with an invalid cell identity type(none)

    return rspData;
}

static UINT8* buildBarringInfoChangedInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_net_barring_chagned_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    BYTE data[33] = {
        0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00 };

    sit_net_barring_chagned_ind *ind = (sit_net_barring_chagned_ind *)rspData;
    memcpy(ind->barring_info_data, data, 33);

    return rspData;
}

static UINT8* buildCdmaInfoRecInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_cdma_information_records);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_cdma_information_records *ind = (sit_cdma_information_records *)rspData;
    ind->num_of_info_recs = 1;
    ind->info_rec[0].cdma_info_name = RIL_CDMA_SIGNAL_INFO_REC;
    ind->info_rec[0].cdma_info.signal.is_present = (BYTE)2;
    ind->info_rec[0].cdma_info.signal.signal_type = (BYTE)3;
    ind->info_rec[0].cdma_info.signal.alert_pitch = (BYTE)4;
    ind->info_rec[0].cdma_info.signal.signal = (BYTE)5;

    return rspData;
}

static UINT8* buildCdmaCallWaitingInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ss_cdma_call_waiting_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_ss_cdma_call_waiting_ind *ind = (sit_ss_cdma_call_waiting_ind *)rspData;
    ind->num_len = 11;
    memcpy(ind->number, "01012345678", 11);
    ind->number_presentation = 1;
    ind->name_len = 4;
    memcpy(ind->name, "Kano", 4);
    ind->number_type = 2;
    ind->number_plan = 3;

    return rspData;
}

static UINT8* buildCdmaOtaProvisionStatusInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_ota_provision_status_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_ota_provision_status_ind *ind = (sit_ota_provision_status_ind *)rspData;
    ind->otaType = (BYTE)SIT_OTA_TYPE_OTASP;
    ind->otaStatus = (BYTE)SIT_OTASP_STATUS_OK_AKEYEX;

    return rspData;
}

static UINT8* buildLceDataRecvInd(INT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_lce_data_ind);

    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_lce_data_ind *ind = (sit_pdp_lce_data_ind *)rspData;
    ind->dl_lc = 1;
    ind->ul_lc = 2;
    ind->conf_lvl = (BYTE)3;
    ind->is_suspended = (BYTE)4;

    return rspData;
}

// sitrilTestNetwork class
void SitrilTestNetwork::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestNetwork::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) { delete mpModemData; mpModemData = nullptr; }
    if (mRspData) { free(mRspData); mRspData = nullptr; }
    mRspDataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
// SET request|response //
TEST_P(SitrilTestNetwork, setNetworkSelectionAutomatic) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC;
    UINT16 sitId = SIT_SET_NTW_MODE_AUTO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_REG_HOME, RIL_CARDSTATE_PRESENT);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_network_mode_auto_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setNetworkSelectionManual) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL;
    UINT16 sitId = SIT_SET_NTW_MODE_MANUAL;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const char *p_cur[2] = { "45005", "1" };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, p_cur, sizeof(char *) * 2, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_metwork_mode_manual_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setNetworkSelectionManualWithRat) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_WITH_RAT;
    UINT16 sitId = SIT_SET_NTW_MODE_MANUAL;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const char *p_cur[2] = { "45005", "1" };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, p_cur, sizeof(char *) * 2, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_metwork_mode_manual_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setBandMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_BAND_MODE;
    UINT16 sitId = SIT_SET_BAND_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_band_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setPreferredNetworkType) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE;
    UINT16 sitId = SIT_SET_PREFERRED_NTW_TYPE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_pref_network_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setUnsolCellInfoListRate) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE;
    UINT16 sitId = SIT_SET_CELL_INFO_LIST_REPORT_RATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_cell_info_list_report_rate_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setAllowData) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_ALLOW_DATA;
    UINT16 sitId = SIT_SET_PS_SERVICE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_ps_service_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setRadioCapability) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_RADIO_CAPABILITY;
    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // Temp: There is no IPC
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setDsNetworkType) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_DS_NETWORK_TYPE;
    UINT16 sitId = SIT_SET_DS_NTW_TYPE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = PREF_NET_TYPE_LTE_GSM_WCDMA;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_ds_network_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setDuplexMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_DUPLEX_MODE;
    UINT16 sitId = SIT_SET_DUPLEX_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_REG_HOME, RIL_CARDSTATE_PRESENT);

    unsigned int value = DUPLEX_MODE_LTG;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_duplex_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setQueryAvailableEmergencyCallStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_QUERY_AVAILABLE_EMERGENCY_CALL_STATUS;
    UINT16 sitId = SIT_SET_EMERGENCY_CALL_STATUS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_emergency_call_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setEmergencyCallStatus) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_EMERGENCY_CALL_STATUS;
    UINT16 sitId = SIT_SET_EMERGENCY_CALL_STATUS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value[2] = { EMERGENCY_CALL_STATUS_START, RADIO_TECH_LTE };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_emergency_call_status_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setFemtoCellSrch) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_FEMTO_CELL_SRCH;
    UINT16 sitId = SIT_SET_MICRO_CELL_SEARCH;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    struct RespHookImpl : public SitrilTest::RilResponseHookBase {
        using SitrilTest::RilResponseHookBase::RilResponseHookBase;

        void onRequestComplete(const void *data, size_t datalen) {
            char **p_cur = (char **) (data);
            ASSERT_NE((UINT32)0, datalen);
            int numStrings = datalen/sizeof(char *);
            EXPECT_EQ(2, numStrings);
            if (p_cur[0] != nullptr) EXPECT_STREQ("1", p_cur[0]);
            if (p_cur[1] != nullptr) EXPECT_STREQ("45005", p_cur[1]);
        }
    } hook(*psitRilTest);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_micro_cell_search_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildSetFemtoCellSrchRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);
}

TEST_P(SitrilTestNetwork, setDualNetworkAndAllowData) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA;
    UINT16 sitId = SIT_SET_DUAL_NTW_AND_PS_TYPE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value[4] = { PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, PREF_NET_TYPE_LTE_GSM_WCDMA, 1, 0 };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_dual_network_and_allow_data_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaHybridMode_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_CDMA_HYBRID_MODE;
    UINT16 sitId = SIT_SET_CDMA_HYBRID_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = HYBRID_MODE_1X_HRPD;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_cdma_hybrid_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaHybridMode_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_CDMA_HYBRID_MODE;
    UINT16 sitId = SIT_SET_CDMA_HYBRID_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = HYBRID_MODE_1X_HRPD;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_cdma_hybrid_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaRoamingPreferrence_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE;
    UINT16 sitId = SIT_SET_CDMA_ROAMING_PREFERENCE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = CDMA_ROAMING_AFFILIATED_NETWORKS;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_cdma_roaming_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaRoamingPreferrence_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE;
    UINT16 sitId = SIT_SET_CDMA_ROAMING_PREFERENCE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = CDMA_ROAMING_AFFILIATED_NETWORKS;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_cdma_roaming_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setNrMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_NR_MODE;
    UINT16 sitId = SIT_SET_NR_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_nr_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setEndcMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_ENDC_MODE;
    UINT16 sitId = SIT_SET_ENDC_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_endc_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setPsAttachDetach) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_PS_ATTACH_DETACH;
    UINT16 sitId = SIT_SET_PS_SERVICE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value = 0;
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, &value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_ps_service_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}


TEST_P(SitrilTestNetwork, setManualRatMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_MANUAL_RAT_MODE;
    UINT16 sitId = SIT_SET_MANUAL_RAT_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value[2] = { 1, 2 };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_manual_rat_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildSetManualRatMode(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int num = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, num);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, setFrequencyLock) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_FREQUENCY_LOCK;
    UINT16 sitId = SIT_SET_FREQUENCY_LOCK;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value[7] = { 1, 2, 3, 4, 5, 6, 7 };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_freq_lock_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildSetFrequencyLock(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int num = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, num);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, setNrSilenceMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_NR_SILENCE_MODE;
    UINT16 sitId = SIT_SET_NR_SILENCE_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    unsigned int value[2] = { 1, 2 };
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, value, sizeof(value), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_set_nr_silence_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setSystemSelectionChannels) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, HAL_VERSION_CODE(1,5));
    UINT16 sitId = SIT_SET_SYSTEM_SELECTION_CHANNELS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RIL_SysSelChannelRequest_V1_5 sscr;
    sscr.isSpecifyChannels = true;
    sscr.specifiers_length = 1;
    sscr.specifiers[0].radio_access_network = RIL_RAN_EUTRAN;
    sscr.specifiers[0].bands_length = 1;
    sscr.specifiers[0].bands.eutran_bands[0] = EUTRAN_BAND_1;
    sscr.specifiers[0].channels_length = 1;
    sscr.specifiers[0].channels[0] = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &sscr, sizeof(sscr), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData =
        sizeof(sit_net_set_system_selection_channels_req) - (sizeof(sit_net_radio_access_specifier) * 3);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setStartNetworkScan) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,5));
    UINT16 sitId = SIT_START_SCANNING_NETWORKS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    RIL_NetworkScanRequest_V1_5 data;
    memset(&data, 0, sizeof(RIL_NetworkScanRequest_V1_5));
    data.type = RIL_ONE_SHOT;
    data.interval = 5;
    data.specifiers_length = 3;
    data.maxSearchTime = 300;
    data.incrementalResults = 1;
    data.incrementalResultsPeriodicity = 3;
    data.numOfMccMncs = 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &data, sizeof(data), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_start_scanning_network)
        - ((sizeof(SIT_NET_SCAN_PLMN_ID) * MAX_NETWORK_PLMN_IDS) + (sizeof(SIT_NET_SCAN_SPECIFIER) * 5));
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setStopNetworkScan) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_STOP_NETWORK_SCAN;
    UINT16 sitId = SIT_START_SCANNING_NETWORKS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_start_scanning_network) - MAX_NETWORK_SCAN_DATA;
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaSetPreferredVoicePrivacyMode_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE;
    UINT16 sitId = SIT_SET_CDMA_VOICE_PRIVACY_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int data = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &data, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_set_cdma_voice_privacy_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaSetPreferredVoicePrivacyMode_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE;
    UINT16 sitId = SIT_SET_CDMA_VOICE_PRIVACY_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int data = 1;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &data, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_set_cdma_voice_privacy_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaBurstDtmf_sim_absent) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_BURST_DTMF;
    // Not used due to current card state
    //UINT16 sitId = SIT_CDMA_BURST_DTMF;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //Card state shall be present to process
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_NOT_REG_AND_NOT_SEARCHING, RIL_CARDSTATE_ABSENT);

    const char *data[3] = {"1", "2", "3"};
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, data, sizeof(char *) * 3, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);

    //reset Card state.
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_NOT_REG_AND_NOT_SEARCHING, RIL_CARDSTATE_ABSENT);
}

TEST_P(SitrilTestNetwork, setCdmaBurstDtmf_sim_present) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_BURST_DTMF;
    // Not used due to current card state
    UINT16 sitId = SIT_CDMA_BURST_DTMF;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //Card state shall be present to process
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_NOT_REG_AND_NOT_SEARCHING, RIL_CARDSTATE_PRESENT);

    const char *data[3] = {"1", "2", "3"};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, data, sizeof(char *) * 3, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_cdma_burst_dtmf_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);

    //reset Card state.
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_NOT_REG_AND_NOT_SEARCHING, RIL_CARDSTATE_ABSENT);
}

TEST_P(SitrilTestNetwork, setCdmaBurstDtmf_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_BURST_DTMF;
    // Not used due to current card state
    UINT16 sitId = SIT_CDMA_BURST_DTMF;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //Card state shall be present to process
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_NOT_REG_AND_NOT_SEARCHING, RIL_CARDSTATE_PRESENT);

    const char *data[3] = {"1", "2", "3"};
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, data, sizeof(char *) * 3, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_cdma_burst_dtmf_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);

    //reset Card state.
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_NOT_REG_AND_NOT_SEARCHING, RIL_CARDSTATE_ABSENT);
}

TEST_P(SitrilTestNetwork, setCdmaFlash_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_FLASH;
    UINT16 sitId = SIT_SET_CDMA_FLASH_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const char *data = "1";
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &data, sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_set_cdma_flash_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setCdmaFlash_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_FLASH;
    UINT16 sitId = SIT_SET_CDMA_FLASH_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const char *data = "1";
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &data, sizeof(char *), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_ss_set_cdma_flash_info_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setPreferredCallCapability) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_PREFERRED_CALL_CAPABILITY;
    UINT16 sitId = SIT_SET_PREFERRED_CALL_CAPABILITY;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    char data = '1';
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &data, sizeof(char), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_set_preferred_call_capability_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildEmptyRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}


////////////////////////////////////////////////////////////////////////////////////////
// GET request|response
TEST_P(SitrilTestNetwork, getOperator) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OPERATOR;
    UINT16 sitId = SIT_GET_OPERATOR;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //deliver PNN info
    mRspData = buildSimDataInfoInd(SIT_IND_SIM_DATA_INFO, &mRspDataLen, NET_SIM_EF_PNN);
    psitRilTest->processInd(0, mRspData, mRspDataLen);
    mRspDataLen = 0; free(mRspData);
    //deliver OPL info
    mRspData = buildSimDataInfoInd(SIT_IND_SIM_DATA_INFO, &mRspDataLen, NET_SIM_EF_OPL);
    psitRilTest->processInd(0, mRspData, mRspDataLen);
    mRspDataLen = 0; free(mRspData);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    struct RespHookImpl : public SitrilTest::RilResponseHookBase {
        using SitrilTest::RilResponseHookBase::RilResponseHookBase;

        void onRequestComplete(const void *data, size_t datalen) {
            char **p_cur = (char **) (data);
            ASSERT_NE((UINT32)0, datalen);
            int numStrings = datalen/sizeof(char *);
            EXPECT_EQ(6, numStrings);
            if (p_cur[0] != nullptr) EXPECT_STREQ(TEST_OP_LONG_NAME, p_cur[0]);
            if (p_cur[1] != nullptr) EXPECT_STREQ(TEST_OP_SHORT_NAME, p_cur[1]);
            if (p_cur[2] != nullptr) EXPECT_STREQ(TEST_OP_PLMN, p_cur[2]);
        }
    } hook(*psitRilTest);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_operator_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetOperatorRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);
}

TEST_P(SitrilTestNetwork, getVoiceRegistration) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_VOICE_REGISTRATION_STATE;
    UINT16 sitId = SIT_GET_CS_REG_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_cs_reg_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetVoiceRegistrationRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(RIL_RegStateResult);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    RIL_RegStateResult *data = (RIL_RegStateResult *)rilRspData;
    // Todo
    EXPECT_EQ(RIL_REG_HOME, data->regState);
    EXPECT_EQ(REG_FAIL_CAUSE_NONE, data->reasonForDenial);
}

TEST_P(SitrilTestNetwork, getDataRegistration) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_DATA_REGISTRATION_STATE;
    UINT16 sitId = SIT_GET_PS_REG_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_ps_reg_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetDataRegistrationRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(RIL_RegStateResult);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    RIL_RegStateResult *data = (RIL_RegStateResult *)rilRspData;
    // Todo
    EXPECT_EQ(RIL_REG_HOME , data->regState);
    EXPECT_EQ(REG_FAIL_CAUSE_NONE, data->reasonForDenial);
}

TEST_P(SitrilTestNetwork, queryNetworkSelectionMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE;
    UINT16 sitId = SIT_GET_NTW_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_network_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildQueryNetworkSelectionModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(SIT_NET_NETWORK_MODE_AUTOMATIC, data[0]);
}

// Deprecated
//TEST_P(SitrilTestNetwork, queryAvailableNetworks) {}

TEST_P(SitrilTestNetwork, queryBplmnSearch) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_QUERY_BPLMN_SEARCH;
    UINT16 sitId = SIT_GET_AVAILABLE_NETWORKS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
    struct RespHookImpl : public SitrilTest::RilResponseHookBase {
        using SitrilTest::RilResponseHookBase::RilResponseHookBase;

        void onRequestComplete(const void *data, size_t datalen) {
            char **p_cur = (char **) (data);
            ASSERT_NE((UINT32)0, datalen);
            int numStrings = datalen/sizeof(char *);
            EXPECT_EQ(5, numStrings);
            if (p_cur[2] != nullptr) EXPECT_STREQ("45005", p_cur[2]);
        }
    } hook(*psitRilTest);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_available_networks_wit_rat_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildQueryAvailableNetworksRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);
}

TEST_P(SitrilTestNetwork, queryAvailableBandMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE;
    UINT16 sitId = SIT_GET_BAND_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_band_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildQueryAvailableBandModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(3, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *) rilRspData;
    EXPECT_EQ(100, data[2]);
}

TEST_P(SitrilTestNetwork, getPrefNetType) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE;
    UINT16 sitId = SIT_GET_PREFERRED_NTW_TYPE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_pref_network_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetPrefNetTypeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(true, data[0] && NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA);
}

TEST_P(SitrilTestNetwork, getCellInfoList) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_CELL_INFO_LIST;
    UINT16 sitId = SIT_GET_CELL_INFO_LIST;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetCellInfoListRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    EXPECT_NE(nullptr, rilRspData);
    RIL_CellInfo_V1_6 *data = (RIL_CellInfo_V1_6 *)rilRspData;
    // Todo
    EXPECT_EQ(RIL_CELL_INFO_TYPE_LTE, data->cellInfoType);
}

TEST_P(SitrilTestNetwork, getRadioCapability) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_RADIO_CAPABILITY;
    UINT16 sitId = SIT_GET_RADIO_CAPABILITY;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_rc_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetRadioCapabilityRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(RIL_RadioCapability);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    RIL_RadioCapability *data = (RIL_RadioCapability *)rilRspData;
    EXPECT_EQ(true, data->rat && (int)RAF_LTE);
}

TEST_P(SitrilTestNetwork, getBarringInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_BARRING_INFO;
    UINT16 sitId = SIT_GET_BARRING_INFOS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_barring_infos_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetBarringInfoRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // Todo
#if 0
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(RIL_CellIdAndBarringInfo);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    RIL_CellIdAndBarringInfo *data = (RIL_CellIdAndBarringInfo *)rilRspData;
    EXPECT_EQ(0, data->numOfrecord);
#endif
}

TEST_P(SitrilTestNetwork, getDuplexMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_DUPLEX_MODE;
    UINT16 sitId = SIT_GET_DUPLEX_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    //Card state shall be present to process
    psitRilTest->processSitrilStateUpdate(RADIO_STATE_ON, RIL_REG_HOME, RIL_CARDSTATE_PRESENT);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_duplex_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetDuplexModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(DUPLEX_MODE_LTG, data[0]);
}

TEST_P(SitrilTestNetwork, getCdmaHybridMode_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_CDMA_HYBRID_MODE;
    UINT16 sitId = SIT_GET_CDMA_HYBRID_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_cdma_hybrid_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetCdmaHybridModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(HYBRID_MODE_EHRPD_ONLY, data[0]);
}

TEST_P(SitrilTestNetwork, getCdmaHybridMode_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_CDMA_HYBRID_MODE;
    UINT16 sitId = SIT_GET_CDMA_HYBRID_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_cdma_hybrid_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, cdmaQueryRoamingPreferrence_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE;
    UINT16 sitId = SIT_GET_CDMA_ROAMING_PREFERENCE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_query_cdma_roaming_rep);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildCdmaQueryRoamingPreferrenceRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(SIT_CDMA_RM_HOME, data[0]);
}

TEST_P(SitrilTestNetwork, cdmaQueryRoamingPreferrence_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE;
    UINT16 sitId = SIT_GET_CDMA_ROAMING_PREFERENCE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_query_cdma_roaming_rep);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, getNrMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_NR_MODE;
    UINT16 sitId = SIT_GET_NR_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetNrModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, getEndcMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_ENDC_MODE;
    UINT16 sitId = SIT_GET_ENDC_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetEndcModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, getImsSupportService_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_IMS_SUPPORT_SERVICE;
    UINT16 sitId = SIT_GET_PS_REG_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_ps_reg_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetImsSupportServiceRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(5, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
    EXPECT_EQ(1, data[1]);
    EXPECT_EQ(mPhoneIndex, data[2]);
    EXPECT_EQ(RADIO_TECH_LTE, data[3]);
    EXPECT_EQ(REGISTERED_HOME, data[4]);
}

TEST_P(SitrilTestNetwork, getImsSupportService_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_IMS_SUPPORT_SERVICE;
    UINT16 sitId = SIT_GET_PS_REG_STATE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_ps_reg_state_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, getManualRatMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_MANUAL_RAT_MODE;
    UINT16 sitId = SIT_GET_MANUAL_RAT_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetManualRatModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(2, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
    EXPECT_EQ(SIT_RAT_TYPE_LTE, data[1]);
}

TEST_P(SitrilTestNetwork, getFrequencyLock) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_FREQUENCY_LOCK;
    UINT16 sitId = SIT_GET_FREQUENCY_LOCK;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetFrequencyLockRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(7, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
    EXPECT_EQ(SIT_RAT_TYPE_LTE, data[1]);
    EXPECT_EQ(3, data[2]);
    EXPECT_EQ(4, data[3]);
    EXPECT_EQ(5, data[4]);
    EXPECT_EQ(6, data[5]);
    EXPECT_EQ(7, data[6]);
}

TEST_P(SitrilTestNetwork, getNrSilenceMode) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_NR_SILENCE_MODE;
    UINT16 sitId = SIT_GET_NR_SILENCE_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_get_nr_silence_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetNrSilenceModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(2, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
    EXPECT_EQ(0, data[1]);
}

TEST_P(SitrilTestNetwork, getFrequencyInfo) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_FREQUENCY_INFO;
    UINT16 sitId = SIT_GET_FREQUENCY_INFO;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetFrequencyInfoRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(6, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
    EXPECT_EQ(2, data[1]);
    EXPECT_EQ(3, data[2]);
    EXPECT_EQ(4, data[3]);
    EXPECT_EQ(5, data[4]);
    EXPECT_EQ(6, data[5]);
}

TEST_P(SitrilTestNetwork, getAllowDataState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_ALLOW_DATA_STATE;
    UINT16 sitId = SIT_GET_PS_SERVICE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_ps_service_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetAllowDataStateRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int numStrings = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, numStrings);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, getVoiceRadioTech) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_VOICE_RADIO_TECH;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    // There is no IPC
    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    ASSERT_NE(nullptr, rilRspData);
    int voiceRat = *((int*)rilRspData);
    EXPECT_NE(-1, voiceRat);
}

TEST_P(SitrilTestNetwork, getCdmaQueryPreferredVoicePrivacyMode_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE;
    UINT16 sitId = SIT_GET_CDMA_VOICE_PRIVACY_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_get_preferred_voice_privacy_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetCdmaQueryPreferredVoicePrivacyModeRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int num = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, num);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, getCdmaQueryPreferredVoicePrivacyMode_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE;
    UINT16 sitId = SIT_GET_CDMA_VOICE_PRIVACY_MODE;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_call_get_preferred_voice_privacy_mode_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, getPreferredCallCapability_success) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_PREFERRED_CALL_CAPABILITY;
    UINT16 sitId = SIT_GET_PREFERRED_CALL_CAPABILITY;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_preferred_call_capability_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildGetPreferredCallCapabilityRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_NE((UINT32)0, rilRspDataLen);
    int num = rilRspDataLen/sizeof(int);
    EXPECT_EQ(1, num);
    EXPECT_NE(nullptr, rilRspData);
    int *data = (int *)rilRspData;
    EXPECT_EQ(1, data[0]);
}

TEST_P(SitrilTestNetwork, getPreferredCallCapability_failure) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_PREFERRED_CALL_CAPABILITY;
    UINT16 sitId = SIT_GET_PREFERRED_CALL_CAPABILITY;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_misc_get_preferred_call_capability_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, cancelAvailableNetworkst) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_CANCEL_AVAILABLE_NETWORKS;
    UINT16 sitId = SIT_CANCEL_GET_AVAILABLE_NETWORKS;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_cancel_get_available_networks_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildCancelAvailableNetworkstRsp(sitId, sitToken, &mRspDataLen);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // rsp data is null
    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}


TEST_P(SitrilTestNetwork, getAllowedTypeBitmap) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP;
    UINT16 sitId = SIT_GET_ALLOW_NETWORK;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_get_allow_network_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, setAllowedTypeBitmap) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP;
    UINT16 sitId = SIT_SET_ALLOW_NETWORK;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    int testData = 0xFFFFFFFF;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_net_set_allow_network_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, oemGetPlmnList) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_GET_PLMN_LIST;
    UINT16 sitId = SIT_GET_PLMN_LIST;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, oemSetPlmnList) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_PLMN_LIST;
    UINT16 sitId = SIT_SET_PLMN_LIST;
    UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataSize = 5;
    char testData[testDataSize] = "test";
    psitRilTest->processExternReq(GetRandomSerialNumber(), testRilCmdId, testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(null_data_format) + sizeof(testData);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    mpModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(mpModemData->GetMessageId(), sitId);

    /* test RSP */
    sitToken = mpModemData->GetToken();
    mRspData = buildErrorRsp(sitId, sitToken, &mRspDataLen, RIL_E_GENERIC_FAILURE);
    psitRilTest->processRsp(testRilCmdId, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    ASSERT_EQ((UINT32)0, rilRspDataLen);
}

TEST_P(SitrilTestNetwork, oemSetImsRegState) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    int testRilCmdId = RIL_REQUEST_OEM_SET_IMS_REGISTRATION_STATE;
    //UINT16 sitId = XXXXX;
    //UINT sitToken = TOKEN_INVALID;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    const int testDataSize = 2;
    int testData[testDataSize] = { REGISTERED_HOME, NET_TYPE_VOLTE };
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, testData, sizeof(testData), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

////////////////////////////////////////////////////////////////////////////////////////
// Indication //
TEST_P(SitrilTestNetwork, networkStateChangeInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_NTW_STATE_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildNetworkStateChangeInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, rrcInfoChangeInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RRC_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildRrcInfoInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_LTE_RRC_STATE_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, mccInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_MCC;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildEmptyInd(sitId, &mRspDataLen);
    psitRilTest->processInd(0, mRspData, mRspDataLen);
}

TEST_P(SitrilTestNetwork, radioCapabilityInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_RADIO_CAPABILITY;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildEmptyInd(sitId, &mRspDataLen);
    psitRilTest->processInd(0, mRspData, mRspDataLen);
}

TEST_P(SitrilTestNetwork, totalOOsInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_TOTAL_OOS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildEmptyInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_TOTAL_OOS, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_EQ((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, emergencyActInfoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_EMERGENCY_ACT_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildEmergencyActInfoInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_EMERGENCY_ACT_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, frequencyInfoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_FREQUENCY_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildFrequencyInfoInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_FREQUENCY_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, cellInfoListInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CELL_INFO_LIST;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildCellInfoInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CELL_INFO_LIST, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, scanningNetworksGsmInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCANNING_NETWORKS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScanningNetworksGsmInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NETWORK_SCAN_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, scanningNetworksCdmaInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCANNING_NETWORKS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScanningNetworksCdmaInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NETWORK_SCAN_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, scanningNetworksLteInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCANNING_NETWORKS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScanningNetworksLteInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NETWORK_SCAN_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, scanningNetworksWcdmaInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCANNING_NETWORKS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScanningNetworksWcdmaInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NETWORK_SCAN_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, scanningNetworksTdscdmaInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCANNING_NETWORKS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScanningNetworksTdscdmaInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NETWORK_SCAN_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, scanningNetworksNrInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCANNING_NETWORKS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScanningNetworksNrInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_NETWORK_SCAN_RESULT, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, b2b1ConfigInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_B2_B1_CONFIG;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildB2b1ConfigInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_B2_B1_CONFIG_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, acBarringInfoInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_AC_BARRING_INFO;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildAcBarringInfoInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_OEM_AIMS_AC_BARRING_INFO, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, physicalChannelConfigInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_PHYSICAL_CHANNEL_CONFIG;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildPhysicalChannelConfigInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

// Path removed
#if 0
TEST_P(SitrilTestNetwork, scgBearerAllocationInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_SCG_BEARER_ALLOCATION;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildScgBearerAllocationInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_PHYSICAL_CHANNEL_CONFIG, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}
#endif

TEST_P(SitrilTestNetwork, regFailedInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_REG_FAILED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildRegFailedInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_REGISTRATION_FAILED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, barringInfoChangedInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_BARRING_INFO_CHANGED;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildBarringInfoChangedInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_BARRING_INFO_CHANGED, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    // Todo
#if 0
    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
#endif
}

TEST_P(SitrilTestNetwork, cdmaInfoRecInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CDMA_INFO_REC;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildCdmaInfoRecInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CDMA_INFO_REC, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, cdmaCallWaitingInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CDMA_CALL_WAITING;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildCdmaCallWaitingInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CDMA_CALL_WAITING, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, cdmaOtaProvisionStatusInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_OTA_PROVISION_STATUS;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildCdmaOtaProvisionStatusInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_CDMA_OTA_PROVISION_STATUS, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}

TEST_P(SitrilTestNetwork, lceDataRecvInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    UINT16 sitId = SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE;

    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);

    /* test IND */
    mRspData = buildLceDataRecvInd(sitId, &mRspDataLen);
    psitRilTest->processInd(RIL_UNSOL_LCEDATA_RECV, mRspData, mRspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 indLen = 0;
    const void *rilIndData= nullptr;
    rilIndData = psitRilTest->getRilIndData(0, &indLen);
    EXPECT_NE((UINT32)0, indLen);
}
