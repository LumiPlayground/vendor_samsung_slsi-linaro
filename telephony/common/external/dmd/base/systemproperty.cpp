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
 * systemproperty.cpp
 *
 *  Created on: 2015. 7. 18.
 *  Modified on: 2021. 9. 9.
 */
#include "systemproperty.h"
#include "textutils.h"
#include <cutils/properties.h>

using namespace std;

#define MAX_SIZE 126

string SystemProperty::Get(const string& name)
{
    return SystemProperty::Get(name, "");
}

string SystemProperty::Get(const string& name, const string& defValue)
{
    char buf[MAX_SIZE] = {0, };
    property_get(name.c_str(), buf, defValue.c_str());
    return string(buf);
}

int SystemProperty::GetInt(const string& name)
{
    return SystemProperty::GetInt(name, INT_MAX);
}

int SystemProperty::GetInt(const string& name, int defValue)
{
    string ret = SystemProperty::Get(name);
    if (TextUtils::IsEmpty(ret)) {
        return defValue;
    }
    return TextUtils::ParseInt(ret);
}

void SystemProperty::Set(const string& name, const string& value)
{
    if (name.length() > 0) {
        property_set(name.c_str(), value.c_str());
    }
}

void SystemProperty::Set(const string& name, int value)
{
    SystemProperty::Set(name, std::to_string(value));
}
