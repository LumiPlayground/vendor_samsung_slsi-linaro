/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#pragma once

#include <ril_internal.h>
#include <map>
#include <vector>

class CommandInfoTable {
private:
    using CommandInfoMap = std::map<int, android::CommandInfo>;
    using UnsolResponseInfoMap = std::map<int, android::UnsolResponseInfo>;
    std::vector<CommandInfoMap> mCommandInfo;
    std::vector<UnsolResponseInfoMap> mUnsolRespInfo;
    CommandInfoMap mDefaultCommandInfo;
    UnsolResponseInfoMap mDefaultUnsolRespInfo;
public:
    CommandInfoTable();

    android::CommandInfo *getCommandInfo(int request);
    android::CommandInfo *getCommandInfo(uint32_t slot, int request);
    void updateCommandInfo(android::CommandInfo *infos, size_t size);
    void updateCommandInfo(const android::CommandInfo& info);
    void updateCommandInfo(uint32_t slot, const android::CommandInfo& info);
    android::UnsolResponseInfo *getUnsolRespInfo(int unsolResponse);
    android::UnsolResponseInfo *getUnsolRespInfo(uint32_t slot, int unsolResponse);
    void updateUnsolResponseInfo(android::UnsolResponseInfo *infos, size_t size);
    void updateUnsolResponseInfo(const android::UnsolResponseInfo& info);
    void updateUnsolResponseInfo(uint32_t slot, const android::UnsolResponseInfo& info);

private:
    void updateCommandInfo(CommandInfoMap& commandInfo, const android::CommandInfo& info);
    void updateUnsolResponseInfo(UnsolResponseInfoMap& unsolRespInfo, const android::UnsolResponseInfo& info);
    android::CommandInfo *getCommandInfo(CommandInfoMap& commandInfo, int request);
    android::UnsolResponseInfo *getUnsolRespInfo(UnsolResponseInfoMap& unsolRespInfo, int unsolResponse);
};