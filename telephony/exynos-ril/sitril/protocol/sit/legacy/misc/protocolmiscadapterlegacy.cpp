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
 * protocolmiscadapterlegacy.cpp
 *
 *  Created on: 2014. 6. 30.
 *      Author: m.afzal
 */
#include "protocolmiscadapterlegacy.h"
#include "netprotocolutils.h"
#include "rillog.h"
#include "modemdata.h"
#include <sstream>
#include <iostream>
#include <iomanip>

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
 * ProtocolMiscVersionAdapterLegacy
 */
int ProtocolMiscVersionAdapterLegacy::GetMask() const
{
    int mask = 0;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            mask = data->ver_mask;
        }
    }
    return mask;
}

const char * ProtocolMiscVersionAdapterLegacy::GetSwVer() const
{
    char *swver = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            swver = data->sw_version;
            //strncpy(swver, data->sw_version, 32);
        }
    }
    return swver;
}

const char * ProtocolMiscVersionAdapterLegacy::GetHwVer() const
{
    char *hwver = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            hwver = data->hw_version;
        }
    }
    return hwver;
}

const char * ProtocolMiscVersionAdapterLegacy::GetRfCalDate() const
{
    char *rfcaldate = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            rfcaldate = data->rf_cal_date;
        }
    }
    return rfcaldate;
}

const char * ProtocolMiscVersionAdapterLegacy::GetProdCode() const
{
    char *prodcode = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            prodcode = data->product_code;
        }
    }
    return prodcode;
}

const char * ProtocolMiscVersionAdapterLegacy::GetModelID() const
{
    char *modelid = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            modelid = data->model_id;
        }
    }
    return modelid;
}

int ProtocolMiscVersionAdapterLegacy::GetPrlNamNum() const
{
    int prlnam = 0;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            prlnam = data->prl_nam_num;
        }
    }
    return prlnam;
}

const BYTE * ProtocolMiscVersionAdapterLegacy::GetPrlVersion() const
{
    BYTE *prlver = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            prlver = data->prl_version;
        }
    }
    return prlver;
}

int ProtocolMiscVersionAdapterLegacy::GetEriNamNum() const
{
    int erinam = 0;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            erinam = data->eri_nam_num;
        }
    }
    return erinam;
}

const BYTE * ProtocolMiscVersionAdapterLegacy::GetEriVersion() const
{
    BYTE *eriver = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            eriver = data->eri_version;
        }
    }
    return eriver;
}

const BYTE * ProtocolMiscVersionAdapterLegacy::GetCPChipSet() const
{
    BYTE *cpchip = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_baseband_version_rsp *data = (sit_misc_get_baseband_version_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BASEBAND_VERSION) {
            cpchip = data->cp_chipsetname;
        }
    }
    return cpchip;
}

/**
 * ProtocolMiscGetTtyAdapterLegacy
 */
int ProtocolMiscGetTtyAdapterLegacy::GetTtyMode() const
{
    int ttymode = SIT_MISC_TTY_MODE_OFF;
    if (m_pModemData != NULL) {
        sit_misc_get_tty_mode_rsp *data = (sit_misc_get_tty_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_TTY_MODE) {
            ttymode = data->tty_mode;
        }
    }
    return ttymode;
}

/**
 * ProtocolSignalStrengthAdapterLegacy
 */
ProtocolSignalStrengthAdapterLegacy::ProtocolSignalStrengthAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
    memset(&mSignalStrength, 0, sizeof(mSignalStrength));
}

RIL_SignalStrength_V1_6& ProtocolSignalStrengthAdapterLegacy::GetSignalStrength(int ssVer)
{
    if (m_pModemData != NULL) {
        sit_misc_signal_strength_rsp_v4 *response = (sit_misc_signal_strength_rsp_v4 *)m_pModemData->GetRawData();
        if (response != NULL && response->hdr.id == SIT_GET_SIGNAL_STRENGTH) {
            int dataLen = GetParameterLength() - sizeof(INT16);

            SignalStrengthAdapterFactory signalStrengthAdapterFactory;
            SignalStrengthAdapter *signalStrengthAdapter = signalStrengthAdapterFactory.GetSignalStrengthAdapter(ssVer);
            if (signalStrengthAdapter != NULL) {
                signalStrengthAdapter->Init(&response->sig_strength, dataLen, response->valid_rat_sig_flag);
                mSignalStrength = signalStrengthAdapter->GetSignalStrength();
            }
        }
    }
    return mSignalStrength;
}

/*
 * SignalStrengthAdapter
 */
SignalStrengthAdapter::SignalStrengthAdapter() : mData(NULL), mDataLen(0), mMask(0)
{
    memset(&mSignalStrength, 0, sizeof(mSignalStrength));
}

 SignalStrengthAdapter::~SignalStrengthAdapter() {
     if (mData) {
         delete[] (char *)mData;
     }
 }

void SignalStrengthAdapter::Init(void *data, int dataLen, int mask)
{
    if (data != NULL && dataLen > 0) {
        mData = new char[dataLen];
        if (mData != NULL) {
            memcpy(mData, data, dataLen);
        }
    }
    mDataLen = dataLen;
    mMask = mask;

    // init as default
    FillDefaultSignalStrength();
}

void SignalStrengthAdapter::FillDefaultSignalStrength(RIL_SignalStrength_V1_6& out)
{
    out.GSM_SignalStrength.signalStrength = 99;
    out.GSM_SignalStrength.bitErrorRate = 99;
    out.GSM_SignalStrength.timingAdvance = INT_MAX;

    out.WCDMA_SignalStrength.signalStrength = 99;
    out.WCDMA_SignalStrength.bitErrorRate = 99;
    out.WCDMA_SignalStrength.rscp = 255;
    out.WCDMA_SignalStrength.ecno = 255;

    out.CDMA_SignalStrength.dbm = -1;
    out.CDMA_SignalStrength.ecio = -1;

    out.EVDO_SignalStrength.dbm = -1;
    out.EVDO_SignalStrength.ecio = -1;
    out.EVDO_SignalStrength.signalNoiseRatio = INT_MAX;

    out.LTE_SignalStrength.signalStrength = INT_MAX;
    out.LTE_SignalStrength.rsrp = INT_MAX;
    out.LTE_SignalStrength.rsrq = INT_MAX;
    out.LTE_SignalStrength.rssnr = INT_MAX;
    out.LTE_SignalStrength.cqiTableIndex = INT_MAX;
    out.LTE_SignalStrength.cqi = INT_MAX;
    out.LTE_SignalStrength.timingAdvance = INT_MAX;

    out.TD_SCDMA_SignalStrength.signalStrength = 99;
    out.TD_SCDMA_SignalStrength.bitErrorRate = 99;
    out.TD_SCDMA_SignalStrength.rscp = 255;

    out.NR_SignalStrength.ssRsrp = INT_MAX;
    out.NR_SignalStrength.ssRsrq = INT_MAX;
    out.NR_SignalStrength.ssSinr = INT_MAX;
    out.NR_SignalStrength.csiRsrp = INT_MAX;
    out.NR_SignalStrength.csiRsrq = INT_MAX;
    out.NR_SignalStrength.csiSinr = INT_MAX;
    out.NR_SignalStrength.csiCqiTableIndex = INT_MAX;
    out.NR_SignalStrength.csiCqiReportLen = 0;
}

void SignalStrengthAdapter::FillGwSignalStrength(RIL_GSM_SignalStrength_v12& out, GW_SIGNAL_STRENGTH& currentGWSignalStrength)
{
    out.signalStrength = currentGWSignalStrength.sig_str;
    out.bitErrorRate = currentGWSignalStrength.ber;
    out.timingAdvance = INT_MAX;
}

void SignalStrengthAdapter::FillGwSignalStrength(RIL_WCDMA_SignalStrength_V1_2& out, GW_SIGNAL_STRENGTH& currentGWSignalStrength)
{
    out.signalStrength = currentGWSignalStrength.sig_str;
    out.bitErrorRate = currentGWSignalStrength.ber;
    out.rscp = 255;
    out.ecno = 255;
}

void SignalStrengthAdapter::FillGsmSignalStrength(RIL_GSM_SignalStrength_v12& out, GSM_SIGNAL_STRENGTH& currentGsmSignalStrength)
{
    out.signalStrength = currentGsmSignalStrength.sig_str;
    out.bitErrorRate = currentGsmSignalStrength.ber;
    out.timingAdvance = currentGsmSignalStrength.ta;
}

void SignalStrengthAdapter::FillWcdmaSignalStrength(RIL_WCDMA_SignalStrength_V1_2& out, WCDMA_SIGNAL_STRENGTH& currentWcdmaSignalStrength)
{
    out.signalStrength = currentWcdmaSignalStrength.sig_str;
    out.bitErrorRate = currentWcdmaSignalStrength.ber;
    out.rscp = currentWcdmaSignalStrength.rscp;
    out.ecno = currentWcdmaSignalStrength.ecno;
}

void SignalStrengthAdapter::FillCdmaSignalStrength(RIL_CDMA_SignalStrength& out, CDMA_SIGNAL_STRENGTH& currentCdmaSignalStrength)
{
    out.dbm = currentCdmaSignalStrength.dbm;
    out.ecio = currentCdmaSignalStrength.ecio;
}

void SignalStrengthAdapter::FillEvdoSignalStrength(RIL_EVDO_SignalStrength& out, EVDO_SIGNAL_STRENGTH& currentEvdoSignalStrength)
{
    out.dbm = currentEvdoSignalStrength.dbm;
    out.ecio = currentEvdoSignalStrength.ecio;
    out.signalNoiseRatio = currentEvdoSignalStrength.snr;
}

void SignalStrengthAdapter::FillLteSignalStrength(RIL_LTE_SignalStrength_V1_6& out, LTE_SIGNAL_STRENGTH& currentLteSignalStrength)
{
    out.signalStrength = currentLteSignalStrength.sig_str;
    out.rsrp = currentLteSignalStrength.rsrp;
    out.rsrq = currentLteSignalStrength.rsrq;
    out.rssnr = currentLteSignalStrength.rssnr;
    out.cqi = currentLteSignalStrength.cqi;
    out.timingAdvance = currentLteSignalStrength.timing_adv;
}

void SignalStrengthAdapter::FillLteSignalStrengthV4(RIL_LTE_SignalStrength_V1_6& out, LTE_SIGNAL_STRENGTH_V4& currentLteSignalStrength)
{
    out.signalStrength = currentLteSignalStrength.sig_str;
    out.rsrp = currentLteSignalStrength.rsrp;
    out.rsrq = currentLteSignalStrength.rsrq;
    out.rssnr = currentLteSignalStrength.rssnr;
    out.cqi = currentLteSignalStrength.cqi;
    out.timingAdvance = currentLteSignalStrength.timing_adv;
    out.cqiTableIndex = currentLteSignalStrength.cqi_table_index;
}

void SignalStrengthAdapter::FillTdscdmaSignalStrength(RIL_TD_SCDMA_SignalStrength_V1_2& out, TD_SCDMA_SIGNAL_STRENGTH& currentTdscdmaSignalStrength)
{
    out.signalStrength = 99;
    out.bitErrorRate = 99;
    out.rscp = currentTdscdmaSignalStrength.rscp;
}

void SignalStrengthAdapter::FillTdscdmaSignalStrengthV2(RIL_TD_SCDMA_SignalStrength_V1_2& out, TD_SCDMA_SIGNAL_STRENGTH_V2& currentTdscdmaSignalStrength)
{
    out.signalStrength = currentTdscdmaSignalStrength.sig_str;
    out.bitErrorRate = currentTdscdmaSignalStrength.ber;
    out.rscp = currentTdscdmaSignalStrength.rscp;
}

void SignalStrengthAdapter::FillNrSignalStrengthV2(RIL_NR_SignalStrength_V1_6& out, NR_SIGNAL_STRENGTH& currentNrSignalStrength)
{
    out.ssRsrp = currentNrSignalStrength.ss_rsrp;
    out.ssRsrq = currentNrSignalStrength.ss_rsrq;
    out.ssSinr = currentNrSignalStrength.ss_sinr;
    out.csiRsrp = currentNrSignalStrength.csi_rsrp;
    out.csiRsrq = currentNrSignalStrength.csi_rsrq;
    out.csiSinr = currentNrSignalStrength.csi_sinr;
}

void SignalStrengthAdapter::FillNrSignalStrengthV4(RIL_NR_SignalStrength_V1_6& out, NR_SIGNAL_STRENGTH_V4& currentNrSignalStrength)
{
    out.ssRsrp = currentNrSignalStrength.ss_rsrp;
    out.ssRsrq = currentNrSignalStrength.ss_rsrq;
    out.ssSinr = currentNrSignalStrength.ss_sinr;
    out.csiRsrp = currentNrSignalStrength.csi_rsrp;
    out.csiRsrq = currentNrSignalStrength.csi_rsrq;
    out.csiSinr = currentNrSignalStrength.csi_sinr;
    out.csiCqiTableIndex = currentNrSignalStrength.cqi_table_index;
    if (currentNrSignalStrength.wb_cqi != INT_MAX) {
        out.csiCqiReportLen = 1;
        out.csiCqiReport[0] = currentNrSignalStrength.wb_cqi;
    } else {
        out.csiCqiReportLen = SIT_MAX_NR_SUBBAND_INDEX;
        memcpy(out.csiCqiReport, currentNrSignalStrength.sb_cqi, sizeof(unsigned int) * SIT_MAX_NR_SUBBAND_INDEX);
    }
}

void SignalStrengthAdapter::FillDefaultSignalStrength()
{
    FillDefaultSignalStrength(mSignalStrength);
}

void SignalStrengthAdapter::FillGwSignalStrength(GW_SIGNAL_STRENGTH& currentGWSignalStrength)
{
    FillGwSignalStrength(mSignalStrength.GSM_SignalStrength, currentGWSignalStrength);
    FillGwSignalStrength(mSignalStrength.WCDMA_SignalStrength, currentGWSignalStrength);
}

void SignalStrengthAdapter::FillGsmSignalStrength(GSM_SIGNAL_STRENGTH& currentGsmSignalStrength)
{
    FillGsmSignalStrength(mSignalStrength.GSM_SignalStrength, currentGsmSignalStrength);
}

void SignalStrengthAdapter::FillWcdmaSignalStrength(WCDMA_SIGNAL_STRENGTH& currentWcdmaSignalStrength)
{
    FillWcdmaSignalStrength(mSignalStrength.WCDMA_SignalStrength, currentWcdmaSignalStrength);
}

void SignalStrengthAdapter::FillCdmaSignalStrength(CDMA_SIGNAL_STRENGTH& currentCdmaSignalStrength)
{
    FillCdmaSignalStrength(mSignalStrength.CDMA_SignalStrength, currentCdmaSignalStrength);
}

void SignalStrengthAdapter::FillEvdoSignalStrength(EVDO_SIGNAL_STRENGTH& currentEvdoSignalStrength)
{
    FillEvdoSignalStrength(mSignalStrength.EVDO_SignalStrength, currentEvdoSignalStrength);
}

void SignalStrengthAdapter::FillLteSignalStrength(LTE_SIGNAL_STRENGTH& currentLteSignalStrength)
{
    FillLteSignalStrength(mSignalStrength.LTE_SignalStrength, currentLteSignalStrength);
}

void SignalStrengthAdapter::FillLteSignalStrengthV4(LTE_SIGNAL_STRENGTH_V4& currentLteSignalStrength)
{
    FillLteSignalStrengthV4(mSignalStrength.LTE_SignalStrength, currentLteSignalStrength);
}

void SignalStrengthAdapter::FillTdscdmaSignalStrength(TD_SCDMA_SIGNAL_STRENGTH& currentTdscdmaSignalStrength)
{
    FillTdscdmaSignalStrength(mSignalStrength.TD_SCDMA_SignalStrength, currentTdscdmaSignalStrength);
}

void SignalStrengthAdapter::FillTdscdmaSignalStrengthV2(TD_SCDMA_SIGNAL_STRENGTH_V2& currentTdscdmaSignalStrength)
{
    FillTdscdmaSignalStrengthV2(mSignalStrength.TD_SCDMA_SignalStrength, currentTdscdmaSignalStrength);
}

void SignalStrengthAdapter::FillNrSignalStrengthV2(NR_SIGNAL_STRENGTH& currentNrSignalStrength)
{
    FillNrSignalStrengthV2(mSignalStrength.NR_SignalStrength, currentNrSignalStrength);
}

void SignalStrengthAdapter::FillNrSignalStrengthV4(NR_SIGNAL_STRENGTH_V4& currentNrSignalStrength)
{
    FillNrSignalStrengthV4(mSignalStrength.NR_SignalStrength, currentNrSignalStrength);
}

/*
 * SignalStrengthParserV1
 */
RIL_SignalStrength_V1_6& SignalStrengthParserV1::GetSignalStrength()
{
    FillSignalStrength();
    return mSignalStrength;
}

void SignalStrengthParserV1::FillSignalStrength()
{
    if (mData == NULL || mDataLen <= 0) {
        RilLogW("Invalid Parameter data length=%d", mDataLen);
        return;
    }

    SIGNAL_STRENGTH *p_cur = (SIGNAL_STRENGTH *)mData;
    if ((mMask & SIT_MISC_SIG_RAT_SIG_GW)/* && !umtsFlag*/) {
        FillGwSignalStrength(p_cur->GW_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_CDMA) {
        FillCdmaSignalStrength(p_cur->CDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_EVDO) {
        FillEvdoSignalStrength(p_cur->EVDO_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_LTE) {
        FillLteSignalStrength(p_cur->LTE_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_TD_SCDMA) {
        FillTdscdmaSignalStrength(p_cur->TD_SCDMA_SignalStrength);
    }
}

/*
 * SignalStrengthParserV2
 */
RIL_SignalStrength_V1_6& SignalStrengthParserV2::GetSignalStrength()
{
    FillSignalStrength();
    return mSignalStrength;
}

void SignalStrengthParserV2::FillSignalStrength()
{
    if (mData == NULL || mDataLen <= 0) {
        RilLogW("Invalid Parameter data length=%d", mDataLen);
        return;
    }

    SIGNAL_STRENGTH_V2 *p_cur = (SIGNAL_STRENGTH_V2 *)mData;
    if (mMask & SIT_MISC_SIG_RAT_SIG_GSM) {
        FillGsmSignalStrength(p_cur->GSM_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_WCDMA) {
        FillWcdmaSignalStrength(p_cur->WCDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
        FillCdmaSignalStrength(p_cur->CDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
        FillEvdoSignalStrength(p_cur->EVDO_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
        FillLteSignalStrength(p_cur->LTE_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
        FillTdscdmaSignalStrengthV2(p_cur->TD_SCDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
        FillNrSignalStrengthV2(p_cur->NR_SignalStrength);
    }
}

/*
 * SignalStrengthParserV4
 */
RIL_SignalStrength_V1_6& SignalStrengthParserV4::GetSignalStrength()
{
    FillSignalStrength();
    return mSignalStrength;
}

void SignalStrengthParserV4::FillSignalStrength()
{
    if (mData == NULL || mDataLen <= 0) {
        RilLogW("Invalid Parameter data length=%d", mDataLen);
        return;
    }

    SIGNAL_STRENGTH_V4 *p_cur = (SIGNAL_STRENGTH_V4 *) mData;
    if (mMask & SIT_MISC_SIG_RAT_SIG_GSM) {
        FillGsmSignalStrength(p_cur->GSM_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_WCDMA) {
        FillWcdmaSignalStrength(p_cur->WCDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_CDMA_V1_4) {
        FillCdmaSignalStrength(p_cur->CDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_EVDO_V1_4) {
        FillEvdoSignalStrength(p_cur->EVDO_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_LTE_V1_4) {
        FillLteSignalStrengthV4(p_cur->LTE_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_TD_SCDMA_V1_4) {
        FillTdscdmaSignalStrengthV2(p_cur->TD_SCDMA_SignalStrength);
    }

    if (mMask & SIT_MISC_SIG_RAT_SIG_NR_V1_4) {
        FillNrSignalStrengthV4(p_cur->NR_SignalStrength);
    }
}

/*
 * SignalStrengthAdapterFactory
 */
SignalStrengthAdapter *SignalStrengthAdapterFactory::GetSignalStrengthAdapter(int ssVer)
{
    RilLogI("SignalStrengthAdapterFactory::GetSignalStrengthAdapter ssVer=%d", ssVer);
    switch (ssVer) {
        case SIGNAL_STRENGTH_PROTOCOL_V1:
            return &mSignalStrengthV1;
        case SIGNAL_STRENGTH_PROTOCOL_V2:
        case SIGNAL_STRENGTH_PROTOCOL_V3: // struct is same as version 2
            return &mSignalStrengthV2;
        case SIGNAL_STRENGTH_PROTOCOL_V4:
            return &mSignalStrengthV4;
    }
    return &mSignalStrengthV2;;
}

/**
 * ProtocolSignalStrengthIndAdapterLegacy
 */
ProtocolSignalStrengthIndAdapterLegacy::ProtocolSignalStrengthIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
    memset(&mSignalStrength, 0, sizeof(mSignalStrength));
}

RIL_SignalStrength_V1_6& ProtocolSignalStrengthIndAdapterLegacy::GetSignalStrength(int ssVer)
{
    if (m_pModemData != NULL) {
        sit_misc_signal_strength_ind_v4 *response = (sit_misc_signal_strength_ind_v4 *)m_pModemData->GetRawData();
        if (response->hdr.id == SIT_IND_SIGNAL_STRENGTH) {
            int dataLen = GetParameterLength() - sizeof(INT16);

            SignalStrengthAdapterFactory signalStrengthAdapterFactory;
            SignalStrengthAdapter *signalStrengthAdapter = signalStrengthAdapterFactory.GetSignalStrengthAdapter(ssVer);
            if (signalStrengthAdapter != NULL) {
                signalStrengthAdapter->Init(&response->sig_strength, dataLen, response->valid_rat_sig_flag);
                mSignalStrength = signalStrengthAdapter->GetSignalStrength();
            }
        }
    }
    return mSignalStrength;
}

/**
 * ProtocolNitzData
 */
int ProtocolNitzData::TimeInfoType() const { return 0; }
int ProtocolNitzData::DayLightValid() const {
    int daylightvalid = SIT_NITZ_DAYLIGHT_INFO_VALID;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        daylightvalid = nitzData->daylight_valid;
    }
    return daylightvalid;
}

int ProtocolNitzData::Year() const {
    int year = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        year = nitzData->year;
    }
    return year;
}
int ProtocolNitzData::Month() const {
    int month = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        month = nitzData->month;
    }
    return month;
}
int ProtocolNitzData::Day() const {
    int day = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        day = nitzData->day;
    }
    return day;
}
int ProtocolNitzData::Hour() const {
    int hour = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        hour = nitzData->hour;
    }
    return hour;
}
int ProtocolNitzData::Minute() const {
    int minute = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        minute = nitzData->minute;
    }
    return minute;
}
int ProtocolNitzData::Second() const {
    int second = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        second = nitzData->second;
    }
    return second;
}
int ProtocolNitzData::TimeZone() const {
    #define BITSIZE 8
    #define SIGNFLAG (1<<(BITSIZE-1))
    #define DATABITS (SIGNFLAG-1)

    int timezone = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        timezone = nitzData->time_zone;
        if ((timezone& SIGNFLAG)!=0) {                    // signflag set
            timezone= (~timezone & DATABITS) + 1;         // 2s complement without signflag
            timezone= -timezone;                          // negative number
        }
    }
    return timezone;
}

int ProtocolNitzData::DayLightAdjust() const {
    int daylightadj = SIT_NITZ_DAYLIGHT_ADJUST_NOADJUST;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        daylightadj = nitzData->daylight_adjust;
    }
    return daylightadj;
}
int ProtocolNitzData::DayofWeek() const {
    int dayofweek = SIT_NITZ_DAY_OF_WEEK_SUN;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        dayofweek = nitzData->day_of_week;
    }
    return dayofweek;
}

int ProtocolNitzData::GetMMInfo() const {
    int mminfo = 0;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        mminfo = nitzData->mminfo;
    }
    return mminfo;
}
const BYTE *ProtocolNitzData::GetPLMN() const {
    BYTE *plmn = NULL;
    auto nitzData = (sit_nitz_time *)data;
    if (nitzData && size >= sizeof(sit_nitz_time)) {
        plmn = nitzData->plmn;
    }
    return plmn;
}
int64_t ProtocolNitzData::age() const {
    int64_t age = 0;
    auto nitzData = (sit_nitz_time_with_age *)data;
    if (nitzData && size >= sizeof(sit_nitz_time_with_age)) {
        age = nitzData->age;
    }
    return age;
}

/**
 * ProtocolMiscNITZTimeAdapterLegacy
 */
ProtocolMiscNITZTimeAdapterLegacy::ProtocolMiscNITZTimeAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    mNitzData.setNitzData(GetParameter(), GetParameterLength());
}

/**
 * ProtocolMiscGetNITZTimeAdapterLegacy
 */
ProtocolMiscGetNITZTimeAdapterLegacy::ProtocolMiscGetNITZTimeAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData) {
    mNitzData.setNitzData(GetParameter(), GetParameterLength());
}

/**
 * ProtocolMiscIMEIAdapterLegacy
 */
int ProtocolMiscIMEIAdapterLegacy::GetIMEILen() const
{
    int imeilen = 0;
    if (m_pModemData != NULL) {
        sit_id_get_imei_rsp *data = (sit_id_get_imei_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_IMEI) {
            imeilen = data->imei_len;
        }
    }
    return imeilen;
}

const BYTE * ProtocolMiscIMEIAdapterLegacy::GetIMEI() const
{
    BYTE *imei = NULL;
    if (m_pModemData != NULL) {
        sit_id_get_imei_rsp *data = (sit_id_get_imei_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_IMEI) {
            imei = data->imei;
        }
    }
    return imei;
}

/**
 * ProtocolMiscIMEISVAdapterLegacy
 */
int ProtocolMiscIMEISVAdapterLegacy::GetIMEISVLen() const
{
    int imeisvlen = 0;
    if (m_pModemData != NULL) {
        sit_id_get_imeisv_rsp *data = (sit_id_get_imeisv_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_IMEISV) {
            imeisvlen = data->imeisv_len;
        }
    }
    return imeisvlen;
}

const BYTE * ProtocolMiscIMEISVAdapterLegacy::GetIMEISV() const
{
    BYTE *imeisv = NULL;
    if (m_pModemData != NULL) {
        sit_id_get_imeisv_rsp *data = (sit_id_get_imeisv_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_IMEISV) {
            imeisv = data->imeisv;
        }
    }
    return imeisv;
}

/**
 * ProtocolMiscDeviceIDAdapterLegacy
 */
int ProtocolMiscDeviceIDAdapterLegacy::GetIMEILen() const
{
    int imeilen = 0;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            imeilen = data->imei_len;
        }
    }
    return imeilen;
}

const BYTE * ProtocolMiscDeviceIDAdapterLegacy::GetIMEI() const
{
    BYTE *imei = NULL;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            imei = data->imei;
        }
    }
    return imei;
}

int ProtocolMiscDeviceIDAdapterLegacy::GetIMEISVLen() const
{
    int imeisvlen = 0;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            imeisvlen = data->imeisv_len;
        }
    }
    return imeisvlen;
}

const BYTE * ProtocolMiscDeviceIDAdapterLegacy::GetIMEISV() const
{
    BYTE *imeisv = NULL;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            imeisv = data->imesv;
        }
    }
    return imeisv;
}

int ProtocolMiscDeviceIDAdapterLegacy::GetMEIDLen() const
{
    int meidlen = 0;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            meidlen = data->meid_len;
        }
    }
    return meidlen;
}

const BYTE * ProtocolMiscDeviceIDAdapterLegacy::GetMEID() const
{
    BYTE *meid = NULL;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            meid = data->meid;
        }
    }
    return meid;
}

int ProtocolMiscDeviceIDAdapterLegacy::GetESNLen() const
{
    int esnlen = 0;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            esnlen = data->esn_len;
        }
    }
    return esnlen;
}

const BYTE * ProtocolMiscDeviceIDAdapterLegacy::GetESN() const
{
    BYTE *esn = NULL;
    if (m_pModemData != NULL) {
        sit_id_get_deviceid_rsp *data = (sit_id_get_deviceid_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_ID) {
            esn = data->esn;
        }
    }
    return esn;
}

/**
 * ProtocolMiscPhoneResetAdapterLegacy
 */
ProtocolMiscPhoneResetAdapterLegacy::ProtocolMiscPhoneResetAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
    m_ResetType = SIT_PWR_RESET_TYPE_PHONE_ONLY;
    m_ResetCause = 0;    //TBD

    if (m_pModemData != NULL) {
        sit_pwr_phone_reset_ind *data = (sit_pwr_phone_reset_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_PHONE_RESET) ) {
            m_ResetType = data->reset_type;
            m_ResetCause = data->reset_cause;
        }
    }
}

BYTE ProtocolMiscPhoneResetAdapterLegacy::GetResetType()
{
    return m_ResetType;
}

BYTE ProtocolMiscPhoneResetAdapterLegacy::GetResetCause()
{
    return m_ResetCause;
}

/**
 * ProtocolMiscDataStateChangeAdapterLegacy
 */
ProtocolMiscDataStateChangeAdapterLegacy::ProtocolMiscDataStateChangeAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
    m_ExpectedState = 0;

    if (m_pModemData != NULL) {
        sit_pdp_data_state_change_ind *data = (sit_pdp_data_state_change_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_DATA_STATE_CHANGE) ) {
            m_ExpectedState = data->expected_state;
        }
    }
}

BYTE ProtocolMiscDataStateChangeAdapterLegacy::GetExpectedState()
{
    return m_ExpectedState;
}

/**
 * ProtocolMiscGetActivityInfoAdapterLegacy
 */
UINT32 ProtocolMiscGetActivityInfoAdapterLegacy::GetSleepPeriod() const
{
    sit_misc_get_activity_info_rsp *data = (sit_misc_get_activity_info_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_GET_ACTIVITY_INFO) {
        return data->sleep_mode_time_ms;
    }
    return 0;
}

UINT32 ProtocolMiscGetActivityInfoAdapterLegacy::GetIdlePeriod() const
{
    sit_misc_get_activity_info_rsp *data = (sit_misc_get_activity_info_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_GET_ACTIVITY_INFO) {
        return data->idle_mode_time_ms;
    }
    return 0;
}

UINT32* ProtocolMiscGetActivityInfoAdapterLegacy::GetTxPeriod() const
{
    sit_misc_get_activity_info_rsp *data = (sit_misc_get_activity_info_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_GET_ACTIVITY_INFO) {
        return data->tx_mode_time_ms;
    }
    return 0;
}

UINT32 ProtocolMiscGetActivityInfoAdapterLegacy::GetRxPeriod() const
{
    sit_misc_get_activity_info_rsp *data = (sit_misc_get_activity_info_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_GET_ACTIVITY_INFO) {
        return data->rx_mode_time_ms;
    }
    return 0;
}

/**
 * ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy
 */
using android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsTechSpecificInfo;

ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy::ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy(
        const ModemData *pModemData) : ProtocolRespAdapter(pModemData) {
    mInfo = {};
    init();
}
void ProtocolMiscGetActivityInfoRatSpecificAdapterLegacy::init() {
    if (GetErrorCode() == RIL_E_SUCCESS) {
        if (GetLength() >= sizeof(sit_misc_get_activity_info_rat_specific_rsp)) {
            sit_misc_get_activity_info_rat_specific_rsp *info =
                    (sit_misc_get_activity_info_rat_specific_rsp *)m_pModemData->GetRawData();
            // common
            mInfo.sleepModeTimeMs = info->activity_info.sleepModeTime;
            mInfo.idleModeTimeMs = info->activity_info.idleModeTime;

            // TechSpecificInfo
            constexpr RIL_RadioAccessNetworks ran[] = {
                RIL_RAN_GERAN,
                RIL_RAN_UTRAN,
                RIL_RAN_EUTRAN,
                (RIL_RadioAccessNetworks)ACCESS_NETWORK_NGRAN,
                (RIL_RadioAccessNetworks)ACCESS_NETWORK_NGRAN,
            };
            constexpr int32_t frequencyRange[] = {
                FREQUENCY_RANGE_UNKNOWN,
                FREQUENCY_RANGE_UNKNOWN,
                FREQUENCY_RANGE_UNKNOWN,
                FREQUENCY_RANGE_HIGH,
                FREQUENCY_RANGE_MMWAVE,
            };
            for (size_t ratPos = 0; ratPos < ACTIVITY_INFO_RAT_MAX; ratPos++) {
                RIL_ActivityStatsTechSpecificInfo techSpecificInfo = { };
                techSpecificInfo.rat = ran[ratPos];
                techSpecificInfo.frequencyRange = frequencyRange[ratPos];
                const sit_rat_statiscts& ratStats =
                                        info->activity_info.ratSpecificInfo[ratPos];
                for (size_t i = 0; i < TX_MODE_RANGE_MAX; i++) {
                    techSpecificInfo.txmModetimeMs.push_back(ratStats.txModeTime[i]);
                }
                techSpecificInfo.rxModeTimeMs = ratStats.rxModeTime;
                mInfo.techSpecificInfo.push_back(techSpecificInfo);
            }
        } else {
            // legacy
            ProtocolMiscGetActivityInfoAdapterLegacy adapter(m_pModemData);
            mInfo.sleepModeTimeMs = (int32_t)adapter.GetSleepPeriod();
            mInfo.idleModeTimeMs = (int32_t)adapter.GetIdlePeriod();
            RIL_ActivityStatsTechSpecificInfo techInfo =
                    { RIL_RAN_UNKNOWN, FREQUENCY_RANGE_UNKNOWN, {}, (int32_t)adapter.GetRxPeriod() };
            techInfo.txmModetimeMs.resize(TX_MODE_RANGE_MAX);
            for (size_t i = 0; i < TX_MODE_RANGE_MAX; i++) {
                techInfo.txmModetimeMs[i] = ((int)adapter.GetTxPeriod()[i]);
            }
            mInfo.techSpecificInfo.push_back(techInfo);
        }
    }
}

/**
 * ProtocolMiscGetMslCodeAdapterLegacy
 */
const char * ProtocolMiscGetMslCodeAdapterLegacy::getMslCode() const
{
    char *msl = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_msl_code_rsp *data = (sit_misc_get_msl_code_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_MSL_CODE) {
            msl = data->msl_code;
        }
    }
    return msl;
}

/**
 * ProtocolMiscPinControlAdapterLegacy
 */
ProtocolMiscPinControlAdapterLegacy::ProtocolMiscPinControlAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
    m_Signal = 0;
    m_Status = 0;

    if (m_pModemData != NULL) {
        sit_misc_pin_control_ind *data = (sit_misc_pin_control_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_PIN_CONTROL) ) {
            m_Signal = data->signal;
            m_Status = data->status;
        }
    }
}

BYTE ProtocolMiscPinControlAdapterLegacy::GetSignal()
{
    return m_Signal;
}

BYTE ProtocolMiscPinControlAdapterLegacy::GetStatus()
{
    return m_Status;
}

/**
 * ProtocolGetVoiceOperationAdapterLegacy
 */
int ProtocolGetVoiceOperationAdapterLegacy::GetMode() const
{
    if (m_pModemData != NULL) {
        sit_misc_get_voice_operation_rsp *data = (sit_misc_get_voice_operation_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_VOICE_OPERATION) ) {
            return data->mode > 0 ? 1 : 0;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetPreferredCallCapability
 */
int ProtocolMiscGetPreferredCallCapabilityAdapterLegacy::GetMode() const
{
    if (m_pModemData != NULL) {
        sit_misc_get_preferred_call_capability_rsp *data = (sit_misc_get_preferred_call_capability_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_PREFERRED_CALL_CAPABILITY) ) {
            return data->mode;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetManualBandModeAdapterLegacy
 */
BYTE ProtocolMiscSetManualBandModeAdapterLegacy::GetCause()
{
    if (m_pModemData != NULL) {
        sit_misc_set_manual_band_mode_rsp *data = (sit_misc_set_manual_band_mode_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_SET_MANUAL_BAND_MODE) ) {
            return data->cause;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetRfDesenseModeAdapterLegacy
 */
BYTE ProtocolMiscSetRfDesenseModeAdapterLegacy::GetCause()
{
    if (m_pModemData != NULL) {
        sit_misc_set_rf_desense_mode_rsp *data = (sit_misc_set_rf_desense_mode_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_SET_RF_DESENSE_MODE) ) {
            return data->cause;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetHwConfigAdapterLegacy
 */
int ProtocolMiscGetHwConfigAdapterLegacy::GetNum() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_misc_get_hw_config_rsp *data = (sit_misc_get_hw_config_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_HW_CONFIG) ) {
            ret = data->num_recodrs;
        }
    }

    return ret;
}

int ProtocolMiscGetHwConfigAdapterLegacy::GetData(RIL_HardwareConfig *pRsp, int num) const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_misc_get_hw_config_rsp *data = (sit_misc_get_hw_config_rsp *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_GET_HW_CONFIG) && pRsp != NULL) {
            for(int index = 0; index < num; index++) {
                RIL_HardwareConfig *pWrite = pRsp + index;
                SIT_HW_CONFIG *pRead = (data->hw_config) + index;

                pWrite->type = (RIL_HardwareConfig_Type)pRead->type;
                pWrite->state = (RIL_HardwareConfig_State)pRead->state;
                pWrite->uuid[MAX_UUID_LENGTH - 1] = 0;
                strncpy(pWrite->uuid, pRead->uuid, MAX_UUID_LENGTH-1);

                memset(&(pWrite->cfg), 0, sizeof(pWrite->cfg));
                if (pWrite->type == RIL_HARDWARE_CONFIG_MODEM) {
                    pWrite->cfg.modem.rilModel = pRead->cfg.modem.ril_model;
                    pWrite->cfg.modem.rat = NetProtocolUtils::ConvertSitRaf2Ril(pRead->cfg.modem.rat, true);
                    pWrite->cfg.modem.maxVoice = pRead->cfg.modem.max_voice;
                    pWrite->cfg.modem.maxData = pRead->cfg.modem.max_data;
                    pWrite->cfg.modem.maxStandby = pRead->cfg.modem.max_standby;
                } else if (pWrite->type == RIL_HARDWARE_CONFIG_SIM) {
                    strncpy(pWrite->cfg.sim.modemUuid, pRead->cfg.sim.modem_uuid, MAX_UUID_LENGTH-1);
                } else {
                    RilLogE("(GetHwConfigAdapter)Invalid HW CFG type %d", pWrite->type);
                }
           }
        }
    }

    return ret;
}

/**
 * ProtocolMiscHwConfigChangeAdapterLegacy
 */
int ProtocolMiscHwConfigChangeAdapterLegacy::GetNum() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_misc_hw_config_change_ind *data = (sit_misc_hw_config_change_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_HW_CONFIG_CHANGED) ) {
            ret = data->num_recodrs;
        }
    }

    return ret;
}

int ProtocolMiscHwConfigChangeAdapterLegacy::GetData(RIL_HardwareConfig *pRsp, int num) const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_misc_hw_config_change_ind *data = (sit_misc_hw_config_change_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_HW_CONFIG_CHANGED) && pRsp != NULL) {
            for(int index = 0; index < num; index++) {
                RIL_HardwareConfig *pWrite = pRsp + index;
                SIT_HW_CONFIG *pRead = (data->hw_config) + index;

                pWrite->type = (RIL_HardwareConfig_Type)pRead->type;
                pWrite->state = (RIL_HardwareConfig_State)pRead->state;
                pWrite->uuid[MAX_UUID_LENGTH - 1] = 0;
                strncpy(pWrite->uuid, pRead->uuid, MAX_UUID_LENGTH-1);

                memset(&(pWrite->cfg), 0, sizeof(pWrite->cfg));
                if (pWrite->type == RIL_HARDWARE_CONFIG_MODEM) {
                    pWrite->cfg.modem.rilModel = pRead->cfg.modem.ril_model;
                    pWrite->cfg.modem.rat = NetProtocolUtils::ConvertSitRaf2Ril(pRead->cfg.modem.rat, true);
                    pWrite->cfg.modem.maxVoice = pRead->cfg.modem.max_voice;
                    pWrite->cfg.modem.maxData = pRead->cfg.modem.max_data;
                    pWrite->cfg.modem.maxStandby = pRead->cfg.modem.max_standby;
                } else if (pWrite->type == RIL_HARDWARE_CONFIG_SIM) {
                    strncpy(pWrite->cfg.sim.modemUuid, pRead->cfg.sim.modem_uuid, MAX_UUID_LENGTH-1);
                } else {
                    RilLogE("(HwConfigChangeAdapter) Invalid HW CFG type %d", pWrite->type);
                }
           }
        }
    }

    return ret;
}

/**
 * ProtocolMiscCdmaPrlChangeAdapterLegacy
 */
int ProtocolMiscCdmaPrlChangeAdapterLegacy::GetPrlVersion() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_misc_cdma_prl_change_ind *data = (sit_misc_cdma_prl_change_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_CDMA_PRL_CHANGED) ) {
            ret = data->prl_ver;
        }
    }

    return ret;
}

/**
 * ProtocolMiscLceAdapterLegacy
 */
ProtocolMiscLceAdapterLegacy::ProtocolMiscLceAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolMiscLceAdapterLegacy::Init()
{
    m_lceStatus = 0;
    m_actualIntervalMs = 0;
    m_dlCapacityKbps = 0;
    m_ulCapacityKbps = 0;
    m_confidenceLevel = 0;
    m_lceSuspended = 0;

    if (m_pModemData != NULL && m_pModemData->GetRawData() != NULL && GetRilErrorCode() == RIL_E_SUCCESS) {
        UINT msgId = m_pModemData->GetMessageId();
        if (msgId == SIT_START_LCE_INFO && (m_pModemData->GetLength() == sizeof(sit_pdp_start_lce_info_rsp))) {
            sit_pdp_start_lce_info_rsp *data = (sit_pdp_start_lce_info_rsp *) m_pModemData->GetRawData();
            m_lceStatus = data->status;
            m_actualIntervalMs = data->interval;
        } else if (msgId == SIT_STOP_LCE_INFO && (m_pModemData->GetLength() == sizeof(sit_pdp_stop_lce_info_rsp))) {
            sit_pdp_stop_lce_info_rsp *data = (sit_pdp_stop_lce_info_rsp *) m_pModemData->GetRawData();
            m_lceStatus = data->status;
            m_actualIntervalMs = data->interval;
        } else if (msgId == SIT_GET_LCE_DATA && (m_pModemData->GetLength() == sizeof(sit_pdp_get_lce_data_rsp))) {
            sit_pdp_get_lce_data_rsp *data = (sit_pdp_get_lce_data_rsp *) m_pModemData->GetRawData();
            m_dlCapacityKbps = data->dl_lc;
            m_ulCapacityKbps = data->ul_lc;
            m_confidenceLevel = data->conf_lvl;
            m_lceSuspended = data->is_suspended;
        }
    }
}

RIL_Errno ProtocolMiscLceAdapterLegacy::GetRilErrorCode() const
{
    if (m_pModemData != NULL) {
        const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
        if (rcmdata != NULL && m_pModemData->GetLength() >= (int)sizeof(RCM_HEADER)) {
            if (rcmdata->type == RCM_TYPE_RESPONSE) {
                int errorCode = rcmdata->ext.rsp.error & 0xFF;
                if ( (int)RCM_E_SUCCESS <= errorCode && errorCode <= RIL_E_NO_SUCH_ELEMENT)
                    return (RIL_Errno)errorCode;
                else if ( (int)RCM_E_UNDEFINED_CMD == errorCode)
                    return (RIL_Errno)RIL_E_REQUEST_NOT_SUPPORTED;
                else if ( (int)RCM_E_NO_SUCH_ELEMENT < errorCode && errorCode < RCM_E_MAX) {
                    return (RIL_Errno)((errorCode - RCM_E_NO_SUCH_ELEMENT) + RIL_E_OEM_ERROR_1 - 1);
                }
            }
        }
    }
    // default error code
    return RIL_E_MODEM_ERR;
}

/**
 * ProtocolMiscLceIndAdapterLegacy
 */
int ProtocolMiscLceIndAdapterLegacy::GetDLLc() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_lce_data_ind *data = (sit_pdp_lce_data_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_LCE_DATA) ) {
            ret = data->dl_lc;
        }
    }

    return ret;
}

int ProtocolMiscLceIndAdapterLegacy::GetULLc() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_lce_data_ind *data = (sit_pdp_lce_data_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_LCE_DATA) ) {
            ret = data->ul_lc;
        }
    }

    return ret;
}

int ProtocolMiscLceIndAdapterLegacy::GetConfLevel() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_lce_data_ind *data = (sit_pdp_lce_data_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_LCE_DATA) ) {
            ret = data->conf_lvl;
        }
    }

    return ret;
}

int ProtocolMiscLceIndAdapterLegacy::GetIsSuspended() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_lce_data_ind *data = (sit_pdp_lce_data_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_LCE_DATA) ) {
            ret = data->is_suspended;
        }
    }

    return ret;
}

/**
 * ProtocolCdmaSubscriptionAdapterLegacy
 */
ProtocolCdmaSubscriptionAdapterLegacy::ProtocolCdmaSubscriptionAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData) {
    Init();
}

void ProtocolCdmaSubscriptionAdapterLegacy::Init()
{
    memset(m_szMdn, 0, MAX_CDMA_MDN_LEN);
    memset(m_szMin, 0, MAX_CDMA_MIN_LEN);
    m_wSid = m_wNid = m_uPrlVersion = 0;

    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_misc_get_cdma_subscription_rsp *data = (sit_misc_get_cdma_subscription_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CDMA_SUBSCRIPTION) {
            int mdnLenToCopy = MIN(data->mdn_size, MAX_CDMA_MDN_LEN);
            memcpy(m_szMdn, data->mdn, mdnLenToCopy);
            m_szMdn[mdnLenToCopy] = '\0';
            memcpy(m_szMin, data->min, MAX_CDMA_MIN_LEN);
            m_szMin[MAX_CDMA_MIN_LEN] = '\0';
            m_wSid = data->sid;
            m_wNid = data->nid;
            m_uPrlVersion = data->prl_version;
        }
    }
}

/**
 * ProtocolMiscSarControlStateAdapterLegacy
 */
ProtocolMiscSarControlStateAdapterLegacy::ProtocolMiscSarControlStateAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
    m_DeviceState = 0;
    if (m_pModemData != NULL) {
        sit_misc_psensor_sar_control_state_ind *data = (sit_misc_psensor_sar_control_state_ind *) m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_SAR_CONTROL_STATE) ) {
            m_DeviceState = data->device_state;
        }
    }
}

BYTE ProtocolMiscSarControlStateAdapterLegacy::GetDeviceState()
{
    return m_DeviceState;
}

/**
 * ProtocolMiscSarRfConnectionAdapterLegacy
 */
ProtocolMiscSarRfConnectionAdapterLegacy::ProtocolMiscSarRfConnectionAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
    m_RfState = 0;
    if (m_pModemData != NULL) {
        sit_misc_sar_rf_connection_ind *data = (sit_misc_sar_rf_connection_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_IND_RF_CONNECTION) ) {
            m_RfState = data->rf_state;
        }
    }
}

BYTE ProtocolMiscSarRfConnectionAdapterLegacy::GetRfState()
{
    return m_RfState;
}

/**
 * ProtocolMiscGetSarStateAdapterLegacy
 */
int ProtocolMiscGetSarStateAdapterLegacy::GetSarState() const
{
    if (m_pModemData != NULL) {
        sit_misc_sar_get_sar_state_rsp *data = (sit_misc_sar_get_sar_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_SAR_STATE) {
            RilLogV("sar state=%d", data->sar_state);
            return data->sar_state;
        }
    }
    return 0;
}

/**
 * ProtocolMiscRssiScanResultAdapterLegacy
 */
int ProtocolMiscRssiScanResultAdapterLegacy::GetTotalPage()
{
    int totalPage = 0;
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            totalPage = data->total_page & 0xFF;
        }
    }
    return totalPage;
}

int ProtocolMiscRssiScanResultAdapterLegacy::GetCurrentPage()
{
    int currentPage = -1;
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            currentPage = data->current_page & 0xFF;
        }
    }
    return currentPage;
}

int ProtocolMiscRssiScanResultAdapterLegacy::GetStartFrequency()
{
    int startFrequency = 0;
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            startFrequency = data->start_frequency & 0xFF;
        }
    }
    return startFrequency;
}

int ProtocolMiscRssiScanResultAdapterLegacy::GetEndFrequency()
{
    int endFrequency = 0;
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            endFrequency = data->end_frequency & 0xFF;
        }
    }
    return endFrequency;
}

int ProtocolMiscRssiScanResultAdapterLegacy::GetStep()
{
    int step = 0;
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            step = data->step & 0xFF;
        }
    }
    return step;
}

int ProtocolMiscRssiScanResultAdapterLegacy::GetScanResultSize()
{
    int len = 0;
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            len = (GetParameterLength()-7)/2;
        }
    }
    return len;
}

INT16* ProtocolMiscRssiScanResultAdapterLegacy::GetScanResult()
{
    if (m_pModemData != NULL) {
        sit_misc_rssi_scan_ind *data = (sit_misc_rssi_scan_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RSSI_SCAN) ) {
            return data->result;
        }
    }
    return 0;
}

/**
  * ProtocolMiscATCommandAdapterLegacy
  */
ProtocolMiscATCommandAdapterLegacy::ProtocolMiscATCommandAdapterLegacy(const ModemData *pModemData)
: ProtocolIndAdapter(pModemData)
, mCommand(NULL)
, mCommandLength(0)
{
    Init();
}

void ProtocolMiscATCommandAdapterLegacy::Init()
{
    if (m_pModemData != NULL) {
        sit_misc_forwarding_at_command_ind *data = (sit_misc_forwarding_at_command_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_FORWARDING_AT_COMMAND) ) {
            mCommandLength = data->length & 0xFFFF;
            if (mCommandLength > MAX_SIT_AT_COMMAND_LENGTH) {
                mCommandLength = MAX_SIT_AT_COMMAND_LENGTH;
            }

            mCommand = new char[mCommandLength + 1];
            if (mCommand != NULL) {
                memset(mCommand, 0, mCommandLength + 1);
                memcpy(mCommand, data->data, mCommandLength);
            }
            else {
                mCommandLength = 0;
            }
        }
    }
}

ProtocolMiscATCommandAdapterLegacy::~ProtocolMiscATCommandAdapterLegacy()
{
    if (mCommand != NULL) {
        delete[] mCommand;
        mCommand = NULL;
    }
}

/**
 * ProtocolMiscGetRadioNodeAdapterLegacy
 */
const char *ProtocolMiscGetRadioNodeAdapterLegacy::GetValue() const
{
    char *value = NULL;
    if (m_pModemData != NULL) {
        sit_misc_get_radio_node_rsp *data = (sit_misc_get_radio_node_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_RADIO_NODE) {
            value = data->value;
        }
    }
    return value;
}

/**
 * ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy
 */
int ProtocolMiscGetVoLteProvisionUpdateAdapterLegacy::GetStatus()
{
    if (m_pModemData != NULL) {
        sit_misc_get_volte_provision_rsp *data = (sit_misc_get_volte_provision_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_VOLTE_PROVISION_UPDATE) {
            return (data->status == 0x00) ? 0 : 1;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy
 */
int ProtocolMiscSetVoLteProvisionUpdateAdapterLegacy::GetResult()
{
    if (m_pModemData != NULL) {
        sit_misc_set_volte_provision_rsp *data = (sit_misc_set_volte_provision_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_VOLTE_PROVISION_UPDATE) {
            return (data->result == 0x00) ? 0 : 1;
        }
    }
    return 0;
}

/*
 * ProtocolMiscGetStackStatusAdapterLegacy
 */
int ProtocolMiscGetStackStatusAdapterLegacy::GetMode()
{
    if (m_pModemData != NULL) {
        sit_pwr_get_stack_status_rsp *data = (sit_pwr_get_stack_status_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_STACK_STATUS) {
            return (data->mode == 0x00) ? SIT_PWR_STATCK_DISABLE : SIT_PWR_STATCK_ENABLE;
        }
    }
    return 0;
}

/**
 * ProtocolOemModemInfoAdapterLegacy
 */
int ProtocolOemModemInfoAdapterLegacy::GetCommandType() const
{
    if (m_pModemData != NULL) {
        sit_oem_nw_info_rsp *resp = (sit_oem_nw_info_rsp *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_NW_INFO) {
            return resp->command;
        }
    }
    return -1;
}

/**
 * ProtocolOemModemInfoAdapterLegacy
 */
unsigned int ProtocolOemModemInfoAdapterLegacy::GetSize() const
{
    if (m_pModemData != NULL) {
        sit_oem_nw_info_rsp *resp = (sit_oem_nw_info_rsp *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_NW_INFO) {
            return resp->length;
        }
    }
    return 0;
}

void *ProtocolOemModemInfoAdapterLegacy::GetData()
{
    if (m_pModemData != NULL) {
        sit_oem_nw_info_rsp *resp = (sit_oem_nw_info_rsp *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_NW_INFO) {
            return resp->data;
        }
    }
    return NULL;
}

/**
 * ProtocolOemModemInfoIndAdapterLegacy
 */
int ProtocolOemModemInfoIndAdapterLegacy::GetCommandType() const
{
    if (m_pModemData != NULL) {
        sit_oem_nw_info_ind *resp = (sit_oem_nw_info_ind *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_IND_NW_INFO) {
            return resp->command;
        }
    }
    return -1;
}

unsigned int ProtocolOemModemInfoIndAdapterLegacy::GetSize() const
{
    if (m_pModemData != NULL) {
        sit_oem_nw_info_ind *resp = (sit_oem_nw_info_ind *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_IND_NW_INFO) {
            return resp->length;
        }
    }
    return 0;
}

void *ProtocolOemModemInfoIndAdapterLegacy::GetData()
{
    if (m_pModemData != NULL) {
        sit_oem_nw_info_ind *resp = (sit_oem_nw_info_ind *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_IND_NW_INFO) {
            return resp->data;
        }
    }
    return NULL;
}

/**
 * ProtocolOemSwitchModemFunctionAdapterLegacy
 */
BYTE ProtocolOemSwitchModemFunctionAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_func_switch_rsp *resp = (sit_oem_set_func_switch_rsp *)m_pModemData->GetRawData();
        if (resp != NULL && resp->hdr.id == SIT_OEM_SET_FUNC_SWITCH_REQ) {
            return resp->result;
        }
    }
    return -1;
}

/**
 * ProtocolMiscSetSelflogAdapterLegacy
 */
int ProtocolMiscSetSelflogAdapterLegacy::GetSelflogResult()
{
    if (m_pModemData != NULL) {
        sit_misc_set_selflog_rsp *data = (sit_misc_set_selflog_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_SELFLOG) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetSelflogStatusAdapterLegacy
 */
int ProtocolMiscGetSelflogStatusAdapterLegacy::GetSelflogStatus()
{
    if (m_pModemData != NULL) {
        sit_misc_get_selflog_status_rsp *data = (sit_misc_get_selflog_status_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_SELFLOG_STATUS) {
            return data->status;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSelflogStatusAdapterLegacy
 */
BYTE ProtocolMiscSelflogStatusAdapterLegacy::GetIndSelflogStatus()
{
    if (m_pModemData != NULL) {
        sit_misc_selflog_status_ind *data = (sit_misc_selflog_status_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_SELFLOG_STATUS) ) {
            return data->status;
        }
    }
    return 0;
}

/**
 * ProtocolOemGetCqiInfoAdapterLegacy
 */
INT16 ProtocolOemGetCqiInfoAdapterLegacy::GetCqiType() const
{
    if (m_pModemData != NULL) {
        sit_oem_get_cqi_info_rsp *data = (sit_oem_get_cqi_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_GET_CQI_INFO) ) {
            return data->type;
        }
    }
    return 0;
}
INT16 ProtocolOemGetCqiInfoAdapterLegacy::GetCqiInfo0() const
{
    if (m_pModemData != NULL) {
        sit_oem_get_cqi_info_rsp *data = (sit_oem_get_cqi_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_GET_CQI_INFO) ) {
            return data->cqi_info0;
        }
    }
    return 0;
}
INT16 ProtocolOemGetCqiInfoAdapterLegacy::GetCqiInfo1() const
{
    if (m_pModemData != NULL) {
        sit_oem_get_cqi_info_rsp *data = (sit_oem_get_cqi_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_GET_CQI_INFO) ) {
            return data->cqi_info1;
        }
    }
    return 0;
}
INT16 ProtocolOemGetCqiInfoAdapterLegacy::GetRi() const
{
    if (m_pModemData != NULL) {
        sit_oem_get_cqi_info_rsp *data = (sit_oem_get_cqi_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_GET_CQI_INFO) ) {
            return data->ri;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetTcsFciAdapterLegacy
 */
int ProtocolMiscSetTcsFciAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_tcs_fci_rsp *data = (sit_oem_set_tcs_fci_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_TCS_FCI_REQ) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetTcsFciAdapterLegacy
 */
const char * ProtocolMiscGetTcsFciAdapterLegacy::GetFci() const
{
    if (m_pModemData != NULL) {
        sit_oem_get_tcs_fci_info_rsp *data = (sit_oem_get_tcs_fci_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_GET_TCS_FCI_INFO) ) {
            return (const char *)data->fci;
        }
    }
    return NULL;
}

/**
 * ProtocolCaBandwidthFilterIndAdapterLegacy
 */
int ProtocolCaBandwidthFilterIndAdapterLegacy::GetCaConfig() const
{
    if (m_pModemData != NULL) {
        sit_oem_ca_bw_filter_ind *data = (sit_oem_ca_bw_filter_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_IND_CA_BW_FILTER) ) {
            return data->ca_config > 0 ? 1 : 0;
        }
    }
    return 0;
}

int ProtocolCaBandwidthFilterIndAdapterLegacy::GetNRB() const
{
    if (m_pModemData != NULL) {
        sit_oem_ca_bw_filter_ind *data = (sit_oem_ca_bw_filter_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_OEM_IND_CA_BW_FILTER) ) {
            return data->num_resource_block;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetModemLogDumpAdapterLegacy
 */
int ProtocolMiscSetModemLogDumpAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_misc_set_modem_log_dump_rsp *data = (sit_misc_set_modem_log_dump_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_MODEM_LOG_DUMP) {
            return data->result;
        }
    }
    return 0;
}

/*
 * ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy
 */
void ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy::InitLinkCapaEstimate()
{
    mCurLinkCapaEstimate.dl_capa_kbps = -1;
    mCurLinkCapaEstimate.ul_capa_kbps = -1;
    mCurLinkCapaEstimate.dcnr_sec_dl_capa_kbps = -1;
    mCurLinkCapaEstimate.dcnr_sec_ul_capa_kbps = -1;
}

ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy::ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy(const ModemData *pModemData) : ProtocolIndAdapter(pModemData)
{
    InitLinkCapaEstimate();

    if(m_pModemData != NULL) {
        sit_misc_current_link_capa_estimate_ind *data = (sit_misc_current_link_capa_estimate_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_CURRENT_LINK_CAPACITY_ESTIMATE) {
            if (data->dl_capa_kbps >= 0) {
                mCurLinkCapaEstimate.dl_capa_kbps = data->dl_capa_kbps;
            }
            if (data->ul_capa_kbps >= 0) {
                mCurLinkCapaEstimate.ul_capa_kbps = data->ul_capa_kbps;
            }
            if (GetParameterLength() >= (sizeof(int) * 4)) {
                if (data->dcnr_sec_dl_capa_kbps >= 0) {
                    mCurLinkCapaEstimate.dcnr_sec_dl_capa_kbps = data->dcnr_sec_dl_capa_kbps;
                }
                if (data->dcnr_sec_ul_capa_kbps >= 0) {
                    mCurLinkCapaEstimate.dcnr_sec_ul_capa_kbps = data->dcnr_sec_ul_capa_kbps;
                }
            }
        }
    }
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy::GetDlCapaKbps() const
{
    return mCurLinkCapaEstimate.dl_capa_kbps;
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy::GetUlCapaKbps() const
{
    return mCurLinkCapaEstimate.ul_capa_kbps;
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy::GetSecDlCapaKbps() const
{
    return mCurLinkCapaEstimate.dcnr_sec_dl_capa_kbps;
}

int ProtocolMiscCurrentLinkCapacityEstimateAdapterLegacy::GetSecUlCapaKbps() const
{
    return mCurLinkCapaEstimate.dcnr_sec_ul_capa_kbps;
}

/*
 * ProtocolMiscEndcCapabilityIndAdapterLegacy
 */
int ProtocolMiscEndcCapabilityIndAdapterLegacy::GetCapability() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_oem_endc_capability_ind *data = (sit_oem_endc_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_IND_ENDC_CAPABILITY) {
            ret = (int)data->endc_capability;
        }
    }
    return ret;
}

int ProtocolMiscEndcCapabilityIndAdapterLegacy::GetCause() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_oem_endc_capability_ind *data = (sit_oem_endc_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_IND_ENDC_CAPABILITY) {
            ret = (int)data->cause;
        }
    }
    return ret;
}


/**
 * ProtocolMiscSetSelflogProfileAdapterLegacy
 */
int ProtocolMiscSetSelflogProfileAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_set_selflog_profile_rsp *data = (sit_set_selflog_profile_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_SELFLOG_PROFILE) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetForbidLteCellAdapterLegacy
 */
int ProtocolMiscSetForbidLteCellAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_forbid_lte_cell_rsp *data = (sit_oem_set_forbid_lte_cell_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_FORBID_LTE_CELL) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetApSystemTimeAdapterLegacy
 */
int ProtocolMiscSetApSystemTimeAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_set_ap_system_time_rsp *data = (sit_set_ap_system_time_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_AP_SYSTEM_TIME) {
            return data->result;
        }
    }
    return 0;
}

/**
* ProtocolMiscSetForbidFrequencyAdapterLegacy
*/
int ProtocolMiscSetForbidFrequencyAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_forbidden_frequency_rsp *data = (sit_oem_set_forbidden_frequency_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_FORBIDDEN_FREQUENCY) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetForbidLteBandAdapterLegacy
 */
int ProtocolMiscSetForbidLteBandAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_forbidden_let_band_rsp *data = (sit_oem_set_forbidden_let_band_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_FORBIDDEN_LTE_BAND) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetForbidLteTacAdapterLegacy
 */
int ProtocolMiscSetForbidLteTacAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_forbidden_let_tac_rsp *data = (sit_oem_set_forbidden_let_tac_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_FORBIDDEN_LTE_TAC) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetAheadRreAdapterLegacy
 */
int ProtocolMiscSetAheadRreAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_oem_set_ahead_rre_rsp *data = (sit_oem_set_ahead_rre_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_AHEAD_RRE) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscSetForbidNrCellAdapterLegacy
 */
int ProtocolMiscSetForbidNrCellAdapterLegacy::GetResult() const {
    if (m_pModemData != NULL) {
        sit_oem_set_forbid_nr_cell_rsp *data = (sit_oem_set_forbid_nr_cell_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_FORBID_NR_CELL) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolRfConnectionStateAdapterLegacy
 */
BYTE ProtocolRfConnectionStateAdapterLegacy::GetState() {
    if (m_pModemData != NULL) {
        sit_rf_connection_ind *data = (sit_rf_connection_ind *)m_pModemData->GetRawData();
        if (data != NULL && (data->hdr.id == SIT_IND_RF_CONNECTION) ) {
            return data->rf_cable_state > 0 ? 1 : 0;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy
 */
BYTE ProtocolMiscGetCdmaCapabilityInfoAdapterLegacy::GetCapability() const {
    if (m_pModemData != NULL) {
        sit_oem_get_cdma_capability_info_rsp *data = (sit_oem_get_cdma_capability_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_CDMA_CAPABILITY_INFO) {
            return data->capability;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetBinaryVersionInfoAdapterLegacy
 */
int ProtocolMiscGetBinaryVersionInfoAdapterLegacy::GetVersionInfoLength() const {
    if (m_pModemData != nullptr) {
        sit_oem_get_binary_version_info_rsp *resp = (sit_oem_get_binary_version_info_rsp *)m_pModemData->GetRawData();
        if (resp != nullptr && resp->hdr.id == SIT_OEM_GET_BINARY_VERSION_INFO) {
            return (int)resp->length;
        }
    }
    return 0;
}

const char * ProtocolMiscGetBinaryVersionInfoAdapterLegacy::GetData() const {
    if (m_pModemData != nullptr) {
        sit_oem_get_binary_version_info_rsp *resp = (sit_oem_get_binary_version_info_rsp *)m_pModemData->GetRawData();
        if (resp != nullptr && resp->hdr.id == SIT_OEM_GET_BINARY_VERSION_INFO) {
            return resp->data;
        }
    }
    return NULL;
}

/**
 * ProtocolMiscSetDeviceInfoAdapterLegacy
 */
ProtocolMiscSetDeviceInfoAdapterLegacy::ProtocolMiscSetDeviceInfoAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
      , mDeviceInfo(NULL)
{
    Init();
}

ProtocolMiscSetDeviceInfoAdapterLegacy::~ProtocolMiscSetDeviceInfoAdapterLegacy()
{
    if (mDeviceInfo != NULL) {
        delete[] mDeviceInfo;
        mDeviceInfo = NULL;
    }
}

void ProtocolMiscSetDeviceInfoAdapterLegacy::Init()
{
    if (m_pModemData != NULL) {
        sit_misc_set_device_info_rsp *data = (sit_misc_set_device_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_DEVICE_INFO && GetErrorCode() == RCM_E_SUCCESS) {
            size_t size = GetParameterLength();
            if (size > 0) {
                mDeviceInfo = new char[size];
                if (mDeviceInfo != NULL) {
                    memset(mDeviceInfo, 0 , size);
                    memcpy(mDeviceInfo, data->sit_version_info, size);

                    char *key = strtok(mDeviceInfo, "=,");
                    char *value = NULL;
                    while (key != NULL) {
                        value = strtok(NULL, "=,");
                        mRilProp.Put(key, value);
                        key = strtok(NULL, "=,");
                    }
                }
            }
        }
    }
}

int ProtocolMiscSetDeviceInfoAdapterLegacy::GetVersionInfo(const char *input)
{
    return mRilProp.GetInt(input, DEFAULT_CELL_INFO_VER);
}

int ProtocolMiscSetActiveVsimAdapterLegacy::GetResult() const{
    if (m_pModemData != NULL) {
        sit_oem_set_activate_visim_rsp *data = (sit_oem_set_activate_visim_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_SET_ACTIVATE_VSIM) {
            return data->result;
        }
    }
    return 0;
}

/**
 * ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy
 */
ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy(
        const ModemData *pModemData) : ProtocolIndAdapter(pModemData) {
    if (m_pModemData == NULL) {
        RilLogE("Null Sim Info Indication");
        return;
    }

    sit_ind_carrier_config_sim_info *data =
        (sit_ind_carrier_config_sim_info *)m_pModemData->GetRawData();
    if (data == NULL || data->hdr.id != SIT_IND_CARRIER_CONFIG_SIM_INFO) {
        RilLogE("Message is not a Sim Info Indication");
        return;
    }

    // TODO: Handle simInfoLen field.
    mIccid = std::string((char *) data->iccid);
    mImsi = std::string((char *) data->imsi);
    if (mImsi.empty()) {
        mImsi = std::string((char *) data->imsiM);
    }
    mMccMnc = mImsi.substr(0, 3 + data->mncLen);
    mSpn = std::string((char *) data->spn);
    mPnn = std::string((char *) data->pnn);
    mGid1 = std::string((char *) data->gid1);
    mGid2 = std::string((char *) data->gid2);

    std::ostringstream ss;
    for (int i = 0; i < MIN(data->privilegeAccessRulesLen, PRIVILEGE_ACCESS_RULES_LEN); ++i) {
        if (i==0) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) data->privilegeAccessRules[i];
        } else {
            ss << (int) data->privilegeAccessRules[i];
        }
    }
    mPrivilegeAccessRules = ss.str();
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getMccMnc() {
    return mMccMnc;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getIccid() {
    return mIccid;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getImsi() {
    return mImsi;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getSpn() {
    return mSpn;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getPnn() {
    return mPnn;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getGid1() {
    return mGid1;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getGid2() {
    return mGid2;
}

const std::string &ProtocolMiscCarrierConfigSimInfoIndAdapterLegacy::getPrivilegeAccessRules() {
    return mPrivilegeAccessRules;
}

/**
 * ProtocolMiscCarrierConfigStatusIndAdapterLegacy
 */
int ProtocolMiscCarrierConfigStatusIndAdapterLegacy::GetStatus() const {
    int status = 0;
    if (m_pModemData != NULL) {
        sit_carrier_config_status_ind *data = (sit_carrier_config_status_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_CARRIER_CONFIG_STATUS) {
            return data->status;
        }
    }
    return status;
}

int ProtocolMiscCarrierConfigStatusIndAdapterLegacy::GetCause() const {
    int cause = 0;
    if (m_pModemData != NULL) {
        sit_carrier_config_status_ind *data = (sit_carrier_config_status_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_CARRIER_CONFIG_STATUS) {
            return data->cause;
        }
    }
    return cause;
}

/**
 * ProtocolMiscGetTraceDumpCfgAdapterLegacy
 */
BYTE ProtocolMiscGetTraceDumpCfgAdapterLegacy::GetConfig() const {
    if (m_pModemData != NULL) {
        sit_oem_get_trace_dump_cfg_rsp *data = (sit_oem_get_trace_dump_cfg_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_TRACE_DUMP_CFG) {
            return data->config;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetUeOperationModeAdapterLegacy
 */
BYTE ProtocolMiscGetUeOperationModeAdapterLegacy::GetMode() const {
    if (m_pModemData != nullptr) {
        sit_oem_get_ue_operation_mode_rsp *data = (sit_oem_get_ue_operation_mode_rsp *)m_pModemData->GetRawData();
        if (data != nullptr && data->hdr.id == SIT_OEM_GET_UE_OPERATION_MODE) {
            return data->mode;
        }
    }
    return 0;
}


/**
 * ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy
 */
BYTE ProtocolMiscGetNrrrmMinReqEnableAdapterLegacy::GetEnable() const {
    if (m_pModemData != nullptr) {
        sit_oem_get_nrrrm_min_req_enable_rsp *data = (sit_oem_get_nrrrm_min_req_enable_rsp *)m_pModemData->GetRawData();
        if (data != nullptr && data->hdr.id == SIT_OEM_GET_NRRRM_MIN_REQ_ENABLE) {
            return data->enable;
        }
    }
    return 0;
}

/**
 * ProtocolMiscGetVcrtAdapterLegacy
 */
BYTE ProtocolMiscGetVcrtAdapterLegacy::GetVcrtMode() const {
    if (m_pModemData != nullptr) {
        sit_oem_get_vcrt_rsp *data = (sit_oem_get_vcrt_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_OEM_GET_VCRT) {
            return data->vcrtMode;
        }
    }
    return 0;
}
