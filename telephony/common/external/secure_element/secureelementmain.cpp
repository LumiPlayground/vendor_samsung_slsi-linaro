/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#define LOG_TAG "secure_element@1.2-service-uicc"
#include <android/hardware/secure_element/1.2/ISecureElement.h>
#include <hidl/LegacySupport.h>
#include <log/log.h>
#include <cutils/properties.h>

#include "secureelement.h"
#include "types.h"

using android::hardware::secure_element::V1_2::ISecureElement;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::OK;
using android::sp;
using android::status_t;

#define BUF_LEN         (8)
#define MAX_THREAD_NUM  (21)    // base 2 + max open channel number is 19

sp<SecureElementImpl> SEService[SIM_COUNT] = {};

int main() {
    ALOGD("SE HAL Service 1.2 is starting.");

    char buf[BUF_LEN] = {};

    configureRpcThreadpool(MAX_THREAD_NUM, true);   // because max open channel number is 19

    for (int i = 0; i < SIM_COUNT; i++) {
        snprintf(buf, sizeof(buf), "SIM%d", i + 1);
        ALOGD("RegisterAsService for %s", buf);
        SEService[i] = new SecureElementImpl(i);
        status_t status = SEService[i]->registerAsService(buf);
        if (status != OK) {
            ALOGD("Not registerAsService for SE HAL Iface (%d).", status);
        }
    }
    ALOGD("Secure Element Service is ready");
    joinRpcThreadpool();
    return 1;
}
