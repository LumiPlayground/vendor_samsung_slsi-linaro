/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <nativehelper/JNIPlatformHelp.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <nativehelper/ScopedUtfChars.h>
#include <semaphore.h>

#include "HciEventManager.h"
#include "JavaClassConstants.h"
#include "NfcAdaptation.h"
#ifdef DTA_ENABLED
#include "NfcDta.h"
#endif /* DTA_ENABLED */
#include "NfcJniUtil.h"
#include "NfcTag.h"
#include "PeerToPeer.h"
#include "PowerSwitch.h"
#include "RoutingManager.h"
#include "SyncEvent.h"
#include "ce_api.h"
#include "debug_lmrt.h"
#include "nfa_api.h"
#include "nfa_ee_api.h"
#include "nfa_p2p_api.h"
#include "nfc_brcm_defs.h"
#include "nfc_config.h"
#include "phNxpExtns.h"
#include "rw_api.h"

/* START [J14111104] - DTA */
#ifdef SEC_NFC_DTA_SUPPORT
#include "Dta.h"
#endif
/* END [J14111104] - DTA */

using android::base::StringPrintf;

extern tNFA_DM_DISC_FREQ_CFG* p_nfa_dm_rf_disc_freq_cfg;  // defined in stack
namespace android {
extern bool gIsTagDeactivating;
extern bool gIsSelectingRfInterface;
extern void nativeNfcTag_doTransceiveStatus(tNFA_STATUS status, uint8_t* buf,
                                            uint32_t buflen);
extern void nativeNfcTag_notifyRfTimeout();
extern void nativeNfcTag_doConnectStatus(jboolean is_connect_ok);
extern void nativeNfcTag_doDeactivateStatus(int status);
extern void nativeNfcTag_doWriteStatus(jboolean is_write_ok);
extern jboolean nativeNfcTag_doDisconnect(JNIEnv*, jobject);
extern void nativeNfcTag_doCheckNdefResult(tNFA_STATUS status,
                                           uint32_t max_size,
                                           uint32_t current_size,
                                           uint8_t flags);
extern void nativeNfcTag_doMakeReadonlyResult(tNFA_STATUS status);
extern void nativeNfcTag_doPresenceCheckResult(tNFA_STATUS status);
extern void nativeNfcTag_formatStatus(bool is_ok);
extern void nativeNfcTag_resetPresenceCheck();
extern void nativeNfcTag_doReadCompleted(tNFA_STATUS status);
extern void nativeNfcTag_setRfInterface(tNFA_INTF_TYPE rfInterface);
extern void nativeNfcTag_setActivatedRfProtocol(tNFA_INTF_TYPE rfProtocol);
extern void nativeNfcTag_abortWaits();
extern void nativeLlcpConnectionlessSocket_abortWait();
extern void nativeNfcTag_registerNdefTypeHandler();
extern void nativeNfcTag_acquireRfInterfaceMutexLock();
extern void nativeNfcTag_releaseRfInterfaceMutexLock();
extern void nativeLlcpConnectionlessSocket_receiveData(uint8_t* data,
                                                       uint32_t len,
                                                       uint32_t remote_sap);
}  // namespace android

/*****************************************************************************
**
** public variables and functions
**
*****************************************************************************/
bool gActivated = false;
SyncEvent gDeactivatedEvent;
SyncEvent sNfaSetPowerSubState;
SyncEvent sNfaVsCmdEvent;
SyncEvent sNfaAidMatchingCmdEvent;
SyncEvent sNfaVsFactoryCmdEvent;
SyncEvent sNfaRfDiscoveryCmdEvent;
SyncEvent sNfaListenTechMaskCmdEvent;
bool legacy_mfc_reader = true;
int recovery_option = 0;
int nfcee_power_and_link_conf = 0;

namespace android {
jmethodID gCachedNfcManagerNotifyNdefMessageListeners;
jmethodID gCachedNfcManagerNotifyTransactionListeners;
jmethodID gCachedNfcManagerNotifyLlcpLinkActivation;
jmethodID gCachedNfcManagerNotifyLlcpLinkDeactivated;
jmethodID gCachedNfcManagerNotifyLlcpFirstPacketReceived;
jmethodID gCachedNfcManagerNotifyHostEmuActivated;
jmethodID gCachedNfcManagerNotifyHostEmuData;
jmethodID gCachedNfcManagerNotifyHostEmuDeactivated;
jmethodID gCachedNfcManagerNotifyRfFieldActivated;
jmethodID gCachedNfcManagerNotifyRfFieldDeactivated;
jmethodID gCachedNfcManagerNotifyEeUpdated;
jmethodID gCachedNfcManagerNotifyHwErrorReported;
jmethodID gCachedNfcManagerNotifyEeActionProtocol;
jmethodID gCachedNfcManagerNotifySeListenActivated;
jmethodID gCachedNfcManagerNotifySeListenDeactivated;
const char* gNativeP2pDeviceClassName =
    "com/android/nfc/dhimpl/NativeP2pDevice";
const char* gNativeLlcpServiceSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpServiceSocket";
const char* gNativeLlcpConnectionlessSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpConnectionlessSocket";
const char* gNativeLlcpSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpSocket";
const char* gNativeNfcTagClassName = "com/android/nfc/dhimpl/NativeNfcTag";
const char* gNativeNfcManagerClassName =
    "com/android/nfc/dhimpl/NativeNfcManager";
void doStartupConfig();
void startStopPolling(bool isStartPolling);
void startRfDiscovery(bool isStart);
bool isDiscoveryStarted();
void doSetForceFWDownload(bool force);
void notifyListenModeState(bool isSeActive);
void slsiSetNfcSleepTimeout(unsigned long sec, int option);
void doSetAidMatchingMode();
}  // namespace android

/*****************************************************************************
**
** private variables and functions
**
*****************************************************************************/
namespace android {
static jint sLastError = ERROR_BUFFER_TOO_SMALL;
static SyncEvent sNfaEnableEvent;                // event for NFA_Enable()
static SyncEvent sNfaDisableEvent;               // event for NFA_Disable()
static SyncEvent sNfaEnableDisablePollingEvent;  // event for
                                                 // NFA_EnablePolling(),
                                                 // NFA_DisablePolling()
SyncEvent gNfaSetConfigEvent;                    // event for Set_Config....
SyncEvent gNfaGetConfigEvent;                    // event for Get_Config....
static bool sIsNfaEnabled = false;
static bool sDiscoveryEnabled = false;  // is polling or listening
static bool sPollingEnabled = false;    // is polling for tag?
static bool sIsDisabling = false;
static bool sRfEnabled = false;   // whether RF discovery is enabled
static bool sSeRfActive = false;  // whether RF with SE is likely active
static bool sReaderModeEnabled =
    false;  // whether we're only reading tags, not allowing P2p/card emu
static bool sP2pEnabled = false;
static bool sP2pActive = false;  // whether p2p was last active
static bool sAbortConnlessWait = false;
static jint sLfT3tMax = 0;
static bool sRoutingInitialized = false;
static bool sIsRecovering = false;

#define CONFIG_UPDATE_TECH_MASK (1 << 1)
#define DEFAULT_TECH_MASK                                                  \
  (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F | \
   NFA_TECHNOLOGY_MASK_V | NFA_TECHNOLOGY_MASK_B_PRIME |                   \
   NFA_TECHNOLOGY_MASK_A_ACTIVE | NFA_TECHNOLOGY_MASK_F_ACTIVE |           \
   NFA_TECHNOLOGY_MASK_KOVIO)
#define DEFAULT_DISCOVERY_DURATION 500
#define READER_MODE_DISCOVERY_DURATION 200

// [SEC] Proprietary command
#define NCI_PROP_FACTORY_TEST_OID   0x16
#define NCI_PROP_SET_SIMVDD2_OID 0x30
#define NCI_PROP_SET_AID_ROUTING_STRATEGY_OID 0x39
#define NCI_PROP_FW_DEBUG_LEVEL 0x17

#define DEBUG_TYPE_ESE_TRACE 0x00
#define DEBUG_TYPE_PART3_TRACE 0x01
#define DEBUG_TYPE_FIELD_STRENGTH_TRACE 0x02
#define DEBUG_TYPE_UICC_TRACE 0x03
#define DEBUG_TYPE_CLT_TRACE 0x04

#define DEBUG_IDX_UICC_TRACE (0x01)
#define DEBUG_IDX_ESE_TRACE (0x01 << 1)
#define DEBUG_IDX_FIELD_STRENGTH_TRACE (0x01 << 2)
#define DEBUG_IDX_PART3_TRACE (0x01 << 3)
#define DEBUG_IDX_CLT_TRACE (0x01 << 4)

#define RFREG_SECTION_SIZE 252
#define RFOPTION_BUFFER_SIZE 3072
#define RFOPTION_SIZE_RN82 3056
#define NEW_RFOPTION_UPDATE_SIZE_RN82 3056
#define RFOPTION_TOTAL_SIZE_SN4V 3584
#define RFOPTION_CHINA_SIZE_SN4V 512
#define NEW_RFOPTION_UPDATE_SIZE_SN4V 512
#define RFOPTION_POSITION_CHINA_SN4V 3072

#define NCI_PROP_START_NEW_RFREG 0x13
#define NCI_PROP_SET_NEW_RFREG 0x14
#define NCI_PROP_SET_RF_META_DATA 0x15
#define NCI_PROP_SET_RFREG_VER 0x25
#define NCI_PROP_GET_OPTION_META_OID 0x29
#define NCI_PROP_STOP_UPDATE 0x15

#define NCI_PROP_SET_SLEEP_TIME     0x1A    /* Last updated value: 20160530 */

static void nfaConnectionCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
static void nfaDeviceManagementCallback(uint8_t event,
                                        tNFA_DM_CBACK_DATA* eventData);
static bool isPeerToPeer(tNFA_ACTIVATED& activated);
static bool isListenMode(tNFA_ACTIVATED& activated);
static tNFA_STATUS stopPolling_rfDiscoveryDisabled();
static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask);
static void nfcManager_doSetScreenState(JNIEnv* e, jobject o,
                                        jint screen_state_mask);

bool doRfOptionUpdate(const char* path);
static bool initializeRfOptionCB();
static void RfUpdateCallback (uint8_t event, uint16_t param_len, uint8_t *p_param);
static bool getRfOptionFile(uint8_t *buffer, const char* path);
static bool compRfOptionVersion();
static bool setRfOptionVersion(uint8_t verType);
static int setRfOptionValue(uint8_t *buffer);

typedef struct {
  uint32_t rfFileSize;
  uint32_t rfOptionOffset;
  uint32_t rfOptionSize;
  uint32_t rfOptionCheckSum;
  uint8_t* rfOptionData;
  uint8_t updateData[RFREG_SECTION_SIZE + 1];
  uint8_t chipVersion[15];
  uint8_t imgVersion[15];
} tRfOptionCB;

tRfOptionCB rfOptionCB;
static uint8_t rfOptionUpdate_rsp = 0;

uint8_t factoryTestResultLen;
uint8_t factoryTestResultData[6];

static void vsCmdCallback (uint8_t event, uint16_t param_len, uint8_t* p_param);
static void vendorCmdCallback (uint8_t event, uint16_t param_len, uint8_t* p_param);

static void doSetScreenStateCallback(uint8_t event, uint16_t param_len, uint8_t* p_param);
static void aidMatchingCmdCallback (uint8_t event, uint16_t param_len, uint8_t* p_param);

static tNFA_STATUS stopPolling_rfDiscoveryDisabled();
static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask);

static void nfcManager_doSetRfEnabled(bool enable);
static void nfcManager_doSetFirmwareDebugLevel(JNIEnv* e, jobject o,
                                               jint type, jboolean enable);

static jint nfcManager_doGetDefaultRoute(JNIEnv* e, jobject o);
static jboolean nfcManager_doSetDefaultRoute(JNIEnv* e, jobject o,
                                        jint defaultRoute, jboolean restart);
static void nfcManager_doSetPreferredSimSlot(JNIEnv *e, jobject o, jint slotId);
static bool nfcManager_doGetSimDetection(JNIEnv *e, jobject o);
static void nfcManager_doSetWaitNfcEeDiscovery(JNIEnv *e, jobject o, jboolean is_wait);
static jintArray nfcManager_doGetSecureElementList(JNIEnv* e, jobject);
static jboolean nfcManager_doRFoptionUpdate(JNIEnv* e, jobject o, jbyteArray filepath);
static jint nfcManager_doGetListenTechMask(JNIEnv *e, jobject o);
static void nfcManager_doSetListenTechMask(JNIEnv *e, jobject o, jint techMask);
static jbyteArray nfcManager_doSetFactoryTestMode (JNIEnv *e, jobject o, jbyte mode);
static void nfcManager_startRfDiscovery(JNIEnv * e, jobject o, jboolean start);
static void nfcManager_doSetForceFWDownload(JNIEnv * e, jobject o, jboolean force);

static tNFA_STATUS nfcManager_setPowerSubStateForScreenState(uint8_t state);

uint16_t gCurrentConfigLen;
uint8_t gConfig[256];
static int prevScreenState = NFA_SCREEN_STATE_OFF_LOCKED;
static int NFA_SCREEN_POLLING_TAG_MASK = 0x10;
static bool gIsDtaEnabled = false;
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool nfc_debug_enabled;

namespace {
void initializeGlobalDebugEnabledFlag() {
  nfc_debug_enabled =
      (NfcConfig::getUnsigned(NAME_NFC_DEBUG_ENABLED, 1) != 0) ? true : false;

  bool debug_enabled = property_get_bool("persist.nfc.debug_enabled", false);

  nfc_debug_enabled = (nfc_debug_enabled || debug_enabled);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: level=%u", __func__, nfc_debug_enabled);
}
void initializeMfcReaderOption() {
  legacy_mfc_reader =
      (NfcConfig::getUnsigned(NAME_LEGACY_MIFARE_READER, 0) != 0) ? true : false;

  DLOG_IF(INFO, nfc_debug_enabled)
      << __func__ <<": mifare reader option=" << legacy_mfc_reader;

}
void initializeRecoveryOption() {
  recovery_option = NfcConfig::getUnsigned(NAME_RECOVERY_OPTION, 0);

  DLOG_IF(INFO, nfc_debug_enabled)
      << __func__ << ": recovery option=" << recovery_option;
}

void initializeNfceePowerAndLinkConf() {
  nfcee_power_and_link_conf =
      NfcConfig::getUnsigned(NAME_ALWAYS_ON_SET_EE_POWER_AND_LINK_CONF, 0);

  DLOG_IF(INFO, nfc_debug_enabled)
      << __func__ << ": Always on set NFCEE_POWER_AND_LINK_CONF="
      << nfcee_power_and_link_conf;
}

}  // namespace

/*******************************************************************************
**
** Function:        getNative
**
** Description:     Get native data
**
** Returns:         Native data structure.
**
*******************************************************************************/
nfc_jni_native_data* getNative(JNIEnv* e, jobject o) {
  static struct nfc_jni_native_data* sCachedNat = NULL;
  if (e) {
    sCachedNat = nfc_jni_get_nat(e, o);
  }
  return sCachedNat;
}

/*******************************************************************************
**
** Function:        handleRfDiscoveryEvent
**
** Description:     Handle RF-discovery events from the stack.
**                  discoveredDevice: Discovered device.
**
** Returns:         None
**
*******************************************************************************/
static void handleRfDiscoveryEvent(tNFC_RESULT_DEVT* discoveredDevice) {
  NfcTag& natTag = NfcTag::getInstance();
  natTag.setNumDiscNtf(natTag.getNumDiscNtf() + 1);
  if (discoveredDevice->more == NCI_DISCOVER_NTF_MORE) {
    // there is more discovery notification coming
    return;
  }

  bool isP2p = natTag.isP2pDiscovered();

  if (natTag.getNumDiscNtf() > 1) {
    natTag.setMultiProtocolTagSupport(true);
    if (isP2p) {
      // Remove NFC_DEP NTF count
      // Skip NFC_DEP protocol in MultiProtocolTag select.
      natTag.setNumDiscNtf(natTag.getNumDiscNtf() - 1);
    }
  }

  if (sP2pEnabled && !sReaderModeEnabled && isP2p) {
    // select the peer that supports P2P
    natTag.selectP2p();
  } else {
    natTag.setNumDiscNtf(natTag.getNumDiscNtf() - 1);
    // select the first of multiple tags that is discovered
    natTag.selectFirstTag();
  }
}

/*******************************************************************************
**
** Function:        nfaConnectionCallback
**
** Description:     Receive connection-related events from stack.
**                  connEvent: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
static void nfaConnectionCallback(uint8_t connEvent,
                                  tNFA_CONN_EVT_DATA* eventData) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: event= %u", __func__, connEvent);

  switch (connEvent) {
    case NFA_POLL_ENABLED_EVT:  // whether polling successfully started
    {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_POLL_ENABLED_EVT: status = %u", __func__, eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_POLL_DISABLED_EVT:  // Listening/Polling stopped
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_POLL_DISABLED_EVT: status = %u", __func__,
                          eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_RF_DISCOVERY_STARTED_EVT:  // RF Discovery started
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_RF_DISCOVERY_STARTED_EVT: status = %u",
                          __func__, eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
      SyncEventGuard guard_rf(sNfaRfDiscoveryCmdEvent);
      sNfaRfDiscoveryCmdEvent.notifyOne();
    } break;

    case NFA_RF_DISCOVERY_STOPPED_EVT:  // RF Discovery stopped event
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_RF_DISCOVERY_STOPPED_EVT: status = %u",
                          __func__, eventData->status);

      gActivated = false;

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
      SyncEventGuard guard_rf(sNfaRfDiscoveryCmdEvent);
      sNfaRfDiscoveryCmdEvent.notifyOne();
    } break;

    case NFA_DISC_RESULT_EVT:  // NFC link/protocol discovery notificaiton
      status = eventData->disc_result.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DISC_RESULT_EVT: status = %d", __func__, status);
      if (status != NFA_STATUS_OK) {
        NfcTag::getInstance().setNumDiscNtf(0);
        LOG(ERROR) << StringPrintf("%s: NFA_DISC_RESULT_EVT error: status = %d",
                                   __func__, status);
      } else {
/* START [2020021901] - skip resetTechnologies() for keeping tag information when operate retry logic */
        if(!gIsSelectingRfInterface)
        {
        NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        }
        else
        {
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: skip connectionEventHandler() becaused of retry logic (0x%x)",
                __func__, gIsSelectingRfInterface);
        }
/* END [2020021901] - skip resetTechnologies() for keeping tag information when operate retry logic */
        handleRfDiscoveryEvent(&eventData->disc_result.discovery_ntf);
      }
      break;

    case NFA_SELECT_RESULT_EVT:  // NFC link/protocol discovery select response
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SELECT_RESULT_EVT: status = %d, gIsSelectingRfInterface = "
          "%d, "
          "sIsDisabling=%d",
          __func__, eventData->status, gIsSelectingRfInterface, sIsDisabling);

      if (sIsDisabling) break;

      if (eventData->status != NFA_STATUS_OK) {
        if (gIsSelectingRfInterface) {
          nativeNfcTag_doConnectStatus(false);
        }

        LOG(ERROR) << StringPrintf(
            "%s: NFA_SELECT_RESULT_EVT error: status = %d", __func__,
            eventData->status);
        NFA_Deactivate(FALSE);
      }
      break;

    case NFA_DEACTIVATE_FAIL_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DEACTIVATE_FAIL_EVT: status = %d", __func__,
                          eventData->status);
      break;

    case NFA_ACTIVATED_EVT:  // NFC link/protocol activated
    {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_ACTIVATED_EVT: gIsSelectingRfInterface=%d, sIsDisabling=%d",
          __func__, gIsSelectingRfInterface, sIsDisabling);
      uint8_t activatedProtocol =
          (tNFA_INTF_TYPE)eventData->activated.activate_ntf.protocol;
      if (NFC_PROTOCOL_T5T == activatedProtocol &&
          NfcTag::getInstance().getNumDiscNtf()) {
        /* T5T doesn't support multiproto detection logic */
        NfcTag::getInstance().setNumDiscNtf(0);
      }
      if ((eventData->activated.activate_ntf.protocol !=
           NFA_PROTOCOL_NFC_DEP) &&
          (!isListenMode(eventData->activated))) {
        nativeNfcTag_setRfInterface(
            (tNFA_INTF_TYPE)eventData->activated.activate_ntf.intf_param.type);
        nativeNfcTag_setActivatedRfProtocol(activatedProtocol);
      }
      if (EXTNS_GetConnectFlag() == TRUE) {
        NfcTag::getInstance().setActivationState();
        nativeNfcTag_doConnectStatus(true);
        break;
      }
      NfcTag::getInstance().setActive(true);
      if (sIsDisabling || !sIsNfaEnabled) break;
      gActivated = true;

      NfcTag::getInstance().setActivationState();
      if (gIsSelectingRfInterface) {
        nativeNfcTag_doConnectStatus(true);
        break;
      }

      nativeNfcTag_resetPresenceCheck();
      if (!isListenMode(eventData->activated) &&
          (prevScreenState == NFA_SCREEN_STATE_OFF_LOCKED ||
           prevScreenState == NFA_SCREEN_STATE_OFF_UNLOCKED)) {
        NFA_Deactivate(FALSE);
      }
      if (isPeerToPeer(eventData->activated)) {
        if (sReaderModeEnabled) {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: ignoring peer target in reader mode.", __func__);
          NFA_Deactivate(FALSE);
          break;
        }
        sP2pActive = true;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: NFA_ACTIVATED_EVT; is p2p", __func__);
        if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
          // Disable RF field events in case of p2p
          uint8_t nfa_disable_rf_events[] = {0x00};
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: Disabling RF field events", __func__);
          status = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO,
                                 sizeof(nfa_disable_rf_events),
                                 &nfa_disable_rf_events[0]);
          if (status == NFA_STATUS_OK) {
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: Disabled RF field events", __func__);
          } else {
            LOG(ERROR) << StringPrintf("%s: Failed to disable RF field events",
                                       __func__);
          }
        }
      } else {
        NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        if (NfcTag::getInstance().getNumDiscNtf()) {
          /*If its multiprotocol tag, deactivate tag with current selected
          protocol to sleep . Select tag with next supported protocol after
          deactivation event is received*/
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: Multi-Protocol Tag detected, Go to sleep", __func__);
          NFA_Deactivate(true);
        }

        // We know it is not activating for P2P.  If it activated in
        // listen mode then it is likely for an SE transaction.
        // Send the RF Event.
        if (isListenMode(eventData->activated)) {
          sSeRfActive = true;
          notifyListenModeState(true);
        }
/* START [J14111102] - KOVIO BARCODE */
        else
        {
          tNFA_ACTIVATED& activated = eventData->activated;
          if (NfcTag::getInstance().IsSameKovio(activated))
          {
            status = NFA_Deactivate (FALSE);
            if (status != NFA_STATUS_OK)
            {
              LOG(ERROR) << StringPrintf("%s: deactivate failed; error=0x%X",
                                            __func__, status);
            }
            break;
          }
        }
/* END [J14111102] - KOVIO BARCODE */
      }
    } break;
    case NFA_DEACTIVATED_EVT:  // NFC link/protocol deactivated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DEACTIVATED_EVT   Type: %u, gIsTagDeactivating: %d, gIsSelectingRfInterface: %d",
          __func__, eventData->deactivated.type, gIsTagDeactivating, gIsSelectingRfInterface);

      NfcTag::getInstance().setDeactivationState(eventData->deactivated);
      NfcTag::getInstance().selectNextTagIfExists();
      if (eventData->deactivated.type != NFA_DEACTIVATE_TYPE_SLEEP) {
        {
          SyncEventGuard g(gDeactivatedEvent);
          gActivated = false;  // guard this variable from multi-threaded access
          gDeactivatedEvent.notifyOne();
        }
        nativeNfcTag_resetPresenceCheck();
/* START [2020021901] - skip resetTechnologies() for keeping tag information when operate retry logic */
        if(!gIsSelectingRfInterface)
          NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        else
        {
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: skip resetTechnologies() becaused of retry logic (0x%x)",
                    __func__, gIsSelectingRfInterface);
        }
/* END [2020021901] - skip resetTechnologies() for keeping tag information when operate retry logic */

        nativeNfcTag_abortWaits();
        NfcTag::getInstance().abort();
      } else if (gIsTagDeactivating) {
        NfcTag::getInstance().setActive(false);
        nativeNfcTag_doDeactivateStatus(0);
      } else if (EXTNS_GetDeactivateFlag() == TRUE) {
        NfcTag::getInstance().setActive(false);
        nativeNfcTag_doDeactivateStatus(0);
      }

      // If RF is activated for what we think is a Secure Element transaction
      // and it is deactivated to either IDLE or DISCOVERY mode, notify w/event.
      if ((eventData->deactivated.type == NFA_DEACTIVATE_TYPE_IDLE) ||
          (eventData->deactivated.type == NFA_DEACTIVATE_TYPE_DISCOVERY)) {
        if (sSeRfActive) {
          sSeRfActive = false;
        } else if (sP2pActive) {
          sP2pActive = false;
          // Make sure RF field events are re-enabled
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: NFA_DEACTIVATED_EVT; is p2p", __func__);
          if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
            // Disable RF field events in case of p2p
            uint8_t nfa_enable_rf_events[] = {0x01};

            if (!sIsDisabling && sIsNfaEnabled) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: Enabling RF field events", __func__);
              status = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO,
                                     sizeof(nfa_enable_rf_events),
                                     &nfa_enable_rf_events[0]);
              if (status == NFA_STATUS_OK) {
                DLOG_IF(INFO, nfc_debug_enabled)
                    << StringPrintf("%s: Enabled RF field events", __func__);
              } else {
                LOG(ERROR) << StringPrintf(
                    "%s: Failed to enable RF field events", __func__);
              }
            }
          }
        }
      }

      break;

    case NFA_TLV_DETECT_EVT:  // TLV Detection complete
      status = eventData->tlv_detect.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_TLV_DETECT_EVT: status = %d, protocol = %d, num_tlvs = %d, "
          "num_bytes = %d",
          __func__, status, eventData->tlv_detect.protocol,
          eventData->tlv_detect.num_tlvs, eventData->tlv_detect.num_bytes);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_TLV_DETECT_EVT error: status = %d",
                                   __func__, status);
      }
      break;

    case NFA_NDEF_DETECT_EVT:  // NDEF Detection complete;
      // if status is failure, it means the tag does not contain any or valid
      // NDEF data;  pass the failure status to the NFC Service;
      status = eventData->ndef_detect.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_NDEF_DETECT_EVT: status = 0x%X, protocol = %u, "
          "max_size = %u, cur_size = %u, flags = 0x%X",
          __func__, status, eventData->ndef_detect.protocol,
          eventData->ndef_detect.max_size, eventData->ndef_detect.cur_size,
          eventData->ndef_detect.flags);
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      nativeNfcTag_doCheckNdefResult(status, eventData->ndef_detect.max_size,
                                     eventData->ndef_detect.cur_size,
                                     eventData->ndef_detect.flags);
      break;

    case NFA_DATA_EVT:  // Data message received (for non-NDEF reads)
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DATA_EVT: status = 0x%X, len = %d", __func__,
                          eventData->status, eventData->data.len);
      nativeNfcTag_doTransceiveStatus(eventData->status, eventData->data.p_data,
                                      eventData->data.len);
      break;
    case NFA_RW_INTF_ERROR_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFC_RW_INTF_ERROR_EVT", __func__);
      nativeNfcTag_notifyRfTimeout();
      nativeNfcTag_doReadCompleted(NFA_STATUS_TIMEOUT);
      break;
    case NFA_SELECT_CPLT_EVT:  // Select completed
      status = eventData->status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SELECT_CPLT_EVT: status = %d", __func__, status);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_SELECT_CPLT_EVT error: status = %d",
                                   __func__, status);
      }
      break;

    case NFA_READ_CPLT_EVT:  // NDEF-read or tag-specific-read completed
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_READ_CPLT_EVT: status = 0x%X", __func__, eventData->status);
      nativeNfcTag_doReadCompleted(eventData->status);
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      break;

    case NFA_WRITE_CPLT_EVT:  // Write completed
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_WRITE_CPLT_EVT: status = %d", __func__, eventData->status);
      nativeNfcTag_doWriteStatus(eventData->status == NFA_STATUS_OK);
      break;

    case NFA_SET_TAG_RO_EVT:  // Tag set as Read only
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SET_TAG_RO_EVT: status = %d", __func__, eventData->status);
      nativeNfcTag_doMakeReadonlyResult(eventData->status);
      break;

    case NFA_CE_NDEF_WRITE_START_EVT:  // NDEF write started
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_NDEF_WRITE_START_EVT: status: %d",
                          __func__, eventData->status);

      if (eventData->status != NFA_STATUS_OK)
        LOG(ERROR) << StringPrintf(
            "%s: NFA_CE_NDEF_WRITE_START_EVT error: status = %d", __func__,
            eventData->status);
      break;

    case NFA_CE_NDEF_WRITE_CPLT_EVT:  // NDEF write completed
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: FA_CE_NDEF_WRITE_CPLT_EVT: len = %u", __func__,
                          eventData->ndef_write_cplt.len);
      break;

    case NFA_LLCP_ACTIVATED_EVT:  // LLCP link is activated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_LLCP_ACTIVATED_EVT: is_initiator: %d  remote_wks: %d, "
          "remote_lsc: %d, remote_link_miu: %d, local_link_miu: %d",
          __func__, eventData->llcp_activated.is_initiator,
          eventData->llcp_activated.remote_wks,
          eventData->llcp_activated.remote_lsc,
          eventData->llcp_activated.remote_link_miu,
          eventData->llcp_activated.local_link_miu);
      if (sIsDisabling) {
        break;
      }
      PeerToPeer::getInstance().llcpActivatedHandler(getNative(0, 0),
                                                     eventData->llcp_activated);
      break;

    case NFA_LLCP_DEACTIVATED_EVT:  // LLCP link is deactivated
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LLCP_DEACTIVATED_EVT", __func__);
      PeerToPeer::getInstance().llcpDeactivatedHandler(
          getNative(0, 0), eventData->llcp_deactivated);
      break;
    case NFA_LLCP_FIRST_PACKET_RECEIVED_EVT:  // Received first packet over llcp
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LLCP_FIRST_PACKET_RECEIVED_EVT", __func__);
      PeerToPeer::getInstance().llcpFirstPacketHandler(getNative(0, 0));
      break;
    case NFA_PRESENCE_CHECK_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_PRESENCE_CHECK_EVT: status=0x%X", __func__,
                          eventData->status);
      nativeNfcTag_doPresenceCheckResult(eventData->status);
      break;
    case NFA_FORMAT_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_FORMAT_CPLT_EVT: status=0x%X", __func__, eventData->status);
      nativeNfcTag_formatStatus(eventData->status == NFA_STATUS_OK);
      break;

    case NFA_I93_CMD_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_I93_CMD_CPLT_EVT: status=0x%X", __func__, eventData->status);
      break;

    case NFA_CE_UICC_LISTEN_CONFIGURED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_UICC_LISTEN_CONFIGURED_EVT : status=0x%X",
                          __func__, eventData->status);
      break;

    case NFA_SET_P2P_LISTEN_TECH_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_SET_P2P_LISTEN_TECH_EVT", __func__);
      PeerToPeer::getInstance().connectionEventHandler(connEvent, eventData);
      break;

#ifdef NFC_SEC_NOT_OPEN_INCLUDED
    case NFA_SET_LISTEN_TECH_MASK_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: NFA_SET_LISTEN_TECH_MASK_EVT", __func__);
      {
        SyncEventGuard g(sNfaListenTechMaskCmdEvent);
        sNfaListenTechMaskCmdEvent.notifyOne();
      }
      break;
#endif

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown event ????", __func__);
      break;
  }
}

/*******************************************************************************
**
** Function:        nfcManager_initNativeStruc
**
** Description:     Initialize variables.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_initNativeStruc(JNIEnv* e, jobject o) {
  initializeGlobalDebugEnabledFlag();
  initializeMfcReaderOption();
  initializeRecoveryOption();
  initializeNfceePowerAndLinkConf();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  nfc_jni_native_data* nat =
      (nfc_jni_native_data*)malloc(sizeof(struct nfc_jni_native_data));
  if (nat == NULL) {
    LOG(ERROR) << StringPrintf("%s: fail allocate native data", __func__);
    return JNI_FALSE;
  }

  memset(nat, 0, sizeof(*nat));
  e->GetJavaVM(&(nat->vm));
  nat->env_version = e->GetVersion();
  nat->manager = e->NewGlobalRef(o);

  ScopedLocalRef<jclass> cls(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(cls.get(), "mNative", "J");
  e->SetLongField(o, f, (jlong)nat);

  /* Initialize native cached references */
  gCachedNfcManagerNotifyNdefMessageListeners =
      e->GetMethodID(cls.get(), "notifyNdefMessageListeners",
                     "(Lcom/android/nfc/dhimpl/NativeNfcTag;)V");
  gCachedNfcManagerNotifyLlcpLinkActivation =
      e->GetMethodID(cls.get(), "notifyLlcpLinkActivation",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");
  gCachedNfcManagerNotifyLlcpLinkDeactivated =
      e->GetMethodID(cls.get(), "notifyLlcpLinkDeactivated",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");
  gCachedNfcManagerNotifyLlcpFirstPacketReceived =
      e->GetMethodID(cls.get(), "notifyLlcpLinkFirstPacketReceived",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");

  gCachedNfcManagerNotifyHostEmuActivated =
      e->GetMethodID(cls.get(), "notifyHostEmuActivated", "(I)V");

  gCachedNfcManagerNotifyHostEmuData =
      e->GetMethodID(cls.get(), "notifyHostEmuData", "(I[B)V");

  gCachedNfcManagerNotifyHostEmuDeactivated =
      e->GetMethodID(cls.get(), "notifyHostEmuDeactivated", "(I)V");

  gCachedNfcManagerNotifyRfFieldActivated =
      e->GetMethodID(cls.get(), "notifyRfFieldActivated", "()V");
  gCachedNfcManagerNotifyRfFieldDeactivated =
      e->GetMethodID(cls.get(), "notifyRfFieldDeactivated", "()V");

  gCachedNfcManagerNotifyTransactionListeners = e->GetMethodID(
      cls.get(), "notifyTransactionListeners", "([B[BLjava/lang/String;)V");

  gCachedNfcManagerNotifyEeUpdated =
      e->GetMethodID(cls.get(), "notifyEeUpdated", "()V");

  gCachedNfcManagerNotifyHwErrorReported =
      e->GetMethodID(cls.get(), "notifyHwErrorReported", "()V");

  gCachedNfcManagerNotifyEeActionProtocol =
    e->GetMethodID(cls.get(), "notifyEeActionProtocolListeners", "(IILjava/lang/String;)V");

  gCachedNfcManagerNotifySeListenActivated =
      e->GetMethodID(cls.get(),"notifySeListenActivated", "()V");

  gCachedNfcManagerNotifySeListenDeactivated =
      e->GetMethodID(cls.get(),"notifySeListenDeactivated", "()V");

  if (nfc_jni_cache_object(e, gNativeNfcTagClassName, &(nat->cached_NfcTag)) ==
      -1) {
    LOG(ERROR) << StringPrintf("%s: fail cache NativeNfcTag", __func__);
    return JNI_FALSE;
  }

  if (nfc_jni_cache_object(e, gNativeP2pDeviceClassName,
                           &(nat->cached_P2pDevice)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail cache NativeP2pDevice", __func__);
    return JNI_FALSE;
  }

/* START [J14111104] - DTA */
#ifdef SEC_NFC_DTA_SUPPORT
    startDtaServer();
#endif
/* END [J14111104] - DTA */

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfaDeviceManagementCallback
**
** Description:     Receive device management events from stack.
**                  dmEvent: Device-management event ID.
**                  eventData: Data associated with event ID.
**
** Returns:         None
**
*******************************************************************************/
void nfaDeviceManagementCallback(uint8_t dmEvent,
                                 tNFA_DM_CBACK_DATA* eventData) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; event=0x%X", __func__, dmEvent);

  switch (dmEvent) {
    case NFA_DM_ENABLE_EVT: /* Result of NFA_Enable */
    {
      SyncEventGuard guard(sNfaEnableEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_ENABLE_EVT; status=0x%X", __func__, eventData->status);
      sIsNfaEnabled = eventData->status == NFA_STATUS_OK;
      sIsDisabling = false;
      sNfaEnableEvent.notifyOne();
    } break;

    case NFA_DM_DISABLE_EVT: /* Result of NFA_Disable */
    {
      SyncEventGuard guard(sNfaDisableEvent);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_DISABLE_EVT", __func__);
      sIsNfaEnabled = false;
      sIsDisabling = false;
      sNfaDisableEvent.notifyOne();
    } break;

    case NFA_DM_SET_CONFIG_EVT:  // result of NFA_SetConfig
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_SET_CONFIG_EVT", __func__);
      {
        SyncEventGuard guard(gNfaSetConfigEvent);
        gNfaSetConfigEvent.notifyOne();
      }
      break;

    case NFA_DM_GET_CONFIG_EVT: /* Result of NFA_GetConfig */
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_GET_CONFIG_EVT", __func__);
      {
        SyncEventGuard guard(gNfaGetConfigEvent);
        if (eventData->status == NFA_STATUS_OK &&
            eventData->get_config.tlv_size <= sizeof(gConfig)) {
          gCurrentConfigLen = eventData->get_config.tlv_size;
          memcpy(gConfig, eventData->get_config.param_tlvs,
                 eventData->get_config.tlv_size);
        } else {
          LOG(ERROR) << StringPrintf("%s: NFA_DM_GET_CONFIG failed", __func__);
          gCurrentConfigLen = 0;
        }
        gNfaGetConfigEvent.notifyOne();
      }
      break;

    case NFA_DM_RF_FIELD_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_RF_FIELD_EVT; status=0x%X; field status=%u", __func__,
          eventData->rf_field.status, eventData->rf_field.rf_field_status);
/* START [H180425] P Preview */
      if (sIsDisabling || !sIsNfaEnabled)
        break;
/* END [H180425] P Preview */
/* START [NFC2608] - Patch for NFC2608 */
      if (gIsSelectingRfInterface && NfcTag::getInstance().isActivated() )
      {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: gIsSelectingRfInterface = 0x%x", __func__,
            gIsSelectingRfInterface);
        gIsSelectingRfInterface = false;
        NfcTag::getInstance().setActive(false);
        nativeNfcTag_doDeactivateStatus(1);
      }
/* END [NFC2608] - Patch for NFC2608 */
      if (!sP2pActive && eventData->rf_field.status == NFA_STATUS_OK) {
        struct nfc_jni_native_data* nat = getNative(NULL, NULL);
        JNIEnv* e = NULL;
        ScopedAttach attach(nat->vm, &e);
        if (e == NULL) {
          LOG(ERROR) << StringPrintf("jni env is null");
          return;
        }
        if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON)
          e->CallVoidMethod(nat->manager,
                            android::gCachedNfcManagerNotifyRfFieldActivated);
        else
          e->CallVoidMethod(nat->manager,
                            android::gCachedNfcManagerNotifyRfFieldDeactivated);
      }
      break;

    case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
    case NFA_DM_NFCC_TIMEOUT_EVT: {
      if (dmEvent == NFA_DM_NFCC_TIMEOUT_EVT)
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TIMEOUT_EVT; abort",
                                   __func__);
      else if (dmEvent == NFA_DM_NFCC_TRANSPORT_ERR_EVT)
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TRANSPORT_ERR_EVT; abort",
                                   __func__);

      if (recovery_option) {
        struct nfc_jni_native_data* nat = getNative(NULL, NULL);
        JNIEnv* e = NULL;
        ScopedAttach attach(nat->vm, &e);
        if (e == NULL) {
          LOG(ERROR) << StringPrintf("jni env is null");
          return;
        }
        LOG(ERROR) << StringPrintf("%s: toggle NFC state to recovery nfc",
                                   __func__);
        sIsRecovering = true;
        e->CallVoidMethod(nat->manager,
                          android::gCachedNfcManagerNotifyHwErrorReported);
        {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: aborting  sNfaEnableDisablePollingEvent", __func__);
          SyncEventGuard guard(sNfaEnableDisablePollingEvent);
          sNfaEnableDisablePollingEvent.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: aborting  sNfaEnableEvent", __func__);
          SyncEventGuard guard(sNfaEnableEvent);
          sNfaEnableEvent.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: aborting  sNfaDisableEvent", __func__);
          SyncEventGuard guard(sNfaDisableEvent);
          sNfaDisableEvent.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: aborting  sNfaSetPowerSubState", __func__);
          SyncEventGuard guard(sNfaSetPowerSubState);
          sNfaSetPowerSubState.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: aborting gNfaSetConfigEvent", __func__);
          SyncEventGuard guard(gNfaSetConfigEvent);
          gNfaSetConfigEvent.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: aborting gNfaGetConfigEvent", __func__);
          SyncEventGuard guard(gNfaGetConfigEvent);
          gNfaGetConfigEvent.notifyOne();
        }
      } else {
        nativeNfcTag_abortWaits();
        NfcTag::getInstance().abort();
        sAbortConnlessWait = true;
        nativeLlcpConnectionlessSocket_abortWait();
        {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: aborting  sNfaEnableDisablePollingEvent", __func__);
          SyncEventGuard guard(sNfaEnableDisablePollingEvent);
          sNfaEnableDisablePollingEvent.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: aborting  sNfaEnableEvent", __func__);
          SyncEventGuard guard(sNfaEnableEvent);
          sNfaEnableEvent.notifyOne();
        }
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: aborting  sNfaDisableEvent", __func__);
          SyncEventGuard guard(sNfaDisableEvent);
          sNfaDisableEvent.notifyOne();
        }
        sDiscoveryEnabled = false;
        sPollingEnabled = false;
        PowerSwitch::getInstance().abort();

        if (!sIsDisabling && sIsNfaEnabled) {
          EXTNS_Close();
          NFA_Disable(FALSE);
          sIsDisabling = true;
        } else {
          sIsNfaEnabled = false;
          sIsDisabling = false;
        }
        PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
        LOG(ERROR) << StringPrintf("%s: crash NFC service", __func__);
        //////////////////////////////////////////////
        // crash the NFC service process so it can restart automatically
        abort();
        //////////////////////////////////////////////
      }
    } break;

    case NFA_DM_PWR_MODE_CHANGE_EVT:
      PowerSwitch::getInstance().deviceManagementCallback(dmEvent, eventData);
      break;

    case NFA_DM_SET_POWER_SUB_STATE_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%X",
                          __FUNCTION__, eventData->power_sub_state.status);
      SyncEventGuard guard(sNfaSetPowerSubState);
      sNfaSetPowerSubState.notifyOne();
    } break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unhandled event", __func__);
      break;
  }
}

/*******************************************************************************
**
** Function:        nfcManager_sendRawFrame
**
** Description:     Send a raw frame.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_sendRawFrame(JNIEnv* e, jobject, jbyteArray data) {
  ScopedByteArrayRO bytes(e, data);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  size_t bufLen = bytes.size();
  tNFA_STATUS status = NFA_SendRawFrame(buf, bufLen, 0);

  return (status == NFA_STATUS_OK);
}

/*******************************************************************************
**
** Function:        nfcManager_routeAid
**
** Description:     Route an AID to an EE
**                  e: JVM environment.
**                  aid: aid to be added to routing table.
**                  route: aid route location. i.e. DH/eSE/UICC
**                  aidInfo: prefix or suffix aid.
**
** Returns:         True if aid is accpted by NFA Layer.
**
*******************************************************************************/
static jboolean nfcManager_routeAid(JNIEnv* e, jobject, jbyteArray aid,
                                    jint route, jint aidInfo, jint power) {
  uint8_t* buf;
  size_t bufLen;

  // [SEC] aidInfo only supports at NCI2.0
  if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
    DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: convert aidInfo", __func__);
    aidInfo &= 0x0F;
  }

  if (NFC_GetNCIVersion() == NCI_VERSION_2_0 && route != 0x00) {
    route |= 0x80;
  }

  if (aid == NULL) {
    buf = NULL;
    bufLen = 0;
    return RoutingManager::getInstance().addAidRouting(buf, bufLen, route,
                                                       aidInfo, power);
  }
  ScopedByteArrayRO bytes(e, aid);
  buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  bufLen = bytes.size();
  return RoutingManager::getInstance().addAidRouting(buf, bufLen, route,
                                                     aidInfo, power);
}

/*******************************************************************************
**
** Function:        nfcManager_unrouteAid
**
** Description:     Remove a AID routing
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_unrouteAid(JNIEnv* e, jobject, jbyteArray aid) {
  uint8_t* buf;
  size_t bufLen;

  if (aid == NULL) {
    buf = NULL;
    bufLen = 0;
    return RoutingManager::getInstance().removeAidRouting(buf, bufLen);
  }
  ScopedByteArrayRO bytes(e, aid);
  buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  bufLen = bytes.size();
  return RoutingManager::getInstance().removeAidRouting(buf, bufLen);
}

/*******************************************************************************
**
** Function:        nfcManager_commitRouting
**
** Description:     Sends the AID routing table to the controller
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_commitRouting(JNIEnv* e, jobject) {
  if (sRfEnabled) {
    /*Update routing table only in Idle state.*/
    startRfDiscovery(false);
  }
  jboolean commitStatus = RoutingManager::getInstance().commitRouting();
  startRfDiscovery(true);
  return commitStatus;
}

/*******************************************************************************
**
** Function:        nfcManager_doRegisterT3tIdentifier
**
** Description:     Registers LF_T3T_IDENTIFIER for NFC-F.
**                  e: JVM environment.
**                  o: Java object.
**                  t3tIdentifier: LF_T3T_IDENTIFIER value (10 or 18 bytes)
**
** Returns:         Handle retrieve from RoutingManager.
**
*******************************************************************************/
static jint nfcManager_doRegisterT3tIdentifier(JNIEnv* e, jobject,
                                               jbyteArray t3tIdentifier) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  ScopedByteArrayRO bytes(e, t3tIdentifier);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  size_t bufLen = bytes.size();
  int handle = RoutingManager::getInstance().registerT3tIdentifier(buf, bufLen);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: handle=%d", __func__, handle);
  if (handle != NFA_HANDLE_INVALID)
    RoutingManager::getInstance().commitRouting();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return handle;
}

/*******************************************************************************
**
** Function:        nfcManager_doDeregisterT3tIdentifier
**
** Description:     Deregisters LF_T3T_IDENTIFIER for NFC-F.
**                  e: JVM environment.
**                  o: Java object.
**                  handle: Handle retrieve from libnfc-nci.
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_doDeregisterT3tIdentifier(JNIEnv*, jobject,
                                                 jint handle) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; handle=%d", __func__, handle);

  RoutingManager::getInstance().deregisterT3tIdentifier(handle);
  RoutingManager::getInstance().commitRouting();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        nfcManager_getLfT3tMax
**
** Description:     Returns LF_T3T_MAX value.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         LF_T3T_MAX value.
**
*******************************************************************************/
static jint nfcManager_getLfT3tMax(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("LF_T3T_MAX=%d", sLfT3tMax);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return sLfT3tMax;
}

/*******************************************************************************
**
** Function:        nfcManager_doInitialize
**
** Description:     Turn on NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_doInitialize(JNIEnv* e, jobject o) {
  initializeGlobalDebugEnabledFlag();
  tNFA_STATUS stat = NFA_STATUS_OK;
  sIsRecovering = false;

  PowerSwitch& powerSwitch = PowerSwitch::getInstance();

  if (sIsNfaEnabled) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: already enabled", __func__);
    goto TheEnd;
  }

  powerSwitch.initialize(PowerSwitch::FULL_POWER);

  {

    NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
    theInstance.Initialize();  // start GKI, NCI task, NFC task

    {
      SyncEventGuard guard(sNfaEnableEvent);
      tHAL_NFC_ENTRY* halFuncEntries = theInstance.GetHalEntryFuncs();

      NFA_Init(halFuncEntries);

      stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
      if (stat == NFA_STATUS_OK) {
        sNfaEnableEvent.wait();  // wait for NFA command to finish
      }
      EXTNS_Init(nfaDeviceManagementCallback, nfaConnectionCallback);
    }

    if (stat == NFA_STATUS_OK) {
      // sIsNfaEnabled indicates whether stack started successfully
      if (sIsNfaEnabled) {
        sRoutingInitialized =
            RoutingManager::getInstance().initialize(getNative(e, o));
        nativeNfcTag_registerNdefTypeHandler();
        NfcTag::getInstance().initialize(getNative(e, o));
        PeerToPeer::getInstance().initialize();
        PeerToPeer::getInstance().handleNfcOnOff(true);
        HciEventManager::getInstance().initialize(getNative(e, o));

        /////////////////////////////////////////////////////////////////////////////////
        // Add extra configuration here (work-arounds, etc.)

        if (gIsDtaEnabled == true) {
          uint8_t configData = 0;
          configData = 0x01; /* Poll NFC-DEP : Highest Available Bit Rates */
          NFA_SetConfig(NCI_PARAM_ID_BITR_NFC_DEP, sizeof(uint8_t),
                        &configData);
          configData = 0x0B; /* Listen NFC-DEP : Waiting Time */
          NFA_SetConfig(NFC_PMID_WT, sizeof(uint8_t), &configData);
          configData = 0x0F; /* Specific Parameters for NFC-DEP RF Interface */
          NFA_SetConfig(NCI_PARAM_ID_NFC_DEP_OP, sizeof(uint8_t), &configData);
        }

        struct nfc_jni_native_data* nat = getNative(e, o);
        if (nat) {
          nat->tech_mask =
              NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: tag polling tech mask=0x%X", __func__, nat->tech_mask);
        }

        // if this value exists, set polling interval.
        nat->discovery_duration = NfcConfig::getUnsigned(
            NAME_NFA_DM_DISC_DURATION_POLL, DEFAULT_DISCOVERY_DURATION);

        NFA_SetRfDiscoveryDuration(nat->discovery_duration);

        // get LF_T3T_MAX
        {
          SyncEventGuard guard(gNfaGetConfigEvent);
          tNFA_PMID configParam[1] = {NCI_PARAM_ID_LF_T3T_MAX};
          stat = NFA_GetConfig(1, configParam);
          if (stat == NFA_STATUS_OK) {
            gNfaGetConfigEvent.wait();
            if (gCurrentConfigLen >= 4 ||
                gConfig[1] == NCI_PARAM_ID_LF_T3T_MAX) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: lfT3tMax=%d", __func__, gConfig[3]);
              sLfT3tMax = gConfig[3];
            }
          }
        }

        prevScreenState = NFA_SCREEN_STATE_OFF_LOCKED;

        // Do custom NFCA startup configuration.
        doStartupConfig();

        // [SEC] Set AID matching mode
        doSetAidMatchingMode();

#ifdef DTA_ENABLED
        NfcDta::getInstance().setNfccConfigParams();
#endif /* DTA_ENABLED */
        goto TheEnd;
      }
    }

    LOG(ERROR) << StringPrintf("%s: fail nfa enable; error=0x%X", __func__,
                               stat);

    if (sIsNfaEnabled) {
      EXTNS_Close();
      stat = NFA_Disable(FALSE /* ungraceful */);
    }

    theInstance.Finalize();
  }

TheEnd:
  if (sIsNfaEnabled)
    PowerSwitch::getInstance().setLevel(PowerSwitch::LOW_POWER);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return sIsNfaEnabled ? JNI_TRUE : JNI_FALSE;
}

static void nfcManager_doEnableDtaMode(JNIEnv*, jobject) {
  gIsDtaEnabled = true;
}

static void nfcManager_doDisableDtaMode(JNIEnv*, jobject) {
  gIsDtaEnabled = false;
}

static void nfcManager_doFactoryReset(JNIEnv*, jobject) {
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.FactoryReset();
}

static void nfcManager_doShutdown(JNIEnv*, jobject, jboolean disableCe) {
  if (disableCe) {
    RoutingManager::getInstance().onNfccShutdown();
  } else {
    {
      SyncEventGuard guard (sNfaVsCmdEvent);
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Set SIMVDD2 to external", __func__);
      uint8_t vdd_src = 0x01; /*  0: Internal, 1: External */
      if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_SET_SIMVDD2_OID, 1, &vdd_src, vsCmdCallback))
        sNfaVsCmdEvent.wait (1000);
    }
  }
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.DeviceShutdown();
}
#ifndef NFC_SEC_NOT_OPEN_INCLUDED
static void nfcManager_configNfccConfigControl(bool flag) {
    // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
    if (NFC_GetNCIVersion() != NCI_VERSION_1_0) {
        uint8_t nfa_set_config[] = { 0x00 };

        nfa_set_config[0] = (flag == true ? 1 : 0);

        tNFA_STATUS status = NFA_SetConfig(NCI_PARAM_ID_NFCC_CONFIG_CONTROL,
                                           sizeof(nfa_set_config),
                                           &nfa_set_config[0]);
        if (status != NFA_STATUS_OK) {
            LOG(ERROR) << __func__
            << ": Failed to configure NFCC_CONFIG_CONTROL";
        }
    }
}
#endif

/*******************************************************************************
**
** Function:        nfcManager_enableDiscovery
**
** Description:     Start polling and listening for devices.
**                  e: JVM environment.
**                  o: Java object.
**                  technologies_mask: the bitmask of technologies for which to
*enable discovery
**                  enable_lptd: whether to enable low power polling (default:
*false)
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_enableDiscovery(JNIEnv* e, jobject o,
                                       jint technologies_mask,
                                       jboolean enable_lptd,
                                       jboolean reader_mode,
                                       jboolean enable_host_routing,
                                       jboolean enable_p2p, jboolean restart) {
  tNFA_TECHNOLOGY_MASK tech_mask = DEFAULT_TECH_MASK;
  struct nfc_jni_native_data* nat = getNative(e, o);

  if (technologies_mask == -1 && nat)
    tech_mask = (tNFA_TECHNOLOGY_MASK)nat->tech_mask;
  else if (technologies_mask != -1)
    tech_mask = (tNFA_TECHNOLOGY_MASK)technologies_mask;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; tech_mask = %02x", __func__, tech_mask);

  if (sDiscoveryEnabled && !restart) {
    LOG(ERROR) << StringPrintf("%s: already discovering", __func__);
    return;
  }

  PowerSwitch::getInstance().setLevel(PowerSwitch::FULL_POWER);

  if (sRfEnabled) {
    // Stop RF discovery to reconfigure
    startRfDiscovery(false);
  }

  // Check polling configuration
  if (tech_mask != 0) {
    stopPolling_rfDiscoveryDisabled();
    startPolling_rfDiscoveryDisabled(tech_mask);

    // Start P2P listening if tag polling was enabled
    if (sPollingEnabled) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Enable p2pListening", __func__);

      if (enable_p2p && !sP2pEnabled) {
        sP2pEnabled = true;
        PeerToPeer::getInstance().enableP2pListening(true);
        NFA_ResumeP2p();
      } else if (!enable_p2p && sP2pEnabled) {
        sP2pEnabled = false;
        PeerToPeer::getInstance().enableP2pListening(false);
        NFA_PauseP2p();
      }

      if (reader_mode && !sReaderModeEnabled) {
        sReaderModeEnabled = true;
        NFA_DisableListening();

#ifndef NFC_SEC_NOT_OPEN_INCLUDED
        // configure NFCC_CONFIG_CONTROL- NFCC not allowed to manage RF configuration.
        nfcManager_configNfccConfigControl(false);
#endif

        NFA_SetRfDiscoveryDuration(READER_MODE_DISCOVERY_DURATION);
      } else if (!reader_mode && sReaderModeEnabled) {
        struct nfc_jni_native_data* nat = getNative(e, o);
        sReaderModeEnabled = false;
        NFA_EnableListening();

#ifndef NFC_SEC_NOT_OPEN_INCLUDED
        // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
        nfcManager_configNfccConfigControl(true);
#endif

        NFA_SetRfDiscoveryDuration(nat->discovery_duration);
      }
    }
  } else {
    /* enable_p2p=> request to enable p2p, P2pEnabled=> current state of p2p */
    if (enable_p2p && !sP2pEnabled) {
      sP2pEnabled = true;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Enable p2pListening", __func__);
      PeerToPeer::getInstance().enableP2pListening(true);
      NFA_ResumeP2p();
    } else if (!enable_p2p && sP2pEnabled) {
      sP2pEnabled = false;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Disable p2pListening", __func__);
      PeerToPeer::getInstance().enableP2pListening(false);
      NFA_PauseP2p();
    }
    // No technologies configured, stop polling
    stopPolling_rfDiscoveryDisabled();
  }

  // Check listen configuration
  if (enable_host_routing) {
    RoutingManager::getInstance().enableRoutingToHost();
    RoutingManager::getInstance().commitRouting();
  } else {
    RoutingManager::getInstance().disableRoutingToHost();
    RoutingManager::getInstance().commitRouting();
  }
  // Actually start discovery.
  startRfDiscovery(true);
  sDiscoveryEnabled = true;

  PowerSwitch::getInstance().setModeOn(PowerSwitch::DISCOVERY);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        nfcManager_disableDiscovery
**
** Description:     Stop polling and listening for devices.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None
**
*******************************************************************************/
void nfcManager_disableDiscovery(JNIEnv* e, jobject o) {
  tNFA_STATUS status = NFA_STATUS_OK;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  if (sDiscoveryEnabled == false) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: already disabled", __func__);
    goto TheEnd;
  }

  // Stop RF Discovery.
  startRfDiscovery(false);

  if (sPollingEnabled) status = stopPolling_rfDiscoveryDisabled();

  PeerToPeer::getInstance().enableP2pListening(false);
  sP2pEnabled = false;
  sDiscoveryEnabled = false;
  // if nothing is active after this, then tell the controller to power down
  if (!PowerSwitch::getInstance().setModeOff(PowerSwitch::DISCOVERY))
    PowerSwitch::getInstance().setLevel(PowerSwitch::LOW_POWER);
TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit: Status = 0x%X", __func__, status);
}

/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpServiceSocket
**
** Description:     Create a new LLCP server socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  sn: Service name
**                  miu: Maximum information unit.
**                  rw: Receive window size.
**                  linearBufferLength: Max buffer size.
**
** Returns:         NativeLlcpServiceSocket Java object.
**
*******************************************************************************/
static jobject nfcManager_doCreateLlcpServiceSocket(JNIEnv* e, jobject,
                                                    jint nSap, jstring sn,
                                                    jint miu, jint rw,
                                                    jint linearBufferLength) {
  PeerToPeer::tJNI_HANDLE jniHandle =
      PeerToPeer::getInstance().getNewJniHandle();

  ScopedUtfChars serviceName(e, sn);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter: sap=%i; name=%s; miu=%i; rw=%i; buffLen=%i", __func__, nSap,
      serviceName.c_str(), miu, rw, linearBufferLength);

  /* Create new NativeLlcpServiceSocket object */
  jobject serviceSocket = NULL;
  if (nfc_jni_cache_object_local(e, gNativeLlcpServiceSocketClassName,
                                 &(serviceSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: Llcp socket object creation error",
                               __func__);
    return NULL;
  }

  /* Get NativeLlcpServiceSocket class object */
  ScopedLocalRef<jclass> clsNativeLlcpServiceSocket(
      e, e->GetObjectClass(serviceSocket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: Llcp Socket get object class error",
                               __func__);
    return NULL;
  }

  if (!PeerToPeer::getInstance().registerServer(jniHandle,
                                                serviceName.c_str())) {
    LOG(ERROR) << StringPrintf("%s: RegisterServer error", __func__);
    return NULL;
  }

  jfieldID f;

  /* Set socket handle to be the same as the NfaHandle*/
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mHandle", "I");
  e->SetIntField(serviceSocket, f, (jint)jniHandle);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: socket Handle = 0x%X", __func__, jniHandle);

  /* Set socket linear buffer length */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(),
                    "mLocalLinearBufferLength", "I");
  e->SetIntField(serviceSocket, f, (jint)linearBufferLength);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: buffer length = %d", __func__, linearBufferLength);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mLocalMiu", "I");
  e->SetIntField(serviceSocket, f, (jint)miu);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: MIU = %d", __func__, miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mLocalRw", "I");
  e->SetIntField(serviceSocket, f, (jint)rw);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s:  RW = %d", __func__, rw);

  sLastError = 0;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return serviceSocket;
}

/*******************************************************************************
**
** Function:        nfcManager_doGetLastError
**
** Description:     Get the last error code.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Last error code.
**
*******************************************************************************/
static jint nfcManager_doGetLastError(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: last error=%i", __func__, sLastError);
  return sLastError;
}

/*******************************************************************************
**
** Function:        nfcManager_doDeinitialize
**
** Description:     Turn off NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_doDeinitialize(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  sIsDisabling = true;

  if (!recovery_option || !sIsRecovering) {
    RoutingManager::getInstance().onNfccShutdown();
  }
  PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
  HciEventManager::getInstance().finalize();

  {
    SyncEventGuard guard (sNfaVsCmdEvent);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Set SIMVDD2 to external", __func__);
    uint8_t vdd_src = 0x01; /*  0: Internal, 1: External */
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_SET_SIMVDD2_OID, 1, &vdd_src, vsCmdCallback))
      sNfaVsCmdEvent.wait (1000);
  }

  if (sIsNfaEnabled) {
    SyncEventGuard guard(sNfaDisableEvent);
    EXTNS_Close();
    tNFA_STATUS stat = NFA_Disable(TRUE /* graceful */);
    if (stat == NFA_STATUS_OK) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: wait for completion", __func__);
      sNfaDisableEvent.wait();  // wait for NFA command to finish
      PeerToPeer::getInstance().handleNfcOnOff(false);
    } else {
      LOG(ERROR) << StringPrintf("%s: fail disable; error=0x%X", __func__,
                                 stat);
    }
  }
  nativeNfcTag_abortWaits();
  NfcTag::getInstance().abort();
  RoutingManager::getInstance().abort();
  sAbortConnlessWait = true;
  nativeLlcpConnectionlessSocket_abortWait();
  sIsNfaEnabled = false;
  sRoutingInitialized = false;
  sDiscoveryEnabled = false;
  sPollingEnabled = false;
  sIsDisabling = false;
  sP2pEnabled = false;
  sReaderModeEnabled = false;
  gActivated = false;
  sLfT3tMax = 0;
/* START [J14121204] - clear sRfEnabled */
  sRfEnabled = false;
/* END [J14121204] */

/* START [J14111106] - Finalize race conditon */
  // Moved from the end of the function.
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Finalize();
  {
    // unblock NFA_EnablePolling() and NFA_DisablePolling()
    SyncEventGuard guard(sNfaEnableDisablePollingEvent);
    sNfaEnableDisablePollingEvent.notifyOne();
  }
/* END [J14111106] - Finalize race conditon */
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpSocket
**
** Description:     Create a LLCP connection-oriented socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  miu: Maximum information unit.
**                  rw: Receive window size.
**                  linearBufferLength: Max buffer size.
**
** Returns:         NativeLlcpSocket Java object.
**
*******************************************************************************/
static jobject nfcManager_doCreateLlcpSocket(JNIEnv* e, jobject, jint nSap,
                                             jint miu, jint rw,
                                             jint linearBufferLength) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; sap=%d; miu=%d; rw=%d; buffer len=%d",
                      __func__, nSap, miu, rw, linearBufferLength);

  PeerToPeer::tJNI_HANDLE jniHandle =
      PeerToPeer::getInstance().getNewJniHandle();
  PeerToPeer::getInstance().createClient(jniHandle, miu, rw);

  /* Create new NativeLlcpSocket object */
  jobject clientSocket = NULL;
  if (nfc_jni_cache_object_local(e, gNativeLlcpSocketClassName,
                                 &(clientSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail Llcp socket creation", __func__);
    return clientSocket;
  }

  /* Get NativeConnectionless class object */
  ScopedLocalRef<jclass> clsNativeLlcpSocket(e,
                                             e->GetObjectClass(clientSocket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: fail get class object", __func__);
    return clientSocket;
  }

  jfieldID f;

  /* Set socket SAP */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mSap", "I");
  e->SetIntField(clientSocket, f, (jint)nSap);

  /* Set socket handle */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mHandle", "I");
  e->SetIntField(clientSocket, f, (jint)jniHandle);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mLocalMiu", "I");
  e->SetIntField(clientSocket, f, (jint)miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mLocalRw", "I");
  e->SetIntField(clientSocket, f, (jint)rw);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return clientSocket;
}

/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpConnectionlessSocket
**
** Description:     Create a connection-less socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  sn: Service name.
**
** Returns:         NativeLlcpConnectionlessSocket Java object.
**
*******************************************************************************/
static jobject nfcManager_doCreateLlcpConnectionlessSocket(JNIEnv*, jobject,
                                                           jint nSap,
                                                           jstring /*sn*/) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: nSap=0x%X", __func__, nSap);
  return NULL;
}

/*******************************************************************************
**
** Function:        isPeerToPeer
**
** Description:     Whether the activation data indicates the peer supports
*NFC-DEP.
**                  activated: Activation data.
**
** Returns:         True if the peer supports NFC-DEP.
**
*******************************************************************************/
static bool isPeerToPeer(tNFA_ACTIVATED& activated) {
  return activated.activate_ntf.protocol == NFA_PROTOCOL_NFC_DEP;
}

/*******************************************************************************
**
** Function:        isListenMode
**
** Description:     Indicates whether the activation data indicates it is
**                  listen mode.
**
** Returns:         True if this listen mode.
**
*******************************************************************************/
static bool isListenMode(tNFA_ACTIVATED& activated) {
  return (
      (NFC_DISCOVERY_TYPE_LISTEN_A ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_B ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_F ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_ISO15693 ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_DISCOVERY_TYPE_LISTEN_B_PRIME ==
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_INTERFACE_EE_DIRECT_RF == activated.activate_ntf.intf_param.type));
}

/*******************************************************************************
**
** Function:        nfcManager_doCheckLlcp
**
** Description:     Not used.
**
** Returns:         True
**
*******************************************************************************/
static jboolean nfcManager_doCheckLlcp(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doActivateLlcp
**
** Description:     Not used.
**
** Returns:         True
**
*******************************************************************************/
static jboolean nfcManager_doActivateLlcp(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doAbort
**
** Description:     Not used.
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_doAbort(JNIEnv* e, jobject, jstring msg) {
  ScopedUtfChars message = {e, msg};
  e->FatalError(message.c_str());
  abort();  // <-- Unreachable
}

/*******************************************************************************
**
** Function:        nfcManager_doDownload
**
** Description:     Download firmware patch files.  Do not turn on NFC.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nfcManager_doDownload(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  bool result = JNI_FALSE;
  theInstance.Initialize();  // start GKI, NCI task, NFC task
  result = theInstance.DownloadFirmware();
  theInstance.Finalize();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return result;
}

/*******************************************************************************
**
** Function:        nfcManager_doResetTimeouts
**
** Description:     Not used.
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_doResetTimeouts(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  NfcTag::getInstance().resetAllTransceiveTimeouts();
}

/*******************************************************************************
**
** Function:        nfcManager_doSetTimeout
**
** Description:     Set timeout value.
**                  e: JVM environment.
**                  o: Java object.
**                  tech: technology ID.
**                  timeout: Timeout value.
**
** Returns:         True if ok.
**
*******************************************************************************/
static bool nfcManager_doSetTimeout(JNIEnv*, jobject, jint tech, jint timeout) {
  if (timeout <= 0) {
    LOG(ERROR) << StringPrintf("%s: Timeout must be positive.", __func__);
    return false;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tech=%d, timeout=%d", __func__, tech, timeout);
  NfcTag::getInstance().setTransceiveTimeout(tech, timeout);
  return true;
}

/*******************************************************************************
**
** Function:        nfcManager_doGetTimeout
**
** Description:     Get timeout value.
**                  e: JVM environment.
**                  o: Java object.
**                  tech: technology ID.
**
** Returns:         Timeout value.
**
*******************************************************************************/
static jint nfcManager_doGetTimeout(JNIEnv*, jobject, jint tech) {
  int timeout = NfcTag::getInstance().getTransceiveTimeout(tech);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tech=%d, timeout=%d", __func__, tech, timeout);
  return timeout;
}

/*******************************************************************************
**
** Function:        nfcManager_doDump
**
** Description:     Get libnfc-nci dump
**                  e: JVM environment.
**                  obj: Java object.
**                  fdobj: File descriptor to be used
**
** Returns:         Void
**
*******************************************************************************/
static void nfcManager_doDump(JNIEnv* e, jobject obj, jobject fdobj) {
  int fd = jniGetFDFromFileDescriptor(e, fdobj);
  if (fd < 0) return;

  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Dump(fd);
}

static jint nfcManager_doGetNciVersion(JNIEnv*, jobject) {
  return NFC_GetNCIVersion();
}

static void nfcManager_doSetScreenState(JNIEnv* e, jobject o,
                                        jint screen_state_mask) {
  tNFA_STATUS status = NFA_STATUS_OK;
  uint8_t state = (screen_state_mask & NFA_SCREEN_STATE_MASK);
  uint8_t discovry_param =
      NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: state = %d prevScreenState= %d, discovry_param = %d",
                      __FUNCTION__, state, prevScreenState, discovry_param);

  if (prevScreenState == state) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "New screen state is same as previous state. No action taken");
    return;
  }

  // [SEC] Supports screen sub state for power mode
  if (sIsDisabling || !sIsNfaEnabled) {
    prevScreenState = state;
    return;
  } else if(NFC_GetNCIVersion() != NCI_VERSION_2_0) {
    status = nfcManager_setPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail SetScreenState; error=0x%X",
                                __FUNCTION__, status);
      return;
    }
    prevScreenState = state;
    return;
  }

  // skip remaining SetScreenState tasks when trying to silent recover NFCC
  if (recovery_option && sIsRecovering) {
    prevScreenState = state;
    return;
  }

  if (prevScreenState == NFA_SCREEN_STATE_OFF_LOCKED ||
      prevScreenState == NFA_SCREEN_STATE_OFF_UNLOCKED ||
      prevScreenState == NFA_SCREEN_STATE_ON_LOCKED) {
    SyncEventGuard guard(sNfaSetPowerSubState);
    status = NFA_SetPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                                 __FUNCTION__, status);
      return;
    } else {
      sNfaSetPowerSubState.wait();
    }
  }

  // skip remaining SetScreenState tasks when trying to silent recover NFCC
  if (recovery_option && sIsRecovering) {
    prevScreenState = state;
    return;
  }

  if (state == NFA_SCREEN_STATE_OFF_LOCKED ||
      state == NFA_SCREEN_STATE_OFF_UNLOCKED) {
    // disable poll and enable listen on DH 0x00
    discovry_param =
        NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
  }

  if (state == NFA_SCREEN_STATE_ON_LOCKED) {
    // disable poll and enable listen on DH 0x00
    discovry_param =
        (screen_state_mask & NFA_SCREEN_POLLING_TAG_MASK)
            ? (NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK)
            : (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
  }

  if (state == NFA_SCREEN_STATE_ON_UNLOCKED) {
    // enable both poll and listen on DH 0x01
    discovry_param =
        NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;
  }

  SyncEventGuard guard(gNfaSetConfigEvent);
  status = NFA_SetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
                         NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discovry_param);
  if (status == NFA_STATUS_OK) {
    gNfaSetConfigEvent.wait();
  } else {
    LOG(ERROR) << StringPrintf("%s: Failed to update CON_DISCOVER_PARAM",
                               __FUNCTION__);
    return;
  }

  // skip remaining SetScreenState tasks when trying to silent recover NFCC
  if (recovery_option && sIsRecovering) {
    prevScreenState = state;
    return;
  }

  if (prevScreenState == NFA_SCREEN_STATE_ON_UNLOCKED) {
    SyncEventGuard guard(sNfaSetPowerSubState);
    status = NFA_SetPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                                 __FUNCTION__, status);
    } else {
      sNfaSetPowerSubState.wait();
    }
  }

  // skip remaining SetScreenState tasks when trying to silent recover NFCC
  if (recovery_option && sIsRecovering) {
    prevScreenState = state;
    return;
  }

  if ((state == NFA_SCREEN_STATE_OFF_LOCKED ||
       state == NFA_SCREEN_STATE_OFF_UNLOCKED) &&
      (prevScreenState == NFA_SCREEN_STATE_ON_UNLOCKED ||
       prevScreenState == NFA_SCREEN_STATE_ON_LOCKED) &&
      (!sP2pActive) && (!sSeRfActive)) {
    // screen turns off, disconnect tag if connected
    nativeNfcTag_doDisconnect(NULL, NULL);
  }

  prevScreenState = state;
}
/*******************************************************************************
**
** Function:        nfcManager_doSetP2pInitiatorModes
**
** Description:     Set P2P initiator's activation modes.
**                  e: JVM environment.
**                  o: Java object.
**                  modes: Active and/or passive modes.  The values are
*specified
**                          in external/libnfc-nxp/inc/phNfcTypes.h.  See
**                          enum phNfc_eP2PMode_t.
**
** Returns:         None.
**
*******************************************************************************/
static void nfcManager_doSetP2pInitiatorModes(JNIEnv* e, jobject o,
                                              jint modes) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modes=0x%X", __func__, modes);
  struct nfc_jni_native_data* nat = getNative(e, o);

  tNFA_TECHNOLOGY_MASK mask = 0;
  if (modes & 0x01) mask |= NFA_TECHNOLOGY_MASK_A;
  if (modes & 0x02) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x04) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x08) mask |= NFA_TECHNOLOGY_MASK_A_ACTIVE;
  if (modes & 0x10) mask |= NFA_TECHNOLOGY_MASK_F_ACTIVE;
  if (modes & 0x20) mask |= NFA_TECHNOLOGY_MASK_F_ACTIVE;
  nat->tech_mask = mask;
}

/*******************************************************************************
**
** Function:        nfcManager_doSetP2pTargetModes
**
** Description:     Set P2P target's activation modes.
**                  e: JVM environment.
**                  o: Java object.
**                  modes: Active and/or passive modes.
**
** Returns:         None.
**
*******************************************************************************/
static void nfcManager_doSetP2pTargetModes(JNIEnv*, jobject, jint modes) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modes=0x%X", __func__, modes);
  // Map in the right modes
  tNFA_TECHNOLOGY_MASK mask = 0;
  if (modes & 0x01) mask |= NFA_TECHNOLOGY_MASK_A;
  if (modes & 0x02) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x04) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x08)
    mask |= NFA_TECHNOLOGY_MASK_A_ACTIVE | NFA_TECHNOLOGY_MASK_F_ACTIVE;

  PeerToPeer::getInstance().setP2pListenMask(mask);
}

static void nfcManager_doEnableScreenOffSuspend(JNIEnv* e, jobject o) {
  PowerSwitch::getInstance().setScreenOffPowerState(
      PowerSwitch::POWER_STATE_FULL);
}

static void nfcManager_doDisableScreenOffSuspend(JNIEnv* e, jobject o) {
  PowerSwitch::getInstance().setScreenOffPowerState(
      PowerSwitch::POWER_STATE_OFF);
}

/*******************************************************************************
**
** Function:        nfcManager_getIsoDepMaxTransceiveLength
**
** Description:     Get maximum ISO DEP Transceive Length supported by the NFC
**                  chip. Returns default 261 bytes if the property is not set.
**
** Returns:         max value.
**
*******************************************************************************/
static jint nfcManager_getIsoDepMaxTransceiveLength(JNIEnv*, jobject) {
  /* Check if extended APDU is supported by the chip.
   * If not, default value is returned.
   * The maximum length of a default IsoDep frame consists of:
   * CLA, INS, P1, P2, LC, LE + 255 payload bytes = 261 bytes
   */
  return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE, 261);
}

/*******************************************************************************
 **
 ** Function:        nfcManager_getAidTableSize
 ** Description:     Get the maximum supported size for AID routing table.
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **
 *******************************************************************************/
static jint nfcManager_getAidTableSize(JNIEnv*, jobject) {
  return NFA_GetAidTableSize();
}

/*******************************************************************************
**
** Function:        nfcManager_doStartStopPolling
**
** Description:     Start or stop NFC RF polling
**                  e: JVM environment.
**                  o: Java object.
**                  start: start or stop RF polling
**
** Returns:         None
**
*******************************************************************************/
static void nfcManager_doStartStopPolling(JNIEnv* e, jobject o,
                                          jboolean start) {
  if (!sIsDisabling && sIsNfaEnabled)
    startStopPolling(start);
}

/*******************************************************************************
**
** Function:        nfcManager_doSetNfcSecure
**
** Description:     Set NfcSecure enable/disable.
**                  e: JVM environment.
**                  o: Java object.
**                  enable: Sets true/false to enable/disable NfcSecure
**                  It only updates the routing table cache without commit to
**                  NFCC.
**
** Returns:         True always
**
*******************************************************************************/
static jboolean nfcManager_doSetNfcSecure(JNIEnv* e, jobject o,
                                          jboolean enable) {
  RoutingManager& routingManager = RoutingManager::getInstance();
  routingManager.setNfcSecure(enable);
  if (sRoutingInitialized) {
    routingManager.disableRoutingToHost();
    routingManager.updateRoutingTable();
    routingManager.enableRoutingToHost();
  }
  return true;
}

static jstring nfcManager_doGetNfaStorageDir(JNIEnv* e, jobject o) {
  string nfaStorageDir = NfcConfig::getString(NAME_NFA_STORAGE, "/data/nfc");
  return e->NewStringUTF(nfaStorageDir.c_str());
}

static void nfcManager_doSetNfceePowerAndLinkCtrl(JNIEnv* e, jobject o,
                                                  jboolean enable) {
  RoutingManager& routingManager = RoutingManager::getInstance();
  if (enable) {
    routingManager.eeSetPwrAndLinkCtrl((uint8_t)nfcee_power_and_link_conf);
  } else {
    routingManager.eeSetPwrAndLinkCtrl(0);
  }
}

/*******************************************************************************
**
** Function:        nfcManager_doGetMaxRoutingTableSize
**
** Description:     Retrieve the max routing table size from cache
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Max Routing Table size
**
*******************************************************************************/
static jint nfcManager_doGetMaxRoutingTableSize(JNIEnv* e, jobject o) {
  return lmrt_get_max_size();
}

/*******************************************************************************
**
** Function:        nfcManager_doGetRoutingTable
**
** Description:     Retrieve the committed listen mode routing configuration
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Committed listen mode routing configuration
**
*******************************************************************************/
static jbyteArray nfcManager_doGetRoutingTable(JNIEnv* e, jobject o) {
  std::vector<uint8_t>* routingTable = lmrt_get_tlvs();

  CHECK(e);
  jbyteArray rtJavaArray = e->NewByteArray((*routingTable).size());
  CHECK(rtJavaArray);
  e->SetByteArrayRegion(rtJavaArray, 0, (*routingTable).size(),
                        (jbyte*)&(*routingTable)[0]);

  return rtJavaArray;
}

/*****************************************************************************
**
** JNI functions for android-4.0.1_r1
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doDownload", "()Z", (void*)nfcManager_doDownload},

    {"initializeNativeStructure", "()Z", (void*)nfcManager_initNativeStruc},

    {"doInitialize", "()Z", (void*)nfcManager_doInitialize},

    {"doDeinitialize", "()Z", (void*)nfcManager_doDeinitialize},

    {"sendRawFrame", "([B)Z", (void*)nfcManager_sendRawFrame},

    {"routeAid", "([BIII)Z", (void*)nfcManager_routeAid},

    {"unrouteAid", "([B)Z", (void*)nfcManager_unrouteAid},

    {"commitRouting", "()Z", (void*)nfcManager_commitRouting},

    {"doRegisterT3tIdentifier", "([B)I",
     (void*)nfcManager_doRegisterT3tIdentifier},

    {"doDeregisterT3tIdentifier", "(I)V",
     (void*)nfcManager_doDeregisterT3tIdentifier},

    {"getLfT3tMax", "()I", (void*)nfcManager_getLfT3tMax},

    {"doEnableDiscovery", "(IZZZZZ)V", (void*)nfcManager_enableDiscovery},

    {"doStartStopPolling", "(Z)V", (void*)nfcManager_doStartStopPolling},

    {"doCheckLlcp", "()Z", (void*)nfcManager_doCheckLlcp},

    {"doActivateLlcp", "()Z", (void*)nfcManager_doActivateLlcp},

    {"doCreateLlcpConnectionlessSocket",
     "(ILjava/lang/String;)Lcom/android/nfc/dhimpl/"
     "NativeLlcpConnectionlessSocket;",
     (void*)nfcManager_doCreateLlcpConnectionlessSocket},

    {"doCreateLlcpServiceSocket",
     "(ILjava/lang/String;III)Lcom/android/nfc/dhimpl/NativeLlcpServiceSocket;",
     (void*)nfcManager_doCreateLlcpServiceSocket},

    {"doCreateLlcpSocket", "(IIII)Lcom/android/nfc/dhimpl/NativeLlcpSocket;",
     (void*)nfcManager_doCreateLlcpSocket},

    {"doGetLastError", "()I", (void*)nfcManager_doGetLastError},

    {"disableDiscovery", "()V", (void*)nfcManager_disableDiscovery},

    {"doSetTimeout", "(II)Z", (void*)nfcManager_doSetTimeout},

    {"doGetTimeout", "(I)I", (void*)nfcManager_doGetTimeout},

    {"doResetTimeouts", "()V", (void*)nfcManager_doResetTimeouts},

    {"doAbort", "(Ljava/lang/String;)V", (void*)nfcManager_doAbort},

    {"doSetP2pInitiatorModes", "(I)V",
     (void*)nfcManager_doSetP2pInitiatorModes},

    {"doSetP2pTargetModes", "(I)V", (void*)nfcManager_doSetP2pTargetModes},

    {"doEnableScreenOffSuspend", "()V",
     (void*)nfcManager_doEnableScreenOffSuspend},

    {"doSetScreenState", "(I)V", (void*)nfcManager_doSetScreenState},

    {"doDisableScreenOffSuspend", "()V",
     (void*)nfcManager_doDisableScreenOffSuspend},

    {"doDump", "(Ljava/io/FileDescriptor;)V", (void*)nfcManager_doDump},

    {"getNciVersion", "()I", (void*)nfcManager_doGetNciVersion},
    {"doEnableDtaMode", "()V", (void*)nfcManager_doEnableDtaMode},
    {"doDisableDtaMode", "()V", (void*)nfcManager_doDisableDtaMode},
    {"doFactoryReset", "()V", (void*)nfcManager_doFactoryReset},
    {"doShutdown", "(Z)V", (void*)nfcManager_doShutdown},

    {"getIsoDepMaxTransceiveLength", "()I",
     (void*)nfcManager_getIsoDepMaxTransceiveLength},

    {"getAidTableSize", "()I", (void*)nfcManager_getAidTableSize},

    {"doSetNfcSecure", "(Z)Z", (void*)nfcManager_doSetNfcSecure},

    {"getNfaStorageDir", "()Ljava/lang/String;",
     (void*)nfcManager_doGetNfaStorageDir},

    {"doGetDefaultRoute", "()I", (void*)nfcManager_doGetDefaultRoute},
    {"doSetDefaultRoute", "(IZ)Z", (void*)nfcManager_doSetDefaultRoute},
    {"doSetPreferredSimSlot", "(I)V", (void *)nfcManager_doSetPreferredSimSlot},
    {"doGetSimDetection", "()Z", (void*)nfcManager_doGetSimDetection},
    {"doSetWaitNfcEeDiscovery", "(Z)V", (void *)nfcManager_doSetWaitNfcEeDiscovery},
    {"doSetRfEnabled", "(Z)V", (void *)nfcManager_doSetRfEnabled},
    {"doSetFirmwareDebugLevel", "(IZ)V", (void *)nfcManager_doSetFirmwareDebugLevel},
    {"doGetSecureElementList", "()[I", (void *)nfcManager_doGetSecureElementList},
    {"doRFoptionUpdate", "([B)Z", (void*)nfcManager_doRFoptionUpdate},
    {"doGetListenTechMask", "()I", (void *)nfcManager_doGetListenTechMask},
    {"doSetListenTechMask", "(I)V", (void *)nfcManager_doSetListenTechMask},
    {"doSetFactoryTestMode", "(B)[B", (void*)nfcManager_doSetFactoryTestMode},
    {"startRfDiscovery", "(Z)V", (void*)nfcManager_startRfDiscovery},
    {"doSetForceFWDownload", "(Z)V", (void*)nfcManager_doSetForceFWDownload},

    {"doSetNfceePowerAndLinkCtrl", "(Z)V",
     (void*)nfcManager_doSetNfceePowerAndLinkCtrl},

    {"getRoutingTable", "()[B", (void*)nfcManager_doGetRoutingTable},

    {"getMaxRoutingTableSize", "()I",
     (void*)nfcManager_doGetMaxRoutingTableSize},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeNfcManager
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeNfcManager(JNIEnv* e) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return jniRegisterNativeMethods(e, gNativeNfcManagerClassName, gMethods,
                                  NELEM(gMethods));
}

/*******************************************************************************
**
** Function:        startRfDiscovery
**
** Description:     Ask stack to start polling and listening for devices.
**                  isStart: Whether to start.
**
** Returns:         None
**
*******************************************************************************/
void startRfDiscovery(bool isStart) {
  tNFA_STATUS status = NFA_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: is start=%d", __func__, isStart);
  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  status = isStart ? NFA_StartRfDiscovery() : NFA_StopRfDiscovery();
  if (status == NFA_STATUS_OK) {
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_RF_DISCOVERY_xxxx_EVT
    sRfEnabled = isStart;
  } else {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to start/stop RF discovery; error=0x%X", __func__, status);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();
}

/*******************************************************************************
**
** Function:        isDiscoveryStarted
**
** Description:     Indicates whether the discovery is started.
**
** Returns:         True if discovery is started
**
*******************************************************************************/
bool isDiscoveryStarted() { return sRfEnabled; }

/*******************************************************************************
**
** Function:        doStartupConfig
**
** Description:     Configure the NFC controller.
**
** Returns:         None
**
*******************************************************************************/
void doStartupConfig() {
  struct nfc_jni_native_data* nat = getNative(0, 0);
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  /* Enable RF Field info */
  {
    SyncEventGuard guard(gNfaSetConfigEvent);
    uint8_t  field_info_param[] = { 0x01 };
    stat = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO, sizeof(field_info_param), &field_info_param[0]);
    if (stat == NFA_STATUS_OK) {
      gNfaSetConfigEvent.wait();
      DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: Enabled RF field events", __func__);
    }
    else
      LOG(ERROR) << __func__ << ": Failed to enable RF field events";
  }

  // If polling for Active mode, set the ordering so that we choose Active over
  // Passive mode first.
  if (nat && (nat->tech_mask &
              (NFA_TECHNOLOGY_MASK_A_ACTIVE | NFA_TECHNOLOGY_MASK_F_ACTIVE))) {
    uint8_t act_mode_order_param[] = {0x01};
    SyncEventGuard guard(gNfaSetConfigEvent);
    stat = NFA_SetConfig(NCI_PARAM_ID_ACT_ORDER, sizeof(act_mode_order_param),
                         &act_mode_order_param[0]);
    if (stat == NFA_STATUS_OK) gNfaSetConfigEvent.wait();
  }

  // configure RF polling frequency for each technology
  static tNFA_DM_DISC_FREQ_CFG nfa_dm_disc_freq_cfg;
  // values in the polling_frequency[] map to members of nfa_dm_disc_freq_cfg
  std::vector<uint8_t> polling_frequency;
  if (NfcConfig::hasKey(NAME_POLL_FREQUENCY))
    polling_frequency = NfcConfig::getBytes(NAME_POLL_FREQUENCY);
  if (polling_frequency.size() == 8) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: polling frequency", __func__);
    memset(&nfa_dm_disc_freq_cfg, 0, sizeof(nfa_dm_disc_freq_cfg));
    nfa_dm_disc_freq_cfg.pa = polling_frequency[0];
    nfa_dm_disc_freq_cfg.pb = polling_frequency[1];
    nfa_dm_disc_freq_cfg.pf = polling_frequency[2];
    nfa_dm_disc_freq_cfg.pi93 = polling_frequency[3];
    nfa_dm_disc_freq_cfg.pbp = polling_frequency[4];
    nfa_dm_disc_freq_cfg.pk = polling_frequency[5];
    nfa_dm_disc_freq_cfg.paa = polling_frequency[6];
    nfa_dm_disc_freq_cfg.pfa = polling_frequency[7];
    p_nfa_dm_rf_disc_freq_cfg = &nfa_dm_disc_freq_cfg;
  }

#ifndef NFC_SEC_NOT_OPEN_INCLUDED
  // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
  nfcManager_configNfccConfigControl(true);

#endif
}

/*******************************************************************************
**
** Function:        nfcManager_isNfcActive
**
** Description:     Used externaly to determine if NFC is active or not.
**
** Returns:         'true' if the NFC stack is running, else 'false'.
**
*******************************************************************************/
bool nfcManager_isNfcActive() { return sIsNfaEnabled; }

/*******************************************************************************
**
** Function:        startStopPolling
**
** Description:     Start or stop polling.
**                  isStartPolling: true to start polling; false to stop
*polling.
**
** Returns:         None.
**
*******************************************************************************/
void startStopPolling(bool isStartPolling) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  uint8_t discovry_param = 0;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; isStart=%u", __func__, isStartPolling);

  if (NFC_GetNCIVersion() >= NCI_VERSION_2_0) {
    SyncEventGuard guard(gNfaSetConfigEvent);
    if (isStartPolling) {
      discovry_param =
          NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;
    } else {
      discovry_param =
          NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_DISABLE_MASK;
    }
    status = NFA_SetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
                           NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discovry_param);
    if (status == NFA_STATUS_OK) {
      gNfaSetConfigEvent.wait();
    } else {
      LOG(ERROR) << StringPrintf("%s: Failed to update CON_DISCOVER_PARAM",
                                 __FUNCTION__);
    }
  } else {
    startRfDiscovery(false);
    if (isStartPolling)
      startPolling_rfDiscoveryDisabled(0);
    else
      stopPolling_rfDiscoveryDisabled();
    startRfDiscovery(true);
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask) {
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  if (tech_mask == 0)
    tech_mask =
        NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);

  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enable polling", __func__);
  stat = NFA_EnablePolling(tech_mask);
  if (stat == NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: wait for enable event", __func__);
    sPollingEnabled = true;
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_POLL_ENABLED_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: fail enable polling; error=0x%X", __func__,
                               stat);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();

  return stat;
}

static tNFA_STATUS stopPolling_rfDiscoveryDisabled() {
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: disable polling", __func__);
  stat = NFA_DisablePolling();
  if (stat == NFA_STATUS_OK) {
    sPollingEnabled = false;
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_POLL_DISABLED_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: fail disable polling; error=0x%X", __func__,
                               stat);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();

  return stat;
}

static void nfcManager_doSetRfEnabled(bool enable) {
  sRfEnabled = enable;
}

static void nfcManager_doSetFirmwareDebugLevel(JNIEnv* e, jobject o,
                                               jint type, jboolean enable) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: type: %d, enable: %d", __func__,
                                                   type, enable);
  tNFA_STATUS status = NFA_STATUS_FAILED;
  uint8_t payload[2] = {0x00, 0x00};
  int payloadLen = 0;

  if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
    payload[0] = type;
    payload[1] = enable;
    payloadLen = 2;
  } else if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    payloadLen = 1;
    switch (type) {
      case DEBUG_TYPE_ESE_TRACE:
        type = DEBUG_IDX_ESE_TRACE;
        break;
      case DEBUG_TYPE_PART3_TRACE:
        type = DEBUG_IDX_PART3_TRACE;
        break;
      case DEBUG_TYPE_FIELD_STRENGTH_TRACE:
        type = DEBUG_IDX_FIELD_STRENGTH_TRACE;
        break;
      case DEBUG_TYPE_UICC_TRACE:
        type = DEBUG_IDX_UICC_TRACE;
        break;
      case DEBUG_TYPE_CLT_TRACE:
        type = DEBUG_IDX_CLT_TRACE | DEBUG_IDX_PART3_TRACE;
        break;
      default:
        LOG(ERROR) << StringPrintf("%s: fail to set debug level type; type=0x%X",
                                   __func__, type);
        return;
    }
    char valueStr[PROPERTY_VALUE_MAX] = {0};
    int len = property_get("nfc.fw.sw_api_trace", valueStr, "");
    payload[0] = (uint8_t)valueStr[0];
    if (len > 0) {
      if (enable) {
        payload[0] |= type;
      } else {
        payload[0] &= ~(type);
      }
    } else {
      if (enable) {
        payload[0] = type;
      } else {
        payload[0] = 0x00;
      }
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Firmware debug level = 0x%X", __func__, payload[0]);
    property_set ("nfc.fw.sw_api_trace", (char*)payload);
  }
  status = NFA_SendVsCommand(NCI_PROP_FW_DEBUG_LEVEL, payloadLen, payload, NULL);
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail send firmware debug level cmd; error=0x%X",
                                __func__, status);
  }
}

void notifyListenModeState(bool isSeActive) {
    static const char fn [] = "NativeNfcmanager::notifyListenMode";
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: enter; listen mode active=%u", fn, isSeActive);
    struct nfc_jni_native_data* nat = getNative(NULL, NULL);
    JNIEnv* e = NULL;
    ScopedAttach attach(nat->vm, &e);
    if (e == NULL) {
      LOG(ERROR) << StringPrintf("jni env is null");
      return;
    }

    if (isSeActive) {
        e->CallVoidMethod (nat->manager, android::gCachedNfcManagerNotifySeListenActivated);
    } else {
        e->CallVoidMethod (nat->manager, android::gCachedNfcManagerNotifySeListenDeactivated);
    }

    DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: exit", fn);
  }

void slsiSetNfcSleepTimeout(unsigned long sec, int option)
{
  uint8_t value[3];

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  if (option > 0)
  {
    value[0] = sec % 60;
    value[1] = sec / 60;
  }
  value[2] = option;

  tNFA_STATUS stat = NFA_SendVsCommand(NCI_PROP_SET_SLEEP_TIME, 3, value, NULL);
  if (stat != NFA_STATUS_OK)
    LOG(ERROR) << StringPrintf("%s: failed to set DTA sleep timeout", __func__);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit;", __func__);
}

void doSetAidMatchingMode() {
  if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
    uint8_t matchingMode[3] = {0x01, 0x00, 0x01};
    switch (RoutingManager::getInstance().getAidMatchingMode()) {
      case 0x00: // AID_MATCHING_EXACT_ONLY
        matchingMode[1] = 0x04;
        break;
      case 0x01: // AID_MATCHING_EXACT_OR_PREFIX
        matchingMode[1] = 0x05;
        break;
      case 0x02: // AID_MATCHING_PREFIX_ONLY
        matchingMode[1] = 0x06;
        break;
      default:
        matchingMode[1] = 0x04;
        break;
    }
    SyncEventGuard guard (sNfaAidMatchingCmdEvent);
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_SET_AID_ROUTING_STRATEGY_OID, 3, matchingMode, aidMatchingCmdCallback))
      sNfaAidMatchingCmdEvent.wait (1000);
  }
}

/*******************************************************************************
 **
 ** Function:        nfcManager_doGetDefaultRoute
 ** Description:     [SEC] Get default routing destination for secure element
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **                  0: DH, 2: eSE, 3: UICC
 **
 *******************************************************************************/
static jint nfcManager_doGetDefaultRoute(JNIEnv* e, jobject o) {
  RoutingManager& routingManager = RoutingManager::getInstance();
  int defaultRoute = routingManager.getDefaultEe();
  return (defaultRoute & 0x0F);
}

/*******************************************************************************
 **
 ** Function:        nfcManager_doSetDefaultRoute
 ** Description:     [SEC] Set default routing destination for secure element
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **                  defaultRoute: Routing destination.
 **                  restart: Restart RF discovery when RF already discovered.
 **
 *******************************************************************************/
static jboolean nfcManager_doSetDefaultRoute(JNIEnv* e, jobject o,
                                             jint defaultRoute, jboolean restart) {
  RoutingManager& routingManager = RoutingManager::getInstance();

  if (NFC_GetNCIVersion() == NCI_VERSION_2_0 && defaultRoute != NFC_DH_ID) {
    defaultRoute |= 0x80;
  }

  if (defaultRoute == routingManager.getDefaultEe() || sIsDisabling) {
    return true;
  }
  if (!restart) {
    routingManager.setDefaultEe(defaultRoute);
    return true;
  }
  bool rfEnabled = sRfEnabled;
  if (sRoutingInitialized) {
    routingManager.setDefaultEe(defaultRoute);
    if (rfEnabled && !sIsDisabling) {
      startRfDiscovery(false);
    }
    if (!sIsDisabling) {
      routingManager.updateRoutingTable();
    }
    if (!sIsDisabling) {
      routingManager.commitRouting();
    }
    if (rfEnabled && !sIsDisabling) {
      startRfDiscovery(true);
    }
  }
  return true;
}

/*******************************************************************************
 **
 ** Function:        nfcManager_doSetPreferredSimSlot
 ** Description:     [SEC] Set preferred SIM slot
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **                  slotId: SIM slot ID.
 **                  restart: Restart RF discovery when RF already discovered.
 **
 *******************************************************************************/
static void nfcManager_doSetPreferredSimSlot(JNIEnv *e, jobject o, jint slotId) {
  slotId = slotId + 1;
  DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: slotId: %d", __func__, slotId);
  NFA_SetPreferredSimSlot(slotId);
}

static bool nfcManager_doGetSimDetection(JNIEnv *e, jobject o) {
  return RoutingManager::getInstance().isUiccDetected();
}

static void nfcManager_doSetWaitNfcEeDiscovery(JNIEnv *e, jobject o, jboolean is_wait) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: is_wait: %d", __func__, is_wait);
  NFA_SetWaitNfcEeDiscovery(is_wait);
}

static jintArray nfcManager_doGetSecureElementList(JNIEnv* e, jobject) {
  return RoutingManager::getInstance().getSecureElementList(e);
}

static jboolean nfcManager_doRFoptionUpdate(JNIEnv* e, jobject o, jbyteArray filepath) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  jboolean result = JNI_FALSE;
  ScopedByteArrayRO bytes(e, filepath);
  char* path =
    const_cast<char*>(reinterpret_cast<const char*>(&bytes[0]));
  size_t pathLen = bytes.size();
  char* filePath = (char*)malloc(pathLen * sizeof(char) + 1);
  memset(filePath, 0, pathLen + 1);
  memcpy(filePath, path, pathLen);

  bool rfEnabled = sRfEnabled;
  if (sRoutingInitialized) {
    if (rfEnabled) {
      startRfDiscovery(false);
    }
    result = doRfOptionUpdate(filePath);
    if (rfEnabled) {
      startRfDiscovery(true);
    }
  }
  free(filePath);
  return result;
}

bool doRfOptionUpdate(const char* path) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  bool status = true;
  uint8_t update_size = 0;

  status = initializeRfOptionCB();
  if (status == false){
    return false;
  }

  // File open & read
  status = getRfOptionFile(rfOptionCB.rfOptionData, path);
  if (status == false) {
    free(rfOptionCB.rfOptionData);
    return false;
  }

  // Get Rf option version in chip
  {
    SyncEventGuard guard (sNfaVsCmdEvent);
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_GET_OPTION_META_OID, 0, NULL, RfUpdateCallback))
      sNfaVsCmdEvent.wait (1000);
    if (rfOptionUpdate_rsp != NFA_STATUS_OK) {
      free(rfOptionCB.rfOptionData);
      return false;
    }
  }

  // Compare RF option version
  status = compRfOptionVersion();
  if (status == true) {
    // Don't need update!!
    free(rfOptionCB.rfOptionData);
    return true;
  }

  // Send to START_NEW_RFREG command
  // (for clear system and rf area in nfc chip)
  {
    SyncEventGuard guard (sNfaVsCmdEvent);
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_START_NEW_RFREG, 0, NULL, RfUpdateCallback))
      sNfaVsCmdEvent.wait (1000);
    if (rfOptionUpdate_rsp != NFA_STATUS_OK) {
      free(rfOptionCB.rfOptionData);
      return false;
    }
  }

  // Set RF option value
  update_size = setRfOptionValue(rfOptionCB.rfOptionData);
  while (update_size != 0) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: update_size=%d", __func__, update_size);
    SyncEventGuard guard (sNfaVsCmdEvent);
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_SET_NEW_RFREG, update_size + 1, rfOptionCB.updateData, RfUpdateCallback))
      sNfaVsCmdEvent.wait (1000);
    if (rfOptionUpdate_rsp != NFA_STATUS_OK) {
      free(rfOptionCB.rfOptionData);
      return false;
    }
    update_size = setRfOptionValue(rfOptionCB.rfOptionData);
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: RF Update Success!!!", __func__);
  free(rfOptionCB.rfOptionData);

  if (NFC_GetNCIVersion() == NCI_VERSION_1_0){ //RN82
    // Set meta data
    if (setRfOptionVersion(NCI_PROP_SET_RF_META_DATA) == false) {
      return false;
    }
    // Set RF option version
    if (setRfOptionVersion(NCI_PROP_SET_RFREG_VER) == false) {
      return false;
    }
    return true;
  } else if (NFC_GetNCIVersion() == NCI_VERSION_2_0) { //SN4V
    int n = 2;
    uint8_t check_sum[2];
    while (n--) {
      *((check_sum) + n) = (uint8_t)(((rfOptionCB.rfOptionCheckSum & 0xFFFF) >> (n * 8))) & 0xFF;
    }
    rfOptionCB.rfOptionCheckSum = 0;

    SyncEventGuard guard (sNfaVsCmdEvent);
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_STOP_UPDATE, 2, check_sum, RfUpdateCallback)) {
      sNfaVsCmdEvent.wait (1000);
      if (rfOptionUpdate_rsp != NFA_STATUS_OK)
        return false;
    }
  }

  // Get Rf option version in chip
  {
    SyncEventGuard guard (sNfaVsCmdEvent);
    if (NFA_STATUS_OK == NFA_SendVsCommand(NCI_PROP_GET_OPTION_META_OID, 0, NULL, RfUpdateCallback))
      sNfaVsCmdEvent.wait (1000);
    if (rfOptionUpdate_rsp != NFA_STATUS_OK)
      return false;
  }

  // Compare RF option version
  if (compRfOptionVersion()) {
    return true;
  } else {
    LOG(ERROR) << StringPrintf("%s: version not matched after update", __func__);
    return false;
  }

  return false;
}

static bool initializeRfOptionCB() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {//RN82 Case
    rfOptionCB.rfFileSize = RFOPTION_SIZE_RN82;
    rfOptionCB.rfOptionOffset = 0;
    rfOptionCB.rfOptionSize = NEW_RFOPTION_UPDATE_SIZE_RN82;
  } else if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {//SN4V, RN4V Case
    rfOptionCB.rfFileSize = RFOPTION_TOTAL_SIZE_SN4V;
    rfOptionCB.rfOptionOffset = RFOPTION_POSITION_CHINA_SN4V;
    rfOptionCB.rfOptionSize = RFOPTION_CHINA_SIZE_SN4V;
  } else {//Unsupport version
    LOG(ERROR) << StringPrintf("%s: This NCI Version is not supported this API", __func__);
    return false;
  }

  rfOptionCB.rfOptionCheckSum = 0;
  memset(rfOptionCB.updateData, 0, sizeof(rfOptionCB.updateData));
  memset(rfOptionCB.chipVersion, 0, sizeof(rfOptionCB.chipVersion));
  memset(rfOptionCB.imgVersion, 0, sizeof(rfOptionCB.imgVersion));

  rfOptionCB.rfOptionData = (uint8_t*)malloc(sizeof(uint8_t) * rfOptionCB.rfOptionSize);
  if (rfOptionCB.rfOptionData == NULL) {
    LOG(ERROR) << StringPrintf("%s: failed malloc bufaddr", __func__);
    return false;
  }
  return true;
}

static bool getRfOptionFile(uint8_t *buffer, const char* path) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  // File open & read
  FILE* fh = NULL;
  uint32_t file_size = 0;
  uint32_t read_size = 0;

  if (buffer == NULL || path == NULL) {
    LOG(ERROR) << StringPrintf("%s: invalid parameter", __func__);
    return false;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Get RF Option from %s", __func__, path);

  fh = fopen(path, "rb");
  if (fh == NULL) {
    LOG(ERROR) << StringPrintf("%s: fail to open file; %s", __func__, path);
    return false;
  }

  if (fseek(fh, 0, SEEK_END) < 0) {
    fclose(fh);
    return false;
  }

  file_size = (int)ftell(fh);
  if (file_size != rfOptionCB.rfFileSize) {
    LOG(ERROR) << StringPrintf("%s: Wrong RF File Size(%u, to be %u)",
                                __func__, file_size, rfOptionCB.rfFileSize);
    fclose(fh);
    return false;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: file size = %u", __func__, rfOptionCB.rfOptionSize);

  if (fseek(fh, rfOptionCB.rfOptionOffset, SEEK_SET) < 0) {
    LOG(ERROR) << StringPrintf("%s: fseek fail", __func__);
    fclose(fh);
    return false;
  }

  read_size = fread(buffer, sizeof(char), rfOptionCB.rfOptionSize, fh);

  if (read_size != rfOptionCB.rfOptionSize) {
    LOG(ERROR) << StringPrintf("%s: fread fail!, rfOptionSize = %u",
                                __func__, rfOptionCB.rfOptionSize);
    fclose(fh);
    return false;
  }

  // Get RF option version from file
  if (fseek(fh, -16, SEEK_END) < 0) {
    LOG(ERROR) << StringPrintf("%s: fseek fail", __func__);
    fclose(fh);
    return false;
  }
  read_size = fread(rfOptionCB.imgVersion, sizeof(char), 0x0f, fh);

  if (read_size != 0x0f) {
    LOG(ERROR) << StringPrintf("Can't get RF Option version!");
    fclose(fh);
    return false;
  }

  fclose(fh);
  return true;
}

void RfUpdateCallback (uint8_t event, uint16_t param_len, uint8_t *p_param) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; event=%d, param_len=%d", __func__, event, param_len);
  if (p_param == NULL || param_len < 3 || (p_param[2] + 3) != param_len) {
    LOG(ERROR) << StringPrintf("%s: Abnormal Response!", __func__);
    return;
  }
  char ver[30];
  uint8_t oid = p_param[1];
  rfOptionUpdate_rsp = NFA_STATUS_OK;

  if (p_param[0] == 0x6f) { // Exception NCI_NTF
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Received NTF!", __func__);
    return;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Received OID = 0x%x", __func__, oid);

  switch (oid) {
    case NCI_PROP_GET_OPTION_META_OID:
      if (param_len >= (3 + 0x0F)) {
        memcpy(rfOptionCB.chipVersion, &p_param[3], 0x0f);
      } else {
        LOG(ERROR) << StringPrintf("%s: Receive Error", __func__);
        rfOptionUpdate_rsp = NFA_STATUS_FAILED;
      }
      break;
    case NCI_PROP_START_NEW_RFREG:
    case NCI_PROP_SET_NEW_RFREG:
    case NCI_PROP_STOP_UPDATE:
      if (param_len != 4) {
        LOG(ERROR) << StringPrintf("%s: Receive Error", __func__);
        rfOptionUpdate_rsp = NFA_STATUS_FAILED;
      } else if (p_param[3] != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: RF update Fail! (0x%X)", __func__, p_param[3]);
        rfOptionUpdate_rsp = p_param[3];
      }
      break;
    case NCI_PROP_SET_RFREG_VER:
      if (param_len != 4) {
        LOG(ERROR) << StringPrintf("%s: Receive Error", __func__);
        rfOptionUpdate_rsp = NFA_STATUS_FAILED;
      } else if (p_param[3] != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: RF update Fail! (0x%X)", __func__, p_param[3]);
        rfOptionUpdate_rsp = p_param[3];
      } else {
        memset(ver, 0, 30);
        sprintf(ver, "%d/%d/%d/%d.%d.%d",
                (rfOptionCB.imgVersion[5]>>4)+14, rfOptionCB.imgVersion[5]&0xF,
                rfOptionCB.imgVersion[6], rfOptionCB.imgVersion[7],
                rfOptionCB.imgVersion[8], rfOptionCB.imgVersion[9]);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("nfc.fw.rfreg_ver is %s", ver);
        property_set ("nfc.fw.rfreg_ver", ver);
      }
      break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unexpect packet", __func__);
      break;
  }

  sNfaVsCmdEvent.notifyOne();
}

static bool compRfOptionVersion() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  bool result = true;
  if (!(memcmp(rfOptionCB.chipVersion, rfOptionCB.imgVersion, 15))) {
    DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Don't need update!!", __func__);
    result = true;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Need to update!!", __func__);
    result = false;
  }
  return result;
}

static bool setRfOptionVersion(uint8_t verType) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter; verType = %d", __func__, verType);

  uint8_t buf[255] = {0};
  uint8_t buf_len = 0;

  memset(buf, 0, 255);

  switch (verType) {
    case NCI_PROP_SET_RFREG_VER:
      memcpy(buf, rfOptionCB.imgVersion, 5);
      memcpy(buf + 5, rfOptionCB.imgVersion + 7, 3);
      buf_len = 8;
      break;
    case NCI_PROP_SET_RF_META_DATA:
      memcpy(buf, rfOptionCB.imgVersion, 15);
      buf_len = 15;
      break;
    default:
      LOG(ERROR) << StringPrintf("%s: Unknown verType", __func__);
      break;
  }

  SyncEventGuard guard (sNfaVsCmdEvent);
  if (NFA_STATUS_OK == NFA_SendVsCommand(verType, buf_len, buf, RfUpdateCallback)) {
    sNfaVsCmdEvent.wait (1000);
    if (rfOptionUpdate_rsp != NFA_STATUS_OK)
      return false;
    return true;
  }
  return false;
}

static int setRfOptionValue(uint8_t *buffer) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  static uint32_t rfregid = 0;
  uint8_t *p = rfOptionCB.updateData;
  uint32_t *check_sum;
  uint64_t section_check_sum = 0;
  size_t size;
  uint32_t total = rfOptionCB.rfOptionSize;
  uint32_t next = rfregid * RFREG_SECTION_SIZE;

  if (buffer == NULL) {
    LOG(ERROR) << StringPrintf("%s: invalid parameter", __func__);
    return 0;
  }

  if (total <= next) {
    rfregid = 0;
    return 0;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Next / Total: %d / %d", __func__, next, total);

  if (total - next < RFREG_SECTION_SIZE) {
    size = total - next;
  } else {
    size = RFREG_SECTION_SIZE;
  }

  memcpy(p + 1, buffer + next, size);

  check_sum = (uint32_t*)(p + 1);

  section_check_sum = rfOptionCB.rfOptionCheckSum;
  for (int i = 1; i <= (int)size; i += 4) {
    section_check_sum += *check_sum++;
    section_check_sum &= 0xFFFFFFFF;
  }
  rfOptionCB.rfOptionCheckSum = (uint32_t)section_check_sum;

  p[0] = rfregid;
  rfregid++;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return size;
}

static jint nfcManager_doGetListenTechMask(JNIEnv *e, jobject o) {
  return RoutingManager::getInstance().getListenTechMask();
}

static void nfcManager_doSetListenTechMask(JNIEnv *e, jobject o, jint techMask) {
  RoutingManager& routingManager = RoutingManager::getInstance();
  bool rfEnabled = sRfEnabled;
  if (sRoutingInitialized) {
    if (rfEnabled) startRfDiscovery(false);
    routingManager.setListenTechMask(techMask);
    if (rfEnabled) startRfDiscovery(true);
  }
}

static jbyteArray nfcManager_doSetFactoryTestMode (JNIEnv *e, jobject o, jbyte mode) {
  uint8_t value;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  value = mode;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: factory mode: %d", __func__, value);
  SyncEventGuard guard (sNfaVsFactoryCmdEvent);
  factoryTestResultLen = 0;
  memset(factoryTestResultData, 0, sizeof(factoryTestResultData));
  tNFA_STATUS stat = NFA_SendVsCommand(NCI_PROP_FACTORY_TEST_OID, 1, &value, vendorCmdCallback);
  if (stat != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: failed to set factory test mode", __func__);
    return NULL;
  }
  if (sNfaVsFactoryCmdEvent.wait (1000) == false) {
    LOG(ERROR) << StringPrintf("%s: failed to get factory test result.", __func__);
    return NULL;
  }
  jbyteArray rfData = NULL;
  if(factoryTestResultLen > 0){
    rfData = e->NewByteArray(factoryTestResultLen);
    e->SetByteArrayRegion(rfData, 0, factoryTestResultLen, (jbyte *)factoryTestResultData);
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit;", __func__);
  return rfData;
}

/*******************************************************************************
 **
 ** Function:        nfcManager_startRfDiscovery()
 **
 ** Description:     Start or stop Rf discovery
 **
 ** Returns:         void
 **
*******************************************************************************/
static void nfcManager_startRfDiscovery(JNIEnv * e, jobject o, jboolean start) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("nfcManager_startRfDiscovery:enter, start: %s", start ? "true" : "false");
  startRfDiscovery(start);
}

/*******************************************************************************
 **
 ** Function:        nfcManager_doSetForceFWDownload()
 **
 ** Description:     set property for Force F/W update.
 **                  reserve F/W update for next enabling nfc.
 **
 ** Returns:         void
 **
*******************************************************************************/
static void nfcManager_doSetForceFWDownload(JNIEnv * e, jobject o, jboolean force) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("nfcManager_doSetForceFWDownload:enter, force: %s", force ? "true" : "false");
  char val;
  if (force) {
    val = 0x01;
    property_set("nfc.fw.downloadmode_force", &val);
  } else {
    val = 0x00;
    property_set("nfc.fw.downloadmode_force", &val);
  }
}

/*******************************************************************************
**
** Function:        doSetScreenStateCallback
**
** Description:     [SEC] Set screen sub state response callback
**                  event: event type.
**                  param_len: parameter length.
**                  p_param: parameter
**
** Returns:         Void
**
*******************************************************************************/
static void doSetScreenStateCallback(uint8_t event, uint16_t param_len, uint8_t* p_param) {
  SyncEventGuard guard (sNfaSetPowerSubState);
  sNfaSetPowerSubState.notifyOne();
}

static void vsCmdCallback (uint8_t event, uint16_t param_len, uint8_t* p_param) {
  SyncEventGuard guard (sNfaVsCmdEvent);
  sNfaVsCmdEvent.notifyOne();
}

static void aidMatchingCmdCallback (uint8_t event, uint16_t param_len, uint8_t* p_param) {
  SyncEventGuard guard (sNfaAidMatchingCmdEvent);
  sNfaAidMatchingCmdEvent.notifyOne();
}

static void vendorCmdCallback (uint8_t event, uint16_t param_len, uint8_t* p_param) {
  SyncEventGuard guard (sNfaVsFactoryCmdEvent);
  LOG(ERROR) << StringPrintf("%s: event=0x%x, len=0x%x", __func__, event, param_len);
  if(p_param[1] == NCI_PROP_FACTORY_TEST_OID){ // param[1] = 0x16
    if (param_len == 4) // get Card TX Calibration value rsp size 4
      factoryTestResultLen = param_len - 3;/*1 byte calibration index*/

    LOG(ERROR) << StringPrintf("%s: result len=0x%x", __func__, factoryTestResultLen);
    if(factoryTestResultLen > sizeof(factoryTestResultData)){
      factoryTestResultLen  = sizeof(factoryTestResultData);
    }
    memcpy(factoryTestResultData, p_param + 3, factoryTestResultLen);
  }
  sNfaVsFactoryCmdEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        nfcManager_setPowerSubStateForScreenState
**
** Description:     [SEC] Set screen sub state by vendor command
**                  state: screen state value.
**
** Returns:         tNFA_STATUS
**
*******************************************************************************/
static tNFA_STATUS nfcManager_setPowerSubStateForScreenState(uint8_t state) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  uint8_t screen_state[2] = {0x00, 0x00};
  uint8_t nci_scren_state = 0xFF;

  switch (state) {
    case NFA_SCREEN_STATE_OFF_UNLOCKED:
    case NFA_SCREEN_STATE_OFF_LOCKED:
      nci_scren_state = SCREEN_STATE_OFF_UNLOCKED;
      break;
    case NFA_SCREEN_STATE_ON_UNLOCKED:
        nci_scren_state = SCREEN_STATE_ON_UNLOCKED + 0x03; // SEC Proprietary
      break;
    case NFA_SCREEN_STATE_ON_LOCKED:
        nci_scren_state = SCREEN_STATE_ON_LOCKED;
      break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s, unknown screen state", __func__);
      break;
  }

  screen_state[0] = nci_scren_state;
  SyncEventGuard guard(sNfaSetPowerSubState);
  status = NFA_SendVsCommand(0x38, 1, screen_state, doSetScreenStateCallback);
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                               __FUNCTION__, status);
    return status;
  } else {
    sNfaSetPowerSubState.wait();
  }
  return status;
}

} /* namespace android */
