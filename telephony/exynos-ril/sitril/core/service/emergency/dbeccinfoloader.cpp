/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "dbeccinfoloader.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include "commondef.h"
#include "rillog.h"
#include "networkservicebase.h"
#include "emergencynumbermanager.h"
#include "iccUtil.h"
#include "simservice.h"

#define ECC_DBG false
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) if (ECC_DBG) \
        CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) if (ECC_DBG) \
        CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_WARNING_LOG, format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) if (ECC_DBG) \
        CRilLog::Log(RIL_LOG_CAT_CALL, CRilLog::E_RIL_CRITICAL_LOG, format, ##__VA_ARGS__)

#define NUM_ELEMS(a) (sizeof(a) / sizeof(a)[0])
#define NOT_FOUND -1
#define MCC_DEFAULT           "DEFAULT"
#define MNC_RETAIL            "#R"

#define DEFAULT_ECC_FILE      "/data/vendor/radio/database/DbEcc_DEFAULT.xml"
#define ECC_FILE_LOCATION     "/data/vendor/radio/database/"

#define ENTITY_NAME           "EccEntry"
#define ENTITY_ATTR_MNC       "mnc"
#define ENTITY_ATTR_NUMBER    "number"
#define ENTITY_ATTR_CATEGORY  "category"
#define ENTITY_ATTR_CONDITION "condition"

#define MAX_ATTR_VALUE_LEN 20
#define MAX_FILE_NAME_LEN 255

#define RIL_OVERALL_FAKE_SIM_PRESENCE      "vendor.ril.app.overall.fake.sim.presence"
#define RIL_OVERALL_FAKE_NET_CONN_PRESENCE "vendor.ril.app.overall.fake.net.conn.presence"

/**
 * Check if the file exists.
 *
 * @param filename the file name including location.
 * @return true if the file exists.
 */
inline bool Exists(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

/*
 * Enum Category and Condition.
 *        - Category: the service category
 *            RIL_EMERGENCY_CATEGORY_UNSPECIFIED     = 0
 *            RIL_EMERGENCY_CATEGORY_POLICE          = 1 << 0
 *            RIL_EMERGENCY_CATEGORY_AMBULANCE       = 1 << 1
 *            RIL_EMERGENCY_CATEGORY_FIRE_BRIGADE    = 1 << 2
 *            RIL_EMERGENCY_CATEGORY_MARINE_GUARD    = 1 << 3
 *            RIL_EMERGENCY_CATEGORY_MOUNTAIN_RESCUE = 1 << 4
 *            RIL_EMERGENCY_CATEGORY_MIEC            = 1 << 5
 *            RIL_EMERGENCY_CATEGORY_AIEC            = 1 << 6
 *        - Condition: there are following values for emergency routing
 *                     except CONDITION_TESTMODE_NORMAL and CONDITION_UI_ONLY
 *            CONDITION_ALWAYS                0b00000001
 *            CONDITION_NO_SIM                0b00000010
 *            CONDITION_TESTMODE_NORMAL       0b00000100
 *            CONDITION_NOT_IMS_REGI          0b00001000
 *            CONDITION_UI_ONLY               0b00010000
 *            CONDITION_EMC_AVAILABLE         0b00100000
 *            CONDITION_NOT_CS_REGI           0b01000000
 *            CONDITION_BOTH_NOT_REGI         0b10000000
 */

/**
 * This has all ECC items from DbEcc header files.
 */
static EccItem eccItemsByMcc[] = {
#include "header/DbEcc_202.h"
#include "header/DbEcc_204.h"
#include "header/DbEcc_206.h"
#include "header/DbEcc_208.h"
#include "header/DbEcc_214.h"
#include "header/DbEcc_216.h"
#include "header/DbEcc_219.h"
#include "header/DbEcc_222.h"
#include "header/DbEcc_226.h"
#include "header/DbEcc_228.h"
#include "header/DbEcc_230.h"
#include "header/DbEcc_231.h"
#include "header/DbEcc_232.h"
#include "header/DbEcc_234.h"
#include "header/DbEcc_235.h"
#include "header/DbEcc_250.h"
#include "header/DbEcc_255.h"
#include "header/DbEcc_260.h"
#include "header/DbEcc_262.h"
#include "header/DbEcc_268.h"
#include "header/DbEcc_272.h"
#include "header/DbEcc_276.h"
#include "header/DbEcc_297.h"
#include "header/DbEcc_330.h"
#include "header/DbEcc_334.h"
#include "header/DbEcc_338.h"
#include "header/DbEcc_370.h"
#include "header/DbEcc_372.h"
#include "header/DbEcc_374.h"
#include "header/DbEcc_404.h"
#include "header/DbEcc_405.h"
#include "header/DbEcc_425.h"
#include "header/DbEcc_440.h"
#include "header/DbEcc_441.h"
#include "header/DbEcc_450.h"
#include "header/DbEcc_460.h"
#include "header/DbEcc_466.h"
#include "header/DbEcc_505.h"
#include "header/DbEcc_525.h"
#include "header/DbEcc_704.h"
#include "header/DbEcc_706.h"
#include "header/DbEcc_708.h"
#include "header/DbEcc_710.h"
#include "header/DbEcc_712.h"
#include "header/DbEcc_714.h"
#include "header/DbEcc_716.h"
#include "header/DbEcc_722.h"
#include "header/DbEcc_724.h"
#include "header/DbEcc_730.h"
#include "header/DbEcc_732.h"
#include "header/DbEcc_734.h"
#include "header/DbEcc_736.h"
#include "header/DbEcc_738.h"
#include "header/DbEcc_740.h"
#include "header/DbEcc_744.h"
#include "header/DbEcc_746.h"
#include "header/DbEcc_748.h"
#include "header/DbEcc_750.h"
#include "header/DbEcc_DEFAULT.h"
};

/**
 * This has ECC items from DbEcc_ICCID header file.
 */
static EccItem eccItemsByIccid[] = {
#include "header/DbEcc_ICCID.h"
};

/**
 * Check if whether XML database is used.
 * The property is set in device-vendor.mk.
 *
 * @return true if the property is set as 1.
 */
bool DbEccInfoLoader::UseDbEccXml() {
    bool ret = SystemProperty::GetInt("persist.vendor.ril.ecc.use.xml", 0) == 1;
    RilLogV("[%s] %s() Use DB ECC XML: %s.", TAG, __FUNCTION__, (ret ? "true" : "false"));
    return ret;
}

/**
 * Convert emergency category value to string.
 *
 * @param category emergency category value. Refer to @1.4::RIL_EmergencyServiceCategory.
 */
const char *DbEccInfoLoader::EmergencyCategoryToString(int category) {
    switch (category) {
    case RIL_EMERGENCY_CATEGORY_POLICE:
        return "CATEGORY_POLICE";
    case RIL_EMERGENCY_CATEGORY_AMBULANCE:
        return "CATEGORY_AMBULANCE";
    case RIL_EMERGENCY_CATEGORY_FIRE_BRIGADE:
        return "CATEGORY_FIRE_BRIGADE";
    case RIL_EMERGENCY_CATEGORY_MARINE_GUARD:
        return "CATEGORY_MARINE_GUARD";
    case RIL_EMERGENCY_CATEGORY_MOUNTAIN_RESCUE:
        return "CATEGORY_MOUNTAIN_RESCUE";
    case RIL_EMERGENCY_CATEGORY_MIEC:
        return "CATEGORY_MIEC";
    case RIL_EMERGENCY_CATEGORY_AIEC:
        return "CATEGORY_AIEC";
    default:
        return "CATEGORY_UNSPECIFIED";
    }
}

static const char *DbEccIdTypeToString(int dbEccIdType) {
    static const char *DB_ECC_ID_TYPE[] = {"NO_CUSTOMIZATION", "SIM_BASED_ONLY", "NET_BASED_ONLY",
                                           "SIM_PREFER", "NET_PREFER", "SIM_NET_COMBINE"};
    if (DB_ECC_ID_TYPE_NO_CUSTOMIZATION <= dbEccIdType &&
        dbEccIdType <= DB_ECC_ID_TYPE_SIM_NET_COMBINE) {
        return DB_ECC_ID_TYPE[dbEccIdType];
    }
    return "UNKNOWN";
}

DbEccInfoLoader::DbEccInfoLoader(NetworkServiceBase *service,
                                 EmergencyNumberManager *emergencyNumberManager)
    : mService(NULL) {
    mService = service;
    mEmergencyNumberManager = emergencyNumberManager;
    snprintf(TAG, sizeof(TAG) - 1, "DbEccInfoLoader_%d", mService->GetRilSocketId());

    ResetDbEccId(true /*forceToErase*/);

    RilLogI("[%s] %s() UseLastDbEccId: %s, DbEccIdType: %s, useIccidToPlmn: %s.", TAG, __FUNCTION__,
            (UseLastDbEccId() ? "true" : "false"), DbEccIdTypeToString(GetDbEccIdType()),
            (UseIccidToPlmn() ? "true" : "false"));
}

DbEccInfoLoader::~DbEccInfoLoader() {
    mService = NULL;
    mEmergencyNumberManager = NULL;
}

/**
 * Add ECC items by MCC and MNC.
 *
 * @param eccItems the list of candidate ECC items.
 * @param totalEccCount the number of eccItems.
 * @param targetMcc the MCC to be found.
 * @param targetMnc the MNC to be found.
 * @param eccInfo It is the place where the loaded ECC item is added.
 * @param isIccid ECC Items of ICCID are generally configured differently.
                  Since the MNC length is 3 to 5, it is a flag to consider this.
 */
void DbEccInfoLoader::AddEccItem(EccItem *eccItems, int totalEccCount, const char *targetMcc,
                             const char *targetMnc, EccInfo *eccInfo, bool isIccid) {
    int i;
    int start_mcc = NOT_FOUND;
    int end_mcc = NOT_FOUND;
    int start_mnc = NOT_FOUND;
    unsigned int mncLen = isIccid ? MAX_VALID_ICCID_LEN : MAX_MNC_LEN;

    /*  1. Find start and end index of ECC items by target MCC. */
    for (i = 0; i < totalEccCount; i++) {
        // find index of mcc.
        if (start_mcc == NOT_FOUND && (strcmp(eccItems[i].mcc, targetMcc) == 0)) {
            start_mcc = i;
        }

        if (start_mcc != NOT_FOUND && end_mcc == NOT_FOUND &&
            (strcmp(eccItems[i].mcc, targetMcc) != 0)) {
            end_mcc = i;
            break;
        }
    }

    //  In case of XML database, end_mcc is set as NOT_FOUND.
    //  So last index is set.
    if (end_mcc == NOT_FOUND) {
        end_mcc = totalEccCount;
    }

    /* 2. Road default ECC list if target MCC is not found. */
    if (start_mcc == NOT_FOUND) {
        RilLogI("[%s] %s() target MCC is not found. Use default list.", TAG, __FUNCTION__);
        for (i = 0; i < totalEccCount; i++) {
            if (strncmp(MCC_DEFAULT, eccItems[i].mcc, strlen(eccItems[i].mcc)) == 0) {
                eccInfo->AddEmergencyNumber(targetMcc, targetMnc,
                                            eccItems[i].number, strlen(eccItems[i].number),
                                            eccItems[i].category, eccItems[i].conditions,
                                            RIL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG);
            }
        }
        return;
    }

    /*  3. Find the ECC items by MNC if MCC is found. */
    do {
        for (i = start_mcc; i < end_mcc; i++) {
            if (strlen(eccItems[i].mnc) == mncLen && strncmp(eccItems[i].mnc, targetMnc, mncLen) == 0) {
                start_mnc = i;
                eccInfo->AddEmergencyNumber(targetMcc, targetMnc,
                                            eccItems[i].number, strlen(eccItems[i].number),
                                            eccItems[i].category, eccItems[i].conditions,
                                            RIL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG);
            } else if (start_mnc != NOT_FOUND) {
                //  It proceeds in consideration of the fact that
                //  the ECC items are grouped into the same MNC.
                //  If the MNC was found and the MNC is not matched after that,
                //  (MNC value is not found, but different.)
                //  there is no longer the same MNC.
                break;
            }
        }
    } while (isIccid && start_mnc == NOT_FOUND && --mncLen > 2);  //  MIN MNC len is 2.

    /* 4. Road retail ECC list if MNC is not found. */
    if (start_mnc == NOT_FOUND) {
        RilLogI("[%s] %s() target MNC is not found. Use retail list.", TAG, __FUNCTION__);
        for (i = start_mcc; i < end_mcc; i++) {
            if (strncmp(MNC_RETAIL, eccItems[i].mnc, strlen(eccItems[i].mnc)) == 0) {
                eccInfo->AddEmergencyNumber(targetMcc, targetMnc,
                                            eccItems[i].number, strlen(eccItems[i].number),
                                            eccItems[i].category, eccItems[i].conditions,
                                            RIL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG);
            }
        }
    }
}

/**
 * Get the value from specific attribute in XML node.
 *
 * @param entityNode an entity node which is to be read.
 * @param attrName the name of attribute.
 * @param store the place where the value stored.
 */
void DbEccInfoLoader::GetAttrValue(xmlNode *entityNode, const xmlChar *attrName, char *store) {
    struct _xmlAttr *pAtt = entityNode->properties;
    xmlChar *p_prop;
    while (pAtt != NULL) {
        if (!xmlStrcmp(pAtt->name, attrName)) {
            p_prop = xmlGetProp(entityNode, pAtt->name);
            int len = MIN(strlen((char *)p_prop), MAX_ATTR_VALUE_LEN);
            strncpy(store, (char *)p_prop, len);
            store[len] = '\0';
            xmlFree(p_prop);
            return;
        }
        pAtt = pAtt->next;
    }
}

/**
 * While searching the xml node, the emergency number information stored in the entity node
 * is read and stored.
 *
 * @param rootNode
 * @param targetMcc the MCC to be found.
 * @param targetMnc the MNC to be found.
 * @param eccInfo It is the place where the loaded ECC item is added.
 * @param isIccid ECC Items of ICCID are generally configured differently.
                  Since the MNC length is 3 to 5, it is a flag to consider this.
 */
void DbEccInfoLoader::ParseEccEntries(xmlNode *rootNode, const char *targetMcc, const char *targetMnc,
                                  EccInfo *eccInfo, bool isIccid) {
    int countEccItem = 0;
    xmlNode *cur_node = NULL;
    cur_node = rootNode->children;

    EccItem eccItemsByXml[MAX_EMERGENCY_NUMBER_LIST_COUNT];

    while (cur_node != NULL) {
        if (cur_node->type == XML_ELEMENT_NODE && (!xmlStrcmp(cur_node->name, (const xmlChar *)ENTITY_NAME))) {
            char mnc[MAX_ATTR_VALUE_LEN + 1] = {0, };
            char number[MAX_ATTR_VALUE_LEN + 1] = {0, };
            char category[MAX_ATTR_VALUE_LEN + 1] = {0, };
            char con[MAX_ATTR_VALUE_LEN + 1] = {0, };
            GetAttrValue(cur_node, (const xmlChar *)ENTITY_ATTR_MNC, mnc);
            GetAttrValue(cur_node, (const xmlChar *)ENTITY_ATTR_NUMBER, number);
            GetAttrValue(cur_node, (const xmlChar *)ENTITY_ATTR_CATEGORY, category);
            GetAttrValue(cur_node, (const xmlChar *)ENTITY_ATTR_CONDITION, con);
            EccItem *eccItem = &eccItemsByXml[countEccItem];
            strncpy(eccItem->mcc, targetMcc, strlen(targetMcc));
            UINT32 mncLen = MIN(strlen(mnc), (sizeof(eccItem->mcc) - 1));
            strncpy(eccItem->mnc, mnc, mncLen);
            strncpy(eccItem->number, number, strlen(number));
            eccItem->mcc[strlen(targetMcc)] = 0;
            eccItem->mnc[mncLen] = 0;
            eccItem->number[strlen(number)] = 0;
            eccItem->category = strtol(category, NULL, 10);
            eccItem->conditions = (int)strtol(con, NULL, 2);
            RilLogV("[%s] %s() EccItem MCC: %s, MNC: %s, number: %s, category: %s, conditions: [ %s].",
                    TAG, __FUNCTION__, eccItem->mcc, eccItem->mnc, eccItem->number,
                    EmergencyCategoryToString(eccItem->category),
                    EmergencyNumberUpdater::ConditionsToString(eccItem->conditions).c_str());
            countEccItem++;
        }
        cur_node = cur_node->next;
    }
    AddEccItem(eccItemsByXml, countEccItem, targetMcc, targetMnc, eccInfo, isIccid);
}

/**
 * Read the database using XML file.
 *
 * @param targetMcc the MCC to be found.
 * @param targetMnc the MNC to be found.
 * @param eccInfo the loaded ECC item is added.
 * @param isIccid ECC Items of ICCID are generally configured differently.
                  The database is differently choosen by ICCID.
 */
void DbEccInfoLoader::ReadDbEccUsingXml(const char *targetMcc, const char *targetMnc,
                                    EccInfo *eccInfo, bool isIccid) {
    char filename[MAX_FILE_NAME_LEN + 1];

    if (isIccid) {
        sprintf(filename, ECC_FILE_LOCATION"DbEcc_ICCID.xml");
    } else {
        sprintf(filename, ECC_FILE_LOCATION"DbEcc_%s.xml", targetMcc);
    }
    RilLogI("[%s] %s() filename: %s.", TAG, __FUNCTION__, filename);
    if (!Exists(filename)) {
        sprintf(filename, DEFAULT_ECC_FILE);
        RilLogE("[%s] %s() Use default ecc list.", TAG, __FUNCTION__);
    }

    xmlDoc *doc;
    xmlNode *root_element;

    doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        RilLogE("[%s] %s() error: could not parse %s file.", TAG, __FUNCTION__, filename);
        return;
    }
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    if (NULL != root_element) {
        ParseEccEntries(root_element, targetMcc, targetMnc, eccInfo, isIccid);
    }

    xmlFreeDoc(doc);
}

/**
 * Read the database using header file.
 *
 * @param targetMcc the MCC to be found.
 * @param targetMnc the MNC to be found.
 * @param eccInfo the loaded ECC item is added.
 * @param isIccid ECC Items of ICCID are generally configured differently.
                  The database is differently choosen by ICCID.
 */
void DbEccInfoLoader::ReadDbEccUsingHeader(const char *targetMcc, const char *targetMnc,
                                       EccInfo *eccInfo, bool isIccid) {
    if (isIccid) {
        AddEccItem(eccItemsByIccid, (int)NUM_ELEMS(eccItemsByIccid),
                   targetMcc, targetMnc, eccInfo, isIccid);
    } else {
        AddEccItem(eccItemsByMcc, (int)NUM_ELEMS(eccItemsByMcc),
                   targetMcc, targetMnc, eccInfo, isIccid);
    }
}

/**
 * Find the database that matches the DB ECC ID and get the ECC info.
 *
 * @param dbEccId the key to find database. It is SIM or network PLMN.
 * @param eccInfo the loaded ECC item is added.
 * @param socketId the socket ID for log.
 */
void DbEccInfoLoader::UpdateDbEccInfo(const char *dbEccId, EccInfo *eccInfo) {
    if (TextUtils::IsEmpty(dbEccId)) {
        RilLogE("[%s] %s() DB ECC ID is empty.", TAG, __FUNCTION__);
        return;
    }

    char targetMcc[MAX_VALID_ICCID_LEN + 1] = {0, };
    char targetMnc[MAX_VALID_ICCID_LEN + 1] = {0, };

    int prefixIccIdLen = strlen(PREFIX_ICCID);
    bool isIccid = (strncmp(dbEccId, PREFIX_ICCID, prefixIccIdLen) == 0);
    bool isDefault = (strncmp(dbEccId, DEFAULT_DB_ECC, strlen(DEFAULT_DB_ECC)) == 0);
    if (isIccid) {
        strncpy(targetMcc, "ICCID", MAX_VALID_ICCID_LEN);
        strncpy(targetMnc, dbEccId + prefixIccIdLen, strlen(dbEccId) - prefixIccIdLen);
    } else if (isDefault) {
        //  Set "DEF" to MCC and MNC.
        strncpy(targetMcc, dbEccId, MAX_MCC_LEN);
        strncpy(targetMnc, dbEccId, MAX_MNC_LEN);
    } else {
        strncpy(targetMcc, dbEccId, MAX_MCC_LEN);
        strncpy(targetMnc, dbEccId + MAX_MCC_LEN, MAX_MNC_LEN);
        if (targetMnc[MAX_MNC_LEN - 1] == '#') {
            targetMnc[MAX_MNC_LEN - 1] = 0;
        }
    }

    eccInfo->SetCount(0);

    RilLogI("[%s] %s() DB ECC ID: %s, isIccid: %s, target MCC: %s, target MNC: %s.",
            TAG, __FUNCTION__, dbEccId, (isIccid ? "true" : "false"), targetMcc, targetMnc);
    if (UseDbEccXml()) {
        ReadDbEccUsingXml(targetMcc, targetMnc, eccInfo, isIccid);
    } else {
        ReadDbEccUsingHeader(targetMcc, targetMnc, eccInfo, isIccid);
    }
}

/**
 * The type of PLMN which is used to select database ECC ID.
 * Refer to DB_ECC_ID_TYPE enum.
 *
 * @return dbEccIdType the value of @DB_ECC_ID_TYPE enum.
 */
int DbEccInfoLoader::GetDbEccIdType() {
    int dbEccIdType = SystemProperty::GetInt(VENDOR_RIL_DB_ECC_ID_TYPE,
                                             DB_ECC_ID_TYPE_NET_PREFER);
    RilLogV("[%s] %s() DB ECC ID type: %s", TAG, __FUNCTION__, DbEccIdTypeToString(dbEccIdType));
    return dbEccIdType;
}

/**
 * Get the network PLMN from RIL_CONTEXT_NET_CURRENT_PLMN property.
 *
 * @return the network PLMN.
 */
std::string DbEccInfoLoader::GetNetPlmn() {
    string netPlmn = "";
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL) {
        netPlmn = property->GetString(RIL_CONTEXT_NET_CURRENT_PLMN);
    }
    return netPlmn;
}

/**
 * The mapping table of ICCID and PLMN.
 */
map<string, string> ICCID_TO_PLMN_MAP = {
#include "iccid_to_plmn.h"
};

/**
 * Get the SIM PLMN for DB ECC ID while PIN lock.
 *
 * @return PLMN from ICCID_TO_PLMN_MAP, if VENDOR_RIL_DB_ECC_USE_ICCID_TO_PLMN is set.
 *         PREFIX_ICCID and 5 digits of ICCID, if it is not set.
 */
std::string DbEccInfoLoader::GetSimPlmnDuringPinLock() {
    bool useIccidToPlmn = UseIccidToPlmn();

    //  Get ICCID from RIL_CONTEXT_SIM_ICC_ID property.
    string iccid = "";
    RilProperty *property = mService->GetRilContextProperty();
    if (property != NULL) {
        iccid = property->GetString(RIL_CONTEXT_SIM_ICC_ID);
    }
    if (iccid.length() < MAX_VALID_ICCID_LEN) {
        RilLogE("[%s] %s ICCID length is invalid.", TAG, __FUNCTION__);
        // PREFIX_ICCID will sent Retail ECC list.
        return PREFIX_ICCID;
    }

    int iccidLen = MAX_VALID_ICCID_LEN;
    RilLogV("[%s] %s ICCID: %s.", TAG, __FUNCTION__, iccid.substr(0, iccidLen).c_str());

    if (useIccidToPlmn) {
        map<string, string>::iterator iter;

        //  ICCID_TO_PLMN_MAP has 4~5 digits.
        //  Search 5 digits and then 4 digits.
        iter = ICCID_TO_PLMN_MAP.find(iccid.substr(0, iccidLen));
        if (iter == ICCID_TO_PLMN_MAP.end()) {
            iter = ICCID_TO_PLMN_MAP.find(iccid.substr(0, --iccidLen));
        }

        if (iter != ICCID_TO_PLMN_MAP.end()) {
            RilLogV("[%s] %s Found PLMN: %s.", TAG, __FUNCTION__, (iter->second).c_str());
            return iter->second;
        }
    } else {
        return PREFIX_ICCID + iccid.substr(0, iccidLen);
    }

    // PREFIX_ICCID will sent Retail ECC list.
    return PREFIX_ICCID;
}

/**
 * Get the SIM PLMN regarding PIN lock.
 *
 * @return the SIM PLMN.
 */
std::string DbEccInfoLoader::GetSimPlmn() {
    string strSimPlmn = mService->GetSimOperatorNumeric();

    int overallSimPresence = GetOverallSimPresence();
    int socketId = mService->GetRilSocketId();
    int isPresent = (overallSimPresence & (0x01 << socketId)) >> socketId;
    int appState = mEmergencyNumberManager->GetAppState();

    if (isPresent) {
        if (appState == RIL_APPSTATE_PIN) {
            strSimPlmn.clear();
            strSimPlmn = GetSimPlmnDuringPinLock();
        }
        RilLogV("[%s] %s() app state: %s, SIM PLMN: %s.", TAG, __FUNCTION__,
                IccUtil::covertAppStateToString((RIL_AppState)appState).c_str(), strSimPlmn.c_str());
        return strSimPlmn;
    } else {
        RilLogV("[%s] %s() SIM is absent.", TAG, __FUNCTION__);
    }
    return "";
}

/**
 * Check if eSIM is disabled on target service
 *
 * @return true if num_applications of card status is 0.
 */
bool DbEccInfoLoader::IsDisabledESim(SimService *pSimService) {
    bool isDisabledESim = false;
    if (pSimService != NULL) {
        const V2_0::RIL_CardStatus &cardStatus = pSimService->GetRilCardStatus();
        //  num_applications == 0 indicates disabled eSIM.
        isDisabledESim = (cardStatus.num_applications == 0);
    }
    RilLogV("[%s] %s() eSIM is disabled: %s.", TAG, __FUNCTION__, (isDisabledESim ? "true" : "false"));
    return isDisabledESim;
}

/**
 * Check if last database ECC ID is used.
 *
 * @return true if VENDOR_RIL_DB_ECC_USE_LAST_ID property is set as 1.
 */
bool DbEccInfoLoader::UseLastDbEccId() {
    bool useLastDbEccId = (SystemProperty::GetInt(VENDOR_RIL_DB_ECC_USE_LAST_ID, 0) == 1);
    //  RilLogV("[%s] %s() Use last DB ECC ID: %s.", TAG, __FUNCTION__, (useLastDbEccId ? "true" : "false"));
    return useLastDbEccId;
}

/**
 * Check if ICCID to PLMN table is used.
 *
 * @return true if VENDOR_RIL_DB_ECC_USE_ICCID_TO_PLMN property is set as 1.
 */
bool DbEccInfoLoader::UseIccidToPlmn() {
    bool useIccidToPlmn = (SystemProperty::GetInt(VENDOR_RIL_DB_ECC_USE_ICCID_TO_PLMN, 0) == 1);
    RilLogV("[%s] %s() Use ICCID to PLMN: %s.", TAG, __FUNCTION__, (useIccidToPlmn ? "true" : "false"));
    return useIccidToPlmn;
}

/**
 * Get the presence of SIM on both stacks.
 * Additionally the disabled state of eSIM is checked.
 *
 * The bitmask of overallSimPresence.
 * bit 0 : For SIM1, bit 1 : For SIM2
 * 0 : No validation of SIM PLMN on both stacks
 * 1 : The validation of SIM PLMN on SIM1 stack only
 * 2 : The validation of SIM PLMN on SIM2 stack only
 * 3 : The validation of SIM PLMN on both stacks
 *
 * @return the bitmask of overallSimPresence.
 */
int DbEccInfoLoader::GetOverallSimPresence() {
    int overallSimPresence = 0;
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        overallSimPresence = pRilAppProperty->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
        for (int i = 0; i < SIM_COUNT; i++) {
            if (overallSimPresence >> i & 0x01) {
                SimService *pSimService = NULL;
                if (i == mService->GetRilSocketId()) {
                    pSimService = (SimService *)mService->GetCurrentService(RIL_SERVICE_SIM);
                } else {
                    pSimService = (SimService *)mService->GetOppositeService(RIL_SERVICE_SIM);
                }
                if (IsDisabledESim(pSimService)) {
                    overallSimPresence &= ~(0x01 << i);
                }
            }
        }
    }
    RilLogV("[%s] %s() overallSimPresence=0x%02X.", TAG, __FUNCTION__, overallSimPresence);
    return overallSimPresence;
}

/**
 * Set the fake presence of SIM to maintain last database ECC ID on current stack,
 * when UseLastDbEccId() is true.
 * It helps to make empty emergency number list on other stack.
 * RIL_OVERALL_FAKE_SIM_PRESENCE property is used.
 */
void DbEccInfoLoader::SetFakeSimPresence() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    int overallFakeSimPresence = 0;
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        overallFakeSimPresence = pRilAppProperty->GetInt(RIL_OVERALL_FAKE_SIM_PRESENCE, 0);
        pRilAppProperty->Put(RIL_OVERALL_FAKE_SIM_PRESENCE,
                             overallFakeSimPresence |= 0x01 << mService->GetRilSocketId());
    }
}

/**
 * Get the fake presence of SIM on both stacks.
 * The consist of bitmask of overallFakeSimPresence is same with overallSimPresence.
 * See the above annotation of "The bitmask of overallSimPresence".
 *
 * @return bitmask of the both fake SIM presence.
 */
int DbEccInfoLoader::GetOverallFakeSimPresence() {
    int overallFakeSimPresence = 0;
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        overallFakeSimPresence = pRilAppProperty->GetInt(RIL_OVERALL_FAKE_SIM_PRESENCE, 0);
    }
    RilLogV("[%s] %s() overallFakeSimPresence=0x%02X.", TAG, __FUNCTION__, overallFakeSimPresence);
    return overallFakeSimPresence;
}

/**
 * Check if it has one SIM presence at least.
 *
 * @return true if overallSimPresence is not 0.
 */
bool DbEccInfoLoader::HasSimPresence() {
    int overallSimPresence = GetOverallSimPresence() | GetOverallFakeSimPresence();
    RilLogV("[%s] %s() overallSimPresence=0x%02X.", TAG, __FUNCTION__, overallSimPresence);

    if (overallSimPresence == 0) {
        return false;
    }
    return true;
}

/**
 * Get the connection presence of network on both stacks.
 *
 * The bitmask of overallNetConnPresence.
 * bit 0 : For SIM1, bit 1 : For SIM2
 * 0 : No connection presence of network on both stacks
 * 1 : The connection presence of network on SIM1 stack only
 * 2 : The connection presence of network on SIM2 stack only
 * 3 : The connection presence of network on both stacks
 *
 * @return the bitmask of overallNetConnPresence.
 */
int DbEccInfoLoader::GetOverallNetConnPresence() {
    int overallNetConnPresence = 0;
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        overallNetConnPresence = pRilAppProperty->GetInt(RIL_OVERALL_NET_CONN_PRESENCE, 0);
    }
    RilLogV("[%s] %s() overallNetConnPresence=0x%02X.", TAG, __FUNCTION__, overallNetConnPresence);
    return overallNetConnPresence;
}

/**
 * Set the fake connection presence of network to maintain last database ECC ID on current stack,
 * when UseLastDbEccId() is true.
 * It helps to make empty emergency number list on other stack.
 * RIL_OVERALL_FAKE_NET_CONN_PRESENCE property is used.
 */
void DbEccInfoLoader::SetFakeNetConnPresence() {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    int overallFakeNetConnPresence = 0;
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        overallFakeNetConnPresence = pRilAppProperty->GetInt(RIL_OVERALL_FAKE_NET_CONN_PRESENCE, 0);
        pRilAppProperty->Put(RIL_OVERALL_FAKE_NET_CONN_PRESENCE,
                             overallFakeNetConnPresence |= 0x01 << mService->GetRilSocketId());
    }
}

/**
 * Get the fake connection presence of network on both stacks.
 * The consist of bitmask of overallFakeNetConnPresence is same with overallNetConnPresence.
 * See the above annotation of "The bitmask of overallNetConnPresence".
 *
 * @return bitmask of the both fake connection presence of network.
 */
int DbEccInfoLoader::GetOverallFakeNetConnPresence() {
    int overallFakeNetConnPresence = 0;
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        overallFakeNetConnPresence = pRilAppProperty->GetInt(RIL_OVERALL_FAKE_NET_CONN_PRESENCE, 0);
    }
    RilLogV("[%s] %s() overallFakeNetConnPresence=0x%02X.", TAG, __FUNCTION__, overallFakeNetConnPresence);
    return overallFakeNetConnPresence;
}

/**
 * Check if it has one network connection presence at least.
 *
 * @return true if overallNetConnPresence is not 0.
 */
bool DbEccInfoLoader::HasNetConnPresence() {
    int overallNetConnPresence = GetOverallNetConnPresence() | GetOverallFakeNetConnPresence();
    RilLogV("[%s] %s() overallNetConnPresence=0x%02X.", TAG, __FUNCTION__, overallNetConnPresence);

    if (overallNetConnPresence == 0) {
        return false;
    }
    return true;
}

/**
 * Load database ECC info from database.
 *
 * @param dbEccId current DB ECC ID
 * @param hasPresence It is used to set default or empty emergency number list.
 * @param lastDbEccId previous DB ECC ID, It helps to prevent duplicate update.
 * @param dbEccInfoIndex the index of DB ECC info.
 * @param dbEccInfos The ECC INFO using database.
 * @return needUpdate true if database is loaded. false if it is duplicate update.
 */
bool DbEccInfoLoader::LoadDbEccInfo(const char *dbEccId, bool hasPresence,
                                    char *lastDbEccId, int dbEccInfoIndex,
                                    EccInfo *dbEccInfos) {
    RilLogI("[%s] %s() %s DB ECC ID: %-7s, hasPresence: %-5s, last DB ECC ID: %s",
            TAG, __FUNCTION__, (dbEccInfoIndex == 0 ? "SIM" : "NET"), dbEccId,
            (hasPresence ? "true" : "false"), lastDbEccId);
    bool needUpdate = false;
    int dbEccIdLen = 0;

    //  Invalid DB ECC ID case.
    if (!IsValidDbEccId(dbEccId) || !strncmp(dbEccId, INVALID_PLMN, MAX_DB_ECC_ID_LEN)) {
        //  DEFAULT LIST
        if (hasPresence == false) {
            //  prevent duplicate update
            if (lastDbEccId[0] == 0 ||
                strncmp(lastDbEccId, DEFAULT_DB_ECC, MAX_DB_ECC_ID_LEN) != 0) {
                RilLogI("[%s] %s() Load default DB ECC.", TAG, __FUNCTION__);
                memset(lastDbEccId, 0, MAX_DB_ECC_ID_LEN);
                strncpy(lastDbEccId, DEFAULT_DB_ECC, strlen(DEFAULT_DB_ECC));
                dbEccInfos[dbEccInfoIndex].Clear();
                UpdateDbEccInfo(lastDbEccId, &dbEccInfos[dbEccInfoIndex]);
                needUpdate = true;
            }
        // EMPTY LIST
        } else {
            //  prevent duplicate update
            if (lastDbEccId[0] == 0 ||
                strncmp(lastDbEccId, EMPTY_DB_ECC, MAX_DB_ECC_ID_LEN) != 0) {
                RilLogI("[%s] %s() Load empty DB ECC.", TAG, __FUNCTION__);
                memset(lastDbEccId, 0, MAX_DB_ECC_ID_LEN);
                strncpy(lastDbEccId, EMPTY_DB_ECC, strlen(EMPTY_DB_ECC));
                dbEccInfos[dbEccInfoIndex].Clear();
                needUpdate = true;
            }
        }
    //  Valid DB ECC ID case.
    } else if (NULL != dbEccId && strncmp(dbEccId, lastDbEccId, MAX_DB_ECC_ID_LEN)) {
        dbEccIdLen = strlen(dbEccId);
        RilLogI("[%s] %s() Load DB ECC ID: %s.", TAG, __FUNCTION__, dbEccId);
        memset(lastDbEccId, 0, MAX_DB_ECC_ID_LEN);
        strncpy(lastDbEccId, dbEccId, (dbEccIdLen > MAX_DB_ECC_ID_LEN ? MAX_DB_ECC_ID_LEN : dbEccIdLen));
        dbEccInfos[dbEccInfoIndex].Clear();
        UpdateDbEccInfo(lastDbEccId, &dbEccInfos[dbEccInfoIndex]);
        needUpdate = true;
    }

    return needUpdate;
}

/**
 * Update ECC Info from database.
 *
 * @param dbEccInfos The ECC INFO using database.
 * @return needUpdate true if database is loaded. false if it is duplicate update.
 */
bool DbEccInfoLoader::Load(EccInfo *dbEccInfos) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);

    bool needUpdate = false;
    //  See DB_ECC_INFO_IDX.
    //  Empty list is set as default.
    const char *dbEccIds[MAX_DB_ECC_INFO_IDX] = {NULL, NULL};
    bool hasPresences[MAX_DB_ECC_INFO_IDX] = {true, true};

    string netPlmn = GetNetPlmn();
    string simPlmn = GetSimPlmn();
    int dbEccIdType = GetDbEccIdType();

    switch (dbEccIdType) {
    case DB_ECC_ID_TYPE_SIM_BASED_ONLY:
        dbEccIds[SIM_DB_ECC_INFO_IDX] = simPlmn.c_str();
        hasPresences[SIM_DB_ECC_INFO_IDX] = HasSimPresence();

        if (NeedLastDbEccId(dbEccIds[SIM_DB_ECC_INFO_IDX], mDbEccId[SIM_DB_ECC_INFO_IDX])) {
            dbEccIds[SIM_DB_ECC_INFO_IDX] = mDbEccId[SIM_DB_ECC_INFO_IDX];
            SetFakeSimPresence();
        }
        break;
    case DB_ECC_ID_TYPE_NET_BASED_ONLY:
        dbEccIds[NET_DB_ECC_INFO_IDX] = netPlmn.c_str();
        hasPresences[NET_DB_ECC_INFO_IDX] = HasNetConnPresence();

        if (NeedLastDbEccId(dbEccIds[NET_DB_ECC_INFO_IDX], mDbEccId[NET_DB_ECC_INFO_IDX])) {
            dbEccIds[NET_DB_ECC_INFO_IDX] = mDbEccId[NET_DB_ECC_INFO_IDX];
            SetFakeNetConnPresence();
        }
        break;
    case DB_ECC_ID_TYPE_SIM_PREFER:
        dbEccIds[SIM_DB_ECC_INFO_IDX] = simPlmn.c_str();
        hasPresences[SIM_DB_ECC_INFO_IDX] = HasSimPresence();

        if (NeedLastDbEccId(dbEccIds[SIM_DB_ECC_INFO_IDX], mDbEccId[SIM_DB_ECC_INFO_IDX])) {
            dbEccIds[SIM_DB_ECC_INFO_IDX] = mDbEccId[SIM_DB_ECC_INFO_IDX];
            SetFakeSimPresence();
        }

        if (!IsValidDbEccId(dbEccIds[SIM_DB_ECC_INFO_IDX]) && IsValidDbEccId(netPlmn.c_str())) {
            RilLogI("[%s] %s() SIM is invalid, use network DB ECC ID.", TAG, __FUNCTION__);
            dbEccIds[NET_DB_ECC_INFO_IDX] = netPlmn.c_str();
            SetFakeSimPresence();  //  make empty list on opposite side.
            hasPresences[SIM_DB_ECC_INFO_IDX] = true;  //  make empty list.
        }
        break;
    case DB_ECC_ID_TYPE_NET_PREFER:
        dbEccIds[NET_DB_ECC_INFO_IDX] = netPlmn.c_str();
        hasPresences[NET_DB_ECC_INFO_IDX] = HasNetConnPresence();

        if (NeedLastDbEccId(dbEccIds[NET_DB_ECC_INFO_IDX], mDbEccId[NET_DB_ECC_INFO_IDX])) {
            dbEccIds[NET_DB_ECC_INFO_IDX] = mDbEccId[NET_DB_ECC_INFO_IDX];
            SetFakeNetConnPresence();
        }

        if (!IsValidDbEccId(dbEccIds[NET_DB_ECC_INFO_IDX]) && IsValidDbEccId(simPlmn.c_str())) {
            RilLogI("[%s] %s() Network is invalid, use SIM DB ECC ID.", TAG, __FUNCTION__);
            dbEccIds[SIM_DB_ECC_INFO_IDX] = simPlmn.c_str();
            SetFakeNetConnPresence();  //  make empty list on opposite side.
            hasPresences[NET_DB_ECC_INFO_IDX] = true;  //  make empty list.
        }
        break;
    case DB_ECC_ID_TYPE_SIM_NET_COMBINE:
        RilLogV("[%s] %s() Use SIM_NET combine mode.", TAG, __FUNCTION__);
        dbEccIds[SIM_DB_ECC_INFO_IDX] = simPlmn.c_str();
        dbEccIds[NET_DB_ECC_INFO_IDX] = netPlmn.c_str();
        hasPresences[SIM_DB_ECC_INFO_IDX] = HasSimPresence();
        hasPresences[NET_DB_ECC_INFO_IDX] = HasNetConnPresence();

        if (NeedLastDbEccId(dbEccIds[SIM_DB_ECC_INFO_IDX], mDbEccId[SIM_DB_ECC_INFO_IDX])) {
            dbEccIds[SIM_DB_ECC_INFO_IDX] = mDbEccId[SIM_DB_ECC_INFO_IDX];
            SetFakeSimPresence();
            hasPresences[SIM_DB_ECC_INFO_IDX] = true;
        }

        if (NeedLastDbEccId(dbEccIds[NET_DB_ECC_INFO_IDX], mDbEccId[NET_DB_ECC_INFO_IDX])) {
            dbEccIds[NET_DB_ECC_INFO_IDX] = mDbEccId[NET_DB_ECC_INFO_IDX];
            SetFakeNetConnPresence();
            hasPresences[NET_DB_ECC_INFO_IDX] = true;
        }

        //  If either SIM or Network PLMN has a present,
        //  PLMN without a present should be set to empty list.
        //  For example, in a no SIM situation, if the network is attached to one side,
        //  set it to empty list so that the default list is not delivered.
        if (!HasSimPresence() && HasNetConnPresence()) {
            hasPresences[SIM_DB_ECC_INFO_IDX] = true;
        }
        if (HasSimPresence() && !HasNetConnPresence()) {
            hasPresences[NET_DB_ECC_INFO_IDX] = true;
        }
        break;
    }

    for (int i = SIM_DB_ECC_INFO_IDX; i < MAX_DB_ECC_INFO_IDX; i++) {
        needUpdate = (LoadDbEccInfo(dbEccIds[i], hasPresences[i], mDbEccId[i], i, dbEccInfos) ||
                      needUpdate);
    }

    return needUpdate;
}

/**
 * Check the validation of dbEccId.
 *
 * @param dbEccId the key to find database. It is SIM or network PLMN.
 * @return true if the length of dbEccId is 5 or 6
 *              or the dbEccId starts with PREFIX_ICCID.
 */
bool DbEccInfoLoader::IsValidDbEccId(const char *dbEccId) {
    if (TextUtils::IsEmpty(dbEccId)) {
        RilLogV("[%s] %s() DB ECC ID is empty.", TAG, __FUNCTION__);
        return false;
    }

    if (!(strlen(dbEccId) == 5 || strlen(dbEccId) == 6 ||
          strncmp(dbEccId, PREFIX_ICCID, strlen(PREFIX_ICCID)) == 0)) {
        RilLogV("[%s] %s() DB ECC ID is invalid.", TAG, __FUNCTION__);
        return false;
    }

    RilLogV("[%s] %s() DB ECC ID: %s is valid.", TAG, __FUNCTION__, dbEccId);
    return true;
}

/**
 * Check if last DB ECC ID is available.
 *
 * @param dbEccId current DB ECC ID
 * @param lastDbEccId previous DB ECC ID
 * @return true if last DB ECC ID can be used.
 */
bool DbEccInfoLoader::NeedLastDbEccId(const char *dbEccId, const char *lastDbEccId) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    return UseLastDbEccId() && !IsValidDbEccId(dbEccId) && IsValidDbEccId(lastDbEccId);
}

/**
 * Reset DB ECC ID and fake properties.
 *
 * @param forceToErase erase last DB ECC ID without considering UseLastDbEccId().
 */
void DbEccInfoLoader::ResetDbEccId(bool forceToErase) {
    RilLogV("[%s] %s()", TAG, __FUNCTION__);
    // reset PLMN for ECC list
    if (!UseLastDbEccId() || forceToErase) {
        memset(mDbEccId, 0, sizeof(mDbEccId));
    }
    RilProperty *pRilAppProperty = mService->GetRilApplicationProperty();
    if (pRilAppProperty != NULL) {
        pRilAppProperty->Put(RIL_OVERALL_FAKE_SIM_PRESENCE, 0);
        pRilAppProperty->Put(RIL_OVERALL_FAKE_NET_CONN_PRESENCE, 0);
    }
}
