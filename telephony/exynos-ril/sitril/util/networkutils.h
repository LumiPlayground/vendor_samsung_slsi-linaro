/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __NETWORK_UTILS_H__
#define __NETWORK_UTILS_H__

#include "types.h"
#include <slsi/radio_v1_2.h>
#include <slsi/radio_v1_5.h>
#include <slsi/radio_v1_6.h>

class NetworkUtils {
private:
    static std::string toePattern[2];

public:
    static int getRadioTechnologyFromAccessNetwork(int accessNetwork);
    static int getRadioTechnologyFromRadioAccessNetwork(int ran);
    static int getRadioTechnologyToAccessNetworkType(int rat);
    static int getDefaultRAF();
    static int getTestRAF(int phoneId);
    static void printRafDetail(int raf);
    static const char *getRadioTechnologyString(int rat);
    static const char *getRegStateString(int regState);
    static const char *getAccessNewtorkString(int accessNetwork);
    static bool isInService(int regState);
    static bool isEmergencyOnly(int regState);
    static bool isGsm(int radioTechnology);
    static bool isCdma(int radioTechnology);
    static bool isLte(int radioTechnology);

    // CellIdentity
    static int fetchMcc(const char *plmn);
    static int fetchMnc(const char *plmn);
    static int fetchMncNoEncoding(const char *plmn);

    // alpha tag
    static int isLteEnhancedAvailable(const std::string& operatorAlpha);
    static std::string addLteEnhancedAvailableToAlphaTag(const std::string& operatorAlpha, int lteEnhancedType);
    static std::string removeLteEnhancedAvailableFromAlphaTag(const std::string& operatorAlpha);

    // Default Data Subscription
    static void setDefaultDataSubscription(int dds);
    static int getDefaultDataSubscription();

    static std::string toString(const RIL_CellIdentityOperatorNames& operatorName);
    static std::string toString(const RIL_CellIdentityGsm_V1_5& gsm);
    static std::string toString(const RIL_CellIdentityWcdma_V1_5& wcdma);
    static std::string toString(const RIL_CellIdentityTdscdma_V1_5& tdscdma);
    static std::string toString(const RIL_CellIdentityLte_V1_5& lte);
    static std::string toString(const RIL_CellIdentityNr_V1_5& nr);
    static std::string toString(const RIL_CellIdentityCdma_V1_2& cdma);

    static std::string toString(const RIL_CellInfo_V1_6& cellInfo);
    static std::string toString(const RIL_CellInfoGsm_V1_5& gsm);
    static std::string toString(const RIL_CellInfoWcdma_V1_5& wcdma);
    static std::string toString(const RIL_CellInfoTdscdma_V1_5& tdscdma);
    static std::string toString(const RIL_CellInfoLte_V1_6& lte);
    static std::string toString(const RIL_CellInfoNr_V1_6& nr);
    static std::string toString(const RIL_CellInfoCdma_V1_2& cdma);
};

bool operator==(const RIL_CellIdentityOperatorNames& lhs, const RIL_CellIdentityOperatorNames& rhs);
bool operator==(const RIL_OptionalCsgInfo& lhs, const RIL_OptionalCsgInfo& rhs);

bool operator==(const RIL_CellIdentityGsm_V1_5& lhs, const RIL_CellIdentityGsm_V1_5& rhs);
bool operator==(const RIL_CellIdentityWcdma_V1_5& lhs, const RIL_CellIdentityWcdma_V1_5& rhs);
bool operator==(const RIL_CellIdentityTdscdma_V1_5& lhs, const RIL_CellIdentityTdscdma_V1_5& rhs);
bool operator==(const RIL_CellIdentityLte_V1_5& lhs, const RIL_CellIdentityLte_V1_5& rhs);
bool operator==(const RIL_CellIdentityNr_V1_5& lhs, const RIL_CellIdentityNr_V1_5& rhs);
bool operator==(const RIL_CellIdentityCdma_V1_2& lhs, const RIL_CellIdentityCdma_V1_2& rhs);
bool operator==(const RIL_CellIdentity_V1_5& lhs, const RIL_CellIdentity_V1_5& rhs);

bool operator==(const RIL_GSM_SignalStrength_v12& lhs, const RIL_GSM_SignalStrength_v12& rhs);
bool operator==(const RIL_WCDMA_SignalStrength_V1_2& lhs, const RIL_WCDMA_SignalStrength_V1_2& rhs);
bool operator==(const RIL_TD_SCDMA_SignalStrength_V1_2& lhs, const RIL_TD_SCDMA_SignalStrength_V1_2& rhs);
bool operator==(const RIL_LTE_SignalStrength_v8& lhs, const RIL_LTE_SignalStrength_v8& rhs);
bool operator==(const RIL_LTE_SignalStrength_V1_6& lhs, const RIL_LTE_SignalStrength_V1_6& rhs);
bool operator==(const RIL_NR_SignalStrength_V1_4& lhs, const RIL_NR_SignalStrength_V1_4& rhs);
bool operator==(const RIL_NR_SignalStrength_V1_6& lhs, const RIL_NR_SignalStrength_V1_6& rhs);
bool operator==(const RIL_CDMA_SignalStrength& lhs, const RIL_CDMA_SignalStrength& rhs);
bool operator==(const RIL_EVDO_SignalStrength& lhs, const RIL_EVDO_SignalStrength& rhs);
bool operator==(const RIL_SignalStrength_V1_6& lhs, const RIL_SignalStrength_V1_6& rhs);

bool operator==(const RIL_CellInfoGsm_V1_5& lhs, const RIL_CellInfoGsm_V1_5& rhs);
bool operator==(const RIL_CellInfoWcdma_V1_5& lhs, const RIL_CellInfoWcdma_V1_5& rhs);
bool operator==(const RIL_CellInfoTdscdma_V1_5& lhs, const RIL_CellInfoTdscdma_V1_5& rhs);
bool operator==(const RIL_CellInfoLte_V1_5& lhs, const RIL_CellInfoLte_V1_5& rhs);
bool operator==(const RIL_CellInfoLte_V1_6& lhs, const RIL_CellInfoLte_V1_6& rhs);
bool operator==(const RIL_CellInfoNr_V1_5& lhs, const RIL_CellInfoNr_V1_5& rhs);
bool operator==(const RIL_CellInfoNr_V1_6& lhs, const RIL_CellInfoNr_V1_6& rhs);
bool operator==(const RIL_CellInfoCdma_V1_2& lhs, const RIL_CellInfoCdma_V1_2& rhs);
bool operator==(const RIL_CellInfo_V1_5& lhs, const RIL_CellInfo_V1_5& rhs);
bool operator==(const RIL_CellInfo_V1_6& lhs, const RIL_CellInfo_V1_6& rhs);

#endif // __NETWORK_UTILS_H__
