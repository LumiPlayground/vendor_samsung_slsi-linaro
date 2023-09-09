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
 * netdatabuilder.cpp
 *
 *  Created on: 2014. 6. 30.
 *      Author: sungwoo48.choi
 */

#include "netdatabuilder.h"
#include <telephony/librilutils.h>
#include <telephony/ril.h>
#include <librilutils/radio_conv_utils.h>
#include "netdatabuilder.h"
#include "operatortable.h"
#include "rilapplication.h"
#include "rillog.h"
#include <librilutils/vendor.h>
#include <librilutils/textutils.h>
#include "networkutils.h"
#include "netdata.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

RilDataUniqPtr NetworkDataBuilder::BuildOperatorResponse(const OperatorInfo &resultOpName, const OperatorInfo &rawOpName) {
    const int numOfOperatorRspStrings = 6;
    auto rildata(std::make_unique<RilDataStrings>(numOfOperatorRspStrings));
    if (rildata != nullptr) {
        rildata->SetString(0, resultOpName.alphaLong.c_str());
        rildata->SetString(1, resultOpName.alphaShort.c_str());
        rildata->SetString(2, resultOpName.numeric.c_str());
        rildata->SetString(3, rawOpName.alphaLong.c_str());
        rildata->SetString(4, rawOpName.alphaShort.c_str());
        rildata->SetString(5, rawOpName.numeric.c_str());
    }

    return std::move(rildata);
}

RilDataUniqPtr NetworkDataBuilder::BuildNetSelectModeResponse(int mode) {
    auto rildata(std::make_unique<RilDataInts>(1));
    if (rildata != nullptr) {
        rildata->SetInt(0, mode);
    }

    return std::move(rildata);
}

RilDataUniqPtr NetworkDataBuilder::BuildNetAvailableBandModeResponse(const int *bandMode, int count) {
    std::unique_ptr<RilDataInts> rildata(nullptr);
    if (count > 0 && bandMode != nullptr) {
        rildata.reset(new RilDataInts(count + 1));
        if (rildata != nullptr) {
            // Test IRadio.getAvailableBandModes() for the response returned.
            // Automatic mode selection must be supported
            rildata->SetInt(0, BAND_MODE_UNSPECIFIED);
            for (int i = 1; i <= count; i++) {
                rildata->SetInt(i+1, bandMode[i]);
            } // end for i
        }
    }

    return std::move(rildata);
}

RilDataUniqPtr NetworkDataBuilder::BuildNetAvailableNetweorkResponse(const NetworkInfo *networks, int count) {
    std::unique_ptr<RilDataStrings> rildata(nullptr);
    if (networks == nullptr || count == 0) {
        rildata.reset(new RilDataStrings(0));
    } else {
        rildata.reset(new RilDataStrings(count * 4));

        for (int i = 0; i < count; i++) {
            const NetworkInfo *p = networks + i;
            rildata->SetString(i*4, p->longPlmn);
            rildata->SetString(i*4+1, p->shortPlmn);
            rildata->SetString(i*4+2, p->plmn);
            rildata->SetString(i*4+3, p->status);
        } // end for i ~
    }

    return std::move(rildata);
}

RilDataUniqPtr NetworkDataBuilder::BuildNetFemtoCellSrchResponse(const int srch_result, const char *plmn) {
    auto rildata(std::make_unique<RilDataStrings>(2));
    if (rildata != nullptr) {
        rildata->SetString(0, srch_result);
        rildata->SetString(1, plmn);
    }

    return std::move(rildata);
}

/**
 * NetworkDataBplmnListBuilder
 */
void NetworkDataBplmnListBuilder::AddNetworkInfo(const NetworkInfo &nwkInfo)
{
    mList.push_back(nwkInfo);
}

const RilData *NetworkDataBplmnListBuilder::Build()
{
    int size = mList.size();
    int colSize = 5;

    RilDataStrings *rildata = new RilDataStrings(size * colSize);
    for (int i = 0; i < size; i++) {
        NetworkInfo &nwkInfo = mList[i];
        rildata->SetString(i*colSize, nwkInfo.longPlmn);
        rildata->SetString(i*colSize+1, nwkInfo.shortPlmn);
        rildata->SetString(i*colSize+2, nwkInfo.plmn);
        rildata->SetString(i*colSize+3, nwkInfo.status);
        rildata->SetString(i*colSize+4, nwkInfo.rat);
    } // end for i ~
    return rildata;
}

static void fillCellIdentity(RIL_CellIdentity_V1_5& cellIdentity, int cellInfoType,
        int mcc, int mnc, const char *alphaLong = NULL, const char *alphaShort = NULL)
{
    switch (cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM: {
        cellIdentity.gsm.mcc = mcc;
        cellIdentity.gsm.mnc = mnc;
        memset(cellIdentity.gsm.operatorNames.alphaLong,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaLong)) {
            strncpy(cellIdentity.gsm.operatorNames.alphaLong,
                    alphaLong, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }

        memset(cellIdentity.gsm.operatorNames.alphaShort,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaShort)) {
            strncpy(cellIdentity.gsm.operatorNames.alphaShort,
                    alphaShort, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }
        break;
    }
    case RIL_CELL_INFO_TYPE_WCDMA: {
        cellIdentity.wcdma.mcc = mcc;
        cellIdentity.wcdma.mnc = mnc;
        memset(cellIdentity.wcdma.operatorNames.alphaLong,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaLong)) {
            strncpy(cellIdentity.wcdma.operatorNames.alphaLong,
                    alphaLong, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }

        memset(cellIdentity.wcdma.operatorNames.alphaShort,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaShort)) {
            strncpy(cellIdentity.wcdma.operatorNames.alphaShort,
                    alphaShort, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }
        break;
    }
    case RIL_CELL_INFO_TYPE_LTE: {
        cellIdentity.lte.mcc = mcc;
        cellIdentity.lte.mnc = mnc;
        memset(cellIdentity.lte.operatorNames.alphaLong,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaLong)) {
            strncpy(cellIdentity.lte.operatorNames.alphaLong,
                    alphaLong, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }

        memset(cellIdentity.lte.operatorNames.alphaShort,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaShort)) {
            strncpy(cellIdentity.lte.operatorNames.alphaShort,
                    alphaShort, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }
        break;
    }
    case RIL_CELL_INFO_TYPE_TD_SCDMA: {
        cellIdentity.tdscdma.mcc = mcc;
        cellIdentity.tdscdma.mnc = mnc;
        memset(cellIdentity.tdscdma.operatorNames.alphaLong,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaLong)) {
            strncpy(cellIdentity.tdscdma.operatorNames.alphaLong,
                    alphaLong, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }

        memset(cellIdentity.tdscdma.operatorNames.alphaShort,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaShort)) {
            strncpy(cellIdentity.tdscdma.operatorNames.alphaShort,
                    alphaShort, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }
        break;
    }
    case RIL_CELL_INFO_TYPE_NR: {
        cellIdentity.nr.mcc = mcc;
        cellIdentity.nr.mnc = mnc;
        memset(cellIdentity.nr.operatorNames.alphaLong,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaLong)) {
            strncpy(cellIdentity.nr.operatorNames.alphaLong,
                    alphaLong, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }

        memset(cellIdentity.nr.operatorNames.alphaShort,
                0, MAX_ALPHA_OPERATOR_NAME_LEN);
        if (!TextUtils::IsEmpty(alphaShort)) {
            strncpy(cellIdentity.nr.operatorNames.alphaShort,
                    alphaShort, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
        }
        break;
    }
    default:
        break;
    } // end switch ~
}

static void fillCdmaCellIdentity(RIL_CellIdentity_V1_5& cellIdentity, int cellInfoType,
        int basestationId, int latitude, int longitude, int systemId, int networkId)
{
    switch (cellInfoType) {
    case RIL_CELL_INFO_TYPE_CDMA: {
        cellIdentity.cdma.basestationId = basestationId;
        cellIdentity.cdma.latitude = latitude;
        cellIdentity.cdma.longitude = longitude;
        cellIdentity.cdma.systemId = systemId;
        cellIdentity.cdma.networkId = networkId;
        break;
    }
    default:
        break;
    }
}

static void fillCellIdentity(RIL_CellIdentity_V1_5& cellIdentity, int cellInfoType,
        int lac, int cid, int psc,
        int tac, int pcid, int eci, int channel, long int nrcid)
{
    switch (cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        cellIdentity.gsm.lac = lac;
        cellIdentity.gsm.cid = cid;
        cellIdentity.gsm.arfcn = channel;
        cellIdentity.gsm.bsic = 0xFF;
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        cellIdentity.wcdma.lac = lac;
        cellIdentity.wcdma.cid = cid;
        cellIdentity.wcdma.psc = psc;
        cellIdentity.wcdma.uarfcn = channel;
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        cellIdentity.lte.tac = tac;
        cellIdentity.lte.ci = eci;
        cellIdentity.lte.pci = pcid;
        cellIdentity.lte.earfcn = channel;
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        cellIdentity.tdscdma.lac = lac;
        cellIdentity.tdscdma.cid = cid;
        cellIdentity.tdscdma.cpid = INT_MAX;
        break;
    case RIL_CELL_INFO_TYPE_NR:
        cellIdentity.nr.tac = tac;
        cellIdentity.nr.nci = nrcid;
        cellIdentity.nr.pci = pcid;
        cellIdentity.nr.nrarfcn = channel;
        break;
    } // end switch ~
}

CellIdentityBuilder::CellIdentityBuilder()
{
    memset(&mCellIdentity, 0, sizeof(mCellIdentity));
    memset(&mCellIdentity_v16, 0, sizeof(mCellIdentity_v16));
    SetCellInfoType(RIL_CELL_INFO_TYPE_NONE);
}

CellIdentityBuilder::CellIdentityBuilder(int cellInfoType)
{
    memset(&mCellIdentity, 0, sizeof(mCellIdentity));
    memset(&mCellIdentity_v16, 0, sizeof(mCellIdentity_v16));
    SetCellInfoType(cellInfoType);
}

void CellIdentityBuilder::SetCellInfoType(int cellInfoType)
{
    mCellInfoType = cellInfoType;
    mCellIdentity.cellInfoType = (RIL_CellInfoType)cellInfoType;
}

void CellIdentityBuilder::SetCsgInfo(bool isCsg, const char *nodeBname, int csgId)
{
    RIL_ClosedSubscriberGroupInfo *pCsgInfo = NULL;
    switch (mCellInfoType) {
        case RIL_CELL_INFO_TYPE_WCDMA:
            pCsgInfo = &(mCellIdentity.wcdma.optionalCsgInfo.csgInfo);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            pCsgInfo = &(mCellIdentity.lte.optionalCsgInfo.csgInfo);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            pCsgInfo = &(mCellIdentity.tdscdma.optionalCsgInfo.csgInfo);
            break;
    }

    if (pCsgInfo != NULL) {
        pCsgInfo->csgIndication = isCsg;
        if (TextUtils::IsEmpty(nodeBname)) {
            pCsgInfo->homeNodebName[0] = 0;
        }
        else {
            int len = strlen(nodeBname);
            if (len > (MAX_CSG_HOME_NODEB_NAME - 1)) len = (MAX_CSG_HOME_NODEB_NAME - 1);
            memcpy(pCsgInfo->homeNodebName, nodeBname, len);
            pCsgInfo->homeNodebName[len] = 0;
        }
        pCsgInfo->csgIdentity = csgId;
    }
}

void CellIdentityBuilder::SetCellIdentity(int mcc, int mnc,
        const char *alphaLong, const char *alphaShort)
{
    fillCellIdentity(mCellIdentity, mCellInfoType, mcc, mnc, alphaLong, alphaShort);
    SetCsgInfo(FALSE, NULL, INT_MAX);    // No Info from CP in current, so set to default value.
}

void CellIdentityBuilder::SetCellIdentity(const char *numeric,
        const char *alphaLong, const char *alphaShort)
{
    int mcc = INT_MAX;
    int mnc = INT_MAX;
    if (TextUtils::IsDigitsOnly(numeric)) {
        mcc = NetworkUtils::fetchMcc(numeric);
        mnc = NetworkUtils::fetchMnc(numeric);
        if (mcc == 0 || mcc == INT_MAX) {
            mnc = INT_MAX;
        }
    }
    SetCellIdentity(mcc, mnc, alphaLong, alphaShort);
}

void CellIdentityBuilder::SetCellIdentity(int lac, int cid, int psc,
        int tac, int pcid, int eci, int channel, long int nrcid)
{
    fillCellIdentity(mCellIdentity, mCellInfoType,
            lac, cid, psc, tac, pcid, eci, channel, nrcid);
}
void CellIdentityBuilder::SetCdmaCellIdentity(int basestationId, int latitude,
        int longitude, int systemId, int networkId)
{
    fillCdmaCellIdentity(mCellIdentity, mCellInfoType,
            basestationId, latitude, longitude, systemId, networkId);
}

void *CellIdentityBuilder::cellIdentity(int halVer)
{
    if (halVer < HAL_VERSION_CODE(1, 2)) {
        RadioConvUtils::convertCellIdentity(mCellIdentity_v16, mCellIdentity);
        return &mCellIdentity_v16;
    }
    return &mCellIdentity;
}

void DataRegStateResultBuilder::SetRegistrationState(
        int regState, int rat, int rejectCause, int /* sdc */)
{
    mDataRegResult.regState = (RIL_RegState)regState;
    mDataRegResult.rat = (RIL_RadioTechnology)rat;
    mDataRegResult.reasonForDenial = (RIL_RegistrationFailCause)rejectCause;
    int cellInfoType = RadioConvUtils::getCellInfoTypeRadioTechnology(rat);
    mCellIdentityBuilder.SetCellInfoType(cellInfoType);
}

void DataRegStateResultBuilder::SetRegisteredPlmn(const char *numeric)
{
    int len = 0;
    if (TextUtils::IsDigitsOnly(numeric)) {
        len = strlen(numeric);
        if (len > MAX_REG_PLMN_LEN -1) len = MAX_REG_PLMN_LEN -1;
        memcpy(mDataRegResult.registeredPlmn, numeric, len);
    }
    mDataRegResult.registeredPlmn[len] = 0;
}

void DataRegStateResultBuilder::SetCellIdentity(
        int mcc, int mnc, const char *alphaLong, const char * /* alphaShort */)
{
    mCellIdentityBuilder.SetCellIdentity(mcc, mnc, alphaLong, alphaLong);
}

void DataRegStateResultBuilder::SetCellIdentity(const char *numeric,
        const char *alphaLong, const char *alphaShort)
{
    mCellIdentityBuilder.SetCellIdentity(numeric, alphaLong, alphaShort);
}

void DataRegStateResultBuilder::SetCellIdentity(
        int lac, int cid, int psc, int tac, int pcid, int eci, int channel, long int nrcid)
{
    mCellIdentityBuilder.SetCellIdentity(lac, cid, psc, tac, pcid, eci, channel, nrcid);
}

void DataRegStateResultBuilder::SetLteVopsInfo(bool isVopsSupported,
        bool isEmcBearerSupported)
{
    mDataRegResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported = isVopsSupported;
    mDataRegResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported = isEmcBearerSupported;
}

void DataRegStateResultBuilder::SetNrIndicators(bool isEndcAvailable,
        bool isDcNrRestricted, bool isNrAvailable)
{
    mDataRegResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable = isEndcAvailable;
    mDataRegResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted = isDcNrRestricted;
    mDataRegResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable = isNrAvailable;
}

void DataRegStateResultBuilder::SetNrVopsInfo(int vopsSupported, int emcSupported, int emfSupported)
{
    mDataRegResult.accessTechnologySpecificInfo.ngranInfo.vopsSupported = (RIL_VopsIndicator) vopsSupported;
    mDataRegResult.accessTechnologySpecificInfo.ngranInfo.emcSupported = (RIL_EmcIndicator) emcSupported;
    mDataRegResult.accessTechnologySpecificInfo.ngranInfo.emfSupported = (RIL_EmfIndicator) emfSupported;
}

RilDataUniqPtr DataRegStateResultBuilder::Build() {
    mDataRegResult.cellIdentity =
                *((RIL_CellIdentity_V1_5 *)mCellIdentityBuilder.cellIdentity(HAL_VERSION_CODE(1, 5)));

    // HAL_VERSION_CODE(1, 5) or less
    if (mHalVer < HAL_VERSION_CODE(1, 6)) {
        RIL_RegStateResult dataRegResult;
        memset(&dataRegResult, 0, sizeof(dataRegResult));
        RadioConvUtils::convertDataRegistrationStateResult(dataRegResult, mDataRegResult);
        return std::make_unique<RilDataRaw>(&dataRegResult, sizeof(RIL_RegStateResult));
    }

    RilLogV("[DataRegStateResultBuilder] Build for %s",
            vendor::radio::halVersionCodeToString(mHalVer).c_str());
    // HAL_VERSION_CODE(1, 6) or higher
    return std::make_unique<RilDataRaw>(&mDataRegResult, sizeof(RIL_RegStateResult_V1_6));
}

void VoiceRegStateResultBuilder::SetRegistrationState(int regState, int rat, int rejectCause)
{
    mVoiceRegResult.regState = (RIL_RegState)regState;
    mVoiceRegResult.rat = (RIL_RadioTechnology)rat;
    mVoiceRegResult.reasonForDenial = (RIL_RegistrationFailCause) rejectCause;
    int cellInfoType = RadioConvUtils::getCellInfoTypeRadioTechnology(rat);
    mCellIdentityBuilder.SetCellInfoType(cellInfoType);
}

void VoiceRegStateResultBuilder::SetCdmaState(int cssSupported, int roamingIndicator,
        int systemIsInPrl, int defaultRoamingIndicator)
{
    mVoiceRegResult.accessTechnologySpecificInfo.cdmaInfo.cssSupported = cssSupported;
    mVoiceRegResult.accessTechnologySpecificInfo.cdmaInfo.roamingIndicator = roamingIndicator;
    mVoiceRegResult.accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl = (RIL_PrlIndicator)systemIsInPrl;
    mVoiceRegResult.accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator = defaultRoamingIndicator;
}

void VoiceRegStateResultBuilder::SetRegisteredPlmn(const char *numeric)
{
    int len = 0;
    if (TextUtils::IsDigitsOnly(numeric)) {
        len = strlen(numeric);
        if (len > MAX_REG_PLMN_LEN -1) len = MAX_REG_PLMN_LEN -1;
        memcpy(mVoiceRegResult.registeredPlmn, numeric, len);
    }
    mVoiceRegResult.registeredPlmn[len] = 0;
}

void VoiceRegStateResultBuilder::SetCellIdentity(
        int mcc, int mnc, const char *alphaLong, const char * /* alphaShort */)
{
    mCellIdentityBuilder.SetCellIdentity(mcc, mnc, alphaLong, alphaLong);
}
void VoiceRegStateResultBuilder::SetCellIdentity(const char *numeric,
        const char *alphaLong, const char *alphaShort)
{
    mCellIdentityBuilder.SetCellIdentity(numeric, alphaLong, alphaShort);
}

void VoiceRegStateResultBuilder::SetCellIdentity(
        int lac, int cid, int psc, int tac, int pcid, int eci, int channel, long int nrcid)
{
    mCellIdentityBuilder.SetCellIdentity(lac, cid, psc, tac, pcid, eci, channel, nrcid);
}

void VoiceRegStateResultBuilder::SetCdmaCellIdentity(
        int basestationId, int latitude,
        int longitude, int systemId, int networkId)
{
    mCellIdentityBuilder.SetCdmaCellIdentity(basestationId, latitude,
            longitude, systemId, networkId);
}

RilDataUniqPtr VoiceRegStateResultBuilder::Build() {
    mVoiceRegResult.cellIdentity =
                *((RIL_CellIdentity_V1_5 *)mCellIdentityBuilder.cellIdentity(RilApplication::RIL_HalVersionCode));

    // HAL_VERSION_CODE(1, 5) or less
    if (mHalVer < HAL_VERSION_CODE(1, 6)) {
        RIL_RegStateResult voiceRegResult;
        memset(&voiceRegResult, 0, sizeof(voiceRegResult));
        RadioConvUtils::convertDataRegistrationStateResult(voiceRegResult, mVoiceRegResult);
        return std::make_unique<RilDataRaw>(&voiceRegResult, sizeof(RIL_RegStateResult));
    }

    RilLogV("[VoiceRegStateResultBuilder] Build for %s",
            vendor::radio::halVersionCodeToString(mHalVer).c_str());
    // HAL_VERSION_CODE(1, 6) or higher
    return std::make_unique<RilDataRaw>(&mVoiceRegResult, sizeof(RIL_RegStateResult_V1_6));
}

/**
 * CellInfoBuilder
 */
const RilData *CellInfoListBuilder::Build(list<RIL_CellInfo_V1_5>& cellInfoList) {
     return new CellInfoList_V1_5(cellInfoList);
}

const RilData *CellInfoListBuilder::Build(list<RIL_CellInfo_V1_6>& cellInfoList) {
    if (mHalVer < HAL_VERSION_CODE(1, 6)) {
        return new CellInfoList_V1_5(cellInfoList);
    }
    return new CellInfoList_V1_6(cellInfoList);
}

class NetworkScanResult : public RilData {
private:
    RIL_NetworkScanResult_V1_5 *mResult_V1_5;
    RIL_NetworkScanResult_V1_6 *mResult_V1_6;
    unsigned int mSize;
    void *mData;
public:
    NetworkScanResult(int halVer) {
        mResult_V1_5 = NULL;
        mResult_V1_6 = NULL;
        mSize = 0;
        mData = NULL;

        if (halVer >= HAL_VERSION_CODE(1,6)) {
            mResult_V1_6 = new RIL_NetworkScanResult_V1_6 {};
            mSize = sizeof(RIL_NetworkScanResult_V1_6);
            mData = mResult_V1_6;
        } else {
            mResult_V1_5 = new RIL_NetworkScanResult_V1_5 {};
            mSize = sizeof(RIL_NetworkScanResult_V1_5);
            mData = mResult_V1_5;
        }
    }
    NetworkScanResult(const NetworkScanResult &) = delete;
    NetworkScanResult& operator=(NetworkScanResult const&) = delete;
    virtual ~NetworkScanResult() {
        if (mResult_V1_6 != NULL) {
            if (mResult_V1_6->network_infos != NULL) {
                delete[] mResult_V1_6->network_infos;
            }
            delete mResult_V1_6;
        }
        if (mResult_V1_5 != NULL) {
            if (mResult_V1_5->network_infos != NULL) {
                delete[] mResult_V1_5->network_infos;
            }
            delete mResult_V1_5;
        }
    }

    void SetStatus(int status, int errorCode) {
        if (mResult_V1_6 != NULL) {
            mResult_V1_6->status = (RIL_ScanStatus)status;
            mResult_V1_6->error = (RIL_Errno)errorCode;
        }
        if (mResult_V1_5 != NULL) {
            mResult_V1_5->status = (RIL_ScanStatus)status;
            mResult_V1_5->error = (RIL_Errno)errorCode;
        }
    }

    void SetCellInfoList(RIL_CellInfo_V1_5 *cellInfos, unsigned int size) {
        if (mResult_V1_5 == NULL)
            return;

        if (cellInfos == NULL || size == 0) {
            return ;
        }

        if (mResult_V1_5->network_infos != NULL) {
            delete[] mResult_V1_5->network_infos;
            mResult_V1_5->network_infos = NULL;
        }

        mResult_V1_5->network_infos = new RIL_CellInfo_V1_5[size] {};
        if (mResult_V1_5->network_infos != NULL) {
            memcpy(mResult_V1_5->network_infos, cellInfos, sizeof(RIL_CellInfo_V1_5) * size);
            mResult_V1_5->network_infos_length = size;
        }
    }

    void SetCellInfoList(RIL_CellInfo_V1_6 *cellInfos, unsigned int size) {
        if (mResult_V1_6 == NULL)
            return;

        if (cellInfos == NULL || size == 0) {
            return ;
        }

        if (mResult_V1_6->network_infos != NULL) {
            delete[] mResult_V1_6->network_infos;
            mResult_V1_6->network_infos = NULL;
        }

        mResult_V1_6->network_infos = new RIL_CellInfo_V1_6[size] {};
        if (mResult_V1_6->network_infos != NULL) {
            memcpy(mResult_V1_6->network_infos, cellInfos, sizeof(RIL_CellInfo_V1_6) * size);
            mResult_V1_6->network_infos_length = size;
        }
    }

    void *GetData() const { return mData; }
    unsigned int GetDataLength() const { return mSize; }
};

const RilData *NetworkScanResultBuilder::Build(int status, int error,
        list<RIL_CellInfo_V1_5>& cellInfoList)
{
    NetworkScanResult *rildata = new NetworkScanResult(mHalVer);
    if (rildata != NULL) {
        rildata->SetStatus(status, error);
        CellInfoListBuilder cellInfoBuilder(mHalVer);
        const RilData *cellInfos = cellInfoBuilder.Build(cellInfoList);
        if (cellInfos != NULL) {
            unsigned int size = cellInfos->GetDataLength() / sizeof(RIL_CellInfo_V1_5);
            rildata->SetCellInfoList((RIL_CellInfo_V1_5 *)cellInfos->GetData(), size);
            delete cellInfos;
        }
    }
    return rildata;
}

const RilData *NetworkScanResultBuilder::Build(int status, int error,
        list<RIL_CellInfo_V1_6>& cellInfoList)
{
    NetworkScanResult *rildata = new NetworkScanResult(mHalVer);
    if (rildata != NULL) {
        rildata->SetStatus(status, error);
        CellInfoListBuilder cellInfoBuilder(mHalVer);
        const RilData *cellInfos = cellInfoBuilder.Build(cellInfoList);
        if (cellInfos != NULL) {
            if (mHalVer >= HAL_VERSION_CODE(1,6)) {
                unsigned int size = cellInfos->GetDataLength() / sizeof(RIL_CellInfo_V1_6);
                rildata->SetCellInfoList((RIL_CellInfo_V1_6 *)cellInfos->GetData(), size);
            } else {
                unsigned int size = cellInfos->GetDataLength() / sizeof(RIL_CellInfo_V1_5);
                rildata->SetCellInfoList((RIL_CellInfo_V1_5 *)cellInfos->GetData(), size);
            }
            delete cellInfos;
        }
    }
    return rildata;
}

/*
 * PhysicalChannelConfigs
 */
class PhysicalChannelConfigs : public RilData {
private:
    RIL_PhysicalChannelConfig_V1_6 mConfigs_V1_6[MAX_PHYSICAL_CHANNEL_CONFIGS];
    RIL_PhysicalChannelConfig_V1_4 mConfigs_V1_4[MAX_PHYSICAL_CHANNEL_CONFIGS];
    RIL_PhysicalChannelConfig mConfigs[MAX_PHYSICAL_CHANNEL_CONFIGS];
    int mSize;
    int mDataLength;
    void *mData;
    int mHalVer;
public:
    PhysicalChannelConfigs(int halVer) : mHalVer(halVer) {
        memset(&mConfigs_V1_6, 0, sizeof(mConfigs_V1_6));
        memset(&mConfigs_V1_4, 0, sizeof(mConfigs_V1_4));
        memset(&mConfigs, 0, sizeof(mConfigs));
        mSize = 0;
        mDataLength = 0;
        mData = NULL;
    }

    virtual ~PhysicalChannelConfigs() {
    }

    void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig& out, PhysicalChannelConfigsData& config) {
        out.status = (RIL_CellConnectionStatus)config.getStatus();
        out.cellBandwidthDownlink = config.getCellBandwidthDownlink();
    }

    void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& out, PhysicalChannelConfigsData& config) {
        out.status = (int)config.getStatus();
        out.cellBandwidthDownlink = config.getCellBandwidthDownlink();
        out.rat = (int)config.getRat();
        out.rfInfoType = config.getRfInfoType();
        if (out.rat == RADIO_TECH_NR) {
            out.rfInfo.range = config.getRange();
        } else {
            out.rfInfo.channelNumber = config.getChannelNumber();
        }
        out.len_contextIds = config.getLenContextIds();
        out.contextIds = config.getContextIds();
        out.physicalCellId = config.getPhysicalCellId();
    }

    void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_6& out, PhysicalChannelConfigsData& config) {
        out.status = (RIL_CellConnectionStatus)config.getStatus();
        out.rat = (RIL_RadioTechnology)config.getRat();
        out.downlinkChannelNumber = config.getChannelNumber();
        out.uplinkChannelNumber = config.getUplinkChannelNumber();
        out.cellBandwidthDownlink = config.getCellBandwidthDownlink();
        out.cellBandwidthUplink = config.getCellBandwidthUplink();
        out.len_contextIds = config.getLenContextIds();
        out.contextIds = config.getContextIds();
        out.physicalCellId = config.getPhysicalCellId();
        switch(NetworkUtils::getRadioTechnologyToAccessNetworkType((int)out.rat)) {
            case ACCESS_NETWORK_GERAN:
                out.band.geranBand = (RIL_GeranBands)config.getBandNumber();
                break;
            case ACCESS_NETWORK_UTRAN:
                out.band.utranBand = (RIL_UtranBands)config.getBandNumber();
                break;
            case ACCESS_NETWORK_EUTRAN:
                out.band.eutranBand = (RIL_EutranBands)config.getBandNumber();
                break;
            case ACCESS_NETWORK_NGRAN:
                out.band.ngranBand = (RIL_NgranBands)config.getBandNumber();
                break;
            default:
                break;
        }
    }

    void SetConfigs(list<PhysicalChannelConfigsData>& configList) {
        mSize = configList.size();
        if (mSize == 0 || mHalVer < HAL_VERSION_CODE(1,2)) {
            return ;
        }

        if (mSize > MAX_PHYSICAL_CHANNEL_CONFIGS) {
            mSize = MAX_PHYSICAL_CHANNEL_CONFIGS;
        }

        list<PhysicalChannelConfigsData>::iterator iter;
        int i = 0;
        for (iter = configList.begin(); iter != configList.end(); iter++) {
            PhysicalChannelConfigsData &cur = *iter;
            if (mHalVer < HAL_VERSION_CODE(1,4)) {
                // 1.2
                convertPhysicalChannelConfig(mConfigs[i], cur);
            } else if (mHalVer < HAL_VERSION_CODE(1,6)) {
                // 1.4
                convertPhysicalChannelConfig(mConfigs_V1_4[i], cur);
            } else {
                // 1.6 or higher
                convertPhysicalChannelConfig(mConfigs_V1_6[i], cur);
            }
            i++;
        }

        if (mHalVer < HAL_VERSION_CODE(1,4)) {
            // 1.2
            mData = mConfigs;
            mDataLength = sizeof(RIL_PhysicalChannelConfig) * mSize;
        } else if (mHalVer < HAL_VERSION_CODE(1,6)) {
            // 1.4
            mData = mConfigs_V1_4;
            mDataLength = sizeof(RIL_PhysicalChannelConfig_V1_4) * mSize;
        } else {
            // 1.6 or higher
            mData = mConfigs_V1_6;
            mDataLength = sizeof(RIL_PhysicalChannelConfig_V1_6) * mSize;
        }
    }

    void *GetData() const { return mData; }
    unsigned int GetDataLength() const { return mDataLength; }
};

/**
 * PhysicalChannelConfigsBuilder
 */
const RilData *PhysicalChannelConfigsBuilder::Build(std::list<PhysicalChannelConfigsData> &configList) {
    PhysicalChannelConfigs *rildata = new PhysicalChannelConfigs(mHalVer);
    rildata->SetConfigs(configList);
    return rildata;
}

/**
 * RestrictedStateBuilder
 */
const RilData* RestrictedStateBuilder::Build(const RIL_CellIdAndBarringInfo *info) {
    int32_t state = RIL_RESTRICTED_STATE_NONE;
    if (info) {
        if (info->cellIdentity.cellInfoType == RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA &&
            info->numOfrecord > 0 && info->precords != nullptr) {
            int32_t csBarred = RIL_RESTRICTED_STATE_NONE;
            int32_t psBarred = RIL_RESTRICTED_STATE_NONE;
            for (size_t i = 0; i < info->numOfrecord; i++) {
                const RIL_BarringInfo& barring = info->precords[i];
                if (barring.typeSpecificInfo.conditionalBarringInfo.isBarred) {
                    /* Block emergency call due to restriction. But allow all normal voice/SMS/USSD/SS/AV64. */
                    if (barring.service == EMERGENCY) {
                        csBarred |= RIL_RESTRICTED_STATE_CS_EMERGENCY;
                        RilLogV("CS_EMERGENCY restricted");
                    }

                    /* Block all normal voice/SMS/USSD/SS/AV64 due to restriction. Only Emergency call allowed. */
                    if (barring.service == CS_SERVICE) {
                        csBarred |= RIL_RESTRICTED_STATE_CS_NORMAL;
                        RilLogV("CS_NORMAL restricted");
                    }

                    /* Block packet data access due to restriction. */
                    if (barring.service == PS_SERVICE) {
                        psBarred = RIL_RESTRICTED_STATE_PS_ALL;
                        RilLogV("PS_ALL restricted");
                    }
                }
            }

            if (csBarred == (RIL_RESTRICTED_STATE_CS_EMERGENCY | RIL_RESTRICTED_STATE_CS_NORMAL)) {
                csBarred = RIL_RESTRICTED_STATE_CS_ALL;
                RilLogV("CS_ALL restricted");
            }
            state = (csBarred | psBarred);
        }
    }

    RilDataInts *rildata = new RilDataInts(1);
    if (rildata) {
        rildata->SetInt(0, state);
    }
    return rildata;
};

/*
 * LocationSantizedBarringInfo
 */
class LocationSantizedBarringInfo : public RilData {
private:
    size_t mSize;
    size_t mDataLength;
    size_t mTotalLength;
    char *mData;
public:
    LocationSantizedBarringInfo() {
        mData = NULL;
        mSize = 0;
        mDataLength = 0;
        mTotalLength = 0;
    }
    LocationSantizedBarringInfo(const LocationSantizedBarringInfo &) = delete;
    LocationSantizedBarringInfo& operator=(LocationSantizedBarringInfo const&) = delete;

    virtual ~LocationSantizedBarringInfo() {
        if (mData) {
            delete[] mData;
        }
    }

    void SetBarringInfo(size_t size, RIL_BarringInfo* data) {
        mSize = size;
        mDataLength = size * sizeof(RIL_BarringInfo);
        mTotalLength = sizeof(mSize) + mDataLength;

        if (data) {
            mData = new char[mTotalLength] {};
            memcpy(mData, &mSize, sizeof(mSize));
            memcpy(mData + sizeof(mSize), data, mDataLength);
        }
    }

    void *GetData() const { return mData; }
    unsigned int GetDataLength() const { return mTotalLength; }
};

/**
 * BarringInfoBuilder
 */
const RilData* BarringInfoBuilder::Build(const RIL_CellIdAndBarringInfo *info) {
    if (info == NULL) {
        return NULL;
    }

    LocationSantizedBarringInfo *rildata = new LocationSantizedBarringInfo();
    if (rildata != NULL) {
        rildata->SetBarringInfo(info->numOfrecord, info->precords);
    }
    return rildata;
};

/**
 * ActivityStatsInfoBuilder
 */
namespace V2_0 = android::hardware::radio::impl::modem::V2_0;

const RilData *ActivityStatsInfoBuilder::build(const V2_0::RIL_ActivityStatsInfo& info) {
    RilData *rildata = nullptr;
    if (mHalVer < HAL_VERSION_CODE(2, 0)) {
        // total sum
        RIL_ActivityStatsInfo stats {
            (uint32_t)info.sleepModeTimeMs, (uint32_t)info.idleModeTimeMs, {}, 0
        };

        for (auto& techSpecificInfo : info.techSpecificInfo) {
            for (size_t i = 0; i < techSpecificInfo.txmModetimeMs.size(); i++) {
                stats.tx_mode_time_ms[i] += techSpecificInfo.txmModetimeMs[i];
            }
            stats.rx_mode_time_ms += techSpecificInfo.rxModeTimeMs;
        }
        rildata = new RilDataPlaceHolder<RIL_ActivityStatsInfo>(stats);
    } else {
        rildata = new RilDataPlaceHolder<V2_0::RIL_ActivityStatsInfo>(info);
    }
    return rildata;
}
