/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _NET_DATA_H_
#define _NET_DATA_H_

#include "requestdata.h"
#include "rildata.h"
#include <string>
#include <sstream>

#define MAX_UUID_LENGTH 64

/**
 * Network RadioCapability
 */
class NetRCData : public RequestData
{
public:
    NetRCData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    NetRCData(const NetRCData &netrc);
    virtual ~NetRCData();

    int GetVersion() { return m_nVersion; }
    int GetSession() { return m_nSession; }
    int GetPhase() { return m_nPhase; }
    int GetRat() { return m_nRat; }
    char *GetString() { return m_nLogicalModemUuid; }
    int GetStatus() { return m_nStatus; }

    virtual INT32 encode(char *data, unsigned int length);

    INT32 m_nVersion;
    INT32 m_nSession;
    INT32 m_nPhase;
    INT32 m_nRat;
    char m_nLogicalModemUuid[MAX_UUID_LENGTH];
    INT32 m_nStatus;
};

class NetworkScanReqData : public RequestData
{
private:
    bool mIsLegacyRequest;  // IRadio@1.1 compatible request

public:
    NetworkScanReqData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    NetworkScanReqData(const NetworkScanReqData &netScanReqData);
    NetworkScanReqData& operator=(NetworkScanReqData const&) = delete;
    virtual ~NetworkScanReqData();

    int GetScanType() { return m_scanType; }
    int GetTimeInterval() { return m_timeInterval; }
    int GetSpecifiersLength() { return m_specifiersLength; }
    RIL_RadioAccessSpecifier_V1_5 *GetRadioAccessSpecifier() { return m_radioAccessSpecifiers; }
    int GetMaxSearchTime() { return m_maxSearchTime; }
    bool GetIncrementalResults() { return m_incrementalResults; }
    int GetIncrementalResultsPeriodicity() { return m_incrementalResultsPeriodicity; }
    int GetNumOfMccMncs() { return m_numOfMccMncs; }
    char **GetMccMncs() { return m_mccMncs; }
    bool IsLegacyRequest() const { return mIsLegacyRequest; }

    virtual INT32 encode(char *data, unsigned int length);

private:
    int m_scanType;
    int m_timeInterval;
    int m_specifiersLength;
    RIL_RadioAccessSpecifier_V1_5 m_radioAccessSpecifiers[MAX_RADIO_ACCESS_NETWORKS];
    int m_maxSearchTime;
    bool m_incrementalResults;
    int m_incrementalResultsPeriodicity;
    int m_numOfMccMncs;
    char **m_mccMncs;

protected:
    int encode(RIL_NetworkScanRequest_V1_5 *nsr);

};

class SystemSelectionChannelsReqData : public RequestData
{
public:
    SystemSelectionChannelsReqData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    SystemSelectionChannelsReqData(const SystemSelectionChannelsReqData &reqData);
    virtual ~SystemSelectionChannelsReqData();

    bool GetIsSpecifyChannels() { return m_isSpecifyChannels; }
    int GetSpecifiersLength() { return m_specifiersLength; }
    RIL_RadioAccessSpecifier_V1_5 *GetRadioAccessSpecifier() { return m_radioAccessSpecifiers; }

    virtual INT32 encode(char *data, unsigned int length);
private:
    bool m_isSpecifyChannels;
    int m_specifiersLength;
    RIL_RadioAccessSpecifier_V1_5 m_radioAccessSpecifiers[MAX_RADIO_ACCESS_NETWORKS];
};

class DataThrottlingReqData : public RequestData
{
public:
    DataThrottlingReqData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    DataThrottlingReqData(const DataThrottlingReqData &reqData);
    virtual ~DataThrottlingReqData();

    int GetDataThrottlingAction() { return m_dataThrottlingAction; }
    int64_t GetCompletionDurationMillis() { return m_completionDurationMillis; }

    virtual INT32 encode(char *data, unsigned int length);
private:
    int m_dataThrottlingAction;
    int64_t m_completionDurationMillis;
};

class PhysicalChannelConfigsData {
public:
    PhysicalChannelConfigsData();
    PhysicalChannelConfigsData(const PhysicalChannelConfigsData& o);
    PhysicalChannelConfigsData& operator=(PhysicalChannelConfigsData const&) = default;  // Implicit implementation is used in test code
    virtual ~PhysicalChannelConfigsData();

    void setCurrentStatusRat(int status, int rat);
    void setDownLinkInfo(int channelNumber, int cellBandwidthDownlink, int range, int physicalCellId);
    void setContextIds(int lenContextIds, int *contextIds);
    void setUplinkInfo(int uplinkChannelNumber, int cellBandwidthUplink, int bandNumber);

    int getStatus() { return mStatus; }
    int getCellBandwidthDownlink() { return mCellBandwidthDownlink; }
    int getRat() { return mRat; }
    int getRfInfoType() { return mRfInfoType; }
    int getRange() { return mRange; }
    int getChannelNumber() { return mChannelNumber; }
    int getLenContextIds() { return mLen_contextIds; }
    int* getContextIds() { return mContextIds; }
    int getPhysicalCellId() { return mPhysicalCellId; }
    int getUplinkChannelNumber() { return mUplinkChannelNumber; }
    int getCellBandwidthUplink() { return mCellBandwidthUplink; }
    int getBandNumber() { return mBandNumber; }

    std::string toString() {
        std::stringstream ss;
        ss << "PhysicalChannelConfigsData={";
        ss << "mStatus=" << mStatus;
        ss << " mCellBandwidthDownlink=" << mCellBandwidthDownlink;
        ss << " mRat=" << mRat;
        ss << " mRfInfoType=" << mRfInfoType;
        ss << " mRange=" << mRange;
        ss << " mChannelNumber=" << mChannelNumber;
        ss << " mLen_contextIds=" << mLen_contextIds;
        if (mLen_contextIds > 0) {
            ss << "contextIds{";
            for (int i = 0; i < mLen_contextIds; i++) {
                ss << " " << mContextIds[i] << ", ";
            }
            ss << "}";
        }
        ss << " mPhysicalCellId=" << mPhysicalCellId;
        ss << " mUplinkChannelNumber=" << mUplinkChannelNumber;
        ss << " mCellBandwidthUplink=" << mCellBandwidthUplink;
        ss << " mBandNumber=" << mBandNumber;
        ss << "}";
        return ss.str();
    }

private:
    // RIL_PhysicalChannelConfig_V1_4
    int mStatus;
    int mCellBandwidthDownlink;
    int mRat;
    int mRfInfoType;
    int mRange;
    int mChannelNumber;
    int mLen_contextIds;
    int *mContextIds;
    int mPhysicalCellId;

    // RIL_PhysicalChannelConfig_V1_6
    int mUplinkChannelNumber;
    int mCellBandwidthUplink;
    int mBandNumber;
};

class CellInfoList_V1_5 : public RilDataListHolder<RIL_CellInfo_V1_5> {
public:
    using RilDataListHolder::RilDataListHolder;

    CellInfoList_V1_5(const std::list<RIL_CellInfo_V1_5>& cellInfoList) {
        setData(cellInfoList);
    }

    CellInfoList_V1_5(const std::list<RIL_CellInfo_V1_6>& cellInfoList) {
        setData(cellInfoList);
    }

    void setData(const std::list<RIL_CellInfo_V1_5>& cellInfoList) {
        mList.resize(cellInfoList.size());
        size_t i = 0;
        for (auto& info : cellInfoList) {
            mList[i++] = info;
        }
    }

    void setData(const std::list<RIL_CellInfo_V1_6>& cellInfoList) {
        mList.resize(cellInfoList.size());
        size_t i = 0;
        for (auto& cur : cellInfoList) {
            RadioConvUtils::convertCellInfo(mList[i++], cur);
        }
    }
};

class CellInfoList_V1_6 : public RilDataListHolder<RIL_CellInfo_V1_6> {
public:
    using RilDataListHolder::RilDataListHolder;

    CellInfoList_V1_6(const std::list<RIL_CellInfo_V1_6>& cellInfoList) {
        setData(cellInfoList);
    }

    void setData(const std::list<RIL_CellInfo_V1_6>& cellInfoList) {
        mList.resize(cellInfoList.size());
        size_t i = 0;
        for (auto& info : cellInfoList) {
            mList[i++] = info;
        }
    }
};

#endif /*_NET_DATA_H_*/
