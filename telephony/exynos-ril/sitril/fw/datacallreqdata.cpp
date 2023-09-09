/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "datacallreqdata.h"
#include "networkutils.h"
#include "rillog.h"

#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include <string>

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * SetupDataCallRequestData
 */
SetupDataCallRequestData::SetupDataCallRequestData(const int nReq, const Token tok, const ReqType type)
    : StringsRequestData(nReq, tok, type)
{
    mLinkAddresses = {};
    mSize = 0;
    mPduSessionId = 0;
    setActivatingPdpCtx(NULL);
    mEpdgOptSliceInfo = {};
    mTrafficDescriptor = {};
    mMatchedAllFlag =0;
    mAlwaysOnPdu = false;
    mVerifyDescriptor = {};
}

SetupDataCallRequestData::~SetupDataCallRequestData() {
    mSize = 0;
    setActivatingPdpCtx(NULL);
}

int SetupDataCallRequestData::encode(char *data, unsigned int datalen)
{
    RilLog("SetupDataCallRequestData::encode m_nReq=%d mHalVer=0x%x", m_nReq, mHalVer);

    if(data == NULL) {
        RilLog("Unexpected: data is null");
        return -1;
    }

    int halVer = GetHalVersion();
    int ret = 0;
    if (halVer >= HAL_VERSION_CODE(2, 0)
        && datalen == sizeof(DataV2_0::RIL_SetupDataCallInfo)) {
        return encode((DataV2_0::RIL_SetupDataCallInfo *)data);
    } else if (halVer >= HAL_VERSION_CODE(1, 6)
        && datalen == sizeof(RIL_SetupDataCallInfo_V1_6)) {
        /* This include extended TrafficDescriptor and VerifyDescriptor, not same to AOSP Radio HAL 1.6 */
        return encode((RIL_SetupDataCallInfo_V1_6 *)data);
    } else if (halVer == HAL_VERSION_CODE(1, 5)
            && datalen == sizeof(RIL_SetupDataCallInfo_V1_5)) {
        return encode((RIL_SetupDataCallInfo_V1_5 *)data);
    } else if (halVer >= HAL_VERSION_CODE(1, 4)
            && datalen == sizeof(RIL_SetupDataCallInfo_V1_4)) {
        RIL_SetupDataCallInfo_V1_5 setupDataInfo;
        RIL_SetupDataCallInfo_V1_4 *p_src = (RIL_SetupDataCallInfo_V1_4 *)data;
        if (p_src->addresses != NULL) {
            setupDataInfo.linkAddress = new RIL_LinkAddress[1];
        } else {
            setupDataInfo.linkAddress = NULL;
        }
        RadioConvUtils::convertSetupDataCallInfo(setupDataInfo, *((RIL_SetupDataCallInfo_V1_4 *)data));
        ret = encode(&setupDataInfo);

        // release temporary used memories
        RadioConvUtils::FreeDataProfileInfo(setupDataInfo.dataProfileInfo);

        if (setupDataInfo.linkAddress != NULL) {
            delete[] setupDataInfo.linkAddress;
            setupDataInfo.linkAddress = NULL;
        }

        return ret;
    }

    if (StringsRequestData::encode(data, datalen) < 0)
        return -1;

    return (m_ptrStrings == NULL || (m_nStringCount < 5) ? -1 : 0);
}

int SetupDataCallRequestData::encode(RIL_SetupDataCallInfo_V1_5 *dataCallInfo)
{
    if (dataCallInfo == NULL) {
        return -1;
    }

    if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfo, dataCallInfo->dataProfileInfo)) {
        RilLogW("[SetupDataCallRequestData] Failed to CopyDataProfileInfo");
        return -1;
    }

    mSize = dataCallInfo->leng_addresses;
    mLinkAddresses.resize(mSize);

    for (int i = 0; i < mSize; i++) {
        RadioConvUtils::CopyLinkAddress(mLinkAddresses[i], ((RIL_LinkAddress *) dataCallInfo->linkAddress)[i]);
    }

    string strRat = std::to_string((int) NetworkUtils::getRadioTechnologyFromAccessNetwork(dataCallInfo->accessNetwork) + 2);
    string strReason = std::to_string(dataCallInfo->reason);
    const char *params[] = {
        strRat.c_str(),
        dataCallInfo->roamingAllow ? "1" : "0",
        strReason.c_str(),
        dataCallInfo->dnses,
    };

    return StringsRequestData::encode((char *)params, sizeof(params));
}

int SetupDataCallRequestData::encode(DataV2_0::RIL_SetupDataCallInfo *dataCallInfo) {
    RilLog("encode V2_0::RIL_SetupDataCallInfo");
    if (dataCallInfo == nullptr) {
        return -1;
    }
    if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfo, dataCallInfo->dataProfileInfo)) {
        RilLogW("[SetupDataCallRequestData] Failed to CopyDataProfileInfo");
        return -1;
    }
    mSize = dataCallInfo->linkAddresses.size();
    mLinkAddresses = dataCallInfo->linkAddresses;

    string strRat = std::to_string((int) NetworkUtils::getRadioTechnologyFromAccessNetwork(dataCallInfo->accessNetwork) + 2);
    string strReason = std::to_string(dataCallInfo->reason);
    for(auto dns : dataCallInfo->dnses) {
        mDnses += dns;
        mDnses += " ";
    }
    mDnses.pop_back();
    const char *params[] = {
        strRat.c_str(),
        dataCallInfo->roamingAllow ? "1" : "0",
        strReason.c_str(),
        mDnses.c_str(),
    };

    mPduSessionId = dataCallInfo->pduSessionId;
    mEpdgOptSliceInfo = dataCallInfo->sliceInfo;

    mTrafficDescriptor = dataCallInfo->tdInfo;
    PrintDebugInfo();

    mMatchedAllFlag = dataCallInfo->matchAllRuleAllowed;

    mVerifyDescriptor = dataCallInfo->vdInfo;

    mAlwaysOnPdu = dataCallInfo->mAlwaysOnPdu;

    // Send legacy paramter thru StringRequestData::encode
    return StringsRequestData::encode((char *)params, sizeof(params));
}

int SetupDataCallRequestData::encode_aosp(RIL_SetupDataCallInfo_V1_6 *dataCallInfo)
{
    int ret = encode((RIL_SetupDataCallInfo_V1_5 *)dataCallInfo);
    if (ret < 0) return ret;
    mPduSessionId = dataCallInfo->pduSessionId;

    if (dataCallInfo->OptionalSliceinfoFlag == RIL_OPT_INFO_VALID) {
        mEpdgOptSliceInfo = dataCallInfo->sliceInfo;
    }

    if (dataCallInfo->OptionalTrafficDescriptorFlag == RIL_OPT_INFO_VALID) {
        if (!mTrafficDescriptor.has_value()) {
            mTrafficDescriptor = DataV2_0::RIL_TrafficDescriptor { };
        }
        if (dataCallInfo->tdInfo.dnnFlag == RIL_OPT_INFO_VALID) {
            RadioConvUtils::convertDnn(mTrafficDescriptor->dnn, dataCallInfo->tdInfo.dnn);
        }
        if(dataCallInfo->tdInfo.osAppIdFlag == RIL_OPT_INFO_VALID) {
            RadioConvUtils::convertOSAppId(mTrafficDescriptor->osAppIds, dataCallInfo->tdInfo.osAppId);
        }
    }

    PrintDebugInfo();

    mMatchedAllFlag = dataCallInfo->matchAllRuleAllowed;
    return 0;
}


int SetupDataCallRequestData::encode(RIL_SetupDataCallInfo_V1_6 *dataCallInfo)
{
    int ret = encode_aosp(dataCallInfo);
    if (ret < 0) return ret;

    if (mTrafficDescriptor.has_value()) {
        if (dataCallInfo->tdInfo.ipDescriptorsFlag) {
            mTrafficDescriptor->ipDescriptors = dataCallInfo->tdInfo.ipDescriptors;
        }
        if (dataCallInfo->tdInfo.domainDescriptorsFlag) {
            mTrafficDescriptor->domainDescriptors.clear();
            mTrafficDescriptor->domainDescriptors = dataCallInfo->tdInfo.domainDescriptors;
        }
        if (dataCallInfo->tdInfo.connectionCapabilitiesFlag) {
            mTrafficDescriptor->connectionCapabilities.clear();
            mTrafficDescriptor->connectionCapabilities = dataCallInfo->tdInfo.connectionCapabilities;
        }

        mTrafficDescriptor->ip3Tuple = dataCallInfo->tdInfo.ip3Tuple;
        mTrafficDescriptor->matchAllType = dataCallInfo->tdInfo.matchAllType;
        mTrafficDescriptor->tdMask = dataCallInfo->tdInfo.tdMask;
    }

    mVerifyDescriptor = dataCallInfo->vdInfo;

    return 0;
}

SetupDataCallRequestData *SetupDataCallRequestData::Clone() const
{
    SetupDataCallRequestData *p = new SetupDataCallRequestData(m_nReq, m_tok, m_reqType);
    if (p != NULL) {
        RadioConvUtils::CopyDataProfileInfo(p->mDataProfileInfo, mDataProfileInfo);
        p->encode((char *)m_ptrStrings, m_nStringCount * sizeof(char *));
    }
    return p;
}

int SetupDataCallRequestData::GetRadioTech() const
{
    const char *strRat = GetString(0);
    if (strRat != NULL) {
        int rat = strtol(strRat, NULL, 10);
        // Radio Tech
        if (rat == 0) {
            rat = RADIO_TECH_IS95A;
        }
        else if (rat == 1) {
            rat = RADIO_TECH_UMTS;
        }
        else {
            rat = rat - 2;
        }
        return rat;
    }
    return RADIO_TECH_UMTS;
}

int SetupDataCallRequestData::GetDataProfileId() const
{
    return mDataProfileInfo.profileId;
}

const char *SetupDataCallRequestData::GetApn() const
{
    return mDataProfileInfo.apn.c_str();
}

const char *SetupDataCallRequestData::GetUsername() const
{
    return mDataProfileInfo.user.c_str();
}

const char *SetupDataCallRequestData::GetPassword() const
{
    return mDataProfileInfo.password.c_str();
}

int SetupDataCallRequestData::GetAuthType() const
{
    return mDataProfileInfo.authType;
}

const char *SetupDataCallRequestData::GetProtocolByInt(int protocol) const
{
    switch (protocol) {
    case PDP_PROTOCOL_TYPE_IP:
        return "IP";
    case PDP_PROTOCOL_TYPE_IPV6:
        return "IPV6";
    case PDP_PROTOCOL_TYPE_IPV4V6:
        return "IPV4V6";
    case PDP_PROTOCOL_TYPE_NON_IP:
        return "NONIP";
    default:
        break;
    }
    return "IP";
}

const char *SetupDataCallRequestData::GetProtocol() const
{
    return GetProtocolByInt(mDataProfileInfo.protocol);
}

const char *SetupDataCallRequestData::GetRoamingProtocol() const
{
    return GetProtocolByInt(mDataProfileInfo.roamingProtocol);
}

int SetupDataCallRequestData::GetSupportedApnTypesBitmap() const
{
    return mDataProfileInfo.supportedApnTypesBitmap;
}

int SetupDataCallRequestData::GetRadioAccessFamily() const
{
    return mDataProfileInfo.bearerBitmap;
}

bool SetupDataCallRequestData::GetAPNSettingStatus() const
{
    return mDataProfileInfo.enabled;
}

int SetupDataCallRequestData::GetMtuSize() const
{
    return mDataProfileInfo.mtuV4;
}

int SetupDataCallRequestData::GetMtuV6Size() const
{
    return mDataProfileInfo.mtuV6;
}

const char *SetupDataCallRequestData::IsDataRoamingAllowed() const
{
    return GetString(1);
}

bool SetupDataCallRequestData::IsRoamingAllowed() const
{
    const char *isRoamingAllowed = GetString(1);
    if (isRoamingAllowed != NULL) {
        return (strtol(isRoamingAllowed, NULL, 10) == 1) ? true : false;
    }
    return false;
}

const char *SetupDataCallRequestData::GetReason() const
{
    return GetString(2);
}

/* Deprecated, only used in HAL 1.4 and only support one Handover IP */
const char *SetupDataCallRequestData::GetAddresses() const
{
    return mLinkAddresses[0].address.c_str();
}

const char *SetupDataCallRequestData::GetDnses() const {
    return GetString(3);
}

int SetupDataCallRequestData::GetPduSessionId() const
{
    return mPduSessionId;
}

bool SetupDataCallRequestData::GetMatchedAllFlag() const
{
    return mMatchedAllFlag;
}

bool SetupDataCallRequestData::GetAlwaysOnPdu() const
{
    return mAlwaysOnPdu;
}

void SetupDataCallRequestData::PrintDebugInfo() {
    if (mEpdgOptSliceInfo.has_value()) {
        RilLogV("SliceInfo {%d, %d, %d, %d}",
            mEpdgOptSliceInfo->sst, mEpdgOptSliceInfo->sliceDifferentiator,
            mEpdgOptSliceInfo->mappedHplmnSst, mEpdgOptSliceInfo->mappedHplmnSD);
    } else {
        RilLogV("SliceInfo is empty");
    }
    if (mTrafficDescriptor.has_value()) {
        if (mTrafficDescriptor->ip3Tuple.empty()) {
            RilLogV("ip3tuple is empty, unexpected");
        } else {
            RilLogV("ip3tuple size: %d, ipv4 size:%d", mTrafficDescriptor->ip3Tuple.size(), mTrafficDescriptor->ip3Tuple[0].ipv4.size());
            if (!mTrafficDescriptor->ip3Tuple[0].ipv4.empty()) {
                RilLogV("ipv4[0] addr: %s", mTrafficDescriptor->ip3Tuple[0].ipv4[0].c_str());
            } else {
                RilLogV("ipv4 is empty");
            }
        }
    } else {
        RilLogV("trafficDescriptor is empty");
    }
}


/**
 * DeactivateDataCallRequestData
 */
DeactivateDataCallRequestData::DeactivateDataCallRequestData(const int nReq, const Token tok, const ReqType type)
    : StringsRequestData(nReq, tok, type)
{
}

DeactivateDataCallRequestData::~DeactivateDataCallRequestData()
{
}

INT32 DeactivateDataCallRequestData::encode(char *data, unsigned int datalen)
{
    if (StringsRequestData::encode(data, datalen) < 0)
        return -1;

    return (m_ptrStrings == NULL || (m_nStringCount != 2) ? -1 : 0);
}

DeactivateDataCallRequestData *DeactivateDataCallRequestData::Clone() const
{
    DeactivateDataCallRequestData *p = new DeactivateDataCallRequestData(m_nReq, m_tok, m_reqType);
    if (p != NULL) {
        p->encode((char *)m_ptrStrings, m_nStringCount * sizeof(char *));
        p->mHalVer = mHalVer;
    }
    return p;
}

int DeactivateDataCallRequestData::GetCid() const
{
    const char *strCid = GetString(0);
    if (strCid != NULL && TextUtils::IsDigitsOnly(strCid)) {
        return strtol(strCid, NULL, 10);
    }
    return -1;
}

int DeactivateDataCallRequestData::GetDisconnectReason() const
{
    int reason = DEACT_REASON_NORMAL;
    const char *strReason = GetString(1);
    if (strReason != NULL && TextUtils::IsDigitsOnly(strReason)) {
        reason = strtol(strReason, NULL, 10);
        if (mHalVer >= HAL_VERSION_CODE(1,2)) {
            switch (reason) {
            case RIL_DataRequestReason::NORMAL:
                reason = DEACT_REASON_NORMAL;
                break;
            case RIL_DataRequestReason::SHUTDOWN:
                reason = DEACT_REASON_RADIO_SHUTDOWN;
                break;
            case RIL_DataRequestReason::HANDOVER:
                reason = DEACT_REASON_HANDOVER;
                break;
            case RIL_DataRequestReason::PDP_RESET:
                reason = DEACT_REASON_PDP_RESET;
                break;
            default:
                reason = DEACT_REASON_NORMAL;
                break;
            }
        }
    }
    return reason;
}

/**
 * SetInitialAttachApnRequestData
 */
SetInitialAttachApnRequestData::SetInitialAttachApnRequestData(const int nReq,const Token tok,const ReqType type)
    : RequestData(nReq, tok, type)
{
}

SetInitialAttachApnRequestData::~SetInitialAttachApnRequestData()
{
}

INT32 SetInitialAttachApnRequestData::encode(char *data, unsigned int datalen)
{
    if (data == NULL || datalen == 0) {
         return -1;
    }

    if (mHalVer >= HAL_VERSION_CODE(2, 0)
        && datalen == sizeof(DataV2_0::RIL_DataProfileInfo)) {
        return encode((DataV2_0::RIL_DataProfileInfo *)data, datalen);
    } else if (mHalVer >= HAL_VERSION_CODE(1, 5)) {
        return encode((RIL_DataProfileInfo_V1_5 *)data, datalen);
    } else if (mHalVer >= HAL_VERSION_CODE(1, 4)) {
        RIL_DataProfileInfo_V1_5 dataProInfo;
        RadioConvUtils::CopyDataProfileInfo(dataProInfo, *((RIL_DataProfileInfo_V1_4 *)data));
        int ret = encode(&dataProInfo, sizeof(RIL_DataProfileInfo_V1_5));
        // release temporary used memories
        RadioConvUtils::FreeDataProfileInfo(dataProInfo);
        return ret;
    } else {
        // legacy, push up with V1_5
        if (datalen == sizeof(RIL_InitialAttachApn_v15)) {
            RIL_DataProfileInfo_V1_5 dataProInfo;
            RadioConvUtils::CopyDataProfileInfo(dataProInfo, *((RIL_InitialAttachApn_v15 *)data));
            int ret = encode(&dataProInfo, sizeof(RIL_DataProfileInfo_V1_5));
            // release temporary used memories
            RadioConvUtils::FreeDataProfileInfo(dataProInfo);
            return ret;
        }
    }
    // not to support old data structure (RIL_VERSION < 15)
    RilLogW("unexpected datalen(%lu) of RIL_DataProfileInfo_V1_4 or RIL_InitialAttachApn_v15", datalen);
    return -1;
}

int SetInitialAttachApnRequestData::encode(DataV2_0::RIL_DataProfileInfo *dataProfileInfo, size_t size)
{
    if (dataProfileInfo == NULL || size != sizeof(DataV2_0::RIL_DataProfileInfo)) {
        RilLogW("[SetInitialAttachApnRequestData] Unexpected datalen(%zu) for RIL_SetupDataCallInfo_V2_0", size);
        return -1;
    }

    if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfo, *dataProfileInfo)) {
        RilLogW("[SetInitialAttachApnRequestData] Failed to CopyDataProfileInfo");
        return -1;
    }

    if (dataProfileInfo->tdInfo.has_value()) {
        if (!dataProfileInfo->tdInfo->dnn.empty()) {
            mDataProfileInfo.tdInfo->dnn = dataProfileInfo->tdInfo->dnn;
        }
        if (!dataProfileInfo->tdInfo->osAppIds.empty()) {
            mDataProfileInfo.tdInfo->osAppIds = dataProfileInfo->tdInfo->osAppIds;
        }
    }

    return 0;
}

int SetInitialAttachApnRequestData::encode(RIL_DataProfileInfo_V1_5 *dataProfileInfo, size_t size)
{
    if (dataProfileInfo == NULL || size != sizeof(RIL_DataProfileInfo_V1_5)) {
        RilLogW("[SetInitialAttachApnRequestData] Unexpected datalen(%zu) for RIL_DataProfileInfo_V1_5", size);
        return -1;
    }

    if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfo, *dataProfileInfo)) {
        RilLogW("[SetInitialAttachApnRequestData] Failed to CopyDataProfileInfo");
        return -1;
    }

    return 0;
}

int SetInitialAttachApnRequestData::encode(RIL_InitialAttachApn_v15 *iaa, size_t size)
{
    if (iaa == NULL || size != sizeof(RIL_InitialAttachApn_v15)) {
        RilLogW("[SetInitialAttachApnRequestData] Unexpected datalen(%zu) for RIL_InitialAttachApn_v15", size);
        return -1;
    }

    mDataProfileInfo.apn = (iaa->apn != nullptr) ? iaa->apn : "";
    mDataProfileInfo.user = (iaa->username != nullptr) ? iaa->username : "";
    mDataProfileInfo.password = (iaa->password != nullptr) ? iaa->password : "";
    mDataProfileInfo.protocol = PDP_PROTOCOL_TYPE_IP;
    if (TextUtils::Equals(iaa->protocol, "IP")) {
        mDataProfileInfo.protocol = PDP_PROTOCOL_TYPE_IP;
    }
    else if (TextUtils::Equals(iaa->protocol, "IPV6")) {
        mDataProfileInfo.protocol = PDP_PROTOCOL_TYPE_IPV6;
    }
    else if (TextUtils::Equals(iaa->protocol, "IPV4V6")) {
        mDataProfileInfo.protocol = PDP_PROTOCOL_TYPE_IPV4V6;
    }
    mDataProfileInfo.authType = (RIL_ApnAuthType)iaa->authtype;
    mDataProfileInfo.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
    if (TextUtils::Equals(iaa->roamingProtocol, "IP")) {
        mDataProfileInfo.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
    }
    else if (TextUtils::Equals(iaa->roamingProtocol, "IPV6")) {
        mDataProfileInfo.roamingProtocol = PDP_PROTOCOL_TYPE_IPV6;
    }
    else if (TextUtils::Equals(iaa->roamingProtocol, "IPV4V6")) {
        mDataProfileInfo.roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6;
    }
    mDataProfileInfo.enabled = true;
    mDataProfileInfo.supportedApnTypesBitmap = iaa->supportedTypesBitmask;
    mDataProfileInfo.bearerBitmap = iaa->bearerBitmask;
    mDataProfileInfo.mtuV4 = iaa->mtu;
    mDataProfileInfo.mtuV6 = iaa->mtu;
    return 0;
}

SetInitialAttachApnRequestData *SetInitialAttachApnRequestData::Clone() const
{
    int request = ENCODE_REQUEST(m_nReq, HAL_VERSION_CODE(1,5));
    SetInitialAttachApnRequestData *p = new SetInitialAttachApnRequestData(request, m_tok, m_reqType);
    if (p != NULL) {
        p->encode((char *)&mDataProfileInfo, sizeof(mDataProfileInfo));
        p->mHalVer = mHalVer;
    }
    return p;
}

const char *SetInitialAttachApnRequestData::GetApn() const
{
    return mDataProfileInfo.apn.c_str();
}

const char *SetInitialAttachApnRequestData::GetUsername() const
{
    return mDataProfileInfo.user.c_str();
}

const char *SetInitialAttachApnRequestData::GetPassword() const
{
    return mDataProfileInfo.password.c_str();
}

const char *SetInitialAttachApnRequestData::GetProtocol() const
{
    switch (mDataProfileInfo.protocol) {
    case PDP_PROTOCOL_TYPE_IP:
        return "IP";
    case PDP_PROTOCOL_TYPE_IPV6:
        return "IPV6";
    case PDP_PROTOCOL_TYPE_IPV4V6:
        return "IPV4V6";
    case PDP_PROTOCOL_TYPE_NON_IP:
        return "NONIP";
    default:
        break;
    }
    return "IP";
}

int SetInitialAttachApnRequestData::GetPdpProtocolType() const{
    return mDataProfileInfo.protocol;
}

int SetInitialAttachApnRequestData::GetAuthType() const
{
    return mDataProfileInfo.authType;
}

const char *SetInitialAttachApnRequestData::GetRoamingProtocol() const
{
    switch (mDataProfileInfo.roamingProtocol) {
    case PDP_PROTOCOL_TYPE_IP:
        return "IP";
    case PDP_PROTOCOL_TYPE_IPV6:
        return "IPV6";
    case PDP_PROTOCOL_TYPE_IPV4V6:
        return "IPV4V6";
    case PDP_PROTOCOL_TYPE_NON_IP:
        return "NONIP";
    default:
        break;
    }
    return "IP";
}

int SetInitialAttachApnRequestData::GetPdpRoamingProtocolType() const
{
    return mDataProfileInfo.roamingProtocol;
}

int SetInitialAttachApnRequestData::GetSupportedApnTypesBitmap() const
{
    return mDataProfileInfo.supportedApnTypesBitmap;
}

int SetInitialAttachApnRequestData::GetBearerBitmap() const
{
    return mDataProfileInfo.bearerBitmap;
}

int SetInitialAttachApnRequestData::GetModemCognitive() const
{
    return mDataProfileInfo.persistent;
}

int SetInitialAttachApnRequestData::GetMtu() const
{
    return mDataProfileInfo.mtuV4;
}

int SetInitialAttachApnRequestData::GetMtuV6() const
{
    return mDataProfileInfo.mtuV6;
}

bool SetInitialAttachApnRequestData::IsEnabled() const
{
    return mDataProfileInfo.enabled;
}

bool SetInitialAttachApnRequestData::IsPrefferred() const
{
    return mDataProfileInfo.preferred;
}

bool SetInitialAttachApnRequestData::IsPersist() const
{
    return mDataProfileInfo.persistent;
}

/**
 * SetDataProfileRequestData
 */
SetDataProfileRequestData::SetDataProfileRequestData(const int nReq,const Token tok,const ReqType type)
    : RequestData(nReq, tok, type), mDataProfileInfos(nullptr), mSize(0)
{
}

SetDataProfileRequestData::~SetDataProfileRequestData()
{
    if (mDataProfileInfos != NULL) {
        delete[] mDataProfileInfos;
        mDataProfileInfos = NULL;
    }
    mSize = 0;
}

int SetDataProfileRequestData::encode(char *data, unsigned int datalen)
{
    if (data == NULL || datalen == 0) {
        return -1;
    }

    if (datalen % sizeof(void *) != 0) {
        return -1;
    }

    size_t size = datalen / sizeof(void *);
    if (mHalVer >= HAL_VERSION_CODE(2, 0)) {
        return encode((DataV2_0::RIL_DataProfileInfo **)data, size);
    } else if (mHalVer >= HAL_VERSION_CODE(1, 5)) {
        return encode((RIL_DataProfileInfo_V1_5 **)data, size);
    } else if (mHalVer == HAL_VERSION_CODE(1, 4)) {
        return encode((RIL_DataProfileInfo_V1_4 **)data, size);
    } else {
        return encode((RIL_DataProfileInfo_v15 **)data, size);
    }
}

int SetDataProfileRequestData::encode(DataV2_0::RIL_DataProfileInfo **dataProfileInfo, size_t size)
{
    if (dataProfileInfo == NULL || size == 0) {
        return -1;
    }

    mDataProfileInfos = new DataV2_0::RIL_DataProfileInfo[size];
    mSize = size;
    for (size_t i = 0; i < size; i++) {
        DataV2_0::RIL_DataProfileInfo *dpi = dataProfileInfo[i];
        if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfos[i], *dpi)) {
            return -1;
        }

        if (dpi->tdInfo.has_value()) {
            if (!dpi->tdInfo->dnn.empty()) {
                mDataProfileInfos[i].tdInfo->dnn = dpi->tdInfo->dnn;
            }
            if (!dpi->tdInfo->osAppIds.empty()) {
                mDataProfileInfos[i].tdInfo->osAppIds = dpi->tdInfo->osAppIds;
            }
        }
    }

    return 0;
}

int SetDataProfileRequestData::encode(RIL_DataProfileInfo_V1_5 **dataProfileInfo, size_t size)
{
    if (dataProfileInfo == NULL || size == 0) {
        return -1;
    }

    mDataProfileInfos = new DataV2_0::RIL_DataProfileInfo[size];
    mSize = size;
    for (size_t i = 0; i < size; i++) {
        RIL_DataProfileInfo_V1_5 *dpi = dataProfileInfo[i];
        if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfos[i], *dpi)) {
            return -1;
        }
    } // end for i ~

    return 0;
}

int SetDataProfileRequestData::encode(RIL_DataProfileInfo_V1_4 **dataProfileInfo, size_t size)
{
    if (dataProfileInfo == NULL || size == 0) {
        return -1;
    }

    mDataProfileInfos = new DataV2_0::RIL_DataProfileInfo[size];
    mSize = size;
    for (size_t i = 0; i < size; i++) {
        RIL_DataProfileInfo_V1_4 *dpi = dataProfileInfo[i];
        if (!RadioConvUtils::CopyDataProfileInfo(mDataProfileInfos[i], *dpi)) {
            return -1;
        }
    } // end for i ~

    return 0;
}

int SetDataProfileRequestData::encode(RIL_DataProfileInfo_v15 **dataProfileInfo, size_t size)
{
    if (dataProfileInfo == NULL || size == 0) {
        return -1;
    }

    mDataProfileInfos = new DataV2_0::RIL_DataProfileInfo[size];
    mSize = size;

    for (unsigned int i = 0; i < size; i++) {

        RIL_DataProfileInfo_v15 *dpi = ((RIL_DataProfileInfo_v15 **) dataProfileInfo)[i];
        RadioConvUtils::CopyDataProfileInfo(mDataProfileInfos[i], *dpi);
    } // end for i ~
    return 0;
}

SetDataProfileRequestData *SetDataProfileRequestData::Clone() const {
    // encode by 2.0 then replace the current HAL version code
    SetDataProfileRequestData *p = new SetDataProfileRequestData(
        ENCODE_REQUEST(m_nReq, HAL_VERSION_CODE(2, 0)), m_tok, m_reqType);
    if (p != NULL) {
        if (mSize > 0) {
            DataV2_0::RIL_DataProfileInfo **dataProfilePtrs = new DataV2_0::RIL_DataProfileInfo * [mSize];
            if (dataProfilePtrs != NULL) {
                for (int i = 0; i < mSize; i++) {
                    dataProfilePtrs[i] = &mDataProfileInfos[i];
                }
                p->encode((char *) dataProfilePtrs, sizeof(DataV2_0::RIL_DataProfileInfo *) * mSize);
                delete[] dataProfilePtrs;
                p->mHalVer = mHalVer;
            }
        }
    }
    return p;
}

const DataV2_0::RIL_DataProfileInfo *SetDataProfileRequestData::GetDataProfileInfo(int index) const
{
    if (index < 0 || index >= mSize)
        return NULL;
    return mDataProfileInfos + index;
}

int SetDataProfileRequestData::GetSize() const
{
    return mSize;
}


/**
 * KeepaliveRequestData
 */
KeepaliveRequestData::KeepaliveRequestData(const int nReq, const Token tok, const ReqType type)
    : RequestData(nReq, tok, type)
{
    memset(&m_keepAliveReq, 0, sizeof(RIL_KeepaliveRequest));
}

KeepaliveRequestData::~KeepaliveRequestData()
{
}

int KeepaliveRequestData::encode(char *data, unsigned int datalen)
{
    int lengthStr;
    if((0 == datalen) || (NULL == data)) return -1;

    RIL_KeepaliveRequest *reqData = (RIL_KeepaliveRequest *)data;

    m_keepAliveReq.type = reqData->type;
    lengthStr = strlen(reqData->sourceAddress);
    if (lengthStr > MAX_INADDR_LEN ) lengthStr = MAX_INADDR_LEN;
    memcpy(m_keepAliveReq.sourceAddress, reqData->sourceAddress, lengthStr);
    m_keepAliveReq.sourcePort = reqData->sourcePort;

    lengthStr = strlen(reqData->destinationAddress);
    if (lengthStr > MAX_INADDR_LEN ) lengthStr = MAX_INADDR_LEN;
    memcpy(m_keepAliveReq.destinationAddress, reqData->destinationAddress, lengthStr);
    m_keepAliveReq.destinationPort = reqData->destinationPort;

    m_keepAliveReq.maxKeepaliveIntervalMillis = reqData->maxKeepaliveIntervalMillis;
    m_keepAliveReq.cid = reqData->cid;

    return 0;
}

KeepaliveRequestData *KeepaliveRequestData::Clone() const
{
    KeepaliveRequestData *p = new KeepaliveRequestData(m_nReq, m_tok, m_reqType);
    if (p != NULL) {
        p->encode((char *)&m_keepAliveReq, sizeof(RIL_KeepaliveRequest));
    }
    return p;
}

/**
 * SetPdnIpAddressRequestData
 */
SetPdnIpAddressRequestData::SetPdnIpAddressRequestData(const int nReq, const Token tok, const ReqType type)
    : RequestData(nReq, tok, type)
{
    memset(&m_SetPdnIpAddressReq, 0, sizeof(RIL_SetPdnIpAddressRequest));
}

SetPdnIpAddressRequestData::~SetPdnIpAddressRequestData()
{
}

int SetPdnIpAddressRequestData::encode(char *data, unsigned int datalen)
{
    if((0 == datalen) || (NULL == data)) return -1;

    RIL_SetPdnIpAddressRequest *reqData = (RIL_SetPdnIpAddressRequest *)data;

    m_SetPdnIpAddressReq.cid = reqData->cid;
    m_SetPdnIpAddressReq.type = reqData->type;

    if (reqData->type == PDP_PROTOCOL_TYPE_IPV4V6 || reqData->type == PDP_PROTOCOL_TYPE_IP) {
        memcpy(m_SetPdnIpAddressReq.IPv4_addresses, reqData->IPv4_addresses, MAX_IPV4_ADDR_LEN);
    }
    else if (reqData->type == PDP_PROTOCOL_TYPE_IPV4V6 ||reqData->type == PDP_PROTOCOL_TYPE_IPV6) {
        memcpy(m_SetPdnIpAddressReq.IPv6_addresses, reqData->IPv6_addresses, MAX_IPV6_ADDR_LEN);
    }

    return 0;
}

SetPdnIpAddressRequestData *SetPdnIpAddressRequestData::Clone() const
{
    SetPdnIpAddressRequestData *p = new SetPdnIpAddressRequestData(m_nReq, m_tok, m_reqType);
    if (p != NULL) {
        p->encode((char *)&m_SetPdnIpAddressReq, sizeof(SetPdnIpAddressRequestData));
    }
    return p;
}

/**
 * SetUrspRequestData
 */
SetUrspRequestData::SetUrspRequestData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    memset(&m_reqData, 0, sizeof(m_reqData));
}

SetUrspRequestData::~SetUrspRequestData()
{
}

INT32 SetUrspRequestData::encode(char *data, unsigned int length)
{
    //Ursp Configuration would be set by test application
    //This shall be exactly same as the definition of set at the application.
    if ((0 == length) || (NULL == data))  return -1;
    if (length == sizeof(UrspData)) {
        memcpy(&m_reqData, data, sizeof(UrspData));
    }

    return 0;
}

