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
 * protocolpsutil.cpp
 *
 */
#include "./protocolpsutil.h"
#include <sstream>
#include "./sitdef.h"
#include "../../base/pdpcontext.h"
#include "../../base/apnsetting.h"
#include "rillog.h"
#include <librilutils/textutils.h>
#include <librilutils/systemproperty.h>

int GetProtocolApnType(int dataProfileId, ApnSetting *apnSetting) {
    int apntype = SIT_PDP_APN_TYPE_DEFAULT;
    int matched = 0;

    // In all type '*' case, we just follows profileID.
    // IMS and Emergency is considered as decidated PDN.
    // SIT interface just concern about special APN like IMS and emergency.
    if (apnSetting->CanHandleType(APN_TYPE_BIT_EMERGENCY)) {
       apntype = SIT_PDP_APN_TYPE_EMERGENCY;
       matched++;
    }
    if (apnSetting->CanHandleType(APN_TYPE_BIT_IMS)) {
       apntype = SIT_PDP_APN_TYPE_IMS;
       matched++;
    }
    // When multiple type is supported, DEFAULT type will be set.
    if (apnSetting->CanHandleType(APN_TYPE_BIT_DEFAULT)) {
       apntype = SIT_PDP_APN_TYPE_DEFAULT;
       matched++;
    }
    if (matched > 1) {
        // Second chance to adjust apn type just for IMS
        switch (dataProfileId) {
        case DATA_PROFILE_IMS:
            apntype = SIT_PDP_APN_TYPE_IMS;
            break;
        default:
            apntype = SIT_PDP_APN_TYPE_DEFAULT;
            break;
        }
        RilLogV("GetProtocolApnType: Multiple Type is requested, follow ProfileID");
    }
    RilLogV("GetProtocolApnType: apnSetting->GetType()=%s, ProfileID:%d, SIT apntype=%d", apnSetting->GetTypeString(), dataProfileId, apntype);

    return apntype;
}

int GetPdpType(const char *protocol) {
    // default
    if (TextUtils::IsEmpty(protocol)) {
        return SIT_PDP_PDP_TYPE_IPV4;
    }

    if (strcmp(protocol, STR_PDP_TYPE_IPV4) == 0) {
        return SIT_PDP_PDP_TYPE_IPV4;
    } else if (strcmp(protocol, STR_PDP_TYPE_IPV6) == 0) {
        return SIT_PDP_PDP_TYPE_IPV6;
    } else if (strcmp(protocol, STR_PDP_TYPE_IPV4V6) == 0) {
        return SIT_PDP_PDP_TYPE_IPV4IPV6;
    }

    // default
    return SIT_PDP_PDP_TYPE_IPV4;
}

int ConvertAuthTypeToProtocolAuthType(int authtype) {
    int type = SIT_PDP_AUTH_TYPE_NONE;
    switch (authtype) {
    case SETUP_DATA_AUTH_NONE:
        type = SIT_PDP_AUTH_TYPE_NONE;
        break;
    case SETUP_DATA_AUTH_PAP:
        type = SIT_PDP_AUTH_TYPE_PAP;
        break;
    case SETUP_DATA_AUTH_CHAP:
        type = SIT_PDP_AUTH_TYPE_CHAP;
        break;
    case SETUP_DATA_AUTH_PAP_CHAP:
        type = SIT_PDP_AUTH_TYPE_PAP_CHAP;
        break;
    default:
        type = SIT_PDP_AUTH_TYPE_NONE;
        break;
    }
    return type;
}

int ConvertDataProfileIdToProtocolDataProfile(int dataProfileId) {
    if (dataProfileId == RIL_DATA_PROFILE_TETHERED) {
        return SIT_PDP_DATA_PROFILE_TETHERED;
    }
    return SIT_PDP_DATA_PROFILE_DEFAULT;
}

int ConvertPdpProtocolTypeToSitPdpType(int protocol) {
    switch (protocol) {
      case 0: /* IP */
          return SIT_PDP_PDP_TYPE_IPV4;
      case 1: /* IPV6 */
          return SIT_PDP_PDP_TYPE_IPV6;
      case 2: /* IPV4V6 */
          return SIT_PDP_PDP_TYPE_IPV4IPV6;
      case 3: /* PPP */
          return SIT_PDP_PDP_TYPE_PPP;
      case -1: /* UNKNOWN */
      case 4: /* NON_IP */
      case 5: /* UNSTRUCTURED */
          return SIT_PDP_PDP_TYPE_UNKNOWN;
      default:
          return SIT_PDP_PDP_TYPE_IPV4;
    }
}

int checkPcscfTypeIpv4v6(const char *protocol) {
    if (strcmp(protocol, STR_PDP_TYPE_IPV4) == 0) {
        return SIT_PDP_PCSCF_REQ_IPV4;
    } else if (strcmp(protocol, STR_PDP_TYPE_IPV6) == 0) {
        return SIT_PDP_PCSCF_REQ_IPV6;
    } else if (strcmp(protocol, STR_PDP_TYPE_IPV4V6) == 0) {
        return SIT_PDP_PCSCF_REQ_IPV4V6;
    }
    return SIT_PDP_PCSCF_REQ_NONE;
}

int GetPcscfReqType(ApnSetting *apnSetting, const char *protocol) {
    if (TextUtils::IsEmpty(apnSetting->GetApn())) {
        RilLogV("GetPcscfReqType: Null APN IPv4v6");
        return SIT_PDP_PCSCF_REQ_IPV4V6;
    } else if (apnSetting->CanHandleType(APN_TYPE_BIT_IMS) || apnSetting->CanHandleType(APN_TYPE_BIT_EMERGENCY) ||
               apnSetting->isPcscfRequiredForDefaultPDN() ) {
        // default
        if (TextUtils::IsEmpty(protocol)) {
            return SIT_PDP_PCSCF_REQ_IPV4;
        }

        int pcscf_iptype = checkPcscfTypeIpv4v6(protocol);
        if (pcscf_iptype != SIT_PDP_PCSCF_REQ_NONE)
            return pcscf_iptype;

        // default
        return SIT_PDP_PCSCF_REQ_IPV4;
    }

    return SIT_PDP_PCSCF_REQ_NONE;
}

int GetPcscfReqTypeforNullAPN(ApnSetting *apnSetting, const char *protocol) {
    if (TextUtils::IsEmpty(apnSetting->GetApn())) {
        RilLogV("GetPcscfReqType: Null APN in DB");
    }

    if (TextUtils::IsEmpty(protocol)) {
        RilLogV("GetPcscfReqTypeforNullAPN : No protocol value exists in DB ,set as default (v4v6)");
        return SIT_PDP_PCSCF_REQ_IPV4V6;
    }

    int pcscf_iptype = checkPcscfTypeIpv4v6(protocol);
    if (pcscf_iptype != SIT_PDP_PCSCF_REQ_NONE)
        return pcscf_iptype;

    // default
    return SIT_PDP_PCSCF_REQ_NONE;
}

bool isRatForCDMA(int rat) {
    switch (rat) {
      case RADIO_TECH_IS95A: [[fallthrough]];
      case RADIO_TECH_IS95B: [[fallthrough]];
      case RADIO_TECH_1xRTT: [[fallthrough]];
      case RADIO_TECH_EVDO_0: [[fallthrough]];
      case RADIO_TECH_EVDO_A: [[fallthrough]];
      case RADIO_TECH_EVDO_B:
          break;
      default:
          return false;
    }
    return true;
}

bool isValidPdpApn(PdpContext *pPdpContext) {
    if (pPdpContext == NULL) {
        RilLog("pPdpContext == NULL");
        return false;
    }

    ApnSetting *pApnSetting = pPdpContext->GetApnSetting();
    if (pApnSetting == NULL) {
        RilLog("pApnSetting == NULL");
        return false;
    }
    return true;
}

int ConvertRadioTechToSitValue(int rat)
{
    switch (rat) {
        case RADIO_TECH_GPRS: return SIT_RAT_TYPE_GPRS;
        case RADIO_TECH_EDGE: return SIT_RAT_TYPE_EDGE;
        case RADIO_TECH_UMTS: return SIT_RAT_TYPE_UMTS;
        case RADIO_TECH_IS95A: return SIT_RAT_TYPE_IS95A;
        case RADIO_TECH_IS95B: return SIT_RAT_TYPE_IS95B;
        case RADIO_TECH_1xRTT: return SIT_RAT_TYPE_1xRTT;
        case RADIO_TECH_EVDO_0: return SIT_RAT_TYPE_EVDO_0;
        case RADIO_TECH_EVDO_A: return SIT_RAT_TYPE_EVDO_A;
        case RADIO_TECH_HSDPA: return SIT_RAT_TYPE_HSDPA;
        case RADIO_TECH_HSUPA: return SIT_RAT_TYPE_HSUPA;
        case RADIO_TECH_HSPA: return SIT_RAT_TYPE_HSPA;
        case RADIO_TECH_EVDO_B: return SIT_RAT_TYPE_EVDO_B;
        case RADIO_TECH_EHRPD: return SIT_RAT_TYPE_EHRPD;
        case RADIO_TECH_LTE: return SIT_RAT_TYPE_LTE;
        case RADIO_TECH_HSPAP: return SIT_RAT_TYPE_HSPAP;
        case RADIO_TECH_GSM: return SIT_RAT_TYPE_GSM;
        case RADIO_TECH_IWLAN: return SIT_RAT_TYPE_IWLAN;
        case RADIO_TECH_TD_SCDMA: return SIT_RAT_TYPE_TD_SCDMA;
        //case RADIO_TECH_HSPAP: return SIT_RAT_TYPE_HSPADCPLUS; // No mapping in Android
        case RADIO_TECH_LTE_CA: return SIT_RAT_TYPE_LTE_CA;
        case RADIO_TECH_NR: return SIT_RAT_TYPE_5G;
        default:
            return SIT_RAT_TYPE_UNKNOWN;
    }
    return SIT_RAT_TYPE_UNKNOWN;
}

void FillHandoverIpAddressLegacy(sit_pdp_setup_data_call_req &req, int slotId) {
    int allow_handover = 0;

    // explicit legacy for backward compatibility
    if (slotId == 0)
        allow_handover = SystemProperty::GetInt(PROPERTY_HANDOVER_0, 0);
    else if (slotId == 1)
        allow_handover = SystemProperty::GetInt(PROPERTY_HANDOVER_1, 0);
    else
        RilLogV("invalid slotId: %d", slotId);

    if (allow_handover == 1) {
        char buf[MAX_ADDRESS_STRING_LEN] = {
            0,
        };
        char nullIp[MAX_IPV6_ADDR_LEN] = {
            0,
        };
        string ipv4Address = "";
        string ipv6Address = "";
        int err;

        // get ipv4 address
        if (slotId == 0)
            ipv4Address = SystemProperty::Get(PROPERTY_HANDOVER_IPV4_ADDRESS_0);
        else if (slotId == 1)
            ipv4Address = SystemProperty::Get(PROPERTY_HANDOVER_IPV4_ADDRESS_1);
        else
            RilLogV("invalid Id");
        memcpy(buf, ipv4Address.c_str(),
               (ipv4Address.length() < sizeof(buf) ? ipv4Address.length() + 1 : sizeof(buf) - 1));

        RilLogV("handover: IPv4 address:%s", buf);
        if (memcmp(buf, INVALID_IP, sizeof(INVALID_IP)) == 0) {
            RilLogV("handover: IPv4 zero set APN protocol as IPv6");
            req.pdp_type = SIT_PDP_PDP_TYPE_IPV6;
        }
        if (memcmp(buf, nullIp, MAX_IPV4_ADDR_LEN) != 0) {
            err = inet_pton(AF_INET, buf, req.IPv4Address);
            if (err != 1) RilLogV("inet_pton for IPv4 failed: err:%d", err);
        }

        // get ipv6 address
        if (slotId == 0)
            ipv6Address = SystemProperty::Get(PROPERTY_HANDOVER_IPV6_ADDRESS_0);
        else if (slotId == 1)
            ipv6Address = SystemProperty::Get(PROPERTY_HANDOVER_IPV6_ADDRESS_1);
        else
            RilLogV("invalid Id");
        memcpy(buf, ipv6Address.c_str(),
               (ipv6Address.length() < sizeof(buf) ? ipv6Address.length() + 1 : sizeof(buf) - 1));

        RilLogV("handover: IPv6 address:%s", buf);
        if (memcmp(buf, nullIp, MAX_IPV6_ADDR_LEN) != 0) {
            err = inet_pton(AF_INET6, buf, req.IPv6Address);
            if (err != 1) RilLogV("inet_pton for IPv6 failed: err:%d", err);
        }
    } else {
        RilLogW("Not handover");
    }
}

void FillHandoverIpAddressLegacy(int slotId) {
    sit_pdp_setup_data_call_req req;  // dummy req
    FillHandoverIpAddressLegacy(req, slotId);
}

void ParseHandoverAddresses(const char *addresses, struct sockaddr_in &sa, struct sockaddr_in6 &sa6,
                                   char (&t)[INET_ADDRSTRLEN+1], char (&t6)[INET6_ADDRSTRLEN+1]) {
    // CP just supports only one ip address for each type
    vector<string> tokens;
    stringstream ss(addresses);
    string intermediate;
    while(getline(ss, intermediate, ' ')) tokens.push_back(intermediate);

    bool ipv4_found=false, ipv6_found=false;
    for (unsigned int i=0; i < tokens.size(); i++) {
        if (!ipv4_found && inet_pton(AF_INET, tokens[i].c_str(), &(sa.sin_addr)) != 0) {
            ipv4_found = true;
            inet_ntop(AF_INET, &sa.sin_addr, t, INET_ADDRSTRLEN);
            RilLogV("handover: IPv4:%s", t);
        } else if (!ipv6_found && inet_pton(AF_INET6, tokens[i].c_str(), &(sa6.sin6_addr)) != 0) {
            ipv6_found = true;
            inet_ntop(AF_INET6, &sa6.sin6_addr, t6, INET6_ADDRSTRLEN);
            RilLogV("handover: IPv6:%s", t6);
        } else {
            RilLogV("ignore secondary address or invalid address. tokens[%d] = %s", i, tokens[i].c_str());
        }
    }
}

void InitQoSession(vector<QosSession_t> &qosSession, int &QosSessionCnt){

    QosSessionCnt = 0;

    if(qosSession.size() != 0){
        for(auto it = qosSession.begin(); it != qosSession.end(); it++) {
            if(it->QosFilters.size() != 0) {
                it->QosFilters.clear();
            }
        }
        qosSession.clear();
    }
}

void InitDataCall(DataCall & dataCall) {
    dataCall.status = 0;
    dataCall.suggestedRetryTime = RETRY_NO_SUGGESTED;
    dataCall.cid = 0;
    dataCall.active = 0;
    dataCall.pdpType = 0;
    memset(&(dataCall.ipv4), 0, sizeof(PDP_ADDR_V4));
    memset(&(dataCall.ipv6), 0, sizeof(PDP_ADDR_V6));

    //m_dataCall.mtu_size;
    dataCall.pco = 0;
    dataCall.IPC_version = 0;
    dataCall.pcscf_ext_count = 0;
    dataCall.ambr = {};

    //RadioHal ver 1.6
    dataCall.ipv4_mtu_size = 0;
    dataCall.ipv6_mtu_size = 0;
    dataCall.pdu_session_id = 0;
    dataCall.defaultQosType = NOT_SET;
    memset(&(dataCall.defaultEpsQos), 0, sizeof(RIL_EpsQos));
    memset(&(dataCall.defaultNrQos), 0, sizeof(RIL_NrQos));

    InitQoSession(dataCall.QosSession, dataCall.QosSessionCnt);

    dataCall.handoverFailureMode = LEGACY;
    //Additional Info for 1.6
    dataCall.sliceInfoType = RIL_OPT_INFO_INVALID;
    memset(&(dataCall.sliceInfo), 0, sizeof(RIL_SliceInfo));
    dataCall.tdLength = 0;
    for (int i = 0; i < MAX_TD_LIST; i++)
        dataCall.tdInfo[i].clear();
}

void FillIpAddr(bool isIpv6, char const *address, char *target) {
    char t[INET6_ADDRSTRLEN+1];
    if (!isIpv6) {
        sockaddr_in sa = {};
        int err = inet_pton(AF_INET, address, &(sa.sin_addr));
        if (err != 1) RilLogV("inet_pton for IPv4 failed: err:%d", err);
        inet_ntop(AF_INET, &sa.sin_addr, t, INET_ADDRSTRLEN);
        RilLogV("IPv4:%s", t);
        memcpy(target, &sa.sin_addr, MAX_IPV4_ADDR_LEN);

    } else {
        sockaddr_in6 sa = {};
        int err = inet_pton(AF_INET6, address, &(sa.sin6_addr));
        if (err != 1) RilLogV("inet_pton for IPv6 failed: err:%d", err);
        inet_ntop(AF_INET6, &sa.sin6_addr, t, INET6_ADDRSTRLEN);
        RilLogV("IPv6:%s", t);
        memcpy(target, &sa.sin6_addr, MAX_IPV6_ADDR_LEN);
    }
}

uint8_t ConvertConnectionCapabilities(std::string cc) {
    uint8_t cc_val = 0;
    string token;
    string delim = ",";
    std::for_each(cc.begin(), cc.end(), [](char &c) {
                  c = ::tolower(c);
                  });
    cc.erase(std::remove_if(cc.begin(), cc.end(), [](char c) {
                            return (c == ' ');
                            }), cc.end());
    size_t pos;
    while ((pos = cc.find(delim)) != std::string::npos) {
        token = cc.substr(0, pos);
        cc.erase(0, pos + delim.length());
        if (token == "ims") cc_val |= CC_IMS;
        if (token == "mms") cc_val |= CC_MMS;
        if (token == "supl") cc_val |= CC_SUPL;
        if (token == "internet") cc_val |= CC_INTERNET;
    }
    return cc_val;
}
