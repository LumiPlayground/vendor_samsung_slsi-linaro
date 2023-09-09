/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsUtil_H__
#define __PsUtil_H__

#include <telephony/ril.h>

struct DataCall;
struct RIL_SetupDataCallResult_V1_6;

namespace PsUtil {
    void DumpBuf(char *data, size_t tlen);
    void DumpPcscfExtPayload(char *data);
    void PrintDataCallInfo(const DataCall *pDc, const char *calling_service, const char *calling_func, const int halVer);
    void PrintRilDataInfo(const RIL_SetupDataCallResult_V1_6 *rilDataCall, const char *caller);
    void PrintRilDataInfo(const RIL_Data_Call_Response_v11 *rilDataCall, const char *caller);
    void decodeAMBR(const DataCall *);
    void PrintAddressInfo(const DataCall *pDc);
};
#endif // __PsUtil_H__
