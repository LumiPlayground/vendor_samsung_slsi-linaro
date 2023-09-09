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
#define LOG_TAG     "VCD"
#include <utils/Log.h>

extern bool VDBG;

#ifdef LOGE
#undef LOGE
#endif
#define LOGE(format, ...) ALOGE(format, ##__VA_ARGS__)

#ifdef LOGD
#undef LOGD
#endif
#define LOGD(format, ...) ALOGD(format, ##__VA_ARGS__)

#ifdef LOGV
#undef LOGV
#endif
#define LOGV(format, ...) if (VDBG) ALOGD(format, ##__VA_ARGS__)

#endif /* __LOG_H__ */
