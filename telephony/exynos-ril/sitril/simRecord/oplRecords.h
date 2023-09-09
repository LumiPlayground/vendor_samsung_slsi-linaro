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

#include "constdef.h"
#include "types.h"
#include <string>
#include <vector>

class OplRecord {
  public:
    OplRecord();
    OplRecord(BYTE *record, int len, bool is5G = false);
    virtual ~OplRecord() = default;

    int *getPlmn() {
        return mPlmn;
    }
    bool isLacInRange(int lac) {
        return ((mLac1 <= lac) && (lac <= mLac2));
    }
    int getPnnRecordNumber() {
        return mPnnRecordNumber;
    }
    bool isPlmnEmpty();
    std::string toString();

  private:
    int mPlmn[MAX_PLMN_LEN];
    int mLac1;
    int mLac2;
    int mPnnRecordNumber;

    void updatePlmn(BYTE *record, int offset, int len);
    void updateLacForOpl5g(BYTE *record, int offset, int len);
    void updateLac(BYTE *record, int offset, int len);
    void updatePnnRecordNumber(BYTE *record, int offset, int len);
};

class OplRecords {
  public:
    OplRecords();
    OplRecords(BYTE **records, int recordSize, int numRecords, bool is5G = false);
    virtual ~OplRecords();

    int size() {
        return mRecords.size();
    }
    int getPnnRecordNumber(const std::string &plmn, int lac, bool useLac);

  private:
    std::vector<OplRecord *> mRecords;

    bool isLacValid(int lac);
    bool isPlmnValid(const std::string &plmn);
    void convertPlmnStrToIntArray(const std::string &plmn, int (&dst)[MAX_PLMN_LEN]);
    bool isPlmnMatched(int plmnFromOpl[MAX_PLMN_LEN], int plmnFromCell[MAX_PLMN_LEN]);
};
