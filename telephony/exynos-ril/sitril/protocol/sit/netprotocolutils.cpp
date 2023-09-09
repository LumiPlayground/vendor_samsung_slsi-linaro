/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "netprotocolutils.h"
#include "networkutils.h"
#include "rillog.h"
#include "ts25table.h"
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include <telephony/ril_mnc.h>

namespace NetProtocolUtils {

int ConvertNetworktype2Sit(int netType)
{
    int net_type = SIT_NET_PREF_NET_TYPE_MAX;
    switch(netType) {
    case PREF_NET_TYPE_GSM_WCDMA:   //=0; /* GSM/WCDMA (WCDMA preferred) */
        net_type = SIT_NET_PREF_NET_TYPE_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_GSM_ONLY:    //= 1; /* GSM only */
        net_type = SIT_NET_PREF_NET_TYPE_GSM_ONLY;
        break;
    case PREF_NET_TYPE_WCDMA:   //= 2; /* WCDMA only */
        net_type = SIT_NET_PREF_NET_TYPE_WCDMA;
        break;
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:  //= 3; /* GSM/WCDMA (auto mode, according to PRL)
        net_type = SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO;
        break;
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:  //= 4; /* CDMA and EvDo (auto mode, according to PRL)
        net_type = SIT_NET_PREF_NET_TYPE_CDMA_EVDO_AUTO;
        break;
    case PREF_NET_TYPE_CDMA_ONLY:   //= 5; /* CDMA only */
        net_type = SIT_NET_PREF_NET_TYPE_CDMA_ONLY;
        break;
    case PREF_NET_TYPE_EVDO_ONLY:   //= 6; /* EvDo only */
        net_type = SIT_NET_PREF_NET_TYPE_EVDO_ONLY;
        break;
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:    //= 7; /* GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL)
        net_type = SIT_NET_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO;
        break;
    case PREF_NET_TYPE_LTE_CDMA_EVDO:   //= 8; /* LTE, CDMA and EvDo */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO;
        break;
    case PREF_NET_TYPE_LTE_GSM_WCDMA:   //= 9; /* LTE, GSM/WCDMA */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA: //10; /* LTE, CDMA, EvDo, GSM/WCDMA */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_LTE_ONLY:    //= 11; /* LTE Only mode. */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_ONLY;
        break;
    case PREF_NET_TYPE_LTE_WCDMA:   //= 12; /* LTE/WCDMA */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_WCDMA;
        break;

    // enum value is added from Android N. refer NETWORK_MODE_* in RILConstants.java
    case PREF_NET_TYPE_TD_SCDMA_ONLY:    //= 13; /* TD-SCDMA only */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_ONLY;
        break;
    case PREF_NET_TYPE_TD_SCDMA_WCDMA:   //= 14; /* TD-SCDMA and WCDMA */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_WCDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_LTE: //= 15; /* TD-SCDMA and LTE */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_GSM: //= 16; /* TD-SCDMA and GSM */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM;
        break;
    case PREF_NET_TYPE_TD_SCDMA_GSM_LTE: //= 17; /* TD-SCDMA,GSM and LTE */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM;
        break;
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA:   //= 18; /* TD-SCDMA, GSM/WCDMA */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE:   //= 19; /* TD-SCDMA, WCDMA and LTE */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_WCDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE:   //= 20; /* TD-SCDMA, GSM/WCDMA and LTE */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO: //= 21; /*TD-SCDMA,EvDo,CDMA,GSM/WCDMA*/
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_EVDO_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA: //= 22; /* TD-SCDMA/LTE/GSM/WCDMA, CDMA, and EvDo */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA;
        break;

    // Add NR feature
    case PREF_NET_TYPE_NR_ONLY:    //= 23
        net_type = SIT_NET_PREF_NET_TYPE_NR_ONLY;
        break;
    case PREF_NET_TYPE_NR_LTE:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE;
        break;
    case PREF_NET_TYPE_NR_LTE_CDMA_EVDO:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO;
        break;
    case PREF_NET_TYPE_NR_LTE_GSM_WCDMA:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM;
        break;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:    //= 33
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA;
        break;
    case PREF_NET_TYPE_NR_LTE_WCDMA:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_WCDMA;
        break;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA;
        break;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA:
        net_type = SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA;
        break;

    case PREF_NET_TYPE_TD_SCDMA_CDMA: //= 50; /* TD-SCDMA , CDMA and EvDo */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA;
        break;
    case PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO: //= 51; /* TD-SCDMA , CDMA */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_NO_EVDO;
        break;
    case PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE: //= 52; /* TD-SCDMA , LTE, CDMA and EvDo */
        net_type = SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO;
        break;
    case PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA: //= 53; /* TD-SCDMA , EVDO */
        net_type = SIT_NET_PREF_NET_TYPE_TDSCDMA_EVDO_NO_CDMA;
        break;

    default:
        net_type = SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA;
        break;
    }

    return net_type;
}

// caution:: RIL_RadioAccessNetworks and RIL_AccessNetwork have differnt raw value.
int ConvertRadioAccessNetworkType2Sit(int radioAccessNetworkType) {
    switch (radioAccessNetworkType) {
    case RIL_RAN_GERAN:
        return SIT_NET_ACCESS_RADIO_TYPE_GERAN;
    case RIL_RAN_UTRAN:
        return SIT_NET_ACCESS_RADIO_TYPE_UTRAN;
    case RIL_RAN_EUTRAN:
        return SIT_NET_ACCESS_RADIO_TYPE_EUTRAN;
    case RIL_RAN_NGRAN:
        return SIT_NET_ACCESS_RADIO_TYPE_NGRAN;
    case RIL_RAN_CDMA2000:
        return SIT_NET_ACCESS_RADIO_TYPE_CDMA2000;
    }
    return SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN;
}

int ConvertDsNetworkType2Sit(int dsNetType) {
    int sitDsNetType = SIT_NET_DS_NET_TYPE_GSM;

    switch (dsNetType) {
    // 3G + 2G
    case PREF_NET_TYPE_GSM_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:
    case PREF_NET_TYPE_TD_SCDMA_GSM:
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA:
        sitDsNetType = SIT_NET_DS_NET_TYPE_GSM_WCDMA;
        break;
    // 4G + 3G + 2G
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_WCDMA:
    case PREF_NET_TYPE_TD_SCDMA_GSM_LTE:
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE:
        sitDsNetType = SIT_NET_DS_NET_TYPE_LTE_GSM_WCDMA;
        break;
    // 2G
    case PREF_NET_TYPE_GSM_ONLY:
        sitDsNetType = SIT_NET_DS_NET_TYPE_GSM;
        break;
    // CDMA
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        sitDsNetType = SIT_NET_DS_NET_TYPE_CDMA_EVDO_AUTO;
        break;
    // 4G + CDMA
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
        sitDsNetType = SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO;
        break;
    // 4G + 3G + 2G + CDMA
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
    case PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
        sitDsNetType = SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA;
        break;
    // CDMA only or EVDO only
    case PREF_NET_TYPE_CDMA_ONLY:
    case PREF_NET_TYPE_EVDO_ONLY:
        sitDsNetType = SIT_NET_DS_NET_TYPE_CDMA;
        break;

    // not clarified
    case PREF_NET_TYPE_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_LTE_ONLY:
    case PREF_NET_TYPE_TD_SCDMA_ONLY:
    case PREF_NET_TYPE_TD_SCDMA_WCDMA:
    case PREF_NET_TYPE_TD_SCDMA_LTE:
    case PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE:
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_TD_SCDMA_CDMA:
    case PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO:
    case PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE:
    case PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA:
        sitDsNetType = SIT_NET_DS_NET_TYPE_GSM;
        break;
    default:
        sitDsNetType = SIT_NET_DS_NET_TYPE_GSM;
        break;
    } // end switch ~

    return sitDsNetType;
}

int ConvertNetworkScanType2Sit(int rilScanType) {
    if (rilScanType == RIL_ONE_SHOT) {
        return SIT_SCAN_TYPE_ONESHOT | (1 << 7);
    }
    else if (rilScanType == RIL_PERIODIC) {
        return SIT_SCAN_TYPE_PERIODIC | (1 << 7);
    }
    // not to start if invalid scan type
    return SIT_SCAN_TYPE_STOP;
}

int ConvertSitRaf2Ril(int raf, bool needToAdjust )
{
    int ret = 0;

    if (raf == RAF_CP_UNKNOWN && needToAdjust) {
        // use RAF default
        return NetworkUtils::getDefaultRAF();
    }

    // bit check
    if (raf & RAF_CP_GPRS) {
        ret |= RAF_GPRS;
    }
    if (raf & RAF_CP_EDGE) {
        ret |= RAF_EDGE;
    }
    if (raf & RAF_CP_UMTS) {
        ret |= RAF_UMTS;
    }
    if (raf & RAF_CP_HSDPA) {
        ret |= RAF_HSDPA;
    }
    if (raf & RAF_CP_HSUPA) {
        ret |= RAF_HSUPA;
    }
    if (raf & RAF_CP_HSPA) {
        ret |= RAF_HSPA;
    }
    if (raf & RAF_CP_LTE) {
        ret |= RAF_LTE;
        ret |= RAF_LTE_CA;
    }
    if (raf & RAF_CP_HSPAP) {
        ret |= RAF_HSPAP;
    }
    if (raf & RAF_CP_GSM) {
        ret |= RAF_GSM;
    }
    if (raf & RAF_CP_TD_SCDMA) {
        ret |= RAF_TD_SCDMA;
    }
    if (raf & RAF_CP_IS95A) {
        ret |= RAF_IS95A;
    }
    if (raf & RAF_CP_IS95B) {
        ret |= RAF_IS95B;
    }
    if (raf & RAF_CP_1xRTT) {
        ret |= RAF_1xRTT;
    }
    if (raf & RAF_CP_EVDO_0) {
        ret |= RAF_EVDO_0;
    }
    if (raf & RAF_CP_EVDO_A) {
        ret |= RAF_EVDO_A;
    }
    if (raf & RAF_CP_EVDO_B) {
        ret |= RAF_EVDO_B;
    }
    if (raf & RAF_CP_EHRPD) {
        ret |= RAF_EHRPD;
    }
    if (raf & RAF_CP_5G) {
        ret |= RAF_NR;
    }

    if (needToAdjust) ret = RadioConvUtils::getAdjustedRaf(ret);

    return ret;
}

int ConvertSitRat2RilRat(int sitRat)
{
    switch (sitRat) {
        case SIT_RAT_TYPE_GPRS: return RADIO_TECH_GPRS;
        case SIT_RAT_TYPE_EDGE: return RADIO_TECH_EDGE;
        case SIT_RAT_TYPE_UMTS: return RADIO_TECH_UMTS;
        case SIT_RAT_TYPE_IS95A:  return RADIO_TECH_IS95A;
        case SIT_RAT_TYPE_IS95B:  return RADIO_TECH_IS95B;
        case SIT_RAT_TYPE_1xRTT:  return RADIO_TECH_1xRTT;
        case SIT_RAT_TYPE_EVDO_0: return RADIO_TECH_EVDO_0;
        case SIT_RAT_TYPE_EVDO_A: return RADIO_TECH_EVDO_A;
        case SIT_RAT_TYPE_HSDPA: return RADIO_TECH_HSDPA;
        case SIT_RAT_TYPE_HSUPA: return RADIO_TECH_HSUPA;
        case SIT_RAT_TYPE_HSPA: return RADIO_TECH_HSPA;
        case SIT_RAT_TYPE_EVDO_B: return RADIO_TECH_EVDO_B;
        case SIT_RAT_TYPE_EHRPD: return RADIO_TECH_EHRPD;
        case SIT_RAT_TYPE_LTE: return RADIO_TECH_LTE;
        case SIT_RAT_TYPE_HSPAP: return RADIO_TECH_HSPAP;
        case SIT_RAT_TYPE_GSM: return RADIO_TECH_GSM;
        case SIT_RAT_TYPE_IWLAN: return RADIO_TECH_IWLAN;
        case SIT_RAT_TYPE_TD_SCDMA: return RADIO_TECH_TD_SCDMA;
        case SIT_RAT_TYPE_HSPADCPLUS: return RADIO_TECH_HSPAP;
        case SIT_RAT_TYPE_LTE_CA: return RADIO_TECH_LTE_CA;
        case SIT_RAT_TYPE_5G: return RADIO_TECH_NR;
        case SIT_RAT_TYPE_UNSPECIFIED: return RADIO_TECH_UNSPECIFIED;
        default:
            return RADIO_TECH_UNKNOWN;
    }
}

int ConvertSitRegState2Ril(int sitRegStatus)
{
    switch (sitRegStatus) {
        case SIT_NET_REG_STATE_REGISTERED:
            return REGISTERED_HOME;
        case SIT_NET_REG_STATE_NOT_REG_SEARCHING:
            return SEARCHING;
        case SIT_NET_REG_STATE_DENIED:
            return DENIED;
        case SIT_NET_REG_STATE_UNKNOWN:
            return UNKNOWN;
        case SIT_NET_REG_STATE_ROAMING:
            return REGISTERED_ROAMING;

        case SIT_NET_REG_STATE_NOT_REG_NO_SEARCH_EMGCALL:
            return NOT_REGISTERED_EMERGENCY_ONLY;
        case SIT_NET_REG_STATE_NOT_REG_SEARCHING_EMGCALL:
            return SEARCHING_EMERGENCY_ONLY;
        case SIT_NET_REG_STATE_DENIED_EMGCALL:
            return DENIED_EMERGENCY_ONLY;
        case SIT_NET_REG_STATE_UNKNOWN_EMGCALL:
            return UNKNOWN_EMERGENCY_ONLY;

        case SIT_NET_REG_STATE_DENIED_ROAMING:
            return DENIED_ROAMING;
        default:
            return NOT_REGISTERED;
    }
}

int ConvertPrefNetworkType2Ril(int sitNetType)
{
    // networkTypeMap[i][0] : RIL type
    // networkTypeMap[i][1] : SIT type
    static int networkTypeMap[][2] = {
        { PREF_NET_TYPE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_GSM_WCDMA},
        { PREF_NET_TYPE_GSM_ONLY, SIT_NET_PREF_NET_TYPE_GSM_ONLY},
        { PREF_NET_TYPE_WCDMA, SIT_NET_PREF_NET_TYPE_WCDMA},
        { PREF_NET_TYPE_GSM_WCDMA_AUTO, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO},
        { PREF_NET_TYPE_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_CDMA_EVDO_AUTO},
        { PREF_NET_TYPE_CDMA_ONLY, SIT_NET_PREF_NET_TYPE_CDMA_ONLY},
        { PREF_NET_TYPE_EVDO_ONLY, SIT_NET_PREF_NET_TYPE_EVDO_ONLY},
        { PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO},
        { PREF_NET_TYPE_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO},
        { PREF_NET_TYPE_LTE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_GSM_WCDMA},
        { PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA},
        { PREF_NET_TYPE_LTE_ONLY, SIT_NET_PREF_NET_TYPE_LTE_ONLY},
        { PREF_NET_TYPE_LTE_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_WCDMA}, // 12
        { PREF_NET_TYPE_TD_SCDMA_ONLY, SIT_NET_PREF_NET_TYPE_TDSCDMA_ONLY},
        { PREF_NET_TYPE_TD_SCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_WCDMA},
        { PREF_NET_TYPE_TD_SCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA},
        { PREF_NET_TYPE_TD_SCDMA_GSM, SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM},
        { PREF_NET_TYPE_TD_SCDMA_GSM_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM},
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_GSM_WCDMA},
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_GSM_WCDMA},
        { PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_EVDO_GSM_WCDMA},
        { PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA}, //22
        { PREF_NET_TYPE_NR_ONLY, SIT_NET_PREF_NET_TYPE_NR_ONLY},
        { PREF_NET_TYPE_NR_LTE, SIT_NET_PREF_NET_TYPE_NR_LTE},
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO},
        { PREF_NET_TYPE_NR_LTE_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA},
        { PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA},
        { PREF_NET_TYPE_NR_LTE_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_WCDMA}, //28
        { PREF_NET_TYPE_NR_LTE_TDSCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA},
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM},
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA},
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA},
        { PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA, SIT_NET_PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA}, //33
        { PREF_NET_TYPE_TD_SCDMA_CDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA}, // 50
        { PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO, SIT_NET_PREF_NET_TYPE_TDSCDMA_CDMA_NO_EVDO},
        { PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE, SIT_NET_PREF_NET_TYPE_LTE_TDSCDMA_CDMA_EVDO},
        { PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA, SIT_NET_PREF_NET_TYPE_TDSCDMA_EVDO_NO_CDMA},
    };
    int size = sizeof(networkTypeMap) / sizeof(networkTypeMap[0]);
    int rilNetType = -1;
    for (int i = 0; i < size; i++) {
        if (networkTypeMap[i][1] == sitNetType) {
            rilNetType = networkTypeMap[i][0];
            break;
        }
    } // end for i ~

    if (rilNetType < 0) {
        RilLogW("no matched network type. current netType=%d", sitNetType);
        rilNetType = PREF_NET_TYPE_GSM_WCDMA;
    }

    return rilNetType;
}

const char *ConvertPlmnStatus2Ril(int plmnStatus) {
    const char *ret = STR_NETWORK_STATUS_UNKNOWN;
    switch (plmnStatus) {
    case SIT_NET_PLMN_STATUS_AVAILABLE:
        ret = STR_NETWORK_STATUS_AVAILABLE;
        break;
    case SIT_NET_PLMN_STATUS_CURRENT:
        ret = STR_NETWORK_STATUS_CURRENT;
        break;
    case SIT_NET_PLMN_STATUS_FORBIDDEN:
        ret = STR_NETWORK_STATUS_FORBIDDEN;
        break;
    }

    return ret;
}

int ConvertSitCdmaRoamingType2Ril(int sitValue)
{
    switch(sitValue) {
    case SIT_CDMA_RM_AFFILIATED:
        return CDMA_ROAMING_AFFILIATED_NETWORKS;
    case SIT_CDMA_RM_ANY:
        return CDMA_ROAMING_ANY_NETWORK;
    case SIT_CDMA_RM_HOME:
    default:
        return CDMA_ROAMING_HOME_ONLY;
    }
}

const char *ConvertSitDsNetworkType2String(int sitDsNetType) {
    switch (sitDsNetType) {
    case SIT_NET_DS_NET_TYPE_GSM_WCDMA: return "DS_NET_TYPE_GSM_WCDMA";
    case SIT_NET_DS_NET_TYPE_LTE_GSM_WCDMA: return "DS_NET_TYPE_LTE_GSM_WCDMA";
    case SIT_NET_DS_NET_TYPE_GSM: return "DS_NET_TYPE_GSM";
    case SIT_NET_DS_NET_TYPE_CDMA_EVDO_AUTO: return "DS_NET_TYPE_CDMA_EVDO_AUTO";
    case SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO: return "DS_NET_TYPE_LTE_CDMA_EVDO";
    case SIT_NET_DS_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA: return "DS_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA";
    case SIT_NET_DS_NET_TYPE_CDMA: return "DS_NET_TYPE_CDMA";
    default: return "<unknown sit ds net type>";
    }
}

const char *ConvertPwrRadioSimState2String(int pwrRadioSimState)
{
    switch (pwrRadioSimState) {
    case SIT_PWR_RADIO_SIM_STATE_OFF:
        return "SIT_PWR_RADIO_SIM_STATE_OFF";
    case SIT_PWR_RADIO_SIM_STATE_UNAVAILABLE:
        return "SIT_PWR_RADIO_SIM_STATE_UNAVAILABLE";
    case SIT_PWR_RADIO_SIM_STATE_SIM_NOT_READY:
        return "SIT_PWR_RADIO_SIM_STATE_SIM_NOT_READY";
    case SIT_PWR_RADIO_SIM_STATE_SIM_LOCK_OR_ABSENT:
        return "SIT_PWR_RADIO_SIM_STATE_SIM_LOCK_OR_ABSENT";
    case SIT_PWR_RADIO_SIM_STATE_SIM_READY:
        return "SIT_PWR_RADIO_SIM_STATE_SIM_READY";
    case SIT_PWR_RADIO_SIM_STATE_RUIM_NOT_READY:
        return "SIT_PWR_RADIO_SIM_STATE_RUIM_NOT_READY";
    case SIT_PWR_RADIO_SIM_STATE_RUIM_LOCK_OR_ABSENT:
        return "SIT_PWR_RADIO_SIM_STATE_RUIM_LOCK_OR_ABSENT";
    case SIT_PWR_RADIO_SIM_STATE_RUIM_READY:
        return "SIT_PWR_RADIO_SIM_STATE_RUIM_READY";
    case SIT_PWR_RADIO_SIM_STATE_NV_NOT_READY:
        return "SIT_PWR_RADIO_SIM_STATE_NV_NOT_READY";
    case SIT_PWR_RADIO_SIM_STATE_NV_READY:
        return "SIT_PWR_RADIO_SIM_STATE_NV_READY";
    case SIT_PWR_RADIO_SIM_STATE_ON:
        return "SIT_PWR_RADIO_SIM_STATE_ON";
    }
    return "SIT_PWR_RADIO_STATE_UNKNOWN";
}

const char *ConvertRadioState2String(int radioState)
{
    switch (radioState) {
    case SIT_PWR_RADIO_STATE_INITIALIZED:
        return "SIT_PWR_RADIO_STATE_INITIALIZED";
    case SIT_PWR_RADIO_STATE_STOP_NETWORK:
        return "SIT_PWR_RADIO_STATE_STOP_NETWORK";
    case SIT_PWR_RADIO_STATE_START_NETWORK:
        return "SIT_PWR_RADIO_STATE_START_NETWORK";
    case SIT_PWR_RADIO_STATE_POWER_OFF:
        return "SIT_PWR_RADIO_STATE_POWER_OFF";
    case SIT_PWR_RADIO_STATE_RESET:
        return "SIT_PWR_RADIO_STATE_RESET";
    }
    return "SIT_PWR_RADIO_STATE_UNKNOWN";
}

const char *ConvertSitPreferredNetType2String(int sitNetType)
{
    switch (sitNetType) {
    case SIT_NET_PREF_NET_TYPE_GSM_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_GSM_WCDMA";
    case SIT_NET_PREF_NET_TYPE_GSM_ONLY:
        return "SIT_NET_PREF_NET_TYPE_GSM_ONLY";
    case SIT_NET_PREF_NET_TYPE_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_WCDMA";
    case SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO:
        return "SIT_NET_PREF_NET_TYPE_GSM_WCDMA_AUTO";
    case SIT_NET_PREF_NET_TYPE_CDMA_EVDO_AUTO:
        return "SIT_NET_PREF_NET_TYPE_CDMA_EVDO_AUTO";
    case SIT_NET_PREF_NET_TYPE_CDMA_ONLY:
        return "SIT_NET_PREF_NET_TYPE_CDMA_ONLY";
    case SIT_NET_PREF_NET_TYPE_EVDO_ONLY:
        return "SIT_NET_PREF_NET_TYPE_EVDO_ONLY";
    case SIT_NET_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
        return "SIT_NET_PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO";
    case SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO:
        return "SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO";
    case SIT_NET_PREF_NET_TYPE_LTE_GSM_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_LTE_GSM_WCDMA";
    case SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_LTE_CDMA_EVDO_GSM_WCDMA";
    case SIT_NET_PREF_NET_TYPE_LTE_ONLY:
        return "SIT_NET_PREF_NET_TYPE_LTE_ONLY";
    case SIT_NET_PREF_NET_TYPE_LTE_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_LTE_WCDMA";
    case SIT_NET_PREF_NET_TYPE_NR_ONLY:
        return "SIT_NET_PREF_NET_TYPE_NR_ONLY";
    case SIT_NET_PREF_NET_TYPE_NR_LTE:
        return "SIT_NET_PREF_NET_TYPE_NR_LTE";
    case SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO:
        return "SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO";
    case SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_NR_LTE_GSM_WCDMA";
    case SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA";
    case SIT_NET_PREF_NET_TYPE_NR_LTE_WCDMA:
        return "SIT_NET_PREF_NET_TYPE_NR_LTE_WCDMA";
    } // end switch ~
    return "UNSUPPORTED_PREF_NET_TYPE";
}

const char *ConvertSitCdmaHybridMode2String(int sitHybridMode)
{
    switch (sitHybridMode) {
    case HYBRID_MODE_1X_HRPD:
        return "HYBRID_MODE_1X_HRPD";
    case HYBRID_MODE_1X_ONLY:
        return "HYBRID_MODE_1X_ONLY";
    case HYBRID_MODE_HRPD_ONLY:
        return "HYBRID_MODE_HRPD_ONLY";
    case HYBRID_MODE_1X_EHRPD:
        return "HYBRID_MODE_1X_EHRPD";
    case HYBRID_MODE_EHRPD_ONLY:
        return "HYBRID_MODE_EHRPD_ONLY";
    }
    return "UNSUPPORTED_CDMA_HYBRID_MODE";
}

int IsSysSelSupported(int sitRan) {
    if (sitRan == SIT_NET_ACCESS_RADIO_TYPE_GERAN ||
        sitRan == SIT_NET_ACCESS_RADIO_TYPE_UTRAN ||
        sitRan == SIT_NET_ACCESS_RADIO_TYPE_EUTRAN ||
        sitRan == SIT_NET_ACCESS_RADIO_TYPE_NGRAN) {
        return true;
    } else {
        return false;
    }
}

void InitAcBarringInfo(AC_BARRING_INFO &info)
{
    info.for_emc = 0;
    info.for_mo_sig_factor = 100;
    info.for_mo_sig_time = -1;
    memset(info.for_mo_sig_ac_list, 0x00, sizeof(info.for_mo_sig_ac_list));

    info.for_mo_data_factor = 100;
    info.for_mo_data_time = -1;
    memset(info.for_mo_data_ac_list, 0x00, sizeof(info.for_mo_data_ac_list));

    info.for_mmtel_voice_factor = 100;
    info.for_mmtel_voice_time = -1;
    memset(info.for_mmtel_voice_ac_list, 0x00, sizeof(info.for_mmtel_voice_ac_list));

    info.for_mmtel_video_factor = 100;
    info.for_mmtel_video_time = -1;
    memset(info.for_mmtel_video_ac_list, 0x00, sizeof(info.for_mmtel_video_ac_list));
}

bool IsValidCellInfo(RIL_CellInfo_V1_6& cellInfo) {
    if (cellInfo.cellInfoType == RIL_CELL_INFO_TYPE_NONE) {
        return false;
    }

    if (cellInfo.cellInfoType == RIL_CELL_INFO_TYPE_CDMA) {
        return true;
    }

    int mcc = 0;
    int mnc = 0;

    switch ((int)cellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            mcc = cellInfo.CellInfo.gsm.cellIdentityGsm.mcc;
            mnc = cellInfo.CellInfo.gsm.cellIdentityGsm.mnc;
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            mcc = cellInfo.CellInfo.lte.cellIdentityLte.mcc;
            mnc = cellInfo.CellInfo.lte.cellIdentityLte.mnc;
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            mcc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mcc;
            mnc = cellInfo.CellInfo.wcdma.cellIdentityWcdma.mnc;
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            mcc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mcc;
            mnc = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.mnc;
            break;
        case RIL_CELL_INFO_TYPE_NR:
            mcc = cellInfo.CellInfo.nr.cellIdentityNr.mcc;
            mnc = cellInfo.CellInfo.nr.cellIdentityNr.mnc;
            break;
        default:
            RilLogV("Unsupported CellInfoType %d",(int)cellInfo.cellInfoType);
            break;
    }

    return !((mcc == INT_MAX && mnc != INT_MAX) || (mcc != INT_MAX && mnc == INT_MAX) || mcc == 0);
}

void FreeAdditionalPlmms(RIL_CellInfo_V1_6& cellInfo) {
    int additionalPlmnNum = 0;
    char **additionalPlmn = NULL;

    switch ((int)cellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        additionalPlmnNum = cellInfo.CellInfo.gsm.cellIdentityGsm.leng_additionalPlmns;
        additionalPlmn = cellInfo.CellInfo.gsm.cellIdentityGsm.additionalPlmns;
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        /* Do noting */
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        additionalPlmnNum = cellInfo.CellInfo.lte.cellIdentityLte.leng_additionalPlmns;
        additionalPlmn = cellInfo.CellInfo.lte.cellIdentityLte.additionalPlmns;
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        additionalPlmnNum = cellInfo.CellInfo.wcdma.cellIdentityWcdma.leng_additionalPlmns;
        additionalPlmn = cellInfo.CellInfo.wcdma.cellIdentityWcdma.additionalPlmns;
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        additionalPlmnNum = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.leng_additionalPlmns;
        additionalPlmn = cellInfo.CellInfo.tdscdma.cellIdentityTdscdma.additionalPlmns;
        break;
    case RIL_CELL_INFO_TYPE_NR:
        additionalPlmnNum = cellInfo.CellInfo.nr.cellIdentityNr.leng_additionalPlmns;
        additionalPlmn = cellInfo.CellInfo.nr.cellIdentityNr.additionalPlmns;
        break;
    default:
        /* Do nothing */
        break;
    }

    if (additionalPlmn != NULL) {
        for (int i = 0; i < additionalPlmnNum; ++i) {
            free (*(additionalPlmn+i));
        }
        free(additionalPlmn);
    }
}

}  // NetProtocolUtils

namespace CellInfoProtocolUtils {

void InitCellInfo(RIL_CellInfo_V1_6& info, int type) {
    switch (type) {
        case RIL_CELL_INFO_TYPE_GSM:
            // RIL_CellInfoGsm_V1_5
            info.CellInfo.gsm.cellIdentityGsm.mcc = INT_MAX;
            info.CellInfo.gsm.cellIdentityGsm.mnc = INT_MAX;
            info.CellInfo.gsm.cellIdentityGsm.lac = INT_MAX;
            info.CellInfo.gsm.cellIdentityGsm.cid = INT_MAX;
            info.CellInfo.gsm.cellIdentityGsm.arfcn = INT_MAX;
            info.CellInfo.gsm.cellIdentityGsm.bsic = 0xff;

            info.CellInfo.gsm.signalStrengthGsm.signalStrength = 99;
            info.CellInfo.gsm.signalStrengthGsm.bitErrorRate = 99;
            info.CellInfo.gsm.signalStrengthGsm.timingAdvance = INT_MAX;
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            // RIL_CellInfoCdma_V1_2
            info.CellInfo.cdma.cellIdentityCdma.networkId = INT_MAX;
            info.CellInfo.cdma.cellIdentityCdma.systemId = INT_MAX;
            info.CellInfo.cdma.cellIdentityCdma.basestationId = INT_MAX;
            info.CellInfo.cdma.cellIdentityCdma.longitude = INT_MAX;
            info.CellInfo.cdma.cellIdentityCdma.latitude = INT_MAX;

            info.CellInfo.cdma.signalStrengthCdma.dbm = 0;
            info.CellInfo.cdma.signalStrengthCdma.ecio = 0;

            info.CellInfo.cdma.signalStrengthEvdo.dbm = 0;
            info.CellInfo.cdma.signalStrengthEvdo.ecio = 0;
            info.CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio = INT_MAX;
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            // RIL_CellInfoLte_V1_6
            info.CellInfo.lte.cellIdentityLte.mcc = INT_MAX;
            info.CellInfo.lte.cellIdentityLte.mnc = INT_MAX;
            info.CellInfo.lte.cellIdentityLte.ci = INT_MAX;
            info.CellInfo.lte.cellIdentityLte.pci = INT_MAX;
            info.CellInfo.lte.cellIdentityLte.tac = INT_MAX;
            info.CellInfo.lte.cellIdentityLte.earfcn = INT_MAX;
            info.CellInfo.lte.cellIdentityLte.bandwidth = INT_MAX;

            info.CellInfo.lte.signalStrengthLte.signalStrength = 99;
            info.CellInfo.lte.signalStrengthLte.rsrp = INT_MAX;
            info.CellInfo.lte.signalStrengthLte.rsrq = INT_MAX;
            info.CellInfo.lte.signalStrengthLte.rssnr = INT_MAX;
            info.CellInfo.lte.signalStrengthLte.cqi = INT_MAX;
            info.CellInfo.lte.signalStrengthLte.timingAdvance = INT_MAX;
            info.CellInfo.lte.signalStrengthLte.cqiTableIndex = INT_MAX;

            info.CellInfo.lte.isEndcAvailable = false;

            info.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.csgIndication = false;
            info.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName[0] = 0;
            // a unique 27-bit CSG Identity. IntRage(from = 1, to = 0x7FFFFFF, INT_MAX invalid)
            info.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity = INT_MAX;
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            // RIL_CellInfoWcdma_V1_5
            info.CellInfo.wcdma.cellIdentityWcdma.mcc = INT_MAX;
            info.CellInfo.wcdma.cellIdentityWcdma.mnc = INT_MAX;
            info.CellInfo.wcdma.cellIdentityWcdma.lac = INT_MAX;
            info.CellInfo.wcdma.cellIdentityWcdma.cid = INT_MAX;
            info.CellInfo.wcdma.cellIdentityWcdma.psc = INT_MAX;
            info.CellInfo.wcdma.cellIdentityWcdma.uarfcn = INT_MAX;

            info.CellInfo.wcdma.signalStrengthWcdma.signalStrength = 99;
            info.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate = 99;
            info.CellInfo.wcdma.signalStrengthWcdma.rscp = 255;
            info.CellInfo.wcdma.signalStrengthWcdma.ecno = 255;

            info.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIndication = false;
            info.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName[0] = 0;
            // a unique 27-bit CSG Identity. IntRage(from = 1, to = 0x7FFFFFF, INT_MAX invalid)
            info.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity = INT_MAX;
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            // RIL_CellInfoTdscdma_V1_5
            info.CellInfo.tdscdma.cellIdentityTdscdma.mcc = INT_MAX;
            info.CellInfo.tdscdma.cellIdentityTdscdma.mnc = INT_MAX;
            info.CellInfo.tdscdma.cellIdentityTdscdma.lac = INT_MAX;
            info.CellInfo.tdscdma.cellIdentityTdscdma.cid = INT_MAX;
            info.CellInfo.tdscdma.cellIdentityTdscdma.cpid = INT_MAX;
            info.CellInfo.tdscdma.cellIdentityTdscdma.uarfcn = INT_MAX;

            info.CellInfo.tdscdma.signalStrengthTdscdma.signalStrength = 99;
            info.CellInfo.tdscdma.signalStrengthTdscdma.bitErrorRate = 99;
            info.CellInfo.tdscdma.signalStrengthTdscdma.rscp = 255;

            info.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.csgIndication = false;
            info.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.homeNodebName[0] = 0;
            // a unique 27-bit CSG Identity. IntRage(from = 1, to = 0x7FFFFFF, INT_MAX invalid)
            info.CellInfo.tdscdma.cellIdentityTdscdma.optionalCsgInfo.csgInfo.csgIdentity = INT_MAX;
            break;
        case RIL_CELL_INFO_TYPE_NR:
            // RIL_CellInfoNr_V1_6
            info.CellInfo.nr.cellIdentityNr.mcc = INT_MAX;
            info.CellInfo.nr.cellIdentityNr.mnc = INT_MAX;
            info.CellInfo.nr.cellIdentityNr.nci = LONG_MAX;
            info.CellInfo.nr.cellIdentityNr.pci = INT_MAX;
            info.CellInfo.nr.cellIdentityNr.tac = INT_MAX;
            info.CellInfo.nr.cellIdentityNr.nrarfcn = INT_MAX;

            info.CellInfo.nr.signalStrengthNr.ssRsrp = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.ssRsrq = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.ssSinr = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.csiRsrp = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.csiRsrq = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.csiSinr = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.csiCqiTableIndex = INT_MAX;
            info.CellInfo.nr.signalStrengthNr.csiCqiReportLen = 0;
            break;
    }
}

void PrintCellInfo(int index, RIL_CellInfo_V1_6& cellInfo) {
    switch ((int)cellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            RilLog("[%d] RIL_CELL_INFO_TYPE_GSM", index);
            // TODO print more information
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            RilLog("[%d] RIL_CELL_INFO_TYPE_CDMA", index);
            // TODO print more information
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            RilLog("[%d] RIL_CELL_INFO_TYPE_LTE", index);
            // TODO print more information
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            RilLog("[%d] RIL_CELL_INFO_TYPE_WCDMA", index);
            // TODO print more information
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            RilLog("[%d] RIL_CELL_INFO_TYPE_TD_SCDMA", index);
            // TODO print more information
            break;
        case RIL_CELL_INFO_TYPE_NR:
            RilLog("[%d] RIL_CELL_INFO_TYPE_NR", index);
            // TODO print more information
            break;
        default:
            RilLog("[%d] Unsupported CellInfoType %d", index, (int)cellInfo.cellInfoType);
            break;
    }
}

namespace Legacy {

int FetchMcc(const char *plmn) {
    char mccStr[MAX_MCC_LEN + 1];
    memset(mccStr, 0, sizeof(mccStr));

    if (plmn != NULL) {
        memcpy(mccStr, plmn, MAX_MCC_LEN);
        if (TextUtils::IsDigitsOnly(mccStr)) {
            int mcc = strtol(mccStr, NULL, 10);
            return mcc;
        }
    }
    return INT_MAX;
}

int FetchMnc(const char *plmn) {
    int mcc = FetchMcc(plmn);
    if (mcc == 0 || mcc == INT_MAX) {
        return INT_MAX;
    }

    char mncStr[MAX_MNC_LEN + 1];
    memset(mncStr, 0, sizeof(mncStr));

    if (plmn != NULL) {
        memcpy(mncStr, plmn + MAX_MCC_LEN, MAX_MNC_LEN);
        if (mncStr[2] == '#') {
            mncStr[2] = 0;
        }

        if (TextUtils::IsDigitsOnly(mncStr)) {
            int mnc = strtol(mncStr, NULL, 10);
            int mccLength = strlen(mncStr);
            mnc = ril::util::mnc::encode(mnc, mccLength);
            return mnc;
        }
    }
    return INT_MAX;
}

void FillOperatorName(RIL_CellIdentityOperatorNames& name, int mcc, int encodedMnc) {
    TS25Record ret;
    int mnc = strtol(ril::util::mnc::decode(encodedMnc).c_str(), NULL, 10);
    RilLog("[%s] Find TS25Table for mcc=%d mnc=%d", __FUNCTION__, mcc, mnc);
    TS25Table *table = TS25Table::GetInstance();
    if (table != NULL) {
        ret = table->GetRecord(mcc, mnc);
    }

    if (ret.IsValid()) {
        strncpy(name.alphaLong, ret.ppcin.c_str(), MAX_FULL_NAME_LEN-1);
        strncpy(name.alphaShort, ret.networkName.c_str(), MAX_SHORT_NAME_LEN-1);
        RilLog("[%s] TS25 record found %d/%d/%s/%s", __FUNCTION__,
                mcc, mnc, name.alphaLong, name.alphaShort);
    } else {
        RilLog("[%s] No TS25 record for mcc=%d, mnc=%d", __FUNCTION__, mcc, mnc);
    }
}

void FillGsmCellInfo(RIL_CellInfoGsm_V1_5& out, cell_info_gsm_v12& cellInfo) {
    out.cellIdentityGsm.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityGsm.mnc = FetchMnc(cellInfo.plmn);

    // 16-bit LAC
    out.cellIdentityGsm.lac = cellInfo.lac;
    if (out.cellIdentityGsm.lac < 0 || out.cellIdentityGsm.lac > 0xFFFF)
        out.cellIdentityGsm.lac = INT32_MAX;

    // 16-bit GSM CID
    out.cellIdentityGsm.cid = cellInfo.cid;
    if (out.cellIdentityGsm.cid < 0 || out.cellIdentityGsm.cid > 0xFFFF)
        out.cellIdentityGsm.cid = INT32_MAX;

    // 16-bit GSM ARFCN
    out.cellIdentityGsm.arfcn = cellInfo.arfcn;
    if (out.cellIdentityGsm.arfcn < 0 || out.cellIdentityGsm.arfcn > 0xFFFF)
        out.cellIdentityGsm.arfcn = INT32_MAX;

    // 6-bit GSM BSIC
    out.cellIdentityGsm.bsic = cellInfo.bsic;
    if (out.cellIdentityGsm.bsic > 0x3F)
        out.cellIdentityGsm.bsic = 0xFF;

    // Operator name
    FillOperatorName(out.cellIdentityGsm.operatorNames, out.cellIdentityGsm.mcc, out.cellIdentityGsm.mnc);

    // BER (0-7,99)
    out.signalStrengthGsm.bitErrorRate = cellInfo.sig_ber;
    if (out.signalStrengthGsm.bitErrorRate < 0 || out.signalStrengthGsm.bitErrorRate > 7)
        out.signalStrengthGsm.bitErrorRate = 99;

    // Signal Strength (0-31,99)
    out.signalStrengthGsm.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthGsm.signalStrength < 0 || out.signalStrengthGsm.signalStrength > 31)
        out.signalStrengthGsm.signalStrength = 99;

    // Timing Advance (0 ~63)
    out.signalStrengthGsm.timingAdvance = cellInfo.sig_ta;
    if (out.signalStrengthGsm.timingAdvance < 0 || out.signalStrengthGsm.timingAdvance > 63)
        out.signalStrengthGsm.timingAdvance = INT32_MAX;
}

void FillGsmCellInfo(RIL_CellInfoGsm_V1_5& out, cell_info_type_gsm_v3& cellInfo) {
    FillCellIdentityGsm(out.cellIdentityGsm, cellInfo.cell_identity);

    // BER (0-7,99)
    out.signalStrengthGsm.bitErrorRate = cellInfo.sig_ber;
    if (out.signalStrengthGsm.bitErrorRate < 0 || out.signalStrengthGsm.bitErrorRate > 7)
        out.signalStrengthGsm.bitErrorRate = 99;

    // Signal Strength (0-31,99)
    out.signalStrengthGsm.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthGsm.signalStrength < 0 || out.signalStrengthGsm.signalStrength > 31)
        out.signalStrengthGsm.signalStrength = 99;

    // Timing Advance (0~63)
    out.signalStrengthGsm.timingAdvance = cellInfo.sig_ta;
    if (out.signalStrengthGsm.timingAdvance < 0 || out.signalStrengthGsm.timingAdvance > 63)
        out.signalStrengthGsm.timingAdvance = INT32_MAX;
}

void FillGsmCellInfo(RIL_CellInfoGsm_V1_5& out, cell_info_type_gsm_v4& cellInfo) {
    FillCellIdentityGsm(out.cellIdentityGsm, cellInfo.cell_identity);

    // BER (0-7,99)
    out.signalStrengthGsm.bitErrorRate = cellInfo.sig_ber;
    if (out.signalStrengthGsm.bitErrorRate < 0 || out.signalStrengthGsm.bitErrorRate > 7)
        out.signalStrengthGsm.bitErrorRate = 99;

    // Signal Strength (0-31,99)
    out.signalStrengthGsm.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthGsm.signalStrength < 0 || out.signalStrengthGsm.signalStrength > 31)
        out.signalStrengthGsm.signalStrength = 99;

    // Timing Advance (0~63)
    out.signalStrengthGsm.timingAdvance = cellInfo.sig_ta;
    if (out.signalStrengthGsm.timingAdvance < 0 || out.signalStrengthGsm.timingAdvance > 63)
        out.signalStrengthGsm.timingAdvance = INT32_MAX;
}

void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_cdma& cellInfo) {
    fillCdmaCellInfo_V1_2(out, cellInfo);
}

template <typename T>
void fillCdmaCellInfo_V1_2(RIL_CellInfoCdma_V1_2 &out, T &cellInfo) {
    // 16-bit Network Id
    out.cellIdentityCdma.networkId = cellInfo.ntw_id;
    if (out.cellIdentityCdma.networkId < 0 || out.cellIdentityCdma.networkId > 0xFFFF)
        out.cellIdentityCdma.networkId = INT32_MAX;

    // 15-bit CDMA System Id
    out.cellIdentityCdma.systemId = cellInfo.sys_id;
    if (out.cellIdentityCdma.systemId < 0 || out.cellIdentityCdma.systemId > 0x7FFF)
        out.cellIdentityCdma.systemId = INT32_MAX;

    // 16-bit Base Station Id
    out.cellIdentityCdma.basestationId = cellInfo.bs_id;
    if (out.cellIdentityCdma.basestationId < 0 || out.cellIdentityCdma.basestationId > 0xFFFF)
        out.cellIdentityCdma.basestationId = INT32_MAX;

    // Longitude (-2592000-2592000)
    out.cellIdentityCdma.longitude = cellInfo.longitude;
    if (out.cellIdentityCdma.longitude < -2592000 || out.cellIdentityCdma.longitude > 2592000)
        out.cellIdentityCdma.longitude = INT32_MAX;

    // Latitude (-1296000-1296000)
    out.cellIdentityCdma.latitude = cellInfo.lat;
    if (out.cellIdentityCdma.latitude < -1296000 || out.cellIdentityCdma.latitude > 1296000)
        out.cellIdentityCdma.latitude = INT32_MAX;

    // actual RSSI value (multiplied by -1)
    out.signalStrengthCdma.dbm = cellInfo.sig_dbm;
    if (out.signalStrengthCdma.dbm < 0)
        out.signalStrengthCdma.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthCdma.ecio = cellInfo.sig_ecio;
    if (out.signalStrengthCdma.ecio < 0)
        out.signalStrengthCdma.ecio *= -10;

    // actual RSSI value (multiplied by -1)
    out.signalStrengthEvdo.dbm = cellInfo.evdo_sig_dbm;
    if (out.signalStrengthEvdo.dbm < 0)
        out.signalStrengthEvdo.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthEvdo.ecio = cellInfo.evdo_sig_ecio;
    if (out.signalStrengthEvdo.ecio < 0)
        out.signalStrengthEvdo.ecio *= -10;

    // signal noise ratio (0-8)
    out.signalStrengthEvdo.signalNoiseRatio = cellInfo.evdo_sig_snr;
    if (out.signalStrengthEvdo.signalNoiseRatio < 0 || out.signalStrengthEvdo.signalNoiseRatio > 8)
        out.signalStrengthEvdo.signalNoiseRatio = INT_MAX;
}

void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_cdma_v14& cellInfo) {
    fillCdmaCellInfo_V1_2(out, cellInfo);
}
/*
    // 16-bit Network Id
    out.cellIdentityCdma.networkId = cellInfo.ntw_id;
    if (out.cellIdentityCdma.networkId < 0 || out.cellIdentityCdma.networkId > 0xFFFF)
        out.cellIdentityCdma.networkId = INT32_MAX;

    // 15-bit CDMA System Id
    out.cellIdentityCdma.systemId = cellInfo.sys_id;
    if (out.cellIdentityCdma.systemId < 0 || out.cellIdentityCdma.systemId > 0x7FFF)
        out.cellIdentityCdma.systemId = INT32_MAX;

    // 16-bit Base Station Id
    out.cellIdentityCdma.basestationId = cellInfo.bs_id;
    if (out.cellIdentityCdma.basestationId < 0 || out.cellIdentityCdma.basestationId > 0xFFFF)
        out.cellIdentityCdma.basestationId = INT32_MAX;

    // Longitude (-2592000-2592000)
    out.cellIdentityCdma.longitude = cellInfo.longitude;
    if (out.cellIdentityCdma.longitude < -2592000 || out.cellIdentityCdma.longitude > 2592000)
        out.cellIdentityCdma.longitude = INT32_MAX;

    // Latitude (-1296000-1296000)
    out.cellIdentityCdma.latitude = cellInfo.lat;
    if (out.cellIdentityCdma.latitude < -1296000 || out.cellIdentityCdma.latitude > 1296000)
        out.cellIdentityCdma.latitude = INT32_MAX;

    // actual RSSI value (multiplied by -1)
    out.signalStrengthCdma.dbm = cellInfo.sig_dbm;
    if (out.signalStrengthCdma.dbm < 0)
        out.signalStrengthCdma.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthCdma.ecio = cellInfo.sig_ecio;
    if (out.signalStrengthCdma.ecio < 0)
        out.signalStrengthCdma.ecio *= -10;

    // actual RSSI value (multiplied by -1)
    out.signalStrengthEvdo.dbm = cellInfo.evdo_sig_dbm;
    if (out.signalStrengthEvdo.dbm < 0)
        out.signalStrengthEvdo.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthEvdo.ecio = cellInfo.evdo_sig_ecio;
    if (out.signalStrengthEvdo.ecio < 0)
        out.signalStrengthEvdo.ecio *= -10;

    // signal noise ratio (0-8)
    out.signalStrengthEvdo.signalNoiseRatio = cellInfo.evdo_sig_snr;
    if (out.signalStrengthEvdo.signalNoiseRatio < 0 || out.signalStrengthEvdo.signalNoiseRatio > 8)
        out.signalStrengthEvdo.signalNoiseRatio = INT_MAX;
}
*/

void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_type_cdma_v3& cellInfo) {
    FillCellIdentityCdma(out.cellIdentityCdma, cellInfo.cell_identity);

    // actual RSSI value (multiplied by -1)
    out.signalStrengthCdma.dbm = cellInfo.cdma_dbm;
    if (out.signalStrengthCdma.dbm < 0)
        out.signalStrengthCdma.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthCdma.ecio = cellInfo.cdma_ecio;
    if (out.signalStrengthCdma.ecio < 0)
        out.signalStrengthCdma.ecio *= -10;

    // actual RSSI value (multiplied by -1)
    out.signalStrengthEvdo.dbm = cellInfo.evdo_dbm;
    if (out.signalStrengthEvdo.dbm < 0)
        out.signalStrengthEvdo.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthEvdo.ecio = cellInfo.evdo_ecio;
    if (out.signalStrengthEvdo.ecio < 0)
        out.signalStrengthEvdo.ecio *= -10;

    // signal noise ratio (0-8)
    out.signalStrengthEvdo.signalNoiseRatio = cellInfo.evdo_snr;
    if (out.signalStrengthEvdo.signalNoiseRatio < 0 || out.signalStrengthEvdo.signalNoiseRatio > 8)
        out.signalStrengthEvdo.signalNoiseRatio = INT_MAX;
}

void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_type_cdma_v4& cellInfo) {
    FillCellIdentityCdma(out.cellIdentityCdma, cellInfo.cell_identity);

    // actual RSSI value (multiplied by -1)
    out.signalStrengthCdma.dbm = cellInfo.cdma_dbm;
    if (out.signalStrengthCdma.dbm < 0)
        out.signalStrengthCdma.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthCdma.ecio = cellInfo.cdma_ecio;
    if (out.signalStrengthCdma.ecio < 0)
        out.signalStrengthCdma.ecio *= -10;

    // actual RSSI value (multiplied by -1)
    out.signalStrengthEvdo.dbm = cellInfo.evdo_dbm;
    if (out.signalStrengthEvdo.dbm < 0)
        out.signalStrengthEvdo.dbm *= -1;

    // actual Ec/Io (multiplied by -10)
    out.signalStrengthEvdo.ecio = cellInfo.evdo_ecio;
    if (out.signalStrengthEvdo.ecio < 0)
        out.signalStrengthEvdo.ecio *= -10;

    // signal noise ratio (0-8)
    out.signalStrengthEvdo.signalNoiseRatio = cellInfo.evdo_snr;
    if (out.signalStrengthEvdo.signalNoiseRatio < 0 || out.signalStrengthEvdo.signalNoiseRatio > 8)
        out.signalStrengthEvdo.signalNoiseRatio = INT_MAX;
}

void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_lte_v12& cellInfo) {
    out.cellIdentityLte.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityLte.mnc = FetchMnc(cellInfo.plmn);

    // 28-bit CID
    out.cellIdentityLte.ci = cellInfo.cell_id;
    if (out.cellIdentityLte.ci < 0 || out.cellIdentityLte.ci > 0xFFFFFFF)
        out.cellIdentityLte.ci = INT32_MAX;

    // Physical CID
    out.cellIdentityLte.pci = cellInfo.phy_cell_id;
    if (out.cellIdentityLte.pci < 0 || out.cellIdentityLte.pci > 503)
        out.cellIdentityLte.pci = INT32_MAX;

    // 16-bit TAC
    out.cellIdentityLte.tac = cellInfo.tac;
    if (out.cellIdentityLte.tac < 0 || out.cellIdentityLte.tac > 0xFFFF)
        out.cellIdentityLte.tac = INT32_MAX;

    // 18-bit earfcn
    out.cellIdentityLte.earfcn = cellInfo.earfcn;
    if (out.cellIdentityLte.earfcn < 0 || out.cellIdentityLte.earfcn > 0x3FFFF)
        out.cellIdentityLte.earfcn = INT32_MAX;

    // Operator name
    FillOperatorName(out.cellIdentityLte.operatorNames, out.cellIdentityLte.mcc, out.cellIdentityLte.mnc);

    // Signal Strength (0-31,99)
    out.signalStrengthLte.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthLte.signalStrength < 0 || out.signalStrengthLte.signalStrength > 31)
        out.signalStrengthLte.signalStrength = 99;

    // RSRP (44-140)
    out.signalStrengthLte.rsrp = cellInfo.sig_rsrp;
    if (out.signalStrengthLte.rsrp < 44 || out.signalStrengthLte.rsrp > 140)
        out.signalStrengthLte.rsrp = INT32_MAX;

    // RSRQ (3-20)
    out.signalStrengthLte.rsrq = cellInfo.sig_rsrq;
    if (out.signalStrengthLte.rsrq < 3 || out.signalStrengthLte.rsrq > 20)
        out.signalStrengthLte.rsrq = INT32_MAX;

    // RSSNR (-200 - 300)
    out.signalStrengthLte.rssnr = cellInfo.sig_rssnr;
    if (out.signalStrengthLte.rssnr < -200 || out.signalStrengthLte.rssnr > 300)
        out.signalStrengthLte.rssnr = INT32_MAX;

    // CQI (0-15)
    out.signalStrengthLte.cqi = cellInfo.sig_cqi;
    if (out.signalStrengthLte.cqi < 0 || out.signalStrengthLte.cqi > 15)
        out.signalStrengthLte.cqi = INT32_MAX;

    // TAVD (0-0x7FFFFFFE)
    out.signalStrengthLte.timingAdvance = cellInfo.ta;
    if (out.signalStrengthLte.timingAdvance < 0 || out.signalStrengthLte.timingAdvance > 0x7FFFFFFE)
        out.signalStrengthLte.timingAdvance = INT32_MAX;
}

void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_lte_v14& cellInfo) {
    out.cellIdentityLte.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityLte.mnc = FetchMnc(cellInfo.plmn);

    // 28-bit CID
    out.cellIdentityLte.ci = cellInfo.cell_id;
    if (out.cellIdentityLte.ci < 0 || out.cellIdentityLte.ci > 0xFFFFFFF)
        out.cellIdentityLte.ci = INT32_MAX;

    // Physical CID
    out.cellIdentityLte.pci = cellInfo.phy_cell_id;
    if (out.cellIdentityLte.pci < 0 || out.cellIdentityLte.pci > 503)
        out.cellIdentityLte.pci = INT32_MAX;

    // 16-bit TAC
    out.cellIdentityLte.tac = cellInfo.tac;
    if (out.cellIdentityLte.tac < 0 || out.cellIdentityLte.tac > 0xFFFF)
        out.cellIdentityLte.tac = INT32_MAX;

    // 18-bit earfcn
    out.cellIdentityLte.earfcn = cellInfo.earfcn;
    if (out.cellIdentityLte.earfcn < 0 || out.cellIdentityLte.earfcn > 0x3FFFF)
        out.cellIdentityLte.earfcn = INT32_MAX;

    out.cellIdentityLte.bandwidth = cellInfo.bandwidth;
    if (out.cellIdentityLte.bandwidth < 0)
        out.cellIdentityLte.bandwidth = INT32_MAX;

    // Operator name
    FillOperatorName(out.cellIdentityLte.operatorNames, out.cellIdentityLte.mcc, out.cellIdentityLte.mnc);

    // E-UTRA-NR Dual Connectivity available
    out.isEndcAvailable = (cellInfo.endc_available == 1)? true: false;

    // Signal Strength (0-31,99)
    out.signalStrengthLte.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthLte.signalStrength < 0 || out.signalStrengthLte.signalStrength > 31)
        out.signalStrengthLte.signalStrength = 99;

    // RSRP (44-140)
    out.signalStrengthLte.rsrp = cellInfo.sig_rsrp;
    if (out.signalStrengthLte.rsrp < 44 || out.signalStrengthLte.rsrp > 140)
        out.signalStrengthLte.rsrp = INT32_MAX;

    // RSRQ (3-20)
    out.signalStrengthLte.rsrq = cellInfo.sig_rsrq;
    if (out.signalStrengthLte.rsrq < 3 || out.signalStrengthLte.rsrq > 20)
        out.signalStrengthLte.rsrq = INT32_MAX;

    // RSSNR (-200 - 300)
    out.signalStrengthLte.rssnr = cellInfo.sig_rssnr;
    if (out.signalStrengthLte.rssnr < -200 || out.signalStrengthLte.rssnr > 300)
        out.signalStrengthLte.rssnr = INT32_MAX;

    // CQI (0-15)
    out.signalStrengthLte.cqi = cellInfo.sig_cqi;
    if (out.signalStrengthLte.cqi < 0 || out.signalStrengthLte.cqi > 15)
        out.signalStrengthLte.cqi = INT32_MAX;

    // TAVD (0-0x7FFFFFFE)
    out.signalStrengthLte.timingAdvance = cellInfo.ta;
    if (out.signalStrengthLte.timingAdvance < 0 || out.signalStrengthLte.timingAdvance > 0x7FFFFFFE)
        out.signalStrengthLte.timingAdvance = INT32_MAX;
}

void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_type_lte_v3& cellInfo) {
    FillCellIdentityLte(out.cellIdentityLte, cellInfo.cell_identity);

    // E-UTRA-NR Dual Connectivity available
    out.isEndcAvailable = (cellInfo.endc_available == 1)? true: false;

    // Signal Strength (0-31,99)
    out.signalStrengthLte.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthLte.signalStrength < 0 || out.signalStrengthLte.signalStrength > 31)
        out.signalStrengthLte.signalStrength = 99;

    // RSRP (44-140)
    out.signalStrengthLte.rsrp = cellInfo.sig_rsrp;
    if (out.signalStrengthLte.rsrp < 44 || out.signalStrengthLte.rsrp > 140)
        out.signalStrengthLte.rsrp = INT32_MAX;

    // RSRQ (3-20)
    out.signalStrengthLte.rsrq = cellInfo.sig_rsrq;
    if (out.signalStrengthLte.rsrq < 3 || out.signalStrengthLte.rsrq > 20)
        out.signalStrengthLte.rsrq = INT32_MAX;

    // RSSNR (-200 - 300)
    out.signalStrengthLte.rssnr = cellInfo.sig_rssnr;
    if (out.signalStrengthLte.rssnr < -200 || out.signalStrengthLte.rssnr > 300)
        out.signalStrengthLte.rssnr = INT32_MAX;

    // CQI (0-15)
    out.signalStrengthLte.cqi = cellInfo.cqi;
    if (out.signalStrengthLte.cqi < 0 || out.signalStrengthLte.cqi > 15)
        out.signalStrengthLte.cqi = INT32_MAX;

    // TAVD (0-0x7FFFFFFE)
    out.signalStrengthLte.timingAdvance = cellInfo.ta;
    if (out.signalStrengthLte.timingAdvance < 0 || out.signalStrengthLte.timingAdvance > 0x7FFFFFFE)
        out.signalStrengthLte.timingAdvance = INT32_MAX;
}

void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_type_lte_v4& cellInfo) {
    FillCellIdentityLte(out.cellIdentityLte, cellInfo.cell_identity);

    // E-UTRA-NR Dual Connectivity available
    out.isEndcAvailable = (cellInfo.endc_available == 1)? true: false;

    // Signal Strength (0-31,99)
    out.signalStrengthLte.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthLte.signalStrength < 0 || out.signalStrengthLte.signalStrength > 31)
        out.signalStrengthLte.signalStrength = 99;

    // RSRP (44-140)
    out.signalStrengthLte.rsrp = cellInfo.sig_rsrp;
    if (out.signalStrengthLte.rsrp < 44 || out.signalStrengthLte.rsrp > 140)
        out.signalStrengthLte.rsrp = INT32_MAX;

    // RSRQ (3-20)
    out.signalStrengthLte.rsrq = cellInfo.sig_rsrq;
    if (out.signalStrengthLte.rsrq < 3 || out.signalStrengthLte.rsrq > 20)
        out.signalStrengthLte.rsrq = INT32_MAX;

    // RSSNR (-200 - 300)
    out.signalStrengthLte.rssnr = cellInfo.sig_rssnr;
    if (out.signalStrengthLte.rssnr < -200 || out.signalStrengthLte.rssnr > 300)
        out.signalStrengthLte.rssnr = INT32_MAX;

    // CQI Table Index
    out.signalStrengthLte.cqiTableIndex = cellInfo.cqi_table_index;
    if (out.signalStrengthLte.cqiTableIndex < 1 || out.signalStrengthLte.cqiTableIndex > 6)
        out.signalStrengthLte.cqiTableIndex = INT32_MAX;

    // CQI (0-15)
    out.signalStrengthLte.cqi = cellInfo.cqi;
    if (out.signalStrengthLte.cqi < 0 || out.signalStrengthLte.cqi > 15)
        out.signalStrengthLte.cqi = INT32_MAX;

    // TAVD (0-0x7FFFFFFE)
    out.signalStrengthLte.timingAdvance = cellInfo.ta;
    if (out.signalStrengthLte.timingAdvance < 0 || out.signalStrengthLte.timingAdvance > 0x7FFFFFFE)
        out.signalStrengthLte.timingAdvance = INT32_MAX;
}

template <typename T>
static void fillWcdmaCellInfo_V1_5(RIL_CellInfoWcdma_V1_5 &out, T &cellInfo) {
    out.cellIdentityWcdma.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityWcdma.mnc = FetchMnc(cellInfo.plmn);

    // 16-bit LAC
    out.cellIdentityWcdma.lac = cellInfo.lac;
    if (out.cellIdentityWcdma.lac < 0 || out.cellIdentityWcdma.lac > 0xFFFF)
        out.cellIdentityWcdma.lac = INT32_MAX;

    // 28-bit UMTS CID
    out.cellIdentityWcdma.cid = cellInfo.cid;
    if (out.cellIdentityWcdma.cid < 0 || out.cellIdentityWcdma.cid > 0xFFFFFFF)
        out.cellIdentityWcdma.cid = INT32_MAX;

    // 9-bit UMTS PSC
    out.cellIdentityWcdma.psc = cellInfo.psc;
    if (out.cellIdentityWcdma.psc < 0 || out.cellIdentityWcdma.psc > 0x1FF)
        out.cellIdentityWcdma.psc = INT32_MAX;

    // 16-bit UMTS uarfcn
    out.cellIdentityWcdma.uarfcn = cellInfo.uarfcn;
    if (out.cellIdentityWcdma.uarfcn < 0 || out.cellIdentityWcdma.uarfcn > 0xFFFF)
        out.cellIdentityWcdma.uarfcn = INT32_MAX;

    // Operator name
    FillOperatorName(out.cellIdentityWcdma.operatorNames, out.cellIdentityWcdma.mcc, out.cellIdentityWcdma.mnc);

    // BER (0-7,99)
    out.signalStrengthWcdma.bitErrorRate = cellInfo.sig_ber;
    if (out.signalStrengthWcdma.bitErrorRate < 0 || out.signalStrengthWcdma.bitErrorRate > 7)
        out.signalStrengthWcdma.bitErrorRate = 99;

    // Signal Strength (0-31,99)
    out.signalStrengthWcdma.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthWcdma.signalStrength < 0 || out.signalStrengthWcdma.signalStrength > 31)
        out.signalStrengthWcdma.signalStrength = 99;
}

void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5 &out, cell_info_wcdma_v12 &cellInfo) {
    fillWcdmaCellInfo_V1_5(out, cellInfo);
}

void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5 &out, cell_info_wcdma_v14 &cellInfo) {
    fillWcdmaCellInfo_V1_5(out, cellInfo);

    // Received Signal Code Power (0-96,255)
    out.signalStrengthWcdma.rscp = (cellInfo.rscp < 0)? 0:cellInfo.rscp;
    if (out.signalStrengthWcdma.rscp > 96)
        out.signalStrengthWcdma.rscp = 255;

    // Ec/No Value (0-49,255)
    out.signalStrengthWcdma.ecno = (cellInfo.ecno < 0)? 0:cellInfo.ecno;
    if (out.signalStrengthWcdma.ecno > 49)
        out.signalStrengthWcdma.ecno = 255;
}

void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5& out, cell_info_type_wcdma_v3& cellInfo) {
    FillCellIdentityWcdma(out.cellIdentityWcdma, cellInfo.cell_identity);

    // BER (0-7,99)
    out.signalStrengthWcdma.bitErrorRate = cellInfo.sig_ber;
    if (out.signalStrengthWcdma.bitErrorRate < 0 || out.signalStrengthWcdma.bitErrorRate > 7)
        out.signalStrengthWcdma.bitErrorRate = 99;

    // Signal Strength (0-31,99)
    out.signalStrengthWcdma.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthWcdma.signalStrength < 0 || out.signalStrengthWcdma.signalStrength > 31)
        out.signalStrengthWcdma.signalStrength = 99;

    // Received Signal Code Power (0-96,255)
    out.signalStrengthWcdma.rscp = (cellInfo.rscp < 0)? 0:cellInfo.rscp;
    if (out.signalStrengthWcdma.rscp > 96)
        out.signalStrengthWcdma.rscp = 255;

    // Ec/No Value (0-49,255)
    out.signalStrengthWcdma.ecno = (cellInfo.ecno < 0)? 0:cellInfo.ecno;
    if (out.signalStrengthWcdma.ecno > 49)
        out.signalStrengthWcdma.ecno = 255;
}

void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5& out, cell_info_type_wcdma_v4& cellInfo) {
    FillCellIdentityWcdma(out.cellIdentityWcdma, cellInfo.cell_identity);

    // BER (0-7,99)
    out.signalStrengthWcdma.bitErrorRate = cellInfo.sig_ber;
    if (out.signalStrengthWcdma.bitErrorRate < 0 || out.signalStrengthWcdma.bitErrorRate > 7)
        out.signalStrengthWcdma.bitErrorRate = 99;

    // Signal Strength (0-31,99)
    out.signalStrengthWcdma.signalStrength = cellInfo.sig_str;
    if (out.signalStrengthWcdma.signalStrength < 0 || out.signalStrengthWcdma.signalStrength > 31)
        out.signalStrengthWcdma.signalStrength = 99;

    // Received Signal Code Power (0-96,255)
    out.signalStrengthWcdma.rscp = (cellInfo.rscp < 0)? 0:cellInfo.rscp;
    if (out.signalStrengthWcdma.rscp > 96)
        out.signalStrengthWcdma.rscp = 255;

    // Ec/No Value (0-49,255)
    out.signalStrengthWcdma.ecno = (cellInfo.ecno < 0)? 0:cellInfo.ecno;
    if (out.signalStrengthWcdma.ecno > 49)
        out.signalStrengthWcdma.ecno = 255;
}

void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_tdscdma& cellInfo) {
    out.cellIdentityTdscdma.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityTdscdma.mnc = FetchMnc(cellInfo.plmn);

    // 16-bit LAC
    out.cellIdentityTdscdma.lac = cellInfo.lac;
    if (out.cellIdentityTdscdma.lac < 0 || out.cellIdentityTdscdma.lac > 0xFFFF)
        out.cellIdentityTdscdma.lac = INT32_MAX;

    // 28-bit UMTS CID
    out.cellIdentityTdscdma.cid = cellInfo.cid;
    if (out.cellIdentityTdscdma.cid < 0 || out.cellIdentityTdscdma.cid > 0xFFFFFFF)
        out.cellIdentityTdscdma.cid = INT32_MAX;

    // 8-bit CPID
    out.cellIdentityTdscdma.cpid = cellInfo.cpid;
    if (out.cellIdentityTdscdma.cpid < 0 || out.cellIdentityTdscdma.cpid > 0xFF)
        out.cellIdentityTdscdma.cpid = INT32_MAX;

    // Operator name
    FillOperatorName(out.cellIdentityTdscdma.operatorNames, out.cellIdentityTdscdma.mcc, out.cellIdentityTdscdma.mnc);

    // RSCP (25-120)
    out.signalStrengthTdscdma.rscp = cellInfo.rscp;
    if (out.signalStrengthTdscdma.rscp < 25 || out.signalStrengthTdscdma.rscp > 120)
        out.signalStrengthTdscdma.rscp = INT32_MAX;
}

void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_tdscdma_v14& cellInfo) {
    out.cellIdentityTdscdma.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityTdscdma.mnc = FetchMnc(cellInfo.plmn);

    // 16-bit LAC
    out.cellIdentityTdscdma.lac = cellInfo.lac;
    if (out.cellIdentityTdscdma.lac < 0 || out.cellIdentityTdscdma.lac > 0xFFFF)
        out.cellIdentityTdscdma.lac = INT32_MAX;

    // 28-bit UMTS CID
    out.cellIdentityTdscdma.cid = cellInfo.cid;
    if (out.cellIdentityTdscdma.cid < 0 || out.cellIdentityTdscdma.cid > 0xFFFFFFF)
        out.cellIdentityTdscdma.cid = INT32_MAX;

    // 8-bit CPID
    out.cellIdentityTdscdma.cpid = cellInfo.cpid;
    if (out.cellIdentityTdscdma.cpid < 0 || out.cellIdentityTdscdma.cpid > 0xFF)
        out.cellIdentityTdscdma.cpid = INT32_MAX;

    // Operator name
    FillOperatorName(out.cellIdentityTdscdma.operatorNames, out.cellIdentityTdscdma.mcc, out.cellIdentityTdscdma.mnc);

    // Received Signal Strength Indication (RSSI) measured from TDSCDMA (0-31,99)
    out.signalStrengthTdscdma.signalStrength = (cellInfo.sig_str < 0)? 0: cellInfo.sig_str;
    if (out.signalStrengthTdscdma.signalStrength > 31)
        out.signalStrengthTdscdma.signalStrength = 99;

    // Bit error rate (0-7, 99)
    out.signalStrengthTdscdma.bitErrorRate = (cellInfo.ber < 0)? 0:cellInfo.ber;
    if (out.signalStrengthTdscdma.bitErrorRate > 7)
        out.signalStrengthTdscdma.bitErrorRate = 99;

    // RSCP (25-120)
    out.signalStrengthTdscdma.rscp = cellInfo.rscp;
    if (out.signalStrengthTdscdma.rscp < 25 || out.signalStrengthTdscdma.rscp > 120)
        out.signalStrengthTdscdma.rscp = INT32_MAX;
}

void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_type_tdscdma_v3& cellInfo) {
    FillCellIdentityTdscdma(out.cellIdentityTdscdma, cellInfo.cell_identity);

    // Received Signal Strength Indication (RSSI) measured from TDSCDMA (0-31,99)
    out.signalStrengthTdscdma.signalStrength = (cellInfo.sig_str < 0)? 0: cellInfo.sig_str;
    if (out.signalStrengthTdscdma.signalStrength > 31)
        out.signalStrengthTdscdma.signalStrength = 99;

    // Bit error rate (0-7, 99)
    out.signalStrengthTdscdma.bitErrorRate = (cellInfo.ber < 0)? 0:cellInfo.ber;
    if (out.signalStrengthTdscdma.bitErrorRate > 7)
        out.signalStrengthTdscdma.bitErrorRate = 99;

    // RSCP (0-96, 255)
    out.signalStrengthTdscdma.rscp = cellInfo.rscp;
    if (out.signalStrengthTdscdma.rscp > 96)
        out.signalStrengthTdscdma.rscp = 255;

}

void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_type_tdscdma_v4& cellInfo) {
    FillCellIdentityTdscdma(out.cellIdentityTdscdma, cellInfo.cell_identity);

    // Received Signal Strength Indication (RSSI) measured from TDSCDMA (0-31,99)
    out.signalStrengthTdscdma.signalStrength = (cellInfo.sig_str < 0)? 0: cellInfo.sig_str;
    if (out.signalStrengthTdscdma.signalStrength > 31)
        out.signalStrengthTdscdma.signalStrength = 99;

    // Bit error rate (0-7, 99)
    out.signalStrengthTdscdma.bitErrorRate = (cellInfo.ber < 0)? 0:cellInfo.ber;
    if (out.signalStrengthTdscdma.bitErrorRate > 7)
        out.signalStrengthTdscdma.bitErrorRate = 99;

    // RSCP (0-96, 255)
    out.signalStrengthTdscdma.rscp = cellInfo.rscp;
    if (out.signalStrengthTdscdma.rscp > 96)
        out.signalStrengthTdscdma.rscp = 255;
}

void FillNrCellInfo(RIL_CellInfoNr_V1_6& out, cell_info_nr& cellInfo) {
    out.cellIdentityNr.mcc = FetchMcc(cellInfo.plmn);
    out.cellIdentityNr.mnc = FetchMnc(cellInfo.plmn);

    // Cell Identity (36 bits)
    out.cellIdentityNr.nci = cellInfo.cell_id;
    if (out.cellIdentityNr.nci > 68719476735)
        out.cellIdentityNr.nci = LONG_MAX;

    // Physical cell id (0-1007)
    out.cellIdentityNr.pci = cellInfo.phy_cell_id;
    if (out.cellIdentityNr.pci > 1007)
        out.cellIdentityNr.pci = INT32_MAX;

    // 16-bit tracking area code
    out.cellIdentityNr.tac = cellInfo.tac;
    if (out.cellIdentityNr.tac < 0 || out.cellIdentityNr.tac > 16777215)
        out.cellIdentityNr.tac = INT32_MAX;

    // NR Absolute Radio Frequency Channel Number (0-3279165)
    out.cellIdentityNr.nrarfcn = cellInfo.arfcn;
    if (out.cellIdentityNr.nrarfcn < 0 || out.cellIdentityNr.nrarfcn > 3279165)
        out.cellIdentityNr.nrarfcn = INT32_MAX;

    // Operator name
    FillOperatorName(out.cellIdentityNr.operatorNames, out.cellIdentityNr.mcc, out.cellIdentityNr.mnc);

    // SS reference signal received power (44-140)
    out.signalStrengthNr.ssRsrp = cellInfo.ss_rsrp;
    if (out.signalStrengthNr.ssRsrp < 44 || out.signalStrengthNr.ssRsrp > 140)
        out.signalStrengthNr.ssRsrp = INT32_MAX;

    // SS reference signal received quality (3-20)
    out.signalStrengthNr.ssRsrq = cellInfo.ss_rsrq;
    if (out.signalStrengthNr.ssRsrq < 3 || out.signalStrengthNr.ssRsrq > 20)
        out.signalStrengthNr.ssRsrq = INT32_MAX;

    // SS signal-to-noise and interference ratio (-23~40)
    out.signalStrengthNr.ssSinr = cellInfo.ss_sinr;
    if (out.signalStrengthNr.ssSinr < -23 || out.signalStrengthNr.ssSinr > 40)
        out.signalStrengthNr.ssSinr = INT32_MAX;

    // CSI reference signal received power (44-140)
    out.signalStrengthNr.csiRsrp = cellInfo.csi_rsrp;
    if (out.signalStrengthNr.csiRsrp < 44 || out.signalStrengthNr.csiRsrp > 140)
        out.signalStrengthNr.csiRsrp = INT32_MAX;

    // CSI reference signal received quality (3-20)
    out.signalStrengthNr.csiRsrq = cellInfo.csi_rsrq;
    if (out.signalStrengthNr.csiRsrq < 3 || out.signalStrengthNr.csiRsrq > 20)
        out.signalStrengthNr.csiRsrq = INT32_MAX;

    // CSI signal-to-noise and interference ratio (-23~40)
    out.signalStrengthNr.csiSinr = cellInfo.csi_sinr;
    if (out.signalStrengthNr.csiSinr < -23 || out.signalStrengthNr.csiSinr > 40)
        out.signalStrengthNr.csiSinr = INT32_MAX;
}

void FillNrCellInfo(RIL_CellInfoNr_V1_6& out, cell_info_type_nr_v3& cellInfo) {
    FillCellIdentityNr(out.cellIdentityNr, cellInfo.cell_identity);

    // SS reference signal received power (44-140)
    out.signalStrengthNr.ssRsrp = cellInfo.ss_rsrp;
    if (out.signalStrengthNr.ssRsrp < 44 || out.signalStrengthNr.ssRsrp > 140)
        out.signalStrengthNr.ssRsrp = INT32_MAX;

    // SS reference signal received quality (3-20)
    out.signalStrengthNr.ssRsrq = cellInfo.ss_rsrq;
    if (out.signalStrengthNr.ssRsrq < 3 || out.signalStrengthNr.ssRsrq > 20)
        out.signalStrengthNr.ssRsrq = INT32_MAX;

    // SS signal-to-noise and interference ratio (-23~40)
    out.signalStrengthNr.ssSinr = cellInfo.ss_sinr;
    if (out.signalStrengthNr.ssSinr < -23 || out.signalStrengthNr.ssSinr > 40)
        out.signalStrengthNr.ssSinr = INT32_MAX;

    // CSI reference signal received power (44-140)
    out.signalStrengthNr.csiRsrp = cellInfo.csi_rsrp;
    if (out.signalStrengthNr.csiRsrp < 44 || out.signalStrengthNr.csiRsrp > 140)
        out.signalStrengthNr.csiRsrp = INT32_MAX;

    // CSI reference signal received quality (3-20)
    out.signalStrengthNr.csiRsrq = cellInfo.csi_rsrq;
    if (out.signalStrengthNr.csiRsrq < 3 || out.signalStrengthNr.csiRsrq > 20)
        out.signalStrengthNr.csiRsrq = INT32_MAX;

    // CSI signal-to-noise and interference ratio (-23~40)
    out.signalStrengthNr.csiSinr = cellInfo.csi_sinr;
    if (out.signalStrengthNr.csiSinr < -23 || out.signalStrengthNr.csiSinr > 40)
        out.signalStrengthNr.csiSinr = INT32_MAX;
}

void FillNrCellInfo(RIL_CellInfoNr_V1_6& out, cell_info_type_nr_v4& cellInfo) {
    FillCellIdentityNr(out.cellIdentityNr, cellInfo.cell_identity);

    // SS reference signal received power (44-140)
    out.signalStrengthNr.ssRsrp = cellInfo.ss_rsrp;
    if (out.signalStrengthNr.ssRsrp < 44 || out.signalStrengthNr.ssRsrp > 140)
        out.signalStrengthNr.ssRsrp = INT32_MAX;

    // SS reference signal received quality (3-20)
    out.signalStrengthNr.ssRsrq = cellInfo.ss_rsrq;
    if (out.signalStrengthNr.ssRsrq < 3 || out.signalStrengthNr.ssRsrq > 20)
        out.signalStrengthNr.ssRsrq = INT32_MAX;

    // SS signal-to-noise and interference ratio (-23~40)
    out.signalStrengthNr.ssSinr = cellInfo.ss_sinr;
    if (out.signalStrengthNr.ssSinr < -23 || out.signalStrengthNr.ssSinr > 40)
        out.signalStrengthNr.ssSinr = INT32_MAX;

    // CSI reference signal received power (44-140)
    out.signalStrengthNr.csiRsrp = cellInfo.csi_rsrp;
    if (out.signalStrengthNr.csiRsrp < 44 || out.signalStrengthNr.csiRsrp > 140)
        out.signalStrengthNr.csiRsrp = INT32_MAX;

    // CSI reference signal received quality (3-20)
    out.signalStrengthNr.csiRsrq = cellInfo.csi_rsrq;
    if (out.signalStrengthNr.csiRsrq < 3 || out.signalStrengthNr.csiRsrq > 20)
        out.signalStrengthNr.csiRsrq = INT32_MAX;

    // CSI signal-to-noise and interference ratio (-23~40)
    out.signalStrengthNr.csiSinr = cellInfo.csi_sinr;
    if (out.signalStrengthNr.csiSinr < -23 || out.signalStrengthNr.csiSinr > 40)
        out.signalStrengthNr.csiSinr = INT32_MAX;

    // CQI Table Index (1~3)
    out.signalStrengthNr.csiCqiTableIndex = cellInfo.cqi_table_index;
    if (out.signalStrengthNr.csiCqiTableIndex < 1 || out.signalStrengthNr.csiCqiTableIndex > 3)
        out.signalStrengthNr.csiCqiTableIndex = INT32_MAX;

    // CQI
    if (cellInfo.wb_cqi != INT_MAX) {
        out.signalStrengthNr.csiCqiReportLen = 1;
        out.signalStrengthNr.csiCqiReport[0] = cellInfo.wb_cqi;
    } else {
        out.signalStrengthNr.csiCqiReportLen = SIT_MAX_NR_SUBBAND_INDEX;
        memcpy(out.signalStrengthNr.csiCqiReport, cellInfo.sb_cqi, sizeof(int) * SIT_MAX_NR_SUBBAND_INDEX);
    }
}

// CellIdentity helper functions
void FillCellIdentityGsm(RIL_CellIdentityGsm_V1_5& out, cell_identity_gsm_v3& cellIdentity) {
    out.mcc = FetchMcc(cellIdentity.plmn);
    out.mnc = FetchMnc(cellIdentity.plmn);

    // 16-bit LAC
    out.lac = cellIdentity.lac;
    if (out.lac < 0 || out.lac > 0xFFFF)
        out.lac = INT32_MAX;

    // 16-bit GSM CID
    out.cid = cellIdentity.cid;
    if (out.cid < 0 || out.cid > 0xFFFF)
        out.cid = INT32_MAX;

    // 16-bit GSM ARFCN
    out.arfcn = cellIdentity.arfcn;
    if (out.arfcn < 0 || out.arfcn > 0xFFFF)
        out.arfcn = INT32_MAX;

    // 6-bit GSM BSIC
    out.bsic = cellIdentity.bsic;
    if (out.bsic > 0x3F)
        out.bsic = 0xFF;

    // Operator name
    if (cellIdentity.plmn_info.plmn_short_name[0] != 0)
        strncpy(out.operatorNames.alphaShort, cellIdentity.plmn_info.plmn_short_name, MAX_SHORT_NAME_LEN-1);
    if (cellIdentity.plmn_info.plmn_long_name[0] != 0)
        strncpy(out.operatorNames.alphaLong, cellIdentity.plmn_info.plmn_long_name, MAX_FULL_NAME_LEN-1);

    /*
    // Additional PLMN
    out.leng_additionalPlmns = cellIdentity.plmn_info.additional_plmn_len;
    if (out.leng_additionalPlmns > 0) {
    char additionalPlmns[EACH_PLMN_LEN + 1];
    int leng_additionalPlmns = out.leng_additionalPlmns;

    for(int i = 0; i < leng_additionalPlmns; i++) {
    memset(additionalPlmns, 0, EACH_PLMN_LEN + 1);
    memcpy(additionalPlmns, cellIdentity.plmn_info.additional_plmn_info[(i*EACH_PLMN_LEN)], EACH_PLMN_LEN);
    std::string str(additionalPlmns);
    out.additionalPlmns.push_back(str);
    }
    }
     */
}

void FillCellIdentityGsm(RIL_CellIdentityGsm_V1_5& out, cell_identity_gsm_v4& cellIdentity) {
    FillCellIdentityGsm(out, *((cell_identity_gsm_v3 *)&cellIdentity));
}

void FillCellIdentityCdma(RIL_CellIdentityCdma_V1_2& out, cell_identity_cdma_v3& cellIdentity) {
    // 16-bit Network Id
    out.networkId = cellIdentity.ntw_id;
    if (out.networkId < 0 || out.networkId > 0xFFFF)
        out.networkId = INT32_MAX;

    // 15-bit CDMA System Id
    out.systemId = cellIdentity.sys_id;
    if (out.systemId < 0 || out.systemId > 0x7FFF)
        out.systemId = INT32_MAX;

    // 16-bit Base Station Id
    out.basestationId = cellIdentity.bs_id;
    if (out.basestationId < 0 || out.basestationId > 0xFFFF)
        out.basestationId = INT32_MAX;

    // Longitude (-2592000-2592000)
    out.longitude = cellIdentity.longitude;
    if (out.longitude < -2592000 || out.longitude > 2592000)
        out.longitude = INT32_MAX;

    // Latitude (-1296000-1296000)
    out.latitude = cellIdentity.latitude;
    if (out.latitude < -1296000 || out.latitude > 1296000)
        out.latitude = INT32_MAX;
}

void FillCellIdentityCdma(RIL_CellIdentityCdma_V1_2& out, cell_identity_cdma_v4& cellIdentity) {
    FillCellIdentityCdma(out, *((cell_identity_cdma_v3 *)&cellIdentity));
}

void FillCellIdentityLte(RIL_CellIdentityLte_V1_5& out, cell_identity_lte_v3& cellIdentity) {
    out.mcc = FetchMcc(cellIdentity.plmn);
    out.mnc = FetchMnc(cellIdentity.plmn);

    // 28-bit CID
    out.ci = cellIdentity.cell_id;
    if (out.ci < 0 || out.ci > 0xFFFFFFF)
        out.ci = INT32_MAX;

    // Physical CID
    out.pci = cellIdentity.phy_cell_id;
    if (out.pci < 0 || out.pci > 503)
        out.pci = INT32_MAX;

    // 16-bit TAC
    out.tac = cellIdentity.tac;
    if (out.tac < 0 || out.tac > 0xFFFF)
        out.tac = INT32_MAX;

    // 18-bit earfcn
    out.earfcn = cellIdentity.earfcn;
    if (out.earfcn < 0 || out.earfcn > 0x3FFFF)
        out.earfcn = INT32_MAX;

    out.bandwidth = cellIdentity.bandwidth;
    if (out.bandwidth < 0)
        out.bandwidth = INT32_MAX;

    // Operator name
    if (cellIdentity.plmn_info.plmn_short_name[0] != 0)
        strncpy(out.operatorNames.alphaShort, cellIdentity.plmn_info.plmn_short_name, MAX_SHORT_NAME_LEN-1);
    if (cellIdentity.plmn_info.plmn_long_name[0] != 0)
        strncpy(out.operatorNames.alphaLong, cellIdentity.plmn_info.plmn_long_name, MAX_FULL_NAME_LEN-1);

    /*
    // Additional PLMN
    out.leng_additionalPlmns = cellIdentity.plmn_info.additional_plmn_len;
    if (out.leng_additionalPlmns > 0) {
    char additionalPlmns[EACH_PLMN_LEN + 1];
    int leng_additionalPlmns = out.leng_additionalPlmns;

    for(int i = 0; i < leng_additionalPlmns; i++) {
    memset(additionalPlmns, 0, EACH_PLMN_LEN + 1);
    memcpy(additionalPlmns, cellIdentity.plmn_info.additional_plmn_info[(i*EACH_PLMN_LEN)], EACH_PLMN_LEN);
    std::string str(additionalPlmns);
    out.additionalPlmns.push_back(str);
    }
    }
     */

    // CSG Info;
    FillCellIdentityCsgInfo(out.optionalCsgInfo, cellIdentity.csg_info);

    // Band Length
    out.leng_bands = cellIdentity.band_len;
    if (out.leng_bands > MAX_BANDS) {
        out.leng_bands = MAX_BANDS;
    }

    // Band Info
    for (int i = 0; i < out.leng_bands; i++) {
        out.bands[i] = (RIL_EutranBands)cellIdentity.band_info[i];
    }
}

void FillCellIdentityLte(RIL_CellIdentityLte_V1_5& out, cell_identity_lte_v4& cellIdentity) {
    FillCellIdentityLte(out, *((cell_identity_lte_v3 *)&cellIdentity));
}

void FillCellIdentityWcdma(RIL_CellIdentityWcdma_V1_5& out, cell_identity_wcdma_v3& cellIdentity) {
    out.mcc = FetchMcc(cellIdentity.plmn);
    out.mnc = FetchMnc(cellIdentity.plmn);

    // 16-bit LAC
    out.lac = cellIdentity.lac;
    if (out.lac < 0 || out.lac > 0xFFFF)
        out.lac = INT32_MAX;

    // 28-bit UMTS CID
    out.cid = cellIdentity.cid;
    if (out.cid < 0 || out.cid > 0xFFFFFFF)
        out.cid = INT32_MAX;

    // 9-bit UMTS PSC
    out.psc = cellIdentity.psc;
    if (out.psc < 0 || out.psc > 0x1FF)
        out.psc = INT32_MAX;

    // 16-bit UMTS uarfcn
    out.uarfcn = cellIdentity.uarfcn;
    if (out.uarfcn < 0 || out.uarfcn > 0xFFFF)
        out.uarfcn = INT32_MAX;


    // Operator name
    if (cellIdentity.plmn_info.plmn_short_name[0] != 0)
        strncpy(out.operatorNames.alphaShort, cellIdentity.plmn_info.plmn_short_name, MAX_SHORT_NAME_LEN-1);
    if (cellIdentity.plmn_info.plmn_long_name[0] != 0)
        strncpy(out.operatorNames.alphaLong, cellIdentity.plmn_info.plmn_long_name, MAX_FULL_NAME_LEN-1);

    /*
    // Additional PLMN
    out.leng_additionalPlmns = cellIdentity.plmn_info.additional_plmn_len;
    if (out.leng_additionalPlmns > 0) {
    char additionalPlmns[EACH_PLMN_LEN + 1];
    int leng_additionalPlmns = out.leng_additionalPlmns;

    for(int i = 0; i < leng_additionalPlmns; i++) {
    memset(additionalPlmns, 0, EACH_PLMN_LEN + 1);
    memcpy(additionalPlmns, cellIdentity.plmn_info.additional_plmn_info[(i*EACH_PLMN_LEN)], EACH_PLMN_LEN);
    std::string str(additionalPlmns);
    out.additionalPlmns.push_back(str);
    }
    }
     */

    // CSG Info
    FillCellIdentityCsgInfo(out.optionalCsgInfo, cellIdentity.csg_info);
}

void FillCellIdentityWcdma(RIL_CellIdentityWcdma_V1_5& out, cell_identity_wcdma_v4& cellIdentity) {
    FillCellIdentityWcdma(out, *((cell_identity_wcdma_v3 *)&cellIdentity));
}

void FillCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_5& out, cell_identity_tdscdma_v3& cellIdentity) {
    out.mcc = FetchMcc(cellIdentity.plmn);
    out.mnc = FetchMnc(cellIdentity.plmn);

    // 16-bit LAC
    out.lac = cellIdentity.lac;
    if (out.lac < 0 || out.lac > 0xFFFF)
        out.lac = INT32_MAX;

    // 28-bit UMTS CID
    out.cid = cellIdentity.cid;
    if (out.cid < 0 || out.cid > 0xFFFFFFF)
        out.cid = INT32_MAX;

    // 8-bit CPID
    out.cpid = cellIdentity.cpid;
    if (out.cpid < 0 || out.cpid > 0xFF)
        out.cpid = INT32_MAX;

    // 16-bit UMTS Absolute RF Channel Number
    out.uarfcn = cellIdentity.uarfcn;
    if (out.uarfcn < 0 || out.uarfcn > 0xFFFF)
        out.uarfcn = INT32_MAX;

    // Operator name
    if (cellIdentity.plmn_info.plmn_short_name[0] != 0)
        strncpy(out.operatorNames.alphaShort, cellIdentity.plmn_info.plmn_short_name, MAX_SHORT_NAME_LEN-1);
    if (cellIdentity.plmn_info.plmn_long_name[0] != 0)
        strncpy(out.operatorNames.alphaLong, cellIdentity.plmn_info.plmn_long_name, MAX_FULL_NAME_LEN-1);

    /*
    // Additional PLMN
    out.leng_additionalPlmns = cellIdentity.plmn_info.additional_plmn_len;
    if (out.leng_additionalPlmns > 0) {
    char additionalPlmns[EACH_PLMN_LEN + 1];
    int leng_additionalPlmns = out.leng_additionalPlmns;

    for(int i = 0; i < leng_additionalPlmns; i++) {
    memset(additionalPlmns, 0, EACH_PLMN_LEN + 1);
    memcpy(additionalPlmns, cellIdentity.plmn_info.additional_plmn_info[(i*EACH_PLMN_LEN)], EACH_PLMN_LEN);
    std::string str(additionalPlmns);
    out.additionalPlmns.push_back(str);
    }
    }
     */

    // CSG Info
    FillCellIdentityCsgInfo(out.optionalCsgInfo, cellIdentity.csg_info);
}

void FillCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_5& out, cell_identity_tdscdma_v4& cellIdentity) {
    FillCellIdentityTdscdma(out, *((cell_identity_tdscdma_v3 *)&cellIdentity));
}

void FillCellIdentityNr(RIL_CellIdentityNr_V1_5& out, cell_identity_nr_v3& cellIdentity) {
    out.mcc = FetchMcc(cellIdentity.plmn);
    out.mnc = FetchMnc(cellIdentity.plmn);

    // Cell Identity (36 bits)
    out.nci = cellIdentity.cell_id;
    if (out.nci > 68719476735)
        out.nci = LONG_MAX;

    // Physical cell id (0-1007)
    out.pci = cellIdentity.phy_cell_id;
    if (out.pci > 1007)
        out.pci = INT32_MAX;

    // 16-bit tracking area code
    out.tac = cellIdentity.tac;
    if (out.tac < 0 || out.tac > 16777215)
        out.tac = INT32_MAX;

    // NR Absolute Radio Frequency Channel Number (0-3279165)
    out.nrarfcn = cellIdentity.arfcn;
    if (out.nrarfcn < 0 || out.nrarfcn > 3279165)
        out.nrarfcn = INT32_MAX;

    // Operator name
    if (cellIdentity.plmn_info.plmn_short_name[0] != 0)
        strncpy(out.operatorNames.alphaShort, cellIdentity.plmn_info.plmn_short_name, MAX_SHORT_NAME_LEN-1);
    if (cellIdentity.plmn_info.plmn_long_name[0] != 0)
        strncpy(out.operatorNames.alphaLong, cellIdentity.plmn_info.plmn_long_name, MAX_FULL_NAME_LEN-1);

    /*
    // Additional PLMN
    out.leng_additionalPlmns = cellIdentity.plmn_info.additional_plmn_len;
    if (out.leng_additionalPlmns > 0) {
    char additionalPlmns[EACH_PLMN_LEN + 1];
    int leng_additionalPlmns = out.leng_additionalPlmns;

    for(int i = 0; i < leng_additionalPlmns; i++) {
    memset(additionalPlmns, 0, EACH_PLMN_LEN + 1);
    memcpy(additionalPlmns, cellIdentity.plmn_info.additional_plmn_info[(i*EACH_PLMN_LEN)], EACH_PLMN_LEN);
    std::string str(additionalPlmns);
    out.additionalPlmns.push_back(str);
    }
    }
     */

    // Band Length
    out.leng_bands = cellIdentity.band_len;
    if (out.leng_bands > MAX_BANDS) {
        out.leng_bands = MAX_BANDS;
    }

    // Band Info
    for (int i = 0; i < out.leng_bands; i++) {
        out.bands[i] = (RIL_NgranBands)cellIdentity.band_info[i];
    }
}

void FillCellIdentityNr(RIL_CellIdentityNr_V1_5& out, cell_identity_nr_v4& cellIdentity) {
    FillCellIdentityNr(out, *((cell_identity_nr_v3 *)&cellIdentity));
}

// CellIdentity CSG
void FillCellIdentityCsgInfo(RIL_OptionalCsgInfo& out, const csg_info_v3& csgInfo) {
    if (!csgInfo.csg_indication && !(csgInfo.csg_identity > 0 && csgInfo.csg_identity <= 0x7FFFFFF)) {
        // noninit
        out = { { false, {}, INT_MAX} };
    } else {
        out.csgInfo.csgIndication = csgInfo.csg_indication;
        if (!TextUtils::IsEmpty(csgInfo.hnb_name)) {
            strlcpy(out.csgInfo.homeNodebName, csgInfo.hnb_name, MAX_CSG_HOME_NODEB_NAME - 1);
        }

        // Modem must guarantee a valid range of cid.
        // IntRage(from = 1, to = 0x7FFFFFF)
        out.csgInfo.csgIdentity = csgInfo.csg_identity;
    }
}

void FillCellIdentityCsgInfo(RIL_OptionalCsgInfo& out, const csg_info_v4& csgInfo) {
    // csg_info_v3 and csg_info_v4 are same
    FillCellIdentityCsgInfo(out, *((csg_info_v3 *)&csgInfo));
}

}  // CellInfoProtocolUtils::Legacy

namespace JSON {
bool isValidPlmn(const char* plmn) {
    if (plmn == NULL) return false;
    int plmnLen = strlen(plmn);
    if (plmnLen < MIN_PLMN_LEN || plmnLen > MAX_PLMN_LEN) return false;

    return TextUtils::IsDigitsOnly(plmn);
}

int FetchMcc(const char* plmn) {
    if (!isValidPlmn(plmn)) return INT_MAX;

    char mccStr[MAX_MCC_LEN + 1];
    memset(mccStr, 0, sizeof(mccStr));
    memcpy(mccStr, plmn, MAX_MCC_LEN);
    int mcc = strtol(mccStr, NULL, 10);
    return (mcc == 0) ? INT_MAX : mcc;
}

int FetchMnc(const char* plmn) {
    if (!isValidPlmn(plmn) || FetchMcc(plmn) == INT_MAX) return INT_MAX;

    int mncLen = strlen(plmn) - MAX_MCC_LEN;
    char mncStr[MAX_MNC_LEN + 1];
    memset(mncStr, 0, sizeof(mncStr));
    memcpy(mncStr, plmn + MAX_MCC_LEN, mncLen);
    int mnc = strtol(mncStr, NULL, 10);
    mnc = ril::util::mnc::encode(mnc, mncLen);
    return mnc;
}
}   // CellInfoProtocolUtils::JSON
}  // CellInfoProtocolUtils
