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

#include <hidl/hidlhalresponse_network.h>
#include <hidl/1.6/hidlhalresponse_network.h>
#include "radionetworkresponse.h"
#include <telephony/ril.h>

namespace android::hardware::radio::impl::network {

struct BaseHidlNetworkCommand {
    ::android::sp<RadioNetworkResponseHook> mHook;
    BaseHidlNetworkCommand(const ::android::sp<RadioNetworkResponseHook>& hook) : mHook(hook) {}
};

struct GetAvailableBandModesResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getAvailableBandModesResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetAvailableNetworksResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getAvailableNetworksResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetBarringInfoResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getBarringInfoResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetCdmaRoamingPreferenceResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getCdmaRoamingPreferenceResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetCellInfoListResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getCellInfoListResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetDataRegistrationStateResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getDataRegistrationStateResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetImsRegistrationStateResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getImsRegistrationStateResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetNetworkSelectionModeResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getNetworkSelectionModeResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetOperatorResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getOperatorResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetSignalStrengthResponse_1_4 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getSignalStrengthResponse_1_4(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetVoiceRadioTechnologyResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getVoiceRadioTechnologyResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetVoiceRegistrationStateResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).getVoiceRegistrationStateResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetBandModeResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setBandModeResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetBarringPasswordResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setBarringPasswordResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetCdmaRoamingPreferenceResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setCdmaRoamingPreferenceResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetCellInfoListRateResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setCellInfoListRateResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetIndicationFilterResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setIndicationFilterResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetLinkCapacityReportingCriteriaResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setLinkCapacityReportingCriteriaResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetLocationUpdatesResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setLocationUpdatesResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetNetworkSelectionModeAutomaticResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setNetworkSelectionModeAutomaticResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetNetworkSelectionModeManualResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setNetworkSelectionModeManualResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetSignalStrengthReportingCriteriaResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setSignalStrengthReportingCriteriaResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetSuppServiceNotificationsResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setSuppServiceNotificationsResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetSystemSelectionChannelsResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).setSystemSelectionChannelsResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct StartNetworkScanResponse_1_5 : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).startNetworkScanResponse_1_5(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct StopNetworkScanResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).stopNetworkScanResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SupplyNetworkDepersonalizationResponse : public BaseHidlNetworkCommand {
    using BaseHidlNetworkCommand::BaseHidlNetworkCommand;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse(mHook).supplyNetworkDepersonalizationResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct BaseHidlNetworkCommand_1_6 {
    ::android::sp<RadioNetworkResponseHook> mHook;
    BaseHidlNetworkCommand_1_6(const ::android::sp<RadioNetworkResponseHook>& hook) : mHook(hook) {}
};

struct GetAllowedNetworkTypesBitmapResponse : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).getAllowedNetworkTypesBitmapResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetSystemSelectionChannelsResponse : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).getSystemSelectionChannelsResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct IsNrDualConnectivityEnabledResponse : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).isNrDualConnectivityEnabledResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetAllowedNetworkTypesBitmapResponse : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).setAllowedNetworkTypesBitmapResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct SetNrDualConnectivityStateResponse : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).setNrDualConnectivityStateResponse(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetCellInfoListResponse_1_6 : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).getCellInfoListResponse_1_6(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetDataRegistrationStateResponse_1_6 : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).getDataRegistrationStateResponse_1_6(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetSignalStrengthResponse_1_6 : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).getSignalStrengthResponse_1_6(
                        slotId, responseType, serial, e, response, responselen);
    }
};

struct GetVoiceRegistrationStateResponse_1_6 : public BaseHidlNetworkCommand_1_6 {
    using BaseHidlNetworkCommand_1_6::BaseHidlNetworkCommand_1_6;

    int operator()(int slotId,int responseType, int serial, RIL_Errno e,
                    void *response, size_t responselen) {
        return HidlHalNetworkResponse_1_6(mHook).getVoiceRegistrationStateResponse_1_6(
                        slotId, responseType, serial, e, response, responselen);
    }
};

}
