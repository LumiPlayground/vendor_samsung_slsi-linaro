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
 * carrierconfigloader.cpp
 *
 *  Created on: 2019. 9. 3.
 *      Author: sungwoo48.choi
 */
#include "carrierconfigloader.h"
#include "rillog.h"
#include "librilutils/textutils.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sstream>

CarrierConfigLoader *CarrierConfigLoader::getInstance(CarrierConfigProvider *provider) {
    return NullCarrierConfigLoader::getInstance(provider);
}

NullCarrierConfigLoader *NullCarrierConfigLoader::instance = NULL;
NullCarrierConfigLoader *NullCarrierConfigLoader::getInstance(CarrierConfigProvider *provider) {
    if (instance == nullptr) {
        instance = new NullCarrierConfigLoader(provider);
    }
    return instance;
}

bool NullCarrierConfigLoader::load(bool reload = false) {
    mLoaded = true;
    if (reload && mProvider != nullptr) {
        mLoaded = mProvider->reset();
    }
    return mLoaded;
}

bool NullCarrierConfigLoader::isLoaded() {
    return mLoaded;
}

bool NullCarrierConfigLoader::setConfigDirectory(const string &path) {
    return mProvider->setConfigDirectory(path);
}

string NullCarrierConfigLoader::getConfigDirectory() {
    return "";
}

void NullCarrierConfigLoader::dumpstate() const {}
