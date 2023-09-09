/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __EMERGENCYNUMBERMANAGER_H__
#define __EMERGENCYNUMBERMANAGER_H__

#include "dbeccinfoloader.h"
#include "emergencynumberupdater.h"
#include "commondef.h"

class EccInfo;

class EmergencyNumberManager {
  private:
    char TAG[MAX_TAG_LEN];
    NetworkServiceBase *mService;
    EmergencyNumberUpdater *mEmergencyNumberUpdater;
    DbEccInfoLoader *mDbEccInfoLoader;

    /**
     * SIM card state. Refer to @RIL_CardState in ril.h.
     */
    int mCardState;

    /**
     * SIM app state. Refer to @RIL_AppState in ril.h.
     */
    int mAppState;

    /**
     * The ESIM disabled state.
     */
    bool mIsDisabledESim;

    /**
     * The ability of Emergency call over the LTE/NRSA network.
     * Refer to @RIL_EmcIndicator in radio_v1_6.h.
     * True if EMC bearer is 1(available) over LTE or
     * EMC_NR_CONNECTED_TO_5GCN over NR SA or
     * EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN over NR SA.
     */
    bool mEmcBearerSupported;

    /**
     * It indicates "EMC Lab test setting" from test mode app.
     * It is used to test emergency call in China operators.
     */
    bool mIsEmcLabTestMode;

    /**
     * The ECC INFO using database. Refer to EccInfo class in callreqdata.h.
     * Two array is used for SIM_NET_COMBINE.
     */
    EccInfo mDbEccInfos[MAX_DB_ECC_INFO_IDX];

    /**
     * The ECC INFO using radio(network). Refer to EccInfo class in callreqdata.h.
     * Two array is used for ENL and EENL(including ENL).
     * Refer to sit_ecc_info_type enum.
     * One of the two is selected by GetRadioEccInfoType().
     */
    EccInfo mRadioEccInfos[SIT_ECC_INFO_TYPE_MAX];

    /**
     * For SIP 380 redial case,
     * normal number is changed to emergency number temporary.
     * The temp emergency number is added by IMS svc
     * using RIL_REQUEST_OEM_AIMS_SET_TEMP_EMERGENCY_NUMBER.
     * Once CS call finished, temp emergency number is removed.
     */
    EmergencyNumber mTempEmergencyNumber;

  private:
    void PrintEccInfo(EccInfo eccInfo);
    void UpdateOppositeDbEccInfo();

  public:
    EmergencyNumberManager(NetworkServiceBase *service);
    EmergencyNumberManager(const EmergencyNumberManager&) = delete;
    EmergencyNumberManager& operator=(EmergencyNumberManager const&) = delete;
    ~EmergencyNumberManager();
    int GetCardState();
    int GetAppState();
    bool IsDisabledESim();
    bool IsEmcBearerSupported();
    bool IsEmcLabTestMode();
    EccInfo *GetDbEccInfos();
    EccInfo *GetRadioEccInfos();
    void SetSimState(int cardState, int appState, bool isDisabledESim);
    void SetEmcBearerSupported(bool isEmcBearerSupported);
    void SetEmcLabTestMode(bool isEmcLabTestMode);
    void UpdateEmergencyNumberList();
    void UpdateDbEccInfo(bool forceToUpdate = true, bool callByOpposite = false);
    void ResetDbEccId(bool forceToErase = false);
    bool IsEmergencyCallRouting(const char *number, bool *normalRoutingWhenInService);
    int GetEmergencyNumberCategory(const char *number);
    void SetTempEmergencyNumber(int operation, int number_len, char *number, int category);

    /**
     * The major calling API to update database emergency number list.
     *
     * -> DbEccInfoLoader.Load()
     *    (LoadDbEccInfo, UpdateDbEccInfo)
     * -> EmergencyNumberUpdater.Update()
     *    (UpdateEmergencyNumberList, AddEmergencyNumberList,
     *     UpdateEmergencyNumberAndRouting, AddEmergencyNumber,
     *     OnUnsolicitedResponse)
     */
};

#endif
