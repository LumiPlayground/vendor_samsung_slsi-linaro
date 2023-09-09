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
 * protocolcalladapterinterface.h
 *
 *  Created on: 2021. 06. 08.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_CALL_ADAPTER_INTERFACE_H__
#define __PROTOCOL_CALL_ADAPTER_INTERFACE_H__

#include <telephony/ril.h>
#include <base/types.h>

class UusInfo;
class CallInfo;
class EccInfo;

/**
 * ProtocolGetCurrentCallAdapterInterface
 */
class ProtocolGetCurrentCallAdapterInterface {
public:
    ProtocolGetCurrentCallAdapterInterface() = default;
    virtual ~ProtocolGetCurrentCallAdapterInterface() = default;
public:
    virtual bool HasValidLength() = 0;
    virtual int GetCallNum() = 0;
    virtual int GetCallInfo(CallInfo* pCallInfo, int index) = 0;
    virtual void DebugPrintCallInfo(CallInfo* pCallInfo) = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;

private:
    // int ConvertSitToUusInfo(UusInfo* pUusInfo, int uusType, int uus_dcs, int len, BYTE* pData) = 0;
    // template<typename T>
    // int SetCallInfo(CallInfo* (&pCallInfo), int index, T* (&pcallinfo));
    // int GetCallInfoV1_1(CallInfo* pCallInfo, int index) = 0;
    // int GetCallInfoV1_2(CallInfo* pCallInfo, int index) = 0;
    // int GetCallInfoV1_6(CallInfo* pCallInfo, int index) = 0;
};

/**
 * ProtocolGetLastCallFailCauseAdapterInterface
 */
class ProtocolGetLastCallFailCauseAdapterInterface {
public:
    ProtocolGetLastCallFailCauseAdapterInterface() = default;
    virtual ~ProtocolGetLastCallFailCauseAdapterInterface() = default;
public:
    virtual int GetLastCallFailCause() = 0;
    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetClipAdapterInterface
 */
class ProtocolGetClipAdapterInterface {
public:
    ProtocolGetClipAdapterInterface() = default;
    virtual ~ProtocolGetClipAdapterInterface() = default;
public:
    virtual int GetClipStatus() = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetClirAdapterInterface
 */
class ProtocolGetClirAdapterInterface {
public:
    ProtocolGetClirAdapterInterface() = default;
    virtual ~ProtocolGetClirAdapterInterface() = default;
public:
    virtual int GetClirStatus() = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetCallForwardingStatusAdapterInterface
 */
class ProtocolGetCallForwardingStatusAdapterInterface {
public:
    ProtocolGetCallForwardingStatusAdapterInterface() = default;
    virtual ~ProtocolGetCallForwardingStatusAdapterInterface() = default;
public:
    virtual int GetCfNum() = 0;
    virtual bool HasValidLength() = 0;
    virtual int GetCfInfo(RIL_CallForwardInfo* pCfInfo, int index) = 0;
    virtual void DebugPrintCfInfo(RIL_CallForwardInfo* pCfInfo) = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetCallWaitingAdapterInterface
 */
class ProtocolGetCallWaitingAdapterInterface {
public:
    ProtocolGetCallWaitingAdapterInterface() = default;
    virtual ~ProtocolGetCallWaitingAdapterInterface() = default;
public:
    virtual int GetServiceStatus() = 0;
    virtual int GetServiceClass() = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolUssdIndAdapterInterface
 */
class ProtocolUssdIndAdapterInterface {
public:
    ProtocolUssdIndAdapterInterface() = default;
    virtual ~ProtocolUssdIndAdapterInterface() = default;
public:
    virtual int GetDecodedUssd(char* decodedUssd, size_t buf_size, int& dcs) = 0;
    virtual int GetUssdStatus() = 0;
};

/**
 * ProtocolSsSvcIndAdapterInterface
 */
class ProtocolSsSvcIndAdapterInterface {
public:
    ProtocolSsSvcIndAdapterInterface() = default;
    virtual ~ProtocolSsSvcIndAdapterInterface() = default;
public:
    virtual int GetNotificationType() = 0;
    virtual int GetCode() = 0;
    virtual int GetCugIndex() = 0;
    virtual UINT GetSSType() = 0;
    virtual int GetNumberLength() = 0;
    virtual char* GetNumber() = 0;
};

/**
 * ProtocolGetColpAdapterInterface
 */
class ProtocolGetColpAdapterInterface {
public:
    ProtocolGetColpAdapterInterface() = default;
    virtual ~ProtocolGetColpAdapterInterface() = default;
public:
    virtual int GetColpStatus() = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetColrAdapterInterface
 */
class ProtocolGetColrAdapterInterface {
public:
    ProtocolGetColrAdapterInterface() = default;
    virtual ~ProtocolGetColrAdapterInterface() = default;
public:
    virtual int GetColrStatus() = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolSendCallConfirmRespAdapterInterface
 */
class ProtocolSendCallConfirmRespAdapterInterface {
public:
    ProtocolSendCallConfirmRespAdapterInterface() = default;
    virtual ~ProtocolSendCallConfirmRespAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetPreferredVoicePrivacyModeAdapterInterface
 */
class ProtocolGetPreferredVoicePrivacyModeAdapterInterface {
public:
    ProtocolGetPreferredVoicePrivacyModeAdapterInterface() = default;
    virtual ~ProtocolGetPreferredVoicePrivacyModeAdapterInterface() = default;
public:
    virtual int GetPreferredVoicePrivacyMode() = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolCdmaCallWaitingIndAdapterInterface
 */
class ProtocolCdmaCallWaitingIndAdapterInterface {
public:
    ProtocolCdmaCallWaitingIndAdapterInterface() = default;
    virtual ~ProtocolCdmaCallWaitingIndAdapterInterface() = default;
public:
    virtual int GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo) = 0;
};

/**
 * ProtocolCdmaInfoListIndAdapterInterface
 */
class ProtocolCdmaInfoListIndAdapterInterface {
public:
    ProtocolCdmaInfoListIndAdapterInterface() = default;
    virtual ~ProtocolCdmaInfoListIndAdapterInterface() = default;
public:
    virtual int GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const = 0;
    virtual int GetNumberOfInfoRecs() const = 0;
};

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapterInterface
 */
class ProtocolCdmaOtaProvisionStatusIndAdapterInterface {
    public:
        ProtocolCdmaOtaProvisionStatusIndAdapterInterface() = default;
        virtual ~ProtocolCdmaOtaProvisionStatusIndAdapterInterface() = default;
    public:
        virtual int GetOtaProvisionStatus() const = 0;
};

/**
 * ProtocolEmergencyCallListIndAdapterInterface
 */
class ProtocolEmergencyCallListIndAdapterInterface {
public:
    ProtocolEmergencyCallListIndAdapterInterface() = default;
    virtual ~ProtocolEmergencyCallListIndAdapterInterface() = default;
public:
  virtual int GetVersion() const = 0;
  virtual const char *GetMcc() const = 0;
  virtual const char *GetMnc() const = 0;
  virtual int GetNum() const = 0;
  virtual void GetEccInfo(EccInfo *eccInfo) = 0;
};

/**
 * ProtocolEmergencySupportRatModeIndAdapterInterface
 */
class ProtocolEmergencySupportRatModeIndAdapterInterface {
public:
    ProtocolEmergencySupportRatModeIndAdapterInterface() = default;
    virtual ~ProtocolEmergencySupportRatModeIndAdapterInterface() = default;
public:
    virtual int GetSupportRatMode() const = 0;
};

/**
 * ProtocolExitEmergencyCbModeRespAdapterInterface
 */
class ProtocolExitEmergencyCbModeRespAdapterInterface {
public:
    ProtocolExitEmergencyCbModeRespAdapterInterface() = default;
    virtual ~ProtocolExitEmergencyCbModeRespAdapterInterface() = default;
public:
    virtual bool GetResult() const = 0;
    virtual RIL_Errno GetRilErrorCode() const = 0;

    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolUnsolOnSSAdapterInterface
 */
class ProtocolUnsolOnSSAdapterInterface {
public:
    ProtocolUnsolOnSSAdapterInterface() = default;
    virtual ~ProtocolUnsolOnSSAdapterInterface() = default;
public:
    virtual int GetServiceType() const = 0;
    virtual int GetRequestType() const = 0;
    virtual int GetTeleServiceType() const = 0;
    virtual int GetServiceClass() const = 0;
    virtual int GetResult() const = 0;
    virtual int GetDataType() const = 0;
    virtual bool GetData(void *pData) const = 0;
};

/**
 * ProtocolSrvccHoIndAdapterInterface
 */
class ProtocolSrvccHoIndAdapterInterface {
public:
    ProtocolSrvccHoIndAdapterInterface() = default;
    virtual ~ProtocolSrvccHoIndAdapterInterface() = default;
public:
    virtual BYTE GetSrvccHoType() const = 0;
    virtual BYTE GetSrvccHoStatus() const = 0;
    virtual BYTE GetSrvccHoFailCause() const = 0;
};
#endif /* __PROTOCOL_CALL_ADAPTER_INTERFACE_H__ */
