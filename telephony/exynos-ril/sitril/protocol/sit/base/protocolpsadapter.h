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
 * protocolpsadapter.h
 *
 *  Created on: 2021. 10. 18.
 *      Author: juneho.kwon
 */

#ifndef __PROTOCOL_PS_ADAPTER_H__
#define __PROTOCOL_PS_ADAPTER_H__

#include "protocolpsadapterinterface.h"

class ModemData;

/**
 * ProtocolPsSetupDataCallAdapter
 */
class ProtocolPsSetupDataCallAdapter : public ProtocolPsSetupDataCallAdapterInterface {
private:
    ProtocolPsSetupDataCallAdapterInterface *protocolPsSetupDataCallAdapterInterface = NULL;

public:
    ProtocolPsSetupDataCallAdapter(const ModemData *pModemData);
    ProtocolPsSetupDataCallAdapter(const ProtocolPsSetupDataCallAdapter&) = delete;
    ProtocolPsSetupDataCallAdapter& operator=(const ProtocolPsSetupDataCallAdapter&) = delete;
    virtual ~ProtocolPsSetupDataCallAdapter();

public:
    int GetStatus() const;
    int GetCid() const;
    int GetActiveStatus() const;
    int GetPdpType() const;
    int GetAddrInfo(PDP_ADDR *pAddr, int pdpType) const;
    int GetAddrInfo(PDP_ADDR *pAddr) const;
    //int GetMTU() const { return m_dataCall.mtu_size; }
    int GetMTUv4() const;
    int GetMTUv6() const;
    int GetPCO() const;
    int64_t GetSuggestedRetryTime() const;
    const DataCall *GetDataCall() const;
    UINT GetErrorCode() const { return protocolPsSetupDataCallAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolPsDataCallListAdapter
 */
class ProtocolPsDataCallListAdapter : public ProtocolPsDataCallListAdapterInterface {
private:
    ProtocolPsDataCallListAdapterInterface *protocolPsDataCallListAdapterInterface = NULL;

public:
    ProtocolPsDataCallListAdapter(const ModemData *pModemData);
    ProtocolPsDataCallListAdapter(const ProtocolPsDataCallListAdapter&) = delete;
    ProtocolPsDataCallListAdapter& operator=(const ProtocolPsDataCallListAdapter&) = delete;
    virtual ~ProtocolPsDataCallListAdapter();

public:
    unsigned int GetDataCallNum() const;
    const DataCall *GetDataCallList() const;
    const DataCall *GetDataCallByIndex(unsigned int index) const;
    const DataCall *GetDataCallByCid(int cid) const;
    UINT GetErrorCode() const { return protocolPsDataCallListAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolPsDataCallListChangedAdapter
 */
class ProtocolPsDataCallListChangedAdapter : public ProtocolPsDataCallListChangedAdapterInterface {
private:
    ProtocolPsDataCallListChangedAdapterInterface *protocolPsDataCallListChangedAdapterInterface = NULL;

public:
    ProtocolPsDataCallListChangedAdapter(const ModemData *pModemData);
    ProtocolPsDataCallListChangedAdapter(const ProtocolPsDataCallListChangedAdapter&) = delete;
    ProtocolPsDataCallListChangedAdapter& operator=(const ProtocolPsDataCallListChangedAdapter&) = delete;
    virtual ~ProtocolPsDataCallListChangedAdapter();

public:
    unsigned int GetDataCallNum() const;
    const DataCall *GetDataCallList() const;
    const DataCall *GetDataCallByIndex(unsigned int index) const;
    const DataCall *GetDataCallByCid(int cid) const;
    UINT GetErrorCode() const { return protocolPsDataCallListChangedAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolPsDedicatedBearInfoAdapter
 */
class ProtocolPsDedicatedBearInfoAdapter : public ProtocolPsDedicatedBearInfoAdapterInterface {
private:
    ProtocolPsDedicatedBearInfoAdapterInterface *protocolPsDedicatedBearInfoAdapterInterface = NULL;

public:
    ProtocolPsDedicatedBearInfoAdapter(const ModemData *pModemData);
    ProtocolPsDedicatedBearInfoAdapter(const ProtocolPsDedicatedBearInfoAdapter&) = delete;
    ProtocolPsDedicatedBearInfoAdapter& operator=(const ProtocolPsDedicatedBearInfoAdapter&) = delete;
    virtual ~ProtocolPsDedicatedBearInfoAdapter();

public:
    const DedicatedBearerInfo *GetDedicatedBearerInfo();
    int GetCid() const;
    int GetStatus() const;
    const QosSession_t &GetQosSessionInfo() const;
};

/**
 * ProtocolPsNasTimerStatusAdapter
 */
class ProtocolPsNasTimerStatusAdapter : public ProtocolPsNasTimerStatusAdapterInterface {
private:
    ProtocolPsNasTimerStatusAdapterInterface *protocolPsNasTimerStatusAdapterInterface = NULL;

public:
    ProtocolPsNasTimerStatusAdapter(const ModemData *pModemData);
    ProtocolPsNasTimerStatusAdapter(const ProtocolPsNasTimerStatusAdapter&) = delete;
    ProtocolPsNasTimerStatusAdapter& operator=(const ProtocolPsNasTimerStatusAdapter&) = delete;
    virtual ~ProtocolPsNasTimerStatusAdapter();

public:
    const SitNasTimerStatus *GetNasTimerStatus();
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolPsStartKeepAliveAdapter
 */
class ProtocolPsStartKeepAliveAdapter : public ProtocolPsStartKeepAliveAdapterInterface {
private:
    ProtocolPsStartKeepAliveAdapterInterface *protocolPsStartKeepAliveAdapterInterface = NULL;

public:
    ProtocolPsStartKeepAliveAdapter(const ModemData *pModemData);
    ProtocolPsStartKeepAliveAdapter(const ProtocolPsStartKeepAliveAdapter&) = delete;
    ProtocolPsStartKeepAliveAdapter& operator=(const ProtocolPsStartKeepAliveAdapter&) = delete;
    virtual ~ProtocolPsStartKeepAliveAdapter();

public:
    unsigned int getSessionHandle() const;
    int getCode() const;
    UINT GetErrorCode() const { return protocolPsStartKeepAliveAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolPsKeepAliveStatusAdapter
 */
class ProtocolPsKeepAliveStatusAdapter : public ProtocolPsKeepAliveStatusAdapterInterface {
private:
    ProtocolPsKeepAliveStatusAdapterInterface *protocolPsKeepAliveStatusAdapterInterface = NULL;

public:
    ProtocolPsKeepAliveStatusAdapter(const ModemData *pModemData);
    ProtocolPsKeepAliveStatusAdapter(const ProtocolPsKeepAliveStatusAdapter&) = delete;
    ProtocolPsKeepAliveStatusAdapter& operator=(const ProtocolPsKeepAliveStatusAdapter&) = delete;
    virtual ~ProtocolPsKeepAliveStatusAdapter();

public:
    unsigned int getSessionHandle() const;
    int getCode() const;
};

/**
 * ProtocolPsPcoDataAdapter
 */
class ProtocolPsPcoDataAdapter : public ProtocolPsPcoDataAdapterInterface {
private:
    ProtocolPsPcoDataAdapterInterface *protocolPsPcoDataAdapterInterface = NULL;

public:
    ProtocolPsPcoDataAdapter(const ModemData *pModemData);
    ProtocolPsPcoDataAdapter(const ProtocolPsPcoDataAdapter&) = delete;
    ProtocolPsPcoDataAdapter& operator=(const ProtocolPsPcoDataAdapter&) = delete;
    virtual ~ProtocolPsPcoDataAdapter();

public:
    // Per CID and SIT_IND
    int GetCid() const;
    int GetPdpType() const;
    int GetPcoNum() const;

    // Per PCO Block
    int GetPcoData(sit_pdp_pco_data_entry &);
};

/**
 * ProtocolPsDataOffStatusAdapter
 */
class ProtocolPsDataOffStatusAdapter : public ProtocolPsDataOffStatusAdapterInterface {
private:
    unique_ptr<ProtocolPsDataOffStatusAdapterInterface> protocolPsDataOffStatusAdapterInterface;

public:
    ProtocolPsDataOffStatusAdapter(const ModemData *pModemData);
    virtual ~ProtocolPsDataOffStatusAdapter();

public:
    int GetPsDataOffAvailableStatus() const;
    int GetExemptListForHome() const;
    int GetExemptListForRoam() const;
};

/**
 * ProtocolPsIpAddrStatusAdapter
 */
class ProtocolPsIpAddrStatusAdapter : public ProtocolPsIpAddrStatusAdapterInterface {
private:
    ProtocolPsIpAddrStatusAdapterInterface *protocolPsIpAddrStatusAdapterInterface = NULL;

public:
    ProtocolPsIpAddrStatusAdapter(const ModemData *pModemData);
    ProtocolPsIpAddrStatusAdapter(const ProtocolPsIpAddrStatusAdapter&) = delete;
    ProtocolPsIpAddrStatusAdapter& operator=(const ProtocolPsIpAddrStatusAdapter&) = delete;
    virtual ~ProtocolPsIpAddrStatusAdapter();

public:
    int GetCid() const;
    int GetStatus() const;
    void GetAddrInfo(char (&)[MAX_IPV4_ADDR_LEN], char (&)[MAX_IPV6_ADDR_LEN]) const;
};

/**
 * ProtocolPsUnthrottleApn
 */
class ProtocolPsUnthrottleApn : public ProtocolPsUnthrottleApnInterface {
private:
    ProtocolPsUnthrottleApnInterface *protocolPsUnthrottleApnInterface = NULL;

public:
    ProtocolPsUnthrottleApn(const ModemData *pModemData);
    ProtocolPsUnthrottleApn(const ProtocolPsUnthrottleApn&) = delete;
    ProtocolPsUnthrottleApn& operator=(const ProtocolPsUnthrottleApn&) = delete;
    virtual ~ProtocolPsUnthrottleApn();

public:
    const char *GetApn() const;
};

/**
 * ProtocolPsAllocatePduSessionId
 */
class ProtocolPsAllocatePduSessionId : public ProtocolPsAllocatePduSessionIdInterface {
private:
    ProtocolPsAllocatePduSessionIdInterface *protocolPsAllocatePduSessionIdInterface = NULL;

public:
    ProtocolPsAllocatePduSessionId(const ModemData *pModemData);
    ProtocolPsAllocatePduSessionId(const ProtocolPsAllocatePduSessionId&) = delete;
    ProtocolPsAllocatePduSessionId& operator=(const ProtocolPsAllocatePduSessionId&) = delete;
    virtual ~ProtocolPsAllocatePduSessionId();

public:
    int GetSessionId() const;
    UINT GetErrorCode() const { return protocolPsAllocatePduSessionIdInterface->GetErrorCode(); }
};

/**
 * ProtocolPsSlicingConfig
 */
class ProtocolPsSlicingConfig : public ProtocolPsSlicingConfigInterface {
private:
    ProtocolPsSlicingConfigInterface *protocolPsSlicingConfigInterface = NULL;

public:
    ProtocolPsSlicingConfig(const ModemData *pModemData);
    ProtocolPsSlicingConfig(const ProtocolPsSlicingConfig&) = delete;
    ProtocolPsSlicingConfig& operator=(const ProtocolPsSlicingConfig&) = delete;
    virtual ~ProtocolPsSlicingConfig();

public:
    DataV2_0::RIL_SlicingConfig GetSlicingConfig() const;
    int GetPayloadSize() const;
    UINT GetErrorCode() const { return protocolPsSlicingConfigInterface->GetErrorCode(); }
};

/**
 * ProtocolPsSlicingConfigUpdated
 */
class ProtocolPsSlicingConfigUpdated : public ProtocolPsSlicingConfigUpdatedInterface {
private:
    ProtocolPsSlicingConfigUpdatedInterface *protocolPsSlicingConfigUpdatedInterface = NULL;

public:
    ProtocolPsSlicingConfigUpdated(const ModemData *pModemData);
    ProtocolPsSlicingConfigUpdated(const ProtocolPsSlicingConfigUpdated&) = delete;
    ProtocolPsSlicingConfigUpdated& operator=(const ProtocolPsSlicingConfigUpdated&) = delete;
    virtual ~ProtocolPsSlicingConfigUpdated();

public:
    DataV2_0::RIL_SlicingConfig GetSlicingConfig() const;
    int GetPayloadSize() const;
    UINT GetErrorCode() const { return protocolPsSlicingConfigUpdatedInterface->GetErrorCode(); }
};

#endif /* __PROTOCOL_PS_ADAPTER_H__ */

