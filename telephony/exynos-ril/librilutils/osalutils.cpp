/*
 * Copyright (c) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <librilutils/osalutils.h>
#include <librilutils/aospsystemclockutil.h>

class SystemClockUtilFactory {
  public:
    static SystemClockUtilPtr CreateClockUtil(OsalUtilsOsType osType) {
        switch (osType) {
        case OsalUtilsOsType::OSAL_UTILS_OS_TYPE_AOSP:
            return std::make_shared<const AospSystemClockUtil>();
        default:
            return std::make_shared<const AospSystemClockUtil>();
        }
    }
};

int64_t OsalUtils::elapsedRealtime() {
    return GetInstance()->systemClockUtil->elapsedRealtime();
}

void OsalUtils::CreateInstance(OsalUtilsOsType osType) {
    instance.reset(new OsalUtils(osType));
}

const OsalUtils *OsalUtils::GetInstance() {
    if (instance == nullptr) {
        CreateInstance(GetOsalLogOsType());
    }
    return instance.get();
}

OsalUtilsOsType OsalUtils::GetOsalLogOsType() {
    //to do: deciding logic when supporting other OS
    return OsalUtilsOsType::OSAL_UTILS_OS_TYPE_AOSP;
}

OsalUtils::OsalUtils(OsalUtilsOsType osType) {
    systemClockUtil = SystemClockUtilFactory::CreateClockUtil(osType);
}

OsalUtilsPtr OsalUtils::instance = nullptr;