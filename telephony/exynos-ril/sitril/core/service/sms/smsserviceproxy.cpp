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
#include "smsservice.h"
#include "smsserviceproxy.h"

SmsServiceProxy::SmsServiceProxy(Service *service)
        : mService(NULL)
{
    //mService should be not NULL
    mService = SmsService::AsService(service);
}

SmsServiceProxy::~SmsServiceProxy()
{
    mService = NULL;
}

void SmsServiceProxy::SetSmscAddress(const char *sca, unsigned int scaLen)
{
    if (!HasService()) {
        return;
    }

    if (scaLen > MAX_GSM_SMS_SERVICE_CENTER_ADDR) {
        return;
    }
    memcpy(mService->m_sca, sca, scaLen);
}

void SmsServiceProxy::SetSmscAddressLength(unsigned int scaLen)
{
    if (!HasService()) {
        return;
    }
    mService->m_scaLen = scaLen;
}

void SmsServiceProxy::SetSmsClass(int smsClass)
{
    if (!HasService()) {
        return;
    }
    mService->m_smsClass = smsClass;
}

char *SmsServiceProxy::GetSmscAddress()
{
    if (!HasService()) {
        return NULL;
    }
    return mService->m_sca;
}

int SmsServiceProxy::GetSmscAddressLength()
{
    if (!HasService()) {
        return 0;
    }
    return mService->m_scaLen;
}

int SmsServiceProxy::GetSmsClass()
{
    if (!HasService()) {
        return -1;
    }
    return mService->m_smsClass;
}
