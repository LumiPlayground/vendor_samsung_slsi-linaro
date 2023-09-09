/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "stkdataparser.h"
#include "rillog.h"
#include "stkservice.h"
#include "requestdata.h"
#include "ber.h"
#include "comprehension.h"
#include "protocolstkadapter.h"
#include "socket.h"
#include "stkdata.h"
#include "tlvparser.h"
#include <unistd.h>
#include <fcntl.h>

static bool debug = true;

// add category to display selective logs
#undef LOGV
#define LOGV(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_VERBOSE_LOG,  "%s::%s() " format, m_szTAG, __FUNCTION__, ##__VA_ARGS__);} while (0)
#undef LOGI
#define LOGI(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_INFO_LOG, "%s " format, m_szTAG,  ##__VA_ARGS__);} while (0)
#undef LOGW
#define LOGW(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_WARNING_LOG,  "%s::%s() " format, m_szTAG, __FUNCTION__, ##__VA_ARGS__);} while (0)
#undef LOGE
#define LOGE(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_CRITICAL_LOG,  "%s::%s() " format, m_szTAG, __FUNCTION__, ##__VA_ARGS__);} while (0)

#undef ENTER_FUNC
#define ENTER_FUNC()        { LOGI("[<--"); }
#undef LEAVE_FUNC
#define LEAVE_FUNC()        { LOGI("[-->"); }

#define PARAM_NULL(msg)     { if (msg == NULL) { LOGE("Parameter = NULL"); return -1; } }
#define NULL_REQ(msg)       { if (msg == NULL || msg->GetRequestData() == NULL) { LOGE("RequestData = NULL"); return -1; } }
#define NULL_RSP(msg)       { if (msg == NULL || msg->GetModemData() == NULL) { LOGE("ModemData = NULL"); return -1; } }

StkDataParser::StkDataParser(StkService *pStkService, int nRilSocketID /* =0 */) {
    snprintf(m_szTAG, sizeof(m_szTAG)-1, "%s[%d]", "StkDataParser", nRilSocketID);

    // ENTER_FUNC();

    m_nCmdRead = -1;
    m_nCmdWritten = -1;

    m_pStkService = pStkService;

    memset(&m_tSetupEventList, EventList::UNKNOWN_EVENT, sizeof(SETUP_EVENTLIST));  // upper 2 events support

    m_pMutex = NULL;
    m_nAccumlatedSendDataLenInBuffer = 0;

    // LEAVE_FUNC();
}

StkDataParser::~StkDataParser() {
    // ENTER_FUNC();

    if(m_bStarted) Finalize();

    // LEAVE_FUNC();
}

int StkDataParser::OnCreate() {
    // ENTER_FUNC();

    // LEAVE_FUNC();
    return 0;
}

void StkDataParser::OnStart() {
    // ENTER_FUNC();

    // LEAVE_FUNC();
}

void StkDataParser::OnDestroy() {
    // ENTER_FUNC();
    delete m_pMutex;
    // LEAVE_FUNC();
}

int StkDataParser::Initialize() {
    // ENTER_FUNC();

    if (m_bStarted) {
        // LOGI("Already Initialized !!!");
        return -1;
    }

    m_pMutex = new CMutex();
    if (m_pMutex == NULL) {
        LOGE("Fail to create Mutex instance");
        return -1;
    }

    if (CreateCommandPipe() < 0) {
        LOGE("Fail to create command read/written pipe");
        return -1;
    }

    // Start Thread
    ClearQueue();
    Start();

    // LOGI("STK Parser Initialized...");
    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::Finalize() {
    // ENTER_FUNC();

    if (m_bStarted) {
        Stop();
        ClearQueue();
    } // else LOGI("Already Finalized !!!");

    // LEAVE_FUNC();
    return 0;
}

void StkDataParser::Run() {
    // ENTER_FUNC();

    if (m_nCmdRead == -1) {
        LOGE("Command pipe is not created");
        return;
    }

    // Callback OnStart();
    OnStart();

    fd_set rfdSet;

    while (1) {
        // Reset FD
        FD_ZERO(&rfdSet);
        FD_SET(m_nCmdRead, &rfdSet);

        int n = select(m_nCmdWritten, &rfdSet, NULL, NULL, NULL);

        if (n > 0) {
            if (FD_ISSET(m_nCmdRead, &rfdSet)) {
                //MsgDirection direction;
                BYTE bFlag = 0;
                int ret = read(m_nCmdRead, &bFlag, 1);
                if (ret < 0) {
                    if (errno == EINTR || errno == EAGAIN) {
                        continue;
                    } else {
                        CreateCommandPipe();
                        continue;
                    }
                } else if (ret == 0) {
                    CreateCommandPipe();
                    continue;
                }

                Process(Dequeue());
            }
        }
    }

    // LEAVE_FUNC();
}

void StkDataParser::ClearQueue() {
    // ENTER_FUNC();

    Message *pMsg;
    while (!m_cmdQueue.empty()) {
        pMsg = m_cmdQueue.front();
        if (pMsg != NULL) {
            delete pMsg;
        }
        m_cmdQueue.pop();
    }

    // LEAVE_FUNC();
}

int StkDataParser::Enqueue(const Message *pMsg) {
    if (NULL == pMsg) {
        LOGE("Enqueue fail: pMsg is NULL");
        return -1;
    }

    // LOGI("%d. 0x%08X, Req:0x%08X, Rsp:0x%08X, Dir:%d", m_cmdQueue.size(), pMsg, ((Message *)pMsg)->GetRequestData(), ((Message *)pMsg)->GetModemData(), ((Message *)pMsg)->GetDirection());

    BYTE bFlag = 1;
    if (m_pMutex != NULL) {
        m_pMutex->lock();
    } else {
        LOGE("mutex is null");
        return 0;
    }
    m_cmdQueue.push((Message *) pMsg);
    if (m_nCmdWritten != -1 && write(m_nCmdWritten, &bFlag, 1) == -1) LOGE("Command Pipe Error: m_nCmdWritten=%d", m_nCmdWritten);
    m_pMutex->unlock();

    return 0;
}

Message *StkDataParser::Dequeue() {
    if (m_cmdQueue.empty() == true) return NULL;

    m_pMutex->lock();
    Message *pMsg = m_cmdQueue.front();
    m_cmdQueue.pop();
    m_pMutex->unlock();
    // LOGI("0x%08X, Req:0x%08X, Rsp:0x%08X, Dir:%d", pMsg, pMsg->GetRequestData(), pMsg->GetModemData(), pMsg->GetDirection());
    return pMsg;
}

int StkDataParser::CreateCommandPipe() {
    // ENTER_FUNC();

    if (m_nCmdRead != -1) {
        //LOGV ("Create command read pipe again");
        close(m_nCmdRead);
        m_nCmdRead = -1;
    }

    if (m_nCmdWritten != -1) {
        // LOGV("Create command write pipe again");
        close(m_nCmdWritten);
        m_nCmdWritten = -1;
    }

    int fds[2];
    int n = pipe2(fds, O_CLOEXEC);
    if (n < 0) {
        LOGE("Command pipe create fail");
        return -1;
    }

    m_nCmdRead = fds[0];
    m_nCmdWritten = fds[1];

    // LOGI("Created Pipes: m_nCmdRead=%d, m_nCmdWritten=%d", m_nCmdRead, m_nCmdWritten);

    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::Process(Message *pMsg) {
    // ENTER_FUNC();
    PARAM_NULL(pMsg);

    switch (pMsg->GetDirection()) {
    case REQUEST:
        ProcessRequest(pMsg);
        break;
    case RESPONSE:
        ProcessModemData(pMsg->GetMsgId(), pMsg->GetModemData());
        break;
    default:
        LOGE("Invalid Direction: %d", pMsg->GetDirection());
        break;
    }

    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::ProcessRequest(Message *pMsg) {
    // ENTER_FUNC();
    PARAM_NULL(pMsg);

    // LOGI("Message ID: %d(0x%02X)", pMsg->GetMsgId(), pMsg->GetMsgId());

    switch (pMsg->GetMsgId()) {
    case MSG_SAT_SEND_ENVELOPE_CMD:
        ProcessEnvelopeCommand(pMsg);
        break;
    case MSG_SAT_SEND_TERMINAL_RSP:
        ProcessTerminalResponse(pMsg);
        break;
    case MSG_SAT_SEND_ENVELOPE_STATUS:
        // ProcessEnvelopeStatus(pReq);
        break;
    default:
        LOGE("Invalid Message ID: %d(0x%02X)", pMsg->GetMsgId(), pMsg->GetMsgId());
        break;
    }

    delete pMsg;
    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::ProcessModemData(int nMsgId, ModemData *pModem) {
    // ENTER_FUNC();
    PARAM_NULL(pModem);

    // LOGI("Message ID: %d(0x%02X)", nMsgId, nMsgId);

    switch (nMsgId) {
    case MSG_SAT_PROACTIVE_COMMAND:
        ProcessProactiveCommand(pModem);
        break;
    case MSG_SAT_SEND_ENVELOPE_CMD_DONE:
        ProcessEnvelopeCommandDone(pModem);
        break;
    case MSG_SAT_SEND_TERMINAL_RSP_DONE:
        ProcessTerminalResponseDone(pModem);
        break;
    case MSG_SAT_SEND_ENVELOPE_STATUS_DONE:
        ProcessEnvelopeStatusDone(pModem);
        break;
    default:
        LOGE("Invalid Message ID: %d(0x%02X)", nMsgId, nMsgId);
        break;
    }

    // LEAVE_FUNC();
    return 0;
}

// Request
int StkDataParser::ProcessEnvelopeCommand(Message *pMsg) {
    // ENTER_FUNC();
    PARAM_NULL(pMsg);

    StkData rildata;
    if (rildata.Parse((StringRequestData *) pMsg->GetRequestData()) == false
            || rildata.GetLength() == 0 || rildata.GetData() == NULL) {
        LEAVE_FUNC();
        return 0;
    }

    CBER *pBer = new CBER(rildata.GetData(), rildata.GetLength());

    if (pBer != NULL) {
        Comprehension *pComprehension = pBer->GetComprehension();
        LOGI("ENVELOPE_COMMAND (TLV Count:%d)", pComprehension->GetTlvCount());
        HandleCommand(pComprehension);
        if (pComprehension != NULL) {
          delete pComprehension;
          pComprehension = NULL;
        }
        delete pBer;
        pBer = NULL;
    } else {
        LOGI("pBer is NULL");
    }

    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::ProcessTerminalResponse(Message *pMsg) {
    // ENTER_FUNC();
    PARAM_NULL(pMsg);

    StkData rildata;
    if (rildata.Parse((StringRequestData *) pMsg->GetRequestData()) == false
            || rildata.GetLength() == 0 || rildata.GetData() == NULL) {
        LEAVE_FUNC();
        return 0;
    }

    CBER *pBer = new CBER();
    if (pBer != NULL) {
      pBer->Set(rildata.GetLength(), rildata.GetData());
      Comprehension *pComprehension = pBer->GetComprehension();
      LOGI("TERMINAL_RESPONSE (TLV Count:%d)", pComprehension->GetTlvCount());
      HandleCommand(pComprehension);
      if (pComprehension != NULL) {
        delete pComprehension;
        pComprehension = NULL;
      }
      delete pBer;
      pBer = NULL;
    } else {
        LOGI("pBer is NULL");
    }

    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::ProcessEnvelopeStatus(RequestData *pReq) {
    // ENTER_FUNC();
    PARAM_NULL(pReq);

    // LEAVE_FUNC();
    return 0;
}

// Response
int StkDataParser::ProcessProactiveCommand(ModemData *pModem) {
    // ENTER_FUNC();
    PARAM_NULL(pModem);

    ProtocolStkProactiveCommandAdapter adapter(pModem);
    CBER *pBer = new CBER(adapter.GetProactiveCommand(), adapter.GetProactiveCmdLength());
    if (pBer && pBer->IsValid()) {
        Comprehension *pComprehension = pBer->GetComprehension();
        if (pComprehension) {
            LOGI("PROACTIVE_COMMAND (TLV Count:%d)", pComprehension->GetTlvCount());
            HandleCommand(pComprehension);
            delete pComprehension;
            pComprehension = NULL;
        }
    }
    if (pBer) {
        delete pBer;
        pBer = NULL;
    }

    // LEAVE_FUNC();
    return 0;
}


int StkDataParser::HandleCommand(Comprehension *pComprehension) {
    // ENTER_FUNC();
    PARAM_NULL(pComprehension);

    bool isPassword = false;

    for (int i = 0; i < pComprehension->GetTlvCount(); i++) {
        CTLV *pTlv = (CTLV *)pComprehension->GetTlv(i);
        switch (pTlv->GetTag()) {
            case TAG_COMMAND_DETAIL:
                LOGI("[COMMAND DETAILS] Number:%d, Type:0x%02X (%s), Qualifier: %d,(%s)",
                        ((CommandDetail *) pTlv)->GetCommandNumber(),
                        ((CommandDetail *) pTlv)->GetCommandType(),
                        ((CommandDetail *) pTlv)->GetCommandTypeString(),
                        ((CommandDetail *) pTlv)->GetQualifier(),
                        ((CommandDetail *) pTlv)->GetQualifierString().c_str());
                break;
            case TAG_BEARER_DESCRIPTION:
                 LOGI("[BEARER DESCRIPTION] BearerType : %s(%d), %s",
                         ((BearerDescription *) pTlv)->GetTypeString().c_str(),
                         ((BearerDescription *) pTlv)->GetType(),
                         ((BearerDescription *) pTlv)->GetParamString().c_str());
                break;
            case TAG_BUFFER_SIZE:
                LOGI("[BUFFER SIZE] BufferSize:%d", ((BufferSize *) pTlv)->GetBufferSize());
                break;
            case TAG_NETWORK_ACCESS_NAME:
                LOGI("[NETWORK ACCESS NAME] strNetAccessName:%s",
                        ((NetworkAccessName *) pTlv)->GetNetworkAccessName().c_str());
                break;
            case TAG_TEXT_STRING:
                if (!isPassword) {
                    isPassword = true;
                    if (((TextString *) pTlv)->GetTextStringLength() > 0 &&
                            ((TextString *) pTlv)->GetTextString() != NULL) {
                        LOGI("[TEXT STRING] CodingScheme = %s (%d), String = %s",
                                ((TextString *) pTlv)->GetCodingString(),
                                ((TextString *) pTlv)->GetCodingScheme(),
                                (char *) ((TextString *) pTlv)->GetTextString());
                    } else {
                        LOGI("[TEXT STRING] Length of TLV Data is 0!!");
                    }
                } else {
                    if (((TextString *) pTlv)->GetTextStringLength() > 0 &&
                            ((TextString *) pTlv)->GetTextString() != NULL) {
                        LOGI("[TEXT STRING] Password:%s", (char *) ((TextString *) pTlv)->GetTextString());
                    } else {
                        LOGI("[TEXT STRING] Length of TLV Data is 0!!");
                    }
                }
                break;
            case TAG_OTHER_ADDRESS:
                {
                    char *pacAddr = (char *) ((OtherAddress *) pTlv)->GetAddress();
                    char szAddr[64];
                    memset(szAddr, 0, 64);
                    if (((OtherAddress *) pTlv)->GetAddressType() == OtherAddress::ADDRESS_IPV4) {
                        sprintf(szAddr, "%d.%d.%d.%d", pacAddr[0], pacAddr[1], pacAddr[2], pacAddr[3]);
                    } else if (((OtherAddress *) pTlv)->GetAddressType() == OtherAddress::ADDRESS_IPV6) {
                        sprintf(szAddr, "%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x",
                                pacAddr[0], pacAddr[1], pacAddr[2], pacAddr[3],
                                pacAddr[4], pacAddr[5], pacAddr[6], pacAddr[7],
                                pacAddr[8], pacAddr[9], pacAddr[10], pacAddr[11],
                                pacAddr[12], pacAddr[13], pacAddr[14], pacAddr[15]);
                    }
                    LOGI("[OTHER ADDRESS] DestinationAddr:%d, %s", ((OtherAddress *) pTlv)->GetAddressType(),
                            szAddr);
                }
                break;
            case TAG_ALPHA_IDENTIFIER:
                LOGI("[ALPHA ID] %s", ((AlphaIdentifier *) pTlv)->GetAlphaIdentifier().c_str());
                break;
            case TAG_ITEM_IDENTIFIER:
                LOGI("[ITEM] %02d. %s", ((ItemIdentifier *) pTlv)->GetId(),
                        ((ItemIdentifier *) pTlv)->GetItemIdentifier().c_str());
                break;
            case TAG_ITEM_ID_IDENTIFIER:
                LOGI("[ITEM ID] %02d", ((ItemIDIdentifier *) pTlv)->GetId());
                break;
            case TAG_BROWSER_TERMINATION_CAUSE:
                LOGI("[BROWSER TERMINATION CAUSE] Cause = %s (%d)",
                        ((BrowserTerminationCause *) pTlv)->GetCauseString(),
                        ((BrowserTerminationCause *) pTlv)->GetCause());
                break;
            case TAG_URL:
                LOGI("[URL] %s", ((Url *) pTlv)->GetUrl().c_str());
                break;
            case TAG_DEVICE_IDENTITY:
                LOGI("[DEVICE IDENTITY] %s (0x%02X) -> %s (0x%02X)",
                        ((DeviceIdentity *) pTlv)->GetDeviceIdString(true),
                        ((DeviceIdentity *) pTlv)->GetSourceID(),
                        ((DeviceIdentity *) pTlv)->GetDeviceIdString(false),
                        ((DeviceIdentity *) pTlv)->GetDestinationID());
                break;
            case TAG_RESULT:
                LOGI("[RESULT] %s (0x%02X)", ((Result *) pTlv)->GetResult(),
                        ((Result *) pTlv)->GetGeneralResult());
                break;
            case TAG_DURATION:
                LOGI("[DURATION] Time Unit = %s (%d), Time Interval = %d",
                        ((Duration *) pTlv)->GetTimeUnitString(),
                        ((Duration *)pTlv)->GetTimeUnit(),
                        ((Duration *) pTlv)->GetTimeInterval());
                break;
            case TAG_ADDRESS:
                LOGI("[ADDRESS] TON = %s (%d), NPI = %s (%d),  Dialing Number = %s",
                        ((Address *) pTlv)->GetTONString(),
                        ((Address *) pTlv)->GetTON(),
                        ((Address *) pTlv)->GetNPIString(),
                        ((Address *) pTlv)->GetNPI(),
                        ((Address *) pTlv)->GetDiallingNumber().c_str());
                break;
            case TAG_ICON_IDENTIFIER:
                LOGI("[ICON IDENTIFIER] IconQualifier = %s (%d), IconIdentifier = %s (%d)",
                        ((IconIdentifier *) pTlv)->GetIconQualifierString(),
                        ((IconIdentifier *) pTlv)->GetIconQualifier(),
                        ((IconIdentifier *) pTlv)->GetIconIdentifierString(),
                        ((IconIdentifier *) pTlv)->GetIconIdentifier());
                break;
            case TAG_EVENT_LIST:
                for(int i = 0; i < ((EventList *) pTlv)->GetEventListLength(); i++) {
                    LOGI("[EVENT LIST] Event %d = %s (0x%02X)", i+1,
                            ((EventList *) pTlv)->GetEventString(i),
                            ((EventList *) pTlv)->GetEvent(i));
                }
                break;
           case TAG_TEXT_ATTRIBUTE:
                LOGI("[TEXT ATTRIBUTE] Formatting Position = %d, Formatting Length = %d",
                        ((TextAttribute *) pTlv)->GetTextFormattingPosition(),
                        ((TextAttribute *) pTlv)->GetTextFormattingLength());
                break;
           case TAG_LANGUAGE:
                LOGI("[LANGUAGE] %s", ((Language *) pTlv)->GetLanguage().c_str());
                break;
           case TAG_RESPONSE_LENGTH:
                LOGI("[RESPONSE LENGTH] Minimum = %d, Maximum = %d",
                        ((ResponseLength *) pTlv)->GetMinimum(), ((ResponseLength *) pTlv)->GetMaximum());
                break;
           case TAG_TONE:
                LOGI("[TONE] %s", ((Tone *) pTlv)->GetToneString());
                break;
           case TAG_DEFAULT_TEXT:
                if (((DefaultText *)pTlv)->GetDefaultTextLength() > 0 &&
                        ((DefaultText *)pTlv)->GetDefaultText() != NULL) {
                    LOGI("[DEFAULT TEXT] CODING_SCHEME = %s (%d), Text = %s",
                            ((DefaultText *) pTlv)->GetCodingString(),
                            ((DefaultText *) pTlv)->GetCodingScheme(),
                            ((DefaultText *) pTlv)->GetDefaultText());
                } else {
                    LOGI("[DEFAULT TEXT] Length of TLV Data is 0!!");
                }
                break;
           case TAG_IMMEDIATE_RESPONSE:
                LOGI("[IMMEDIATE RESPONSE]");
                break;
           case TAG_HELP_REQUEST:
                LOGI("[HELP REQUEST]");
                break;
           case TAG_TIMER_IDENTIFIER:
                LOGI("[TIMER IDENTIFIER] TimerId = %s (%d)",
                        ((TimerIdentifier *) pTlv)->GetTimerString(),
                        ((TimerIdentifier *) pTlv)->GetTimerIdentifier());
                break;
           case TAG_DTMF_STRING:
                LOGI("[DTMF STRING] DTMF = %s", ((DTMFString *) pTlv)->GetDTMFIdentifier().c_str());
                break;
           case TAG_CHANNEL_DATA_LENGTH:
                LOGI("[CHANNEL DATA LENGTH] Length = %d",
                        ((ChannelDataLength *) pTlv)->GetChannelDataLength());
                break;
           case TAG_CHANNEL_DATA:
                LOGI("[CHANNEL DATA] Channel Data = %s",
                        ((ChannelData *) pTlv)->GetChannelDataString().c_str());
                break;
           case TAG_UICC_TERMINAL_INTERFACE_TRANSPORT_LEVEL:
                LOGI("[UICC TERMINAL INTERFACE TRANSPORT LEVEL] Transport Port:%d, Transport Type:%s (%d)",
                        ((TerminalInterfaceTransportLevel *) pTlv)->GetPort(),
                        ((TerminalInterfaceTransportLevel *) pTlv)->GetTransportTypeString(),
                        ((TerminalInterfaceTransportLevel *) pTlv)->GetTransportType());
                break;
           case TAG_UTRAN_EUTRAN_MEASUREMENT_QUALIFIER:
                LOGI("[MEASUREMENT QUALIFIER] Qualifier Value = %d, Qualifier String = %s",
                        ((MeasurementQualifier *) pTlv)->GetQualifier(),
                        ((MeasurementQualifier *) pTlv)->GetQualifierString());
                break;
        }
    }

    // LEAVE_FUNC();
    return 0;
}

int StkDataParser::ProcessEnvelopeCommandDone(ModemData *pModem) {
    ENTER_FUNC();
    PARAM_NULL(pModem);

    LEAVE_FUNC();
    return 0;
}


int StkDataParser::ProcessTerminalResponseDone(ModemData *pModem) {
    ENTER_FUNC();
    PARAM_NULL(pModem);

    LEAVE_FUNC();
    return 0;
}

int StkDataParser::ProcessEnvelopeStatusDone(ModemData *pModem) {
    ENTER_FUNC();
    PARAM_NULL(pModem);

    LEAVE_FUNC();
    return 0;
}
