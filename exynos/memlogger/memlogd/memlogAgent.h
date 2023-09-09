/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MEMLOG_AGENT_H__
#define __MEMLOG_AGENT_H__

#include "memlogd_main.h"

int MemlogAgent_Init();
void addList(bool level, uint8_t enabled, uint8_t type, std::string name);
void removeList(std::string name);
#pragma pack(push, 1)
struct file_packet {
    uint8_t length;
    bool enabled;
    uint8_t level;
    uint8_t type;
    char name[250];
};
#pragma pack(pop)
enum {
    COMMAND_ENABLE_FILE,
    COMMAND_DISABLE_FILE,
    COMMAND_LEVEL_BASE,
    COMMAND_LEVEL_EMERG = COMMAND_LEVEL_BASE,
    COMMAND_LEVEL_ERR,
    COMMAND_LEVEL_CAUTION,
    COMMAND_LEVEL_NOTICE,
    COMMAND_LEVEL_INFO,
    COMMAND_LEVEL_DEBUG,
    COMMAND_REFRESH_FILE_LIST,
    COMMAND_ADD_LIST,
    COMMAND_REMOVE_LIST,
    COMMAND_SAVE_DUMPSTATE,
};

#endif /* __MEMLOG_AGENT_H__ */
