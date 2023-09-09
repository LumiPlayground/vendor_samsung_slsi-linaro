/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#pragma once

#include <hidl/hidlhalindication_network.h>
#include <hidl/1.6/hidlhalindication_network.h>

namespace android::hardware::radio::impl::network {

struct BaseNetworkUnsolResponse {
    ::android::sp<RadioNetworkIndicationHook> mHook;
    BaseNetworkUnsolResponse(const ::android::sp<RadioNetworkIndicationHook>& hook) : mHook(hook) {}
};

struct BarringInfoChangedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).barringInfoChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CdmaPrlChangedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).cdmaPrlChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CellInfoListInd_1_5 : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).cellInfoListInd_1_5(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CurrentPhysicalChannelConfigs_1_4 : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).currentPhysicalChannelConfigs_1_4(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CurrentSignalStrengthInd_1_4 : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).currentSignalStrengthInd_1_4(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct ImsNetworkStateChangedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).imsNetworkStateChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct NetworkScanResultInd_1_5 : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).networkScanResultInd_1_5(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct NetworkStateChangedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).networkStateChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct NitzTimeReceivedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).nitzTimeReceivedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct RegistrationFailedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).registrationFailedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct RestrictedStateChangedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).restrictedStateChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct SuppSvcNotifyInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).suppSvcNotifyInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct VoiceRadioTechChangedInd : public BaseNetworkUnsolResponse {
    using BaseNetworkUnsolResponse::BaseNetworkUnsolResponse;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication(mHook).voiceRadioTechChangedInd(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct BaseNetworkUnsolResponse_1_6 {
    ::android::sp<RadioNetworkIndicationHook> mHook;
    BaseNetworkUnsolResponse_1_6(const ::android::sp<RadioNetworkIndicationHook>& hook) : mHook(hook) {}
};

struct CurrentLinkCapacityEstimate_1_6 : public BaseNetworkUnsolResponse_1_6 {
    using BaseNetworkUnsolResponse_1_6::BaseNetworkUnsolResponse_1_6;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication_1_6(mHook).currentLinkCapacityEstimate_1_6(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CellInfoListInd_1_6 : public BaseNetworkUnsolResponse_1_6 {
    using BaseNetworkUnsolResponse_1_6::BaseNetworkUnsolResponse_1_6;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication_1_6(mHook).cellInfoListInd_1_6(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CurrentPhysicalChannelConfigs_1_6 : public BaseNetworkUnsolResponse_1_6 {
    using BaseNetworkUnsolResponse_1_6::BaseNetworkUnsolResponse_1_6;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication_1_6(mHook).currentPhysicalChannelConfigs_1_6(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct CurrentSignalStrengthInd_1_6 : public BaseNetworkUnsolResponse_1_6 {
    using BaseNetworkUnsolResponse_1_6::BaseNetworkUnsolResponse_1_6;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication_1_6(mHook).currentSignalStrengthInd_1_6(
                        slotId, indicationType, token, e, response, responselen);
    }
};

struct NetworkScanResultInd_1_6 : public BaseNetworkUnsolResponse_1_6 {
    using BaseNetworkUnsolResponse_1_6::BaseNetworkUnsolResponse_1_6;

    int operator()(int slotId,int indicationType, int token, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkIndication_1_6(mHook).networkScanResultInd_1_6(
                        slotId, indicationType, token, e, response, responselen);
    }
};

}
