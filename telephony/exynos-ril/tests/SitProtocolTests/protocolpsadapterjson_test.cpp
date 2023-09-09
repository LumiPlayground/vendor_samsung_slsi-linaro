#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "json/ps/protocolpsadapterjson.h"
#include "psjsondef.h"
#include "pdpcontext.h"
#include "constdef.h"

//For testing SETUP_DATA_CATT, GET_CALL_LIST, DATA_CALL_LIST_CHANGED
const sit_pdp_data_call_item dataCallInfo[] = {
    {0, 9, SIT_PDP_ACTIVE_DORMANT,
     PDP_IPV4V6, {11, 12, 1, 4, 9, 9, 9, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
     PDP_IPV4V6, {1, 13, 11, 34, 29, 49, 19, 13, 12, 11, 10, 19, 28, 27, 16, 15, 14, 23, 12, 21},
                  {11, 3, 1, 14, 19, 19, 29, 23, 22, 11, 10, 19, 18, 27, 16, 15, 14, 23, 12, 11},
     SIT_PDP_PDP_TYPE_IPV4IPV6, { 0xac, 0x1c, 0x66, 0x96, 0xac, 0x1c, 0x66, 0x96, 0xac, 0x1c, 0x66, 0x96,
                  0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                  0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                  0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16},
     100, 11},

    {0, 9, SIT_PDP_ACTIVE_DORMANT,
     PDP_IPV4V6, {11, 12, 1, 14, 9, 9, 9, 13, 12, 11, 10, 9, 18, 7, 6, 5, 4, 3, 2, 1},
     PDP_IPV4V6, {1, 13, 11, 34, 29, 49, 19, 13, 12, 11, 10, 1, 28, 27, 16, 15, 14, 23, 12, 21},
                  {11, 3, 1, 14, 19, 19, 29, 23, 22, 1, 10, 19, 18, 27, 16, 15, 14, 23, 12, 11},
     SIT_PDP_PDP_TYPE_IPV4IPV6, { 0xac, 0x1c, 0x66, 0x96, 0xac, 0x1c, 0x66, 0x96, 0xac, 0x1c, 0x66, 0x96,
                  0x21, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x81, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                  0x31, 0x2, 0x3, 0x4, 0x15, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                  0x41, 0x2, 0x23, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16},
     100, 11},

};

int ipv6_mtu = 200;
int pudSessionId = 102;
int qosRatType = EPS_QOS;

sit_pdp_EpsQos epQos[] = {
    {1, {2, 3}, {4, 5}},
    {11, {12, 23}, {14, 15}},
};

sit_pdp_NrQos nrQos[] = {
    {6, {7, 8}, {9, 10}, 11, 12},
    {1, {2, 3}, {4, 50}, 11, 22},
};

sit_pdp_data_call_item_pcscf_ext pcscfExt[] = {
    {DC_SLICE_TDINFO, 0,
        {1, 2, 3, 4,
         5, 6, 7, 8,
         9, 10, 11, 12,
         13, 14, 15, 16,
         17, 18, 19, 20},
        { 0x1, 0x2, 0x3, 0x4, 0x5, 0x16, 0x7, 0x8, 0x9, 0x10, 0x51, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x4, 0x5, 0x26, 0x7, 0x8, 0x9, 0x10, 0x41, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x4, 0x5, 0x36, 0x7, 0x8, 0x9, 0x10, 0x31, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x4, 0x5, 0x46, 0x7, 0x8, 0x9, 0x10, 0x21, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x4, 0x5, 0x56, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16},
    },

    {DC_SLICE_TDINFO, 0,
        {1, 12, 3, 4,
         5, 6, 17, 8,
         9, 12, 11, 12,
         23, 14, 15, 16,
         17, 18, 29, 20},
        { 0x1, 0x2, 0x3, 0x14, 0x5, 0x16, 0x27, 0x8, 0x9, 0x10, 0x51, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x24, 0x5, 0x26, 0x37, 0x8, 0x9, 0x10, 0x41, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x34, 0x5, 0x36, 0x47, 0x8, 0x9, 0x10, 0x31, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x44, 0x5, 0x46, 0x57, 0x8, 0x9, 0x10, 0x21, 0x12, 0x13, 0x14, 0x15, 0x16,
          0x1, 0x2, 0x3, 0x54, 0x5, 0x56, 0x67, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16},
    },
};

sit_pdp_data_call_item_ext_lp_retrytime_1_6 retryTime[] = {
    {0 ,0},
    {0 ,0},
};

BYTE ambr[8] = {0x04, 0xfe, 0xfe, 0x9e, 0x6c, 0, 0, };

sit_pdp_optional_slice_info_param sliceInfoParam[] = {
    {8, 1, 3, 4, 5},
    {2, 3, 4, 5, 6},
};

int tdLen = 2;

int tdBitMask = 15;
const char * testString[3] ={"testString_1", "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111",""};
int testOSId[MAX_OS_ID_LENGTH] = {9, 1, 2, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int testOsAppId[MAX_OS_APP_ID_LENGTH] = {1,2,3,4,5,6,7,9,8,10,0};

static void FillArrayData(Json::Value &jsonAddr, const BYTE * addr, int len) {
    for(int i = 0; i < len; i++){
        jsonAddr.append(addr[i]);
    }
}

TEST(ProtocolPsAdapterJson, ProtocolPsSetupDataCallAdapterjson) {
    unsigned int sitId = SIT_SETUP_DATA_CALL;

    Json::Value jsonData;
    jsonData[ns_ps_setup_data_call_rsp::key_data_call_status] = dataCallInfo[0].status;
    jsonData[ns_ps_setup_data_call_rsp::key_cid] = dataCallInfo[0].cid;
    jsonData[ns_ps_setup_data_call_rsp::key_pdp_status] = dataCallInfo[0].active;
    jsonData[ns_ps_setup_data_call_rsp::key_data_call_ipc_version] = pcscfExt[0].IPC_version;
    jsonData[ns_ps_setup_data_call_rsp::key_retry_timer] = retryTime[0].suggestedRetryTime;

    Json::Value jsonCallInfo;
    jsonCallInfo[ns_ps_call_info::key_pdp_type] = dataCallInfo[0].pdp_type;
    jsonCallInfo[ns_ps_call_info::key_dns_type] = dataCallInfo[0].dns_type;
    jsonCallInfo[ns_ps_call_info::key_pcscf_type] = dataCallInfo[0].pcscf_type;

    FillArrayData(jsonCallInfo[ns_ps_call_info::key_ipv4_address], dataCallInfo[0].address, MAX_IPV4_ADDR_LEN);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], dataCallInfo[0].primary_dns, MAX_IPV4_ADDR_LEN);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], dataCallInfo[0].secondary_dns, MAX_IPV4_ADDR_LEN);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], dataCallInfo[0].pcscf, MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);

    FillArrayData(jsonCallInfo[ns_ps_call_info::key_ipv6_address], (dataCallInfo[0].address + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], (dataCallInfo[0].primary_dns + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], (dataCallInfo[0].secondary_dns + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], (dataCallInfo[0].pcscf + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM), MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);

    jsonCallInfo[ns_ps_call_info::key_ipv4_mtu] = dataCallInfo[0].ipv4_mtu_size;
    jsonCallInfo[ns_ps_call_info::key_pco_value] = dataCallInfo[0].pco;
    jsonCallInfo[ns_ps_call_info::key_reserved] = 0;
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4_ext], pcscfExt[0].pcscf_ipv4_ext, MAX_IPV4_ADDR_LEN * 5);
    FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6_ext], pcscfExt[0].pcscf_ipv6_ext, MAX_IPV6_ADDR_LEN * 5);

    FillArrayData(jsonCallInfo[ns_ps_call_info::key_apn_ambr], ambr, 7);

    jsonCallInfo[ns_ps_call_info::key_ipv6_mtu] = ipv6_mtu;
    jsonCallInfo[ns_ps_call_info::key_pdu_session_id] = pudSessionId;
    jsonCallInfo[ns_ps_call_info::key_qos_rat_type] = qosRatType;

    Json::Value jsonQosType;
    if(qosRatType == EPS_QOS){
        jsonQosType[ns_ps_lte_qos_information::key_qci] = epQos[0].qci;
        jsonQosType[ns_ps_lte_qos_information::key_dl_guaranteed_bit_rate] = epQos[0].downlink.guaranteedBitrateKbps;
        jsonQosType[ns_ps_lte_qos_information::key_dl_max_bit_rate] = epQos[0].downlink.maxBitrateKbps;
        jsonQosType[ns_ps_lte_qos_information::key_ul_guaranteed_bit_rate] = epQos[0].uplink.guaranteedBitrateKbps;
        jsonQosType[ns_ps_lte_qos_information::key_ul_max_bit_rate] = epQos[0].uplink.maxBitrateKbps;
    }
    else if(qosRatType == NR_QOS){
        jsonQosType[ns_ps_nr_qos_information::key_five_qi] = nrQos[0].fiveQi;
        jsonQosType[ns_ps_nr_qos_information::key_dl_guaranteed_bit_rate] = nrQos[0].downlink.guaranteedBitrateKbps;
        jsonQosType[ns_ps_nr_qos_information::key_dl_max_bit_rate] = nrQos[0].downlink.maxBitrateKbps;
        jsonQosType[ns_ps_nr_qos_information::key_ul_guaranteed_bit_rate] = nrQos[0].uplink.guaranteedBitrateKbps;
        jsonQosType[ns_ps_nr_qos_information::key_ul_max_bit_rate] = nrQos[0].uplink.maxBitrateKbps;
        jsonQosType[ns_ps_nr_qos_information::key_qfi] = nrQos[0].qfi;
        jsonQosType[ns_ps_nr_qos_information::key_average_window] = nrQos[0].averagingWindowMs;
    }

    jsonCallInfo[ns_ps_call_info::key_qos] = jsonQosType;

    jsonCallInfo[ns_ps_call_info::key_snssai_type] = sliceInfoParam[0].optionalSliceInfoType;
    jsonCallInfo[ns_ps_call_info::key_slice_service_type] = sliceInfoParam[0].optionalSliceInfoSst;
    jsonCallInfo[ns_ps_call_info::key_slice_differentiator] = sliceInfoParam[0].optionalSliceInfoSd;
    jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sst] = sliceInfoParam[0].optionalSliceMappedHplmnSst;
    jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sd] = sliceInfoParam[0].optionalSliceMappedHplmnSd;

    for(int k = 0 ; k < tdLen; k++) {
        Json::Value jsonTD;

        jsonTD[ns_ps_data_call_traffic_descriptor::key_td_bit_mask] = tdBitMask;
        jsonTD[ns_ps_data_call_traffic_descriptor::key_dnn] = string(testString[k]);

        for(int i = 0; i < MAX_OS_ID_LENGTH; i++){
            jsonTD[ns_ps_data_call_traffic_descriptor::key_os_id].append(testOSId[i]);
        }

        for(int i = 0; i < MAX_OS_APP_ID_LENGTH; i++){
            jsonTD[ns_ps_data_call_traffic_descriptor::key_os_app_id].append(testOsAppId[i]);
        }

        jsonCallInfo[ns_ps_call_info::key_traffic_descriptor_list].append(jsonTD);
    }
    jsonData[ns_ps_setup_data_call_rsp::key_call_info] = jsonCallInfo;

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsSetupDataCallAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    EXPECT_EQ(testAdapter.GetStatus(), dataCallInfo[0].status);
    EXPECT_EQ(testAdapter.GetCid(), dataCallInfo[0].cid);

    // Remap SIT_PDP_ACTIVE_* STATE to RIL DATA_CONNECTION_ACTIVE STATE
    if(pcscfExt[0].IPC_version == DC_V10 || dataCallInfo[0].active == SIT_PDP_ACTIVE_INACTIVE || dataCallInfo[0].active == SIT_PDP_ACTIVE_MAX){
        EXPECT_EQ(testAdapter.GetActiveStatus(), dataCallInfo[0].active);
    } else if(dataCallInfo[0].active == SIT_PDP_ACTIVE_DORMANT) {
        EXPECT_EQ(testAdapter.GetActiveStatus(), ACTIVE_AND_LINKDOWN);
    } else if(dataCallInfo[0].active == SIT_PDP_ACTIVE_ACTIVE) {
        EXPECT_EQ(testAdapter.GetActiveStatus(), ACTIVE_AND_LINKUP);
    }

    EXPECT_EQ(testAdapter.GetPdpType(), dataCallInfo[0].pdp_type);

    const DataCall *pDc = testAdapter.GetDataCall();

    if (testAdapter.GetPdpType() == SIT_PDP_PDP_TYPE_IPV4
        || testAdapter.GetPdpType() == SIT_PDP_PDP_TYPE_IPV4IPV6
        || testAdapter.GetPdpType() == SIT_PDP_PDP_TYPE_PPP) {
        for(int i = 0 ; i < MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM ; i++){
            if(i < MAX_IPV4_ADDR_LEN && pDc->ipv4.valid){
                EXPECT_EQ(pDc->ipv4.addr[i], dataCallInfo[0].address[i]);
            }
            if(i < MAX_IPV4_ADDR_LEN && ((pcscfExt[0].IPC_version == DC_V10 && pDc->ipv4.valid)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].dns_type == SIT_PDP_PDP_TYPE_IPV4)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].dns_type == SIT_PDP_PDP_TYPE_IPV4IPV6))){
                EXPECT_EQ(pDc->ipv4.dns1[i], dataCallInfo[0].primary_dns[i]);
                EXPECT_EQ(pDc->ipv4.dns2[i], dataCallInfo[0].secondary_dns[i]);
            }

            if((pcscfExt[0].IPC_version == DC_V10 && pDc->ipv4.valid)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].pcscf_type == SIT_PDP_PDP_TYPE_IPV4)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)){
                EXPECT_EQ(pDc->ipv4.pcscf[i], dataCallInfo[0].pcscf[i]);
            }
        }
    }
    if (testAdapter.GetPdpType() == SIT_PDP_PDP_TYPE_IPV6
        || testAdapter.GetPdpType() == SIT_PDP_PDP_TYPE_IPV4IPV6) {
        for(int i = 0 ; i < MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM ; i++){
            if(i < MAX_IPV6_ADDR_LEN && pDc->ipv6.valid){
                EXPECT_EQ(pDc->ipv6.addr[i], dataCallInfo[0].address[i + MAX_IPV4_ADDR_LEN]);
            }
            if(i < MAX_IPV6_ADDR_LEN && ((pcscfExt[0].IPC_version == DC_V10 && pDc->ipv6.valid)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6))){
                EXPECT_EQ(pDc->ipv6.dns1[i], dataCallInfo[0].primary_dns[i + MAX_IPV4_ADDR_LEN]);
                EXPECT_EQ(pDc->ipv6.dns2[i], dataCallInfo[0].secondary_dns[i + MAX_IPV4_ADDR_LEN]);
            }
            if((pcscfExt[0].IPC_version == DC_V10 && pDc->ipv6.valid)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
                || (pDc->IPC_version != DC_V10 && dataCallInfo[0].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)){
                EXPECT_EQ(pDc->ipv6.pcscf[i], dataCallInfo[0].pcscf[i + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM]);
            }
        }
    }

    if( (dataCallInfo[0].pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV4)
    {
        for(int i = 0; i < MAX_IPV4_ADDR_LEN * 5 ; i++){
            EXPECT_EQ(pDc->ipv4.pcscf[i + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM], pcscfExt[0].pcscf_ipv4_ext[i]);
        }
    }
    if( (dataCallInfo[0].pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV6)
    {
        for(int i = 0; i < MAX_IPV6_ADDR_LEN * 5 ; i++){
            EXPECT_EQ(pDc->ipv6.pcscf[i + MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM], pcscfExt[0].pcscf_ipv6_ext[i]);
        }
    }

    if(pcscfExt[0].IPC_version > DC_V10){
        EXPECT_EQ(pDc->IPC_version, pcscfExt[0].IPC_version);
        EXPECT_EQ(testAdapter.GetMTUv4(), dataCallInfo[0].ipv4_mtu_size);
        EXPECT_EQ(testAdapter.GetPCO(), dataCallInfo[0].pco);
    } else if(pcscfExt[0].IPC_version == DC_V10){
        EXPECT_EQ(pDc->IPC_version, 1);
    }

    if(pcscfExt[0].IPC_version > DC_V11){
        EXPECT_EQ(pDc->suggestedRetryTime, retryTime[0].suggestedRetryTime);
    }


    if(pcscfExt[0].IPC_version > DC_RETRYTIME){
        EXPECT_EQ(pDc->ambr.octet2, ambr[0]);
        EXPECT_EQ(pDc->ambr.octet3, ambr[1]);
        EXPECT_EQ(pDc->ambr.octet4, ambr[2]);
        EXPECT_EQ(pDc->ambr.octet5, ambr[3]);
        EXPECT_EQ(pDc->ambr.octet6, ambr[4]);
        EXPECT_EQ(pDc->ambr.octet7, ambr[5]);
        EXPECT_EQ(pDc->ambr.octet8, ambr[6]);
    }

    if(pcscfExt[0].IPC_version > DC_AMBR){
        EXPECT_EQ(pDc->ipv6_mtu_size, ipv6_mtu);
        EXPECT_EQ(pDc->pdu_session_id, pudSessionId);
        EXPECT_EQ(pDc->defaultQosType, qosRatType);

        if(pDc->defaultQosType == EPS_QOS){
            EXPECT_EQ(pDc->defaultEpsQos.qci, epQos[0].qci);
            EXPECT_EQ(pDc->defaultEpsQos.downlink.guaranteedBitrateKbps, epQos[0].downlink.guaranteedBitrateKbps);
            EXPECT_EQ(pDc->defaultEpsQos.downlink.maxBitrateKbps, epQos[0].downlink.maxBitrateKbps);
            EXPECT_EQ(pDc->defaultEpsQos.uplink.guaranteedBitrateKbps, epQos[0].uplink.guaranteedBitrateKbps);
            EXPECT_EQ(pDc->defaultEpsQos.uplink.maxBitrateKbps, epQos[0].uplink.maxBitrateKbps);
        }
        else if(pDc->defaultQosType == NR_QOS){
            EXPECT_EQ(pDc->defaultNrQos.fiveQi, nrQos[0].fiveQi);
            EXPECT_EQ(pDc->defaultNrQos.downlink.guaranteedBitrateKbps, nrQos[0].downlink.guaranteedBitrateKbps);
            EXPECT_EQ(pDc->defaultNrQos.downlink.maxBitrateKbps, nrQos[0].downlink.maxBitrateKbps);
            EXPECT_EQ(pDc->defaultNrQos.uplink.guaranteedBitrateKbps, nrQos[0].uplink.guaranteedBitrateKbps);
            EXPECT_EQ(pDc->defaultNrQos.uplink.maxBitrateKbps, nrQos[0].uplink.maxBitrateKbps);
            EXPECT_EQ(pDc->defaultNrQos.qfi, nrQos[0].qfi);
            EXPECT_EQ(pDc->defaultNrQos.averagingWindowMs, nrQos[0].averagingWindowMs);
        }
    }

    if(pcscfExt[0].IPC_version > DC_QOS){
        switch(sliceInfoParam[0].optionalSliceInfoType) {
          case OPT_SST: // 1
              EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
              EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[0].optionalSliceInfoSst);
              EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
          break;
          case OPT_SST_MAPSST: // 2
              EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
              EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[0].optionalSliceInfoSst);
              EXPECT_EQ(pDc->sliceInfo.mappedHplmnSst, sliceInfoParam[0].optionalSliceMappedHplmnSst);
              EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
          break;
          case OPT_SST_SD:
              EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
              EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[0].optionalSliceInfoSst);
              EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[0].optionalSliceInfoSd);
              EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
          break;
          case OPT_SST_SD_MAPSST:
              EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
              EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[0].optionalSliceInfoSst);
              EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[0].optionalSliceInfoSd);
              EXPECT_EQ(pDc->sliceInfo.mappedHplmnSD, sliceInfoParam[0].optionalSliceMappedHplmnSd);
              EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
          break;
          case OPT_SST_SD_MAPSST_MAPSD:
              EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
              EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[0].optionalSliceInfoSst);
              EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[0].optionalSliceInfoSd);
              EXPECT_EQ(pDc->sliceInfo.mappedHplmnSst, sliceInfoParam[0].optionalSliceMappedHplmnSst);
              EXPECT_EQ(pDc->sliceInfo.mappedHplmnSD, sliceInfoParam[0].optionalSliceMappedHplmnSd);
              EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
          break;
          default:
              EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_INVALID);
          break;
        }

#if 0
        for(int k = 0 ; k < tdLen ; k++){
            EXPECT_EQ(pDc->tdLength, 2);

            if (!pDc->tdInfo[k].dnn.empty()) {
                if(k == 0){
                    EXPECT_STREQ(pDc->tdInfo[k].dnn[0].value, testString[0]);
                }else{
                    string tempString = string(100, '1');
                    EXPECT_STREQ(pDc->tdInfo[k].dnn[0].value, tempString.c_str());
                }
            }

            if(pDc->tdInfo[k].osAppIdFlag == RIL_OPT_INFO_VALID){
                for(int i = 0; i < MAX_OS_ID_LENGTH; i++){
                    EXPECT_EQ(pDc->tdInfo[k].osAppId[0].value[i], testOSId[i]);
                }

                EXPECT_EQ(pDc->tdInfo[k].osAppId[-].value[MAX_OS_ID_LENGTH], 10);

                for(int i = 0 ; i < MAX_OS_APP_ID_LENGTH; i++){
                    EXPECT_EQ(pDc->tdInfo[k].osAppId[0].value[MAX_OS_ID_LENGTH + 1 + i], testOsAppId[i]);
                }
            }
        }
#endif
    }
}

TEST(ProtocolPsAdapterJson, ProtocolPsDataCallListAdapterjson) {
    unsigned int DataCallNum = 2;

    unsigned int sitId = SIT_GET_DATA_CALL_LIST;

    Json::Value jsonDataCallList;

    for(unsigned int t = 0; t < DataCallNum ; t++){

        Json::Value jsonData;
        jsonData[ns_ps_setup_data_call_rsp::key_data_call_status] = dataCallInfo[t].status;
        jsonData[ns_ps_setup_data_call_rsp::key_cid] = dataCallInfo[t].cid;
        jsonData[ns_ps_setup_data_call_rsp::key_pdp_status] = dataCallInfo[t].active;
        jsonData[ns_ps_setup_data_call_rsp::key_data_call_ipc_version] = pcscfExt[t].IPC_version;
        jsonData[ns_ps_setup_data_call_rsp::key_retry_timer] = retryTime[t].suggestedRetryTime;

        Json::Value jsonCallInfo;
        jsonCallInfo[ns_ps_call_info::key_pdp_type] = dataCallInfo[t].pdp_type;
        jsonCallInfo[ns_ps_call_info::key_dns_type] = dataCallInfo[t].dns_type;
        jsonCallInfo[ns_ps_call_info::key_pcscf_type] = dataCallInfo[t].pcscf_type;

        FillArrayData(jsonCallInfo[ns_ps_call_info::key_ipv4_address], dataCallInfo[t].address, MAX_IPV4_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], dataCallInfo[t].primary_dns, MAX_IPV4_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], dataCallInfo[t].secondary_dns, MAX_IPV4_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], dataCallInfo[t].pcscf, MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);

        FillArrayData(jsonCallInfo[ns_ps_call_info::key_ipv6_address], (dataCallInfo[t].address + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], (dataCallInfo[t].primary_dns + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], (dataCallInfo[t].secondary_dns + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], (dataCallInfo[t].pcscf + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM), MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);

        jsonCallInfo[ns_ps_call_info::key_ipv4_mtu] = dataCallInfo[t].ipv4_mtu_size;
        jsonCallInfo[ns_ps_call_info::key_pco_value] = dataCallInfo[t].pco;
        jsonCallInfo[ns_ps_call_info::key_reserved] = 0;
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4_ext], pcscfExt[t].pcscf_ipv4_ext, MAX_IPV4_ADDR_LEN * 5);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6_ext], pcscfExt[t].pcscf_ipv6_ext, MAX_IPV6_ADDR_LEN * 5);

        FillArrayData(jsonCallInfo[ns_ps_call_info::key_apn_ambr], ambr, 7);

        jsonCallInfo[ns_ps_call_info::key_ipv6_mtu] = ipv6_mtu;
        jsonCallInfo[ns_ps_call_info::key_pdu_session_id] = pudSessionId;
        jsonCallInfo[ns_ps_call_info::key_qos_rat_type] = qosRatType;

        Json::Value jsonQosType;
        if(qosRatType == EPS_QOS){
            jsonQosType[ns_ps_lte_qos_information::key_qci] = epQos[t].qci;
            jsonQosType[ns_ps_lte_qos_information::key_dl_guaranteed_bit_rate] = epQos[t].downlink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_lte_qos_information::key_dl_max_bit_rate] = epQos[t].downlink.maxBitrateKbps;
            jsonQosType[ns_ps_lte_qos_information::key_ul_guaranteed_bit_rate] = epQos[t].uplink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_lte_qos_information::key_ul_max_bit_rate] = epQos[t].uplink.maxBitrateKbps;
        }
        else if(qosRatType == NR_QOS){
            jsonQosType[ns_ps_nr_qos_information::key_five_qi] = nrQos[t].fiveQi;
            jsonQosType[ns_ps_nr_qos_information::key_dl_guaranteed_bit_rate] = nrQos[t].downlink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_dl_max_bit_rate] = nrQos[t].downlink.maxBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_ul_guaranteed_bit_rate] = nrQos[t].uplink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_ul_max_bit_rate] = nrQos[t].uplink.maxBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_qfi] = nrQos[t].qfi;
            jsonQosType[ns_ps_nr_qos_information::key_average_window] = nrQos[t].averagingWindowMs;
        }

        jsonCallInfo[ns_ps_call_info::key_qos] = jsonQosType;

        jsonCallInfo[ns_ps_call_info::key_snssai_type] = sliceInfoParam[t].optionalSliceInfoType;
        jsonCallInfo[ns_ps_call_info::key_slice_service_type] = sliceInfoParam[t].optionalSliceInfoSst;
        jsonCallInfo[ns_ps_call_info::key_slice_differentiator] = sliceInfoParam[t].optionalSliceInfoSd;
        jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sst] = sliceInfoParam[t].optionalSliceMappedHplmnSst;
        jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sd] = sliceInfoParam[t].optionalSliceMappedHplmnSd;

        for(int k = 0 ; k < tdLen; k++) {
            Json::Value jsonTD;

            jsonTD[ns_ps_data_call_traffic_descriptor::key_td_bit_mask] = tdBitMask;
            jsonTD[ns_ps_data_call_traffic_descriptor::key_dnn] = string(testString[k]);

            for(int i = 0; i < MAX_OS_ID_LENGTH; i++){
                jsonTD[ns_ps_data_call_traffic_descriptor::key_os_id].append(testOSId[i]);
            }

            for(int i = 0; i < MAX_OS_APP_ID_LENGTH; i++){
                jsonTD[ns_ps_data_call_traffic_descriptor::key_os_app_id].append(testOsAppId[i]);
            }

            jsonCallInfo[ns_ps_call_info::key_traffic_descriptor_list].append(jsonTD);
        }
        jsonData[ns_ps_setup_data_call_rsp::key_call_info] = jsonCallInfo;

        jsonDataCallList[ns_ps_data_call_list_changed_ind::key_data_call_info_list].append(jsonData);
    }

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonDataCallList));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsDataCallListAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    const DataCall * pDataCallList = testAdapter.GetDataCallList();

    EXPECT_EQ(testAdapter.GetDataCallNum(), DataCallNum);

    for(unsigned int t = 0 ; t < testAdapter.GetDataCallNum() ; t++){

        const DataCall *pDc = (pDataCallList + t);

        if (pDc->pdpType == SIT_PDP_PDP_TYPE_IPV4
            || pDc->pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6
            || pDc->pdpType == SIT_PDP_PDP_TYPE_PPP) {
            for(int i = 0 ; i < MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM ; i++){
                if(i < MAX_IPV4_ADDR_LEN && pDc->ipv4.valid){
                    EXPECT_EQ(pDc->ipv4.addr[i], dataCallInfo[t].address[i]);
                }
                if(i < MAX_IPV4_ADDR_LEN && ((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv4.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].dns_type == SIT_PDP_PDP_TYPE_IPV4)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].dns_type == SIT_PDP_PDP_TYPE_IPV4IPV6))){
                    EXPECT_EQ(pDc->ipv4.dns1[i], dataCallInfo[t].primary_dns[i]);
                    EXPECT_EQ(pDc->ipv4.dns2[i], dataCallInfo[t].secondary_dns[i]);
                }

                if((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv4.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)){
                    EXPECT_EQ(pDc->ipv4.pcscf[i], dataCallInfo[t].pcscf[i]);
                }
            }
        }
        if (pDc->pdpType == SIT_PDP_PDP_TYPE_IPV6
            || pDc->pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
            for(int i = 0 ; i < MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM ; i++){
                if(i < MAX_IPV6_ADDR_LEN && pDc->ipv6.valid){
                    EXPECT_EQ(pDc->ipv6.addr[i], dataCallInfo[t].address[i + MAX_IPV4_ADDR_LEN]);
                }
                if(i < MAX_IPV6_ADDR_LEN && ((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv6.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6))){
                    EXPECT_EQ(pDc->ipv6.dns1[i], dataCallInfo[t].primary_dns[i + MAX_IPV4_ADDR_LEN]);
                    EXPECT_EQ(pDc->ipv6.dns2[i], dataCallInfo[t].secondary_dns[i + MAX_IPV4_ADDR_LEN]);
                }
                if((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv6.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)){
                    EXPECT_EQ(pDc->ipv6.pcscf[i], dataCallInfo[t].pcscf[i + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM]);
                }
            }
        }

        if( pcscfExt[t].IPC_version != DC_SLICE_TDINFO && ((dataCallInfo[t].pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV4))
        {
            for(int i = 0; i < MAX_IPV4_ADDR_LEN * 5 ; i++){
                EXPECT_EQ(pDc->ipv4.pcscf[i + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM], pcscfExt[t].pcscf_ipv4_ext[i]);
            }
        }
        if( pcscfExt[t].IPC_version != DC_SLICE_TDINFO && ((dataCallInfo[t].pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV6))
        {
            for(int i = 0; i < MAX_IPV6_ADDR_LEN * 5 ; i++){
                EXPECT_EQ(pDc->ipv6.pcscf[i + MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM], pcscfExt[t].pcscf_ipv6_ext[i]);
            }
        }

        if(pcscfExt[t].IPC_version > DC_V10){
            EXPECT_EQ(pDc->IPC_version, pcscfExt[t].IPC_version);
            EXPECT_EQ(pDc->ipv4_mtu_size, dataCallInfo[t].ipv4_mtu_size);
            EXPECT_EQ(pDc->pco, dataCallInfo[t].pco);
        } else if(pcscfExt[t].IPC_version == DC_V10){
            EXPECT_EQ(pDc->IPC_version, 1);
        }

        if(pcscfExt[t].IPC_version > DC_V11){
            EXPECT_EQ(pDc->suggestedRetryTime, retryTime[t].suggestedRetryTime);
        }


        if(pcscfExt[0].IPC_version > DC_RETRYTIME){
            EXPECT_EQ(pDc->ambr.octet2, ambr[0]);
            EXPECT_EQ(pDc->ambr.octet3, ambr[1]);
            EXPECT_EQ(pDc->ambr.octet4, ambr[2]);
            EXPECT_EQ(pDc->ambr.octet5, ambr[3]);
            EXPECT_EQ(pDc->ambr.octet6, ambr[4]);
            EXPECT_EQ(pDc->ambr.octet7, ambr[5]);
            EXPECT_EQ(pDc->ambr.octet8, ambr[6]);
        }

        if(pcscfExt[t].IPC_version > DC_AMBR){
            EXPECT_EQ(pDc->ipv6_mtu_size, ipv6_mtu);
            EXPECT_EQ(pDc->pdu_session_id, pudSessionId);
            EXPECT_EQ(pDc->defaultQosType, qosRatType);

            if(pDc->defaultQosType == EPS_QOS){
                EXPECT_EQ(pDc->defaultEpsQos.qci, epQos[t].qci);
                EXPECT_EQ(pDc->defaultEpsQos.downlink.guaranteedBitrateKbps, epQos[t].downlink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultEpsQos.downlink.maxBitrateKbps, epQos[t].downlink.maxBitrateKbps);
                EXPECT_EQ(pDc->defaultEpsQos.uplink.guaranteedBitrateKbps, epQos[t].uplink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultEpsQos.uplink.maxBitrateKbps, epQos[t].uplink.maxBitrateKbps);
            }
            else if(pDc->defaultQosType == NR_QOS){
                EXPECT_EQ(pDc->defaultNrQos.fiveQi, nrQos[t].fiveQi);
                EXPECT_EQ(pDc->defaultNrQos.downlink.guaranteedBitrateKbps, nrQos[t].downlink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.downlink.maxBitrateKbps, nrQos[t].downlink.maxBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.uplink.guaranteedBitrateKbps, nrQos[t].uplink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.uplink.maxBitrateKbps, nrQos[t].uplink.maxBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.qfi, nrQos[t].qfi);
                EXPECT_EQ(pDc->defaultNrQos.averagingWindowMs, nrQos[t].averagingWindowMs);
            }
        }

        if(pcscfExt[t].IPC_version > DC_QOS){
            switch(sliceInfoParam[t].optionalSliceInfoType) {
              case OPT_SST: // 1
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_MAPSST: // 2
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSst, sliceInfoParam[t].optionalSliceMappedHplmnSst);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_SD:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[t].optionalSliceInfoSd);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_SD_MAPSST:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[t].optionalSliceInfoSd);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSD, sliceInfoParam[t].optionalSliceMappedHplmnSd);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_SD_MAPSST_MAPSD:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[t].optionalSliceInfoSd);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSst, sliceInfoParam[t].optionalSliceMappedHplmnSst);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSD, sliceInfoParam[t].optionalSliceMappedHplmnSd);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              default:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_INVALID);
              break;
            }

            for(int k = 0 ; k < tdLen ; k++){
                EXPECT_EQ(pDc->tdLength, 2);

                if (!pDc->tdInfo[k].dnn.empty()) {
                    if(k == 0){
                        EXPECT_STREQ(pDc->tdInfo[k].dnn[0].c_str(), testString[k]);
                    }else{
                        string tempString = string(100, '1');
                        EXPECT_STREQ(pDc->tdInfo[k].dnn[0].c_str(), tempString.c_str());
                    }
                }

#if 0
                if(pDc->tdInfo[k].osAppIdFlag == RIL_OPT_INFO_VALID){
                    for(int i = 0; i < MAX_OS_ID_LENGTH; i++){
                        EXPECT_EQ(pDc->tdInfo[k].osAppId[0].value[i], testOSId[i]);
                    }

                    EXPECT_EQ(pDc->tdInfo[k].osAppId[0].value[MAX_OS_ID_LENGTH], 10);

                    for(int i = 0 ; i < MAX_OS_APP_ID_LENGTH; i++){
                        EXPECT_EQ(pDc->tdInfo[k].osAppId[0].value[MAX_OS_ID_LENGTH + 1 + i], testOsAppId[i]);
                    }
                }
#endif
            }
        }

    }
}


TEST(ProtocolPsAdapterJson, ProtocolPsDataCallListChangedAdapterjson) {
    unsigned int DataCallNum = 2;

    unsigned int sitId = SIT_IND_DATA_CALL_LIST_CHANGED;

    Json::Value jsonDataCallList;

    for(unsigned int t = 0; t < DataCallNum ; t++){

        Json::Value jsonData;
        jsonData[ns_ps_setup_data_call_rsp::key_data_call_status] = dataCallInfo[t].status;
        jsonData[ns_ps_setup_data_call_rsp::key_cid] = dataCallInfo[t].cid;
        jsonData[ns_ps_setup_data_call_rsp::key_pdp_status] = dataCallInfo[t].active;
        jsonData[ns_ps_setup_data_call_rsp::key_data_call_ipc_version] = pcscfExt[t].IPC_version;
        jsonData[ns_ps_setup_data_call_rsp::key_retry_timer] = retryTime[t].suggestedRetryTime;

        Json::Value jsonCallInfo;
        jsonCallInfo[ns_ps_call_info::key_pdp_type] = dataCallInfo[t].pdp_type;
        jsonCallInfo[ns_ps_call_info::key_dns_type] = dataCallInfo[t].dns_type;
        jsonCallInfo[ns_ps_call_info::key_pcscf_type] = dataCallInfo[t].pcscf_type;

        FillArrayData(jsonCallInfo[ns_ps_call_info::key_ipv4_address], dataCallInfo[t].address, MAX_IPV4_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv4], dataCallInfo[t].primary_dns, MAX_IPV4_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv4], dataCallInfo[t].secondary_dns, MAX_IPV4_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4], dataCallInfo[t].pcscf, MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);

        FillArrayData(jsonCallInfo[ns_ps_call_info::key_ipv6_address], (dataCallInfo[t].address + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_primary_dns_ipv6], (dataCallInfo[t].primary_dns + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_secondary_dns_ipv6], (dataCallInfo[t].secondary_dns + MAX_IPV4_ADDR_LEN), MAX_IPV6_ADDR_LEN);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6], (dataCallInfo[t].pcscf + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM), MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);

        jsonCallInfo[ns_ps_call_info::key_ipv4_mtu] = dataCallInfo[t].ipv4_mtu_size;
        jsonCallInfo[ns_ps_call_info::key_pco_value] = dataCallInfo[t].pco;
        jsonCallInfo[ns_ps_call_info::key_reserved] = 0;
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv4_ext], pcscfExt[t].pcscf_ipv4_ext, MAX_IPV4_ADDR_LEN * 5);
        FillArrayData(jsonCallInfo[ns_ps_call_info::key_pcscf_ipv6_ext], pcscfExt[t].pcscf_ipv6_ext, MAX_IPV6_ADDR_LEN * 5);

        FillArrayData(jsonCallInfo[ns_ps_call_info::key_apn_ambr], ambr, 7);

        jsonCallInfo[ns_ps_call_info::key_ipv6_mtu] = ipv6_mtu;
        jsonCallInfo[ns_ps_call_info::key_pdu_session_id] = pudSessionId;
        jsonCallInfo[ns_ps_call_info::key_qos_rat_type] = qosRatType;

        Json::Value jsonQosType;
        if(qosRatType == EPS_QOS){
            jsonQosType[ns_ps_lte_qos_information::key_qci] = epQos[t].qci;
            jsonQosType[ns_ps_lte_qos_information::key_dl_guaranteed_bit_rate] = epQos[t].downlink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_lte_qos_information::key_dl_max_bit_rate] = epQos[t].downlink.maxBitrateKbps;
            jsonQosType[ns_ps_lte_qos_information::key_ul_guaranteed_bit_rate] = epQos[t].uplink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_lte_qos_information::key_ul_max_bit_rate] = epQos[t].uplink.maxBitrateKbps;
        }
        else if(qosRatType == NR_QOS){
            jsonQosType[ns_ps_nr_qos_information::key_five_qi] = nrQos[t].fiveQi;
            jsonQosType[ns_ps_nr_qos_information::key_dl_guaranteed_bit_rate] = nrQos[t].downlink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_dl_max_bit_rate] = nrQos[t].downlink.maxBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_ul_guaranteed_bit_rate] = nrQos[t].uplink.guaranteedBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_ul_max_bit_rate] = nrQos[t].uplink.maxBitrateKbps;
            jsonQosType[ns_ps_nr_qos_information::key_qfi] = nrQos[t].qfi;
            jsonQosType[ns_ps_nr_qos_information::key_average_window] = nrQos[t].averagingWindowMs;
        }

        jsonCallInfo[ns_ps_call_info::key_qos] = jsonQosType;

        jsonCallInfo[ns_ps_call_info::key_snssai_type] = sliceInfoParam[t].optionalSliceInfoType;
        jsonCallInfo[ns_ps_call_info::key_slice_service_type] = sliceInfoParam[t].optionalSliceInfoSst;
        jsonCallInfo[ns_ps_call_info::key_slice_differentiator] = sliceInfoParam[t].optionalSliceInfoSd;
        jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sst] = sliceInfoParam[t].optionalSliceMappedHplmnSst;
        jsonCallInfo[ns_ps_call_info::key_mapped_hplmn_sd] = sliceInfoParam[t].optionalSliceMappedHplmnSd;

        for(int k = 0 ; k < tdLen; k++) {
            Json::Value jsonTD;

            jsonTD[ns_ps_data_call_traffic_descriptor::key_td_bit_mask] = tdBitMask;
            jsonTD[ns_ps_data_call_traffic_descriptor::key_dnn] = string(testString[k]);

            for(int i = 0; i < MAX_OS_ID_LENGTH; i++){
                jsonTD[ns_ps_data_call_traffic_descriptor::key_os_id].append(testOSId[i]);
            }

            for(int i = 0; i < MAX_OS_APP_ID_LENGTH; i++){
                jsonTD[ns_ps_data_call_traffic_descriptor::key_os_app_id].append(testOsAppId[i]);
            }

            jsonCallInfo[ns_ps_call_info::key_traffic_descriptor_list].append(jsonTD);
        }
        jsonData[ns_ps_setup_data_call_rsp::key_call_info] = jsonCallInfo;

        jsonDataCallList[ns_ps_data_call_list_changed_ind::key_data_call_info_list].append(jsonData);
    }

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonDataCallList));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsDataCallListChangedAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    const DataCall * pDataCallList = testAdapter.GetDataCallList();

    EXPECT_EQ(testAdapter.GetDataCallNum(), DataCallNum);

    for(unsigned int t = 0 ; t < testAdapter.GetDataCallNum() ; t++){

        const DataCall *pDc = (pDataCallList + t);

        if (pDc->pdpType == SIT_PDP_PDP_TYPE_IPV4
            || pDc->pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6
            || pDc->pdpType == SIT_PDP_PDP_TYPE_PPP) {
            for(int i = 0 ; i < MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM ; i++){
                if(i < MAX_IPV4_ADDR_LEN && pDc->ipv4.valid){
                    EXPECT_EQ(pDc->ipv4.addr[i], dataCallInfo[t].address[i]);
                }
                if(i < MAX_IPV4_ADDR_LEN && ((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv4.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].dns_type == SIT_PDP_PDP_TYPE_IPV4)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].dns_type == SIT_PDP_PDP_TYPE_IPV4IPV6))){
                    EXPECT_EQ(pDc->ipv4.dns1[i], dataCallInfo[t].primary_dns[i]);
                    EXPECT_EQ(pDc->ipv4.dns2[i], dataCallInfo[t].secondary_dns[i]);
                }

                if((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv4.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)){
                    EXPECT_EQ(pDc->ipv4.pcscf[i], dataCallInfo[t].pcscf[i]);
                }
            }
        }
        if (pDc->pdpType == SIT_PDP_PDP_TYPE_IPV6
            || pDc->pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
            for(int i = 0 ; i < MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM ; i++){
                if(i < MAX_IPV6_ADDR_LEN && pDc->ipv6.valid){
                    EXPECT_EQ(pDc->ipv6.addr[i], dataCallInfo[t].address[i + MAX_IPV4_ADDR_LEN]);
                }
                if(i < MAX_IPV6_ADDR_LEN && ((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv6.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6))){
                    EXPECT_EQ(pDc->ipv6.dns1[i], dataCallInfo[t].primary_dns[i + MAX_IPV4_ADDR_LEN]);
                    EXPECT_EQ(pDc->ipv6.dns2[i], dataCallInfo[t].secondary_dns[i + MAX_IPV4_ADDR_LEN]);
                }
                if((pcscfExt[t].IPC_version == DC_V10 && pDc->ipv6.valid)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
                    || (pDc->IPC_version != DC_V10 && dataCallInfo[t].pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)){
                    EXPECT_EQ(pDc->ipv6.pcscf[i], dataCallInfo[t].pcscf[i + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM]);
                }
            }
        }

        if( pcscfExt[t].IPC_version != DC_SLICE_TDINFO && ((dataCallInfo[t].pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV4))
        {
            for(int i = 0; i < MAX_IPV4_ADDR_LEN * 5 ; i++){
                EXPECT_EQ(pDc->ipv4.pcscf[i + MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM], pcscfExt[t].pcscf_ipv4_ext[i]);
            }
        }
        if( pcscfExt[t].IPC_version != DC_SLICE_TDINFO && ((dataCallInfo[t].pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV6))
        {
            for(int i = 0; i < MAX_IPV6_ADDR_LEN * 5 ; i++){
                EXPECT_EQ(pDc->ipv6.pcscf[i + MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM], pcscfExt[t].pcscf_ipv6_ext[i]);
            }
        }

        if(pcscfExt[t].IPC_version > DC_V10){
            EXPECT_EQ(pDc->IPC_version, pcscfExt[t].IPC_version);
            EXPECT_EQ(pDc->ipv4_mtu_size, dataCallInfo[t].ipv4_mtu_size);
            EXPECT_EQ(pDc->pco, dataCallInfo[t].pco);
        } else if(pcscfExt[t].IPC_version == DC_V10){
            EXPECT_EQ(pDc->IPC_version, 1);
        }

        if(pcscfExt[t].IPC_version > DC_V11){
            EXPECT_EQ(pDc->suggestedRetryTime, retryTime[t].suggestedRetryTime);
        }


        if(pcscfExt[0].IPC_version > DC_RETRYTIME){
            EXPECT_EQ(pDc->ambr.octet2, ambr[0]);
            EXPECT_EQ(pDc->ambr.octet3, ambr[1]);
            EXPECT_EQ(pDc->ambr.octet4, ambr[2]);
            EXPECT_EQ(pDc->ambr.octet5, ambr[3]);
            EXPECT_EQ(pDc->ambr.octet6, ambr[4]);
            EXPECT_EQ(pDc->ambr.octet7, ambr[5]);
            EXPECT_EQ(pDc->ambr.octet8, ambr[6]);
        }

        if(pcscfExt[t].IPC_version > DC_AMBR){
            EXPECT_EQ(pDc->ipv6_mtu_size, ipv6_mtu);
            EXPECT_EQ(pDc->pdu_session_id, pudSessionId);
            EXPECT_EQ(pDc->defaultQosType, qosRatType);

            if(pDc->defaultQosType == EPS_QOS){
                EXPECT_EQ(pDc->defaultEpsQos.qci, epQos[t].qci);
                EXPECT_EQ(pDc->defaultEpsQos.downlink.guaranteedBitrateKbps, epQos[t].downlink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultEpsQos.downlink.maxBitrateKbps, epQos[t].downlink.maxBitrateKbps);
                EXPECT_EQ(pDc->defaultEpsQos.uplink.guaranteedBitrateKbps, epQos[t].uplink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultEpsQos.uplink.maxBitrateKbps, epQos[t].uplink.maxBitrateKbps);
            }
            else if(pDc->defaultQosType == NR_QOS){
                EXPECT_EQ(pDc->defaultNrQos.fiveQi, nrQos[t].fiveQi);
                EXPECT_EQ(pDc->defaultNrQos.downlink.guaranteedBitrateKbps, nrQos[t].downlink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.downlink.maxBitrateKbps, nrQos[t].downlink.maxBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.uplink.guaranteedBitrateKbps, nrQos[t].uplink.guaranteedBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.uplink.maxBitrateKbps, nrQos[t].uplink.maxBitrateKbps);
                EXPECT_EQ(pDc->defaultNrQos.qfi, nrQos[t].qfi);
                EXPECT_EQ(pDc->defaultNrQos.averagingWindowMs, nrQos[t].averagingWindowMs);
            }
        }

        if(pcscfExt[t].IPC_version > DC_QOS){
            switch(sliceInfoParam[t].optionalSliceInfoType) {
              case OPT_SST: // 1
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_MAPSST: // 2
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSst, sliceInfoParam[t].optionalSliceMappedHplmnSst);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_SD:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[t].optionalSliceInfoSd);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_SD_MAPSST:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[t].optionalSliceInfoSd);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSD, sliceInfoParam[t].optionalSliceMappedHplmnSd);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              case OPT_SST_SD_MAPSST_MAPSD:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_VALID);
                  EXPECT_EQ(pDc->sliceInfo.sst, sliceInfoParam[t].optionalSliceInfoSst);
                  EXPECT_EQ(pDc->sliceInfo.sliceDifferentiator, sliceInfoParam[t].optionalSliceInfoSd);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSst, sliceInfoParam[t].optionalSliceMappedHplmnSst);
                  EXPECT_EQ(pDc->sliceInfo.mappedHplmnSD, sliceInfoParam[t].optionalSliceMappedHplmnSd);
                  EXPECT_EQ(pDc->sliceInfo.status, ALLOWED);
              break;
              default:
                  EXPECT_EQ(pDc->sliceInfoType, RIL_OPT_INFO_INVALID);
              break;
            }

            for(int k = 0 ; k < tdLen ; k++){
                EXPECT_EQ(pDc->tdLength, 2);

                if (!pDc->tdInfo[k].dnn.empty()) {
                    if(k == 0){
                        EXPECT_STREQ(pDc->tdInfo[k].dnn[0].c_str(), testString[k]);
                    }else{
                        string tempString = string(100, '1');
                        EXPECT_STREQ(pDc->tdInfo[k].dnn[0].c_str(), tempString.c_str());
                    }
                }

                if (!pDc->tdInfo[k].osAppIds.empty()) {
                    for(int i = 0; i < MAX_OS_ID_LENGTH; i++){
                        EXPECT_EQ(pDc->tdInfo[k].osAppIds[0].value[i], testOSId[i]);
                    }

                    EXPECT_EQ(pDc->tdInfo[k].osAppIds[0].value[MAX_OS_ID_LENGTH], 10);

                    for(int i = 0 ; i < MAX_OS_APP_ID_LENGTH; i++){
                        EXPECT_EQ(pDc->tdInfo[k].osAppIds[0].value[MAX_OS_ID_LENGTH + 1 + i], testOsAppId[i]);
                    }
                }
            }
        }
    }
}

TEST(ProtocolPsAdapterJson, ProtocolPsNasTimerStatusAdapterjson) {

    unsigned int sitId = SIT_IND_NAS_TIMER_STATUS;

    const char * testString[3] ={"testString_1", "testString_2", "testString_3"};

    Json::Value jsonIndData;
    jsonIndData[ns_ps_nas_timer_status_ind::key_timer_type] = 0;
    jsonIndData[ns_ps_nas_timer_status_ind::key_timer_status] = 2;
    jsonIndData[ns_ps_nas_timer_status_ind::key_timer_value] = 1;
    jsonIndData[ns_ps_nas_timer_status_ind::key_apn] = string(testString[0]);

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonIndData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsNasTimerStatusAdapterjson testAdapter(modemData.get());

    const SitNasTimerStatus *pNasTimerStatus = testAdapter.GetNasTimerStatus();

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    EXPECT_EQ(0, pNasTimerStatus->type);
    EXPECT_EQ(2, pNasTimerStatus->status);
    EXPECT_EQ(1, pNasTimerStatus->value);
    EXPECT_STREQ(testString[0], pNasTimerStatus->apn);
}

TEST(ProtocolPsAdapterJson, ProtocolPsStartKeepAliveAdapterjson) {
    unsigned int sessionHande = 10U;
    int statusCode = KEEPALIVE_ACTIVE;
    unsigned int sitId = SIT_START_KEEPALIVE;

    Json::Value jsonData;
    jsonData[ns_ps_start_keepalive_rsp::key_session_handle] = sessionHande;
    jsonData[ns_ps_start_keepalive_rsp::key_status_code] = statusCode;

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsStartKeepAliveAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    EXPECT_EQ(testAdapter.getSessionHandle(), sessionHande);
    EXPECT_EQ(testAdapter.getCode(), statusCode);
}

TEST(ProtocolPsAdapterJson, ProtocolPsKeepAliveStatusAdapterjson) {
    unsigned int sessionHande = 10U;
    int statusCode = KEEPALIVE_ACTIVE;
    unsigned int sitId = SIT_IND_KEEPALIVE_STATUS;

    Json::Value jsonData;
    jsonData[ns_ps_keepalive_status_ind::key_session_handle] = sessionHande;
    jsonData[ns_ps_keepalive_status_ind::key_status_code] = statusCode;

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsKeepAliveStatusAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    EXPECT_EQ(testAdapter.getSessionHandle(), sessionHande);
    EXPECT_EQ(testAdapter.getCode(), statusCode);
}

TEST(ProtocolPsAdapterJson, ProtocolPsPcoDataAdapterjson) {
    unsigned int sitId = SIT_IND_PCO_DATA;
    int contents[4] = {1,2,3,4};

    Json::Value jsonData;
    jsonData[ns_ps_pco_data_ind::key_cid] = 1;
    jsonData[ns_ps_pco_data_ind::key_pdp_type] = PDP_TYPE_IPV4V6;

    Json::Value jsonOperator;
    jsonOperator[ns_ps_operator_pco_info::key_pco_id] = 1;
    for(int i = 0; i < 4 ;i++) {
        jsonOperator[ns_ps_operator_pco_info::key_contents].append(contents[i]);
    }
    jsonData[ns_ps_pco_data_ind::key_operator_pco_info_list].append(jsonOperator);

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsPcoDataAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    EXPECT_EQ(testAdapter.GetCid(), 1);
    EXPECT_EQ(testAdapter.GetPdpType(), PDP_TYPE_IPV4V6);
    EXPECT_EQ(testAdapter.GetPcoNum(), 1);

    sit_pdp_pco_data_entry e;
    testAdapter.GetPcoData(e);

    EXPECT_EQ(e.pco_id, 1);
    EXPECT_EQ(e.contents_len, 4);

    for(int i = 0; i < e.contents_len ; i++) {
        EXPECT_EQ(e.contents[i], (char)contents[i]);
    }
}

TEST(ProtocolPsAdapterJson, ProtocolPsAllocatePduSessionIdjson) {

    unsigned int sitId = SIT_ALLOC_PDU_SESSION;

    int sessionId = 987;

    Json::Value jsonData;
    jsonData[ns_ps_alloc_pdu_session_rsp::key_session_id] = sessionId;

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsAllocatePduSessionIdjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    EXPECT_EQ(sessionId, testAdapter.GetSessionId());
}

TEST(ProtocolPsAdapterJson, ProtocolPsDataOffStatusAdapterjson) {

    unsigned int sitId = SIT_IND_PS_DATA_OFF_STATUS;

    int psDataOffAvailableStatus = 0;
    int exemptListForHome = 0b10;
    int exemptListForRoam = 0b1000;

    Json::Value jsonIndData;
    jsonIndData[ns_ps_data_off_status_ind::key_ps_data_off_status] = psDataOffAvailableStatus;
    jsonIndData[ns_ps_data_off_status_ind::key_exempt_home_list] = exemptListForHome;
    jsonIndData[ns_ps_data_off_status_ind::key_exempt_roam_list] = exemptListForRoam;

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonIndData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsDataOffStatusAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    EXPECT_EQ(psDataOffAvailableStatus, testAdapter.GetPsDataOffAvailableStatus());
    EXPECT_EQ(exemptListForHome, testAdapter.GetExemptListForHome());
    EXPECT_EQ(exemptListForRoam, testAdapter.GetExemptListForRoam());
}

TEST(ProtocolPsAdapterJson, ProtocolPsIpAddrStatusAdapterjson) {

    unsigned int sitId = SIT_IND_IP_ADDR_STATUS;

    sit_pdp_ip_addr_status_ind testVector=
    {
        .hdr ={},
        .cid = 0x1,
        .status = 0x2,
        .IPv4Address ={0xA,0xB,0xC,0xD},
        .IPv6Address ={0x9,0x8,0x7,0x6,0x5,0x4,0x3,0x2,0x1,0,},
    };

    Json::Value jsonIndData;
    jsonIndData[ns_ps_ip_addr_status_ind::key_cid] = testVector.cid;
    jsonIndData[ns_ps_ip_addr_status_ind::key_status] = testVector.status;
    FillArrayData(jsonIndData[ns_ps_ip_addr_status_ind::key_ipv4_address], testVector.IPv4Address, MAX_IPV4_ADDR_LEN);
    FillArrayData(jsonIndData[ns_ps_ip_addr_status_ind::key_ipv6_address], testVector.IPv6Address, MAX_IPV6_ADDR_LEN);

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonIndData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsIpAddrStatusAdapterjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    EXPECT_EQ(testVector.cid, testAdapter.GetCid());
    EXPECT_EQ(testVector.status, testAdapter.GetStatus());

    char resultIpv4[MAX_IPV4_ADDR_LEN] = {0, };
    char resultIpv6[MAX_IPV6_ADDR_LEN] = {0, };

    testAdapter.GetAddrInfo(resultIpv4, resultIpv6);

    for(int i = 0; i < MAX_IPV4_ADDR_LEN ; i++){
        EXPECT_EQ(testVector.IPv4Address[i], resultIpv4[i]);
    }

    for(int i = 0; i < MAX_IPV6_ADDR_LEN ; i++){
        EXPECT_EQ(testVector.IPv6Address[i], resultIpv6[i]);
    }
}

TEST(ProtocolPsAdapterJson, ProtocolPsUnthrottleApnjson) {

    unsigned int sitId = SIT_IND_UNTHROTTLE_APN;

    const char * testString[1] ={"testString_1"};

    Json::Value jsonIndData;
    jsonIndData[ns_ps_unthrottle_apn_ind::key_apn] = string(testString[0]);

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonIndData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsUnthrottleApnjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);

    EXPECT_STREQ(testString[0], testAdapter.GetApn());
}

TEST(ProtocolPsAdapterJson, ProtocolPsDedicatedBearInfoAdapterjson) {

    struct {
        int cid;
        int status;
        int qos_session_id;
        int qos_rat_type;
        int field_flag;
        BYTE local_address[MAX_PDP_ADDRESS_LEN];
        int local_address_prefix_ipv4;
        int local_address_prefix_ipv6;
        BYTE remote_address[MAX_PDP_ADDRESS_LEN];
        int remote_address_prefix_ipv4;
        int remote_address_prefix_ipv6;
        int32_t local_port_low;
        int32_t local_port_high;
        int32_t remote_port_low;
        int32_t remote_port_high;
        uint32_t qos_protocol;
        uint8_t type_of_service;
        uint32_t ipv6_flow_label;
        uint32_t ip_sec_spi;
        uint8_t direction;
        int32_t precedence;
        int qci;
        int dl_guaranteed_bit_rate;
        int dl_max_bit_rate;
        int ul_guaranteed_bit_rate;
        int ul_max_bit_rate;
        int five_qi;
        int qfi;
        int average_window;
    } testVector[] = {
        { 1, 1, 1, EPS_QOS, 0x1F,
          /*local_address, prefix*/{0x1,0x2,0x3,0x4,0x5,0x4,0x4,0x3,0x1,0x9,0x6,0x4,0x3,0x4,0x5,0x1,0x5,0x7,0x8,0x8}, 2, 4,
          /*remote_address, prefix*/{0x1,0x1,0x1,0x1,0x1,0x1,0x9,0x1,0x1,0x1,0x1,0x1,0x3,0x1,0x1,0x1,0x1,0x1,0x1,0x1}, 3, 5,
          2, 3, 4, 5, 6, 7, 8, 9, 1, 10, 1, 2, 3, 4, 5, 6, 7, 8},

        { 1, 1, 1, NR_QOS, 0x1F,
          /*local_address, prefix*/{0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x4,0x1,0x1,0x1,0x9,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1}, 2, 1,
          /*remote_address, prefix*/{0x1,0x1,0x1,0x3,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x9,0x1,0x1,0x1,0x1,0x1,0x1}, 9, 2,
          2, 13, 14, 15, 16, 17, 18, 19, 1, 10, 2, 3, 4, 5, 6, 7, 9, 5},
    };

    int size = 2;
    unsigned int sitId = SIT_IND_DEDICATED_BEARER_INFO;

    for (int i = 0; i < size; i++) {
        Json::Value jsonData;
        jsonData[ns_ps_dedicated_bearer_info_ind::key_qos_session_id] = testVector[i].qos_session_id;
        jsonData[ns_ps_dedicated_bearer_info_ind::key_default_cid] = testVector[i].cid;
        jsonData[ns_ps_dedicated_bearer_info_ind::key_status] = testVector[i].status;
        jsonData[ns_ps_dedicated_bearer_info_ind::key_qos_rat_type] = testVector[i].qos_rat_type;

        Json::Value qosType;
        if(testVector[i].qos_rat_type == EPS_QOS){
            qosType[ns_ps_lte_qos_information::key_qci] = testVector[i].qci;
            qosType[ns_ps_lte_qos_information::key_dl_guaranteed_bit_rate] = testVector[i].dl_guaranteed_bit_rate;
            qosType[ns_ps_lte_qos_information::key_dl_max_bit_rate] = testVector[i].dl_max_bit_rate;
            qosType[ns_ps_lte_qos_information::key_ul_guaranteed_bit_rate] = testVector[i].ul_guaranteed_bit_rate;
            qosType[ns_ps_lte_qos_information::key_ul_max_bit_rate] = testVector[i].ul_max_bit_rate;
        }
        else if(testVector[i].qos_rat_type == NR_QOS){
            qosType[ns_ps_nr_qos_information::key_five_qi] = testVector[i].qci;
            qosType[ns_ps_nr_qos_information::key_dl_guaranteed_bit_rate] = testVector[i].dl_guaranteed_bit_rate;
            qosType[ns_ps_nr_qos_information::key_dl_max_bit_rate] = testVector[i].dl_max_bit_rate;
            qosType[ns_ps_nr_qos_information::key_ul_guaranteed_bit_rate] = testVector[i].ul_guaranteed_bit_rate;
            qosType[ns_ps_nr_qos_information::key_ul_max_bit_rate] = testVector[i].ul_max_bit_rate;
            qosType[ns_ps_nr_qos_information::key_qfi] = testVector[i].qfi;
            qosType[ns_ps_nr_qos_information::key_average_window] = testVector[i].average_window;
        }
        jsonData[ns_ps_dedicated_bearer_info_ind::key_qos] = qosType;

        Json::Value jsonPacketFilter;
        jsonPacketFilter[ns_ps_packet_filter_information::key_field_flag] = testVector[i].field_flag;
        for(int j = 0; j < MAX_PDP_ADDRESS_LEN ;j++) {
            jsonPacketFilter[ns_ps_packet_filter_information::key_local_address].append(testVector[i].local_address[j]);
            jsonPacketFilter[ns_ps_packet_filter_information::key_remote_address].append(testVector[i].remote_address[j]);
        }

        jsonPacketFilter[ns_ps_packet_filter_information::key_local_address_prefix_ipv4] = testVector[i].local_address_prefix_ipv4;
        jsonPacketFilter[ns_ps_packet_filter_information::key_local_address_prefix_ipv6] = testVector[i].local_address_prefix_ipv6;
        jsonPacketFilter[ns_ps_packet_filter_information::key_remote_address_prefix_ipv4] = testVector[i].remote_address_prefix_ipv4;
        jsonPacketFilter[ns_ps_packet_filter_information::key_remote_address_prefix_ipv6] = testVector[i].remote_address_prefix_ipv6;

        jsonPacketFilter[ns_ps_packet_filter_information::key_local_port_low] = testVector[i].local_port_low;
        jsonPacketFilter[ns_ps_packet_filter_information::key_local_port_high] = testVector[i].local_port_high;
        jsonPacketFilter[ns_ps_packet_filter_information::key_remote_prot_low] = testVector[i].remote_port_low;
        jsonPacketFilter[ns_ps_packet_filter_information::key_remote_port_high] = testVector[i].remote_port_high;
        jsonPacketFilter[ns_ps_packet_filter_information::key_qos_protocol] = testVector[i].qos_protocol;
        jsonPacketFilter[ns_ps_packet_filter_information::key_type_of_service] = testVector[i].type_of_service;
        jsonPacketFilter[ns_ps_packet_filter_information::key_ipv6_flow_label] = testVector[i].ipv6_flow_label;
        jsonPacketFilter[ns_ps_packet_filter_information::key_ip_sec_spi] = testVector[i].ip_sec_spi;
        jsonPacketFilter[ns_ps_packet_filter_information::key_direction] = testVector[i].direction;
        jsonPacketFilter[ns_ps_packet_filter_information::key_precedence] = testVector[i].precedence;

        jsonData[ns_ps_dedicated_bearer_info_ind::key_packet_filter_list].append(jsonPacketFilter);

        auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonInd(sitId, jsonData));
        ASSERT_NE(modemData, nullptr);

        ProtocolPsDedicatedBearInfoAdapterjson testAdapter(modemData.get());

        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), sitId);

        EXPECT_EQ(testAdapter.GetCid(), testVector[i].cid);
        EXPECT_EQ(testAdapter.GetStatus(), testVector[i].status);

        QosSession_t m_QosSession = testAdapter.GetQosSessionInfo();

        EXPECT_EQ(m_QosSession.qosSessionId, testVector[i].qos_session_id);
        EXPECT_EQ(m_QosSession.defaultQosType, testVector[i].qos_rat_type);

        if(m_QosSession.defaultQosType == EPS_QOS){
            EXPECT_EQ(m_QosSession.defaultEpsQos.qci, testVector[i].qos_session_id);
            EXPECT_EQ(m_QosSession.defaultEpsQos.downlink.guaranteedBitrateKbps, testVector[i].dl_guaranteed_bit_rate);
            EXPECT_EQ(m_QosSession.defaultEpsQos.downlink.maxBitrateKbps, testVector[i].dl_max_bit_rate);
            EXPECT_EQ(m_QosSession.defaultEpsQos.uplink.guaranteedBitrateKbps, testVector[i].ul_guaranteed_bit_rate);
            EXPECT_EQ(m_QosSession.defaultEpsQos.uplink.maxBitrateKbps, testVector[i].ul_max_bit_rate);
        }
        else if(m_QosSession.defaultQosType == NR_QOS){
            EXPECT_EQ(m_QosSession.defaultNrQos.fiveQi, testVector[i].qci);
            EXPECT_EQ(m_QosSession.defaultNrQos.downlink.guaranteedBitrateKbps, testVector[i].dl_guaranteed_bit_rate);
            EXPECT_EQ(m_QosSession.defaultNrQos.downlink.maxBitrateKbps, testVector[i].dl_max_bit_rate);
            EXPECT_EQ(m_QosSession.defaultNrQos.uplink.guaranteedBitrateKbps, testVector[i].ul_guaranteed_bit_rate);
            EXPECT_EQ(m_QosSession.defaultNrQos.uplink.maxBitrateKbps, testVector[i].ul_max_bit_rate);
            EXPECT_EQ(m_QosSession.defaultNrQos.qfi, testVector[i].qfi);
            EXPECT_EQ(m_QosSession.defaultNrQos.averagingWindowMs, testVector[i].average_window);
        }

        for(int k = 0 ; k < m_QosSession.QosFilterCnt ; k++){
            for(int j = 0 ; j < 16 ; j ++){
                EXPECT_EQ(m_QosSession.QosFilters[k].localAddress[0][j], testVector[i].local_address[j]);
                EXPECT_EQ(m_QosSession.QosFilters[k].remoteAddress[0][j], testVector[i].remote_address[j]);
            }

            for(int j = 0 ; j < 4 ; j ++){
                EXPECT_EQ(m_QosSession.QosFilters[k].localAddress[1][j], testVector[i].local_address[16 + j]);
                EXPECT_EQ(m_QosSession.QosFilters[k].remoteAddress[1][j], testVector[i].remote_address[16 + j]);
            }

            EXPECT_EQ(m_QosSession.QosFilters[k].localAddrPrefix_v4, testVector[i].local_address_prefix_ipv4);
            EXPECT_EQ(m_QosSession.QosFilters[k].localAddrPrefix_v6, testVector[i].local_address_prefix_ipv6);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortprefix_4, testVector[i].remote_address_prefix_ipv4);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortprefix_6, testVector[i].remote_address_prefix_ipv6);
            EXPECT_EQ(m_QosSession.QosFilters[k].localPortFlag, MAYBE_PORT_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].localPortStart, testVector[i].local_port_low);
            EXPECT_EQ(m_QosSession.QosFilters[k].localPortEnd, testVector[i].local_port_high);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortFlag, MAYBE_PORT_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortStart, testVector[i].remote_port_low);
            EXPECT_EQ(m_QosSession.QosFilters[k].remotePortEnd, testVector[i].remote_port_high);
            EXPECT_EQ(m_QosSession.QosFilters[k].protocol, testVector[i].qos_protocol);
            EXPECT_EQ(m_QosSession.QosFilters[k].tosFlag, 1);
            EXPECT_EQ(m_QosSession.QosFilters[k].tosValue, testVector[i].type_of_service);
            EXPECT_EQ(m_QosSession.QosFilters[k].flowLabelFlag, IPV6_FLOW_LABEL_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].flowLabelValue, testVector[i].ipv6_flow_label);
            EXPECT_EQ(m_QosSession.QosFilters[k].spiFlag, IP_SEC_SPI_VALID);
            EXPECT_EQ(m_QosSession.QosFilters[k].spiValue, testVector[i].ip_sec_spi);
            EXPECT_EQ(m_QosSession.QosFilters[k].direction, testVector[i].direction - 1);
            EXPECT_EQ(m_QosSession.QosFilters[k].precedence, testVector[i].precedence);
        }
    }
}

#if 0
TEST(ProtocolPsAdapterJson, ProtocolPsSlicingConfigjson) {

    unsigned int sitId = SIT_GET_SLICING_CONFIG;

    struct {
        int ipcVersion;
        int precedence;
        int os_app_id[10];
        int slice_service_type;
        int slice_differentiator;
        int mapped_hplmn_sst;
        int mapped_hplmn_sd;
        int status;
        int sessionType;
        int sscMode;
        const char * testString[2];
    } testVector[] = {
        { 1, 2, {1,2,3,4,5,6,7,8,9,10}, 1,2,3,4,5, 1,2,{"testString_1", "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"}},
    };

    Json::Value jsonData;
    jsonData[ns_ps_get_slicing_config_rsp::key_ipc_version] = testVector[0].ipcVersion;

    Json::Value jsonUrspRule;
    jsonUrspRule[ns_ps_ursp_rule_info::key_precedence] = testVector[0].precedence;

    Json::Value jsonTrafficDescriptor;
    jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_dnn] = string(testVector[0].testString[0]);

    for(int i = 0; i < 5; i++){
        jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_os_app_id].append(testVector[0].os_app_id[i]);
        jsonTrafficDescriptor[ns_ps_traffic_descriptor::key_os_id].append(testVector[0].os_app_id[5 + i]);
    }
    jsonUrspRule[ns_ps_ursp_rule_info::key_traffic_descriptor] = jsonTrafficDescriptor;

    Json::Value RSD;
    RSD[ns_ps_route_selection_descriptor_info::key_precedence] = testVector[0].precedence;
    RSD[ns_ps_route_selection_descriptor_info::key_session_type] = testVector[0].sessionType;
    RSD[ns_ps_route_selection_descriptor_info::key_ssc_mode] = testVector[0].sscMode;

    for(int i = 0; i < 5; i++){
        Json::Value sliceInfo;
        sliceInfo[ns_ps_slice_info::key_slice_service_type] = (testVector[0].slice_service_type + i) % 4;
        sliceInfo[ns_ps_slice_info::key_slice_differentiator] = testVector[0].slice_differentiator + i;
        sliceInfo[ns_ps_slice_info::key_mapped_hplmn_sst] = (testVector[0].mapped_hplmn_sst + i) % 4;
        sliceInfo[ns_ps_slice_info::key_mapped_hplmn_sd] = testVector[0].mapped_hplmn_sd + i;
        sliceInfo[ns_ps_slice_info::key_status] = (testVector[0].status + i) % 6;
        RSD[ns_ps_route_selection_descriptor_info::key_slice_info_list].append(sliceInfo);
    }

    for(int i = 0; i < 2; i++){
        Json::Value dnn;
        dnn[ns_ps_dnn::key_dnn] = string(testVector[0].testString[i]);
        RSD[ns_ps_route_selection_descriptor_info::key_dnn_list].append(dnn);
    }

    jsonUrspRule[ns_ps_ursp_rule_info::key_route_selection_descriptor_list].append(RSD);

    jsonData[ns_ps_get_slicing_config_rsp::key_ursp_rule_list].append(jsonUrspRule);

    for(int i = 3; i < 8; i++){
        Json::Value sliceInfo;
        sliceInfo[ns_ps_slice_info::key_slice_service_type] = (testVector[0].slice_service_type + i) % 4;
        sliceInfo[ns_ps_slice_info::key_slice_differentiator] = testVector[0].slice_differentiator + i;
        sliceInfo[ns_ps_slice_info::key_mapped_hplmn_sst] = (testVector[0].mapped_hplmn_sst + i) % 4;
        sliceInfo[ns_ps_slice_info::key_mapped_hplmn_sd] = testVector[0].mapped_hplmn_sd + i;
        sliceInfo[ns_ps_slice_info::key_status] = (testVector[0].status + i) % 6;
        jsonData[ns_ps_get_slicing_config_rsp::key_slice_info_list].append(sliceInfo);
    }

    auto modemData = std::unique_ptr<ModemData>(ModemDataBuilder::BuildJsonRsp(sitId, 0, RCM_E_SUCCESS, jsonData));
    ASSERT_NE(modemData, nullptr);

    ProtocolPsSlicingConfigjson testAdapter(modemData.get());

    //General checks for resposne
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

    SliceConfig m_slicingConfig = testAdapter.GetSlicingConfig();

    EXPECT_EQ(m_slicingConfig.urspNum, 1);

    EXPECT_EQ(m_slicingConfig.urspRules[0].precedence, testVector[0].precedence);

    EXPECT_STREQ(m_slicingConfig.urspRules[0].trafficDescriptors[0].dnn[0].c_str(), testVector[0].testString[0]);

    for(int i = 0; i < 10; i++){
        EXPECT_EQ(m_slicingConfig.urspRules[0].trafficDescriptors[0].osAppIds[0].value[i], testVector[0].os_app_id[i]);
    }

    EXPECT_EQ(m_slicingConfig.urspRules[0].RSDNum, 1);

    EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].precedence, testVector[0].precedence);
    EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sessionType.value, testVector[0].sessionType);
    EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sscMode.value, testVector[0].sscMode);

    EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sliceNum, 5);
    for(int i = 0 ; i < 5 ; i++){
        EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sliceInfo[i].sst, ((testVector[0].slice_service_type + i) % 4));
        EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sliceInfo[i].sliceDifferentiator, testVector[0].slice_differentiator + i);
        EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sliceInfo[i].mappedHplmnSst, ((testVector[0].mapped_hplmn_sst + i) % 4));
        EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sliceInfo[i].mappedHplmnSD, testVector[0].mapped_hplmn_sd + i);
        EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].sliceInfo[i].status, ((testVector[0].status + i) % 6));
    }

    EXPECT_EQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].dnnNum, 2);

    for(int i = 0 ; i < 2 ; i++){
        if(i == 0){
            EXPECT_STREQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].dnn[i].c_str(), testVector[0].testString[i]);
        }else {
            string tempString = string(100, '1');
            EXPECT_STREQ(m_slicingConfig.urspRules[0].routeSelectionDescriptor[0].dnn[i].c_str(), tempString.c_str());
        }
    }

    EXPECT_EQ(m_slicingConfig.sliceInfoNum, 5);
    for(int i = 3 ; i < 8 ; i++){
        EXPECT_EQ(m_slicingConfig.sliceInfo[i - 3].sst, ((testVector[0].slice_service_type + i) % 4));
        EXPECT_EQ(m_slicingConfig.sliceInfo[i - 3].sliceDifferentiator, testVector[0].slice_differentiator + i);
        EXPECT_EQ(m_slicingConfig.sliceInfo[i - 3].mappedHplmnSst, ((testVector[0].mapped_hplmn_sst + i) % 4));
        EXPECT_EQ(m_slicingConfig.sliceInfo[i - 3].mappedHplmnSD, testVector[0].mapped_hplmn_sd + i);
        EXPECT_EQ(m_slicingConfig.sliceInfo[i - 3].status, ((testVector[0].status + i) % 6));
    }
}

#endif
