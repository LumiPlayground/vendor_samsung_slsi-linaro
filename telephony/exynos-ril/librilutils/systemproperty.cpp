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
 */
#include <cutils/properties.h>
#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include <memory>

using std::make_shared;
using std::string;

#define MAX_SIZE 126

class PropertyOsalInterface {
  public:
    virtual ~PropertyOsalInterface() = default;
    virtual void GetProperty(const char *key, char *value, const char *defaultValue) = 0;
    virtual void SetProperty(const char *key, const char *value) = 0;
};

using PropertyOsalPtr = std::shared_ptr<PropertyOsalInterface>;

class PropertyOsalAosp : public PropertyOsalInterface {
    void GetProperty(const char *key, char *value, const char *defaultValue) override {
        property_get(key, value, defaultValue);
    }

    void SetProperty(const char *key, const char *value) override {
        property_set(key, value);
    }
};

class PropertyOsalFactory {
  public:
    static PropertyOsalPtr createProperty(PropertyOsalType osType) {
        switch (osType) {
        case PropertyOsalType::PROPERTY_OS_AOSP:
            return make_shared<PropertyOsalAosp>();
        }
        return nullptr;
    }
};

string SystemProperty::Get(const string &name) {
    return SystemProperty::Get(name, "");
}

string SystemProperty::Get(const string &name, const string &defValue) {
    char buf[MAX_SIZE] = {
        0,
    };
    PropertyOsalPtr property = PropertyOsalFactory::createProperty(GetPropertyOsalType());
    if (property != nullptr)
        property->GetProperty(name.c_str(), buf, defValue.c_str());
    return string(buf);
}

int SystemProperty::GetInt(const string &name) {
    return SystemProperty::GetInt(name, INT_MAX);
}

int SystemProperty::GetInt(const string &name, int defValue) {
    string ret = SystemProperty::Get(name);
    if (TextUtils::IsEmpty(ret)) {
        return defValue;
    }
    return TextUtils::ParseInt(ret);
}

void SystemProperty::Set(const string &name, const string &value) {
    if (name.length() > 0) {
        PropertyOsalPtr property = PropertyOsalFactory::createProperty(GetPropertyOsalType());
        if (property != nullptr)
            property->SetProperty(name.c_str(), value.c_str());
    }
}

void SystemProperty::Set(const string &name, int value) {
    SystemProperty::Set(name, std::to_string(value));
}

PropertyOsalType SystemProperty::GetPropertyOsalType() {
    //to do: deciding type when supporting other OS
    return PropertyOsalType::PROPERTY_OS_AOSP;
}