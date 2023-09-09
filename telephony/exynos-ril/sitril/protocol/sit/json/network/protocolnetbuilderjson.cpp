/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolnetbuilderjson.h"
#include "netprotocolutils.h"
#include "protocolutils.h"
#include "mcctable.h"
#include "rillog.h"
#include "netjsondef.h"
#include <librilutils/textutils.h>

using namespace std;

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * ProtocolNetworkRegistrationBuilderJson
 */
ModemData *ProtocolNetworkRegistrationBuilderJson::BuildNetworkRegistrationState(int domain)
{
    if (domain == NETWORK_DOMAIN_CS) return GetJsonNullReqModemData(SIT_GET_CS_REG_STATE);
    else if (domain == NETWORK_DOMAIN_PS) return GetJsonNullReqModemData(SIT_GET_PS_REG_STATE);
    else return NULL;
}

ModemData *ProtocolNetworkRegistrationBuilderJson::BuildOperator()
{
    return GetJsonNullReqModemData(SIT_GET_OPERATOR);
}

/**
 * ProtocolRadioPowerBuilderJson
 */
ModemData *ProtocolRadioPowerBuilderJson::BuildRadioPower(int powerState, int forEmergencyCall, int preferredForEmergencyCall)
{
    Json::Value jsonReqData;
    jsonReqData[ns_pwr_set_radio_power_req::key_radio_state] = (powerState == 0 ?
            SIT_PWR_RADIO_STATE_STOP_NETWORK : SIT_PWR_RADIO_STATE_START_NETWORK);
    jsonReqData[ns_pwr_set_radio_power_req::key_for_emc] = forEmergencyCall;
    jsonReqData[ns_pwr_set_radio_power_req::key_preferred_for_emc] = preferredForEmergencyCall;
    return GetJsonReqModemData(jsonReqData, SIT_SET_RADIO_POWER);
}

ModemData *ProtocolRadioPowerBuilderJson::BuildRestartModem()
{
    Json::Value jsonReqData;
    jsonReqData[ns_pwr_set_radio_power_req::key_radio_state] = SIT_PWR_RADIO_STATE_RESET;
    jsonReqData[ns_pwr_set_radio_power_req::key_for_emc] = 0;
    jsonReqData[ns_pwr_set_radio_power_req::key_preferred_for_emc] = 0;
    return GetJsonReqModemData(jsonReqData, SIT_SET_RADIO_POWER);
}

ModemData *ProtocolRadioPowerBuilderJson::BuildShutdown()
{
    Json::Value jsonReqData;
    jsonReqData[ns_pwr_set_radio_power_req::key_radio_state] = SIT_PWR_RADIO_STATE_POWER_OFF;
    jsonReqData[ns_pwr_set_radio_power_req::key_for_emc] = 0;
    jsonReqData[ns_pwr_set_radio_power_req::key_preferred_for_emc] = 0;
    return GetJsonReqModemData(jsonReqData, SIT_SET_RADIO_POWER);
}

ModemData *ProtocolRadioPowerBuilderJson::BuildGetRadioState()
{
    return GetJsonNullReqModemData(SIT_GET_RADIO_POWER);
}

/**
 * ProtocolNetworkModeBuilderJson
 */
ModemData *ProtocolNetworkModeBuilderJson::BuildQueryNetworkSelectionMode()
{
    return GetJsonNullReqModemData(SIT_GET_NTW_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetNetworkSelectionAuto()
{
    return GetJsonNullReqModemData(SIT_SET_NTW_MODE_AUTO);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetNetworkSelectionManual(int rat, const char *plmn)
{
    if (MccTable::isValidPlmn(plmn) == false) return NULL;

    int sitRat = (int)SIT_RAT_TYPE_UNKNOWN;
    if ((int)RADIO_TECH_UNKNOWN <= rat && rat <= (int)RADIO_TECH_GSM) sitRat = rat;
    else if (rat == (int)RADIO_TECH_TD_SCDMA) sitRat = (int)SIT_RAT_TYPE_TD_SCDMA;
    else if (rat == (int)RADIO_TECH_LTE_CA) sitRat = SIT_RAT_TYPE_LTE_CA;
    else if (rat == (int)RADIO_TECH_NR) sitRat = (int)SIT_RAT_TYPE_5G;

    string sitPlmn(plmn);

    Json::Value jsonReqData;
    jsonReqData[ns_net_set_ntw_mode_manual_req::key_rat] = sitRat;
    jsonReqData[ns_net_set_ntw_mode_manual_req::key_plmn] = sitPlmn;
    return GetJsonReqModemData(jsonReqData, SIT_SET_NTW_MODE_MANUAL);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetBandMode(int bandMode)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_set_band_mode_req::key_band] = bandMode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_BAND_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildQueryAvailableBandMode()
{
    return GetJsonNullReqModemData(SIT_GET_BAND_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetEndcMode(int mode)
{
    int sitEndcMode = SIT_NET_DISABLE;
    int sitImmediateScgRel = SIT_ENDC_NOT_RELEASE;
    if (mode == 0) { sitEndcMode = SIT_NET_DISABLE; sitImmediateScgRel = SIT_ENDC_NOT_RELEASE; }
    else if (mode == 1) { sitEndcMode = SIT_NET_ENABLE; sitImmediateScgRel = SIT_ENDC_NOT_RELEASE; }
    else if (mode == 2) { sitEndcMode = SIT_NET_DISABLE; sitImmediateScgRel = SIT_ENDC_RELEASE; }

    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_endc_mode::key_endc_mode] = sitEndcMode;
    jsonReqData[ns_net_getset_endc_mode::key_immediate_scg_rel] = sitImmediateScgRel;
    return GetJsonReqModemData(jsonReqData, SIT_SET_ENDC_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetEndcMode()
{
    return GetJsonNullReqModemData(SIT_GET_ENDC_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetFrequencyInfo()
{
    return GetJsonNullReqModemData(SIT_GET_FREQUENCY_INFO);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetNrMode(int mode)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_nr_mode::key_nr_mode] = mode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_NR_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetNrMode()
{
    return GetJsonNullReqModemData(SIT_GET_NR_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetNrSilenceMode(int nsa, int sa) {
    Json::Value jsonReqData;
    jsonReqData[ns_net_misc_set_nr_silence_mode_req::key_nsa_mode] = nsa;
    jsonReqData[ns_net_misc_set_nr_silence_mode_req::key_sa_mode] = sa;
    return GetJsonReqModemData(jsonReqData, SIT_SET_NR_SILENCE_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetNrSilenceMode()
{
    return GetJsonNullReqModemData(SIT_GET_NR_SILENCE_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetVonrCapa(int capa)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_misc_set_vonr_capability_req::key_vonr_capa] = capa;
    return GetJsonReqModemData(jsonReqData, SIT_SET_VONR_CAPABILITY);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetVonrCapa()
{
    return GetJsonNullReqModemData(SIT_GET_VONR_CAPABILITY);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetManualRatMode()
{
    return GetJsonNullReqModemData(SIT_GET_MANUAL_RAT_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetManualRatMode(int mode, int rat)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_manual_rat_mode::key_manual_rat_mode] = mode;
    jsonReqData[ns_net_getset_manual_rat_mode::key_pref_net_type] = rat;
    return GetJsonReqModemData(jsonReqData, SIT_SET_MANUAL_RAT_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetBarringInfo()
{
    return GetJsonNullReqModemData(SIT_GET_BARRING_INFOS);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetAllowData(int state)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_ps_service::key_ps_state] = (state == ALLOW_DATA_CALL ? 1 : 0);
    return GetJsonReqModemData(jsonReqData, SIT_SET_PS_SERVICE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetPsService()
{
    return GetJsonNullReqModemData(SIT_GET_PS_SERVICE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetDuplexMode(BYTE mode_4g, BYTE mode_3g)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_duplex_mode::key_duplex_lte] = (int)mode_4g;
    jsonReqData[ns_net_getset_duplex_mode::key_duplex_hedge] = (int)mode_3g;
    return GetJsonReqModemData(jsonReqData, SIT_SET_DUPLEX_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetDuplexMode(int mode)
{
    int duplex_mode_4g = (int)SIT_NET_DUPLEX_MODE_FDD_TDD;
    int duplex_mode_3g = (int)SIT_NET_DUPLEX_MODE_FDD_TDD;

    if(mode == DUPLEX_MODE_LTG) {
        duplex_mode_4g = (int)SIT_NET_DUPLEX_MODE_TDD;
        duplex_mode_3g = (int)SIT_NET_DUPLEX_MODE_TDD;
    } else if(mode == DUPLEX_MODE_LWG) {
        duplex_mode_4g = (int)SIT_NET_DUPLEX_MODE_FDD_TDD;
        duplex_mode_3g = (int)SIT_NET_DUPLEX_MODE_FDD;
    } else if (mode == DUPLEX_MODE_GLOBAL) {
        duplex_mode_4g = (int)SIT_NET_DUPLEX_MODE_FDD_TDD;
        duplex_mode_3g = (int)SIT_NET_DUPLEX_MODE_FDD_TDD;
    }

    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_duplex_mode::key_duplex_lte] = duplex_mode_4g;
    jsonReqData[ns_net_getset_duplex_mode::key_duplex_hedge] = duplex_mode_3g;
    return GetJsonReqModemData(jsonReqData, SIT_SET_DUPLEX_MODE);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetDuplexMode()
{
    return GetJsonNullReqModemData(SIT_GET_DUPLEX_MODE);
}

ModemData* ProtocolNetworkModeBuilderJson::BuildSetSysSelChannels(bool isSpecifyChannel,
                                int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *pSpecifiers)
{
    if (pSpecifiers != NULL) {
        ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, specifiersLength, SIT_MAX_NO_RAS);
    } else {
        specifiersLength = 0;
    }

    Json::Value jsonReqData;
    jsonReqData[ns_net_set_system_selection_channels_req::key_is_specify_channels] = isSpecifyChannel;
    if (specifiersLength == 0) {
        jsonReqData[ns_net_set_system_selection_channels_req::key_radio_access_specifier_list] = Json::arrayValue;
    } else {
        // set radio_access_specifier which is variable length field
        for ( int srcIndex = 0; srcIndex < specifiersLength; ++srcIndex) {
            RIL_RadioAccessSpecifier_V1_5 *pSrc = pSpecifiers + srcIndex;

            int networkType = NetProtocolUtils::ConvertRadioAccessNetworkType2Sit((int)pSrc->radio_access_network);
            // skip unsupported network type
            if (networkType < SIT_NET_ACCESS_RADIO_TYPE_GERAN || networkType > SIT_NET_ACCESS_RADIO_TYPE_NGRAN) {
                continue;
            }

            Json::Value jsonRASData;
            jsonRASData[ns_net_ras::key_ran] = networkType;

            // bands
            UINT32 bandsLength = pSrc->bands_length;
            ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, bandsLength, MAX_BANDS);
            if (bandsLength == 0) {
                jsonRASData[ns_net_ras::key_band_list] = Json::arrayValue;
            } else {
                for (unsigned int i = 0; i < bandsLength; i++ ) {
                    switch (networkType) {
                        case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
                            jsonRASData[ns_net_ras::key_band_list].append((int)pSrc->bands.geran_bands[i]);
                            break;
                        case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
                            jsonRASData[ns_net_ras::key_band_list].append((int)pSrc->bands.utran_bands[i]);
                            break;
                        case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
                            jsonRASData[ns_net_ras::key_band_list].append((int)pSrc->bands.eutran_bands[i]);
                            break;
                        case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
                            jsonRASData[ns_net_ras::key_band_list].append((int)pSrc->bands.ngran_bands[i]);
                            break;
                    }
                }
            }

            // channels
            UINT32 channelsLength = pSrc->channels_length;
            ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, channelsLength, MAX_CHANNELS);
            if (channelsLength == 0) {
                jsonRASData[ns_net_ras::key_channel_list] = Json::arrayValue;
            } else {
                for (unsigned int i = 0; i < channelsLength; i++ ) {
                    jsonRASData[ns_net_ras::key_channel_list].append(pSrc->channels[i]);
                }
            }

            jsonReqData[ns_net_set_system_selection_channels_req::key_radio_access_specifier_list].append(jsonRASData);
        }
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_SYSTEM_SELECTION_CHANNELS);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildGetFrequencyLock()
{
    return GetJsonNullReqModemData(SIT_GET_FREQUENCY_LOCK);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetFrequencyLock(int mode, int rat, int ltePci,
                                int lteEarfcn, int gsmArfcn, int wcdmaPsc, int wcdmaUarfcn)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_freq_lock::key_freq_lock_mode] = mode;
    jsonReqData[ns_net_getset_freq_lock::key_rat] = rat;

    if (rat == (int)SIT_FREQ_LOCK_RAT_GSM) {
        jsonReqData[ns_net_getset_freq_lock::key_arfcn]  = gsmArfcn;
    }

    if (rat == (int)SIT_FREQ_LOCK_RAT_WCDMA) {
        jsonReqData[ns_net_getset_freq_lock::key_psc]  = wcdmaPsc;
        jsonReqData[ns_net_getset_freq_lock::key_uarfcn]  = wcdmaUarfcn;
    }

    if (rat == (int)SIT_FREQ_LOCK_RAT_LTE) {
        jsonReqData[ns_net_getset_freq_lock::key_pcid] = ltePci;
        jsonReqData[ns_net_getset_freq_lock::key_earfcn]  = lteEarfcn;
    }

    return GetJsonReqModemData(jsonReqData, SIT_SET_FREQUENCY_LOCK);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetEmergencyCallStatus(int status, int rat)
{
    if (status < EMERGENCY_CALL_STATUS_START || status > EMERGENCY_CALL_STATUS_FAIL) {
        // invalid statuss
        return NULL;
    }

    int emergencyRadioTech = SIT_RAT_TYPE_UNKNOWN;

    if (0 < rat && rat <= RADIO_TECH_GSM) {
        emergencyRadioTech = rat;
    }
    else if (rat == RADIO_TECH_TD_SCDMA) {
        emergencyRadioTech = SIT_RAT_TYPE_TD_SCDMA;
    }
    else if (rat == RADIO_TECH_IWLAN) {
        emergencyRadioTech = SIT_RAT_TYPE_IWLAN;
    }
    else if (rat == RADIO_TECH_NR) {
        emergencyRadioTech = SIT_RAT_TYPE_5G;
    }
    else if (rat < 0 || rat == RADIO_TECH_UNSPECIFIED) {
        emergencyRadioTech = SIT_RAT_TYPE_UNSPECIFIED;
    }
    else {
        // consider as LTE in temporary
        if (rat == RADIO_TECH_LTE_CA) {
            emergencyRadioTech = SIT_RAT_TYPE_LTE;
        }
        // not supported yet
        // SIT_NET_EMERGENCY_AVAILABLE_RAT_HSPADCPLUS, SIT_NET_EMERGENCY_AVAILABLE_RAT_LTE_CA
    }

    Json::Value jsonReqData;
    jsonReqData[ns_net_set_emergency_call_status_req::key_status] = status;
    jsonReqData[ns_net_set_emergency_call_status_req::key_rat] = emergencyRadioTech;
    return GetJsonReqModemData(jsonReqData, SIT_SET_EMERGENCY_CALL_STATUS);
}

ModemData *ProtocolNetworkModeBuilderJson::BuildSetMicroCellSearch(BYTE srch_mode)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_set_micro_cell_search_req::key_search_mode] = (int)srch_mode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_MICRO_CELL_SEARCH);
}

/**
 * ProtocolNetworkScanBuilderJson
 */
ModemData *ProtocolNetworkScanBuilderJson::BuildQueryAvailableNetwork(int ran)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_get_available_networks_req::key_ran] = NetProtocolUtils::ConvertRadioAccessNetworkType2Sit(ran);
    jsonReqData[ns_net_get_available_networks_req::key_band] = 0;    // all bands
    return GetJsonReqModemData(jsonReqData, SIT_GET_AVAILABLE_NETWORKS);
}

ModemData *ProtocolNetworkScanBuilderJson::BuildCancelQueryAvailableNetwork()
{
    return GetJsonNullReqModemData(SIT_CANCEL_GET_AVAILABLE_NETWORKS);
}

ModemData *ProtocolNetworkScanBuilderJson::BuildStartNetworkScan(int scantype, int timeInterval, int lenSpecifiers,
                                RIL_RadioAccessSpecifier_V1_5 *pSpecifiers)
{
    return this->BuildStartNetworkScan(scantype, timeInterval, lenSpecifiers, pSpecifiers,
            300, true, 3, 0, NULL);
}

ModemData *ProtocolNetworkScanBuilderJson::BuildStartNetworkScan(
                            int scantype, int timeInterval, int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *specifiers,
                            int maxSearchTime, bool incrementalResults, int incrementalResultsPeriodicity,
                            int numOfMccMncs, char **mccMncs)
{
    int is2gAllSearch = 0;
    int is3gAllSearch = 0;
    int is4gAllSearch = 0;
    //int is5gAllSearch = 0;
    int valueFor2g3g4gAllSearch = 3;

    // VtsHalRadioV1_4Target#RadioHidlTest_v1_4.startNetworkScan
    // Test IRadio.startNetworkScan() with invalid specifier.
    if (specifiersLength <= 0 || specifiers == NULL) {
        RilLogW("Assert. Test IRadio.startNetworkScan() with invalid specifier.");
        return NULL;
    }

    // Test IRadio.startNetworkScan() with invalid interval.
    if (timeInterval < SCAN_INTERVAL_MIN || timeInterval > SCAN_INTERVAL_MAX) {
        RilLogW("Assert. Test IRadio.startNetworkScan() with invalid interval.");
        return NULL;
    }

    if (maxSearchTime != 0) {
        // Test IRadio.startNetworkScan() with invalid max search time.
        if (maxSearchTime < MAX_SEARCH_TIME_MIN || maxSearchTime > MAX_SEARCH_TIME_MAX) {
            RilLogW("Assert. Test IRadio.startNetworkScan() with invalid  max search time.");
            return NULL;
        }

        // Test IRadio.startNetworkScan() with invalid periodicity.
        if (incrementalResultsPeriodicity < INCREMENT_PERIODIC_MIN
                || incrementalResultsPeriodicity > INCREMENT_PERIODIC_MAX) {
            RilLogW("Assert. Test IRadio.startNetworkScan() with invalid periodicity.");
            return NULL;
        }
    }

    Json::Value jsonReqData;

    jsonReqData[ns_net_start_scanning_networks_req::key_scan_type_bitmask] = NetProtocolUtils::ConvertNetworkScanType2Sit(scantype);
    jsonReqData[ns_net_start_scanning_networks_req::key_interval] = 0;
    if (scantype == RIL_PERIODIC) {
        jsonReqData[ns_net_start_scanning_networks_req::key_interval] = timeInterval;
    }

    jsonReqData[ns_net_start_scanning_networks_req::key_max_search_time] = maxSearchTime;
    jsonReqData[ns_net_start_scanning_networks_req::key_incremental_result] =
                            incrementalResults ? SIT_SCAN_INCREMENTAL_RESULT : SIT_SCAN_NO_INCREMENTAL_RESULT;

    if (incrementalResultsPeriodicity < 3) {
        incrementalResultsPeriodicity = 3;
    }
    else if (incrementalResultsPeriodicity > 10) {
        incrementalResultsPeriodicity = 10;
    }
    jsonReqData[ns_net_start_scanning_networks_req::key_incremental_result_periodicity] = incrementalResultsPeriodicity;

    // radio specifiers
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, specifiersLength, MAX_RADIO_ACCESS_NETWORKS);
    if (specifiersLength == 0) jsonReqData[ns_net_start_scanning_networks_req::key_network_scan_info_list] = Json::arrayValue;

    for (int p = 0; p < specifiersLength; p++) {
        RIL_RadioAccessSpecifier_V1_5 *ras = &specifiers[p];
        Json::Value jsonNetworkScanInfo;
        int networkType = NetProtocolUtils::ConvertRadioAccessNetworkType2Sit((int)ras->radio_access_network);
        // skip unknown network type
        if (networkType == SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN) {
            continue;
        }

        if (networkType == SIT_NET_ACCESS_RADIO_TYPE_GERAN) is2gAllSearch = 1;
        else if (networkType == SIT_NET_ACCESS_RADIO_TYPE_UTRAN) is3gAllSearch = 1;
        else if (networkType == SIT_NET_ACCESS_RADIO_TYPE_EUTRAN) is4gAllSearch = 1;
        //else if (networkType == SIT_NET_ACCESS_RADIO_TYPE_NGRAN) is5gAllSearch = 1;

        jsonNetworkScanInfo[ns_net_scan_info::key_ran] = networkType;

        // bands
        unsigned int bandsLength = ras->bands_length;
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, bandsLength, MAX_BANDS);

        jsonNetworkScanInfo[ns_net_scan_info::key_band_list] = Json::arrayValue;
        for (unsigned int i = 0; i < bandsLength; i++ ) {
            switch (networkType) {
            case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
                jsonNetworkScanInfo[ns_net_scan_info::key_band_list].append((int)ras->bands.geran_bands[i]);
                is2gAllSearch = 0;
                break;
            case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
                jsonNetworkScanInfo[ns_net_scan_info::key_band_list].append((int)ras->bands.utran_bands[i]);
                is3gAllSearch = 0;
                break;
            case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
                jsonNetworkScanInfo[ns_net_scan_info::key_band_list].append((int)ras->bands.eutran_bands[i]);
                is4gAllSearch = 0;
                break;
            case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
                jsonNetworkScanInfo[ns_net_scan_info::key_band_list].append((int)ras->bands.ngran_bands[i]);
                //is5gAllSearch = 0;
                break;
            }
        }

        // channels
        unsigned int channelsLength = ras->channels_length;
        ProtocolUtils::LimitLengthToMax<unsigned int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, channelsLength, MAX_CHANNELS);

        jsonNetworkScanInfo[ns_net_scan_info::key_channel_list] = Json::arrayValue;
        for (unsigned int i = 0; i < channelsLength; i++ ) {
            jsonNetworkScanInfo[ns_net_scan_info::key_channel_list].append(ras->channels[i]);
        }

        jsonReqData[ns_net_start_scanning_networks_req::key_network_scan_info_list].append(jsonNetworkScanInfo);
    }

    // PLMN IDs
    ProtocolUtils::LimitLengthToMax<int>(RIL_LOG_CAT_NET, __CLASS_FUNC__, numOfMccMncs, MAX_NETWORK_PLMN_IDS);
    if (numOfMccMncs < 0 || mccMncs == NULL) numOfMccMncs = 0;
    if (numOfMccMncs == 0) jsonReqData[ns_net_start_scanning_networks_req::key_plmn_list] = Json::arrayValue;
    for (int i = 0; i < numOfMccMncs; i++) {
        char *mccMnc = mccMncs[i];
        if (TextUtils::IsDigitsOnly(mccMnc)) {
            int len = strlen(mccMnc);
            if (len == 5 || len == 6) {
                string plmn(mccMnc);
                jsonReqData[ns_net_start_scanning_networks_req::key_plmn_list].append(plmn);
            }
        }
    }

    // F/W does not set NGRAN on 2020.04.22. To inlcude NGRAN, the below code is added.
    // It is overwritten to 0 in order to let CP search all supported RATs when 2G/3G/4G all band is set.
    if (is2gAllSearch + is3gAllSearch + is4gAllSearch >= valueFor2g3g4gAllSearch) {
        // if CP support NGRAN, NGRAN will be searched by this settings.
        jsonReqData[ns_net_start_scanning_networks_req::key_network_scan_info_list] = Json::arrayValue;
    }

    return GetJsonReqModemData(jsonReqData, SIT_START_SCANNING_NETWORKS);
}

ModemData *ProtocolNetworkScanBuilderJson::BuildStopNetworkScan()
{
    Json::Value jsonReqData;

    jsonReqData[ns_net_start_scanning_networks_req::key_scan_type_bitmask] = SIT_SCAN_TYPE_STOP;
    return GetJsonReqModemData(jsonReqData, SIT_START_SCANNING_NETWORKS);
}

/**
 * ProtocolNetworkTypeBuilderJson
 */
ModemData *ProtocolNetworkTypeBuilderJson::BuildSetPreferredNetworkType(int netType)
{
    int sitNetType = NetProtocolUtils::ConvertNetworktype2Sit(netType);
    // use SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA instead of
    // SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO (not in use)
    if (netType == PREF_NET_TYPE_NR_LTE_CDMA_EVDO) sitNetType = SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA;
    else if (netType == PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA) sitNetType = SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA;

    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_preferred_ntw_type::key_pref_net_type] = sitNetType;
    return GetJsonReqModemData(jsonReqData, SIT_SET_PREFERRED_NTW_TYPE);
}

ModemData *ProtocolNetworkTypeBuilderJson::BuildGetPreferredNetworkType()
{
    return GetJsonNullReqModemData(SIT_GET_PREFERRED_NTW_TYPE);
}

ModemData *ProtocolNetworkTypeBuilderJson::BuildSetDSNetworkType(int netType)
{
    int sitDsNetType = NetProtocolUtils::ConvertDsNetworkType2Sit(netType);
    RilLogI("BuildSetDSNetworkType: netType=%d, set to %s", netType, NetProtocolUtils::ConvertSitDsNetworkType2String(sitDsNetType));

    Json::Value jsonReqData;
    jsonReqData[ns_net_set_ds_ntw_type_req::key_pref_net_type] = sitDsNetType;
    return GetJsonReqModemData(jsonReqData, SIT_SET_DS_NTW_TYPE);
}

ModemData* ProtocolNetworkTypeBuilderJson::BuildSetDualNetworkAndAllowData(int typeForPrimary,
                                        int typeForSecondary, int allowedForPrimary,
                                        int allowedForSecondary)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_set_dual_ntw_and_ps_type_req::key_pri_pref_net_type] = NetProtocolUtils::ConvertNetworktype2Sit(typeForPrimary);
    jsonReqData[ns_net_set_dual_ntw_and_ps_type_req::key_sec_pref_net_type] = NetProtocolUtils::ConvertNetworktype2Sit(typeForSecondary);
    jsonReqData[ns_net_set_dual_ntw_and_ps_type_req::key_pri_ps_capa] = allowedForPrimary;
    jsonReqData[ns_net_set_dual_ntw_and_ps_type_req::key_sec_ps_capa] = allowedForSecondary;
    return GetJsonReqModemData(jsonReqData, SIT_SET_DUAL_NTW_AND_PS_TYPE);
}

/**
 * ProtocolCdmaBuilderJson
 */
ModemData *ProtocolCdmaBuilderJson::BuildSetCdmaSetRoamingType(int cdmaRoamingType)
{
    int sitCdmaRoamingType = SIT_CDMA_RM_ANY;
    switch(cdmaRoamingType) {
        case CDMA_ROAMING_AFFILIATED_NETWORKS:
            sitCdmaRoamingType = SIT_CDMA_RM_AFFILIATED;
            break;
        case CDMA_ROAMING_ANY_NETWORK:
            sitCdmaRoamingType = SIT_CDMA_RM_ANY;
            break;
        case CDMA_ROAMING_HOME_ONLY:
            sitCdmaRoamingType = SIT_CDMA_RM_HOME;
            break;
    }

    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_cdma_roaming_preferrence::key_cdma_mode] = sitCdmaRoamingType;
    return GetJsonReqModemData(jsonReqData, SIT_SET_CDMA_ROAMING_PREFERENCE);
}

ModemData *ProtocolCdmaBuilderJson::BuildQueryCdmaRoamingType()
{
    return GetJsonNullReqModemData(SIT_GET_CDMA_ROAMING_PREFERENCE);
}

ModemData *ProtocolCdmaBuilderJson::BuildSetCdmaHybridMode(int hybridMode)
{
    if (hybridMode < (int)HYBRID_MODE_1X_HRPD || hybridMode > (int)HYBRID_MODE_EHRPD_ONLY) return NULL;

    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_cdma_hybrid_mode::key_hybrid_mode] = hybridMode;
    return GetJsonReqModemData(jsonReqData, SIT_SET_CDMA_HYBRID_MODE);
}

ModemData *ProtocolCdmaBuilderJson::BuildGetCdmaHybridMode()
{
    return GetJsonNullReqModemData(SIT_GET_CDMA_HYBRID_MODE);
}

/**
 * ProtocolNetworkBuilderJson
 */
ModemData *ProtocolNetworkBuilderJson::BuildGetCellInfoList()
{
    return GetJsonNullReqModemData(SIT_GET_CELL_INFO_LIST);
}

ModemData *ProtocolNetworkBuilderJson::BuildSetCellInfoListReportRate(int rate)
{
    // rate 0 means report SIT_SET_CELL_INFO_LIST_REPORT_RATE when information is changed.
    // rate 0x7FFFFFFF means never report SIT_IND_CELL_INFO_LIST
    if (rate < 0) {
        rate = 0x7FFFFFFF;
    }

    Json::Value jsonReqData;
    jsonReqData[ns_net_set_cell_info_list_report_rate_req::key_rate] = rate;
    return GetJsonReqModemData(jsonReqData, SIT_SET_CELL_INFO_LIST_REPORT_RATE);
}

ModemData *ProtocolNetworkBuilderJson::BuildSetRCNetworkType(int /* rcVersion */, int /* rcSession */, int /* rcPhase */, int rcRaf, char *pUuid, int /* rcStatus */) {
    string sitUuid;
    if (TextUtils::IsEmpty(pUuid)) sitUuid = "";
    else sitUuid = pUuid;

    Json::Value jsonReqData;
    jsonReqData[ns_net_set_radio_capability_req::key_raf] = switchRafValueForCP(rcRaf);
    //jsonReqData[ns_net_set_radio_capability_req::key_version] = rcVersion;
    //jsonReqData[ns_net_set_radio_capability_req::key_session_id] = rcSession;
    //jsonReqData[ns_net_set_radio_capability_req::key_phase] = rcPhase;
    //jsonReqData[ns_net_set_radio_capability_req::key_status] = rcStatus;
    //jsonReqData[ns_net_set_radio_capability_req::key_uuid] = sitUuid;
    return GetJsonReqModemData(jsonReqData, SIT_SET_RADIO_CAPABILITY);
}

ModemData *ProtocolNetworkBuilderJson::BuildGetRCNetworkType()
{
    return GetJsonNullReqModemData(SIT_GET_RADIO_CAPABILITY);
}

ModemData *ProtocolNetworkBuilderJson::BuildSvNumber(const char * /* svn */) {
    return NULL;
}

ModemData *ProtocolNetworkBuilderJson::BuildGetSystemSelectionChannels()
{
    return GetJsonNullReqModemData(SIT_GET_SYSTEM_SELECTION_CHANNELS);
}

ModemData *ProtocolNetworkBuilderJson::BuildSetAllowedNetworkTypeBitmap(int networkTypeBitmap)
{
    int sitRaf = switchRafValueForCP(networkTypeBitmap);

    Json::Value jsonReqData;
    jsonReqData[ns_net_getset_allow_network::key_raf] = sitRaf;
    return GetJsonReqModemData(jsonReqData, SIT_SET_ALLOW_NETWORK);
}

ModemData *ProtocolNetworkBuilderJson::BuildGetAllowedNetworkTypeBitmap()
{
    return GetJsonNullReqModemData(SIT_GET_ALLOW_NETWORK);
}

ModemData *ProtocolNetworkBuilderJson::BuildSetPlmnList(int /* requestId */, void * /* data */, unsigned int /* datalen */) {
    return NULL;
}

ModemData *ProtocolNetworkBuilderJson::BuildGetPlmnList()
{
    return GetJsonNullReqModemData(SIT_GET_PLMN_LIST);
}

ModemData *ProtocolNetworkBuilderJson::BuildSetDeviceService(int /* setting */) {
    // To do implementaion
    return NULL;
}

ModemData *ProtocolNetworkBuilderJson::BuildGetDeviceService() {
    // To do implementaion
    return NULL;
}


/******************************************************************************
 * PS
 *****************************************************************************/

ModemData *ProtocolNetworkBuilderJson::BuildSetMobileDataState(int mobileData, int roamingData)
{
    Json::Value jsonReqData;
    jsonReqData[ns_net_ps_set_mobile_data_state_req::key_mobile_data] = mobileData;
    jsonReqData[ns_net_ps_set_mobile_data_state_req::key_roaming_data] = roamingData;
    return GetJsonReqModemData(jsonReqData, SIT_SET_MOBILE_DATA_STATE);
}

