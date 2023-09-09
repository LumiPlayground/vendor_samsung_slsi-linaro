/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef    __PDP_CONTEXT_H__
#define    __PDP_CONTEXT_H__

#include "rildef.h"
#include "apnsetting.h"
#include "DeprecatedAddress.h"

enum {
    PDP_CONTEXT_UNAVAILABLE,
    PDP_CONTEXT_DISCONNECTED,
    PDP_CONTEXT_CONNECTING,
    PDP_CONTEXT_CONNECTED,
    PDP_CONTEXT_DISCONNECTING,
    PDP_CONTEXT_IPV6_CONFIGURING,
    PDP_CONTEXT_STATE_MAX,
};

enum PDP_ACTIVE_STATUS {
    INACTIVE = 0,  // Same to CP's Inactive (SIT_PDP_ACTIVE_INACTIVE) and Network Interface(rmnet) down
    ACTIVE = 1,
    ACTIVE_AND_LINKDOWN = 1,  // Same to DORMANT, NOT Network interface(rmnet) down
    ACTIVE_AND_LINKUP = 2,    // Same to CP's Active (SIT_PDP_ACTIVE_ACTIVE) and Network Interface(rmnet) up
    ACTIVE_STATE_MAX,
};

/* Remove duplicate definition
// Instead use SIT active definition
enum {
    CP_DATA_CONNECTION_INACTIVE = 0,
    CP_DATA_CONNECTION_ACTIVE = 1,
    CP_DATA_CONNECTION_DORMANT = 2,
};

// Instead, use above RadioHAL aligned active definition
enum {
    DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE = 0,
    DATA_CONNECTION_ACTIVE_PH_LINK_DORMANT = 1,
    DATA_CONNECTION_ACTIVE_PH_LINK_UP = 2,
};
*/

enum {
    QOS_UNDEFINED = 0,
    QOS_ADDED = 1,
    QOS_MODIFIED = 2,
    QOS_DELETED = 3,
};

class NetIfController;

class PdpContext {
protected:
    int m_cid;
    char m_szIfname[20+1];
    ApnSetting *m_pApnSetting;
    std::optional<DataV2_0::RIL_TrafficDescriptor> mTdInfo;
    int m_dataProfileId;
    int m_state;
    // data Call will hold only primary address
    DataCall m_dataCall;
    vector<DeprecatedAddress> secondaryAddresses;
    NetIfController *m_pNetIfController;
    string m_sHandoverDnses;
    vector<QosSession_t> m_QosSession;
    int m_QosSessionCnt;

public:
    PdpContext(int cid);
    PdpContext(int cid, const char *ifprefix, int ifindex);
    PdpContext(const PdpContext&) = delete;
    PdpContext& operator=(PdpContext const&) = delete;
    ~PdpContext();

public:
    void Init();
    void InitDataCall();
    void SetApnSetting(ApnSetting *pApnSetting);
    void SetTdSetting(const std::optional<DataV2_0::RIL_TrafficDescriptor> &tdInfo);
    ApnSetting *GetApnSetting() { return m_pApnSetting; }
    std::optional<DataV2_0::RIL_TrafficDescriptor> &GetTdSetting() { return mTdInfo; }
    int GetCID() const { return m_cid; }
    int GetState() const { return m_state; }
    void SetState(int state);
    void SetActive(int active);
    int GetActive() const { return m_dataCall.active; }
    bool IsAvailable() const;
    void SetDataProfileId(int dataProfileId) { m_dataProfileId = dataProfileId; }
    int GetDataProfileId() const { return m_dataProfileId; }
    int SetAddr(const DataCall *dc);
    DataCall *GetDataCallInfo() { return &m_dataCall;}
    bool UpdateDataCallInfo(const DataCall *dc);
    const char *GetInterfaceName() const { return m_szIfname; }
    void SetHandoverDnses(string dnses);
    bool checkIpv4AddressChanged(const DataCall& old_dc, const DataCall& new_dc);
    bool checkIpv6AddressChanged(const DataCall& old_dc, const DataCall& new_dc);
    bool isConnected();

    string GetStateToString() const;
    string GetActiveToString() const;
    string ToString() const;
public:
    int OnChanged(const DataCall *dc);
    int OnUpdated(int state, char (&)[MAX_IPV4_ADDR_LEN], char (&)[MAX_IPV6_ADDR_LEN]);
    int OnActivated(const DataCall *dc);
    int OnActivated();
    int OnDeactivated();
    int OnUpdateQosInfo(int status, const QosSession_t& pQosSession);
    void SetQosSession();
    void Reset();

private:
    void DumpIpAddress(int family, char *);
    void DumpIpAddress(char *, char *);
    bool isMatchedIp(bool isIpv6, char (&ipv4)[MAX_IPV4_ADDR_LEN], char (&ipv6)[MAX_IPV6_ADDR_LEN], const char *addr);
};

#endif // __PDP_CONTEXT_H__
