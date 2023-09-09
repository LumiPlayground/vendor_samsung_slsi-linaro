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
 * protocolpsbuilderlegacy.cpp
 *
 *  Created on: 2014. 6. 27.
 *      Author: sungwoo48.choi
 */
#include "protocolpsbuilderlegacy.h"
#include <librilutils/textutils.h>
#include <librilutils/systemproperty.h>
#include "apnsetting.h"
#include "modemdata.h"
#include "pdpcontext.h"
#include "rillog.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PROPERTY_IWLAN_OPERATION_MODE    "ro.telephony.iwlan_operation_mode"
#define PROPERTY_NWSLICE_EXT_MODE        "vendor.radio.nwslice_ext"
#define IWLAN_OPERATION_MODE_DEFAULT     "default"
#define IWLAN_OPERATION_MODE_LEGACY      "legacy"
#define IWLAN_OPERATION_MODE_AP_ASSISTED "AP-assisted"

template <typename T>
int ProtocolPsBuilderLegacy::FillSetupDataCallRequest(T &req, int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses) {
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SETUP_DATA_CALL, length);

    // CID
    int cid = pPdpContext->GetCID();
    if (cid < 0) {
        RilLog("%s cid < 0", __FUNCTION__);
        return 0;
    }
    req.cid = (BYTE)(cid & 0xFF);

    // RAT
    req.rat = (BYTE)(ConvertRadioTechToSitValue(rat) & 0xFF);

    if ( !FillApnInfo(req, pPdpContext, false, false) ) {
        RilLogV("%s, return 0 by FillApnInfo fail", __FUNCTION__);
        return 0;
    }

    FillHandoverIpAddress(req, slotId, reason, addresses);

    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    req.lowPriority = 0; // Set as default value
    req.apnTypeBitMask = pApnSetting->GetSupportedTypesBitmask(); // for supporting b/155194905

    return length;
}

// Cover under HAL Radio Interface 1.5
ModemData *ProtocolPsBuilderLegacy::BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses)
{
    RilLog("%s", __FUNCTION__);
    if (!isValidPdpApn(pPdpContext)) return NULL;

    sit_pdp_setup_data_call_req req;
    int length = FillSetupDataCallRequest(req, rat, pPdpContext, slotId, reason, addresses);
    if (length == 0) return NULL;
    req.reqIpcVersion = REQ_IPC_VERSION_0;

    return new ModemData((char *)&req, length);
}

int ProtocolPsBuilderLegacy::checkOptSliceInfoType(int optionalSliceInfoSst, int optionalSliceInfoSD, int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD)
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

int ProtocolPsBuilderLegacy::FillSliceInfo(sit_pdp_setup_data_call_v2_req &req, int pduSessionId, int /* optionalSliceInfoFlag */,
                                           int optionalSliceInfoSst, int optionalSliceInfoSD, int optionalSliceInfoMappedSst,
                                           int optionalSliceInfoMappedSD, const DataV2_0::RIL_TrafficDescriptor &tdInfo,
                                           bool matchedAllFlag) {
    uint32_t tdBitMask = 0;

    req.sliceInfoData.pduSessionId = pduSessionId;
    req.sliceInfoData.optionalSliceInfoType = checkOptSliceInfoType(optionalSliceInfoSst, optionalSliceInfoSD, optionalSliceInfoMappedSst, optionalSliceInfoMappedSD);
    req.sliceInfoData.optionalSliceInfoSst = optionalSliceInfoSst;
    req.sliceInfoData.optionalSliceInfoSD = optionalSliceInfoSD;

    req.sliceInfoData.optionalSliceInfoMappedHplmnSst = optionalSliceInfoMappedSst;
    req.sliceInfoData.optionalSliceInfoMappedHplmnSD= optionalSliceInfoMappedSD;

    req.matchAllRuleAllowed = (BYTE) matchedAllFlag;
    RilLog("matchAllRuleAllowed %d", matchedAllFlag);

    if (!tdInfo.dnn.empty()) {
        tdBitMask |= TD_FLAG_DNN;
        size_t len = tdInfo.dnn[0].size() > MAX_PDP_APN_LEN ? MAX_PDP_APN_LEN : tdInfo.dnn[0].size();
        memcpy(req.tdData.optionalDnn, tdInfo.dnn[0].c_str(), len);
        // Log can be buggy, when all 255 byte has character, but possibility is very low,
        // and effect is just spamming log
        RilLog("TdInfo-Dnn:%s", (char*) req.tdData.optionalDnn);
    }

    const int nilUUID[MAX_OSID_SIZE] = {0};
    // OsAppId + OsId, need to copy at cross position
    if (!tdInfo.osAppIds.empty()) {
        memset(req.tdData.optionalOsAppID, 0, MAX_OSAPPID_SIZE);
        // OsAppId max 255 bytes
        int osAppIdSiz = (BYTE) tdInfo.osAppIds[0].value[MAX_OSID_SIZE];
        if (osAppIdSiz != 0) tdBitMask |= TD_FLAG_OSAPPID;
        RilLog("TdInfo-osAppIdSiz:%d", osAppIdSiz);
        memcpy(req.tdData.optionalOsAppID, &(tdInfo.osAppIds[0].value[MAX_OSID_SIZE+1]), osAppIdSiz);

        // OsId 16 bytes
        tdBitMask |= TD_FLAG_OSID;
        if (memcmp(&tdInfo.osAppIds[0].value[0], nilUUID, MAX_OSID_SIZE) != 0) {
            tdBitMask |= TD_FLAG_OSID;
            memcpy(&(req.tdData.optionalOsID[0]), &tdInfo.osAppIds[0].value[0], MAX_OSID_SIZE);
        }
    }

    req.tdData.infoBitmask = tdBitMask;
    return sizeof(req);
}


static bool isValidAddress(bool isIpv6, const char *addr) {
     const char nullIP[16] = {};
     const char loopback4[4] = {127, 0, 0, 1};
     const char loopback6[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
     // :: address is actaully valid, but normally not used because it means default route
     if (isIpv6 && memcmp(nullIP, addr, 16)) return false;
     if (isIpv6 && memcmp(loopback6, addr, 16)) return false;
     if (!isIpv6 && memcmp(nullIP, addr, 4)) return false;
     if (!isIpv6 && memcmp(loopback4, addr, 4)) return false;
     return true;
}

bool ProtocolPsBuilderLegacy::FillSliceInfoExt(sit_pdp_setup_data_call_v3_req &req, const DataV2_0::RIL_TrafficDescriptor &tdInfo) {
    bool updated = false;
    uint32_t tdBitMask = req.tdData.infoBitmask;

    // Fill extended information
    if (tdInfo.ipDescriptors.has_value()) {
        tdBitMask |= (tdInfo.tdMask & TD_BITMASK_IP_DESCRIPTORS);  // Bit 4-8 is for IpDescriptors
        /*
        if (!tdInfo.ipDescriptors.ipv4.empty()) {
        tdBitMask |= TD_FLAG_IPV4_REMOTE_ADDRESS;
        }
        if (!tdInfo.ipDescriptors.ipv6.empty() && isValidAddress(true, tdInfo.ipDescriptors.ipv6[0].c_str())) {
        tdBitMask |= TD_FLAG_IPV6_REMOTE_ADDRESS;
        }
        // port 0 is actually valid. but it's normally not used in network server for security reason
        if (!tdInfo.ipDescriptors.port.empty() && tdInfo.ipDescriptors.port[0]) {
        tdBitMask |= TD_FLAG_SINGLE_REMOTE_PORT;
        }
        if (!tdInfo.ipDescriptors.portStartRange.empty() && tdInfo.ipDescriptors.portStartRange[0]) {
        tdBitMask |= TD_FLAG_REMOTE_PORT_RANGE_LIMIT;
        }
        if (!tdInfo.ipDescriptors.protocolId.empty()) {
        tdBitMask |= TD_FLAG_PROTOCOL_IDENTIFIER;
        }
        */
        FillIpDescriptor(req.tdExtData, tdInfo.ipDescriptors.value());

        updated = true;
    }
    if (!tdInfo.ip3Tuple.empty()) {
        // When Ip3Tuple is used, each bitmasks related with IP are not expected,
        // so just set one bit flag
        tdBitMask |= TD_FLAG_IP3TUPLE;
        RilLogV("Set Ip3Tuple");
        FillIp3Tuple(req.tdExtData.ip3Tuple, tdInfo);
        updated = true;
    }
    if (!tdInfo.domainDescriptors.empty()) {
        if (!tdInfo.domainDescriptors[0].value.empty()) {
            tdBitMask |= TD_FLAG_DESTINATION_FQDN;
            memcpy(req.tdExtData.destinationFQDN, tdInfo.domainDescriptors[0].value.c_str(),
                   (tdInfo.domainDescriptors[0].value.length() > 255) ? 255 :
                   tdInfo.domainDescriptors[0].value.length());
            RilLogV("Set FQDN:%s", tdInfo.domainDescriptors[0].value.c_str());
        }
        updated = true;
    }
    // This one byte bitmask in Spec, need to parse and make bitmask
    // Higher layer format is "IMS, MMS, SUPL, Internet" ...
    if (!tdInfo.connectionCapabilities.empty()) {
        if (!tdInfo.connectionCapabilities[0].value.empty()) {
            tdBitMask |= TD_FLAG_CONNECTION_CAPABILITIES;
            string s = tdInfo.connectionCapabilities[0].value.c_str();
            req.tdExtData.connectionCapabilities = ConvertConnectionCapabilities(s);
            RilLogV("Set CC:%s", tdInfo.connectionCapabilities[0].value.c_str());
        }
        updated = true;
    }
    if (tdInfo.matchAllType == true) {
        tdBitMask |= TD_FLAG_MATCHED_ALL;
        RilLogV("matchAllType in TD is true, set bit with others");
    }
    if (updated) {
        req.tdData.infoBitmask = tdBitMask;
        RilLogV("tdBitMask:%x", req.tdData.infoBitmask);
    }

    return updated;
}

template<typename T>
void ProtocolPsBuilderLegacy::FillIpDescriptor(T &ipdesc, const RIL_IpDescriptors& RilIpDesc) {
    if (!RilIpDesc.ipv4.empty()) {
        FillIpAddr(false, RilIpDesc.ipv4[0].c_str(), (char *)ipdesc.ipv4RemoteAddress);
        RilLogV("Set Ipv4: %s", RilIpDesc.ipv4[0].c_str());

        if (RilIpDesc.maskV4.empty()) {
            // Fill 255.255.255.255
            for (int i = 0; i < 4; i++)
                ipdesc.ipv4RemoteAddressMask[i] = 0xff;
        } else {
            FillIpAddr(false, RilIpDesc.maskV4[0].c_str(), (char *)ipdesc.ipv4RemoteAddressMask);
        }
    }
    if (!RilIpDesc.ipv6.empty()) {
        FillIpAddr(true, RilIpDesc.ipv6[0].c_str(), (char *)ipdesc.ipv6RemoteAddress);
        RilLogV("Set Ipv6:%s/%d", RilIpDesc.ipv6[0].c_str(), RilIpDesc.prefixLength[0]);

        if (RilIpDesc.prefixLength[0] <= 0 ||
            RilIpDesc.prefixLength[0] > 128) {
            ipdesc.prefixLength = 128;
        } else {
            ipdesc.prefixLength = RilIpDesc.prefixLength[0];
        }
    }
    if (!RilIpDesc.port.empty()) {
        *((uint16_t *)ipdesc.singleRemotePort) = RilIpDesc.port[0];
        RilLogV("Set SingleRemotePort:%d", RilIpDesc.port[0]);
    }
    if (!RilIpDesc.portStartRange.empty()) {
        *((uint16_t *)ipdesc.remotePortRangeLowLimit) = RilIpDesc.portStartRange[0];
        *((uint16_t *)ipdesc.remotePortRangeHighLimit) = RilIpDesc.portEndRange[0];
        RilLogV("Set RemotePortRange:%d~%d", RilIpDesc.portStartRange[0], RilIpDesc.portEndRange[0]);
    }
    if (!RilIpDesc.protocolId.empty()) {
        ipdesc.protocolIdentifier = RilIpDesc.protocolId[0];
        RilLogV("Set ProtocolId:%d", RilIpDesc.protocolId[0]);
    }
}

void ProtocolPsBuilderLegacy::FillIp3Tuple(sit_pdp_ip3tuple &ip3Tuple, const DataV2_0::RIL_TrafficDescriptor &tdInfo) {
    ip3Tuple.bitmap = 0;

    if (tdInfo.ip3Tuple.empty()) {
        RilLogV("ip3TupleFlag is true, but empty information");
        return;
    }

    if (!tdInfo.ip3Tuple[0].ipv4.empty()) {
        ip3Tuple.bitmap |= IP3TUPLE_BITMAP_IPV4;
    }
    if (!tdInfo.ip3Tuple[0].ipv6.empty() && isValidAddress(true, tdInfo.ip3Tuple[0].ipv6[0].c_str())) {
        ip3Tuple.bitmap |= IP3TUPLE_BITMAP_IPV6;
    }
    if (!tdInfo.ip3Tuple[0].port.empty() && tdInfo.ip3Tuple[0].port[0]) {
        ip3Tuple.bitmap |= IP3TUPLE_BITMAP_SINGLEPORT;
    }
    if (!tdInfo.ip3Tuple[0].portStartRange.empty()) {
        ip3Tuple.bitmap |= IP3TUPLE_BITMAP_PORTRANGE;
    }
    if (!tdInfo.ip3Tuple[0].protocolId.empty()) {
        ip3Tuple.bitmap |= IP3TUPLE_BITMAP_PROTOCOLID;
    }
    FillIpDescriptor(ip3Tuple, tdInfo.ip3Tuple[0]);
}

template<typename T>
int ProtocolPsBuilderLegacy::FillSetupDataCallIpcV3(T &req, int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses,
                                              int pduSessionId, int optionalSliceInfoFlag,
                                              int optionalSliceInfoSst, int optionalSliceInfoSD,
                                              int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD,
                                              const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag) {
    int length = FillSetupDataCallRequest(req, rat, pPdpContext, slotId, reason, addresses);
    if (length == 0) {
        RilLogV("exit due to length 0");
        return 0;
    }
    // If null apn and empty TD, it's not expected in Modem side, so return fail.
    if (TextUtils::IsEmpty(req.apn) && (!tdInfo.has_value() || tdInfo->empty())) {
        RilLogV("null APN without TD for SetupDataCall is not allowed");
        return 0;
    }

    length = FillSliceInfo(req, pduSessionId, optionalSliceInfoFlag, optionalSliceInfoSst,
                           optionalSliceInfoSD, optionalSliceInfoMappedSst, optionalSliceInfoMappedSD,
                           tdInfo.has_value() ? tdInfo.value() : DataV2_0::RIL_TrafficDescriptor {}, matchedAllFlag);
    // check extension if there are extended information which use V3 structure
    bool updated = FillSliceInfoExt(req, tdInfo.has_value() ? tdInfo.value() : DataV2_0::RIL_TrafficDescriptor {});
    RilLogV("Extended TD information : %s", (updated) ? "exists" : "none");

    return sizeof(sit_pdp_setup_data_call_v3_req);
}

// Cover from HAL Radio Interface 1.6 and later
ModemData *ProtocolPsBuilderLegacy::BuildSetupDataCall(int rat, PdpContext *pPdpContext, int slotId, int reason, const char *addresses,
                                                       int pduSessionId, int optionalSliceInfoFlag, int optionalSliceInfoSst,
                                                       int optionalSliceInfoSD, int optionalSliceInfoMappedSst, int optionalSliceInfoMappedSD,
                                                       const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo, bool matchedAllFlag, bool alwaysOnPdu) {
    if (!isValidPdpApn(pPdpContext)) return NULL;

    sit_pdp_setup_data_call_v3_req req_v3;

    const int lengthV3 = FillSetupDataCallIpcV3(req_v3, rat, pPdpContext, slotId, reason, addresses,
                                            pduSessionId, optionalSliceInfoFlag,
                                            optionalSliceInfoSst, optionalSliceInfoSD,
                                            optionalSliceInfoMappedSst, optionalSliceInfoMappedSD,
                                            tdInfo, matchedAllFlag);
    if (lengthV3 == 0) return NULL;

    req_v3.reqIpcVersion = GetIpcVer();
    if (req_v3.reqIpcVersion  == ProtocolPsBuilderInterface::REQ_IPC_VERSION_1) {
        const int lengthV2 = sizeof(sit_pdp_setup_data_call_v2_req);
        RilLogV("size of req(v2) : %d", lengthV2);
        sit_pdp_setup_data_call_v2_req *req_v2 = (sit_pdp_setup_data_call_v2_req *)&req_v3;
        // Reconfigure Header for v2 downgrade
        InitRequestHeader(&(*req_v2).hdr, SIT_SETUP_DATA_CALL, lengthV2);
        return new ModemData((char *)req_v2, lengthV2);
    }
    RilLogV("size of req(v3) : %d", lengthV3);
    req_v3.alwaysOnPdu = alwaysOnPdu;
    return new ModemData((char *)&req_v3, lengthV3);
}

void ProtocolPsBuilderLegacy::FillHandoverIpAddress(sit_pdp_setup_data_call_req &req, int slotId, int reason, const char *addresses)
{
    //Handover
    //clear address
    memset(req.IPv4Address, 0, MAX_IPV4_ADDR_LEN);
    memset(req.IPv6Address, 0, MAX_IPV6_ADDR_LEN);

    /*
        ro.telephony.iwlan_operation_mode=legacy
        // From Q version default is AP-assisted
        public static final String IWLAN_OPERATION_MODE_DEFAULT = "default";
        public static final String IWLAN_OPERATION_MODE_LEGACY = "legacy";
        public static final String IWLAN_OPERATION_MODE_AP_ASSISTED = "AP-assisted";
    */
    string iwlanOpMode = SystemProperty::Get(PROPERTY_IWLAN_OPERATION_MODE, "default");
    RilLogV("Handover Reason on IWLAN operation mode: %s", iwlanOpMode.c_str());
    if (iwlanOpMode == string{"legacy"}) {
        ::FillHandoverIpAddressLegacy(req, slotId);
    } else {
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
            memcpy(req.IPv4Address, &sa.sin_addr, MAX_IPV4_ADDR_LEN);
        }
        if (tempaddrstr6[0]) {
            //req.pdp_type = SIT_PDP_PDP_TYPE_IPV6;
            memcpy(req.IPv6Address, &sa6.sin6_addr, MAX_IPV6_ADDR_LEN);
        }
    }
}

ModemData *ProtocolPsBuilderLegacy::BuildSetInitialAttachApn(PdpContext *pPdpContext, bool isEsmFlagZero/* = false*/)
{
    RilLog("%s", __FUNCTION__);
    if (!::isValidPdpApn(pPdpContext)) return NULL;

    sit_pdp_set_initial_attach_apn_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_INITIAL_ATTACH_APN, length);

    // CID
    int cid = pPdpContext->GetCID();
    if (cid < 0) {
        return NULL;
    }
    req.cid = (BYTE)(cid & 0xFF);
    req.rat = SIT_RAT_TYPE_LTE;

    if ( !FillApnInfo(req, pPdpContext, isEsmFlagZero, true) )
        return NULL;

    // ignore return
    FillDataProfileId(req, pPdpContext, true);

    // For TMO TC : L_LTE_ROAM_54650_INT Roam_302370_Data Roaming ON
    // roaming protocol
    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    req.roaming_pdp_type = ::GetPdpType(pApnSetting->GetRoamingProtocol());

    if (isEsmFlagZero || TextUtils::IsEmpty(pApnSetting->GetApn())) {
        req.roaming_pcscf_req_type =  ::GetPcscfReqTypeforNullAPN(pApnSetting, pApnSetting->GetRoamingProtocol());
    } else {
        // Roaming P-CSCF req type
        req.roaming_pcscf_req_type = ::GetPcscfReqType(pApnSetting, pApnSetting->GetRoamingProtocol());
        RilLogV("roaming_pcscf_req_type: %d", req.roaming_pcscf_req_type);
    }

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetDataProfile(const RIL_DataProfileInfo_v15 *dpi, bool /* isVzw */)
{
    if (dpi == NULL) {
        return NULL;
    }

    sit_pdp_set_data_profile_req req;
    int length = prepareSetDataProfileRequest(req, dpi);
    if (length == 0) return NULL;

    // protocol
    req.pdp_type = ::GetPdpType(dpi->protocol);
    // roaming protocol
    req.roaming_pdp_type = ::GetPdpType(dpi->roamingProtocol);
    // auth type
    req.auth_type = ::ConvertAuthTypeToProtocolAuthType(dpi->authType);

    req.radio_access_family = dpi->bearerBitmask;
    req.apn_type = dpi->supportedTypesBitmask;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetDataProfile(const RIL_DataProfileInfo_V1_4 *dpi, bool /* isVzw */)
{
    if (dpi == NULL) {
        return NULL;
    }

    sit_pdp_set_data_profile_req req;
    int length = prepareSetDataProfileRequest(req, dpi);

    // protocol
    req.pdp_type = ::ConvertPdpProtocolTypeToSitPdpType(dpi->protocol);
    // roaming protocol
    req.roaming_pdp_type = ::ConvertPdpProtocolTypeToSitPdpType(dpi->roamingProtocol);
    // auth type
    req.auth_type = ::ConvertAuthTypeToProtocolAuthType(dpi->authType);

    req.radio_access_family = dpi->bearerBitmap;
    req.apn_type = dpi->supportedApnTypesBitmap;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetDataProfile(const RIL_DataProfileInfo_V1_5 *dpi) {
    if (dpi == NULL) {
        return NULL;
    }

    sit_pdp_set_data_profile_req req;
    int length = prepareSetDataProfileRequest(req, dpi);

    // protocol
    req.pdp_type = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->protocol);
    // roaming protocol
    req.roaming_pdp_type = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->roamingProtocol);
    // auth type
    req.auth_type = ::ConvertAuthTypeToProtocolAuthType((int)dpi->authType);

    req.radio_access_family = dpi->bearerBitmap;
    req.apn_type = dpi->supportedApnTypesBitmap;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetDataProfile(const DataV2_0::RIL_DataProfileInfo *dpi) {
    if (dpi == NULL) {
        return NULL;
    }

    sit_pdp_set_data_profile_req req;
    int length = prepareSetDataProfileRequest(req, dpi);

    // protocol
    req.pdp_type = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->protocol);
    // roaming protocol
    req.roaming_pdp_type = ::ConvertPdpProtocolTypeToSitPdpType((int)dpi->roamingProtocol);
    // auth type
    req.auth_type = ::ConvertAuthTypeToProtocolAuthType((int)dpi->authType);

    req.radio_access_family = dpi->bearerBitmap;
    req.apn_type = dpi->supportedApnTypesBitmap;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildDeactDataCall(int cid, int reason)
{
    if (cid < 0) {
        return NULL;
    }

    sit_pdp_deact_data_call_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_DEACT_DATA_CALL, length);
    req.cid = (BYTE)(cid & 0xFF);
    req.deact_reason = (BYTE)(reason & 0xFF);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildGetDataCallList()
{
    null_data_format req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_DATA_CALL_LIST, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetFastDormancyInfo(BYTE lcdOn, BYTE lcdOff, BYTE rel8LcdOn, BYTE rel8LcdOff)
{
    sit_pdp_set_fd_info_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_FD_INFO, length);
    req.lcd_on = lcdOn & 0xFF;
    req.lcd_off = lcdOff & 0xFF;
    req.rel8_lcd_on = rel8LcdOn & 0xFF;
    req.rel8_lcd_off = rel8LcdOff & 0xFF;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildStartKeepAlive(RIL_KeepaliveRequest reqData)
{
    sit_pdp_start_keepalive_req req;
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_START_KEEPALIVE, length);

    req.keepalive_type = reqData.type;

    memcpy(req.src_addr, reqData.sourceAddress, MAX_IPV6_ADDR_LEN);
    req.source_port = reqData.sourcePort;

    memcpy(req.dst_addr, reqData.destinationAddress, MAX_IPV6_ADDR_LEN);
    req.dst_port = reqData.destinationPort;

    req.max_interval = reqData.maxKeepaliveIntervalMillis;
    req.cid = reqData.cid;

    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildStopKeepAlive(int sessionHande)
{
    sit_pdp_stop_keepalive_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_STOP_KEEPALIVE, length);

    req.keepalive_handle = sessionHande;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildDetach(int detachReason)
{
    sit_pdp_detach_req req;
    int length = sizeof(sit_pdp_detach_req);
    InitRequestHeader(&req.hdr, SIT_DETACH, length);

    req.detach_reason = (BYTE)detachReason;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetPreferredDataModem(int stackId)
{
    if (stackId < 0) {
        return NULL;
    }

    sit_net_set_preferred_data_modem_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_SET_PREFERRED_DATA_MODEM, length);
    req.stackId = (BYTE)(stackId & 0xFF);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSetPdnIpAddress(RIL_SetPdnIpAddressRequest reqData)
{
    sit_pdp_set_pdn_ip_address_req req;
    int length = sizeof(sit_pdp_set_pdn_ip_address_req);
    req.cid = reqData.cid;
    req.pdp_type = reqData.type;

    if (reqData.type == PDP_PROTOCOL_TYPE_IPV4V6 || reqData.type == PDP_PROTOCOL_TYPE_IP) {
        memcpy(req.IPv4Address, reqData.IPv4_addresses, MAX_IPV4_ADDR_LEN);
    }
    else if (reqData.type == PDP_PROTOCOL_TYPE_IPV4V6 ||reqData.type == PDP_PROTOCOL_TYPE_IPV6) {
        memcpy(req.IPv6Address, reqData.IPv6_addresses, MAX_IPV6_ADDR_LEN);
    }
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildAllocatePduSessionId()
{
    sit_pdp_alloc_pdu_session_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_ALLOC_PDU_SESSION, length);
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildReleasePduSessionId(int sessionId)
{
    sit_pdp_rel_pdu_session_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_REL_PDU_SESSION, length);
    req.session_id = sessionId;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildStartHandover(int callId)
{
    sit_pdp_start_handover_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_START_HANDOVER, length);
    req.call_id= callId;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildCancelHandover(int callId)
{
    sit_pdp_cancel_handover_req req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_CANCEL_HANDOVER, length);
    req.call_id= callId;
    return new ModemData((char *)&req, length);
}

ModemData *ProtocolPsBuilderLegacy::BuildSlicingConfig(int version)
{
    sit_pdp_get_slicing_config req;
    int length = sizeof(req);
    InitRequestHeader(&req.hdr, SIT_GET_SLICING_CONFIG, length);

    req.slicing_config_version= version;
    return new ModemData((char *)&req, length);
}

/*
 *  retrun false if failed, it needs to return NULL int the parent
 */
template <typename T>
bool ProtocolPsBuilderLegacy::FillApnInfo(T &req, PdpContext *pPdpContext, bool isEsmFlagZero, bool /* forInitialAttach */)
{
    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    if (pApnSetting == NULL) {
        RilLogV("%s, return false ApnSetting is null", __FUNCTION__);
        return false;
    }

    // APN
    if (!isEsmFlagZero) {
        // APN
        const char *apn = pApnSetting->GetApn();

        // Bypass Null APN check, There were very few usage for null APN in previous Android version.
        // Now it's better to rebuild filtering condition later
        // SetupDataCall for NetworkSlicing with TD in Android T requirement does not need available APN or DNN미내.
        strlcpy(req.apn, apn, MAX_PDP_APN_LEN - 1);

        // username and password
        if (!TextUtils::IsEmpty(pApnSetting->GetUsername()))
            strncpy(req.username, pApnSetting->GetUsername(), MAX_AUTH_USER_NAME_LEN - 1);
        if (!TextUtils::IsEmpty(pApnSetting->GetPassword()))
            strncpy(req.password, pApnSetting->GetPassword(), MAX_AUTH_PASSWORD_LEN - 1);

        // auth type
        req.auth_type = ::ConvertAuthTypeToProtocolAuthType(pApnSetting->GetAuthType());
        // P-CSCF req type
        req.pcscf_addr_req = ::GetPcscfReqType(pApnSetting, pApnSetting->GetProtocol());
    }
    else {
        // use NULL APN
       *req.apn = 0;
       *req.username = 0;
       *req.password = 0;
       req.auth_type = SIT_PDP_AUTH_TYPE_NONE;
       // P-CSCF req type
       req.pcscf_addr_req = ::GetPcscfReqTypeforNullAPN(pApnSetting, pApnSetting->GetProtocol());
    }

    // legacy data profile and APN type
    req.data_profile = ::ConvertDataProfileIdToProtocolDataProfile(pPdpContext->GetDataProfileId());
    req.apn_type = ::GetProtocolApnType(pPdpContext->GetDataProfileId(), pApnSetting);

    // protocol
    req.pdp_type = ::GetPdpType(pApnSetting->GetProtocol());

    return true;
}

template <typename T>
bool ProtocolPsBuilderLegacy::FillDataProfileId(T &req, PdpContext *pPdpContext, bool forInitialAttach)
{
    // DataProfileId
    if (forInitialAttach) {
        req.profile_id = SIT_IA_PROFILE_ID_UNKNOWN;
        if(TextUtils::IsEmpty(req.apn))
            pPdpContext->SetDataProfileId(DATA_PROFILE_NULL_APN_IA);
    } else {
        req.profile_id = pPdpContext->GetDataProfileId();
        if(req.profile_id == DATA_PROFILE_DEFAULT) {
            req.profile_id = SIT_IA_PROFILE_ID_INTERNET;
        } else {
            RilLogV("ignore non-Internet ProfileId: %d", req.profile_id);
            return false;
        }
    }
    return true;
}


int ProtocolPsBuilderLegacy::prepareSetDataProfileRequest(sit_pdp_set_data_profile_req &req, const DataV2_0::RIL_DataProfileInfo *dpi)
{
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_DATA_PROFILE, length);

    // profile id, vzw checks
    // Don't care operator. just pass profile_id directly
    req.profile_id = dpi->profileId;
    // 0xFF will be required for DualVoLTE, but higher layer will set this
    // if(!isVzw // APN==DEFAULT) req.profile_id = SIT_IA_PROFILE_ID_INTERNET;
    RilLogV("profile_id: %d", req.profile_id);

    // APN
    dpi->apn.copy(req.apn,MAX_PDP_APN_LEN-1);
    // username and password
    if (!dpi->user.empty())
        dpi->user.copy(req.username, MAX_AUTH_USER_NAME_LEN - 1);
    if (!dpi->password.empty())
        dpi->password.copy(req.password, MAX_AUTH_PASSWORD_LEN - 1);

    req.data_profile_info_type = dpi->type;

    // apn disable flag
    req.enabled = dpi->enabled;

    // ? Vzw only parameters ?
    req.max_conns_time = dpi->maxConnsTime;
    req.max_conns = dpi->maxConns;
    req.wait_time = dpi->waitTime;

    return length;
}


template<typename T>
int ProtocolPsBuilderLegacy::prepareSetDataProfileRequest(sit_pdp_set_data_profile_req &req, T *(&dpi))
{
    int length = sizeof(req);
    memset(&req, 0, length);
    InitRequestHeader(&req.hdr, SIT_SET_DATA_PROFILE, length);

    // profile id, vzw checks
    // Don't care operator. just pass profile_id directly
    req.profile_id = dpi->profileId;
    // 0xFF will be required for DualVoLTE, but higher layer will set this
    // if(!isVzw // APN==DEFAULT) req.profile_id = SIT_IA_PROFILE_ID_INTERNET;
    RilLogV("profile_id: %d", req.profile_id);

    // APN
    strlcpy(req.apn, dpi->apn, MAX_PDP_APN_LEN - 1);

    // username and password
    if (!TextUtils::IsEmpty(dpi->user))
        strncpy(req.username, dpi->user, MAX_AUTH_USER_NAME_LEN - 1);
    if (!TextUtils::IsEmpty(dpi->password))
        strncpy(req.password, dpi->password, MAX_AUTH_PASSWORD_LEN - 1);

    req.data_profile_info_type = dpi->type;

    // apn disable flag
    req.enabled = dpi->enabled;

    // ? Vzw only parameters ?
    req.max_conns_time = dpi->maxConnsTime;
    req.max_conns = dpi->maxConns;
    req.wait_time = dpi->waitTime;

    return length;
}

