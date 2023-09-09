/* copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __RADIOCONFIG_V1_3_H__
#define __RADIOCONFIG_V1_3_H__

#include <string>
#include <slsi/radioconfig_v1_2.h>

/**
 * Contains the device capabilities with respect to the Radio HAL.
 */
typedef struct {
    bool modemReducedFeatureSet1;
} RIL_HalDeviceCapabilities;

/**
 * Gets the available Radio Hal capabilities on the current device.
 *
 * This is called once per device boot up.
 *
 * @param serial Serial number of request
 *
 * Response callback is
 * IRadioConfigResponse.getHalDeviceCapabilitiesResponse()
 */
#define RIL_REQUEST_GET_HAL_DEVICE_CAPABILITIES 220

#endif // __RADIOCONFIG_V1_3_H__
