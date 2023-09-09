/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include "data/hook/structs.h"
#include "commonStructs.h"
#include "collections.h"
#include <slsi/radio_v1_6.h>

using namespace ::android::hardware::radio;
using namespace android::hardware::radio::compat;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;

namespace aidl_data = ::aidl::android::hardware::radio::data;

int length = 5;

static std::string getDnn_aidl(){
    std::string dnn("test dnn");

    return dnn;
}

static aidl_data::OsAppId getOsAppId_aidl(){
    std::vector<uint8_t> tmpOsAppIdValue;
    aidl_data::OsAppId osAppId = {};

    tmpOsAppIdValue.resize(length);
    for(int i = 0 ; i < length ; i++){
        tmpOsAppIdValue[i] = i;
    }
    osAppId.osAppId = tmpOsAppIdValue;

    return osAppId;
}

static aidl_data::TrafficDescriptor getTrafficDescriptor_aidl(){
    aidl_data::TrafficDescriptor tdInfo = {};
    tdInfo.dnn = getDnn_aidl();
    tdInfo.osAppId = getOsAppId_aidl();
    return tdInfo;
}

static V1_6::OptionalDnn getDnn_V1_6(){
    V1_6::OptionalDnn tmpDnn;
    hidl_string ret("test dnn");
    tmpDnn.value(ret);

    return tmpDnn;
}

static V1_6::OsAppId getOsAppId_V1_6(){
    V1_6::OsAppId tmpOsAppIdValue;

    tmpOsAppIdValue.osAppId.resize(length);

    for(int i = 0 ; i < length ; i++){
        tmpOsAppIdValue.osAppId[i] = i;
    }

    return tmpOsAppIdValue;
}

static V1_6::OptionalOsAppId getOptionalOsAppId_V1_6(){
    V1_6::OptionalOsAppId tmpOsAppID;

    tmpOsAppID.value(getOsAppId_V1_6());

    return tmpOsAppID;
}

static V1_6::TrafficDescriptor getTrafficDescriptor_V1_6(){

    V1_6::TrafficDescriptor tdInfo = {getDnn_V1_6(), getOptionalOsAppId_V1_6()};

    return tdInfo;
}

static V1_6::SliceInfo getSliceInfo_V1_6(){

    V1_6::SliceInfo sliceInfo = {
        .sst = (V1_6::SliceServiceType)NONE,
        .sliceDifferentiator = 2,
        .mappedHplmnSst = (V1_6::SliceServiceType)NONE,
        .mappedHplmnSD = 4,
        .status = (V1_6::SliceStatus)ALLOWED,
   };

    return sliceInfo;
}

TEST(HookStructs, DataProfileInfo_toHidl) {
    std::string apn("test apn");
    std::string user("test user");
    std::string password("test password");

    aidl_data::DataProfileInfo testVector = {
        .profileId = 1,
        .apn = apn,
        .protocol = (aidl_data::PdpProtocolType)PDP_PROTOCOL_TYPE_IPV4V6,
        .roamingProtocol = (aidl_data::PdpProtocolType)PDP_PROTOCOL_TYPE_IP,
        .authType = (aidl_data::ApnAuthType)PAP_CHAP,
        .user = user,
        .password = password,
        .type = 0,
        .maxConnsTime = 0,
        .maxConns = 0,
        .waitTime = 0,
        .enabled = false,
        .supportedApnTypesBitmap = 0x40,
        .bearerBitmap = 0x01,
        .mtuV4 = 1400,
        .mtuV6 = 1400,
        .preferred = false,
        .persistent = false,
        .alwaysOn = false,
        .trafficDescriptor = getTrafficDescriptor_aidl(),
    };

    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.profileId == V1_0::DataProfileId{testVector.profileId});
    EXPECT_TRUE(test.apn == testVector.apn);
    EXPECT_TRUE(test.protocol == V1_4::PdpProtocolType{testVector.protocol});
    EXPECT_TRUE(test.roamingProtocol == V1_4::PdpProtocolType{testVector.roamingProtocol});
    EXPECT_TRUE(test.authType == V1_0::ApnAuthType{testVector.authType});
    EXPECT_TRUE(test.user == testVector.user);
    EXPECT_TRUE(test.password == testVector.password);
    EXPECT_TRUE(test.type == V1_0::DataProfileInfoType{testVector.type});
    EXPECT_TRUE(test.maxConnsTime == testVector.maxConnsTime);
    EXPECT_TRUE(test.maxConns == testVector.maxConns);
    EXPECT_TRUE(test.waitTime == testVector.waitTime);
    EXPECT_TRUE(test.enabled == testVector.enabled);
    EXPECT_TRUE(test.supportedApnTypesBitmap == toHidlBitfield<V1_5::ApnTypes>(testVector.supportedApnTypesBitmap));
    EXPECT_TRUE(test.bearerBitmap == toHidlBitfield<V1_4::RadioAccessFamily>(testVector.bearerBitmap));
    EXPECT_TRUE(test.mtuV4 == testVector.mtuV4);
    EXPECT_TRUE(test.mtuV6 == testVector.mtuV6);
    EXPECT_TRUE(test.preferred == testVector.preferred);
    EXPECT_TRUE(test.persistent == testVector.persistent);
}

TEST(HookStructs, LinkAddress_toHidl) {
    std::string address("test address");
    aidl_data::LinkAddress testVector = {
        .address = address,
        .addressProperties = 0,
        .deprecationTime = 1,
        .expirationTime = 2,
        };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.address == testVector.address);
    EXPECT_TRUE(test.properties == testVector.addressProperties);
    EXPECT_TRUE(test.deprecationTime == static_cast<uint64_t>(testVector.deprecationTime));
    EXPECT_TRUE(test.expirationTime == static_cast<uint64_t>(testVector.expirationTime));
}

TEST(HookStructs, SliceInfo_toAidl) {
    V1_6::SliceInfo testVector = {
        .sst = (V1_6::SliceServiceType)NONE,
        .sliceDifferentiator = 2,
        .mappedHplmnSst = (V1_6::SliceServiceType)NONE,
        .mappedHplmnSD = 4,
        .status = (V1_6::SliceStatus)ALLOWED,
        };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.sliceServiceType == static_cast<int8_t>(testVector.sst));
    EXPECT_TRUE(test.sliceDifferentiator == testVector.sliceDifferentiator);
    EXPECT_TRUE(test.mappedHplmnSst == static_cast<int8_t>(testVector.mappedHplmnSst));
    EXPECT_TRUE(test.mappedHplmnSd == testVector.mappedHplmnSD);
    EXPECT_TRUE(test.status == static_cast<int8_t>(testVector.status));
}

TEST(HookStructs, SliceInfo_toHidl) {
    aidl_data::SliceInfo testVector = {
        .sliceServiceType = NONE,
        .sliceDifferentiator = 2,
        .mappedHplmnSst = NONE,
        .mappedHplmnSd = 4,
        .status = ALLOWED,
        };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.sst == static_cast<V1_6::SliceServiceType>(testVector.sliceServiceType));
    EXPECT_TRUE(test.sliceDifferentiator == testVector.sliceDifferentiator);
    EXPECT_TRUE(test.mappedHplmnSst == static_cast<V1_6::SliceServiceType>(testVector.mappedHplmnSst));
    EXPECT_TRUE(test.mappedHplmnSD == testVector.mappedHplmnSd);
    EXPECT_TRUE(test.status == V1_6::SliceStatus{testVector.status});
}

TEST(HookStructs, TrafficDescriptor_toAidl) {
    V1_6::TrafficDescriptor testVector = getTrafficDescriptor_V1_6();
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.dnn == toAidl(testVector.dnn));
    EXPECT_TRUE(test.osAppId == toAidl(testVector.osAppId));
}

TEST(HookStructs, TrafficDescriptor_toHidl) {
    aidl_data::TrafficDescriptor testVector = getTrafficDescriptor_aidl();
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.dnn == toHidl<V1_6::OptionalDnn>(testVector.dnn));
    EXPECT_TRUE(test.osAppId == toHidl<V1_6::OptionalOsAppId>(testVector.osAppId));
}

TEST(HookStructs, OsAppId_toAidl) {
    V1_6::OsAppId testVector = {};
    testVector = getOsAppId_V1_6();
    auto test = compat::toAidl(testVector);
    for(int i = 0 ; i < length ; i++){
        EXPECT_TRUE(test.osAppId[i] == i);
    }
}

TEST(HookStructs, OsAppId_toHidl) {
    aidl_data::OsAppId testVector = {};
    testVector = getOsAppId_aidl();
    auto test = compat::toHidl(testVector);
    for(int i = 0 ; i < length ; i++){
        EXPECT_TRUE(test.osAppId[i] == i);
    }
}

TEST(HookStructs, KeepaliveRequest_toHidl) {
    std::vector<uint8_t> sourceAddress = {1, 2, 3, 4};
    std::vector<uint8_t> destinationAddress = {4, 3, 2, 1};

    aidl_data::KeepaliveRequest testVector = {
        .type = RIL_KeepaliveType::NATT_IPV4,
        .sourceAddress = sourceAddress,
        .sourcePort = 22,
        .destinationAddress = destinationAddress,
        .destinationPort = 33,
        .maxKeepaliveIntervalMillis = 9,
        .cid = 1,
        };
    auto test = compat::toHidl(testVector);
    EXPECT_TRUE(test.type == V1_1::KeepaliveType{testVector.type});
    EXPECT_TRUE(test.sourceAddress == testVector.sourceAddress);
    EXPECT_TRUE(test.sourcePort == testVector.sourcePort);
    EXPECT_TRUE(test.destinationAddress == testVector.destinationAddress);
    EXPECT_TRUE(test.destinationPort == testVector.destinationPort);
    EXPECT_TRUE(test.maxKeepaliveIntervalMillis == testVector.maxKeepaliveIntervalMillis);
    EXPECT_TRUE(test.cid == testVector.cid);
}

std::string address("192.0.1.11/16");
static V1_4::DataCallFailCause cause = (V1_4::DataCallFailCause)PDP_FAIL_NONE;
static int32_t suggestedRetryTime = 0;
static int32_t cid = 1;
static V1_4::DataConnActiveStatus active = (V1_4::DataConnActiveStatus)DATA_STATE_ACTIVE;
static V1_4::PdpProtocolType type = (V1_4::PdpProtocolType)PDP_PROTOCOL_TYPE_IP;
static std::string ifname("test ifname");
static std::vector<V1_5::LinkAddress> addresses = {{address, 0, 1, 2}};
static std::vector<hidl_string> dnses = {"192.0.1.3"};
static std::vector<hidl_string> gateways = {"2.10.11.31"};
static std::vector<hidl_string> pcscf = {"2001:db8::1 2001:db8::2 2001:db8::3"};
static int32_t mtuV4 = 1;
static int32_t mtuV6 = 0;

TEST(HookStructs, SetupDataCallResult_1_5_toAidl) {
    V1_5::SetupDataCallResult testVector = {
        .cause = cause,
        .suggestedRetryTime = suggestedRetryTime,
        .cid = cid,
        .active = active,
        .type = type,
        .ifname = ifname,
        .addresses = addresses,
        .dnses = dnses,
        .gateways = gateways,
        .pcscf = pcscf,
        .mtuV4 = mtuV4,
        .mtuV6 = mtuV6,
        };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cause == aidl_data::DataCallFailCause(testVector.cause));
    EXPECT_TRUE(test.suggestedRetryTime == testVector.suggestedRetryTime);
    EXPECT_TRUE(test.cid == testVector.cid);
    EXPECT_TRUE(test.active == static_cast<int32_t>(testVector.active));
    EXPECT_TRUE(test.type == aidl_data::PdpProtocolType(testVector.type));
    EXPECT_TRUE(test.ifname == testVector.ifname);
    EXPECT_TRUE(test.addresses == toAidl(testVector.addresses));
    EXPECT_TRUE(test.dnses == toAidl(testVector.dnses));
    EXPECT_TRUE(test.gateways == toAidl(testVector.gateways));
    EXPECT_TRUE(test.pcscf == toAidl(testVector.pcscf));
    EXPECT_TRUE(test.mtuV4 == testVector.mtuV4);
    EXPECT_TRUE(test.mtuV6 == testVector.mtuV6);
}

TEST(HookStructs, SetupDataCallResult_1_6_toAidl) {

    std::vector<hidl_string> address = {"192.0.1.3"};
    V1_6::SliceInfo sliceInfo = getSliceInfo_V1_6();
    V1_6::PortRange portRange = {3, 4};

    V1_6::Qos defaultQos = {};
    V1_6::EpsQos epsQos = {1, {1, 2}, {3, 4}};
    defaultQos.eps(epsQos);
    std::vector<V1_6::QosSession> qosSessions = {};
    qosSessions.resize(1);
    qosSessions[0].qosSessionId = 1;

    V1_6::NrQos nrQos = {1, {1, 2}, {3, 4}, 2, 3};
    qosSessions[0].qos.nr(nrQos);
    qosSessions[0].qosFilters.resize(1);

    V1_6::QosFilter qosFilter = {};
    qosFilter.localAddresses = address;
    qosFilter.remoteAddresses = address;
    qosFilter.localPort.range(portRange);
    qosFilter.remotePort.range(portRange);
    qosFilter.protocol = (V1_6::QosProtocol)UNSPECIFIED;
    qosFilter.tos.value(1);
    qosFilter.flowLabel.value(1);
    qosFilter.spi.value(1);
    qosFilter.direction = (V1_6::QosFilterDirection)BIDIRECTIONAL;
    qosFilter.precedence = -1;
    qosSessions[0].qosFilters[0] = qosFilter;
    V1_6::HandoverFailureMode handoverFailureMode = (V1_6::HandoverFailureMode)NO_FALLBACK_RETRY_HANDOVER;
    int32_t pduSessionId = 0;
    V1_6::OptionalSliceInfo optSliceInfo;
    optSliceInfo.value(sliceInfo);
    std::vector<V1_6::TrafficDescriptor> trafficDescriptors = {{getTrafficDescriptor_V1_6()}};

    V1_6::SetupDataCallResult testVector = {
        .cause = (V1_6::DataCallFailCause)cause,
        .suggestedRetryTime = suggestedRetryTime,
        .cid = cid,
        .active = active,
        .type = type,
        .ifname = ifname,
        .addresses = addresses,
        .dnses = dnses,
        .gateways = gateways,
        .pcscf = pcscf,
        .mtuV4 = mtuV4,
        .mtuV6 = mtuV6,
        .defaultQos = defaultQos,
        .qosSessions = qosSessions,
        .handoverFailureMode = handoverFailureMode,
        .pduSessionId = pduSessionId,
        .sliceInfo = optSliceInfo,
        .trafficDescriptors = trafficDescriptors,
    };

    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cause == aidl_data::DataCallFailCause(testVector.cause));
    EXPECT_TRUE(test.suggestedRetryTime == testVector.suggestedRetryTime);
    EXPECT_TRUE(test.cid == testVector.cid);
    EXPECT_TRUE(test.active == static_cast<int32_t>(testVector.active));
    EXPECT_TRUE(test.type == aidl_data::PdpProtocolType(testVector.type));
    EXPECT_TRUE(test.ifname == testVector.ifname);
    EXPECT_TRUE(test.addresses == toAidl(testVector.addresses));
    EXPECT_TRUE(test.dnses == toAidl(testVector.dnses));
    EXPECT_TRUE(test.gateways == toAidl(testVector.gateways));
    EXPECT_TRUE(test.pcscf == toAidl(testVector.pcscf));
    EXPECT_TRUE(test.mtuV4 == testVector.mtuV4);
    EXPECT_TRUE(test.mtuV6 == testVector.mtuV6);
    EXPECT_TRUE(test.qosSessions == toAidl(testVector.qosSessions));
    EXPECT_TRUE(test.handoverFailureMode == static_cast<int8_t>(testVector.handoverFailureMode));
    EXPECT_TRUE(test.pduSessionId == testVector.pduSessionId);
    EXPECT_TRUE(test.sliceInfo == toAidl(testVector.sliceInfo));
    EXPECT_TRUE(test.trafficDescriptors == toAidl(testVector.trafficDescriptors));
}

TEST(HookStructs, KeepaliveStatus_toAidl) {
    V1_1::KeepaliveStatus testVector = {
        .sessionHandle = 0,
        .code = (V1_1::KeepaliveStatusCode)0,
        };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.sessionHandle == testVector.sessionHandle);
    EXPECT_TRUE(test.code == static_cast<int32_t>(testVector.code));
}

TEST(HookStructs, PcoDataInfo_toAidl) {
    std::string bearerProto("test bearerProto");
    std::vector<uint8_t> contests = {1};

    V1_0::PcoDataInfo testVector = {
        .cid = 1,
        .bearerProto = bearerProto,
        .pcoId = 2,
        .contents = contests,
        };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.cid == testVector.cid);
    EXPECT_TRUE(test.bearerProto == testVector.bearerProto);
    EXPECT_TRUE(test.pcoId == testVector.pcoId);
}

TEST(HookStructs, SlicingConfig_toAidl) {
    V1_6::OptionalPdpProtocolType sessionType;
    sessionType.value((V1_4::PdpProtocolType)PDP_PROTOCOL_TYPE_IP);
    V1_6::OptionalSscMode sscMode;
    sscMode.value((V1_6::SscMode)MODE_1);

    // for SliceInfo
    std::vector<V1_6::SliceInfo> sliceInfos = {{getSliceInfo_V1_6()},};

    // for UrspRule
    V1_6::UrspRule urspRule = {};
    urspRule.precedence = 0;
    std::vector<V1_6::TrafficDescriptor> trafficDescriptors = {{getTrafficDescriptor_V1_6()}};

    urspRule.trafficDescriptors = {trafficDescriptors};

    V1_6::RouteSelectionDescriptor routeSelectionDescriptor= {};
    routeSelectionDescriptor.precedence = 0;
    routeSelectionDescriptor.sessionType = sessionType;
    routeSelectionDescriptor.sscMode = sscMode;
    routeSelectionDescriptor.sliceInfo = sliceInfos;
    routeSelectionDescriptor.dnn = {"test dnn"};
    std::vector<V1_6::RouteSelectionDescriptor> routeSelectionDescriptors = {routeSelectionDescriptor};
    urspRule.routeSelectionDescriptor = {routeSelectionDescriptors};
    std::vector<V1_6::UrspRule> urspRules = {urspRule};

    V1_6::SlicingConfig testVector = {
        .urspRules = urspRules,
        .sliceInfo = sliceInfos,
        };
    auto test = compat::toAidl(testVector);
    EXPECT_TRUE(test.urspRules == toAidl(testVector.urspRules));
    EXPECT_TRUE(test.sliceInfo == toAidl(testVector.sliceInfo));
}
