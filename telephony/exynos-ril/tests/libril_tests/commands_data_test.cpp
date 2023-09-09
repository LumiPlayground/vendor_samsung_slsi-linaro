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
#include <gtest/gtest.h>

// libril
#include <radiocallbackadapter.h>
#include <aidl-service/data/commands.h>
#include <aidl-service/data/vendorstructs.h>
#include <vendorfunctions.h>

#include <slsi/radio_v2_0.h>
#include <testutils.h>
#include "commands_test_base.h"

using namespace android::hardware::radio::impl;
using namespace android::hardware::radio::impl::data;
using libriltest::GetRandomSerialNumber;
using libriltest::commands::setupCommand;
using libriltest::commands::setupIndCommand;
using libriltest::commands::setupVendorFunctions;

TEST(DataCommands, SetupDataCallCommand_onNullRequest) {
    namespace aidl = ::aidl::android::hardware::radio::data;
    static int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2, 0));
    static bool testDone = false;

    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void * /*data*/, size_t /*datalen*/, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<RadioDataResponseCallbackAdapter>();
    auto test = setupCommand<SetupDataCallCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), (::aidl::android::hardware::radio::AccessNetwork)1, {}, false,
                    (aidl_data::DataRequestReason)1, {}, {}, 11, {}, false);
    EXPECT_TRUE(testDone);
}

TEST(DataCommands, SetupDataCallCommand_onRequest) {
    namespace aidl = ::aidl::android::hardware::radio::data;
    static int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2, 0));
    ::aidl::android::hardware::radio::data::TrafficDescriptor *testTd = new ::aidl::android::hardware::radio::data::TrafficDescriptor();
    std::string testdnn("enterprise");
    testTd->dnn = testdnn;
    static const aidl_data::DataProfileInfo testDp = {
        .profileId = 0,
        .apn = "testAPN",
        .protocol = ::aidl::android::hardware::radio::data::PdpProtocolType(0),
        .roamingProtocol = ::aidl::android::hardware::radio::data::PdpProtocolType(0),
        .authType = ::aidl::android::hardware::radio::data::ApnAuthType(0),
        .user = "",
        .password = "password",
        .type = 0,
        .maxConnsTime = 1000,
        .maxConns = 5,
        .enabled = true,
        .supportedApnTypesBitmap = 0,
        .bearerBitmap = 0,
        .mtuV4 = 0,
        .mtuV6 = 0,
        .preferred = false,
        .persistent = false,
        .alwaysOn = false,
        .trafficDescriptor = *testTd,
    };
    static std::vector<aidl_data::LinkAddress> testAddrV;
    static const aidl_data::LinkAddress testAddr = {
        .address = "127.0.0.1",
        .addressProperties = 0,
        .deprecationTime = 0,
        .expirationTime = 0};
    testAddrV.push_back(testAddr);

    const std::vector<std::string> dnses = {};
    const std::optional<aidl_data::SliceInfo> testSliceInfo = {};
    static bool testDone = false;

    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            EXPECT_NE(data, nullptr);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);

            DataV2_0::RIL_SetupDataCallInfo *testVector = (DataV2_0::RIL_SetupDataCallInfo *)data;
            EXPECT_EQ(sizeof(V2_0::RIL_SetupDataCallInfo), datalen);

            EXPECT_EQ(testVector->dataProfileInfo.apn, testDp.apn);
            EXPECT_EQ(testVector->dataProfileInfo.user, testDp.user);
            EXPECT_EQ(testVector->dataProfileInfo.password, testDp.password);
            EXPECT_EQ(testVector->linkAddresses[0].address, testAddrV.at(0).address);

            EXPECT_EQ(testVector->tdInfo.has_value(), true);
            EXPECT_EQ(testVector->tdInfo->dnn.empty(), false);
            if (!testVector->tdInfo->dnn.empty()) {
                EXPECT_EQ(testVector->tdInfo->dnn[0], testDp.trafficDescriptor.dnn);
            }
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<RadioDataResponseCallbackAdapter>();
    auto test = setupCommand<SetupDataCallCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), (::aidl::android::hardware::radio::AccessNetwork)1, testDp, false,
                    (aidl_data::DataRequestReason)1, testAddrV, dnses, 11, testSliceInfo, false);
    EXPECT_TRUE(testDone);
    delete testTd;
}

TEST(DataCommands, SetupDataCallCommand_onNullResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static bool testDone = false;
    struct CallbackImpl : public RadioDataResponseCallbackAdapter {
        void setupDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info,
                                   const ::aidl::android::hardware::radio::data::SetupDataCallResult /*dcResponse*/) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE || info.error == aidl::RadioError::INVALID_RESPONSE);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<SetupDataCallCommand>(func, cb, request);

    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    // invalid response
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, 0);
    EXPECT_TRUE(testDone);
}

// Check 1.6 Result to 2.0 AIDL
TEST(DataCommands, SetupDataCallCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    const char *name = "rmnet1";
    std::vector<RIL_QosSession> testQosSv = {};
    std::vector<DataV2_0::RIL_TrafficDescriptor> testTDset = {};
    std::vector<std::string> testAddr;
    testAddr.clear();
    testAddr.push_back("127.0.0.1");

    std::vector<RIL_QosFilter> qosFilters;
    RIL_QosFilter testFilter = {
        .localAddresses = testAddr,
        .remoteAddresses = testAddr,
        .localPortFlag = MAYBE_PORT_VALID,
        .localPort = {80, 90},
        .remotePortFlag = MAYBE_PORT_VALID,
        .remotePort = {80, 90},
        .protocol = QOS_PROTOCOL_TCP,
        .tosFlag = TYPE_OF_SERVICE_VALID,
        .tos = 1,
        .flowLabelFlag = IPV6_FLOW_LABEL_VALID,
        .flowLabel = {11},
        .spiFlag = IP_SEC_SPI_VALID,
        .spi = {12},
        .direction = BIDIRECTIONAL,
        .precedence = -1};
    qosFilters.push_back(testFilter);

    // QosSession
    RIL_QosSession testSess =
        {11, EPS_QOS, {{1, {200, 300}, {400, 500}}}, 1, qosFilters};
    testQosSv.clear();
    testQosSv.push_back(testSess);
    // TD
    const char testdnn[MAX_DNN_LENGTH] = "testDnn";
    std::vector<RIL_TrafficDescriptor> testVec;
    RIL_TrafficDescriptor *testTd = new RIL_TrafficDescriptor();
    testTd->dnn.clear();
    testTd->osAppId.clear();
    RIL_OptionalDNN *tDnn = new RIL_OptionalDNN();
    RIL_OptionalOSAppId *tOsAppId = new RIL_OptionalOSAppId();

    memset(tDnn->value, 0, sizeof(RIL_OptionalDNN));
    memcpy(tDnn->value, testdnn, sizeof(testdnn));
    testTd->dnn.push_back(*tDnn);

    memset(tOsAppId->value, 0, sizeof(RIL_OptionalOSAppId));
    tOsAppId->value[0] = 'a';
    tOsAppId->value[1] = 'n';
    tOsAppId->value[2] = 'd';
    tOsAppId->value[3] = 'r';
    tOsAppId->value[4] = 'o';
    tOsAppId->value[5] = 'i';
    tOsAppId->value[6] = 'd';

    testTd->osAppIdFlag = RIL_OPT_INFO_VALID;
    testTd->osAppId.push_back(*tOsAppId);

    testVec.push_back(*testTd);

    static const RIL_SetupDataCallResult_V1_6 testVector = {
        .cause = (RIL_DataCallFailCause)PDP_FAIL_PDN_CONN_DOES_NOT_EXIST,
        .suggestedRetryTime = 0,
        .cid = 1,
        .active = DATA_STATE_INACTIVE,
        .type = PDP_PROTOCOL_TYPE_UNKNOWN,
        .ifname = (char *)name,
        .addresses = {},
        .linkaddresses = {},
        .dnses = {},
        .gateways = {},
        .pcscf = {},
        .mtuV4 = 0,
        .mtuV6 = 0,
        .defaultQosType = NR_QOS,
        .defaultQos = {.nr = {1, {200, 300}, {400, 500}, 3, 4}},
        .lenQosSessions = 1,
        .qosSessions = testQosSv,
        .handoverFailureMode = NO_FALLBACK_RETRY_HANDOVER,
        .pduSessionId = 11,
        .sliceInfoFlag = RIL_OPT_INFO_VALID,
        .sliceInfo = {(RIL_SliceServiceType)1, 2, (RIL_SliceServiceType)2, 4, (RIL_SliceStatus)3},
        .trafficDescriptors = testVec,
    };
#if 0
    const RIL_SetupDataCallResult_V1_6 testVector2 = {
        .cause = (RIL_DataCallFailCause)0,
        .suggestedRetryTime = 0,
        .cid = 1,
        .active = DATA_STATE_INACTIVE,
        .type = PDP_PROTOCOL_TYPE_UNKNOWN,
        .ifname = (char *)name,
        .addresses = {},
        .linkaddresses = {},
        .dnses = {},
        .gateways = {},
        .pcscf = {},
        .mtuV4 = 0,
        .mtuV6 = 0,
        .defaultQosType = NR_QOS,
        .defaultQos = {.nr = {1, {200, 300}, {400, 500}, 3, 4}},
        .lenQosSessions = 1,
        .qosSessions = testQosSv,
        .handoverFailureMode = NO_FALLBACK_RETRY_HANDOVER,
        .pduSessionId = 11,
        .sliceInfoFlag = RIL_OPT_INFO_VALID,
        .sliceInfo = {(RIL_SliceServiceType)1, 2, (RIL_SliceServiceType)2, 4, (RIL_SliceStatus)3},
        .trafficDescriptors = testVec};
        aidl_data::SetupDataCallResult result2 = toAidl(testVector2);
#endif
    static bool testDone = false;

    struct CallbackImpl : public RadioDataResponseCallbackAdapter {
        void setupDataCallResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info,
                                   const ::aidl::android::hardware::radio::data::SetupDataCallResult dcResponse) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE || info.error == aidl::RadioError::INVALID_RESPONSE);

            aidl_data::SetupDataCallResult result = toAidl(testVector);
            EXPECT_EQ(dcResponse.cause, result.cause);
            EXPECT_EQ(dcResponse.suggestedRetryTime, result.suggestedRetryTime);
            EXPECT_EQ(dcResponse.cid, result.cid);
            EXPECT_EQ(dcResponse.active, result.active);
            EXPECT_EQ(dcResponse.type, result.type);
            EXPECT_EQ(dcResponse.ifname, "rmnet1");
            EXPECT_EQ(dcResponse.dnses, result.dnses);
            EXPECT_EQ(dcResponse.gateways, result.gateways);
            EXPECT_EQ(dcResponse.pcscf, result.pcscf);
            EXPECT_EQ(dcResponse.mtuV4, result.mtuV4);
            EXPECT_EQ(dcResponse.mtuV6, result.mtuV6);
            EXPECT_EQ(dcResponse.handoverFailureMode, result.handoverFailureMode);
            EXPECT_EQ(dcResponse.pduSessionId, 11);
            EXPECT_EQ(dcResponse.sliceInfo, result.sliceInfo);
            EXPECT_EQ(dcResponse.trafficDescriptors, result.trafficDescriptors);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<SetupDataCallCommand>(func, cb, request);

    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    // invalid response
    // test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, sizeof(testVector));
    // test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, (void *)&testVector, 0);
    // success
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, (void *)&testVector, sizeof(RIL_SetupDataCallResult_V1_6));

    EXPECT_TRUE(testDone);
    delete tDnn;
    delete tOsAppId;
    delete testTd;
}

TEST(DataCommands, SetDataProfileCommand_onNullRequest) {
    namespace aidl = ::aidl::android::hardware::radio::data;
    static int request = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(2, 0));
    const std::vector<aidl_data::DataProfileInfo> testVector = {{}, {}};
    static bool testDone = false;

    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t /*datalen*/, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            EXPECT_NE(data, nullptr);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<RadioDataResponseCallbackAdapter>();
    auto test = setupCommand<SetDataProfileCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), testVector);
    EXPECT_TRUE(testDone);
}

TEST(DataCommands, SetDataProfileCommand_onRequest) {
    namespace aidl = ::aidl::android::hardware::radio::data;
    static const int request = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(2, 0));
    static const aidl_data::TrafficDescriptor td = {"dnnTest", {}};
    static const aidl_data::DataProfileInfo dp1 = {1, "testAPN1", aidl_data::PdpProtocolType::IPV4V6, aidl_data::PdpProtocolType::IPV4V6,
                                                   aidl_data::ApnAuthType::PAP_CHAP, "user1", "password2", 2, 3, 4, 5, true, 6, 7, 1280, 1380, false, false, true, td};
    static const aidl_data::DataProfileInfo dp2 = {2, "testAPN2", aidl_data::PdpProtocolType::IP, aidl_data::PdpProtocolType::IPV6,
                                                   aidl_data::ApnAuthType::PAP_CHAP, "", "password2", 2, 3, 4, 5, true, 6, 7, 1280, 1380, true, true, false, td};
    static const std::vector<aidl_data::DataProfileInfo> testVector = {dp1, dp2};

    static bool testDone = false;

    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            DataV2_0::RIL_DataProfileInfo *testVector1 = ((DataV2_0::RIL_DataProfileInfo **)data)[0];
            DataV2_0::RIL_DataProfileInfo *testVector2 = ((DataV2_0::RIL_DataProfileInfo **)data)[1];

            EXPECT_EQ(request, request);
            EXPECT_EQ(sizeof(V2_0::RIL_DataProfileInfo *) * 2, datalen);

            EXPECT_EQ(testVector1->apn, dp1.apn);
            EXPECT_EQ(testVector1->user, dp1.user);
            EXPECT_EQ(testVector1->password, dp1.password);

            EXPECT_EQ((int)testVector1->profileId, dp1.profileId);
            EXPECT_EQ(testVector1->apn, dp1.apn);
            EXPECT_EQ((int)testVector1->protocol, (int)dp1.protocol);
            EXPECT_EQ((int)testVector1->roamingProtocol, (int)dp1.roamingProtocol);
            EXPECT_EQ((int)testVector1->authType, (int)dp1.authType);
            EXPECT_EQ(testVector1->user, dp1.user);
            EXPECT_EQ(testVector1->password, dp1.password);
            EXPECT_EQ(testVector1->type, dp1.type);
            EXPECT_EQ(testVector1->maxConnsTime, dp1.maxConnsTime);
            EXPECT_EQ(testVector1->maxConns, dp1.maxConns);
            EXPECT_EQ(testVector1->waitTime, dp1.waitTime);
            EXPECT_EQ(testVector1->enabled, dp1.enabled);
            EXPECT_EQ((int)testVector1->supportedApnTypesBitmap, (int)dp1.supportedApnTypesBitmap);
            EXPECT_EQ((int)testVector1->bearerBitmap, (int)dp1.bearerBitmap);
            EXPECT_EQ(testVector1->mtuV4, dp1.mtuV4);
            EXPECT_EQ(testVector1->mtuV6, dp1.mtuV6);
            EXPECT_EQ(testVector1->preferred, dp1.preferred);
            EXPECT_EQ(testVector1->persistent, dp1.persistent);
            EXPECT_EQ(testVector1->alwaysOn, dp1.alwaysOn);

            EXPECT_EQ(testVector2->apn, dp2.apn);
            EXPECT_EQ(testVector2->user, dp2.user);
            EXPECT_EQ(testVector2->password, dp2.password);
            EXPECT_FALSE(testVector2->preferred == dp1.preferred);
            EXPECT_FALSE(testVector2->persistent == dp1.persistent);
            EXPECT_FALSE(testVector2->alwaysOn == dp1.alwaysOn);

            EXPECT_EQ(testVector1->tdInfo.has_value(), true);
            EXPECT_EQ(testVector1->tdInfo->dnn.empty(), false);
            if (!testVector1->tdInfo->dnn.empty()) {
                EXPECT_EQ(testVector1->tdInfo->dnn[0], dp1.trafficDescriptor.dnn.value());
            }

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<RadioDataResponseCallbackAdapter>();
    auto test = setupCommand<SetDataProfileCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), testVector);
    EXPECT_TRUE(testDone);
}

TEST(DataCommands, SetDataProfileCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;

    static bool testDone = false;

    struct CallbackImpl : public RadioDataResponseCallbackAdapter {
        void setDataProfileResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE || info.error == aidl::RadioError::INVALID_RESPONSE);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<SetDataProfileCommand>(func, cb, request);

    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, 0);

    EXPECT_TRUE(testDone);
}

TEST(DataCommands, SetInitialAttachApnCommand_onNullRequest) {
    namespace aidl = ::aidl::android::hardware::radio::data;
    static int request = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(2, 0));
    static bool testDone = false;

    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void * /*data*/, size_t /*datalen*/, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            // EXPECT_NE(data, nullptr);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<RadioDataResponseCallbackAdapter>();
    auto test = setupCommand<SetInitialAttachApnCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), {});
    EXPECT_TRUE(testDone);
}

TEST(DataCommands, SetInitialAttachApnCommand_onRequest) {
    namespace aidl = ::aidl::android::hardware::radio::data;
    static int request = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(2, 0));
    static const aidl_data::DataProfileInfo testVector = {
        .profileId = 0,
        .apn = "testAPN",
        .protocol = ::aidl::android::hardware::radio::data::PdpProtocolType(0),
        .roamingProtocol = ::aidl::android::hardware::radio::data::PdpProtocolType(1),
        .authType = ::aidl::android::hardware::radio::data::ApnAuthType(0),
        .user = "",
        .password = "",
        .type = 0,
        .maxConnsTime = 1000,
        .maxConns = 5,
        .enabled = true,
        .supportedApnTypesBitmap = 0,
        .bearerBitmap = 0,
        .mtuV4 = 0,
        .mtuV6 = 0,
        .preferred = false,
        .persistent = false,
        .alwaysOn = false,
        .trafficDescriptor = {},
    };

    static bool testDone = false;

    struct CallbackImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t /*datalen*/, RIL_Token t, RIL_SOCKET_ID) {
            testDone = true;
            EXPECT_EQ(request, request);
            EXPECT_NE(data, nullptr);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);

            // AIDL to RIL
            aidl_data::DataProfileInfo input = *(aidl_data::DataProfileInfo *)data;
            V2_0::RIL_DataProfileInfo result = {};
            convertHalDataProfileToRilV2_0(result, input);
            convertHalTDToRil(result.tdInfo, input.trafficDescriptor);
            // RIL to AIDL
            aidl_data::DataProfileInfo convertedResult = toAidl(result);

            EXPECT_EQ(input.apn, convertedResult.apn);
            EXPECT_EQ(input.user, convertedResult.user);
            EXPECT_EQ(input.password, convertedResult.password);
            EXPECT_EQ(input.protocol, convertedResult.protocol);
            EXPECT_EQ(input.roamingProtocol, convertedResult.roamingProtocol);
            EXPECT_EQ(input.type, convertedResult.type);
        }
    } callback;
    auto func = setupVendorFunctions(&callback);
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<RadioDataResponseCallbackAdapter>();
    auto test = setupCommand<SetInitialAttachApnCommand>(func, cb, request);
    test->onRequest(GetRandomSerialNumber(), testVector);
    EXPECT_TRUE(testDone);
}

TEST(DataCommands, SetInitialAttachApnCommand_onResponse) {
    namespace aidl = ::aidl::android::hardware::radio;
    static bool testDone = false;

    struct CallbackImpl : public RadioDataResponseCallbackAdapter {
        void setInitialAttachApnResponse(const ::aidl::android::hardware::radio::RadioResponseInfo &info) {
            testDone = true;
            EXPECT_TRUE(info.error == aidl::RadioError::NONE || info.error == aidl::RadioError::INVALID_RESPONSE);
        }
    };
    auto func = setupVendorFunctions();
    std::shared_ptr<RadioDataResponseCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(2, 0));
    auto test = setupCommand<SetInitialAttachApnCommand>(func, cb, request);

    int slotId = (int)func.mSlotId;
    int32_t serial = GetRandomSerialNumber();

    // invalid response
    test->onResponse(slotId, RESPONSE_SOLICITED, serial, RIL_E_SUCCESS, nullptr, 0);

    EXPECT_TRUE(testDone);
}

TEST(DataCommands, UnthrottleApnInd_Indication) {
    const char testdnn[MAX_DNN_LENGTH] = "unthrottleApnIndAPN";

    DataV2_0::RIL_TrafficDescriptor *testTd = new DataV2_0::RIL_TrafficDescriptor();
    testTd->dnn.push_back(testdnn);

    static V2_0::RIL_DataProfileInfo *testVector = new V2_0::RIL_DataProfileInfo();
    testVector->profileId = RIL_DataProfileId::DEFAULT;
    testVector->apn = testdnn;
    testVector->protocol = PDP_PROTOCOL_TYPE_IPV4V6;
    testVector->roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6;
    testVector->authType = PAP_CHAP;
    testVector->user = "test_user";
    testVector->password = "test_pw";
    testVector->type = THREE_GPP;
    testVector->maxConnsTime = 10000;
    testVector->maxConns = 10;
    testVector->waitTime = 10000;
    testVector->enabled = true;
    testVector->supportedApnTypesBitmap = 0x0002;
    testVector->bearerBitmap = 2;
    testVector->mtuV4 = 1570;
    testVector->mtuV6 = 1570;
    testVector->preferred = false;
    testVector->persistent = false;
    testVector->alwaysOn = false;
    testVector->tdInfo = *testTd;

    static bool testDone = false;
    struct CallbackImpl : public RadioDataIndicationCallbackAdapter {
        void unthrottleApn(const ::aidl::android::hardware::radio::RadioIndicationType /*type*/, ::aidl::android::hardware::radio::data::DataProfileInfo dp) {
            testDone = true;
            // AIDL to RIL
            V2_0::RIL_DataProfileInfo result = {};
            convertHalDataProfileToRilV2_0(result, dp);
            convertHalTDToRil(result.tdInfo, dp.trafficDescriptor);

            // Compare testvector & converted input
            EXPECT_EQ(result.profileId, testVector->profileId);
            EXPECT_EQ(result.apn, testVector->apn);
            EXPECT_EQ(result.protocol, testVector->protocol);
            EXPECT_EQ(result.roamingProtocol, testVector->roamingProtocol);
            EXPECT_EQ(result.authType, testVector->authType);
            EXPECT_EQ(result.user, testVector->user);
            EXPECT_EQ(result.password, testVector->password);
            EXPECT_EQ(result.type, testVector->type);
            EXPECT_EQ(result.maxConnsTime, testVector->maxConnsTime);
            EXPECT_EQ(result.maxConns, testVector->maxConns);
            EXPECT_EQ(result.waitTime, testVector->waitTime);
            EXPECT_EQ(result.enabled, testVector->enabled);
            EXPECT_EQ(result.supportedApnTypesBitmap, testVector->supportedApnTypesBitmap);
            EXPECT_EQ(result.bearerBitmap, testVector->bearerBitmap);
            EXPECT_EQ(result.mtuV4, testVector->mtuV4);
            EXPECT_EQ(result.mtuV6, testVector->mtuV6);
            EXPECT_EQ(result.preferred, testVector->preferred);
            EXPECT_EQ(result.persistent, testVector->persistent);
            EXPECT_EQ(result.tdInfo.has_value(), true);
            if (result.tdInfo.has_value() && !result.tdInfo->dnn.empty()) {
                EXPECT_EQ(result.tdInfo->dnn, testVector->tdInfo->dnn);
            }
            // Vector Shall be empty
            EXPECT_EQ(result.tdInfo->osAppIds.empty(), true);
        }
    };
    int slotId = 0;
    std::shared_ptr<RadioDataIndicationCallback> cb = std::make_shared<CallbackImpl>();
    int request = ENCODE_REQUEST(RIL_UNSOL_UNTHROTTLE_APN, HAL_VERSION_CODE(2, 0));
    auto test = setupIndCommand<UnthrottleApnInd>(slotId, cb, request);

    // invalid response
    // test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, 0);
    // test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, nullptr, sizeof(testVector));
    // test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, 0);
    // normal
    test->onIndication(slotId, RESPONSE_UNSOLICITED, 0, RIL_E_SUCCESS, testVector, sizeof(testVector));
    EXPECT_TRUE(testDone);

    delete testTd;
    delete testVector;
}
