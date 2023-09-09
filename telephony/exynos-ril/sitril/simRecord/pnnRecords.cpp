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
#include "pnnRecords.h"
#include "iccUtil.h"
#include "rillog.h"
#include <librilutils/textutils.h>

#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)

PnnRecord::PnnRecord()
    : mFullName(""), mShortName(""), mAddInfo("") {
}

PnnRecord::PnnRecord(BYTE *record, int len)
    : PnnRecord() {
    const int TAG_FULL_NAME_IEI = 0x43;
    const int TAG_SHORT_NAME_IEI = 0x45;
    const int TAG_ADDL_INFO = 0x80;

    SimTlv *tlv = new SimTlv(record, 0, len);

    mFullName = getDecodedStrFromTlv(tlv, TAG_FULL_NAME_IEI);

    tlv->nextObject();
    mShortName = getDecodedStrFromTlv(tlv, TAG_SHORT_NAME_IEI);

    tlv->nextObject();
    mAddInfo = getDecodedStrFromTlv(tlv, TAG_ADDL_INFO);

    delete tlv;
}

std::string PnnRecord::toString() {
    std::stringstream ss;
    ss << mFullName << ", " << mShortName;
    return ss.str();
}

std::string PnnRecord::getDecodedStrFromTlv(void *in_tlv, int tagId) {
    if (in_tlv == nullptr) {
        return "";
    }

    SimTlv *tlv = (SimTlv *)in_tlv;
    std::string ret("");
    if (tlv->isValidObject() && tlv->getTag() == tagId) {
        BYTE *tlvData = tlv->getData();
        int tlvDataLen = tlv->getDataLength();
        if (tlvData != nullptr) {
            ret = IccUtil::networkNameToString(tlvData, 0, tlvDataLen);
            delete [] tlvData;
        }
    } else {
        if (DBG) {
            RilLogV("Invalid tlv Object for tadId(%d), tag= %d, valid=%d", tagId, tlv->getTag(), tlv->isValidObject());
        }
    }

    return ret;
}

PnnRecords::PnnRecords() {
    mRecords.clear();
}

PnnRecords::PnnRecords(BYTE **records, int recordSize, int numRecords)
    : PnnRecords() {
    for (int i = 0; i < numRecords; ++i) {
        PnnRecord *pnnRecord = new PnnRecord(records[i], recordSize);
        if (pnnRecord->getFullName().empty()) {
            RilLogV("Input PNN index %d (start form 1) is empty. so skip it", i + 1);
            delete pnnRecord;
            continue;
        }
        mRecords.push_back(pnnRecord);
        RilLogV("PNN Record[%d]: %s", size(), pnnRecord->toString().c_str());
    }
}

PnnRecords::~PnnRecords() {
    for (auto record : mRecords) {
        if (record != nullptr) {
            delete record;
        }
    }
    mRecords.clear();
}

std::string PnnRecords::getNameFromPnnRecord(int recordNumber){
    if (recordNumber < 1 || recordNumber > size()) {
        if (DBG) {
            RilLogV("Invalid PNN record number %d", recordNumber);
        }
        return "";
    }

    auto record = mRecords[recordNumber - 1];
    if (record != nullptr) {
        return record->getFullName();
    }

    return "";
}
