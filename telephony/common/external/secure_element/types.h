/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef SE_TYPES_H
#define SE_TYPES_H

// Definitions
#define MAX_CHANNEL_NUM     (20)
#define MAX_COMMON_DATA     (256 * 2)
#define MAX_APDU_SIZE       (1024 * 64)       // 64K
#define MIN_APDU_SIZE       (4)
#define BASIC_CHANNEL       (0)
#undef SIM_COUNT
#define SIM_COUNT           (2)

// SitRil Secure Element Errors
enum {
    SITRIL_SE_ERROR_NONE,
    SITRIL_SE_ERROR_FAILURE,
    SITRIL_SE_ERROR_NOT_OPENED_LIB,
    SITRIL_SE_ERROR_LIB_LOAD_FAIL,
    SITRIL_SE_ERROR_NO_SYMBOL,
    SITRIL_SE_ERROR_NO_DEVICE,
    SITRIL_SE_ERROR_OPEN_FAIL,
    SITRIL_SE_ERROR_SEND_FAIL,
    SITRIL_SE_ERROR_INVALID_SOCKET_ID,
    SITRIL_SE_ERROR_INVALID_PARAM,
    SITRIL_SE_ERROR_REGISTRATION_FAIL,
    SITRIL_SE_ERROR_ALREADY_LIB_LOADED,
    SITRIL_SE_ERROR_ALREADY_REGISTERD,
    SITRIL_SE_ERROR_TIMEOUT,
    SITRIL_SE_ERROR_MAX
} SitRilSeError;

#endif // #ifndef SE_TYPES_H
