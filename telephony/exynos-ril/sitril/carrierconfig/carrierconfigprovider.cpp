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
 * carrierconfigprovider.cpp
 *
 *  Created on: 2019. 9. 5.
 *      Author: sungwoo48.choi
 */
#include "carrierconfigprovider.h"
#include "testcarrierconfigprovider.h"

IMPLEMENT_MODULE_TAG(CarrierConfigProvider, CarrierConfigProvider)

CarrierConfigProvider *CarrierConfigProvider::getInstance()
{
    // Factory function. Add logic to select between provider classes if any new ones are added.
    return TestCarrierConfigProvider::getInstance();
}
