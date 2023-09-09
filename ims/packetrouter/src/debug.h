/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
/*
    Debug
*/
#ifndef _wfc_debug_h
#define _wfc_debug_h

#include <android/log.h>

#define WFC_ROUTER_LOG_TAG    "WPKTRT"

/* Use the per-tag properties "log.tag.<tagname>" */
#define __IS_VERBOSE        __android_log_is_loggable(ANDROID_LOG_VERBOSE, WFC_ROUTER_LOG_TAG, ANDROID_LOG_INFO)
#define __VERBOSE(fmt, ...) if (__IS_VERBOSE) __android_log_print(ANDROID_LOG_VERBOSE, WFC_ROUTER_LOG_TAG, fmt, ##__VA_ARGS__)
#define __IS_DBG            __android_log_is_loggable(ANDROID_LOG_DEBUG, WFC_ROUTER_LOG_TAG, ANDROID_LOG_INFO)
#define __DBG(fmt, ...)     if (__IS_DBG) __android_log_print(ANDROID_LOG_DEBUG, WFC_ROUTER_LOG_TAG, fmt, ##__VA_ARGS__)
#define __IS_INFO           __android_log_is_loggable(ANDROID_LOG_INFO, WFC_ROUTER_LOG_TAG, ANDROID_LOG_INFO)
#define __INFO(fmt, ...)    if (__IS_INFO) __android_log_print(ANDROID_LOG_INFO, WFC_ROUTER_LOG_TAG, fmt, ##__VA_ARGS__)
#define __WARN(fmt, ...)    __android_log_print(ANDROID_LOG_WARN, WFC_ROUTER_LOG_TAG, fmt, ##__VA_ARGS__)
#define __ERROR(fmt, ...)   __android_log_print(ANDROID_LOG_ERROR, WFC_ROUTER_LOG_TAG, fmt, ##__VA_ARGS__)
#define __FATAL(fmt, ...)   __android_log_print(ANDROID_LOG_FATAL, WFC_ROUTER_LOG_TAG, fmt, ##__VA_ARGS__)

#endif
