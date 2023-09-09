/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <signal.h>
#include <sys/system_properties.h>

#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#define SOCKET_WLAND "socket_wland"

#include "common.h"
#include "wlbtlog.h"
#include "commandDispatcher.h"
#include "wlandServer.h"
#include "forkManager.h"

#define UNUSED(p)   ((void)(p))

#define BUILD_TYPE_PROPERTY "ro.build.type"
#define DEFAULT_BUILD_TYPE "user"

#define WIFI_START_MX_CMD "wifilog start mxlog"
#define WIFI_START_UDI_CMD "wifilog start udilog"
#define WIFI_START_ALL_CMD "wifilog start all"
#define WIFI_STOP_MX_CMD "wifilog stop mxlog"
#define WIFI_STOP_UDI_CMD "wifilog stop udilog"
#define WIFI_STOP_ALL_CMD "wifilog stop all"
#define WIFI_SAVE_HIHP4_CMD "wifilog hip4 hip4"
#define BT_START_GENERAL_CMD "btlog start general"
#define BT_START_AUDIO_CMD "btlog start audio"
#define BT_START_CUSTOM_CMD "btlog start custom"
#define BT_STOP_CMD "btlog stop"

static char buildtype[PROP_VALUE_MAX + 1];

void register_command() {
    commandDispatcher::registerCommand(WIFI_START_MX_CMD, {WlbtLog::start_mxlog});
    commandDispatcher::registerCommand(WIFI_START_UDI_CMD, {WlbtLog::start_udilog});
    commandDispatcher::registerCommand(WIFI_START_ALL_CMD, {WlbtLog::start_mxlog, WlbtLog::start_udilog});
    commandDispatcher::registerCommand(WIFI_STOP_MX_CMD, {WlbtLog::stop_mxlog});
    commandDispatcher::registerCommand(WIFI_STOP_UDI_CMD, {WlbtLog::stop_udilog});
    commandDispatcher::registerCommand(WIFI_STOP_ALL_CMD, {WlbtLog::stop_mxlog, WlbtLog::stop_udilog});
    commandDispatcher::registerCommand(WIFI_SAVE_HIHP4_CMD, {WlbtLog::start_hip4_profile});

    commandDispatcher::registerCommand(BT_START_GENERAL_CMD, {WlbtLog::start_bt_normal_log, WlbtLog::start_udilog});
    commandDispatcher::registerCommand(BT_START_AUDIO_CMD, {WlbtLog::start_bt_audio_log, WlbtLog::start_udilog});
    commandDispatcher::registerCommand(BT_START_CUSTOM_CMD, {WlbtLog::start_bt_custom_log, WlbtLog::start_udilog});
    commandDispatcher::registerCommand(BT_STOP_CMD, {WlbtLog::stop_bt_log, WlbtLog::stop_udilog});
}

bool serverUserHandler(char* buf) {
    bool ret = false;
    if (commandDispatcher::parserBuffer(buf)) {
        ret = commandDispatcher::runCommand();
    }
    return ret;
}

int32_t main(void) {
    memset(buildtype, 0, sizeof(buildtype));
    /**
    * Read build type from ro.build.type
    */
    if (__system_property_get(BUILD_TYPE_PROPERTY, buildtype) == 0) {
        strcpy(buildtype, DEFAULT_BUILD_TYPE);
    }
    ALOGD("%s = %s", BUILD_TYPE_PROPERTY, buildtype);

    ForkManager::start_signal_handler();

    Common::LoadConfiguration();
    register_command();

    if (!wlandServer::createServer(SOCKET_WLAND, serverUserHandler)) {
        return -1;
    }

    while (1) {
        if (!wlandServer::waitClientConnection()) {
            continue;
        }
        wlandServer::runService();
    }
    return 0;
}
