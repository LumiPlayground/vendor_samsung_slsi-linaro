/* copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */


/* RIL EXTERNAL  */
#ifndef _RIL_EXTERNAL_H
#define _RIL_EXTERNAL_H

#include <telephony/ril.h>

/* RIL vendor external */
typedef void * RIL_External_Token;

struct RIL_External_Env {
    void (*ExternalOnRequestComplete)(RIL_External_Token t, RIL_Errno e, void *response, size_t responselen);
    void (*ExternalOnUnsolicitedResponse)(int unsolResponse, const void *data, size_t datalen, int slotId);
};

#if NO_RIL_ENV_UNITTEST
#else
struct RIL_External_EnvUnitTest : public RIL_External_Env {
    void (*OnUpdateReqModemData)(RIL_Token t, void *data, size_t datalen);
};
#endif

typedef void (*RIL_ExteternalRequestFunc) (int reqOemId, RIL_External_Token externalToken, void *data, size_t datalen, RIL_SOCKET_ID socket_id);

typedef struct {
    int version;        /* set to RIL_VERSION */
    RIL_ExteternalRequestFunc externalOnRequest;
    RIL_RadioStateRequest onStateRequest;
} RIL_RadioExternalFunctions;

typedef struct RadioExternalRequestInfo {
    int32_t serial;
    int32_t clientId;
    int32_t rilcReqId;
    int32_t slotId;
    int32_t rilOemReqId;
    int(*responseFunction) (int clientId, int rilcMsgId, int slotId, int serial, RIL_Errno e, void *response, size_t responseLen);
    struct RadioExternalRequestInfo *p_next;
} RadioExternalRequestInfo;
#endif // _RIL_EXTERNAL_H
