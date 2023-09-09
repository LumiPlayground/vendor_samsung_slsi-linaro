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
 * protocolpsadapterlegacy.cpp
 *
 *  Created on: 2014. 6. 27.
 *      Author: sungwoo48.choi
 */

#include "protocolpsadapterlegacy.h"
#include "pdpcontext.h"
#include "rillog.h"
#include "modemdata.h"
#include <arpa/inet.h>
#include <librilutils/systemproperty.h>
#include <base/rilproperty.h>

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

// Map of TYPE to size
/*
static int getSizeOfExtensionPayload(data_call_extension_header_type type)
{
    switch(type)
    {
      case EXT_PCSCF:
          return sizeof(sit_pdp_data_call_item_pcscf_ext);
      case EXT_LP_RETRYTIME:
          return sizeof(sit_pdp_data_call_item_ext_lp_retrytime);
      default:
          break;
    }
    return 0;
}
*/

//when reset the dataCall, qosFilters shold be cleared
// Using Simple IPC Version to fixed size addtional block scheme
static sit_pdp_data_call_item_pcscf_ext *getExtensionPayload(void *dataCallList)
{
    char *start = NULL;
    const size_t legacyHeaderSize = LEN_DC_V11;
    // start from PCSCF extension
    start = (char *)(dataCallList) + legacyHeaderSize;

    int IPC_version = start[0];

    // IPC_version field
    if(IPC_version >= 2) {
        RilLogV("IPC version is %d, pcscf_ext blk start @ %p", IPC_version, start);
        return (sit_pdp_data_call_item_pcscf_ext *)start;
    }
    else if(IPC_version == 1){
        RilLogV("IPC version is 1, return NULL");
        return NULL;
    }
    return NULL;
}

/*
static void *searchNextExtensionPayload(data_call_extension_header_type type, void *dataCallList, int remainingPayloadSize)
{
    char *start = NULL;
    const size_t legacyHeaderSize = LEN_DC_V11;
    // start from PCSCF extension
    start = (char *)(dataCallList) + legacyHeaderSize;

    remainingPayloadSize -= legacyHeaderSize;
    int IPC_version = start[0];

    // IPC_version field
    if((IPC_version == 2 || IPC_version == 3) && type == EXT_PCSCF) {
        RilLogV("IPC version is %d, start @ %p", IPC_version, start);
        return start;
    }
    else if(IPC_version == 1){
        RilLogV("IPC version is 1, return NULL");
        return NULL;
    }
    // IPC_version == 3 is reserved for EXT_PCSCF + RETRYTIMER

    data_call_extension_header_type currentEHT = EXT_PCSCF;
    data_call_extension_header_type nextEHT = (data_call_extension_header_type)start[1];
    if(type != EXT_PCSCF && IPC_version > 3)
    {
        while(nextEHT != NO_NEXT_HEADER && remainingPayloadSize > 0){
            // Loop to check next
            start += getSizeOfExtensionPayload(currentEHT);
            remainingPayloadSize -= getSizeOfExtensionPayload(currentEHT);
            // TBD: DecodeNextBlock or return
            if(nextEHT == type) return start;
            currentEHT = nextEHT;
            nextEHT = (data_call_extension_header_type)start[0];
        }
    }
    return start;
}
*/

static void fillPcscfExtInfoForLegacy(DataCall &out)
{
    out.IPC_version = 1;
    out.pcscf_ext_count = 0;
}

template<typename T>
static int fillExtInfoForLpRetryTime(DataCall &out, const sit_pdp_data_call_item *dataCallList, int payload_size, T *data)
{
    data = (T *)( ((char *) dataCallList) + payload_size - sizeof(T));
    out.suggestedRetryTime = (int64_t) data->suggestedRetryTime;
    RilLogV("retrytime:0x%llx", (int64_t) data->suggestedRetryTime);
    return sizeof(T);
    // ignore low_priority
}

static int fillAMBR(DataCall &out, const sit_pdp_data_call_item *dataCallList, int payload_size)
{
    sit_pdp_data_call_item_ext_ambr *data =
        (sit_pdp_data_call_item_ext_ambr *)
        ( ((char *) dataCallList) + payload_size
                                  - sizeof(sit_pdp_data_call_item_ext_ambr));
    out.ambr = *((AMBR *)data);
    RilLogV("octet2~8 = %d, (%d, %d), (%d, %d), (%d, %d)",
            data->octet2, data->octet3, data->octet4,
            data->octet5, data->octet6,
            data->octet7, data->octet8);

    return sizeof(sit_pdp_data_call_item_ext_ambr);
}

/*
static void DumpPcscfExt(DataCall &out)
{
    RilLogV("Dump PCSCF Extension");
    char buf[100];
    for(int i = 0; i < 5; i++) {
        int len = 0;
        for(int j=0; j<4; j++) {
            len += snprintf(&buf[len], 100-len, "%02x", out.ipv4.pcscf[MAX_IPV4_ADDR_LEN*MAX_PCSCF_NUM + j+4*i]);
        }
        RilLogV("IPv4[%d]: %s", i, buf);
        len = 0;
        for(int j=0; j<16; j++) {
            len += snprintf(&buf[len], 100-len, "%02x", out.ipv6.pcscf[MAX_IPV6_ADDR_LEN*MAX_PCSCF_NUM + j+16*i]);
        }
        RilLogV("IPv6[%d]: %s", i, buf);
    }
}
*/

/*
static void DumpPcscfExtPayload(char *data)
{
    RilLogV("Dump PCSCF Extension Payload");
    char buf[100];
    for(int i = 0; i < 5; i++) {
        int len = 0;
        for(int j=0; j<4; j++) {
            len += snprintf(&buf[len], 100-len, "%02x", data[2 + 4*i + j]);
        }
        RilLogV("IPv4[%d]: %s", i, buf);
        len = 0;
        for(int j=0; j<16; j++) {
            len += snprintf(&buf[len], 100-len, "%02x", data[2 + 4*5 + 16*i + j]);
        }
        RilLogV("IPv6[%d]: %s", i, buf);
    }
}
*/

static void FillPcscfExt(DataCall &out, const sit_pdp_data_call_item_pcscf_ext *data, BYTE pcscf_type)
{
    //DumpPcscfExtPayload( (char*) data);

    if( (pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV4)
    {
        memcpy(&out.ipv4.pcscf[MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM], &(data->pcscf_ipv4_ext[0]), MAX_IPV4_ADDR_LEN * MAX_PCSCF_EXT_NUM);
        RilLogV("IPV4 PCSCF Copy from %p to %p with %d bytes, offset:%d",
                &(data->pcscf_ipv4_ext[0]), &out.ipv4.pcscf[MAX_IPV4_ADDR_LEN*MAX_PCSCF_NUM],
                MAX_IPV4_ADDR_LEN * MAX_PCSCF_EXT_NUM, MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
    }
    if( (pcscf_type & SIT_PDP_PDP_TYPE_IPV4IPV6) & SIT_PDP_PDP_TYPE_IPV6)
    {
        memcpy(&out.ipv6.pcscf[MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM], &(data->pcscf_ipv6_ext[0]), MAX_IPV6_ADDR_LEN * MAX_PCSCF_EXT_NUM);
        RilLogV("IPV6 PCSCF Copy from %p to %p with %d bytes, offset:%d",
                &(data->pcscf_ipv6_ext[0]), &out.ipv6.pcscf[MAX_IPV6_ADDR_LEN*MAX_PCSCF_NUM],
                MAX_IPV6_ADDR_LEN * MAX_PCSCF_EXT_NUM, MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }
    RilLogV("Decoded Information IPC_version:%d @ %p", data->IPC_version, &data->IPC_version);
    out.IPC_version = data->IPC_version;
    out.pcscf_ext_count = 5;
    //DumpPcscfExt(out);
}

static int fillDefaultQosParam(DataCall &out, const sit_pdp_data_call_item *dataCallList, int payload_size)
{
    RilLogV("fillDefaultQosParam");

    sit_pdp_default_Qos_param *data =
        (sit_pdp_default_Qos_param *)
        ( ((char *) dataCallList) + payload_size
                                  - sizeof(sit_pdp_default_Qos_param));

    out.ipv6_mtu_size = (int)data->ipv6_mtu_size;
    out.pdu_session_id = (int)data->pdu_session_id;

    out.defaultQosType= (RIL_QosType) data->defaultQosType;

    if(data->defaultQosType ==EPS_QOS) {
        out.defaultEpsQos.qci = data->defaultQos.eps.qci;
        out.defaultEpsQos.downlink.guaranteedBitrateKbps= data->defaultQos.eps.downlink.guaranteedBitrateKbps;
        out.defaultEpsQos.downlink.maxBitrateKbps= data->defaultQos.eps.downlink.maxBitrateKbps;
        out.defaultEpsQos.uplink.guaranteedBitrateKbps= data->defaultQos.eps.uplink.guaranteedBitrateKbps;
        out.defaultEpsQos.uplink.maxBitrateKbps= data->defaultQos.eps.uplink.maxBitrateKbps;
        return sizeof(sit_pdp_EpsQos);
    }
    else if (out.defaultQosType ==NR_QOS) {
        out.defaultNrQos.fiveQi= data->defaultQos.nr.fiveQi;
        out.defaultNrQos.downlink.guaranteedBitrateKbps= data->defaultQos.nr.downlink.guaranteedBitrateKbps;
        out.defaultNrQos.downlink.maxBitrateKbps= data->defaultQos.nr.downlink.maxBitrateKbps;
        out.defaultNrQos.uplink.guaranteedBitrateKbps= data->defaultQos.nr.uplink.guaranteedBitrateKbps;
        out.defaultNrQos.uplink.maxBitrateKbps= data->defaultQos.nr.uplink.maxBitrateKbps;
        out.defaultNrQos.qfi= data->defaultQos.nr.qfi;
        out.defaultNrQos.averagingWindowMs= data->defaultQos.nr.averagingWindowMs;
        return sizeof(sit_pdp_NrQos);
    }
    else {
        RilLogV("fillDefaultQosParam, no info from CP");
        return sizeof(sit_pdp_NrQos);
    }

}

static int fillOptionalSliceInfo(DataCall &out, const sit_pdp_data_call_item *dataCallList)
{
    sit_pdp_optional_slice_info_param *data = (sit_pdp_optional_slice_info_param *)
        ( ((char *) dataCallList) + LEN_DC_V11_DEFAULT_QOS_PARAM);

//Fill the default value first
    out.sliceInfo.sst = (RIL_SliceServiceType) 0;
    out.sliceInfo.sliceDifferentiator = -1;
    out.sliceInfo.mappedHplmnSst = (RIL_SliceServiceType) 0;
    out.sliceInfo.mappedHplmnSD = -1;

//Fill received values
    switch(data->optionalSliceInfoType) {
      case OPT_SST: // 1
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) data->optionalSliceInfoSst;
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_MAPSST: // 2
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) data->optionalSliceInfoSst;
          out.sliceInfo.mappedHplmnSst=  (RIL_SliceServiceType) data->optionalSliceMappedHplmnSst;
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_SD:
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) data->optionalSliceInfoSst;
          out.sliceInfo.sliceDifferentiator= data->optionalSliceInfoSd;
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_SD_MAPSST:
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) data->optionalSliceInfoSst;
          out.sliceInfo.sliceDifferentiator= data->optionalSliceInfoSd;
          out.sliceInfo.mappedHplmnSD = data->optionalSliceMappedHplmnSd;
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      case OPT_SST_SD_MAPSST_MAPSD:
          out.sliceInfoType = RIL_OPT_INFO_VALID;
          out.sliceInfo.sst  = (RIL_SliceServiceType) data->optionalSliceInfoSst;
          out.sliceInfo.sliceDifferentiator= data->optionalSliceInfoSd;
          out.sliceInfo.mappedHplmnSst = (RIL_SliceServiceType) data->optionalSliceMappedHplmnSd;
          out.sliceInfo.mappedHplmnSD = data->optionalSliceMappedHplmnSd;
          out.sliceInfo.status = (RIL_SliceStatus)ALLOWED;
      break;
      default:
	  out.sliceInfoType = RIL_OPT_INFO_INVALID;
      break;
    }

    RilLogV("fillOptionalSliceInfo, dataCallList @ %p, %d %d %d %d %d", dataCallList, out.sliceInfoType, out.sliceInfo.sst,
               out.sliceInfo.sliceDifferentiator, out.sliceInfo.mappedHplmnSst, out.sliceInfo.mappedHplmnSD);
    return sizeof(sit_pdp_optional_slice_info_param);
}



static int fillTDInfo(DataCall &out, const sit_pdp_data_call_item *dataCallList)
{
    const int maxOsAppIdLen = 255;
    const int maxOsIdLen = 16;
    const int totalLen = maxOsAppIdLen + maxOsIdLen +1; //1BYTE for length
    uint8_t tdLen =  *((BYTE*) dataCallList + LEN_DC_V11_SLICE_INFO);

    out.tdLength = tdLen;
    RilLogV("fillTDInfo, tdLen: %d dataCallList @ %p", tdLen, dataCallList);

    if(tdLen == 0) {
        RilLogV("fillTDInfo, tdLen is Zero");
        return 0;
    }
    if (tdLen > MAX_TD_LIST) {
        RilLogV("tdLen %d is over MAX_TD_LIST, set to MAX(%d)", tdLen, MAX_TD_LIST);
        out.tdLength = tdLen = MAX_TD_LIST;

    }

    const sit_pdp_traffic_descriptor_param *data = (sit_pdp_traffic_descriptor_param *)
        ( ((char *) dataCallList) + LEN_DC_V11_SLICE_INFO + sizeof(BYTE));

    for(int i = 0; i < tdLen; i++) {
        BYTE dnnflag = (BYTE)(((data->tdBitMask) & TD_FLAG_DNN) >> 1);
        RilLogV("fillTDInfo, dnnflag 0x%x", dnnflag);
        if (dnnflag) {
            out.tdInfo[i].dnn.resize(1);
            out.tdInfo[i].dnn[0].append((char *)data->dnn, 0, MAX_PDP_APN_LEN);
        }

       /**
        * Byte array representing OSId + OSAppId. The minimum length of the array is
        * 18 and maximum length is 272 (16 bytes for OSId + 1 byte for OSAppId length
        * + up to 255 bytes for OSAppId).
        */
        BYTE osAppID = (BYTE)(((data->tdBitMask) & TD_FLAG_OSAPPID) >> 2);
        BYTE osID = (BYTE)(((data->tdBitMask) & TD_FLAG_OSID) >> 3);
        if (osAppID || osID) {
            RilLogV("fillTDInfo, osAppID exist");
            out.tdInfo[i].osAppIds.resize(1);
            memset(out.tdInfo[i].osAppIds[0].value, 0, totalLen);
            if (osID) {
                for (int idx = 0; idx < maxOsIdLen; idx++)
                    out.tdInfo[i].osAppIds[0].value[idx] = data->osID[idx];
            }
            if (osAppID) {
                out.tdInfo[i].osAppIds[0].value[maxOsIdLen] = (BYTE) strnlen((char*) data->osAppID, 255);
                RilLogV("fillTDInfo, osAppIDLen %d", out.tdInfo[i].osAppIds[0].value[maxOsIdLen]);
                for(int idx = 0; idx < maxOsAppIdLen; idx++)
                    out.tdInfo[i].osAppIds[0].value[maxOsIdLen + 1 + idx] = data->osAppID[idx];
            }
        }
    }

    return tdLen;
}

// Now support extension chain
// dataCallList will get each data_call_item
static int processPcscfExtension(DataCall &out, const sit_pdp_data_call_item *dataCallList, int remainingPayloadSize)
{
    if (dataCallList == NULL) {
        return 1;
    }

    RilLogV("processPcscfExtension, dataCallList @ %p, size:%d", dataCallList, remainingPayloadSize);
    // DataCall should be already parsed, Don't check validity
    //const sit_pdp_data_call_item_pcscf_ext *data = (sit_pdp_data_call_item_pcscf_ext *) searchNextExtensionPayload(EXT_PCSCF, (void *)dataCallList, remainingPayloadSize);
    const sit_pdp_data_call_item_pcscf_ext *data = (sit_pdp_data_call_item_pcscf_ext *) getExtensionPayload((void *)dataCallList);
    if( data == NULL ) {
        RilLogV("Can't find next extension payload");
        fillPcscfExtInfoForLegacy(out);
        return 1;
    }

    RilLogV("Found PCSCF Extension Payload, pcscf_ext @ %p, pcscf_type:%d", data, dataCallList->pcscf_type);
    FillPcscfExt(out, data, dataCallList->pcscf_type);
    return out.IPC_version;
}

// For v11
// Changed to care about one DataCall Item
// Indexing should be completed before calling.
// @return : processed data size
static int GetDataCallFromDataCallList_v11(DataCall &out, const sit_pdp_data_call_item *dataCallList)
{
    if (dataCallList == NULL) {
        return 0;
    }

    const sit_pdp_data_call_item *data = dataCallList;

    out.status = data->status & 0xffff;
    out.cid = (int)data->cid;
    out.active = data->active & 0x00ff;
    // Remap SIT_PDP_ACTIVE_* STATE to RIL DATA_CONNECTION_ACTIVE STATE
    if(out.active == SIT_PDP_ACTIVE_DORMANT) {
        out.active = ACTIVE_AND_LINKDOWN;
    } else if(out.active == SIT_PDP_ACTIVE_ACTIVE) {
        out.active = ACTIVE_AND_LINKUP;
    }

    out.pdpType = (int)data->pdp_type;
    // Do not store : out.dnsType, out.pcscfType
    //out.mtu_size= (int)data->ipv4_mtu_size;
    out.ipv4_mtu_size = (int)data->ipv4_mtu_size;
    out.pco = (int)data->pco;
    memset(&(out.ipv4), 0, sizeof(PDP_ADDR_V4));
    memset(&(out.ipv6), 0, sizeof(PDP_ADDR_V6));

    if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4) {
        out.ipv4.valid = TRUE;
        memcpy(out.ipv4.addr, data->address, 4);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV6) {
        out.ipv6.valid = TRUE;
        memcpy(out.ipv6.addr, data->address + 4, 16);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
        out.ipv4.valid = TRUE;
        memcpy(out.ipv4.addr, data->address, 4);

        out.ipv6.valid = TRUE;
        memcpy(out.ipv6.addr, data->address + 4, 16);
    }

    // Add for DNS IPv4/IPv6 Chcker Routine
    if((int)data->dns_type == SIT_PDP_PDP_TYPE_IPV4)
    {
        memcpy(out.ipv4.dns1, data->primary_dns, 4);
        memcpy(out.ipv4.dns2, data->secondary_dns, 4);
    }
    else if((int)data->dns_type == SIT_PDP_PDP_TYPE_IPV6)
    {
        memcpy(out.ipv6.dns1, data->primary_dns + 4, 16);
        memcpy(out.ipv6.dns2, data->secondary_dns + 4, 16);
    }
    else if((int)data->dns_type == SIT_PDP_PDP_TYPE_IPV4IPV6)
    {
        memcpy(out.ipv4.dns1, data->primary_dns, 4);
        memcpy(out.ipv4.dns2, data->secondary_dns, 4);
        memcpy(out.ipv6.dns1, data->primary_dns + 4, 16);
        memcpy(out.ipv6.dns2, data->secondary_dns + 4, 16);
    }

    if((int)data->pcscf_type == SIT_PDP_PDP_TYPE_IPV4)
    {
        memcpy(out.ipv4.pcscf, &(data->pcscf[0]), MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if((int)data->pcscf_type == SIT_PDP_PDP_TYPE_IPV6)
    {
        memcpy(out.ipv6.pcscf, &(data->pcscf[MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM]),
            MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if((int)data->pcscf_type == SIT_PDP_PDP_TYPE_IPV4IPV6)
    {
        memcpy(out.ipv4.pcscf, &(data->pcscf[0]), MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
        memcpy(out.ipv6.pcscf, &(data->pcscf[MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM]),
            MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }

    return sizeof(sit_pdp_data_call_item);
}

// For under v10, This function can use v11 DataCall structure
// @return : processed data size
static int GetDataCallFromDataCallList_v10(DataCall &out, int index, const sit_pdp_data_call_item_old *dataCallList, int listSize)
{
    if (index < 0 || listSize < 0 || index >= listSize) {
        return 0;
    }

    if (dataCallList == NULL) {
        return 0;
    }

    const sit_pdp_data_call_item_old *data = dataCallList + index;
    /*DEADCODE <- dataCallList cannot be null
    if (data == NULL) {
        return false;
    }
    */

    out.status = (int)data->status;
    out.cid = (int)data->cid;
    out.active = (int)data->active;
    out.pdpType = (int)data->pdp_type;
    memset(&(out.ipv4), 0, sizeof(PDP_ADDR_V4));
    memset(&(out.ipv6), 0, sizeof(PDP_ADDR_V6));

    if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4) {
        out.ipv4.valid = TRUE;
        memcpy(out.ipv4.addr, data->address, 4);
        memcpy(out.ipv4.dns1, data->primary_dns, 4);
        memcpy(out.ipv4.dns2, data->secondary_dns, 4);
        memcpy(out.ipv4.pcscf, &(data->pcscf[0]), MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV6) {
        out.ipv6.valid = TRUE;
        memcpy(out.ipv6.addr, data->address, 16);
        memcpy(out.ipv6.dns1, data->primary_dns, 16);
        memcpy(out.ipv6.dns2, data->secondary_dns, 16);
        memcpy(out.ipv6.pcscf, &(data->pcscf[0]), MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }
    else if (out.pdpType == SIT_PDP_PDP_TYPE_IPV4IPV6) {
        out.ipv4.valid = TRUE;
        memcpy(out.ipv4.addr, data->address, 4);
        memcpy(out.ipv4.dns1, data->primary_dns, 4);
        memcpy(out.ipv4.dns2, data->secondary_dns, 4);
        memcpy(out.ipv4.pcscf, &(data->pcscf[0]), MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM);

        out.ipv6.valid = TRUE;
        memcpy(out.ipv6.addr, data->address + 4, 16);
        memcpy(out.ipv6.dns1, data->primary_dns + 4, 16);
        memcpy(out.ipv6.dns2, data->secondary_dns + 4, 16);
        memcpy(out.ipv6.pcscf, &(data->pcscf[MAX_IPV4_ADDR_LEN * MAX_PCSCF_NUM]),
               MAX_IPV6_ADDR_LEN * MAX_PCSCF_NUM);
    }

    return sizeof(sit_pdp_data_call_item_old);
}


/**
 * ProtocolPsSetupDataCallAdapterLegacy
 */
ProtocolPsSetupDataCallAdapterLegacy::ProtocolPsSetupDataCallAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), ProtocolPsSetupDataCallAdapterInterface()
{
    InitDataCall(m_dataCall);
    Init();
}
void ProtocolPsSetupDataCallAdapterLegacy::Init()
{
    UINT payload_size = 0;
    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_pdp_setup_data_call_rsp *data = (sit_pdp_setup_data_call_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SETUP_DATA_CALL) {
            RilLogV("Modem RSP Length:%d, orgSize:%d", m_pModemData->GetLength(), sizeof(sit_pdp_setup_data_call_rsp));
            //m_pModemData->Dump();
            payload_size = sizeof(sit_pdp_setup_data_call_rsp);
            if( payload_size <= m_pModemData->GetLength() ) {
                RilLogV("Decode DC V11, payload:%d", payload_size);
                if (!GetDataCallFromDataCallList_v11(m_dataCall, &data->data_call)) {
                    InitDataCall(m_dataCall);
                }
                payload_size += sizeof(sit_pdp_data_call_item_pcscf_ext);
                if( payload_size <= m_pModemData->GetLength() ) {
                    RilLogV("Decode PCSCF EXT, payload:%d", payload_size);
                    // Extension Payload, IPC Version 2 is expected
                    // Process Extension
                    int IPC_version = processPcscfExtension(m_dataCall, &data->data_call,
                                          m_pModemData->GetLength() - sizeof(sit_pdp_setup_data_call_rsp));

                    sit_pdp_data_call_item_ext_lp_retrytime *t = nullptr;
                    sit_pdp_data_call_item_ext_lp_retrytime_1_6 *t_16 = nullptr;

                    switch(IPC_version) {
                        case DC_SLICE_TDINFO:
                            RilLogV("Decode Retry SuggestedTime, payload:%d, data_call:%p",
                            payload_size, &data->data_call);
                            payload_size += fillExtInfoForLpRetryTime(m_dataCall, &data->data_call, LEN_DC_V11_LP_RETRY_1_6, t_16);
                            RilLogV("Decode AMBR octet2~7, CID:%d, payload_size:%d", data->data_call.cid, payload_size);
                            payload_size += fillAMBR(m_dataCall, &data->data_call, LEN_DC_V11_AMBR_1_6);
                            RilLogV("Decode DefaultQoS, payload_size:%d", payload_size);
                            payload_size += fillDefaultQosParam(m_dataCall, &data->data_call, LEN_DC_V11_DEFAULT_QOS_PARAM);
                            //Newly-addedPart
                            RilLogV("Decode Optional SliceInfo, payload_size:%d", payload_size);
                            payload_size += fillOptionalSliceInfo(m_dataCall, &data->data_call);
                            if(checkValidTD(&data->data_call, (m_pModemData->GetLength() - payload_size)))  {
                                RilLogV("Decode Traffic Descriptor");
                                fillTDInfo(m_dataCall, &data->data_call);
                            }
                        break;
                        case DC_QOS:
                            RilLogV("Decode Retry SuggestedTime, payload:%d, data_call:%p",
                            payload_size, &data->data_call);
                            payload_size += fillExtInfoForLpRetryTime(m_dataCall, &data->data_call, LEN_DC_V11_LP_RETRY_1_6, t_16);
                            RilLogV("Decode AMBR octet2~7, CID:%d, payload_size:%d", data->data_call.cid, payload_size);
                            payload_size += fillAMBR(m_dataCall, &data->data_call, LEN_DC_V11_AMBR_1_6);
                            RilLogV("Decode DefaultQoS, CID:%d, payload_size:%d", data->data_call.cid, payload_size);
                            payload_size += fillDefaultQosParam(m_dataCall, &data->data_call, LEN_DC_V11_DEFAULT_QOS_PARAM);
                        break;
                        case DC_AMBR:
                            RilLogV("Decode AMBR octet2~7, CID:%d, payload_size:%d", data->data_call.cid, payload_size);
                            payload_size += fillAMBR(m_dataCall, &data->data_call, LEN_DC_V11_AMBR);
                            RilLogV("Decode Retry SuggestedTime, payload:%d, data_call:%p",
                            payload_size, &data->data_call);
                            payload_size += fillExtInfoForLpRetryTime(m_dataCall, &data->data_call, LEN_DC_V11_LP_RETRY, t);
                        break;
                        case DC_RETRYTIME:
                            RilLogV("Decode Retry SuggestedTime, payload:%d, data_call:%p",
                            payload_size, &data->data_call);
                            payload_size += fillExtInfoForLpRetryTime(m_dataCall, &data->data_call, LEN_DC_V11_LP_RETRY, t);
                        break;
                    }

                } else {
                    fillPcscfExtInfoForLegacy(m_dataCall);
                    return;
                }
            } else {
                // DEPRECATED
                payload_size = sizeof(sit_pdp_setup_data_call_rsp_old);
                sit_pdp_setup_data_call_rsp_old *data = (sit_pdp_setup_data_call_rsp_old *)m_pModemData->GetRawData();
                if (!GetDataCallFromDataCallList_v10(m_dataCall, 0, &data->data_call, 1)) {
                    InitDataCall(m_dataCall);
                }
                fillPcscfExtInfoForLegacy(m_dataCall);
            }
        }
    }
    RilLogV("processed %d bytes", payload_size);
}

UINT ProtocolPsSetupDataCallAdapterLegacy::GetErrorCode() const
{
    UINT errorCode = ProtocolRespAdapter::GetErrorCode();
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

int ProtocolPsSetupDataCallAdapterLegacy::GetAddrInfo(PDP_ADDR *pAddr, int pdpType) const
{
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

int ProtocolPsSetupDataCallAdapterLegacy::GetAddrInfo(PDP_ADDR *pAddr) const
{
    if (pAddr == NULL) {
        return -1;
    }

    // PDP_ADDR is union type, it's same to PDP_ADDR_V6
    memset(pAddr, 0, sizeof(PDP_ADDR));
    memcpy(&pAddr->ipv4, &m_dataCall.ipv4, sizeof(m_dataCall.ipv4));
    memcpy(&pAddr->ipv6, &m_dataCall.ipv6, sizeof(m_dataCall.ipv6));
    return 0;
}

/**
 * ProtocolPsDataCallListAdapterLegacy
 */
ProtocolPsDataCallListAdapterLegacy::ProtocolPsDataCallListAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), ProtocolPsDataCallListAdapterInterface() {
    m_dataCallNum = 0;
    for(int i = 0; i < MAX_DATA_CALL_SIZE; i++){
        InitDataCall(m_dataCallList[i]);
    }
    Init();
}

void ProtocolPsDataCallListAdapterLegacy::Init()
{
    m_dataCallNum = 0;

    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        m_dataCallNum = processDataCallList(GetParameter()[0], (sit_pdp_data_call_item *)&GetParameter()[1], GetParameterLength() - sizeof(BYTE), m_dataCallList);
    }
}

const DataCall *ProtocolPsDataCallListAdapterLegacy::GetDataCallList() const
{
    if (m_dataCallNum == 0)
        return NULL;
    return m_dataCallList;
}

const DataCall *ProtocolPsDataCallListAdapterLegacy::GetDataCallByIndex(unsigned int index) const
{
    if (index >= m_dataCallNum) {
        return NULL;
    }
    return &m_dataCallList[index];
}

const DataCall *ProtocolPsDataCallListAdapterLegacy::GetDataCallByCid(int cid) const
{
    for (unsigned int i = 0; i < m_dataCallNum; i++) {
        if (m_dataCallList[i].cid == cid)
            return &m_dataCallList[i];
    }
    return NULL;
}

/**
 * ProtocolPsDataCallListChangedAdapterLegacy
 */
ProtocolPsDataCallListChangedAdapterLegacy::ProtocolPsDataCallListChangedAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsDataCallListChangedAdapterInterface() {
    m_dataCallNum = 0;
    for(int i = 0; i < MAX_DATA_CALL_SIZE; i++){
        InitDataCall(m_dataCallList[i]);
    }
    Init();
}

dc_type DataCallUtil::DetermineDCTypeBySize(int num, const int length)
{
    dc_type dct = DC_V10;
    if ((num * LEN_DC_V11) == length) {
        dct = DC_V11;
    } else if ((num * LEN_DC_V11_PCSCF_EXT) == length) {
        dct = DC_EXTENSION;
    } else if ((num * LEN_DC_V11_LP_RETRY) == length) {
        dct = DC_RETRYTIME;
    } else if ((num * LEN_DC_V11_AMBR) == length) {
        dct = DC_AMBR;
    } else if ((num * LEN_DC_V11_DEFAULT_QOS_PARAM) == length) {
        dct = DC_QOS;
    } else if ((num * LEN_DC_V11_SLICE_INFO) <= length) {
        // We have no DataCall validation scheme. So we just assume,
        // In-size data is correct with latest format then ignore others
        RilLogV(" DataCallNum=%d, ExpectedSize=%d, CurrentSize = %d",
                num, num * (LEN_DC_V11_SLICE_INFO +1), length);
        dct = DC_SLICE_TDINFO;
    } else {
        RilLogV("Unknown packet. Set deprecated format type. This format is no more used. Something other problem");
        dct = DC_V10;
    }
    RilLogV("dc_type = %d", dct);
    return dct;
}

UINT ProtocolPsDataCallListChangedAdapterLegacy::GetErrorCode() const
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

void ProtocolPsDataCallListChangedAdapterLegacy::Init()
{
    m_dataCallNum = 0;

    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        m_dataCallNum = processDataCallList(GetParameter()[0], (sit_pdp_data_call_item *)&GetParameter()[1], GetParameterLength() - sizeof(BYTE), m_dataCallList);
    }
}

/* DCsizes = payloadsize - sizeof(BYTE) { DataCallNum has 1 byte variable } */
int DataCallUtil::processDataCallList(int dataCallNum, const sit_pdp_data_call_item *data, const int DCsizes, DataCall (&dataCallList)[MAX_DATA_CALL_SIZE])
{
    if (data != NULL && dataCallNum != 0) {
        dc_type dct = DetermineDCTypeBySize(dataCallNum, DCsizes);

        if (dataCallNum > NUM_OF_ELM(dataCallList)) {
            dataCallNum = NUM_OF_ELM(dataCallList);
        }

        RilLogV("DataCallNum:%d, Data:%p", dataCallNum, data);

        if (dct == DC_SLICE_TDINFO)
            FillDataCallListFromModemPayload_ForTD(dataCallList, dataCallNum, data, DCsizes);
        else
            FillDataCallListFromModemPayload(dataCallList, dataCallNum, data, dct);
    }
    return dataCallNum;
}

int DataCallUtil::GetDataCallFromList(DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE], const int blocksize,
    const sit_pdp_data_call_item *anchor, sit_pdp_data_call_item *(&pdata), int idx) {

    pdata = (sit_pdp_data_call_item *) ((char *) (anchor) + blocksize * idx);
    if (!GetDataCallFromDataCallList_v11(m_dataCallList[idx], pdata)) {
        InitDataCall(m_dataCallList[idx]);
    }
    return processPcscfExtension(m_dataCallList[idx], pdata, sizeof(sit_pdp_data_call_item_pcscf_ext));
}

void DataCallUtil::FillDataCallListFromModemPayload_ForTD(
    DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE],
    const unsigned int m_dataCallNum,
    const sit_pdp_data_call_item *data, const int DCsizes) {

    const sit_pdp_data_call_item* tmp_data_call_ptr = data;

    int processed_size = 0;
    for (unsigned int i = 0; i < m_dataCallNum; i++) {
        RilLogV("FillDataCallListFromModemPayload_ForTD: %d th Data Call", i);

        processed_size += GetDataCallFromDataCallList_v11(m_dataCallList[i], tmp_data_call_ptr);
        sit_pdp_data_call_item_ext_lp_retrytime_1_6 *t = nullptr;
        processed_size += fillExtInfoForLpRetryTime(m_dataCallList[i], tmp_data_call_ptr, LEN_DC_V11_LP_RETRY_1_6, t);

        RilLogV("0.Decode P-CSCF Extension, CID:%d", tmp_data_call_ptr->cid);
        processPcscfExtension(m_dataCallList[i], tmp_data_call_ptr,  LEN_DC_V11_PCSCF_EXT);

        RilLogV("1.Decode AMBR octet2~7, CID:%d", tmp_data_call_ptr->cid);
        processed_size += fillAMBR(m_dataCallList[i], tmp_data_call_ptr,  LEN_DC_V11_AMBR_1_6);

        RilLogV("2.Decode QOS, CID:%d", tmp_data_call_ptr->cid);
        processed_size += fillDefaultQosParam(m_dataCallList[i], tmp_data_call_ptr, LEN_DC_V11_DEFAULT_QOS_PARAM);

        RilLogV("3.Decode SliceInfo/TD Info, CID:%d", tmp_data_call_ptr->cid);
        processed_size += fillOptionalSliceInfo(m_dataCallList[i], tmp_data_call_ptr);

        int tdLength;
        if(checkValidTD(tmp_data_call_ptr, (DCsizes - processed_size)))  {
            //Check the next datacall addr, 1byte needs for td length
            tdLength = fillTDInfo(m_dataCallList[i], tmp_data_call_ptr);
        } else {
            RilLogW("!!!WARN!!! parsing of nextdatacall is not guaranteed, just assume no TD");
            tdLength = 0;
        }
        processed_size += tdLength * sizeof(sit_pdp_optional_trafficDesc_req);
        const int proceeding_size = LEN_DC_V11_SLICE_INFO + sizeof(BYTE) + (tdLength * sizeof(sit_pdp_optional_trafficDesc_req));
        RilLogV("Total processed size = %d for CID:%d, expected %d", processed_size, tmp_data_call_ptr->cid, proceeding_size);
        tmp_data_call_ptr = (sit_pdp_data_call_item *)
            ((char *) (tmp_data_call_ptr) + proceeding_size);
    }

}

void DataCallUtil::FillDataCallListFromModemPayload(
    DataCall (&m_dataCallList)[MAX_DATA_CALL_SIZE],
    const unsigned int m_dataCallNum,
    const sit_pdp_data_call_item *data,
    dc_type dct)
{
    const int BlockSize = dcsize_tbl[dct];
    switch(dct) {
    case DC_QOS:  // 5
    case DC_AMBR:  // 4
    case DC_RETRYTIME:  // 3
    case DC_EXTENSION:  // 2
        for (unsigned int i = 0; i < m_dataCallNum; i++) {
            sit_pdp_data_call_item *data_call;
            int IPC_version = GetDataCallFromList(m_dataCallList, BlockSize, data, data_call, i);

            if (IPC_version >= 3) {
                if (dct == DC_QOS) {
                    sit_pdp_data_call_item_ext_lp_retrytime_1_6 *t = nullptr;
                    fillExtInfoForLpRetryTime(m_dataCallList[i], data_call, LEN_DC_V11_LP_RETRY_1_6, t);
                } else {
                    sit_pdp_data_call_item_ext_lp_retrytime *t = nullptr;
                    fillExtInfoForLpRetryTime(m_dataCallList[i], data_call, LEN_DC_V11_LP_RETRY, t);
                }
            }
            if (IPC_version >= 4) {
                RilLogV("Decode AMBR octet2~7, CID:%d", data_call->cid);
                fillAMBR(m_dataCallList[i], data_call, (dct == DC_QOS) ? LEN_DC_V11_AMBR_1_6 : LEN_DC_V11_AMBR);
            }
            if (IPC_version >= 5) {
                RilLogV("Decode QOS, CID:%d", data_call->cid);
                fillDefaultQosParam(m_dataCallList[i], data_call, LEN_DC_V11_DEFAULT_QOS_PARAM);
            }
            if (IPC_version < 2) {
                RilLogV("Invalid IPC version:%d", IPC_version);
            }
        } // end for i ~
        break;
    case DC_V11:
        for (unsigned int i = 0; i < m_dataCallNum; i++) {
            if (!GetDataCallFromDataCallList_v11(m_dataCallList[i], &data[i])) {
                InitDataCall(m_dataCallList[i]);
            }
            fillPcscfExtInfoForLegacy(m_dataCallList[i]);
        } // end for i ~
        break;
    case DC_V10:
    default:
        // Deprecated.
        RilLogV("This format is no more used. Something other problem");
        /*
        sit_pdp_data_call_list_changed_ind_old *data = (sit_pdp_data_call_list_changed_ind_old *) m_pModemData->GetRawData();
        for (unsigned int i = 0; i < m_dataCallNum; i++) {
            if (!GetDataCallFromDataCallList_v10(m_dataCallList[i], i, data, m_dataCallNum)) {
                memset(&m_dataCallList[i], 0, sizeof(DataCall));
            }
            fillPcscfExtInfoForLegacy(m_dataCallList[i]);
        } // end for i ~
        */
        break;
    }
}

int DataCallUtil::stripDataCallFromModemData(const ModemData *m_pModemData, const int rcm_type, const int rcm_id, sit_pdp_data_call_item *(&data))
{
    unsigned int dataCallNum = 0;

    // RSP type
    if(rcm_type == RCM_TYPE_RESPONSE && rcm_id == SIT_GET_DATA_CALL_LIST) {
        sit_pdp_get_data_call_list_rsp *data_rsp = NULL;
        data_rsp = (sit_pdp_get_data_call_list_rsp *)m_pModemData->GetRawData();
        RilLogV("Rsp dataCallNum:%d", dataCallNum);
        dataCallNum = data_rsp->datacall_info_num;
        data = data_rsp->data_call;
        RilLogV("Data:%p", data);
    }
    // IND type
    else if (rcm_type == RCM_TYPE_INDICATION && rcm_id == SIT_IND_DATA_CALL_LIST_CHANGED) {
        sit_pdp_data_call_list_changed_ind *data_ind = NULL;
        data_ind = (sit_pdp_data_call_list_changed_ind *)m_pModemData->GetRawData();
        dataCallNum = data_ind->datacall_info_num;
        RilLogV("Ind dataCallNum:%d", dataCallNum);
        data = data_ind->data_call;
        RilLogV("Data:%p", data);
    }
    return dataCallNum;

}

bool DataCallUtil::checkValidTD(const sit_pdp_data_call_item *orgdata, unsigned int remainingsize) {
    uint8_t tdLen =  *((BYTE*) orgdata + LEN_DC_V11_SLICE_INFO);
    if (remainingsize < tdLen * sizeof(sit_pdp_traffic_descriptor_param) + sizeof(BYTE)) return false;
    else return true;
}


const DataCall *ProtocolPsDataCallListChangedAdapterLegacy::GetDataCallList() const
{
    if (m_dataCallNum == 0)
        return NULL;
    return m_dataCallList;
}

const DataCall *ProtocolPsDataCallListChangedAdapterLegacy::GetDataCallByIndex(unsigned int index) const
{
    if (index >= m_dataCallNum) {
        return NULL;
    }
    return &m_dataCallList[index];
}

const DataCall *ProtocolPsDataCallListChangedAdapterLegacy::GetDataCallByCid(int cid) const
{
    for (unsigned int i = 0; i < m_dataCallNum; i++) {
        if (m_dataCallList[i].cid == cid)
            return &m_dataCallList[i];
    }
    return NULL;
}

/**
 * ProtocolPsDedicatedBearInfoAdapterLegacy
 */
ProtocolPsDedicatedBearInfoAdapterLegacy::ProtocolPsDedicatedBearInfoAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsDedicatedBearInfoAdapterInterface()
{
    m_cid= -1;
    m_status = -1;
    m_QosSession = {};
    m_QosSession.qosSessionId = -1;
    Init();
}

void ProtocolPsDedicatedBearInfoAdapterLegacy::Init()
{
    if (m_pModemData != NULL) {
        processQosSession(m_pModemData);
    }
}

void ProtocolPsDedicatedBearInfoAdapterLegacy::processQosSession(const ModemData *m_pModemData)
{
    sit_pdp_dedicated_bearer_info_ind *data = (sit_pdp_dedicated_bearer_info_ind *)m_pModemData->GetRawData();
    QosFilter_t qosFilter;

    if (data != NULL) {
        m_QosSession.qosSessionId = data->qos_session_id;
        m_cid = data->cid;
        m_status = data->status;
        m_QosSession.defaultQosType = (RIL_QosType)data->qos_rat_type;

        if(m_QosSession.defaultQosType == EPS_QOS) {
            m_QosSession.defaultEpsQos.qci= data->qos.eps.qci;
            m_QosSession.defaultEpsQos.downlink.guaranteedBitrateKbps= data->qos.eps.downlink.guaranteedBitrateKbps;
            m_QosSession.defaultEpsQos.downlink.maxBitrateKbps= data->qos.eps.downlink.maxBitrateKbps;
            m_QosSession.defaultEpsQos.uplink.guaranteedBitrateKbps= data->qos.eps.uplink.guaranteedBitrateKbps;
            m_QosSession.defaultEpsQos.uplink.maxBitrateKbps= data->qos.eps.uplink.maxBitrateKbps;
        }
        else if (m_QosSession.defaultQosType == NR_QOS) {
            m_QosSession.defaultNrQos.fiveQi= data->qos.nr.fiveQi;
            m_QosSession.defaultNrQos.downlink.guaranteedBitrateKbps= data->qos.nr.downlink.guaranteedBitrateKbps;
            m_QosSession.defaultNrQos.downlink.maxBitrateKbps= data->qos.nr.downlink.maxBitrateKbps;
            m_QosSession.defaultNrQos.uplink.guaranteedBitrateKbps= data->qos.nr.uplink.guaranteedBitrateKbps;
            m_QosSession.defaultNrQos.uplink.maxBitrateKbps= data->qos.nr.uplink.maxBitrateKbps;
            m_QosSession.defaultNrQos.qfi= data->qos.nr.qfi;
            m_QosSession.defaultNrQos.averagingWindowMs= data->qos.nr.averagingWindowMs;
        }
        else {
            RilLogV("fillDefaultQosParam, no info from CP");
        }

        m_QosSession.QosFilterCnt = data->num_of_qos_filter > 0 ? MIN(data->num_of_qos_filter, MAX_QOS_FILTERS) : 0;

        for(int i = 0 ; i < m_QosSession.QosFilterCnt; i++)
        {
            memset(&qosFilter, 0, sizeof(QosFilter_t));
            memcpy(qosFilter.localAddress[0], data->qos_filter[i].local_address, 16);
            memcpy(qosFilter.localAddress[1], data->qos_filter[i].local_address + 16, 4);
            memcpy(&qosFilter.localAddrPrefix_v6, &data->qos_filter[i].local_address_prefix_v6, 1);
            memcpy(&qosFilter.localAddrPrefix_v4, &data->qos_filter[i].local_address_prefix_v4, 1);

            memcpy(qosFilter.remoteAddress[0], data->qos_filter[i].remote_address, 16);
            memcpy(qosFilter.remoteAddress[1], data->qos_filter[i].remote_address + 16, 4);
            memcpy(&qosFilter.remotePortprefix_6, &data->qos_filter[i].remote_address_prefix_v6, 1);
            memcpy(&qosFilter.remotePortprefix_4, &data->qos_filter[i].remote_address_prefix_v4, 1);

            if(data->qos_filter[i].field_flag & 0x01){
                qosFilter.localPortFlag = MAYBE_PORT_VALID;
            }
            qosFilter.localPortStart= data->qos_filter[i].local_port_low;
            qosFilter.localPortEnd= data->qos_filter[i].local_port_high;

            if(data->qos_filter[i].field_flag & 0x02){
                qosFilter.remotePortFlag = MAYBE_PORT_VALID;
            }
            qosFilter.remotePortStart= data->qos_filter[i].remote_port_low;
            qosFilter.remotePortEnd= data->qos_filter[i].remote_port_high;

            qosFilter.protocol= data->qos_filter[i].qos_protocol;
            if(data->qos_filter[i].field_flag & 0x04){
                qosFilter.tosFlag = 1;
            }
            qosFilter.tosValue= data->qos_filter[i].type_of_service;

            if(data->qos_filter[i].field_flag & 0x08){
                qosFilter.flowLabelFlag = IPV6_FLOW_LABEL_VALID;
            }
            qosFilter.flowLabelValue= data->qos_filter[i].ipv6_flow_label;

            if(data->qos_filter[i].field_flag & 0x10){
                qosFilter.spiFlag = IP_SEC_SPI_VALID;
            }
            qosFilter.spiValue = data->qos_filter[i].ip_sec_spi;

            // Does -1 to convert the value defined in the SIT interface to the value defined in hal.type.
            // In CISCO equipment, there is a voc that REL-7 should be treated as downlink in that case.
            // There is no clear criterion to treat rel-7 as a downlink, but first treat it as a downlink.
            if(data->qos_filter[i].direction == SIT_REL_7) {
                data->qos_filter[i].direction = SIT_DOWNLINK;
            }

            // The direction defined by SIT and the direction defined by android are different.
            // -1 to match the direction based on android.
            qosFilter.direction = data->qos_filter[i].direction - 1;
            qosFilter.precedence= data->qos_filter[i].precedence;

            m_QosSession.QosFilters.push_back(qosFilter);
        }
    }
}

/**
 * ProtocolPsNasTimerStatusAdapter
 */
ProtocolPsNasTimerStatusAdapterLegacy::ProtocolPsNasTimerStatusAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsNasTimerStatusAdapterInterface()
{
}

const SitNasTimerStatus *ProtocolPsNasTimerStatusAdapterLegacy::GetNasTimerStatus()
{
    if (m_pModemData != NULL) {
        sit_pdp_nas_timer_status_ind *data = (sit_pdp_nas_timer_status_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_NAS_TIMER_STATUS) {
             if ((unsigned int)m_pModemData->GetLength() >= sizeof(sit_pdp_nas_timer_status_ind)) {
                return (SitNasTimerStatus *)&data->timer_type;
            }
        }
    }
    return NULL;
}

/**
 * ProtocolPsStartKeepAliveAdapterLegacy
 */
ProtocolPsStartKeepAliveAdapterLegacy::ProtocolPsStartKeepAliveAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), ProtocolPsStartKeepAliveAdapterInterface()
{
}

unsigned int ProtocolPsStartKeepAliveAdapterLegacy::getSessionHandle() const
{
    unsigned int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_start_keepalive_rsp *data = (sit_pdp_start_keepalive_rsp *)m_pModemData->GetRawData();
        if (data != NULL ) {
            ret = data->session_handle;
        }
    }
    return ret;
}

int ProtocolPsStartKeepAliveAdapterLegacy::getCode() const
{
    int status = KEEPALIVE_INACTIVE;
    if (m_pModemData != NULL) {
        sit_pdp_start_keepalive_rsp *data = (sit_pdp_start_keepalive_rsp *)m_pModemData->GetRawData();
        if (data != NULL ) {
            status = data->status_code;
        }
    }
    return status;
}

/**
 * ProtocolPsKeepAliveStatusAdapterLegacy
 */
ProtocolPsKeepAliveStatusAdapterLegacy::ProtocolPsKeepAliveStatusAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsKeepAliveStatusAdapterInterface()
{
}

unsigned int ProtocolPsKeepAliveStatusAdapterLegacy::getSessionHandle() const
{
    unsigned int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_keepalive_status_ind *data = (sit_pdp_keepalive_status_ind *)m_pModemData->GetRawData();
        if (data != NULL ) {
            ret = data->session_handle;
        }
    }
    return ret;
}

int ProtocolPsKeepAliveStatusAdapterLegacy::getCode() const
{
    int status = KEEPALIVE_INACTIVE;
    if (m_pModemData != NULL) {
        sit_pdp_keepalive_status_ind *data = (sit_pdp_keepalive_status_ind *)m_pModemData->GetRawData();
        if (data != NULL ) {
            status = data->status_code;
        }
    }
    return status;
}

/**
 * ProtocolPsPcoDataAdapterLegacy
 */
ProtocolPsPcoDataAdapterLegacy::ProtocolPsPcoDataAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsPcoDataAdapterInterface()
{
    if (m_pModemData != NULL) {
        sit_pdp_pco_data_ind *data = (sit_pdp_pco_data_ind *)m_pModemData->GetRawData();
        remainPcoBlocks = data->pco_num;
    } else {
        remainPcoBlocks = 0;
    }
    nextPcoBlockPos = 0;
}

int ProtocolPsPcoDataAdapterLegacy::GetCid() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_pco_data_ind *data = (sit_pdp_pco_data_ind *)m_pModemData->GetRawData();
        if (data != NULL ) {
            ret = data->cid;
        }
    }
    return ret;
}

int ProtocolPsPcoDataAdapterLegacy::GetPdpType() const
{
    int ret = PDP_TYPE_IPV4;
    if (m_pModemData != NULL) {
        sit_pdp_pco_data_ind *data = (sit_pdp_pco_data_ind *)m_pModemData->GetRawData();
        if (data != NULL ) {
            ret = data->pdp_type;
        }
    }
    return ret;
}

int ProtocolPsPcoDataAdapterLegacy::GetPcoData(sit_pdp_pco_data_entry &pco_data)
{
    if (remainPcoBlocks == 0) return -1;

    if (m_pModemData != NULL) {
        //m_pModemData->Dump();
        sit_pdp_pco_data_ind *data = (sit_pdp_pco_data_ind *)m_pModemData->GetRawData();
        char *pco_block = (char*)m_pModemData->GetRawData() + sizeof(sit_pdp_pco_data_ind) + nextPcoBlockPos;
        //int total_length = data->hdr.length - 4; // HEADER.type(1)+reserved(1)+id(2)=4
        //RilLogV("total_length:%d pco_block = %p, nextPcoBlockPos:%d, remainPcoBlocks:%d",
        //        total_length, pco_block, nextPcoBlockPos, remainPcoBlocks);
        pco_data.pco_id = ((INT32*)(pco_block))[0];
        pco_data.contents_len = pco_block[4];
        unsigned int current_blocksize = sizeof(INT32) + sizeof(BYTE) + pco_data.contents_len;
        pco_data.contents =(char *)(data) +
            sizeof(sit_pdp_pco_data_ind) + nextPcoBlockPos +
            sizeof(INT32) /* pco_id */ + sizeof(BYTE); /* contents_len */
        //RilLogV("pco_id=%x(%d), contents_len = %d, current_blocksize=%d, contents:%p",
        //        pco_data.pco_id, pco_data.pco_id, pco_data.contents_len, current_blocksize, pco_data.contents);
        nextPcoBlockPos += current_blocksize;
    }

    return --remainPcoBlocks;
}

int ProtocolPsPcoDataAdapterLegacy::GetPcoNum() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_pdp_pco_data_ind *data = (sit_pdp_pco_data_ind *)m_pModemData->GetRawData();
        if (data != NULL ) {
            ret = data->pco_num;
        }
    }
    return ret;
}

/**
 * ProtocolPsDataOffStatusAdapterLegacy
 */
ProtocolPsDataOffStatusAdapterLegacy::ProtocolPsDataOffStatusAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsDataOffStatusAdapterInterface()
{
    psDataOffAvailableStatus = -1;
    exemptListForHome = -1;
    exemptListForRoam = -1;

    sit_pdp_ps_data_off_status_ind *data = (sit_pdp_ps_data_off_status_ind *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_IND_PS_DATA_OFF_STATUS) {
        psDataOffAvailableStatus = data->ps_data_off_available_status;
        exemptListForHome = data->exempt_list_for_home;
        exemptListForRoam = data->exempt_list_for_roam;
    }
}

int ProtocolPsDataOffStatusAdapterLegacy::GetPsDataOffAvailableStatus() const
{
    return psDataOffAvailableStatus;
}

int ProtocolPsDataOffStatusAdapterLegacy::GetExemptListForHome() const
{
    return exemptListForHome;
}

int ProtocolPsDataOffStatusAdapterLegacy::GetExemptListForRoam() const
{
    return exemptListForRoam;
}

/**
 * ProtocolPsIpAddrStatusAdapterLegacy
 */
ProtocolPsIpAddrStatusAdapterLegacy::ProtocolPsIpAddrStatusAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsIpAddrStatusAdapterInterface()
{
    cid = status = -1;
    memset(ipv4_addr, 0, MAX_IPV4_ADDR_LEN);
    memset(ipv6_addr, 0, MAX_IPV6_ADDR_LEN);
    Init();
}

void ProtocolPsIpAddrStatusAdapterLegacy::Init() {
    if (m_pModemData != NULL) {
        sit_pdp_ip_addr_status_ind *data = (sit_pdp_ip_addr_status_ind *)m_pModemData->GetRawData();
        if (data != NULL ) {
            status = data->status;
            cid = data->cid;

            memcpy(ipv4_addr, data->IPv4Address, MAX_IPV4_ADDR_LEN);
            memcpy(ipv6_addr, data->IPv6Address, MAX_IPV6_ADDR_LEN);
        }
    }
}

int ProtocolPsIpAddrStatusAdapterLegacy::GetCid() const {
    return cid;
}

int ProtocolPsIpAddrStatusAdapterLegacy::GetStatus() const {
    return status;
}

void ProtocolPsIpAddrStatusAdapterLegacy::GetAddrInfo(char (&ipv4addr)[MAX_IPV4_ADDR_LEN], char (&ipv6addr)[MAX_IPV6_ADDR_LEN]) const {
    memcpy(ipv4addr, ipv4_addr, MAX_IPV4_ADDR_LEN);
    memcpy(ipv6addr, ipv6_addr, MAX_IPV6_ADDR_LEN);
}

/**
 * ProtocolPsUnthrottleApnLegacy
 */
ProtocolPsUnthrottleApnLegacy::ProtocolPsUnthrottleApnLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData), ProtocolPsUnthrottleApnInterface()
{
    memset(m_apn, 0, sizeof(m_apn));

    sit_pdp_unthrottle_apn_ind *data = (sit_pdp_unthrottle_apn_ind *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_IND_UNTHROTTLE_APN) {
        if (*data->apn != 0) {
            memcpy(m_apn, data->apn, MAX_PDP_APN_LEN);
        }
    }
}

const char *ProtocolPsUnthrottleApnLegacy::GetApn() const
{
    return m_apn;
}

/**
 * ProtocolPsAllocatePduSessionId
 */
ProtocolPsAllocatePduSessionIdLegacy::ProtocolPsAllocatePduSessionIdLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), ProtocolPsAllocatePduSessionIdInterface()
{
    m_sessionId = -1;

    sit_pdp_alloc_pdu_session_resp *data = (sit_pdp_alloc_pdu_session_resp *)m_pModemData->GetRawData();
    if (GetErrorCode() == RIL_E_SUCCESS && data != NULL && data->hdr.id == SIT_ALLOC_PDU_SESSION) {
        m_sessionId = data->session_id;
    }
}

int ProtocolPsAllocatePduSessionIdLegacy::GetSessionId() const
{
    return m_sessionId;
}

/**
 * ProtocolPsSlicingConfigLegacy
 */
ProtocolPsSlicingConfigLegacy::ProtocolPsSlicingConfigLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), ProtocolPsSlicingConfigInterface()
{
    m_ipcVersion = 0;
    m_payload_size = 0;

    m_slicingConfig.urspNum = 0;
    m_slicingConfig.urspRules.clear();
    m_slicingConfig.sliceInfoNum = 0;
    m_slicingConfig.sliceInfo.clear();
    if (m_pModemData != NULL && GetErrorCode() == RIL_E_SUCCESS) {
        sit_pdp_slicing_config_item *slicing_config = (sit_pdp_slicing_config_item *)GetParameter();
        processSlicingConfig(slicing_config);
    }
}

DataV2_0::RIL_SlicingConfig ProtocolPsSlicingConfigLegacy::GetSlicingConfig() const {
    return SlicingConfigUtil::GetSlicingConfig();
}

int ProtocolPsSlicingConfigLegacy::GetPayloadSize() const {
    return SlicingConfigUtil::GetPayloadSize();
}

UINT ProtocolPsSlicingConfigLegacy::GetErrorCode() const {
    return ProtocolRespAdapter::GetErrorCode();
}

/**
 * ProtocolPsSlicingConfigUpdatedLegacy
 */
ProtocolPsSlicingConfigUpdatedLegacy::ProtocolPsSlicingConfigUpdatedLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    m_ipcVersion = 0;
    m_payload_size = 0;

    m_slicingConfig.urspNum = 0;
    m_slicingConfig.urspRules.clear();
    m_slicingConfig.sliceInfoNum = 0;
    m_slicingConfig.sliceInfo.clear();
    if (m_pModemData != NULL) {
        sit_pdp_slicing_config_item *slicing_config = (sit_pdp_slicing_config_item *)GetParameter();
        processSlicingConfig(slicing_config);
    }
}

DataV2_0::RIL_SlicingConfig ProtocolPsSlicingConfigUpdatedLegacy::GetSlicingConfig() const {
    return SlicingConfigUtil::GetSlicingConfig();
}

int ProtocolPsSlicingConfigUpdatedLegacy::GetPayloadSize() const {
    return SlicingConfigUtil::GetPayloadSize();
}

UINT ProtocolPsSlicingConfigUpdatedLegacy::GetErrorCode() const {
    return 0;  // No meaning, just to reuse same interface
}

/**
 * SlicingConfigUtil
 */
void SlicingConfigUtil::processSlicingConfig(sit_pdp_slicing_config_item *slicing_config) {
    int payload_size = 0;

    m_ipcVersion = slicing_config->slicing_config_version;
    payload_size += sizeof(slicing_config->slicing_config_version);

    RilLogV("m_ipVersion : %d", m_ipcVersion);
    if (m_ipcVersion >= VALID_SLICINGCONFIG) {
        if (m_ipcVersion >= EXTENDED_SLICINGCONFIG )
            fillURSPRuleMultiParam(m_slicingConfig, slicing_config, payload_size);
        else
            fillURSPRuleParamLegacy(m_slicingConfig, slicing_config, payload_size);
        m_slicingConfig.sliceInfo = getSliceInfoParam((char *)slicing_config, payload_size);
        SetPayloadSize(payload_size);
    } else {
        RilLogE("IPC version is invalid");
    }

    return;
}

void SlicingConfigUtil::clearRouteSelectionDescriptor(RIL_RouteSelectionDescriptor &rsd) {
    memset(&rsd, 0, sizeof(rsd));
    rsd.precedence = 0;
    rsd.sessionType.value = PDP_PROTOCOL_TYPE_UNKNOWN;
    rsd.sscMode.value = MODE_1;
    rsd.sliceNum = 0;
    rsd.sliceInfo.clear();
    rsd.dnnNum = 0;
    rsd.dnn.clear();
}

static void fillExtendedSlicingConfig(RIL_RouteSelectionDescriptor &dst, sit_pdp_route_selection_descriptor *src, int &payload_size) {
    dst.preferredAccessType = src->preferredAccessType;
    dst.multiAccessPreferenceType = src->multiAccessPreferenceType;
    dst.nonSeamlessNon3gppOffloadIndication = src->nonSeamlessNon3gppOffloadIndication;
    memcpy(dst.timeWindowType, src->timeWindowType, 16);
    payload_size += (sizeof(src->preferredAccessType) + sizeof(src->multiAccessPreferenceType) +
                     sizeof(src->nonSeamlessNon3gppOffloadIndication) + 16);
}

void SlicingConfigUtil::fillRouteSelectionDescriptor(DataV2_0::RIL_SlicingConfig &/* out */, sit_pdp_slicing_config_item *config_data, int &payload_size, DataV2_0::RIL_UrspRule &urspRule) {
    // ursp_data = (sit_pdp_ursp_rules *)(((char *)config_data) + payload_size); // payload_size include TD size
    urspRule.RSDNum = *(((char *) config_data) + payload_size);
    payload_size += sizeof(BYTE);

    RIL_RouteSelectionDescriptor routeSelectionDescriptor;
    urspRule.routeSelectionDescriptor.clear();

    sit_pdp_route_selection_descriptor * route_selection_data = (sit_pdp_route_selection_descriptor *)(((char *)config_data) + payload_size);
    RilLogV("%d  RSDNUM :%d, payload_size:%d", __LINE__, urspRule.RSDNum, payload_size);

    for (int k = 0; k < urspRule.RSDNum; k++) {
        clearRouteSelectionDescriptor(routeSelectionDescriptor);

        // precedence, sessionType, sscMode
        routeSelectionDescriptor.precedence = route_selection_data->precedence;
        routeSelectionDescriptor.sessionType.value = (PdpProtocolType)route_selection_data->sessionType;
        routeSelectionDescriptor.sscMode.value = (SscMode)route_selection_data->sscMode;
        payload_size += (sizeof(route_selection_data->precedence) + sizeof(route_selection_data->sessionType) + sizeof(route_selection_data->sscMode));

        //* New updated for IPC v2, fixed size field is added in the middle
        if (m_ipcVersion >= EXTENDED_SLICINGCONFIG) {
            fillExtendedSlicingConfig(routeSelectionDescriptor, route_selection_data, payload_size);
            // Location Criteria Type info
            // increase payload_size
            fillLocationCriteriaTypeInfo(routeSelectionDescriptor, (char *)config_data, payload_size);
        }

        // Slice Info
        routeSelectionDescriptor.sliceNum = * (((char *) config_data) + payload_size);
        routeSelectionDescriptor.sliceInfo = getSliceInfoParam((char *)config_data, payload_size);

        // DNN
        routeSelectionDescriptor.dnnNum = * (((char *) config_data) + payload_size);
        payload_size += sizeof(char);

        for (int j = 0; j < routeSelectionDescriptor.dnnNum; j++) {
            routeSelectionDescriptor.dnn.push_back(getDnnInfoParam((char *)config_data, payload_size));
        }

        route_selection_data = (sit_pdp_route_selection_descriptor *)(((char *)config_data) + payload_size);

        urspRule.routeSelectionDescriptor.push_back(routeSelectionDescriptor);
    }
    RilLogV("%d  RSD Done, payload_size:%d", __LINE__, payload_size);
}

void SlicingConfigUtil::fillURSPRuleParamLegacy(DataV2_0::RIL_SlicingConfig &out, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    DataV2_0::RIL_UrspRule urspRule = {};

    out.urspRules.clear();

    out.urspNum = config_data->urspNum;
    payload_size += sizeof(config_data->urspNum);

    sit_pdp_ursp_rules * ursp_data = (sit_pdp_ursp_rules *)(((char *)config_data) + payload_size);
    // RilLogV("ursp_data:%p payload_size:%d", ursp_data, payload_size);

    for (int i = 0; i < out.urspNum; i++) {
        DataV2_0::RIL_TrafficDescriptor trafficDescriptor = {};
        urspRule.trafficDescriptors.clear();
        urspRule.routeSelectionDescriptor.clear();

        /*
         * precedence @ [0] byte
         */
        urspRule.precedence = ursp_data->precedence;
        payload_size += sizeof(ursp_data->precedence);

        /*
         * trafficDescriptor
         */
        fillDnnIntoTdLegacy(trafficDescriptor, config_data, payload_size);
        fillOsAppIdIntoTdLegacy(trafficDescriptor, config_data, payload_size);

        urspRule.trafficDescriptors.push_back(trafficDescriptor);

        fillRouteSelectionDescriptor(out, config_data, payload_size, urspRule);

        out.urspRules.push_back(urspRule);
        RilLogV("%d  processed payload_size:%d, urspnum:%d", __LINE__, payload_size, out.urspRules.size());
        // update to next URSP RULE
        ursp_data = (sit_pdp_ursp_rules *)(((char *)config_data) + payload_size);
    }
}

void SlicingConfigUtil::fillDnnIntoTdLegacy(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    sit_pdp_traffic_descriptor * td_data = (sit_pdp_traffic_descriptor *)((char *)config_data + payload_size);
    if (td_data->Length > 0) {
        td.dnn.resize(1);
        td.dnn.push_back(getDnnInfoParam2((char *)config_data, payload_size));
    }
}

void SlicingConfigUtil::fillOsAppIdIntoTdLegacy(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    td.osAppIds.resize(1);
    memset(td.osAppIds[0].value, 0, sizeof(td.osAppIds[0].value));
    int OS_id_Length = copyVariableData(&td.osAppIds[0].value[0], (char *)config_data, payload_size);
    // OS_ID
    if (OS_id_Length > 0) {
        char osid[MAX_OSID_SIZE];
        memcpy(osid, &td.osAppIds[0].value[0], MAX_OSID_SIZE);
        RilLogV("Os_id_Length:%d, osId:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x, payload_size:%d",
                OS_id_Length, osid[0], osid[1], osid[2], osid[3], osid[4], osid[5], osid[6], osid[7],
                osid[8], osid[9], osid[10], osid[11], osid[12], osid[13], osid[14], osid[15], payload_size);
    } else {
        RilLogV("Os_id_length:%d", OS_id_Length);
        // First 16 bytes shall be osid. fixed position
        memset(&td.osAppIds[0].value[0], 0, sizeof(BYTE) * MAX_OSID_SIZE);
    }

    // OS_APPID
    int OS_App_id_Length = copyVariableData(&td.osAppIds[0].value[MAX_OSID_SIZE + 1], (char *)config_data, payload_size);
    td.osAppIds[0].value[MAX_OSID_SIZE] = OS_App_id_Length;
    if (OS_App_id_Length > 0) {
        RilLogV("OS_App_id_Length:%d=%d, osAppIds[0]:%s, payload_size:%d", OS_App_id_Length, td.osAppIds[0].value[MAX_OSID_SIZE],
                &td.osAppIds[0].value[MAX_OSID_SIZE + 1], payload_size);
    } else {
        RilLogV("OS_App_id_Length:%d, payload_size:%d", OS_App_id_Length, payload_size);
    }

    if (OS_id_Length == 0 && OS_App_id_Length == 0) {
        td.osAppIds.clear();
    }
}

void SlicingConfigUtil::fillDnnIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    td.dnn.clear();
    uint16_t length = copyVariableMultiString(td.dnn, (char *)config_data, payload_size);
    RilLogV("dnn_length:%d, dnn[0]:%s, payload_size:%d", length, td.dnn.empty() ? "dnn is empty!" : td.dnn[0].c_str(), payload_size);
}

void SlicingConfigUtil::fillOsAppIdIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    // OS_ID
    td.osAppIds.clear();
    vector<DataV2_0::RIL_OSAppId> tmpOsId;
    uint16_t OS_id_Length = copyVariableMultiData(tmpOsId, (char *)config_data, payload_size, false);
    RilLogV("%d  Os_id_length:%d, ps:%d", __LINE__, OS_id_Length, payload_size);
    if (OS_id_Length > 0) {
        char osid[MAX_OSID_SIZE];
        memcpy(osid, &tmpOsId[0].value[0], MAX_OSID_SIZE);
        RilLogV("  osId[0]:%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
                osid[0], osid[1], osid[2], osid[3], osid[4], osid[5], osid[6], osid[7],
                osid[8], osid[9], osid[10], osid[11], osid[12], osid[13], osid[14], osid[15]);
    }
    // OS_APPID
    vector<DataV2_0::RIL_OSAppId> tmpOsAppId;
    uint16_t OS_App_id_Length = copyVariableMultiData(tmpOsAppId, (char *)config_data, payload_size, true);
    RilLogV("%d  OS_App_id_Length:%d, payload_size:%d", __LINE__, OS_App_id_Length, payload_size);
    if (OS_App_id_Length > 0) {
        RilLogV("   length[0] = %d, osAppId[0]:%s", tmpOsAppId[0].value[0], &tmpOsAppId[0].value[1]);
    }

    if (OS_id_Length != 0 || OS_App_id_Length != 0) {
        int osappid_size = 0;
        if (tmpOsAppId.size() < tmpOsId.size()) {
            RilLogV("Unexpected id count, osAppId:%d, osId:%d", tmpOsAppId.size(), tmpOsId.size());
            osappid_size = tmpOsId.size();
        } else {
            osappid_size = tmpOsAppId.size();
        }

        td.osAppIds.resize(osappid_size);
        // combine OSID and OsAppId if it's needed
        for (size_t i = 0; i < td.osAppIds.size(); i++) {
            memset(&td.osAppIds[i].value[0], 0, MAX_OSID_SIZE + 1 + MAX_OSAPPID_SIZE);

            if (i < tmpOsAppId.size()) {
                memcpy(&td.osAppIds[i].value[MAX_OSID_SIZE], tmpOsAppId[i].value, MAX_OSAPPID_SIZE+1);
            }
            if (i < tmpOsId.size()) {
                memcpy(&td.osAppIds[i].value[0], tmpOsId[i].value, MAX_OSID_SIZE);
            }
        }
    }
}

void SlicingConfigUtil::fillRemoteAddrV4IntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  ipv4 length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        RIL_IpDescriptors &ipd = td.getIpDescriptorsWithInit();
        readField(cnt, (char *)config_data, payload_size);
        for (int i = 0; i < cnt; i++) {
            char t[8] = { 0 };
            char str[100] = { 0 };
            length = copyVariableData(t, (char *)config_data, payload_size);
            inet_ntop(AF_INET, &t[0], str, (socklen_t)sizeof(str));
            ipd.ipv4.push_back(str);
            RilLogV("%d  ipv4:%s", __LINE__, ipd.ipv4[i].c_str());
            inet_ntop(AF_INET, &t[4], str, (socklen_t)sizeof(str));
            ipd.maskV4.push_back(str);
            RilLogV("%d  maskv4:%s", __LINE__, ipd.maskV4[i].c_str());
        }
    }
}

void SlicingConfigUtil::fillRemoteAddrV6IntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  ipv6 length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        RIL_IpDescriptors &ipd = td.getIpDescriptorsWithInit();
        readField(cnt, (char *)config_data, payload_size);
        for (int i = 0; i < cnt; i++) {
            char t[17] = { 0 };
            char str[100] = { 0 };
            length = copyVariableData(t, (char *)config_data, payload_size);
            inet_ntop(AF_INET6, &t[0], str, (socklen_t)sizeof(str));
            ipd.ipv6.push_back(str);
            if (t[16] > 0 && t[16] <= 128)
                ipd.prefixLength.push_back(t[16]);
            else
                ipd.prefixLength.push_back(0);
            RilLogV("%d  ipv6:%s/%d", __LINE__,
                    ipd.ipv6[i].c_str(),
                    ipd.prefixLength[i]);
        }
    }
}

void SlicingConfigUtil::fillProtocolIdNextHeaderIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  ProtocolId length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        RIL_IpDescriptors &ipd = td.getIpDescriptorsWithInit();
        readField(cnt, (char *)config_data, payload_size);
        ipd.protocolId.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            char t[1] = { 0 };
            length = copyVariableData(t, (char *)config_data, payload_size);
            ipd.protocolId[i] = t[0];
        }
    }
}

void SlicingConfigUtil::fillSinglePortIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  SinglePort length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        RIL_IpDescriptors &ipd = td.getIpDescriptorsWithInit();
        readField(cnt, (char *)config_data, payload_size);
        ipd.port.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            char t[2] = { 0 };
            length = copyVariableData(t, (char *)config_data, payload_size);
            ipd.port[i] = *reinterpret_cast<uint16_t *>(&t[0]);
        }
    }
}

void SlicingConfigUtil::fillRemotePortRangeIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  PortRange length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        RIL_IpDescriptors &ipd = td.getIpDescriptorsWithInit();
        readField(cnt, (char *)config_data, payload_size);
        ipd.portStartRange.resize(cnt);
        ipd.portEndRange.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            char t[4] = { 0 };
            length = copyVariableData(t, (char *)config_data, payload_size);
            ipd.portStartRange[i] = *reinterpret_cast<uint16_t *>(&t[0]);
            ipd.portEndRange[i] = *reinterpret_cast<uint16_t *>(&t[2]);
        }
    }
}

void SlicingConfigUtil::fillIpDescriptorsIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    td.ipDescriptors.reset();
    // For IPaddress numeric to canonical
    fillRemoteAddrV4IntoTd(td, config_data, payload_size);
    fillRemoteAddrV6IntoTd(td, config_data, payload_size);
    fillProtocolIdNextHeaderIntoTd(td, config_data, payload_size);
    fillSinglePortIntoTd(td, config_data, payload_size);
    fillRemotePortRangeIntoTd(td, config_data, payload_size);
}

void SlicingConfigUtil::fillIp3TupleIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  Ip3Tuple total length:%d, payload_size:%d", __LINE__, length, payload_size);
    td.ip3Tuple.clear();
    if (length > 0) {
        uint8_t cnt = 0;
        readField(cnt, (char *)config_data, payload_size);
        td.ip3Tuple.resize(cnt);
        uint8_t len = 0;
        // fixed size 33 bytes
        readField(len, (char *)config_data, payload_size);
        for (int i = 0; i < cnt; i++) {
            RilLogV("%d  Ip3Tuple[%d] len:%d, payload_size:%d", __LINE__, i, len, payload_size);
            sit_pdp_ip3tuple t;
            memset(&t, 0, sizeof(t));
            readField(t, (char *)config_data, payload_size);
            if (t.bitmap & 0x1) {
                char str[100] = { 0 };
                inet_ntop(AF_INET, t.ipv4RemoteAddress, str, (socklen_t)sizeof(str));
                td.ip3Tuple[i].ipv4.push_back(str);
                inet_ntop(AF_INET, t.ipv4RemoteAddressMask, str, (socklen_t)sizeof(str));
                td.ip3Tuple[i].maskV4.push_back(str);
            }
            if (t.bitmap & 0x2) {
                char str[100] = { 0 };
                inet_ntop(AF_INET6, t.ipv6RemoteAddress, str, (socklen_t)sizeof(str));
                td.ip3Tuple[i].ipv6.push_back(str);
                td.ip3Tuple[i].prefixLength.push_back(t.prefixLength);
            }
            if (t.bitmap & 0x4) {
                td.ip3Tuple[i].protocolId.push_back(t.protocolIdentifier);
            }
            if (t.bitmap & 0x8) {
                td.ip3Tuple[i].port.push_back(*reinterpret_cast<uint16_t *>(t.singleRemotePort));
            }
            if (t.bitmap & 0x10) {
                td.ip3Tuple[i].portStartRange.push_back(*reinterpret_cast<uint16_t *>(t.remotePortRangeLowLimit));
                td.ip3Tuple[i].portEndRange.push_back(*reinterpret_cast<uint16_t *>(t.remotePortRangeHighLimit));
            }
        }
    }
}

void SlicingConfigUtil::fillTosTcIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    // TosTc & TosTcMask;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d  TOSTC length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        td.tosTc.clear();
        td.tosTcm.clear();
        readField(cnt, (char *)config_data, payload_size);
        td.tosTc.resize(cnt);
        td.tosTcm.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            char t[2] = { 0 };
            length = copyVariableData(t, (char *)config_data, payload_size);
            td.tosTc[i] = t[0];
            td.tosTcm[i] = t[1];
        }
    }
}

void SlicingConfigUtil::fillConnectionCapabilitiesIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    uint16_t length = 0;
    readField(length, (char *)config_data, payload_size);
    RilLogV("%d E ConnectionCapa length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        td.connectionCapabilities.clear();
        readField(cnt, (char *)config_data, payload_size);
        td.connectionCapabilities.resize(cnt);
        for (int i = 0; i < cnt; i++) {
            char t[1] = { 0 };
            std::string &str = td.connectionCapabilities[i].value;
            str = "";
            length = copyVariableData(t, (char *)config_data, payload_size);
            if (t[0] & 0x1) str += "IMS,";
            if (t[0] & 0x2) str += "MMS,";
            if (t[0] & 0x4) str += "SUPL,";
            if (t[0] & 0x8) str += "INTERNET";
            if (str[str.size()-1] == ',') str = str.substr(0, str.size() -1);
        }
    }
    RilLogV("%d X ConnectionCapa length:%d, payload_size:%d", __LINE__, length, payload_size);
    if (!td.connectionCapabilities.empty())
        RilLogV("   ConnectionCapa[0]:%s", td.connectionCapabilities[0].value.c_str());
}
void SlicingConfigUtil::fillDomainDescriptorIntoTd(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    char FQDN[512] = {0};
    uint16_t length = 0;
    int mode = SystemProperty::GetInt(RIL_VENDOR_RADIO_TEST_FQDN_OPT, 1);
    switch (mode & 0x01) {
      case 0:
          // Not verified
          length = copyVariableData(FQDN, (char *)config_data, payload_size);
          RilLogV("%d  FQDN Field length:%d, payload_size:%d", __LINE__, length, payload_size);
          convertFQDN(FQDN);
          td.domainDescriptors[0].value = FQDN;
          break;
      case 1:
          [[fallthrough]];
      default:
          length = copyVariableMultiString(td.domainDescriptors, (char *)config_data, payload_size);
          RilLogV("%d  FQDN String length:%d, payload_size:%d", __LINE__, length, payload_size);
          if (!td.domainDescriptors.empty())
              RilLogV("  FQDN[0] = %s", td.domainDescriptors[0].value.c_str());
          break;
    }
}

void SlicingConfigUtil::fillURSPRuleWithTD(DataV2_0::RIL_TrafficDescriptor &td, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    // payload_size is increased, so calling sequence is very important
    fillDnnIntoTd(td, config_data, payload_size);
    fillOsAppIdIntoTd(td, config_data, payload_size);
    fillIpDescriptorsIntoTd(td, config_data, payload_size);
    fillIp3TupleIntoTd(td, config_data, payload_size);

    uint16_t length = 0;

    copyCastedVariableMultiData((char *)config_data, payload_size, td.securityParameterIndex);
    RilLogV("%d  SPI length:%d, payload_size:%d", __LINE__, length, payload_size);

    fillTosTcIntoTd(td, config_data, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.flowLabel);
    RilLogV("%d  FlowLabel length:%d, payload_size:%d", __LINE__, length, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.destMacAddress);
    RilLogV("%d  MacAddr length:%d, payload_size:%d", __LINE__, length, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.cTagVid);
    RilLogV("%d  CTAGVID length:%d, payload_size:%d", __LINE__, length, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.sTagVid);
    RilLogV("%d  STAGVID length:%d, payload_size:%d", __LINE__, length, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.cTagPcpDei);
    RilLogV("%d  CTAGPCPDEI length:%d, payload_size:%d", __LINE__, length, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.sTagPcpDei);
    RilLogV("%d  STAGPCPDEI length:%d, payload_size:%d", __LINE__, length, payload_size);

    copyCastedVariableMultiData((char *)config_data, payload_size, td.etherType);
    RilLogV("%d  ETHERTYPE length:%d, payload_size:%d", __LINE__, length, payload_size);

    fillConnectionCapabilitiesIntoTd(td, config_data, payload_size);
    fillDomainDescriptorIntoTd(td, config_data, payload_size);

    // matchAllType
    td.matchAllType = *((char *)config_data + payload_size);
    payload_size += sizeof(uint8_t);
    RilLogV("%d  MatchAll = %d, payload_size:%d", __LINE__, td.matchAllType, payload_size);
    // td_data = (sit_pdp_traffic_descriptor *)(((char *)config_data) + payload_size);
}

void SlicingConfigUtil::fillURSPRuleMultiParam(DataV2_0::RIL_SlicingConfig &out, sit_pdp_slicing_config_item *config_data, int &payload_size) {
    RilLogV("%s : config_data %p, payload_size: %d", __func__, config_data, payload_size);

    out.urspRules.clear();

    out.urspNum = config_data->urspNum;
    payload_size += sizeof(config_data->urspNum);
    RilLogV("config_data->urspNum:%p(%d) payload_size:%d", &config_data->urspNum, config_data->urspNum, payload_size);

    sit_pdp_ursp_rules * ursp_data = (sit_pdp_ursp_rules *)(((char *)config_data) + payload_size);
    // RilLogV("ursp_data:%p payload_size:%d", ursp_data, payload_size);

    for (int i = 0; i < out.urspNum; i++) {
        DataV2_0::RIL_UrspRule urspRule = {};
        /*
         * precedence @ [0] byte
         */
        urspRule.precedence = ursp_data->precedence;
        payload_size += sizeof(ursp_data->precedence);
        // RilLogV("ursp->precedence:%p(%d) payload_size:%d", &ursp_data->precedence, ursp_data->precedence, payload_size);

        DataV2_0::RIL_TrafficDescriptor trafficDescriptor = {};
        fillURSPRuleWithTD(trafficDescriptor, config_data, payload_size);
        urspRule.trafficDescriptors.clear();
        urspRule.trafficDescriptors.push_back(trafficDescriptor);

        fillRouteSelectionDescriptor(out, config_data, payload_size, urspRule);

        out.urspRules.push_back(urspRule);
        RilLogV("%d  processed payload_size:%d, urspnum:%d", __LINE__, payload_size, out.urspRules.size());
        // update to next URSP RULE
        ursp_data = (sit_pdp_ursp_rules *)(((char *)config_data) + payload_size);
    }
}

void SlicingConfigUtil::convertFQDN( char (&fqdn)[512] ) {
    int p = 0, t = 0;
    char tmp[512];
    while(fqdn[p]) {
        if ( p == 0 ) { p++; continue; }
        if (fqdn[p] < 'a') {
            fqdn[p] = '.';
        }
        tmp[t++] = fqdn[p++];
    }
    tmp[t] = 0;
    strncpy(fqdn, tmp, strlen(tmp));
}

/*
 * DNN on IPC V1 format, DNN follows TS 23.003 spec definition same to APN.
 * See also: TS 29.571 Release 15 and Release 16, Table 5.3.2-1 and TS 23.003 9A
 * https://www.etsi.org/deliver/etsi_TS/129500_129599/129571/15.01.00_60/ts_129571v150100p.pdf
 * https://www.etsi.org/deliver/etsi_ts/129500_129599/129571/16.04.00_60/ts_129571v160400p.pdf
 * It's not always same to trivial FQDN style using '.' like "ims.server.com"
 * Just Release 16 added requirement for dotted string
 * DNN data from network or TE can follow the format word length + word + word length + word + ...
 * So it can contain "<length>ims<length>server<length>com"
 * From IPC V2, DNN string will be converted in modem side which supports r16
 * to FQDN style with removing first length byte and replacing other length byte to '.'
 */
RIL_OptionalDNN SlicingConfigUtil::getDnnInfoParam(char *  modemData, int & payload_size) {
    int dnnLength = 0;
    RIL_OptionalDNN out;
    memset(&out, 0, sizeof(RIL_OptionalDNN));

    sit_pdp_dnn_info * dnn_data = (sit_pdp_dnn_info *)(((char *)modemData) + payload_size);
    dnnLength = dnn_data->DNN_length;
    memcpy(out.value, &(dnn_data->DNN), dnnLength);
    payload_size += (sizeof(dnn_data->DNN_length) + dnnLength);// (1) + variable

    return out;
}

std::string SlicingConfigUtil::getDnnInfoParam2(char *modemData, int &payload_size) {
    int dnnLength = 0;
    std::string out("");

    sit_pdp_dnn_info * dnn_data = (sit_pdp_dnn_info *)(((char *)modemData) + payload_size);
    dnnLength = dnn_data->DNN_length;
    out.append((char *)&(dnn_data->DNN), 0, dnnLength);
    payload_size += (sizeof(dnn_data->DNN_length) + dnnLength);// (1) + variable

    return out;
}


std::vector<RIL_SliceInfo> SlicingConfigUtil::getSliceInfoParam(char *modemData, int &payload_size) {
    RIL_SliceInfo SliceInfo;
    std::vector<RIL_SliceInfo> out;
    out.clear();

    uint8_t SliceNum = *(((char *)modemData) + payload_size);
    payload_size += sizeof(char);
    sit_pdp_slice_info *slice_info_data = (sit_pdp_slice_info *)(((char *)modemData) + payload_size);

    for (int i = 0; i < SliceNum; i++) {
        memset(&SliceInfo, 0, sizeof(RIL_SliceInfo));
        //NSSAI_TYPE
        SliceInfo.sst = (RIL_SliceServiceType)slice_info_data->SliceInfoSst;
        SliceInfo.sliceDifferentiator = slice_info_data->SliceInfoSd;
        SliceInfo.mappedHplmnSst = (RIL_SliceServiceType)slice_info_data->SliceMappedHplmnSst;
        SliceInfo.mappedHplmnSD = slice_info_data->SliceMappedHplmnSd;
        SliceInfo.status = (RIL_SliceStatus)slice_info_data->status;

        //push_back
        payload_size += sizeof(sit_pdp_slice_info);
        slice_info_data = (sit_pdp_slice_info *)((char *)modemData + payload_size);
        out.push_back(SliceInfo);
    }

    return out;
}

void SlicingConfigUtil::fillLocationCriteriaTypeInfo(RIL_RouteSelectionDescriptor &rsdout, char *modemData, int &payload_size) {
    RIL_EutraCellIdentities eutra;
    RIL_NrCellIdentities nr;
    RIL_GlobalRanNodeIdentities grn;

    uint16_t length = *(((char *)(modemData)) + payload_size);
    payload_size += sizeof(uint16_t);
    uint16_t processed = 0;

    uint8_t eutra_len = *(((char *)(modemData)) + payload_size);
    payload_size += sizeof(uint8_t);
    processed += sizeof(uint8_t);
    for(int i = 0; i < eutra_len; i++) {
        memcpy(eutra.value, (char *)modemData + payload_size, sizeof(eutra.value));
        payload_size += sizeof(eutra.value);
        processed += sizeof(eutra.value);
        rsdout.eutraCellIdentities.push_back(eutra);
    }

    uint8_t nr_len = *(((char *)(modemData)) + payload_size);
    payload_size += sizeof(uint8_t);
    processed += sizeof(uint8_t);
    for(int i = 0; i < nr_len; i++) {
        memcpy(nr.value, (char *)modemData + payload_size, sizeof(nr.value));
        payload_size += sizeof(nr.value);
        processed += sizeof(nr.value);
        rsdout.nrCellIdentities.push_back(nr);
    }

    uint8_t grn_len = *(((char *)(modemData)) + payload_size);
    payload_size += sizeof(uint8_t);
    processed += sizeof(uint8_t);
    for(int i = 0; i < grn_len; i++) {
        memcpy(grn.value, (char *)modemData + payload_size, sizeof(grn.value));
        payload_size += sizeof(grn.value);
        processed += sizeof(grn.value);
        rsdout.globalRanNodeIdentities.push_back(grn);
    }

    uint8_t tailist_len = *(((char *)(modemData)) + payload_size);
    payload_size += sizeof(uint8_t);
    processed += sizeof(uint8_t);
    for(int i=0; i<tailist_len; i++) {
        rsdout.taiList.push_back(*((char *)(modemData) + payload_size));
        payload_size++;
        processed++;
    }
    if (length != processed)
        RilLogW("processed bytes is different to length len=%d, processed=%d", length, processed);
}

void SlicingConfigUtil::SetPayloadSize(const int size)
{
    m_payload_size = size;
}

template<typename T>
void SlicingConfigUtil::readField(T &ret, char *sp, int &payload_size) {
    T v = * (T *) &(sp + payload_size)[0];
    // RilLogV("readField : ps=%d", payload_size);
    payload_size += sizeof(T);
    // RilLogV("readField : sizeof(T)=%d, ps=%d", sizeof(T), payload_size);
    ret = v;
}

// Assuming one byte length field @ [0] position
template<typename T>
int SlicingConfigUtil::copyVariableData(T *ret, char *sp, int &payload_size) {
    uint8_t length = ((uint8_t *)sp + payload_size)[0];
    payload_size += sizeof(BYTE);
    if (length > 0) {
        memcpy(ret, (sp + payload_size), sizeof(BYTE) * length);
        payload_size += length; // parameter length + length field(1 byte)
        // RilLogV("copyVariable: t %p, sp:%p (%x, %x), payload_size:%d, length:%d", ret, sp, sp[0], sp[1], payload_size, length);
    }
    return length;
}

// Assuming two byte length field @ [0] position
// Then one byte cnt field @ [2] position, if length > 0
// Then multiple data field will be present
template<typename T>
uint16_t SlicingConfigUtil::copyVariableMultiData(vector<T> &ret, char *sp, int &payload_size, bool includeLen) {
    uint16_t length = 0;
    readField(length, sp, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        readField(cnt, sp, payload_size);
        if (cnt > 0) {
            ret.resize(cnt);
            RilLogV(" cnt :%d", cnt);
            for (int i = 0; i < cnt; i++) {
                uint8_t len = 0;
                readField(len, sp, payload_size);
                RilLogV(" len[%d]=%d", i, len);
                memset(ret[i].value, 0, sizeof(ret[i].value));
                if (includeLen) {
                    ret[i].value[0] = len;
                    memcpy(&ret[i].value[1], (sp + payload_size), sizeof(BYTE) * len);
                    if (len + 1 < sizeof(ret[i].value)) {
                        ret[i].value[len + 1] = 0;
                    } else {
                        RilLogV("length is full, so no processing null termination as string");
                    }
                } else {
                    memcpy(ret[i].value, (sp + payload_size), sizeof(BYTE) * len);
                    if (len < sizeof(ret[i].value)) {
                        ret[i].value[len] = 0;
                    } else {
                        RilLogV("length is full, so no processing null termination as string");
                    }
                }
                payload_size += len; // one item length
            }
        }
    }
    return length;
}

int SlicingConfigUtil::copyVariableString(std::string &ret, char *sp, int &payload_size) {
    char t[256] = { 0 };
    uint8_t length = ((uint8_t *)sp + payload_size)[0];
    payload_size += sizeof(BYTE);
    if (length > 0) {
        memcpy(t, sp + payload_size, sizeof(char) * length);
        ret = t;
        payload_size += length; // parameter length + length field(1 byte)
    }
    return length;
}

template<typename T>
uint16_t SlicingConfigUtil::copyVariableMultiString(std::vector<T> &ret, char *sp, int &payload_size) {
    uint16_t length = 0;
    readField(length, sp, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        readField(cnt, sp, payload_size);
        if (cnt > 0) {
            ret.resize(cnt);
            for (int i = 0; i < cnt; i++) {
                uint8_t len = 0;
                char t[256] = { 0 };
                readField(len, sp, payload_size);
                memcpy(t, sp + payload_size, sizeof(char) * len);
                t[len] = 0;
                ret[i].value = t;
                payload_size += len; // one item length
            }
        }
    }
    return length;
}

uint16_t SlicingConfigUtil::copyVariableMultiString(std::vector<std::string> &ret, char *sp, int &payload_size) {
    uint16_t length = 0;
    readField(length, sp, payload_size);
    if (length > 0) {
        uint8_t cnt = 0;
        readField(cnt, sp, payload_size);
        if (cnt > 0) {
            ret.resize(cnt);
            for (int i = 0; i < cnt; i++) {
                uint8_t len = 0;
                char t[256] = { 0 };
                readField(len, sp, payload_size);
                memcpy(t, sp + payload_size, sizeof(char) * len);
                t[len] = 0;
                ret[i] = t;
                payload_size += len; // one item length
            }
        }
    }
    return length;
}

template<typename T>
void SlicingConfigUtil::copyCastedVariableData(char *sp, int &payload_size, T &var,
                                               RIL_OptionalInfoFlag &flag) {
    char t[255] = { 0 };
    flag = RIL_OPT_INFO_INVALID;
    int length = copyVariableData(t, sp, payload_size);
    if (length > 0) {
        flag = RIL_OPT_INFO_VALID;
        var = *reinterpret_cast<T *>(&t[0]);
    }
}

template<typename T>
void SlicingConfigUtil::copyCastedVariableMultiData(char *sp, int &payload_size, std::vector<T> &var) {
    uint16_t length = 0;
    uint8_t cnt = 0;

    readField(length, sp, payload_size);
    if (length > 0) {
        readField(cnt, sp, payload_size);
        if (cnt > 0) {
            var.resize(cnt);
            for (int i = 0; i < cnt; i++) {
                uint8_t len = 0;
                T t;
                readField(len, sp, payload_size);
                readField(t, sp, payload_size);
                var[i] = t;
            }
        }
    }
}

template<typename T>
void SlicingConfigUtil::copyByteArrayData(char *sp, int &payload_size, T *var,
                                          RIL_OptionalInfoFlag &flag) {
    char t[255] = { 0 };

    flag = RIL_OPT_INFO_INVALID;
    int length = copyVariableData(t, sp, payload_size);
    int limit = sizeof(T);
    if (length > 0) {
        flag = RIL_OPT_INFO_VALID;
        memcpy(var, t, (length > limit) ? limit : length);
    }
}

SlicingConfigUtil::SlicingConfigUtil() {}

SlicingConfigUtil::~SlicingConfigUtil() {}
