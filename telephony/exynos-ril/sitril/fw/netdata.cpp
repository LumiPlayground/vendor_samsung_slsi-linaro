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
 * netdata.cpp
 *
 *  Created on: 2014. 6. 28.
 *      Author: mox
 */
#include <librilutils/radio_conv_utils.h>

#include "netdata.h"
#include "rillog.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

/**
 * Net RadioCapability
 */
NetRCData::NetRCData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok,type)
{
    m_nVersion = -1;
    m_nSession = -1;
    m_nPhase = -1;
    m_nRat = -1;
    memset(m_nLogicalModemUuid, 0, sizeof(m_nLogicalModemUuid));
    m_nStatus = -1;
}

NetRCData::NetRCData(const NetRCData &netrc) : RequestData(netrc.m_nReq, netrc.m_tok, netrc.m_reqType)
{
    m_nVersion = netrc.m_nVersion;
    m_nSession = netrc.m_nSession;
    m_nPhase = netrc.m_nPhase;
    m_nRat = netrc.m_nRat;
    memcpy(m_nLogicalModemUuid, netrc.m_nLogicalModemUuid, sizeof(m_nLogicalModemUuid));
    m_nStatus = netrc.m_nStatus;
}

NetRCData::~NetRCData()
{
}

INT32 NetRCData::encode(char *data, unsigned int length)
{
    if((0 == length) || (NULL == data)) return -1;

    RIL_RadioCapability *pNetRC = (RIL_RadioCapability *)data;

    m_nVersion = pNetRC->version;
    m_nSession = pNetRC->session;
    m_nPhase = pNetRC->phase;
    m_nRat = pNetRC->rat;
    memcpy(m_nLogicalModemUuid, pNetRC->logicalModemUuid, sizeof(m_nLogicalModemUuid));
    m_nStatus = pNetRC->status;

    RilLogV("NetRCData::%s() RAT(%d)", __FUNCTION__, m_nRat);

    return 0;
}

/**
 * Network Scan
 */
NetworkScanReqData::NetworkScanReqData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
    m_scanType = -1;
    m_timeInterval = 0;
    m_specifiersLength = 0;
    memset(&m_radioAccessSpecifiers, 0, sizeof(m_radioAccessSpecifiers));
    m_maxSearchTime = 0;
    m_incrementalResults = false;
    m_incrementalResultsPeriodicity = 0;
    m_numOfMccMncs = 0;
    m_mccMncs = NULL;
    // mIsLegacyRequest: true RadioHidlTest_v1_x#startNetworkScan
    mIsLegacyRequest = true;
}

NetworkScanReqData::NetworkScanReqData(const NetworkScanReqData &netScanReqData): RequestData(netScanReqData.m_nReq, netScanReqData.m_tok, netScanReqData.m_reqType)
{
    m_scanType = netScanReqData.m_scanType;
    m_timeInterval = netScanReqData.m_timeInterval;
    m_specifiersLength = netScanReqData.m_specifiersLength;
    for (int index=0; index<MAX_RADIO_ACCESS_NETWORKS; index++)
    {
        if(index < m_specifiersLength) {
            m_radioAccessSpecifiers[index] = netScanReqData.m_radioAccessSpecifiers[index];
        } else {
            memset(&(m_radioAccessSpecifiers[index]), 0, sizeof(RIL_RadioAccessSpecifier));
        }
    }
    m_maxSearchTime = 0;
    m_incrementalResults = false;
    m_incrementalResultsPeriodicity = 0;
    m_numOfMccMncs = 0;
    m_mccMncs = NULL;
    // mIsLegacyRequest: true RadioHidlTest_v1_x#startNetworkScan
    mIsLegacyRequest = true;
}

NetworkScanReqData::~NetworkScanReqData()
{
    if (m_mccMncs != NULL) {
        for (int i = 0; i < m_numOfMccMncs; i++) {
            if (m_mccMncs[i] != NULL) {
                free(m_mccMncs[i]);
            }
        }
        free(m_mccMncs);
    }
}

int NetworkScanReqData::encode(RIL_NetworkScanRequest_V1_5 *nsr)
{
    m_scanType = nsr->type;
    m_timeInterval = nsr->interval;
    m_specifiersLength = nsr->specifiers_length;

    for (int index = 0; index < m_specifiersLength; index++) {
        m_radioAccessSpecifiers[index] = nsr->specifiers[index];
    } // end for i ~

    m_maxSearchTime = nsr->maxSearchTime;
    m_incrementalResults = nsr->incrementalResults;
    m_incrementalResultsPeriodicity = nsr->incrementalResultsPeriodicity;
    m_numOfMccMncs = nsr->numOfMccMncs;
    size_t memSize = m_numOfMccMncs * sizeof(char *);
    m_mccMncs = (char **)malloc(memSize);
    if (m_mccMncs != NULL) {
        memset(m_mccMncs, 0, memSize);
        for (int i = 0; i < m_numOfMccMncs; i++) {
            m_mccMncs[i] = (char *) calloc(MAX_PLMN_LEN+1, sizeof(char));
            if (m_mccMncs[i] != NULL) {
                memset(m_mccMncs[i], 0, MAX_PLMN_LEN+1);
                strncpy(m_mccMncs[i], nsr->mccMncs[i], MAX_PLMN_LEN+1);
            }
            else {
                // fail
                return -1;
            }
        }
    }

    // RadioHidlTest_v1_x#tartNetworkScan_XXX
    if (m_maxSearchTime != 0 || m_incrementalResults != 0 || m_incrementalResultsPeriodicity != 0) {
        mIsLegacyRequest = false;
    }
    return 0;
}

INT32 NetworkScanReqData::encode(char *data, unsigned int length)
{
    if ((0 == length) || (NULL == data)) return -1;

    if (mHalVer == HAL_VERSION_CODE(1,5)) {
        encode((RIL_NetworkScanRequest_V1_5 *)data);
    }
    else {
        RIL_NetworkScanRequest *nsr = (RIL_NetworkScanRequest *)data;
        m_scanType = nsr->type;
        m_timeInterval = nsr->interval;
        m_specifiersLength = nsr->specifiers_length;
        for (int index = 0; index < m_specifiersLength; index++) {
            RadioConvUtils::convertRadioAccessSpecifiers(m_radioAccessSpecifiers[index],
                    ((RIL_RadioAccessSpecifier *)nsr->specifiers)[index]);
        } // end for i ~

        if (length >= sizeof(RIL_NetworkScanRequest_V1_2)) {
            RIL_NetworkScanRequest_V1_2 *nsr_v12 = (RIL_NetworkScanRequest_V1_2 *)data;
            m_maxSearchTime = nsr_v12->maxSearchTime;
            m_incrementalResults = nsr_v12->incrementalResults;
            m_incrementalResultsPeriodicity = nsr_v12->incrementalResultsPeriodicity;
            m_numOfMccMncs = nsr_v12->numOfMccMncs;
            size_t memSize = m_numOfMccMncs * sizeof(char *);
            m_mccMncs = (char **)malloc(memSize);
            if (m_mccMncs != NULL) {
                memset(m_mccMncs, 0, memSize);
                for (int i = 0; i < m_numOfMccMncs; i++) {
                    m_mccMncs[i] = (char *) calloc(MAX_PLMN_LEN+1, sizeof(char));
                    if (m_mccMncs[i] != NULL) {
                        memset(m_mccMncs[i], 0, MAX_PLMN_LEN+1);
                        strncpy(m_mccMncs[i], nsr_v12->mccMncs[i], MAX_PLMN_LEN+1);
                    }
                    else {
                        // fail
                        return -1;
                    }
                }
            }

            // RadioHidlTest_v1_x#tartNetworkScan_XXX
            if (m_maxSearchTime != 0 || m_incrementalResults != 0 || m_incrementalResultsPeriodicity != 0) {
                mIsLegacyRequest = false;
            }
        }
    }

    return 0;
}


/**
 * System selection channesl
 */
SystemSelectionChannelsReqData::SystemSelectionChannelsReqData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
    m_isSpecifyChannels = false;
    m_specifiersLength = 0;
    memset(&m_radioAccessSpecifiers, 0, sizeof(m_radioAccessSpecifiers));
}

SystemSelectionChannelsReqData::SystemSelectionChannelsReqData(const SystemSelectionChannelsReqData &reqData) : RequestData(reqData.m_nReq, reqData.m_tok, reqData.m_reqType)
{
    m_isSpecifyChannels = reqData.m_isSpecifyChannels;
    m_specifiersLength = reqData.m_specifiersLength;
    for (int index=0; index < MAX_RADIO_ACCESS_NETWORKS; index++)
    {
        if(index < m_specifiersLength) {
            m_radioAccessSpecifiers[index] = reqData.m_radioAccessSpecifiers[index];
        } else {
            memset(&(m_radioAccessSpecifiers[index]), 0, sizeof(RIL_RadioAccessSpecifier_V1_5));
        }
    }
}

SystemSelectionChannelsReqData::~SystemSelectionChannelsReqData()
{
}

INT32 SystemSelectionChannelsReqData::encode(char *data, unsigned int length)
{
    if ((0 == length) || (NULL == data)) return -1;

    int halVer = GetHalVersion();
    if (halVer < HAL_VERSION_CODE(1, 5)) {
        // it is blocked in ril service. so pleae implement when it is needed.
        return -1;
    } else {
        RIL_SysSelChannelRequest_V1_5 *sscr = (RIL_SysSelChannelRequest_V1_5 *)data;
        m_isSpecifyChannels = sscr->isSpecifyChannels;
        m_specifiersLength = sscr->specifiers_length;
        for (int index = 0; index < m_specifiersLength && index < MAX_RADIO_ACCESS_NETWORKS; index++) {
            m_radioAccessSpecifiers[index] = sscr->specifiers[index];
        } // end for index ~
    }

    return 0;
}

/**
 * Set Data Throttling
 */
DataThrottlingReqData::DataThrottlingReqData(const int nReq, const Token tok, const ReqType type) : RequestData(nReq, tok, type)
{
    m_dataThrottlingAction = -1;
    m_completionDurationMillis = -1;
}

DataThrottlingReqData::DataThrottlingReqData(const DataThrottlingReqData &reqData) : RequestData(reqData.m_nReq, reqData.m_tok, reqData.m_reqType)
{
    m_dataThrottlingAction = reqData.m_dataThrottlingAction;
    m_completionDurationMillis = reqData.m_completionDurationMillis;
}

DataThrottlingReqData::~DataThrottlingReqData()
{
}

INT32 DataThrottlingReqData::encode(char *data, unsigned int length)
{
    if ((0 == length) || (NULL == data)) return -1;

    RIL_DataThrottling *pDataThrottling = (RIL_DataThrottling *)data;
    m_dataThrottlingAction = (int)pDataThrottling->dataThrottlingAction;
    m_completionDurationMillis = pDataThrottling->completionDurationMillis;

    return 0;
}

/**
 * PhysicalChannelConfigsData
 */
PhysicalChannelConfigsData::PhysicalChannelConfigsData() {
    mStatus = 0;
    mCellBandwidthDownlink = 0;
    mRat = 0;
    mRfInfoType = 0;
    mRange = 0;
    mChannelNumber = 0;
    mLen_contextIds = 0;
    mContextIds = NULL;
    mPhysicalCellId = 0;
    mUplinkChannelNumber = 0;
    mCellBandwidthUplink = 0;
    mBandNumber = 0;
}

PhysicalChannelConfigsData::PhysicalChannelConfigsData(const PhysicalChannelConfigsData& o)
    : mStatus(o.mStatus), mCellBandwidthDownlink(o.mCellBandwidthDownlink), mRat(o.mRat),
    mRfInfoType(o.mRfInfoType), mRange(o.mRange), mChannelNumber(o.mChannelNumber),
    mLen_contextIds(0), mContextIds(nullptr), mPhysicalCellId(o.mPhysicalCellId),
    mUplinkChannelNumber(o.mUplinkChannelNumber), mCellBandwidthUplink(o.mCellBandwidthUplink),
    mBandNumber(o.mBandNumber) {
    setContextIds(o.mLen_contextIds, o.mContextIds);
}

PhysicalChannelConfigsData::~PhysicalChannelConfigsData() {
    if (mContextIds != NULL) {
        delete[] mContextIds;
        mContextIds = NULL;
    }
}

void PhysicalChannelConfigsData::setCurrentStatusRat(int conStatus, int rat) {
    mStatus = conStatus;
    mRat = rat;
}

void PhysicalChannelConfigsData::setDownLinkInfo(int channelNumber, int cellBandwidthDownlink, int range, int physicalCellId) {
    mChannelNumber = channelNumber;
    if (mRat == RADIO_TECH_NR) {
        mRfInfoType = RF_INFO_TYPE_RANGE;
        mRange = range;
    } else {
        mRfInfoType = RF_INFO_TYPE_CHANNEL_NUMBER;
    }
    mCellBandwidthDownlink = cellBandwidthDownlink;
    mPhysicalCellId = physicalCellId;
}

void PhysicalChannelConfigsData::setContextIds(int lenContextIds, int *contextIds) {
    if (mContextIds != NULL) {
        delete[] mContextIds;
        mContextIds = NULL;
    }

    mLen_contextIds = lenContextIds;
    if (mLen_contextIds > 0) {
        mContextIds = new int[mLen_contextIds];
        if (mContextIds != NULL) {
            for (int i = 0; i < mLen_contextIds; i++) {
                mContextIds[i] = contextIds[i];
            }
        } else {
            mLen_contextIds = 0;
        }
    }
}

void PhysicalChannelConfigsData::setUplinkInfo(int uplinkChannelNumber, int cellBandwidthUplink, int bandNumber) {
    mUplinkChannelNumber = uplinkChannelNumber;
    mCellBandwidthUplink = cellBandwidthUplink;
    mBandNumber = bandNumber;
}
