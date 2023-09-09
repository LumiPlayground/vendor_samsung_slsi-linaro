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

#include <slsi/radio_v2_0.h>
#include <string>
#include <sstream>
#include <vector>

#define MAX_DIAL_NUM                       82
#define MAX_DIAL_NAME                      82
#define MAX_CALL_NUM                       8

#define RIL_UNSOL_TRIGGER_IMS_DEREGISTRATION        1107
#define RIL_UNSOL_CONNECTION_SETUP_FAILURE          1108

namespace android::hardware::radio::impl::ims::V2_1 {
enum class RIL_ImsTrafficType {
    EMERGENCY = 0,
    EMERGENCY_SMS = 1,
    VOICE = 2,
    VIDEO = 3,
    SMS = 4,
    REGISTRATION = 5,
    UT_XCAP = 6,
};

enum class RIL_ImsRegState {
    NOT_REGISTERED = 0,
    REGISTERED = 1,
};

enum class RIL_SuggestedAction {
    NONE = 0,
    TRIGGER_PLMN_BLOCK = 1,
    TRIGGER_PLMN_BLOCK_WITH_TIMEOUT = 2,
};

enum class RIL_ImsDeregistrationReason {
    REASON_SIM_REMOVED = 1,
    REASON_SIM_REFRESH = 2,
    REASON_NETWORK_MODE_CHANGED = 3,
};

enum class RIL_EpsFallbackReason {
    NO_NETWORK_TRIGGER = 1,
    NO_NETWORK_RESPONSE = 2,
};

enum class RIL_CallType {
    NORMAL = 0,
    EMERGENCY = 1,
};

enum class RIL_CallSubState {
    NONE = 0,
    PREALERTING = 1,
};

enum class RIL_ToneType {
    NONE = 0,
    LOCAL = 1,
    NETWORK = 2,
};

struct RIL_UpdateImsRegistration {
    RIL_ImsRegState regState;
    int32_t capabilities;
    int32_t accessNetworkType;
    RIL_SuggestedAction suggestedAction;
};

struct RIL_StartImsTraffic {
    int32_t token;
    RIL_ImsTrafficType imsTrafficType;
    int32_t accessNetworks;
    int32_t direction;
};

struct RIL_ConnectionFailureInfo {
    int32_t failureReason;
    int32_t causeCode;
    int32_t waitTimeMillis;
};

struct RIL_OnConnectionSetupFailure {
    int32_t token;
    RIL_ConnectionFailureInfo info;
};

struct RIL_TriggerFpsFallbackInd {
    uint8_t result;
    uint8_t rat;
};

struct RIL_SrvccCall {
    int32_t index;
    RIL_CallType callType;
    int32_t callState;
    RIL_CallSubState callSubstate;
    RIL_ToneType ringbackToneType;
    bool isMpty;
    bool isMT;
    std::string number;
    int32_t numPresentation;
    std::string name;
    int32_t namePresentation;
};

#pragma pack(push, 1)
struct RIL_OemSrvccCall {
    int32_t index;
    uint8_t callType;
    uint8_t callState;
    uint8_t callSubstate;
    uint8_t ringbackToneType;
    bool isMpty;
    bool isMT;
    uint32_t numberLen;
    char number[MAX_DIAL_NAME];
    int32_t numPresentation;
    uint32_t nameLen;
    char name[MAX_DIAL_NUM];
    int32_t namePresentation;
};

struct RIL_SrvccCallListOem{
    int32_t callListNum;
    RIL_OemSrvccCall callList[MAX_CALL_NUM];
};
#pragma pack(pop)

}

namespace android::hardware::radio::impl::modem::V2_1 {

}

namespace android::hardware::radio::impl::sim::V2_1 {

}

namespace android::hardware::radio::impl::network::V2_1 {

}

namespace android::hardware::radio::impl::voice::V2_1 {

}

namespace android::hardware::radio::impl::data::V2_1 {

}

namespace RilImsV2_1 = android::hardware::radio::impl::ims::V2_1;
namespace RilModemV2_1 = android::hardware::radio::impl::modem::V2_1;
namespace RilSimV2_1 = android::hardware::radio::impl::sim::V2_1;
namespace RilNetworkV2_1 = android::hardware::radio::impl::network::V2_1;
namespace RilVoiceV2_1 = android::hardware::radio::impl::voice::V2_1;
namespace RilDataV2_1 = android::hardware::radio::impl::data::V2_1;
