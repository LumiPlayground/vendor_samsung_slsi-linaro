/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _DATA_CALL_REQ_DATA_H_
#define _DATA_CALL_REQ_DATA_H_

#include "requestdata.h"
#include <slsi/radio_v2_0.h>

class PdpContext;

/**
 * SetupDataCallRequestData
 * This will cover upto V2_0 definition
 */
class SetupDataCallRequestData : public StringsRequestData {
public:
    DataV2_0::RIL_DataProfileInfo mDataProfileInfo;
    std::string mDnses;
    std::vector<DataV2_0::RIL_LinkAddress> mLinkAddresses;
    int mSize;
    PdpContext* mActivatingPdpCtx;
    int mPduSessionId;
    std::optional<RIL_SliceInfo> mEpdgOptSliceInfo;
    std::optional<DataV2_0::RIL_TrafficDescriptor> mTrafficDescriptor;
    bool mMatchedAllFlag;
    bool mAlwaysOnPdu;
    /* Extended */
    std::optional<RIL_VerifyDescriptor> mVerifyDescriptor;

public:
    SetupDataCallRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    virtual ~SetupDataCallRequestData();
    virtual int encode(char *data, unsigned int datalen);
    virtual SetupDataCallRequestData *Clone() const;

public:
    int GetRadioTech() const;
    int GetDataProfileId() const;
    const char *GetApn() const;
    const char *GetUsername() const;
    const char *GetPassword() const;
    int GetAuthType() const;
    const char *GetProtocolByInt(int) const;
    const char *GetProtocol() const;
    const char *GetRoamingProtocol() const;
    int GetRadioAccessFamily() const;
    bool GetAPNSettingStatus() const;
    int GetMtuSize() const;
    int GetMtuV6Size() const;
    const char *IsDataRoamingAllowed() const;
    bool IsRoamingAllowed() const;
    int GetSupportedApnTypesBitmap() const;
    const char *GetReason() const;
    const char *GetAddresses() const;
    const char *GetDnses() const;
    void setActivatingPdpCtx (PdpContext* pActPdpCtx){mActivatingPdpCtx= pActPdpCtx;};
    PdpContext* getActivatingPdpCtx (void){return mActivatingPdpCtx;};
    int GetPduSessionId() const;

    const std::optional<RIL_SliceInfo> &getSliceInfo() { return mEpdgOptSliceInfo; };
    const std::optional<DataV2_0::RIL_TrafficDescriptor>& GetTdInfo() { return mTrafficDescriptor; };
    bool GetMatchedAllFlag() const;
    bool GetAlwaysOnPdu() const;
    void PrintDebugInfo();

protected:
    int encode(RIL_SetupDataCallInfo_V1_5 *dataCallInfo);
    int encode_aosp(RIL_SetupDataCallInfo_V1_6 *dataCallInfo);
    int encode(RIL_SetupDataCallInfo_V1_6 *dataCallInfo);
    int encode(DataV2_0::RIL_SetupDataCallInfo *dataCallInfo);
};

/**
 * DeactivateDataCallRequestData
 */
class DeactivateDataCallRequestData : public StringsRequestData {
public:
    DeactivateDataCallRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    virtual ~DeactivateDataCallRequestData();
    virtual int encode(char *data, unsigned int datalen);
    virtual DeactivateDataCallRequestData *Clone() const;

public:
    int GetCid() const;
    int GetDisconnectReason() const;
};

/**
 * SetInitialAttachApnRequestData
 */
class SetInitialAttachApnRequestData : public RequestData {
public:
    DataV2_0::RIL_DataProfileInfo mDataProfileInfo;

public:
    SetInitialAttachApnRequestData(const int nReq,const Token tok,const ReqType type = REQ_FW);
    virtual ~SetInitialAttachApnRequestData();
    INT32 encode(char *data, unsigned int datalen);
    virtual SetInitialAttachApnRequestData *Clone() const;
private:
    int encode(DataV2_0::RIL_DataProfileInfo *dataProfileInfo, size_t size);
    int encode(RIL_DataProfileInfo_V1_5 *dataProfileInfo, size_t size);
    int encode(RIL_InitialAttachApn_v15 *iaa, size_t size);

public:
    const char *GetApn() const;
    const char *GetUsername() const;
    const char *GetPassword() const;
    const char *GetProtocol() const;
    const char *GetRoamingProtocol() const;
    const std::optional<DataV2_0::RIL_TrafficDescriptor> GetTdInfo() { return mDataProfileInfo.tdInfo;};
    int GetPdpProtocolType() const;
    int GetPdpRoamingProtocolType() const;
    int GetAuthType() const;
    int GetSupportedApnTypesBitmap() const;
    int GetBearerBitmap() const;
    int GetModemCognitive() const;
    int GetMtu() const;
    int GetMtuV6() const;
    bool IsEnabled() const;
    bool IsPrefferred() const;
    bool IsPersist() const;
};

/**
 * SetDataProfileRequestData
 */
class SetDataProfileRequestData : public RequestData {
public:
    DataV2_0::RIL_DataProfileInfo *mDataProfileInfos;
    int mSize;
public:
    SetDataProfileRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    SetDataProfileRequestData(const SetDataProfileRequestData&) = delete;
    SetDataProfileRequestData& operator=(SetDataProfileRequestData const&) = delete;
    virtual ~SetDataProfileRequestData();
    virtual int encode(char *data, unsigned int datalen);
    virtual SetDataProfileRequestData *Clone() const;

public:
    const DataV2_0::RIL_DataProfileInfo *GetDataProfileInfo(int index) const;
    int GetSize() const;

private:
    int encode(DataV2_0::RIL_DataProfileInfo **dataProfileInfo, size_t size);
    int encode(RIL_DataProfileInfo_V1_5 **dataProfileInfo, size_t size);
    int encode(RIL_DataProfileInfo_V1_4 **dataProfileInfo, size_t size);
    int encode(RIL_DataProfileInfo_v15 **dataProfileInfo, size_t size);
};

class KeepaliveRequestData : public RequestData {
public:
    RIL_KeepaliveRequest m_keepAliveReq;
public:
    KeepaliveRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    virtual ~KeepaliveRequestData();
    virtual int encode(char *data, unsigned int datalen);
    virtual KeepaliveRequestData *Clone() const;
};

class SetPdnIpAddressRequestData : public RequestData {
public:
    RIL_SetPdnIpAddressRequest m_SetPdnIpAddressReq;
public:
    SetPdnIpAddressRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    virtual ~SetPdnIpAddressRequestData();
    virtual int encode(char *data, unsigned int datalen);
    virtual SetPdnIpAddressRequestData *Clone() const;
};

/**
 * SetUrspRequestData
 */

typedef struct {
    int osIdFlag;
    char osId[16];
    int appIdFlag;
    char appId[256];
    int fqdnFlag;
    char fqdn[256];
    int ipv4AddrFlag;
    char ipv4Addr[4];
    int ipv4MaskFlag;
    char ipv4Mask[4];
    int ipv6AddrFlag;
    char ipv6Addr[16];
    int prefixLenFlag;
    int prefixLen;
    int nextIpHdrFlag;
    int nextIpHdr;
    int singleRemotePortFlag;
    int singleRemotePort;
    int portRangeLowLimitFlag;
    int portRangeLowLimit;
    int portRangeHighLimitFlag;
    int portRangeHighLimit;
} UrspData;

class SetUrspRequestData : public RequestData {
public:
    SetUrspRequestData(const int nReq,const Token tok,const ReqType type = REQ_FW);
    virtual ~SetUrspRequestData();

    char *GetOsId() { return m_reqData.osId; }
    char *GetAppId() { return m_reqData.appId; }
    char *GetFQDN() { return m_reqData.fqdn; }
    char *GetIpv4Addr() { return m_reqData.ipv4Addr; }
    char *GetIpv4Mask() { return m_reqData.ipv4Mask; }
    char *GetIpv6Addr() { return m_reqData.ipv6Addr; }
    int GetPrefixLen() { return m_reqData.prefixLen; }
    int GetNextIpHdr() { return m_reqData.nextIpHdr; }
    int GetSinglePort() { return m_reqData.singleRemotePort; }
    int GetPortRangeLowLimit() { return m_reqData.portRangeLowLimit; }
    int GetPortRangeHighLimit() { return m_reqData.portRangeHighLimit; }

    int GetOsIdFlag() { return m_reqData.osIdFlag; }
    int GetAppIdFlag() { return m_reqData.appIdFlag; }
    int GetFQDNFlag() { return m_reqData.fqdnFlag; }
    int GetIpv4AddrFlag() { return m_reqData.ipv4AddrFlag; }
    int GetIpv4MaskFlag() { return m_reqData.ipv4MaskFlag; }
    int GetIpv6AddrFlag() { return m_reqData.ipv6AddrFlag; }
    int GetPrefixLenFlag() { return m_reqData.prefixLenFlag; }
    int GetNextIpHdrFlag() { return m_reqData.nextIpHdrFlag; }
    int GetSinglePortFlag() { return m_reqData.singleRemotePortFlag; }
    int GetPortRangeLowLimitFlag() { return m_reqData.portRangeLowLimitFlag; }
    int GetPortRangeHighLimitFlag() { return m_reqData.portRangeHighLimitFlag; }


    virtual INT32 encode(char *data, unsigned int datalen);

private:
    UrspData m_reqData;
};

#endif /*_DATA_CALL_REQ_DATA_H_*/
