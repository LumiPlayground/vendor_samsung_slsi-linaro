/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MOCK_PDPCONTEXT_H__
#define __MOCK_PDPCONTEXT_H__

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <pdpcontext.h>

// This cause bad-free after test done
class MockPdpContext : public PdpContext {
    public:
        MockPdpContext() : PdpContext(0) {
        }
        MockPdpContext(int cid) : PdpContext(cid) {
        }
        virtual ~MockPdpContext() = default;
};

#endif /*__MOCK_PDPCONTEXT_H__*/
