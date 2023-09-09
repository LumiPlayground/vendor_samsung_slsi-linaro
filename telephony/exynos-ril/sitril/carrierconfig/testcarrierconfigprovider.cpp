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

#include "testcarrierconfigprovider.h"
#include "rillog.h"
#include <fstream>
#include <sstream>
#include <string>
#include <librilutils/systemproperty.h>

#define CFG_INDEX_FILE_SUBPATH "/index"

TestCarrierConfigProvider::TestCarrierConfigProvider() {}

TestCarrierConfigProvider *TestCarrierConfigProvider::instance;
TestCarrierConfigProvider *TestCarrierConfigProvider::getInstance() {
    if (instance == nullptr) {
        instance = new TestCarrierConfigProvider();
    }
    return instance;
}

bool TestCarrierConfigProvider::reset() {
    return load(mConfigIndexPath);
}

bool TestCarrierConfigProvider::setConfigDirectory(const std::string &path) {
    mConfigIndexPath = path + CFG_INDEX_FILE_SUBPATH;
    RilLogV("%s %s config index path=%s", TAG, __FUNCTION__, mConfigIndexPath.c_str());
    return load(mConfigIndexPath);
}

CarrierManifest TestCarrierConfigProvider::find(CarrierManifest &carrier_info) {
    std::string mccmnc = carrier_info.getProperty(ManifestProperty::MCCMNC_TUPLE);

    std::string manifest = "";
    if (mConfigMap.find(mccmnc) != mConfigMap.end()) {
        manifest = mConfigMap.find(mccmnc)->second;
    }

    RilLogV("%s %s mccmnc=%s, manifest=%s", TAG, __FUNCTION__, mccmnc.c_str(), manifest.c_str());
    carrier_info.setManifest(manifest);
    return carrier_info;
}

void TestCarrierConfigProvider::dumpstate() const {
    RilLogV("%s %s", TAG, __FUNCTION__);
    //TODO dumpstate();
}

bool TestCarrierConfigProvider::load(const string& path) {
    RilLogV("%s %s", TAG, __FUNCTION__);
    std::ifstream file(path);

    if (!file.is_open()) {
        RilLogE("%s %s file open fail", TAG, __FUNCTION__);
        return false;
    }

    mConfigMap.clear();
    std::string buffer;
    while (std::getline(file, buffer)) {
        std::istringstream iss(buffer);
        string key, value;
        if (!(iss >> key >> value)) break;
        RilLogV("%s %s mccmnc=%s, manifests=%s", TAG, __FUNCTION__, key.c_str(), value.c_str());
        mConfigMap.insert(make_pair(key, value));
    }
    file.close();

    return true;
}
