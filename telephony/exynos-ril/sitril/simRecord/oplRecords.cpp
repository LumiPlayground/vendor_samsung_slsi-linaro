/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include "oplRecords.h"
#include "iccUtil.h"
#include "rillog.h"

#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

OplRecord::OplRecord()
    : mPlmn(), mLac1(0), mLac2(0), mPnnRecordNumber(0) {
}

OplRecord::OplRecord(BYTE *record, int len, bool is5G)
    : OplRecord() {
    const int plmnOffset = 0;
    const int lacOffset = 3;
    const int pnnRecordNumberOffset = 7;
    const int pnnRecordNumberOffsetForOpl5g = 9;

    updatePlmn(record, plmnOffset, len);
    if (is5G) {
        updateLacForOpl5g(record, lacOffset, len);
        updatePnnRecordNumber(record, pnnRecordNumberOffsetForOpl5g, len);

    } else {
        updateLac(record, lacOffset, len);
        updatePnnRecordNumber(record, pnnRecordNumberOffset, len);
    }
}

bool OplRecord::isPlmnEmpty() {
    bool ret = true;
    for (auto digit : mPlmn) {
        if (digit != 0) {
            ret = false;
            break;
        }
    }
    return ret;
}

std::string OplRecord::toString() {
    char dbgBuf[100] = { 0, };
    sprintf(dbgBuf, "PLMN=%1x%1x%1x%1x%1x%1x, LAC1=%d, LAC2=%d, PNN Record=%d",
            mPlmn[0], mPlmn[1], mPlmn[2], mPlmn[3], mPlmn[4], mPlmn[5], mLac1, mLac2, mPnnRecordNumber);
    return std::string(dbgBuf);
}

void OplRecord::updatePlmn(BYTE *record, int offset, int len) {
    std::string plmnCode = IccUtil::bcdPlmnToString(record, offset, len);
    int i = 0;
    auto iter = plmnCode.begin();
    while (iter != plmnCode.end() && i < MAX_PLMN_LEN) {
        mPlmn[i++] = IccUtil::hexCharToInt(*iter++);
    }
}

void OplRecord::updateLacForOpl5g(BYTE *record, int offset, int len) {
    const int tacLen = 3;
    mLac1 = IccUtil::bytesToInt(record, len, offset, tacLen);
    mLac2 = IccUtil::bytesToInt(record, len, offset + tacLen, tacLen);
}

void OplRecord::updateLac(BYTE *record, int offset, int len) {
    const int lacLen = 2;
    mLac1 = IccUtil::bytesToInt(record, len, offset, lacLen);
    mLac2 = IccUtil::bytesToInt(record, len, offset + lacLen, lacLen);
}

void OplRecord::updatePnnRecordNumber(BYTE *record, int offset, int len) {
    const int pnnRecordNumberLen = 1;
    mPnnRecordNumber = IccUtil::bytesToInt(record, len, offset, pnnRecordNumberLen);
}

OplRecords::OplRecords() {
    mRecords.clear();
}

OplRecords::OplRecords(BYTE **records, int recordSize, int numRecords, bool is5G)
    : OplRecords() {
    for (int i = 0; i < numRecords; ++i) {
        OplRecord *oplRecord = new OplRecord(records[i], recordSize, is5G);
        if (oplRecord->isPlmnEmpty()) {
            RilLogV("Input OPL index %d (start from 1) is empty. so skip it", i + 1);
            delete oplRecord;
            continue;
        }
        mRecords.push_back(oplRecord);
        RilLogV("OPL Record[%d]: %s", size(), oplRecord->toString().c_str());
    }
}

OplRecords::~OplRecords() {
    for (auto record : mRecords) {
        if (record != nullptr) {
            delete record;
        }
    }
    mRecords.clear();
}

int OplRecords::getPnnRecordNumber(const std::string &plmn, int lac, bool useLac) {
    if (isPlmnValid(plmn) == false) {
        return 0;
    }

    if (useLac && !isLacValid(lac)) {
        return 0;
    }

    int plmnFromNetwork[MAX_PLMN_LEN] = {0, 0, 0, 0, 0, 0};
    convertPlmnStrToIntArray(plmn, plmnFromNetwork);

    for (auto record : mRecords) {
        if (isPlmnMatched(record->getPlmn(), plmnFromNetwork)) {
            // For Netework Scan case, lac is not available. It means useLac is false.
            if (!useLac || record->isLacInRange(lac)) {
                return record->getPnnRecordNumber();
            }
        }
    }

    return 0;
}

bool OplRecords::isLacValid(int lac) {
    const int invalidLac = -1;
    return (lac != invalidLac);
}

bool OplRecords::isPlmnValid(const std::string &plmn) {
    const int plmnLen5 = 5;
    const int plmnLen6 = 6;
    return (plmn.length() == plmnLen5) || (plmn.length() == plmnLen6);
}

void OplRecords::convertPlmnStrToIntArray(const std::string &plmn, int (&dst)[MAX_PLMN_LEN]) {
    int i = 0;
    auto iter = plmn.begin();
    while (iter != plmn.end() && i < MAX_PLMN_LEN) {
        dst[i++] = *iter++ - '0';
    }
}

bool OplRecords::isPlmnMatched(int plmnFromOpl[MAX_PLMN_LEN], int plmnFromNetwork[MAX_PLMN_LEN]) {
    const int wildCardDigit = 0x0D;
    bool match = true;
    for (int i = 0; i < MAX_PLMN_LEN && match == true; i++) {
        match = match && ((plmnFromOpl[i] == plmnFromNetwork[i]) || (plmnFromOpl[i] == wildCardDigit));
    }
    return match;
}
