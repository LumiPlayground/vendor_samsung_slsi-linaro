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
 * carrier.cpp
 *
 *  Created on: 2019. 9. 5.
 *      Author: sungwoo48.choi
 */
#include <sstream>
#include <stdlib.h>

#include "carriermanifest.h"
#include "rillog.h"
#include "librilutils/textutils.h"

std::string ToString(ManifestProperty property) {
    static std::map<ManifestProperty, std::string> manifestPropertyToStringMap = {
        {ManifestProperty::MCCMNC_TUPLE, "mccmnc"},
        //{ManifestProperty::IMSI, "imsi"},
        {ManifestProperty::SPN, "spn"},
        {ManifestProperty::PLMN, "plmn"},
        {ManifestProperty::GID1, "gid1"},
        {ManifestProperty::GID2, "gid2"},
        {ManifestProperty::PREFERRED_APN, "preferred_apn"},
        //{ManifestProperty::ICCID, "iccid"},
        {ManifestProperty::PRIVILEGE_ACCESS_RULE, "privelege_access_rule"},
    };
    auto it = manifestPropertyToStringMap.find(property);
    if (it == manifestPropertyToStringMap.end()) {
        return "";
    }
    return it->second;
}

std::string CarrierManifest::getProperty(ManifestProperty property) const {
   auto it = mManifestProperties.find(property);
   if (it == mManifestProperties.end()) {
       return "";
   }
   return it->second;
}

bool CarrierManifest::isValid() const {
    return !TextUtils::IsEmpty(mManifest);
}


std::string CarrierManifest::toString() const {
    std::stringstream ss;
    ss << "{";
    for (const auto& pair : mManifestProperties) {
        if (ToString(pair.first).empty()) continue;
        ss << ToString(pair.first) << "=" << pair.second << ",";
    };
    ss << "manifest=" << mManifest << "}";
    return ss.str();
}

std::vector<char> CarrierManifest::getEncodedManifest() const {
    std::vector<char> encoded;
    for (unsigned int i = 0; i < mManifest.size(); i += 2) {
        unsigned int c = strtol(mManifest.substr(i, 2).c_str(), nullptr, 16);
        encoded.push_back(c);
    }
    return encoded;
}
