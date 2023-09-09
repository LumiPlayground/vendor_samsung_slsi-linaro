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
 * carrier.h
 *
 *  Created on: 2019. 9. 3.
 *      Author: sungwoo48.choi
 */

#ifndef __CARRIER_MANIFEST_H__
#define __CARRIER_MANIFEST_H__

#include <map>
#include <string>
#include <vector>

enum class ManifestProperty {
  MCCMNC_TUPLE,
  IMSI,
  SPN,
  PLMN,
  GID1,
  GID2,
  PREFERRED_APN,
  ICCID,
  PRIVILEGE_ACCESS_RULE,
};

std::string ToString(ManifestProperty property);

class CarrierManifest {
public:
    CarrierManifest() {}
    CarrierManifest(std::initializer_list<std::pair<const ManifestProperty,
                    std::string>> properties)
            : mManifestProperties(properties) {}
public:
    void setProperty(ManifestProperty property, const std::string& value) {
        mManifestProperties[property] = value; }
    void setManifest(const std::string& manifest) { this->mManifest = manifest; };
    std::string getProperty(ManifestProperty property) const;
    const std::string& getManifest() const { return mManifest; }
    std::vector<char> getEncodedManifest() const;
    bool isValid() const;
    std::string toString() const;
private:
    std::map<ManifestProperty, std::string> mManifestProperties;
    std::string mManifest;
};

#endif // __CARRIER_MANIFEST_H__
