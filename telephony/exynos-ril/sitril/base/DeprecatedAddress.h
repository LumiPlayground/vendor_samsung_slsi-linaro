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
 * DeprecatedAddress.h
 *
 */

#ifndef __DEPRECATED_ADDRESS_H__
#define __DEPRECATED_ADDRESS_H__

#include <telephony/ril.h>

class DeprecatedAddress {
private:
    bool mIsIpv6;
    int mState;
    int mLifetime;
    char mAddr[MAX_IPV6_ADDR_LEN];

public:
    DeprecatedAddress() {};
    DeprecatedAddress(bool, int, int, char *);
    ~DeprecatedAddress() {};

    bool isIpv6() { return mIsIpv6; }
    int getState() { return mState; }
    int getLifetime() { return mLifetime; }
    const char * getAddr() { return (char *)mAddr; }
    void setLifetime(int lifetime) { mLifetime = lifetime; }
    void setState(int state) { mState = state; }

};
#endif // __DEPRECATED_ADDRESS_H__
