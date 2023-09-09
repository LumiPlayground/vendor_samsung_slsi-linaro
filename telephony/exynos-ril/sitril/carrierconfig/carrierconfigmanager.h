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
 * carrierconfigmanager.h
 *
 *  Created on: 2019. 9. 3.
 *      Author: sungwoo48.choi
 */

#ifndef __CARRIER_CONFIG_MANAGER_H__
#define __CARRIER_CONFIG_MANAGER_H__

#include "carrierconfigloader.h"
#include "rildef.h"

class CarrierConfigManager {
    DECLARE_MODULE_TAG()
private:
    void ListDir(const string& dirPath);

    int mRevision;
    string mConfigDir;
    bool mLoaded;
    CarrierConfigLoader *mConfigLoader;
    CarrierConfigProvider *mConfigProvider;
public:
    CarrierConfigManager();

public:
    void Init();
    bool Load(bool reload = false);
    bool Load(string &configDir);
    bool IsLoaded() { return mLoaded; }
    const char *GetCurrentConfigDir() { return mConfigDir.c_str(); }
    CarrierManifest GetCarrierManifest(CarrierManifest carrier_info);

protected:

public:

private:
    static CarrierConfigManager *instance;
public:
    static CarrierConfigManager *GetInstance();
    static bool IsValidCarrierManifest(CarrierManifest &carrier);
    static bool IsValidCarrierManifest(CarrierManifest *carrier);
};

#endif /* __CARRIER_CONFIG_MANAGER_H__ */
