/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolpsadapterjson.h"
#include "rillog.h"
#include "psjsondef.h"
#include "pdpcontext.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_DATA, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define NUM_OF_ELM(a)       (int)(sizeof(a) / sizeof(a[0]))

// defined @ protocoladapter.cpp
extern int ConvertProtocolErrorCodeToRilErrorCode(int errorCode);

static void fillExtInfoForLpRetryTime(DataCall &out, Json::Value &jsonData)
{
    out.suggestedRetryTime = (int64_t) jsonData[ns_ps_setup_data_call_rsp::key_retry_timer].asInt64();
    RilLogV("retrytime:0x%llx", out.suggestedRetryTime);
    return;
}

static int fillAMBR(DataCall &out, Json::Value &jsonCallInfo)
{
    AMBR tempAMBR = {0, 0, 0, 0, 0, 0, 0};

    tempAMBR.octet2 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][0].asInt();
    tempAMBR.octet3 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][1].asInt();
    tempAMBR.octet4 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][2].asInt();
    tempAMBR.octet5 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][3].asInt();
    tempAMBR.octet6 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][4].asInt();
    tempAMBR.octet7 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][5].asInt();
    tempAMBR.octet8 = jsonCallInfo[ns_ps_call_info::key_apn_ambr][6].asInt();

    out.ambr = tempAMBR;
    RilLogV("octet2~8 = %d, (%d, %d), (%d, %d), (%d, %d)",
            out.ambr.octet2, out.ambr.octet3, out.ambr.octet4,
            out.ambr.octet5, out.ambr.octet6,
            out.ambr.octet7, out.ambr.octet8);

    return sizeof(sit_pdp_data_call_item_ext_ambr);
}

static void fillDefaultQosParam(DataCall &out, Json::Value &jsonCallInfo)
{
    RilLogV("fillDefaultQosParam");

    out.ipv6_mtu_size = (int)jsonCallInfo[ns_ps_call_info::key_ipv6_mtu].asUInt();
    out.pdu_session_id = (int)jsonCallInfo[ns_ps_call_info::key_pdu_session_id].asUInt(); // TODO: It is options key so handle accordingly

    out.defaultQosType = (RIL_QosType) jsonCallInfo[ns_ps_call_info::key_qos_rat_type].asInt(); // TODO: It is options key so handle accordingly

    Json::Value jsonQos = jsonCallInfo[ns_ps_call_info::key_qos];
    if(out.defaultQosType == EPS_QOS) {
        out.defaultEpsQos.qci = jsonQos[ns_ps_lte_qos_information::key_qci].asInt();
        out.defaultEpsQos.downlink.guaranteedBitrateKbps= (int)jsonQos[ns_ps_lte_qos_information::key_dl_guaranteed_bit_rate].asUInt();
        out.defaultEpsQos.downlink.maxBitrateKbps= (int)jsonQos[ns_ps_lte_qos_information::key_dl_max_bit_rate].asUInt();
        out.defaultEpsQos.uplink.guaranteedBitrateKbps= (int)jsonQos[ns_ps_lte_qos_information::key_ul_guaranteed_bit_rate].asUInt();
        out.defaultEpsQos.uplink.maxBitrateKbps= (int)jsonQos[ns_ps_lte_qos_information::key_ul_max_bit_rate].asUInt();
        return;
    }
    else if (out.defaultQosType == NR_QOS) {
        out.defaultNrQos.fiveQi= jsonQos[ns_ps_nr_qos_information::key_five_qi].asInt();
        out.defaultNrQos.downlink.guaranteedBitrateKbps= (int)jsonQos[ns_ps_nr_qos_information::key_dl_guaranteed_bit_rate].asUInt();
        out.defaultNrQos.downlink.maxBitrateKbps= (int)jsonQos[ns_ps_nr_qos_information::key_dl_max_bit_rate].asUInt();
        out.defaultNrQos.uplink.guaranteedBitrateKbps= (int)jsonQos[ns_ps_nr_qos_information::key_ul_guaranteed_bit_rate].asUInt();
        out.defaultNrQos.uplink.maxBitrateKbps= (int)jsonQos[ns_ps_nr_qos_information::key_ul_max_bit_rate].asUInt();
        out.defaultNrQos.qfi= jsonQos[ns_ps_nr_qos_information::key_qfi].asInt();
        out.defaultNrQos.averagingWindowMs= jsonQos[ns_ps_nr_qos_information::key_average_window].asInt();
        return;
    }
    else {
        RilLogV("fillDefaultQosParam, no info from CP");
        return;
    }

}

static void fillOptionalSliceInfo(DataCall &out, Json::Value &jsonCallInfo)
{

//Fill the default value first
    out.sliceInfo.sst = (RIL_SliceServiceType) 0;
    out.sliceInfo.sliceDifferentiator = -1;
    out.sliceInfo.mappedHplmnSst = (RIL_SliceServiceType) 0;
    out.sliceInfo.mappedHplmnSD = -1;

    int optionalSliceInfoType = jsonCallInfo[ns_ps_call_info::key_snssai_type].asInt();
//Fill received values
    switch(optionalSliceInfoType) {
      case OPT_SST: // 1
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_slice_service_type].asInt();
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_MAPSST: // 2
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_slice_service_type].asInt();
          out.sliceInfo.mappedHplmnSst=  (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sst].asInt();
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_SD:
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_slice_service_type].asInt();
          out.sliceInfo.sliceDifferentiator= jsonCallInfo[ns_ps_call_info::key_slice_differentiator].asInt();
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_SD_MAPSST:
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_slice_service_type].asInt();
          out.sliceInfo.sliceDifferentiator= jsonCallInfo[ns_ps_call_info::key_slice_differentiator].asInt();
          out.sliceInfo.mappedHplmnSD = jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sd].asInt();
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_SD_MAPSST_MAPSD:
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_slice_service_type].asInt();
          out.sliceInfo.sliceDifferentiator= jsonCallInfo[ns_ps_call_info::key_slice_differentiator].asInt();
          out.sliceInfo.mappedHplmnSst = (RIL_SliceServiceType) jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sst].asInt();
          out.sliceInfo.mappedHplmnSD = jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sd].asInt();
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      default:
          out.sliceInfoType = RIL_OPT_INFO_INVALID;
      break;
    }

    RilLogV("fillOptionalSliceInfo, %d %d %d %d %d", out.sliceInfoType, out.sliceInfo.sst,
               out.sliceInfo.sliceDifferentiator, out.sliceInfo.mappedHplmnSst, out.sliceInfo.mappedHplmnSD);
    return;
}

static int fillTDInfo(DataCall &out, Json::Value &jsonCallInfo)
{
    const int maxOsAppIdLen = 255;
    const int maxOsIdLen = 16;
    const int totalLen = maxOsAppIdLen + maxOsIdLen +1; //1BYTE for length
    int tdLen =  jsonCallInfo[ns_ps_call_info::key_traffic_descriptor_list].size();

    Json::Value jsonTDinfoList = jsonCallInfo[ns_ps_call_info::key_traffic_descriptor_list];

    out.tdLength = tdLen;
    RilLogV("fillTDInfo, tdLen: %d", tdLen);

    if(tdLen == 0) {
        RilLogV("fillTDInfo, tdLen is Zero");
        return 0;
    }
    if (tdLen > MAX_TD_LIST) {
        RilLogV("tdLen %d is over MAX_TD_LIST, set to MAX(%d)", tdLen, MAX_TD_LIST);
        out.tdLength = tdLen = MAX_TD_LIST;
    }

    for(int i = 0; i < tdLen; i++) {

        Json::Value jsonTDinfo = jsonTDinfoList[i];
        int tdBitMask = jsonTDinfo[ns_ps_data_call_traffic_descriptor::key_td_bit_mask].asInt();

        BYTE dnnflag = (BYTE)((tdBitMask & TD_FLAG_DNN) >> 1);
        RilLogV("fillTDInfo, dnnflag 0x%x", dnnflag);
        if (dnnflag) {
            out.tdInfo[i].dnn.resize(1);

            string dnn = jsonTDinfo[ns_ps_data_call_traffic_descriptor::key_dnn].asString();
            out.tdInfo[i].dnn[0].append(dnn, 0, MAX_DNN_LENGTH - 1);
        }

       /**
        * Byte array representing OSId + OSAppId. The minimum length of the array is
        * 18 and maximum length is 272 (16 bytes for OSId + 1 byte for OSAppId length
        * + up to 255 bytes for OSAppId).
        */
        BYTE osAppID = (BYTE)((tdBitMask & TD_FLAG_OSAPPID) >> 2);
        BYTE osID = (BYTE)((tdBitMask & TD_FLAG_OSID) >> 3);

        if (osAppID || osID) {
            RilLogV("fillTDInfo, osAppID exist");
            out.tdInfo[i].osAppIds.resize(1);
            memset(out.tdInfo[i].osAppIds[0].value, 0, totalLen);
        }

        if (osID) {
            for(int idx = 0 ; idx < maxOsIdLen ; idx++) {
                out.tdInfo[i].osAppIds[0].value[idx] = jsonTDinfo[ns_ps_data_call_traffic_descriptor::key_os_id][idx].asInt();
            }
        }

        if (osAppID) {
            int OsAppLen = 0;
            for(int idx = 0 ; idx < maxOsAppIdLen ; idx++) {
                out.tdInfo[i].osAppIds[0].value[maxOsIdLen+1+idx] = jsonTDinfo[ns_ps_data_call_traffic_descriptor::key_os_app_id][idx].asInt();

                if(out.tdInfo[i].osAppIds[0].value[maxOsIdLen+1+idx] != 0){
                    OsAppLen++;
                }
            }

            out.tdInfo[i].osAppIds[0].value[maxOsIdLen] = (BYTE) OsAppLen;
            RilLogV("fillTDInfo, osAppIDLen %d", out.tdInfo[i].osAppIds[0].value[maxOsIdLen]);
        }
    }
    return tdLen;
}

static void copyAddress(BYTE * dst, Json::Value &jsonAddr, int len){
    BYTE * temp_ipv = (BYTE *)calloc(len, sizeof(BYTE));
    for(int i = 0; i < (int)jsonAddr.size() ; i++) {
        temp_ipv[i] = (BYTE)jsonAddr[i].asInt();
    }

    memcpy(dst, temp_ipv, len);
    free(temp_ipv);
}

static void fillPcscfExtInfoForLegacy(DataCall &out)
{
    out.IPC_version = 1;
    out.pcscf_ext_count = 0;
}

static void FillPcscfExt(DataCall &out, Json::Value &jsonData, BYTE pcscf_type)
{
    Json::Value jsonCallInfo = jsonData[ns_ps_setup_data_call_rsp::key_call_info];

    if( (pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV4)
    {
        copyAddress(&out.ipv4.pcscf[MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM], jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4_ext], MAX_IPV4_ADDR_LEN * MAX_PCSCF_EXT_NUM);

        RilLogV("IPV4 PCSCF Copy to %p with %d bytes, offset:%d",
                 &out.ipv4.pcscf[MAX_IPV4_ADDR_LEN*MAX_PCSCF_NUM], MAX_IPV4_ADDR_LEN * MAX_PCSCF_EXT_NUM, MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
    }
    if( (pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV6)
    {
        copyAddress(&out.ipv6.pcscf[MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM], jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6_ext], MAX_IPV6_ADDR_LEN * MAX_PCSCF_EXT_NUM);

        RilLogV("IPV6 PCSCF Copy to %p with %d bytes, offset:%d",
                &out.ipv6.pcscf[MAX_IPV6_ADDR_LEN*MAX_PCSCF_NUM], MAX_IPV6_ADDR_LEN * MAX_PCSCF_EXT_NUM, MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }

    RilLogV("Decoded Information IPC_version:%d", out.IPC_version);
    out.pcscf_ext_count = 5;
}

// Now support extension chain
// dataCallList will get each data_call_item
static void processPcscfExtension(DataCall &out, Json::Value &jsonData)
{
    RilLogV("processPcscfExtension");

    if( out.IPC_version <= DC_V11) {
        RilLogV("Can't find next extension payload");
        fillPcscfExtInfoForLegacy(out);
        return;
    }

    Json::Value jsonCallInfo = jsonData[ns_ps_setup_data_call_rsp::key_call_info];

    int pcscfType = jsonCallInfo[ns_ps_call_info::key_pcscf_type].asInt();
    RilLogV("Found PCSCF Extension Payload, pcscf_type:%d", pcscfType);
    FillPcscfExt(out, jsonData, pcscfType);
    return;
}

// For v11
// Changed to care about one DataCall Item
// Indexing should be completed before calling.
static void GetDataCallFromDataCallList_v11(DataCall &out, Json::Value &jsonData)
{
    out.status = jsonData[ns_ps_setup_data_call_rsp::key_data_call_status].asInt();
    out.cid = jsonData[ns_ps_setup_data_call_rsp::key_cid].asInt();
    out.active = jsonData[ns_ps_setup_data_call_rsp::key_pdp_status].asInt();

    // Remap SIT_PDP_ACTIVE_* STATE to RIL DATA_CONNECTION_ACTIVE STATE
    if(out.active == SIT_PDP_ACTIVE_DORMANT) {
        out.active = ACTIVE_AND_LINKDOWN;
    } else if(out.active == SIT_PDP_ACTIVE_ACTIVE) {
        out.active = ACTIVE_AND_LINKUP;
    }

    Json::Value jsonCallInfo = jsonData[ns_ps_setup_data_call_rsp::key_call_info];
    out.pdpType = jsonCallInfo[ns_ps_call_info::key_pdp_type].asInt();

    // Do not store : out.dnsType, out.pcscfType
    //out.mtu_size= (int)data->ipv4_mtu_size;
    out.ipv4_mtu_size = (int)jsonCallInfo[ns_ps_call_info::key_ipv4_mtu].asUInt();
    out.pco = jsonCallInfo[ns_ps_call_info::key_pco_value].asInt();

    memset(&(out.ipv4), 0, sizeof(PDP_ADDR_V4));
    memset(&(out.ipv6), 0, sizeof(PDP_ADDR_V6));

    if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4) {
        out.ipv4.valid = TRUE;
        copyAddress(out.ipv4.addr, jsonCallInfo[ns_ps_call_info::key_ipv4_address], MAX_IPV4_ADDR_LEN);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV6) {
        out.ipv6.valid = TRUE;
        copyAddress(out.ipv6.addr, jsonCallInfo[ns_ps_call_info::key_ipv6_address], MAX_IPV6_ADDR_LEN);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
        out.ipv4.valid = TRUE;
        out.ipv6.valid = TRUE;

        copyAddress(out.ipv4.addr, jsonCallInfo[ns_ps_call_info::key_ipv4_address], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv6.addr, jsonCallInfo[ns_ps_call_info::key_ipv6_address], MAX_IPV6_ADDR_LEN);
    }

    // Add for DNS IPv4/IPv6 Chcker Routine
    if(jsonCallInfo[ns_ps_call_info::key_dns_type].asInt() == SIT_PDP_PDP_TYPE_IPV4)
    {
        copyAddress(out.ipv4.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], MAX_IPV4_ADDR_LEN);
    }
    else if(jsonCallInfo[ns_ps_call_info::key_dns_type].asInt() == SIT_PDP_PDP_TYPE_IPV6)
    {
        copyAddress(out.ipv6.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], MAX_IPV6_ADDR_LEN);
    }
    else if(jsonCallInfo[ns_ps_call_info::key_dns_type].asInt() == SIT_PDP_PDP_TYPE_IPV4IPV6)
    {
        copyAddress(out.ipv4.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv6.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], MAX_IPV6_ADDR_LEN);
    }

    if(jsonCallInfo[ns_ps_call_info::key_pcscf_type].asInt() == SIT_PDP_PDP_TYPE_IPV4)
    {
        copyAddress(out.ipv4.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if(jsonCallInfo[ns_ps_call_info::key_pcscf_type].asInt() == SIT_PDP_PDP_TYPE_IPV6)
    {
        copyAddress(out.ipv6.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if(jsonCallInfo[ns_ps_call_info::key_pcscf_type].asInt() == SIT_PDP_PDP_TYPE_IPV4IPV6)
    {
        copyAddress(out.ipv4.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
        copyAddress(out.ipv6.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }

    return;
}

// For under v10, This function can use v11 DataCall structure
// @return : processed data size
static void GetDataCallFromDataCallList_v10(DataCall &out, Json::Value &jsonData)
{
    out.status = jsonData[ns_ps_setup_data_call_rsp::key_data_call_status].asInt();
    out.cid = jsonData[ns_ps_setup_data_call_rsp::key_cid].asInt();
    out.active = jsonData[ns_ps_setup_data_call_rsp::key_pdp_status].asInt();

    Json::Value jsonCallInfo = jsonData[ns_ps_setup_data_call_rsp::key_call_info];
    out.pdpType = jsonCallInfo[ns_ps_call_info::key_pdp_type].asInt();

    memset(&(out.ipv4), 0, sizeof(PDP_ADDR_V4));
    memset(&(out.ipv6), 0, sizeof(PDP_ADDR_V6));

    if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4) {
        out.ipv4.valid = TRUE;
        copyAddress(out.ipv4.addr, jsonCallInfo[ns_ps_call_info::key_ipv4_address], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV6) {
        out.ipv6.valid = TRUE;
        copyAddress(out.ipv6.addr, jsonCallInfo[ns_ps_call_info::key_ipv6_address], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
        out.ipv4.valid = TRUE;
        copyAddress(out.ipv4.addr, jsonCallInfo[ns_ps_call_info::key_ipv4_address], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], MAX_IPV4_ADDR_LEN);
        copyAddress(out.ipv4.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);

        out.ipv6.valid = TRUE;
        copyAddress(out.ipv6.addr, jsonCallInfo[ns_ps_call_info::key_ipv6_address], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.dns1, jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.dns2, jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], MAX_IPV6_ADDR_LEN);
        copyAddress(out.ipv6.pcscf, jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }
    return;
}

/**
 * ProtocolPsSetupDataCallAdapterjson
 */
ProtocolPsSetupDataCallAdapterjson::ProtocolPsSetupDataCallAdapterjson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_SETUP_DATA_CALL), ProtocolPsSetupDataCallAdapterInterface()
{
    InitDataCall(m_dataCall);
    Init();
}

void ProtocolPsSetupDataCallAdapterjson::Init()
{
    if (m_bParsingSuccess == true) {

        int IPC_version = m_jsonData[ns_ps_setup_data_call_rsp::key_data_call_ipc_version].asInt();
        m_dataCall.IPC_version = IPC_version;

        if( IPC_version > DC_V10) {//payload_size <= m_pModemData->GetLength()
            RilLogV("Decode DC V11");
            GetDataCallFromDataCallList_v11(m_dataCall, m_jsonData);

            if( IPC_version > DC_V11 ) { // payload_size <= m_pModemData->GetLength()
                RilLogV("Decode PCSCF EXT");
                // Extension Payload, IPC Version 2 is expected
                // Process Extension
                processPcscfExtension(m_dataCall, m_jsonData);

                switch(IPC_version) {
                    case DC_SLICE_TDINFO:
                        RilLogV("Decode Retry SuggestedTime");
                        fillExtInfoForLpRetryTime(m_dataCall, m_jsonData);
                        RilLogV("Decode AMBR octet2~7, CID:%d", m_jsonData[ns_ps_setup_data_call_rsp::key_cid].asInt());
                        fillAMBR(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                        RilLogV("Decode DefaultQoS");
                        fillDefaultQosParam(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                        //Newly-addedPart
                        RilLogV("Decode Optional SliceInfo");
                        fillOptionalSliceInfo(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                        if(checkValidTD(m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]))  {
                            RilLogV("Decode Traffic Descriptor");
                            fillTDInfo(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                        }
                    break;
                    case DC_QOS:
                        RilLogV("Decode Retry SuggestedTime");
                        fillExtInfoForLpRetryTime(m_dataCall, m_jsonData);
                        RilLogV("Decode AMBR octet2~7, CID:%d", m_jsonData[ns_ps_setup_data_call_rsp::key_cid].asInt());
                        fillAMBR(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                        RilLogV("Decode DefaultQoS, CID:%d", m_jsonData[ns_ps_setup_data_call_rsp::key_cid].asInt());
                        fillDefaultQosParam(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                    break;
                    case DC_AMBR:
                        RilLogV("Decode AMBR octet2~7");
                        fillAMBR(m_dataCall, m_jsonData[ns_ps_setup_data_call_rsp::key_call_info]);
                        RilLogV("Decode Retry SuggestedTime");
                        fillExtInfoForLpRetryTime(m_dataCall, m_jsonData);
                    break;
                    case DC_RETRYTIME:
                        RilLogV("Decode Retry SuggestedTime");
                        fillExtInfoForLpRetryTime(m_dataCall, m_jsonData);
                    break;
                }

            } else {
                fillPcscfExtInfoForLegacy(m_dataCall);
                return;
            }
        } else {
            RilLogV("Decode DC V10");
            GetDataCallFromDataCallList_v10(m_dataCall, m_jsonData);
            fillPcscfExtInfoForLegacy(m_dataCall);
        }
    }
    RilLogV("ProtocolPsSetupDataCallAdapterjson : Init() - X");
}

UINT ProtocolPsSetupDataCallAdapterjson::GetErrorCode() const {
    UINT errorCode = ProtocolJsonRespAdapter::GetErrorCode();
    switch (errorCode) {
    case RCM_E_SUCCESS:
        return RIL_E_SUCCESS;
    case RCM_E_RADIO_NOT_AVAILABLE:
        return RIL_E_RADIO_NOT_AVAILABLE;
    case RCM_E_GENERIC_FAILURE:
        return RIL_E_GENERIC_FAILURE;
    case RCM_E_REQUEST_NOT_SUPPORTED:
        return RIL_E_REQUEST_NOT_SUPPORTED;
    case RCM_E_OP_NOT_ALLOWED_DURING_VOICE_CALL:
        return RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL;
    case RCM_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW:
        return RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW;
    } // end switch ~

    return RIL_E_GENERIC_FAILURE;
}


int ProtocolPsSetupDataCallAdapterjson::GetStatus() const {
    return m_dataCall.status;
}

int ProtocolPsSetupDataCallAdapterjson::GetCid() const {
    return m_dataCall.cid;
}

int ProtocolPsSetupDataCallAdapterjson::GetActiveStatus() const {
    return m_dataCall.active;
}

int ProtocolPsSetupDataCallAdapterjson::GetPdpType() const {
    return m_dataCall.pdpType;
}

int ProtocolPsSetupDataCallAdapterjson::GetAddrInfo(PDP_ADDR *pAddr, int pdpType) const {
    if (pAddr == NULL) {
        return -1;
    }

    memset(pAddr, 0, sizeof(PDP_ADDR));

    switch (pdpType) {
    case SIT_PDP_PDP_TYPE_IPV4:
    case SIT_PDP_PDP_TYPE_IPV6:
    case SIT_PDP_PDP_TYPE_IPV4IPV6:
    case SIT_PDP_PDP_TYPE_PPP:
        break;
    default:
        return -1;
    }

    memset(pAddr, 0, sizeof(PDP_ADDR));
    if (pdpType == SIT_PDP_PDP_TYPE_IPV4 || pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6 ||
            pdpType == SIT_PDP_PDP_TYPE_PPP) {
        if (m_dataCall.ipv4.valid) {
            memcpy(&pAddr->ipv4, &m_dataCall.ipv4, sizeof(m_dataCall.ipv4));
        }
    }

    if (pdpType == SIT_PDP_PDP_TYPE_IPV6 || pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
        if (m_dataCall.ipv6.valid) {
            memcpy(&pAddr->ipv6, &m_dataCall.ipv6, sizeof(m_dataCall.ipv6));
        }
    }

    return 0;
}

int ProtocolPsSetupDataCallAdapterjson::GetAddrInfo(PDP_ADDR *pAddr) const {
    if (pAddr == NULL) {
        return -1;
    }

    // PDP_ADDR is union type, it's same to PDP_ADDR_V6
    memset(pAddr, 0, sizeof(PDP_ADDR));
    memcpy(&pAddr->ipv4, &m_dataCall.ipv4, sizeof(m_dataCall.ipv4));
    memcpy(&pAddr->ipv6, &m_dataCall.ipv6, sizeof(m_dataCall.ipv6));
    return 0;
}

int ProtocolPsSetupDataCallAdapterjson::GetMTUv4() const {
    return m_dataCall.ipv4_mtu_size;
}

int ProtocolPsSetupDataCallAdapterjson::GetMTUv6() const {
    return m_dataCall.ipv6_mtu_size;
}

int ProtocolPsSetupDataCallAdapterjson::GetPCO() const {
    return m_dataCall.pco;
}

int64_t ProtocolPsSetupDataCallAdapterjson::GetSuggestedRetryTime() const {
    return m_dataCall.suggestedRetryTime;
}

const DataCall * ProtocolPsSetupDataCallAdapterjson::GetDataCall() const {
    return &m_dataCall;
}

/**
 * ProtocolPsDataCallListAdapterjson
 */
ProtocolPsDataCallListAdapterjson::ProtocolPsDataCallListAdapterjson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_GET_DATA_CALL_LIST), ProtocolPsDataCallListAdapterInterface() {
    m_dataCallNum = 0;
    for(int i = 0; i < MAX_DATA_CALL_SIZE; i++){
        InitDataCall(m_dataCallList[i]);
    }
    Init();
}

void ProtocolPsDataCallListAdapterjson::Init()
{
    m_dataCallNum = 0;

    if (m_bParsingSuccess  && GetErrorCode() == RCM_E_SUCCESS) {
        m_dataCallNum = processDataCallList(m_jsonData, m_dataCallList);
    }
}

unsigned int ProtocolPsDataCallListAdapterjson::GetDataCallNum() const {
    return m_dataCallNum;
}

const DataCall *ProtocolPsDataCallListAdapterjson::GetDataCallList() const
{
    if (m_dataCallNum == 0)
        return NULL;
    return m_dataCallList;
}

const DataCall *ProtocolPsDataCallListAdapterjson::GetDataCallByIndex(unsigned int index) const
{
    if (index >= m_dataCallNum) {
        return NULL;
    }
    return &m_dataCallList[index];
}

const DataCall *ProtocolPsDataCallListAdapterjson::GetDataCallByCid(int cid) const
{
    for (unsigned int i = 0; i < m_dataCallNum; i++) {
        if (m_dataCallList[i].cid == cid)
            return &m_dataCallList[i];
    }
    return NULL;
}

UINT ProtocolPsDataCallListAdapterjson::GetErrorCode() const {
    return ProtocolJsonRespAdapter::GetErrorCode();
}

/**
 * ProtocolPsDataCallListChangedAdapterjson
 */
ProtocolPsDataCallListChangedAdapterjson::ProtocolPsDataCallListChangedAdapterjson(const ModemData *pModemData)
    :  ProtocolJsonIndAdapter(pModemData, SIT_IND_DATA_CALL_LIST_CHANGED), ProtocolPsDataCallListChangedAdapterInterface() {
    m_dataCallNum = 0;

    for(int i = 0; i < MAX_DATA_CALL_SIZE; i++){
        InitDataCall(m_dataCallList[i]);
    }
    Init();
}

void ProtocolPsDataCallListChangedAdapterjson::Init()
{
    m_dataCallNum = 0;

    if (m_bParsingSuccess) {
        m_dataCallNum = processDataCallList(m_jsonData, m_dataCallList);
    }
}

unsigned int ProtocolPsDataCallListChangedAdapterjson::GetDataCallNum() const {
    return m_dataCallNum;
}

const DataCall *ProtocolPsDataCallListChangedAdapterjson::GetDataCallList() const
{
    if (m_dataCallNum == 0)
        return NULL;
    return m_dataCallList;
}

const DataCall *ProtocolPsDataCallListChangedAdapterjson::GetDataCallByIndex(unsigned int index) const
{
    if (index >= m_dataCallNum) {
        return NULL;
    }
    return &m_dataCallList[index];
}

const DataCall *ProtocolPsDataCallListChangedAdapterjson::GetDataCallByCid(int cid) const
{
    for (unsigned int i = 0; i < m_dataCallNum; i++) {
        if (m_dataCallList[i].cid == cid)
            return &m_dataCallList[i];
    }
    return NULL;
}

UINT ProtocolPsDataCallListChangedAdapterjson::GetErrorCode() const
{
    // Follow RespAdapter
    if (m_pModemData != NULL) {
        if (GetType() == RCM_TYPE_INDICATION)
            return RCM_E_SUCCESS;
        else if(GetType() == RCM_TYPE_RESPONSE) {
            const RCM_HEADER *rcmdata = (RCM_HEADER *)m_pModemData->GetRawData();
            if (rcmdata != NULL && m_pModemData->GetLength() >= (int)sizeof(RCM_HEADER)) {
                    return ConvertProtocolErrorCodeToRilErrorCode((UINT)(rcmdata->ext.rsp.error & 0xFF));
            }
        }
    }
    // default error code
    return RIL_E_GENERIC_FAILURE;
}

bool JsonDataCallUtil::checkValidTD(Json::Value &jsonData) {
    int tdSize = jsonData[ns_ps_call_info::key_traffic_descriptor_list].size();
    if (tdSize <= 0) return false;
    else return true;
}

/* DCsizes = payloadsize - sizeof(BYTE) { DataCallNum has 1 byte variable } */
int JsonDataCallUtil::processDataCallList(Json::Value &jsonData, DataCall (&dataCallList)[MAX_DATA_CALL_SIZE])
{
    int dataCallNum = jsonData[ns_ps_data_call_list_changed_ind::key_data_call_info_list].size();
    if (dataCallNum != 0) {
        dc_type dct = DetermineDCType(dataCallNum, jsonData);

        if (dataCallNum > NUM_OF_ELM(dataCallList)) {
            dataCallNum = NUM_OF_ELM(dataCallList);
        }

        RilLogV("DataCallNum:%d", dataCallNum);

        if (dct == DC_SLICE_TDINFO)
            FillDataCallListFromModemPayload_ForTD(dataCallList, dataCallNum, jsonData);
        else
            FillDataCallListFromModemPayload(dataCallList, dataCallNum, jsonData, dct);
    }
    return dataCallNum;
}

dc_type JsonDataCallUtil::DetermineDCType(int num, Json::Value &jsonData)
{
    dc_type dct = DC_V10;

    Json::Value jsonDataCallList = jsonData[ns_ps_data_call_list_changed_ind::key_data_call_info_list];

    Json::Value jsonDataCall = jsonDataCallList[0];
    if(dct != (dc_type)jsonDataCall[ns_ps_setup_data_call_rsp::key_data_call_ipc_version].asInt()){
        dct = (dc_type)jsonDataCall[ns_ps_setup_data_call_rsp::key_data_call_ipc_version].asInt();
    }
    for(int i = 0 ; i < num ; i++){
        Json::Value jsonDataCall = jsonDataCallList[i];
        if(dct != (dc_type)jsonDataCall[ns_ps_setup_data_call_rsp::key_data_call_ipc_version].asInt()){
            RilLogV("The ipc versions are different.");
        }
    }

    RilLogV("dc_type = %d", dct);
    return dct;
}

int JsonDataCallUtil::GetDataCallFromList(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], Json::Value &jsonDataCall, int idx) {

    GetDataCallFromDataCallList_v11(m_dataCallList[idx], jsonDataCall);
    processPcscfExtension(m_dataCallList[idx], jsonDataCall);
    m_dataCallList[idx].IPC_version = jsonDataCall[ns_ps_setup_data_call_rsp::key_data_call_ipc_version].asInt();

    return m_dataCallList[idx].IPC_version;
}

void JsonDataCallUtil::FillDataCallListFromModemPayload(
    DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const unsigned int m_dataCallNum, Json::Value &jsonData, dc_type dct)
{
    switch(dct) {
    case DC_QOS:  // 5
    case DC_AMBR:  // 4
    case DC_RETRYTIME:  // 3
    case DC_EXTENSION:  // 2
        for (unsigned int i = 0; i < m_dataCallNum; i++) {

            Json::Value jsonDataCall = jsonData[ns_ps_data_call_list_changed_ind::key_data_call_info_list][i];

            int IPC_version = GetDataCallFromList(m_dataCallList, jsonDataCall, i);

            if (IPC_version >= 3) {
                fillExtInfoForLpRetryTime(m_dataCallList[i], jsonDataCall);
            }
            if (IPC_version >= 4) {
                RilLogV("Decode AMBR octet2~7, CID:%d", m_dataCallList[i].cid);
                fillAMBR(m_dataCallList[i], jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]);
            }
            if (IPC_version >= 5) {
                RilLogV("Decode QOS, CID:%d", m_dataCallList[i].cid);
                fillDefaultQosParam(m_dataCallList[i], jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]);
            }
            if (IPC_version < 2) {
                RilLogV("Invalid IPC version:%d", IPC_version);
            }
        } // end for i ~
        break;
    case DC_V11:
        for (unsigned int i = 0; i < m_dataCallNum; i++) {
            Json::Value jsonDataCall = jsonData[ns_ps_data_call_list_changed_ind::key_data_call_info_list][i];
            GetDataCallFromDataCallList_v11(m_dataCallList[i], jsonDataCall);
            fillPcscfExtInfoForLegacy(m_dataCallList[i]);
        } // end for i ~
        break;
    case DC_V10:
    default:
        // Deprecated.
        RilLogV("This format is no more used. Something other problem");
        break;
    }
}

void JsonDataCallUtil::FillDataCallListFromModemPayload_ForTD(
    DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const unsigned int m_dataCallNum, Json::Value &jsonData) {

    for (unsigned int i = 0; i < m_dataCallNum; i++) {
        m_dataCallList[i].IPC_version = DC_SLICE_TDINFO;

        Json::Value jsonDataCall = jsonData[ns_ps_data_call_list_changed_ind::key_data_call_info_list][i];

        GetDataCallFromDataCallList_v11(m_dataCallList[i], jsonDataCall);

        processPcscfExtension(m_dataCallList[i], jsonDataCall);

        fillExtInfoForLpRetryTime(m_dataCallList[i], jsonDataCall);

        RilLogV("1.Decode AMBR octet2~7, CID:%d", m_dataCallList[i].cid);
        fillAMBR(m_dataCallList[i], jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]);

        RilLogV("2.Decode QOS, CID:%d", m_dataCallList[i].cid);
        fillDefaultQosParam(m_dataCallList[i], jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]);

        RilLogV("3.Decode SliceInfo/TD Info, CID:%d", m_dataCallList[i].cid);
        fillOptionalSliceInfo(m_dataCallList[i], jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]);

        if(checkValidTD(jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]))  {
            //Check the next datacall addr, 1byte needs for td length
            fillTDInfo(m_dataCallList[i], jsonDataCall[ns_ps_setup_data_call_rsp::key_call_info]);
        } else {
            RilLogW("!!!WARN!!! parsing of nextdatacall is not guaranteed, just assume no TD");
        }
    }
}

/**
 * ProtocolPsNasTimerStatusAdapterjson
 */
ProtocolPsNasTimerStatusAdapterjson::ProtocolPsNasTimerStatusAdapterjson(const ModemData *pModemData)
             : ProtocolJsonIndAdapter(pModemData, SIT_IND_NAS_TIMER_STATUS) {
    m_NasTimerStatus = {};
}

const SitNasTimerStatus * ProtocolPsNasTimerStatusAdapterjson::GetNasTimerStatus()
{
    if (m_bParsingSuccess) {
        m_NasTimerStatus.type = m_jsonData[ns_ps_nas_timer_status_ind::key_timer_type].asInt();
        m_NasTimerStatus.status = m_jsonData[ns_ps_nas_timer_status_ind::key_timer_status].asInt();
        m_NasTimerStatus.value = m_jsonData[ns_ps_nas_timer_status_ind::key_timer_value].asInt();

        string apn = m_jsonData[ns_ps_nas_timer_status_ind::key_apn].asString();
        strncpy(m_NasTimerStatus.apn, apn.c_str(), MIN(apn.length(), MAX_PDP_APN_LEN - 1));

        return &m_NasTimerStatus;
    }
    return NULL;
}

/**
 * ProtocolPsStartKeepAliveAdapterjson
 */
unsigned int ProtocolPsStartKeepAliveAdapterjson::getSessionHandle() const {
    unsigned int ret = 0;
    if (m_bParsingSuccess) {
        ret = m_jsonData[ns_ps_start_keepalive_rsp::key_session_handle].asInt();
    }
    return ret;
}

int ProtocolPsStartKeepAliveAdapterjson::getCode() const {
    int status = KEEPALIVE_INACTIVE;
    if (m_bParsingSuccess) {
        status = m_jsonData[ns_ps_start_keepalive_rsp::key_status_code].asInt();
    }
    return status;
}

/**
 * ProtocolPsKeepAliveStatusAdapterjson
 */
unsigned int ProtocolPsKeepAliveStatusAdapterjson::getSessionHandle() const {
    unsigned int ret = 0;
    if (m_bParsingSuccess) {
        ret = m_jsonData[ns_ps_keepalive_status_ind::key_session_handle].asInt();
    }
    return ret;
}

int ProtocolPsKeepAliveStatusAdapterjson::getCode() const {
    int status = KEEPALIVE_INACTIVE;
    if (m_bParsingSuccess) {
        status = m_jsonData[ns_ps_keepalive_status_ind::key_status_code].asInt();
    }
    return status;
}

/**
 * ProtocolPsPcoDataAdapterjson
 */
ProtocolPsPcoDataAdapterjson::ProtocolPsPcoDataAdapterjson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_PCO_DATA)
{
    nextPcoBlockPos = 0;
    remainPcoBlocks = 0;

    if (m_bParsingSuccess) {
        remainPcoBlocks = m_jsonData[ns_ps_pco_data_ind::key_operator_pco_info_list].size();
    }
}

int ProtocolPsPcoDataAdapterjson::GetCid() const
{
    int ret = 0;
    if (m_bParsingSuccess) {
        ret = m_jsonData[ns_ps_pco_data_ind::key_cid].asInt();
    }
    return ret;
}

int ProtocolPsPcoDataAdapterjson::GetPdpType() const
{
    int ret = PDP_TYPE_IPV4;
    if (m_bParsingSuccess) {
        ret = m_jsonData[ns_ps_pco_data_ind::key_pdp_type].asInt();
    }
    return ret;
}

int ProtocolPsPcoDataAdapterjson::GetPcoData(sit_pdp_pco_data_entry &pco_data)
{
    if (remainPcoBlocks == 0) return -1;
    memset(contents, 0, sizeof(contents));

    if (m_bParsingSuccess) {
       Json::Value jsonContests = m_jsonData[ns_ps_pco_data_ind::key_operator_pco_info_list][remainPcoBlocks - 1];
       pco_data.pco_id = jsonContests[ns_ps_operator_pco_info::key_pco_id].asInt();
       pco_data.contents_len= jsonContests[ns_ps_operator_pco_info::key_contents].size();
       for(int i = 0; i < pco_data.contents_len; i++){
          contents[i] = (char)jsonContests[ns_ps_operator_pco_info::key_contents][i].asInt();
       }
       pco_data.contents = contents;
    }

     return --remainPcoBlocks;
 }

 int ProtocolPsPcoDataAdapterjson::GetPcoNum() const
 {
     int ret = 0;
     if (m_bParsingSuccess) {
         ret = m_jsonData[ns_ps_pco_data_ind::key_operator_pco_info_list].size();
     }
     return ret;
 }

/**
 * ProtocolPsDataOffStatusAdapterjson
 */
ProtocolPsDataOffStatusAdapterjson::ProtocolPsDataOffStatusAdapterjson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_PS_DATA_OFF_STATUS), ProtocolPsDataOffStatusAdapterInterface()
{
    psDataOffAvailableStatus = -1;
    exemptListForHome = -1;
    exemptListForRoam = -1;

    if (m_bParsingSuccess) {
        psDataOffAvailableStatus = m_jsonData[ns_ps_data_off_status_ind::key_ps_data_off_status].asInt();
        exemptListForHome = m_jsonData[ns_ps_data_off_status_ind::key_exempt_home_list].asInt();
        exemptListForRoam = m_jsonData[ns_ps_data_off_status_ind::key_exempt_roam_list].asInt();
    }
}

int ProtocolPsDataOffStatusAdapterjson::GetPsDataOffAvailableStatus() const
{
    return psDataOffAvailableStatus;
}

int ProtocolPsDataOffStatusAdapterjson::GetExemptListForHome() const
{
    return exemptListForHome;
}

int ProtocolPsDataOffStatusAdapterjson::GetExemptListForRoam() const
{
    return exemptListForRoam;
}

/**
 * ProtocolPsIpAddrStatusAdapterjson
 */
ProtocolPsIpAddrStatusAdapterjson::ProtocolPsIpAddrStatusAdapterjson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_IP_ADDR_STATUS), ProtocolPsIpAddrStatusAdapterInterface()
{
    cid = status = -1;
    memset(ipv4_addr, 0, MAX_IPV4_ADDR_LEN);
    memset(ipv6_addr, 0, MAX_IPV6_ADDR_LEN);
    Init();
}

void ProtocolPsIpAddrStatusAdapterjson::Init()
{
   if (m_bParsingSuccess) {
       status = m_jsonData[ns_ps_ip_addr_status_ind::key_status].asInt();
       cid = m_jsonData[ns_ps_ip_addr_status_ind::key_cid].asInt();
       copyAddress(ipv4_addr, m_jsonData[ns_ps_ip_addr_status_ind::key_ipv4_address], MAX_IPV4_ADDR_LEN);
       copyAddress(ipv6_addr, m_jsonData[ns_ps_ip_addr_status_ind::key_ipv6_address], MAX_IPV6_ADDR_LEN);
   }
}

int ProtocolPsIpAddrStatusAdapterjson::GetCid() const
{
    return cid;
}

int ProtocolPsIpAddrStatusAdapterjson::GetStatus() const
{
    return status;
}

void ProtocolPsIpAddrStatusAdapterjson::GetAddrInfo(char (&ipv4addr)[MAX_IPV4_ADDR_LEN], char (&ipv6addr)[MAX_IPV6_ADDR_LEN]) const
{
    memcpy(ipv4addr, &ipv4_addr, MAX_IPV4_ADDR_LEN);
    memcpy(ipv6addr, &ipv6_addr, MAX_IPV6_ADDR_LEN);
}

/**
 * ProtocolPsUnthrottleApnjson
 */
ProtocolPsUnthrottleApnjson::ProtocolPsUnthrottleApnjson(const ModemData *pModemData)
    : ProtocolJsonIndAdapter(pModemData, SIT_IND_UNTHROTTLE_APN), ProtocolPsUnthrottleApnInterface()
{
    memset(m_apn, 0, sizeof(m_apn));
    if (m_bParsingSuccess) {
       string apn = m_jsonData[ns_ps_unthrottle_apn_ind::key_apn].asString();
       strncpy(m_apn, apn.c_str(), MIN(apn.length(), MAX_PDP_APN_LEN - 1));
    }
}

const char *ProtocolPsUnthrottleApnjson::GetApn() const
{
    return m_apn;
}

/**
 * ProtocolPsAllocatePduSessionIdjson
 */
ProtocolPsAllocatePduSessionIdjson::ProtocolPsAllocatePduSessionIdjson(const ModemData *pModemData)
    : ProtocolJsonRespAdapter(pModemData, SIT_ALLOC_PDU_SESSION), ProtocolPsAllocatePduSessionIdInterface()
{
    m_sessionId = -1;

    if (m_bParsingSuccess) {
       m_sessionId = m_jsonData[ns_ps_alloc_pdu_session_rsp::key_session_id].asInt();
    }
}

int ProtocolPsAllocatePduSessionIdjson::GetSessionId() const
{
    return m_sessionId;
}

UINT ProtocolPsAllocatePduSessionIdjson::GetErrorCode() const
{
    return ProtocolRespAdapter::GetErrorCode();
}

/**
 * ProtocolPsDedicatedBearInfoAdapterjson
 */
ProtocolPsDedicatedBearInfoAdapterjson::ProtocolPsDedicatedBearInfoAdapterjson(const ModemData *pModemData)
      : ProtocolJsonIndAdapter(pModemData, SIT_IND_DEDICATED_BEARER_INFO)
{
     m_cid = -1;
     m_status = -1;
     memset(&m_QosSession, 0, sizeof(m_QosSession));
     m_QosSession.qosSessionId = -1;
     processQosSession();
}

void ProtocolPsDedicatedBearInfoAdapterjson::processQosSession()
{
    if (m_bParsingSuccess) {
        int fieldFlag = 0;

        m_QosSession.qosSessionId = m_jsonData[ns_ps_dedicated_bearer_info_ind::key_qos_session_id].asInt();;
        m_cid = m_jsonData[ns_ps_dedicated_bearer_info_ind::key_default_cid].asInt();;
        m_status = m_jsonData[ns_ps_dedicated_bearer_info_ind::key_status].asInt();
        m_QosSession.defaultQosType = (RIL_QosType)m_jsonData[ns_ps_dedicated_bearer_info_ind::key_qos_rat_type].asInt();

        Json::Value qosType = m_jsonData[ns_ps_dedicated_bearer_info_ind::key_qos];

        if(m_QosSession.defaultQosType == EPS_QOS) {
            m_QosSession.defaultEpsQos.qci = (uint16_t)qosType[ns_ps_lte_qos_information::key_qci].asUInt();
            m_QosSession.defaultEpsQos.downlink.guaranteedBitrateKbps = (int)qosType[ns_ps_lte_qos_information::key_dl_guaranteed_bit_rate].asUInt();
            m_QosSession.defaultEpsQos.downlink.maxBitrateKbps = (int)qosType[ns_ps_lte_qos_information::key_dl_max_bit_rate].asUInt();
            m_QosSession.defaultEpsQos.uplink.guaranteedBitrateKbps = (int)qosType[ns_ps_lte_qos_information::key_ul_guaranteed_bit_rate].asUInt();
            m_QosSession.defaultEpsQos.uplink.maxBitrateKbps = (int)qosType[ns_ps_lte_qos_information::key_ul_max_bit_rate].asUInt();
        }
        else if (m_QosSession.defaultQosType == NR_QOS) {
            m_QosSession.defaultNrQos.fiveQi = (uint16_t)qosType[ns_ps_nr_qos_information::key_five_qi].asUInt();
            m_QosSession.defaultNrQos.downlink.guaranteedBitrateKbps = (int)qosType[ns_ps_nr_qos_information::key_dl_guaranteed_bit_rate].asUInt();
            m_QosSession.defaultNrQos.downlink.maxBitrateKbps = (int)qosType[ns_ps_nr_qos_information::key_dl_max_bit_rate].asUInt();
            m_QosSession.defaultNrQos.uplink.guaranteedBitrateKbps = (int)qosType[ns_ps_nr_qos_information::key_ul_guaranteed_bit_rate].asUInt();
            m_QosSession.defaultNrQos.uplink.maxBitrateKbps = (int)qosType[ns_ps_nr_qos_information::key_ul_max_bit_rate].asUInt();
            m_QosSession.defaultNrQos.qfi = (uint8_t)qosType[ns_ps_nr_qos_information::key_qfi].asUInt();
            m_QosSession.defaultNrQos.averagingWindowMs = (uint16_t)qosType[ns_ps_nr_qos_information::key_average_window].asUInt();
        }
        else {
            RilLogV("fillDefaultQosParam, no info from CP");
        }

        m_QosSession.QosFilterCnt = m_jsonData[ns_ps_dedicated_bearer_info_ind::key_packet_filter_list].size();
        // Max value of QosFilterCnt is 255.
        if(m_QosSession.QosFilterCnt > 255){
            m_QosSession.QosFilterCnt = 255;
        }
        RilLogV("QosFilterCnt : %d", m_QosSession.QosFilterCnt);
        m_QosSession.QosFilters.resize(m_QosSession.QosFilterCnt);
        for(int i = 0 ; i < m_QosSession.QosFilterCnt; i++)
        {
            BYTE local_address[MAX_PDP_ADDRESS_LEN] = {0, };
            BYTE remote_address[MAX_PDP_ADDRESS_LEN] = {0, };

            Json::Value packetFilter = m_jsonData[ns_ps_dedicated_bearer_info_ind::key_packet_filter_list][i];

            fieldFlag = packetFilter[ns_ps_packet_filter_information::key_field_flag].asInt();

            for(int j = 0; j < MAX_PDP_ADDRESS_LEN ; j++)
            {
                local_address[j] = packetFilter[ns_ps_packet_filter_information::key_local_address][j].asInt();
                remote_address[j] = packetFilter[ns_ps_packet_filter_information::key_remote_address][j].asInt();
            }

            memcpy(m_QosSession.QosFilters[i].localAddress[0], local_address, 16);
            memcpy(m_QosSession.QosFilters[i].localAddress[1], local_address + 16, 4);
            // Waiting for SIT document revision.
            m_QosSession.QosFilters[i].localAddrPrefix_v6 = packetFilter[ns_ps_packet_filter_information::key_local_address_prefix_ipv6].asInt();
            m_QosSession.QosFilters[i].localAddrPrefix_v4 = packetFilter[ns_ps_packet_filter_information::key_local_address_prefix_ipv4].asInt();

            memcpy(m_QosSession.QosFilters[i].remoteAddress[0], remote_address, 16);
            memcpy(m_QosSession.QosFilters[i].remoteAddress[1], remote_address + 16, 4);
            // Waiting for SIT document revision.
            m_QosSession.QosFilters[i].remotePortprefix_6 = packetFilter[ns_ps_packet_filter_information::key_remote_address_prefix_ipv6].asInt();
            m_QosSession.QosFilters[i].remotePortprefix_4 = packetFilter[ns_ps_packet_filter_information::key_remote_address_prefix_ipv4].asInt();

            if(fieldFlag & 0x01){
                m_QosSession.QosFilters[i].localPortFlag = MAYBE_PORT_VALID;
            }
            m_QosSession.QosFilters[i].localPortStart= packetFilter[ns_ps_packet_filter_information::key_local_port_low].asInt();
            m_QosSession.QosFilters[i].localPortEnd= packetFilter[ns_ps_packet_filter_information::key_local_port_high].asInt();

            if(fieldFlag & 0x02){
                m_QosSession.QosFilters[i].remotePortFlag = MAYBE_PORT_VALID;
            }
            m_QosSession.QosFilters[i].remotePortStart= packetFilter[ns_ps_packet_filter_information::key_remote_prot_low].asInt();
            m_QosSession.QosFilters[i].remotePortEnd= packetFilter[ns_ps_packet_filter_information::key_remote_port_high].asInt();

            m_QosSession.QosFilters[i].protocol= packetFilter[ns_ps_packet_filter_information::key_qos_protocol].asInt();
            if(fieldFlag & 0x04){
                m_QosSession.QosFilters[i].tosFlag = 1;
            }
            m_QosSession.QosFilters[i].tosValue= packetFilter[ns_ps_packet_filter_information::key_type_of_service].asInt();

            if(fieldFlag & 0x08){
                m_QosSession.QosFilters[i].flowLabelFlag = IPV6_FLOW_LABEL_VALID;
            }
            m_QosSession.QosFilters[i].flowLabelValue= packetFilter[ns_ps_packet_filter_information::key_ipv6_flow_label].asInt();

            if(fieldFlag & 0x10){
                m_QosSession.QosFilters[i].spiFlag = IP_SEC_SPI_VALID;
            }
            m_QosSession.QosFilters[i].spiValue = packetFilter[ns_ps_packet_filter_information::key_ip_sec_spi].asInt();

            // Does -1 to convert the value defined in the SIT interface to the value defined in hal.type.
            m_QosSession.QosFilters[i].direction= packetFilter[ns_ps_packet_filter_information::key_direction].asInt() - 1;
            m_QosSession.QosFilters[i].precedence= packetFilter[ns_ps_packet_filter_information::key_precedence].asInt();
        }

   }
}

const DedicatedBearerInfo * ProtocolPsDedicatedBearInfoAdapterjson::GetDedicatedBearerInfo()
{
    return NULL;
}

int ProtocolPsDedicatedBearInfoAdapterjson::GetCid() const
{
    return m_cid;
}

int ProtocolPsDedicatedBearInfoAdapterjson::GetStatus() const
{
    return m_status;
}

const QosSession_t &ProtocolPsDedicatedBearInfoAdapterjson::GetQosSessionInfo() const
{
    return m_QosSession;
}

 /**
  * ProtocolPsSlicingConfigjson
  */
ProtocolPsSlicingConfigjson::ProtocolPsSlicingConfigjson(const ModemData *pModemData)
      : ProtocolJsonRespAdapter(pModemData, SIT_GET_SLICING_CONFIG)
{
     m_ipcVersion = 0;

     m_slicingConfig.urspNum = 0;
     m_slicingConfig.urspRules.clear();
     m_slicingConfig.sliceInfoNum = 0;
     m_slicingConfig.sliceInfo.clear();
     processSlicingConfig();
}

void ProtocolPsSlicingConfigjson::processSlicingConfig()
{

    if (m_bParsingSuccess) {
        m_ipcVersion = m_jsonData[ns_ps_get_slicing_config_rsp::key_ipc_version].asInt();

        switch(m_ipcVersion){
        case VALID:
            fillURSPRuleParam(m_slicingConfig);
            m_slicingConfig.sliceInfoNum = m_jsonData[ns_ps_get_slicing_config_rsp::key_slice_info_list].size();
            // Max value of sliceInfoNum is 20.
            if(m_slicingConfig.sliceInfoNum > 20){
                m_slicingConfig.sliceInfoNum = 20;
            }
            m_slicingConfig.sliceInfo = getSliceInfoParam(m_slicingConfig.sliceInfoNum, m_jsonData[ns_ps_get_slicing_config_rsp::key_slice_info_list]);
            break;
        case INVALID:
            RilLogE("IPC version is invalid");
            break;
        }
    }

    return;
}

void ProtocolPsSlicingConfigjson::fillURSPRuleParam(DataV2_0::RIL_SlicingConfig &out) {

    RilLogV("fillURSPRuleParam");

    int OS_id_Length = 0;
    int OS_App_ID_Length = 0;

    DataV2_0::RIL_UrspRule urspRule = {};
    RIL_RouteSelectionDescriptor routeSelectionDescriptor;

    out.urspNum = m_jsonData[ns_ps_get_slicing_config_rsp::key_ursp_rule_list].size();
    // Max value of urspNum is 50.
    if(out.urspNum > 50){
        out.urspNum = 50;
    }

    for (int i = 0; i < out.urspNum; i++) {
        DataV2_0::RIL_TrafficDescriptor trafficDescriptor = {};
        urspRule.routeSelectionDescriptor.clear();

        Json::Value jsonUrspRule = m_jsonData[ns_ps_get_slicing_config_rsp::key_ursp_rule_list][i];

        /*
         * precedence
         */
        urspRule.precedence = jsonUrspRule[ns_ps_ursp_rule_info::key_precedence].asInt();

        /*
         * trafficDescriptor
         */
        Json::Value jsonTrafficDescriptor = jsonUrspRule[ns_ps_ursp_rule_info::key_traffic_descriptor];

        //DNN
        trafficDescriptor.dnn.clear();
        string dnn = jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_dnn].asString();
        trafficDescriptor.dnn[0].append(dnn, 0, MAX_DNN_LENGTH - 1);

        //OS_ID
        OS_id_Length = jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_os_id].size();
        // Max value of OS_id_Length is 16.
        if(OS_id_Length > 16){
            OS_id_Length = 16;
        }
        //OS_APPID
        OS_App_ID_Length = jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_os_app_id].size();
        // Max value of OS_App_ID_Length is 255.
        if(OS_App_ID_Length > 255){
            OS_App_ID_Length = 255;
        }

        if (OS_id_Length == 0 && OS_App_ID_Length == 0) {
            trafficDescriptor.osAppIds.clear();
        } else {
            trafficDescriptor.osAppIds.resize(1);
        }
        for(int j = 0 ; j < OS_id_Length ; j++) {
            trafficDescriptor.osAppIds[0].value[j] = jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_os_id][j].asInt();
        }

        for(int j = 0 ; j < OS_App_ID_Length ; j++) {
            trafficDescriptor.osAppIds[0].value[OS_id_Length + j] = jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_os_app_id][j].asInt();
        }


        urspRule.trafficDescriptors.push_back(trafficDescriptor);

        /*
         * RouteSelectionDescriptor
         */
        urspRule.RSDNum = jsonUrspRule[ns_ps_ursp_rule_info::key_route_selection_descriptor_list].size();
        // Max value of RSDNum is 10.
        if(urspRule.RSDNum > 10){
            urspRule.RSDNum = 10;
        }

        for (int k = 0; k < urspRule.RSDNum; k++) {
            memset(&routeSelectionDescriptor,0,sizeof(RIL_RouteSelectionDescriptor));

            Json::Value jsonRSD = jsonUrspRule[ns_ps_ursp_rule_info::key_route_selection_descriptor_list][k];

            //precedence, sessionType, sscMode
            routeSelectionDescriptor.precedence = jsonRSD[ns_ps_route_selection_descriptor_info::key_precedence].asInt();
            routeSelectionDescriptor.sessionType.value = (PdpProtocolType)jsonRSD[ns_ps_route_selection_descriptor_info::key_session_type].asInt();
            routeSelectionDescriptor.sscMode.value = (SscMode)jsonRSD[ns_ps_route_selection_descriptor_info::key_ssc_mode].asInt();

            // Slice Info
            routeSelectionDescriptor.sliceNum = jsonRSD[ns_ps_route_selection_descriptor_info::key_slice_info_list].size();
            // Max value of sliceNum is 10.
            if(routeSelectionDescriptor.sliceNum > 5){
                routeSelectionDescriptor.sliceNum = 5;
            }

            routeSelectionDescriptor.sliceInfo = getSliceInfoParam(routeSelectionDescriptor.sliceNum, jsonRSD[ns_ps_route_selection_descriptor_info::key_slice_info_list]);

            // DNN
            routeSelectionDescriptor.dnnNum = jsonRSD[ns_ps_route_selection_descriptor_info::key_dnn_list].size();
            // Max value of dnnNum is 10.
            if(routeSelectionDescriptor.dnnNum > 5){
                routeSelectionDescriptor.dnnNum = 5;
            }

            routeSelectionDescriptor.dnn = getDnnInfoParam(routeSelectionDescriptor.dnnNum, jsonRSD[ns_ps_route_selection_descriptor_info::key_dnn_list]);

            urspRule.routeSelectionDescriptor.push_back(routeSelectionDescriptor);
        }
        out.urspRules.push_back(urspRule);
    }
}

std::vector<RIL_OptionalDNN> ProtocolPsSlicingConfigjson::getDnnInfoParam(int dnnNum, Json::Value &jsonData) {

    RIL_OptionalDNN dnn;
    std::vector<RIL_OptionalDNN> out;
    out.clear();

    for(int i = 0 ; i < dnnNum ; i++){
        memset(&dnn, 0, sizeof(RIL_OptionalDNN));

        Json::Value jsonDnn = jsonData[i];
        string tempDnn = jsonDnn[ns_ps_dnn::key_dnn].asString();
        strncpy(dnn.value, tempDnn.c_str(), MIN(tempDnn.length(), MAX_DNN_LENGTH - 1));

        out.push_back(dnn);
    }

    return out;
}

std::vector<RIL_SliceInfo> ProtocolPsSlicingConfigjson::getSliceInfoParam(int sliceNum, Json::Value &jsonData) {
    RIL_SliceInfo SliceInfo;
    std::vector<RIL_SliceInfo> out;
    out.clear();

    for (int i = 0; i < sliceNum; i++) {
        memset(&SliceInfo, 0, sizeof(RIL_SliceInfo));

        Json::Value jsonsliceInfo = jsonData[i];

        //NSSAI_TYPE
        SliceInfo.sst = (RIL_SliceServiceType)jsonsliceInfo[ns_ps_slice_info::key_slice_service_type].asInt();
        SliceInfo.sliceDifferentiator = jsonsliceInfo[ns_ps_slice_info::key_slice_differentiator].asInt();
        SliceInfo.mappedHplmnSst = (RIL_SliceServiceType)jsonsliceInfo[ns_ps_slice_info::key_mapped_hplmn_sst].asInt();
        SliceInfo.mappedHplmnSD = jsonsliceInfo[ns_ps_slice_info::key_mapped_hplmn_sd].asInt();
        SliceInfo.status = (RIL_SliceStatus)jsonsliceInfo[ns_ps_slice_info::key_status].asInt();

        out.push_back(SliceInfo);
    }

    return out;
}

int ProtocolPsSlicingConfigjson::GetPayloadSize() const
{
    return sizeof(m_slicingConfig);
}

DataV2_0::RIL_SlicingConfig ProtocolPsSlicingConfigjson::GetSlicingConfig() const
{
    return m_slicingConfig;
}

UINT ProtocolPsSlicingConfigjson::GetErrorCode() const
{
    return ProtocolJsonRespAdapter::GetErrorCode();
}
