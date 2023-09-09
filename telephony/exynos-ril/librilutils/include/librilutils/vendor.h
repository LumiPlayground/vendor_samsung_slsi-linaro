/* copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * vendor.h
 *
 *  Modified on: 2020. 11. 3.
 */

#ifndef __VENDOR_H__
#define __VENDOR_H__

#include <string>

/* HAL Version Code macro */
#define HAL_VERSION_CODE(m,n)   (((m) & 0xF) << 4 | ((n) & 0xF))
#define HAL_MAJOR_VERSION(n)    ((((n) >> 4) & 0xF))
#define HAL_MINOR_VERSION(n)    (((n) & 0xF))
#define HAL_VERSION_DEFAULT()   HAL_VERSION_CODE(1,0)
#define ENCODE_REQUEST(id,h)    (((id) & 0xFFFFFF) | (((h) & 0xFF) << 24))
#define DECODE_REQUEST(id)      ((id) & 0xFFFFFF)
#define DECODE_HAL(id)          (((id) >> 24) & 0xFF)

namespace vendor {
namespace radio {
int getCurrentHalVersionCode();
int getMaxHalVersionCode();
std::string halVersionCodeToString(int halVersionCode);
}
}

#endif // __VENDOR_H__
