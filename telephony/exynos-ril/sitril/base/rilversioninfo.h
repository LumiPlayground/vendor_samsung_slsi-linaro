/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __RIL_VERSION_INFO_H__
#define __RIL_VERSION_INFO_H__

// Old version Android has 4096 bytes, recent one has 5120 bytes
// Just set to old version length for backward compatibility
#define LOGGER_ENTRY_MAX_LEN (4 * 1024)

#include <string>
#define RIL_VENDOR_RADIO_BUILD_INFO "vendor.ril.build.info"
#define PROPERTY_MAX_LENGTH 92

class RILVersionInfo
{
public:
    RILVersionInfo();
    virtual ~RILVersionInfo();

public:
    static const char* getString(bool includeBuildInfo);
    static std::string getBuildId();
    static void saveBuildId2Poperty(std::string BuildId);

private:
    static char mVersion[LOGGER_ENTRY_MAX_LEN];
};

#endif  //__RIL_VERSION_INFO_H__
