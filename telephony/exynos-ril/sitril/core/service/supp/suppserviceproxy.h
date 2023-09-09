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
 * suppserviceproxy.h
 */

#ifndef __SUPP_SERVICE_PROXY_H__
#define __SUPP_SERVICE_PROXY_H__

class Service;
class SupplementaryService;

class SuppServiceProxy {
private:
    SupplementaryService* mService;
public:
    SuppServiceProxy(Service* service);
    ~SuppServiceProxy();
public:
    bool HasService() { return mService != nullptr; }
    int GetValidErrors(int errorCode);
    bool IsOperatorUsingUnknownServiceClass();
    ClirInfo *GetClirInfo();
    bool IsUssdUserInitiated();
    void SetUssdUserInitiated(bool ussdUserInitiated);
};

#endif /* __SUPP_SERVICE_PROXY_H__ */
