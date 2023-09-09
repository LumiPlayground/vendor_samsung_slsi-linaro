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
 * protocoljsonlogger.h
 *
 *  Created on: 2022. 03. 31.
 *      Author: p1.bansal
 */

#pragma once

#include <base/types.h>

#define MAX_JSON_LOG_FORMAT_LEN 150
#define MAX_JSON_LOG_PRINT_LEN  150


class ProtocolJsonLogger {
public:
    static void LogJsonRespData(UINT rcmId, const char* jsonString);
    static void LogJsonIndData(UINT rcmId, const char* jsonString);
    static void LogJsonReqData(UINT rcmId, const char* jsonString);
private:
    static void LogJsonData(char *format, const char* jsonString);
};
