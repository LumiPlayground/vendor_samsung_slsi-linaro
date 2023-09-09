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
 * tlvparser.h
 *
 *  Created on: 2017.02.21.
 *      Author: MOX
 */

#ifndef __TLV_PARSER_H__
#define __TLV_PARSER_H__

#include "tlv.h"
#include "stkdef.h"

//For OpenChannel Command Qualifier
#define OP_ONDEMAND_LINK_ESTABLISHMENT  (0 << 0)
#define OP_IMMEDIATE_LINK_ESTABLISHMENT  (1 << 0)

#define OP_NO_AUTOMATIC_RECONNECTION  (0 << 0)
#define OP_AUTOMATIC_RECONNECTION  (1 << 0)

#define OP_NO_BACKGROUND_MODE  (0 << 2)
#define OP_IMMEDIATE_LINK_IN_BACKGROUND_MODE  (1 << 2)

#define OP_NO_DNS_SERVER_ADDRESS_REQUESTED  (0 << 3)
#define OP_DNS_SERVER_ADDRESS_REQUESTED  (1 << 3)

// For SendData Command Qualifier
#define SD_STORE_DATA_IN_TX_BUFFER  (0 << 0)
#define SD_SEND_IMMEDIATELY (1 << 0)

#define RFU (0 << 0)

class Address : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_ADDRESS)

private:
    ADDRESS *m_ptAddress;
    typedef enum TonValue {
       TON_UNKNOWN = 0x00,
       INTERNATIONAL_NUMBER,
       NATIONAL_NUMBER,
       NETWORK_SPECIFIC_NUMBER,
       TON_RESERVED
    } TON_VALUE;

    typedef enum NpiValue {
       NPI_UNKNOWN = 0x00,
       TELEPHONY_NUMBERING_PLAN,
       DATA_NUMBERING_PLAN = 0x03,
       TELEX_NUMBERING_PLAN,
       PRIVATE_NUMBERING_PLAN = 0x09,
       NPI_RESERVED = 0x0F
    } NPI_VALUE;

public:
    Address() : CTLV(), m_ptAddress(NULL) { }
    Address(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Address() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    const char *GetTONString();
    const char *GetNPIString();
    int GetTON() { return m_ptAddress ? (int) m_ptAddress->cTON: 0x80; }       // TON : Unknown, NPI : Unknown value 1000 0000
    int GetNPI() { return m_ptAddress? (int) m_ptAddress->cNPI: 0x80; }
    std::string GetDiallingNumber() const {
        return m_ptAddress ? m_ptAddress->cDialingNumber : "";
    }

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class Subaddress : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_SUB_ADDRESS)

private:
    SUB_ADDRESS *m_ptSubaddress;

public:
    Subaddress() : CTLV(), m_ptSubaddress(NULL) { }
    Subaddress(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Subaddress() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetAddressLength() { return m_ptSubaddress? (int) m_ptSubaddress->cAddressLength: 0; }
    BYTE *GetAddress() const { return m_ptSubaddress? m_ptSubaddress->acAddress: NULL; }

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class CommandDetail : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_COMMAND_DETAIL)

private:
    COMMAND_DETAIL m_tCmdDetail;
    typedef enum TypeOfCommand {
        REFRESH = 0x01,
        MORE_TIME,
        POLL_INTERVAL,
        POLLING_OFF,
        SET_UP_EVENT_LIST,
        SET_UP_CALL = 0x10,
        SEND_SS,
        SEND_USSD,
        SEND_SMS,
        SEND_DTMF,
        LAUNCH_BROWSER,
        GEOGRAPHICAL_LOCATION_REQUEST,
        PLAY_TONE = 0x20,
        DISPLAY_TEXT,
        GET_INKEY,
        GET_INPUT,
        SELECT_ITEM,
        SET_UP_MENU,
        PROVIDE_LOCAL_INFORMATION,
        TIMER_MANAGEMENT,
        SET_UP_IDLE_MODE_TEXT,
        PERFORM_CARD_APDU = 0x30,
        POWER_ON_CARD,
        GET_READER_STATUS,
        RUN_AT_COMMAND,
        LANGUAGE_NOTIFICATION,
        OPEN_CHANNEL = 0x40,
        CLOSE_CHANNEL,
        RECEIVE_DATA,
        SEND_DATA,
        GET_CHANNEL_STATUS,
        SERVICE_SEARCH,
        GET_SERVICE_INFORMATION,
        DECLARE_SERVICE,
        SET_FRAMES = 0x50,
        GET_FRAMES_STATUS,
        RETRIVE_MULTIMEDIA_MESSAGE = 0x60,
        SUBMIT_MULTIMEDIA_MESSAGE,
        DISPLAY_MULTIMEDIA_MESSAGE,
        ACTIVATE = 0x70,
        CONTACTLESS_STATE_CHANGED,
        COMMAND_CONTAINER,
        ENCAPSULATED_SESSION_CONTROL,
        END_PROACTIVE_UICC_SESSION = 0x81,
    } TYPE_OF_COMMAND;

public:
    CommandDetail() : CTLV() { memset(&m_tCmdDetail, 0, sizeof(COMMAND_DETAIL)); }
    CommandDetail(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~CommandDetail() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetCommandNumber() { return (int) m_tCmdDetail.cNumber; }
    int GetCommandType() { return (int) m_tCmdDetail.cType; }
    const char *GetCommandTypeString();
    int GetQualifier() { return (int) m_tCmdDetail.cQulaifier; }
    std::string GetQualifierString();

protected:
    UINT GetBitFromValue(UINT value, int index, int length);
    std::string GetQualifierStringOnRefresh();
    std::string GetQualifierStringOnSetUpCall();
    std::string GetQualifierStringOnSendSms();
    std::string GetQualifierStringOnPlayTone();
    std::string GetQualifierStringOnDisplayText();
    std::string GetQualifierStringOnInKey();
    std::string GetQualifierStringOnGetInput();
    std::string GetQualifierStringOnSelectItem();
    std::string GetQualifierStringOnSetupMenu();
    std::string GetQualifierStringOnProvideLocalInformation();
    std::string GetQualifierStringOnGetReaderStatus();
    std::string GetQualifierStringOnTimerManagement();
    std::string GetQualifierStringOnLanguageNotification();
    std::string GetQualifierStringOnLaunchBrowser();
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class DeviceIdentity : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_DEVICE_IDENTITY)

private:
    DEVICE_ID m_tDeviceId;
    typedef enum Device_Identity {
        KEYPAD = 0x01,
        DISPLAY,
        EARPIECE,
        CHANNEL1 = 0x21,
        CHANNEL2,
        CHANNEL3,
        CHANNEL4,
        CHANNEL5,
        CHANNEL6,
        CHANNEL7,
        UICC = 0x81,
        TERMINAL,
        NETWORK
    } DEVICE_IDENTITY;

public:
    DeviceIdentity() : CTLV() { memset(&m_tDeviceId, 0, sizeof(DEVICE_ID)); }
    DeviceIdentity(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~DeviceIdentity() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetSourceID() { return (int) m_tDeviceId.cSource; }
    int GetDestinationID() { return (int) m_tDeviceId.cDestination; }

    const char *GetDeviceIdString(bool isSource);

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class Duration : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_DURATION)

private:
    DURATION m_tDuration;
    typedef enum TimeUnit {
        MINUTES = 0x00,
        SECONDS,
        TENTHS_OF_SECONDS,
        RESERVED_TU
    } TIME_UINT;

    typedef enum TimeInterval {    // range : 1~255
        RESERVED_TI = 0x00,
        UNIT_1,
        UNIT_2,
        UNIT_255 = 0xFF
    } TIME_INTERVAL;

public:
    Duration() : CTLV() { memset(&m_tDuration, 0, sizeof(DURATION)); }
    Duration(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Duration() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetTimeUnit() { return (int) m_tDuration.cTimeUnit; }
    int GetTimeInterval() { return (int) m_tDuration.cTimeInterval; }

    const char *GetTimeUnitString();

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class Result : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_RESULT)

private:
    RESULT *m_ptResult;
    std::string m_strAddtionalInfo;
    typedef enum GeneralResult {
        SUCCESS = 0x00,
        PERFORM_WITH_PARTIAL,
        PERFORM_WITH_MISSING,
        REFRESH_WITH_EFS,
        SUCCESS_WITHOUT_ICON_DISPLAY,
        PERFORM_BY_CALL_CONTROL,
        SUCCESS_LIMITED_SERVICE,
        PERFORM_WITH_MODIFICATION,
        REFRESH_INACTIVE_NAA,
        SUCCESS_WITHOUT_TONE,
        PROACTIVE_SESSION_TERMINATED = 0x10,
        BACKWARD_PROACTIVE_BY_USER = 0x11,
        NO_RESPONSE_FROM_USER = 0x12,
        RESERVED_GSM_3G = 0x14,
        TERMINAL_UNABLE_PROCESS_COMMAND = 0x20,
        NETWORK_UNABLE_PROCESS_COMMAND,
        USER_UNACCEPT_PROACTIVE_COMMAND,
        USER_CLEAR_CALL_BEOFRE_CONNECTION_OR_NETWORK,
        BEYOND_TERMINAL_CAPABILITY = 0x30,
        BIP_ERROR = 0x3a,
    } GENERAL_RESULT;

    typedef enum AdditionalInformation { //for terminal problem
        NO_SPECIFIC_CAUSE,
        SCREEN_IS_BUSY,
        TERMINAL_BUSY,
        RESERVED_FOR_GSM_3G,
        NO_SERVICE,
        ACCESS_CONTROL_CLASS_BAR,
        RADIO_RESOURCE_NOT_GRANTED,
        NOT_IN_SPEECH_CALL,
        RESERVED_4_GSM_3G,
        TERMINAL_BUSY_DTMF,
        NO_NAA_ACTIVE
    } ADDITIONAL_INFORMATION;

    typedef enum AdditionalInformationBIP { //Bearer Independent Protocol
        NO_SPECIFIC_CAUSE_BIP,
        NO_CAHNNEL_AVAILABLE,
        CHANNEL_CLOSED,
        CHANNEL_ID_NOT_VAILD,
        REQUEST_BUFFERSIZE_NOT_AVAILABLE,
        SECURITY_ERROR,
        TRANSPORT_LEVEL_NOT_AVAILABLE,
        REMOTE_DEVICE_NOT_REACHABLE,
        SERVICE_ERROR,
        SERVICE_ID_UNKNOWN,
        PORT_NOT_AVAILABLE,
        PARAMETER_MISSING_INCORRECT
    } ADDITIONAL_INFORMATION_BIP;

public:
    Result() : CTLV(), m_ptResult(NULL), m_strAddtionalInfo("") { }
    Result(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Result() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetGeneralResult() { return (int) m_ptResult->cGeneralResult; }
    std::string GetAdditionalInfoString() {
        return m_strAddtionalInfo;
    }
    const char *GetResult();
    void SetAdditionalInformation(BYTE * /*pAdditionalInfo*/) { }

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class AlphaIdentifier : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_ALPHA_IDENTIFIER)

private:
    std::string m_strAlphaIdentifier;

public:
    AlphaIdentifier() : CTLV() { m_strAlphaIdentifier = ""; }
    AlphaIdentifier(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~AlphaIdentifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    std::string GetAlphaIdentifier() {
        return m_strAlphaIdentifier;
    }

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

class ItemIdentifier : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_ITEM_IDENTIFIER)

private:
    std::string m_strItemIdentifier;
    int m_nId;

public:
    ItemIdentifier() : CTLV() {m_nId = -1; m_strItemIdentifier = ""; }
    ItemIdentifier(const BYTE *pData, int nLength) {Set(pData, nLength);}
    virtual ~ItemIdentifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetId() { return m_nId; }
    std::string GetItemIdentifier() {
        return m_strItemIdentifier;
    }

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

class ItemIDIdentifier : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_ITEM_ID_IDENTIFIER)

private:
    int m_nId;

public:
    ItemIDIdentifier() : CTLV() {m_nId = -1;}
    ItemIDIdentifier(const BYTE *pData, int nLength) {Set(pData, nLength);}
    virtual ~ItemIDIdentifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetId() { return m_nId; }

protected:
    void Initialize();
    void Finalize() {CTLV::Finalize();}
};


class IconIdentifier : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_ICON_IDENTIFIER)

private:
    ICON_IDENTIFIER m_tIconIdentifier;
    typedef enum Icon_Qualifier {
        Icon_is_self_explanatory,
        Icon_is_not_self_explanatory,
    }ICON_QUALIFIER;

    typedef enum Icon_Identifiers {
        record_1_in_EF_IMG = 0x01,
        record_2_in_EF_IMG = 0x02,
    }ICON_IDENTIFIERS;

public:
    IconIdentifier() : CTLV() { memset(&m_tIconIdentifier, 0, sizeof(ICON_IDENTIFIER)); }
    IconIdentifier(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~IconIdentifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetIconQualifier() { return (int)m_tIconIdentifier.cIconQualifier; }
    int GetIconIdentifier() { return (int)m_tIconIdentifier.cIconIdentifier; }
    const char* GetIconQualifierString();
    const char* GetIconIdentifierString();

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class TextString : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_TEXT_STRING)

private:
    int m_nTextStringLength;
    BYTE *m_pTextString;
    typedef enum CodingScheme {
        GSM7BITS = 0x00,
        GSM8BITS = 0x04,
        UCS2 = 0x08
    } CODING_SCHEME;

    CodingScheme m_CodingScheme;

public:
    TextString();
    TextString(const BYTE *pData, int nLength) : m_pTextString(NULL) { Set(pData, nLength); }
    virtual ~TextString() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetCodingScheme() { return m_CodingScheme; }
    int GetTextStringLength() { return  m_nTextStringLength; }
    BYTE *GetTextString() const { return m_pTextString; }
    const char* GetCodingString();

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Bearer description
class BearerDescription : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_BEARER_DESCRIPTION)

private:
    BEARER_DESC *m_ptDescription;
    std::string m_strParameter;
    typedef enum _TlvBearerType_ {
        CSD = 0x01,
        GPRS = 0x02,
        DEFAULT_BEARER = 0x03,
        LOCAL_LINK = 0x04,
        BLUETOOTH = 0x05,
        IRDA = 0x06,
        RS232 = 0x07,
        CDMA2000 = 0x08,
        UTRAN = 0x09,
        I_WLAN = 0x0a,
        E_UTRAN = 0x0b,
        USB = 0x10
    } TLV_BEARER_TYPE;

    /*
    typedef enum CbstSpeed {
        AUTOBAUNDING = 0,
        BPS_300_V21 = 1,
        BPS_1200_V22 = 2,
        BPS_1200_75_V23 = 3,
        BPS_2400_V22_BIS = 4,
        BPS_2400_V26_TER = 5,
        BPS_4800_V32 = 6,
        BPS_9600_V32 = 7,
        BPS_9600_V34 = 12,
        BPS_14400_V34 = 14,
        BPS_19200_V34 = 15,
        BPS_28800_V34 = 16,
        BPS_33600_V34 = 17,
        BPS_1200_V120 = 34,
        BPS_2400_V120 = 36,
        BPS_4800_V120 = 38,
        BPS_9600_V120 = 39,
        BPS_14400_V120 = 43,
        BPS_19200_V120 = 47,
        BPS_28800_V120 = 48,
        BPS_38400_V120 = 49,
        BPS_48000_V120 = 50,
        BPS_56000_V120 = 51,
        BPS_300_V110 = 65,
        BPS_1200_V110 = 66,
        BPS_2400_V110_X31 = 68,
        BPS_4800_V110_X31 = 70,
        BPS_9600_V110_X31 = 71,
        BPS_14400_V110_X31 = 75,
        BPS_19200_V110_X31 = 79,
        BPS_28800_V110_X31 = 80,
        BPS_38400_V110_X31 = 81,
        BPS_48000_V110_X31 = 82,
        BPS_56000_V110_X31 = 83,
        BPS_64000_X_31 = 84,
        BPS_56000_BIT_TRANSPARENT = 115,
        BPS_64000_BIT_TRANSPARENT = 116,
        BPS_32000_PIAFS32k = 120,
        BPS_64000_PIAFS64k = 121,
        BPS_28800_MULTIMEDIA = 130,
        BPS_32000_MULTIMEDIA = 131,
        BPS_33600_MULTIMEDIA = 132,
        BPS_56000_MULTIMEDIA = 133,
        BPS_64000_MULTIMEDIA = 134
    };

    typedef enum CbstName {
        DATA_CIRCUIT_ASYNCH_UDI = 0,
        DATA_CIRCUIT_SYNCH_UDI = 1,
        PAD_ACCESS_UDI = 2,
        PACKET_ACCESS_UDI = 3,
        DATA_CIRCUIT_ASYNCH_RDI = 4,
        DATA_CIRCUIT_SYNCH_RDI = 5,
        PAD_ACCESS_RDI = 6,
        PACKET_ACCESS_RDI = 7
    };

    typedef enum CbstCe {
        TRANSPARENT = 0,
        NON_TRANSPARENT = 1,
        BOTH_TRANSPARENT = 2,
        BOTH_NON_TRANSPARENT = 3
    };

    typedef enum ProtocolType {
        PROTOCOL_IP = 0x02
    };
    */

public:
    BearerDescription() : CTLV(), m_ptDescription(NULL) { m_strParameter = "";  }
    BearerDescription(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~BearerDescription() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);
    int GetBearerLength(BYTE cBearerType);

    BYTE GetType() { return m_ptDescription? m_ptDescription->cType: 0; }
    std::string GetTypeString();
    std::string GetParamString() {
        return m_strParameter;
    }
    BYTE *GetParameter() { return m_ptDescription ? (BYTE *)(m_ptDescription->acParameter) : NULL; }

protected:
    // it has Service Record that is associated with.
    //CServiceRecord *m_objServiceRecord;

    void Initialize();
    void Finalize() {
        CTLV::Finalize();
    }
};

/*
// Service Record
class CServiceRecord : public CTLV
{
    DECLARE_MODULE_TAG()

public:
    typedef enum BearerTechnologyId {
        TECHNOLOGY_INDEPENDENT = 0,
        BLUETOOTH = 1,
        IRDA = 2,
        RS232 = 3,
        USB = 4,
        RFU
    };

    CServiceRecord() : CTLV() { }
    CServiceRecord(const BYTE *pData, int nLength) { Set(pData, nLength); }
    CServiceRecord(BYTE cTag, const BYTE *pValue, int nLength) : CTLV(cTag, pValue, nLength) { }
    virtual ~CServiceRecord() { Finalize(); }

    virtual BYTE *Set(const BYTE *pData, int nLength);
    //virtual void Set(BYTE cTag, );

    virtual BYTE getLocalBearerTechnologyId() { return m_nLocalBearerTechnologyId; }
    virtual BYTE getServiceId() { return m_nServiceId; }
    virtual BYTE *getServiceRecord() { return m_pServiceRecord; }

protected:
    BYTE m_nLocalBearerTechnologyId;
    BYTE m_nServiceId;
    BYTE *m_pServiceRecord;

    virtual void Initialize();
    virtual void Finalize() { CTLV::Finalize(); }
};
*/

// Channel data
class ChannelData : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_CHANNEL_DATA)

public:
    ChannelData() : CTLV() { }
    ChannelData(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~ChannelData() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    std::string GetChannelDataString() {
        return m_strChannelData;
    }

private:
  std::string m_strChannelData;

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

// Channel data length
class ChannelDataLength : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_CHANNEL_DATA_LENGTH)

public:
    ChannelDataLength() : CTLV(), m_nChannelDataLength(0) { }
    ChannelDataLength(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~ChannelDataLength() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetChannelDataLength() { return (int)m_nChannelDataLength; }

private:
    BYTE m_nChannelDataLength;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Channel Status
class ChannelStatus : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_CHANNEL_STATUS)

private:
    char m_szChannelStatus[2];
    typedef enum CommunicationMode {
        CS_PACKET_DATA_SERVICE = 1,
        UICC_SERVER = 2,
        TERMINAL_SERVER_AND_DIRECT_COMMUNICATION_CHANNEL = 3,
        TERMINAL_SERVER_AND_UDP = 4
    } COMMUNICATION_MODE;

    typedef enum Channel_Status {
        CLOSED = 0,
        LISTEN = 1,
        ESTABLISHED = 2,
        RESERVED = 3
    } CHANNEL_STATUS;

    typedef enum FurtherInfo {
        NO_FURTHER_INFO = 0,
        LINK_DROPPED = 5
    } FUTHER_INFO;

public:
    ChannelStatus() : CTLV() { }
    ChannelStatus(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~ChannelStatus() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    BYTE GetRfu(int mode);
    BYTE GetChannelId() { return m_szChannelStatus[0] & 0x7; }
    BYTE IsServiceEstablished() { return m_szChannelStatus[0] & 0x80;}
    BYTE GetTcpStatus() { return m_szChannelStatus[0] & 0xc0; }
    BYTE GetFurtherInfo() { return m_szChannelStatus[1]; }

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Buffer size
class BufferSize : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_BUFFER_SIZE)

public:
    BufferSize() : CTLV(), m_nBufferSize(0) { }
    BufferSize(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~BufferSize() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetBufferSize() { return m_nBufferSize; }

private:
    int m_nBufferSize;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Uicc/terminal interface transport level
class TerminalInterfaceTransportLevel : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_UICC_TERMINAL_INTERFACE_TRANSPORT_LEVEL)

private:
    typedef enum TransportProtocolType {
        // all other values are reserved
        UDP_CLIENT_REMOTE = 0x01,
        TCP_CLIENT_REMOTE = 0x02,
        TCP_SERVER = 0x03,
        UDP_CLIENT_LOCAL = 0x04,
        TCP_CLIENT_LOCAL = 0x05,
        DIRECT_COMMUNICATION_CHENNEL = 0x06
    } TRANSPORT_PROTOCOL_TYPE;

public:
    TerminalInterfaceTransportLevel() : CTLV() { memset(&m_tTransportLevel, 0, sizeof(TRANSPORT_LEVEL)); }
    TerminalInterfaceTransportLevel(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~TerminalInterfaceTransportLevel() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetTransportType() { return (int) m_tTransportLevel.cTransportType; }
    int GetPort() { return (int) m_tTransportLevel.wPort; }
    const char *GetTransportTypeString();

private:
    TRANSPORT_LEVEL m_tTransportLevel;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class TextAttribute : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_TEXT_ATTRIBUTE)

public:
    TextAttribute() : CTLV(), m_nFormattingPosition(-1), m_nFormattingLength(-1) { }
    TextAttribute(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~TextAttribute() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetTextFormattingPosition() { return m_nFormattingPosition; }
    int GetTextFormattingLength() { return m_nFormattingLength; }

private:
  int m_nFormattingPosition;
  int m_nFormattingLength;

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

// Frame identifier
class FrameIdentifier : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_FRAME_IDENTIFIER)

public:
    FrameIdentifier() : CTLV(), m_nIdentifierOfFrame(0) { }
    FrameIdentifier(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~FrameIdentifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetIdentifierOfFrame() { return m_nIdentifierOfFrame; }

private:
    int m_nIdentifierOfFrame;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Other address
class OtherAddress : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_OTHER_ADDRESS)

public:
    typedef enum AddressType {
        ADDRESS_IPV4 = 0x21,
        ADDRESS_IPV6 = 0x57
    } ADDRESS_TYPE;

    static const int IPV4_OCTET_SIZE = 4;
    static const int IPV6_OCTET_SIZE = 16;

    OtherAddress() : CTLV() { memset(&m_tOtherAddress, 0, sizeof(OTHER_ADDRESS)); }
    OtherAddress(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~OtherAddress() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetAddressType() { return (int) m_tOtherAddress.cAddressType; }
    BYTE *GetAddress() { return m_tOtherAddress.acIP; }

private:
    OTHER_ADDRESS m_tOtherAddress;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Network access name
class NetworkAccessName : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_NETWORK_ACCESS_NAME)

public:
    NetworkAccessName() : CTLV() { }
    NetworkAccessName(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~NetworkAccessName() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    std::string GetNetworkAccessName() {
        return m_strNetworkAccessName;
    }

private:
  std::string m_strNetworkAccessName;

  char *ParseAndGetNetworkAccessName();

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

// UTRAN/E-UTRAN measurement qualifier
class MeasurementQualifier : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_UTRAN_EUTRAN_MEASUREMENT_QUALIFIER)

private:
    typedef enum QualifierSpec {
        UTRAN_INTRA_FREQUENCY = 0x01,
        UTRAN_INTER_FREQUENCY = 0x02,
        UTRAN_INTER_RAT_GERAN = 0x03,
        UTRAN_INTER_RAT_EUTRAN = 0x04,
        EUTRAN_INTRA_FREQUENCY = 0x05,
        EUTRAN_INTER_FREQUENCY = 0x06,
        EUTRAN_INTER_RAT_GERAN =0x07,
        EUTRAN_INTER_RAT_UTRAN = 0x08
    } QUALIFIER_SPEC;

public:
    MeasurementQualifier() : CTLV(), m_nQualifier(0) { }
    MeasurementQualifier(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~MeasurementQualifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetQualifier() { return m_nQualifier; }
    const char* GetQualifierString();

private:
    int m_nQualifier;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

// Remote entity address
class RemoteEntityAddress : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_REMOTE_ENTITY_ADDRESS)

private:
    typedef enum CodingType {
        ADDR_BIT48 = 0x00, // IEEE 802.16 , 48-bit address;
        ADDR_BIT32,    // 32-bit IrDA device address;
        ADDR_RESERVED, // 02 to FF are reserved values
    } CONDING_TYPE;

public:
    RemoteEntityAddress() : CTLV(), m_ptRemoteEntityAddress(NULL) { }
    RemoteEntityAddress(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~RemoteEntityAddress() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetCodingType() { return m_ptRemoteEntityAddress? (int) m_ptRemoteEntityAddress->cCodingType: ADDR_RESERVED; }
    LONG GetTextLength() { return m_ptRemoteEntityAddress? m_ptRemoteEntityAddress->lLength: 0; }
    BYTE *GetTextString() const { return m_ptRemoteEntityAddress? m_ptRemoteEntityAddress->acAddress: NULL; }

private:
    REMOTE_ENTITY_ADDRESS *m_ptRemoteEntityAddress;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class EventList : public CTLV
{
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_EVENT_LIST)

public:
    typedef enum EventType {
        MT_CALL = 0x00,
        CALL_CONNECTED = 0x01,
        CALL_DISCONNECTED = 0x02,
        LOCATION_STATUS = 0x03,
        USER_ACTIVITY = 0x04,
        IDLE_SCREEN_AVAILABLE = 0x05,
        CARD_READER_STATUS = 0x06,
        LANGUAGE_SELECTION = 0x07,
        BROWSER_TERMINATION = 0x08,
        DATA_AVAILABLE_EVENT = 0x09,
        CHANNEL_STATUS_EVENT = 0x0A,
        SINGLE_ACCESS_TECHNOLOGY_CHANGE = 0x0B,
        DISPLAY_PARAMETERS_CHANGED = 0x0C,
        LOCAL_CONNECTION = 0x0D,
        NETWORK_SEARCH_MODE_CHANGE = 0x0E,
        BROWSING_STATUS = 0x0F,
        FRAMES_INFORMATION_CHANGE = 0x10,
        IWLAN_ACCESS_STATUS = 0x11,
        NETWORK_REJECTION = 0x12,
        HCI_CONNECTIVITY_EVENT = 0x13,
        MULTIPLE_ACCESS_TECHNOLOGY_CHANGE = 0x14,
        CSG_CELL_SELECTION = 0x15,
        CONTACTLESS_STATE_REQUEST = 0x16,
        IMS_REGISTRATION = 0x17,
        IMS_INCOMMING_DATA = 0x18,
        PROFILE_CONTAINER = 0x19,
        VOID = 0x1A,
        SECURED_PROFILE_CONTAINER = 0x1B,
        POLL_INTERVAL = 0x1C,
        UNKNOWN_EVENT = 0xFF,
    } EVENT_TYPE;

    EventList() : CTLV(), m_pEventList(NULL), m_nEventListLength(-1) { }
    EventList(const BYTE *pData, int nLength) {
        m_pEventList = NULL;
        Set(pData, nLength);
    }
    virtual ~EventList() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetEventList() { return (m_pEventList) ? (int)m_pEventList[0]: -1; }
    int GetEventListLength() { return m_nEventListLength; }
    int GetEvent(int index) { return (m_pEventList) ? (int)m_pEventList[index]: -1;}
    const char *GetEventString(int index);

private:
    BYTE *m_pEventList;
    int m_nEventListLength;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class BrowserTerminationCause : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_BROWSER_TERMINATION_CAUSE)

private:
    typedef enum Browser_Termination_Cause {
       USER_TERMINATION,
       ERROR_TERMINATION,
    } BROWSER_TERMINATION_CAUSE;

public:
    BrowserTerminationCause() : CTLV() { m_nCause = -1; }
    BrowserTerminationCause(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~BrowserTerminationCause() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetCause() { return m_nCause; }

    const char *GetCauseString();

private:
    int m_nCause;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};



class Url : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_URL)

public:
    Url() : CTLV() { m_strUrl = ""; }
    Url(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Url() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    std::string GetUrl() {
        return m_strUrl;
    }

private:
  std::string m_strUrl;

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

class Language : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_LANGUAGE)

public:
    Language() : CTLV() { m_strLanguage = ""; }
    Language(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Language() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    std::string GetLanguage() {
        return m_strLanguage;
    }

private:
  std::string m_strLanguage;

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

class ResponseLength : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_RESPONSE_LENGTH)

public:
    ResponseLength() : CTLV() { m_nMinimum = -1; m_nMaximum = -1; }
    ResponseLength(const BYTE *pData, int nLength) {Set(pData, nLength);}
    virtual ~ResponseLength() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetMinimum() { return m_nMinimum; }
    int GetMaximum() { return m_nMaximum; }

private:
    int m_nMinimum;
    int m_nMaximum;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class Tone : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_TONE)

private:
    typedef enum Tone_Value {
        DIAL_TONE = 0x01,
        CALLED_SUBSCRIBER_BUSY,
        CONGESTION,
        RADIO_PATH_ACKNOWLEDGE,
        RADIO_PATH_NOT_AVAILABLE,
        ERROR,
        CALL_WAITING_TONE,
        RINGING_TONE,
    } TONE_VALUE;

public:
    Tone() : CTLV() { m_nTone = -1; }
    Tone(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~Tone() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetTone() { return m_nTone; }

    const char *GetToneString();

private:
    int m_nTone;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class DefaultText : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_DEFAULT_TEXT)

private:
    typedef enum CodingScheme {
        GSM7BITS = 0x00,
        GSM8BITS = 0x04,
        UCS2 = 0x08
    } CODING_SCHEME;

public:
    DefaultText();
    DefaultText(const BYTE *pData, int nLength) : m_pDefaultText(NULL)  { Set(pData, nLength); }
    virtual ~DefaultText() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetCodingScheme() { return m_CodingScheme; }
    int GetDefaultTextLength() { return m_nDefaultTextStringLength; }

    BYTE *GetDefaultText() const { return m_pDefaultText; }
    const char* GetCodingString();

private:
    CodingScheme m_CodingScheme;
    int m_nDefaultTextStringLength;
    BYTE *m_pDefaultText;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class ImmediateResponse : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_IMMEDIATE_RESPONSE)

public:
    ImmediateResponse() : CTLV() { }
    ImmediateResponse(const BYTE *pData, int nLength) { Set(pData, nLength); }

    virtual ~ImmediateResponse() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class HelpRequest : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_HELP_REQUEST)

public:
    HelpRequest() : CTLV() { }
    HelpRequest(const BYTE *pData, int nLength) { Set(pData, nLength); }

    virtual ~HelpRequest() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class TimerIdentifier : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_ITEM_IDENTIFIER)

private:
    typedef enum Timer_Identifier {
        TIMER_1 = 0x01,
        TIMER_2,
        TIMER_3,
        TIMER_4,
        TIMER_5,
        TIMER_6,
        TIMER_7,
        TIMER_8,
        } TIMER_IDENTIFIER;

public:
    TimerIdentifier() : CTLV() { m_nValue = -1; }
    TimerIdentifier(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~TimerIdentifier() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    int GetTimerIdentifier() { return m_nValue; }

    const char *GetTimerString();

private:
    int m_nValue;

protected:
    void Initialize();
    void Finalize() { CTLV::Finalize(); }
};

class DTMFString : public CTLV {
    DECLARE_MODULE_TAG()
    DECLARE_TLV_TAG(TAG_DTMF_STRING)

public:
    DTMFString() : CTLV() { m_strIdentifier = "";}
    DTMFString(const BYTE *pData, int nLength) { Set(pData, nLength); }
    virtual ~DTMFString() { Finalize(); }

    BYTE *Set(const BYTE *pData, int nLength);

    std::string GetDTMFIdentifier() {
        return m_strIdentifier;
    }

private:
  std::string m_strIdentifier;

protected:
  void Initialize();
  void Finalize() {
      CTLV::Finalize();
  }
};

#endif  // __TLV_PARSER_H_
