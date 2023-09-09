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
#include "vendorstructs.h"
#include <aidl-log.h>

namespace aidl_data = ::aidl::android::hardware::radio::data;

template<typename T>
static void fillQosSessions(aidl_data::SetupDataCallResult& dcResult, const T &qosInfo, uint32_t fill_idx) {
    if (fill_idx == 0) {
        dcResult.defaultQos = qosInfo;
    } else {
        dcResult.qosSessions[fill_idx - 1].qos = qosInfo;
    }
}

static void fillDefaultQosParam(aidl_data::SetupDataCallResult& dcResult,
                                const RIL_QosType qostype, const RIL_Qos &qos, uint32_t filerparam) {
    if (qostype == EPS_QOS) {
        aidl_data::EpsQos EpsQosInfo;
        EpsQosInfo.qci = static_cast<int32_t>(qos.eps.qci);
        EpsQosInfo.uplink.guaranteedBitrateKbps = static_cast<int32_t>(qos.eps.uplink.guaranteedBitrateKbps);
        EpsQosInfo.uplink.maxBitrateKbps = static_cast<int32_t>(qos.eps.uplink.maxBitrateKbps);
        EpsQosInfo.downlink.guaranteedBitrateKbps = static_cast<int32_t>(qos.eps.downlink.guaranteedBitrateKbps);
        EpsQosInfo.downlink.maxBitrateKbps = static_cast<int32_t>(qos.eps.downlink.maxBitrateKbps);
        fillQosSessions(dcResult, EpsQosInfo, filerparam);
    } else if (qostype == NR_QOS) {
        aidl_data::NrQos NRQosInfo;
        NRQosInfo.fiveQi = static_cast<int32_t>(qos.nr.fiveQi);
        NRQosInfo.uplink.guaranteedBitrateKbps = static_cast<int32_t>(qos.nr.uplink.guaranteedBitrateKbps);
        NRQosInfo.uplink.maxBitrateKbps = static_cast<int32_t>(qos.nr.uplink.maxBitrateKbps);
        NRQosInfo.downlink.guaranteedBitrateKbps = static_cast<int32_t>(qos.nr.downlink.guaranteedBitrateKbps);
        NRQosInfo.downlink.maxBitrateKbps = static_cast<int32_t>(qos.nr.downlink.maxBitrateKbps);
        NRQosInfo.qfi = static_cast<int8_t>(qos.nr.qfi);
        NRQosInfo.averagingWindowMs = static_cast<char>(qos.nr.averagingWindowMs);
        fillQosSessions(dcResult, NRQosInfo, filerparam);
    }
}

static void fillOptSliceInfoParam(aidl_data::SetupDataCallResult& dcResult,
                                  const RIL_OptionalInfoFlag flag, const RIL_SliceInfo &rilSliceInfo) {
    if (flag == RIL_OPT_INFO_VALID) {
        aidl_data::SliceInfo tmpSliceInfo = {};
        tmpSliceInfo.sliceServiceType = static_cast<int8_t>(rilSliceInfo.sst);
        tmpSliceInfo.sliceDifferentiator = static_cast<int32_t>(rilSliceInfo.sliceDifferentiator);
        tmpSliceInfo.mappedHplmnSst =  static_cast<int8_t>(rilSliceInfo.mappedHplmnSst);
        tmpSliceInfo.mappedHplmnSd = static_cast<int32_t>(rilSliceInfo.mappedHplmnSD);
        dcResult.sliceInfo = tmpSliceInfo;
    } else {
        RLOGD("No valid OSliInfo");
    }
}

static void copyTrafficDescriptor(aidl_data::TrafficDescriptor &aidl_td,
                        const RIL_TrafficDescriptor &ril_td) {

    if (ril_td.dnnFlag == RIL_OPT_INFO_VALID) {
        aidl_td.dnn = static_cast<std::string>(ril_td.dnn[0].value);
        RLOGD("DNN: Found dnn %s", aidl_td.dnn.value().c_str());
    }
    if (ril_td.osAppIdFlag == RIL_OPT_INFO_VALID) {
        constexpr int osappid_offset = MAX_OS_ID_LENGTH + 1;
        const size_t appid_len = ril_td.osAppId[0].value[MAX_OS_ID_LENGTH];
        int length = osappid_offset + appid_len;
        aidl_data::OsAppId tmp;
        for (int idx = 0; idx < length; idx++) {
            tmp.osAppId.push_back(ril_td.osAppId[0].value[idx]);
        }
        aidl_td.osAppId = tmp;
        RLOGD("OsAppID: Found osAppId %s", aidl_td.osAppId.value().toString().c_str());
    }
}

static void copyTrafficDescriptor(aidl_data::TrafficDescriptor &aidl_td,
                        const std::optional<DataV2_0::RIL_TrafficDescriptor> &ril_td) {
    if (!ril_td.has_value()) return;

    if (!ril_td->dnn.empty()) {
        aidl_td.dnn = ril_td->dnn[0];
        RLOGD("DNN: Found dnn %s", aidl_td.dnn.value().c_str());
    }
    if (!ril_td->osAppIds.empty()) {
        constexpr int osappid_offset = MAX_OS_ID_LENGTH + 1;
        const size_t appid_len = ril_td->osAppIds[0].value[MAX_OS_ID_LENGTH];
        int length = osappid_offset + appid_len;
        aidl_data::OsAppId tmp;
        for (int idx = 0; idx < length; idx++) {
            tmp.osAppId.push_back(ril_td->osAppIds[0].value[idx]);
        }
        aidl_td.osAppId = tmp;
        RLOGD("OsAppID: Found osAppId %s", aidl_td.osAppId.value().toString().c_str());
    }
}

static void fillOptTDParam(aidl_data::SetupDataCallResult& dcResult,
                           const RIL_SetupDataCallResult_V1_6 &dcResponse) {
    dcResult.trafficDescriptors.resize(dcResponse.trafficDescriptors.size());

    for (std::size_t i = 0; i < dcResponse.trafficDescriptors.size(); ++i) {
        copyTrafficDescriptor(dcResult.trafficDescriptors[i], dcResponse.trafficDescriptors[i]);
    }
}

void convertLinkAddress(std::vector<aidl_data::LinkAddress> &dst, const char *str) {
    std::vector<std::string> ipAdd;
    std::string src(str ? str : "");
    std::string ip;
    std::stringstream ss(src);
    while (getline(ss, ip, ' ')) {
        ipAdd.push_back(ip);
    }
    dst.resize(ipAdd.size());
    RLOGD("ipAdd size:%lu", (unsigned long)ipAdd.size());
    for (size_t i = 0; i < ipAdd.size(); i++) {
        dst[i].address = static_cast<std::string>(ipAdd[i].c_str());
        RLOGD("ipAdd[%zu] = %s", i, ipAdd[i].c_str());
        dst[i].addressProperties = 0;
        dst[i].deprecationTime = -1;
        dst[i].expirationTime = -1;
    }
}

namespace android::hardware::radio::impl {

aidl_data::SetupDataCallResult toAidl(const RIL_SetupDataCallResult_V1_6 &dcResponse) {
    aidl_data::SetupDataCallResult dcResult;

    dcResult.cause = (aidl_data::DataCallFailCause)dcResponse.cause;
    dcResult.suggestedRetryTime = static_cast<int64_t>(dcResponse.suggestedRetryTime);
    dcResult.cid = static_cast<int32_t>(dcResponse.cid);
    dcResult.active = static_cast<int32_t>(dcResponse.active);
    dcResult.type = (aidl_data::PdpProtocolType) dcResponse.type;
    if (dcResponse.ifname != nullptr) {
        dcResult.ifname = static_cast<std::string>(dcResponse.ifname);
    }

    convertLinkAddress(dcResult.addresses, dcResponse.addresses);
    dcResult.dnses = dcResponse.dnses;
    dcResult.gateways = dcResponse.gateways;
    dcResult.pcscf = dcResponse.pcscf;

    dcResult.mtuV4 = dcResponse.mtuV4;
    dcResult.mtuV6 = dcResponse.mtuV6;

    // (1) Default QoS
    fillDefaultQosParam(dcResult, dcResponse.defaultQosType, dcResponse.defaultQos, 0);

    // (2) QoS filters
    dcResult.qosSessions.resize(dcResponse.lenQosSessions);
    for (int i = 0; i < dcResponse.lenQosSessions; i++) {
        aidl_data::QosSession &aidlQos = dcResult.qosSessions[i];
        const RIL_QosSession &rilQos = dcResponse.qosSessions[i];

        aidlQos.qosSessionId = rilQos.qosSessionId;
        fillDefaultQosParam(dcResult, rilQos.qosType, rilQos.qos, i + 1);

        aidlQos.qosFilters.resize(rilQos.lenQosFilters);

        for (int j = 0; j < rilQos.lenQosFilters; j++) {
            aidl_data::QosFilter &aidlQosFilter = aidlQos.qosFilters[j];
            const RIL_QosFilter &rilQosFilter = rilQos.qosFilters[j];

            aidlQosFilter.localAddresses = rilQosFilter.localAddresses;
            aidlQosFilter.remoteAddresses = rilQosFilter.remoteAddresses;

            aidl_data::PortRange portRange = {};
            if (rilQosFilter.localPortFlag == MAYBE_PORT_VALID) {
                portRange.start = static_cast<int32_t>(rilQosFilter.localPort.range.start);
                portRange.end = static_cast<int32_t>(rilQosFilter.localPort.range.end);
                aidlQosFilter.localPort = portRange;
            }

            if (rilQosFilter.remotePortFlag == MAYBE_PORT_VALID) {
                portRange.start = static_cast<int32_t>(rilQosFilter.remotePort.range.start);
                portRange.end = static_cast<int32_t>(rilQosFilter.remotePort.range.end);
                aidlQosFilter.remotePort = portRange;
            }

            aidlQosFilter.protocol = static_cast<int8_t>(rilQosFilter.protocol);

            if (rilQosFilter.tosFlag != 0) {
                aidlQosFilter.tos = static_cast<int8_t>(rilQosFilter.tos);
            }

            if (rilQosFilter.flowLabelFlag == IPV6_FLOW_LABEL_VALID) {
                aidlQosFilter.flowLabel = static_cast<int32_t>(rilQosFilter.flowLabel.value);
            }

            if (rilQosFilter.spiFlag == IP_SEC_SPI_VALID) {
                aidlQosFilter.spi = static_cast<int32_t>(rilQosFilter.spi.value);
            }

            aidlQosFilter.direction= static_cast<int8_t>(rilQosFilter.direction);
            aidlQosFilter.precedence = static_cast<int32_t>(rilQosFilter.precedence);
        }
    }

    //(3) HandOverFailureMode
    /* Related issue - https://partnerissuetracker.corp.google.com/issues/170656064
    //Addi-https://partnerissuetracker.corp.google.com/issues/198626604
    https://partnerissuetracker.corp.google.com/u/0/issues/201037349 */
    if (dcResponse.cause == PDP_FAIL_PDN_CONN_DOES_NOT_EXIST  ||
        dcResponse.cause == PDP_FAIL_PAYLOAD_NOT_FORWARDED) {
        dcResult.handoverFailureMode = static_cast<int8_t>(NO_FALLBACK_RETRY_SETUP_NORMAL);
    } else {
        dcResult.handoverFailureMode = static_cast<int8_t>(NO_FALLBACK_RETRY_HANDOVER);
    }

    //(4) PduSessionId
    dcResult.pduSessionId = static_cast<int32_t>(dcResponse.pduSessionId);

    //(5) Optional SliceInfo
    fillOptSliceInfoParam(dcResult, dcResponse.sliceInfoFlag, dcResponse.sliceInfo);

    //(6) Traffic Descriptor
    fillOptTDParam(dcResult, dcResponse);

    return dcResult;
}

aidl_data::DataProfileInfo toAidl(const DataV2_0::RIL_DataProfileInfo &dataProfileInfo) {

    aidl_data::DataProfileInfo dataProfileResult = {};

    dataProfileResult.profileId = static_cast<int32_t>(dataProfileInfo.profileId);

    // TODO: Need to consider null apn case for NULL IA APN purpose
    dataProfileResult.apn = dataProfileInfo.apn;
    dataProfileResult.protocol = (aidl_data::PdpProtocolType)dataProfileInfo.protocol;
    dataProfileResult.roamingProtocol = (aidl_data::PdpProtocolType)dataProfileInfo.roamingProtocol;
    dataProfileResult.authType = (aidl_data::ApnAuthType)dataProfileInfo.authType;
    dataProfileResult.user = dataProfileInfo.user.empty() ? "" : dataProfileInfo.user;
    dataProfileResult.password = dataProfileInfo.password.empty() ? "" : dataProfileInfo.password;
    dataProfileResult.type = static_cast<int32_t>(dataProfileInfo.type);
    dataProfileResult.maxConnsTime = static_cast<int32_t>(dataProfileInfo.maxConnsTime);
    dataProfileResult.maxConns = static_cast<int32_t>(dataProfileInfo.maxConns);
    dataProfileResult.waitTime = static_cast<int32_t>(dataProfileInfo.waitTime);
    dataProfileResult.enabled = static_cast<bool>(dataProfileInfo.enabled);
    dataProfileResult.supportedApnTypesBitmap = static_cast<int32_t>(dataProfileInfo.supportedApnTypesBitmap);
    dataProfileResult.bearerBitmap = static_cast<int32_t>(dataProfileInfo.bearerBitmap);
    dataProfileResult.mtuV4 = static_cast<int32_t>(dataProfileInfo.mtuV4);
    dataProfileResult.mtuV6 = static_cast<int32_t>(dataProfileInfo.mtuV6);
    dataProfileResult.preferred = static_cast<bool>(dataProfileInfo.preferred);
    dataProfileResult.persistent = static_cast<bool>(dataProfileInfo.persistent);
    dataProfileResult.alwaysOn = static_cast<bool>(dataProfileInfo.alwaysOn);

    copyTrafficDescriptor(dataProfileResult.trafficDescriptor, dataProfileInfo.tdInfo);

    return dataProfileResult;
}

}
