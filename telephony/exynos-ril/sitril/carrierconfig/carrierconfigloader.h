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
 * carrierconfigloader.h
 *
 *  Created on: 2019. 9. 3.
 *      Author: sungwoo48.choi
 */

#ifndef __CARRIER_CONFIG_LOADER_H__
#define __CARRIER_CONFIG_LOADER_H__

#include "carrierconfigprovider.h"

class CarrierConfigProvider;

// Base class for loading carrier configurations. Primary responsibility is to initialize an
// instance of CarrierConfigProvider for use by CarrierConfigManager class.
class CarrierConfigLoader {

public:
    virtual ~CarrierConfigLoader() {}
    virtual bool load(bool reload = false) = 0;
    virtual bool isLoaded() = 0;
    virtual bool setConfigDirectory(const std::string &path) = 0;
    virtual std::string getConfigDirectory() = 0;
    virtual void dumpstate() const = 0;
    static CarrierConfigLoader *getInstance(CarrierConfigProvider *provider);
};

// CarrierConfigLoader implementation for CarrierConfigProviders that don't require parsing other
// files/data to be loaded (for instance SqlCarrierConfigProvider).
class NullCarrierConfigLoader : public CarrierConfigLoader {
public:
    bool load(bool reload);
    bool isLoaded();
    bool setConfigDirectory(const std::string &path);
    std::string getConfigDirectory();
    void dumpstate() const;
    static NullCarrierConfigLoader *getInstance(CarrierConfigProvider *provider);
private:
    NullCarrierConfigLoader(CarrierConfigProvider *provider) : mLoaded(true), mProvider(provider) {}
    bool mLoaded;
    std::string mConfigDir;
    CarrierConfigProvider *mProvider;
    static NullCarrierConfigLoader *instance;
};

#endif /* __CARRIER_CONFIG_LOADER_H__ */
