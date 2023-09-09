/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __EMERGENCYNUMBERUPDATER_H__
#define __EMERGENCYNUMBERUPDATER_H__

#include "rildef.h"
#include "callreqdata.h"

class EmergencyNumberManager;
class NetworkServiceBase;
class EccInfo;

#pragma pack(1)

typedef struct
{
    /**
     * The emergency number. The character in the number string should
     * only be the dial pad character('0'-'9', '*', or '#'). For example: 911.
     */
    int32_t number_len;
    char number[MAX_EMERGENCY_NUMBER_LEN + 1];

    /* 3-digit Mobile Country Code, 0..999, INT_MAX if unknown  */
    int32_t mcc_len;
    char mcc[MAX_MCC_LEN + 1];

    /**
     * 2 or 3-digit Mobile Network Code, 0..999;
     * the most significant nibble encodes the number of digits - {2, 3, 0 (unset)};
     * INT_MAX if unknown
     */
    int32_t mnc_len;
    char mnc[MAX_MNC_LEN + 1];

    /**
     * The bitfield of @1.4::EmergencyServiceCategory(s).
     * See @1.4::EmergencyServiceCategory for the value of each bit.
     */
    int32_t categories;

    /**
     * The bitfield of @1.4::EmergencyNumberSource(s).
     * See @1.4::EmergencyNumberSource for the value of each bit.
     */
    int32_t sources;

    /**
     * The routing to tell how to handle the call for the corresponding emergency number.
     * See @1.4::EmergencyCallRouting for the value.
     */
    int32_t routing;
} RIL_AimsEmergencyNumber;

typedef struct
{
    /**
     * The number of AimsEmergencyNumberList
     */
    int32_t count;
    RIL_AimsEmergencyNumber AimsEmergencyNumberList[MAX_EMERGENCY_NUMBER_LIST_COUNT * 2];
} RIL_AimsEmergencyNumberList;

#pragma pack()

class EmergencyNumberUpdater {
  private:
    char TAG[MAX_TAG_LEN];
    NetworkServiceBase *mService;
    EmergencyNumberManager *mEmergencyNumberManager;

    /**
     * The list of emergency number to send RILJ.
     * Radio and DB ECC Info are merged into the list.
     */
    RIL_EmergencyNumber mRilEmergencyNumberList[MAX_EMERGENCY_NUMBER_LIST_COUNT * 2];

    /**
     * The list of emergency number to send IMS svc.
     * Radio and DB ECC Info are merged into the list.
     */
    RIL_AimsEmergencyNumberList mAimsEmergencyNumberList;

  private:
    void PrintEccInfo(EccInfo *eccInfo);
    void AddEmergencyNumber(RIL_EmergencyNumber *rilEmergencyNumber,
                            RIL_AimsEmergencyNumber *aimsEmergencyNumber,
                            EmergencyNumber *emergencyNumber);
    void UpdateEmergencyTypeAndRouting(EmergencyNumber *emergencyNumber);
    int AddEmergencyNumberList(RIL_EmergencyNumber *rilEmergencyNumberList,
                               RIL_AimsEmergencyNumber *aimsEmergencyNumberList,
                               EmergencyNumber *tempEmergencyNumber,
                               EccInfo *dbEccInfos, EccInfo *radioEccInfo);

  public:
    EmergencyNumberUpdater(NetworkServiceBase *service, EmergencyNumberManager *emergencyNumberManager);
    ~EmergencyNumberUpdater();
    static bool HasCondition(int conditions, int condition);
    static string ConditionsToString(int conditions);
    int GetRadioEccInfoType(EccInfo *radioEccInfos);
    void Update(EmergencyNumber *tempEmergencyNumber, EccInfo *dbEccInfos, EccInfo *radioEccInfos);
};

#endif
