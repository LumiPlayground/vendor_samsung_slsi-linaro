/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.exynos.memlogcontroller;
/**
 * Created by Sukwon Ryoo 2020-06-25
 */
public class MemlogControllerConstant {
    /* command id */
    public static final int COMMAND_ENABLE_FILE = 0;
    public static final int COMMAND_DISABLE_FILE = 1;
    public static final int COMMAND_LEVEL_BASE = 2;
    public static final int COMMAND_LEVEL_EMERG = COMMAND_LEVEL_BASE;
    public static final int COMMAND_LEVEL_ERR = 3;
    public static final int COMMAND_LEVEL_CAUTION = 4;
    public static final int COMMAND_LEVEL_NOTICE = 5;
    public static final int COMMAND_LEVEL_INFO = 6;
    public static final int COMMAND_LEVEL_DEBUG = 7;
    public static final int COMMAND_REFRESH_FILE_LIST = 8;

    public static final String[] objType = {
        "mask",
        "dump",
        "default",
        "array",
        "string",
        "direct",
        "file",
        "printf",
    };
}
