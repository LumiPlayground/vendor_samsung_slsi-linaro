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
 * miscdatabuilder.cpp
 *
 *  Created on: 2014. 7. 3.
 *      Author: m.afzal
 */
#include <librilutils/osalutils.h>
#include <librilutils/radio_conv_utils.h>
#include "miscdatabuilder.h"
#include "constdef.h"
#include "rillog.h"
#include <librilutils/textutils.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_MISC, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

const RilData *MiscDataBuilder::BuildBaseBandVersionResponse( const char *SwVer)
{
    RilDataString *rildata = new RilDataString();
    if (rildata != NULL) {
        rildata->SetString((char *)SwVer);
    }
    return rildata;
}


const RilData *MiscDataBuilder::BuildGetTtyModeResponse(int TtyMode)
{
    RilDataInts *rildata = new RilDataInts(1);
    if (rildata != NULL) {
        rildata->SetInt(0, TtyMode);
    }

    return rildata;
}

const RilData *MiscDataBuilder::BuildNitzTimeIndication(int /* DayLightValid */, int Year, int Month, int Day, int Hour,
        int Minute, int Second, int TimeZone, int DayLightAdjust, int /* DayofWeek */)
{
    char mRespTimeInfo[28];    //static const int MAX_TIME_INFO_LEN = 28;
    snprintf(mRespTimeInfo, sizeof(mRespTimeInfo)-1,
         "%02d/%02d/%02d,%02d:%02d:%02d%s%02d,%02d",
         Year, Month, Day, Hour, Minute, Second,
         TimeZone >= 0 ? "+" : "-",
         abs(TimeZone), DayLightAdjust);

    RilDataString *rildata = new RilDataString();
    if (rildata != NULL) {
        rildata->SetString(mRespTimeInfo);
    }

    return rildata;
}

const RilData *MiscDataBuilder::BuildIMEIResponse(int IMEILen, const BYTE *IMEI)
{
    RilLogI("%s()", __FUNCTION__);
    if (IMEI == NULL || IMEILen <= 0) {
        return NULL;
    }

    char buf[MAX_IMEI_LEN + 1] = { 0, };
    memcpy(buf, IMEI, IMEILen);
    buf[IMEILen] = 0;

    RilDataString *rildata = new RilDataString();
    if (rildata != NULL) {
        rildata->SetString(buf);
    }
    return rildata;
}

const RilData *MiscDataBuilder::BuildIMEISVResponse(int IMEISVLen, const BYTE *IMEISV)
{
    RilLogI("%s()", __FUNCTION__);
    if (IMEISV == NULL || IMEISVLen <= 0) {
        return NULL;
    }

    char buf[MAX_IMEISV_LEN + 1] = { 0, };
    memcpy(buf, IMEISV, IMEISVLen);
    buf[IMEISVLen] = 0;

    RilDataString *rildata = new RilDataString();
    if (rildata != NULL) {
        rildata->SetString(buf);
    }
    return rildata;
}

const RilData *MiscDataBuilder::BuildDevIDResponse(int IMEILen, const BYTE *IMEI, int IMEISVLen, const BYTE *IMEISV, int MEIDLen, const BYTE *MEID,
        int ESNLen, const BYTE *ESN)
{
    const int indexForIMEI = 0;
    const int indexForIMEISV = 1;
    const int indexForESN = 2;
    const int indexForMEID = 3;
    const int numOfDevIDStrings = 4;

    RilDataStrings *rildata = new RilDataStrings(numOfDevIDStrings);

    auto setStringInRilDataStrings = [rildata](int srcLen, const BYTE *src, int dstIndex) {
        if (srcLen <= 0 || src == NULL)
            return;

        const int maxStrLen = 32;
        char buf[maxStrLen + 1] = {
            0,
        };

        int cpyLen = MIN(srcLen, maxStrLen);
        memcpy(buf, src, cpyLen);
        buf[cpyLen] = 0;
        rildata->SetString(dstIndex, buf);
    };

    if (rildata != NULL) {
        setStringInRilDataStrings(IMEILen, IMEI, indexForIMEI);
        setStringInRilDataStrings(IMEISVLen, IMEISV, indexForIMEISV);
        setStringInRilDataStrings(ESNLen, ESN, indexForESN);
        setStringInRilDataStrings(MEIDLen, MEID, indexForMEID);
    }

    return rildata;
}

const RilData *MiscDataBuilder::BuildDisplayEngIndication( const char * pResponse, int Length)
{
    RilDataRaw *pRilData = new RilDataRaw();
    if (pRilData != NULL) {
        pRilData->SetData(pResponse, Length);
    }

    return pRilData;
}

const RilData *MiscDataBuilder::BuildLceDataIndication(int /* dlLc */, int /* ulLc */, int /* confLevel */, int /* isSuspended */)
{
    RilLogW("%s() need to implement", __FUNCTION__);
    return NULL;
}

const RilData *MiscDataBuilder::BuildCdmaSubscriptionSource(int nSubscriptionSource)
{
    RilDataInts *pRilData = new RilDataInts(1);
    if(pRilData != NULL) {
        pRilData->SetInt(0, nSubscriptionSource);
    }

    return pRilData;
}

const RilData *MiscDataBuilder::BuildCdmaSubscription(char *mdn, WORD sid, WORD nid, char *min, UINT prl_version)
{
    RilDataStrings *pRilData = new RilDataStrings(5);
    if(pRilData != NULL)
    {
        char szBuffer[32];

        if(mdn!=NULL) pRilData->SetString(0, mdn);
        sprintf(szBuffer, "%d", sid);
        pRilData->SetString(1, szBuffer);
        sprintf(szBuffer, "%d", nid);
        pRilData->SetString(2, szBuffer);
        if(min!=NULL) pRilData->SetString(3, min);
        sprintf(szBuffer, "%d", prl_version);
        pRilData->SetString(4, szBuffer);
    }

    return pRilData;
}

const RilData *MiscDataBuilder::BuildHardwareConfigNV(char *pszUUID, int nState, int nModel, UINT uRat, int nMaxCS, int nMaxPS, int nMaxStandby)
{
    RilDataRaw *pRilData = new RilDataRaw();
    if (pRilData != NULL) {
        RIL_HardwareConfig tRilHwCfg;
        memset(&tRilHwCfg, 0, sizeof(tRilHwCfg));
        tRilHwCfg.uuid[0] = '\0';
        //memset(tRilHwCfg.uuid, 0, sizeof(tRilHwCfg.uuid));
        //memcpy(tRilHwCfg.uuid, DEFAULT_UUID, MAX_UUID_LENGTH - 1);
        tRilHwCfg.type = RIL_HARDWARE_CONFIG_MODEM;
        if(pszUUID!=NULL) strncpy(tRilHwCfg.uuid, pszUUID, MAX_UUID_LENGTH-1);
        tRilHwCfg.state = (RIL_HardwareConfig_State) nState;

        tRilHwCfg.cfg.modem.rilModel = nModel;
        tRilHwCfg.cfg.modem.rat = uRat;
        tRilHwCfg.cfg.modem.maxVoice = nMaxCS;
        tRilHwCfg.cfg.modem.maxData = nMaxPS;
        tRilHwCfg.cfg.modem.maxStandby = nMaxStandby;

        pRilData->SetData(&tRilHwCfg, sizeof(tRilHwCfg));
    }
    return pRilData;
}

const RilData *MiscDataBuilder::BuildHardwareConfigRuim(char *pszUUID, int nState, char *pszModemUUID)
{
    RilDataRaw *pRilData = new RilDataRaw();
    if (pRilData != NULL) {
        RIL_HardwareConfig tRilHwCfg;
        memset(&tRilHwCfg, 0, sizeof(tRilHwCfg));
        tRilHwCfg.uuid[0] = '\0';
        //memset(tRilHwCfg.uuid, 0, sizeof(tRilHwCfg.uuid));
        //memcpy(tRilHwCfg.uuid, DEFAULT_UUID, MAX_UUID_LENGTH - 1);
        tRilHwCfg.cfg.sim.modemUuid[0] = '\0';
        tRilHwCfg.type = RIL_HARDWARE_CONFIG_SIM;
        if(pszUUID!=NULL) strncpy(tRilHwCfg.uuid, pszUUID, MAX_UUID_LENGTH-1);
        tRilHwCfg.state = (RIL_HardwareConfig_State) nState;

        if(pszModemUUID!=NULL) strncpy(tRilHwCfg.cfg.sim.modemUuid, pszModemUUID, MAX_UUID_LENGTH-1);

        pRilData->SetData(&tRilHwCfg, sizeof(tRilHwCfg));
    }
    return pRilData;
}

const RilData *MiscDataBuilder::BuildRssiScanResult(int total, int current, int startFrequency, int endFrequency, int step, INT16* result, int resultSize)
{
    int totalLen = 4 * 5 + 2 * resultSize;
    char *buf = new char[totalLen];
    if (buf == NULL) {
        return NULL;
    }
    ((int *)buf)[0] = total;
    ((int *)buf)[1] = current;
    ((int *)buf)[2] = startFrequency;
    ((int *)buf)[3] = endFrequency;
    ((int *)buf)[4] = step;
    if (resultSize > 0 && result != NULL) {
        memcpy(&((int *)buf)[5], result, sizeof(INT16) * resultSize);
    }
    RilDataRaw *rildata = new RilDataRaw(buf, totalLen);
    delete[] buf;
    return rildata;
}

const RilData *MiscDataBuilder::BuildATCommand(const char *command)
{
    const int MAX_BUF = 1000;
    char buf[MAX_BUF + 1] = {0, };
    RilDataRaw *rildata = NULL;
    if (!TextUtils::IsEmpty(command)) {
        int len = strlen(command);
        if (len > MAX_BUF)
            len = MAX_BUF;

        strncpy(buf, command, len);
        buf[len] = 0;

        rildata = new RilDataRaw(buf, len);
    }
    return rildata;
}

const RilData *MiscDataBuilder::BuildGetModemStatus(int status)
{
    RilDataInts *pRilData = new RilDataInts(1);
    if(pRilData != NULL) {
        pRilData->SetInt(0, status);
    }

    return pRilData;
}

/**
 * SignalStrengthBuilder
 */
SignalStrengthBuilder::SignalStrengthBuilder(int halVer)
{
    mHalVer = halVer;
}

/**
 * SignalStrengthBuilder
 */
const RilData *SignalStrengthBuilder::Build(RIL_SignalStrength_V1_6& currentSignalStrength)
{
    // HAL_VERSION_CODE(1, 5) or less
    if (mHalVer < HAL_VERSION_CODE(1, 6)) {
        RIL_SignalStrength_V1_4 signalStrength_V1_4;
        memset(&signalStrength_V1_4, 0, sizeof(signalStrength_V1_4));
        RadioConvUtils::convertSignalStrengthResult(signalStrength_V1_4, currentSignalStrength);
        return new RilDataRaw(&signalStrength_V1_4, sizeof(RIL_SignalStrength_V1_4));
    }

    // HAL_VERSION_CODE(1, 6)
    return new RilDataRaw(&currentSignalStrength, sizeof(RIL_SignalStrength_V1_6));
}

const RilData *MiscDataBuilder::BuildModemInfo(int type, void *data, unsigned int datalen)
{
    int length = sizeof(int) * 2 + datalen;
    RilDataRaw *rildata = NULL;
    char *buf = new char[length];
    if (buf != NULL) {
        ((int *)buf)[0] = type;
        ((int *)buf)[1] = datalen;
        if (datalen > 0) {
            memcpy(&((int *)buf)[2], data, datalen);
        }
        rildata = new RilDataRaw(buf, length);
        delete[] buf;
    }
    return rildata;
}

/**
 * CurrentLinkCapacityEstimateBuilder
 */
CurrentLinkCapacityEstimateBuilder::CurrentLinkCapacityEstimateBuilder(int halVer)
{
    mHalVer = halVer;
}

const RilData* CurrentLinkCapacityEstimateBuilder::Build(int dlCapaKbps, int ulCapaKbps, int secDlCapaKbps, int secUlCapaKbps)
{
    RIL_LinkCapacityEstimate_V1_6 currentLinkCapacityEstimate;
    memset(&currentLinkCapacityEstimate, 0, sizeof(RIL_LinkCapacityEstimate_V1_6));
    currentLinkCapacityEstimate.downlinkCapacityKbps = dlCapaKbps;
    currentLinkCapacityEstimate.uplinkCapacityKbps = ulCapaKbps;
    currentLinkCapacityEstimate.DCNRSecondaryDownlinkCapacityKbps = secDlCapaKbps;
    currentLinkCapacityEstimate.DCNRSecondaryUplinkCapacityKbps = secUlCapaKbps;
    return new RilDataRaw(&currentLinkCapacityEstimate, sizeof(RIL_LinkCapacityEstimate_V1_6));
}

/**
 * NitzTimeBuilder
*/
const RilData *NitzTimeBuilder::BuildNitzTime(int year, int month, int day, int hour,
        int minute, int second, int timeZone, int dayLightAdjust, int64_t age, int64_t timestamp) {
    char buf[100] = {0, };
    snprintf(buf, sizeof(buf)-1,
        "%02d/%02d/%02d,%02d:%02d:%02d%s%02d,%02d",
        year, month, day, hour, minute, second,
        timeZone >= 0 ? "+" : "-",
        abs(timeZone), dayLightAdjust);

    if (age < 0) {
        age = 0;
    }

    if (timestamp < 0) {
        timestamp = OsalUtils::elapsedRealtime();
    }

    RilData *rildata = nullptr;
    if (mHalVer < HAL_VERSION_CODE(2, 0)) {
        rildata = new RilDataString(buf);
    } else {
        using android::hardware::radio::impl::network::V2_0::RIL_NitzTime;
        RIL_NitzTime nitzData = { buf, age, timestamp };
        rildata = new RilDataPlaceHolder<RIL_NitzTime>(nitzData);
    }
    return rildata;
}