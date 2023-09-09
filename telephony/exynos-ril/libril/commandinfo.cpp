/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#define LOG_TAG "RILC"

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

#include <map>
using namespace std;

namespace android {

static CommandInfo s_commands[] = {
#include "ril_commands.h"
#include "radio/1.2/ril_commands.h"
#include "radio/1.3/ril_commands.h"
#include "radio/1.4/ril_commands.h"
#include "radio/1.5/ril_commands.h"
#include "radioconfig/ril_commands.h"
};

static UnsolResponseInfo s_unsolResponses[] = {
#include "ril_unsol_commands.h"
#include "radio/1.2/ril_unsol_commands.h"
#include "radio/1.4/ril_unsol_commands.h"
#include "radio/1.5/ril_unsol_commands.h"
#include "radioconfig/ril_unsol_commands.h"
};

static bool debug = (SystemProperty::GetInt("persist.vendor.config.vdbg", 0) != 0);

class CommandInfoTable {
private:
    map<int, CommandInfo *> mCommandInfo;
    map<int, UnsolResponseInfo *> mUnsolRespInfo;
public:
    CommandInfoTable();
public:
    CommandInfo *getCommandInfo(int request);
    void updateCommandInfo(CommandInfo *infos, size_t size);
    void updateCommandInfo(CommandInfo *info) {
        updateCommandInfo(info, 1);
    }
    UnsolResponseInfo *getUnsolRespInfo(int unsolResponse);
    void updateUnsolResponseInfo(UnsolResponseInfo *infos, size_t size);
    void updateUnsolResponseInfo(UnsolResponseInfo *info) {
        updateUnsolResponseInfo(info, 1);
    }
};
static CommandInfoTable s_instance;

CommandInfoTable::CommandInfoTable() {
    int32_t size = 0;
    size = (int32_t)NUM_ELEMS(s_commands);
    RLOGI("s_commands size: %d", size);
    updateCommandInfo(s_commands, size);

    size = (int32_t)NUM_ELEMS(s_unsolResponses);
    RLOGI("s_unsolResponses size: %d", size);
    updateUnsolResponseInfo(s_unsolResponses, size);
}

CommandInfo *CommandInfoTable::getCommandInfo(int request) {
    bool found = false;
    map<int, CommandInfo *>::iterator iter = mCommandInfo.find(request);
    if (iter != mCommandInfo.end()) {
        found = true;
    }
    else {
        int decodeId = DECODE_REQUEST(request);
        if (DECODE_HAL(request) > HAL_VERSION_CODE(1,0)) {
            iter = mCommandInfo.find(decodeId);
            if (iter != mCommandInfo.end()) {
                found = true;
            }
        }
    }

    if (found) {
        CommandInfo *p = iter->second;
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

void CommandInfoTable::updateCommandInfo(CommandInfo *infos, size_t size) {
    if (infos != NULL) {
        for (size_t i = 0; i < size; i++) {
            mCommandInfo[infos[i].requestNumber] = &infos[i];
            if (debug) {
                RLOGI("mapping: mCommandInfo[%d] = %zu", infos[i].requestNumber, i);
            }
        } // end for i ~
    }
}

UnsolResponseInfo *CommandInfoTable::getUnsolRespInfo(int unsolResponse) {
    bool found = false;
    map<int, UnsolResponseInfo *>::iterator iter = mUnsolRespInfo.find(unsolResponse);
    if (iter != mUnsolRespInfo.end()) {
        found = true;
    }
    else {
        int decodeId = DECODE_REQUEST(unsolResponse);
        if (DECODE_HAL(unsolResponse) > HAL_VERSION_CODE(1,0)) {
            iter = mUnsolRespInfo.find(decodeId);
            if (iter != mUnsolRespInfo.end()) {
                found = true;
            }
        }
    }

    if (found) {
        UnsolResponseInfo *p = iter->second;
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

void CommandInfoTable::updateUnsolResponseInfo(UnsolResponseInfo *infos, size_t size) {
    if (infos != NULL) {
        for (size_t i = 0; i < size; i++) {
            mUnsolRespInfo[infos[i].requestNumber] = &infos[i];
            if (debug) {
                RLOGI("mapping: mUnsolRespInfo[%d] = %zu", infos[i].requestNumber, i);
            }
        } // end for i ~
    }
}

CommandInfo *getCommandInfo(int request) {
    return s_instance.getCommandInfo(request);
}

void updateCommandInfo(CommandInfo *infos, size_t size) {
    s_instance.updateCommandInfo(infos, size);
}

UnsolResponseInfo *getUnsolRespInfo(int unsolResponse) {
    return s_instance.getUnsolRespInfo(unsolResponse);
}

void updateUnsolResponseInfo(UnsolResponseInfo *infos, size_t size) {
    s_instance.updateUnsolResponseInfo(infos, size);
}
}
