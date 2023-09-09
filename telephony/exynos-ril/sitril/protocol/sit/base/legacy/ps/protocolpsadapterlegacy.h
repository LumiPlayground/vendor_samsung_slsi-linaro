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
 * protocolpsadapterlegacy.h
 *
 *  Created on: 2014. 6. 25.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_PS_ADAPTER_LEGACY_H__
#define __PROTOCOL_PS_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolpsadapterinterface.h"
#include "protocolpsutil.h"

static UINT dcsize_tbl[6] =
    { LEN_DC_V10, LEN_DC_V11, LEN_DC_V11_PCSCF_EXT, LEN_DC_V11_LP_RETRY, LEN_DC_V11_AMBR, LEN_DC_V11_DEFAULT_QOS_PARAM};

class DataCallUtil
{
public:
    void FillDataCallListFromModemPayload_ForTD(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const unsigned int m_dataCallNum,
                                          const sit_pdp_data_call_item *data, int DCsizes);
    void FillDataCallListFromModemPayload(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const unsigned int m_dataCallNum,
                                          const sit_pdp_data_call_item *data, dc_type dct);
    int stripDataCallFromModemData(const ModemData *m_pModemData, const int rcm_type, const int rcm_id, sit_pdp_data_call_item *(&data));
    dc_type DetermineDCTypeBySize(int num, const int length);
    int processDataCallList(int dataCallNum, const sit_pdp_data_call_item *data, const int payloadLength, DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE]);
    int GetDataCallFromList(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const int blocksize,
                                          const sit_pdp_data_call_item *orgdata, sit_pdp_data_call_item *(&pdata), int idx);
    bool checkValidTD(const sit_pdp_data_call_item *orgdata, unsigned int remainingsize);
};

/**
 * ProtocolPsSetupDataCallAdapterLegacy
 */
class ProtocolPsSetupDataCallAdapterLegacy
    : public ProtocolRespAdapter, DataCallUtil, public ProtocolPsSetupDataCallAdapterInterface{
private:
    DataCall m_dataCall;

public:
    ProtocolPsSetupDataCallAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsSetupDataCallAdapterLegacy() = default;

protected:
    void Init();

public:
    UINT GetErrorCode() const;
    int GetStatus() const { return m_dataCall.status; }
    int GetCid() const { return m_dataCall.cid; }
    int GetActiveStatus() const { return m_dataCall.active; }
    int GetPdpType() const { return m_dataCall.pdpType; }
    int GetAddrInfo(PDP_ADDR *pAddr, int pdpType) const;
    int GetAddrInfo(PDP_ADDR *pAddr) const;
    //int GetMTU() const { return m_dataCall.mtu_size; }
    int GetMTUv4() const { return m_dataCall.ipv4_mtu_size; }
    int GetMTUv6() const { return m_dataCall.ipv6_mtu_size; }
    int GetPCO() const { return m_dataCall.pco; }
    int64_t GetSuggestedRetryTime() const { return m_dataCall.suggestedRetryTime; }
    const DataCall *GetDataCall() const { return &m_dataCall; }
};

/**
 * ProtocolPsDataCallListAdapterLegacy
 */
class ProtocolPsDataCallListAdapterLegacy
    : public ProtocolRespAdapter, DataCallUtil, public ProtocolPsDataCallListAdapterInterface{
private:
    DataCall m_dataCallList[MAX_DATA_CALL_SIZE];
    unsigned int m_dataCallNum;

public:
    ProtocolPsDataCallListAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsDataCallListAdapterLegacy() = default;

protected:
    void Init();
public:
    unsigned int GetDataCallNum() const { return m_dataCallNum; }
    const DataCall *GetDataCallList() const;
    const DataCall *GetDataCallByIndex(unsigned int index) const;
    const DataCall *GetDataCallByCid(int cid) const;
    UINT GetErrorCode() const { return ProtocolRespAdapter::GetErrorCode(); }
};

/**
 * ProtocolPsDataCallListChangedAdapterLegacy
 */
class ProtocolPsDataCallListChangedAdapterLegacy
    : public ProtocolIndAdapter, DataCallUtil, public ProtocolPsDataCallListChangedAdapterInterface{
private:
    DataCall m_dataCallList[MAX_DATA_CALL_SIZE];
    unsigned int m_dataCallNum;

public:
    ProtocolPsDataCallListChangedAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsDataCallListChangedAdapterLegacy() = default;

protected:
    void Init();
public:
    unsigned int GetDataCallNum() const { return m_dataCallNum; }
    const DataCall *GetDataCallList() const;
    const DataCall *GetDataCallByIndex(unsigned int index) const;
    const DataCall *GetDataCallByCid(int cid) const;
    UINT GetErrorCode() const;
};

/**
 * ProtocolPsDedicatedBearInfoAdapterLegacy
 */
class ProtocolPsDedicatedBearInfoAdapterLegacy
    : public ProtocolIndAdapter, public ProtocolPsDedicatedBearInfoAdapterInterface{
private:
    int m_cid;
    int m_status;
    QosSession_t m_QosSession;

public:
    ProtocolPsDedicatedBearInfoAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsDedicatedBearInfoAdapterLegacy() = default;

protected:
    void Init();
    void processQosSession(const ModemData *m_pModemData);
public:
    const DedicatedBearerInfo *GetDedicatedBearerInfo() { return NULL; }
    int GetCid() const { return m_cid; }
    int GetStatus() const { return m_status; }
    const QosSession_t& GetQosSessionInfo() const { return m_QosSession; }
};

/**
 * ProtocolPsNasTimerStatusAdapterLegacy
 */
class ProtocolPsNasTimerStatusAdapterLegacy
    : public ProtocolIndAdapter, public ProtocolPsNasTimerStatusAdapterInterface{

public:
    ProtocolPsNasTimerStatusAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsNasTimerStatusAdapterLegacy() = default;

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
 * ProtocolPsStartKeepAliveAdapterLegacy
 */
class ProtocolPsStartKeepAliveAdapterLegacy
    : public ProtocolRespAdapter, public ProtocolPsStartKeepAliveAdapterInterface{

public:
    ProtocolPsStartKeepAliveAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsStartKeepAliveAdapterLegacy() = default;

public:
    unsigned int getSessionHandle() const;
    int getCode() const;
    UINT GetErrorCode() const { return ProtocolRespAdapter::GetErrorCode(); }
};

/**
 * ProtocolPsKeepAliveStatusAdapterLegacy
 */
class ProtocolPsKeepAliveStatusAdapterLegacy
    : public ProtocolIndAdapter, public ProtocolPsKeepAliveStatusAdapterInterface{

public:
    ProtocolPsKeepAliveStatusAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsKeepAliveStatusAdapterLegacy() = default;

public:
    unsigned int getSessionHandle() const;
    int getCode() const;
};

/**
 * ProtocolPsPcoDataAdapterLegacy
 */
class ProtocolPsPcoDataAdapterLegacy
    : public ProtocolIndAdapter, public ProtocolPsPcoDataAdapterInterface{

public:
    ProtocolPsPcoDataAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsPcoDataAdapterLegacy() = default;

    int remainPcoBlocks;
    int nextPcoBlockPos;
public:
    // Per CID and SIT_IND
    int GetCid() const;
    int GetPdpType() const;
    int GetPcoNum() const;

    // Per PCO Block
    int GetPcoData(sit_pdp_pco_data_entry &);
};

/**
 * ProtocolPsDataOffStatusAdapterLegacy
 */
class ProtocolPsDataOffStatusAdapterLegacy
    : public ProtocolIndAdapter, public ProtocolPsDataOffStatusAdapterInterface {
private:
    int psDataOffAvailableStatus;
    int exemptListForHome;
    int exemptListForRoam;

public:
    ProtocolPsDataOffStatusAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsDataOffStatusAdapterLegacy() = default;

public:
    int GetPsDataOffAvailableStatus() const;
    int GetExemptListForHome() const;
    int GetExemptListForRoam() const;
};

/**
 * ProtocolPsIpAddrStatusAdapterLegacy
 */
class ProtocolPsIpAddrStatusAdapterLegacy
    : public ProtocolIndAdapter, public ProtocolPsIpAddrStatusAdapterInterface{
private:
    int cid;
    int status;
    BYTE ipv4_addr[MAX_IPV4_ADDR_LEN];
    BYTE ipv6_addr[MAX_IPV6_ADDR_LEN];
protected:
    void Init();

public:
    ProtocolPsIpAddrStatusAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsIpAddrStatusAdapterLegacy() = default;

public:
    int GetCid() const;
    int GetStatus() const;
    void GetAddrInfo(char (&)[MAX_IPV4_ADDR_LEN], char (&)[MAX_IPV6_ADDR_LEN]) const;
};

/**
 * ProtocolPsUnthrottleApnLegacy
 */
class ProtocolPsUnthrottleApnLegacy
    : public ProtocolIndAdapter, public ProtocolPsUnthrottleApnInterface{
private:
    char m_apn[MAX_PDP_APN_LEN];

public:
    ProtocolPsUnthrottleApnLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsUnthrottleApnLegacy() = default;

public:
    const char *GetApn() const;
};

/**
 * ProtocolPsAllocatePduSessionIdLegacy
 */
class ProtocolPsAllocatePduSessionIdLegacy
    : public ProtocolRespAdapter, public ProtocolPsAllocatePduSessionIdInterface{
private:
    int m_sessionId;

public:
    ProtocolPsAllocatePduSessionIdLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsAllocatePduSessionIdLegacy() = default;

public:
    int GetSessionId() const;
    UINT GetErrorCode() const { return ProtocolRespAdapter::GetErrorCode(); }
};

/**
 * SlicingConfigUtil
 */
class SlicingConfigUtil {
private:
    const static int MAX_OSID_SIZE = 16;
    const static int MAX_OSAPPID_SIZE = 255;
protected:
    int m_payload_size = 0;
    int m_ipcVersion = 0;
    DataV2_0::RIL_SlicingConfig m_slicingConfig;
    enum FLAG_FOR_SLICING_CONFIG_VERSION {
        UNUSED_IPC = 0,
        VALID_SLICINGCONFIG = 1,
        EXTENDED_SLICINGCONFIG = 2,
    };

public:
    SlicingConfigUtil();
    virtual ~SlicingConfigUtil();
    void SetPayloadSize(const int size);
    int GetPayloadSize() const { return m_payload_size; }
    virtual DataV2_0::RIL_SlicingConfig GetSlicingConfig() const {return m_slicingConfig;}

    void processSlicingConfig(sit_pdp_slicing_config_item *slicing_config);

    void clearRouteSelectionDescriptor(RIL_RouteSelectionDescriptor &rsd);
    void fillRouteSelectionDescriptor(DataV2_0::RIL_SlicingConfig &out, sit_pdp_slicing_config_item *config_data, int &payload_size, DataV2_0::RIL_UrspRule &urspRule);
    void fillURSPRuleParamLegacy(DataV2_0::RIL_SlicingConfig &out, sit_pdp_slicing_config_item*config_data, int & payload_size);
    void fillDnnIntoTdLegacy(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillOsAppIdIntoTdLegacy(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillDnnIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillOsAppIdIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillIpDescriptorsIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillRemoteAddrV4IntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillRemoteAddrV6IntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillProtocolIdNextHeaderIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillSinglePortIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillRemotePortRangeIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillIp3TupleIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillTosTcIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillConnectionCapabilitiesIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillDomainDescriptorIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillURSPRuleWithTD(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_data);
    void fillURSPRuleMultiParam(DataV2_0::RIL_SlicingConfig &out, sit_pdp_slicing_config_item*config_data, int & payload_size);
    std::vector<RIL_SliceInfo> getSliceInfoParam(char *modemData, int & payload_size);
    RIL_OptionalDNN getDnnInfoParam(char *modemData, int & payload_size);
    std::string getDnnInfoParam2(char *modemData, int &payload_size);
    void fillLocationCriteriaTypeInfo(RIL_RouteSelectionDescriptor &rsdout, char *modemData, int &payload_size);

    template<typename T>
        void readField(T &ret, char *sp, int &payload_size);
    template<typename T>
        int copyVariableData(T *ret, char *sp, int &payload_size);
    template<typename T>
        uint16_t copyVariableMultiData(std::vector<T> &ret, char *sp, int &payload_size, bool includeLen);
    int copyVariableString(std::string &ret, char *sp, int &payload_size);
    template<typename T>
        uint16_t copyVariableMultiString(std::vector<T> &ret, char *sp, int &payload_size);
    uint16_t copyVariableMultiString(std::vector<std::string> &ret, char *sp, int &payload_size);
    template<typename T>
        void copyCastedVariableData(char *sp, int &payload_size, T &var, RIL_OptionalInfoFlag &flag);
    template<typename T>
        void copyCastedVariableMultiData(char *sp, int &payload_size, std::vector<T> &var);
    template<typename T>
        void copyByteArrayData(char *sp, int &payload_size, T *var, RIL_OptionalInfoFlag &flag);
    void convertFQDN(char (&fqdn)[512]);
};

/**
 * ProtocolPsSlicingConfigLegacy
 */
class ProtocolPsSlicingConfigLegacy : public ProtocolRespAdapter,
    public SlicingConfigUtil, public ProtocolPsSlicingConfigInterface {
public:
    ProtocolPsSlicingConfigLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsSlicingConfigLegacy() = default;
    DataV2_0::RIL_SlicingConfig GetSlicingConfig() const;
    int GetPayloadSize() const;
    UINT GetErrorCode() const;
};

class ProtocolPsSlicingConfigUpdatedLegacy : public ProtocolIndAdapter,
    public SlicingConfigUtil, public ProtocolPsSlicingConfigUpdatedInterface {
public:
    ProtocolPsSlicingConfigUpdatedLegacy(const ModemData *pModemData);
    virtual ~ProtocolPsSlicingConfigUpdatedLegacy() = default;
    DataV2_0::RIL_SlicingConfig GetSlicingConfig() const;
    int GetPayloadSize() const;
    UINT GetErrorCode() const;
};
#endif /* __PROTOCOL_PS_ADAPTER_LEGACY_H__ */

