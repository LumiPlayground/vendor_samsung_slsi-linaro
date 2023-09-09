/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsTestHandler.h"
#include "rillog.h"
#include "psservice.h"
#include "protocolpsadapter.h"

PsTestHandler::PsTestHandler()
{
    m_pPsService = NULL;
}

PsTestHandler::~PsTestHandler()
{
}

void PsTestHandler::Init(PsService* pPsService)
{
    if(pPsService != NULL)
        m_pPsService = pPsService;
}

ModemData *PsTestHandler::replaceModemDataForTest(ModemData *pModemData, int type)
{
    // bypass, Don't manipulate ModemData
    if (!m_bTestNewIPCFormat) return pModemData;

    unsigned char ext_payload[][102] = {
      { 0x02, 0x00,
        198, 162, 5, 1,
        198, 162, 15, 12,
        198, 162, 25, 23,
        198, 162, 35, 34,
        198, 162, 45, 45,
        0x20, 0x02, 0x02, 0xd8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0,

        0x20, 0x02, 0x02, 0xd8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xe0,

        0x20, 0x02, 0x02, 0xd8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xd0,

        0x20, 0x02, 0x02, 0xd8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xc0,

        0x20, 0x02, 0x02, 0xd8, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xb0},
      { 0x02, 0x00,
        190, 112, 105, 101,
        190, 112, 115, 112,
        190, 112, 125, 123,
        190, 112, 135, 134,
        190, 112, 145, 145,
        0x20, 0x02, 0x02, 0xd8, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x01, 0xf0,

        0x20, 0x02, 0x02, 0xd8, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x02, 0xe0,

        0x20, 0x02, 0x02, 0xd8, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x03, 0xd0,

        0x20, 0x02, 0x02, 0xd8, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x04, 0xc0,

        0x20, 0x02, 0x02, 0xd8, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x05, 0xb0},
    };

    bool isTx = pModemData->IsTx();
    const size_t old_payload_length = pModemData->GetLength();
    const size_t DATACALL_EXT_SIZE = sizeof(sit_pdp_data_call_item_pcscf_ext);
    //const size_t NEW_DATACALL_SIZE = DATACALL_EXT_SIZE + sizeof(sit_pdp_data_call_item);
    size_t new_payload_length = 0;
    char *new_payload = NULL;

    RilLogV("=-=- Dump Original ModemData @ %p", pModemData->GetRawData());
    pModemData->Dump();

    switch(type) {
      case 0: // SETUP_DATA_CALL_RSP
          {
              const sit_pdp_setup_data_call_rsp *data = reinterpret_cast<const sit_pdp_setup_data_call_rsp *>(pModemData->GetRawData());
              new_payload_length = old_payload_length + DATACALL_EXT_SIZE;
              new_payload = new char[new_payload_length];

              memcpy(new_payload, data, old_payload_length);
              memcpy(new_payload + old_payload_length, ext_payload[0], DATACALL_EXT_SIZE);

              RilLogV("=-=- Original Data Copy from %p to %p with %d bytes", data, new_payload, old_payload_length);
              RilLogV("=-=- Test Vector Copy from %p to %p with %d bytes", ext_payload[0], new_payload + old_payload_length, DATACALL_EXT_SIZE);
              RilLogV("=-=- ext_payload[0]:%p, ext_payload[1]:%p", ext_payload[0], ext_payload[1]);
              //RilLogV("Dump PCSCF Extension Payload - Test Vector");
              //DumpPcscfExtPayload(ext_payload[0]);
          }
          break;
      case 1: // GET_DATA_CALL_LIST_RSP
          {
              const sit_pdp_get_data_call_list_rsp *data = reinterpret_cast<const sit_pdp_get_data_call_list_rsp *>(pModemData->GetRawData());
              const size_t datacall_num = data->datacall_info_num;
              const size_t hLen = sizeof(RCM_HEADER) + 1;
              const size_t old_datacall_size = (old_payload_length - hLen) / datacall_num;
              new_payload_length = old_payload_length + DATACALL_EXT_SIZE * datacall_num;
              new_payload = new char[new_payload_length];

              memcpy(new_payload, data, hLen);
              for(size_t i=0; i < datacall_num; i++) {
                  memcpy(new_payload + (old_datacall_size + DATACALL_EXT_SIZE) * i, (char *)data + hLen + old_datacall_size * i, old_datacall_size);
                  memcpy(new_payload + (old_datacall_size + DATACALL_EXT_SIZE) * i + old_datacall_size, ext_payload[i%2], DATACALL_EXT_SIZE);
              }
          }
          break;
      case 2: // DATA_CALL_LIST_CHANGED_IND
          {
              const sit_pdp_data_call_list_changed_ind *data = reinterpret_cast<const sit_pdp_data_call_list_changed_ind *>(pModemData->GetRawData());
              const size_t datacall_num = data->datacall_info_num;
              const size_t hLen = sizeof(RCM_IND_HEADER) + 1;
              const size_t old_datacall_size = (old_payload_length - hLen) / datacall_num;
              new_payload_length = old_payload_length + DATACALL_EXT_SIZE * datacall_num;
              new_payload = new char[new_payload_length];

              //DumpBuf((char *)data, old_payload_length);
              memcpy(new_payload, data, hLen);
              for(size_t i=0; i < datacall_num ; i++) {
                  memcpy(new_payload + hLen + (old_datacall_size + DATACALL_EXT_SIZE) * i,
                         (char *)data + hLen + old_datacall_size * i, old_datacall_size);
                  memcpy(new_payload + hLen + (old_datacall_size + DATACALL_EXT_SIZE) * i + old_datacall_size,
                         ext_payload[i%2], DATACALL_EXT_SIZE);
                  RilLogV("=-=- DataCallNum:%d, hLen:%d", i, hLen);
                  RilLogV("=-=- Original Data Copy from %p to %p with %d bytes", data + hLen + old_datacall_size * i,
                          new_payload + hLen + (old_datacall_size + DATACALL_EXT_SIZE) * i, old_datacall_size);
                  RilLogV("=-=- Test Vector Copy from %p to %p with %d bytes", ext_payload[i%2],
                          new_payload + hLen + (old_datacall_size + DATACALL_EXT_SIZE) * i + old_datacall_size, DATACALL_EXT_SIZE);
                  RilLogV("=-=- ext_payload[0]:%p, ext_payload[1]:%p", ext_payload[0], ext_payload[1]);
              }
          }
          break;
    }

    ModemData *newModemData = NULL;
    if(new_payload != NULL) {
        if(new_payload_length > 0) {
            newModemData = new ModemData(new_payload, new_payload_length, isTx);
            delete[] new_payload;
        } else {
            RilLogV("=-=- Invalid payload length");
            delete[] new_payload;
            return pModemData;
        }
    } else {
        RilLogV("=-=- Invalid payload");
        return pModemData;
    }

    RilLogV("=-=- Original ModemData Length:%d ", old_payload_length);
    RilLogV("=-=- AddedLength:%d, isTx:%u", sizeof(sit_pdp_data_call_item_pcscf_ext), isTx);
    RilLogV("=-=- New ModemData @ %p, Length:%d", newModemData->GetRawData(), newModemData->GetLength());

    RilLogV("=-=- Dump New ModemData - Test Vector is added");
    newModemData->Dump();
    RilLogV("=-=- exit %s",  __func__);
    delete pModemData;
    return newModemData;
    //delete newModemData; // When delete? NO. This is Test code.
}

int PsTestHandler::forceFailOnceForTest(int errorCode)
{
    if (m_bForceFailAfterSuccess) {
        errorCode = RIL_E_GENERIC_FAILURE;
        m_bForceFailAfterSuccess = false;
    }
    return errorCode;
}


template<typename T>
int PsTestHandler::fillTestPayload(char *(&new_payload), T *header, const char *testdata, int len, bool &isTx) {
    header->length = len;
    int payload_length = sizeof(T) + len;
    new_payload = new char[payload_length];
    isTx = false;

    memcpy(new_payload, header, sizeof(T));
    memcpy(new_payload + sizeof(T), testdata, len);
    return payload_length;
}

ModemData *PsTestHandler::generateModemDataForTest(int type) {
    char *new_payload = NULL;
    int payload_length = 0;
    bool isTx = false;
    RilLogV("=-=- %s Test type : %d", __func__, type);
    RCM_HEADER rsp_header = {
        .type = RCM_TYPE_RESPONSE,
        .reserved = 0,
        .id = SIT_GET_SLICING_CONFIG,
        .length = 0, // to be filled
    };
    switch(type) {
        // GET_SLICING_CONFIG_RSP
      case 3:  // one ursp TD with
          {
              const char testdata[] = {
                  1,  // slicing_config_version
                  0,  // urspNum
                  0,  // num_of_slice
              };

              payload_length = fillTestPayload(new_payload, &rsp_header, testdata, sizeof(testdata), isTx);
          }
          break;
      case 4:  // one ursp TD with
          {
              const char testdata[] = {
                  2,  // slicing_config_version
                  1,  // urspNum
                  1,  // urspRules.precedence
                  9,  // urspRules.TD.dnnlength
                  8, 'i','n','t','e','r','n','e','t', // Spec include length
                  0,  // urspRules.TD.os_id_length
                  0,  // urspRules.TD.os_app_id_length
                  0,  // urspRules.TD.ipv4_remote_addr_len
                  0,  // urspRules.TD.ipv6_remote_addr_len
                  0,  // urspRules.TD.protocol_id_next_header_len
                  0,  // urspRules.TD.single_port_len
                  0,  // urspRules.TD.remote_port_range_len
                  0,  // urspRules.TD.security_param_idx_len
                  0,  // urspRules.TD.tos_tc_len
                  0,  // urspRules.TD.flow_label_len
                  0,  // urspRules.TD.mac_addr_len
                  0,  // urspRules.TD.c_tag_vid_len
                  0,  // urspRules.TD.s_tag_vid_len
                  0,  // urspRules.TD.c_tag_pcp_dei_len
                  0,  // urspRules.TD.s_tag_pcp_dei_len
                  0,  // urspRules.TD.ethertype_len
                  0,  // urspRules.TD.connection_capa_len
                  0,  // urspRules.TD.fqdn_len
                  1,  // urspRules.TD.match_all_type
                  0,  // urslRules.num_of_rsd
                  0,  // num_of_slice
              };

              payload_length = fillTestPayload(new_payload, &rsp_header, testdata, sizeof(testdata), isTx);
          }
          break;
      case 5:  // one ursp TD with
          {
              const char testdata[] = {
                  2,  // slicing_config_version
                  1,  // urspNum
                  2,  // urspRules.precedence
                  9,  // urspRules.TD.dnnlength
                  8, 'i','n','t','e','r','n','e','t', // Spec include length
                  16,  // urspRules.TD.os_id_length
                  0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
                  0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
                  7,  // urspRules.TD.os_app_id_length
                  'a','n','d','r','o','i','d',
                  0,  // urspRules.TD.ipv4_remote_addr_len
                  0,  // urspRules.TD.ipv6_remote_addr_len
                  0,  // urspRules.TD.protocol_id_next_header_len
                  0,  // urspRules.TD.single_port_len
                  0,  // urspRules.TD.remote_port_range_len
                  0,  // urspRules.TD.security_param_idx_len
                  0,  // urspRules.TD.tos_tc_len
                  0,  // urspRules.TD.flow_label_len
                  0,  // urspRules.TD.mac_addr_len
                  0,  // urspRules.TD.c_tag_vid_len
                  0,  // urspRules.TD.s_tag_vid_len
                  0,  // urspRules.TD.c_tag_pcp_dei_len
                  0,  // urspRules.TD.s_tag_pcp_dei_len
                  0,  // urspRules.TD.ethertype_len
                  0,  // urspRules.TD.connection_capa_len
                  0,  // urspRules.TD.fqdn_len
                  1,  // urspRules.TD.match_all_type
                  0,  // urslRules.num_of_rsd
                  0,  // num_of_slice
              };

              payload_length = fillTestPayload(new_payload, &rsp_header, testdata, sizeof(testdata), isTx);
          }
          break;
      case 6:  // one ursp TD with
          {
              const char testdata[] = {
                  2,  // slicing_config_version
                  1,  // urspNum
                  3,  // urspRules.precedence
                  9,  // urspRules.TD.dnnlength
                  8, 'i','n','t','e','r','n','e','t', // Spec include length
                  0,  // urspRules.TD.os_id_length
                  0,  // urspRules.TD.os_app_id_length
                  8,  // urspRules.TD.ipv4_remote_addr_len
                  0x12,0x34,0x56,0x78,
                  0xff,0xff,0xff,0x00,
                  0,  // urspRules.TD.ipv6_remote_addr_len
                  0,  // urspRules.TD.protocol_id_next_header_len
                  0,  // urspRules.TD.single_port_len
                  0,  // urspRules.TD.remote_port_range_len
                  0,  // urspRules.TD.security_param_idx_len
                  0,  // urspRules.TD.tos_tc_len
                  0,  // urspRules.TD.flow_label_len
                  0,  // urspRules.TD.mac_addr_len
                  0,  // urspRules.TD.c_tag_vid_len
                  0,  // urspRules.TD.s_tag_vid_len
                  0,  // urspRules.TD.c_tag_pcp_dei_len
                  0,  // urspRules.TD.s_tag_pcp_dei_len
                  0,  // urspRules.TD.ethertype_len
                  0,  // urspRules.TD.connection_capa_len
                  0,  // urspRules.TD.fqdn_len
                  1,  // urspRules.TD.match_all_type
                  0,  // urslRules.num_of_rsd
                  0,  // num_of_slice
              };

              payload_length = fillTestPayload(new_payload, &rsp_header, testdata, sizeof(testdata), isTx);
          }
          break;

      case 7:  // one ursp TD with
          {
              const char testdata[] = {
                  2,  // slicing_config_version
                  1,  // urspNum
                  4,  // urspRules.precedence
                  9,  // urspRules.TD.dnnlength
                  8, 'i','n','t','e','r','n','e','t',
                  0,  // urspRules.TD.os_id_length
                  0,  // urspRules.TD.os_app_id_length
                  0,  // urspRules.TD.ipv4_remote_addr_len
                  17,  // urspRules.TD.ipv6_remote_addr_len
                  0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
                  0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
                  128,
                  0,  // urspRules.TD.protocol_id_next_header_len
                  0,  // urspRules.TD.single_port_len
                  0,  // urspRules.TD.remote_port_range_len
                  0,  // urspRules.TD.security_param_idx_len
                  0,  // urspRules.TD.tos_tc_len
                  0,  // urspRules.TD.flow_label_len
                  0,  // urspRules.TD.mac_addr_len
                  0,  // urspRules.TD.c_tag_vid_len
                  0,  // urspRules.TD.s_tag_vid_len
                  0,  // urspRules.TD.c_tag_pcp_dei_len
                  0,  // urspRules.TD.s_tag_pcp_dei_len
                  0,  // urspRules.TD.ethertype_len
                  0,  // urspRules.TD.connection_capa_len
                  0,  // urspRules.TD.fqdn_len
                  1,  // urspRules.TD.match_all_type
                  0,  // urslRules.num_of_rsd
                  0,  // num_of_slice
              };

              payload_length = fillTestPayload(new_payload, &rsp_header, testdata, sizeof(testdata), isTx);
          }
          break;
      case 8:  // one ursp TD with
          {
              const char testdata[] = {
                  2,  // slicing_config_version
                  1,  // urspNum
                  4,  // urspRules.precedence
                  9,  // urspRules.TD.dnnlength
                  8, 'i','n','t','e','r','n','e','t',
                  0,  // urspRules.TD.os_id_length
                  0,  // urspRules.TD.os_app_id_length
                  0,  // urspRules.TD.ipv4_remote_addr_len
                  17,  // urspRules.TD.ipv6_remote_addr_len
                  0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
                  0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,



                  128,
                  0,  // urspRules.TD.protocol_id_next_header_len
                  2,  // urspRules.TD.single_port_len
                  0x10,0x20,
                  0,  // urspRules.TD.remote_port_range_len
                  0,  // urspRules.TD.security_param_idx_len
                  0,  // urspRules.TD.tos_tc_len
                  0,  // urspRules.TD.flow_label_len
                  0,  // urspRules.TD.mac_addr_len
                  0,  // urspRules.TD.c_tag_vid_len
                  0,  // urspRules.TD.s_tag_vid_len
                  0,  // urspRules.TD.c_tag_pcp_dei_len
                  0,  // urspRules.TD.s_tag_pcp_dei_len
                  0,  // urspRules.TD.ethertype_len
                  1,  // urspRules.TD.connection_capa_len
                  0xF1,
                  15, // urspRules.TD.fqdn_len
                  'w','w','w','.','a','n','d','r','o','i','d','.','c','o','m',
                  1,  // urspRules.TD.match_all_type
                  0,  // urslRules.num_of_rsd
                  0,  // num_of_slice
              };

              payload_length = fillTestPayload(new_payload, &rsp_header, testdata, sizeof(testdata), isTx);
          }
          break;
    }

    ModemData *newModemData = NULL;
    if(new_payload != NULL) {
        if(payload_length > 0) {
            newModemData = new ModemData(new_payload, payload_length, isTx);
            delete[] new_payload;
        } else {
            RilLogV("=-=- Invalid payload length");
            delete[] new_payload;
            return NULL;
        }
    } else {
        RilLogV("=-=- Invalid payload");
        return NULL;
    }

    RilLogV("=-=- New ModemData @ %p, Length:%d", newModemData->GetRawData(), newModemData->GetLength());

    RilLogV("=-=- Dump New ModemData - Test Vector is added");
    newModemData->Dump();
    RilLogV("=-=- exit %s",  __func__);
    return newModemData;
    //delete newModemData; // When delete? NO. This is Test code.
}
