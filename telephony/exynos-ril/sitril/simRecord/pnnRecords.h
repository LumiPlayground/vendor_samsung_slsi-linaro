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
#pragma once

#include "types.h"
#include <string>
#include <vector>

class PnnRecord {
  public:
    PnnRecord();
    PnnRecord(BYTE *record, int len);
    virtual ~PnnRecord() = default;

    std::string getFullName() {
        return mFullName;
    }
    std::string getShortName() {
        return mShortName;
    }
    std::string getAddlInfo() {
        return mAddInfo;
    }

    std::string toString();

  private:
    std::string getDecodedStrFromTlv(void *in_tlv, int tagId);

    static const bool DBG = false;

    std::string mFullName;
    std::string mShortName;
    std::string mAddInfo;
};

class PnnRecords {
  public:
    PnnRecords();
    PnnRecords(BYTE **records, int recordSize, int numRecords);
    virtual ~PnnRecords();

    int size() {
        return mRecords.size();
    }
    std::string getNameFromPnnRecord(int recordNumber);

  private:
    static const bool DBG = false;

    std::vector<PnnRecord *> mRecords;
};
