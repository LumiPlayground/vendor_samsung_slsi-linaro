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
 * protocolpsutil.h
 *
 */

#ifndef __PROTOCOL_PS_UTIL_H__
#define __PROTOCOL_PS_UTIL_H__

#include <arpa/inet.h>
#include <string>

class ApnSetting;
class PdpContext;
struct sit_pdp_setup_data_call_req;
struct DataCall;
struct QosSession_t;

enum {
    PDP_UNKNOWN = 0x00,
    PDP_IPV4 = 0x01,
    PDP_IPV6 = 0x02,
    PDP_IPV4V6 = 0x03,
    PDP_PPP = 0x04,
};

enum dc_type {
    DC_V10 = 0, DC_V11 = 1, DC_EXTENSION = 2, DC_RETRYTIME =3, DC_AMBR = 4, DC_QOS = 5, DC_SLICE_TDINFO = 6,
};

enum FLAG_FOR_OPT_SLICE_INFO {
    OPT_SST                     = 1,
    OPT_SST_MAPSST              = 2,
    OPT_SST_SD                  = 4,
    OPT_SST_SD_MAPSST           = 5,
    OPT_SST_SD_MAPSST_MAPSD     = 8,
};

enum FLAG_FOR_TRAFFIC_DESCRIPTOR {
    TD_FLAG_MATCHED_ALL         = (1 << 0),
    TD_FLAG_DNN                 = (1 << 1),
    TD_FLAG_OSAPPID             = (1 << 2),
    TD_FLAG_OSID                = (1 << 3),
};

extern int GetProtocolApnType(int dataProfileId, ApnSetting *apnSetting);
extern int GetPdpType(const char *protocol);
extern int ConvertAuthTypeToProtocolAuthType(int authtype);
extern int ConvertDataProfileIdToProtocolDataProfile(int dataProfileId);
extern int ConvertPdpProtocolTypeToSitPdpType(int protocol);
extern int checkPcscfTypeIpv4v6(const char *protocol);
extern int GetPcscfReqType(ApnSetting *apnSetting, const char *protocol);
extern int GetPcscfReqTypeforNullAPN(ApnSetting *apnSetting, const char *protocol);
extern bool isRatForCDMA(int rat);
extern bool isValidPdpApn(PdpContext *pPdpContext);
extern int ConvertRadioTechToSitValue(int rat);

// For Legacy mode
#define PROPERTY_HANDOVER_0                "vendor.ril.allow.handover.0"
#define PROPERTY_HANDOVER_IPV4_ADDRESS_0   "vendor.ril.ipv4.address.0"
#define PROPERTY_HANDOVER_IPV6_ADDRESS_0   "vendor.ril.ipv6.address.0"
#define PROPERTY_HANDOVER_1                "vendor.ril.allow.handover.1"
#define PROPERTY_HANDOVER_IPV4_ADDRESS_1   "vendor.ril.ipv4.address.1"
#define PROPERTY_HANDOVER_IPV6_ADDRESS_1   "vendor.ril.ipv6.address.1"
#define INVALID_IP   "0.0.0.0"

extern void FillHandoverIpAddressLegacy(sit_pdp_setup_data_call_req &req, int slotId);
extern void FillHandoverIpAddressLegacy(int slotId);
extern void ParseHandoverAddresses(const char *addresses, struct sockaddr_in &sa, struct sockaddr_in6 &sa6,
                                   char (&t)[INET_ADDRSTRLEN+1], char (&t6)[INET6_ADDRSTRLEN+1]);
extern void FillIpAddr(bool isIpv6, const char *address, char *target);
extern uint8_t ConvertConnectionCapabilities(std::string cc);

extern void InitQoSession(std::vector<QosSession_t> &qosSession, int &QosSessionCnt);
extern void InitDataCall(DataCall & dataCall);

#endif /* __PROTOCOL_PS_UTIL_H__ */
