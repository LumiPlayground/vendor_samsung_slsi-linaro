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

#ifndef __CARRIER_CONFIG_PROVIDER_H__
#define __CARRIER_CONFIG_PROVIDER_H__

#include "carriermanifest.h"
#include "rildef.h"

// Base class for class that selects which manifest to use. Selection is made based off variety
// of SIM data captured in CarrierManifest object.
class CarrierConfigProvider {
public:
    virtual ~CarrierConfigProvider() {}
    virtual void updateCarrier(CarrierManifest & /* carrier */){};
    virtual bool reset() = 0;
    virtual bool setConfigDirectory(const std::string &path) = 0;
    virtual CarrierManifest find(CarrierManifest &carrier_info) = 0;
    virtual void dumpstate() const = 0;
    static CarrierConfigProvider *getInstance();
protected:
    DECLARE_MODULE_TAG()
};

#endif /* __CARRIER_CONFIG_PROVIDER_H__ */
