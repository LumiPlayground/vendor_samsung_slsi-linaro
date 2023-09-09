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
 * miscserviceproxy.h
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#ifndef __MISC_SERVICE_PROXY_H__
#define __MISC_SERVICE_PROXY_H__

#include <base/types.h>

class MiscService;
class NetworkService;
class Service;

class MiscServiceProxy {
private:
    MiscService *mService;

public:
    MiscServiceProxy(Service *service);
    ~MiscServiceProxy();
public:
    bool HasService() { return mService != NULL; }
    NetworkService *FindNetworkService();
};

#endif /* __MISC_SERVICE_PROXY_H__ */
