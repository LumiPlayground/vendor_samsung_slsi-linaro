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
 * psdatabuilder.h
 *
 *  Created on: 2014. 11. 24.
 *      Author: sungwoo48.choi
 */

#ifndef __PS_DATA_BUILDER_H__
#define __PS_DATA_BUILDER_H__

#include "rildatabuilder.h"
#include <vector>
#include "rildef.h"
#include <librilutils/vendor.h>

class PdpContext;
class PsTestHandler;

class PsDataBuilder : public RilDataBuilder {
private:
    int mHalVer;
    // cmcc/chnopen has 'true', default is europen, so 'false'
    bool m_bIpv4DnsQueryFirst = false;
private:
    char *ConvertPdpType(int nPdpType);
public:
    PsDataBuilder(int halVer = HAL_VERSION_CODE(1, 5));
    const RilData *BuildSetupDataCallResponse(PdpContext *pPdpContext);
    const RilData *BuildSetupDataCallResponse(int errorCode, PdpContext *pPdpContext);
    const RilData *BuildSetupDataCallResponse(int errorCode, int status);
    const RilData *BuildPcoData(int cid, int nPdpType, int pcoId, int contentsLen, char *pContents);
public:
    inline void setIpv4DnsQueryFirst(bool bIpv4First) { m_bIpv4DnsQueryFirst = bIpv4First; }
};

class PsDataCallListBuilder : public RilDataBuilder {
private:
    int mHalVer;
    // cmcc/chnopen has 'true', default is europen, so 'false'
    bool m_bIpv4DnsQueryFirst = false;
    bool mIsRrcCon = false;
private:
    std::vector<PdpContext *> m_PdpContextList;

public:
    PsDataCallListBuilder(int halVer = HAL_VERSION_CODE(1, 5));
    int AddDataCall(PdpContext *pPdpContext);
    const RilData *Build(int halVer = HAL_VERSION_CODE(1, 5));
    void Clear();
    void SetCurRrcState(bool state) { mIsRrcCon = state; }
    bool IsRrcConState() { return mIsRrcCon; }
public:
    inline void setIpv4DnsQueryFirst(bool bIpv4First) { m_bIpv4DnsQueryFirst = bIpv4First; }
};
class PsDataNasTimerStatusBuilder : public RilDataBuilder {
public:
    const RilData *BuildNasTimerStatus(const SitNasTimerStatus *pNasTimerStatus);
};
#endif /* __PS_DATA_BUILDER_H__ */
