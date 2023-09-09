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
 * carrierconfigmanager.cpp
 *
 *  Created on: 2019. 9. 3.
 *      Author: sungwoo48.choi
 */

#include "carrierconfigmanager.h"
#include "rillog.h"
#include "librilutils/systemproperty.h"
#include "librilutils/textutils.h"
#include "types.h"

#include <string.h>

#include <sys/types.h>
#include <dirent.h>
#include <vector>

#define DEFAULT_CARRIER_CONFIG_DIR              "/vendor/firmware/carrierconfig"
#define PROPERTY_CARRIER_CONFIG_DIR             "vendor.radio.config.carrier_config_dir"
#define PROPERTY_PERSIST_CARRIER_CONFIG_DIR     "persist.vendor.radio.config.carrier_config_dir"

static bool VDBG = true;

IMPLEMENT_MODULE_TAG(CarrierConfigManager, CpCarrierConfigManager)

CarrierConfigManager::CarrierConfigManager() {
    mRevision = 0;
    mConfigDir = DEFAULT_CARRIER_CONFIG_DIR;
    mLoaded = false;
    mConfigLoader = NULL;
    mConfigProvider = NULL;
}

void CarrierConfigManager::Init() {
    mConfigProvider = CarrierConfigProvider::getInstance();
    if (mConfigProvider != NULL) {
        mConfigLoader = CarrierConfigLoader::getInstance(mConfigProvider);
    }

    if (mConfigLoader == NULL || mConfigProvider == NULL) {
        RilLogW("%s %s  failed to intialize CarrierConfigLoader or CarrierConfigProvider", TAG, __FUNCTION__);
        return ;
    }

    RilLogV("%s %s", TAG, __FUNCTION__);
    string dir = SystemProperty::Get(PROPERTY_CARRIER_CONFIG_DIR, "");
    string persistDir = SystemProperty::Get(PROPERTY_PERSIST_CARRIER_CONFIG_DIR, "");
    string defaultDir = DEFAULT_CARRIER_CONFIG_DIR;
    if (VDBG) {
        RilLogV("%s %s defDir:%s configDir:%s dir:%s persistDir:%s", TAG, __FUNCTION__,
                DEFAULT_CARRIER_CONFIG_DIR, defaultDir.c_str(), dir.c_str(), persistDir.c_str());
    }

    if ((dir.empty() || (mLoaded = Load(dir)) != true) &&
            (persistDir.empty() || (mLoaded = Load(persistDir)) != true) &&
            (defaultDir.empty() || (mLoaded = Load(defaultDir)) != true)) {
        RilLogW("%s %s  failed to load carrier config", TAG, __FUNCTION__);
        ListDir(dir);
        ListDir(persistDir);
        ListDir(defaultDir);
    }
}

bool CarrierConfigManager::Load(bool reload/* = false*/) {
    RilLogV("%s %s", TAG, __FUNCTION__);
    int ret = false;
    if (mConfigLoader != NULL) {
        if (!reload) {
            ret = mConfigLoader->load();
        }
        else {
            mLoaded = false;
            if ((ret = mConfigLoader->load(reload))) {
                mLoaded = true;
            }
        }
    }

    return ret;
}

bool CarrierConfigManager::Load(string &configDir) {
    RilLogV("%s %s", TAG, __FUNCTION__);
    if (mConfigLoader != NULL) {
        if (mConfigLoader->setConfigDirectory(configDir)) {
            mConfigDir = configDir;
            if (mConfigLoader->load()) {
                mLoaded = true;

                if (VDBG) {
                    mConfigLoader->dumpstate();
                }
                return true;
            }
        }
    }

    return false;
}

CarrierManifest CarrierConfigManager::GetCarrierManifest(CarrierManifest carrier_info) {
    CarrierManifest ret;
    if (mConfigProvider != NULL) {
        ret = mConfigProvider->find(carrier_info);
        RilLogV("%s %s IsValidCarrierManifest: %s", TAG, __FUNCTION__,
                IsValidCarrierManifest(ret) ? "valid" : "invalid");
        if (!ret.isValid()) {
            RilLogW("%s %s failed to get a valid manifest for %s",
                    TAG, __FUNCTION__, carrier_info.toString().c_str());
        }
    }
    else {
        RilLogW("%s %s CarrierConfigProvider is null", TAG, __FUNCTION__);
    }
    return ret;
}

CarrierConfigManager *CarrierConfigManager::instance = NULL;
CarrierConfigManager *CarrierConfigManager::GetInstance() {
    if (instance == NULL) {
        instance = new CarrierConfigManager();
        if (instance != NULL) {
            instance->Init();
        }
    }
    return instance;
}

bool CarrierConfigManager::IsValidCarrierManifest(CarrierManifest &carrier) {
    RilLogV("CarrierManifest: %s", carrier.toString().c_str());
    return carrier.isValid();
}

bool CarrierConfigManager::IsValidCarrierManifest(CarrierManifest *carrier) {
    return (carrier == NULL) ? false : IsValidCarrierManifest(*carrier);
}

void CarrierConfigManager::ListDir(const string &dirPath) {
    struct dirent *file = NULL;
    DIR *dir = NULL;
    if ((dir = opendir(dirPath.c_str())) == NULL) {
        RilLogW("%s %s Failed to open %s", TAG, __FUNCTION__,  dirPath.c_str());
        return;
    }
    std::vector<string> files;
    while ((file = readdir(dir)) != NULL) {
        files.push_back(file->d_name);
    }
    RilLogW("%s %s %s:", TAG, __FUNCTION__, dirPath.c_str());
    for (auto &data : files) {
        RilLogW("%s ", data.c_str());
    }
    closedir(dir);
}
