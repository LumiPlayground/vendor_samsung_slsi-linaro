/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "networkutils.h"
#include "rillog.h"
#include "rilproperty.h"
#include <librilutils/systemproperty.h>
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>
#include <librilutils/textutils.h>
#include "rildef.h"

#include <sstream>

int NetworkUtils::getRadioTechnologyFromAccessNetwork(int accessNetwork)
{
    switch (accessNetwork) {
    case ACCESS_NETWORK_UNKNOWN:
        return RADIO_TECH_UNKNOWN;
    case ACCESS_NETWORK_GERAN:
        return RADIO_TECH_GPRS;
    case ACCESS_NETWORK_UTRAN:
        return RADIO_TECH_UMTS;
    case ACCESS_NETWORK_EUTRAN:
        return RADIO_TECH_LTE;
    case ACCESS_NETWORK_CDMA2000:
        return RADIO_TECH_1xRTT;
    case ACCESS_NETWORK_NGRAN:
        return RADIO_TECH_NR;
    default:
        break;
    }
    return RADIO_TECH_UNKNOWN;
}

int NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(int ran)
{
    switch (ran) {
    case RIL_RAN_UNKNOWN:
        return RADIO_TECH_UNKNOWN;
    case RIL_RAN_GERAN:
        return RADIO_TECH_GPRS;
    case RIL_RAN_UTRAN:
        return RADIO_TECH_UMTS;
    case RIL_RAN_EUTRAN:
        return RADIO_TECH_LTE;
    case RIL_RAN_NGRAN:
        return RADIO_TECH_NR;
    case RIL_RAN_CDMA2000:
        return RADIO_TECH_1xRTT;
    default:
        break;
    }
    return RADIO_TECH_UNKNOWN;
}

int NetworkUtils::getRadioTechnologyToAccessNetworkType(int rat)
{
    switch(rat) {
    case RADIO_TECH_GPRS:
    case RADIO_TECH_EDGE:
    case RADIO_TECH_GSM:
        return ACCESS_NETWORK_GERAN;
    case RADIO_TECH_UMTS:
    case RADIO_TECH_HSDPA:
    case RADIO_TECH_HSPAP:
    case RADIO_TECH_HSUPA:
    case RADIO_TECH_HSPA:
    case RADIO_TECH_TD_SCDMA:
        return ACCESS_NETWORK_UTRAN;
    case RADIO_TECH_IS95A:
    case RADIO_TECH_IS95B:
    case RADIO_TECH_1xRTT:
    case RADIO_TECH_EVDO_0:
    case RADIO_TECH_EVDO_A:
    case RADIO_TECH_EVDO_B:
    case RADIO_TECH_EHRPD:
        return ACCESS_NETWORK_CDMA2000;
    case RADIO_TECH_LTE:
    case RADIO_TECH_LTE_CA:
        return ACCESS_NETWORK_EUTRAN;
    case RADIO_TECH_IWLAN:
        return ACCESS_NETWORK_IWLAN;
    case RADIO_TECH_NR:
        return ACCESS_NETWORK_NGRAN;
    case RADIO_TECH_UNKNOWN:
    default:
        return ACCESS_NETWORK_UNKNOWN;
    }
}

const char *NetworkUtils::getRadioTechnologyString(int rat)
{
    switch(rat) {
    case RADIO_TECH_GPRS:
        return "GPRS";
    case RADIO_TECH_EDGE:
        return "EDGE";
    case RADIO_TECH_GSM:
        return "GSM";
    case RADIO_TECH_UMTS:
        return "UMTS";
    case RADIO_TECH_HSDPA:
        return "HSDPA";
    case RADIO_TECH_HSPAP:
        return "HSPAP";
    case RADIO_TECH_HSUPA:
        return "HSUPA";
    case RADIO_TECH_HSPA:
        return "HSPA";
    case RADIO_TECH_TD_SCDMA:
        return "TD_SCDMA";
    case RADIO_TECH_IS95A:
        return "IS95A";
    case RADIO_TECH_IS95B:
        return "IS95B";
    case RADIO_TECH_1xRTT:
        return "1xRTT";
    case RADIO_TECH_EVDO_0:
        return "EVDO_0";
    case RADIO_TECH_EVDO_A:
        return "EVDO_A";
    case RADIO_TECH_EVDO_B:
        return "EVDO_B";
    case RADIO_TECH_EHRPD:
        return "EHRPD";
    case RADIO_TECH_LTE:
        return "LTE";
    case RADIO_TECH_LTE_CA:
        return "LTE_CA";
    case RADIO_TECH_IWLAN:
        return "IWLAN";
    case RADIO_TECH_NR:
        return "NR";
    case RADIO_TECH_UNKNOWN:
        return "UNKNOWN";
    default:
        return "UNKNOWN";
    }
}
const char *NetworkUtils::getRegStateString(int regState)
{
    switch (regState) {
    case RIL_NOT_REG_AND_NOT_SEARCHING:
        return "NOT_REG_AND_NOT_SEARCHING";
    case RIL_REG_HOME:
        return "REG_HOME";
    case RIL_NOT_REG_AND_SEARCHING:
        return "NOT_REG_AND_SEARCHING";
    case RIL_REG_DENIED:
        return "REG_DENIED";
    case RIL_UNKNOWN:
        return "UNKNOWN";
    case RIL_REG_ROAMING:
        return "REG_ROAMING";
    case RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING:
        return "NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING";
    case RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING:
        return "NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING";
    case RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE:
        return "REG_DENIED_AND_EMERGENCY_AVAILABLE";
    case RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE:
        return "UNKNOWN_AND_EMERGENCY_AVAILABLE";
    default:
        return "UNKNOWN";
    }
}
const char *NetworkUtils::getAccessNewtorkString(int accessNetwork)
{
    switch (accessNetwork) {
    case ACCESS_NETWORK_UNKNOWN:
        return "UNKNOWN";
    case ACCESS_NETWORK_GERAN:
        return "GERAN";
    case ACCESS_NETWORK_UTRAN:
        return "UTRAN";
    case ACCESS_NETWORK_EUTRAN:
        return "EUTRAN";
    case ACCESS_NETWORK_CDMA2000:
        return "CDMA2000";
    case ACCESS_NETWORK_NGRAN:
        return "NGRAN";
    default:
        return "UNKNOWN";
    }
}

int NetworkUtils::getDefaultRAF()
{
    int raf = RAF_RIL_3GPP | RAF_RIL_3GPP2;
    bool isNrSupport = SystemProperty::GetInt(VENDOR_RIL_SUPPORT_NR, 0) == 1;
    if (isNrSupport) {
        raf |= RAF_NR;
    }
    return raf;
}

int NetworkUtils::getTestRAF(int phoneId)
{
    stringstream ss;
    ss << "persist.vendor.ril.test_raf" << phoneId;
    string name = ss.str();
    int raf = SystemProperty::GetInt(name, 0);
    if (raf > 0) {
        return raf;
    }
    return -1;
}

void NetworkUtils::printRafDetail(int raf)
{
    RilLogV("RAF=%d(%08X)", raf, raf);
    RilLogV("  NETWORK_TYPE_BITMAP_GSM: %s", ((NETWORK_TYPE_BITMAP_GSM & raf) > 0) ? "true" : "false");
    RilLogV("  NETWORK_TYPE_BITMAP_WCDMA: %s", ((NETWORK_TYPE_BITMAP_WCDMA & raf) > 0) ? "true" : "false");
    RilLogV("  NETWORK_TYPE_BITMAP_CDMA: %s", ((NETWORK_TYPE_BITMAP_CDMA & raf) > 0) ? "true" : "false");
    RilLogV("  NETWORK_TYPE_BITMAP_EVDO: %s", ((NETWORK_TYPE_BITMAP_EVDO & raf) > 0) ? "true" : "false");
    RilLogV("  NETWORK_TYPE_BITMAP_LTE: %s", ((NETWORK_TYPE_BITMAP_LTE & raf) > 0) ? "true" : "false");
    RilLogV("  NETWORK_TYPE_BITMAP_NR: %s", ((NETWORK_TYPE_BITMAP_NR & raf) > 0) ? "true" : "false");
    RilLogV("  NETWORK_TYPE_BITMAP_TDS_CDMA: %s", ((NETWORK_TYPE_BITMAP_TDS_CDMA & raf) > 0) ? "true" : "false");
}

bool NetworkUtils::isInService(int regState)
{
    return (regState == RIL_REG_HOME) || (regState == RIL_REG_ROAMING);
}

bool NetworkUtils::isEmergencyOnly(int regState)
{
    return (regState == RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING) ||
           (regState == RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING) ||
           (regState == RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE) ||
           (regState == RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE);
}

bool NetworkUtils::isGsm(int radioTechnology)
{
    return radioTechnology == RADIO_TECH_GPRS
            || radioTechnology == RADIO_TECH_EDGE
            || radioTechnology == RADIO_TECH_UMTS
            || radioTechnology == RADIO_TECH_HSDPA
            || radioTechnology == RADIO_TECH_HSUPA
            || radioTechnology == RADIO_TECH_HSPA
            || radioTechnology == RADIO_TECH_LTE
            || radioTechnology == RADIO_TECH_HSPAP
            || radioTechnology == RADIO_TECH_GSM
            || radioTechnology == RADIO_TECH_TD_SCDMA
            || radioTechnology == RADIO_TECH_IWLAN
            || radioTechnology == RADIO_TECH_LTE_CA
            || radioTechnology == RADIO_TECH_NR;
}

bool NetworkUtils::isCdma(int radioTechnology)
{
    return radioTechnology == RADIO_TECH_IS95A
            || radioTechnology == RADIO_TECH_IS95B
            || radioTechnology == RADIO_TECH_1xRTT
            || radioTechnology == RADIO_TECH_EVDO_0
            || radioTechnology == RADIO_TECH_EVDO_A
            || radioTechnology == RADIO_TECH_EVDO_B
            || radioTechnology == RADIO_TECH_EHRPD;
}

bool NetworkUtils::isLte(int radioTechnology)
{
    return radioTechnology == RADIO_TECH_LTE
            || radioTechnology == RADIO_TECH_LTE_CA;
}


int NetworkUtils::fetchMcc(const char *plmn)
{
    if (TextUtils::IsDigitsOnly(plmn)) {
        int len = strlen(plmn);
        if (len == 5 || len == 6) {
            char mccStr[4] = {0, };
            memcpy(mccStr, plmn, 3);
            int mcc = strtol(mccStr, NULL, 10);
            return mcc;
        }
    }
    return INT_MAX;
}

int NetworkUtils::fetchMnc(const char *plmn)
{
    char mccmncStr[7] = {0, };
    if (plmn != NULL) {
        memcpy(mccmncStr, plmn, 6);
        if (mccmncStr[5] == '#') {
            mccmncStr[5] = 0;
        }

        if (TextUtils::IsDigitsOnly(mccmncStr)) {
            int len = strlen(mccmncStr);
            if (len == 5 || len == 6) {
                char mncStr[4] = {0, };
                int mccLength = len - 3;
                memcpy(mncStr, mccmncStr + 3, mccLength);
                int mnc = strtol(mncStr, NULL, 10);
                return ril::util::mnc::encode(mnc, mccLength);
            }
        }
    }
    return INT_MAX;
}

int NetworkUtils::fetchMncNoEncoding(const char *plmn)
{
    char mncStr[4] = {0, };
    if (plmn != NULL) {
        memcpy(mncStr, plmn + 3, 3);
        if (mncStr[2] == '#') {
            mncStr[2] = 0;
        }

        if (TextUtils::IsDigitsOnly(mncStr)) {
            int mncLength = strlen(mncStr);
            if (mncLength == 2 || mncLength == 3) {
                int mnc = strtol(mncStr, NULL, 10);
                return mnc;
            }
        }
    }
    return INT_MAX;
}

std::string NetworkUtils::toePattern[2] = {"...1", "...2"};

int NetworkUtils::isLteEnhancedAvailable(const std::string& operatorAlpha)
{
    int ret = 0;
    int lenPattern = 0;
    int lenOperatorAlpha = operatorAlpha.length();

    for (int i = 0; i < 2; ++i) {
        lenPattern = toePattern[i].length();
        if (lenPattern <= lenOperatorAlpha
                && operatorAlpha.compare(lenOperatorAlpha-lenPattern, lenPattern, toePattern[i]) == 0) {
            ret = i+1;
            break;
        }
    }

    return ret;
}

std::string NetworkUtils::addLteEnhancedAvailableToAlphaTag(const std::string& operatorAlpha, int lteEnhancedType)
{
    std::string ret = operatorAlpha;

    if (lteEnhancedType > 0) {
        char lteEnhnacedAvailable[10];
        sprintf(lteEnhnacedAvailable, "...%d", lteEnhancedType);
        ret += lteEnhnacedAvailable;
    }

    return ret;
}

std::string NetworkUtils::removeLteEnhancedAvailableFromAlphaTag(const std::string& operatorAlpha)
{
    std::string ret = operatorAlpha;
    int lenPattern = 0;
    int lenOperatorAlpha = operatorAlpha.length();

    for (int i = 0; i < 2; ++i) {
        lenPattern = toePattern[i].length();
        if (lenPattern <= lenOperatorAlpha
                && operatorAlpha.compare(lenOperatorAlpha-lenPattern, lenPattern, toePattern[i]) == 0) {
            ret = operatorAlpha.substr(0, lenOperatorAlpha-lenPattern);
            break;
        }
    }

    return ret;
}

void NetworkUtils::setDefaultDataSubscription(int dds)
{
    SystemProperty::Set("persist.vendor.radio.dds", dds);
}

int NetworkUtils::getDefaultDataSubscription()
{
    return SystemProperty::GetInt("persist.vendor.radio.dds", -1);
}

std::string NetworkUtils::toString(const RIL_CellIdentityOperatorNames& operatorName) {
    std::stringstream ss;
    ss << "RIL_CellIdentityOperatorNames:{";
    ss << "alphaLong=" << operatorName.alphaLong << " alphaShort=" << operatorName.alphaShort;
    ss << "}";
    return ss.str();
}


std::string NetworkUtils::toString(const RIL_CellIdentityGsm_V1_5& cellIdentity) {

    std::stringstream ss;
    ss << "RIL_CellIdentityGsm_V1_5:{";
    ss << " mcc=" <<  ril::util::mcc::decode(cellIdentity.mcc) << " mnc=" << ril::util::mnc::decode(cellIdentity.mnc);
    ss << " lac=" << cellIdentity.lac << " cid=" << cellIdentity.cid;
    ss << " arfcn=" << cellIdentity.arfcn << " bsic=" << cellIdentity.bsic;
    ss << " " << NetworkUtils::toString(cellIdentity.operatorNames);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellIdentityWcdma_V1_5& cellIdentity) {
    std::stringstream ss;
    ss << "RIL_CellIdentityWcdma_V1_5:{";
    ss << " mcc=" <<  ril::util::mcc::decode(cellIdentity.mcc) << " mnc=" << ril::util::mnc::decode(cellIdentity.mnc);
    ss << " lac=" << cellIdentity.lac << " cid=" << cellIdentity.cid;
    ss << " psc=" << cellIdentity.psc << " uarfcn=" << cellIdentity.uarfcn;
    ss << " " << NetworkUtils::toString(cellIdentity.operatorNames);
    ss << "}";
    return ss.str();
}
std::string NetworkUtils::toString(const RIL_CellIdentityTdscdma_V1_5& cellIdentity) {
    std::stringstream ss;
    ss << "RIL_CellIdentityTdscdma_V1_5:{";
    ss << " mcc=" <<  ril::util::mcc::decode(cellIdentity.mcc) << " mnc=" << ril::util::mnc::decode(cellIdentity.mnc);
    ss << " lac=" << cellIdentity.lac << " cid=" << cellIdentity.cid;
    ss << " cpid=" << cellIdentity.cpid << " uarfcn=" << cellIdentity.uarfcn;
    ss << " " << NetworkUtils::toString(cellIdentity.operatorNames);
    ss << "}";
    return ss.str();
}
std::string NetworkUtils::toString(const RIL_CellIdentityLte_V1_5& cellIdentity) {
    std::stringstream ss;
    ss << "RIL_CellIdentityLte_V1_5:{";
    ss << " mcc=" <<  ril::util::mcc::decode(cellIdentity.mcc) << " mnc=" << ril::util::mnc::decode(cellIdentity.mnc);
    ss << " ci=" << cellIdentity.ci << " pci=" << cellIdentity.pci;
    ss << " tac=" << cellIdentity.tac << " earfcn=" << cellIdentity.earfcn;
    ss << " " << NetworkUtils::toString(cellIdentity.operatorNames);
    ss << " bandwidth=" << cellIdentity.bandwidth << " leng_bands=" << cellIdentity.leng_bands;
    ss << "}";
    return ss.str();
}
std::string NetworkUtils::toString(const RIL_CellIdentityNr_V1_5& cellIdentity) {
    std::stringstream ss;
    ss << "RIL_CellIdentityNr_V1_5:{";
    ss << " mcc=" <<  ril::util::mcc::decode(cellIdentity.mcc) << " mnc=" << ril::util::mnc::decode(cellIdentity.mnc);
    ss << " nci=" << cellIdentity.nci << " pci=" << cellIdentity.pci;
    ss << " tac=" << cellIdentity.tac << " nrarfcn=" << cellIdentity.nrarfcn;
    ss << " " << NetworkUtils::toString(cellIdentity.operatorNames);
    ss << " leng_bands=" << cellIdentity.leng_bands;
    ss << "}";
    return ss.str();
}
std::string NetworkUtils::toString(const RIL_CellIdentityCdma_V1_2& cellIdentity) {
    std::stringstream ss;
    ss << "RIL_CellIdentityCdma_V1_2:{";
    ss << " networkId=" << cellIdentity.networkId << " systemId=" << cellIdentity.systemId;
    ss << " basestationId=" << cellIdentity.basestationId << " longitude=" << cellIdentity.longitude;
    ss << " latitude=" << cellIdentity.latitude;
    ss << " " << NetworkUtils::toString(cellIdentity.operatorNames);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfo_V1_6& cellInfo) {
    std::stringstream ss;
    ss << "RIL_CellInfo_V1_6:{";
    ss << " registered=" << cellInfo.registered << " timeStampType=" << cellInfo.timeStampType;
    ss << " connectionStatus=" << cellInfo.connectionStatus;
    switch((int)(cellInfo.cellInfoType)) {
    case RIL_CELL_INFO_TYPE_GSM:
        ss << " " << NetworkUtils::toString(cellInfo.CellInfo.gsm);
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        ss << " " << NetworkUtils::toString(cellInfo.CellInfo.wcdma);
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        ss << " " << NetworkUtils::toString(cellInfo.CellInfo.lte);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        ss << " " << NetworkUtils::toString(cellInfo.CellInfo.tdscdma);
        break;
    case RIL_CELL_INFO_TYPE_NR:
        ss << " " << NetworkUtils::toString(cellInfo.CellInfo.nr);
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        ss << " " << NetworkUtils::toString(cellInfo.CellInfo.cdma);
        break;
    default:
        break;
    }
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfoGsm_V1_5& gsm) {
    std::stringstream ss;
    ss << "RIL_CellInfoGsm_V1_5:{ " << NetworkUtils::toString(gsm.cellIdentityGsm);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfoWcdma_V1_5& wcdma) {
    std::stringstream ss;
    ss << "RIL_CellInfoGsm_V1_5:{ " << NetworkUtils::toString(wcdma.cellIdentityWcdma);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfoTdscdma_V1_5& tdscdma) {
    std::stringstream ss;
    ss << "RIL_CellInfoGsm_V1_5:{ " << NetworkUtils::toString(tdscdma.cellIdentityTdscdma);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfoLte_V1_6& lte) {
    std::stringstream ss;
    ss << "RIL_CellInfoGsm_V1_5:{ " << NetworkUtils::toString(lte.cellIdentityLte);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfoNr_V1_6& nr) {
    std::stringstream ss;
    ss << "RIL_CellInfoGsm_V1_5:{ " << NetworkUtils::toString(nr.cellIdentityNr);
    ss << "}";
    return ss.str();
}

std::string NetworkUtils::toString(const RIL_CellInfoCdma_V1_2& cdma) {
    std::stringstream ss;
    ss << "RIL_CellInfoGsm_V1_5:{ " << NetworkUtils::toString(cdma.cellIdentityCdma);
    ss << "}";
    return ss.str();
}

static auto toVector(const char **strPtr, size_t length) {
    std::vector<std::string> ret;
    if (strPtr) {
        ret.resize(length);
        for (size_t i = 0; i < length; i++) {
            ret[i] = strPtr[i] ? strPtr[i] : "";
        }
    }
    return ret;
};

template<typename T>
static auto toVector(T *array, size_t length) {
    std::vector<T> ret;
    if (array) {
        ret.resize(length);
        for (size_t i = 0; i < length; i++) {
            ret[i] = array[i];
        }
    }
    return ret;
}

bool operator==(const RIL_CellIdentityOperatorNames& lhs, const RIL_CellIdentityOperatorNames& rhs) {
    return (std::string(lhs.alphaLong) == std::string(rhs.alphaLong)) &&
            (std::string(lhs.alphaShort) == std::string(rhs.alphaShort));
}

bool operator==(const RIL_OptionalCsgInfo& lhs, const RIL_OptionalCsgInfo& rhs) {
    return lhs.csgInfo.csgIndication == rhs.csgInfo.csgIndication &&
            std::string(lhs.csgInfo.homeNodebName) == std::string(rhs.csgInfo.homeNodebName) &&
            lhs.csgInfo.csgIdentity == rhs.csgInfo.csgIdentity;
}

bool operator==(const RIL_CellIdentityGsm_V1_5& lhs, const RIL_CellIdentityGsm_V1_5& rhs) {
    auto lhs_additionalPlmns = toVector((const char **)lhs.additionalPlmns, lhs.leng_additionalPlmns);
    auto rhs_additionalPlmns = toVector((const char **)rhs.additionalPlmns, rhs.leng_additionalPlmns);
    return std::tie(lhs.mcc, lhs.mnc, lhs.lac, lhs.cid, lhs.arfcn, lhs.bsic, lhs.operatorNames,
                    lhs_additionalPlmns) ==
           std::tie(rhs.mcc, rhs.mnc, rhs.lac, rhs.cid, rhs.arfcn, rhs.bsic, rhs.operatorNames,
                    rhs_additionalPlmns);
}

bool operator==(const RIL_CellIdentityWcdma_V1_5& lhs, const RIL_CellIdentityWcdma_V1_5& rhs) {
    auto lhs_additionalPlmns = toVector((const char **)lhs.additionalPlmns, lhs.leng_additionalPlmns);
    auto rhs_additionalPlmns = toVector((const char **)rhs.additionalPlmns, rhs.leng_additionalPlmns);
    return std::tie(lhs.mcc, lhs.mnc, lhs.lac, lhs.cid, lhs.psc, lhs.uarfcn, lhs.operatorNames,
                    lhs_additionalPlmns, lhs.optionalCsgInfo) ==
           std::tie(rhs.mcc, rhs.mnc, rhs.lac, rhs.cid, rhs.psc, rhs.uarfcn, rhs.operatorNames,
                    rhs_additionalPlmns, rhs.optionalCsgInfo);
}

bool operator==(const RIL_CellIdentityTdscdma_V1_5& lhs, const RIL_CellIdentityTdscdma_V1_5& rhs) {
    auto lhs_additionalPlmns = toVector((const char **)lhs.additionalPlmns, lhs.leng_additionalPlmns);
    auto rhs_additionalPlmns = toVector((const char **)rhs.additionalPlmns, rhs.leng_additionalPlmns);
    return std::tie(lhs.mcc, lhs.mnc, lhs.lac, lhs.cid, lhs.cpid, lhs.uarfcn, lhs.operatorNames,
                    lhs_additionalPlmns, lhs.optionalCsgInfo) ==
           std::tie(rhs.mcc, rhs.mnc, rhs.lac, rhs.cid, rhs.cpid, rhs.uarfcn, rhs.operatorNames,
                    rhs_additionalPlmns, rhs.optionalCsgInfo);
}

bool operator==(const RIL_CellIdentityLte_V1_5& lhs, const RIL_CellIdentityLte_V1_5& rhs) {
    auto lhs_additionalPlmns = toVector((const char **)lhs.additionalPlmns, lhs.leng_additionalPlmns);
    auto rhs_additionalPlmns = toVector((const char **)rhs.additionalPlmns, rhs.leng_additionalPlmns);
    auto lhs_bands = toVector((int *)lhs.bands, lhs.leng_bands);
    auto rhs_bands = toVector((int *)rhs.bands, rhs.leng_bands);
    return std::tie(lhs.mcc, lhs.mnc, lhs.ci, lhs.pci, lhs.tac, lhs.earfcn, lhs.operatorNames,
                    lhs.bandwidth, lhs_additionalPlmns, lhs.optionalCsgInfo, lhs_bands) ==
           std::tie(rhs.mcc, rhs.mnc, rhs.ci, rhs.pci, rhs.tac, rhs.earfcn, rhs.operatorNames,
                    rhs.bandwidth, rhs_additionalPlmns, rhs.optionalCsgInfo, rhs_bands);
}

bool operator==(const RIL_CellIdentityNr_V1_5& lhs, const RIL_CellIdentityNr_V1_5& rhs) {
    auto lhs_additionalPlmns = toVector((const char **)lhs.additionalPlmns, lhs.leng_additionalPlmns);
    auto rhs_additionalPlmns = toVector((const char **)rhs.additionalPlmns, rhs.leng_additionalPlmns);
    auto lhs_bands = toVector((int *)lhs.bands, lhs.leng_bands);
    auto rhs_bands = toVector((int *)rhs.bands, rhs.leng_bands);
    return std::tie(lhs.mcc, lhs.mnc, lhs.nci, lhs.pci, lhs.tac, lhs.nrarfcn, lhs.operatorNames,
                    lhs_additionalPlmns, lhs_bands) ==
           std::tie(rhs.mcc, rhs.mnc, rhs.nci, rhs.pci, rhs.tac, rhs.nrarfcn, rhs.operatorNames,
                    rhs_additionalPlmns, rhs_bands);
}

bool operator==(const RIL_CellIdentityCdma_V1_2& lhs, const RIL_CellIdentityCdma_V1_2& rhs) {
    return std::tie(lhs.networkId, lhs.systemId, lhs.basestationId, lhs.longitude, lhs.latitude,
                    lhs.operatorNames) ==
           std::tie(rhs.networkId, rhs.systemId, rhs.basestationId, rhs.longitude, rhs.latitude,
                    rhs.operatorNames);
}

bool operator==(const RIL_CellIdentity_V1_5& lhs, const RIL_CellIdentity_V1_5& rhs) {
    if (lhs.cellInfoType != rhs.cellInfoType) {
        return false;
    }

    switch ((int)lhs.cellInfoType) {
    case RIL_CELL_INFO_TYPE_NONE:
        return true;
    case RIL_CELL_INFO_TYPE_GSM:
        return lhs.gsm == rhs.gsm;
    case RIL_CELL_INFO_TYPE_CDMA:
        return lhs.cdma == rhs.cdma;
    case RIL_CELL_INFO_TYPE_LTE:
        return lhs.lte == rhs.lte;
    case RIL_CELL_INFO_TYPE_WCDMA:
        return lhs.wcdma == rhs.wcdma;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        return lhs.tdscdma == rhs.tdscdma;
    case RIL_CELL_INFO_TYPE_NR:
        return lhs.nr == rhs.nr;
    default:
        return false;
    }
}

bool operator==(const RIL_GSM_SignalStrength_v12& lhs, const RIL_GSM_SignalStrength_v12& rhs) {
    return std::tie(lhs.signalStrength, lhs.bitErrorRate, lhs.timingAdvance) ==
            std::tie(rhs.signalStrength, rhs.bitErrorRate, rhs.timingAdvance);
}

bool operator==(const RIL_WCDMA_SignalStrength_V1_2& lhs, const RIL_WCDMA_SignalStrength_V1_2& rhs) {
    return std::tie(lhs.signalStrength, lhs.bitErrorRate, lhs.rscp, lhs.ecno) ==
            std::tie(rhs.signalStrength, rhs.bitErrorRate, rhs.rscp, rhs.ecno);
}

bool operator==(const RIL_TD_SCDMA_SignalStrength_V1_2& lhs, const RIL_TD_SCDMA_SignalStrength_V1_2& rhs) {
    return std::tie(lhs.signalStrength, lhs.bitErrorRate, lhs.rscp) ==
            std::tie(rhs.signalStrength, rhs.bitErrorRate, rhs.rscp);
}

bool operator==(const RIL_LTE_SignalStrength_v8& lhs, const RIL_LTE_SignalStrength_v8& rhs) {
    return std::tie(lhs.signalStrength, lhs.rsrp, lhs.rsrq, lhs.rssnr,
                    lhs.cqi, lhs.timingAdvance) ==
            std::tie(rhs.signalStrength, rhs.rsrp, rhs.rsrq, rhs.rssnr,
                    rhs.cqi, rhs.timingAdvance);
}

bool operator==(const RIL_LTE_SignalStrength_V1_6& lhs, const RIL_LTE_SignalStrength_V1_6& rhs) {
    return std::tie(lhs.signalStrength, lhs.rsrp, lhs.rsrq, lhs.rssnr,
                    lhs.cqi, lhs.timingAdvance, lhs.cqiTableIndex) ==
            std::tie(rhs.signalStrength, rhs.rsrp, rhs.rsrq, rhs.rssnr,
                    rhs.cqi, rhs.timingAdvance, rhs.cqiTableIndex);
}

bool operator==(const RIL_NR_SignalStrength_V1_4& lhs, const RIL_NR_SignalStrength_V1_4& rhs) {
    return std::tie(lhs.ssRsrp, lhs.ssRsrq, lhs.ssSinr, lhs.csiRsrp, lhs.csiRsrq, lhs.csiSinr) ==
            std::tie(rhs.ssRsrp, rhs.ssRsrq, rhs.ssSinr, rhs.csiRsrp, rhs.csiRsrq, rhs.csiSinr);
}

bool operator==(const RIL_NR_SignalStrength_V1_6& lhs, const RIL_NR_SignalStrength_V1_6& rhs) {
    auto lhs_cqiReport = toVector((int *)lhs.csiCqiReport, lhs.csiCqiReportLen);
    auto rhs_cqiReport = toVector((int *)rhs.csiCqiReport, rhs.csiCqiReportLen);
    return std::tie(lhs.ssRsrp, lhs.ssRsrq, lhs.ssSinr, lhs.csiRsrp, lhs.csiRsrq, lhs.csiSinr,
                lhs.csiCqiTableIndex, lhs_cqiReport) ==
            std::tie(rhs.ssRsrp, rhs.ssRsrq, rhs.ssSinr, rhs.csiRsrp, rhs.csiRsrq, rhs.csiSinr,
                rhs.csiCqiTableIndex, rhs_cqiReport);
}

bool operator==(const RIL_CDMA_SignalStrength& lhs, const RIL_CDMA_SignalStrength& rhs) {
    return std::tie(lhs.dbm, lhs.ecio) == std::tie(rhs.dbm, rhs.ecio);
}

bool operator==(const RIL_EVDO_SignalStrength& lhs, const RIL_EVDO_SignalStrength& rhs) {
    return std::tie(lhs.dbm, lhs.ecio, lhs.signalNoiseRatio) ==
            std::tie(rhs.dbm, rhs.ecio, rhs.signalNoiseRatio);
}

bool operator==(const RIL_SignalStrength_V1_6& lhs, const RIL_SignalStrength_V1_6& rhs) {
    return std::tie(lhs.GSM_SignalStrength, lhs.CDMA_SignalStrength, lhs.EVDO_SignalStrength,
                lhs.LTE_SignalStrength, lhs.TD_SCDMA_SignalStrength, lhs.WCDMA_SignalStrength,
                lhs.NR_SignalStrength) ==
            std::tie(rhs.GSM_SignalStrength, rhs.CDMA_SignalStrength, rhs.EVDO_SignalStrength,
                rhs.LTE_SignalStrength, rhs.TD_SCDMA_SignalStrength, rhs.WCDMA_SignalStrength,
                rhs.NR_SignalStrength);
}

bool operator==(const RIL_CellInfoGsm_V1_5& lhs, const RIL_CellInfoGsm_V1_5& rhs) {
    return std::tie(lhs.cellIdentityGsm, lhs.signalStrengthGsm) ==
            std::tie(rhs.cellIdentityGsm, rhs.signalStrengthGsm);
}

bool operator==(const RIL_CellInfoWcdma_V1_5& lhs, const RIL_CellInfoWcdma_V1_5& rhs) {
    return std::tie(lhs.cellIdentityWcdma, lhs.signalStrengthWcdma) ==
            std::tie(rhs.cellIdentityWcdma, rhs.signalStrengthWcdma);
}

bool operator==(const RIL_CellInfoTdscdma_V1_5& lhs, const RIL_CellInfoTdscdma_V1_5& rhs) {
    return std::tie(lhs.cellIdentityTdscdma, lhs.signalStrengthTdscdma) ==
            std::tie(rhs.cellIdentityTdscdma, rhs.signalStrengthTdscdma);
}

bool operator==(const RIL_CellInfoLte_V1_5& lhs, const RIL_CellInfoLte_V1_5& rhs) {
    return std::tie(lhs.cellIdentityLte, lhs.signalStrengthLte, lhs.isEndcAvailable) ==
            std::tie(rhs.cellIdentityLte, rhs.signalStrengthLte, rhs.isEndcAvailable);
}

bool operator==(const RIL_CellInfoLte_V1_6& lhs, const RIL_CellInfoLte_V1_6& rhs) {
    return std::tie(lhs.cellIdentityLte, lhs.signalStrengthLte, lhs.isEndcAvailable) ==
            std::tie(rhs.cellIdentityLte, rhs.signalStrengthLte, rhs.isEndcAvailable);
}

bool operator==(const RIL_CellInfoNr_V1_5& lhs, const RIL_CellInfoNr_V1_5& rhs) {
    return std::tie(lhs.cellIdentityNr, lhs.signalStrengthNr) ==
            std::tie(rhs.cellIdentityNr, rhs.signalStrengthNr);
}

bool operator==(const RIL_CellInfoNr_V1_6& lhs, const RIL_CellInfoNr_V1_6& rhs) {
    return std::tie(lhs.cellIdentityNr, lhs.signalStrengthNr) ==
            std::tie(rhs.cellIdentityNr, rhs.signalStrengthNr);
}

bool operator==(const RIL_CellInfoCdma_V1_2& lhs, const RIL_CellInfoCdma_V1_2& rhs) {
    return std::tie(lhs.cellIdentityCdma, lhs.signalStrengthCdma, lhs.signalStrengthEvdo) ==
            std::tie(rhs.cellIdentityCdma, rhs.signalStrengthCdma, rhs.signalStrengthEvdo);
}

bool operator==(const RIL_CellInfo_V1_5& lhs, const RIL_CellInfo_V1_5& rhs) {
    if (!(std::tie(lhs.cellInfoType, lhs.registered, lhs.timeStampType, lhs.connectionStatus) ==
            std::tie(rhs.cellInfoType, rhs.registered, rhs.timeStampType,  rhs.connectionStatus))) {
        return false;
    }

    switch ((int)lhs.cellInfoType) {
    case RIL_CELL_INFO_TYPE_NONE:
        return true;
    case RIL_CELL_INFO_TYPE_GSM:
        return lhs.CellInfo.gsm == rhs.CellInfo.gsm;
    case RIL_CELL_INFO_TYPE_CDMA:
        return lhs.CellInfo.cdma == rhs.CellInfo.cdma;
    case RIL_CELL_INFO_TYPE_LTE:
        return lhs.CellInfo.lte == rhs.CellInfo.lte;
    case RIL_CELL_INFO_TYPE_WCDMA:
        return lhs.CellInfo.wcdma == rhs.CellInfo.wcdma;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        return lhs.CellInfo.tdscdma == rhs.CellInfo.tdscdma;
    case RIL_CELL_INFO_TYPE_NR:
        return lhs.CellInfo.nr == rhs.CellInfo.nr;
    default:
        return false;
    }
}

bool operator==(const RIL_CellInfo_V1_6& lhs, const RIL_CellInfo_V1_6& rhs) {
    if (!(std::tie(lhs.cellInfoType, lhs.registered, lhs.connectionStatus) ==
            std::tie(rhs.cellInfoType, rhs.registered, rhs.connectionStatus))) {
        return false;
    }

    switch ((int)lhs.cellInfoType) {
    case RIL_CELL_INFO_TYPE_NONE:
        return true;
    case RIL_CELL_INFO_TYPE_GSM:
        return lhs.CellInfo.gsm == rhs.CellInfo.gsm;
    case RIL_CELL_INFO_TYPE_CDMA:
        return lhs.CellInfo.cdma == rhs.CellInfo.cdma;
    case RIL_CELL_INFO_TYPE_LTE:
        return lhs.CellInfo.lte == rhs.CellInfo.lte;
    case RIL_CELL_INFO_TYPE_WCDMA:
        return lhs.CellInfo.wcdma == rhs.CellInfo.wcdma;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        return lhs.CellInfo.tdscdma == rhs.CellInfo.tdscdma;
    case RIL_CELL_INFO_TYPE_NR:
        return lhs.CellInfo.nr == rhs.CellInfo.nr;
    default:
        return false;
    }
}