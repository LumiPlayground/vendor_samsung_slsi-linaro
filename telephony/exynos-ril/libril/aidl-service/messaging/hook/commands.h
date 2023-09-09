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

#include <hidl/hidlhalresponse_messaging.h>
#include <hidl/1.6/hidlhalresponse_messaging.h>
#include "radiomessagingresponse.h"
#include <telephony/ril.h>

namespace android::hardware::radio::impl::messaging {

struct BaseHidlMessagingCommand {
    ::android::sp<RadioMessagingResponseHook> mHook;
    BaseHidlMessagingCommand(const ::android::sp<RadioMessagingResponseHook> &hook)
        : mHook(hook) {}
};

struct AcknowledgeIncomingGsmSmsWithPduResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).acknowledgeIncomingGsmSmsWithPduResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct AcknowledgeLastIncomingCdmaSmsResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).acknowledgeLastIncomingCdmaSmsResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct AcknowledgeLastIncomingGsmSmsResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).acknowledgeLastIncomingGsmSmsResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct DeleteSmsOnRuimResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).deleteSmsOnRuimResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct DeleteSmsOnSimResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).deleteSmsOnSimResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct GetCdmaBroadcastConfigResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).getCdmaBroadcastConfigResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct GetGsmBroadcastConfigResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).getGsmBroadcastConfigResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct GetSmscAddressResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).getSmscAddressResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct ReportSmsMemoryStatusResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).reportSmsMemoryStatusResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendCdmaSmsExpectMoreResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).sendCdmaSmsExpectMoreResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendCdmaSmsResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).sendCdmaSmsResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendImsSmsResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).sendImsSmsResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendSmsExpectMoreResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).sendSmsExpectMoreResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendSmsResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).sendSmsResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SetCdmaBroadcastActivationResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).setCdmaBroadcastActivationResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SetCdmaBroadcastConfigResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).setCdmaBroadcastConfigResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SetGsmBroadcastActivationResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).setGsmBroadcastActivationResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SetGsmBroadcastConfigResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).setGsmBroadcastConfigResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SetSmscAddressResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).setSmscAddressResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct WriteSmsToRuimResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).writeSmsToRuimResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct WriteSmsToSimResponse : public BaseHidlMessagingCommand {
    using BaseHidlMessagingCommand::BaseHidlMessagingCommand;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse(mHook).writeSmsToSimResponse(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct BaseHidlMessagingCommand_1_6 {
    ::android::sp<RadioMessagingResponseHook> mHook;
    BaseHidlMessagingCommand_1_6(const ::android::sp<RadioMessagingResponseHook> &hook)
        : mHook(hook) {}
};

struct SendCdmaSmsExpectMoreResponse_1_6 : public BaseHidlMessagingCommand_1_6 {
    using BaseHidlMessagingCommand_1_6::BaseHidlMessagingCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse_1_6(mHook).sendCdmaSmsExpectMoreResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendCdmaSmsResponse_1_6 : public BaseHidlMessagingCommand_1_6 {
    using BaseHidlMessagingCommand_1_6::BaseHidlMessagingCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse_1_6(mHook).sendCdmaSmsResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendSmsExpectMoreResponse_1_6 : public BaseHidlMessagingCommand_1_6 {
    using BaseHidlMessagingCommand_1_6::BaseHidlMessagingCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse_1_6(mHook).sendSmsExpectMoreResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

struct SendSmsResponse_1_6 : public BaseHidlMessagingCommand_1_6 {
    using BaseHidlMessagingCommand_1_6::BaseHidlMessagingCommand_1_6;

    int operator()(int slotId, int responseType, int serial, RIL_Errno e,
                   void *response, size_t responselen) {
        return HidlHalMessagingResponse_1_6(mHook).sendSmsResponse_1_6(
            slotId, responseType, serial, e, response, responselen);
    }
};

}  // namespace android::hardware::radio::impl::messaging
