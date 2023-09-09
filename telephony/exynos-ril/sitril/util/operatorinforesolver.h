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
 * operatorinforesolver.h
 *
 *  Created on: 2021. 10. 1.
 *      Author: sungwoo48.choi
 */
#ifndef __OPERATOR_INFO_RESOLVER_H__
#define __OPERATOR_INFO_RESOLVER_H__

#include "operatorinfo.h"

/**
 * OperatorInfoResolveRule
 */
struct OperatorInfoResolveRule {
    OperatorInfoResolveRule *next;
    OperatorInfoResolveRule(OperatorInfoResolveRule *rule) : next(rule) {}
    virtual ~OperatorInfoResolveRule()=default;
    virtual OperatorInfo resolve()=0;
};

/**
 * OperatorInfoResolver
 * - implement a default rule
 */
class EonsResolver;
struct OperatorInfoResolver {
    OperatorInfo resolve(const std::string& networkNumeric,
            const std::string& simNumeric,
            const std::string& spn, EonsResolver& resolver);
};


/**
 * WifiOperatorInfoResolver
 */
struct WifiOperatorInfoResolver {
    OperatorInfo resolve(const std::string& simNumeric);
};

/**
 * RegOperatorInfoResolver
 */
struct RegisteredOperatorInfoResolver {
    OperatorInfo resolve(const NetworkOperator &netOp,
                         const SimOperator &simOp,
                         EonsResolver &resolver);
};

#endif /* __OPERATOR_INFO_RESOLVER_H__ */
