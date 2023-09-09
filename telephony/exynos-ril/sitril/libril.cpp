/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "rilapplication.h"
#include "rillog.h"

#ifdef __cplusplus
extern "C" {
#endif


#if defined(ANDROID_MULTI_SIM)
static void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID socket_id)
{
    RilApplication::RIL_OnRequest(request, data, datalen, t, socket_id);
}

static RIL_RadioState onStateRequest(RIL_SOCKET_ID socket_id)
{
    return RilApplication::RIL_OnRadioStateRequest(socket_id);
}
#else
static void onRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    RilApplication::RIL_OnRequest(request, data, datalen, t);
}

static RIL_RadioState onStateRequest()
{
    return RilApplication::RIL_OnRadioStateRequest();
}
#endif // ANDROID_MULTI_SIM

static int onSupports(int requestCode)
{
    return RilApplication::RIL_OnSupports(requestCode);
}

static void onCancel(RIL_Token t)
{
    RilApplication::RIL_OnCancel(t);
}

static const char * getVersion(void)
{
    return RilApplication::RIL_OnGetVersion();
}

/**
 * RIL callbacks exposed to RILD daemon
 */
static const RIL_RadioFunctions s_callbacks =
{
    RilApplication::RIL_VersionCode,
    onRequest,
    onStateRequest,
    onSupports,
    onCancel,
    getVersion
};


const RIL_RadioFunctions * RIL_Init(const struct RIL_Env *env, int /* argc */, char ** /* argv */)
{
    CREATE_OSAL_LOG_INSTANCE;
    CRilLog::InitRilLog();
    CRilLog::DumpResetLog("RIL_INIT");

    RilApplication *rilApp = RilApplication::CreateInstance(env);
    if (rilApp != NULL) {
        return &s_callbacks;
    }
    return NULL;
}

/**
 * RIL-SAP callbacks
 */

#if defined(ANDROID_MULTI_SIM)
static void onSapRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID socket_id)
{
    RilApplication::SAP_OnRequest(request, data, datalen, t, socket_id);
}
#else
static void onSapRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    RilApplication::SAP_OnRequest(request, data, datalen, t);
}
#endif // ANDROID_MULTI_SIM

static const RIL_RadioFunctions s_SapCallbacks =
{
    RilApplication::RIL_VersionCode,
    onSapRequest,
    NULL,
    NULL,
    NULL,
    NULL,
};

/* RIL SAP */
const RIL_RadioFunctions * RIL_SAP_Init(const struct RIL_Env *env, int /* argc */, char ** /* argv */) {
    // RilApplication must be initialized in RIL_Init
    RilApplication *rilApp = RilApplication::GetInstance();
    if (rilApp != NULL) {
        rilApp->SetSapEnv(env);
        return &s_SapCallbacks;
    }
    return NULL;
}

/* RIL vendor external */
static void externalOnRequest(int reqOemId, RIL_External_Token externalToken, void *data, size_t datalen, RIL_SOCKET_ID socket_id)
{
    RilApplication::RIL_externalOnRequest(reqOemId, data, datalen, (RIL_Token)externalToken, socket_id);
}

static const RIL_RadioExternalFunctions s_externalCallbacks =
{
    RilApplication::RIL_VersionCode,
    externalOnRequest,
    onStateRequest,
};

const RIL_RadioExternalFunctions * RIL_External_Init(const struct RIL_External_Env *env)
{
    RilApplication *rilApp = RilApplication::GetInstance();
    if (rilApp != NULL) {
        rilApp->setRilExternalEnv(env);
        return &s_externalCallbacks;
    }
    return NULL;
}

/**
 * RIL callbacks for unit test
 */
static void onModemRspSend(void *data, unsigned int datalen) {
    if (data != NULL && datalen > 0) {
        RilApplication::RIL_OnModemRspSend(data, datalen);
    }
}

static void onSitrilStateUpdate(int radioState, int networkRegState, int cardState) {
    RilApplication::RIL_OnSitrilStateUpdate(radioState, networkRegState, cardState);
}

static const RIL_UnitTestFunctions s_callbacksUnitTest = {
    RilApplication::RIL_VersionCode,
    onRequest,
    onStateRequest,
    onModemRspSend,
    onSitrilStateUpdate,
    externalOnRequest,
    onSapRequest,
};

const RIL_UnitTestFunctions * RIL_UnitTest(const struct RIL_Env *env,
                                            const struct RIL_External_Env *envExtern,
                                            int argc, char **argv) {
    // argv handling
    int utMode = 0;    // 0 means default operation, others mean unitTest operation
    for (int i = 1; i < argc ;) {
        if (argv[i] != NULL && 0 == strcmp(argv[i], "-t") &&  (argc - i > 1)) {
            utMode = strtol(argv[i + 1], NULL, 10);
            i += 2;
        } else {
            i++;
        }
    }

    CREATE_OSAL_LOG_INSTANCE;
    CRilLog::InitRilLog();
    CRilLog::DumpResetLog("RIL_UNITTEST_INIT");

    RilApplication *rilApp = RilApplication::CreateInstance(env, utMode);
    if (rilApp != NULL) {
        rilApp->setRilExternalEnv(envExtern);
        rilApp->SetSapEnv(env);
        return &s_callbacksUnitTest;
    }

    return NULL;
}

void RIL_UnitTestClose() {
    RilApplication::SetInstance(nullptr);
}

#ifdef __cplusplus
}
#endif
