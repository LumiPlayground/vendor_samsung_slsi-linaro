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
 * operatorinforesolverule.cpp
 *
 *  Created on: 2021. 10. 1.
 *      Author: sungwoo48.choi
 */
#include "mcctable.h"
#include "eonsResolver.h"
#include "operatorinforesolverule.h"
#include "operatortable.h"
#include "networkutils.h"
#include "rillog.h"
#include "ts25table.h"
#include <librilutils/textutils.h>

/**
 * OperatorNumericRule
 */
OperatorNumericRule::OperatorNumericRule(OperatorInfoResolveRule *next, const std::string& numeric)
        : OperatorInfoResolveRule(next), mNumeric(numeric) {
}

OperatorInfo OperatorNumericRule::resolve() {
    if (TextUtils::IsDigitsOnly(mNumeric) &&
        (mNumeric.length() == 5 || mNumeric.length() == 6)) {
        RilLogV("[OperatorNumericRule] for numeric %s", mNumeric.c_str());
        return OperatorInfo::getOperatorInfo(mNumeric, mNumeric, mNumeric);
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * SpnRule
 */
SpnRule::SpnRule(OperatorInfoResolveRule *next, const std::string& networkNumeric,
        const std::string simNumeric, const std::string& spn) : OperatorInfoResolveRule(next),
                mNetworkNumeric(networkNumeric), mSimNumeric(simNumeric), mSpn(spn) {
}

OperatorInfo SpnRule::resolve() {
    if (TextUtils::IsDigitsOnly(mSimNumeric) && TextUtils::Equals(mNetworkNumeric, mSimNumeric) &&
        MccTable::isUsingSpnForAvailablePlmnSrch(mNetworkNumeric.c_str()) &&
        !TextUtils::IsEmpty(mSpn)) {
        RilLogV("[SpnRule] for simNumeric %s", mSimNumeric.c_str());
        return OperatorInfo::getOperatorInfo(mSpn, mSpn, mSimNumeric);
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * EonsDBRule
 */
EonsDBRule::EonsDBRule(OperatorInfoResolveRule *next, const std::string& numeric, const std::string& gid1)
        : OperatorInfoResolveRule(next), mNumeric(numeric), mGid1(gid1) {
    mcc = NetworkUtils::fetchMcc(numeric.c_str());
    mnc = NetworkUtils::fetchMncNoEncoding(numeric.c_str());
}

OperatorInfo EonsDBRule::resolve() {
    OperatorNameProvider *provider = OperatorNameProvider::GetInstance();
    if (provider == nullptr) {
        return OperatorInfoResolveRule::resolve();
    }

    const OperatorContentValue *opname = provider->Find(mcc, mnc, mGid1);
    if (opname == nullptr && mGid1 != GID_DEFAULT_VALUE_ANY) {
        RilLogV("[EonsDBRule] Try to find a record using gid=* instead of %d/%d/%s", mcc, mnc, mGid1.c_str());
        opname = provider->Find(mcc, mnc, GID_DEFAULT_VALUE_ANY);
    }

    if (opname && opname->AreBothLongPlmnShortPlmnAvailable()) {
        RilLogV("[EonsDBRule] for mcc=%d, mnc=%d, gid1=%s", mcc, mnc, mGid1.c_str());
        return OperatorInfo::getOperatorInfo(opname->GetLongPlmn(), opname->GetShortPlmn(), mNumeric);
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * PnnOplRule
 */
PnnOplRule::PnnOplRule(OperatorInfoResolveRule *next, const std::string& numeric,
        EonsResolver& resolver)
        : OperatorInfoResolveRule(next), mNumeric(numeric), mResolver(resolver) {
}

OperatorInfo PnnOplRule::resolve() {
    if (TextUtils::IsDigitsOnly(mNumeric) &&
        MccTable::IsCarrierUsePnnOplForEons(mNumeric.c_str())) {
        std::string alpha = mResolver.getEonsForAvailableNetworks(mNumeric);
        if (!TextUtils::IsEmpty(alpha)) {
            RilLogV("[PnnOplRule] for numeric %s", mNumeric.c_str());
            return OperatorInfo::getOperatorInfo(alpha, alpha, mNumeric);
        }
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * RegNetPnnOplRule
 */
RegNetPnnOplRule::RegNetPnnOplRule(OperatorInfoResolveRule *next, const std::string& numeric,
        EonsResolver& resolver, const std::string& simNumeric, int lac)
        : OperatorInfoResolveRule(next), mNumeric(numeric), mResolver(resolver),
          mSimNumeric(simNumeric), mLac(lac) {
}

OperatorInfo RegNetPnnOplRule::resolve() {
    if (TextUtils::IsDigitsOnly(mNumeric) &&
        MccTable::IsCarrierUsePnnOplForEons(mNumeric.c_str())) {
        std::string alpha = mResolver.getEons(mNumeric, mLac, mSimNumeric);
        if (!TextUtils::IsEmpty(alpha)) {
            RilLogV("[RegNetPnnOplRule] for numeric %s", mNumeric.c_str());
            return OperatorInfo::getOperatorInfo(alpha, alpha, mNumeric);
        }
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * Ts25Rule
 */
Ts25Rule::Ts25Rule(OperatorInfoResolveRule *next, const std::string& numeric)
        : OperatorInfoResolveRule(next), mNumeric(numeric) {
}

OperatorInfo Ts25Rule::resolve() {
    if (TextUtils::IsDigitsOnly(mNumeric) &&
        (mNumeric.length() == 5 || mNumeric.length() == 6)) {
        int mcc = std::stoi(mNumeric.substr(0, 3));
        int mnc = std::stoi(mNumeric.substr(3));
        TS25Table *ts25table = TS25Table::GetInstance();
        if (ts25table != NULL) {
            TS25Record record = ts25table->GetRecord(mcc, mnc);
            if (record.IsValid()) {
                RilLogV("[Ts25Rule] for numeric %s", mNumeric.c_str());
                return OperatorInfo::getOperatorInfo(record.ppcin, record.networkName, mNumeric);;
            }
        }
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * SpecialRuleForBrazil
 */
SpecialRuleForBrazil::SpecialRuleForBrazil(OperatorInfoResolveRule *next,
        const std::string& networkNumeric, const std::string simNumeric)
        : Ts25Rule(next, networkNumeric), mNetworkNumeric(networkNumeric),
        mSimNumeric(simNumeric) {
}

OperatorInfo SpecialRuleForBrazil::resolve() {
    const int MCC_BRAZIL = 724;
    if (TextUtils::Equals(mNetworkNumeric, mSimNumeric) &&
        TextUtils::IsDigitsOnly(mNumeric) &&
        (mNumeric.length() == 5 || mNumeric.length() == 6)) {
        int mcc = std::stoi(mNumeric.substr(0, 3));
        if (mcc == MCC_BRAZIL) {
            RilLogV("[SpecialRuleForBrazil] for numeric %s", mNumeric.c_str());
            return Ts25Rule::resolve();
        }
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * VendorCustomRule
 */
VendorCustomRule::VendorCustomRule(OperatorInfoResolveRule *next, const std::string& networkNumeric,
        const std::string simNumeric) : OperatorInfoResolveRule(next),
                mNetworkNumeric(networkNumeric), mSimNumeric(simNumeric) {
}

OperatorInfo VendorCustomRule::resolve() {
    OperatorContentValue oper = OperatorNameProvider::GetVendorCustomOperatorName(
        mSimNumeric, mNetworkNumeric);
    if (oper.AreBothLongPlmnShortPlmnAvailable()) {
        RilLogV("[VendorCustomRule] for numeric %s", mNetworkNumeric.c_str());
        return OperatorInfo::getOperatorInfo(oper.GetLongPlmn(), oper.GetShortPlmn(),
                mNetworkNumeric);
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * EonsCashRule
 */
EonsCashRule::EonsCashRule(OperatorInfoResolveRule *next,
        const std::string& numeric, const std::string& iccId)
        : OperatorInfoResolveRule(next), mNumeric(numeric), mIccId(iccId) {
}

OperatorInfo EonsCashRule::resolve() {
    OperatorNameProvider *provider = OperatorNameProvider::GetInstance();
    if (provider == nullptr) {
        return OperatorInfoResolveRule::resolve();
    }

    const OperatorContentValue *opname = provider->FindEons(mIccId, mNumeric);
    if (opname && opname->AreBothLongPlmnShortPlmnAvailable()) {
        RilLogV("[EonsCashRule] for numeric %s, iccId %s", mNumeric.c_str(), mIccId.c_str());
        return OperatorInfo::getOperatorInfo(opname->GetLongPlmn(), opname->GetShortPlmn(), mNumeric);
    }
    return OperatorInfoResolveRule::resolve();
}

/**
 * NitzRule
 */
NitzRule::NitzRule(OperatorInfoResolveRule *next,
            const std::string& numeric, const std::string& longAlpha, const std::string& shortAlpha)
            : OperatorInfoResolveRule(next), mNumeric(numeric), mNitzLongAlpha(longAlpha), mNitzShortAlpha(shortAlpha) {
}

OperatorInfo NitzRule::resolve() {
    if (!TextUtils::IsEmpty(mNumeric) &&
        !TextUtils::IsEmpty(mNitzLongAlpha) &&
        !TextUtils::IsEmpty(mNitzShortAlpha)) {
        RilLogV("[NitzRule] for numeric %s", mNumeric.c_str());
        return OperatorInfo::getOperatorInfo(mNitzLongAlpha, mNitzShortAlpha, mNumeric);
    }
    return OperatorInfoResolveRule::resolve();
}


/**
 * RegNetSpnRule
 */
RegNetSpnRule::RegNetSpnRule(OperatorInfoResolveRule *next, const std::string& networkNumeric,
        const std::string& simNumeric, const std::string& spn,
        bool isNitzAvailable, bool isRegHome)
        : OperatorInfoResolveRule(next), mNetworkNumeric(networkNumeric), mSimNumeric(simNumeric), mSpn(spn),
          mIsNitzAvailable(isNitzAvailable), mIsRegHome(isRegHome) {
}

OperatorInfo RegNetSpnRule::resolve() {
    // check whether SPN shall be used or not
    bool isUseSpnInRegHome = MccTable::isUsingSpnForOperatorNameInRegHome(mSimNumeric.c_str());

    // use SPN when sim plmn and network plmn is same for certian sim PLMN.
    if (isUseSpnInRegHome == true && MccTable::isNeedCheckPlmnMatcingForSpnUsing(mSimNumeric.c_str()) == true) {
        isUseSpnInRegHome = TextUtils::Equals(mSimNumeric, mNetworkNumeric);
    }

    // check NITZ has priority than SPN
    bool nitzHasPriorityThanSpn = false;
    if (mIsNitzAvailable && isUseSpnInRegHome ) {
        nitzHasPriorityThanSpn = MccTable::isNitzHasPriority(mSimNumeric.c_str(), mNetworkNumeric.c_str());
    }

    if (mIsRegHome &&
        (isUseSpnInRegHome) &&
        (!nitzHasPriorityThanSpn) &&
        !TextUtils::IsEmpty(mSpn)) {
        RilLogV("[RegNetSpnRule] for numeric %s", mNetworkNumeric.c_str());
        return OperatorInfo::getOperatorInfo(mSpn, mSpn, mNetworkNumeric);
    }

    return OperatorInfoResolveRule::resolve();
}
