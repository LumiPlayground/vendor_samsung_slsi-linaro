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
 * log.h
 *
 *  Created on: 2020. 2. 12.
 */

#ifndef __LOG_H__
#define __LOG_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG     "DMD"
#include <utils/Log.h>

extern bool VDBG;

#define PRINT_CONSOLE 0

#if PRINT_CONSOLE

#ifdef LOGE
#undef LOGE
#endif
#define LOGE(format, ...) printf("[ERROR]"format"\n", ##__VA_ARGS__)

#ifdef LOGW
#undef LOGW
#endif
#define LOGW(format, ...) printf("[WARN]"format"\n", ##__VA_ARGS__)

#ifdef LOGD
#undef LOGD
#endif
#define LOGD(format, ...) printf("[DEBUG]"format"\n", ##__VA_ARGS__)

#ifdef LOGV
#undef LOGV
#endif
#define LOGV(format, ...) if (VDBG) printf("[VERBOSE]"format"\n", ##__VA_ARGS__)

#else

#ifdef LOGE
#undef LOGE
#endif
#define LOGE(format, ...) ALOGE(format, ##__VA_ARGS__)

#ifdef LOGW
#undef LOGW
#endif
#define LOGW(format, ...) ALOGW(format, ##__VA_ARGS__)

#ifdef LOGD
#undef LOGD
#endif
#define LOGD(format, ...) ALOGD(format, ##__VA_ARGS__)

#ifdef LOGV
#undef LOGV
#endif
#define LOGV(format, ...) if (VDBG) ALOGD(format, ##__VA_ARGS__)

#endif

#endif /* __LOG_H__ */
