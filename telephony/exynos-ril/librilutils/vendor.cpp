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
 * vendor.cpp
 *
 *  Created on: 2020. 11. 3.
 *      Author: sungwoo48.choi
 */

#include <librilutils/vendor.h>
#include <sstream>

namespace vendor {
namespace radio {
int getCurrentHalVersionCode() {
#ifdef __RADIO_HAL_AIDL__
    return HAL_VERSION_CODE(2, 0);
#elif __RADIO_HAL_V1_6__
    return HAL_VERSION_CODE(1, 6);
#else
    return HAL_VERSION_CODE(1, 5);
#endif
}

int getMaxHalVersionCode() {
#ifdef __RADIO_HAL_AIDL__
    return HAL_VERSION_CODE(2, 0);
#elif __RADIO_HAL_V1_6__
    return HAL_VERSION_CODE(1, 6);
#else
    return HAL_VERSION_CODE(1, 5);
#endif
}

std::string halVersionCodeToString(int halVersionCode) {
    std::stringstream ss;
    ss << "HAL_VERSION_CODE(" << HAL_MAJOR_VERSION(halVersionCode);
    ss << "," << HAL_MINOR_VERSION(halVersionCode) << ")";
    return ss.str();
}

} // radio
} // vendor
