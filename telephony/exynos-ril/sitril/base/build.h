/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * build.h
 *
 *  Created on: 2015. 7. 20.
 */

#ifndef __BUILD_H__
#define __BUILD_H__

#include <string>

class Build
{
public:
    static std::string ID();
    static std::string Display();
    static std::string Product();
    static std::string Device();
    static std::string Board();
    static std::string Manufacturer();
    static std::string Brand();
    static std::string Model();
    static std::string Bootloader();
    static std::string Hardware();
    static std::string Serial();
    static std::string SupportedABIS();
    static std::string Supported32bitABIS();
    static std::string Supported64bitABIS();
    static std::string Type();
    static bool IS_ENG;
    static bool IS_USERDEBUG;
    static bool IS_USER;
};

#endif /* __BUILD_H__ */
