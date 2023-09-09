/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "rilversioninfo.h"
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <base/rillog.h>
#include "version_info.h"
#include <librilutils/systemproperty.h>

#include <fstream>
#include <iostream>
#include <sstream>

static const char* VersionString = VERSION_STRING;

static const char VersionInfo[] = {
    VERSION_MAJOR_INIT,
    '.',
    VERSION_MINOR_INIT,
    '\0'
};

char RILVersionInfo::mVersion[LOGGER_ENTRY_MAX_LEN];

RILVersionInfo::RILVersionInfo() {
}

RILVersionInfo::~RILVersionInfo() {
}

const char* RILVersionInfo::getString(bool /* includeBuildInfo */) {
    memset(mVersion, 0, sizeof(mVersion));
    snprintf(mVersion, sizeof(mVersion) - 1, "%s V%s %s", VersionString, VersionInfo,
            RILVersionInfo::getBuildId().c_str());
    return (const char*)mVersion;
}

std::string RILVersionInfo::getBuildId() {
    std::ifstream ifs("/vendor/etc/version_info_exynos_ril.txt");
    std::string version = "Unknown";
    if (ifs) {
        std::ostringstream oss;
        oss << ifs.rdbuf();
        version = oss.str();
    }

    RILVersionInfo::saveBuildId2Poperty(version);
    return version;
}

void RILVersionInfo::saveBuildId2Poperty(std::string BuildId){
    if(BuildId.size() > PROPERTY_MAX_LENGTH) {
        BuildId.resize(PROPERTY_MAX_LENGTH);
    }
    // When the output format of version_info_defs.h.in is changed,
    // the standard of the find() function may need to be changed.
    const unsigned long position = BuildId.find(")", 0);
    if(position != std::string::npos) {
        BuildId = BuildId.substr(0, position + 1);
        SystemProperty::Set(RIL_VENDOR_RADIO_BUILD_INFO, BuildId);
    }
}