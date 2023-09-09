/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include "commands.h"
#include "commonvendorstructs.h"
#include "vendorstructs.h"
#include <aidl-log.h>
#include <ril_internal.h>
#include "ril_service_common.h"


namespace android::hardware::radio::impl::data {

namespace aidl_data = ::aidl::android::hardware::radio::data;

void freeRilLinkAddress(RIL_LinkAddress& out) {
    memsetAndFreeStrings(1, out.address);
    out.address = NULL;
}

bool copyAidlStringToRil(char **dest, const std::string &src, android::RequestInfo *pRI, bool allowEmpty) {
    size_t len = src.size();
    if (len == 0 && !allowEmpty) {
        *dest = nullptr;
        return true;
    }
    *dest = (char *) calloc(len + 1, sizeof(char));
    if (*dest == nullptr) {
        RLOGE("Memory allocation failed for request %s", android::requestToString(pRI->pCI->requestNumber));
        //sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        return false;
    }
    if (strlcpy(*dest, src.c_str(), len + 1) >= (len + 1)) {
        RLOGE("Copy of the HIDL string has been truncated, as "
              "the string length reported by size() does not "
              "match the length of string returned by c_str().");
        free(*dest);
        *dest = nullptr;
        //sendErrorResponse(pRI, RIL_E_INTERNAL_ERR);
        return false;
    }
    return true;
}

void convertHalLinkAddressToRil(DataV2_0::RIL_LinkAddress& out, const aidl_data::LinkAddress& linkAddress) {
    out.address = linkAddress.address;
    out.properties = linkAddress.addressProperties;
    out.deprecationTime = linkAddress.deprecationTime;
    out.expirationTime = linkAddress.expirationTime;
}

bool convertHalDataProfileToRilV2_0(V2_0::RIL_DataProfileInfo& out, const aidl_data::DataProfileInfo& dataProfileInfo) {

    out.apn = dataProfileInfo.apn;
    RLOGD("[%s]apn:%s", __FUNCTION__, out.apn.c_str());
    out.user = dataProfileInfo.user;
    out.password = dataProfileInfo.password;

    out.profileId = (RIL_DataProfileId) dataProfileInfo.profileId;
    out.protocol = (PdpProtocolType) dataProfileInfo.protocol;
    out.roamingProtocol = (PdpProtocolType) dataProfileInfo.roamingProtocol;
    out.authType = (RIL_ApnAuthType) dataProfileInfo.authType;
    out.type = (RIL_DataProfileInfoType) dataProfileInfo.type;
    out.maxConnsTime = dataProfileInfo.maxConnsTime;
    out.maxConns = dataProfileInfo.maxConns;
    out.waitTime = dataProfileInfo.waitTime;
    out.enabled = dataProfileInfo.enabled;
    out.supportedApnTypesBitmap = (int)dataProfileInfo.supportedApnTypesBitmap;
    out.bearerBitmap = dataProfileInfo.bearerBitmap;
    out.mtuV4 = dataProfileInfo.mtuV4;
    out.mtuV6 = dataProfileInfo.mtuV6;
    out.preferred = dataProfileInfo.preferred;
    out.persistent = dataProfileInfo.persistent;
    out.alwaysOn= dataProfileInfo.alwaysOn;

    return true;
}

bool convertHalTDToRil(RIL_TrafficDescriptor_aosp& outTdInfo, RIL_OptionalInfoFlag& outTdFlag,
                             const std::optional<aidl_data::TrafficDescriptor> trafficDescriptor){
    outTdInfo.dnn.clear();
    outTdInfo.osAppId.clear();
    if(trafficDescriptor != std::nullopt) {
        aidl_data::TrafficDescriptor tdInfo = trafficDescriptor.value();
        outTdFlag = RIL_OPT_INFO_VALID;
        if(tdInfo.dnn != std::nullopt){
            RLOGD("%s: set dnn Info", __FUNCTION__);
            outTdInfo.dnnFlag = RIL_OPT_INFO_VALID;
            RIL_OptionalDNN dnn;
            memset(dnn.value, 0, sizeof(RIL_OptionalDNN));
            const char* input = tdInfo.dnn.value().c_str();

            for(size_t i=0;i<tdInfo.dnn.value().size();i++)
                dnn.value[i] =input[i];

            outTdInfo.dnn.push_back(dnn);
        } else {
            RLOGD("%s: no dnn info", __FUNCTION__);
            outTdInfo.dnnFlag = RIL_OPT_INFO_INVALID;
        }
        if(tdInfo.osAppId != std::nullopt){
            RLOGD("%s: set osAppId Info", __FUNCTION__);
            outTdInfo.osAppIdFlag = RIL_OPT_INFO_VALID;
            RIL_OptionalOSAppId osappid;
            memset(osappid.value, 0, sizeof(RIL_OptionalOSAppId));

            size_t osAppIdLength = tdInfo.osAppId.value().osAppId.size();
            for(size_t i = 0; i < osAppIdLength; i++)
                osappid.value[i] = tdInfo.osAppId.value().osAppId[i];

            outTdInfo.osAppId.push_back(osappid);

        }else {
            RLOGD("%s: no osAppId info", __FUNCTION__);
            outTdInfo.osAppIdFlag = RIL_OPT_INFO_INVALID;
        }
    } else {
        RLOGD("%s: no opt traffic info", __FUNCTION__);
        outTdFlag = RIL_OPT_INFO_INVALID;
    }

    return true;
}

bool convertHalTDToRil(std::optional<DataV2_0::RIL_TrafficDescriptor>& outTdInfo,
                             const std::optional<aidl_data::TrafficDescriptor> &trafficDescriptor) {
    outTdInfo.reset();
    if (trafficDescriptor != std::nullopt) {
        DataV2_0::RIL_TrafficDescriptor rilTd;
        aidl_data::TrafficDescriptor tdInfo = trafficDescriptor.value();

        if (tdInfo.dnn != std::nullopt){
            RLOGD("%s: set dnn Info", __FUNCTION__);
            std::string dnn = tdInfo.dnn.value();
            rilTd.dnn.push_back(dnn);
        } else {
            RLOGD("%s: no dnn info", __FUNCTION__);
        }

        if (tdInfo.osAppId != std::nullopt) {
            RLOGD("%s: set osAppId Info", __FUNCTION__);
            DataV2_0::RIL_OSAppId osappid = { { } };
            size_t arrSize = MIN(tdInfo.osAppId.value().osAppId.size(), sizeof(osappid.value));
            std::copy(tdInfo.osAppId.value().osAppId.begin(), tdInfo.osAppId.value().osAppId.begin() + arrSize, osappid.value);
            rilTd.osAppIds.push_back(osappid);
        } else {
            RLOGD("%s: no osAppId info", __FUNCTION__);
        }
        outTdInfo = rilTd;
    } else {
        RLOGD("%s: no opt traffic info", __FUNCTION__);
    }

    return true;
}

bool fillSetupDataCallInfo_V2_0(V2_0::RIL_SetupDataCallInfo &setupDataCallInfo, aidl_radio::AccessNetwork accessNetwork,
                                const aidl_data::DataProfileInfo& dataProfileInfo, bool roamingAllowed, aidl_data::DataRequestReason reason,
                                const std::vector<aidl_data::LinkAddress>& addresses, const std::vector<std::string>& dnses, int32_t pduSessionId,
                                const std::optional<aidl_data::SliceInfo>& sliceInfo, const std::optional<aidl_data::TrafficDescriptor> trafficDescriptor, bool matchAllRuleAllowed,
                                bool AlwaysOnPdu, android::RequestInfo *pRI) {


   setupDataCallInfo.accessNetwork = (int)accessNetwork;

   if (!convertHalDataProfileToRilV2_0(setupDataCallInfo.dataProfileInfo, dataProfileInfo)) {
       RLOGD("%s: error to convert DPV2_0", __FUNCTION__);
    }

    setupDataCallInfo.roamingAllow = roamingAllowed;
    setupDataCallInfo.reason = (int)reason;

    size_t num = addresses.size();
    setupDataCallInfo.linkAddresses.resize(num);

    for (size_t i = 0; i < num; i++) {
        convertHalLinkAddressToRil(setupDataCallInfo.linkAddresses[i], addresses[i]);
    }

    setupDataCallInfo.dnses = dnses;

    setupDataCallInfo.pduSessionId = pduSessionId;

    if (sliceInfo != std::nullopt) {
        aidl_data::SliceInfo inputInfo = sliceInfo.value();
        RIL_SliceInfo rsi = { };
        rsi.sst = (RIL_SliceServiceType)inputInfo.sliceServiceType;
        rsi.sliceDifferentiator = inputInfo.sliceDifferentiator;
        rsi.mappedHplmnSD = inputInfo.mappedHplmnSd;
        rsi.mappedHplmnSst = (RIL_SliceServiceType)inputInfo.mappedHplmnSst;
        setupDataCallInfo.sliceInfo = rsi;
    } else {
        RLOGD("%s: no opt slice info", __FUNCTION__);
    }

    convertHalTDToRil(setupDataCallInfo.tdInfo, trafficDescriptor);

    setupDataCallInfo.matchAllRuleAllowed = matchAllRuleAllowed;
    setupDataCallInfo.mAlwaysOnPdu = AlwaysOnPdu;

    return true;
}

/**
 * SetupDataCallCommand
 */
void SetupDataCallCommand::onRequest( int32_t serial, aidl_radio::AccessNetwork accessNetwork, const aidl_data::DataProfileInfo &dataProfileInfo, bool roamingAllowed,
                                        aidl_data::DataRequestReason reason, const std::vector<aidl_data::LinkAddress> &addresses, const std::vector<std::string> &dnses,
                                        int32_t pduSessionId, const std::optional<aidl_data::SliceInfo> &sliceInfo, bool matchAllRuleAllowed) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    android::RequestInfo *pRI = allocateRequestInfo(serial);

    if (pRI == NULL) {
        return;
    }

    V2_0::RIL_SetupDataCallInfo setupDataCallInfo = {};

    bool ret = fillSetupDataCallInfo_V2_0(setupDataCallInfo, accessNetwork, dataProfileInfo,
                                   roamingAllowed, reason, addresses, dnses, pduSessionId, sliceInfo,
                                   dataProfileInfo.trafficDescriptor, matchAllRuleAllowed, dataProfileInfo.alwaysOn, pRI);
    if(!ret) {
        free(pRI);
        return;
    }

    RLOGD("sending size:%zu, checked size:%zu", sizeof(setupDataCallInfo), sizeof(V2_0::RIL_SetupDataCallInfo));
    mVendorFunctions.onRequest(pRI, &setupDataCallInfo, sizeof(V2_0::RIL_SetupDataCallInfo));

    return;
}

int SetupDataCallCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl_radio::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        aidl_data::SetupDataCallResult dcResult = {};

        // User v11 format, For V1_4, only two parameter are removed, and just type change for Cause
        if (response == NULL) {
            RLOGE("%s: Invalid response", __FUNCTION__);
            if (e == RIL_E_SUCCESS) responseInfo.error = aidl_radio::RadioError(RIL_E_INVALID_RESPONSE);
                dcResult.cause = aidl_data::DataCallFailCause::ERROR_UNSPECIFIED;
                dcResult.type = aidl_data::PdpProtocolType::UNKNOWN;
                dcResult.ifname = static_cast<std::string>("");
                dcResult.addresses.resize(0);
                dcResult.dnses.resize(0);
                dcResult.gateways.resize(0);
                dcResult.pcscf.resize(0);
        } else {
            RLOGE("%s: convertRilDataCallToHal", __FUNCTION__);
            dcResult = toAidl(*(RIL_SetupDataCallResult_V1_6 *)response);
        }


        RLOGV("%s: Before Callback ", __FUNCTION__);
        mCb->setupDataCallResponse(responseInfo, dcResult);
        RLOGV("%s: after Callback ", __FUNCTION__);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }
    return 0;
}

/**
 * SetDataProfileCommand
 */
void SetDataProfileCommand::onRequest(int32_t serial, const std::vector<aidl_data::DataProfileInfo> &profiles) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    android::RequestInfo *pRI = allocateRequestInfo(serial);
    if (pRI == NULL) {
        return;
    }

    size_t num = profiles.size();

    V2_0::RIL_DataProfileInfo *dataProfiles = new (std::nothrow) V2_0::RIL_DataProfileInfo[num]{};

    if (dataProfiles == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        free(pRI);
        return;
    }

    V2_0::RIL_DataProfileInfo **dataProfilePtrs = new (std::nothrow) V2_0::RIL_DataProfileInfo*[num];
    if (dataProfilePtrs == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        delete[] dataProfiles;
        sendErrorResponse(pRI, RIL_E_NO_MEMORY);
        free(pRI);
        return;
    }

    for (size_t i = 0; i < num; i++) {
        dataProfilePtrs[i] = &dataProfiles[i];
        if (!convertHalDataProfileToRilV2_0(dataProfiles[i], profiles[i]) ||
            !convertHalTDToRil(dataProfiles[i].tdInfo, profiles[i].trafficDescriptor)) {

            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            free(pRI);
            delete[] dataProfiles;
            delete[] dataProfilePtrs;
            return;
        }
    }

    mVendorFunctions.onRequest(pRI, dataProfilePtrs, sizeof(V2_0::RIL_DataProfileInfo *) * num);

    delete[] dataProfiles;
    delete[] dataProfilePtrs;

    return;

}

int SetDataProfileCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
                                          void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl_radio::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        mCb->setDataProfileResponse(responseInfo);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }

    return 0;
}

/**
 * SetInitialAttachApnCommand
 */
void SetInitialAttachApnCommand::onRequest(int32_t serial, const std::optional<aidl_data::DataProfileInfo> &dataProfileInfo) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif

    android::RequestInfo *pRI = allocateRequestInfo(serial);
    if (pRI == NULL) {
        return;
    }

    V2_0::RIL_DataProfileInfo dataProfile = {};

    if (dataProfileInfo.has_value()){
        if(!convertHalDataProfileToRilV2_0(dataProfile, dataProfileInfo.value())
        || !convertHalTDToRil(dataProfile.tdInfo, dataProfileInfo.value().trafficDescriptor)) {
            sendErrorResponse(pRI, RIL_E_NO_MEMORY);
            free(pRI);
            return;
        }
    }

    mVendorFunctions.onRequest(pRI, &dataProfile, sizeof(V2_0::RIL_DataProfileInfo));

    return;
}

int SetInitialAttachApnCommand::onResponse(int slotId,int responseType, int serial, RIL_Errno e,
            void *response, size_t responselen) {
#if VDBG
    RLOGD("%s::%s: serial %d", TAG, __FUNCTION__, serial);
#endif
    if (mCb) {
        aidl_radio::RadioResponseInfo responseInfo = populateResponseInfo(serial, responseType, e);
        mCb->setInitialAttachApnResponse(responseInfo);
    } else {
        RLOGE("%s::%s: [%d] mCb == NULL", TAG, __FUNCTION__, slotId);
    }

    return 0;
}

/**
 * UnthrottleApnInd
 */
int UnthrottleApnInd::onIndication(int slotId, int indicationType, int token,
                                   RIL_Errno e, void *response,
                                   size_t responselen) {
#if VDBG
    RLOGD("%s::%s mCb=%p", TAG, __FUNCTION__, mCb.get());
#endif
    if (mCb != NULL && response != NULL) {
        aidl_data::DataProfileInfo dataProfileInfo = toAidl(*((const V2_0::RIL_DataProfileInfo *)response));
        mCb->unthrottleApn(aidl_radio::RadioIndicationType(indicationType), dataProfileInfo);
    } else {
        RLOGE("%s::%s: [%d] mCb %s, response = %p", TAG, __FUNCTION__, slotId, (mCb == NULL) ? "is NULL" : "exist", response);
    }
    return 0;
}

}  // namespace android::hardware::radio::impl::data
