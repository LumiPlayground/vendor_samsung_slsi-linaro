/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __DBECCINFOLOADER_H__
#define __DBECCINFOLOADER_H__

#include <libxml/tree.h>
#include "callreqdata.h"

class EmergencyNumberManager;
class NetworkServiceBase;
class SimService;

/**
 * The type of getting database ECC ID.
 */
typedef enum {
    DB_ECC_ID_TYPE_NO_CUSTOMIZATION = 0x00,
    DB_ECC_ID_TYPE_SIM_BASED_ONLY,
    DB_ECC_ID_TYPE_NET_BASED_ONLY,
    DB_ECC_ID_TYPE_SIM_PREFER,
    DB_ECC_ID_TYPE_NET_PREFER,
    DB_ECC_ID_TYPE_SIM_NET_COMBINE,
} DB_ECC_ID_TYPE;

/**
 * The PLMN(SIM or network) which is set database ECC ID.
 */
typedef enum {
    SIM_DB_ECC_INFO_IDX = 0x00,
    NET_DB_ECC_INFO_IDX,
    MAX_DB_ECC_INFO_IDX
} DB_ECC_INFO_IDX;

/**
 * The components of database ECC.
 */
typedef struct
{
    char mcc[MAX_VALID_ICCID_LEN + 1];
    char mnc[MAX_VALID_ICCID_LEN + 1];
    char number[MAX_EMERGENCY_NUMBER_LEN + 1];
    int category;
    int conditions;
} EccItem;

class DbEccInfoLoader {
  private:
    char TAG[MAX_TAG_LEN];
    NetworkServiceBase *mService;
    EmergencyNumberManager *mEmergencyNumberManager;

    /**
     * The last ECC database ID to compare the new ECC database ID.
     * The database ECC ID is set from SIM or network PLMN.
     * The database ECC ID used to load database ECC.
     * For SIM_NET_COMBINE type, two arrays are used.
     */
    char mDbEccId[MAX_DB_ECC_INFO_IDX][MAX_DB_ECC_ID_LEN + 1];


    bool UseDbEccXml();
    void GetAttrValue(xmlNode *entityNode, const xmlChar *attrName, char *store);
    void ParseEccEntries(xmlNode *rootNode, const char *targetMcc, const char *targetMnc,
                         EccInfo *eccInfo, bool isIccid);
    void ReadDbEccUsingXml(const char *targetMcc, const char *targetMnc,
                           EccInfo *eccInfo, bool isIccid);
    void ReadDbEccUsingHeader(const char *targetMcc, const char *targetMnc, EccInfo *eccInfo, bool isIccid);
    void AddEccItem(EccItem *eccItems, int totalEccCount, const char *targetMcc, const char *targetMnc,
                    EccInfo *eccInfo, bool isIccid);
    void UpdateDbEccInfo(const char *dbEccId, EccInfo *eccInfo);

    int GetDbEccIdType();
    std::string GetNetPlmn();
    std::string GetSimPlmnDuringPinLock();
    std::string GetSimPlmn();
    bool IsDisabledESim(SimService *pSimService);
    bool UseLastDbEccId();
    bool UseIccidToPlmn();
    int GetOverallSimPresence();
    void SetFakeSimPresence();
    int GetOverallFakeSimPresence();
    bool HasSimPresence();
    int GetOverallNetConnPresence();
    void SetFakeNetConnPresence();
    int GetOverallFakeNetConnPresence();
    bool HasNetConnPresence();
    bool LoadDbEccInfo(const char *dbEccId, bool hasPresence,
                       char *lastDbEccId, int dbEccInfoIndex,
                       EccInfo *dbEccInfos);
    bool IsValidDbEccId(const char *dbEccId);
    bool NeedLastDbEccId(const char *dbEccId, const char *lastDbEccId);

  public:
    DbEccInfoLoader(NetworkServiceBase *service, EmergencyNumberManager *emergencyNumberManager);
    ~DbEccInfoLoader();
    static const char *EmergencyCategoryToString(int category);
    bool Load(EccInfo *dbEccInfos);
    void ResetDbEccId(bool forceToErase = false);
    void SetSimAppState(int appState);
};
#endif
