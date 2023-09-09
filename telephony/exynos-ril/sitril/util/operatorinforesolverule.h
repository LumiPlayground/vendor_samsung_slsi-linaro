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
 * operatorinforesolverule.h
 *
 *  Created on: 2021. 10. 1.
 *      Author: sungwoo48.choi
 */
#ifndef __OPERATOR_INFO_RESOLVE_RULE_H__
#define __OPERATOR_INFO_RESOLVE_RULE_H__

#include "operatorinforesolver.h"

/**
 * OperatorNumericRule
 */
struct OperatorNumericRule : public OperatorInfoResolveRule {
    std::string mNumeric;
    OperatorNumericRule(OperatorInfoResolveRule *next, const std::string& numeric);
    OperatorInfo resolve() override;
};

#endif /* __OPERATOR_INFO_RESOLVE_RULE_H__ */

/**
 * SpnRule
 */
struct SpnRule : public OperatorInfoResolveRule {
    std::string mNetworkNumeric;
    std::string mSimNumeric;
    std::string mSpn;
    SpnRule(OperatorInfoResolveRule *next, const std::string& networkNumeric,
            const std::string simNumeric, const std::string& spn);
    OperatorInfo resolve() override;
};

/**
 * EonsDBRule
 */
struct EonsDBRule : public OperatorInfoResolveRule {
    std::string mNumeric;
    int mcc;
    int mnc;
    std::string mGid1;
    EonsDBRule(OperatorInfoResolveRule *next, const std::string& numeric, const std::string& gid1);
    OperatorInfo resolve() override;
};

/**
 * PnnOplRule
 */
class EonsResolver;
struct PnnOplRule : public OperatorInfoResolveRule {
    std::string mNumeric;
    EonsResolver& mResolver;
    PnnOplRule(OperatorInfoResolveRule *next, const std::string& numeric, EonsResolver& resolver);
    OperatorInfo resolve() override;
};

/**
 * RegNetPnnOplRule
 */
struct RegNetPnnOplRule : public OperatorInfoResolveRule {
    std::string mNumeric;
    EonsResolver& mResolver;
    std::string mSimNumeric;
    int mLac;
    RegNetPnnOplRule(OperatorInfoResolveRule *next, const std::string& numeric, EonsResolver& resolver,
            const std::string& simNumeric, int lac);
    OperatorInfo resolve() override;
};


/**
 * Ts25Rule
 */
struct Ts25Rule : public OperatorInfoResolveRule {
    std::string mNumeric;
    Ts25Rule(OperatorInfoResolveRule *next, const std::string& numeric);
    OperatorInfo resolve() override;
};

/**
 * SpecialRuleForBrazil
 */
struct SpecialRuleForBrazil : public Ts25Rule {
    std::string mNetworkNumeric;
    std::string mSimNumeric;
    SpecialRuleForBrazil(OperatorInfoResolveRule *next, const std::string& networkNumeric,
            const std::string simNumeric);
    OperatorInfo resolve() override;
};


/**
 * VendorCustomRule
 */
struct VendorCustomRule : public OperatorInfoResolveRule {
    std::string mNetworkNumeric;
    std::string mSimNumeric;
    VendorCustomRule(OperatorInfoResolveRule *next, const std::string& networkNumeric,
            const std::string simNumeric);
    OperatorInfo resolve() override;
};

/**
 * EonsCashRule
 */
struct EonsCashRule : public OperatorInfoResolveRule {
    std::string mNumeric;
    std::string mIccId;
    EonsCashRule(OperatorInfoResolveRule *next, const std::string& numeric, const std::string& iccId);
    OperatorInfo resolve() override;
};

/**
 * NitzRule
 */
struct NitzRule : public OperatorInfoResolveRule {
    std::string mNumeric;
    std::string mNitzLongAlpha;
    std::string mNitzShortAlpha;
    NitzRule(OperatorInfoResolveRule *next, const std::string& numeric,
            const std::string& longAlpha, const std::string& shortAlpha);
    OperatorInfo resolve() override;
};

/**
 * RegNetSpnRule
 */
struct RegNetSpnRule : public OperatorInfoResolveRule {
    std::string mNetworkNumeric;
    std::string mSimNumeric;
    std::string mSpn;
    bool mIsNitzAvailable;
    bool mIsRegHome;
    RegNetSpnRule(OperatorInfoResolveRule *next, const std::string& networkNumeric,
            const std::string& simNumeric, const std::string& spn,
            bool isNitzAvailable, bool isRegHome);
    OperatorInfo resolve() override;
};
