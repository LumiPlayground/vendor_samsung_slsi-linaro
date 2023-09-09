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
 * protocolnetbuilderlegacy.cpp
 *
 *  Created on: 2014. 6. 27.
 *      Author: sungwoo48.choi
 */

#include "protocolnetbuilderlegacy.h"
#include "mcctable.h"
#include "rillog.h"
#include "modemdata.h"
#include <librilutils/textutils.h>

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
 * ProtocolNetworkRegistrationBuilderLegacy
 */
ModemData *ProtocolNetworkRegistrationBuilderLegacy::BuildNetworkRegistrationState(int domain)
{
    if (domain == NETWORK_DOMAIN_CS) {
        sit_net_get_cs_reg_state_req req;
        int length = sizeof(req);
        InitRequestHeader(&req.hdr, SIT_GET_CS_REG_STATE, length);
        return new ModemData((char *)&req, length);
    }
    else if (domain == NETWORK_DOMAIN_PS) {
        sit_net_get_ps_reg_state_req req;
        int length = sizeof(req);
        InitRequestHeader(&req.hdr, SIT_GET_PS_REG_STATE, length);
        return new ModemData((char *)&req, length);
    }
    return NULL;
}

ModemData *ProtocolNetworkRegistrationBuilderLegacy::BuildOperator()
{
    sit_net_get_operator_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_OPERATOR, length);
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolRadioPowerBuilderLegacy
 */
ModemData *ProtocolRadioPowerBuilderLegacy::BuildRadioPower(int powerState, int forEmergencyCall, int preferredForEmergencyCall)
{
    sit_pwr_set_radio_power_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_RADIO_POWER, length);
    req.radio_state = (powerState == 0 ? SIT_PWR_RADIO_STATE_STOP_NETWORK : SIT_PWR_RADIO_STATE_START_NETWORK);
    req.for_emc = forEmergencyCall;
    req.prefered_for_emc = preferredForEmergencyCall;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolRadioPowerBuilderLegacy::BuildRestartModem()
{
    sit_pwr_set_radio_power_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_RADIO_POWER, length);
    req.radio_state = 4;    // fixed value, 0x04 = Phone reset

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolRadioPowerBuilderLegacy::BuildShutdown()
{
    sit_pwr_set_radio_power_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_RADIO_POWER, length);
    req.radio_state = SIT_PWR_RADIO_STATE_POWER_OFF;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolRadioPowerBuilderLegacy::BuildGetRadioState()
{
    sit_pwr_get_radio_power_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_RADIO_POWER, length);
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolNetworkModeBuilderLegacy
 */
ModemData *ProtocolNetworkModeBuilderLegacy::BuildQueryNetworkSelectionMode()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_NTW_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetNetworkSelectionAuto()
{
    sit_net_set_network_mode_auto_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_NTW_MODE_AUTO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetBandMode(int bandMode)
{
    sit_net_set_band_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_BAND_MODE, length);
    req.band = bandMode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildQueryAvailableBandMode()
{
    sit_net_get_band_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_BAND_MODE , length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetNetworkSelectionManual(int rat, const char *plmn)
{
    if (MccTable::isValidPlmn(plmn) == false) {
        return NULL;
    }

    int len = strlen(plmn);
    sit_net_set_metwork_mode_manual_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_NTW_MODE_MANUAL, length);

    int sitRat = (int)SIT_RAT_TYPE_UNKNOWN;
    if ((int)RADIO_TECH_UNKNOWN <= rat && rat <= (int)RADIO_TECH_GSM) sitRat = rat;
    else if (rat == (int)RADIO_TECH_TD_SCDMA) sitRat = (int)SIT_RAT_TYPE_TD_SCDMA;
    else if (rat == (int)RADIO_TECH_LTE_CA) sitRat = SIT_RAT_TYPE_LTE_CA;
    else if (rat == (int)RADIO_TECH_NR) sitRat = (int)SIT_RAT_TYPE_5G;
    req.rat = sitRat;

    req.plmn[5] = '#';
    memcpy(req.plmn, plmn, len);

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetEndcMode(int mode)
{
    sit_net_set_endc_mode_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_ENDC_MODE, length);

    switch(mode) {
        case 0:
            req.mode = (BYTE)SIT_NET_DISABLE;
            req.immediate_scg_rel = (BYTE)SIT_ENDC_NOT_RELEASE;
            break;
        case 1:
            req.mode = (BYTE)SIT_NET_ENABLE;
            req.immediate_scg_rel = (BYTE)SIT_ENDC_NOT_RELEASE;
            break;
        case 2:
            req.mode = (BYTE)SIT_NET_DISABLE;
            req.immediate_scg_rel = (BYTE)SIT_ENDC_RELEASE;
            break;
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetEndcMode()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_ENDC_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetFrequencyInfo()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_FREQUENCY_INFO, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetNrMode(int mode)
{
    sit_net_set_nr_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_NR_MODE, length);

    req.mode = (BYTE)mode;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetNrMode()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_NR_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetNrSilenceMode(int nsa, int sa)
{
    sit_set_nr_silence_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_NR_SILENCE_MODE, length);
    req.nsa_mode = nsa;
    req.sa_mode = sa;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetNrSilenceMode() {
    sit_get_nr_silence_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_NR_SILENCE_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetVonrCapa(int capa) {
    sit_net_set_vonr_capa_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_VONR_CAPABILITY, length);

    req.vonr_capa = capa;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetVonrCapa() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_VONR_CAPABILITY, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetManualRatMode()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_MANUAL_RAT_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetManualRatMode(int mode, int rat)
{
    sit_net_set_manual_rat_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_MANUAL_RAT_MODE, length);

    req.rat_mode_set = (BYTE)mode;
    req.rat = rat;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetFrequencyLock()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_FREQUENCY_LOCK, length);
    return new ModemData((char *)&req, length);
}

ModemData* ProtocolNetworkModeBuilderLegacy::BuildSetFrequencyLock(int mode, int rat,
        int ltePci, int lteEarfcn, int gsmArfcn, int wcdmaPsc,
        int wcdmaUarfcn)
{
    sit_net_set_freq_lock_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_FREQUENCY_LOCK, length);

    req.freq_mode_set = (BYTE)mode;
    req.rat = (BYTE)rat;
    req.lte_pci  = ltePci;
    req.lte_earfcn  = lteEarfcn;
    req.gsm_arfcn  = gsmArfcn;
    req.wcdma_psc  = wcdmaPsc;
    req.wcdma_uarfcn  = wcdmaUarfcn;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetDuplexMode(BYTE mode_4g, BYTE mode_3g)
{
    sit_net_set_duplex_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DUPLEX_MODE, length);

    req.duplex_mode_4g = mode_4g;
    req.duplex_mode_3g = mode_3g;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetDuplexMode(int mode)
{
    BYTE duplex_mode_4g = (BYTE)SIT_NET_DUPLEX_MODE_FDD_TDD;
    BYTE duplex_mode_3g = (BYTE)SIT_NET_DUPLEX_MODE_FDD_TDD;

    if(mode == DUPLEX_MODE_LTG) {
        duplex_mode_4g = (BYTE)SIT_NET_DUPLEX_MODE_TDD;
        duplex_mode_3g = (BYTE)SIT_NET_DUPLEX_MODE_TDD;
    } else if(mode == DUPLEX_MODE_LWG) {
        duplex_mode_4g = (BYTE)SIT_NET_DUPLEX_MODE_FDD_TDD;
        duplex_mode_3g = (BYTE)SIT_NET_DUPLEX_MODE_FDD;
    } else if (mode == DUPLEX_MODE_GLOBAL) {
        duplex_mode_4g = (BYTE)SIT_NET_DUPLEX_MODE_FDD_TDD;
        duplex_mode_3g = (BYTE)SIT_NET_DUPLEX_MODE_FDD_TDD;
    } else {
        // invalid duplex mode request
    }

    sit_net_set_duplex_mode_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DUPLEX_MODE, length);

    req.duplex_mode_4g = duplex_mode_4g;
    req.duplex_mode_3g = duplex_mode_3g;

    return new ModemData((char *)&req, length);

    return NULL;
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetDuplexMode()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_DUPLEX_MODE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetEmergencyCallStatus(int status, int rat)
{
    sit_net_set_emergency_call_status_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_EMERGENCY_CALL_STATUS, length);

    if (status < EMERGENCY_CALL_STATUS_START || status > EMERGENCY_CALL_STATUS_FAIL) {
        // invalid statuss
        return NULL;
    }
    int emergencyCallStatus = status;
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

    req.status = emergencyCallStatus & 0xFF;
    req.rat = emergencyRadioTech & 0xFF;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetBarringInfo()
{
    sit_net_get_barring_infos_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_BARRING_INFOS, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetAllowData(int state)
{
    sit_net_set_ps_service_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_PS_SERVICE, length);
    req.state = (BYTE) (state == ALLOW_DATA_CALL ? SIT_NET_ENABLE : SIT_NET_DISABLE);
    return new ModemData((char *) &req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildGetPsService()
{
    sit_net_get_ps_service_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_PS_SERVICE, length);
    return new ModemData((char *) &req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetMicroCellSearch(BYTE srch_mode)
{
    sit_net_set_micro_cell_search_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_MICRO_CELL_SEARCH, length);

    req.srch_mode = srch_mode;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkModeBuilderLegacy::BuildSetSysSelChannels(bool isSpecifyChannel,
                                                                    int specifiersLength,
                                                                    RIL_RadioAccessSpecifier_V1_5 *pSpecifiers) {
    sit_net_set_system_selection_channels_req req{};

    req.is_specific_channel = (BYTE)isSpecifyChannel;
    if (pSpecifiers != nullptr) {
        specifiersLength = MIN(specifiersLength, SIT_MAX_NO_RAS);
    } else {
        specifiersLength = 0;
    }

    // set radio_access_specifier which is variable length field
    int dstIndex = 0;
    for (int srcIndex = 0; srcIndex < specifiersLength; ++srcIndex) {
        RIL_RadioAccessSpecifier_V1_5 *pSrc = pSpecifiers + srcIndex;
        const int sitNetworkType = NetProtocolUtils::ConvertRadioAccessNetworkType2Sit((int)pSrc->radio_access_network);
        if (NetProtocolUtils::IsSysSelSupported(sitNetworkType) == false) {
            continue;
        }
        FillRasRilSrcToSitDst(sitNetworkType, *pSrc, req.radio_access_specifier_array[dstIndex]);
        dstIndex++;
    }
    req.radio_access_specifier_len = dstIndex;

    // calcluate totoal data length.
    constexpr size_t lenOfFixedData =
        sizeof(sit_net_set_system_selection_channels_req) - sizeof(sit_net_set_system_selection_channels_req::radio_access_specifier_array);
    constexpr size_t lenOfRas = sizeof(sit_net_radio_access_specifier);
    int length = lenOfFixedData + (req.radio_access_specifier_len * lenOfRas);
    InitRequestHeader(&req.hdr, SIT_SET_SYSTEM_SELECTION_CHANNELS, length);

    return new ModemData((char *)&req, length);
}

void ProtocolNetworkModeBuilderLegacy::FillRasRilSrcToSitDst(int sitNetworkType,
                                                             RIL_RadioAccessSpecifier_V1_5 &rilSrc,
                                                             sit_net_radio_access_specifier &sitDst) {
    switch (sitNetworkType) {
    case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
        FillRasRilSrcToSitDst(GERAN_TYPE(), rilSrc, sitDst);
        break;
    case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
        FillRasRilSrcToSitDst(UTRAN_TYPE(), rilSrc, sitDst);
        break;
    case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
        FillRasRilSrcToSitDst(EUTRAN_TYPE(), rilSrc, sitDst);
        break;
    case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
        FillRasRilSrcToSitDst(NGRAN_TYPE(), rilSrc, sitDst);
        break;
    default:
        //assert(false);
        break;
    }
}

void ProtocolNetworkModeBuilderLegacy::FillRasRilSrcToSitDst(const GERAN_TYPE &,
                                                             RIL_RadioAccessSpecifier_V1_5 &rilSrc,
                                                             sit_net_radio_access_specifier &sitDst) {
    sitDst.radio_access_network = (BYTE)SIT_NET_ACCESS_RADIO_TYPE_GERAN;
    sitDst.bands_len = MIN(MIN(rilSrc.bands_length, MAX_BANDS), SIT_MAX_NO_SSC_GERAN_BANDS);
    sitDst.channels_len = MIN(MIN(rilSrc.channels_length, MAX_CHANNELS),SIT_MAX_NO_SSC_GERAN_CHANNELS);
    CopyArrayDataSrcToDst(rilSrc.bands.geran_bands, sitDst.bands.geran_bands, sitDst.bands_len);
    CopyArrayDataSrcToDst(rilSrc.channels, sitDst.channels.geran_channels, sitDst.channels_len);
}

void ProtocolNetworkModeBuilderLegacy::FillRasRilSrcToSitDst(const UTRAN_TYPE &,
                                                             RIL_RadioAccessSpecifier_V1_5 &rilSrc,
                                                             sit_net_radio_access_specifier &sitDst) {
    sitDst.radio_access_network = (BYTE)SIT_NET_ACCESS_RADIO_TYPE_UTRAN;
    sitDst.bands_len = MIN(MIN(rilSrc.bands_length, MAX_BANDS),SIT_MAX_NO_SSC_UTRAN_BANDS);
    sitDst.channels_len = MIN(MIN(rilSrc.channels_length, MAX_CHANNELS), SIT_MAX_NO_SSC_UTRAN_CHANNELS);
    CopyArrayDataSrcToDst(rilSrc.bands.utran_bands, sitDst.bands.utran_bands, sitDst.bands_len);
    CopyArrayDataSrcToDst(rilSrc.channels, sitDst.channels.utran_channels, sitDst.channels_len);
}

void ProtocolNetworkModeBuilderLegacy::FillRasRilSrcToSitDst(const EUTRAN_TYPE &,
                                                             RIL_RadioAccessSpecifier_V1_5 &rilSrc,
                                                             sit_net_radio_access_specifier &sitDst) {
    sitDst.radio_access_network = (BYTE)SIT_NET_ACCESS_RADIO_TYPE_EUTRAN;
    sitDst.bands_len = MIN(MIN(rilSrc.bands_length, MAX_BANDS), SIT_MAX_NO_SSC_EUTRAN_BANDS);
    sitDst.channels_len = MIN(MIN(rilSrc.channels_length, MAX_CHANNELS), SIT_MAX_NO_SSC_EUTRAN_CHANNELS);
    CopyArrayDataSrcToDst(rilSrc.bands.eutran_bands, sitDst.bands.eutran_bands, sitDst.bands_len);
    CopyArrayDataSrcToDst(rilSrc.channels, sitDst.channels.eutran_channels, sitDst.channels_len);
}

void ProtocolNetworkModeBuilderLegacy::FillRasRilSrcToSitDst(const NGRAN_TYPE &,
                                                             RIL_RadioAccessSpecifier_V1_5 &rilSrc,
                                                             sit_net_radio_access_specifier &sitDst) {
    sitDst.radio_access_network = (BYTE)SIT_NET_ACCESS_RADIO_TYPE_NGRAN;
    sitDst.bands_len = MIN(MIN(rilSrc.bands_length, MAX_BANDS), SIT_MAX_NO_SSC_NGRAN_BANDS);
    sitDst.channels_len = MIN(MIN(rilSrc.channels_length, MAX_CHANNELS), SIT_MAX_NO_SSC_NGRAN_CHANNELS);
    CopyArrayDataSrcToDst(rilSrc.bands.ngran_bands, sitDst.bands.ngran_bands, sitDst.bands_len);
    CopyArrayDataSrcToDst(rilSrc.channels, sitDst.channels.ngran_channels, sitDst.channels_len);
}

/**
 * ProtocolNetworkTypeBuilderLegacy
 */
ModemData *ProtocolNetworkTypeBuilderLegacy::BuildSetPreferredNetworkType(int netType)
{
    sit_net_set_pref_network_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_PREFERRED_NTW_TYPE, length);

    req.pref_net_type = NetProtocolUtils::ConvertNetworktype2Sit(netType);

    // use SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA instead of
    // SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO (not in use)
    if (netType == PREF_NET_TYPE_NR_LTE_CDMA_EVDO) req.pref_net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA;
    else if (netType == PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA) req.pref_net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkTypeBuilderLegacy::BuildGetPreferredNetworkType()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_PREFERRED_NTW_TYPE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkTypeBuilderLegacy::BuildSetDSNetworkType(int netType)
{
    sit_net_set_ds_network_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DS_NTW_TYPE, length);
    req.pref_ds_net_type = NetProtocolUtils::ConvertDsNetworkType2Sit(netType);

    RilLogI("BuildSetDSNetworkType: netType=%d, set to %s", netType, NetProtocolUtils::ConvertSitDsNetworkType2String(req.pref_ds_net_type));

    return new ModemData((char *)&req, length);
}

ModemData* ProtocolNetworkTypeBuilderLegacy::BuildSetDualNetworkAndAllowData(
        int typeForPrimary, int typeForSecondary, int allowedForPrimary,
        int allowedForSecondary)
{
    sit_net_set_dual_network_and_allow_data_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DUAL_NTW_AND_PS_TYPE, length);

    req.pref_net_type_for_primary = NetProtocolUtils::ConvertNetworktype2Sit(typeForPrimary);
    req.pref_net_type_for_secondary = NetProtocolUtils::ConvertNetworktype2Sit(typeForSecondary);
    req.allowed_for_primary = allowedForPrimary;
    req.allowed_for_secondary = allowedForSecondary;
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolNetworkScanBiulderLegacy
 */
ModemData *ProtocolNetworkScanBuilderLegacy::BuildQueryAvailableNetwork(int ran)
{
    sit_net_get_available_networks_wit_rat_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_AVAILABLE_NETWORKS, length);
    req.ran = NetProtocolUtils::ConvertRadioAccessNetworkType2Sit(ran);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkScanBuilderLegacy::BuildCancelQueryAvailableNetwork()
{
    sit_net_cancel_get_available_networks_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_CANCEL_GET_AVAILABLE_NETWORKS, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkScanBuilderLegacy::BuildStartNetworkScan(int scantype, int timeInterval, int lenSpecifiers,
                                                            RIL_RadioAccessSpecifier_V1_5 *pSpecifiers)
{
    return this->BuildStartNetworkScan(scantype, timeInterval, lenSpecifiers, pSpecifiers,
            300, true, 3, 0, NULL);
}

ModemData *ProtocolNetworkScanBuilderLegacy::BuildStartNetworkScan(
        int scantype, int timeInterval, int specifiersLength, RIL_RadioAccessSpecifier_V1_5 *specifiers,
        int maxSearchTime, bool incrementalResults, int incrementalResultsPeriodicity,
        int numOfMccMncs, char **mccMncs) {
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

    sit_net_start_scanning_network req;
    memset(&req, 0, sizeof(req));
    // init header in later b/c data is variable
    int length = sizeof(sit_net_start_scanning_network) - MAX_NETWORK_SCAN_DATA; // default length without variable data

    req.scan_type = NetProtocolUtils::ConvertNetworkScanType2Sit(scantype);
    req.interval = 0;
    if (scantype == RIL_PERIODIC) {
        req.interval = timeInterval;
    }

    req.max_search_time = maxSearchTime;

    req.incremental_results =
            incrementalResults ? SIT_SCAN_INCREMENTAL_RESULT : SIT_SCAN_NO_INCREMENTAL_RESULT;
    if (incrementalResultsPeriodicity < 3) {
        incrementalResultsPeriodicity = 3;
    }
    else if (incrementalResultsPeriodicity > 10) {
        incrementalResultsPeriodicity = 10;
    }
    req.periodicity = incrementalResultsPeriodicity;

    // radio specifiers
    if (specifiersLength > MAX_RADIO_ACCESS_NETWORKS) {
        specifiersLength = MAX_RADIO_ACCESS_NETWORKS;
    }
    req.num_record = (BYTE)specifiersLength;

    SIT_NET_SCAN_SPECIFIER *records = (SIT_NET_SCAN_SPECIFIER *)req.data;
    for (int p = 0; p < specifiersLength; p++) {
        RIL_RadioAccessSpecifier_V1_5 *ras = &specifiers[p];
        SIT_NET_SCAN_SPECIFIER *out = &records[p];
        int networkType = NetProtocolUtils::ConvertRadioAccessNetworkType2Sit((int)ras->radio_access_network);
        // skip unknown network type
        if (networkType == SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN) {
            continue;
        }

        if (networkType == SIT_NET_ACCESS_RADIO_TYPE_GERAN) is2gAllSearch = 1;
        else if (networkType == SIT_NET_ACCESS_RADIO_TYPE_UTRAN) is3gAllSearch = 1;
        else if (networkType == SIT_NET_ACCESS_RADIO_TYPE_EUTRAN) is4gAllSearch = 1;
        //else if (networkType == SIT_NET_ACCESS_RADIO_TYPE_NGRAN) is5gAllSearch = 1;

        // bands
        out->network_type = (BYTE)networkType;
        int bandsLength = ras->bands_length;
        if (bandsLength > MAX_BANDS) {
            bandsLength = MAX_BANDS;
        }
        out->num_band = (UINT32)bandsLength;

        for (int i = 0; i < bandsLength; i++ ) {
            switch (networkType) {
            case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
                out->bands[i] = (BYTE)ras->bands.geran_bands[i];
                is2gAllSearch = 0;
                break;
            case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
                out->bands[i] = (BYTE)ras->bands.utran_bands[i];
                is3gAllSearch = 0;
                break;
            case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
                out->bands[i] = (BYTE)ras->bands.eutran_bands[i];
                is4gAllSearch = 0;
                break;
            case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
                out->bands[i] = (BYTE)ras->bands.ngran_bands[i];
                //is5gAllSearch = 0;
                break;
            }
        } // end for i ~

        // channels
        int channelsLength = ras->channels_length;
        if (channelsLength > MAX_CHANNELS) {
            channelsLength = MAX_CHANNELS;
        }
        out->num_channel = channelsLength;

        for (int i = 0; i < channelsLength; i++ ) {
            out->channels[i] = (UINT16)(ras->channels[i] & 0xFFFF);
        } // end for i ~
    } // end for p ~
    int sizeOfSpecifierData = sizeof(SIT_NET_SCAN_SPECIFIER) * specifiersLength;
    length += sizeOfSpecifierData;

    // PLMN IDs
    if (numOfMccMncs < 0) {
        numOfMccMncs = 0;
        mccMncs = NULL;
    }
    if (numOfMccMncs > MAX_NETWORK_PLMN_IDS) {
        numOfMccMncs = MAX_NETWORK_PLMN_IDS;
    }
    req.num_plmn = (BYTE)numOfMccMncs;
    SIT_NET_SCAN_PLMN_ID *plmnIds = (SIT_NET_SCAN_PLMN_ID *)(req.data + sizeOfSpecifierData);
    for (int i = 0; i < numOfMccMncs; i++) {
        char *mccMnc = mccMncs[i];
        memset(&plmnIds[i], 0, sizeof(SIT_NET_SCAN_PLMN_ID));
        if (TextUtils::IsDigitsOnly(mccMnc)) {
            int len = strlen(mccMnc);
            if (len == 5 || len == 6) {
                memcpy(plmnIds[i].mcc, mccMnc, 3);
                memcpy(plmnIds[i].mnc, mccMnc, len - 3);
                if (len == 5) {
                    plmnIds[i].mnc[2] = '#';
                }
            }
        }
    } // end for i ~
    length += sizeof(SIT_NET_SCAN_PLMN_ID) * numOfMccMncs;
    InitRequestHeader(&req.hdr, SIT_START_SCANNING_NETWORKS, length);

    // F/W does not set NGRAN on 2020.04.22. To inlcude NGRAN, the below code is added.
    // It is overwritten to 0 in order to let CP search all supported RATs when 2G/3G/4G all band is set.
    if (is2gAllSearch + is3gAllSearch + is4gAllSearch >= valueFor2g3g4gAllSearch) {
        // if CP support NGRAN, NGRAN will be searched by this settings.
        req.num_record = 0;
    }

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkScanBuilderLegacy::BuildStopNetworkScan()
{
    sit_net_start_scanning_network req;
    memset(&req, 0, sizeof(req));
    int length = sizeof(sit_net_start_scanning_network) - MAX_NETWORK_SCAN_DATA; // default length without variable data
    InitRequestHeader(&req.hdr, SIT_START_SCANNING_NETWORKS, length);
    req.scan_type = SIT_SCAN_TYPE_STOP;
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolCdmaBuilderLegacy
 */
ModemData *ProtocolCdmaBuilderLegacy::BuildSetCdmaSetRoamingType(int cdmaRoamingType)
{
    sit_net_set_cdma_roaming_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_CDMA_ROAMING_PREFERENCE, length);
    switch(cdmaRoamingType) {
    case CDMA_ROAMING_AFFILIATED_NETWORKS:
        req.cdma_roaming_type = SIT_CDMA_RM_AFFILIATED;
        break;
    case CDMA_ROAMING_ANY_NETWORK:
        req.cdma_roaming_type = SIT_CDMA_RM_ANY;
        break;
    case CDMA_ROAMING_HOME_ONLY:
        req.cdma_roaming_type = SIT_CDMA_RM_HOME;
        break;
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolCdmaBuilderLegacy::BuildQueryCdmaRoamingType()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_CDMA_ROAMING_PREFERENCE, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolCdmaBuilderLegacy::BuildSetCdmaHybridMode(int hybridMode)
{
    sit_net_set_cdma_hybrid_mode_req req;
    int length = sizeof(req);
    if (hybridMode >= (int)HYBRID_MODE_1X_HRPD && hybridMode <= (int)HYBRID_MODE_EHRPD_ONLY) {
        InitRequestHeader(&req.hdr, SIT_SET_CDMA_HYBRID_MODE, length);
        req.hybrid_mode = hybridMode;
        return new ModemData((char *)&req, length);
    }
    return NULL;
}

ModemData *ProtocolCdmaBuilderLegacy::BuildGetCdmaHybridMode()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_CDMA_HYBRID_MODE, length);
    return new ModemData((char *)&req, length);
}

/**
 * ProtocolNetworkBuilderLegacy
 */
ModemData *ProtocolNetworkBuilderLegacy::BuildGetCellInfoList()
{
    sit_net_get_cell_info_list_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_CELL_INFO_LIST , length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSetCellInfoListReportRate(int rate)
{
    sit_net_cell_info_list_report_rate_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_CELL_INFO_LIST_REPORT_RATE , length);

    // rate 0 means report SIT_SET_CELL_INFO_LIST_REPORT_RATE when information is changed.
    // rate 0x7FFFFFFF means never report SIT_IND_CELL_INFO_LIST
    if (rate < 0) {
        rate = 0x7FFFFFFF;
    }
    req.report_rate = rate;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSetRCNetworkType(int rcVersion, int rcSession, int rcPhase, int rcRaf, char *pUuid, int rcStatus)
{
    sit_net_set_rc_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_RADIO_CAPABILITY, length);

    req.version = rcVersion;
    req.session_id = rcSession;
    req.phase = rcPhase;
    req.rc_raf = switchRafValueForCP(rcRaf);

    if (pUuid != NULL) {
        int uuidLen = strlen(pUuid);
        memcpy(req.uuid, pUuid, (uuidLen > SIT_MAX_UUID_LENGTH)? SIT_MAX_UUID_LENGTH:uuidLen );
    }
    req.status = rcStatus;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildGetRCNetworkType()
{
    sit_net_get_rc_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_RADIO_CAPABILITY, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSvNumber(const char *svn) {
    sit_id_set_sv_number_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_OEM_SET_SVN, length);

    if (svn != NULL) {
        memcpy(req.sv_number, svn, sizeof(req.sv_number));
    }
    return new ModemData((char *) &req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildGetSystemSelectionChannels()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_SYSTEM_SELECTION_CHANNELS, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSetAllowedNetworkTypeBitmap(int networkTypeBitmap)
{

    sit_net_set_allow_network_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_ALLOW_NETWORK, length);
    req.rat = switchRafValueForCP(networkTypeBitmap);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildGetAllowedNetworkTypeBitmap()
{
    sit_net_get_allow_network_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_ALLOW_NETWORK, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSetPlmnList(int requestId, void *data, unsigned int datalen) {
    if (requestId < 0) {
        return NULL;
    }

    int length = sizeof(RCM_HEADER) + datalen;
    char *buf = new char[length];
    InitRequestHeader((RCM_HEADER *)buf, SIT_SET_PLMN_LIST, length);

    if (data != NULL && datalen > 0) {
        memcpy(buf + sizeof(RCM_HEADER), data, datalen);
    }

    ModemData *modemData = new ModemData(buf, length);
    delete[] buf;
    return modemData;
}

ModemData *ProtocolNetworkBuilderLegacy::BuildGetPlmnList() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_PLMN_LIST, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSetMobileDataState(int mobileData, int roamingData)
{
    sit_set_mobile_data_state_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_MOBILE_DATA_STATE, length);

    req.mobile_data = (BYTE) (mobileData & 0xFF);
    req.roaming_data = (BYTE) (roamingData & 0xFF);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildGetNitzTime() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_NITZ_TIME, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildSetDeviceService(int setting) {
    sit_net_set_device_service_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_DEVICE_SERVICE, length);

    req.usage_setting = setting;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolNetworkBuilderLegacy::BuildGetDeviceService() {
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_DEVICE_SERVICE, length);
    return new ModemData((char *)&req, length);
}
