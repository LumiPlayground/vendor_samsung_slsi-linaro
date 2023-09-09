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
 * netdatabuilder.h
 *
 *  Created on: 2014. 11. 24.
 *      Author: sungwoo48.choi
 */

#ifndef __NET_DATA_BUILDER_H__
#define __NET_DATA_BUILDER_H__

#include "rildatabuilder.h"
#include "operatorinfo.h"
#include <vector>
#include <list>
#include "rildef.h"

class Networkinfo;
class PhysicalChannelConfigsData;

int getRadioTechToCellInfoType(int rat);

/**
 * NetworkDataBuilder
 */
class NetworkDataBuilder : public RilDataBuilder {
public:
  RilDataUniqPtr BuildOperatorResponse(const OperatorInfo &resultOpName, const OperatorInfo &rawOpName);
  RilDataUniqPtr BuildNetSelectModeResponse(int mode);
  RilDataUniqPtr BuildNetAvailableBandModeResponse(const int *bandMode, int count);
  RilDataUniqPtr BuildNetAvailableNetweorkResponse(const NetworkInfo *networks, int count);
  RilDataUniqPtr BuildNetFemtoCellSrchResponse(const int srch_result, const char *plmn);
};

class NetworkDataBplmnListBuilder : public RilDataBuilder {
private:
  std::vector<NetworkInfo> mList;

public:
    void AddNetworkInfo(const NetworkInfo &nwkInfo);
    const RilData *Build();
};

/**
 * CellIdentityBuilder
 */
class CellIdentityBuilder {
protected:
    int mCellInfoType;
private:
//    RIL_CellIdentity_V1_2 mCellIdentity;
    RIL_CellIdentity_v16 mCellIdentity_v16;
    RIL_CellIdentity_V1_5 mCellIdentity;
public:
    CellIdentityBuilder();
    CellIdentityBuilder(int cellInfoType);
    void SetCellInfoType(int cellInfoType);
    void SetCsgInfo(bool isCsg, const char *nodeBname, int csgId);
    virtual ~CellIdentityBuilder() {}
    void SetCellIdentity(int mcc, int mnc, const char *alphaLong = NULL, const char *alphaShort = NULL);
    void SetCellIdentity(const char *numeric, const char *alphaLong = NULL, const char *alphaShort = NULL);
    void SetCellIdentity(int lac, int cid, int psc, int tac, int pcid, int eci, int channel, long int nrcid);
    void SetCdmaCellIdentity(int basestationId, int latitude,
            int longitude, int systemId, int networkId);
    void *cellIdentity(int halVer);
};

/**
 * DataRegStateResultBuilder
 */
class DataRegStateResultBuilder : public RilDataBuilder {
private:
    CellIdentityBuilder mCellIdentityBuilder;
    int mHalVer;
    RIL_RegStateResult_V1_6 mDataRegResult;
public:
    DataRegStateResultBuilder(int halVer = HAL_VERSION_CODE(1, 0)) {
        mHalVer = halVer;
        memset(&mDataRegResult, 0, sizeof(mDataRegResult));
    }
    void SetRegistrationState(int regState, int rat, int rejectCause, int sdc);
    void SetRegisteredPlmn(const char *numeric);
    void SetCellIdentity(int mcc, int mnc, const char *alphaLong = NULL, const char *alphaShort = NULL);
    void SetCellIdentity(const char *numeric, const char *alphaLong = NULL, const char *alphaShort = NULL);
    void SetCellIdentity(int lac, int cid, int psc, int tac, int pcid, int eci, int channel, long int nrcid);
    void SetLteVopsInfo(bool isVopsSupported, bool isEmcBearerSupported);
    void SetNrIndicators(bool isEndcAvailable, bool isDcNrRestricted, bool isNrAvailable);
    void SetNrVopsInfo(int vopsSupported, int emcSupported, int emfSupported);
    RilDataUniqPtr Build();
};

/**
 * VoiceRegStateResultBuilder
 */
class VoiceRegStateResultBuilder : public RilDataBuilder {
private:
    CellIdentityBuilder mCellIdentityBuilder;
    int mHalVer;
    RIL_RegStateResult_V1_6 mVoiceRegResult;
public:
    VoiceRegStateResultBuilder(int halVer = HAL_VERSION_CODE(1, 0)) {
        mHalVer = halVer;
        memset(&mVoiceRegResult, 0, sizeof(mVoiceRegResult));
    }
    void SetRegistrationState(int regState, int rat, int rejectCause);
    void SetCdmaState(int cssSupported, int roamingIndicator, int systemIsInPrl, int defaultRoamingIndicator);
    void SetRegisteredPlmn(const char *numeric);
    void SetCellIdentity(int mcc, int mnc, const char *alphaLong = NULL, const char *alphaShort = NULL);
    void SetCellIdentity(const char *numeric, const char *alphaLong = NULL, const char *alphaShort = NULL);
    void SetCellIdentity(int lac, int cid, int psc, int tac, int pcid, int eci, int channel, long int nrcid);
    void SetCdmaCellIdentity(int basestationId, int latitude,
            int longitude, int systemId, int networkId);
    RilDataUniqPtr Build();
};

/**
 * CellInfoBuilder
 */
class CellInfoListBuilder : public RilDataBuilder {
private:
    int mHalVer;
public:
    CellInfoListBuilder(int halVer = HAL_VERSION_CODE(1,0)) { mHalVer = halVer; }
    const RilData *Build(std::list<RIL_CellInfo_V1_5> &cellInfoList);
    const RilData *Build(std::list<RIL_CellInfo_V1_6> &cellInfoList);
};

/**
 * NetworkScanResultBuilder
 */
class NetworkScanResultBuilder : public RilDataBuilder {
private:
    int mHalVer;
public:
    NetworkScanResultBuilder(int halVer = HAL_VERSION_CODE(1,0)) {
        mHalVer = halVer;
    }
    virtual ~NetworkScanResultBuilder() {}
    const RilData *Build(int status, int result, std::list<RIL_CellInfo_V1_5> &cellInfoList);
    const RilData *Build(int status, int result, std::list<RIL_CellInfo_V1_6> &cellInfoList);
};

/**
 * PhysicalChannelConfigs
 */
class PhysicalChannelConfigsBuilder : public RilDataBuilder {
private:
    int mHalVer;
public:
    PhysicalChannelConfigsBuilder(int halVer = HAL_VERSION_CODE(1,2)) : mHalVer(halVer) {}
    const RilData *Build(std::list<PhysicalChannelConfigsData> &configList);
};

/**
 * RestrictedStateBuilder
 */
class RestrictedStateBuilder {
public:
    const RilData* Build(const RIL_CellIdAndBarringInfo *info);
};

/**
 * BarringInfoBuilder
 */
class BarringInfoBuilder {
public:
    const RilData* Build(const RIL_CellIdAndBarringInfo *info);
};

/**
 * ActivityStatsInfoBuilder
 */
class ActivityStatsInfoBuilder {
    int mHalVer;
public:
    ActivityStatsInfoBuilder(int halVer = HAL_VERSION_CODE(1,0)) : mHalVer(halVer) {}
    const RilData *build(const android::hardware::radio::impl::modem::V2_0::RIL_ActivityStatsInfo& info);
};
#endif /* __NET_DATA_BUILDER_H__ */
