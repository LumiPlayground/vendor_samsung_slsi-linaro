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
 * smsserviceproxy.cpp
 *
 *  Created on: 2020. 8. 26.
 *      Author: sihwan14.lee
 */
#ifndef __SMS_SERVICE_PROXY_H__
#define __SMS_SERVICE_PROXY_H__

class SmsService;

class SmsServiceProxy {
private:
    SmsService *mService;
public:
    SmsServiceProxy(Service * service);
    ~SmsServiceProxy();
public:
    bool HasService() { return mService != NULL; }
    void SetSmscAddress(const char *sca, unsigned int scaLen);
    void SetSmscAddressLength(unsigned int scaLen);
    void SetSmsClass(int smsClass);

    char *GetSmscAddress();
    int GetSmscAddressLength();
    int GetSmsClass();
};

#endif /* __SMS_SERVICE_PROXY_H__ */
