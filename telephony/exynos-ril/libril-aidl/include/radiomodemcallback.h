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

#include <aidl/android/hardware/radio/modem/BnRadioModem.h>

namespace android::hardware::radio::impl {

struct RadioModemResponseCallback;
struct RadioModemIndicationCallback;

struct RadioModemCallback {
    virtual ~RadioModemCallback() = default;
    virtual void setResponseFunctions(
        const std::shared_ptr<RadioModemResponseCallback>& responseCb,
        const std::shared_ptr<RadioModemIndicationCallback>& indicationCb) = 0;
    virtual void enableModem(int32_t serial, bool on) = 0;
    virtual void getBasebandVersion(int32_t serial) = 0;
    virtual void getDeviceIdentity(int32_t serial) = 0;
    virtual void getHardwareConfig(int32_t serial) = 0;
    virtual void getModemActivityInfo(int32_t serial) = 0;
    virtual void getModemStackStatus(int32_t serial) = 0;
    virtual void getRadioCapability(int32_t serial) = 0;
    virtual void nvReadItem(
        int32_t serial, ::aidl::android::hardware::radio::modem::NvItem itemId) = 0;
    virtual void nvResetConfig(
        int32_t serial, ::aidl::android::hardware::radio::modem::ResetNvType type) = 0;
    virtual void nvWriteCdmaPrl(int32_t serial, const std::vector<uint8_t>& prl) = 0;
    virtual void nvWriteItem(
        int32_t serial, const ::aidl::android::hardware::radio::modem::NvWriteItem& i) = 0;
    virtual void requestShutdown(int32_t serial) = 0;
    virtual void responseAcknowledgement() = 0;
    virtual void sendDeviceState(
        int32_t serial, ::aidl::android::hardware::radio::modem::DeviceStateType stateType,
        bool state) = 0;
    virtual void setRadioCapability(
        int32_t serial, const ::aidl::android::hardware::radio::modem::RadioCapability& rc) = 0;
    virtual void setRadioPower(int32_t serial, bool powerOn, bool forEmergencyCall,
                               bool preferredForEmergencyCall) = 0;
};

struct RadioModemResponseCallback {
    virtual ~RadioModemResponseCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemResponse>&
                    radioModemResponse) = 0;
    virtual void acknowledgeRequest(int32_t in_serial) = 0;
    virtual void enableModemResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void getBasebandVersionResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const std::string& in_version) = 0;
    virtual void getDeviceIdentityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const std::string& in_imei, const std::string& in_imeisv,
        const std::string& in_esn, const std::string& in_meid) = 0;
    virtual void getHardwareConfigResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const std::vector<::aidl::android::hardware::radio::modem::HardwareConfig>& in_config) = 0;
    virtual void getModemActivityInfoResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const ::aidl::android::hardware::radio::modem::ActivityStatsInfo& in_activityInfo) = 0;
    virtual void getModemStackStatusResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        bool in_isEnabled) = 0;
    virtual void getRadioCapabilityResponse(
        const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
        const ::aidl::android::hardware::radio::modem::RadioCapability& in_rc) = 0;
    virtual void nvReadItemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
                                    const std::string& in_result) = 0;
    virtual void nvResetConfigResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void nvWriteCdmaPrlResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void nvWriteItemResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void requestShutdownResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void sendDeviceStateResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
    virtual void setRadioCapabilityResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info,
                                            const ::aidl::android::hardware::radio::modem::RadioCapability& in_rc) = 0;
    virtual void setRadioPowerResponse(const ::aidl::android::hardware::radio::RadioResponseInfo& in_info) = 0;
};

struct RadioModemIndicationCallback {
    virtual ~RadioModemIndicationCallback() = default;
    virtual void setResponseFunction(
            const std::shared_ptr<::aidl::android::hardware::radio::modem::IRadioModemIndication>&
                    radioModemIndication) = 0;
    virtual void hardwareConfigChanged(
        ::aidl::android::hardware::radio::RadioIndicationType in_type,
        const std::vector<::aidl::android::hardware::radio::modem::HardwareConfig>& in_configs) = 0;
    virtual void modemReset(
        ::aidl::android::hardware::radio::RadioIndicationType in_type,
        const std::string& in_reason) = 0;
    virtual void radioCapabilityIndication(::aidl::android::hardware::radio::RadioIndicationType in_type,
                                           const ::aidl::android::hardware::radio::modem::RadioCapability& in_rc) = 0;
    virtual void radioStateChanged(
        ::aidl::android::hardware::radio::RadioIndicationType in_type,
        ::aidl::android::hardware::radio::modem::RadioState in_radioState) = 0;
    virtual void rilConnected(::aidl::android::hardware::radio::RadioIndicationType in_type) = 0;
};

}  // namespace android::hardware::radio::impl {