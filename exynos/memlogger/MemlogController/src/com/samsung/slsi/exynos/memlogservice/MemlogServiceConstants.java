/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.exynos.memlogservice;

public class MemlogServiceConstants {

    /* command type */
    public static final int TYPE_COMMAND = 0;
    public static final int TYPE_RAW = 1;

    /* command id */
    public static final int COMMAND_ENABLE_FILE = 0;
    public static final int COMMAND_DISABLE_FILE = 1;
    public static final int COMMAND_LEVEL_EMERG = 2;
    public static final int COMMAND_LEVEL_BASE = COMMAND_LEVEL_EMERG;
    public static final int COMMAND_LEVEL_ERR = 3;
    public static final int COMMAND_LEVEL_CAUTION = 4;
    public static final int COMMAND_LEVEL_NOTICE = 5;
    public static final int COMMAND_LEVEL_INFO = 6;
    public static final int COMMAND_LEVEL_DEBUG = 7;
    public static final int COMMAND_REFRESH_FILE_LIST = 8;
    public static final int COMMAND_ADD_LIST = 9;
    public static final int COMMAND_REMOVE_LIST = 10;
    public static final int COMMAND_SAVE_DUMPSTATE = 11;
}