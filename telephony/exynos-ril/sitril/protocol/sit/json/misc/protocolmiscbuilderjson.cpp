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
 * protocolmiscbuilderjson.cpp
 *
 *  Created on: 2021. 07. 19.
 *      Author: p1.bansal
 */

#include "protocolmiscbuilderjson.h"
#include "miscprotocolutils.h"
#include "protocolutils.h"
#include "miscjsondef.h"
#include "rilproperty.h"
#include "oemjsondef.h"
#include "psjsondef.h"
#include "rillog.h"
#include <mcctable.h>
#include <string>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

using std::string;

static inline ULONG Byte2LongLittleEndian(BYTE *byte, int size) {
    LONG ret = 0;
    int longSize = sizeof(LONG);
    for (int i = 0; i < size && i < longSize; ++i) {
        LONG tmpData = ((*byte) & 0xFF);
        ret = (ret) | (tmpData << (i*8));
        byte++;
    }
    return ret;
}

/**
 * ProtocolDtmfBuilderJson
 */
ModemData *ProtocolDtmfBuilderJson::BuildDtmf(int /* dtmf_length */, char *dtmf) {
    Json::Value jsonReqData;

    string dtmfString = (dtmf == NULL) ? "" : dtmf;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, dtmfString, MAX_DTMF_LEN);
    jsonReqData[ns_misc_call_dtmf_req::key_dtmf_digits] = dtmfString;

    return GetJsonReqModemData(jsonReqData, SIT_DTMF);
}

ModemData *ProtocolDtmfBuilderJson::BuildDtmfStart(bool tone_type, BYTE digit) {
    Json::Value jsonReqData;

    jsonReqData[ns_misc_call_dtmf_start_req::key_tone_type] = tone_type==true?SIT_CALL_LOCAL_DTMF_ON:SIT_CALL_LOCAL_DTMF_OFF;
    jsonReqData[ns_misc_call_dtmf_start_req::key_tone_duration] = SIT_CALL_DTMF_TONE_DURATION_SHORT;
    jsonReqData[ns_misc_call_dtmf_start_req::key_dtmf_digit] = digit;

    return GetJsonReqModemData(jsonReqData, SIT_DTMF_START);
}

ModemData *ProtocolDtmfBuilderJson::BuildDtmfStop() {
    return GetJsonNullReqModemData(SIT_DTMF_STOP);
}

/**
 * ProtocolMiscBuilderJson
 */
ModemData *ProtocolMiscBuilderJson::GetTtyMode() {
    return GetJsonNullReqModemData(SIT_GET_TTY_MODE);
}

ModemData *ProtocolMiscBuilderJson::SetTtyMode(int mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_tty_mode_req::key_tty_mode] = mode;

    return GetJsonReqModemData(jsonReqData, SIT_SET_TTY_MODE);
}

ModemData *ProtocolMiscBuilderJson::BuildNvReadItem(int /* nvItemId */) {
    return NULL;
}

ModemData *ProtocolMiscBuilderJson::BuildNvWriteItem(int /* nvItemId */, const char * /* value */) {
    return NULL;
}

ModemData *ProtocolMiscBuilderJson::GetModemActivityInfo() {
    return GetJsonNullReqModemData(SIT_GET_ACTIVITY_INFO);
}

ModemData *ProtocolMiscBuilderJson::BuildSetSuppSvcNotification(int /* enable */) {
    RilLogI("%s need to implement", __FUNCTION__);
    return NULL;
}

ModemData *ProtocolMiscBuilderJson::BuildModemInfo(int type, char *data, unsigned int size) {
    Json::Value jsonReqData;
    string data_str(data,MIN(size,2048));
    jsonReqData[ns_oem_nw_info_req::key_command_id] = type;
    jsonReqData[ns_oem_nw_info_req::key_buffer] = data_str;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_NW_INFO);
}

ModemData *ProtocolMiscBuilderJson::BuildSwitchModemFunction(int feature, BYTE enable) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_fun_swtich_req::key_feature] = feature;
    jsonReqData[ns_oem_set_fun_swtich_req::key_enable] = enable;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_FUNC_SWITCH_REQ);
}

ModemData *ProtocolMiscBuilderJson::BuildSetPdcpDiscardTimer(int discardTimer) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_pdcp_discard_timer_req::key_discard_timer] = discardTimer;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_PDCP_DISCARD_TIMER);
}


ModemData *ProtocolMiscBuilderJson::BuildSetActivateVsim(int slot, int iccidLen, const char *pIccid,
        int imsiLen, const char *pImsi, const char *pHplmn,
        int vsimState, int vsimCardType) {


    if (pIccid == NULL || iccidLen < 0) {
        iccidLen = 0;
    }

    if (pImsi == NULL || imsiLen < 0) {
        imsiLen = 0;
    }

    Json::Value jsonReqData;

    string iccid_str = (pIccid == NULL) ? "" : pIccid;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, iccid_str, MAX_ICCID_STRING_LEN);
    string imsi_str = (pImsi == NULL) ? "" : pImsi;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, imsi_str, MAX_IMSI_LEN);
    string hplmn_str(pHplmn);
    if(hplmn_str.length() == 5)
    {
        hplmn_str.append("#");
    }
    jsonReqData[ns_oem_set_activate_factory_vsim_req::key_sim_slot] = slot;
    jsonReqData[ns_oem_set_activate_factory_vsim_req::key_iccid] = iccid_str;
    jsonReqData[ns_oem_set_activate_factory_vsim_req::key_imsi] = imsi_str;
    jsonReqData[ns_oem_set_activate_factory_vsim_req::key_hplmn] = hplmn_str;
    jsonReqData[ns_oem_set_activate_factory_vsim_req::key_vsim_state] = vsimState;
    jsonReqData[ns_oem_set_activate_factory_vsim_req::key_vsim_card_type] = vsimCardType;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_ACTIVATE_VSIM);
}
ModemData *ProtocolMiscBuilderJson::BuildGetCqiInfo() {
    return GetJsonNullReqModemData(SIT_OEM_GET_CQI_INFO);
}

ModemData *ProtocolMiscBuilderJson::BuildSetSarSetting(int dsi) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_sar_setting_req::key_dsi] = dsi;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_SAR_SETTING);
}

ModemData *ProtocolMiscBuilderJson::BuildSetImsTestMode(int mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_ps_set_ims_test_mode_req::key_mode] = mode;

    return GetJsonReqModemData(jsonReqData, SIT_SET_IMS_TEST_MODE);
}

ModemData *ProtocolMiscBuilderJson::BuildSetGmoSwitch(int feature) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_gmo_switch_req::key_feature] = feature;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_GMO_SWITCH);
}

ModemData *ProtocolMiscBuilderJson::BuildSetTcsFci(int state, int /* len */, char *fci) {
    Json::Value jsonReqData;
    string fci_str = (fci == NULL) ? "" : fci;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, fci_str, MAX_FCI_LEN);

    jsonReqData[ns_oem_set_tcs_fci_req::key_state] = state;
    jsonReqData[ns_oem_set_tcs_fci_req::key_fci] = fci_str;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_TCS_FCI_REQ);
}

ModemData *ProtocolMiscBuilderJson::BuildGetTcsFci() {
    return GetJsonNullReqModemData(SIT_OEM_GET_TCS_FCI_INFO);
}

ModemData *ProtocolMiscBuilderJson::BuildSetCABandwidthFilter(int /* enable */) {
    return NULL;
}

ModemData *ProtocolMiscBuilderJson::BuildSetElevatorSensor(int enable) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_elevator_sensor_req::key_sensor_status] = enable;

    return GetJsonReqModemData(jsonReqData, SIT_SET_ELEVATOR_SENSOR);
}

ModemData *ProtocolMiscBuilderJson::BuildSetLocationUpdates(int enable) {
    Json::Value jsonReqData;
    int update_setting = enable > 0 ? 1 : 0;
    jsonReqData[ns_misc_net_set_location_update_setting_req::key_update_setting] = update_setting;

    return GetJsonReqModemData(jsonReqData, SIT_SET_LOCATION_UPDATE_SETTING);
}

ModemData *ProtocolMiscBuilderJson::BuildSetForbidLteCell(int mode, int cellId, int forbiddenTimer, char *plmn) {
    Json::Value jsonReqData;
    if (MccTable::isValidPlmn(plmn) == false) {
        return NULL;
    }

    string plmn_str(plmn);
    if(plmn_str.length() == 5)
    {
        plmn_str.append("#");
    }

    jsonReqData[ns_oem_set_forbid_lte_req::key_mode] = mode;
    jsonReqData[ns_oem_set_forbid_lte_req::key_cell_id] = cellId;
    jsonReqData[ns_oem_set_forbid_lte_req::key_forbidden_timer] = forbiddenTimer;
    jsonReqData[ns_oem_set_forbid_lte_req::key_plmn] = plmn_str;
    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_FORBID_LTE_CELL);
}

ModemData *ProtocolMiscBuilderJson::BuildGetModemActivityInfo() {
    return GetJsonNullReqModemData(SIT_OEM_GET_ACTIVITY_INFO);
}

ModemData *ProtocolMiscBuilderJson::BuildOemSetSvn(char *svn) {
    if (svn == NULL)
        return NULL;

    Json::Value jsonReqData;
    string svn_str = svn;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, svn_str, 2);
    jsonReqData[ns_oem_set_svn_req::key_svn] = svn_str;

    return GetJsonReqModemData(jsonReqData, SIT_OEM5_SET_SVN);
}

ModemData *ProtocolMiscBuilderJson::BuildSetForbidFrequency(int mode, int frequency, int forbiddenTimer) {
    Json::Value jsonReqData;

    jsonReqData[ns_oem_set_forbidden_freq_req::key_mode] = mode;
    jsonReqData[ns_oem_set_forbidden_freq_req::key_frequency] = frequency;
    jsonReqData[ns_oem_set_forbidden_freq_req::key_forbidden_timer] = forbiddenTimer;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_FORBIDDEN_FREQUENCY);
}

//Deprecated
ModemData *ProtocolMiscBuilderJson::BuildSetForbidLetBand(int /* numBands */, int * /* band */) {
    return NULL;
}

//Deprecated
ModemData *ProtocolMiscBuilderJson::BuildSetForbidLteTac(int /* mode */, int /* tac */) {
    return NULL;
}

ModemData *ProtocolMiscBuilderJson::BuildSetAllDivSetting() {
    return GetJsonNullReqModemData(SIT_OEM_SET_ALL_4RX_DIV_SETTING_DEFAULT);
}

ModemData *ProtocolMiscBuilderJson::BuildSetBsrPara(int switchStatus, int packetPeriod, int packetSize) {
    Json::Value jsonReqData;

    jsonReqData[ns_oem_set_bsr_param_req::key_switch_status] = switchStatus;
    jsonReqData[ns_oem_set_bsr_param_req::key_packet_period] = packetPeriod;
    jsonReqData[ns_oem_set_bsr_param_req::key_packet_size] = packetSize;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_BSR_PARA_REQ);
}

ModemData *ProtocolMiscBuilderJson::BuildSetAheadRre(int searchType, int earfcn, int globalCellId) {
    Json::Value jsonReqData;

    jsonReqData[ns_oem_set_ahead_rre_req::key_search_type] = searchType;
    jsonReqData[ns_oem_set_ahead_rre_req::key_earfcn] = earfcn;
    jsonReqData[ns_oem_set_ahead_rre_req::key_global_cell_id] = globalCellId;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_AHEAD_RRE);
}

ModemData *ProtocolMiscBuilderJson::BuildSetDrdv(int flag) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_drdv_req::key_drdv_mask] = flag;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_DRDV);
}

ModemData *ProtocolMiscBuilderJson::BuildSetLabTestConfig(int /* lab_test_config */, BYTE value) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_lab_test_config_req::key_config_mode] = 0x01; // Emergency Call Lab Test Mode
    jsonReqData[ns_misc_set_lab_test_config_req::key_config_status] = value;

    return GetJsonReqModemData(jsonReqData, SIT_SET_LAB_TEST_CONFIG);
}

ModemData *ProtocolMiscBuilderJson::BuildSetForbidNrCell(int mode, ULONG *cellId, int forbiddenTimer, char *plmn) {
    Json::Value jsonReqData;

    if (MccTable::isValidPlmn(plmn) == false) {
        return NULL;
    }

    string plmn_str(plmn);
    if(plmn_str.length() == 5)
    {
        plmn_str.append("#");
    }

    jsonReqData[ns_oem_set_forbid_nr_req::key_mode] = mode;
#if defined(JSON_HAS_INT64)
    jsonReqData[ns_oem_set_forbid_nr_req::key_cell_id] = *cellId;
#else
    jsonReqData[ns_oem_set_forbid_nr_req::key_cell_id] = (double)(*cellId);
#endif
    jsonReqData[ns_oem_set_forbid_nr_req::key_forbidden_timer] = forbiddenTimer;
    jsonReqData[ns_oem_set_forbid_nr_req::key_plmn] = plmn_str;
    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_FORBID_NR_CELL);
}

ModemData *ProtocolMiscBuilderJson::BuildGetCdmaCapabilityInfo() {
    return GetJsonNullReqModemData(SIT_OEM_GET_CDMA_CAPABILITY_INFO);
}

ModemData *ProtocolMiscBuilderJson::BuildGetBinaryVersionInfo() {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildSetDataThrottling(int dataThrottlingAction, int64_t completionDurationMillis) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_data_throttling_req::key_throttling_action] = dataThrottlingAction;
    jsonReqData[ns_misc_set_data_throttling_req::key_completion_duration_millis] = completionDurationMillis;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DATA_THROTTLING);
}

ModemData *ProtocolMiscBuilderJson::BuildSetSrsPwrLimit(int level) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_srs_pwr_limit_req::key_level] = level;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_SRS_PWR_LIMIT);
}

ModemData *ProtocolMiscBuilderJson::BuildSetTraceDumpCfg(BYTE config) {
    Json::Value jsonReqData;
    jsonReqData[ns_oem_set_trace_dump_cfg_req::key_config] = config;

    return GetJsonReqModemData(jsonReqData, SIT_OEM_SET_TRACE_DUMP_CFG);
}

ModemData *ProtocolMiscBuilderJson::BuildGetTraceDumpCfg() {
    return GetJsonNullReqModemData(SIT_OEM_GET_TRACE_DUMP_CFG);
}

ModemData *ProtocolMiscBuilderJson::BuildSetUeOperationMode(int /*mode*/) {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildGetUeOperationMode() {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildSetNrrrmMinReqEnable(int /*enable*/) {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildGetNrrrmMinReqEnable() {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildSetPwrDetectThr(int /*txPwrDetectThr*/, int /*txPwrDifferenceThr*/) {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildSetEnableN28(int /*enable*/) {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildSetUlMimo(int /*disable*/) {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildRadioConfigReset(int type) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_cfg_default_req::key_reset_type] = type; // NOTE: In legacy this field is not used

    return GetJsonReqModemData(jsonReqData, SIT_SET_CFG_DEFAULT);
}

ModemData *ProtocolMiscBuilderJson::BuildSetVcrt(int /*vcrtMode*/) {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildGetVcrt() {
    return nullptr;
}

ModemData *ProtocolMiscBuilderJson::BuildSetNrDisableBand(void * /*data*/, unsigned int /*datalen*/) {
    return nullptr;
}

/**
 * ProtocolDeviceInfoBuilderJson
 */
ModemData *ProtocolDeviceInfoBuilderJson::GetBaseBandVersion(BYTE /* mask = 0xFF */) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_get_baseband_version_req::key_ver_mask] = 0xFF; // All bits are set

    return GetJsonReqModemData(jsonReqData, SIT_GET_BASEBAND_VERSION);
}

ModemData *ProtocolDeviceInfoBuilderJson::GetIMEI() {
    return GetJsonNullReqModemData(SIT_GET_IMEI);
}

ModemData *ProtocolDeviceInfoBuilderJson::GetIMEISV() {
    return GetJsonNullReqModemData(SIT_GET_IMEISV);
}

ModemData *ProtocolDeviceInfoBuilderJson::GetDevID() {
    return GetJsonNullReqModemData(SIT_GET_DEVICE_ID);
}

ModemData *ProtocolDeviceInfoBuilderJson::SetScreenState(int state) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_screen_state_req::key_screen_state] = state;

    return GetJsonReqModemData(jsonReqData, SIT_SET_SCREEN_STATE);
}

ModemData *ProtocolDeviceInfoBuilderJson::SetPinControl(BYTE signal, BYTE status) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_pin_control_req::key_pin_signal] = signal;
    jsonReqData[ns_misc_set_pin_control_req::key_pin_status] = status;

    return GetJsonReqModemData(jsonReqData, SIT_SET_PIN_CONTROL);
}


ModemData *ProtocolDeviceInfoBuilderJson::SendSGCValue(const int TargetOp, const int /* Rsv1 */,
        const int /* Rsv2 */) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_id_set_sgc_req::key_sgc] = MiscProtocolUtils::MappingSGCValue(TargetOp);
    // rsv1 and rsv2 are not used by CP currently

    return GetJsonReqModemData(jsonReqData, SIT_SET_SGC);
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildGetHwConfig() {
    return NULL;
}

ModemData *ProtocolDeviceInfoBuilderJson::SendDeviceInfo(const char* model, const char* swVer, const char* productName, const char* OsVer) {
    Json::Value jsonReqData;
    string modelString = model;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, modelString, MAX_MODEL_NAME_LEN);
    jsonReqData[ns_misc_set_device_info_req::key_ap_model_name] = modelString;

    string swVerString = swVer;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, swVerString, MAX_BB_SW_VER_LEN);
    jsonReqData[ns_misc_set_device_info_req::key_ap_sw_version] = swVerString;

    string productNameString = productName;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, productNameString, MAX_BB_PRODUCT_CODE_LEN);
    jsonReqData[ns_misc_set_device_info_req::key_ap_product_name] = productNameString;

    string osVerString = OsVer;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_MISC, __CLASS_FUNC__, osVerString, MAX_AP_OS_VERSION_LEN);
    jsonReqData[ns_misc_set_device_info_req::key_ap_os_version] = osVerString;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DEVICE_INFO);
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildSetFeatureInfo(RilProperty &bundle) {
    int feature = 0;
    if (bundle.GetBool("iwlan_ap_assist")) {
        feature |= SIT_FEATURE_IWLAN_AP_ASSIST;
    }

    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_feature_info_req::key_feature_bitmask] = feature;
    return GetJsonReqModemData(jsonReqData, SIT_SET_FEATURE_INFO);
}

ModemData *ProtocolDeviceInfoBuilderJson::GetSignalStrength() {
    return GetJsonNullReqModemData(SIT_GET_SIGNAL_STRENGTH);
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildLceStart(int /* lceMode */, int /* interval */) {
    return NULL;
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildLceStop() {
    return NULL;
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildLcePullLceData() {
    return NULL;
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildSetUnsolicitedResponseFilter(unsigned int bitMask) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_ind_cmd_filter_req::key_filter_bitmask] = bitMask;
    return GetJsonReqModemData(jsonReqData, SIT_SET_IND_CMD_FILTER);
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildSetCarrierInfoImsiEncryption(char * /* pMcc */, char * /* pMnc */, int /* keyLen */, BYTE * /* pKey */, int /* keyIdLen */, char * /* pKeyId */, LONG /* expTime */, int /* keyType */) {
    return NULL;
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildSetOpenCarierInfo(unsigned int /* opc */, const char * /* plmn */) {
    return NULL;
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildSetSignalReportCriteria(int ms, int db, int len, int *dbm, int accessNetwork, int measureType, bool isEnable) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_h_ms] = (unsigned int)ms;
    jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_h_db] = (unsigned int)db;
    jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_t_dbm] = Json::arrayValue;

    if (len > 0 && dbm != NULL) {
        for(int i = 0; i < len && i < 10; i++)
            jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_t_dbm].append(dbm[i]);
    }
    jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_ran] = MiscProtocolUtils::ConvertAccessNetworkType2Sit(accessNetwork);
    jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_measure_type] = measureType;
    jsonReqData[ns_misc_set_signal_strength_reporting_criteria_req::key_enable] = isEnable;

    return GetJsonReqModemData(jsonReqData, SIT_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA);
}

ModemData *ProtocolDeviceInfoBuilderJson::BuildSetLinkCapaReportCriteria(int hMs, int hDlKbps, int hUlKbps, int tDlLen, int *tDlKbps, int tUlLen, int *tUlKbps, int accessNetwork) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_h_ms] = (unsigned int)hMs;
    jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_h_dl_kbps] = (unsigned int)hDlKbps;
    jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_h_ul_kbps] = (unsigned int)hUlKbps;
    jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_dl_kbps] = Json::arrayValue;
    jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_ul_kbps] = Json::arrayValue;

    if (tDlLen > 0 && tDlKbps != NULL) {
        for(int i = 0; i < tDlLen && i < 20; i++)
            jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_dl_kbps].append((unsigned int)tDlKbps[i]);
    }

    if (tUlLen > 0 && tUlKbps != NULL) {
        for(int i = 0; i < tUlLen && i < 20; i++)
            jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_t_ul_kbps].append((unsigned int)tUlKbps[i]);
    }
    jsonReqData[ns_misc_set_link_capacity_reporting_criteria_req::key_ran] = MiscProtocolUtils::ConvertAccessNetworkType2Sit(accessNetwork);

    return GetJsonReqModemData(jsonReqData, SIT_SET_LINK_CAPACITY_REPORTING_CRITERIA);
}

/**
 * ProtocolSensorBuilderJson
 */
ModemData *ProtocolSensorBuilderJson::GetRfDesenseMode() {
    return GetJsonNullReqModemData(SIT_GET_RF_DESENSE_MODE);
}

ModemData *ProtocolSensorBuilderJson::SetRfDesenseMode(void *data, unsigned int datalen) {
    const int vaildDataLen = sizeof(sit_rf_desense_info_type);
    if (data == NULL || datalen < vaildDataLen) {
        RilLogW("%s(): datalen = %d", __FUNCTION__, datalen);
        return NULL;
    }

    sit_rf_desense_info_type *rfDesenseInfos = (sit_rf_desense_info_type *)data;
    Json::Value jsonReqData;
    Json::Value jsonRatRelatedData;
    int desense_rat = rfDesenseInfos->rat;
    int desense_op_mode = rfDesenseInfos->desense_operation;

    jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_desense_status] = desense_op_mode;
    jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_desense_rat] = desense_rat;
    jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_power_level] = Byte2LongLittleEndian(rfDesenseInfos->power_level, 2);
    jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_arfcn] = Byte2LongLittleEndian(rfDesenseInfos->arfcn, 4);

    if (desense_rat == (int)SIT_RF_DESENSE_RAT_GSM) {
        jsonRatRelatedData[ns_misc_net_gsm_desense_info::key_gsm_band_bitmask] = (int)rfDesenseInfos->gsm;
        jsonRatRelatedData[ns_misc_net_gsm_desense_info::key_afc] = Byte2LongLittleEndian(rfDesenseInfos->afc, 2);
        jsonRatRelatedData[ns_misc_net_gsm_desense_info::key_tsc] = Byte2LongLittleEndian(rfDesenseInfos->tsc, 2);
        jsonRatRelatedData[ns_misc_net_gsm_desense_info::key_pattern] = (int)rfDesenseInfos->pattern;

        jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_gsm_desense_info] = jsonRatRelatedData;
    } else if (desense_rat == (int)SIT_RF_DESENSE_RAT_WCDMA) {
        // WCDMA band
        jsonRatRelatedData[ns_misc_net_wcdma_desense_info::key_wcdma_band_bitmask] = Byte2LongLittleEndian(rfDesenseInfos->wcdma, 8);

        jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_wcdma_desense_info] = jsonRatRelatedData;
    } else if (desense_rat == (int)SIT_RF_DESENSE_RAT_LTE) {
        // LTE band
        for (int i = 0; i < 32; i = i + 8) {
            jsonRatRelatedData[ns_misc_net_lte_desense_info::key_lte_band_bitmask_list].append(Byte2LongLittleEndian(rfDesenseInfos->lte + i, 8));
        }
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_duplex] = (int)rfDesenseInfos->duplex;
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_ul_bw] = (int)rfDesenseInfos->ul_bw;
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_ul_freq] = Byte2LongLittleEndian(rfDesenseInfos->ul_freq, 4);
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_config_index] = (int)rfDesenseInfos->config_index;
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_sf_config_index] = (int)rfDesenseInfos->special_sf_config_index;
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_vrb_start] = (int)rfDesenseInfos->vrb_start;
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_vrb_len] = (int)rfDesenseInfos->vrb_length;
        jsonRatRelatedData[ns_misc_net_lte_desense_info::key_mcs] = (int)rfDesenseInfos->mcs;

       jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_lte_desense_info] = jsonRatRelatedData;
    }else if (desense_rat == (int)SIT_RF_DESENSE_RAT_CDMA) {
        jsonRatRelatedData[ns_misc_net_cdma_desense_info::key_cdma_band_bitmask] = Byte2LongLittleEndian(rfDesenseInfos->cdma, 2);
        jsonRatRelatedData[ns_misc_net_cdma_desense_info::key_cdma_mod] = (int)rfDesenseInfos->cdma_modulation;

        jsonReqData[misc_ns_net_set_rf_desense_mode_req::key_cdma_desense_info] = jsonRatRelatedData;
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_RF_DESENSE_MODE);
}

ModemData *ProtocolSensorBuilderJson::BuildPSensorStatus(int pSensorStatus) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_sensor_status_req::key_status_bitmask] = pSensorStatus;

    return GetJsonReqModemData(jsonReqData, SIT_SET_SENSOR_STATUS);
}

ModemData *ProtocolSensorBuilderJson::BuildSetSarState(int /* sarState */) {
    return NULL;
}

ModemData *ProtocolSensorBuilderJson::BuildGetSarState() {
    return NULL;
}

ModemData *ProtocolSensorBuilderJson::BuildScanRssi(int rat, int band, int rbw, int scanMode, int startFreq, int endFreq, int step, int antenna, int sampling,
            int tx1, int tx1Band, int tx1Bw, int tx1Freq, int tx1Power, int tx1RbNum, int tx1RbOffset, int tx1Mcs,
            int tx2, int tx2Band, int tx2Bw, int tx2Freq, int tx2Power, int tx2RbNum, int tx2RbOffset, int tx2Mcs) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_rssi_scan_req::key_rat] = rat;
    jsonReqData[ns_misc_set_rssi_scan_req::key_rx_band] = band;
    jsonReqData[ns_misc_set_rssi_scan_req::key_rbw] = rbw;
    jsonReqData[ns_misc_set_rssi_scan_req::key_scan_mode] = scanMode;
    if (scanMode == SCAN_PARTIAL) {
        jsonReqData[ns_misc_set_rssi_scan_req::key_start_freq] = startFreq;
        jsonReqData[ns_misc_set_rssi_scan_req::key_end_freq] = endFreq;
    }
    jsonReqData[ns_misc_set_rssi_scan_req::key_offset] = step;
    jsonReqData[ns_misc_set_rssi_scan_req::key_antenna_selection] = antenna;
    jsonReqData[ns_misc_set_rssi_scan_req::key_sampling_count] = sampling;

    Json::Value tx1Info;
    tx1Info[ns_misc_tx_info::key_status] = tx1;
    if(rat == SCAN_RAT_3G || rat == SCAN_RAT_LTE || rat == SCAN_RAT_LTE_CA || rat == SCAN_RAT_TDSCDMA || rat == SCAN_RAT_NR) {
        tx1Info[ns_misc_tx_info::key_band] = tx1Band;
    }
    tx1Info[ns_misc_tx_info::key_bw] = tx1Bw;
    tx1Info[ns_misc_tx_info::key_freq] = tx1Freq;
    tx1Info[ns_misc_tx_info::key_power] = tx1Power;
    tx1Info[ns_misc_tx_info::key_rb_num] = tx1RbNum;
    tx1Info[ns_misc_tx_info::key_rb_offset] = tx1RbOffset;
    tx1Info[ns_misc_tx_info::key_mcs] = tx1Mcs;

    Json::Value tx2Info;
    tx2Info[ns_misc_tx_info::key_status] = tx2;
    if(rat == SCAN_RAT_3G || rat == SCAN_RAT_LTE || rat == SCAN_RAT_LTE_CA || rat == SCAN_RAT_TDSCDMA || rat == SCAN_RAT_NR) {
        tx2Info[ns_misc_tx_info::key_band] = tx2Band;
    }
    tx2Info[ns_misc_tx_info::key_bw] = tx2Bw;
    tx2Info[ns_misc_tx_info::key_freq] = tx2Freq;
    tx2Info[ns_misc_tx_info::key_power] = tx2Power;
    tx2Info[ns_misc_tx_info::key_rb_num] = tx2RbNum;
    tx2Info[ns_misc_tx_info::key_rb_offset] = tx2RbOffset;
    tx2Info[ns_misc_tx_info::key_mcs] = tx2Mcs;

    jsonReqData[ns_misc_set_rssi_scan_req::key_tx1_info] = tx1Info;
    jsonReqData[ns_misc_set_rssi_scan_req::key_tx2_info] = tx2Info;

    jsonReqData[ns_misc_set_rssi_scan_req::key_rsvd_info_list] = Json::arrayValue;

    return GetJsonReqModemData(jsonReqData, SIT_SET_RSSI_SCAN);
}

/**
 * ProtocolMiscNetworkBuilderJson
 */
ModemData *ProtocolMiscNetworkBuilderJson::GetManualBandMode() {
    return GetJsonNullReqModemData(SIT_GET_MANUAL_BAND_MODE);
}

ModemData *ProtocolMiscNetworkBuilderJson::SetManualBandMode(void *data, unsigned int datalen) {
    const int vaildDataLen = 1 + 32 + 8 + 1 + 2;
    if (data == NULL || datalen < vaildDataLen) {
        RilLogW("%s(): datalen = %d", __FUNCTION__, datalen);
        return NULL;
    }

    BYTE *srcData = (BYTE *)data;
    Json::Value jsonReqData;

    // status
    jsonReqData[ns_misc_net_set_manual_band_mode_req::key_manual_mode_status] = *srcData++;

    // LTE band (32 bytes)
    for (int i = 0; i < 32; i = i + 8) {
        jsonReqData[ns_misc_net_set_manual_band_mode_req::key_lte_band_bitmask_list].append(Byte2LongLittleEndian(srcData, 8));
        srcData = srcData + 8;
    }

    // WCDMA band (8 bytes)
    jsonReqData[ns_misc_net_set_manual_band_mode_req::key_wcdma_band_bitmask] = Byte2LongLittleEndian(srcData, 8);
    srcData = srcData + 8;

    // gsm band (1 byte)
    jsonReqData[ns_misc_net_set_manual_band_mode_req::key_gsm_band_bitmask] = *srcData++;

    // cdma band (2 bytes)
    jsonReqData[ns_misc_net_set_manual_band_mode_req::key_cdma_band_bitmask] = Byte2LongLittleEndian(srcData, 2);
    srcData = srcData + 2;

    return GetJsonReqModemData(jsonReqData, SIT_SET_MANUAL_BAND_MODE);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildCdmaSubscription() {
    return GetJsonNullReqModemData(SIT_GET_CDMA_SUBSCRIPTION);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildSetVoiceOperation(int mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_voice_operation_req::key_mode] = (mode == 0) ? 0 : 3;     // for backward compatibility, use 0x03 for enabled

    return GetJsonReqModemData(jsonReqData, SIT_SET_VOICE_OPERATION);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildGetVoiceOperation() {
    return GetJsonNullReqModemData(SIT_GET_VOICE_OPERATION);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildSetPreferredCallCapability(int mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_preferred_call_capability_req::key_mode] = mode;

    return GetJsonReqModemData(jsonReqData, SIT_SET_PREFERRED_CALL_CAPABILITY);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildGetPreferredCallCapability() {
    return GetJsonNullReqModemData(SIT_GET_PREFERRED_CALL_CAPABILITY);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildSetStatckStatus(int mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_pwr_set_stack_status_req::key_status] = mode;

    return GetJsonReqModemData(jsonReqData, SIT_SET_STACK_STATUS);
}

ModemData *ProtocolMiscNetworkBuilderJson::BuildGetStatckStatus() {
    return GetJsonNullReqModemData(SIT_GET_STACK_STATUS);
}

/**
 * ProtocolOemOmaDmBuilderJson
 */
ModemData *ProtocolOemOmaDmBuilderJson::BuildGetRadioNode(const char * /* path */) {
    return NULL;
}

ModemData *ProtocolOemOmaDmBuilderJson::BuildSetRadioNode(const char * /* path */, const char * /* value */) {
    return NULL;
}

ModemData *ProtocolOemOmaDmBuilderJson::BuildGetVoLteProvisionUpdate() {
    return NULL;
}

ModemData *ProtocolOemOmaDmBuilderJson::BuildSetVoLteProvisionUpdate() {
    return NULL;
}

ModemData *ProtocolOemOmaDmBuilderJson::GetMslCode() {
    return GetJsonNullReqModemData(SIT_GET_MSL_CODE);
}

/**
 * ProtocolMiscDebugBuilderJson
 */
ModemData *ProtocolMiscDebugBuilderJson::SetEngMode(BYTE mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_eng_mode_req::key_mode] = mode;

    return GetJsonReqModemData(jsonReqData, SIT_SET_ENG_MODE);
}

ModemData *ProtocolMiscDebugBuilderJson::SetEngMode(BYTE mode, BYTE sub_mode) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_eng_mode_req::key_mode] = mode;
    jsonReqData[ns_misc_set_eng_mode_req::key_sub_mode] = sub_mode;

    return GetJsonReqModemData(jsonReqData, SIT_SET_ENG_MODE);
}

ModemData *ProtocolMiscDebugBuilderJson::SetScrLine(BYTE lineno) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_screen_line_req::key_screen_line] = lineno;

    return GetJsonReqModemData(jsonReqData, SIT_SET_SCREEN_LINE);
}

ModemData *ProtocolMiscDebugBuilderJson::SetEngStringInput(BYTE /*len*/, char* input) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_eng_string_input_req::key_string] = string(input);

    return GetJsonReqModemData(jsonReqData, SIT_SET_ENG_STRING_INPUT);
}

ModemData *ProtocolMiscDebugBuilderJson::SetDebugTrace(BYTE value) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_debug_trace_req::key_debug_trace] = value;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DEBUG_TRACE);
}

ModemData *ProtocolMiscDebugBuilderJson::BuildATCommand(const char *command) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_forwarding_at_command_req::key_data] = string(command, 0, MAX_SIT_AT_COMMAND_LENGTH);

    return GetJsonReqModemData(jsonReqData, SIT_SET_FORWARDING_AT_COMMAND);
}

ModemData *ProtocolMiscDebugBuilderJson::BuildSetSelflog(int mode, int size) {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_selflog_req::key_mode] = mode;
    jsonReqData[ns_misc_set_selflog_req::key_buf_size] = size;

    return GetJsonReqModemData(jsonReqData, SIT_SET_SELFLOG);
}

ModemData *ProtocolMiscDebugBuilderJson::BuildGetSelflogStatus() {
    return GetJsonNullReqModemData(SIT_GET_SELFLOG_STATUS);
}

ModemData *ProtocolMiscDebugBuilderJson::BuildSetSelflogProfile() {
    return NULL;
}

ModemData *ProtocolMiscDebugBuilderJson::BuildSetSelflogTcpDumpMode(int /* tcpDumpEnable */, int /* headerDumpEnable */) {
    return NULL;
}

ModemData *ProtocolMiscDebugBuilderJson::BuildSetModemLogDump() {
    Json::Value jsonReqData;
    jsonReqData[ns_misc_set_modem_log_dump_req::key_type] = 0x01;

    return GetJsonReqModemData(jsonReqData, SIT_SET_MODEM_LOG_DUMP);
}

ModemData *ProtocolMiscDebugBuilderJson::BuildSetApSystemTime() {
    Json::Value jsonReqData;

    time_t timeSec;
    struct tm *pTmLocal;

    time(&timeSec);
    pTmLocal = localtime(&timeSec);

    jsonReqData[ns_misc_set_ap_system_time_req::key_year] = pTmLocal->tm_year;
    jsonReqData[ns_misc_set_ap_system_time_req::key_month] = pTmLocal->tm_mon;
    jsonReqData[ns_misc_set_ap_system_time_req::key_day] = pTmLocal->tm_mday;
    jsonReqData[ns_misc_set_ap_system_time_req::key_hour] = pTmLocal->tm_hour;
    jsonReqData[ns_misc_set_ap_system_time_req::key_minute] = pTmLocal->tm_min;
    jsonReqData[ns_misc_set_ap_system_time_req::key_second] = pTmLocal->tm_sec;

    return GetJsonReqModemData(jsonReqData, SIT_SET_AP_SYSTEM_TIME);
}

ModemData *ProtocolMiscDebugBuilderJson::StoreAdbSerialNumber(void * /* data */, unsigned int /* datalen */) {
    return NULL;
}

ModemData *ProtocolMiscDebugBuilderJson::ReadAdbSerialNumber() {
    return NULL;
}

ModemData *ProtocolMiscDebugBuilderJson::BuildSetBtlBufMaxSize(int /* size */) {
    return NULL;
}
