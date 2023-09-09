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
 * protocolpsbuilderjson.cpp
 *
 *  Created on: 2021. 10. 20.
 *      Author: juneho.kwon
 */

#include "protocolpsbuilderjson.h"
#include "protocolutils.h"
#include "modemdata.h"
#include "pdpcontext.h"
#include "apnsetting.h"
#include "rillog.h"
#include <librilutils/textutils.h>
#include "psjsondef.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

template <typename T>
bool ProtocolPsBuilderJson::FillApnInfo(T &jsonReqData, PdpContext *pPdpContext, bool isEsmFlagZero, bool forInitialAttach)
{
    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    if (pApnSetting == nullptr) {
        return false;
    }

    // APN
    Json::Value jsonApnInfo;
    if (!isEsmFlagZero) {
        // APN
        jsonApnInfo[ns_ps_apn_info::key_apn] = "";
        const char *apn = pApnSetting->GetApn();
        if (!TextUtils::IsEmpty(apn)) {
            jsonApnInfo[ns_ps_apn_info::key_apn] = string(apn, MIN(strlen(apn), MAX_PDP_APN_LEN - 1));
        }
        else {
            if (forInitialAttach) {
                if (!pApnSetting->CanHandleType(APN_TYPE_BIT_IA) ||
                    pApnSetting->CanHandleType(APN_TYPE_BIT_EMERGENCY) ) {
                    return false;
                }
            } else if (::isRatForCDMA(jsonReqData[ns_ps_setup_data_call_req::key_rat].asInt()) ) {
                // Allow null APN for SetupDataCall on CDMA RATs
                // RilLogV("Allow null APN for CDMA RAT:%d", jsonReqData[ns_ps_set_initial_attach_apn_req::key_rat]);
                RilLogV("Allow null APN for CDMA RAT");
            } else if ( !pApnSetting->CanHandleType(APN_TYPE_BIT_EMERGENCY) ) {
                return false;
            }
        }
         // username and password
        string userName = (pApnSetting->GetUsername() == NULL) ? "" : pApnSetting->GetUsername();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_DATA, __CLASS_FUNC__, userName, MAX_AUTH_USER_NAME_LEN-1);
        jsonApnInfo[ns_ps_apn_info::key_username] = userName;

        string password = (pApnSetting->GetPassword() == NULL) ? "" : pApnSetting->GetPassword();
        ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_DATA, __CLASS_FUNC__, password, MAX_AUTH_PASSWORD_LEN-1);
        jsonApnInfo[ns_ps_apn_info::key_password] = password;

        // auth type
        jsonApnInfo[ns_ps_apn_info::key_auth_type] = ::ConvertAuthTypeToProtocolAuthType(pApnSetting->GetAuthType());
        // P-CSCF req type
        jsonApnInfo[ns_ps_apn_info::key_pcscf_addr_req] = ::GetPcscfReqType(pApnSetting, pApnSetting->GetProtocol());
    }
    else {
        // use NULL APN
       jsonApnInfo[ns_ps_apn_info::key_apn] = "";
       jsonApnInfo[ns_ps_apn_info::key_username] = "";
       jsonApnInfo[ns_ps_apn_info::key_password] = "";
       jsonApnInfo[ns_ps_apn_info::key_auth_type] = SIT_PDP_AUTH_TYPE_NONE;
       // P-CSCF req type
       jsonApnInfo[ns_ps_apn_info::key_pcscf_addr_req] = ::GetPcscfReqTypeforNullAPN(pApnSetting, pApnSetting->GetProtocol());
    }
    // legacy data profile and APN type
    jsonApnInfo[ns_ps_apn_info::key_data_profile] = ::ConvertDataProfileIdToProtocolDataProfile(pPdpContext->GetDataProfileId());
    jsonApnInfo[ns_ps_apn_info::key_apn_type] = ::GetProtocolApnType(pPdpContext->GetDataProfileId(), pApnSetting);

    // protocol
    jsonApnInfo[ns_ps_apn_info::key_pdp_type] = ::GetPdpType(pApnSetting->GetProtocol());

    if(forInitialAttach) {
        jsonReqData[ns_ps_set_initial_attach_apn_req::key_apn_info] = jsonApnInfo;
    } else {
        jsonReqData[ns_ps_setup_data_call_req::key_apn_info] = jsonApnInfo;
    }

    return true;
}

 /**
  * BuildSetupDataCall
  */
template <typename T>
void ProtocolPsBuilderJson::FillHandoverIpAddress(T &jsonReqData, int reason, const char *addresses)
{
    // default or AP-assisted
    // DataRequestReason = 1 : NORMAL, 2 : SHUTDOWN, 3 : HANDOVER
    if (reason != REQUEST_TYPE_HANDOVER) return;
    // CP just supports only one ip address for each type
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    char tempaddrstr[INET_ADDRSTRLEN+1] = {0};
    char tempaddrstr6[INET6_ADDRSTRLEN+1] = {0};

    ::ParseHandoverAddresses(addresses, sa, sa6, tempaddrstr, tempaddrstr6);

    if (tempaddrstr[0]) {
        //req.pdp_type = SIT_PDP_PDP_TYPE_IPV4;
        BYTE IPv4Address[MAX_IPV4_ADDR_LEN];
        memcpy(IPv4Address, &sa.sin_addr, MAX_IPV4_ADDR_LEN);
        for(int i = 0 ; i < MAX_IPV4_ADDR_LEN ; i++){
           jsonReqData[ns_ps_setup_data_call_req::key_ipv4_address].append(IPv4Address[i]);
        }
    }
    if (tempaddrstr6[0]) {
        //req.pdp_type = MAX_IPV6_ADDR_LEN;
        BYTE IPv6Address[MAX_IPV6_ADDR_LEN];
        memcpy(IPv6Address, &sa6.sin6_addr, MAX_IPV6_ADDR_LEN);
        for(int i = 0 ; i < MAX_IPV6_ADDR_LEN ; i++){
           jsonReqData[ns_ps_setup_data_call_req::key_ipv6_address].append(IPv6Address[i]);
        }
    }
}

template <typename T>
int ProtocolPsBuilderJson::FillSetupDataCallRequest(T &jsonReqData, int rat, PdpContext *pPdpContext, int reason, const char *addresses) {
    // CID
    int cid = pPdpContext->GetCID();
    if (cid < 0) {
        RilLog("%s cid < 0", __FUNCTION__);
        return 0;
    }
    jsonReqData[ns_ps_setup_data_call_req::key_cid] = (BYTE)(cid & 0xFF);

    // RAT
    jsonReqData[ns_ps_setup_data_call_req::key_rat] = (BYTE)(ConvertRadioTechToSitValue(rat) & 0xFF);

    if (!FillApnInfo(jsonReqData, pPdpContext, false, false))
        return 0;

    FillHandoverIpAddress(jsonReqData, reason, addresses);

    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    jsonReqData[ns_ps_setup_data_call_req::key_apn_type_bitmask] = pApnSetting->GetSupportedTypesBitmask(); // for supporting b/155194905

    return true;
}

int ProtocolPsBuilderJson::checkOptSliceInfoType(int optionalSliceInfoSst, int optionalSliceInfoSD, int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD)
{
    int type = 0;

    if(optionalSliceInfoSst != 0)
        type =1;

    if(optionalSliceInfoSst != 0 && optionalSliceInfoMappedSst != 0)
        type =2;

    if(optionalSliceInfoSst != 0 && optionalSliceInfoSD != -1)
        type =4;

    if(optionalSliceInfoSst != 0 && optionalSliceInfoSD != -1 && optionalSliceInfoMappedSst != 0)
        type =5;

    if(optionalSliceInfoSst != 0 && optionalSliceInfoSD != -1 && optionalSliceInfoMappedSst != 0 && optionalSliceInfoMappedSD != -1 )
        type =8;

    return type;
}

ModemData *ProtocolPsBuilderJson::BuildSetupDataCall(int rat, PdpContext *pPdpContext, int /*slotId*/, int reason, const char *addresses)
{
    if (!isValidPdpApn(pPdpContext)) return NULL;
    Json::Value jsonReqData;

    int length = FillSetupDataCallRequest(jsonReqData, rat, pPdpContext, reason, addresses);
    if (length == 0) return NULL;
    jsonReqData[ns_ps_setup_data_call_req::key_req_ipc_version] = REQ_IPC_VERSION_1;
    return GetJsonReqModemData(jsonReqData, SIT_SETUP_DATA_CALL);
}

ModemData *ProtocolPsBuilderJson::BuildSetupDataCall(int rat, PdpContext *pPdpContext, int /*slotId*/, int reason, const char *addresses,
                                                        int pduSessionId, int /* optionalSliceInfoFlag */, int optionalSliceInfoSst, int optionalSliceInfoSD, int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD,
                                                        const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag, bool /* alwaysOnPdu */) {
    RilLog("%s_extendedVersion", __FUNCTION__);
    if (!isValidPdpApn(pPdpContext)) return NULL;
    Json::Value jsonReqData;

    int length = FillSetupDataCallRequest(jsonReqData, rat, pPdpContext, reason, addresses);
    if (length == 0) return NULL;

    uint32_t tdBitMask = 0;

    jsonReqData[ns_ps_setup_data_call_req::key_req_ipc_version] = REQ_IPC_VERSION_2;//2:PduSessionId, SliceInfo

    jsonReqData[ns_ps_setup_data_call_req::key_pdu_session_id] = pduSessionId;
    jsonReqData[ns_ps_setup_data_call_req::key_snssai_type] = checkOptSliceInfoType(optionalSliceInfoSst, optionalSliceInfoSD, optionalSliceInfoMappedSst, optionalSliceInfoMappedSD);
    jsonReqData[ns_ps_setup_data_call_req::key_slice_service_type] = optionalSliceInfoSst;
    jsonReqData[ns_ps_setup_data_call_req::key_slice_differentiator] = optionalSliceInfoSD;

    jsonReqData[ns_ps_setup_data_call_req::key_mapped_hplmn_sst] = optionalSliceInfoMappedSst;
    jsonReqData[ns_ps_setup_data_call_req::key_mapped_hplmn_sd] = optionalSliceInfoMappedSD;

    jsonReqData[ns_ps_setup_data_call_req::key_match_all_rule_allowed] = (BYTE) matchedAllFlag;
    RilLog("matchAllRuleAllowed %d", matchedAllFlag);

    Json::Value jsonTdInfo;
    if (tdInfo.has_value()) {
        if (!tdInfo->dnn.empty()){
            tdBitMask |= TD_FLAG_DNN;

            jsonTdInfo[ns_ps_data_call_traffic_descriptor::key_dnn] = tdInfo->dnn[0];
            RilLog("TdInfo-Dnn:%s", (char*) tdInfo->dnn[0].c_str());
        }

        // OsAppId + OsId
        if (!tdInfo->osAppIds.empty()){
            // OsId 16 bytes
            char osid[MAX_OSID_SIZE + 1] = {};
            for(int i = 0; i < MAX_OSID_SIZE; i++) {
                osid[i] = tdInfo->osAppIds[0].value[i];
                tdBitMask |= TD_FLAG_OSID;
            }
            jsonTdInfo[ns_ps_data_call_traffic_descriptor::key_os_id] = osid;

            // OsAppId max 255 bytes
            int osAppIdSiz = (BYTE) tdInfo->osAppIds[0].value[MAX_OSID_SIZE];
            if (osAppIdSiz != 0) tdBitMask |= TD_FLAG_OSAPPID;
            RilLog("TdInfo-osAppIdSiz:%d", osAppIdSiz);

            char osappid[MAX_OSAPPID_SIZE+1] = {};
            for(int i = 0; i < MAX_OSAPPID_SIZE ; i++){
                osappid[i] = tdInfo->osAppIds[0].value[i + MAX_OSID_SIZE + 1];
            }
            jsonTdInfo[ns_ps_data_call_traffic_descriptor::key_os_app_id] = osappid;
        }
    }

    jsonTdInfo[ns_ps_data_call_traffic_descriptor::key_td_bit_mask] = tdBitMask;

    jsonReqData[ns_ps_setup_data_call_req::key_traffic_descriptor] = jsonTdInfo;
    return GetJsonReqModemData(jsonReqData, SIT_SETUP_DATA_CALL);
}

/**
 * BuildDeactDataCall
 */
ModemData *ProtocolPsBuilderJson::BuildDeactDataCall(int cid, int reason)
{
    if (cid < 0) {
        return NULL;
    }

    Json::Value jsonReqData;

    jsonReqData[ns_ps_deact_data_call_req::key_cid] = cid;
    jsonReqData[ns_ps_deact_data_call_req::key_reason] = reason;
    return GetJsonReqModemData(jsonReqData, SIT_DEACT_DATA_CALL);
}

 /**
  * BuildGetDataCallList
  */
 ModemData *ProtocolPsBuilderJson::BuildGetDataCallList()
 {
     return GetJsonNullReqModemData(SIT_GET_DATA_CALL_LIST);
 }

/**
 * BuildSetInitialAttachApn
 */
ModemData *ProtocolPsBuilderJson::BuildSetInitialAttachApn(PdpContext *pPdpContext, bool isEsmFlagZero/* = false*/)
{
    if (!::isValidPdpApn(pPdpContext)) return NULL;

   Json::Value jsonReqData;

    // CID
    int cid = pPdpContext->GetCID();
    if (cid < 0) {
        return NULL;
    }
    jsonReqData[ns_ps_set_initial_attach_apn_req::key_cid] = cid & 0xFF;
    jsonReqData[ns_ps_set_initial_attach_apn_req::key_rat] = SIT_RAT_TYPE_LTE;

    if (!FillApnInfo(jsonReqData, pPdpContext, isEsmFlagZero, true))
        return NULL;

    Json::Value jsonApnInfo;
    jsonApnInfo = jsonReqData[ns_ps_set_initial_attach_apn_req::key_apn_info];
    if(TextUtils::IsEmpty(jsonApnInfo[ns_ps_apn_info::key_apn].asString()))
        pPdpContext->SetDataProfileId(DATA_PROFILE_NULL_APN_IA);

    // ignore return
    FillDataProfileId(jsonReqData, pPdpContext, true);

    // For TMO TC : L_LTE_ROAM_54650_INT Roam_302370_Data Roaming ON
    // roaming protocol
    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    jsonReqData[ns_ps_set_initial_attach_apn_req::key_roaming_pdp_type] = ::GetPdpType(pApnSetting->GetRoamingProtocol());

    if(isEsmFlagZero ||TextUtils::IsEmpty(pApnSetting->GetApn()) ){
        jsonReqData[ns_ps_set_initial_attach_apn_req::key_roaming_pcscf_req_type] =  ::GetPcscfReqTypeforNullAPN(pApnSetting, pApnSetting->GetRoamingProtocol());
    }
    else{
        // Roaming P-CSCF req type
        jsonReqData[ns_ps_set_initial_attach_apn_req::key_roaming_pcscf_req_type] = ::GetPcscfReqType(pApnSetting, pApnSetting->GetRoamingProtocol());
        RilLogV("roaming_pcscf_req_type: %d", ::GetPcscfReqType(pApnSetting, pApnSetting->GetRoamingProtocol()));
    }

    jsonReqData[ns_ps_set_initial_attach_apn_req::key_ipv4_address] = Json::arrayValue;
    jsonReqData[ns_ps_set_initial_attach_apn_req::key_ipv6_address] = Json::arrayValue;

    return GetJsonReqModemData(jsonReqData, SIT_SET_INITIAL_ATTACH_APN);
}

template <typename T>
bool ProtocolPsBuilderJson::FillDataProfileId(T &jsonReqData, PdpContext *pPdpContext, bool forInitialAttach)
{
    // DataProfileId
    if (forInitialAttach) {
        jsonReqData[ns_ps_set_initial_attach_apn_req::key_profile_id] = SIT_IA_PROFILE_ID_UNKNOWN;
    } else {
        jsonReqData[ns_ps_set_initial_attach_apn_req::key_profile_id] = pPdpContext->GetDataProfileId();
        if(jsonReqData[ns_ps_set_initial_attach_apn_req::key_profile_id].asInt() == DATA_PROFILE_DEFAULT) {
            jsonReqData[ns_ps_set_initial_attach_apn_req::key_profile_id] = SIT_IA_PROFILE_ID_INTERNET;
        } else {
            RilLogV("ignore non-Internet ProfileId: %d", jsonReqData[ns_ps_set_initial_attach_apn_req::key_profile_id].asUInt());
            return false;
        }
    }
    return true;
}

/**
 * BuildSetDataProfile
 */
ModemData *ProtocolPsBuilderJson::BuildSetDataProfile(const RIL_DataProfileInfo_v15 *  dpi, bool /* isVzw */) {
    if (dpi == NULL) {
        return NULL;
    }
    Json::Value jsonReqData;

    prepareSetDataProfileRequest(jsonReqData, dpi);

    // protocol
    jsonReqData[ns_ps_set_data_profile_req::key_pdp_type] = ::GetPdpType(dpi->protocol);
    // roaming protocol
    jsonReqData[ns_ps_set_data_profile_req::key_roaming_pdp_type] = ::GetPdpType(dpi->roamingProtocol);
    // auth type
    jsonReqData[ns_ps_set_data_profile_req::key_auth_type] = ::ConvertAuthTypeToProtocolAuthType(dpi->authType);

    jsonReqData[ns_ps_set_data_profile_req::key_radio_access_family] = dpi->bearerBitmask;
    jsonReqData[ns_ps_set_data_profile_req::key_apn_type] = dpi->supportedTypesBitmask;

    jsonReqData[ns_ps_set_data_profile_req::key_data_profile_info_type] = 0;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DATA_PROFILE);
}

/**
 * BuildSetDataProfile
 */
ModemData *ProtocolPsBuilderJson::BuildSetDataProfile(const RIL_DataProfileInfo_V1_4 * dpi, bool /* isVzw */) {
    if (dpi == NULL) {
        return NULL;
    }
    Json::Value jsonReqData;

    prepareSetDataProfileRequest(jsonReqData, dpi);

    // protocol
    jsonReqData[ns_ps_set_data_profile_req::key_pdp_type] = ::ConvertPdpProtocolTypeToSitPdpType(dpi->protocol);
    // roaming protocol
    jsonReqData[ns_ps_set_data_profile_req::key_roaming_pdp_type] = ::ConvertPdpProtocolTypeToSitPdpType(dpi->roamingProtocol);
    // auth type
    jsonReqData[ns_ps_set_data_profile_req::key_auth_type] = ::ConvertAuthTypeToProtocolAuthType(dpi->authType);

    jsonReqData[ns_ps_set_data_profile_req::key_radio_access_family] = dpi->bearerBitmap;
    jsonReqData[ns_ps_set_data_profile_req::key_apn_type] = dpi->supportedApnTypesBitmap;

    jsonReqData[ns_ps_set_data_profile_req::key_data_profile_info_type] = 0;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DATA_PROFILE);
}

/**
 * BuildSetDataProfile
 */
ModemData *ProtocolPsBuilderJson::BuildSetDataProfile(const RIL_DataProfileInfo_V1_5 *dpi) {
    if (dpi == NULL) {
        return NULL;
    }

    Json::Value jsonReqData;

    prepareSetDataProfileRequest(jsonReqData, dpi);

    // protocol
    jsonReqData[ns_ps_set_data_profile_req::key_pdp_type] = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->protocol);
    // roaming protocol
    jsonReqData[ns_ps_set_data_profile_req::key_roaming_pdp_type] = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->roamingProtocol);
    // auth type
    jsonReqData[ns_ps_set_data_profile_req::key_auth_type] = ::ConvertAuthTypeToProtocolAuthType((int)dpi->authType);

    jsonReqData[ns_ps_set_data_profile_req::key_radio_access_family] = dpi->bearerBitmap;
    jsonReqData[ns_ps_set_data_profile_req::key_apn_type] = dpi->supportedApnTypesBitmap;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DATA_PROFILE);
}

/**
 * BuildSetDataProfile
 */
ModemData *ProtocolPsBuilderJson::BuildSetDataProfile(const DataV2_0::RIL_DataProfileInfo *dpi) {
    if (dpi == NULL) {
        return NULL;
    }

    Json::Value jsonReqData;

    prepareSetDataProfileRequest(jsonReqData, dpi);

    // protocol
    jsonReqData[ns_ps_set_data_profile_req::key_pdp_type] = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->protocol);
    // roaming protocol
    jsonReqData[ns_ps_set_data_profile_req::key_roaming_pdp_type] = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->roamingProtocol);
    // auth type
    jsonReqData[ns_ps_set_data_profile_req::key_auth_type] = ::ConvertAuthTypeToProtocolAuthType((int)dpi->authType);

    jsonReqData[ns_ps_set_data_profile_req::key_radio_access_family] = dpi->bearerBitmap;
    jsonReqData[ns_ps_set_data_profile_req::key_apn_type] = dpi->supportedApnTypesBitmap;

    return GetJsonReqModemData(jsonReqData, SIT_SET_DATA_PROFILE);
}

void ProtocolPsBuilderJson::prepareSetDataProfileRequest(Json::Value &jsonData, const DataV2_0::RIL_DataProfileInfo *dpi)
{
    // profile id, vzw checks
    // Don't care operator. just pass profile_id directly
    jsonData[ns_ps_set_data_profile_req::key_profile_id] = dpi->profileId;
    // 0xFF will be required for DualVoLTE, but higher layer will set this
    //if(!isVzw // APN==DEFAULT) req.profile_id = SIT_IA_PROFILE_ID_INTERNET;
    RilLogV("profile_id: %d", jsonData[ns_ps_set_data_profile_req::key_profile_id].asUInt());

    // APN
    if (!dpi->apn.empty()) {
        jsonData[ns_ps_set_data_profile_req::key_apn] = dpi->apn;
    } else {
        RilLogV("Null APN is not allowed in DataProfile");
        return;
    }

    // username and password
    string userName = (dpi->user.empty()) ? "" : dpi->user;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_DATA, __CLASS_FUNC__, userName, MAX_AUTH_USER_NAME_LEN-1);
    jsonData[ns_ps_set_data_profile_req::key_username] = userName;

    string password = (dpi->password.empty()) ? "" : dpi->password;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_DATA, __CLASS_FUNC__, password, MAX_AUTH_PASSWORD_LEN-1);
    jsonData[ns_ps_set_data_profile_req::key_password] = password;

    jsonData[ns_ps_set_data_profile_req::key_data_profile_info_type] = dpi->type;

    // apn disable flag
    jsonData[ns_ps_set_data_profile_req::key_enabled] = dpi->enabled;

    // ? Vzw only parameters ?
    jsonData[ns_ps_set_data_profile_req::key_max_conn_time] = dpi->maxConnsTime;
    jsonData[ns_ps_set_data_profile_req::key_max_conn] = dpi->maxConns;
    jsonData[ns_ps_set_data_profile_req::key_wait_time] = dpi->waitTime;
    return;
}

template<typename T>
void ProtocolPsBuilderJson::prepareSetDataProfileRequest(Json::Value &jsonData, T *(&dpi))
{
    // profile id, vzw checks
    // Don't care operator. just pass profile_id directly
    jsonData[ns_ps_set_data_profile_req::key_profile_id] = dpi->profileId;
    // 0xFF will be required for DualVoLTE, but higher layer will set this
    //if(!isVzw // APN==DEFAULT) req.profile_id = SIT_IA_PROFILE_ID_INTERNET;
    RilLogV("profile_id: %d", jsonData[ns_ps_set_data_profile_req::key_profile_id].asUInt());

    // APN
    if (!TextUtils::IsEmpty(dpi->apn)) {
        string apnString(dpi->apn, MIN(strlen(dpi->apn), MAX_PDP_APN_LEN - 1));
        jsonData[ns_ps_set_data_profile_req::key_apn] = apnString;
    }
    else {
        RilLogV("Null APN is not allowed in DataProfile");
        return;
    }

    // username and password
    string userName = (dpi->user == NULL) ? "" : dpi->user;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_DATA, __CLASS_FUNC__, userName, MAX_AUTH_USER_NAME_LEN-1);
    jsonData[ns_ps_set_data_profile_req::key_username] = userName;

    string password = (dpi->password == NULL) ? "" : dpi->password;
    ProtocolUtils::LimitStringToMaxLength(RIL_LOG_CAT_DATA, __CLASS_FUNC__, password, MAX_AUTH_PASSWORD_LEN-1);
    jsonData[ns_ps_set_data_profile_req::key_password] = password;

    jsonData[ns_ps_set_data_profile_req::key_data_profile_info_type] = dpi->type;

    // apn disable flag
    jsonData[ns_ps_set_data_profile_req::key_enabled] = dpi->enabled;

    // ? Vzw only parameters ?
    jsonData[ns_ps_set_data_profile_req::key_max_conn_time] = dpi->maxConnsTime;
    jsonData[ns_ps_set_data_profile_req::key_max_conn] = dpi->maxConns;
    jsonData[ns_ps_set_data_profile_req::key_wait_time] = dpi->waitTime;
    return;
}

/**
 * BuildSetFastDormancyInfo
 */
ModemData *ProtocolPsBuilderJson::BuildSetFastDormancyInfo(BYTE lcdOn, BYTE lcdOff, BYTE rel8LcdOnl, BYTE rel8LcdOff) {
    Json::Value jsonReqData;

    jsonReqData[ns_ps_set_fd_info_req::key_lcd_on_timer] = lcdOn;
    jsonReqData[ns_ps_set_fd_info_req::key_lcd_off_timer] = lcdOff;
    jsonReqData[ns_ps_set_fd_info_req::key_rel8_lcd_on_timer] = rel8LcdOnl;
    jsonReqData[ns_ps_set_fd_info_req::key_rel8_lcd_off_timer] = rel8LcdOff;

    return GetJsonReqModemData(jsonReqData, SIT_SET_FD_INFO);
}

/**
 * BuildDetach
 */
ModemData *ProtocolPsBuilderJson::BuildDetach(int detachReason) {
    Json::Value jsonReqData;

    jsonReqData[ns_ps_detach_req::key_detach_reason] = detachReason;
    return GetJsonReqModemData(jsonReqData, SIT_DETACH);
}

/**
 * BuildStartKeepAlive
 */
ModemData *ProtocolPsBuilderJson::BuildStartKeepAlive(RIL_KeepaliveRequest reqData) {
    Json::Value jsonReqData;

    jsonReqData[ns_ps_start_keepalive_req::key_keepalive_type] = reqData.type;
    for(int i = 0 ; i < MAX_IPV6_ADDR_LEN ; i++){
       jsonReqData[ns_ps_start_keepalive_req::key_source_addr].append((int) reqData.sourceAddress[i]);
       jsonReqData[ns_ps_start_keepalive_req::key_dest_addr].append((int) reqData.destinationAddress[i]);
    }
    jsonReqData[ns_ps_start_keepalive_req::key_source_port] = reqData.sourcePort;
    jsonReqData[ns_ps_start_keepalive_req::key_dest_port] = reqData.destinationPort;
    jsonReqData[ns_ps_start_keepalive_req::key_max_interval] = reqData.maxKeepaliveIntervalMillis;
    jsonReqData[ns_ps_start_keepalive_req::key_cid] = reqData.cid;

    return GetJsonReqModemData(jsonReqData, SIT_START_KEEPALIVE);
}

/**
 * BuildStopKeepAlive
 */
ModemData *ProtocolPsBuilderJson::BuildStopKeepAlive(int sessionHande) {
    Json::Value jsonReqData;

    jsonReqData[ns_ps_stop_keepalive_req::key_keepalive_handle] = sessionHande;

    return GetJsonReqModemData(jsonReqData, SIT_STOP_KEEPALIVE);
}

/**
 * BuildSetPreferredDataModem
 */
ModemData *ProtocolPsBuilderJson::BuildSetPreferredDataModem(int stackId) {
    Json::Value jsonReqData;

    jsonReqData[ns_ps_net_set_preferred_data_modem_req::key_stack_id] = stackId;

    return GetJsonReqModemData(jsonReqData, SIT_SET_PREFERRED_DATA_MODEM);
}

/**
 * BuildSetPdnIpAddress
 */
ModemData *ProtocolPsBuilderJson::BuildSetPdnIpAddress(RIL_SetPdnIpAddressRequest /* reqData */) {
    //For IOT, not currently in use.
    return GetJsonNullReqModemData(SIT_SET_PDN_IP_ADDRESS);
}

/**
 * BuildAllocatePduSessionId
 */
ModemData *ProtocolPsBuilderJson::BuildAllocatePduSessionId() {
    return  GetJsonNullReqModemData(SIT_ALLOC_PDU_SESSION);
}

/**
 * BuildReleasePduSessionId
 */
ModemData *ProtocolPsBuilderJson::BuildReleasePduSessionId(int sessionId) {
    Json::Value jsonReqData;
    jsonReqData[ns_ps_rel_pdu_session_req::key_session_id] = sessionId;

    return GetJsonReqModemData(jsonReqData, SIT_REL_PDU_SESSION);
}

/**
 * BuildStartHandover
 */
ModemData *ProtocolPsBuilderJson::BuildStartHandover(int /* callId */) {
    Json::Value jsonReqData;

    return GetJsonReqModemData(jsonReqData, SIT_START_HANDOVER);
}

/**
 * BuildCancelHandover
 */
ModemData *ProtocolPsBuilderJson::BuildCancelHandover(int /* callId */) {
    Json::Value jsonReqData;

    return GetJsonReqModemData(jsonReqData, SIT_CANCEL_HANDOVER);
}

/**
 * BuildSlicingConfig
 */
ModemData *ProtocolPsBuilderJson::BuildSlicingConfig(int version) {
    Json::Value jsonReqData;

    jsonReqData[ns_ps_get_slicing_config_req::key_ipc_version] = version;

    return GetJsonReqModemData(jsonReqData, SIT_GET_SLICING_CONFIG);
}
