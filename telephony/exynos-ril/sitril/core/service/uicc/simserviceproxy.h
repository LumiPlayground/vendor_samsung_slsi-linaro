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
  * simserviceproxy.h
  *
  *  Created on: 2020. 8. 11.
  *  Author: saesaem.lee
  */

#ifndef __SIM_SERVICE_PROXY_H__
#define __SIM_SERVICE_PROXY_H__

class SimService;
class Service;

class SimServiceProxy {
private:
    SimService *mService;

public:
    SimServiceProxy(Service *service);
    ~SimServiceProxy();

public:
    bool HasService() { return mService != NULL; }
    int GetCurrentUiccEnableState();
    int GetReqUiccEnableState();
    void SetCurrentUiccEnableState(int enableState);
    void SetReqUiccEnableState(int enableState);
};

#endif /* __SIM_SERVICE_PROXY_H__ */

