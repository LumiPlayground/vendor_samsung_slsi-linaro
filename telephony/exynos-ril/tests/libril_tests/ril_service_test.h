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
 * ril_service_test.h
 *
 *  Created on: 2021. 7. 15.
 */

#ifndef __RIL_SERVICE_TEST_H__
#define __RIL_SERVICE_TEST_H__

#include "vendorfunctionexecutor.h"
#include <stdio.h>

#if 0
#define dlog(x, ...) printf(x"\n", ##__VA_ARGS__)
#else
#define dlog(x, ...)
#endif

#define NUM_OF_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

namespace libriltest {
int GetRandomSerialNumber();
}

extern void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
        void *response, size_t responselen);

#endif /* __RIL_SERVICE_TEST_H__ */
