/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include <telephony/librilutils.h>

#define LOG_TAG "RadioConvUtils"
#include <utils/Log.h>

static bool debug = true;

namespace RadioConvUtils {

#define DATA_PROTOCOL_IP        "IP"
#define DATA_PROTOCOL_IPV6      "IPV6"
#define DATA_PROTOCOL_IPV4V6    "IPV4V6"
#define DATA_PROTOCOL_PPP       "PPP"

int getAdjustedRaf(int raf) {
    int newRaf = 0;
    newRaf = ((NETWORK_TYPE_BITMAP_GSM & raf) > 0) ? (NETWORK_TYPE_BITMAP_GSM | newRaf) : newRaf;
    newRaf = ((NETWORK_TYPE_BITMAP_WCDMA & raf) > 0) ? (NETWORK_TYPE_BITMAP_WCDMA | newRaf) : newRaf;
    newRaf = ((NETWORK_TYPE_BITMAP_CDMA & raf) > 0) ? (NETWORK_TYPE_BITMAP_CDMA | newRaf) : newRaf;
    newRaf = ((NETWORK_TYPE_BITMAP_EVDO & raf) > 0) ? (NETWORK_TYPE_BITMAP_EVDO | newRaf) : newRaf;
    newRaf = ((NETWORK_TYPE_BITMAP_LTE & raf) > 0) ? (NETWORK_TYPE_BITMAP_LTE | newRaf) : newRaf;
    newRaf = ((NETWORK_TYPE_BITMAP_NR & raf) > 0) ? (NETWORK_TYPE_BITMAP_NR | newRaf) : newRaf;
    newRaf = ((NETWORK_TYPE_BITMAP_TDS_CDMA & raf) > 0) ? (NETWORK_TYPE_BITMAP_TDS_CDMA | newRaf) : newRaf;

    return newRaf;
}

int getNetworkTypeFromRaf(int raf)
{
    int oldRaf = raf;
    raf = getAdjustedRaf(raf);
    if (debug) {
        if (oldRaf != raf) {
            RLOGD("adjust RAF from 0x%08X to 0x%08X.", oldRaf, raf);
        }
    }

    switch (raf) {
    case NETWORK_TYPE_BITMAP_GSM_WCDMA:
        return PREF_NET_TYPE_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_GSM_ONLY:
        return PREF_NET_TYPE_GSM_ONLY;
    case NETWORK_TYPE_BITMAP_WCDMA_ONLY:
        return PREF_NET_TYPE_WCDMA;
    case NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO:
        return PREF_NET_TYPE_CDMA_EVDO_AUTO;
    case NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO:
        return PREF_NET_TYPE_LTE_CDMA_EVDO;
    case NETWORK_TYPE_BITMAP_CDMA_ONLY:
        return PREF_NET_TYPE_CDMA_ONLY;
    case NETWORK_TYPE_BITMAP_EVDO_ONLY:
        return PREF_NET_TYPE_EVDO_ONLY;
    case NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO:
        return PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO;
    case NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA:
        return PREF_NET_TYPE_LTE_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA:
        return PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_LTE_ONLY:
        return PREF_NET_TYPE_LTE_ONLY;
    case NETWORK_TYPE_BITMAP_LTE_WCDMA:
        return PREF_NET_TYPE_LTE_WCDMA;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_ONLY:
        return PREF_NET_TYPE_TD_SCDMA_ONLY;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA:
        return PREF_NET_TYPE_TD_SCDMA_WCDMA;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_LTE:
        return PREF_NET_TYPE_TD_SCDMA_LTE;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_GSM:
        return PREF_NET_TYPE_TD_SCDMA_GSM;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE:
        return PREF_NET_TYPE_TD_SCDMA_GSM_LTE;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA:
        return PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE:
        return PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE:
        return PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO:
        return PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO;
    case NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
        return PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
    // support NR
    case NETWORK_TYPE_BITMAP_NR_ONLY:
        return PREF_NET_TYPE_NR_ONLY;
    case NETWORK_TYPE_BITMAP_NR_LTE:
        return PREF_NET_TYPE_NR_LTE;
    case NETWORK_TYPE_BITMAP_NR_LTE_CDMA_EVDO_AUTO:
        return PREF_NET_TYPE_NR_LTE_CDMA_EVDO;
    case NETWORK_TYPE_BITMAP_NR_LTE_CDMA_GSM_WCDMA:
        return PREF_NET_TYPE_NR_LTE_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_NR_LTE_CMDA_EVDO_GSM_WCDMA:
        return PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_NR_LTE_WCDMA:
        return PREF_NET_TYPE_NR_LTE_WCDMA;
    case NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE:
        return PREF_NET_TYPE_NR_LTE_TDSCDMA;
    case NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_GSM:
        return PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM;
    case NETWORK_TYPE_BITMAP_NR_TD_SCDMA_WCDMA_LTE:
        return PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA;
    case NETWORK_TYPE_BITMAP_NR_TD_SCDMA_GSM_WCDMA_LTE:
        return PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA;
    case NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
        return PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA;
    default:
        break;
    }

    if (debug) {
        printSupportedNetworkTypeBitmap();
        return PREF_NET_TYPE_LTE_GSM_WCDMA;
    }

    return -1;
}

int getRafFromNetworkType(int networkType)
{
    switch (networkType) {
    case PREF_NET_TYPE_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_GSM_WCDMA;
    case PREF_NET_TYPE_GSM_ONLY:
        return NETWORK_TYPE_BITMAP_GSM_ONLY;
    case PREF_NET_TYPE_WCDMA:
        return NETWORK_TYPE_BITMAP_WCDMA_ONLY;
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        return NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO;
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
        return NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO;
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA;
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA;
    case PREF_NET_TYPE_LTE_ONLY:
        return NETWORK_TYPE_BITMAP_LTE_ONLY;
    case PREF_NET_TYPE_LTE_WCDMA:
        return NETWORK_TYPE_BITMAP_LTE_WCDMA;
    case PREF_NET_TYPE_CDMA_ONLY:
        return NETWORK_TYPE_BITMAP_CDMA_ONLY;
    case PREF_NET_TYPE_EVDO_ONLY:
        return NETWORK_TYPE_BITMAP_EVDO_ONLY;
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
        return NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO;
    case PREF_NET_TYPE_TD_SCDMA_ONLY:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_ONLY;
    case PREF_NET_TYPE_TD_SCDMA_WCDMA:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA;
    case PREF_NET_TYPE_TD_SCDMA_LTE:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_LTE;
    case PREF_NET_TYPE_TD_SCDMA_GSM:
        return (NETWORK_TYPE_BITMAP_GSM | RAF_TD_SCDMA);
    case PREF_NET_TYPE_TD_SCDMA_GSM_LTE:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE;
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA;
    case PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE;
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE;
    case PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO;
    case PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
    // support NR
    case PREF_NET_TYPE_NR_ONLY:
        return NETWORK_TYPE_BITMAP_NR_ONLY;
    case PREF_NET_TYPE_NR_LTE:
        return NETWORK_TYPE_BITMAP_NR_LTE;
    case PREF_NET_TYPE_NR_LTE_CDMA_EVDO:
        return NETWORK_TYPE_BITMAP_NR_LTE_CDMA_EVDO_AUTO;
    case PREF_NET_TYPE_NR_LTE_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_NR_LTE_CDMA_GSM_WCDMA;
    case PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_NR_LTE_CMDA_EVDO_GSM_WCDMA;
    case PREF_NET_TYPE_NR_LTE_WCDMA:
        return NETWORK_TYPE_BITMAP_NR_LTE_WCDMA;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA:
        return NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM:
        return NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_GSM;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA:
        return NETWORK_TYPE_BITMAP_NR_TD_SCDMA_WCDMA_LTE;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_NR_TD_SCDMA_GSM_WCDMA_LTE;
    case PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
        return NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
    default:
        return -1;
    }
}

void printSupportedNetworkTypeBitmap()
{
    struct NetworTypeBitmap {
        const char *name;
        int bitmap;
    };

    struct NetworTypeBitmap supportedList[] = {
        {"NETWORK_TYPE_BITMAP_GSM_WCDMA", NETWORK_TYPE_BITMAP_GSM_WCDMA},
        {"NETWORK_TYPE_BITMAP_GSM_ONLY", NETWORK_TYPE_BITMAP_GSM_ONLY},
        {"NETWORK_TYPE_BITMAP_WCDMA", NETWORK_TYPE_BITMAP_WCDMA},
        {"NETWORK_TYPE_BITMAP_GSM_WCDMA_AUTO", NETWORK_TYPE_BITMAP_GSM_WCDMA_AUTO},
        {"NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO", NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO},
        {"NETWORK_TYPE_BITMAP_CDMA_ONLY", NETWORK_TYPE_BITMAP_CDMA_ONLY},
        {"NETWORK_TYPE_BITMAP_EVDO_ONLY", NETWORK_TYPE_BITMAP_EVDO_ONLY},
        {"NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO", NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO},
        {"NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO", NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO},
        {"NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA", NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA},
        {"NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA", NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA},
        {"NETWORK_TYPE_BITMAP_LTE_ONLY", NETWORK_TYPE_BITMAP_LTE_ONLY},
        {"NETWORK_TYPE_BITMAP_LTE_WCDMA", NETWORK_TYPE_BITMAP_LTE_WCDMA},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_ONLY", NETWORK_TYPE_BITMAP_TD_SCDMA_ONLY},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA", NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_LTE", NETWORK_TYPE_BITMAP_TD_SCDMA_LTE},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_GSM", NETWORK_TYPE_BITMAP_TD_SCDMA_GSM},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE", NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA", NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE", NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE", NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO", NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO},
        {"NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA", NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA},
    };

    size_t size = sizeof(supportedList) / sizeof(supportedList[0]);
    RLOGD("###########printSupportedNetworkTypeBitmap###########");
    for (size_t i = 0; i < size; i++) {
        RLOGD("[%zu] %s(0x%08X)", i, supportedList[i].name, supportedList[i].bitmap);
    } // end for i ~
    RLOGD("######################################################");
}

int getCellInfoTypeRadioTechnology(int rat) {
    switch(rat) {
        case RADIO_TECH_GPRS:
        case RADIO_TECH_EDGE:
        case RADIO_TECH_GSM: {
            return RIL_CELL_INFO_TYPE_GSM;
        }
        case RADIO_TECH_UMTS:
        case RADIO_TECH_HSDPA:
        case RADIO_TECH_HSUPA:
        case RADIO_TECH_HSPA:
        case RADIO_TECH_HSPAP: {
            return RIL_CELL_INFO_TYPE_WCDMA;
        }
        case RADIO_TECH_IS95A:
        case RADIO_TECH_IS95B:
        case RADIO_TECH_1xRTT:
        case RADIO_TECH_EVDO_0:
        case RADIO_TECH_EVDO_A:
        case RADIO_TECH_EVDO_B:
        case RADIO_TECH_EHRPD: {
            return RIL_CELL_INFO_TYPE_CDMA;
        }
        case RADIO_TECH_LTE:
        case RADIO_TECH_LTE_CA: {
            return RIL_CELL_INFO_TYPE_LTE;
        }
        case RADIO_TECH_TD_SCDMA: {
            return RIL_CELL_INFO_TYPE_TD_SCDMA;
        }
        case RADIO_TECH_NR:
            return RIL_CELL_INFO_TYPE_NR;
        default: {
            break;
        }
    }
    return RIL_CELL_INFO_TYPE_NONE;
}

int getRadioTechnologyToAccessNetworkType(int rat)
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

bool isServiceTypeCfQuery(RIL_SsServiceType serType, RIL_SsRequestType reqType) {
    if ((reqType == SS_INTERROGATION) &&
        (serType == SS_CFU ||
         serType == SS_CF_BUSY ||
         serType == SS_CF_NO_REPLY ||
         serType == SS_CF_NOT_REACHABLE ||
         serType == SS_CF_ALL ||
         serType == SS_CF_ALL_CONDITIONAL)) {
        return true;
    }
    return false;
}

void fillOperatorName(RIL_CellIdentityOperatorNames& to, const RIL_CellIdentityOperatorNames& from) {
    memset(to.alphaLong, 0, MAX_ALPHA_OPERATOR_NAME_LEN);
    memset(to.alphaShort, 0, MAX_ALPHA_OPERATOR_NAME_LEN);

    if (*from.alphaLong != 0) {
        strncpy(to.alphaLong, from.alphaLong, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
    }

    if (*from.alphaShort != 0) {
        strncpy(to.alphaShort, from.alphaShort, MAX_ALPHA_OPERATOR_NAME_LEN - 1);
    }
}

void convertCellIdentityGsm(RIL_CellIdentityGsm_V1_2 &out,
        const RIL_CellIdentityGsm_V1_5 &cellId) {
    out.arfcn = cellId.arfcn;
    out.bsic = cellId.bsic;
    out.cid = cellId.cid;
    out.lac = cellId.lac;
    out.mcc = cellId.mcc;
    out.mnc = cellId.mnc;
    out.operatorNames = cellId.operatorNames;
}

void convertCellIdentityGsm(RIL_CellIdentityGsm_v12& out,
        const RIL_CellIdentityGsm_V1_5& cellIdentityGsm)
{
    out.lac = cellIdentityGsm.lac;
    out.cid = cellIdentityGsm.cid;
    out.arfcn = cellIdentityGsm.arfcn;
    out.bsic = cellIdentityGsm.bsic;
    out.mcc = cellIdentityGsm.mcc;
    out.mnc = cellIdentityGsm.mnc;
}

void convertCellIdentityGsm(RIL_CellIdentityGsm_v12& out,
        const RIL_CellIdentityGsm_V1_2& cellIdentityGsm)
{
    out.lac = cellIdentityGsm.lac;
    out.cid = cellIdentityGsm.cid;
    out.arfcn = cellIdentityGsm.arfcn;
    out.bsic = cellIdentityGsm.bsic;
    out.mcc = cellIdentityGsm.mcc;
    out.mnc = cellIdentityGsm.mnc;
}
void convertCellIdentityCdma(RIL_CellIdentityCdma& out,
        const RIL_CellIdentityCdma_V1_2& cellIdentityCdma)
{
    out.basestationId = cellIdentityCdma.basestationId;
    out.latitude = cellIdentityCdma.latitude;
    out.longitude = cellIdentityCdma.longitude;
    out.systemId = cellIdentityCdma.systemId;
    out.networkId = cellIdentityCdma.networkId;
}

void convertCellIdentityWcdma(RIL_CellIdentityWcdma_V1_2 &out,
        const RIL_CellIdentityWcdma_V1_5 &cellId) {
    out.uarfcn = cellId.uarfcn;
    out.psc = cellId.psc;
    out.cid = cellId.cid;
    out.lac = cellId.lac;
    out.mcc = cellId.mcc;
    out.mnc = cellId.mnc;
    out.operatorNames = cellId.operatorNames;
}

void convertCellIdentityWcdma(RIL_CellIdentityWcdma_v12& out,
        const RIL_CellIdentityWcdma_V1_5& cellIdentityWcdma)
{
    out.lac = cellIdentityWcdma.lac;
    out.cid = cellIdentityWcdma.cid;
    out.psc = cellIdentityWcdma.psc;
    out.uarfcn = cellIdentityWcdma.uarfcn;
    out.mcc = cellIdentityWcdma.mcc;
    out.mnc = cellIdentityWcdma.mnc;
}

void convertCellIdentityWcdma(RIL_CellIdentityWcdma_v12& out,
        const RIL_CellIdentityWcdma_V1_2& cellIdentityWcdma)
{
    out.lac = cellIdentityWcdma.lac;
    out.cid = cellIdentityWcdma.cid;
    out.psc = cellIdentityWcdma.psc;
    out.uarfcn = cellIdentityWcdma.uarfcn;
    out.mcc = cellIdentityWcdma.mcc;
    out.mnc = cellIdentityWcdma.mnc;
}

void convertCellIdentityLte(RIL_CellIdentityLte_V1_2 &out,
        const RIL_CellIdentityLte_V1_5 &cellId) {
    out.earfcn = cellId.earfcn;
    out.pci = cellId.pci;
    out.ci = cellId.ci;
    out.tac = cellId.tac;
    out.mcc = cellId.mcc;
    out.mnc = cellId.mnc;
    out.operatorNames = cellId.operatorNames;
    out.bandwidth = cellId.bandwidth;
}

void convertCellIdentityLte(RIL_CellIdentityLte_v12& out,
        const RIL_CellIdentityLte_V1_5& cellIdentityLte)
{
    out.tac = cellIdentityLte.tac;
    out.ci = cellIdentityLte.ci;
    out.pci = cellIdentityLte.pci;
    out.earfcn = cellIdentityLte.earfcn;
    out.mcc = cellIdentityLte.mcc;
    out.mnc = cellIdentityLte.mnc;
}

void convertCellIdentityLte(RIL_CellIdentityLte_v12& out,
        const RIL_CellIdentityLte_V1_2& cellIdentityLte)
{
    out.tac = cellIdentityLte.tac;
    out.ci = cellIdentityLte.ci;
    out.pci = cellIdentityLte.pci;
    out.earfcn = cellIdentityLte.earfcn;
    out.mcc = cellIdentityLte.mcc;
    out.mnc = cellIdentityLte.mnc;
}

void convertCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_2 &out,
        const RIL_CellIdentityTdscdma_V1_5 &cellId) {
    out.uarfcn = cellId.uarfcn;
    out.cpid = cellId.cpid;
    out.cid = cellId.cid;
    out.lac = cellId.lac;
    out.mcc = cellId.mcc;
    out.mnc = cellId.mnc;
    out.operatorNames = cellId.operatorNames;
}

void convertCellIdentityTdscdma(RIL_CellIdentityTdscdma& out,
        const RIL_CellIdentityTdscdma_V1_5& cellIdentityTdscdma)
{
    out.lac = cellIdentityTdscdma.lac;
    out.cid = cellIdentityTdscdma.cid;
    out.cpid = cellIdentityTdscdma.cpid;
    out.mcc = cellIdentityTdscdma.mcc;
    out.mnc = cellIdentityTdscdma.mnc;
}

void convertCellIdentityTdscdma(RIL_CellIdentityTdscdma& out,
        const RIL_CellIdentityTdscdma_V1_2& cellIdentityTdscdma)
{
    out.lac = cellIdentityTdscdma.lac;
    out.cid = cellIdentityTdscdma.cid;
    out.cpid = cellIdentityTdscdma.cpid;
    out.mcc = cellIdentityTdscdma.mcc;
    out.mnc = cellIdentityTdscdma.mnc;
}

void convertCellIdentityNr(RIL_CellIdentityNr_V1_4 &out,
        const RIL_CellIdentityNr_V1_5 &cellId) {
    out.nrarfcn = cellId.nrarfcn;
    out.pci = cellId.pci;
    out.nci = cellId.nci;
    out.tac = cellId.tac;
    out.mcc = cellId.mcc;
    out.mnc = cellId.mnc;
    out.operatorNames = cellId.operatorNames;
}

void convertCellIdentity(RIL_CellIdentity_V1_2& out, const RIL_CellIdentity_V1_5& cellIdentity)
{
    out.cellInfoType = cellIdentity.cellInfoType;
    switch (cellIdentity.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        convertCellIdentityGsm(out.cellIdentityGsm, cellIdentity.gsm);
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        convertCellIdentityWcdma(out.cellIdentityWcdma, cellIdentity.wcdma);
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        convertCellIdentityLte(out.cellIdentityLte, cellIdentity.lte);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        convertCellIdentityTdscdma(out.cellIdentityTdscdma, cellIdentity.tdscdma);
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        out.cellIdentityCdma = cellIdentity.cdma;
        break;
    default:
        break;
    } // end switch ~
}

void convertCellIdentity(RIL_CellIdentity_v16& out, const RIL_CellIdentity_V1_5& cellIdentity)
{
    out.cellInfoType = cellIdentity.cellInfoType;
    switch (cellIdentity.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        convertCellIdentityGsm(out.cellIdentityGsm, cellIdentity.gsm);
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        convertCellIdentityWcdma(out.cellIdentityWcdma, cellIdentity.wcdma);
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        convertCellIdentityLte(out.cellIdentityLte, cellIdentity.lte);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        convertCellIdentityTdscdma(out.cellIdentityTdscdma, cellIdentity.tdscdma);
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        convertCellIdentityCdma(out.cellIdentityCdma, cellIdentity.cdma);
        break;
    default:
        break;
    } // end switch ~
}

void convertCellIdentity(RIL_CellIdentity_v16& out, const RIL_CellIdentity_V1_2& cellIdentity)
{
    out.cellInfoType = cellIdentity.cellInfoType;
    switch (cellIdentity.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            convertCellIdentityGsm(out.cellIdentityGsm, cellIdentity.cellIdentityGsm);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            convertCellIdentityWcdma(out.cellIdentityWcdma, cellIdentity.cellIdentityWcdma);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            convertCellIdentityLte(out.cellIdentityLte, cellIdentity.cellIdentityLte);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            convertCellIdentityTdscdma(out.cellIdentityTdscdma, cellIdentity.cellIdentityTdscdma);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            convertCellIdentityCdma(out.cellIdentityCdma, cellIdentity.cellIdentityCdma);
            break;
        default:
            break;
    } // end switch ~
}

void convertDataRegistrationStateResult(
        RIL_RegStateResult& out, RIL_RegStateResult_V1_6& dataRegStateResult)
{
    out.regState = dataRegStateResult.regState;
    out.rat = dataRegStateResult.rat;
    out.reasonForDenial = dataRegStateResult.reasonForDenial;
    out.cellIdentity = dataRegStateResult.cellIdentity;

    memset(out.registeredPlmn, 0, MAX_REG_PLMN_LEN);
    if (*dataRegStateResult.registeredPlmn != 0) {
        strncpy(out.registeredPlmn, dataRegStateResult.registeredPlmn, MAX_REG_PLMN_LEN - 1);
    }

    int ran = getRadioTechnologyToAccessNetworkType(dataRegStateResult.rat);
    if (ran == ACCESS_NETWORK_NGRAN) {
        // FIXME: W/A to support NR vops/emc for R
        out.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported
            = dataRegStateResult.accessTechnologySpecificInfo.ngranInfo.vopsSupported > 0;
        out.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported
            = dataRegStateResult.accessTechnologySpecificInfo.ngranInfo.emcSupported > 0;
    }
    else if (ran == ACCESS_NETWORK_EUTRAN) {
        out.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported;
        out.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported;
        out.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable;
        out.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted;
        out.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable;
    }
    else if (ran == ACCESS_NETWORK_CDMA2000) {
        out.accessTechnologySpecificInfo.cdmaInfo.cssSupported
            = dataRegStateResult.accessTechnologySpecificInfo.cdmaInfo.cssSupported;
        out.accessTechnologySpecificInfo.cdmaInfo.roamingIndicator
            = dataRegStateResult.accessTechnologySpecificInfo.cdmaInfo.roamingIndicator;
        out.accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl
            = dataRegStateResult.accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl;
        out.accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator
            = dataRegStateResult.accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator;
    }
}

void convertDataRegistrationStateResult(
        RIL_DataRegistrationStateResponse_V1_4& out, RIL_RegStateResult& dataRegStateResult)
{
    out.regState = dataRegStateResult.regState;
    out.rat = dataRegStateResult.rat;
    out.reasonDataDenied = (int)dataRegStateResult.reasonForDenial;
    // I think this value doens't be used. Also,
    // RIL_RegStateResult doesn't have a maxDataCalls value.
    out.maxDataCalls = 0;
    convertCellIdentity(out.cellIdentity, dataRegStateResult.cellIdentity);
    int ran = getRadioTechnologyToAccessNetworkType(dataRegStateResult.rat);
    if (ACCESS_NETWORK_EUTRAN == ran || ACCESS_NETWORK_NGRAN == ran ) {
        out.lteVopsInfo.isVopsSupported
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported;
        out.lteVopsInfo.isEmcBearerSupported
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported;
        out.nrIndicators.isEndcAvailable
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable;
        out.nrIndicators.isDcNrRestricted
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted;
        out.nrIndicators.isNrAvailable
            = dataRegStateResult.accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable;
    }
}

void convertDataRegistrationStateResult(
        RIL_DataRegistrationStateResponse_V1_2& out,
        RIL_DataRegistrationStateResponse_V1_4& dataRegStateResult)
{
    out.regState = dataRegStateResult.regState;
    out.rat = dataRegStateResult.rat;
    out.reasonDataDenied = dataRegStateResult.reasonDataDenied;
    // I think this value doens't be used. Also,
    // RIL_RegStateResult doesn't have a maxDataCalls value.
    out.maxDataCalls = 0;
    out.cellIdentity = dataRegStateResult.cellIdentity;
}

void convertDataRegistrationStateResult(
        RIL_DataRegistrationStateResponse& out,
        RIL_DataRegistrationStateResponse_V1_2& dataRegStateResult)
{
    out.regState = dataRegStateResult.regState;
    out.rat = dataRegStateResult.rat;
    out.reasonDataDenied = dataRegStateResult.reasonDataDenied;
    // I think this value doens't be used. Also,
    // RIL_RegStateResult doesn't have a maxDataCalls value.
    out.maxDataCalls = 0;
    convertCellIdentity(out.cellIdentity, dataRegStateResult.cellIdentity);
}

void convertVoiceRegistrationStateResult(
        RIL_RegStateResult& out, RIL_RegStateResult_V1_6& voiceRegStateResult)
{
    out.regState = voiceRegStateResult.regState;
    out.rat = voiceRegStateResult.rat;
    out.reasonForDenial = voiceRegStateResult.reasonForDenial;
    out.cellIdentity = voiceRegStateResult.cellIdentity;

    int ran = getRadioTechnologyToAccessNetworkType(voiceRegStateResult.rat);
    if (ran == ACCESS_NETWORK_CDMA2000) {
        out.accessTechnologySpecificInfo.cdmaInfo.cssSupported
            = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.cssSupported;
        out.accessTechnologySpecificInfo.cdmaInfo.roamingIndicator
            = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.roamingIndicator;
        out.accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl
            = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl;
        out.accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator
            = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator;
    }
}

void convertVoiceRegistrationStateResult(
        RIL_VoiceRegistrationStateResponse_V1_2& out, RIL_RegStateResult& voiceRegStateResult)
{
    out.regState = voiceRegStateResult.regState;
    out.rat = voiceRegStateResult.rat;
    out.reasonForDenial = (int)voiceRegStateResult.reasonForDenial;

    switch(getRadioTechnologyToAccessNetworkType(voiceRegStateResult.rat)) {
        case ACCESS_NETWORK_CDMA2000:
            out.cssSupported = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.cssSupported;
            out.roamingIndicator = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.roamingIndicator;
            out.systemIsInPrl = (int)voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl;
            out.defaultRoamingIndicator = voiceRegStateResult.accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator;
            break;
    }

    convertCellIdentity(out.cellIdentity, voiceRegStateResult.cellIdentity);
}

void convertVoiceRegistrationStateResult(
        RIL_VoiceRegistrationStateResponse& out,
        RIL_VoiceRegistrationStateResponse_V1_2& voiceRegStateResult)
{
    out.regState = voiceRegStateResult.regState;
    out.rat = voiceRegStateResult.rat;
    out.cssSupported = voiceRegStateResult.cssSupported;
    out.roamingIndicator = voiceRegStateResult.roamingIndicator;
    out.systemIsInPrl = voiceRegStateResult.systemIsInPrl;
    out.defaultRoamingIndicator = voiceRegStateResult.defaultRoamingIndicator;
    out.reasonForDenial = voiceRegStateResult.reasonForDenial;
}

void convertSignalStrengthResult(RIL_SignalStrength_v10 &out, const RIL_SignalStrength_V1_2 &rilSignalStrength) {
    out.GW_SignalStrength.signalStrength =
            rilSignalStrength.WCDMA_SignalStrength.signalStrength;
    if (out.GW_SignalStrength.signalStrength < 0
        || out.GW_SignalStrength.signalStrength > 31) {
        out.GW_SignalStrength.signalStrength =
                rilSignalStrength.GSM_SignalStrength.signalStrength;
    }
    out.GW_SignalStrength.bitErrorRate =
            rilSignalStrength.WCDMA_SignalStrength.bitErrorRate;
    if (out.GW_SignalStrength.bitErrorRate < 0
            || out.GW_SignalStrength.bitErrorRate > 7) {
        out.GW_SignalStrength.bitErrorRate =
                rilSignalStrength.GSM_SignalStrength.bitErrorRate;
    }

    out.CDMA_SignalStrength = rilSignalStrength.CDMA_SignalStrength;
    out.EVDO_SignalStrength = rilSignalStrength.EVDO_SignalStrength;
    out.LTE_SignalStrength = rilSignalStrength.LTE_SignalStrength;
    out.TD_SCDMA_SignalStrength = rilSignalStrength.TD_SCDMA_SignalStrength;
}

void convertSignalStrengthResult(RIL_SignalStrength_v10& out,
        const RIL_SignalStrength_V1_4& currentSignalStrength)
{
    out.GW_SignalStrength.signalStrength =
            currentSignalStrength.WCDMA_SignalStrength.signalStrength;
    if (out.GW_SignalStrength.signalStrength < 0
        || out.GW_SignalStrength.signalStrength > 31) {
        out.GW_SignalStrength.signalStrength =
                currentSignalStrength.GSM_SignalStrength.signalStrength;
    }
    out.GW_SignalStrength.bitErrorRate =
            currentSignalStrength.WCDMA_SignalStrength.bitErrorRate;
    if (out.GW_SignalStrength.bitErrorRate < 0
            || out.GW_SignalStrength.bitErrorRate > 7) {
        out.GW_SignalStrength.bitErrorRate =
                currentSignalStrength.GSM_SignalStrength.bitErrorRate;
    }

    out.CDMA_SignalStrength = currentSignalStrength.CDMA_SignalStrength;
    out.EVDO_SignalStrength = currentSignalStrength.EVDO_SignalStrength;
    out.LTE_SignalStrength = currentSignalStrength.LTE_SignalStrength;
    out.TD_SCDMA_SignalStrength.rscp =
            currentSignalStrength.TD_SCDMA_SignalStrength.rscp;
}

void convertSignalStrengthResult(RIL_SignalStrength_V1_2& out,
        const RIL_SignalStrength_V1_4& currentSignalStrength)
{
    out.GSM_SignalStrength = currentSignalStrength.GSM_SignalStrength;
    out.CDMA_SignalStrength = currentSignalStrength.CDMA_SignalStrength;
    out.EVDO_SignalStrength = currentSignalStrength.EVDO_SignalStrength;
    out.LTE_SignalStrength = currentSignalStrength.LTE_SignalStrength;
    out.TD_SCDMA_SignalStrength.rscp =
            currentSignalStrength.TD_SCDMA_SignalStrength.rscp;
    out.WCDMA_SignalStrength = currentSignalStrength.WCDMA_SignalStrength;
}

void convertSignalStrengthResult(RIL_SignalStrength_V1_4& out,
        const RIL_SignalStrength_V1_6& currentSignalStrength)
{
    out.GSM_SignalStrength = currentSignalStrength.GSM_SignalStrength;
    out.CDMA_SignalStrength = currentSignalStrength.CDMA_SignalStrength;
    out.EVDO_SignalStrength = currentSignalStrength.EVDO_SignalStrength;
    out.TD_SCDMA_SignalStrength =
            currentSignalStrength.TD_SCDMA_SignalStrength;
    out.WCDMA_SignalStrength = currentSignalStrength.WCDMA_SignalStrength;

    out.LTE_SignalStrength.signalStrength = currentSignalStrength.LTE_SignalStrength.signalStrength;
    out.LTE_SignalStrength.rsrp = currentSignalStrength.LTE_SignalStrength.rsrp;
    out.LTE_SignalStrength.rsrq = currentSignalStrength.LTE_SignalStrength.rsrq;
    out.LTE_SignalStrength.rssnr = currentSignalStrength.LTE_SignalStrength.rssnr;
    out.LTE_SignalStrength.cqi = currentSignalStrength.LTE_SignalStrength.cqi;
    out.LTE_SignalStrength.timingAdvance = currentSignalStrength.LTE_SignalStrength.timingAdvance;

    out.NR_SignalStrength.ssRsrp = currentSignalStrength.NR_SignalStrength.ssRsrp;
    out.NR_SignalStrength.ssRsrq = currentSignalStrength.NR_SignalStrength.ssRsrq;
    out.NR_SignalStrength.ssSinr = currentSignalStrength.NR_SignalStrength.ssSinr;
    out.NR_SignalStrength.csiRsrp = currentSignalStrength.NR_SignalStrength.csiRsrp;
    out.NR_SignalStrength.csiRsrq = currentSignalStrength.NR_SignalStrength.csiRsrq;
    out.NR_SignalStrength.csiSinr = currentSignalStrength.NR_SignalStrength.csiSinr;
}

// Cell Info
void convertCellInfoGsm(RIL_CellInfoGsm_v12& out, const RIL_CellInfoGsm_V1_2& gsm)
{
    convertCellIdentityGsm(out.cellIdentityGsm, gsm.cellIdentityGsm);
    out.signalStrengthGsm = gsm.signalStrengthGsm;
}

void convertCellInfoCdma(RIL_CellInfoCdma& out, const RIL_CellInfoCdma_V1_2& cdma)
{
    convertCellIdentityCdma(out.cellIdentityCdma, cdma.cellIdentityCdma);
    out.signalStrengthCdma = cdma.signalStrengthCdma;
    out.signalStrengthEvdo = cdma.signalStrengthEvdo;
}

void convertCellInfoWcdma(RIL_CellInfoWcdma_v12& out, const RIL_CellInfoWcdma_V1_2& wcdma)
{
    convertCellIdentityWcdma(out.cellIdentityWcdma, wcdma.cellIdentityWcdma);
    out.signalStrengthWcdma.signalStrength = wcdma.signalStrengthWcdma.signalStrength;
    out.signalStrengthWcdma.bitErrorRate = wcdma.signalStrengthWcdma.bitErrorRate;
}

void convertCellInfoTdscdma(RIL_CellInfoTdscdma& out, const RIL_CellInfoTdscdma_V1_2& tdscdma)
{
    convertCellIdentityTdscdma(out.cellIdentityTdscdma, tdscdma.cellIdentityTdscdma);
    out.signalStrengthTdscdma.rscp = tdscdma.signalStrengthTdscdma.rscp;
}

void convertCellInfoLte(RIL_CellInfoLte_v12& out, const RIL_CellInfoLte_V1_2& lte)
{
    convertCellIdentityLte(out.cellIdentityLte, lte.cellIdentityLte);
    out.signalStrengthLte = lte.signalStrengthLte;
}

void convertCellInfoLte(RIL_CellInfoLte_v12& out, const RIL_CellInfoLte_V1_4& lte)
{
    convertCellIdentityLte(out.cellIdentityLte, lte.cellInfo.cellIdentityLte);
    out.signalStrengthLte = lte.cellInfo.signalStrengthLte;
}

void convertCellInfoLte(RIL_CellInfoLte_V1_2& out, const RIL_CellInfoLte_V1_4& lte)
{
    out.cellIdentityLte = lte.cellInfo.cellIdentityLte;
    out.signalStrengthLte = lte.cellInfo.signalStrengthLte;
}

void convertSignalStrengthLte(RIL_LTE_SignalStrength_v8& out, const RIL_LTE_SignalStrength_V1_6& lte)
{
    out.signalStrength = lte.signalStrength;
    out.rsrp = lte.rsrp;
    out.rsrq = lte.rsrq;
    out.rssnr = lte.rssnr;
    out.cqi = lte.cqi;
    out.timingAdvance = lte.timingAdvance;
}

void convertSignalStrengthNr(RIL_NR_SignalStrength_V1_4& out, const RIL_NR_SignalStrength_V1_6& nr)
{
    out.ssRsrp = nr.ssRsrp;
    out.ssRsrq = nr.ssRsrq;
    out.ssSinr = nr.ssSinr;
    out.csiRsrp = nr.csiRsrp;
    out.csiRsrq = nr.csiRsrq;
    out.csiSinr = nr.csiSinr;
}

void convertCellInfo(RIL_CellInfo_v12 &out, const RIL_CellInfo_V1_2 &rillCellInfo) {
    out.cellInfoType = rillCellInfo.cellInfoType;
    out.registered = rillCellInfo.registered;
    out.timeStampType = rillCellInfo.timeStampType;
    out.timeStamp = rillCellInfo.timeStamp;

    switch((int)rillCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM: {
            convertCellInfoGsm(out.CellInfo.gsm, rillCellInfo.CellInfo.gsm);
            break;
        }
        case RIL_CELL_INFO_TYPE_WCDMA: {
            convertCellInfoWcdma(out.CellInfo.wcdma, rillCellInfo.CellInfo.wcdma);
            break;
        }
        case RIL_CELL_INFO_TYPE_CDMA: {
            convertCellInfoCdma(out.CellInfo.cdma, rillCellInfo.CellInfo.cdma);
            break;
        }
        case RIL_CELL_INFO_TYPE_LTE: {
            convertCellInfoLte(out.CellInfo.lte, rillCellInfo.CellInfo.lte);

            break;
        }
        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            convertCellInfoTdscdma(out.CellInfo.tdscdma, rillCellInfo.CellInfo.tdscdma);
            break;
        }
        case RIL_CELL_INFO_TYPE_NR: {
            // NR not available
            out.cellInfoType = RIL_CELL_INFO_TYPE_NONE;
            break;
        }
        default:
            break;
    } // end switch
}

void convertCellInfo(RIL_CellInfo_v12& out, const RIL_CellInfo_V1_4& cellInfo)
{
    out.cellInfoType = cellInfo.cellInfoType;
    out.registered = cellInfo.registered;
    out.timeStamp = cellInfo.timeStamp;
    out.timeStampType = cellInfo.timeStampType;

    switch (out.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        convertCellInfoGsm(out.CellInfo.gsm, cellInfo.CellInfo.gsm);
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        convertCellInfoWcdma(out.CellInfo.wcdma, cellInfo.CellInfo.wcdma);
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        convertCellInfoLte(out.CellInfo.lte, cellInfo.CellInfo.lte);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        convertCellInfoTdscdma(out.CellInfo.tdscdma, cellInfo.CellInfo.tdscdma);
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        convertCellInfoCdma(out.CellInfo.cdma, cellInfo.CellInfo.cdma);
        break;
    default:
        break;
    } // end switch ~
}

void convertCellInfo(RIL_CellInfo_V1_2& out, const RIL_CellInfo_V1_4& cellInfo)
{
    out.cellInfoType = cellInfo.cellInfoType;
    out.registered = cellInfo.registered;
    out.timeStamp = cellInfo.timeStamp;
    out.timeStampType = cellInfo.timeStampType;

    switch (out.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        out.CellInfo.gsm = cellInfo.CellInfo.gsm;
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        out.CellInfo.wcdma = cellInfo.CellInfo.wcdma;
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        convertCellInfoLte(out.CellInfo.lte, cellInfo.CellInfo.lte);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        out.CellInfo.tdscdma = cellInfo.CellInfo.tdscdma;
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        out.CellInfo.cdma = cellInfo.CellInfo.cdma;
        break;
    default:
        break;
    } // end switch ~
}

void convertCellInfo(RIL_CellInfo_V1_4 &out, const RIL_CellInfo_V1_5 &rillCellInfo) {
    out.cellInfoType = rillCellInfo.cellInfoType;
    out.registered = rillCellInfo.registered;
    out.timeStampType = rillCellInfo.timeStampType;
    out.timeStamp = rillCellInfo.timeStamp;
    out.connectionStatus = rillCellInfo.connectionStatus;

    switch((int)rillCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM: {
            convertCellIdentityGsm(out.CellInfo.gsm.cellIdentityGsm, rillCellInfo.CellInfo.gsm.cellIdentityGsm);
            out.CellInfo.gsm.signalStrengthGsm = rillCellInfo.CellInfo.gsm.signalStrengthGsm;
            break;
        }
        case RIL_CELL_INFO_TYPE_WCDMA: {
            convertCellIdentityWcdma(out.CellInfo.wcdma.cellIdentityWcdma, rillCellInfo.CellInfo.wcdma.cellIdentityWcdma);
            out.CellInfo.wcdma.signalStrengthWcdma = rillCellInfo.CellInfo.wcdma.signalStrengthWcdma;
            break;
        }
        case RIL_CELL_INFO_TYPE_CDMA: {
            out.CellInfo.cdma = rillCellInfo.CellInfo.cdma;
            break;
        }
        case RIL_CELL_INFO_TYPE_LTE: {
            convertCellIdentityLte(out.CellInfo.lte.cellInfo.cellIdentityLte, rillCellInfo.CellInfo.lte.cellIdentityLte);
            out.CellInfo.lte.cellInfo.signalStrengthLte = rillCellInfo.CellInfo.lte.signalStrengthLte;
            // no data in V1_5. need to check
            out.CellInfo.lte.cellConfig.isEndcAvailable = rillCellInfo.CellInfo.lte.isEndcAvailable;
            break;
        }
        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            convertCellIdentityTdscdma(out.CellInfo.tdscdma.cellIdentityTdscdma, rillCellInfo.CellInfo.tdscdma.cellIdentityTdscdma);
            out.CellInfo.tdscdma.signalStrengthTdscdma = rillCellInfo.CellInfo.tdscdma.signalStrengthTdscdma;
            break;
        }
        case RIL_CELL_INFO_TYPE_NR: {
            convertCellIdentityNr(out.CellInfo.nr.cellidentityNr, rillCellInfo.CellInfo.nr.cellIdentityNr);
            out.CellInfo.nr.signalStrengthNr = rillCellInfo.CellInfo.nr.signalStrengthNr;
            break;
        }
        default:
            break;
    } // end switch
}

void convertCellInfo(RIL_CellInfo_V1_5& out, const RIL_CellInfo_V1_6& rillCellInfo) {
    out.cellInfoType = rillCellInfo.cellInfoType;
    out.registered = rillCellInfo.registered;
    out.timeStampType = rillCellInfo.timeStampType;
    out.timeStamp = ril_nano_time();
    out.connectionStatus = rillCellInfo.connectionStatus;

    switch((int)rillCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM: {
            out.CellInfo.gsm = rillCellInfo.CellInfo.gsm;
            break;
        }
        case RIL_CELL_INFO_TYPE_WCDMA: {
            out.CellInfo.wcdma = rillCellInfo.CellInfo.wcdma;
            break;
        }
        case RIL_CELL_INFO_TYPE_CDMA: {
            out.CellInfo.cdma = rillCellInfo.CellInfo.cdma;
            break;
        }
        case RIL_CELL_INFO_TYPE_LTE: {
            out.CellInfo.lte.cellIdentityLte = rillCellInfo.CellInfo.lte.cellIdentityLte;
            convertSignalStrengthLte(out.CellInfo.lte.signalStrengthLte, rillCellInfo.CellInfo.lte.signalStrengthLte);
            out.CellInfo.lte.isEndcAvailable = rillCellInfo.CellInfo.lte.isEndcAvailable;
            break;
        }
        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            out.CellInfo.tdscdma = rillCellInfo.CellInfo.tdscdma;
            break;
        }
        case RIL_CELL_INFO_TYPE_NR: {
            out.CellInfo.nr.cellIdentityNr = rillCellInfo.CellInfo.nr.cellIdentityNr;
            convertSignalStrengthNr(out.CellInfo.nr.signalStrengthNr, rillCellInfo.CellInfo.nr.signalStrengthNr);
            break;
        }
        default:
            break;
    } // end switch
}

void convertCellInfoList(list<RIL_CellInfo_v12>& out, list<RIL_CellInfo_V1_4>& cellInfoList)
{
    out.resize(0);
    list<RIL_CellInfo_V1_4>::iterator iter;
    for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++) {
        RIL_CellInfo_v12 cellInfo;
        convertCellInfo(cellInfo, *iter);
        out.push_back(cellInfo);
    } // end for ~
}

void convertCellInfoList(list<RIL_CellInfo_V1_2>& out, list<RIL_CellInfo_V1_4>& cellInfoList)
{
    out.resize(0);
    list<RIL_CellInfo_V1_4>::iterator iter;
    for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++) {
        RIL_CellInfo_V1_2 cellInfo{};
        convertCellInfo(cellInfo, *iter);
        out.push_back(cellInfo);
    } // end for ~
}

void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig& out,
        const RIL_PhysicalChannelConfig_V1_4& config)
{
    out.status = (RIL_CellConnectionStatus)config.status;
    out.cellBandwidthDownlink = config.cellBandwidthDownlink;
}

void convertPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& out,
        const RIL_PhysicalChannelConfig_V1_6& config)
{
    out.status = (int)config.status;
    out.rat = (int)config.rat;
    out.cellBandwidthDownlink = config.cellBandwidthDownlink;
    // channelNumber only
    out.rfInfoType = 2;
    out.rfInfo.channelNumber = config.downlinkChannelNumber;
    if (config.len_contextIds > 0 && config.contextIds != NULL) {
        out.contextIds = new int[config.len_contextIds] {};
        if (out.contextIds != NULL) {
            memcpy(out.contextIds, config.contextIds, sizeof(int) * config.len_contextIds);
            out.len_contextIds = config.len_contextIds;
        }
    } else {
        out.len_contextIds = 0;
        out.contextIds = NULL;
    }
    out.physicalCellId = config.physicalCellId;
}

void dupPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& out,
        const RIL_PhysicalChannelConfig_V1_4& config)
{
    out = config;
    if (config.len_contextIds > 0 && config.contextIds != NULL) {
        out.contextIds = new int[config.len_contextIds];
        if (out.contextIds != NULL) {
            memcpy(out.contextIds, config.contextIds, sizeof(int) * config.len_contextIds);
        }
        else {
            out.len_contextIds = 0;
        }
    }
}

void dupPhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_6& out,
        const RIL_PhysicalChannelConfig_V1_6& config)
{
    out = config;
}

void freePhysicalChannelConfig(RIL_PhysicalChannelConfig_V1_4& pcc) {
    if (pcc.contextIds && pcc.len_contextIds) {
        delete[] pcc.contextIds;
        pcc.contextIds = nullptr;
    }
}

bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const DataV2_0::RIL_DataProfileInfo &src)
{
    dest.apn = src.apn;
    dest.user = src.user;
    dest.password = src.password;
    dest.profileId = (RIL_DataProfileId) src.profileId;
    dest.protocol = (PdpProtocolType) src.protocol;
    dest.roamingProtocol = (PdpProtocolType) src.roamingProtocol;
    dest.authType = (RIL_ApnAuthType) src.authType;
    dest.type = (RIL_DataProfileInfoType) src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedApnTypesBitmap;
    dest.bearerBitmap = src.bearerBitmap;
    dest.mtuV4 = src.mtuV4;
    dest.mtuV6 = src.mtuV6;
    dest.preferred = src.preferred;
    dest.persistent = src.persistent;
    dest.alwaysOn = src.alwaysOn;

    return true;
}

bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const RIL_DataProfileInfo_V1_5 &src) {
    dest.apn = (src.apn != nullptr) ? src.apn : "" ;
    dest.user = (src.user != nullptr) ? src.user : "";
    dest.password = (src.password != nullptr) ? src.password : "";
    dest.profileId = (RIL_DataProfileId) src.profileId;
    dest.protocol = (PdpProtocolType) src.protocol;
    dest.roamingProtocol = (PdpProtocolType) src.roamingProtocol;
    dest.authType = (RIL_ApnAuthType) src.authType;
    dest.type = (RIL_DataProfileInfoType) src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedApnTypesBitmap;
    dest.bearerBitmap = src.bearerBitmap;
    dest.mtuV4 = src.mtuV4;
    dest.mtuV6 = src.mtuV6;
    dest.preferred = src.preferred;
    dest.persistent = src.persistent;

    return true;
}

bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const RIL_DataProfileInfo_V1_4 &src)
{
    dest.apn = (src.apn != nullptr) ? src.apn : "";
    dest.user = (src.user != nullptr) ? src.user : "";
    dest.password = (src.password != nullptr) ? src.password : "";
    dest.profileId = (RIL_DataProfileId) src.profileId;
    dest.protocol = (PdpProtocolType) src.protocol;
    dest.roamingProtocol = (PdpProtocolType) src.roamingProtocol;
    dest.authType = (RIL_ApnAuthType) src.authType;
    dest.type = (RIL_DataProfileInfoType) src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedApnTypesBitmap;
    dest.bearerBitmap = src.bearerBitmap;
    dest.mtuV4 = src.mtu;
    dest.mtuV6 = src.mtu;
    dest.preferred = src.preferred;
    dest.persistent = src.persistent;

    return true;
}

bool CopyDataProfileInfo(DataV2_0::RIL_DataProfileInfo &dest, const RIL_DataProfileInfo_v15 &src) {
    dest.apn = (src.apn != nullptr) ? src.apn : "";
    dest.user = (src.user != nullptr) ? src.user : "";
    dest.password = (src.password != nullptr) ? src.password : "";

    dest.protocol = PDP_PROTOCOL_TYPE_IP;
    if (src.protocol != NULL) {
        if (TextUtils::Equals(src.protocol, "IP")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IP;
        }
        else if (TextUtils::Equals(src.protocol, "IPV6")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IPV6;
        }
        else if (TextUtils::Equals(src.protocol, "IPV4V6")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IPV4V6;
        }
    }

    dest.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
    if (src.roamingProtocol != NULL) {
        if (TextUtils::Equals(src.roamingProtocol, "IP")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
        }
        else if (TextUtils::Equals(src.roamingProtocol, "IPV6")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IPV6;
        }
        else if (TextUtils::Equals(src.roamingProtocol, "IPV4V6")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6;
        }
    }

    dest.profileId = (RIL_DataProfileId)src.profileId;
    dest.authType = (RIL_ApnAuthType)src.authType;
    dest.type = (RIL_DataProfileInfoType)src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedTypesBitmask;
    dest.bearerBitmap = src.bearerBitmask;
    dest.mtuV4 = src.mtu;
    dest.mtuV6 = src.mtu;
    dest.preferred = false;
    dest.persistent = true;

    return true;
}

bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const DataV2_0::RIL_DataProfileInfo &src)
{
    static const int MAX_PDP_APN_LEN = 101;
    static const int MAX_PDP_USER_NAME_LEN = 20;
    if(!src.apn.empty() && dest.apn != nullptr)
        src.apn.copy(dest.apn, MAX_PDP_APN_LEN);
    if(!src.user.empty() && dest.user != nullptr)
        src.user.copy(dest.user, MAX_PDP_USER_NAME_LEN);
    if(!src.password.empty() && dest.password != nullptr)
        src.password.copy(dest.password, MAX_PDP_USER_NAME_LEN);

    if(dest.apn != nullptr && dest.user != nullptr && dest.password != nullptr)
    RLOGD("Copied from V2_0:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.profileId = (RIL_DataProfileId) src.profileId;
    dest.protocol = (PdpProtocolType) src.protocol;
    dest.roamingProtocol = (PdpProtocolType) src.roamingProtocol;
    dest.authType = (RIL_ApnAuthType) src.authType;
    dest.type = (RIL_DataProfileInfoType) src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedApnTypesBitmap;
    dest.bearerBitmap = src.bearerBitmap;
    dest.mtuV4 = src.mtuV4;
    dest.mtuV6 = src.mtuV6;
    dest.preferred = src.preferred;
    dest.persistent = src.persistent;

    return true;
}

bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_DataProfileInfo_V1_5 &src)
{
    if (!TextUtils::DupString(&dest.apn, src.apn) ||
        !TextUtils::DupString(&dest.user, src.user) ||
        !TextUtils::DupString(&dest.password, src.password)) {
        return false;
    }
    RLOGD("Copied:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.profileId = (RIL_DataProfileId) src.profileId;
    dest.protocol = (PdpProtocolType) src.protocol;
    dest.roamingProtocol = (PdpProtocolType) src.roamingProtocol;
    dest.authType = (RIL_ApnAuthType) src.authType;
    dest.type = (RIL_DataProfileInfoType) src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedApnTypesBitmap;
    dest.bearerBitmap = src.bearerBitmap;
    dest.mtuV4 = src.mtuV4;
    dest.mtuV6 = src.mtuV6;
    dest.preferred = src.preferred;
    dest.persistent = src.persistent;

    return true;
}

bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_DataProfileInfo_V1_4 &src)
{
    if (!TextUtils::DupString(&dest.apn, src.apn) ||
            !TextUtils::DupString(&dest.user, src.user) ||
            !TextUtils::DupString(&dest.password, src.password)) {
        return false;
    }
    RLOGD("Copied:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.profileId = (RIL_DataProfileId) src.profileId;
    dest.protocol = (PdpProtocolType) src.protocol;
    dest.roamingProtocol = (PdpProtocolType) src.roamingProtocol;
    dest.authType = (RIL_ApnAuthType) src.authType;
    dest.type = (RIL_DataProfileInfoType) src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedApnTypesBitmap;
    dest.bearerBitmap = src.bearerBitmap;
    dest.mtuV4 = src.mtu;
    dest.mtuV6 = src.mtu;
    dest.preferred = src.preferred;
    dest.persistent = src.persistent;

    return true;
}

bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_DataProfileInfo_v15 &src)
{
    if (!TextUtils::DupString(&dest.apn, src.apn) ||
        !TextUtils::DupString(&dest.user, src.user) ||
        !TextUtils::DupString(&dest.password, src.password)) {
        return false;
    }
    RLOGD("Copied:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.protocol = PDP_PROTOCOL_TYPE_IP;
    if (src.protocol != NULL) {
        if (TextUtils::Equals(src.protocol, "IP")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IP;
        }
        else if (TextUtils::Equals(src.protocol, "IPV6")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IPV6;
        }
        else if (TextUtils::Equals(src.protocol, "IPV4V6")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IPV4V6;
        }
    }

    dest.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
    if (src.roamingProtocol != NULL) {
        if (TextUtils::Equals(src.roamingProtocol, "IP")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
        }
        else if (TextUtils::Equals(src.roamingProtocol, "IPV6")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IPV6;
        }
        else if (TextUtils::Equals(src.roamingProtocol, "IPV4V6")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6;
        }
    }

    dest.profileId = (RIL_DataProfileId)src.profileId;
    dest.authType = (RIL_ApnAuthType)src.authType;
    dest.type = (RIL_DataProfileInfoType)src.type;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.supportedApnTypesBitmap = src.supportedTypesBitmask;
    dest.bearerBitmap = src.bearerBitmask;
    dest.mtuV4 = src.mtu;
    dest.mtuV6 = src.mtu;
    dest.preferred = false;
    dest.persistent = true;

    return true;
}

bool CopyDataProfileInfo(RIL_DataProfileInfo_v15 &dest, const RIL_DataProfileInfo_v15 &src)
{
    if (!TextUtils::DupString(&dest.apn, src.apn) ||
        !TextUtils::DupString(&dest.protocol, src.protocol) ||
        !TextUtils::DupString(&dest.roamingProtocol, src.roamingProtocol) ||
        !TextUtils::DupString(&dest.mvnoType, src.mvnoType) ||
        !TextUtils::DupString(&dest.mvnoMatchData, src.mvnoMatchData) ||
        !TextUtils::DupString(&dest.user, src.user) ||
        !TextUtils::DupString(&dest.password, src.password)) {
        return false;
    }
    RLOGD("Copied:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.profileId = src.profileId;
    dest.authType = src.authType;
    dest.type = src.type;
    dest.supportedTypesBitmask = src.supportedTypesBitmask;
    dest.bearerBitmask = src.bearerBitmask;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.mtu = src.mtu;
    return true;
}

bool CopyDataProfileInfo(RIL_DataProfileInfo_v15 &dest, const RIL_DataProfileInfo &src)
{
    if (!TextUtils::DupString(&dest.apn, src.apn) ||
        !TextUtils::DupString(&dest.protocol, src.protocol) ||
        !TextUtils::DupString(&dest.roamingProtocol, src.protocol) ||
        !TextUtils::DupString(&dest.user, src.user) ||
        !TextUtils::DupString(&dest.password, src.password)) {
        return false;
    }
    RLOGD("Copied:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.profileId = src.profileId;
    dest.roamingProtocol = NULL;
    dest.authType = src.authType;
    dest.type = src.type;
    dest.supportedTypesBitmask = 0;
    dest.bearerBitmask = 0;
    dest.maxConnsTime = src.maxConnsTime;
    dest.maxConns = src.maxConns;
    dest.waitTime = src.waitTime;
    dest.enabled = src.enabled;
    dest.mtu = 0;
    dest.mvnoType = NULL;
    dest.mvnoMatchData = NULL;
    return true;
}

// For legacy, will be removed when HIDL is removed or R(V1_5) version becomes baseline
bool CopyDataProfileInfo(RIL_DataProfileInfo_V1_5 &dest, const RIL_InitialAttachApn_v15 &src)
{
    if (!TextUtils::DupString(&dest.apn, src.apn) ||
        !TextUtils::DupString(&dest.user, src.username) ||
        !TextUtils::DupString(&dest.password, src.password)) {
        return false;
    }
    RLOGD("Copied:APN:%s(%p), user:%s(%p), password:%s(%p), %d",
          dest.apn, dest.apn, dest.user, dest.user, dest.password, dest.password, __LINE__);

    dest.protocol = PDP_PROTOCOL_TYPE_IP;
    if (src.protocol != NULL) {
        if (TextUtils::Equals(src.protocol, "IP")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IP;
        }
        else if (TextUtils::Equals(src.protocol, "IPV6")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IPV6;
        }
        else if (TextUtils::Equals(src.protocol, "IPV4V6")) {
            dest.protocol = PDP_PROTOCOL_TYPE_IPV4V6;
        }
    }

    dest.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
    if (src.roamingProtocol != NULL) {
        if (TextUtils::Equals(src.roamingProtocol, "IP")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IP;
        }
        else if (TextUtils::Equals(src.roamingProtocol, "IPV6")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IPV6;
        }
        else if (TextUtils::Equals(src.roamingProtocol, "IPV4V6")) {
            dest.roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6;
        }
    }

    dest.profileId = RIL_DataProfileId::INVALID;
    dest.authType = (RIL_ApnAuthType)src.authtype;
    dest.type = RIL_DataProfileInfoType::COMMON;
    dest.maxConnsTime = 0;
    dest.maxConns = 0;
    dest.waitTime = 0;
    dest.enabled = true;
    dest.supportedApnTypesBitmap = src.supportedTypesBitmask;
    dest.bearerBitmap = src.bearerBitmask;
    dest.mtuV4 = src.mtu;
    dest.mtuV6 = src.mtu;
    dest.preferred = false;
    dest.persistent = src.modemCognitive;
    // mvnoType and mvnoMatchData is not supported higher version

    return true;
}

void FreeDataProfileInfo(RIL_DataProfileInfo_V1_4 &dpi)
{
    if (dpi.apn != NULL) {
        delete[] dpi.apn;
        dpi.apn = NULL;
    }

    if (dpi.user != NULL) {
        delete[] dpi.user;
        dpi.user = NULL;
    }

    if (dpi.password != NULL) {
        delete[] dpi.password;
        dpi.password = NULL;
    }
}

void FreeDataProfileInfo(RIL_DataProfileInfo_V1_5 &dpi)
{
    if (dpi.apn != NULL) {
        delete[] dpi.apn;
        dpi.apn = NULL;
    }

    if (dpi.user != NULL) {
        delete[] dpi.user;
        dpi.user = NULL;
    }

    if (dpi.password != NULL) {
        delete[] dpi.password;
        dpi.password = NULL;
    }
}

void CopyLinkAddress(RIL_LinkAddress &dest, const RIL_LinkAddress &src) {
    TextUtils::DupString(&dest.address, src.address);
    dest.properties = src.properties;
    dest.deprecationTime = src.deprecationTime;
    dest.expirationTime = src.expirationTime;
}

void CopyLinkAddress(RIL_LinkAddress &dest, const DataV2_0::RIL_LinkAddress &src) {
    TextUtils::DupString(&dest.address, (char *)src.address.c_str());
    dest.properties = src.properties;
    dest.deprecationTime = src.deprecationTime;
    dest.expirationTime = src.expirationTime;
}

void CopyLinkAddress(DataV2_0::RIL_LinkAddress &dest, const RIL_LinkAddress &src) {
    dest.address = (src.address == nullptr) ? "" : src.address;
    dest.properties = src.properties;
    dest.deprecationTime = src.deprecationTime;
    dest.expirationTime = src.expirationTime;
}

void FreeLinkAddress(RIL_LinkAddress &linkAddress)
{
    if (linkAddress.address != NULL) {
        delete[] linkAddress.address;
        linkAddress.address = NULL;
    }
}

void convertSetupDataCallInfo(RIL_SetupDataCallInfo_V1_5& to, RIL_SetupDataCallInfo_V1_4& from)
{
    to.accessNetwork = from.accessNetwork;
    to.roamingAllow = from.roamingAllow;
    to.reason = from.reason;
    to.dnses = from.dnses;
    CopyDataProfileInfo(to.dataProfileInfo, from.dataProfileInfo);
    if (to.linkAddress != NULL) {
        to.leng_addresses = 1;
        to.linkAddress[0].address = from.addresses;
    }
}

/*
void convertDataCallList(RIL_Data_Call_Response_v11 *outlist, RIL_Data_Call_Response_V1_4 *dataCallList, int num)
{
     for (int i = 0; i < num; ++i) {
        convertDataCall(outlist[i], dataCallList[i]);
     }
}
*/

void convertDataCallInfo(RIL_Data_Call_Response_v11* dst, void* src, int num)
{
    RIL_SetupDataCallResult_V1_6* dcResponse = (RIL_SetupDataCallResult_V1_6*)src;

    for (int i = 0; i < num; i++) {
        convertSetupDataCallInfo(&dst[i], &dcResponse[i]);
    }
}

void freeIPAddrInfo(RIL_Data_Call_Response_v11 &dcV11)
{
    if (dcV11.addresses != NULL) {
        free(dcV11.addresses);
    }

    if (dcV11.dnses != NULL) {
        free(dcV11.dnses);
    }

    if (dcV11.pcscf != NULL) {
         free(dcV11.pcscf);
    }

    if (dcV11.gateways != NULL) {
        free(dcV11.gateways);
    }
}


const char* setProtocolType(PdpProtocolType src)
{
    if (src == PDP_PROTOCOL_TYPE_IP)
        return  DATA_PROTOCOL_IP;
    else if (src == PDP_PROTOCOL_TYPE_IPV6)
       return  DATA_PROTOCOL_IPV6;
    else if (src == PDP_PROTOCOL_TYPE_IPV4V6)
        return DATA_PROTOCOL_IPV4V6;
    else if (src == PDP_PROTOCOL_TYPE_PPP)
       return DATA_PROTOCOL_PPP;
    else return DATA_PROTOCOL_IP;
}

void convertSetupDataCallInfo(RIL_Data_Call_Response_v11* dst, RIL_SetupDataCallResult_V1_6* src)
{
    string totalAddr;
    string dnses;
    string pcscf;
    string gateways;

    if(dst ==NULL || src==NULL){
        RLOGD("invalid input");
        return;
    }

    dst->status = (int)src->cause;
    dst->suggestedRetryTime = (int32_t)(src->suggestedRetryTime);
    dst->cid = src->cid;
    dst->active = (int)src->active;
    dst->type = setProtocolType(src->type);
    dst->ifname = src->ifname;
    dst->mtu = src->mtuV4;

    //IP Addr
    for (auto &linkaddr : src->linkaddresses) {
        totalAddr.append(linkaddr.address);
        totalAddr.append(" ");
    }

    dst->addresses = (char*)malloc(totalAddr.size()+1);
    memset(dst->addresses, 0, totalAddr.size()+1);
    totalAddr.copy(dst->addresses, sizeof(char) * totalAddr.size());
    dst->addresses[totalAddr.size()] = '\0';

    //DNS
    for (auto &a : src->dnses) {
        dnses.append(a);
        dnses.append(" ");
    }

    dst->dnses = (char*)malloc(dnses.size() + 1);
    memset(dst->dnses, 0, dnses.size() + 1);
    dnses.copy(dst->dnses, sizeof(char) * dnses.size());
    dst->dnses[dnses.size()] = '\0';

    //P-CSCF
    for (auto &a : src->pcscf) {
        pcscf.append(a);
        pcscf.append(" ");
    }

    dst->pcscf = (char*)malloc(pcscf.size() + 1);
    memset(dst->pcscf, 0, pcscf.size() + 1);
    pcscf.copy(dst->pcscf, sizeof(char) * pcscf.size());
    dst->pcscf[pcscf.size()] = '\0';

    //Gateway
    for (auto &a : src->gateways) {
        gateways.append(a);
        gateways.append(" ");
    }

    dst->gateways = (char*)malloc(gateways.size() + 1);
    memset(dst->gateways, 0, gateways.size() + 1);
    gateways.copy(dst->gateways, sizeof(char) * gateways.size());
    dst->gateways[gateways.size()] = '\0';

    //RLOGD("protocolType %s",  dst->type);
    //RLOGD("src Addrs:%s %s %s %s", totalAddr.c_str(), dnses.c_str(), pcscf.c_str(), gateways.c_str());
    //RLOGD("dst Addrs:%s %s %s %s",  dst->addresses, dst->dnses, dst->pcscf,  dst->gateways);

}

void convertDnn(std::vector<std::string>&dst, std::vector<RIL_OptionalDNN>& src) {
    for (auto &dnn : src) {
        dst.push_back(dnn.value);
    }
}

void convertOSAppId(std::vector<DataV2_0::RIL_OSAppId> &dst, std::vector<RIL_OptionalOSAppId> &src) {
    for (auto osappid : src) {
        DataV2_0::RIL_OSAppId tmp = { { } };
        memcpy(&tmp.value[0], &osappid.value[0], std::min(sizeof(osappid.value), sizeof(tmp.value)));
        dst.push_back(tmp);
    }
}

void convertRadioAccessSpecifiers(RIL_RadioAccessSpecifier_V1_5 &out, RIL_RadioAccessSpecifier &ras)
{
    int i;

    out.radio_access_network = ras.radio_access_network;
    out.bands_length = ras.bands_length;

    switch (ras.radio_access_network) {
        case RIL_RAN_GERAN:
            for (i = 0; i < (int)ras.bands_length; i++)
                out.bands.geran_bands[i] = ras.bands.geran_bands[i];
            break;
        case RIL_RAN_UTRAN:
            for (i = 0; i < (int)ras.bands_length; i++)
                out.bands.utran_bands[i] = ras.bands.utran_bands[i];
            break;
        case RIL_RAN_EUTRAN:
            for (i = 0; i < (int)ras.bands_length; i++)
                out.bands.eutran_bands[i] = ras.bands.eutran_bands[i];
            break;
        default:
            break;
    } // end switch ~

    out.channels_length = ras.channels_length;
    for (i = 0; i < (int)ras.channels_length; i++)
        out.channels[i] = ras.channels[i];

}

} // namespace RadioConvUtils
