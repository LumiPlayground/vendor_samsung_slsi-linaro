/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * psdatabuilder.cpp
 *
 *  Created on: 2014. 6. 30.
 *      Author: sungwoo48.choi
 */


#include "psdatabuilder.h"
#include <arpa/inet.h>
#include "pdpcontext.h"
#include "rillog.h"
#include "rilproperty.h"
#include <librilutils/textutils.h>
#include <librilutils/systemproperty.h>

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define    PREFIX_IFNAME    "rmnet"

#if(RIL_VERSION < 11)
#error RIL_VERSION Definition is too old, something wrong
#endif

const int RADIO_HAL_1_6 = HAL_VERSION_CODE(1, 6);
const int RADIO_HAL_2_0 = HAL_VERSION_CODE(2, 0);

class DataCallListResponse : public RilData {
public:
    RIL_Data_Call_Response_v11 m_dataCall[MAX_DATA_CALL_SIZE];
    RIL_SetupDataCallResult_V1_6 mDataCall_1_6[MAX_DATA_CALL_SIZE];
    int m_dataCallNum;
    int mHalVer;
public:
    DataCallListResponse(int dataCallNum, int halVer) : RilData() {
        memset(&m_dataCall, 0, sizeof(m_dataCall));
        initDataCall_1_6();
        if (dataCallNum > MAX_DATA_CALL_SIZE)
            dataCallNum = MAX_DATA_CALL_SIZE;
        m_dataCallNum = dataCallNum;
        mHalVer = halVer;
        RilLogV("DataCallListResponse : 0x%x", mHalVer);
    }
    virtual ~DataCallListResponse() {
        if(mHalVer == RADIO_HAL_1_6 || mHalVer == RADIO_HAL_2_0) {
            for (int i = 0; i < m_dataCallNum; i++) {
                if (mDataCall_1_6[i].ifname)
                    delete[] mDataCall_1_6[i].ifname;
                if (mDataCall_1_6[i].addresses)
                    delete[] mDataCall_1_6[i].addresses;
                if(mDataCall_1_6[i].qosSessions.size() != 0)
                {
                    for(int j = 0; j < mDataCall_1_6[i].lenQosSessions ; j++)
                    {
                        if(mDataCall_1_6[i].qosSessions[j].qosFilters.size() != 0)
                            mDataCall_1_6[i].qosSessions[j].qosFilters.clear();
                    }
                    mDataCall_1_6[i].qosSessions.clear();
                }
                //TD
                mDataCall_1_6[i].trafficDescriptors.clear();
            }
        } else {
            for (int i = 0; i < m_dataCallNum; i++) {
                if (m_dataCall[i].ifname)
                    delete[] m_dataCall[i].ifname;
                if (m_dataCall[i].addresses)
                    delete[] m_dataCall[i].addresses;
                if (m_dataCall[i].dnses)
                    delete[] m_dataCall[i].dnses;
                if (m_dataCall[i].gateways)
                    delete[] m_dataCall[i].gateways;
                if (m_dataCall[i].pcscf)
                    delete[] m_dataCall[i].pcscf;
            }
        }
    }
    void initDataCall_1_6() {
        //memset(&mDataCall_1_6, 0, sizeof(mDataCall_1_6));
        for (int i = 0; i < MAX_DATA_CALL_SIZE; i++) {
            ClearDataCall_1_6(&mDataCall_1_6[i]);
        }
    }
    void ClearDataCall_1_6(RIL_SetupDataCallResult_V1_6 *dataCallResp) {
        if (dataCallResp != nullptr) {
            dataCallResp->cause = PDP_FAIL_NONE;
            dataCallResp->suggestedRetryTime = RETRY_NO_SUGGESTED;
            dataCallResp->cid = 0;
            dataCallResp->active = DATA_STATE_INACTIVE;
            dataCallResp->type = PDP_PROTOCOL_TYPE_UNKNOWN;
            dataCallResp->ifname = 0;
            dataCallResp->addresses = 0;
            dataCallResp->linkaddresses.clear();
            dataCallResp->dnses.clear();
            dataCallResp->gateways.clear();
            dataCallResp->pcscf.clear();
            dataCallResp->mtuV4 = 0;
            dataCallResp->mtuV6 = 0;
            dataCallResp->defaultQosType = NOT_SET;
            dataCallResp->defaultQos.nr = {};
            dataCallResp->lenQosSessions = 0;
            dataCallResp->qosSessions.clear();
            dataCallResp->handoverFailureMode = NO_FALLBACK_RETRY_HANDOVER;
            dataCallResp->pduSessionId = 0;
            dataCallResp->sliceInfoFlag = RIL_OPT_INFO_INVALID;
            dataCallResp->sliceInfo = {};
            dataCallResp->trafficDescriptors.clear();
            memset(&dataCallResp->sliceInfo, 0, sizeof(dataCallResp->sliceInfo));
            memset(&dataCallResp->defaultQos, 0, sizeof(dataCallResp->defaultQos));
        }
    }

public:
    virtual void *GetData() const {
         if(mHalVer == RADIO_HAL_1_6 || mHalVer == RADIO_HAL_2_0)
             return (void *)mDataCall_1_6;
         else
             return (void *)m_dataCall;
     }
    virtual unsigned int GetDataLength() const {
        if(mHalVer == RADIO_HAL_1_6 || mHalVer == RADIO_HAL_2_0)
            return m_dataCallNum * sizeof(RIL_SetupDataCallResult_V1_6);
        else
            return m_dataCallNum * sizeof(RIL_Data_Call_Response_v11);
    }

private:
    // RIL: UNKNOWN(0), IPV4(1), IPV6(2), IPV4V6(3), PPP(4), NON_IP(5), UNSTRUCTURED(6)
    // NON_IP and UNSTRUCTURED is not supported by CP, Fallback to IPV4
    const char *PdpTypeCharMap[7] = { DATA_PROTOCOL_IP, DATA_PROTOCOL_IP, DATA_PROTOCOL_IPV6, DATA_PROTOCOL_IPV4V6,
                                DATA_PROTOCOL_PPP, DATA_PROTOCOL_IP, DATA_PROTOCOL_IP };
    void FillPdpType(RIL_Data_Call_Response_v11 *resp, const DataCall *dataCall)
    {
        if (dataCall->pdpType > PDP_TYPE_UNSTRUCTURED)
            resp->type = (char *)DATA_PROTOCOL_IP;
        else
            resp->type = PdpTypeCharMap[dataCall->pdpType];
    }

    PdpProtocolType PdpTypeIntMap[7] = { PDP_PROTOCOL_TYPE_UNKNOWN, PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IPV6,
                              PDP_PROTOCOL_TYPE_IPV4V6, PDP_PROTOCOL_TYPE_PPP, PDP_PROTOCOL_TYPE_NON_IP,
                              PDP_PROTOCOL_TYPE_UNSTRUCTURED };
    void FillPdpType(RIL_SetupDataCallResult_V1_6 *resp, const DataCall *dataCall)
    {
        if(dataCall->pdpType > PDP_TYPE_UNSTRUCTURED)
            resp->type = PDP_PROTOCOL_TYPE_IP;
        else
            resp->type = PdpTypeIntMap[dataCall->pdpType];
    }

    template <typename T>
    void FillInterfaceName(T *resp, const DataCall *dataCall, const char *ifname = NULL)
    {
        // set interface name
        // PREFIX_IFNAME + (cid-1)
        // e.g., CID = 1, interface name is "rmnet0"
        if (resp->ifname)
            delete[] resp->ifname;
        resp->ifname = new char[MAX_IFNAME_LEN];
        memset(resp->ifname, 0, MAX_IFNAME_LEN);
        if (TextUtils::IsEmpty(ifname)) {
            snprintf(resp->ifname, MAX_IFNAME_LEN, "%s%d", PREFIX_IFNAME, dataCall->cid - 1);
        }
        else {
            strncpy(resp->ifname, ifname, MAX_IFNAME_LEN);
        }
    }

    const char delim = ' ';

    template<typename T>
    void FillAddress(bool bIpv6, char (&addresses)[MAX_ADDRESS_STRING_LEN], T &pdpAddr)
    {
        if( pdpAddr.valid) {
            char buf[MAX_ADDRESS_STRING_LEN] = {0, };
            // IPv4 address
            if (inet_ntop((bIpv6)? AF_INET6: AF_INET, pdpAddr.addr, buf, (socklen_t)sizeof(buf)) != NULL) {
                if(bIpv6) AddPrefix(buf, "/64");
                int len = 0;
                len = strlen(addresses);
                if (len > 0) {
                    addresses[len++] = delim;
                }
                snprintf(addresses + len, sizeof(addresses) - len - 1, "%s", buf);
            }
        }
    }

    void AddPrefix(char (&buf)[MAX_ADDRESS_STRING_LEN], const char *prefix)
    {
        int len = strlen(buf);
        snprintf(buf + len, sizeof(buf) - len - 1, "%s", prefix);
    }

    template<typename T>
    void FillAddresses(bool bIpv6, const T &pdpAddr, char (&gateways)[MAX_ADDRESS_STRING_LEN],
                       char (&dnses)[MAX_ADDRESS_STRING_LEN], char (&pcscf)[MAX_ADDRESS_STRING_LEN * 2], int pcscf_count) {
        FillGateway<T>(bIpv6, pdpAddr, gateways);
        FillDnsAddresses<T>(bIpv6, pdpAddr, dnses);
        FillPcscfAddresses<T>(bIpv6, pdpAddr, pcscf, pcscf_count);
    }

    template<typename T>
    void FillGateway(bool bIpv6, const T &pdpAddr, char (&gateways)[MAX_ADDRESS_STRING_LEN]) {
        if (pdpAddr.valid) {
            // IPv4 Gateway
            if (!isNullIp(pdpAddr.addr)) {
                int len = 0;
                len = strlen(gateways);
                if (len > 0) {
                    gateways[len++] = delim;
                }
                // IPv4 gateway comment
                // No manipulation. Just fill 0.0.0.0 address. This is allowed
                // 32bit length(self) will work as gateway because it's ppp
                // just this is required to set default routing rule into each interface table
                // This is applicable from AOSP N and required for AOSP P version
                // which will not use main table
                // IPv6  gateway comment
                // No manipulation. Just fill '::' address. This is allowed and work as gateway because it's ppp
                // just this is required to set default routing rule into each interface table
                // This should be replaced Router Address after ND, Use Kernel Configuration with Link-Local
                // Router addrees '/proc/sys/net/ipv6/conf/all/accept_ra_defrtr'
                // This is required when SetupDataCall gets global ipv6 address directly without RA.
                // There's no Link-Local Setting, RS from Linux kernel won't be sent.
                // ConnectivityService will try to set default route with these next-hop address
                // This make occasionally netd warning (file exist) when kernel already setup default route with RA
                // but EEXIST error is ignored and doesn't cause critical failure
                // in ModifyIpRoute() of netd RouteController.cpp
                strncpy(gateways + len, ip_dftgw[bIpv6? 1 : 0], sizeof(gateways) - len - 1);
            }
        }
    }

    template<typename T>
    void FillDnsAddresses(bool bIpv6, const T &pdpAddr,
                              char (&dnses)[MAX_ADDRESS_STRING_LEN]) {
        if (pdpAddr.valid) {
            AddDns(bIpv6, pdpAddr.dns1, dnses, 0);
            AddDns(bIpv6, pdpAddr.dns2, dnses, 1);
        }
    }

    template <typename T>
    void AddDns(bool bIpv6, const T &dns, char (&dnses)[MAX_ADDRESS_STRING_LEN], int slot)
    {
        int len = 0;
        char buf[MAX_ADDRESS_STRING_LEN] = {0, };
        const char publicDNS[2][2][MAX_ADDRESS_STRING_LEN] = {
            {"2001:4860:4860::8888", "2001:4860:4860::8844"},
            {"8.8.8.8", "8.8.4.4"}
        };

        // IPv6 DNS1
        if (!isNullIp(dns)) {
            len = strlen(dnses);
            if (len > 0) {
                dnses[len++] = delim;
            }
            if (inet_ntop((bIpv6)? AF_INET6 : AF_INET, dns, buf, (socklen_t)sizeof(buf)) != NULL) {
                strncpy(dnses + len, buf, sizeof(dnses) - len - 1);
            }
        } else {
            if (!bIpv6) return;
            int defaultIpv6Dns = SystemProperty::GetInt(RIL_PS_DEFAULT_IPV6_DNS, 0);
            if (defaultIpv6Dns == 1 && slot == 0) {
                len = strlen(dnses);
                if (len > 0) {
                    dnses[len++] = delim;
                }
                // Fill Google Public DNS1
                strncpy(dnses + len, publicDNS[(bIpv6)? 0 : 1][slot], sizeof(dnses) - len - 1);
            }
        }
    }

    template<typename T>
    void FillPcscfAddresses(bool bIpv6, const T &pdpAddr,
                            char (&pcscf)[MAX_ADDRESS_STRING_LEN *2], int pcscf_count) {
        if (pdpAddr.valid) {
            int len = 0;
            char buf[MAX_ADDRESS_STRING_LEN] = {0, };

            // IPv4 P-CSCF
            const int addrLen = bIpv6 ? MAX_IPV6_ADDR_LEN : MAX_IPV4_ADDR_LEN;
            for(int i=0; i < pcscf_count; i++) {
                if (memcmp(&pdpAddr.pcscf[addrLen * i], nullIp, addrLen) != 0) {
                    len = strlen(pcscf);
                    if (len > 0) {
                        pcscf[len++] = delim;
                    }
                    if (inet_ntop(bIpv6 ? AF_INET6 : AF_INET, &pdpAddr.pcscf[addrLen * i], buf, (socklen_t)sizeof(buf)) != NULL) {
                        strncpy(pcscf + len, buf, sizeof(pcscf) - len - 1);
                    }
                }
            }
        }
    }

    void FillDefaultQosParam(RIL_SetupDataCallResult_V1_6*resp_1_6, const DataCall *dataCall) {
        resp_1_6->defaultQosType= dataCall->defaultQosType;
        RIL_Qos *qos = &resp_1_6->defaultQos;

        if(dataCall->defaultQosType == EPS_QOS) {
            const RIL_EpsQos *srcQos = &dataCall->defaultEpsQos;
            FillDefaultQosParam(qos, EPS_QOS, srcQos, NULL);
        } else if(dataCall->defaultQosType == NR_QOS) {
            const RIL_NrQos *srcQos = &dataCall->defaultNrQos;
            FillDefaultQosParam(qos, NR_QOS, NULL, srcQos);
        }

    }

    void FillDefaultQosParam(RIL_Qos * out, const RIL_QosType qostype, const RIL_EpsQos *epsqos, const RIL_NrQos *nrqos)
    {
        if(qostype == EPS_QOS && epsqos != NULL){
            out->eps = *epsqos;
        }else if(qostype == NR_QOS && nrqos != NULL){
            out->nr = *nrqos;
        }
    }

    std::string addNetworkPrefix(bool bIpv6, const BYTE (&addr)[MAX_ADDRESS_STRING_LEN], int prefix) {
        char addrstr[MAX_ADDRESS_STRING_LEN] = {0};
        inet_ntop((bIpv6) ? AF_INET6 : AF_INET, addr, addrstr,
                  (bIpv6) ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN);
        return addrstr + std::string("/") + std::to_string(prefix);
    }

    void FillQosfilterAddress(RIL_QosFilter * out, const QosFilter_t &qosfilter)
    {
        if (!TextUtils::IsEmpty((char*)qosfilter.localAddress[0])) {
            out->localAddresses.push_back (addNetworkPrefix(true, qosfilter.localAddress[0], qosfilter.localAddrPrefix_v6));
        }
        if (!TextUtils::IsEmpty((char*)qosfilter.localAddress[1])) {
            out->localAddresses.push_back (addNetworkPrefix(false, qosfilter.localAddress[1], qosfilter.localAddrPrefix_v4));
        }
        if (!TextUtils::IsEmpty((char*)qosfilter.remoteAddress[0])) {
            out->remoteAddresses.push_back (addNetworkPrefix(true, qosfilter.remoteAddress[0], qosfilter.remotePortprefix_6));
        }
        if (!TextUtils::IsEmpty((char*)qosfilter.remoteAddress[1])) {
            out->remoteAddresses.push_back (addNetworkPrefix(false, qosfilter.remoteAddress[1], qosfilter.remotePortprefix_4));
        }
    }

    RIL_NrQos testNrQos = { .fiveQi = 1,
                           .uplink.maxBitrateKbps = 300,
                           .uplink.guaranteedBitrateKbps = 100,
                           .downlink.maxBitrateKbps = 400,
                           .downlink.guaranteedBitrateKbps = 350,
                           .qfi = 1,
                           .averagingWindowMs = 200};

    void testFillDefaultQosParam(RIL_SetupDataCallResult_V1_6*resp_1_6) {
        resp_1_6->defaultQosType = NR_QOS;
        resp_1_6->defaultQos.nr = testNrQos;
    }

    void ConvertToRilQosFilter(RIL_QosFilter *dst, const QosFilter_t *src) {
        FillQosfilterAddress(dst, *src);

        //localPort
        dst->localPortFlag = (RIL_MaybePortFlag)src->localPortFlag;
        dst->localPort.range.start = src->localPortStart;
        dst->localPort.range.end = src->localPortEnd;

        //remotePort
        dst->remotePortFlag = (RIL_MaybePortFlag)src->remotePortFlag;
        dst->remotePort.range.start = src->remotePortStart;
        dst->remotePort.range.end = src->remotePortEnd;

        dst->protocol = (RIL_QosProtocol)src->protocol;

        //tos
        dst->tosFlag = (RIL_TypeOfServiceFlag)src->tosFlag;
        dst->tos = src->tosValue;

        //flow
        dst->flowLabelFlag = (RIL_Ipv6FlowLabelFlag)src->flowLabelFlag;
        dst->flowLabel.value = src->flowLabelValue;

        //spi
        dst->spiFlag = (RIL_IpsecSpiFlag)src->spiFlag;
        dst->spi.value = src->spiValue;

        dst->direction = (RIL_QosFilterDirection)src->direction;

        dst->precedence = src->precedence;
    }

    void ConvertToRilQosSession(RIL_QosSession *dst, const QosSession_t *src) {
        dst->qosSessionId = src->qosSessionId;
        dst->qosType = src->defaultQosType;

        //Fill QosParam
        FillDefaultQosParam(&dst->qos, dst->qosType,
                            &src->defaultEpsQos, &src->defaultNrQos);

        dst->lenQosFilters = src->QosFilterCnt;
        dst->qosFilters.clear();
        dst->qosFilters.resize(src->QosFilterCnt);

        for(int j = 0; j < dst->lenQosFilters; j++)
        {
            RIL_QosFilter *dstQosFilter = &dst->qosFilters[j];
            const QosFilter_t *srcQosFilter = &src->QosFilters[j];

            ConvertToRilQosFilter(dstQosFilter, srcQosFilter);
        }
    }

    void FillQosSessionParam(RIL_SetupDataCallResult_V1_6 *resp_1_6, const DataCall *dataCall) {
        if(dataCall->QosSession.size() == 0) {
            return;
        }

        resp_1_6->lenQosSessions = dataCall->QosSessionCnt;
        resp_1_6->qosSessions.clear();
        resp_1_6->qosSessions.resize(dataCall->QosSessionCnt);

        for(int i = 0; i < dataCall->QosSessionCnt; i++)
        {
            RIL_QosSession *dstQosSession = &resp_1_6->qosSessions[i];
            const QosSession_t *srcQosSession = &dataCall->QosSession[i];

            ConvertToRilQosSession(dstQosSession, srcQosSession);
        }
    }

    void CopyAddrStr(char *(&dst), const char *src, int len)
    {
        if (dst)
            delete[] dst;
        dst = new char[len];
        memset(dst, 0, len);
        if (!TextUtils::IsEmpty(src))
            strncpy(dst, src, len - 1);
    }

    void CopyToResponse(RIL_Data_Call_Response_v11 *resp, const DataCall *dataCall)
    {
        char dnses[MAX_ADDRESS_STRING_LEN] = {0, };
        char gateways[MAX_ADDRESS_STRING_LEN] = {0, };
        char pcscf[MAX_ADDRESS_STRING_LEN * 2] = {0, };
        int pcscf_count = MAX_PCSCF_NUM + dataCall->pcscf_ext_count;

        if (needToV4DnsFirst(dataCall)) {
            FillAddresses<PDP_ADDR_V4>(false, dataCall->ipv4, gateways, dnses, pcscf, pcscf_count);
            FillAddresses<PDP_ADDR_V6>(true, dataCall->ipv6, gateways, dnses, pcscf, pcscf_count);
        } else {
            FillAddresses<PDP_ADDR_V6>(true, dataCall->ipv6, gateways, dnses, pcscf, pcscf_count);
            FillAddresses<PDP_ADDR_V4>(false, dataCall->ipv4, gateways, dnses, pcscf, pcscf_count);
        }
        CopyAddrStr(resp->dnses, dnses, MAX_ADDRESS_STRING_LEN);
        CopyAddrStr(resp->gateways, gateways, MAX_ADDRESS_STRING_LEN);
        CopyAddrStr(resp->pcscf, pcscf, MAX_ADDRESS_STRING_LEN * 2);

        // MTU value from Network
        resp->mtu = dataCall->ipv4_mtu_size;
        // PCO value
        // TODO: Need to deliver pcoData IND from O version, still need this?
        // RIL_UNSOL_PCO_DATA from CP is required with more data
        //resp->pco = dataCall->pco;
    }

    bool needToV4DnsFirst(const DataCall *dataCall) {
        if (m_bIpv4DnsQueryFirst) {
            if (dataCall->ipv4.valid && !isNullIp(dataCall->ipv4.dns1)) {
                return true;
            } else {
                return false;
            }
        } else {
            if (dataCall->ipv6.valid && !isNullIp(dataCall->ipv6.dns1)) {
                return false;
            } else {
                return true;
            }
        }
    }

    const char nullIp[16] = {0, };
    // 0: ipv4, 1: ipv6
    const char ip_dftgw[2][INET6_ADDRSTRLEN] = { "0.0.0.0", "::" };
    template<typename T>
    bool isNullIp(const T& src) {
        return !memcmp(src, nullIp, sizeof(src));
    }
    bool isNullIp(bool bIpv6, const BYTE src[]) {
        return !memcmp(src, nullIp, bIpv6 ? MAX_IPV6_ADDR_LEN : MAX_IPV4_ADDR_LEN);
    }

    void FillAddrVector(bool bIpv6, vector<string> &dst, const BYTE src[]) {
        char addrstr[MAX_ADDRESS_STRING_LEN+1];

        if (!isNullIp(bIpv6, src)){
            memset(addrstr, 0, MAX_ADDRESS_STRING_LEN+1);
            inet_ntop(bIpv6 ? AF_INET6 : AF_INET, src, addrstr, bIpv6 ? INET6_ADDRSTRLEN : INET_ADDRSTRLEN);
            std::string str(addrstr);
            dst.push_back (str);
        }
    }

    void CopyIpv4InformationToResponse(RIL_SetupDataCallResult_V1_6 *resp, const DataCall *dataCall) {
        //IPv4
        FillAddrVector(false, resp->dnses, dataCall->ipv4.dns1);
        FillAddrVector(false, resp->dnses, dataCall->ipv4.dns2);
        FillAddrVector(false, resp->gateways, dataCall->ipv4.gw);
        if (!isNullIp(dataCall->ipv4.addr) && isNullIp(dataCall->ipv4.gw)) {
            resp->gateways.push_back (string(ip_dftgw[0]));
        }
        // IPv4 P-CSCF
        int pcscf_count = MAX_PCSCF_NUM + dataCall->pcscf_ext_count;
        for(int i = 0; i < pcscf_count; i++) {
            const BYTE *src = &dataCall->ipv4.pcscf[MAX_IPV4_ADDR_LEN * i];
            FillAddrVector(false, resp->pcscf, src);
        }
    }

    void CopyIpv6InformationToResponse(RIL_SetupDataCallResult_V1_6 *resp, const DataCall *dataCall) {
        //IPv6
        FillAddrVector(true, resp->dnses, dataCall->ipv6.dns1);
        FillAddrVector(true, resp->dnses, dataCall->ipv6.dns2);
        FillAddrVector(true, resp->gateways, dataCall->ipv6.gw);
        if (!isNullIp(dataCall->ipv6.addr) && isNullIp(dataCall->ipv6.gw)) {
            resp->gateways.push_back (string(ip_dftgw[1]));
        }
        // IPv6 P-CSCF
        int pcscf_count = MAX_PCSCF_NUM + dataCall->pcscf_ext_count;
        for(int i = 0; i < pcscf_count; i++) {
            const BYTE *src = &dataCall->ipv6.pcscf[MAX_IPV6_ADDR_LEN * i];
            FillAddrVector(true, resp->pcscf, src);
        }
    }

    void DumpVectorItems(RIL_SetupDataCallResult_V1_6 *resp) {
        for (unsigned int i = 0; i < resp->dnses.size(); i++) {
            RilLogV("dnses:%s", resp->dnses[i].c_str());
        }
        for (unsigned int i = 0; i < resp->pcscf.size(); i++) {
            RilLogV("pcscf: %s", resp->pcscf[i].c_str());
        }
        for (unsigned int i = 0; i < resp->gateways.size(); i++) {
            RilLogV("gateway: %s", resp->gateways[i].c_str());
        }
    }

    void CopyToResponse(RIL_SetupDataCallResult_V1_6 *resp, const DataCall *dataCall)
    {
        resp->dnses.clear();
        resp->gateways.clear();
        resp->pcscf.clear();

        if (needToV4DnsFirst(dataCall)) {
            CopyIpv4InformationToResponse(resp, dataCall);
            CopyIpv6InformationToResponse(resp, dataCall);
        } else{
            CopyIpv6InformationToResponse(resp, dataCall);
            CopyIpv4InformationToResponse(resp, dataCall);
        }
        DumpVectorItems(resp);

        // MTU value from Network
        resp->mtuV4 = dataCall->ipv4_mtu_size;
        resp->mtuV6 = dataCall->ipv6_mtu_size;  // TODO : shall be matched for further cases

        // pdu session id
        resp->pduSessionId = dataCall->pdu_session_id;

        // Optional slice Info
        resp->sliceInfoFlag = dataCall->sliceInfoType;
        memset(&resp->sliceInfo, 0, sizeof(RIL_SliceInfo));
        memcpy(&resp->sliceInfo, &dataCall->sliceInfo, sizeof(RIL_SliceInfo));

        // Traffic Description Info
        resp->trafficDescriptors.clear();
        resp->trafficDescriptors.resize(dataCall->tdLength);

        for (int i = 0; i < dataCall->tdLength; i++) {
            if (!dataCall->tdInfo[i].dnn.empty()) {
                resp->trafficDescriptors[i].dnnFlag = RIL_OPT_INFO_VALID;
                resp->trafficDescriptors[i].dnn.resize(1);
                size_t len = (dataCall->tdInfo[i].dnn[0].size() > MAX_PDP_APN_LEN) ? MAX_PDP_APN_LEN : dataCall->tdInfo[i].dnn[0].size();
                for (unsigned int idx = 0; idx < len; idx++) {
                    resp->trafficDescriptors[i].dnn[0].value[idx] = (dataCall->tdInfo[i].dnn[0])[idx];
                }
                resp->trafficDescriptors[i].dnn[0].value[MAX_PDP_APN_LEN - 1] = 0;
            }
            if (!dataCall->tdInfo[i].osAppIds.empty()) {
                resp->trafficDescriptors[i].osAppIdFlag = RIL_OPT_INFO_VALID;
                resp->trafficDescriptors[i].osAppId.resize(1);
                for (unsigned int idx = 0; idx < sizeof(dataCall->tdInfo[i].osAppIds[0].value); idx++) {
                    resp->trafficDescriptors[i].osAppId[0].value[idx] = dataCall->tdInfo[i].osAppIds[0].value[idx];
                }
            }
        }
    }

private:
    // cmcc/chnopen has 'true', default is europen, so 'false'
    bool m_bIpv4DnsQueryFirst = false;
    bool mIsRrcCon = false;
public:
    inline void setIpv4DnsQueryFirst(bool bIpv4First) {
        m_bIpv4DnsQueryFirst = bIpv4First;
        RilLogV("set Ipv4DnsQueryFirst : %d", m_bIpv4DnsQueryFirst);
    }

    inline void SetCurRrcState(bool state) {
        mIsRrcCon = state;
    }

    template<typename T>
    void CopyAddressesToResponseCommon(T *resp, const DataCall *dataCall, const char *ifname) {
        FillPdpType(resp, dataCall);
        FillInterfaceName(resp, dataCall, ifname);

        // set IP addresses
        char addresses[MAX_ADDRESS_STRING_LEN] = {0, };
        if (needToV4DnsFirst(dataCall)) {
            FillAddress(false, addresses, dataCall->ipv4);
            FillAddress(true, addresses, dataCall->ipv6);
        } else {
            FillAddress(true, addresses, dataCall->ipv6);
            FillAddress(false, addresses, dataCall->ipv4);
        }

        CopyAddrStr(resp->addresses, addresses, MAX_ADDRESS_STRING_LEN);
    }

    template<typename T>
    void TranslateToResponse(T *resp, const DataCall *dataCall, const char *ifname)
    {
        CopyAddressesToResponseCommon(resp, dataCall, ifname);
        CopyToResponse(resp, dataCall);
    }

    template<typename T>
    void InitCommon(T *dataCallResp);
    void Init(RIL_Data_Call_Response_v11 *dataCallResp);
    void Init(RIL_SetupDataCallResult_V1_6 *dataCallResp);
    int GetDataCallFailCause(int errorCode, int status);
    int64_t GetSuggestedRetryTime(const DataCall *dataCall, int status);

    bool SetDataCall(int index, const DataCall *dataCall, const char *ifname = NULL, int halVer = HAL_VERSION_CODE(1,5)) {
        if (index < 0 || index >= m_dataCallNum) {
            return false;
        }

        if (dataCall == NULL) {
            return false;
        }

        if( halVer == HAL_VERSION_CODE(1,6) || halVer == HAL_VERSION_CODE(2, 0)) {
           RilLogV("SetDataCall_1_6");
            RIL_SetupDataCallResult_V1_6 *resp = &mDataCall_1_6[index];
            resp->cause = (RIL_DataCallFailCause) dataCall->status;
            resp->suggestedRetryTime = dataCall->suggestedRetryTime;
            resp->cid = dataCall->cid;
            resp->active = (RIL_DataConnActiveStatus) dataCall->active;
            TranslateToResponse(resp, dataCall, ifname);
            FillDefaultQosParam(resp, dataCall);
            //FillDefaultQosParam(&resp->defaultQos, dataCall->defaultQosType, dataCall->defaultEpsQos, dataCall->defaultNrQos);
            FillQosSessionParam(resp, dataCall);
         }else {
             RilLogV("SetDataCall");
             RIL_Data_Call_Response_v11 *resp = &m_dataCall[index];
             resp->status = dataCall->status;
             resp->suggestedRetryTime = dataCall->suggestedRetryTime;
             resp->cid = dataCall->cid;
             if (dataCall->active == ACTIVE_AND_LINKUP && !mIsRrcCon) {
                 RilLogV("SetDataCall active state is changed as ACTIVE_AND_LINKDOWN due to rrc con state(%d)", mIsRrcCon);
                 resp->active = ACTIVE_AND_LINKDOWN;
             } else {
                 resp->active = dataCall->active;
             }
             TranslateToResponse(resp, dataCall, ifname);
         }

        return true;
    }
public:
    bool SetDataCall(int index, int errorCode, const char *ifname, const DataCall *dataCall, int halVer);
    bool SetDataCall(int index, PdpContext *pPdpContext, int halVer);
    bool SetDataCall(int index, int errorCode, PdpContext *pPdpContext ,int halVer);
    bool SetDataCall(int index, int errorCode, int halVer);
};

template<typename T>
void DataCallListResponse::InitCommon(T *dataCallResp)
{
    if (dataCallResp != NULL) {
        if (dataCallResp->ifname) delete[] dataCallResp->ifname;
        if (dataCallResp->addresses) delete[] dataCallResp->addresses;

        dataCallResp->suggestedRetryTime = RETRY_NO_SUGGESTED;
        dataCallResp->cid = 0;
    }
}
void DataCallListResponse::Init(RIL_Data_Call_Response_v11 *dataCallResp)
{
    InitCommon(dataCallResp);
    memset(dataCallResp, 0, sizeof(RIL_Data_Call_Response_v11));
    if (dataCallResp != NULL) {
        if (dataCallResp->dnses) delete[] dataCallResp->dnses;
        if (dataCallResp->gateways) delete[] dataCallResp->gateways;
        if (dataCallResp->pcscf) delete[] dataCallResp->pcscf;

        dataCallResp->status = PDP_FAIL_ERROR_UNSPECIFIED;
        dataCallResp->active = INACTIVE;
        dataCallResp->type = (char *)DATA_PROTOCOL_IP;
    }
}

void DataCallListResponse::Init(RIL_SetupDataCallResult_V1_6 *dataCallResp)
{
    InitCommon(dataCallResp);
    ClearDataCall_1_6(dataCallResp);
    if (dataCallResp != NULL) {
        if (!dataCallResp->dnses.empty()) dataCallResp->dnses.clear();
        if (!dataCallResp->gateways.empty()) dataCallResp->gateways.clear();
        if (!dataCallResp->pcscf.empty()) dataCallResp->pcscf.clear();

        dataCallResp->cause = PDP_FAIL_ERROR_UNSPECIFIED;
        dataCallResp->active = (RIL_DataConnActiveStatus)INACTIVE;
        dataCallResp->type = PDP_PROTOCOL_TYPE_IP;
    }
}

int DataCallListResponse::GetDataCallFailCause(int errorCode, int status)
{
    // PDP_FAIL_* that's not specified in TS 24.008
    if (errorCode < 0)
        return errorCode;
    if (errorCode != RIL_E_SUCCESS) {
        switch (errorCode) {
        case RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW:
            return PDP_FAIL_ERROR_UNSPECIFIED;
        default:
            return PDP_FAIL_ERROR_UNSPECIFIED;
        } // end switch ~
    }
    return status;
}

int64_t DataCallListResponse::GetSuggestedRetryTime(const DataCall *dataCall, int /* status */)
{
    /*
    int time = RETRY_NO_SUGGESTED;
    //fw will check Fail Status
    if(status != PDP_FAIL_NONE) {
        time = dataCall->suggestedRetryTime;
    }
    return time;
    */
    return dataCall->suggestedRetryTime;
}

/**
 * SetDataCall
 *
 * a response in case of error. Without PDP Context, cannot make a response of successes case.
 */
bool DataCallListResponse::SetDataCall(int index, int errorCode, int halVer)
{
    if (index < 0 || index >= m_dataCallNum) {
         return false;
    }

    switch(halVer){
        case RADIO_HAL_1_6:
        case RADIO_HAL_2_0: {
                RIL_SetupDataCallResult_V1_6 *resp_1_6 = &mDataCall_1_6[index];
                Init(resp_1_6);
                resp_1_6->cause = (RIL_DataCallFailCause) GetDataCallFailCause(errorCode, PDP_FAIL_ERROR_UNSPECIFIED);
            }
        break;
        default: { //case for legacy operations
                RIL_Data_Call_Response_v11 *resp = &m_dataCall[index];
                Init(resp);
                resp->status = GetDataCallFailCause(errorCode, PDP_FAIL_ERROR_UNSPECIFIED);
            }
        break;
    }

    return true;
}

bool DataCallListResponse::SetDataCall(int index, PdpContext *pPdpContext , int halVer) {
    return SetDataCall(index, pPdpContext->GetDataCallInfo(), pPdpContext->GetInterfaceName(),halVer);
}

bool DataCallListResponse::SetDataCall(int index, int errorCode, const char *ifname, const DataCall *dataCall, int halVer)
{
    if (index < 0 || index >= m_dataCallNum) {
         return false;
    }

    if (dataCall == NULL) {
        return false;
    }

    switch (halVer) {
        case RADIO_HAL_1_6:
        case RADIO_HAL_2_0: {
                RilLogV("SetDataCall_1_6_Translate");
                RIL_SetupDataCallResult_V1_6 *resp = &mDataCall_1_6[index];
                Init(resp);

                resp->cause = (RIL_DataCallFailCause)GetDataCallFailCause(errorCode, dataCall->status);
                resp->suggestedRetryTime = GetSuggestedRetryTime(dataCall, dataCall->status);
                resp->cid = dataCall->cid;
                resp->active = (RIL_DataConnActiveStatus) dataCall->active;

                if (resp->cause == PDP_FAIL_NONE) {
                    if (resp->active >= DATA_STATE_DORMANT) {
                        // Active or Dormant
                        TranslateToResponse(resp, dataCall, ifname);
                    }
                    else {
                        // unexpected case. set a default error status.
                        resp->cause = PDP_FAIL_ERROR_UNSPECIFIED;
                    }
                }
                FillDefaultQosParam(resp, dataCall);
                //FillDefaultQosParam(&resp->defaultQos, dataCall->defaultQosType, dataCall->defaultEpsQos, dataCall->defaultNrQos);
                //testFillDefaultQosParam(resp, dataCall);
                FillQosSessionParam(resp, dataCall);
            }
        break;
        default: {
                RilLogV("SetDataCall_Translate");
                RIL_Data_Call_Response_v11 *resp = &m_dataCall[index];
                Init(resp);

                resp->status = GetDataCallFailCause(errorCode, dataCall->status);
                resp->suggestedRetryTime = GetSuggestedRetryTime(dataCall, dataCall->status);
                // signed 64 bit to signed 32 bit integer
                int64_t t = GetSuggestedRetryTime(dataCall, dataCall->status);
                if ( t == INT32_MAX ) t = INT32_MAX - 1;
                if ( t == INT64_MAX ) t = INT32_MAX;
                resp->suggestedRetryTime = t;
                resp->cid = dataCall->cid;
                resp->active = dataCall->active;

                if (resp->status == PDP_FAIL_NONE) {
                    if (resp->active >= DATA_STATE_DORMANT) {
                        // Active or Dormant
                        TranslateToResponse(resp, dataCall, ifname);
                    } else {
                        // unexpected case. set a default error status.
                        resp->status = PDP_FAIL_ERROR_UNSPECIFIED;
                    }
                }
            }
        break;
    }

    return true;
}

/**
 * SetDataCall
 *
 * if PdpContext is NULL, it should be error.
 *
 * @param index a index of data call list
 * @param errorCode a result of SETUP_DATA_CALL
 * @param pPdpContext an instance of PdpContext. It has all information that a result of SETUP_DATA_CALL.
 * @return returns true when set information of the result successfully, otherwise returns false.
 */

bool DataCallListResponse::SetDataCall(int index, int errorCode, PdpContext *pPdpContext, int halVer)
{
    if (pPdpContext == NULL) {
        RilLogV("pdpContext is null");
        return SetDataCall(index, errorCode, halVer);
    }
    return SetDataCall(index, errorCode, pPdpContext->GetInterfaceName(), pPdpContext->GetDataCallInfo(),halVer);
}

/**
 * PsDataBuilder
 */
PsDataBuilder::PsDataBuilder(int halVer)
{
    mHalVer = halVer;
}
const RilData *PsDataBuilder::BuildSetupDataCallResponse(PdpContext *pPdpContext)
{
    if (pPdpContext == NULL)
        return NULL;

    DataCallListResponse *rildata = new DataCallListResponse(1,mHalVer);
    if (rildata != NULL) {
        rildata->setIpv4DnsQueryFirst(m_bIpv4DnsQueryFirst);
        rildata->SetDataCall(0, pPdpContext, mHalVer);
    }
    return rildata;
}

const RilData *PsDataBuilder::BuildSetupDataCallResponse(int errorCode, PdpContext *pPdpContext)
{
    // pPdpContext==NULL will generate Default DataCall, SetDataCall(int, int, PdpContext *) can process this.
    // if (pPdpContext == NULL)
    //     return NULL;

    DataCallListResponse *rildata = new DataCallListResponse(1,mHalVer);
    if (rildata != NULL) {
        RilLogV("BuildSetupDataCallResponse ");
        rildata->setIpv4DnsQueryFirst(m_bIpv4DnsQueryFirst);
        rildata->SetDataCall(0, errorCode, pPdpContext, mHalVer);
    }
    return rildata;
}

const RilData *PsDataBuilder::BuildSetupDataCallResponse(int errorCode, int status)
{
    DataCallListResponse *rildata = new DataCallListResponse(1,mHalVer);
    if (rildata != NULL) {
        rildata->setIpv4DnsQueryFirst(m_bIpv4DnsQueryFirst);
        rildata->SetDataCall(0, errorCode, mHalVer);
        rildata->m_dataCall[0].status = status;
    }
    // Override status
    return rildata;
}

const RilData *PsDataBuilder::BuildPcoData(int cid, int nPdpType, int pcoId, int contentsLen, char *pContents)
{
    RIL_PCO_Data rsp;

    rsp.cid = cid;
    rsp.bearer_proto = ConvertPdpType(nPdpType);
    rsp.pco_id = pcoId;
    rsp.contents_length = contentsLen;
    rsp.contents = pContents;

    return new RilDataRaw(&rsp, sizeof(rsp));
}

char *PsDataBuilder::ConvertPdpType(int nPdpType)
{
    switch (nPdpType) {
      case PDP_TYPE_IPV4:
          return (char *)DATA_PROTOCOL_IP;
      case PDP_TYPE_IPV6:
          return (char *)DATA_PROTOCOL_IPV6;
      case PDP_TYPE_IPV4V6:
          return (char *)DATA_PROTOCOL_IPV4V6;
      case PDP_TYPE_PPP:
          return (char *)DATA_PROTOCOL_PPP;
      default:
          return (char *)DATA_PROTOCOL_IP;
    } // end switch ~
}

///////////////////////////////////////////////////////////
// PsDataCallListBuilder
///////////////////////////////////////////////////////////
PsDataCallListBuilder::PsDataCallListBuilder(int halVer)
{
    mHalVer = halVer;
}
int PsDataCallListBuilder::AddDataCall(PdpContext *pPdpContext)
{
    if (pPdpContext == NULL) {
        return -1;
    }

    m_PdpContextList.push_back(pPdpContext);
    return 0;
}

const RilData *PsDataCallListBuilder::Build(int halVer)
{
    int size = m_PdpContextList.size();
    DataCallListResponse *rildata = new DataCallListResponse(size,halVer);
    if (rildata != NULL) {
        rildata->setIpv4DnsQueryFirst(m_bIpv4DnsQueryFirst);
        rildata->SetCurRrcState(mIsRrcCon);
        for (int i = 0; i < size; i++) {
            PdpContext *pPdpContext = m_PdpContextList[i];
            rildata->SetDataCall(i, pPdpContext, halVer);
        } // end for i ~
    }
    return rildata;
}

void PsDataCallListBuilder::Clear()
{
    m_PdpContextList.clear();
}

///////////////////////////////////////////////////////////
// PsDataNasTimerStatusBuilder
///////////////////////////////////////////////////////////
class NasTimerStatus : public RilData {
public:
    RIL_NasTimerStatus mResp;
    char mApn[MAX_PDP_APN_LEN];

    NasTimerStatus() {
        memset(&mResp, 0, sizeof(mResp));
        memset(mApn, 0, sizeof(mApn));
        mResp.apn = mApn;
    }
    virtual ~NasTimerStatus() {}

    void *GetData() const { return (void *)&mResp; }
    unsigned int GetDataLength() const { return sizeof(mResp); }
};

const RilData *PsDataNasTimerStatusBuilder::BuildNasTimerStatus(const SitNasTimerStatus *pNasTimerStatus)
{
    if (pNasTimerStatus == NULL)
        return NULL;

    NasTimerStatus *rildata = new NasTimerStatus();
    if (rildata != NULL) {
        rildata->mResp.type = pNasTimerStatus->type;
        rildata->mResp.status = pNasTimerStatus->status;
        rildata->mResp.value= pNasTimerStatus->value;
        strncpy(rildata->mResp.apn, pNasTimerStatus->apn, MAX_PDP_APN_LEN - 1);
    }
    return rildata;
}

