/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "memlogAgent.h"
#include "MemlogServiceManager.h"

#include "memlogservice.h"
#include <unistd.h>
#include <stdio.h>
#include <string>

#define SERVICE_NAME    "memlog"

static void Memlog_OnRequest(int type, int id, void *data, unsigned int datalen);

static MemlogServiceManager sMemlogServiceManager;
static Memlog_ServiceFunctions sMemlogServiceFunction = { Memlog_OnRequest };

int MemlogAgent_Init() {
    if (sMemlogServiceManager.init()) {
        sMemlogServiceManager.registerService("memlog", &sMemlogServiceFunction);
    }
    return 0;
}

/**************************************************************************************/
/**************************************************************************************/
// types of handler
static void OnCommandHandler(int id, void *data, unsigned int datalen);

/* examples */
//sMemlogServiceManager.notifyCallback(TYPE_COMMAND, COMMAND_STOP_MEMLOG, &ret, sizeof(int));
//sMemlogServiceManager.notifyCallback(TYPE_RAW, COMMAND_NOTIFY_MEMLOG_LOG, data, datalen);

void OnCommandHandler(int id, void *data, unsigned int datalen) {
    ALOGD("%s id %d", __FUNCTION__, id);
    switch (id) {
        case COMMAND_ENABLE_FILE:
            EnableFile(true, data, datalen);
            break;
        case COMMAND_DISABLE_FILE:
            EnableFile(false, data, datalen);
            break;
        case COMMAND_LEVEL_EMERG:
        case COMMAND_LEVEL_ERR:
        case COMMAND_LEVEL_CAUTION:
        case COMMAND_LEVEL_NOTICE:
        case COMMAND_LEVEL_INFO:
        case COMMAND_LEVEL_DEBUG:
            SetLevel(id - COMMAND_LEVEL_BASE, data, datalen);
            break;
        case COMMAND_REFRESH_FILE_LIST:
            scanCtrlList();
            break;
        case COMMAND_SAVE_DUMPSTATE:
            saveDumpstate();
            break;
        default:
            ALOGE("%s: invalid cmd! [%d]", __func__, id);
            break;
    }
}

void addList(bool add, bool enabled, uint8_t level, uint8_t type, std::string name)
{
    file_packet packet;
    memset(&packet, 0, sizeof(file_packet));
    if (add) {
        packet.enabled = enabled;
        packet.level = level;
        packet.type = type;
    }
    strncpy(packet.name, name.c_str(), name.size() >= 250 - 1 ? 250 - 1 : name.size());
    packet.name[250 - 1] = 0;
    packet.length = 4 + strlen(packet.name);
    //ALOGI("%x %x %x %x", ((uint8_t *)&packet)[0], ((uint8_t *)&packet)[1], ((uint8_t *)&packet)[2], ((uint8_t *)&packet)[3]);
    sMemlogServiceManager.notifyCallback(TYPE_RAW, add ? COMMAND_ADD_LIST : COMMAND_REMOVE_LIST, &packet, sizeof(uint8_t) * 4 + name.size());
}

void addList(bool enabled, uint8_t level, uint8_t type, std::string name)
{
    addList(true, enabled, level, type, name);
}

void removeList(std::string name)
{
    addList(false,0,0,0,name);
}

// Entry function from HIDL server
static void Memlog_OnRequest(int type, int id, void *data, unsigned int datalen) {
    ALOGD("Memlog_OnRequest type=%d id=%d, datalen=%u", type, id, datalen);

    if (type == TYPE_COMMAND) {
        OnCommandHandler(id, data, datalen);
    }
    else {
        ALOGE("unsupported type. type=%d", type);
    }
}
