/*
 * Copyright (C) 2018 Samsung Electronics Co. LTD
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 *
 */
#ifndef RELEASE_SOURCE_INCLUDE_OFI_API_H_
#define RELEASE_SOURCE_INCLUDE_OFI_API_H_

#include <iostream>

#include "ofi_mm_memory-public.h"
#include "ofi_api-public.h"

#ifndef OFI_TST_PRINT
#define OFI_TST_PRINT printf
#endif

#ifndef OFI_INFO_PRINT
#define OFI_INFO_PRINT printf
#endif

#ifndef OFI_ERR_PRINT
#define OFI_ERR_PRINT printf
#endif

#ifndef OFI_DBG_PRINT
#define OFI_DBG_PRINT printf
#endif

#ifndef OFI_WARN_PRINT
#define OFI_WARN_PRINT printf
#endif

#ifndef OFI_INFO_PRINT_FORCE
#define OFI_INFO_PRINT_FORCE printf
#endif

#define DBG_PREFIX "[RELEASE_TEST]"

#define CHECK_AND_RETURN_ERR(cond, ret, message, ...)                          \
  do {                                                                         \
    if (cond) {                                                                \
      OFI_ERR_PRINT(DBG_PREFIX message, ##__VA_ARGS__);                        \
      return ret;                                                              \
    }                                                                          \
  } while (0)

#endif  // TOOLS_RELEASE_SOURCE_INCLUDE_OFI_API_H_