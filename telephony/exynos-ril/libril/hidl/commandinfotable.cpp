/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#define LOG_TAG "RILC-CommandInfo"
#include <utils/Log.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ril_service_legacy.h>
#include <radioconfig_service.h>
#include <slsi/radio_v1_5.h>
#include <librilutils/systemproperty.h>
#include "commandinfotable.h"

using namespace std;

static bool debug = (SystemProperty::GetInt("persist.vendor.config.vdbg", 0) != 0);

static android::CommandInfo s_commands[] = {
#include "ril_commands.h"
#include "radio/1.2/ril_commands.h"
#include "radio/1.3/ril_commands.h"
#include "radio/1.4/ril_commands.h"
#include "radio/1.5/ril_commands.h"
#include "radioconfig/ril_commands.h"
};

static android::UnsolResponseInfo s_unsolResponses[] = {
#include "ril_unsol_commands.h"
#include "radio/1.2/ril_unsol_commands.h"
#include "radio/1.4/ril_unsol_commands.h"
#include "radio/1.5/ril_unsol_commands.h"
#include "radioconfig/ril_unsol_commands.h"
};

CommandInfoTable s_instance;

CommandInfoTable::CommandInfoTable() {
    int simCount = 1;
    #if (SIM_COUNT >= 2)
    simCount = SIM_COUNT;
    #endif
    for (int i = 0; i < simCount; i++) {
        mCommandInfo.emplace_back();
        mUnsolRespInfo.emplace_back();
    }

    int32_t size = 0;
    size = (int32_t)NUM_ELEMS(s_commands);
    RLOGI("s_commands size: %d", size);
    updateCommandInfo(s_commands, size);

    size = (int32_t)NUM_ELEMS(s_unsolResponses);
    RLOGI("s_unsolResponses size: %d", size);
    updateUnsolResponseInfo(s_unsolResponses, size);
}

android::CommandInfo *CommandInfoTable::getCommandInfo(CommandInfoMap& commandInfo, int request) {
    bool found = false;
    auto iter = commandInfo.find(request);
    if (iter != commandInfo.end()) {
        found = true;
    } else {
        int decodeId = DECODE_REQUEST(request);
        if (DECODE_HAL(request) > HAL_VERSION_CODE(1,0)) {
            iter = commandInfo.find(decodeId);
            if (iter != commandInfo.end()) {
                found = true;
            }
        }
    }

    if (found) {
        auto p = &(iter->second);
        if (p != NULL) {
            RLOGI("%s found {request=%d halVer=%02X}", __FUNCTION__,
                    DECODE_REQUEST(request), DECODE_HAL(request));
            return p;
        }
    }

    RLOGI("%s not found(request=%d halVer=%02X)", __FUNCTION__,
            DECODE_REQUEST(request), DECODE_HAL(request));
    return NULL;
}

android::CommandInfo *CommandInfoTable::getCommandInfo(int request) {
    return getCommandInfo(mDefaultCommandInfo, request);
}

android::CommandInfo *CommandInfoTable::getCommandInfo(uint32_t slot, int request) {
    auto info = getCommandInfo(mCommandInfo[slot], request);
    if (info == nullptr) {
        info = getCommandInfo(request);
    }
    return info;
}

void CommandInfoTable::updateCommandInfo(android::CommandInfo *infos, size_t size) {
    if (infos != NULL) {
        for (size_t i = 0; i < size; i++) {
            updateCommandInfo(infos[i]);
            if (debug) {
                RLOGI("mapping: mCommandInfo[%d] = %zu", infos[i].requestNumber, i);
            }
        } // end for i ~
    }
}

void CommandInfoTable::updateCommandInfo(const android::CommandInfo& info) {
    updateCommandInfo(mDefaultCommandInfo, info);
}

void CommandInfoTable::updateCommandInfo(uint32_t slot, const android::CommandInfo& info) {
    updateCommandInfo(mCommandInfo[slot], info);
}

android::UnsolResponseInfo *CommandInfoTable::getUnsolRespInfo(UnsolResponseInfoMap& unsolRespInfo,
                                                             int unsolResponse) {
    bool found = false;
    auto iter = unsolRespInfo.find(unsolResponse);
    if (iter != unsolRespInfo.end()) {
        found = true;
    } else {
        int decodeId = DECODE_REQUEST(unsolResponse);
        if (DECODE_HAL(unsolResponse) > HAL_VERSION_CODE(1,0)) {
            iter = unsolRespInfo.find(decodeId);
            if (iter != unsolRespInfo.end()) {
                found = true;
            }
        }
    }

    if (found) {
        auto p = &(iter->second);
        if (p != NULL) {
            RLOGI("%s found {unsolResponse=%d halVer=%02X}", __FUNCTION__,
                    DECODE_REQUEST(unsolResponse), DECODE_HAL(unsolResponse));
            return p;
        }
    }
    RLOGI("%s not found(unsolResponse=%d halVer=%02X)", __FUNCTION__,
            DECODE_REQUEST(unsolResponse), DECODE_HAL(unsolResponse));
    return NULL;
}

android::UnsolResponseInfo *CommandInfoTable::getUnsolRespInfo(int unsolResponse) {
    return getUnsolRespInfo(mDefaultUnsolRespInfo, unsolResponse);
}

android::UnsolResponseInfo *CommandInfoTable::getUnsolRespInfo(uint32_t slot, int unsolResponse) {
    auto info = getUnsolRespInfo(mUnsolRespInfo[slot], unsolResponse);
    if (info == nullptr) {
        info = getUnsolRespInfo(unsolResponse);
    }
    return info;
}

void CommandInfoTable::updateUnsolResponseInfo(android::UnsolResponseInfo *infos, size_t size) {
    if (infos != NULL) {
        for (size_t i = 0; i < size; i++) {
            updateUnsolResponseInfo(infos[i]);
            if (debug) {
                RLOGI("mapping: mUnsolRespInfo[%d] = %zu", infos[i].requestNumber, i);
            }
        } // end for i ~
    }
}

void CommandInfoTable::updateUnsolResponseInfo(const android::UnsolResponseInfo& info) {
    updateUnsolResponseInfo(mDefaultUnsolRespInfo, info);
}

void CommandInfoTable::updateUnsolResponseInfo(uint32_t slot, const android::UnsolResponseInfo& info) {
    updateUnsolResponseInfo(mUnsolRespInfo[slot], info);
}

void CommandInfoTable::updateCommandInfo(CommandInfoMap& commandInfo,
        const android::CommandInfo& info) {
    commandInfo[info.requestNumber] = info;
}

void CommandInfoTable::updateUnsolResponseInfo(UnsolResponseInfoMap& unsolRespInfo,
        const android::UnsolResponseInfo& info) {
    unsolRespInfo[info.requestNumber] = info;
}

namespace android {
android::CommandInfo *getCommandInfo(int request) {
    return s_instance.getCommandInfo(request);
}

android::CommandInfo *getCommandInfo(uint32_t slot, int request) {
    return s_instance.getCommandInfo(slot, request);
}

void updateCommandInfo(android::CommandInfo *infos, size_t size) {
    s_instance.updateCommandInfo(infos, size);
}

void updateCommandInfo(const android::CommandInfo& info) {
    s_instance.updateCommandInfo(info);
}

void updateCommandInfo(uint32_t slot, const android::CommandInfo& info) {
    s_instance.updateCommandInfo(slot, info);
}

android::UnsolResponseInfo *getUnsolRespInfo(int unsolResponse) {
    return s_instance.getUnsolRespInfo(unsolResponse);
}

android::UnsolResponseInfo *getUnsolRespInfo(uint32_t slot, int unsolResponse) {
    return s_instance.getUnsolRespInfo(slot, unsolResponse);
}

void updateUnsolResponseInfo(android::UnsolResponseInfo *infos, size_t size) {
    s_instance.updateUnsolResponseInfo(infos, size);
}

void updateUnsolResponseInfo(const android::UnsolResponseInfo& info) {
    s_instance.updateUnsolResponseInfo(info);
}

void updateUnsolResponseInfo(uint32_t slot, const android::UnsolResponseInfo& info) {
    s_instance.updateUnsolResponseInfo(slot, info);
}
}