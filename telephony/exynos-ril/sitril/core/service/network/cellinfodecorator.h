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
 * cellinfodecorator.h
 *
 *  Created on: 2021. 10. 5.
 */

#ifndef __CELL_INFO_DECORATOR_H__
#define __CELL_INFO_DECORATOR_H__

#include "slsi/radio_v1_6.h"
#include <string>

class EonsResolver;

struct CellInfoDecorator {
    virtual ~CellInfoDecorator()=default;
    virtual RIL_CellInfo_V1_6 *decorate()=0;
};

/**
 * CellInfoBase
 */
struct CellInfoBase : public CellInfoDecorator {
    RIL_CellInfo_V1_6 *mCellinfo;
    CellInfoBase(RIL_CellInfo_V1_6 *cellinfo) : mCellinfo(cellinfo) {}
    RIL_CellInfo_V1_6 *decorate() { return mCellinfo; }
};

/**
 * CellInfoDecoratorBase
 */
struct CellInfoDecoratorBase : public CellInfoDecorator {
    CellInfoDecorator& mBase;
    CellInfoDecoratorBase(CellInfoDecorator& base) : mBase(base) {}
};

/**
 * SignalStrengthVisibleDecorator
 */
struct SignalStrengthVisibleDecorator: public CellInfoDecoratorBase {
    SignalStrengthVisibleDecorator(CellInfoDecorator& base) :
            CellInfoDecoratorBase(base) {
    }
    RIL_CellInfo_V1_6 *decorate();
};

/**
 * AlphaEnosDecorator
 */
struct AlphaEnosDecorator: public CellInfoDecoratorBase {
    std::string mSimNumeric;
    std::string mSpn;
    EonsResolver& mResolver;
    AlphaEnosDecorator(CellInfoDecorator& base, const std::string simNumeric,
            const std::string& spn, EonsResolver& resolver) :
            CellInfoDecoratorBase(base), mSimNumeric(simNumeric), mSpn(spn),
            mResolver(resolver) {
    }
    RIL_CellInfo_V1_6 *decorate();
    void decorate(RIL_CellInfoGsm_V1_5& info);
    void decorate(RIL_CellInfoWcdma_V1_5& info);
    void decorate(RIL_CellInfoTdscdma_V1_5& info);
    void decorate(RIL_CellInfoLte_V1_6& info);
    void decorate(RIL_CellInfoNr_V1_6& info);
    void fillOperatorName(RIL_CellIdentityOperatorNames& opname,
            const std::string& alphaLong, const std::string& alphaShort);
};

#endif /* __CELL_INFO_DECORATOR_H__ */
