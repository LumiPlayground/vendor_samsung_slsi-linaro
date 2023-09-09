/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "commandDispatcher.h"
#include "jsonInterface.h"

#include <utils/Log.h>

#include <sstream>
#include <utility>


const std::list<std::string> commandDispatcher::cmdRule = {"name", "exec", "option", "dir", "data" };
const std::list<std::string> commandDispatcher::logNamePostfix = { "_mxlog", "_udilog" };
std::map<std::string, std::string> commandDispatcher::parserdCmd;

std::vector<std::list<std::function<bool(const char* prefix, const char* data)>>>
commandDispatcher::logHandlers;
std::list<std::map<std::string, std::string>> commandDispatcher::logOptions;

bool commandDispatcher::parserBuffer(char* buf) {
    jsonInterface json;

    bool is_success = json.parserJsonString(buf);
    if (!is_success) {
        return false;
    }

    parserdCmd.clear();
    for (std::string cmd : cmdRule) {
        parserdCmd[cmd] = json.getStringValue(cmd);
    }

    return true;
}

std::map<std::string, std::string> commandDispatcher::parserOptions(std::string options) {
    std::stringstream ss(options);
    std::string option;
    std::list<std::string> optionList;
    std::map<std::string, std::string> map;

    while (std::getline(ss, option, ' ')) {
        optionList.push_back(option);
    }

    for (std::string cmd : cmdRule) {
        map.insert(std::make_pair(cmd, optionList.front()));
        optionList.pop_front();
        if (!optionList.size()) {
            break;
        }
    }
    return map;
}

void commandDispatcher::registerCommand(std::string commandString,
    std::list<std::function<bool(const char* prefix, const char* data)>> handlers) {
    std::map<std::string, std::string> map;

    if (handlers.size() == 0 || commandString.length() == 0) {
        ALOGD("commandDispatcher : registerCommand fail\n");
        return;
    }

    map = parserOptions(commandString);
    logOptions.push_back(map);
    logHandlers.push_back(handlers);
}

int commandDispatcher::findHandlerIdx(std::list<std::map<std::string, std::string>> logOptions) {
    std::list<std::string> keySet;
    bool found = 0;
    int idx = 0;

    for (auto const& x : parserdCmd) {
        if (x.first != "dir" && x.first != "data") {
            keySet.push_back(x.first);
        }
    }

    for (std::map<std::string, std::string> cmd : logOptions) {
        found = 1;
        for (auto const& key : keySet) {
            if (parserdCmd[key] != cmd[key]) {
                found = 0;
                break;
            }
        }
        if (found) {
            return idx;
        }
        idx++;
    }
    return -1;
}

std::string commandDispatcher::makeDirValue(std::list<std::string>::const_iterator iter) {
    if (parserdCmd["name"] == "wifilog" &&
        parserdCmd["option"] != "all") {
        return parserdCmd["dir"];
    }
    return parserdCmd["dir"] + *iter;
}

bool commandDispatcher::runCommand() {
    std::list<std::map<std::string, std::string>> options;
    std::string logName = "";
    bool ret = false, isFailed = false;

    int idx = findHandlerIdx(logOptions);
    if (idx == -1) {
        ALOGD("commandDispatcher : Could not find handler");
        return false;
    }

    std::list<std::string>::const_iterator iter = logNamePostfix.begin();
    std::list<std::function<bool(const char* prefix, const char* data)>>::iterator handlerIter;

    for (handlerIter = logHandlers[idx].begin(); handlerIter != logHandlers[idx].end(); handlerIter++) {
        ret = (*handlerIter)(makeDirValue(iter).c_str(), parserdCmd["data"].c_str());
        if (ret == false) {
            isFailed = true;
        }
        iter++;
    }
    return !isFailed;
}
