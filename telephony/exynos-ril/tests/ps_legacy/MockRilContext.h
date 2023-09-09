/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MOCK_RIL_CONTEXT_H__
#define __MOCK_RIL_CONTEXT_H__

#include <gmock/gmock.h>
#include <rilapplication.h>
#include <rilcontextwrapper.h>

class ModemData;

class MockRilContext : public RilContextWrapper {
    public:
        MockRilContext() : RilContextWrapper(nullptr, nullptr) {}
};
#endif /*__MOCK_RIL_CONTEXT_H__*/
