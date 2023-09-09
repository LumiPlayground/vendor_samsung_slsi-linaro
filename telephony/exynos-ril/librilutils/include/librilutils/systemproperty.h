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
 * systemproperty.h
 *
 *  Created on: 2015. 7. 18.
 *  Modified on: 2020. 11. 3.
 */

#ifndef __SYSTEM_PROPERTY_H__
#define __SYSTEM_PROPERTY_H__

#include <string>

enum class PropertyOsalType {
    PROPERTY_OS_AOSP,
};

class SystemProperty {
  public:
    static std::string Get(const std::string &name);
    static std::string Get(const std::string &name, const std::string &defValue);
    static int GetInt(const std::string &name);
    static int GetInt(const std::string &name, int defValue);
    static void Set(const std::string &name, const std::string &value);
    static void Set(const std::string &name, int value);

  private:
    static PropertyOsalType GetPropertyOsalType();
};

#endif /* __SYSTEM_PROPERTY_H__ */
