/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _RIL_TYPES_H_
#define _RIL_TYPES_H_

#include <string>
#include "basedef.h"

typedef bool                BOOL;
typedef unsigned char       BYTE;
typedef unsigned char       UINT8;
typedef signed char         INT8;
typedef unsigned short      UINT16;
typedef short               INT16;
typedef unsigned int        UINT32;
typedef int                 INT32;
typedef int64_t             LONG;
typedef uint64_t            ULONG;
typedef wchar_t             WCHAR;

typedef unsigned int        UINT;
typedef unsigned int        DWORD;
typedef unsigned short      WORD;
typedef void *              HANDLE;

#ifndef FALSE
#define FALSE               false
#endif

#ifndef TRUE
#define TRUE                true
#endif

#ifndef NULL
#define NULL (0L)
#endif

#endif /*_RIL_TYPES_H_*/
