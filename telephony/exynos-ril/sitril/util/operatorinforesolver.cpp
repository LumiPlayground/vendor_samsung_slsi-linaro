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
 * operatornameresolver.cpp
 *
 *  Created on: 2021. 10. 1.
 *      Author: sungwoo48.choi
 */
#include "constdef.h"
#include "operatorinforesolver.h"
#include "operatorinforesolverule.h"
#include "librilutils/textutils.h"

/**
 * OperatorInfoResolveRule
 */
OperatorInfo OperatorInfoResolveRule::resolve() {
    if (next) {
        return next->resolve();
    }
    return OperatorInfo();
}

/**
 * OperatorInfoResolver
 */
OperatorInfo OperatorInfoResolver::resolve(const std::string& networkNumeric,
        const std::string& simNumeric,
        const std::string& spn, EonsResolver& resolver) {
    // Priority of manual plmn name
    // 1. Vendor Custom
    // 2. when Network PLMN == SIM PLMN
    //      a. ts25 table for Brazil
    //      b. SPN for specific SIM PLMN
    // 3. PNN/OPL
    // 4. Eons DB
    // 5. plmn number

    OperatorNumericRule numericRule(nullptr, networkNumeric);
    EonsDBRule eonsRule(&numericRule, networkNumeric, GID_DEFAULT_VALUE_ANY);
    PnnOplRule pnnOplRule(&eonsRule, networkNumeric, resolver);
    SpnRule spnRule(&pnnOplRule, networkNumeric, simNumeric, spn);
    SpecialRuleForBrazil brazilRule(&spnRule, networkNumeric, simNumeric);
    VendorCustomRule vendorRule(&brazilRule, networkNumeric, simNumeric);

    OperatorInfoResolveRule& rule = vendorRule;
    return rule.resolve();
}

/**
 * WifiOperatorInfoResolver
 */
OperatorInfo WifiOperatorInfoResolver::resolve(const std::string& numeric) {
    // Priority of manual plmn name
    // 1. T25 table
    // 2. Eons DB

    EonsDBRule eonsDbRule(nullptr, numeric, GID_DEFAULT_VALUE_ANY);
    Ts25Rule t25DbRule(&eonsDbRule, numeric);

    OperatorInfoResolveRule& rule = t25DbRule;
    return rule.resolve();
}

/**
 * RegOperatorInfoResolver
 */
OperatorInfo RegisteredOperatorInfoResolver::resolve(const NetworkOperator &netOp,
                                                     const SimOperator &simOp,
                                                     EonsResolver &resolver) {
    const std::string netNumeric = netOp.GetNumeric();
    const std::string nitzLongAlpha = netOp.GetAlphaLong();
    const std::string nitzShortAlpha = netOp.GetAlphaShort();
    const std::string gid1 = netOp.GetGid1();
    const std::string simNumeric = simOp.GetNumeric();
    const std::string simSpn = simOp.GetSpn();
    const std::string iccId = simOp.GetIccid();
    int lac = netOp.GetLac();
    bool isRegHome = netOp.IsRegHome();

    // Priority of registered network operator name
    // 1. Vendor Custom
    // 2. SPN for specific SIM PLMN
    // 3. PNN/OPL
    // 4. NITZ
    // 5. Eons cash
    // 6. Eons DB
    // 7. plmn number

    OperatorNumericRule numericRule(nullptr, netNumeric);
    EonsDBRule eonsDBRule(&numericRule, netNumeric, gid1);
    EonsCashRule eonsCashRule(&eonsDBRule, netNumeric, iccId);
    NitzRule nitzRule(&eonsCashRule, netNumeric, nitzLongAlpha, nitzShortAlpha);
    RegNetPnnOplRule regNetPnnOplRule(&nitzRule, netNumeric, resolver, simNumeric, lac);
    RegNetSpnRule regSpnRule(&regNetPnnOplRule, netNumeric, simNumeric, simSpn,
            (!TextUtils::IsEmpty(nitzLongAlpha) && !TextUtils::IsEmpty(nitzShortAlpha)),
            isRegHome);
    VendorCustomRule vendorRule(&regSpnRule, netNumeric, simNumeric);

    OperatorInfoResolveRule& rule = vendorRule;
    return rule.resolve();
}
