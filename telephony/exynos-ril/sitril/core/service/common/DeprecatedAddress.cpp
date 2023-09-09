/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "DeprecatedAddress.h"
#include <cstring>

DeprecatedAddress::DeprecatedAddress(bool isIpv6, int state, int lifetime, char *ip)
{
    mIsIpv6 = isIpv6;
    mState = state;
    mLifetime = lifetime;
    memcpy(mAddr, ip, (isIpv6) ? MAX_IPV6_ADDR_LEN : MAX_IPV4_ADDR_LEN);
}
