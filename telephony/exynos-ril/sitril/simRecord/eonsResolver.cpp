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
#include "eonsResolver.h"
#include "rillog.h"
#include <telephony/ril.h>

#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

EonsResolver::EonsResolver()
    : mPnnDataState(EF_ABSENT), mOplDataState(EF_ABSENT), mOpl5GDataState(EF_ABSENT),
      mpOplRecords(nullptr), mpOpl5GRecords(nullptr), mpPnnRecords(nullptr) {
}

std::string EonsResolver::getEons(const std::string &regOperator, int lac, const std::string &simOperator) {
    if (getEonsOpState() == EONS_OP_PNN_OPL) {
        return getEonsUsingPnnOpl(regOperator, lac);
    } else if (getEonsOpState() == EONS_OP_PNN) {
        return getEonsUsingPnnIfRegInHplmn(regOperator, simOperator);
    } else {
        RilLogV("PNN/OPL data is not available");
    }

    return "";
}

std::string EonsResolver::getEonsForAvailableNetworks(const std::string &availOperator) {
    if (getEonsOpState() != EONS_OP_PNN_OPL) {
        RilLogV("PNN/OPL data is not available");
        return "";
    }

    const int invalidLac = -1;
    int pnnRecordNumber = getPnnRecordNumberFromOpl5G(availOperator, invalidLac, false);
    if (pnnRecordNumber == 0) {
        pnnRecordNumber = getPnnRecordNumberFromOpl(availOperator, invalidLac, false);
    }
    return getNameFromPnn(pnnRecordNumber);
}

EonsResolver &EonsResolver::createForPhoneId(int phoneId) {
    if (phoneId < 0 || phoneId >= SIM_COUNT) {
        phoneId = 0;
    }
    return sResolver[phoneId];
}

std::string EonsResolver::getEonsUsingPnnOpl(const std::string &regOperator, int lac) {
    int pnnRecordNumber = getPnnRecordNumberFromOpl5G(regOperator, lac, true);
    if (pnnRecordNumber == 0) {
        pnnRecordNumber = getPnnRecordNumberFromOpl(regOperator, lac, true);
    }
    return getNameFromPnn(pnnRecordNumber);
}

std::string EonsResolver::getEonsUsingPnnIfRegInHplmn(const std::string &regOperator, const std::string &simOperator) {
    if (regOperator.empty() || simOperator.empty() || simOperator.compare(regOperator) != 0) {
        return "";
    }
    return getNameFromPnn(1);
}

int EonsResolver::getPnnRecordNumberFromOpl5G(const std::string &plmn, int lac, bool useLac) {
    if (mpOpl5GRecords != nullptr) {
        return mpOpl5GRecords->getPnnRecordNumber(plmn, lac, useLac);
    }
    return 0;
}

int EonsResolver::getPnnRecordNumberFromOpl(const std::string &plmn, int lac, bool useLac) {
    if (mpOplRecords != nullptr) {
        return mpOplRecords->getPnnRecordNumber(plmn, lac, useLac);
    }
    return 0;
}

std::string EonsResolver::getNameFromPnn(int pnnRecordNumber) {
    if (mpPnnRecords != nullptr) {
        return mpPnnRecords->getNameFromPnnRecord(pnnRecordNumber);
    }
    return "";
}

EonsResolver EonsResolver::sResolver[SIM_COUNT];