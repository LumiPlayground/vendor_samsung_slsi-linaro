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
 * protocolpsadapterjson.h
 *
 *  Created on: 2021. 10. 20.
 *      Author: juneho.kwon
 */

#ifndef __PROTOCOL_PS_ADAPTER_JSON_H__
#define __PROTOCOL_PS_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolpsadapterinterface.h"
#include "protocolpsutil.h"

class JsonDataCallUtil
{
public:
    bool checkValidTD(Json::Value &jsonData);
    dc_type DetermineDCType(int num, Json::Value &jsonData);
    int processDataCallList(Json::Value &jsonData, DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE]);
    void FillDataCallListFromModemPayload_ForTD(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const unsigned int m_dataCallNum,
                                          Json::Value &jsonData);
    void FillDataCallListFromModemPayload(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const unsigned int m_dataCallNum,
                                          Json::Value &jsonData, dc_type dct);
    int GetDataCallFromList(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], Json::Value &jsonDataCall, int idx);
 };

/**
 * ProtocolPsSetupDataCallAdapterjson
 */
class ProtocolPsSetupDataCallAdapterjson
    : public ProtocolJsonRespAdapter, JsonDataCallUtil, public ProtocolPsSetupDataCallAdapterInterface{

private:
    void Init();

public:
    ProtocolPsSetupDataCallAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsSetupDataCallAdapterjson() = default;

private:
    DataCall m_dataCall;

public:
    UINT GetErrorCode() const;
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
};

/**
 * ProtocolPsDataCallListAdapterjson
 */
class ProtocolPsDataCallListAdapterjson
    : public ProtocolJsonRespAdapter, JsonDataCallUtil, public ProtocolPsDataCallListAdapterInterface{

private:
    void Init();

public:
    ProtocolPsDataCallListAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsDataCallListAdapterjson() = default;

private:
    DataCall m_dataCallList[MAX_DATA_CALL_SIZE];
    unsigned int m_dataCallNum;

public:
    unsigned int GetDataCallNum() const;
    const DataCall *GetDataCallList() const;
    const DataCall *GetDataCallByIndex(unsigned int index) const;
    const DataCall *GetDataCallByCid(int cid) const;
    UINT GetErrorCode() const;
};

/**
 * ProtocolPsDataCallListChangedAdapterjson
 */
class ProtocolPsDataCallListChangedAdapterjson
    : public ProtocolJsonIndAdapter, JsonDataCallUtil, public ProtocolPsDataCallListChangedAdapterInterface{

private:
    void Init();

public:
    ProtocolPsDataCallListChangedAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsDataCallListChangedAdapterjson() = default;

private:
    DataCall m_dataCallList[MAX_DATA_CALL_SIZE];
    unsigned int m_dataCallNum;

public:
    unsigned int GetDataCallNum() const;
    const DataCall *GetDataCallList() const;
    const DataCall *GetDataCallByIndex(unsigned int index) const;
    const DataCall *GetDataCallByCid(int cid) const;
    UINT GetErrorCode() const;
};

/**
 * ProtocolPsNasTimerStatusAdapterjson
 */
class ProtocolPsNasTimerStatusAdapterjson
    : public ProtocolJsonIndAdapter, public ProtocolPsNasTimerStatusAdapterInterface{
private:
    SitNasTimerStatus m_NasTimerStatus;

public:
    ProtocolPsNasTimerStatusAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsNasTimerStatusAdapterjson() = default;

public:
    const SitNasTimerStatus *GetNasTimerStatus();
    const char *GetParameter() const {
        return ProtocolIndAdapter::GetParameter();
    }
    UINT GetParameterLength() const {
        return ProtocolIndAdapter::GetParameterLength();
    }
};

/**
 * ProtocolPsStartKeepAliveAdapterjson
 */
class ProtocolPsStartKeepAliveAdapterjson : public ProtocolJsonRespAdapter, public ProtocolPsStartKeepAliveAdapterInterface {
public:
    ProtocolPsStartKeepAliveAdapterjson(const ModemData *pModemData) : ProtocolJsonRespAdapter(pModemData, SIT_START_KEEPALIVE) { }
    virtual ~ProtocolPsStartKeepAliveAdapterjson() = default;

public:
    unsigned int getSessionHandle() const;
    int getCode() const;
    //For ProtocolResAdatper
    UINT GetErrorCode() const { return ProtocolJsonRespAdapter::GetErrorCode(); }
};

/**
 * ProtocolPsKeepAliveStatusAdapterjson
 */
class ProtocolPsKeepAliveStatusAdapterjson : public ProtocolJsonIndAdapter, public ProtocolPsKeepAliveStatusAdapterInterface {
public:
    ProtocolPsKeepAliveStatusAdapterjson(const ModemData *pModemData) : ProtocolJsonIndAdapter(pModemData, SIT_IND_KEEPALIVE_STATUS) { }
    virtual ~ProtocolPsKeepAliveStatusAdapterjson()  = default;

public:
    unsigned int getSessionHandle() const;
    int getCode() const;
};

/**
 * ProtocolPsPcoDataAdapterjson
 */
class ProtocolPsPcoDataAdapterjson : public ProtocolJsonIndAdapter, public ProtocolPsPcoDataAdapterInterface{

public:
    ProtocolPsPcoDataAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsPcoDataAdapterjson() = default;

private:
    int remainPcoBlocks;
    int nextPcoBlockPos;
    char contents[255];

public:
    // Per CID and SIT_IND
    int GetCid() const;
    int GetPdpType() const;
    int GetPcoNum() const;

    // Per PCO Block
    int GetPcoData(sit_pdp_pco_data_entry &);
};

/**
 * ProtocolPsDataOffStatusAdapterjson
 */
class ProtocolPsDataOffStatusAdapterjson
    : public ProtocolJsonIndAdapter, public ProtocolPsDataOffStatusAdapterInterface {
private:
    int psDataOffAvailableStatus;
    int exemptListForHome;
    int exemptListForRoam;

public:
    ProtocolPsDataOffStatusAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsDataOffStatusAdapterjson() = default;

public:
    int GetPsDataOffAvailableStatus() const;
    int GetExemptListForHome() const;
    int GetExemptListForRoam() const;
};

/**
 * ProtocolPsIpAddrStatusAdapterjson
 */
class ProtocolPsIpAddrStatusAdapterjson
    : public ProtocolJsonIndAdapter, public ProtocolPsIpAddrStatusAdapterInterface{
private:
    int cid;
    int status;
    BYTE ipv4_addr[MAX_IPV4_ADDR_LEN];
    BYTE ipv6_addr[MAX_IPV6_ADDR_LEN];

private:
    void Init();

public:
    ProtocolPsIpAddrStatusAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsIpAddrStatusAdapterjson() = default;

public:
    int GetCid() const;
    int GetStatus() const;
    void GetAddrInfo(char (&)[MAX_IPV4_ADDR_LEN], char (&)[MAX_IPV6_ADDR_LEN]) const;
};

/**
 * ProtocolPsUnthrottleApnjson
 */
class ProtocolPsUnthrottleApnjson
    : public ProtocolJsonIndAdapter, public ProtocolPsUnthrottleApnInterface{
private:
    char m_apn[MAX_PDP_APN_LEN];

public:
    ProtocolPsUnthrottleApnjson(const ModemData *pModemData);
    virtual ~ProtocolPsUnthrottleApnjson() = default;

public:
    const char *GetApn() const;
};

/**
 * ProtocolPsAllocatePduSessionIdjson
 */
class ProtocolPsAllocatePduSessionIdjson
    : public ProtocolJsonRespAdapter, public ProtocolPsAllocatePduSessionIdInterface{
private:
    int m_sessionId;

public:
    ProtocolPsAllocatePduSessionIdjson(const ModemData *pModemData);
    virtual ~ProtocolPsAllocatePduSessionIdjson() = default;

public:
    int GetSessionId() const;
    UINT GetErrorCode() const;
};

/**
 * ProtocolPsDedicatedBearInfoAdapterjson
 */
class ProtocolPsDedicatedBearInfoAdapterjson
    : public ProtocolJsonIndAdapter, public ProtocolPsDedicatedBearInfoAdapterInterface{
private:
    int m_cid;
    int m_status;
    QosSession_t m_QosSession;

public:
    ProtocolPsDedicatedBearInfoAdapterjson(const ModemData *pModemData);
    virtual ~ProtocolPsDedicatedBearInfoAdapterjson() = default;

private:
    void Init();
    void processQosSession();
public:
    const DedicatedBearerInfo *GetDedicatedBearerInfo();
    int GetCid() const;
    int GetStatus() const;
    const QosSession_t &GetQosSessionInfo() const;
};

/**
 * ProtocolPsSlicingConfigjson
 */
class ProtocolPsSlicingConfigjson
    : public ProtocolJsonRespAdapter, public ProtocolPsSlicingConfigInterface{
private:
    DataV2_0::RIL_SlicingConfig m_slicingConfig;
    int m_ipcVersion;

    enum FLAG_FOR_SLICING_CONFIG_VERSION {
        INVALID      = 0,
        VALID        = 1,
    };

private:
    int GetPayloadSize() const;
    void SetPayloadSize(const int size);
    void processSlicingConfig();
    void fillURSPRuleParam(DataV2_0::RIL_SlicingConfig &out);
    std::vector<RIL_SliceInfo> getSliceInfoParam(int sliceNum, Json::Value &jsonData);
    std::vector<RIL_OptionalDNN> getDnnInfoParam(int dnnNum, Json::Value &jsonData);

public:
    ProtocolPsSlicingConfigjson(const ModemData *pModemData);
    virtual ~ProtocolPsSlicingConfigjson() = default;

public:
    DataV2_0::RIL_SlicingConfig GetSlicingConfig() const;
    UINT GetErrorCode() const;

};

#endif /* __PROTOCOL_PS_ADAPTER_JSON_H__ */
