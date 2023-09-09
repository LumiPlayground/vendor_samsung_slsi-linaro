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
 * pdpcontext.cpp
 *
 *  Created on: 2014. 6. 25.
 *      Author: sungwoo48.choi
 */

#include "pdpcontext.h"
#include "netifcontroller.h"
#include "rillog.h"
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <librilutils/textutils.h>
#include <arpa/inet.h>

#include <sstream>
#include "PsUtil.h"
#include "protocolpsutil.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_PDP, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_PDP, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_PDP, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_PDP, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define    PREFIX_IFNAME    "rmnet"

PdpContext::PdpContext(int cid) : PdpContext(cid, nullptr, -1)
{
}

PdpContext::PdpContext(int cid, const char *ifprefix, int ifindex)
{
    // InitDataCall();
    m_dataCall = {};
    m_dataCall.suggestedRetryTime = RETRY_NO_SUGGESTED;
    memset(m_szIfname, 0, sizeof(m_szIfname));
    m_pApnSetting = NULL;
    m_dataProfileId = DATA_PROFILE_DEFAULT;
    m_state = PDP_CONTEXT_UNAVAILABLE;
    m_pNetIfController = NULL;
    if (cid > 0) {
        m_cid = cid;
        if (ifprefix == NULL || *ifprefix == 0) {
            ifprefix = PREFIX_IFNAME;
        }
        if (ifindex < 0) {
            ifindex = cid - 1;
        }
        snprintf(m_szIfname, sizeof(m_szIfname), "%s%d", ifprefix, ifindex);
    }
    else
    {
        m_cid = 0;
    }
    InitQoSession(m_QosSession, m_QosSessionCnt);
    mTdInfo = { };
}

PdpContext::~PdpContext()
{
    if (m_pApnSetting != NULL) {
        delete m_pApnSetting;
    }

    // release NetIfController
    if (m_pNetIfController != NULL) {
        delete m_pNetIfController;
    }

    InitQoSession(m_QosSession, m_QosSessionCnt);
    m_dataCall = {};
}

void PdpContext::InitDataCall() {
    ::InitDataCall(m_dataCall);
}

void PdpContext::Init()
{
    if (m_cid > 0) {
//        if (*m_szIfname == 0) {
//            sprintf(m_szIfname, "%s%d", PREFIX_IFNAME, m_cid - 1);
//        }
        m_pNetIfController = new NetIfController(this);
        SetState(PDP_CONTEXT_DISCONNECTED);
    }
}

void PdpContext::SetApnSetting(ApnSetting *pApnSetting)
{
    if (m_pApnSetting != NULL) {
        delete m_pApnSetting;
    }
    m_pApnSetting = pApnSetting;
}

void PdpContext::SetTdSetting(const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo){
    mTdInfo = tdInfo;
}

bool PdpContext::checkIpv4AddressChanged(const DataCall& old_dc, const DataCall& new_dc)
{
    // old dc can be inactive
    if(!old_dc.ipv4.valid && new_dc.ipv4.valid) return true;
    if(!new_dc.ipv4.valid) return false;
    if(memcmp(old_dc.ipv4.addr, new_dc.ipv4.addr, 4) != 0) {
        return true;
    }
    return false;
}

bool PdpContext::checkIpv6AddressChanged(const DataCall& old_dc, const DataCall& new_dc)
{
    // old dc can be inactive
    if(!old_dc.ipv6.valid && new_dc.ipv6.valid) return true;
    if(!new_dc.ipv6.valid) return false;
    if(memcmp(old_dc.ipv6.addr, new_dc.ipv6.addr, 16) != 0) {
        return true;
    }
    return false;
}

bool PdpContext::UpdateDataCallInfo(const DataCall *dc)
{
    bool ipchanged[2] = {false, false};
    if (dc != NULL) {
        if (m_cid == dc->cid) {
            int old_active = INACTIVE;
            // AP can have higher Active state.
            if(m_dataCall.active != INACTIVE && dc->active != INACTIVE)
            {
                old_active = m_dataCall.active;
            }

            // Now Check IPV6 Global existing vs Link-Local incoming
            // CP will report Global IPV6 Address at best effort. This will treat LinkLocal Address reporting.
            // If we have already generated IPV6 Global Address or something different to Link-Local
            // We will replace whole datacall information
            if(m_dataCall.active != INACTIVE &&
               dc->active != INACTIVE &&
               GetState() == PDP_CONTEXT_CONNECTED &&
               m_dataCall.ipv6.valid && dc->ipv6.valid &&
               !(m_dataCall.ipv6.addr[0] == 0xFE && m_dataCall.ipv6.addr[1] == 0x80) &&
               (dc->ipv6.addr[0] == 0xFE && dc->ipv6.addr[1] == 0x80) &&
               (memcmp(&m_dataCall.ipv6.addr[8], &dc->ipv6.addr[8], 8) == 0)
               )
            {
                // Now Found. So just update other paramters except for IPV6 address and active state
                PDP_ADDR_V6 temp_ipv6;
                RilLogI("Deprecated! IPV6 has network prefix, so keep global address, cid=%d", m_dataCall.cid);
                memcpy(&temp_ipv6, &m_dataCall.ipv6, sizeof(PDP_ADDR_V6));
                InitQoSession(m_dataCall.QosSession, m_dataCall.QosSessionCnt);
                m_dataCall = *dc;
                memcpy(&m_dataCall.ipv6, &temp_ipv6, sizeof(PDP_ADDR_V6));
                ipchanged[1] = true;
            }
            else{
                ipchanged[0] = checkIpv4AddressChanged(m_dataCall, *dc);
                ipchanged[1] = checkIpv6AddressChanged(m_dataCall, *dc);
                InitQoSession(m_dataCall.QosSession, m_dataCall.QosSessionCnt);
                m_dataCall = *dc;
            }

            if(old_active != INACTIVE && dc->active != INACTIVE)
            {
                RilLogI("Sync up ACTIVE or DORMANT, cid=%d, new active=%d, old active=%d", m_dataCall.cid, dc->active, m_dataCall.active);
                m_dataCall.active = dc->active;
            }

            if ((m_dataCall.ipv4.valid && ipchanged[0]) ||
                (m_dataCall.ipv6.valid && ipchanged[1])) {
                return true;
            }
        }
    }
    return false;
}

int PdpContext::SetAddr(const DataCall *dc)
{
    if(m_dataCall.cid != dc->cid){
        RilLogE("PdpContext cid in't matched");
        return -1;
    }

    // TODO copy address information to PDP Context
    if (dc->ipv4.valid) {
        m_dataCall.ipv4.valid= TRUE;
        memcpy(&m_dataCall.ipv4.addr, &dc->ipv4.addr, sizeof(m_dataCall.ipv4.addr));
    }

    if (dc->ipv6.valid) {
        memcpy(&m_dataCall.ipv6.addr, &dc->ipv6.addr, sizeof(m_dataCall.ipv6.addr));
    }

    m_dataCall.active = ACTIVE_AND_LINKDOWN;

    // TODO link down and deactivate

    return 0;
}

void PdpContext::SetState(int state)
{
    if (state >= 0 && state < PDP_CONTEXT_STATE_MAX) {
        m_state = state;
    }
}

void PdpContext::SetActive(int active)
{
    if (active >= INACTIVE && active < ACTIVE_STATE_MAX) {
        m_dataCall.active = active;
    }
}

bool PdpContext::IsAvailable() const
{
    return (GetState() == PDP_CONTEXT_DISCONNECTED && GetState()!= PDP_CONTEXT_CONNECTING);
}


// Just update then act with updated information
int PdpContext::OnChanged(const DataCall *dc)
{
    // Only care old and new DataCall has ACTIVE state.
    if (dc == NULL || dc->active == INACTIVE || m_dataCall.active == INACTIVE || m_cid != dc->cid)
        return -1;

    // update data call information
    if (UpdateDataCallInfo(dc)) {
        if (m_pNetIfController != NULL && m_pNetIfController->ChangeAddress()) {
            RilLogI("Set DataCall(cid=%d) Active State to ACTIVE_AND_LINKUP(%d)", dc->cid, ACTIVE_AND_LINKUP);
        }
        else {
            // fail to change network interface
            return -1;
        }
    }
    return 0;
}

void PdpContext::DumpIpAddress(int family, char *ip) {
    char szAddr[100] = {0, };
    if (ip == NULL) {
        RilLogI("ip is not valid");
        return;
    }
    inet_ntop(family, ip, szAddr, (socklen_t)sizeof(szAddr));
    RilLogI("valid ip=%s", szAddr);
}

void PdpContext::DumpIpAddress(char *ipv4, char *ipv6) {
    if (m_dataCall.ipv4.valid && ipv4 != NULL) {
        DumpIpAddress(AF_INET, ipv4);
    } else {
        RilLogI("currently ipv4 is not valid");
    }
    if (m_dataCall.ipv6.valid && ipv6 != NULL) {
        DumpIpAddress(AF_INET6, ipv6);
    } else {
        RilLogI("currently ipv6 is not valid");
    }
}

bool PdpContext::isMatchedIp(bool isIpv6, char (&ipv4)[MAX_IPV4_ADDR_LEN], char (&ipv6)[MAX_IPV6_ADDR_LEN], const char *addr) {
    if (!isIpv6) {
        return !memcmp(ipv4, addr, MAX_IPV4_ADDR_LEN);
    } else {
        return !memcmp(ipv6, addr, MAX_IPV6_ADDR_LEN);
    }
}

int PdpContext::OnUpdated(int state, char (&ipv4)[MAX_IPV4_ADDR_LEN], char (&ipv6)[MAX_IPV6_ADDR_LEN]) {
    // Only infinite or 0 lifetime will be treated.
    // PREFERRED(Valid), DEPRECATED(Valid), INVALID state will be changed immediately when receiving event.
    // ipv4 and ipv6 is numerical form of ip address
    char nullIp[MAX_IPV6_ADDR_LEN]={0, };
    if(!m_pNetIfController) {
        RilLogE("Unexpected: NetIfController is not initialized");
        return -1;
    }

    RilLogI("E OnUpdated : state = %d", state);
    DumpIpAddress(ipv4, ipv6);

    // Check Add/Update/Delete of IP
    // 0: Invalid -> Delete, same to valid lifetime '1'ms, minimum value
    // 1: Preferred -> Add new if there's no IP address with time inifite
    // 2: Deprecated -> Set deprecated : time 0
    // lifetime means preferred lifetime,
    // valid lifetime will be determined in NetIfController upto state
    unsigned int ifa_valid = 0;
    unsigned int ifa_prefered = 0;
    switch(state) {
      case IP_PREFERRED: // Replace primary address, old primary address will be secondary address
          ifa_valid = INT_MAX;
          ifa_prefered = INT_MAX;
          break;
      case IP_INVALID:   // This will set valid lifetime to 1
          ifa_valid = 1;
          ifa_prefered = 0;
          break;
      case IP_DEPRECATED: // Deprecate assigned address, almost be old primary address
          ifa_valid = 100000;
          ifa_prefered = 0;
          break;
      default:
          RilLogE("Status of IP ADDR is not valid: %d", state);
          break;
    }

    auto it = secondaryAddresses.begin();
    switch(state) {
      case IP_INVALID:
          RilLogI("IP_INVALID");
          // Search existing IP address from primary and deprecatedAddress pool then delete it.
          while(it != secondaryAddresses.end()) {
              char szAddr[30] = {0, };
              inet_ntop(it->isIpv6() ? AF_INET6 : AF_INET, it->getAddr(), szAddr, (socklen_t)sizeof(szAddr));
              RilLogI("it->isIpv6()=%d, ifa_valid=%d, ifa_prefered=%d, it->getAddr() = %s", it->isIpv6(), ifa_valid, ifa_prefered, szAddr);
              DumpIpAddress(!it->isIpv6() ? (char *)it->getAddr() : (char *)NULL, it->isIpv6()? (char *)it->getAddr() : (char *)NULL);

              if (isMatchedIp(it->isIpv6(), ipv4, ipv6, it->getAddr())) {
                  m_pNetIfController->UpdateIfAddr(IP_DELETE, it->isIpv6() ? AF_INET6 : AF_INET, it->getAddr(), ifa_valid, ifa_prefered);
                  it = secondaryAddresses.erase(it);
              } else {
                  it++;
              }
          }
          if(m_dataCall.ipv4.valid) {
              RilLogI("m_dataCall.ipv4.addr = ");
              DumpIpAddress(AF_INET, (char *)m_dataCall.ipv4.addr);
              if (!memcmp(ipv4, &m_dataCall.ipv4.addr, MAX_IPV4_ADDR_LEN)) {
                  // drop current ipv4
                  RilLogI("drop current ipv4");
                  m_pNetIfController->UpdateIfAddr(IP_DELETE, AF_INET, ipv4, ifa_valid, ifa_prefered);
              }
          }
          if(m_dataCall.ipv6.valid) {
              RilLogI("m_dataCall.ipv6.addr = ");
              DumpIpAddress(AF_INET6, (char *)m_dataCall.ipv6.addr);
              if (!memcmp(ipv6, &m_dataCall.ipv6.addr, MAX_IPV6_ADDR_LEN)) {
                  RilLogI("drop current ipv6");
                  m_pNetIfController->UpdateIfAddr(IP_DELETE, AF_INET6, ipv6, ifa_valid, ifa_prefered);
              }
          }
          break;
      case IP_PREFERRED:
          RilLogI("IP_PREFERRED");
          // First check if IP address is existing from primary and deprecatedAddress,
          // if not-exist, add, (make old primary to deprecatedcandiate) then break;

          if (memcmp(nullIp, ipv4, MAX_IPV4_ADDR_LEN) && m_dataCall.ipv4.valid) {
              secondaryAddresses.push_back(DeprecatedAddress(false, IP_PREFERRED, INT_MAX, (char *)m_dataCall.ipv4.addr));
              memcpy(&m_dataCall.ipv4.addr, ipv4, MAX_IPV4_ADDR_LEN);
              m_pNetIfController->UpdateIfAddr(IP_CHANGE, AF_INET, (char *)ipv4, ifa_valid, ifa_prefered);
          }
          if (memcmp(nullIp, ipv6, MAX_IPV6_ADDR_LEN) && m_dataCall.ipv6.valid) {
              secondaryAddresses.push_back(DeprecatedAddress(true, IP_PREFERRED, INT_MAX, (char *)m_dataCall.ipv6.addr));
              memcpy(&m_dataCall.ipv6.addr, ipv6, MAX_IPV6_ADDR_LEN);
              m_pNetIfController->UpdateIfAddr(IP_CHANGE, AF_INET6, (char *)ipv6, ifa_valid, ifa_prefered);
          }
          break;
      case IP_DEPRECATED:
          RilLogI("IP_DEPRECATED");
          // Update exising IP address with provided preferred lifetime
          // from primary address, not expected anyway can process this too
          if (memcmp(nullIp, ipv4, MAX_IPV4_ADDR_LEN) && m_dataCall.ipv4.valid) {
              m_pNetIfController->UpdateIfAddr(IP_UPDATE, AF_INET, (char *)ipv4, ifa_valid, ifa_prefered);
          }
          if (memcmp(nullIp, ipv6, MAX_IPV6_ADDR_LEN) && m_dataCall.ipv6.valid) {
              m_pNetIfController->UpdateIfAddr(IP_UPDATE, AF_INET6, (char *)ipv6, ifa_valid, ifa_prefered);
          }
          while (it != secondaryAddresses.end()) {
              char szAddr[30] = {0, };
              inet_ntop(it->isIpv6() ? AF_INET6 : AF_INET, it->getAddr(), szAddr, (socklen_t)sizeof(szAddr));
              RilLogI("it->isIpv6()=%d, Addr=%s, ifa_valid=%d, ifa_prefered=%d", it->isIpv6(), szAddr, ifa_valid, ifa_prefered);
              if (isMatchedIp(it->isIpv6(), ipv4, ipv6, it->getAddr())) {
                  m_pNetIfController->UpdateIfAddr(IP_UPDATE, it->isIpv6() ? AF_INET6 : AF_INET,
                                                              it->isIpv6() ? ipv6 : ipv4, ifa_valid, ifa_prefered);
              } else {
                  it++;
                  continue;
              }
              it->setLifetime(ifa_valid);
              it->setState(IP_DEPRECATED);
              it++;
          }
          break;
      default:
          break;
    }

    RilLogI("X OnUpdated");

    return 0;
}

int PdpContext::OnActivated(const DataCall *dc)
{
    if (dc == NULL) {
        return -1;
    }

    if (dc->active == INACTIVE) {
        return -1;
    }

    if (m_cid != dc->cid) {
        return -1;
    }

    // update data call information
    if (UpdateDataCallInfo(dc)) {
        if (m_pNetIfController != NULL && m_pNetIfController->BringUp()) {
            RilLogI("Set DataCall(cid=%d) Active State to ACTIVE_AND_LINKUP(%d)", dc->cid, ACTIVE_AND_LINKUP);
            SetActive(ACTIVE_AND_LINKUP);
        }
        else {
            // fail to bring-up network interface
            SetActive(INACTIVE);

            // return error
            return -1;
        }
    }

    if (m_dataCall.ipv6.valid && m_dataCall.ipv6.addr[0] == 0xFE && m_dataCall.ipv6.addr[1] == 0x80) {
        RilLogI("Ipv6 Link-Local Address is detected. DataService may not work correctly.(cid : %d)", dc->cid);
    }
    SetState(PDP_CONTEXT_CONNECTED);

    return 0;
}

int PdpContext::OnActivated()
{
    return OnActivated(&m_dataCall);
}

int PdpContext::OnDeactivated()
{
    if (m_pNetIfController != NULL) {
        m_pNetIfController->TearDown();
    }
    Reset();
    SetState(PDP_CONTEXT_DISCONNECTED);

    return 0;
}

void PdpContext::Reset()
{
    SetState(PDP_CONTEXT_DISCONNECTED);
    m_dataCall = {};
    m_dataCall.suggestedRetryTime = RETRY_NO_SUGGESTED;
    InitQoSession(m_QosSession, m_QosSessionCnt);
    secondaryAddresses.clear();
}

void PdpContext::SetHandoverDnses(string dnses)
{
    RilLogI("dnses:%s", dnses.c_str());
    m_sHandoverDnses = dnses;
}

int PdpContext::OnUpdateQosInfo(int status, const QosSession_t& pQosSession) {
    if(pQosSession.qosSessionId == -1) {
        return -1;
    }

    RilLogI("status : %d, qosSessionId : %d ", status, pQosSession.qosSessionId);

    switch(status) {
        case QOS_ADDED:
            for(auto it = m_QosSession.begin(); it != m_QosSession.end(); it++) {
                if(it->qosSessionId == pQosSession.qosSessionId) {
                    RilLogI("Same qosSessionId is already added.");
                    return -1;
                }
            }
            m_QosSession.push_back(pQosSession);
            m_QosSessionCnt++;
            break;
        case QOS_MODIFIED:
            for (auto it = m_QosSession.begin(); it != m_QosSession.end(); it++) {
                if (it->qosSessionId == pQosSession.qosSessionId) {
                    auto temp = m_QosSession.insert(it, pQosSession);
                    m_QosSession.erase(temp + 1);
                    break;
                }
            }
            break;
        case QOS_DELETED:
            for (auto it = m_QosSession.begin(); it != m_QosSession.end(); it++) {
                if (it->qosSessionId == pQosSession.qosSessionId) {
                    m_QosSession.erase(it);
                    m_QosSessionCnt--;
                    break;
                }
            }
            break;
        case QOS_UNDEFINED:
            RilLogI("QOS_UNDEFINED");
            break;
        default:
            break;
    }

    return 0;
}

void PdpContext::SetQosSession() {
    int size = m_QosSession.size();
    m_dataCall.QosSession.clear();
    m_dataCall.QosSessionCnt = m_QosSessionCnt;
    if (size != 0) {
        m_dataCall.QosSession.resize(size);
        copy(m_QosSession.begin(), m_QosSession.end(), m_dataCall.QosSession.begin());
    }
}

bool PdpContext::isConnected() {
    char nullIp[MAX_IPV6_ADDR_LEN] = {0, };

    if (m_state == PDP_CONTEXT_CONNECTED && m_dataCall.active == INACTIVE) {
        RilLogW("pdp state mismatch is detected cid:%d", m_cid);
        OnDeactivated();
    } else {
        if ((m_state == PDP_CONTEXT_CONNECTED && m_dataCall.active != INACTIVE)) {
            if ((m_dataCall.ipv4.valid && !memcmp(nullIp, m_dataCall.ipv4.addr, MAX_IPV4_ADDR_LEN) ) ||
                (m_dataCall.ipv6.valid && !memcmp(nullIp, m_dataCall.ipv6.addr, MAX_IPV6_ADDR_LEN) )) {
                return true;
            }
            RilLogW("cid:%d address is invalid(%d,%d), discard context",
                    m_cid, m_dataCall.ipv4.valid, m_dataCall.ipv6.valid);
            PsUtil::PrintAddressInfo(&m_dataCall);
            OnDeactivated();
        }
    }

    return false;
}

string PdpContext::GetStateToString() const
{
    switch(GetState()) {
        case PDP_CONTEXT_UNAVAILABLE:
            return "UNAVAILABLE";
        case PDP_CONTEXT_DISCONNECTED:
            return "DISCONNECTED";
        case PDP_CONTEXT_CONNECTING:
            return "CONNECTING";
        case PDP_CONTEXT_CONNECTED:
            return "CONNECTED";
        case PDP_CONTEXT_DISCONNECTING:
            return "DISCONNECTING";
        case PDP_CONTEXT_IPV6_CONFIGURING:
            return "IPV6_CONFIGURING";
        case PDP_CONTEXT_STATE_MAX:
            return "STATE_MAX";
        default:
            return "";
    }
}

string PdpContext::GetActiveToString() const
{
    switch(GetActive()) {
        case INACTIVE:
            return "INACTIVE";
        case ACTIVE:
            return "ACTIVE";
        // 'ACTIVE' and 'ACTIVE_AND_LINKDOWN' both equal '1'
        //case ACTIVE_AND_LINKDOWN:
        //    return "ACTIVE_AND_LINKDOWN";
        case ACTIVE_AND_LINKUP:
            return "ACTIVE_AND_LINKUP";
        case ACTIVE_STATE_MAX:
            return "ACTIVE_STATE_MAX";
        default:
            return "";
    }
}

string PdpContext::ToString() const
{
    stringstream ss;
    ss << "PdpContext{";
    ss << "cid=";
    ss << GetCID();
    ss << ", state=";
    ss << GetStateToString();
    ss << ", active=";
    ss << GetActiveToString();
    ss << "}";
    return ss.str();
}

