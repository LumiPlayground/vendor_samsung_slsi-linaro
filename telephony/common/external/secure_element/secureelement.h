/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENT_H
#define ANDROID_HARDWARE_SECURE_ELEMENT_V1_2_SECUREELEMENT_H

#include <android/hardware/secure_element/1.0/types.h>
#include <android/hardware/secure_element/1.2/ISecureElement.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "interface.h"

using ::android::hidl::base::V1_0::IBase;
using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::secure_element::V1_2::ISecureElement;
using ::android::hardware::secure_element::V1_0::SecureElementStatus;
using ::android::hardware::Void;
using ::android::sp;

using namespace ::android::hardware::secure_element;

typedef enum {
    E_STATUS_SUCCESS = 0,
    E_STATUS_FAIL,
    E_STATUS_SIM_ABSENT,
    E_STATUS_SECURE_EXCEPTION,
    E_STATUS_ILLEGAL_ARGUEMENT_EXCEPTION,
    E_STATUS_IO_EXCEPTION
} E_STATUS;

struct SecureElementImpl : public ISecureElement, public hidl_death_recipient {
    SecureElementImpl(unsigned int SocketId);
    ~SecureElementImpl();
    Return<void> init(const sp<V1_0::ISecureElementHalCallback>& clientCallback) override;
    Return<void> init_1_1(const sp<V1_1::ISecureElementHalCallback>& clientCallback) override;
    Return<void> getAtr(getAtr_cb _hidl_cb) override;
    Return<bool> isCardPresent() override;
    Return<void> transmit(const hidl_vec<uint8_t>& data, transmit_cb _hidl_cb) override;
    E_STATUS internalTransmit(
            TransmitApduChannelResponse *pResponse, TransmitLengthData *command);
    Return<void> openLogicalChannel(const hidl_vec<uint8_t>& aid, uint8_t p2,
            openLogicalChannel_cb _hidl_cb) override;
    Return<void> openBasicChannel(const hidl_vec<uint8_t>& aid, uint8_t p2,
            openBasicChannel_cb _hidl_cb) override;
    Return<SecureElementStatus> closeChannel(uint8_t channelNumber) override;
	Return<SecureElementStatus> reset() override;
    void serviceDied(uint64_t cookie, const android::wp<IBase>& /*who*/) override;

    const char *statusToString(SecureElementStatus status);
    uint8_t setChannelToClassByte(uint8_t cla, int channelNumber);
    uint8_t clearChannelNumber(uint8_t cla);
    int parseChannelNumber(uint8_t cla);
    static void onStateChange(int channel, void *data);

private:
    uint8_t mOpenedchannelCount = 0;
    bool mOpenedChannels[MAX_CHANNEL_NUM];
    static sp<V1_0::ISecureElementHalCallback> mCallbackV1_0[SIM_COUNT];
    static sp<V1_1::ISecureElementHalCallback> mCallbackV1_1[SIM_COUNT];
    unsigned int mSocketId;
    static bool mConnected[SIM_COUNT];
    pthread_mutex_t mutex_socket;
    int nMutexCount;
};

#endif  // ANDROID_HARDWARE_SECURE_ELEMENT_V1_1_SECUREELEMENT_H
