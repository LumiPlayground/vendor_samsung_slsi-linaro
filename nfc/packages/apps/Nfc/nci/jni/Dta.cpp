/*
 * Copyright (C) 2014 SAMSUNG S.LSI
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifdef SEC_NFC_DTA_SUPPORT

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <semaphore.h>

#include "JavaClassConstants.h"
#include "NfcJniUtil.h"
#include "NfcTag.h"
#include "NfcAdaptation.h"
#include "SyncEvent.h"

#include "nfa_api.h"
#include "nfa_dta_api.h"

#define SET_SLEEP_TIME_CFG      0
#define SET_SLEEP_TIME_ONCE     1
#define SET_SLEEP_TIME_FORCE    2

using android::base::StringPrintf;
extern bool nfc_debug_enabled;

namespace android
{
extern bool nfcManager_isNfcActive();
extern void nativeNfcTag_abortWaits ();
extern void slsiSetNfcSleepTimeout(unsigned long sec, int option);
}

static bool         DTAmode = false;
static bool         isDTAEnabled = false;
static SyncEvent    sNfaDTAEvent;
static SyncEvent    sNfaDTAStartEvent;
static SyncEvent    sNfaDTAStopEvent;
static SyncEvent    sNfaDTAConfigEvent;

enum {
    DTA_ENABLE = 0x00,
    DTA_DISABLE,
    DTA_START,
    DTA_STOP,
    DTA_CONFIG,
};
static void nfaSECDTACallback (uint8_t event, tNFA_DTA_CBACK_DATA *eventData)
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter; event=0x%02X", __func__, event);

    switch (event) {
    case NFA_DTA_ENABLED_EVT:
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: NFA_DTA_ENABLED_EVT - status = 0x%02X", __func__, eventData->status);
        isDTAEnabled = (eventData->status == NFA_STATUS_OK) ? true : false;
        SyncEventGuard guard (sNfaDTAEvent);
        sNfaDTAEvent.notifyOne ();
    }
        break;
    case NFA_DTA_DISABLED_EVT:
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: NFA_DTA_DISABLED_EVT", __func__);
        isDTAEnabled = false;
        SyncEventGuard guard (sNfaDTAEvent);
        sNfaDTAEvent.notifyOne ();
    }
        break;
    case NFA_DTA_START_EVT:
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: NFA_DTA_START_EVT - status = 0x%02X", __func__, eventData->start);
        SyncEventGuard guard (sNfaDTAStartEvent);
        sNfaDTAStartEvent.notifyOne ();
    }
        break;
    case NFA_DTA_STOP_EVT:
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: NFA_DTA_STOP_EVT - status = 0x%02X", __func__, eventData->status);
        SyncEventGuard guard (sNfaDTAStopEvent);
        sNfaDTAStopEvent.notifyOne ();
    }
        break;
    case NFA_DTA_CONFIG_EVT:
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: NFA_DTA_CONFIG_EVT - status = 0x%02X", __func__, eventData->status);
        SyncEventGuard guard (sNfaDTAConfigEvent);
        sNfaDTAConfigEvent.notifyOne ();
        break;
    }
    default:
        break;
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Exit", __func__);
}

bool SECDTA_doEnable(bool autoStart)
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter; auto_start=%d", __func__, autoStart);
    tNFA_STATUS stat = NFA_STATUS_OK;

    if (true == DTAmode)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: DTA already running", __func__);
        return JNI_TRUE;
    }

    if (android::nfcManager_isNfcActive())
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: NFC is on. Please turn NFC off", __func__);
        return JNI_FALSE;
    }

    NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
    theInstance.Initialize(); //start GKI, NCI task, NFC task

    DTAmode = true;

    {
        SyncEventGuard guard (sNfaDTAEvent);
        tHAL_NFC_ENTRY* halFuncEntries = theInstance.GetHalEntryFuncs ();
        NFA_DTA_Init(halFuncEntries);

        stat = NFA_DTA_Enable(autoStart, nfaSECDTACallback);

        if (stat == NFA_STATUS_OK)
        {
            sNfaDTAEvent.wait ();

            if (true == isDTAEnabled)
                goto TheEnd;
        }
    }

    LOG(ERROR) << StringPrintf("%s: Failed DTA Enabling; error=0x%X", __func__, stat);
    theInstance.Finalize();
    DTAmode = false;

TheEnd:
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
    return isDTAEnabled ? JNI_TRUE : JNI_FALSE;
}

bool SECDTA_doDisable()
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter", __func__);
    tNFA_STATUS stat;

    if (!isDTAEnabled)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Already disabled", __func__);
        return JNI_TRUE;
    }

    {
        SyncEventGuard guard(sNfaDTAEvent);
        stat = NFA_DTA_Disable();

        if(stat == NFA_STATUS_OK)
            sNfaDTAEvent.wait ();
    }

    android::nativeNfcTag_abortWaits();
    NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
    NfcTag::getInstance().abort ();
    theInstance.Finalize();
    isDTAEnabled = false;
    DTAmode = false;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Exit", __func__);
    return JNI_TRUE;
}

bool SECDTA_doStart(int patternNo)
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter - patternNo : 0x%02X", __func__, patternNo);
    tNFA_STATUS stat;
    if(false == DTAmode) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: DTA is not running.", __func__);
        return JNI_FALSE;
    }

// [START] NFCSM00000267 - Override sleep timeout for DTA
    android::slsiSetNfcSleepTimeout(5, SET_SLEEP_TIME_FORCE);
// [END] NFCSM00000267 - Override sleep timeout for DTA
    {
        SyncEventGuard guard(sNfaDTAStartEvent);
        stat = NFA_DTA_Start(patternNo, 0, NULL);
        if(NFA_STATUS_OK == stat)
            sNfaDTAStartEvent.wait();
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: EXit", __func__);
    return (NFA_STATUS_OK == stat) ? JNI_TRUE : JNI_FALSE;
}


bool SECDTA_doStop()
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter", __func__);
    tNFA_STATUS stat;
    if(false == DTAmode) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: DTA is not running.", __func__);
        return JNI_FALSE;
    }

    {
        SyncEventGuard guard(sNfaDTAStopEvent);
        stat = NFA_DTA_Stop();
        if(NFA_STATUS_OK == stat)
            sNfaDTAStopEvent.wait();
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: EXit", __func__);
    return (NFA_STATUS_OK == stat) ? JNI_TRUE : JNI_FALSE;
}

bool SECDTA_doConfig(int configItem, int configData)
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Enter - ConfigItem = 0x%02X", __func__, configItem);

    tNFA_STATUS stat;

    {
//    SyncEventGuard guard(sNfaDTAConfigEvent);
    stat = NFA_DTA_Config(configItem, configData);
//    if(NFA_STATUS_OK == stat)
//    sNfaDTAStartEvent.wait();
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: EXit", __func__);
    return (NFA_STATUS_OK == stat) ? JNI_TRUE : JNI_FALSE;
}
/*********************************
 * Dta server/client
 ********************************/
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <Dta.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

const char *SOCK_ADDR = "com.slsi.nfc.dta";
void* dta_server(void *arg)
{
    void *tmpRet = NULL;
    int server, client;
    int serverLen;
    struct sockaddr_un serverAddr;
    unsigned char cmd, ret = 0;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: dta server initialize", __func__);
    if (access(SOCK_ADDR, F_OK) == 0)
        unlink(SOCK_ADDR);

    if ((server = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX)) < 0)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: socket fail", __func__);
        return tmpRet;
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: socket OK", __func__);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_LOCAL;
    serverAddr.sun_path[0] = '\0';
    strcpy(serverAddr.sun_path+1, SOCK_ADDR);
    serverLen = 1 + strlen(SOCK_ADDR) + offsetof(struct sockaddr_un, sun_path);

    if (bind(server, (struct sockaddr *)&serverAddr, serverLen) < 0)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: bind fail", __func__);
        close (server);
        return tmpRet;
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: bind OK", __func__);

    if (listen(server, 5) == -1)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: listen fail", __func__);
        close (server);
        return tmpRet;
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: listen OK", __func__);

    while (ret != 0xFF)
    {
        client = accept(server, NULL, NULL);
        if (client == -1)
            // accetp error
            continue;
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: accept OK", __func__);

        if (read(client, &cmd, 1) != 1)
        {
            close(client);
            continue;
        }

        // handler
        ret = 0;
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: event id: %d", __func__, cmd);
        switch (cmd) {
        case DTA_ENABLE:
            if (read(client, &cmd, 1) != 1)
                ret = 0xFF;
            else
                SECDTA_doEnable((int)cmd);
            break;

        case DTA_DISABLE:
            SECDTA_doDisable();
            break;

        case DTA_START:
            /* To sync up new DTA application */
            unsigned char arg[2];
            if(read(client, arg, 2) != 2)
                ret = 0xFF;
            else
                SECDTA_doStart((int)((int)(arg[1] << 8 | (int)arg[0])));
            break;

        case DTA_STOP:
            SECDTA_doStop();
            break;

        case DTA_CONFIG:
            {
                unsigned char arg[2];
                if (read(client, arg, 2) != 2)
                    ret = 0xFF;
                else
                    SECDTA_doConfig((int)arg[0], (int)arg[1]);
            }
            break;

        default:
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: dta server terminate", __func__);
            ret = 0xFF;
            break;
        }

        if (ret != 0xFF)
            write(client, &ret, 1);

        close(client);
    }
    close (server);

    return tmpRet;
}

bool sendMsgToDtaServer(unsigned char msg, int argc, unsigned char *argv)
{
    int client, clientLen;
    struct sockaddr_un clientAddr;
    unsigned char ret;

    if ((client = socket(AF_LOCAL, SOCK_STREAM, PF_UNIX)) < 0)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: socket fail", __func__);
        return false;
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sun_family = AF_LOCAL;
    clientAddr.sun_path[0] = '\0';
    strcpy(clientAddr.sun_path + 1, SOCK_ADDR);
    clientLen = 1 + strlen(SOCK_ADDR) + offsetof(struct sockaddr_un, sun_path);

    if (connect(client, (struct sockaddr *)&clientAddr, clientLen) < 0)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: connect fail", __func__);
        close(client);
        return false;
    }

    if (write(client, &msg, 1) != 1)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: write fail", __func__);
        close(client);
        return false;
    }

    if (argc > 0 && argv != NULL)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: send args %d", __func__, argc);
        if (write(client, argv, argc) != argc)
        {
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: write arg fail", __func__);
            close(client);
            return false;
        }
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: write success!", __func__);
    }

    if (read(client, &ret, 1) != 1)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: read fail", __func__);
        close(client);
        return false;
    }

    close(client);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: result: %d", __func__, ret);
    return true;
}

bool startDtaServer()
{
    pthread_attr_t  attr;
    pthread_t    task;
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
    pthread_attr_init(&attr);
    if ( pthread_create( &task, &attr, dta_server, NULL))
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s : pthread_create failed", __func__);
    }
    pthread_attr_destroy(&attr);

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
    return true;
}

bool stopDtaServer()
{
    sendMsgToDtaServer('E', 0, NULL);
    return true;
}

#else

// Dummy API for Undefined SEC_NFC_DTA_SUPPORT Feature.

bool startDtaServer()
{
    return false;
}

bool stopDtaServer()
{
    return false;
}

#endif //SEC_NFC_DTA_SUPPORT
