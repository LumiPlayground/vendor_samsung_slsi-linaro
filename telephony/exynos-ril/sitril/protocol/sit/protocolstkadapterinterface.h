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
 * protocolstkadapterinterface.h
 *
 *  Created on: 2021. 06. 17.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_STK_ADAPTER_INTERFACE_H__
#define __PROTOCOL_STK_ADAPTER_INTERFACE_H__

#include <base/types.h>

class ProtocolStkResponseAdapterInterface {
public:
    ProtocolStkResponseAdapterInterface() { }
    virtual ~ProtocolStkResponseAdapterInterface() = default;

public:
    virtual UINT GetErrorCode() const = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

class ProtocolStkIndAdapterInterface {
public:
    ProtocolStkIndAdapterInterface() { }
    virtual ~ProtocolStkIndAdapterInterface() = default;

public:
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

class ProtocolStkEnvelopeCommandAdapterInterface {
public:
    ProtocolStkEnvelopeCommandAdapterInterface() { }
    virtual ~ProtocolStkEnvelopeCommandAdapterInterface() = default;

public:
    virtual int GetEnvelopeCmdLength() const = 0;
    virtual BYTE *GetEnvelopeCommand() const = 0;
};

// not in use by service
class ProtocolStkTerminalRspAdapterInterface {
public:
    ProtocolStkTerminalRspAdapterInterface() { }
    virtual ~ProtocolStkTerminalRspAdapterInterface() = default;

public:
    virtual int GetSW1() const = 0;
    virtual int GetSW2() const = 0;
};

class ProtocolStkEnvelopeStatusAdapterInterface {
public:
    ProtocolStkEnvelopeStatusAdapterInterface() { }
    virtual ~ProtocolStkEnvelopeStatusAdapterInterface() = default;

public:
    virtual int GetSW1() const = 0;
    virtual int GetSW2() const = 0;
    virtual int GetEnvelopeRspLength() const = 0;
    virtual BYTE *GetEnvelopeResponse() const = 0;
};

class ProtocolStkProactiveCommandAdapterInterface {
public:
#pragma pack(1)
  typedef struct tagBerTLV {
      BYTE cTag;
      BYTE cLength;
      BYTE acData[0];
  } BER_TLV;

  // MOX: 2015.08.06 - STK Call Setup
  typedef struct tagComprehensionCommon {
      BYTE cTag;
      BYTE cLength;
  } TAG_COMMON;

  typedef struct tagComprehensionAddress {
      TAG_COMMON common;
      BYTE cNpi : 4;
      BYTE cTon : 3;
      BYTE cTonNpiFlag : 1;
      BYTE acDialNumString[0];
  } CMPH_ADDRESS;

  typedef struct tagComprehensionAlphaIdentifier {
      TAG_COMMON common;
      BYTE acAlphaId[0];
  } CMPH_ALPHA_ID;

  typedef struct tagComprehensionCmdDetail {
      TAG_COMMON common;
      BYTE cNumber;
      BYTE cType;
      BYTE cQualifier;
  } CMPH_CMD_DETAIL;

  typedef struct tagComprehensionDeviceIdentifier {
      TAG_COMMON common;
      BYTE cSrcId;
      BYTE cDstId;
  } CMPH_DEVICE_ID;

  typedef struct tagComprehensionDuration {
      TAG_COMMON common;
      BYTE cTimeUnit;
      BYTE cTimeInterval;
  } CMPH_DURATION;

  typedef struct tagComprehensionResult {
      TAG_COMMON common;
      BYTE cGeneralResult;
      BYTE acAdditionalInfo[0];
  } CMPH_RESULT;

  typedef struct tagComprehensionTextString {
      TAG_COMMON common;
      BYTE cDataCodingScheme;
      BYTE acTextString[0];
  } CMPH_TEXT_STRING;

  typedef struct tagComprehensionFileList {
      TAG_COMMON common;
      BYTE cNumOfFiles;
      WORD awFiles[0];
  } CMPH_FILE_LIST;

  typedef struct tagComprehensionAID {
      TAG_COMMON common;
      BYTE acAID[0];
  } CMPH_AID;
#pragma pack()

  // Comprehension Tag
  enum eComprehensionTag {
      CAT_CMPH_TAG_CMD_DETAIL = 0x01,
      CAT_CMPH_TAG_DEVICE_ID,
      CAT_CMPH_TAG_RESULT,
      CAT_CMPH_TAG_DURATION,
      CAT_CMPH_TAG_ALPHA,
      CAT_CMPH_TAG_ADDRESS,
      CAT_CMPH_TAG_TEXT_STRING = 0x0D,
      CAT_CMPH_TAG_FILE_LIST = 0x12,
      CAT_CMPH_TAG_AID = 0x2F,
  };

  // Type of Command
  enum eTypeOfCommand {
      CAT_CMD_TYPE_REFRESH = 0x01,
      CAT_CMD_TYPE_DISPLAY_TEXT = 0x21,
      CAT_CMD_TYPE_SETUP_CALL = 0x10,
  };

  // Device Identities
  enum eDeviceIdentities {
      CAT_DEVICE_ID_KEYPAD = 0x01,
      CAT_DEVICE_ID_DISPLAY,
      CAT_DEVICE_ID_EARPIECE,
      CAT_DEVICE_ID_UICC = 0x81,
      CAT_DEVICE_ID_TERMINAL,
      CAT_DEVICE_ID_NETWORK
  };

  // Text Coding Scheme
  enum eTextCodingScheme {
      CAT_TEXT_CODING_GSM7BIT = 0x00,
      CAT_TEXT_CODING_GSM8BIT = 0x04,
      CAT_TEXT_CODING_UCS2 = 0x08
  };

  // From Legacy only
  // Command Qualifier
  enum eCommandQualifier {
      CAT_CMD_QUALFR_SETUPCALL_NOT_BUSY = 0x00,
      CAT_CMD_QUALFR_SETUPCALL_NOT_BUSY_REDIAL,
      CAT_CMD_QUALFR_SETUPCALL_OTHERS_HOLD,
      CAT_CMD_QUALFR_SETUPCALL_OTHERS_HOLD_REDIAL,
      CAT_CMD_QUALFR_SETUPCALL_DISCONNECTING_OTHERS,
      CAT_CMD_QUALFR_SETUPCALL_DISCONNECTING_OTHERS_REDIAL
  };

  // Address TON
  enum eAddressTON {
      CAT_ADDR_TON_UNKNOWN = 0b000,
      CAT_ADDR_TON_INTERNATIONAL = 0b001,
      CAT_ADDR_TON_NATIONAL = 0b010,
      CAT_ADDR_TON_NET_SPECIFIC = 0b11
  };

  // Address NPI
  enum eAddressNPI {
      CAT_ADDR_NPI_UNKNOWN = 0b0000,
      CAT_ADDR_NPI_ISDN_TEL_NUM_PLAN = 0b0001,
      CAT_ADDR_NPI_DATA_NUM_PLAN = 0b0011,
      CAT_ADDR_NPI_TEL_NUM_PLAN = 0b0100,
      CAT_ADDR_NPI_PRIVATE_NUM_PLAN = 0b1001,
      CAT_ADDR_NPI_RESERVED_EXT = 0b1111
  };

  // Duration Time Unit
  enum eDurationTimeUnit {
      CAT_DURATION_TIME_UNIT_MINUTE = 0x00,
      CAT_DURATION_TIME_UNIT_SECONDS,
      CAT_DURATION_TIME_UNIT_TENTHS_SECONDS
  };

  // General Result
  enum eGeneralResult {
      CAT_GENERAL_RESULT_SUCCESS = 0x00,
      CAT_GENERAL_RESULT_USER_REJECT = 0x22,
  };

  ProtocolStkProactiveCommandAdapterInterface() {}
  virtual ~ProtocolStkProactiveCommandAdapterInterface() = default;

  virtual int GetProactiveCmdLength() const = 0;
  virtual BYTE *GetProactiveCommand() const = 0;

  virtual int GetEfidCount(void) = 0;
  virtual UINT GetEFID(int nIndex) = 0;

  virtual int GetAidLength(void) = 0;
  virtual BYTE *GetAID(void) = 0;
};

class ProtocolStkSimRefreshAdapterInterface {
public:
    ProtocolStkSimRefreshAdapterInterface() { }
    virtual ~ProtocolStkSimRefreshAdapterInterface() = default;

public:
    virtual int GetResult() const = 0;
};

class ProtocolSsReturnResultAdapterInterface {
public:
    ProtocolSsReturnResultAdapterInterface() { }
    virtual ~ProtocolSsReturnResultAdapterInterface() = default;

public:
    virtual int GetReturnResultLength() const = 0;
    virtual BYTE *GetReturnResult() const = 0;
};

class ProtocolStkCcAlphaNtfAdapterInterface {
public:
    ProtocolStkCcAlphaNtfAdapterInterface() { }
    virtual ~ProtocolStkCcAlphaNtfAdapterInterface() = default;

    virtual int GetAlphaLength() const = 0;
    virtual BYTE* GetAlpha() const = 0;
};
#endif /* __PROTOCOL_STK_ADAPTER_INTERFACE_H__ */
