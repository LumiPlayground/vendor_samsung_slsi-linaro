/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "tlvparser.h"
#include <string>
#include "rillog.h"
#include "util.h"
#include <base/types.h>

static bool debug = true;

using std::string;
using std::to_string;

// add category to display selective logs
#undef LOGV
#define LOGV(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_VERBOSE_LOG,  "%s::%s() " format, TAG, __FUNCTION__, ##__VA_ARGS__);} while(0)
#undef LOGI
#define LOGI(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_INFO_LOG, "%s::%s() " format, TAG, __FUNCTION__, ##__VA_ARGS__);} while(0)
#undef LOGW
#define LOGW(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_WARNING_LOG,  "%s::%s() " format, TAG, __FUNCTION__, ##__VA_ARGS__);} while(0)
#undef LOGE
#define LOGE(format, ...) do { if (debug) CRilLog::Log(RIL_LOG_CAT_STK, CRilLog::E_RIL_CRITICAL_LOG,  "%s::%s() " format, TAG, __FUNCTION__, ##__VA_ARGS__);} while(0)


// Address class
IMPLEMENT_MODULE_TAG(Address, Address)
void Address::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    if(m_ptAddress) delete m_ptAddress;
    m_ptAddress = NULL;
}

BYTE *Address::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_pValue != NULL) {
        m_ptAddress = new ADDRESS();
        BYTE temp = *m_pValue;
        m_ptAddress->cTON = ((temp >> 4) & 0x07);
        m_ptAddress->cNPI = (temp & 0x0f);
        string dialingnumber = bcdToString(m_pValue+1, m_nLength-1);
        m_ptAddress->cDialingNumber = dialingnumber;
    }

    return pRet;
}

const char *Address::GetTONString() {
    int nTON = m_ptAddress ? (int) m_ptAddress->cTON : 0x80;
    switch (nTON) {
        case TON_UNKNOWN: return "TON_UNKNOWN"; break;
        case INTERNATIONAL_NUMBER: return "INTERNATIONAL_NUMBER"; break;
        case NATIONAL_NUMBER: return "NATIONAL_NUMBER"; break;
        case NETWORK_SPECIFIC_NUMBER: return "NETWORK_SPECIFIC_NUMBER"; break;
        case TON_RESERVED: return "TON_RESERVED"; break;
        default: return "Unknown_NPI_Value"; break;
    }
    return NULL;
}

const char *Address::GetNPIString() {
    int nNPI = m_ptAddress? (int) m_ptAddress->cNPI : 0x80;
    switch (nNPI) {
        case NPI_UNKNOWN: return "NPI_UNKNOWN"; break;
        case TELEPHONY_NUMBERING_PLAN: return "TELEPHONY_NUMBERING_PLAN"; break;
        case DATA_NUMBERING_PLAN: return "DATA_NUMBERING_PLAN"; break;
        case TELEX_NUMBERING_PLAN: return "TELEX_NUMBERING_PLAN"; break;
        case PRIVATE_NUMBERING_PLAN: return "PRIVATE_NUMBERING_PLAN"; break;
        case NPI_RESERVED: return "NPI_RESERVED"; break;
        default: return "Unknown_NPI_Value"; break;
    }
    return NULL;
}

// Subaddress class
IMPLEMENT_MODULE_TAG(Subaddress, Subaddress)
void Subaddress::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_ptSubaddress = NULL;
}

BYTE *Subaddress::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > (int)sizeof(SUB_ADDRESS) && m_pValue != NULL)
    {
        m_ptSubaddress = (SUB_ADDRESS *) m_pValue;
    }

    return pRet;
}

// CommandDetail class
IMPLEMENT_MODULE_TAG(CommandDetail, CommandDetail)
void CommandDetail::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(&m_tCmdDetail, 0, sizeof(COMMAND_DETAIL));
}

BYTE *CommandDetail::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= (int)sizeof(COMMAND_DETAIL) && m_pValue != NULL)
    {
        memcpy(&m_tCmdDetail, m_pValue, sizeof(COMMAND_DETAIL));
    }

    return pRet;
}

const char *CommandDetail::GetCommandTypeString()
{
    switch(m_tCmdDetail.cType)
    {
    case REFRESH: return "REFRESH"; break;
    case MORE_TIME: return "MORE_TIME"; break;
    case POLL_INTERVAL: return "POLL_INTERVAL"; break;
    case POLLING_OFF: return "POLLING_OFF"; break;
    case SET_UP_EVENT_LIST: return "SET_UP_EVENT_LIST"; break;
    case SET_UP_CALL: return "SET_UP_CALL"; break;
    case SEND_SS: return "SEND_SS"; break;
    case SEND_USSD: return "SEND_USSD"; break;
    case SEND_SMS: return "SEND_SMS"; break;
    case SEND_DTMF: return "SEND_DTMF"; break;
    case LAUNCH_BROWSER: return "LAUNCH_BROWSER"; break;
    case GEOGRAPHICAL_LOCATION_REQUEST: return "GEOGRAPHICAL_LOCATION_REQUEST"; break;
    case PLAY_TONE: return "PLAY_TONE"; break;
    case DISPLAY_TEXT: return "DISPLAY_TEXT"; break;
    case GET_INKEY: return "GET_INKEY"; break;
    case GET_INPUT: return "GET_INPUT"; break;
    case SELECT_ITEM: return "SELECT_ITEM"; break;
    case SET_UP_MENU: return "SET_UP_MENU"; break;
    case PROVIDE_LOCAL_INFORMATION: return "PROVIDE_LOCAL_INFORMATION"; break;
    case TIMER_MANAGEMENT: return "TIMER_MANAGEMENT"; break;
    case SET_UP_IDLE_MODE_TEXT: return "SET_UP_IDLE_MODE_TEXT"; break;
    case PERFORM_CARD_APDU: return "PERFORM_CARD_APDU"; break;
    case POWER_ON_CARD: return "POWER_ON_CARD"; break;
    case GET_READER_STATUS: return "GET_READER_STATUS"; break;
    case RUN_AT_COMMAND: return "RUN_AT_COMMAND"; break;
    case LANGUAGE_NOTIFICATION: return "LANGUAGE_NOTIFICATION"; break;
    case OPEN_CHANNEL: return "OPEN_CHANNEL"; break;
    case CLOSE_CHANNEL: return "CLOSE_CHANNEL"; break;
    case RECEIVE_DATA: return "RECEIVE_DATA"; break;
    case SEND_DATA: return "SEND_DATA"; break;
    case GET_CHANNEL_STATUS: return "GET_CHANNEL_STATUS"; break;
    case SERVICE_SEARCH: return "SERVICE_SEARCH"; break;
    case GET_SERVICE_INFORMATION: return "GET_SERVICE_INFORMATION"; break;
    case DECLARE_SERVICE: return "DECLARE_SERVICE"; break;
    case SET_FRAMES: return "SET_FRAMES"; break;
    case GET_FRAMES_STATUS: return "GET_FRAMES_STATUS"; break;
    case RETRIVE_MULTIMEDIA_MESSAGE: return "RETRIVE_MULTIMEDIA_MESSAGE"; break;
    case SUBMIT_MULTIMEDIA_MESSAGE: return "SUBMIT_MULTIMEDIA_MESSAGE"; break;
    case DISPLAY_MULTIMEDIA_MESSAGE: return "DISPLAY_MULTIMEDIA_MESSAGE"; break;
    case ACTIVATE: return "ACTIVATE"; break;
    case CONTACTLESS_STATE_CHANGED: return "CONTACTLESS_STATE_CHANGED"; break;
    case COMMAND_CONTAINER: return "COMMAND_CONTAINER"; break;
    case ENCAPSULATED_SESSION_CONTROL: return "ENCAPSULATED_SESSION_CONTROL"; break;
    case END_PROACTIVE_UICC_SESSION: return "END_PROACTIVE_UICC_SESSION"; break;
    default: return "Unknown"; break;
    }
}

UINT CommandDetail::GetBitFromValue(UINT value, int index, int length) {
    int shift = index-length;
    UINT shiftValue = value >> shift;
    UINT mask = 0;
    for (int i = 0; i < length; i++) {
        mask |= (UINT)(1 << i);
    }
    shiftValue &= mask;
    return shiftValue;
}

string CommandDetail::GetQualifierStringOnRefresh() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "UICC Initialization and Full File Change Notification";
    case 1:
        return "File Change Notification";
    case 2:
        return "UICC Initialization and File Change Notification";
    case 3:
        return "UICC Initialization";
    case 4:
        return "UICC Reset";
    case 5:
        return "UICC Application Reset, only applicable for a 3G platform";
    case 6:
        return "UICC Session Reset, only applicable for a 3G platform";
    case 7:
        return "Reserved by 3GPP('Steering of Roaming' REFRESH support)";
    case 8:
        return "Reserved by 3GPP(Steering of Roaming for I-WLAN)";
    case 9:
        return "eUICC Profile State Change";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}

string CommandDetail::GetQualifierStringOnSetUpCall() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "set up call, but only if not currently busy on another call";
    case 1:
        return "set up call, but only if not currently busy on another call, with redial";
    case 2:
        return "set up call, putting all other calls(if any) on hold";
    case 3:
        return "set up call, putting all other calls(if any) on hold, with redial";
    case 4:
        return "set up call, disconnecting all other calls(if any)";
    case 5:
        return "set up call, disconnecting all other calls(if any), with redial";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}

string CommandDetail::GetQualifierStringOnSendSms() {
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        return "packing not required";
    else {
        return "SMS packing by the terminal required";
    }
}

string CommandDetail::GetQualifierStringOnPlayTone() {
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        return "use of vibrate alert is up to the terminal";
    else {
        return "vibrate alert, if available, with the tone";
    }
}

string CommandDetail::GetQualifierStringOnDisplayText() {
    string strQualifierString = "";
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        strQualifierString = "normal priority";
    else {
        strQualifierString = "high priority";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 8, 1) == 0x00)
        strQualifierString += ", clear message after a delay";
    else {
        strQualifierString += ", wait for user to clear message";
    }
    return strQualifierString;
}

string CommandDetail::GetQualifierStringOnInKey() {
    string strQualifierString = "";
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        strQualifierString = "digits (0 to 9, *, # and +) only";
    else {
        strQualifierString = "alphabet set";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 2, 1) == 0x00)
        strQualifierString += ", SMS default alphabet";
    else {
        strQualifierString += ", UCS2 alphabet";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 3, 1) == 0x00)
        strQualifierString += ", character sets defined by bit 1 and bit 2 are enabled";
    else {
        strQualifierString += ", character sets defined by bit 1 and bit 2 are disabled and the Yes/No response is requested";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 4, 1) == 0x00)
        strQualifierString += ", user response shall be displayed. The terminal may allow alteration and/or confirmation";
    else {
        strQualifierString += ", an immediate digit response (0 to 9, * and #) is requested";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 8, 1) == 0x00)
        strQualifierString += ", no help information available";
    else {
        strQualifierString += ", help information available";
    }
    return strQualifierString;
}

string CommandDetail::GetQualifierStringOnGetInput() {
    string strQualifierString = "";
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        strQualifierString = "digits (0 to 9, *, # and +) only";
    else {
        strQualifierString = "alphabet set";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 2, 1) == 0x00)
        strQualifierString += ", SMS default alphabet";
    else {
        strQualifierString += ", UCS2 alphabet";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 3, 1) == 0x00)
        strQualifierString += ", terminal may echo user input on the display";
    else {
        strQualifierString += ", user input shall not be revealed in any way";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 4, 1) == 0x00)
        strQualifierString += ", user input to be in unpacked format";
    else {
        strQualifierString += ", user input to be in SMS packed format";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 8, 1) == 0x00)
        strQualifierString += ", no help information available";
    else {
        strQualifierString += ", help information available";
    }
    return strQualifierString;
}

string CommandDetail::GetQualifierStringOnSelectItem() {
    string strQualifierString = "";
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        strQualifierString = "presentation type is not specified";
    else {
        strQualifierString = "presentation type is specified in bit 2";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 2, 1) == 0x00)
        strQualifierString += ", presentation as a choice of data values if bit 1 is 1";
    else {
        strQualifierString += ", presentation as choice of navigation options if bit 1 is 1";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 3, 1) == 0x00)
        strQualifierString += ", no selection preference";
    else {
        strQualifierString += ", selection using soft key preferred";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 8, 1) == 0x00)
        strQualifierString += ", no help information available";
    else {
        strQualifierString += ", help information available";
    }
    return strQualifierString;
}

string CommandDetail::GetQualifierStringOnSetupMenu() {
    string strQualifierString = "";
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 1, 1) == 0x00)
        strQualifierString = "no selection preference";
    else {
        strQualifierString = "selection using soft key preferred";
    }
    if (GetBitFromValue((UINT)m_tCmdDetail.cQulaifier, 8, 1) == 0x00)
        strQualifierString += ", no help information available";
    else {
        strQualifierString += ", help information available";
    }
    return strQualifierString;
}

string CommandDetail::GetQualifierStringOnProvideLocalInformation() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "Location Inforamtion according to current NAA";
    case 1:
        return "IMEI of the terminal";
    case 2:
        return "Network Measurment Results according to current NAA";
    case 3:
        return "Date, Time and Time zone";
    case 4:
        return "Language Setting";
    case 5:
        return "Reserved for GSM";
    case 6:
        return "Access Technology (Single Access Technology)";
    case 7:
        return "ESN of the terminal";
    case 8:
        return "IMEISV of the terminal";
    case 9:
        return "Search Mode";
    case 10:
        return "Charge state of the battery";
    case 11:
        return "MEID of the terminal";
    case 12:
        return "reserved for 3GPP";
    case 13:
        return "Broadcast network information according to currnt broadcast network technology used";
    case 14:
        return "Multiple Access Technology";
    case 15:
        return "Local information for mutiple access technologies";
    case 16:
        return "Network Measurment results for multiple access technologies";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}

string CommandDetail::GetQualifierStringOnGetReaderStatus() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "Card Reader Status";
    case 1:
        return "Card Reader Identifier";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}

string CommandDetail::GetQualifierStringOnTimerManagement() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "start the timer";
    case 1:
        return "deactivate the timer";
    case 2:
        return "get current value of timer";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}

string CommandDetail::GetQualifierStringOnLanguageNotification() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "non-specific language notification";
    case 1:
        return "specific language notification";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}
string CommandDetail::GetQualifierStringOnLaunchBrowser() {
    switch ((UINT)m_tCmdDetail.cQulaifier) {
    case 0:
        return "launch browser if not already launched";
    case 1:
        return "not used";
    case 2:
        return "use the existing browser (the browser shall not use the active existing securd session)";
    case 3:
        return "close the existing browser session and launch new browser session";
    case 4:
        return "not used";
    default:
        return to_string(m_tCmdDetail.cQulaifier);
    }
}
string CommandDetail::GetQualifierString() {
    string strQualifierString = "";
    switch (m_tCmdDetail.cType) {
        case REFRESH:
            strQualifierString = GetQualifierStringOnRefresh();
            break;
        case SET_UP_CALL:
            strQualifierString = GetQualifierStringOnSetUpCall();
            break;
        case SEND_SMS:
            strQualifierString = GetQualifierStringOnSendSms();
            break;
        case PLAY_TONE:
            strQualifierString = GetQualifierStringOnPlayTone();
            break;
        case DISPLAY_TEXT:
            strQualifierString = GetQualifierStringOnDisplayText();
            break;
        case GET_INKEY:
            strQualifierString = GetQualifierStringOnInKey();
            break;
        case GET_INPUT:
            strQualifierString = GetQualifierStringOnGetInput();
            break;
        case SELECT_ITEM:
            strQualifierString = GetQualifierStringOnSelectItem();
            break;
        case SET_UP_MENU:
            strQualifierString = GetQualifierStringOnSetupMenu();
            break;
        case PROVIDE_LOCAL_INFORMATION:
            strQualifierString = GetQualifierStringOnProvideLocalInformation();
            break;
        case GET_READER_STATUS:
            strQualifierString = GetQualifierStringOnGetReaderStatus();
            break;
        case TIMER_MANAGEMENT:
            strQualifierString = GetQualifierStringOnTimerManagement();
            break;
        case LANGUAGE_NOTIFICATION:
            strQualifierString = GetQualifierStringOnLanguageNotification();
            break;
        case LAUNCH_BROWSER:
            strQualifierString = GetQualifierStringOnLaunchBrowser();
            break;
         default : strQualifierString += (m_tCmdDetail.cQulaifier); break;
    }

    return strQualifierString;
}

// DeviceIdentity class
IMPLEMENT_MODULE_TAG(DeviceIdentity, DeviceIdentity)
void DeviceIdentity::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(&m_tDeviceId, 0, sizeof(DEVICE_ID));
}

BYTE *DeviceIdentity::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= (int)sizeof(DEVICE_ID) && m_pValue != NULL)
    {
        memcpy(&m_tDeviceId, m_pValue, sizeof(DEVICE_ID));
    }

    return pRet;
}

const char *DeviceIdentity::GetDeviceIdString(bool isSource)
{
    int nDeviceId = isSource ? (int) m_tDeviceId.cSource : (int) m_tDeviceId.cDestination;
    switch(nDeviceId)
    {
    case KEYPAD: return "KEYPAD"; break;
    case DISPLAY: return "DISPLAY"; break;
    case EARPIECE: return "EARPIECE"; break;
    case CHANNEL1: return "CHANNEL1"; break;
    case CHANNEL2: return "CHANNEL2"; break;
    case CHANNEL3: return "CHANNEL3"; break;
    case CHANNEL4: return "CHANNEL4"; break;
    case CHANNEL5: return "CHANNEL5"; break;
    case CHANNEL6: return "CHANNEL6"; break;
    case CHANNEL7: return "CHANNEL7"; break;
    case UICC: return "UICC"; break;
    case TERMINAL: return "TERMINAL"; break;
    case NETWORK: return "NETWORK"; break;
    default: return "Unknown"; break;
    }

    return NULL;
}

// Duration class
IMPLEMENT_MODULE_TAG(Duration, Duration)
void Duration::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(&m_tDuration, 0, sizeof(DURATION));
}

BYTE *Duration::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= (int)sizeof(DURATION) && m_pValue != NULL)
    {
        memcpy(&m_tDuration, m_pValue, sizeof(DURATION));
    }

    return pRet;
}

const char *Duration::GetTimeUnitString() {
    switch ((int) m_tDuration.cTimeUnit) {
        case MINUTES: return "MINUTES"; break;
        case SECONDS: return "SECONDS"; break;
        case TENTHS_OF_SECONDS: return "TENTHS_OF_SECONDS"; break;
        case RESERVED_TU: return "RESERVED_TU"; break;
        default: return "Unknow_Value"; break;
    }
    return NULL;
}

// Result class
IMPLEMENT_MODULE_TAG(Result, Result)
void Result::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_ptResult = NULL;
    m_strAddtionalInfo = "";
}

BYTE *Result::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= (int)sizeof(RESULT) && m_pValue != NULL)
    {
        m_ptResult = (RESULT *) m_pValue;
        m_ptResult->cGeneralResult = m_pValue[0];
        if (m_nLength > 1)
        {
            char *szDump = new char[2*(m_nLength-1)+1];
            int nResult = Value2HexString(szDump, m_ptResult->cAdditionalInformation, m_nLength-1);
            if (nResult > 0) m_strAddtionalInfo = szDump;
            if (szDump != NULL) delete[] szDump;
        }
    }
    return pRet;
}

const char *Result::GetResult()
{
    int result = m_ptResult ? (int) m_ptResult->cGeneralResult : 0xff;
    switch (result)
    {
        case SUCCESS: return "SUCCESS"; break;
        case PERFORM_WITH_PARTIAL: return "PERFORM_WITH_PARTIAL"; break;
        case PERFORM_WITH_MISSING: return "PERFORM_WITH_MISSING"; break;
        case REFRESH_WITH_EFS: return "REFRESH_WITH_EFS"; break;
        case SUCCESS_WITHOUT_ICON_DISPLAY: return "SUCCESS_WITHOUT_ICON_DISPLAY"; break;
        case PERFORM_BY_CALL_CONTROL: return "PERFORM_BY_CALL_CONTROL"; break;
        case SUCCESS_LIMITED_SERVICE: return "SUCCESS_LIMITED_SERVICE"; break;
        case PERFORM_WITH_MODIFICATION: return "PERFORM_WITH_MODIFICATION"; break;
        case REFRESH_INACTIVE_NAA: return "REFRESH_INACTIVE_NAA"; break;
        case SUCCESS_WITHOUT_TONE: return "SUCCESS_WITHOUT_TONE"; break;
        case PROACTIVE_SESSION_TERMINATED: return "PROACTIVE_SESSION_TERMINATED"; break;
        case BACKWARD_PROACTIVE_BY_USER: return "BACKWARD_PROACTIVE_BY_USER"; break;
        case NO_RESPONSE_FROM_USER: return "NO_RESPONSE_FROM_USER"; break;
        case RESERVED_GSM_3G: return "RESERVED_GSM_3G"; break;
        case TERMINAL_UNABLE_PROCESS_COMMAND: return "TERMINAL_UNABLE_PROCESS_COMMAND"; break;
        case NETWORK_UNABLE_PROCESS_COMMAND: return "NETWORK_UNABLE_PROCESS_COMMAND"; break;
        case USER_UNACCEPT_PROACTIVE_COMMAND: return "USER_UNACCEPT_PROACTIVE_COMMAND"; break;
        case USER_CLEAR_CALL_BEOFRE_CONNECTION_OR_NETWORK: return "USER_CLEAR_CALL_BEOFRE_CONNECTION_OR_NETWORK"; break;
        case BEYOND_TERMINAL_CAPABILITY: return "BEYOND_TERMINAL_CAPABILITY"; break;
        default: return "UNKNOWN"; break;
    }
    return NULL;
}

// AlphaIdentifier class
IMPLEMENT_MODULE_TAG(AlphaIdentifier, AlphaIdentifier)
void AlphaIdentifier::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_strAlphaIdentifier = "";
}

BYTE *AlphaIdentifier::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL)
    {
        char *szAlphaId = new char[m_nLength+1];
        if(szAlphaId!=NULL)
        {
            memset(szAlphaId, 0, m_nLength+1);
            memcpy(szAlphaId, (char *)m_pValue, m_nLength);
            szAlphaId[m_nLength] = '\0';
            m_strAlphaIdentifier = szAlphaId;
            delete []szAlphaId;
        }
    }

    return pRet;
}

// ItemIdentifier class
IMPLEMENT_MODULE_TAG(ItemIdentifier, ItemIdentifier)
void ItemIdentifier::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_strItemIdentifier = "";
}

BYTE *ItemIdentifier::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_nLength > 0 && m_pValue != NULL)
    {
        m_nId = *m_pValue;
        char *pItemIdentifier = new char[m_nLength];
        if (pItemIdentifier != NULL) {
            memset(pItemIdentifier, 0, m_nLength);
            memcpy(pItemIdentifier, (char *)(m_pValue+1), m_nLength-1);
            pItemIdentifier[m_nLength-1] = '\0';
            m_strItemIdentifier = pItemIdentifier;
            delete []pItemIdentifier;
            pItemIdentifier = NULL;
        }
    }

    return pRet;
}

// ItemIDIdentifier class
IMPLEMENT_MODULE_TAG(ItemIDIdentifier, ItemIDIdentifier)
void ItemIDIdentifier::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_nId = -1;
}

BYTE *ItemIDIdentifier::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL) {
        m_nId = *m_pValue;
    }

    return pRet;
}

// IconIdentifier class
IMPLEMENT_MODULE_TAG(IconIdentifier, IconIdentifier)
void IconIdentifier::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(&m_tIconIdentifier, 0, sizeof(ICON_IDENTIFIER));
}

BYTE *IconIdentifier::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= (int)sizeof(ICON_IDENTIFIER) && m_pValue != NULL)
    {
        memcpy(&m_tIconIdentifier, m_pValue, sizeof(ICON_IDENTIFIER));
    }

    return pRet;
}

const char *IconIdentifier::GetIconQualifierString() {
    switch ((int) m_tIconIdentifier.cIconQualifier) {
        case Icon_is_self_explanatory : return "Icon is self explanatory"; break;
        case Icon_is_not_self_explanatory : return "Icon is not self explanatory"; break;
        default: return "Unknow_Qualifer_Value"; break;
    }
    return NULL;
}

const char *IconIdentifier::GetIconIdentifierString() {
    switch ((int) m_tIconIdentifier.cIconIdentifier) {
        case record_1_in_EF_IMG : return "record 1 in EF_IMG"; break;
        case record_2_in_EF_IMG : return "record 2 in EF_IMG"; break;
        default: return "Unknown_Identifier_Value"; break;
    }
    return NULL;
}

// TextString class
IMPLEMENT_MODULE_TAG(TextString, TextString)
TextString::TextString() : CTLV()
{
    m_pTextString = NULL;
    m_nTextStringLength = 0;
    m_CodingScheme = GSM7BITS;
}

void TextString::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_CodingScheme = GSM8BITS;
    m_nTextStringLength = 0;
    if(m_pTextString) delete []m_pTextString;
    m_pTextString = NULL;
}

BYTE *TextString::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > (int)sizeof(TEXT_STRING) && m_pValue != NULL)
    {
        TEXT_STRING *ptTextString = (TEXT_STRING *) m_pValue;
        m_CodingScheme = (CodingScheme) (ptTextString->cCodingScheme & 0x0c);
        if (m_CodingScheme == GSM8BITS) {
            m_nTextStringLength = m_nLength - 1;      // 1 is CodingScheme
            m_pTextString = new BYTE[m_nTextStringLength+1];
            memcpy(m_pTextString, ptTextString->acTextString, m_nTextStringLength);

            // Add null-termination
            m_pTextString[m_nTextStringLength] = '\0';
        } else if(m_CodingScheme == GSM7BITS) {
            m_nTextStringLength = (8*(m_nLength-1))/7;      // 1 is CodingScheme
            m_pTextString = new BYTE[m_nTextStringLength+1];
            memset(m_pTextString, 0, m_nTextStringLength+1);
            unpacking_gsm7bit(m_pValue + 1, m_nLength - 1, m_pTextString);
            m_pTextString[m_nTextStringLength] = '\0';
        } else if(m_CodingScheme == UCS2) {
            m_nTextStringLength = (m_nLength-1)/2;     // 1 is CodingScheme
            m_pTextString = new BYTE[m_nTextStringLength+1];
            char temp[m_nTextStringLength+1];
            char tempText[m_nLength];
            memcpy(tempText, m_pValue, m_nLength);
            for (int i = 0, j = 0; i < m_nTextStringLength && j < m_nLength; i++) {
                j += 2;
                temp[i] = tempText[j];
            }
            temp[m_nTextStringLength] = '\0';
            memcpy(m_pTextString, temp, m_nTextStringLength);
            m_pTextString[m_nTextStringLength] = '\0';
        }
    }

    return pRet;
}

const char *TextString::GetCodingString() {
    switch (m_CodingScheme) {
        case GSM7BITS: return "GSM7BITS"; break;
        case GSM8BITS: return "GSM8BITS"; break;
        case UCS2: return "UCS2"; break;
        default : return "Unknown_Coding_Scheme";
    }
    return NULL;
}

// Bearer description
IMPLEMENT_MODULE_TAG(BearerDescription, BearerDescription)
void BearerDescription::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_ptDescription = NULL;
    m_strParameter = "";
}

BYTE *BearerDescription::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);

    if(m_nLength > 0 && m_pValue != NULL)
    {
        m_ptDescription = (BEARER_DESC *) m_pValue;
        int nParamLength = (m_nLength - sizeof(BEARER_DESC)) * 2;
        char *pszParam = new char[nParamLength+1];      // 1 is for null-termination
        int nHexStrLen = Value2HexString(pszParam, m_ptDescription->acParameter, m_nLength-1);
        if(nHexStrLen>0) m_strParameter = pszParam;
        if(pszParam!=NULL) delete []pszParam;
    }

    return pRet;
}

int BearerDescription::GetBearerLength(BYTE cBearerType) {
    switch(cBearerType)
    {
    case CSD:
        return sizeof(BEARER_DESC) + sizeof(CSD_CBST);
    case GPRS:
        return sizeof(BEARER_DESC) + sizeof(GPRS_CGQREQ);
        // the terminal shall provide its default available bearer parameter configuration. X (length of
        // parameters) = 0;
    case DEFAULT_BEARER:
        return sizeof(BEARER_DESC) + 0;
        // in this case, X = variable, Contains Service Identifier" and "Service Record"
        // fields as defined in clause 8.63 and according to the Bearer Type coding
        // Ts_102.223
    case LOCAL_LINK:
    case BLUETOOTH:
    case IRDA:
    case RS232:
    case I_WLAN:
        return sizeof(BEARER_DESC);
    case E_UTRAN:
        //Refer 27.22.4.27.6/2 (OPEN CHANNEL, immediate link establishment, E-UTRAN, bearer type '0B')
        return 3;
    case USB:
        return sizeof(BEARER_DESC);
    default:
        return 0;
    }
    return 0;
}

std::string BearerDescription::GetTypeString() {
    int type = m_ptDescription ? m_ptDescription->cType: 0;
    switch(type) {
        case CSD: return "CSD";
        case GPRS: return "GPRS";
        case DEFAULT_BEARER: return "DEFAULT_BEARER";
        case LOCAL_LINK: return "LOCAL_LINK";
        case BLUETOOTH: return "BLUETOOTH";
        case IRDA: return "IRDA";
        case RS232: return "RS232";
        case CDMA2000: return "CDMA2000";
        case UTRAN: return "UTRAN";
        case I_WLAN: return "I_WLAN";
        case E_UTRAN: return "E_UTRAN";
        case USB: return "USB";
    }
    return "UNKNOWN_TYPE";
}

// Channel data
IMPLEMENT_MODULE_TAG(ChannelData, ChannelData)
void ChannelData::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_strChannelData = "";
}

BYTE *ChannelData::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL)
    {
        char *pszParam = new char[m_nLength*2 + 1];      // 1 is for null-termination
        if(pszParam != NULL)
        {
            int nHexStrLen = Value2HexString(pszParam, m_pValue, m_nLength);
            pszParam[nHexStrLen] = '\0';
            if(nHexStrLen > 0) m_strChannelData = pszParam;
            delete []pszParam;
        }
    }

    return pRet;
}

//Channel data Length
IMPLEMENT_MODULE_TAG(ChannelDataLength, ChannelDataLength)
void ChannelDataLength::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_nChannelDataLength = 0;
}

BYTE *ChannelDataLength::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= 1 && m_pValue != NULL)
    {
        m_nChannelDataLength = m_pValue[0];
    }
    return pRet;
}

//Channel Status
IMPLEMENT_MODULE_TAG(ChannelStatus, ChannelStatus)
void ChannelStatus::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(m_szChannelStatus, 0, 2);
}

BYTE *ChannelStatus::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= 2 && m_pValue != NULL)
    {
        memcpy(m_szChannelStatus, m_pValue, m_nLength);
    }
    return pRet;
}

BYTE ChannelStatus::GetRfu(int mode)
{
    if (mode == CS_PACKET_DATA_SERVICE)
    {
        return m_szChannelStatus[0] & 0x78;
    }
    else if (mode == UICC_SERVER || mode == TERMINAL_SERVER_AND_DIRECT_COMMUNICATION_CHANNEL)
    {
        return m_szChannelStatus[0] & 0x38;
    }
    else
    {
        return m_szChannelStatus[0] & 0xf8;
    }
}

//Buffer size
IMPLEMENT_MODULE_TAG(BufferSize, BufferSize)
void BufferSize::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_nBufferSize = 0;
}

BYTE *BufferSize::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= 2 && m_pValue != NULL)
    {
        m_nBufferSize = SWAP16(*((unsigned short *) m_pValue));
    }
    return pRet;
}

// Uicc/terminal interface transport level
IMPLEMENT_MODULE_TAG(TerminalInterfaceTransportLevel, TerminalInterfaceTransportLevel)
void TerminalInterfaceTransportLevel::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(&m_tTransportLevel, 0, sizeof(TRANSPORT_LEVEL));
}

BYTE *TerminalInterfaceTransportLevel::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= (int)sizeof(TRANSPORT_LEVEL) && m_pValue != NULL)
    {
        //memcpy(&m_tTransportLevel, m_pValue, sizeof(TRANSPORT_LEVEL));
        m_tTransportLevel.cTransportType = m_pValue[0];
        m_tTransportLevel.wPort = SWAP16( (*((WORD *)(m_pValue+1))) );
    }

    return pRet;
}

const char *TerminalInterfaceTransportLevel::GetTransportTypeString() {
    switch (m_tTransportLevel.cTransportType) {
        case UDP_CLIENT_REMOTE: return "UDP_CLIENT_REMOTE";  break;
        case TCP_CLIENT_REMOTE: return "TCP_CLIENT_REMOTE"; break;
        case TCP_SERVER: return "TCP_SERVER"; break;
        case UDP_CLIENT_LOCAL: return "UDP_CLIENT_LOCAL"; break;
        case TCP_CLIENT_LOCAL: return "TCP_CLIENT_LOCAL"; break;
        case DIRECT_COMMUNICATION_CHENNEL: return "DIRECT_COMMUNICATION_CHENNEL"; break;
        default: return "UNKNOWN_TRANSPORT_TYPE";
    }
    return NULL;
}

//Other Address
IMPLEMENT_MODULE_TAG(OtherAddress, OtherAddress)
void OtherAddress::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    memset(&m_tOtherAddress, 0, sizeof(OTHER_ADDRESS));
}

BYTE *OtherAddress::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > IPV4_OCTET_SIZE && m_pValue != NULL)
    {
        memcpy(&m_tOtherAddress, m_pValue, m_nLength);
    }

    return pRet;
}

// Network access name
IMPLEMENT_MODULE_TAG(NetworkAccessName, NetworkAccessName)
void NetworkAccessName::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_strNetworkAccessName = "";
}

BYTE *NetworkAccessName::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL)
    {
        char *pszNetAccessName = ParseAndGetNetworkAccessName();
        m_strNetworkAccessName = pszNetAccessName;
        delete [] pszNetAccessName;
        if (m_strNetworkAccessName == "") LOGE("Network Access Name is NULL!!");
    }
    return pRet;
}

char *NetworkAccessName::ParseAndGetNetworkAccessName()
{
    int len = m_nLength;
    int strLen = 0;
    int index = 0, strIndex = 0;
    char *strResult = new char[m_nLength * 2 + 1];

    while (len > 1)
    {
        strLen = m_pValue[index++];
        if (strLen < len)
        {
            strncpy(strResult+strIndex, (char *)(m_pValue+index), strLen);
            strIndex+=strLen;
            index+=strLen;

            len = len - (strLen + 1);
            if (len > 1)
            {
                strResult[strIndex++] = '.';
            }
            else
            {
                break;
            }
        }
    }
    strResult[strIndex] = '\0';
    return strResult;
}

// UTRAN/E-UTRAN measurement qualifier
IMPLEMENT_MODULE_TAG(MeasurementQualifier, MeasurementQualifier)

void MeasurementQualifier::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_nQualifier = 0;
}

BYTE *MeasurementQualifier::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= 1 && m_pValue != NULL)
    {
        m_nQualifier = (int) m_pValue[0];
    }

    return pRet;
}

const char *MeasurementQualifier::GetQualifierString()
{
    switch (m_nQualifier)
    {
        case UTRAN_INTRA_FREQUENCY: return "UTRAN_INTRA_FREQUENCY";
        case UTRAN_INTER_FREQUENCY: return "UTRAN_INTER_FREQUENCY";
        case UTRAN_INTER_RAT_GERAN: return "UTRAN_INTER_RAT_GERAN";
        case UTRAN_INTER_RAT_EUTRAN: return "UTRAN_INTER_RAT_EUTRAN";
        case EUTRAN_INTRA_FREQUENCY: return "EUTRAN_INTRA_FREQUENCY";
        case EUTRAN_INTER_FREQUENCY: return "EUTRAN_INTER_FREQUENCY";
        case EUTRAN_INTER_RAT_GERAN: return "EUTRAN_INTER_RAT_GERAN";
        case EUTRAN_INTER_RAT_UTRAN: return "EUTRAN_INTER_RAT_UTRAN";
        default: return "UNKNOWN_TYPE";
    }

    return NULL;
}

// TextAttribute class
IMPLEMENT_MODULE_TAG(TextAttribute, TextAttribute)
void TextAttribute::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_nFormattingPosition = -1;
    m_nFormattingLength = -1;
}

BYTE *TextAttribute::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL)
    {
        m_nFormattingPosition = *m_pValue;
        m_nFormattingLength = *(m_pValue+1);
    }

    return pRet;
}

// Frame identifier
IMPLEMENT_MODULE_TAG(FrameIdentifier, FrameIdentifier)
void FrameIdentifier::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_nIdentifierOfFrame = 0;
}

BYTE *FrameIdentifier::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= 1 && m_pValue != NULL)
    {
        m_nIdentifierOfFrame = (int) m_pValue[0];
    }

    return pRet;
}

// RemoteEntityAddress class
IMPLEMENT_MODULE_TAG(RemoteEntityAddress, RemoteEntityAddress)
void RemoteEntityAddress::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    m_ptRemoteEntityAddress = NULL;
}

BYTE *RemoteEntityAddress::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > (int)sizeof(REMOTE_ENTITY_ADDRESS) && m_pValue != NULL)
    {
        m_ptRemoteEntityAddress = (REMOTE_ENTITY_ADDRESS *) m_pValue;
    }

    return pRet;
}

// EventList class
IMPLEMENT_MODULE_TAG(EventList, EventList)
void EventList::Initialize()
{
    LOGI("");
    CTLV::Initialize();
    if (m_pEventList != NULL) delete[] m_pEventList;
    m_pEventList = NULL;
    m_nEventListLength = -1;
}

BYTE *EventList::Set(const BYTE *pData, int nLength)
{
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength >= 1 && m_pValue != NULL)
    {
        m_nEventListLength = m_nLength;
        m_pEventList = new BYTE[m_nLength];
        memcpy(m_pEventList, m_pValue, m_nLength);
    }

    return pRet;
}

const char *EventList::GetEventString(int index) {
    switch (m_pEventList[index]) {
        case MT_CALL: return "MT_CALL"; break;
        case CALL_CONNECTED: return "CALL_CONNECTED"; break;
        case CALL_DISCONNECTED: return "CALL_DISCONNECTED"; break;
        case LOCATION_STATUS: return "LOCATION_STATUS"; break;
        case USER_ACTIVITY: return "USER_ACTIVITY"; break;
        case IDLE_SCREEN_AVAILABLE: return "IDLE_SCREEN_AVAILABLE"; break;
        case CARD_READER_STATUS: return  "CARD_READER_STATUS"; break;
        case LANGUAGE_SELECTION: return "LANGUAGE_SELECTION"; break;
        case BROWSER_TERMINATION: return "BROWSER_TERMINATION"; break;
        case DATA_AVAILABLE_EVENT: return "DATA_AVAILABLE_EVENT"; break;
        case CHANNEL_STATUS_EVENT: return "CHANNEL_STATUS_EVENT"; break;
        case SINGLE_ACCESS_TECHNOLOGY_CHANGE: return "SINGLE_ACCESS_TECHNOLOGY_CHANGE"; break;
        case DISPLAY_PARAMETERS_CHANGED: return "DISPLAY_PARAMETERS_CHANGED"; break;
        case LOCAL_CONNECTION: return "LOCAL_CONNECTION"; break;
        case NETWORK_SEARCH_MODE_CHANGE: return "NETWORK_SEARCH_MODE_CHANGE"; break;
        case BROWSING_STATUS: return "BROWSING_STATUS"; break;
        case FRAMES_INFORMATION_CHANGE: return "FRAMES_INFORMATION_CHANGE"; break;
        case IWLAN_ACCESS_STATUS: return "IWLAN_ACCESS_STATUS"; break;
        case NETWORK_REJECTION: return "NETWORK_REJECTION"; break;
        case HCI_CONNECTIVITY_EVENT: return "HCI_CONNECTIVITY_EVENT"; break;
        case MULTIPLE_ACCESS_TECHNOLOGY_CHANGE: return "MULTIPLE_ACCESS_TECHNOLOGY_CHANGE"; break;
        case CSG_CELL_SELECTION: return "CSG_CELL_SELECTION"; break;
        case CONTACTLESS_STATE_REQUEST: return "CONTACTLESS_STATE_REQUEST"; break;
        case IMS_REGISTRATION: return "IMS_REGISTRATION"; break;
        case IMS_INCOMMING_DATA: return "IMS_INCOMMING_DATA"; break;
        case PROFILE_CONTAINER: return "PROFILE_CONTAINER"; break;
        case VOID: return "VOID"; break;
        case SECURED_PROFILE_CONTAINER: return "SECURED_PROFILE_CONTAINER"; break;
        case POLL_INTERVAL: return "POLL_INTERVAL"; break;
        default: return "UNKNOWN_EVENT"; break;
    }
    return NULL;
}

// Browser Termination Cause
IMPLEMENT_MODULE_TAG(BrowserTerminationCause, BrowserTerminationCause)
void BrowserTerminationCause::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_nCause = -1;
}

BYTE *BrowserTerminationCause::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL) {
        m_nCause = *m_pValue;
    }

    return pRet;
}

const char *BrowserTerminationCause::GetCauseString() {
    switch (m_nCause) {
        case USER_TERMINATION: return "USER_TERMINATION"; break;
        case ERROR_TERMINATION: return "ERROR_TERMINATION"; break;
        default: return "Unknown_Cause";
    }
    return NULL;
}

// Url Class
IMPLEMENT_MODULE_TAG(Url, Url)
void Url::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_strUrl = "";
}

BYTE *Url::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_nLength > 0 && m_pValue != NULL) {
        char *szUrl = new char[m_nLength+1];
        if (szUrl != NULL) {
            memset(szUrl, 0, m_nLength+1);
            memcpy(szUrl, (char *)m_pValue, m_nLength);
            szUrl[m_nLength] = '\0';
            m_strUrl = szUrl;
            delete []szUrl;
        }
    }

    return pRet;
}

// Language Class
IMPLEMENT_MODULE_TAG(Language, Language)
void Language::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_strLanguage = "";
}

BYTE *Language::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_nLength > 0 && m_pValue != NULL) {
        char *szLanguage = new char[m_nLength+1];
        if(szLanguage != NULL) {
            memset(szLanguage, 0, m_nLength+1);
            memcpy(szLanguage, (char *)m_pValue, m_nLength);
            szLanguage[m_nLength] = '\0';
            m_strLanguage = szLanguage;
            delete []szLanguage;
        }
    }

    return pRet;
}

// Response Length Class
IMPLEMENT_MODULE_TAG(ResponseLength, ResponseLength)
void ResponseLength::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_nMinimum = -1;
    m_nMaximum = -1;
}

BYTE *ResponseLength::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_nLength > 0 && m_pValue != NULL) {
        m_nMinimum = *m_pValue;
        m_nMaximum = *(m_pValue+1);
    }

    return pRet;
}

// Tone Class
IMPLEMENT_MODULE_TAG(Tone, Tone)
void Tone::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_nTone = -1;
}

BYTE *Tone::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_nLength > 0 && m_pValue != NULL) {
        m_nTone = *m_pValue;
    }

    return pRet;
}

const char *Tone::GetToneString() {
    switch (m_nTone) {
        case DIAL_TONE: return "DIAL_TONE"; break;
        case CALLED_SUBSCRIBER_BUSY: return "CALLED_SUBSCRIBER_BUSY"; break;
        case CONGESTION: return "CONGESTION"; break;
        case RADIO_PATH_ACKNOWLEDGE: return "RADIO_PATH_ACKNOWLEDGE"; break;
        case RADIO_PATH_NOT_AVAILABLE: return "RADIO_PATH_NOT_AVAILABLE"; break;
        case ERROR: return "ERROR"; break;
        case CALL_WAITING_TONE: return "CALL_WAITING_TONE"; break;
        case RINGING_TONE: return "RINGING_TONE"; break;
        default: return "UNKNOWN_TONE";
    }
    return NULL;
}

// Default Text Class
IMPLEMENT_MODULE_TAG(DefaultText, DefaultText)
DefaultText::DefaultText() : CTLV() {
    m_pDefaultText = NULL;
    m_nDefaultTextStringLength = 0;
    m_CodingScheme = GSM7BITS;
}

void DefaultText::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_CodingScheme = GSM8BITS;
    m_nDefaultTextStringLength = 0;
    if (m_pDefaultText) delete []m_pDefaultText;
    m_pDefaultText = NULL;
}

BYTE *DefaultText::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > (int)sizeof(DEFAULT_TEXT) && m_pValue != NULL) {
        DEFAULT_TEXT *ptDefaultTextString = (DEFAULT_TEXT *) m_pValue;
        m_CodingScheme = (CodingScheme) (ptDefaultTextString->cCodingScheme & 0x0c);
        if (m_CodingScheme == GSM8BITS) {
            m_nDefaultTextStringLength = m_nLength - 1;      // 1 is CodingScheme
            m_pDefaultText = new BYTE[m_nDefaultTextStringLength+1];
            memcpy(m_pDefaultText, ptDefaultTextString->acDefaultTextString, m_nDefaultTextStringLength);

            // Add null-termination
            m_pDefaultText[m_nDefaultTextStringLength] = '\0';
        } else if (m_CodingScheme==GSM7BITS) {
            m_nDefaultTextStringLength = (8*(m_nLength-1))/7;      // 1 is CodingScheme
            m_pDefaultText = new BYTE[m_nDefaultTextStringLength+1];
            memset(m_pDefaultText, 0, m_nDefaultTextStringLength+1);
            unpacking_gsm7bit(m_pValue+1, m_nLength-1, m_pDefaultText);
            m_pDefaultText[m_nDefaultTextStringLength] = '\0';
        } else if (m_CodingScheme==UCS2) {
            m_nDefaultTextStringLength = (m_nLength-1)/2;
            m_pDefaultText = new BYTE[m_nDefaultTextStringLength+1];
            char temp[m_nDefaultTextStringLength+1];
            char tempText[m_nLength];
            memcpy(tempText, m_pValue, m_nLength);
            for (int i= 0, j = 0; i < m_nDefaultTextStringLength && j < m_nLength; i++) {
                j += 2;
                temp[i] = tempText[j];
            }
            temp[m_nDefaultTextStringLength] = '\0';
            memcpy(m_pDefaultText, temp, m_nDefaultTextStringLength);
            m_pDefaultText[m_nDefaultTextStringLength] = '\0';
        }
    }
    return pRet;
}

const char *DefaultText::GetCodingString() {
    switch (m_CodingScheme) {
        case GSM7BITS: return "GSM7BITS"; break;
        case GSM8BITS: return "GSM8BITS"; break;
        case UCS2: return "UCS2"; break;
        default : return "UNKNOWN_CODING_SCHEME";
    }
    return NULL;
}

// Immediate Response Length
IMPLEMENT_MODULE_TAG(ImmediateResponse, ImmediateResponse)
void ImmediateResponse::Initialize() {
    LOGI("");
    CTLV::Initialize();
}

BYTE *ImmediateResponse::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);

    // no decoding needed as length will be 0 for this tag
    return pRet;
}

// Help Request Class
IMPLEMENT_MODULE_TAG(HelpRequest, HelpRequest)
void HelpRequest::Initialize() {
    LOGI("");
    CTLV::Initialize();
}

BYTE *HelpRequest::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);

    // no decoding needed as length will be 0 for this tag
    return pRet;
}

// Timer Identifier Class
IMPLEMENT_MODULE_TAG(TimerIdentifier, TimerIdentifier)
void TimerIdentifier::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_nValue = -1;
}

BYTE *TimerIdentifier::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if(m_nLength > 0 && m_pValue != NULL) {
        m_nValue = *m_pValue;
    }

    return pRet;
}

const char *TimerIdentifier::GetTimerString() {
    switch (m_nValue) {
        case TIMER_1: return "TIMER_1"; break;
        case TIMER_2: return "TIMER_2"; break;
        case TIMER_3: return "TIMER_3"; break;
        case TIMER_4: return "TIMER_4"; break;
        case TIMER_5: return "TIMER_5"; break;
        case TIMER_6: return "TIMER_6"; break;
        case TIMER_7: return "TIMER_7"; break;
        case TIMER_8: return "TIMER_8"; break;
        default: return "Unknown_Timer_Value";
    }
    return NULL;
}

// DTMF String Class
IMPLEMENT_MODULE_TAG(DTMFString, DTMFString)
void DTMFString::Initialize() {
    LOGI("");
    CTLV::Initialize();
    m_strIdentifier = "";
}

BYTE *DTMFString::Set(const BYTE *pData, int nLength) {
    BYTE *pRet = CTLV::Set(pData, nLength);
    if (m_nLength > 0 && m_pValue != NULL) {
        string dtmfstring = bcdToString(m_pValue, m_nLength);
        m_strIdentifier = dtmfstring;
    }

    return pRet;
}
