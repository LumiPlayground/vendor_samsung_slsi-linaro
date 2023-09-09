/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _BASE_DEF_H_
#define _BASE_DEF_H_

#include "secure_c.h"

#define MIN(a, b)       ((a) > (b) ? (b) : (a))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))

#define    DECLARE_MODULE_TAG()    static const char *TAG;
#define    IMPLEMENT_MODULE_TAG(theClassName, theTag)    const char *theClassName::TAG = #theTag;
#define    IMPLEMENT_TAG(theTag)    const char *TAG = #theTag;

#endif /*_BASE_DEF_H_*/
