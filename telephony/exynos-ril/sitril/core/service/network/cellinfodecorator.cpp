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
 * cellinfodecorator.cpp
 *
 *  Created on: 2021. 10. 5.
 */
#include "cellinfodecorator.h"
#include "eonsResolver.h"
#include "operatorinforesolver.h"
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>
#include <librilutils/textutils.h>

/**
 * SignalStrengthVisibleDecorator
 */
// Fill a valid signal strength info so that the result UI can show a valid antenna bar
RIL_CellInfo_V1_6 *SignalStrengthVisibleDecorator::decorate() {
    RIL_CellInfo_V1_6 *cellInfo = mBase.decorate();
    if (cellInfo) {
        switch((int)(cellInfo->cellInfoType)) {
            case RIL_CELL_INFO_TYPE_GSM:
                cellInfo->CellInfo.gsm.signalStrengthGsm.signalStrength = 30;
                break;
            case RIL_CELL_INFO_TYPE_WCDMA:
                cellInfo->CellInfo.wcdma.signalStrengthWcdma.signalStrength = 30;
                break;
            case RIL_CELL_INFO_TYPE_LTE:
                cellInfo->CellInfo.lte.signalStrengthLte.signalStrength = 31;
                cellInfo->CellInfo.lte.signalStrengthLte.rsrp = 44;
                cellInfo->CellInfo.lte.signalStrengthLte.rsrq = 3;
                break;
            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                cellInfo->CellInfo.tdscdma.signalStrengthTdscdma.signalStrength = 30;
                cellInfo->CellInfo.tdscdma.signalStrengthTdscdma.rscp = 90;
                break;
            case RIL_CELL_INFO_TYPE_NR:
                cellInfo->CellInfo.nr.signalStrengthNr.ssRsrp = 44;
                cellInfo->CellInfo.nr.signalStrengthNr.ssRsrq = 3;
                cellInfo->CellInfo.nr.signalStrengthNr.ssSinr = 20;
                break;
            default:
                break;
        }
    }
    return cellInfo;
}

/**
 * AlphaEnosDecorator
 */
RIL_CellInfo_V1_6 *AlphaEnosDecorator::decorate() {
    RIL_CellInfo_V1_6 *cellInfo = mBase.decorate();
    if (cellInfo) {
        switch((int)(cellInfo->cellInfoType)) {
            case RIL_CELL_INFO_TYPE_GSM:
                decorate(cellInfo->CellInfo.gsm);
                break;
            case RIL_CELL_INFO_TYPE_WCDMA:
                decorate(cellInfo->CellInfo.wcdma);
                break;
            case RIL_CELL_INFO_TYPE_LTE:
                decorate(cellInfo->CellInfo.lte);
                break;
            case RIL_CELL_INFO_TYPE_TD_SCDMA:
                decorate(cellInfo->CellInfo.tdscdma);
                break;
            case RIL_CELL_INFO_TYPE_NR:
                decorate(cellInfo->CellInfo.nr);
                break;
            default:
                break;
        }
    }
    return cellInfo;
}

void AlphaEnosDecorator::decorate(RIL_CellInfoGsm_V1_5& info) {
    std::string numeric = ril::util::mcc::decode(info.cellIdentityGsm.mcc) +
            ril::util::mnc::decode(info.cellIdentityGsm.mnc);
    OperatorInfo opname = OperatorInfoResolver().resolve(numeric, mSimNumeric, mSpn, mResolver);
    if (opname.valid()) {
        fillOperatorName(info.cellIdentityGsm.operatorNames, opname.alphaLong, opname.alphaShort);
    }
}

void AlphaEnosDecorator::decorate(RIL_CellInfoWcdma_V1_5& info) {
    std::string numeric = ril::util::mcc::decode(info.cellIdentityWcdma.mcc) +
            ril::util::mnc::decode(info.cellIdentityWcdma.mnc);
    OperatorInfo opname = OperatorInfoResolver().resolve(numeric, mSimNumeric, mSpn, mResolver);
    if (opname.valid()) {
        fillOperatorName(info.cellIdentityWcdma.operatorNames, opname.alphaLong, opname.alphaShort);
    }
}

void AlphaEnosDecorator::decorate(RIL_CellInfoTdscdma_V1_5& info) {
    std::string numeric = ril::util::mcc::decode(info.cellIdentityTdscdma.mcc) +
            ril::util::mnc::decode(info.cellIdentityTdscdma.mnc);
    OperatorInfo opname = OperatorInfoResolver().resolve(numeric, mSimNumeric, mSpn, mResolver);
    if (opname.valid()) {
        fillOperatorName(info.cellIdentityTdscdma.operatorNames, opname.alphaLong, opname.alphaShort);
    }
}

void AlphaEnosDecorator::decorate(RIL_CellInfoLte_V1_6& info) {
    std::string numeric = ril::util::mcc::decode(info.cellIdentityLte.mcc) +
            ril::util::mnc::decode(info.cellIdentityLte.mnc);
    OperatorInfo opname = OperatorInfoResolver().resolve(numeric, mSimNumeric, mSpn, mResolver);
    if (opname.valid()) {
        fillOperatorName(info.cellIdentityLte.operatorNames, opname.alphaLong, opname.alphaShort);
    }
}

void AlphaEnosDecorator::decorate(RIL_CellInfoNr_V1_6& info) {
    std::string numeric = ril::util::mcc::decode(info.cellIdentityNr.mcc) +
            ril::util::mnc::decode(info.cellIdentityNr.mnc);
    OperatorInfo opname = OperatorInfoResolver().resolve(numeric, mSimNumeric, mSpn, mResolver);
    if (opname.valid()) {
        fillOperatorName(info.cellIdentityNr.operatorNames, opname.alphaLong, opname.alphaShort);
    }
}

void AlphaEnosDecorator::fillOperatorName(RIL_CellIdentityOperatorNames& opname,
        const std::string& alphaLong, const std::string& alphaShort) {
    if (!TextUtils::IsEmpty(alphaLong) && !TextUtils::IsEmpty(alphaShort)) {
        strlcpy(opname.alphaLong, alphaLong.c_str(), MAX_FULL_NAME_LEN-1);
        strlcpy(opname.alphaShort, alphaShort.c_str(), MAX_SHORT_NAME_LEN-1);
    }
}
