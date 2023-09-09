/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <librilutils/radio_oemhal_utils_1_2.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "Radio2HalUtils"
#endif
#include <utils/Log.h>

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using namespace std;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;

namespace Radio2HalUtils {

///////////////////////////////////////////////////////////////////////////
// radio 1.6 (Oem HAL 1.2) RIL => HAL convert function
///////////////////////////////////////////////////////////////////////////
static void fillDefaultQosParam(vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult& dcResult, const RIL_QosType qostype, const RIL_Qos qos, int filerparam) {
    if (qostype == EPS_QOS) {
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::EpsQos EpsQosInfo;
        EpsQosInfo.qci = qos.eps.qci;
        EpsQosInfo.uplink.guaranteedBitrateKbps = qos.eps.uplink.guaranteedBitrateKbps;
        EpsQosInfo.uplink.maxBitrateKbps = qos.eps.uplink.maxBitrateKbps;
        EpsQosInfo.downlink.guaranteedBitrateKbps = qos.eps.downlink.guaranteedBitrateKbps;
        EpsQosInfo.downlink.maxBitrateKbps = qos.eps.downlink.maxBitrateKbps;
        if (filerparam == 0) {
            dcResult.defaultQos.eps(EpsQosInfo);
        } else {
            dcResult.qosSessions[filerparam - 1].qos.eps(EpsQosInfo);
        }
    } else if (qostype == NR_QOS) {
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::NrQos NRQosInfo;
        NRQosInfo.fiveQi = qos.nr.fiveQi;
        NRQosInfo.uplink.guaranteedBitrateKbps = qos.nr.uplink.guaranteedBitrateKbps;
        NRQosInfo.uplink.maxBitrateKbps = qos.nr.uplink.maxBitrateKbps;
        NRQosInfo.downlink.guaranteedBitrateKbps = qos.nr.downlink.guaranteedBitrateKbps;
        NRQosInfo.downlink.maxBitrateKbps = qos.nr.downlink.maxBitrateKbps;
        NRQosInfo.qfi = qos.nr.qfi;
        NRQosInfo.averagingWindowMs = qos.nr.averagingWindowMs;
        if (filerparam == 0) {
            dcResult.defaultQos.nr(NRQosInfo);
        } else {
            dcResult.qosSessions[filerparam - 1].qos.nr(NRQosInfo);
        }
    }
}

static void fillOptSliceInfoParam(vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult& dcResult, const RIL_OptionalInfoFlag flag, const RIL_SliceInfo rilSliceInfo) {
    if (flag == RIL_OPT_INFO_VALID) {
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceInfo tmpSliceInfo = {};
        tmpSliceInfo.sst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType) rilSliceInfo.sst;
        tmpSliceInfo.sliceDifferentiator = rilSliceInfo.sliceDifferentiator;
        tmpSliceInfo.mappedHplmnSst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType) rilSliceInfo.mappedHplmnSst;
        tmpSliceInfo.mappedHplmnSD = rilSliceInfo.mappedHplmnSD;
        dcResult.sliceInfo.value(tmpSliceInfo);
    } else {
        RLOGD("No valid OSliInfo");
    }
}

/* Extended */
static void fillOptTDParam(vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult& dcResult, RIL_SetupDataCallResult_V1_6 *dcResponse) {
    dcResult.trafficDescriptors.resize(dcResponse->trafficDescriptors.size());

    for (std::size_t i = 0; i < dcResponse->trafficDescriptors.size(); ++i) {
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalDnn tmpDnn = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalOsAppId tmpOsAppID = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::OsAppId tmpOsAppIdValue = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::TrafficDescriptor tmpTDInfo = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalIpDescriptors tmpIPInfo = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalDomainDescriptors tmpDDInfo = {};
        vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalConnectionCapabilities tmpCCInfo = {};

        if (dcResponse->trafficDescriptors[i].dnnFlag == RIL_OPT_INFO_VALID) {
            tmpDnn.value(convertCharPtrToHidlString(dcResponse->trafficDescriptors[i].dnn[0].value));
            dcResult.trafficDescriptors[i].dnn= tmpDnn;
            RLOGD("fillOptTDParam, DNN : RIL_OPT_INFO_VALID");
        }
        if (dcResponse->trafficDescriptors[i].osAppIdFlag == RIL_OPT_INFO_VALID) {
            int length = strlen((char*) (dcResponse->trafficDescriptors[i].osAppId[0].value));
            tmpOsAppIdValue.osAppId.resize(length);
            for (int idx=0; idx < length; idx++)
                tmpOsAppIdValue.osAppId[idx] = dcResponse->trafficDescriptors[i].osAppId[0].value[idx];
            tmpOsAppID.value(tmpOsAppIdValue);
            RLOGD("fillOptTDParam, osAppId : RIL_OPT_INFO_VALID");
        }
        tmpTDInfo.dnn = tmpDnn;
        tmpTDInfo.osAppId = tmpOsAppID;
        tmpTDInfo.ipDescriptors = tmpIPInfo;
        tmpTDInfo.domainDescriptors = tmpDDInfo;
        tmpTDInfo.connectionCapabilities = tmpCCInfo;
        dcResult.trafficDescriptors[i] = tmpTDInfo;
    }
}

void convertLinkAddress(android::hardware::hidl_vec<::android::hardware::radio::V1_5::LinkAddress>&dst, char *str) {
    vector<string> ipAdd;
    string src(str ? str : "");
    string ip;
    stringstream ss(src);
    while (getline(ss, ip, ' '))
        ipAdd.push_back(ip);
    dst.resize(ipAdd.size());
    RLOGD("ipAdd size:%lu", (unsigned long)ipAdd.size());
    for(size_t i = 0; i < ipAdd.size(); i++) {
        dst[i].address =ipAdd[i].c_str();
        RLOGD("ipAdd[%zu] = %s", i, ipAdd[i].c_str());
        dst[i].properties = NONE;
        dst[i].deprecationTime = -1;
        dst[i].expirationTime = -1;
    }
}

void convertVector(android::hardware::hidl_vec<hidl_string>&dst2, std::vector<std::string>& src)
{
    dst2.resize(src.size());
    for(size_t i = 0; i < src.size(); i++)
    {
        dst2[i] = convertCharPtrToHidlString(src[i].c_str());
    }
}

void convertRilDataCallToHal(RIL_SetupDataCallResult_V1_6 *dcResponse,
                             vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult& dcResult) {
    /* 1.5 */
    dcResult.cause = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::DataCallFailCause)dcResponse->cause; // Undefined Numbers
    dcResult.suggestedRetryTime = dcResponse->suggestedRetryTime;
    dcResult.cid = dcResponse->cid;
    dcResult.active = (V1_4::DataConnActiveStatus) dcResponse->active;
    dcResult.type = (V1_4::PdpProtocolType) dcResponse->type;
    dcResult.ifname = convertCharPtrToHidlString(dcResponse->ifname);
    convertLinkAddress(dcResult.addresses , dcResponse->addresses);
    convertVector(dcResult.dnses, dcResponse->dnses);
    convertVector(dcResult.gateways, dcResponse->gateways);
    convertVector(dcResult.pcscf, dcResponse->pcscf);

    //dcResult->dnses = dcResponse.dnses;
    //dcResult->gateways = dcResponse.gateways;
    //dcResult->pcscf = dcResponse.pcscf;

    dcResult.mtuV4 = dcResponse->mtuV4;
    dcResult.mtuV6 = dcResponse->mtuV6;
    /* 1.5 */

    // (1) Default QoS
    fillDefaultQosParam(dcResult, dcResponse->defaultQosType, dcResponse->defaultQos, 0);

    // (2) QoS filters
    dcResult.qosSessions.resize(dcResponse->lenQosSessions);
    for (int i = 0; i < dcResponse->lenQosSessions; i++) {
        dcResult.qosSessions[i].qosSessionId= dcResponse->qosSessions[i].qosSessionId;
        fillDefaultQosParam(dcResult, dcResponse->qosSessions[i].qosType, dcResponse->qosSessions[i].qos, i + 1);

        dcResult.qosSessions[i].qosFilters.resize(dcResponse->qosSessions[i].lenQosFilters);

        for (int j = 0; j < dcResponse->qosSessions[i].lenQosFilters; j++) {
            convertVector(dcResult.qosSessions[i].qosFilters[j].localAddresses, dcResponse->qosSessions[i].qosFilters[j].localAddresses);
            convertVector(dcResult.qosSessions[i].qosFilters[j].remoteAddresses, dcResponse->qosSessions[i].qosFilters[j].remoteAddresses);

            vendor::samsung_slsi::telephony::hardware::radio::V1_2::PortRange portRange = {};
            if (dcResponse->qosSessions[i].qosFilters[j].localPortFlag == MAYBE_PORT_VALID) {
                portRange.start = dcResponse->qosSessions[i].qosFilters[j].localPort.range.start;
                portRange.end = dcResponse->qosSessions[i].qosFilters[j].localPort.range.end;
                dcResult.qosSessions[i].qosFilters[j].localPort.range(portRange);
            }

            if (dcResponse->qosSessions[i].qosFilters[j].remotePortFlag== MAYBE_PORT_VALID) {
                portRange.start = dcResponse->qosSessions[i].qosFilters[j].remotePort.range.start;
                portRange.end = dcResponse->qosSessions[i].qosFilters[j].remotePort.range.end;
                dcResult.qosSessions[i].qosFilters[j].remotePort.range(portRange);
            }

            dcResult.qosSessions[i].qosFilters[j].protocol = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::QosProtocol)dcResponse->qosSessions[i].qosFilters[j].protocol;

            if (dcResponse->qosSessions[i].qosFilters[j].tosFlag != 0) {
                dcResult.qosSessions[i].qosFilters[j].tos.value(dcResponse->qosSessions[i].qosFilters[j].tos);
            }

            if (dcResponse->qosSessions[i].qosFilters[j].flowLabelFlag == IPV6_FLOW_LABEL_VALID) {
                dcResult.qosSessions[i].qosFilters[j].flowLabel.value(dcResponse->qosSessions[i].qosFilters[j].flowLabel.value);
            }

            if (dcResponse->qosSessions[i].qosFilters[j].spiFlag == IP_SEC_SPI_VALID) {
                dcResult.qosSessions[i].qosFilters[j].spi.value(dcResponse->qosSessions[i].qosFilters[j].spi.value);
            }

            dcResult.qosSessions[i].qosFilters[j].direction = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::QosFilterDirection)dcResponse->qosSessions[i].qosFilters[j].direction;
            dcResult.qosSessions[i].qosFilters[j].precedence = dcResponse->qosSessions[i].qosFilters[j].precedence;
        }
    }

    // (3) HandOverFailureMode
    dcResult.handoverFailureMode = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::HandoverFailureMode) NO_FALLBACK_RETRY_HANDOVER;
    RLOGD("handoverFailureMode %d", (int) dcResult.handoverFailureMode);

    // (4) PduSessionId
    dcResult.pduSessionId = dcResponse->pduSessionId;

    // (5) Optional SliceInfo
    fillOptSliceInfoParam(dcResult, dcResponse->sliceInfoFlag, dcResponse->sliceInfo);

    // (6) Traffic Descriptor
    fillOptTDParam(dcResult, dcResponse);
}

void convertRilDataCallListToHal(void *response, size_t responseLen,
                                 hidl_vec<vendor::samsung_slsi::telephony::hardware::radio::V1_2::SetupDataCallResult>& dcResultList) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_SetupDataCallResult_V1_6);

    RIL_SetupDataCallResult_V1_6 *dcResponse = (RIL_SetupDataCallResult_V1_6 *) response;
    dcResultList.resize(num);
    for (int i = 0; i < num; i++) {
        convertRilDataCallToHal(&dcResponse[i], dcResultList[i]);
    }
}

void convertRilDataCallToHal(RIL_SetupDataCallResult_V1_6 *dcResponse, V1_5::SetupDataCallResult& dcResult) {
    if (dcResponse == nullptr) {
        return ;
    }

    dcResult.cause =  (V1_4::DataCallFailCause)dcResponse->cause;
    dcResult.suggestedRetryTime = dcResponse->suggestedRetryTime;
    dcResult.cid = dcResponse->cid;
    dcResult.active = (V1_4::DataConnActiveStatus) dcResponse->active;
    dcResult.type = (V1_4::PdpProtocolType) dcResponse->type;
    dcResult.ifname = convertCharPtrToHidlString(dcResponse->ifname);
    convertLinkAddress(dcResult.addresses , dcResponse->addresses);
    convertVector(dcResult.dnses, dcResponse->dnses);
    convertVector(dcResult.gateways, dcResponse->gateways);
    convertVector(dcResult.pcscf, dcResponse->pcscf);
    dcResult.mtuV4 = dcResponse->mtuV4;
    dcResult.mtuV6 = dcResponse->mtuV6;
}

void convertRilSlicingConfigToHal(void *dcResponse, vendor::samsung_slsi::telephony::hardware::radio::V1_2::SlicingConfig& dcResult){
    if (dcResponse == nullptr) {
        return ;
    }

    RLOGD("convertRilSlicingConfigToHal");

    RIL_SlicingConfig * rilSlicingConfig = (RIL_SlicingConfig*) dcResponse;


    int urspRule_count = 0;
    int trafficDescriptor_count = 0;
    int routeSelectionDescriptor_count = 0;
    int sliceInfo_count = 0;
    int dnn_count = 0;

    //it_1 == urspRules : precedence, trafficDescriptors, routeSelectionDescriptor
    dcResult.urspRules.resize(rilSlicingConfig->urspNum);
    for (std::vector<RIL_UrspRule >::iterator it_1 = rilSlicingConfig->urspRules.begin()  ; it_1 != rilSlicingConfig->urspRules.end() ; it_1++, urspRule_count++)
    {
        dcResult.urspRules[urspRule_count].precedence = it_1->precedence;

        //it_2 == trafficDescriptors : dnn, osAppId
        //trafficDescriptor num do not come from CP. Just 1 now.
        trafficDescriptor_count = 0;
        dcResult.urspRules[urspRule_count].trafficDescriptors.resize(trafficDescriptor_count + 1);
        for (std::vector<RIL_TrafficDescriptor >::iterator it_2 = it_1->trafficDescriptors.begin(); it_2 != it_1->trafficDescriptors.end(); it_2++, trafficDescriptor_count++)
        {
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalDnn tmpDnn = {};
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::OptionalOsAppId tmpOsAppID = {};
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::OsAppId tmpOsAppIdValue = {};
            //tmpTDInfo data type was TrafficDescriptor_aosp
            vendor::samsung_slsi::telephony::hardware::radio::V1_2::TrafficDescriptor tmpTDInfo = {};

            //dnn
            tmpDnn.value(convertCharPtrToHidlString(it_2->dnn[0].value));

            //osAppId
            int length = strlen((char*) (it_2->osAppId[0].value));
            tmpOsAppIdValue.osAppId.resize(length);
            for(int idx=0;idx<length;idx++)
                tmpOsAppIdValue.osAppId[idx] = it_2->osAppId[0].value[idx];
            tmpOsAppID.value(tmpOsAppIdValue);

            tmpTDInfo.dnn = tmpDnn;
            tmpTDInfo.osAppId = tmpOsAppID;
            dcResult.urspRules[urspRule_count].trafficDescriptors[trafficDescriptor_count] = tmpTDInfo;
        }

        //it_2 == RouteSelectionDescriptor : precedence, routeSelectionDescriptorParams
        routeSelectionDescriptor_count = 0;
        dcResult.urspRules[urspRule_count].routeSelectionDescriptor.resize(it_1->RSDNum);
        for (std::vector<RIL_RouteSelectionDescriptor >::iterator it_2 = it_1->routeSelectionDescriptor.begin() ; it_2 != it_1->routeSelectionDescriptor.end(); it_2++, routeSelectionDescriptor_count++) {
            //printf("Descriptor precedence : %d \n", it_2->precedence);
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].precedence = it_2->precedence;
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sessionType.value((V1_4::PdpProtocolType)it_2->sessionType.value);
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sscMode.value((vendor::samsung_slsi::telephony::hardware::radio::V1_2::SscMode)it_2->sscMode.value);

            //it_3 == SliceInfo
            sliceInfo_count = 0;
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo.resize(it_2->sliceNum);
            for (std::vector<RIL_SliceInfo >::iterator it_3 = it_2->sliceInfo.begin(); it_3 != it_2->sliceInfo.end(); it_3++, sliceInfo_count++)   {
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].sst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_3->sst;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].sliceDifferentiator = it_3->sliceDifferentiator;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].mappedHplmnSst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_3->mappedHplmnSst;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].mappedHplmnSD = it_3->mappedHplmnSD;
                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].sliceInfo[sliceInfo_count].status = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceStatus)it_3->status;

            }

            //it_3 == dnn
            dnn_count = 0;
            dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].dnn.resize(it_2->dnnNum);
            for (std::vector<RIL_OptionalDNN >::iterator it_3 = it_2->dnn.begin(); it_3 != it_2->dnn.end(); it_3++, dnn_count++) {

                dcResult.urspRules[urspRule_count].routeSelectionDescriptor[routeSelectionDescriptor_count].dnn[dnn_count] = convertCharPtrToHidlString(it_3->value);
            }
        }
    }

    sliceInfo_count = 0;
    dcResult.sliceInfo.resize(rilSlicingConfig->sliceInfoNum);
    for (std::vector<RIL_SliceInfo >::iterator it_1 = rilSlicingConfig->sliceInfo.begin(); it_1 != rilSlicingConfig->sliceInfo.end(); it_1++, sliceInfo_count++)
    {
        dcResult.sliceInfo[sliceInfo_count].sst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_1->sst;
        dcResult.sliceInfo[sliceInfo_count].sliceDifferentiator = it_1->sliceDifferentiator;
        dcResult.sliceInfo[sliceInfo_count].mappedHplmnSst = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceServiceType)it_1->mappedHplmnSst;
        dcResult.sliceInfo[sliceInfo_count].mappedHplmnSD = it_1->mappedHplmnSD;
        dcResult.sliceInfo[sliceInfo_count].status = (vendor::samsung_slsi::telephony::hardware::radio::V1_2::SliceStatus)it_1->status;
    }

}


} // namespace Radio2HalUtils

