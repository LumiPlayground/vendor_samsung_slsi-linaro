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

#include <hidl/hidlhalresponse_sim.h>
#include <hidl/1.6/hidlhalresponse_sim.h>
#include <telephony/ril.h>
#include "radiosimresponse.h"

namespace android::hardware::radio::impl::sim {

struct BaseHidlSimCommand {
    ::android::sp<RadioSimResponseHook> mHook;
    BaseHidlSimCommand(const ::android::sp<RadioSimResponseHook>& hook) : mHook(hook) {}
};

struct AreUiccApplicationsEnabledResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).areUiccApplicationsEnabledResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct ChangeIccPin2ForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).changeIccPin2ForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct ChangeIccPinForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).changeIccPinForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct EnableUiccApplicationsResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).enableUiccApplicationsResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetAllowedCarriersResponse_1_4 : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).getAllowedCarriersResponse_1_4(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetCdmaSubscriptionResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).getCDMASubscriptionResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetCdmaSubscriptionSourceResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).getCdmaSubscriptionSourceResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetFacilityLockForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).getFacilityLockForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetIMSIForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).getIMSIForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct IccCloseLogicalChannelResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).iccCloseLogicalChannelResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct IccIoForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).iccIoForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct IccOpenLogicalChannelResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).iccOpenLogicalChannelResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct IccTransmitApduBasicChannelResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).iccTransmitApduBasicChannelResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct IccTransmitApduLogicalChannelResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).iccTransmitApduLogicalChannelResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct ReportStkServiceIsRunningResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).reportStkServiceIsRunningResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct RequestIccSimAuthenticationResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).requestIccSimAuthenticationResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SendEnvelopeResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).sendEnvelopeResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SendEnvelopeWithStatusResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).sendEnvelopeWithStatusResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SendTerminalResponseToSimResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).sendTerminalResponseToSimResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetAllowedCarriersResponse_1_4 : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).setAllowedCarriersResponse_1_4(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetCarrierInfoForImsiEncryptionResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).setCarrierInfoForImsiEncryptionResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetCdmaSubscriptionSourceResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).setCdmaSubscriptionSourceResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetFacilityLockForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).setFacilityLockForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetSimCardPowerResponse_1_6 : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse_1_6(mHook).setSimCardPowerResponse_1_6(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetUiccSubscriptionResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).setUiccSubscriptionResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SupplyIccPin2ForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).supplyIccPin2ForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SupplyIccPinForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).supplyIccPinForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SupplyIccPuk2ForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).supplyIccPuk2ForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SupplyIccPukForAppResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).supplyIccPukForAppResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SupplySimDepersonalizationResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse(mHook).supplySimDepersonalizationResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

// V1.6
struct GetSimPhonebookCapacityResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse_1_6(mHook).getSimPhonebookCapacityResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetSimPhonebookRecordsResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse_1_6(mHook).getSimPhonebookRecordsResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct UpdateSimPhonebookRecordsResponse : public BaseHidlSimCommand {
    using BaseHidlSimCommand::BaseHidlSimCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalSimResponse_1_6(mHook).updateSimPhonebookRecordsResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

}
