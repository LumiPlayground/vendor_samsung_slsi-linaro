/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsUtil.h"
#include <arpa/inet.h>
#include <rildef.h>
#include <sitdef.h>
#include "rillog.h"

namespace PsUtil {

void DumpBuf(char *data, size_t tlen)
{
    RilLogV("=-=- Dump Buf @ %p", data);
    char buf[100];
    size_t len = 0;
    for(size_t i=0,j=0; i<tlen; i+=16) {
        len = 0;
        for(j=0; j<16; j++) {
            if(i+j == tlen) break;
            len += snprintf(&buf[len], 100-len, "%02x", data[i+j]);
        }
        buf[len]='\0';
        RilLogV("Dump 16bytes(idx=%d~%d): %s", i, i+j, buf);
    }
}

void DumpPcscfExtPayload(char *data)
{
    char buf[100];
    size_t len = 0;
    DumpBuf(data, sizeof(sit_pdp_data_call_item_pcscf_ext));
    for(size_t i=0, j=0; i<5; i++) {
        len = 0;
        for(j=0; j<4; j++) {
            len += snprintf(&buf[len], 100-len, "%02x", data[2 + j + 4*i]);
        }
        buf[len]='\0';
        RilLogV("IPv4[%d,%d]: %s", i, 2 + j + 4*i, buf);
        len = 0;
        for(j=0; j<16; j++) {
            len += snprintf(&buf[len], 100-len, "%02x", data[2 + 4*5 + j + 16*i]);
        }
        buf[len]='\0';
        RilLogV("IPv6[%d,%d]: %s", i, 2 + 4*5 + j + 16*i, buf);
    }
}


void PrintDataCallInfo(const DataCall *dc, const char * /* calling_service */, const char *calling_func, const int halVer) {
    int cid = dc->cid;
    int status = dc->status;
    int active = dc->active;
    int mtu_v4 = dc->ipv4_mtu_size;
    int mtu_v6 = dc->ipv6_mtu_size;
    int pco = dc->pco;
    int64_t suggestedRetryTime = dc->suggestedRetryTime;

    RilLogV("===================================================");
    RilLogV("  %s", calling_func);
    RilLogV("  CID=%d", cid);
    RilLogV("  Status=0x%X", status);
    RilLogV("  Active=%s(%d)", active ? "ACTIVE" : "INACTIVE", active);
    RilLogV("  MTU=%d, PCO=%d", mtu_v4, pco);
    if(halVer >= HAL_VERSION_CODE(1, 6)) {
        RilLogV("  MTU_v6=%d", mtu_v6);
    }
    RilLogV("  suggestedRetryTime=%lld", suggestedRetryTime);
    RilLogV("===================================================");
}

void PrintRilDataInfo(const RIL_SetupDataCallResult_V1_6 *rilDataCall, const char *caller)
{
    int cid = rilDataCall->cid;
    int cause = rilDataCall->cause;
    int active = rilDataCall->active;
    int mtuV4 = rilDataCall->mtuV4;
    int mtuV6 = rilDataCall->mtuV6;
    int64_t suggestedRetryTime = rilDataCall->suggestedRetryTime;

    RilLogV("===================================================");
    RilLogV("  %s", caller);
    RilLogV("  CID=%d", cid);
    RilLogV("  Cause=0x%X", cause);
    RilLogV("  Active=%s(%d)", active ? "ACTIVE" : "INACTIVE", active);
    RilLogV("  MTUV4=%d, MTUV6=%d", mtuV4, mtuV6);
    RilLogV("  suggestedRetryTime=%lld", suggestedRetryTime);
    RilLogV("===================================================");
}

void PrintRilDataInfo(const RIL_Data_Call_Response_v11 *rilDataCall, const char *caller)
{
    int cid = rilDataCall->cid;
    int status = rilDataCall->status;
    int active = rilDataCall->active;
    int mtu = rilDataCall->mtu;
    int32_t suggestedRetryTime = rilDataCall->suggestedRetryTime;

    RilLogV("===================================================");
    RilLogV("  %s", caller);
    RilLogV("  CID=%d", cid);
    RilLogV("  Status=0x%X", status);
    RilLogV("  Active=%s(%d)", active ? "ACTIVE" : "INACTIVE", active);
    RilLogV("  MTU=%d", mtu);
    RilLogV("  suggestedRetryTime=%d", suggestedRetryTime);
    RilLogV("===================================================");
}

void decodeAMBR(const DataCall *out)
{
    int up = 0, down = 0;
    // Down, Evaluate first octet 3, 5
    switch(out->ambr.octet2)
    {
      case 6:
        // Use Extended value, ignore octet3,4
        if(out->ambr.octet5 != 0x00) {
            if( out->ambr.octet5 <= 0x4a) {
                down = 8600 + out->ambr.octet5 * 100;
            } else if( out->ambr.octet5 <= 0xba) {
                down = 16000 + (out->ambr.octet5 - 0x4a) * 1000;
            } else if( out->ambr.octet5 <= 0xfa) {
                down = 128000 + (out->ambr.octet5 - 0xba) * 2000;
            } else {
                down = 128000 + 0xfa * 2000;
            }
            break;
        }
        [[fallthrough]];
      case 4:
        if ( out->ambr.octet3 <= 0x3f ) {
            down = out->ambr.octet3;
        } else if( out->ambr.octet3 <= 0x7f ) {
            down = 64 + (out->ambr.octet3 - 0x40) * 8;
        } else if( out->ambr.octet3 <= 0xfe ) {
            down = 576 + (out->ambr.octet3 - 0x80) * 64;
        } else { // 0xFF
            down = 0;
        }
        break;
    }
    // Up
    switch(out->ambr.octet2) {
      case 6:
        if ( out->ambr.octet6 != 0x00 ) {
            if( out->ambr.octet6 <= 0x4a ) {
                up = 8600 + out->ambr.octet6 * 100;
            } else if( out->ambr.octet6 <= 0xba ) {
                up = 16000 + (out->ambr.octet6 - 0x4a) * 1000;
            } else if( out->ambr.octet6 <= 0xfa ) {
                up = 128000 + (out->ambr.octet6 - 0xba) * 2000;
            } else {
                up = 128000 + 0xfa * 2000;
            }
            break;
        }
        [[fallthrough]];
      case 4:
        if ( out->ambr.octet4 <= 0x3f ) {
            up = out->ambr.octet4;
        } else if( out->ambr.octet4 <= 0x7f ) {
            up = 64 + (out->ambr.octet4 - 0x40) * 8;
        } else if( out->ambr.octet4 <= 0xfe ) {
            up = 576 + (out->ambr.octet4 - 0x80) * 64;
        } else { // 0xFF
            up = 0;
        }
        break;
    }
    // Down octet 7, Up octet 8
    if (out->ambr.octet2 == 8){
        if ( out->ambr.octet7 > 0x00 && out->ambr.octet7 < 0xff ) {
            down += out->ambr.octet7 * 256000;
        }
        if ( out->ambr.octet8 > 0x00 && out->ambr.octet8 < 0xff ) {
            up += out->ambr.octet8 * 256000;
        }
    }

    RilLogV("CID:%d, Downlink %d kbps, Uplink %d kbps", out->cid, down, up);
}


void PrintAddressInfo(const DataCall *pDc)
{
    if (pDc != NULL) {
        char szAddr[100] = { 0, };
        char szDns1[100] = { 0, };
        char szDns2[100] = { 0, };
        char szPcscf[100] = { 0, };
        char szGWAddr[100] = { 0, };

        RilLogV("***** IPv4 *****");
        if (pDc->ipv4.valid) {
            inet_ntop(AF_INET, pDc->ipv4.addr, szAddr, (socklen_t)sizeof(szAddr));
            inet_ntop(AF_INET, pDc->ipv4.dns1, szDns1, (socklen_t)sizeof(szDns1));
            inet_ntop(AF_INET, pDc->ipv4.dns2, szDns2, (socklen_t)sizeof(szDns2));
        }
        RilLogV("IP      : %s", szAddr);
        RilLogV("DNS1    : %s", szDns1);
        RilLogV("DNS2    : %s", szDns2);

        RilLogV("DataCall IPC Version is %d", pDc->IPC_version);
        RilLogV("PCSCF_EXT count is %d", pDc->pcscf_ext_count);

        int pcscf_count = MAX_PCSCF_NUM + pDc->pcscf_ext_count;

        for(int i = 0; i < pcscf_count; i++) {
            *szPcscf = 0;
            if(pDc->ipv4.valid) {
                inet_ntop(AF_INET, &pDc->ipv4.pcscf[i * MAX_IPV4_ADDR_LEN], szPcscf, (socklen_t)sizeof(szPcscf));
            }
            RilLogV("P-CSCF%1d  : %s", i + 1, szPcscf);
        }

        RilLogV("****************");

        *szAddr = 0;
        *szDns1 = 0;
        *szDns2 = 0;
        *szGWAddr = 0;
        if (pDc->ipv6.valid) {
            inet_ntop(AF_INET6, pDc->ipv6.addr, szAddr, (socklen_t)sizeof(szAddr));
            inet_ntop(AF_INET6, pDc->ipv6.dns1, szDns1, (socklen_t)sizeof(szDns1));
            inet_ntop(AF_INET6, pDc->ipv6.dns2, szDns2, (socklen_t)sizeof(szDns2));
            inet_ntop(AF_INET6, pDc->ipv6.gw, szGWAddr, (socklen_t)sizeof(szGWAddr));
        }
        RilLogV("***** IPv6 *****");
        RilLogV("IP      : %s", szAddr);
        RilLogV("DNS1    : %s", szDns1);
        RilLogV("DNS2    : %s", szDns2);

        for(int i = 0; i < pcscf_count; i++) {
            *szPcscf = 0;
            if(pDc->ipv6.valid) {
                inet_ntop(AF_INET6, &pDc->ipv6.pcscf[i * MAX_IPV6_ADDR_LEN], szPcscf, (socklen_t)sizeof(szPcscf));
            }
            RilLogV("P-CSCF%1d  : %s", i + 1, szPcscf);
        }

        RilLogV("****************");
    }
}

}  // namespace PsUtil
