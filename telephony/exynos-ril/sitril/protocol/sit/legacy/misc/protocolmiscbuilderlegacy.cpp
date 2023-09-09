/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * protocolmiscbuilderlegacy.cpp
 *
 *  Created on: 2014. 6. 30.
 *      Author: m.afzal
 */

#include "protocolmiscbuilderlegacy.h"
#include <fstream>
#include "miscprotocolutils.h"
#include <mcctable.h>
#include "rilproperty.h"
#include "rillog.h"
#include "modemdata.h"
#include <librilutils/textutils.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolMiscBuilderLegacy
 */
ModemData *ProtocolMiscBuilderLegacy::GetTtyMode() {
    sit_misc_get_tty_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_TTY_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::SetTtyMode(int mode) {
    sit_misc_set_tty_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_TTY_MODE, length);
    req.tty_mode = mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildNvReadItem(int /* nvItemId */) {
    return NULL;
}

ModemData *ProtocolMiscBuilderLegacy::BuildNvWriteItem(int /* nvItemId */, const char * /* value */) {
    return NULL;
}

ModemData *ProtocolMiscBuilderLegacy::GetModemActivityInfo() {
    sit_misc_get_activity_info_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_ACTIVITY_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetSuppSvcNotification(int /* enable */) {
    RilLogI("%s need to implement", __FUNCTION__);
    return NULL;
}

ModemData *ProtocolMiscBuilderLegacy::BuildModemInfo(int type, char *data, unsigned int size) {
    if (data == NULL && size > 0) {
        size = 0;
    }

    int length = sizeof(sit_oem_nw_info_req) + size;
    char *buf = new char[length];
    ModemData *ret = NULL;
    if (buf != NULL) {
        memset(buf, 0, length);
        sit_oem_nw_info_req *req = (sit_oem_nw_info_req *)buf;
        InitRequestHeader(&req->hdr, SIT_OEM_NW_INFO, length);
        req->command = type;
        if (size > 0) {
            req->length = size;
            memcpy(req->data, data, size);
        }
        ret = new ModemData(buf, length);
        delete[] buf;
    }
    return ret;
}

ModemData *ProtocolMiscBuilderLegacy::BuildSwitchModemFunction(int feature, BYTE enable) {
    sit_oem_set_func_switch_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_FUNC_SWITCH_REQ, length);
    req.feature = feature;
    req.enable = enable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetPdcpDiscardTimer(int discardTimer) {
    sit_oem_set_pdcp_discard_timer_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_PDCP_DISCARD_TIMER, length);
    req.discardTimer = discardTimer;
    return new ModemData((char *)&req, length);
}


ModemData *ProtocolMiscBuilderLegacy::BuildSetActivateVsim(int slot, int iccidLen, const char *pIccid,
                                                int imsiLen, const char *pImsi, const char *pHplmn,
                                                int vsimState, int vsimCardType) {
    if (pIccid == NULL || iccidLen < 0) {
        iccidLen = 0;
    }

    if (pImsi == NULL || imsiLen < 0) {
        imsiLen = 0;
    }

    sit_oem_set_activate_visim_req req;
    int length = sizeof(sit_oem_set_activate_visim_req);
    memset(&req, 0, sizeof(sit_oem_set_activate_visim_req));
    InitRequestHeader(&req.hdr, SIT_OEM_SET_ACTIVATE_VSIM, length);

    int cplen = 0;
    req.simSlot = slot;

    req.iccidLen = iccidLen;
    cplen = iccidLen < MAX_ICCID_STRING_LEN ? iccidLen : MAX_ICCID_STRING_LEN;
    if (pIccid != NULL) memcpy(req.iccid, pIccid, cplen);

    req.imsiLen = imsiLen;
    cplen = imsiLen < MAX_IMSI_LEN ? imsiLen : MAX_IMSI_LEN;
    if (pImsi != NULL) memcpy(req.imsi, pImsi, cplen);

    if (pHplmn != NULL) memcpy(req.hplmn, pHplmn, MAX_PLMN_LEN);
    if (req.hplmn[5] == 0) req.hplmn[5] = '#';

    req.vsimState = vsimState;
    req.vsimCardType = vsimCardType;

    return new ModemData((char *)&req, length);
}
ModemData *ProtocolMiscBuilderLegacy::BuildGetCqiInfo() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_CQI_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetSarSetting(int dsi) {
    sit_oem_set_sar_setting_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_SAR_SETTING, length);
    req.dsi = dsi;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetImsTestMode(int mode) {
    sit_pdp_set_ims_test_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_IMS_TEST_MODE, length);
    req.mode = (BYTE)(mode & 0xFF);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetGmoSwitch(int feature) {
    sit_oem_set_gmo_switch_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_GMO_SWITCH, length);
    req.feature = feature;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetTcsFci(int state, int len, char *fci) {
    sit_oem_set_tcs_fci_req req = {};
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_TCS_FCI_REQ, length);
    req.state = (BYTE)state;
    memcpy(req.fci, fci, MIN(len, MAX_FCI_LEN));
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetTcsFci() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_TCS_FCI_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetCABandwidthFilter(int enable) {
    sit_oem_set_ca_bw_filter_req req;
    int length = sizeof(req);
    memset(&req, 0, sizeof(req));
    InitRequestHeader(&req.hdr, SIT_OEM_SET_CA_BW_FILTER, length);
    req.enable = enable > 0 ? 1 : 0;
    return new ModemData((char *)&req, length);
}


ModemData *ProtocolMiscBuilderLegacy::BuildSetElevatorSensor(int enable) {
    sit_set_elevator_sensor_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_ELEVATOR_SENSOR, length);
    req.enable = enable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetLocationUpdates(int enable) {
    sit_set_location_update_setting_req req;
    int length = sizeof(req);
    memset(&req, 0, sizeof(req));
    InitRequestHeader(&req.hdr, SIT_SET_LOCATION_UPDATE_SETTING, length);
    req.update_setting = enable > 0 ? 1 : 0;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetForbidLteCell(int mode, int cellId, int forbiddenTimer, char *plmn) {
    sit_oem_set_forbid_lte_cell_req req;
    int length = sizeof(sit_oem_set_forbid_lte_cell_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_FORBID_LTE_CELL, length);
    req.mode = mode;
    req.cellid = cellId;
    req.forbidden_timer = forbiddenTimer;

    if (MccTable::isValidPlmn(plmn) == false) {
        return NULL;
    }

    size_t len = strlen(plmn);
    req.plmn[5] = '#';
    memcpy(req.plmn, plmn, len);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetModemActivityInfo() {
    sit_oem_get_modem_activity_req req;
    int length = sizeof(sit_oem_get_modem_activity_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_ACTIVITY_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildOemSetSvn(char *svn) {
    if (svn == NULL)
        return NULL;
    sit_oem5_set_svn_req req;
    int length = sizeof(sit_oem5_set_svn_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM5_SET_SVN, length);
    memcpy(req.svn, svn, 2);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetForbidFrequency(int mode, int frequency, int forbiddenTimer) {
    sit_oem_set_forbidden_frequency_req  req;
    int length = sizeof(sit_oem_set_forbidden_frequency_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_FORBIDDEN_FREQUENCY, length);
    req.mode = (BYTE)mode;
    req.frequency = frequency;
    req.forbidden_timer = forbiddenTimer;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetForbidLetBand(int numBands, int *band) {
    if (band == NULL || numBands < 0 || numBands > MAX_FORBID_LTE_BAND) {
        RilLogE("BuildSetForbidLetBand parameter is invalid");
        return NULL;
    }
    sit_oem_set_forbidden_let_band_req  req;
    int length = sizeof(sit_oem_set_forbidden_let_band_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_FORBIDDEN_LTE_BAND, length);
    req.numBands = (BYTE)numBands;
    for (int i=0; i < numBands; i ++) {
        req.band[i] = (INT16)band[i];
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetForbidLteTac(int mode, int tac) {
    sit_oem_set_forbidden_let_tac_req  req;
    int length = sizeof(sit_oem_set_forbidden_let_tac_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_FORBIDDEN_LTE_TAC, length);
    req.mode = (BYTE)mode;
    req.tac = (INT16)tac;
    return new ModemData((char *)&req, length);
}


ModemData *ProtocolMiscBuilderLegacy::BuildSetAllDivSetting() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_ALL_4RX_DIV_SETTING_DEFAULT, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetBsrPara(int switchStatus, int packetPeriod, int packetSize) {
    sit_oem_set_bsr_para_req req;
    int length = sizeof(req);
    memset(&req, 0, sizeof(req));
    InitRequestHeader(&req.hdr, SIT_OEM_SET_BSR_PARA_REQ, length);
    req.switch_status = (BYTE)switchStatus;
    req.packet_period = (INT16)packetPeriod;
    req.packet_size = packetSize;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetAheadRre(int searchType, int earfcn, int globalCellId) {
    sit_oem_set_ahead_rre_req req;
    int length = sizeof(sit_oem_set_ahead_rre_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_AHEAD_RRE, length);
    req.searchType = (BYTE)searchType;
    req.earfcn = (INT32)earfcn;
    req.globalCellId = (INT32)globalCellId;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetDrdv(int flag) {
    sit_oem_set_drdv_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_DRDV, length);
    req.drdv_mask = (BYTE)flag;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetLabTestConfig(int /* lab_test_config */, BYTE value) {
    sit_oem_set_lab_test_config_req req;
    int length = sizeof(sit_oem_set_lab_test_config_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_LAB_TEST_CONFIG, length);
    req.lab_test_config = 0x01; // Emergency Call Lab Test Mode
    req.value = (BYTE)value;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetForbidNrCell(int mode, ULONG *cellId, int forbiddenTimer, char *plmn) {
    sit_oem_set_forbid_nr_cell_req req;
    int length = sizeof(sit_oem_set_forbid_nr_cell_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_FORBID_NR_CELL, length);
    req.mode = mode;
    req.cellid = *cellId;
    req.forbidden_timer = forbiddenTimer;

    if (MccTable::isValidPlmn(plmn) == false) {
        return NULL;
    }

    size_t len = strlen(plmn);
    req.plmn[5] = '#';
    memcpy(req.plmn, plmn, len);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetCdmaCapabilityInfo() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_CDMA_CAPABILITY_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetDataThrottling(int dataThrottlingAction, int64_t completionDurationMillis) {
    sit_set_data_throttling_req req;
    int length = sizeof(sit_set_data_throttling_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_DATA_THROTTLING, length);
    req.throttling_action = (BYTE)dataThrottlingAction;
    req.completion_duration_millis = completionDurationMillis;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetSrsPwrLimit(int level) {
    sit_oem_set_srs_pwr_limit_req req;
    int length = sizeof(sit_oem_set_srs_pwr_limit_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_SRS_PWR_LIMIT, length);
    req.level = (UINT16) level;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetTraceDumpCfg(BYTE config) {
    sit_oem_set_trace_dump_cfg_req req;
    int length = sizeof(sit_oem_set_trace_dump_cfg_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_TRACE_DUMP_CFG, length);
    req.config = config;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetTraceDumpCfg() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_TRACE_DUMP_CFG, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetUeOperationMode(int mode) {
    sit_oem_set_ue_operation_mode_req req;
    int length = sizeof(sit_oem_set_ue_operation_mode_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_UE_OPERATION_MODE, length);
    req.mode = (BYTE) mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetUeOperationMode() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_UE_OPERATION_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetNrrrmMinReqEnable(int enable) {
    sit_oem_set_nrrrm_min_req_enable_req req;
    int length = sizeof(sit_oem_set_nrrrm_min_req_enable_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_NRRRM_MIN_REQ_ENABLE, length);
    req.enable = (BYTE) enable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetNrrrmMinReqEnable() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_NRRRM_MIN_REQ_ENABLE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetPwrDetectThr(int txPwrDetectThr, int txPwrDifferenceThr) {
    sit_oem_set_pwr_detect_thr_req req;
    int length = sizeof(sit_oem_set_pwr_detect_thr_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_PWR_DETECT_THR_REQ, length);
    req.txPwrDetectThr = (INT16) txPwrDetectThr;
    req.txPwrDifferenceThr = (INT16) txPwrDifferenceThr;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetEnableN28(int enable) {
    sit_oem_set_enable_n28_req req;
    int length = sizeof(sit_oem_set_enable_n28_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_ENABLE_N28, length);
    req.enable_n28 = enable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetUlMimo(int disable) {
    sit_set_ul_mimo_req req;
    int length = sizeof(sit_set_ul_mimo_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_UL_MIMO, length);
    req.disable_ul_mimo = disable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetBinaryVersionInfo() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_BINARY_VERSION_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetVcrt(int vcrtMode) {
    sit_oem_set_vcrt_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_VCRT, length);
    req.vcrtMode = (BYTE)vcrtMode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildGetVcrt() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_VCRT, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildRadioConfigReset(int type) {
    sit_misc_set_cfg_default_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_CFG_DEFAULT, length);
    switch (type) {
    case 1: // RELOAD
        req.type = SIT_MISC_CFG_DEFAULT_REASON_MODEM_RESET;
        break;
    case 2: // ERASE (SCRTN)
        req.type = SIT_MISC_CFG_DEFAULT_REASON_NET_REGI_RESET;
        break;
    case 3: // FACTORY_RESET (RTN)
        req.type = SIT_MISC_CFG_DEFAULT_REASON_FACTORY_RESET;
        break;
    default:
        req.type = SIT_MISC_CFG_DEFAULT_REASON_MODEM_RESET;
        break;
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscBuilderLegacy::BuildSetNrDisableBand(void *data, unsigned int datalen) {
    if(data == nullptr || datalen == 0) {
        RilLogE("BuildSetNrDisableBand parameter is invalid");
        return nullptr;
    }

    int length = sizeof(RCM_HEADER) + datalen;
    char *buf = new char[length];
    InitRequestHeader((RCM_HEADER *)buf, SIT_OEM_SET_NR_DISABLE_BAND, length);
    memcpy(buf + sizeof(RCM_HEADER), data, datalen);
    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}

/**
 * ProtocolDeviceInfoBuilderLegacy
 */
ModemData *ProtocolDeviceInfoBuilderLegacy::GetBaseBandVersion(BYTE mask/* = 0xFF*/) {
    sit_misc_get_baseband_version_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_BASEBAND_VERSION, length);
    req.ver_mask = mask;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::GetIMEI() {
    sit_id_get_imei_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_IMEI, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::GetIMEISV() {
    sit_id_get_imeisv_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_IMEISV, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::GetDevID() {
    sit_id_get_deviceid_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_DEVICE_ID, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::SetScreenState(int state) {
    sit_misc_set_screen_state_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_SCREEN_STATE, length);
    req.screen_state = state;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::SetPinControl(BYTE signal, BYTE status) {
    sit_misc_set_pin_control_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_PIN_CONTROL, length);
    req.signal = signal;
    req.status = status;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::SendSGCValue(const int TargetOp, const int Rsv1, const int Rsv2) {
    sit_id_set_sgc_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_SGC, length);
    req.SGC = MiscProtocolUtils::MappingSGCValue(TargetOp);
    req.Rsv1 = Rsv1;
    req.Rsv2 = Rsv2;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildGetHwConfig() {
    sit_misc_get_hw_config_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_HW_CONFIG, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::SendDeviceInfo(const char* model, const char* swVer, const char* productName, const char* OsVer) {
    sit_misc_set_device_info_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DEVICE_INFO, length);

    memset(req.model_name, 0x00, sizeof(req.model_name));
    memset(req.sw_version, 0x00, sizeof(req.sw_version));
    memset(req.product_name, 0x00, sizeof(req.product_name));
    memset(req.ap_os_version, 0x00, sizeof(req.ap_os_version));

    if (model != NULL)
        strncpy((char*)req.model_name, model, sizeof(req.model_name)-1);
    if (swVer != NULL)
        strncpy((char*)req.sw_version, swVer, sizeof(req.sw_version)-1);
    if (productName != NULL)
        strncpy((char*)req.product_name, productName, sizeof(req.product_name)-1);
    if(OsVer != NULL)
        strncpy((char*)req.ap_os_version, OsVer, sizeof(req.ap_os_version)-1);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildSetFeatureInfo(RilProperty &bundle) {
    int feature = 0;
    if (bundle.GetBool("iwlan_ap_assist")) {
        feature |= SIT_FEATURE_IWLAN_AP_ASSIST;
    }

    sit_set_feature_info_req req;
    int length = sizeof(sit_set_feature_info_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_FEATURE_INFO, length);
    req.feature = feature;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::GetSignalStrength() {
    sit_misc_get_signal_strength_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_SIGNAL_STRENGTH, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildLceStart(int lceMode, int interval) {
    sit_pdp_start_lce_info_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_START_LCE_INFO, length);

    req.mode = lceMode;
    req.interval = interval;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildLceStop() {
    sit_pdp_stop_lce_info_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_STOP_LCE_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildLcePullLceData() {
    sit_pdp_get_lce_data_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_LCE_DATA, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildSetUnsolicitedResponseFilter(unsigned int bitMask) {
    sit_misc_set_ind_cmd_filter_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_IND_CMD_FILTER, length);
    req.ind_cmd_filter = bitMask;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildSetCarrierInfoImsiEncryption(char *pMcc, char *pMnc, int keyLen, BYTE *pKey, int keyIdLen, char *pKeyId, LONG expTime, int keyType) {
    sit_id_set_carrier_info_imsi_encription_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_CARRIER_INFO_IMSI_ENCRYPTION, length);

    if (pMcc != NULL) memcpy(req.mcc, pMcc, MAX_MCC_LEN);
    if (pMnc != NULL) memcpy(req.mnc, pMnc, MAX_MNC_LEN);
    if (keyLen > 0 && pKey != NULL) {
        req.carrier_len = (keyLen > MAX_IMSI_ENCRIPTION_KEY_LEN) ? MAX_IMSI_ENCRIPTION_KEY_LEN:keyLen;
        memcpy(req.carrier_key, pKey, req.carrier_len);
    }
    if (keyIdLen > 0 && pKeyId != NULL) {
        req.key_id_len = (keyIdLen > MAX_IMSI_ENCRIPTION_KEY_LEN) ? MAX_IMSI_ENCRIPTION_KEY_LEN:keyIdLen;;
        memcpy(req.key_id, pKeyId, req.key_id_len);
    }
    req.expire_time = expTime;
    req.public_key_type = keyType;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildSetOpenCarierInfo(unsigned int opc, const char *plmn) {
    if (MccTable::isValidPlmn(plmn) == false) {
        return NULL;
    }

    int len = strlen(plmn);
    sit_misc_set_operator_info_req req;
    int length = sizeof(sit_misc_set_operator_info_req);

    memset(&req, 0, sizeof(sit_misc_set_operator_info_req));
    InitRequestHeader(&req.hdr, SIT_SET_OPERATOR_INFO, length);
    req.plmn[5] = '#';
    memcpy(req.plmn, plmn, len);
    req.openCarrierIndex = opc;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildSetSignalReportCriteria(int ms, int db, int len, int *dbm, int accessNetwork, int measureType, bool isEnable) {
    sit_misc_set_signal_strength_report_criteria_req req;
    int length = sizeof(req);
    memset(&req, 0, sizeof(req));
    InitRequestHeader(&req.hdr, SIT_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, length);

    req.ms = ms;
    req.db = db;
    len = MIN(len, MAX_DBM_LEN);
    req.len = (BYTE) len;
    if (len > 0 && dbm != NULL) {
        for(int i = 0; i < len; i++)
            req.dbm[i] = dbm[i];
    }
    req.radio_acc_net = MiscProtocolUtils::ConvertAccessNetworkType2Sit(accessNetwork);
    req.measure_type = (BYTE)measureType;
    req.enable = (BYTE)isEnable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolDeviceInfoBuilderLegacy::BuildSetLinkCapaReportCriteria(int hMs, int hDlKbps, int hUlKbps, int tDlLen, int *tDlKbps, int tUlLen, int *tUlKbps, int accessNetwork) {
    sit_misc_set_link_capacity_report_criteria_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_LINK_CAPACITY_REPORTING_CRITERIA, length);

    int i = 0;
    req.h_ms = hMs;
    req.h_dl_kbps = hDlKbps;
    req.h_ul_kbps = hUlKbps;
    tDlLen = MIN(tDlLen, MAX_UL_DL_KBPS_LEN);
    req.t_dl_len = (BYTE) tDlLen;
    if (tDlLen > 0 && tDlKbps != NULL) {
        for(i = 0; i < tDlLen; i++)
            req.t_dl_kbps[i] = tDlKbps[i];
    }
    tUlLen = MIN(tUlLen, MAX_UL_DL_KBPS_LEN);
    req.t_ul_len = (BYTE) tUlLen;
    if (tUlLen > 0 && tUlKbps != NULL) {
        for(i = 0; i < tUlLen; i++)
            req.t_ul_kbps[i] = tUlKbps[i];
    }
    req.radio_acc_net = MiscProtocolUtils::ConvertAccessNetworkType2Sit(accessNetwork);
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolMiscDebugBuilderLegacy
 */
ModemData *ProtocolMiscDebugBuilderLegacy::SetEngMode(BYTE mode) {
    sit_misc_set_eng_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_ENG_MODE, length);
    req.eng_mode = mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::SetEngMode(BYTE mode, BYTE sub_mode) {
    sit_misc_set_eng_mode_ex_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_ENG_MODE, length);
    req.eng_mode = mode;
    req.sub_mode = sub_mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::SetScrLine(BYTE lineno) {
    sit_misc_set_scr_line_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_SCREEN_LINE, length);
    req.scr_line= lineno;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::SetEngStringInput(BYTE len, char* input) {
    sit_misc_set_eng_string_input_req req;
    int length = sizeof(req.hdr) + len + 1; // len+data
    InitRequestHeader(&req.hdr, SIT_SET_ENG_STRING_INPUT, length);

    req.len = len;
    memset(req.input, 0, sizeof(req.input));
    memcpy(req.input, input, len);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::SetDebugTrace(BYTE value) {
    sit_misc_set_debug_trace_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DEBUG_TRACE, length);
    req.debug_trace= value;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildATCommand(const char *command) {
    if (TextUtils::IsEmpty(command))  {
        return NULL;
    }

    sit_misc_forwarding_at_command_req req;
    memset(&req, 0, sizeof(req));
    unsigned int commandLength = strlen(command);
    if (commandLength > MAX_SIT_AT_COMMAND_LENGTH) {
        commandLength = MAX_SIT_AT_COMMAND_LENGTH;
    }
    unsigned int length = sizeof(RCM_HEADER) + sizeof(req.length) + commandLength;
    InitRequestHeader(&req.hdr, SIT_SET_FORWARDING_AT_COMMAND, length);
    req.length = commandLength & 0xFFFF;
    memcpy(req.data, command, commandLength);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildSetSelflog(int mode, int size) {
    sit_misc_set_selflog_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_SELFLOG, length);
    req.mode = (BYTE) mode;
    req.size = (BYTE) size;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildGetSelflogStatus() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_SELFLOG_STATUS, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildSetSelflogProfile() {
    std::string profile("/data/vendor/rild/profile.hex");
    std::ifstream ifs(profile.c_str());
    if (!ifs) {
        RilLogE("Failed to read %s", profile.c_str());
        return NULL;
    }
    ifs.seekg(0, std::ios::end);
    size_t profileLen = ifs.tellg();
    char *data = new char[profileLen];
    if (data == NULL) {
        RilLogE("Failed to alloc");
        return NULL;
    }
    ifs.seekg(0, std::ios::beg);
    ifs.read(data, profileLen);
    ifs.close();

    size_t length = sizeof(RCM_HEADER) + profileLen;
    char *buff = new char[length];
    if (buff == NULL) {
        RilLogE("Failed to alloc");
        delete[] data;
        return NULL;
    }
    sit_set_selflog_profile_req * req = (sit_set_selflog_profile_req *)buff;
    memset(req, 0, length);
    InitRequestHeader(&req->hdr, SIT_SET_SELFLOG_PROFILE, length);
    memcpy(req->data, data, profileLen);
    ModemData *pModemData = new ModemData((char *)req, length);

    delete[] data;
    delete [] buff;

    return pModemData;
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildSetSelflogTcpDumpMode(int tcpDumpEnable, int headerDumpEnable) {
    sit_oem_set_selflog_tcp_dump_mode_req req;
    int length = sizeof(sit_oem_set_selflog_tcp_dump_mode_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_SELFLOG_TCP_DUMP_MODE, length);
    req.tcp_dump_enable= (BYTE)tcpDumpEnable;
    req.header_dump_enable= (BYTE)headerDumpEnable;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildSetModemLogDump() {
    sit_misc_set_modem_log_dump_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_MODEM_LOG_DUMP, length);
    req.type = 0x01;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildSetApSystemTime() {
    sit_set_ap_system_time_req req;
    int length = sizeof(sit_set_ap_system_time_req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_AP_SYSTEM_TIME, length);

    time_t timeSec;
    struct tm *pTmLocal;

    time(&timeSec);
    pTmLocal = localtime(&timeSec);

    req.year = pTmLocal->tm_year;
    req.month = pTmLocal->tm_mon;
    req.day = pTmLocal->tm_mday;
    req.hour = pTmLocal->tm_hour;
    req.minute = pTmLocal->tm_min;
    req.second = pTmLocal->tm_sec;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::StoreAdbSerialNumber(void *data, unsigned int /* datalen */) {
    sit_oem_store_adb_serial_number_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_STORE_ADB_SERIAL_NUMBER_REQ, length);
    memset(req.adbSerialNumber, 0, sizeof(req.adbSerialNumber));
    strncpy(req.adbSerialNumber, (char *)data, MAX_ADB_SERIAL_NUMBER-1);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::ReadAdbSerialNumber() {
    sit_oem_read_adb_serial_number_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_READ_ADB_SERIAL_NUMBER_REQ, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscDebugBuilderLegacy::BuildSetBtlBufMaxSize(int size) {
    sit_misc_set_btl_bufmax_size_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_BTL_BUFMAX_SIZE, length);
    req.size = (INT16) size;
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolSensorBuilderLegacy
 */
ModemData *ProtocolSensorBuilderLegacy::GetRfDesenseMode() {
    sit_misc_get_rf_desense_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_RF_DESENSE_MODE, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSensorBuilderLegacy::SetRfDesenseMode(void *data, unsigned int datalen) {
    char buf[2048] = {0, };
    int length = 0;
    int headerlen = sizeof(RCM_HEADER);
    unsigned int tlen = 2048 - headerlen;
    datalen = MIN(datalen, tlen);
    length = headerlen + datalen;
    InitRequestHeader((RCM_HEADER *)buf, SIT_SET_RF_DESENSE_MODE, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + headerlen, data, datalen);
    }
    return new ModemData(buf, length);
}

ModemData *ProtocolSensorBuilderLegacy::BuildPSensorStatus(int pSensorStatus) {
    sit_misc_psensor_set_psensor_status_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_SENSOR_STATUS, length);
    req.psensor_status= pSensorStatus;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSensorBuilderLegacy::BuildSetSarState(int sarState) {
    sit_misc_sar_set_sar_state_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_SAR_STATE, length);
    req.sar_status = sarState;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSensorBuilderLegacy::BuildGetSarState() {
    sit_misc_sar_get_sar_state_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_OEM_GET_SAR_STATE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolSensorBuilderLegacy::BuildScanRssi(int rat, int band, int rbw, int scanMode, int startFreq, int endFreq, int step, int antenna, int sampling,
                                                int tx1, int tx1Band, int tx1Bw, int tx1Freq, int tx1Power, int tx1RbNum, int tx1RbOffset, int tx1Mcs,
                                                int tx2, int tx2Band, int tx2Bw, int tx2Freq, int tx2Power, int tx2RbNum, int tx2RbOffset, int tx2Mcs) {
    sit_misc_set_rssi_scan_req req;
    memset(&req, 0, sizeof(req));
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_RSSI_SCAN, length);
    req.rat = (BYTE)(rat & 0xFF);
    req.band = (BYTE)(band & 0xFF);
    req.rbw = (BYTE)(rbw & 0xFF);
    req.scan_mode = (BYTE)(scanMode & 0xFF);
    if (scanMode == SCAN_PARTIAL) {
        req.start_frequency = (UINT16)(startFreq & 0xFFFF);
        req.end_frequency = (UINT16)(endFreq & 0xFFFF);
    }
    req.step = (BYTE)(step & 0xFF);
    req.antenna_selection = (BYTE)(antenna & 0xFF);
    req.sampling_count = (UINT16)(sampling & 0xFFFF);
    req.tx1 = (BYTE)(tx1 & 0xFF);
    req.tx1_band = (BYTE)(tx1Band & 0xFF);
    req.tx1_bw = (BYTE)(tx1Bw & 0xFF);
    req.tx1_freq = (UINT16)(tx1Freq & 0xFFFF);
    req.tx1_power = (UINT16)(tx1Power & 0xFFFF);
    req.tx1_rb_num = (BYTE)(tx1RbNum & 0xFF);
    req.tx1_rb_offset = (BYTE)(tx1RbOffset & 0xFF);
    req.tx1_mcs = (BYTE)(tx1Mcs & 0xFF);
    if (rat == SCAN_RAT_LTE || rat == SCAN_RAT_LTE_CA) {
        req.tx2 = (BYTE)(tx2 & 0xFF);
        req.tx2_band = (BYTE)(tx2Band & 0xFF);
        req.tx2_bw = (BYTE)(tx2Bw & 0xFF);
        req.tx2_freq = (UINT16)(tx2Freq & 0xFFFF);
        req.tx2_power = (UINT16)(tx2Power & 0xFFFF);
        req.tx2_rb_num = (BYTE)(tx2RbNum & 0xFF);
        req.tx2_rb_offset = (BYTE)(tx2RbOffset & 0xFF);
        req.tx2_mcs = (BYTE)(tx2Mcs & 0xFF);
    }

    return new ModemData((char *)&req, length);
}

/**
 * ProtocolOemOmaDmBuilderLegacy
 */
ModemData *ProtocolOemOmaDmBuilderLegacy::BuildGetRadioNode(const char *path) {
    if (TextUtils::IsEmpty(path))  {
        return NULL;
    }

    if (strlen(path) >= MAX_RADIO_NODE_DATA_LEN) {
        return NULL;
    }

    sit_misc_get_radio_node_req req;
    int length = sizeof(sit_misc_get_radio_node_req);
    memset(&req, 0, sizeof(sit_misc_get_radio_node_req));
    InitRequestHeader(&req.hdr, SIT_GET_RADIO_NODE, length);
    strncpy(req.path, path, MAX_RADIO_NODE_DATA_LEN-1);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolOemOmaDmBuilderLegacy::BuildSetRadioNode(const char *path, const char *value) {
    if (TextUtils::IsEmpty(path) || TextUtils::IsEmpty(value))  {
        return NULL;
    }

    if (strlen(path) >= MAX_RADIO_NODE_DATA_LEN || strlen(value) >= MAX_RADIO_NODE_DATA_LEN) {
        return NULL;
    }

    sit_misc_set_radio_node_req req;
    int length = sizeof(sit_misc_set_radio_node_req);
    memset(&req, 0, sizeof(sit_misc_set_radio_node_req));
    InitRequestHeader(&req.hdr, SIT_SET_RADIO_NODE, length);
    strncpy(req.path, path, MAX_RADIO_NODE_DATA_LEN-1);
    strncpy(req.value, value, MAX_RADIO_NODE_DATA_LEN-1);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolOemOmaDmBuilderLegacy::BuildGetVoLteProvisionUpdate() {
    sit_misc_get_volte_provision_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_VOLTE_PROVISION_UPDATE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolOemOmaDmBuilderLegacy::BuildSetVoLteProvisionUpdate() {
    sit_misc_set_volte_provision_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_VOLTE_PROVISION_UPDATE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolOemOmaDmBuilderLegacy::GetMslCode() {
    sit_misc_get_msl_code_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_MSL_CODE, length);

    return new ModemData((char *)&req, length);
}

/**
 * ProtocolDtmfBuilderLegacy
 */
ModemData *ProtocolDtmfBuilderLegacy::BuildDtmf(int dtmf_length, char* dtmf) {
    sit_call_dtmf_req data;
    int length = sizeof(data);
    InitRequestHeader(&data.hdr, SIT_DTMF, length);

    data.dtmf_len = dtmf_length>=MAX_DTMF_LEN?MAX_DTMF_LEN:dtmf_length;
    memcpy(data.dtmf_digit,dtmf, data.dtmf_len);

    return new ModemData((char *)&data, length);
}


ModemData *ProtocolDtmfBuilderLegacy::BuildDtmfStart(bool tone_type, BYTE digit) {
    sit_call_dtmf_start_req data;
    int length = sizeof(data);
    InitRequestHeader(&data.hdr, SIT_DTMF_START, length);

    data.tone_type = tone_type==true?SIT_CALL_LOCAL_DTMF_ON:SIT_CALL_LOCAL_DTMF_OFF;
    data.tone_len = SIT_CALL_DTMF_TONE_DURATION_SHORT;
    data.dtmf_digit= digit;

    return new ModemData((char *)&data, length);
}

ModemData *ProtocolDtmfBuilderLegacy::BuildDtmfStop() {
    sit_call_dtmf_stop_req data;
    int length = sizeof(data);
    InitRequestHeader(&data.hdr, SIT_DTMF_STOP, length);
    return new ModemData((char *)&data, length);
}

/**
 * ProtocolMiscNetworkBuilderLegacy
 */
ModemData *ProtocolMiscNetworkBuilderLegacy::GetManualBandMode() {
    sit_misc_get_manual_band_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_MANUAL_BAND_MODE, length);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::SetManualBandMode(void *data, unsigned int datalen) {
    char buf[2048] = {0, };
    int length = 0;
    int headerlen = sizeof(RCM_HEADER);
    unsigned int tlen = 2048 - headerlen;
    datalen = MIN(datalen, tlen);
    length = headerlen + datalen;
    InitRequestHeader((RCM_HEADER *)buf, SIT_SET_MANUAL_BAND_MODE, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + headerlen, data, datalen);
    }
    return new ModemData(buf, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildCdmaSubscription() {
    sit_misc_get_cdma_subscription_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_CDMA_SUBSCRIPTION, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildSetVoiceOperation(int mode) {
    sit_misc_set_voice_operation_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_VOICE_OPERATION, length);
    req.mode = (mode == 0) ? 0 : 3;     // for backward compatibility, use 0x03 for enabled
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildGetVoiceOperation() {
    sit_misc_get_voice_operation_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_VOICE_OPERATION, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildSetPreferredCallCapability(int mode) {
    sit_misc_set_preferred_call_capability_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_PREFERRED_CALL_CAPABILITY, length);
    req.mode = mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildGetPreferredCallCapability() {
    sit_misc_get_preferred_call_capability_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_PREFERRED_CALL_CAPABILITY, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildSetStatckStatus(int mode) {
    sit_pwr_set_stack_status_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_STACK_STATUS, length);
    req.mode = (BYTE) mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolMiscNetworkBuilderLegacy::BuildGetStatckStatus() {
    sit_pwr_get_stack_status_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_STACK_STATUS, length);
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolCarrierConfigBuilderLegacy
 */
ModemData *ProtocolCarrierConfigBuilderLegacy::BuildSetCpCarrierConfig(const char *configDir, char *encodedManifest, int encodedManifestLen)
{
    if (TextUtils::IsEmpty(configDir) || encodedManifestLen <= 0) {
        return NULL;
    }

    sit_set_carrier_config_manifest_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_CARRIER_CONFIG_MANIFEST, length);
    strncpy(req.confpack_path, configDir, MAX_CONFPACK_LEN - 1);
    req.confpack_path_len = strlen(req.confpack_path);
    if (encodedManifestLen > MAX_MANIFEST_LEN) {
        encodedManifestLen = MAX_MANIFEST_LEN;
    }
    memcpy(req.manifest_path, encodedManifest, encodedManifestLen);
    req.manifest_len = encodedManifestLen;
    return new ModemData((char *)&req, length);
}
