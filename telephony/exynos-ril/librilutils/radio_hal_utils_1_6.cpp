/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <librilutils/radio_hal_utils_1_6.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "Radio2HalUtils"
#endif
#include <utils/Log.h>
#include <librilutils/radio_hal_utils.h>

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using namespace std;

using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;

namespace Radio2HalUtils {

///////////////////////////////////////////////////////////////////////////
// radio 1.6  RIL => HAL convert function
///////////////////////////////////////////////////////////////////////////

void convertRilPhysicalChannelConfigToHal(void *response, size_t responseLen,
    V1_6::PhysicalChannelConfig& config) {
    if (response == NULL || responseLen != sizeof(RIL_PhysicalChannelConfig_V1_6)) {
        return ;
    }

    RIL_PhysicalChannelConfig_V1_6 *rilConfig = (RIL_PhysicalChannelConfig_V1_6 *)response;
    config.status = (V1_2::CellConnectionStatus)rilConfig->status;
    config.rat = (V1_4::RadioTechnology)rilConfig->rat;
    config.downlinkChannelNumber = rilConfig->downlinkChannelNumber;
    config.uplinkChannelNumber = rilConfig->uplinkChannelNumber;
    config.cellBandwidthDownlinkKhz = rilConfig->cellBandwidthDownlink;
    config.cellBandwidthUplinkKhz = rilConfig->cellBandwidthUplink;
    config.contextIds.resize(0);
    if (rilConfig->len_contextIds > 0) {
        config.contextIds.resize(rilConfig->len_contextIds);
        for (int i = 0; i < rilConfig->len_contextIds; i++) {
            config.contextIds[i] = rilConfig->contextIds[i];
        }
    }
    config.physicalCellId = rilConfig->physicalCellId;

    switch(RadioConvUtils::getRadioTechnologyToAccessNetworkType((int)rilConfig->rat)) {
        case ACCESS_NETWORK_GERAN:
            config.band.geranBand((V1_1::GeranBands)rilConfig->band.geranBand);
            break;
        case ACCESS_NETWORK_UTRAN:
            config.band.utranBand((V1_1::UtranBands)rilConfig->band.utranBand);
            break;
        case ACCESS_NETWORK_EUTRAN:
            config.band.eutranBand((V1_1::EutranBands)rilConfig->band.eutranBand);
            break;
        case ACCESS_NETWORK_NGRAN:
            config.band.ngranBand((V1_6::NgranBands)rilConfig->band.ngranBand);
            break;
        default:
            break;
    }
}

void parseString(hidl_vec<hidl_string>&dst, char *str) {
    vector<string> temp;
    string src(str ? str : "");
    stringstream ss(src);
    copy(istream_iterator<string>(ss), istream_iterator<string>(), back_inserter(temp));
    dst.resize(temp.size());
    for(size_t i = 0; i < temp.size(); i++)
        dst[i] = temp[i];
}

void convertVector(hidl_vec<hidl_string>&dst2, const vector<string>&src)
{
    dst2.resize(src.size());
    for(size_t i = 0; i < src.size(); i++)
    {
        dst2[i] = convertCharPtrToHidlString(src[i].c_str());
    }
}

void convertLinkAddress(hidl_vec<V1_5::LinkAddress>&dst, const char *str) {
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

static void fillDefaultQosParam(V1_6::SetupDataCallResult& dcResult, const RIL_QosType qostype, const RIL_Qos qos, int filerparam) {
     if (qostype == EPS_QOS) {
         V1_6::EpsQos EpsQosInfo;
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
     } else if(qostype == NR_QOS) {
         V1_6::NrQos NRQosInfo;
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

static void fillOptSliceInfoParam(V1_6::SetupDataCallResult& dcResult, const RIL_OptionalInfoFlag flag, const RIL_SliceInfo& rilSliceInfo) {
    if (flag == RIL_OPT_INFO_VALID) {
        V1_6::SliceInfo tmpSliceInfo = {};
        tmpSliceInfo.sst = (V1_6::SliceServiceType) rilSliceInfo.sst;
        tmpSliceInfo.sliceDifferentiator = rilSliceInfo.sliceDifferentiator;
        tmpSliceInfo.mappedHplmnSst = (V1_6::SliceServiceType) rilSliceInfo.mappedHplmnSst;
        tmpSliceInfo.mappedHplmnSD = rilSliceInfo.mappedHplmnSD;
        dcResult.sliceInfo.value(tmpSliceInfo);
    } else {
        RLOGD("No valid OSliInfo");
    }
}

static void fillRilOsAppIdToHal(const RIL_OptionalOSAppId &rilOsAppId, V1_6::OptionalOsAppId &halOsAppId) {
    constexpr int osappid_offset = 16 + 1;
    int length = rilOsAppId.value[16] + osappid_offset;
    V1_6::OsAppId tmpOsAppIdValue;
    tmpOsAppIdValue.osAppId.resize(length);
    for (int idx = 0; idx < length; idx++) {
        tmpOsAppIdValue.osAppId[idx] = rilOsAppId.value[idx];
    }
    halOsAppId.value(tmpOsAppIdValue);
}

static void fillOptTDParam(V1_6::SetupDataCallResult& dcResult, const RIL_SetupDataCallResult_V1_6 &dcResponse) {
    dcResult.trafficDescriptors.resize(dcResponse.trafficDescriptors.size());

    for (std::size_t i = 0; i < dcResponse.trafficDescriptors.size(); ++i) {
        V1_6::OptionalDnn tmpDnn;
        V1_6::OptionalOsAppId tmpOsAppID;
        V1_6::OsAppId tmpOsAppIdValue;
        V1_6::TrafficDescriptor tmpTDInfo;

        if (dcResponse.trafficDescriptors[i].dnnFlag == RIL_OPT_INFO_VALID) {
            tmpDnn.value(convertCharPtrToHidlString(dcResponse.trafficDescriptors[i].dnn[0].value));
            dcResult.trafficDescriptors[i].dnn = tmpDnn;
            RLOGD("fillOptTDParam, DNN : RIL_OPT_INFO_VALID");
        }
        if (dcResponse.trafficDescriptors[i].osAppIdFlag == RIL_OPT_INFO_VALID) {
            fillRilOsAppIdToHal(dcResponse.trafficDescriptors[i].osAppId[0], tmpOsAppID);
            RLOGD("fillOptTDParam, osAppId : RIL_OPT_INFO_VALID");
        }
        tmpTDInfo = {tmpDnn, tmpOsAppID};
        dcResult.trafficDescriptors[i] = tmpTDInfo;
    }
}

void convertRilDataCallToHal(const RIL_SetupDataCallResult_V1_6 &dcResponse, V1_6::SetupDataCallResult& dcResult) {

    dcResult.cause = (V1_6::DataCallFailCause)dcResponse.cause;
    dcResult.suggestedRetryTime = dcResponse.suggestedRetryTime;
    dcResult.cid = dcResponse.cid;
    dcResult.active = (V1_4::DataConnActiveStatus) dcResponse.active;
    dcResult.type = (V1_4::PdpProtocolType) dcResponse.type;
    dcResult.ifname = convertCharPtrToHidlString(dcResponse.ifname);
    convertLinkAddress(dcResult.addresses , dcResponse.addresses);
    convertVector(dcResult.dnses, dcResponse.dnses);
    convertVector(dcResult.gateways, dcResponse.gateways);
    convertVector(dcResult.pcscf, dcResponse.pcscf);
    dcResult.mtuV4 = dcResponse.mtuV4;
    dcResult.mtuV6 = dcResponse.mtuV6;

    // (1) Default QoS
    fillDefaultQosParam(dcResult, dcResponse.defaultQosType, dcResponse.defaultQos, 0);

    // (2) QoS filters
    dcResult.qosSessions.resize(dcResponse.lenQosSessions);
    for (int i = 0; i < dcResponse.lenQosSessions; i++) {
        V1_6::QosSession &hidlQos = dcResult.qosSessions[i];
        const RIL_QosSession &rilQos = dcResponse.qosSessions[i];

        hidlQos.qosSessionId = rilQos.qosSessionId;
        fillDefaultQosParam(dcResult, rilQos.qosType, rilQos.qos, i + 1);

        hidlQos.qosFilters.resize(rilQos.lenQosFilters);

        for (int j = 0; j < rilQos.lenQosFilters; j++) {
            V1_6::QosFilter &hidlQosFilter = hidlQos.qosFilters[j];
            const RIL_QosFilter &rilQosFilter = rilQos.qosFilters[j];

            convertVector(hidlQosFilter.localAddresses, rilQosFilter.localAddresses);
            convertVector(hidlQosFilter.remoteAddresses, rilQosFilter.remoteAddresses);

            V1_6::PortRange portRange = {};
            if (rilQosFilter.localPortFlag == MAYBE_PORT_VALID) {
                portRange.start = rilQosFilter.localPort.range.start;
                portRange.end = rilQosFilter.localPort.range.end;
                hidlQosFilter.localPort.range(portRange);
            }

            if (rilQosFilter.remotePortFlag == MAYBE_PORT_VALID) {
                portRange.start = rilQosFilter.remotePort.range.start;
                portRange.end = rilQosFilter.remotePort.range.end;
                hidlQosFilter.remotePort.range(portRange);
            }

            hidlQosFilter.protocol = (V1_6::QosProtocol)rilQosFilter.protocol;

            if (rilQosFilter.tosFlag != 0) {
                hidlQosFilter.tos.value(rilQosFilter.tos);
            }

            if (rilQosFilter.flowLabelFlag == IPV6_FLOW_LABEL_VALID) {
                hidlQosFilter.flowLabel.value(rilQosFilter.flowLabel.value);
            }

            if (rilQosFilter.spiFlag == IP_SEC_SPI_VALID) {
                hidlQosFilter.spi.value(rilQosFilter.spi.value);
            }

            hidlQosFilter.direction = (V1_6::QosFilterDirection)rilQosFilter.direction;
            hidlQosFilter.precedence = rilQosFilter.precedence;
        }
    }

    // (3) HandOverFailureMode
    /* Related issue - b/170656064, b/198626604, b/201037349 */
    if (dcResponse.cause == PDP_FAIL_PDN_CONN_DOES_NOT_EXIST ||
       dcResponse.cause == PDP_FAIL_PAYLOAD_NOT_FORWARDED) {
        dcResult.handoverFailureMode = (V1_6::HandoverFailureMode) NO_FALLBACK_RETRY_SETUP_NORMAL;
    } else {
        dcResult.handoverFailureMode = (V1_6::HandoverFailureMode) NO_FALLBACK_RETRY_HANDOVER;
    }

    // (4) PduSessionId
    dcResult.pduSessionId = dcResponse.pduSessionId;

    // (5) Optional SliceInfo
    fillOptSliceInfoParam(dcResult, dcResponse.sliceInfoFlag, dcResponse.sliceInfo);

    // (6) Traffic Descriptor
    fillOptTDParam(dcResult, dcResponse);
}

void convertRilDataCallListToHal(const void *response, size_t responseLen,
        hidl_vec<V1_6::SetupDataCallResult>& dcResultList) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_SetupDataCallResult_V1_6);

    const RIL_SetupDataCallResult_V1_6 *dcResponse = static_cast<const RIL_SetupDataCallResult_V1_6 *>(response);
    dcResultList.resize(num);
    for (int i = 0; i < num; i++) {
        convertRilDataCallToHal(dcResponse[i], dcResultList[i]);
    }
}

bool convertRilCallToHal(RIL_Call_V1_6 *rilCall, size_t rilCallLen, V1_6::Call& call) {
    if (rilCall == NULL || rilCallLen != sizeof(RIL_Call_V1_6)) {
        return false;
    }

    if (!convertRilCallToHal((RIL_Call_V1_2 *)rilCall, sizeof(RIL_Call_V1_2), call.base)) {
        return false;
    }

    call.forwardedNumber = convertCharPtrToHidlString(rilCall->forwardedNumber);
    return true;
}

bool convertRilRegStateToHal(RIL_RegStateResult_V1_6 *from, size_t responselen, V1_6::RegStateResult& to) {
    if (from == NULL ||
            responselen != sizeof(RIL_RegStateResult_V1_6)) {
        RLOGE("convertRilRegStateToHal Invalid response: NULL");
        return false;
    }

    to.regState = (RegState) from->regState;
    to.rat = (V1_4::RadioTechnology) from->rat;
    to.reasonForDenial = (V1_5::RegistrationFailCause) from->reasonForDenial;
    to.registeredPlmn = convertCharPtrToHidlString(from->registeredPlmn);

    switch (RadioConvUtils::getRadioTechnologyToAccessNetworkType((int)from->rat)) {
        case ACCESS_NETWORK_NGRAN: {
            V1_6::NrVopsInfo nrVopsInfo;
            nrVopsInfo.vopsSupported
                            = (V1_6::VopsIndicator) from->accessTechnologySpecificInfo.ngranInfo.vopsSupported;
            nrVopsInfo.emcSupported
                            = (V1_6::EmcIndicator) from->accessTechnologySpecificInfo.ngranInfo.emcSupported;
            nrVopsInfo.emfSupported
                            = (V1_6::EmfIndicator) from->accessTechnologySpecificInfo.ngranInfo.emfSupported;
            to.accessTechnologySpecificInfo.ngranNrVopsInfo(nrVopsInfo);
            break;
        }
        case ACCESS_NETWORK_EUTRAN: {
            V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo eutranReginfo;
            eutranReginfo.lteVopsInfo.isVopsSupported
                = from->accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported;
            eutranReginfo.lteVopsInfo.isEmcBearerSupported
                = from->accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported;
            eutranReginfo.nrIndicators.isEndcAvailable
                = from->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable;
            eutranReginfo.nrIndicators.isDcNrRestricted
                = from->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted;
            eutranReginfo.nrIndicators.isNrAvailable
                = from->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable;
            to.accessTechnologySpecificInfo.eutranInfo(eutranReginfo);
            break;
        }
        case ACCESS_NETWORK_CDMA2000: {
            V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo cdmaReginfo;
            cdmaReginfo.cssSupported
                = from->accessTechnologySpecificInfo.cdmaInfo.cssSupported;
            cdmaReginfo.roamingIndicator
                = from->accessTechnologySpecificInfo.cdmaInfo.roamingIndicator;
            cdmaReginfo.systemIsInPrl
                = (V1_5::PrlIndicator) from->accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl;
            cdmaReginfo.defaultRoamingIndicator
                = from->accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator;
            to.accessTechnologySpecificInfo.cdmaInfo(cdmaReginfo);
            break;
        }
        default:
            break;
    }

    convertRilCellIdentityToHal(to.cellIdentity, from->cellIdentity);
    return true;
}

void convertRilSignalStrengthToHal(void *response, size_t responseLen, V1_6::SignalStrength& signalStrength) {
    if (response == NULL || responseLen != sizeof(RIL_SignalStrength_V1_6)) {
        return ;
    }

    RIL_SignalStrength_V1_6 *rilSignalStrength = (RIL_SignalStrength_V1_6 *)response;

    // Fixup LTE for backwards compatibility
    // signalStrength: -1 -> 99
    if (rilSignalStrength->LTE_SignalStrength.signalStrength == -1) {
        rilSignalStrength->LTE_SignalStrength.signalStrength = 99;
    }
    // rsrp: -1 -> INT_MAX all other negative value to positive.
    // So remap here
    if (rilSignalStrength->LTE_SignalStrength.rsrp == -1) {
        rilSignalStrength->LTE_SignalStrength.rsrp = INT_MAX;
    } else if (rilSignalStrength->LTE_SignalStrength.rsrp < -1) {
        rilSignalStrength->LTE_SignalStrength.rsrp = -rilSignalStrength->LTE_SignalStrength.rsrp;
    }
    // rsrq: -1 -> INT_MAX
    if (rilSignalStrength->LTE_SignalStrength.rsrq == -1) {
        rilSignalStrength->LTE_SignalStrength.rsrq = INT_MAX;
    }
    // Not remapping rssnr is already using INT_MAX
    // cqi: -1 -> INT_MAX
    if (rilSignalStrength->LTE_SignalStrength.cqi == -1) {
        rilSignalStrength->LTE_SignalStrength.cqi = INT_MAX;
    }

    signalStrength.gsm.signalStrength = rilSignalStrength->GSM_SignalStrength.signalStrength;
    signalStrength.gsm.bitErrorRate = rilSignalStrength->GSM_SignalStrength.bitErrorRate;
    signalStrength.gsm.timingAdvance = rilSignalStrength->GSM_SignalStrength.timingAdvance;
    signalStrength.cdma.dbm = rilSignalStrength->CDMA_SignalStrength.dbm;
    signalStrength.cdma.ecio = rilSignalStrength->CDMA_SignalStrength.ecio;
    signalStrength.evdo.dbm = rilSignalStrength->EVDO_SignalStrength.dbm;
    signalStrength.evdo.ecio = rilSignalStrength->EVDO_SignalStrength.ecio;
    signalStrength.evdo.signalNoiseRatio =
            rilSignalStrength->EVDO_SignalStrength.signalNoiseRatio;
    signalStrength.lte.base.signalStrength = rilSignalStrength->LTE_SignalStrength.signalStrength;
    signalStrength.lte.base.rsrp = rilSignalStrength->LTE_SignalStrength.rsrp;
    signalStrength.lte.base.rsrq = rilSignalStrength->LTE_SignalStrength.rsrq;
    signalStrength.lte.base.rssnr = rilSignalStrength->LTE_SignalStrength.rssnr;
    signalStrength.lte.base.cqi = rilSignalStrength->LTE_SignalStrength.cqi;
    signalStrength.lte.base.timingAdvance = rilSignalStrength->LTE_SignalStrength.timingAdvance;
    signalStrength.lte.cqiTableIndex = rilSignalStrength->LTE_SignalStrength.cqiTableIndex;
    signalStrength.tdscdma.signalStrength = rilSignalStrength->TD_SCDMA_SignalStrength.signalStrength;
    signalStrength.tdscdma.bitErrorRate = rilSignalStrength->TD_SCDMA_SignalStrength.bitErrorRate;
    signalStrength.tdscdma.rscp = rilSignalStrength->TD_SCDMA_SignalStrength.rscp;
    signalStrength.wcdma.base.signalStrength = rilSignalStrength->WCDMA_SignalStrength.signalStrength;
    signalStrength.wcdma.base.bitErrorRate = rilSignalStrength->WCDMA_SignalStrength.bitErrorRate;
    signalStrength.wcdma.rscp = rilSignalStrength->WCDMA_SignalStrength.rscp;
    signalStrength.wcdma.ecno = rilSignalStrength->WCDMA_SignalStrength.ecno;
    signalStrength.nr.base.ssRsrp = rilSignalStrength->NR_SignalStrength.ssRsrp;
    signalStrength.nr.base.ssRsrq = rilSignalStrength->NR_SignalStrength.ssRsrq;
    signalStrength.nr.base.ssSinr = rilSignalStrength->NR_SignalStrength.ssSinr;
    signalStrength.nr.base.csiRsrp = rilSignalStrength->NR_SignalStrength.csiRsrp;
    signalStrength.nr.base.csiRsrq = rilSignalStrength->NR_SignalStrength.csiRsrq;
    signalStrength.nr.base.csiSinr = rilSignalStrength->NR_SignalStrength.csiSinr;
    signalStrength.nr.csiCqiTableIndex = rilSignalStrength->NR_SignalStrength.csiCqiTableIndex;
    signalStrength.nr.csiCqiReport.resize(0);
    if (rilSignalStrength->NR_SignalStrength.csiCqiReportLen > 0) {
        signalStrength.nr.csiCqiReport.resize(rilSignalStrength->NR_SignalStrength.csiCqiReportLen);
        for (int i = 0; i < rilSignalStrength->NR_SignalStrength.csiCqiReportLen; i++) {
            signalStrength.nr.csiCqiReport[i] = rilSignalStrength->NR_SignalStrength.csiCqiReport[i];
        }
    }
}

void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_6 *rilCellInfo, V1_6::CellInfoLte& cellInfo) {
    if (rilCellInfo != NULL) {
        convertRilCellIdentityLteToHal(cellInfo.cellIdentityLte, rilCellInfo->cellIdentityLte);
        cellInfo.signalStrengthLte.base.signalStrength =
                rilCellInfo->signalStrengthLte.signalStrength;
        cellInfo.signalStrengthLte.base.rsrp =
                rilCellInfo->signalStrengthLte.rsrp;
        cellInfo.signalStrengthLte.base.rsrq =
                rilCellInfo->signalStrengthLte.rsrq;
        cellInfo.signalStrengthLte.base.rssnr =
                rilCellInfo->signalStrengthLte.rssnr;
        cellInfo.signalStrengthLte.base.cqi =
                rilCellInfo->signalStrengthLte.cqi;
        cellInfo.signalStrengthLte.base.timingAdvance =
                rilCellInfo->signalStrengthLte.timingAdvance;
        cellInfo.signalStrengthLte.cqiTableIndex =
                rilCellInfo->signalStrengthLte.cqiTableIndex;
    }
}

void convertRilCellInfoNrToHal(RIL_CellInfoNr_V1_6 *rilCellInfo, V1_6::CellInfoNr& cellInfo) {
    if (rilCellInfo != NULL) {
        convertRilCellIdentityNrToHal(cellInfo.cellIdentityNr, rilCellInfo->cellIdentityNr);
        cellInfo.signalStrengthNr.base.ssRsrp = rilCellInfo->signalStrengthNr.ssRsrp;
        cellInfo.signalStrengthNr.base.ssRsrq = rilCellInfo->signalStrengthNr.ssRsrq;
        cellInfo.signalStrengthNr.base.ssSinr = rilCellInfo->signalStrengthNr.ssSinr;
        cellInfo.signalStrengthNr.base.csiRsrp = rilCellInfo->signalStrengthNr.csiRsrp;
        cellInfo.signalStrengthNr.base.csiRsrq = rilCellInfo->signalStrengthNr.csiRsrq;
        cellInfo.signalStrengthNr.base.csiSinr = rilCellInfo->signalStrengthNr.csiSinr;
        cellInfo.signalStrengthNr.csiCqiTableIndex = rilCellInfo->signalStrengthNr.csiCqiTableIndex;
        cellInfo.signalStrengthNr.csiCqiReport.resize(0);
        if(rilCellInfo->signalStrengthNr.csiCqiReportLen > 0) {
            cellInfo.signalStrengthNr.csiCqiReport.resize(rilCellInfo->signalStrengthNr.csiCqiReportLen);
            for (int i = 0; i < rilCellInfo->signalStrengthNr.csiCqiReportLen; i++) {
                cellInfo.signalStrengthNr.csiCqiReport[i] = rilCellInfo->signalStrengthNr.csiCqiReport[i];
            }
        }
    }
}

void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_6::CellInfo>& records) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_CellInfo_V1_6);
    records.resize(num);

    RIL_CellInfo_V1_6 *rillCellInfo = (RIL_CellInfo_V1_6 *) response;
    for (int i = 0; i < num; i++) {
        records[i].registered = rillCellInfo->registered;
        records[i].connectionStatus = (V1_2::CellConnectionStatus)rillCellInfo->connectionStatus;
        switch((int)rillCellInfo->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                V1_5::CellInfoGsm cellInfo;
                convertRilCellInfoGsmToHal(&rillCellInfo->CellInfo.gsm, cellInfo);
                records[i].ratSpecificInfo.gsm(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_WCDMA: {
                V1_5::CellInfoWcdma cellInfo;
                convertRilCellInfoWcdmaToHal(&rillCellInfo->CellInfo.wcdma, cellInfo);
                records[i].ratSpecificInfo.wcdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_CDMA: {
                V1_2::CellInfoCdma cellInfo;
                convertRilCellInfoCdmaToHal(&rillCellInfo->CellInfo.cdma, cellInfo);
                records[i].ratSpecificInfo.cdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_LTE: {
                V1_6::CellInfoLte cellInfo;
                convertRilCellInfoLteToHal(&rillCellInfo->CellInfo.lte, cellInfo);
                records[i].ratSpecificInfo.lte(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                V1_5::CellInfoTdscdma cellInfo;
                convertRilCellInfoTdscdmaToHal(&rillCellInfo->CellInfo.tdscdma, cellInfo);
                records[i].ratSpecificInfo.tdscdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_NR: {
                V1_6::CellInfoNr cellInfo;
                convertRilCellInfoNrToHal(&rillCellInfo->CellInfo.nr, cellInfo);
                records[i].ratSpecificInfo.nr(cellInfo);
                break;
            }

            default: {
                break;
            }
        }
        rillCellInfo += 1;
    }
}

static void convertRilTdToHal(const std::vector<RIL_TrafficDescriptor> &td, V1_6::UrspRule &urspRule) {
    // trafficDescriptor num does not come from CP. Just 1 now.
    int trafficDescriptor_count = 0;
    urspRule.trafficDescriptors.resize(trafficDescriptor_count + 1);
    for (auto it = td.begin(); it != td.end(); it++, trafficDescriptor_count++) {
        V1_6::OptionalDnn tmpDnn;
        V1_6::OptionalOsAppId tmpOsAppID;
        V1_6::TrafficDescriptor tmpTDInfo;

        tmpDnn.value(convertCharPtrToHidlString(it->dnn[0].value));

        fillRilOsAppIdToHal(it->osAppId[0], tmpOsAppID);

        tmpTDInfo = {tmpDnn, tmpOsAppID};
        urspRule.trafficDescriptors[trafficDescriptor_count] = tmpTDInfo;
    }
}

static void fillRilSliceInfoToHal(const RIL_SliceInfo &rilSi, V1_6::SliceInfo &halSi) {
    halSi.sst = (V1_6::SliceServiceType)rilSi.sst;
    halSi.sliceDifferentiator = rilSi.sliceDifferentiator;
    halSi.mappedHplmnSst = (V1_6::SliceServiceType)rilSi.mappedHplmnSst;
    halSi.mappedHplmnSD = rilSi.mappedHplmnSD;
    halSi.status = (V1_6::SliceStatus)rilSi.status;
}

static void convertRilRsdToHal(const std::vector<RIL_RouteSelectionDescriptor> &rsd, V1_6::UrspRule &urspRule) {
    int rsd_count = 0;
    for (auto it = rsd.begin() ; it != rsd.end(); it++, rsd_count++) {
        urspRule.routeSelectionDescriptor[rsd_count].precedence = it->precedence;
        urspRule.routeSelectionDescriptor[rsd_count].sessionType.value((V1_4::PdpProtocolType)it->sessionType.value);
        urspRule.routeSelectionDescriptor[rsd_count].sscMode.value((V1_6::SscMode)it->sscMode.value);

        int sliceInfo_count = 0;
        urspRule.routeSelectionDescriptor[rsd_count].sliceInfo.resize(it->sliceNum);
        for (auto it_3 = it->sliceInfo.begin(); it_3 != it->sliceInfo.end(); it_3++, sliceInfo_count++)   {
            fillRilSliceInfoToHal(*it_3, urspRule.routeSelectionDescriptor[rsd_count].sliceInfo[sliceInfo_count]);
        }

        int dnn_count = 0;
        urspRule.routeSelectionDescriptor[rsd_count].dnn.resize(it->dnnNum);
        for (auto it_3 = it->dnn.begin(); it_3 != it->dnn.end(); it_3++, dnn_count++) {
            urspRule.routeSelectionDescriptor[rsd_count].dnn[dnn_count] = convertCharPtrToHidlString(it_3->value);
        }
    }
}

void convertRilSlicingConfigToHal(const RIL_SlicingConfig &rilSlicingConfig, V1_6::SlicingConfig& scResult) {
    RLOGD("convertRilSlicingConfigToHal");

    int urspRule_count = 0;

    // it == urspRules : precedence, trafficDescriptors, routeSelectionDescriptor
    scResult.urspRules.resize(rilSlicingConfig.urspNum);
    for (auto it = rilSlicingConfig.urspRules.begin(); it != rilSlicingConfig.urspRules.end(); it++, urspRule_count++) {
        scResult.urspRules[urspRule_count].precedence = it->precedence;

        convertRilTdToHal(it->trafficDescriptors, scResult.urspRules[urspRule_count]);

        scResult.urspRules[urspRule_count].routeSelectionDescriptor.resize(it->RSDNum);
        convertRilRsdToHal(it->routeSelectionDescriptor, scResult.urspRules[urspRule_count]);
    }

    int sliceInfo_count = 0;
    scResult.sliceInfo.resize(rilSlicingConfig.sliceInfoNum);
    for (auto it = rilSlicingConfig.sliceInfo.begin(); it != rilSlicingConfig.sliceInfo.end(); it++, sliceInfo_count++) {
        fillRilSliceInfoToHal(*it, scResult.sliceInfo[sliceInfo_count]);
    }
}

} // namespace Radio2HalUtils
