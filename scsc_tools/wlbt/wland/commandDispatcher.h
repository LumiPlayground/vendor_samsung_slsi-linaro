/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include <map>
#include <list>
#include <vector>
#include <functional>
#include <string>

class commandDispatcher {
 private:
    static std::map<std::string, std::string> parserdCmd;
    static const std::list<std::string> cmdRule;
    static const std::list<std::string> logNamePostfix;
    static std::vector<std::list<std::function<bool(const char*, const char*)>>> logHandlers;
    static std::list<std::map<std::string, std::string>> logOptions;

    static int findHandlerIdx(std::list< std::map<std::string, std::string>> logOptions);
    static std::map<std::string, std::string> parserOptions(std::string options);
    static std::string makeDirValue(std::list<std::string>::const_iterator);
 public:
    static bool parserBuffer(char* buf);
    static void registerCommand(std::string commandString,
        std::list<std::function<bool(const char*, const char*)>> handlers);
    static bool runCommand();
};

