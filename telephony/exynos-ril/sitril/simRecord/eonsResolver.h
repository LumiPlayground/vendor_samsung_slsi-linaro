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

#include "pnnRecords.h"
#include "oplRecords.h"
#include <string>

class EonsResolver {
  public:
    enum EonsOpState {
        EONS_OP_INIT,
        EONS_OP_PNN,
        EONS_OP_PNN_OPL
    };

    EonsResolver();

    void reset() {
        resetOplData();
        resetOpl5GData();
        resetPnnData();
    }

    void setOplData(BYTE **records, int recordSize, int numRecords) {
        resetOplData();
        mpOplRecords = new OplRecords(records, recordSize, numRecords);
        if (mpOplRecords != nullptr && mpOplRecords->size() > 0) {
            mOplDataState = EF_PRESENT;
        }
    }

    void setOpl5GData(BYTE **records, int recordSize, int numRecords) {
        resetOpl5GData();
        mpOpl5GRecords = new OplRecords(records, recordSize, numRecords, true);
        if (mpOpl5GRecords != nullptr && mpOpl5GRecords->size() > 0) {
            mOpl5GDataState = EF_PRESENT;
        }
    }

    void setPnnData(BYTE **records, int recordSize, int numRecords) {
        resetPnnData();
        mpPnnRecords = new PnnRecords(records, recordSize, numRecords);
        if (mpPnnRecords != nullptr && mpPnnRecords->size() > 0) {
            mPnnDataState = EF_PRESENT;
        }
    }

    void resetOplData() {
        mOplDataState = EF_ABSENT;
        if (mpOplRecords != nullptr) {
            delete mpOplRecords;
            mpOplRecords = nullptr;
        }
    }

    void resetOpl5GData() {
        mOpl5GDataState = EF_ABSENT;
        if (mpOpl5GRecords != nullptr) {
            delete mpOpl5GRecords;
            mpOpl5GRecords = nullptr;
        }
    }

    void resetPnnData() {
        mPnnDataState = EF_ABSENT;
        if (mpPnnRecords != nullptr) {
            delete mpPnnRecords;
            mpPnnRecords = nullptr;
        }
    }

    std::string getEons(const std::string &regOperator, int lac, const std::string &simOperator);
    std::string getEonsForAvailableNetworks(const std::string &availOperator);

    static EonsResolver& createForPhoneId(int phoneId);

  private:
    EonsOpState getEonsOpState() {
        if (mPnnDataState == EF_PRESENT) {
            if (mOplDataState == EF_PRESENT || mOpl5GDataState == EF_PRESENT) {
                return EONS_OP_PNN_OPL;
            } else {
                return EONS_OP_PNN;
            }
        } else {
            return EONS_OP_INIT;
        }
    }
    std::string getEonsUsingPnnOpl(const std::string &regOperator, int lac);
    std::string getEonsUsingPnnIfRegInHplmn(const std::string &regOperator, const std::string &simOperator);
    int getPnnRecordNumberFromOpl5G(const std::string &plmn, int lac, bool useLac);
    int getPnnRecordNumberFromOpl(const std::string &plmn, int lac, bool useLac);
    std::string getNameFromPnn(int pnnRecordNumber);

    static EonsResolver sResolver[];

    enum EfDataState {
        EF_PRESENT,
        EF_ABSENT
    };

    EfDataState mPnnDataState;
    EfDataState mOplDataState;
    EfDataState mOpl5GDataState;
    OplRecords *mpOplRecords;
    OplRecords *mpOpl5GRecords;
    PnnRecords *mpPnnRecords;
};
