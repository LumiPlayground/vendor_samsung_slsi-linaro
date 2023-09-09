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
 * protocolcalladapter.h
 *
 *  Created on: 2021. 06. 08.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_CALL_ADAPTER_H__
#define __PROTOCOL_CALL_ADAPTER_H__

#include "protocolcalladapterinterface.h"
#include "types.h"

class ModemData;

/**
 * ProtocolGetCurrentCallAdapter
 */
class ProtocolGetCurrentCallAdapter : public ProtocolGetCurrentCallAdapterInterface {
private:
    ProtocolGetCurrentCallAdapterInterface *protocolGetCurrentCallAdapterInterface = NULL;

public:
    ProtocolGetCurrentCallAdapter(const ModemData *pModemData);
    ProtocolGetCurrentCallAdapter(const ProtocolGetCurrentCallAdapter&) = delete;
    ProtocolGetCurrentCallAdapter& operator=(ProtocolGetCurrentCallAdapter const&) = delete;
    virtual ~ProtocolGetCurrentCallAdapter();

public:
    bool HasValidLength();
    int GetCallNum();
    int GetCallInfo(CallInfo* pCallInfo, int index);
    void DebugPrintCallInfo(CallInfo* pCallInfo);
    UINT GetErrorCode() const { return protocolGetCurrentCallAdapterInterface->GetErrorCode(); }

private:
    //int ConvertSitToUusInfo(UusInfo* pUusInfo, int uusType, int uus_dcs, int len, BYTE* pData);
    //template<typename T>
    //int SetCallInfo(CallInfo* (&pCallInfo), int index, T* (&pcallinfo));
};

/**
 * ProtocolGetLastCallFailCauseAdapter
 */
class ProtocolGetLastCallFailCauseAdapter : public ProtocolGetLastCallFailCauseAdapterInterface {
private:
    ProtocolGetLastCallFailCauseAdapterInterface *protocolGetLastCallFailCauseAdapterInterface = NULL;

public:
    ProtocolGetLastCallFailCauseAdapter(const ModemData *pModemData);
    ProtocolGetLastCallFailCauseAdapter(const ProtocolGetLastCallFailCauseAdapter&) = delete;
    ProtocolGetLastCallFailCauseAdapter& operator=(ProtocolGetLastCallFailCauseAdapter const&) = delete;
    virtual ~ProtocolGetLastCallFailCauseAdapter();

public:
    int GetLastCallFailCause();
    UINT GetErrorCode() const { return protocolGetLastCallFailCauseAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetClipAdapter
 */
class ProtocolGetClipAdapter : public ProtocolGetClipAdapterInterface {
private:
    ProtocolGetClipAdapterInterface *protocolGetClipAdapterInterface = NULL;

public:
    ProtocolGetClipAdapter(const ModemData *pModemData);
    ProtocolGetClipAdapter(const ProtocolGetClipAdapter&) = delete;
    ProtocolGetClipAdapter& operator=(ProtocolGetClipAdapter const&) = delete;
    virtual ~ProtocolGetClipAdapter();

public:
    int GetClipStatus();
    UINT GetErrorCode() const { return protocolGetClipAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetClirAdapter
 */
class ProtocolGetClirAdapter : public ProtocolGetClirAdapterInterface {
private:
    ProtocolGetClirAdapterInterface *protocolGetClirAdapterInterface = NULL;

public:
    ProtocolGetClirAdapter(const ModemData *pModemData);
    ProtocolGetClirAdapter(const ProtocolGetClirAdapter&) = delete;
    ProtocolGetClirAdapter& operator=(ProtocolGetClirAdapter const&) = delete;
    virtual ~ProtocolGetClirAdapter();

public:
    int GetClirStatus();
    UINT GetErrorCode() const { return protocolGetClirAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetCallForwardingStatusAdapter
 */
class ProtocolGetCallForwardingStatusAdapter : public ProtocolGetCallForwardingStatusAdapterInterface {
private:
    ProtocolGetCallForwardingStatusAdapterInterface *protocolGetCallForwardingStatusAdapterInterface = NULL;

public:
    ProtocolGetCallForwardingStatusAdapter(const ModemData *pModemData);
    ProtocolGetCallForwardingStatusAdapter(const ProtocolGetCallForwardingStatusAdapter&) = delete;
    ProtocolGetCallForwardingStatusAdapter& operator=(ProtocolGetCallForwardingStatusAdapter const&) = delete;
    virtual ~ProtocolGetCallForwardingStatusAdapter();

public:
    int GetCfNum();
    bool HasValidLength();
    int GetCfInfo(RIL_CallForwardInfo* pCfInfo, int index);
    void DebugPrintCfInfo(RIL_CallForwardInfo* pCfInfo);
    UINT GetErrorCode() const { return protocolGetCallForwardingStatusAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetCallWaitingAdapter
 */
class ProtocolGetCallWaitingAdapter : public ProtocolGetCallWaitingAdapterInterface {
private:
    ProtocolGetCallWaitingAdapterInterface *protocolGetCallWaitingAdapterInterface = NULL;

public:
    ProtocolGetCallWaitingAdapter(const ModemData *pModemData);
    ProtocolGetCallWaitingAdapter(const ProtocolGetCallWaitingAdapter&) = delete;
    ProtocolGetCallWaitingAdapter& operator=(ProtocolGetCallWaitingAdapter const&) = delete;
    virtual ~ProtocolGetCallWaitingAdapter();

public:
    int GetServiceStatus();
    int GetServiceClass();
    UINT GetErrorCode() const { return protocolGetCallWaitingAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolUssdIndAdapter
 */
class ProtocolUssdIndAdapter : public ProtocolUssdIndAdapterInterface {
private:
    ProtocolUssdIndAdapterInterface *protocolUssdIndAdapterInterface = NULL;

public:
    ProtocolUssdIndAdapter(const ModemData *pModemData);
    ProtocolUssdIndAdapter(const ProtocolUssdIndAdapter&) = delete;
    ProtocolUssdIndAdapter& operator=(ProtocolUssdIndAdapter const&) = delete;
    virtual ~ProtocolUssdIndAdapter();

public:
    int GetDecodedUssd(char* decodedUssd, size_t buf_size, int& dcs);
    int GetUssdStatus();
};

/**
 * ProtocolSsSvcIndAdapter
 */
class ProtocolSsSvcIndAdapter : public ProtocolSsSvcIndAdapterInterface {
private:
    ProtocolSsSvcIndAdapterInterface *protocolSsSvcIndAdapterInterface = NULL;

public:
    ProtocolSsSvcIndAdapter(const ModemData *pModemData);
    ProtocolSsSvcIndAdapter(const ProtocolSsSvcIndAdapter&) = delete;
    ProtocolSsSvcIndAdapter& operator=(ProtocolSsSvcIndAdapter const&) = delete;
    virtual ~ProtocolSsSvcIndAdapter();

public:
    int GetNotificationType();
    int GetCode();
    int GetCugIndex();
    UINT GetSSType();
    int GetNumberLength();
    char* GetNumber();
};

/**
 * ProtocolGetColpAdapter
 */
class ProtocolGetColpAdapter : public ProtocolGetColpAdapterInterface {
private:
    ProtocolGetColpAdapterInterface *protocolGetColpAdapterInterface = NULL;

public:
    ProtocolGetColpAdapter(const ModemData *pModemData);
    ProtocolGetColpAdapter(const ProtocolGetColpAdapter&) = delete;
    ProtocolGetColpAdapter& operator=(ProtocolGetColpAdapter const&) = delete;
    virtual ~ProtocolGetColpAdapter();

public:
    int GetColpStatus();
    UINT GetErrorCode() const { return protocolGetColpAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetColrAdapter
 */
class ProtocolGetColrAdapter : public ProtocolGetColrAdapterInterface {
private:
    ProtocolGetColrAdapterInterface *protocolGetColrAdapterInterface = NULL;

public:
    ProtocolGetColrAdapter(const ModemData *pModemData);
    ProtocolGetColrAdapter(const ProtocolGetColrAdapter&) = delete;
    ProtocolGetColrAdapter& operator=(ProtocolGetColrAdapter const&) = delete;
    virtual ~ProtocolGetColrAdapter();

public:
    int GetColrStatus();
    UINT GetErrorCode() const { return protocolGetColrAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolSendCallConfirmRespAdapter
 */
class ProtocolSendCallConfirmRespAdapter : public ProtocolSendCallConfirmRespAdapterInterface {
private:
    ProtocolSendCallConfirmRespAdapterInterface *protocolSendCallConfirmRespAdapterInterface = NULL;

public:
    ProtocolSendCallConfirmRespAdapter(const ModemData *pModemData);
    ProtocolSendCallConfirmRespAdapter(const ProtocolSendCallConfirmRespAdapter&) = delete;
    ProtocolSendCallConfirmRespAdapter& operator=(ProtocolSendCallConfirmRespAdapter const&) = delete;
    virtual ~ProtocolSendCallConfirmRespAdapter();

public:
    int GetResult() const;
    UINT GetErrorCode() const { return protocolSendCallConfirmRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetPreferredVoicePrivacyModeAdapter
 */
class ProtocolGetPreferredVoicePrivacyModeAdapter : public ProtocolGetPreferredVoicePrivacyModeAdapterInterface {
private:
    ProtocolGetPreferredVoicePrivacyModeAdapterInterface *protocolGetPreferredVoicePrivacyModeAdapterInterface = NULL;

public:
    ProtocolGetPreferredVoicePrivacyModeAdapter(const ModemData *pModemData);
    ProtocolGetPreferredVoicePrivacyModeAdapter(const ProtocolGetPreferredVoicePrivacyModeAdapter&) = delete;
    ProtocolGetPreferredVoicePrivacyModeAdapter& operator=(ProtocolGetPreferredVoicePrivacyModeAdapter const&) = delete;
    virtual ~ProtocolGetPreferredVoicePrivacyModeAdapter();

public:
    int GetPreferredVoicePrivacyMode();
    UINT GetErrorCode() const { return protocolGetPreferredVoicePrivacyModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolCdmaCallWaitingIndAdapter
 */
class ProtocolCdmaCallWaitingIndAdapter : public ProtocolCdmaCallWaitingIndAdapterInterface {
private:
    ProtocolCdmaCallWaitingIndAdapterInterface *protocolCdmaCallWaitingIndAdapterInterface = NULL;

public:
    ProtocolCdmaCallWaitingIndAdapter(const ModemData *pModemData);
    ProtocolCdmaCallWaitingIndAdapter(const ProtocolCdmaCallWaitingIndAdapter&) = delete;
    ProtocolCdmaCallWaitingIndAdapter& operator=(ProtocolCdmaCallWaitingIndAdapter const&) = delete;
    virtual ~ProtocolCdmaCallWaitingIndAdapter();

public:
    int GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo);
};

/**
 * ProtocolCdmaInfoListIndAdapter
 */
class ProtocolCdmaInfoListIndAdapter : public ProtocolCdmaInfoListIndAdapterInterface {
private:
    ProtocolCdmaInfoListIndAdapterInterface *protocolCdmaInfoListIndAdapterInterface = NULL;

public:
    ProtocolCdmaInfoListIndAdapter(const ModemData *pModemData);
    ProtocolCdmaInfoListIndAdapter(const ProtocolCdmaInfoListIndAdapter&) = delete;
    ProtocolCdmaInfoListIndAdapter& operator=(ProtocolCdmaInfoListIndAdapter const&) = delete;
    virtual ~ProtocolCdmaInfoListIndAdapter();

public:
    int GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const;
    int GetNumberOfInfoRecs() const;
};

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapter
 */
class ProtocolCdmaOtaProvisionStatusIndAdapter : public ProtocolCdmaOtaProvisionStatusIndAdapterInterface {
private:
    ProtocolCdmaOtaProvisionStatusIndAdapterInterface *protocolCdmaOtaProvisionStatusIndAdapterInterface = NULL;

public:
    ProtocolCdmaOtaProvisionStatusIndAdapter(const ModemData *pModemData);
    ProtocolCdmaOtaProvisionStatusIndAdapter(const ProtocolCdmaOtaProvisionStatusIndAdapter&) = delete;
    ProtocolCdmaOtaProvisionStatusIndAdapter& operator=(ProtocolCdmaOtaProvisionStatusIndAdapter const&) = delete;
    virtual ~ProtocolCdmaOtaProvisionStatusIndAdapter();

public:
    int GetOtaProvisionStatus() const;
};

/**
 * ProtocolEmergencyCallListIndAdapter
 */
class ProtocolEmergencyCallListIndAdapter : public ProtocolEmergencyCallListIndAdapterInterface {
private:
    ProtocolEmergencyCallListIndAdapterInterface *protocolEmergencyCallListIndAdapterInterface = NULL;

public:
    ProtocolEmergencyCallListIndAdapter(const ModemData *pModemData);
    ProtocolEmergencyCallListIndAdapter(const ProtocolEmergencyCallListIndAdapter&) = delete;
    ProtocolEmergencyCallListIndAdapter& operator=(ProtocolEmergencyCallListIndAdapter const&) = delete;
    virtual ~ProtocolEmergencyCallListIndAdapter();

public:
  int GetVersion() const;
  const char *GetMcc() const;
  const char *GetMnc() const;
  int GetNum() const;
  void GetEccInfo(EccInfo *eccInfo);
};

/**
 * ProtocolEmergencySupportRatModeIndAdapter
 */
class ProtocolEmergencySupportRatModeIndAdapter : public ProtocolEmergencySupportRatModeIndAdapterInterface {
private:
    ProtocolEmergencySupportRatModeIndAdapterInterface *protocolEmergencySupportRatModeIndAdapterInterface = NULL;

public:
    ProtocolEmergencySupportRatModeIndAdapter(const ModemData *pModemData);
    ProtocolEmergencySupportRatModeIndAdapter(const ProtocolEmergencySupportRatModeIndAdapter&) = delete;
    ProtocolEmergencySupportRatModeIndAdapter& operator=(ProtocolEmergencySupportRatModeIndAdapter const&) = delete;
    virtual ~ProtocolEmergencySupportRatModeIndAdapter();

public:
    int GetSupportRatMode() const;
};

/**
 * ProtocolExitEmergencyCbModeRespAdapter
 */
class ProtocolExitEmergencyCbModeRespAdapter : public ProtocolExitEmergencyCbModeRespAdapterInterface {
private:
    ProtocolExitEmergencyCbModeRespAdapterInterface *protocolExitEmergencyCbModeRespAdapterInterface = NULL;

public:
    ProtocolExitEmergencyCbModeRespAdapter(const ModemData *pModemData);
    ProtocolExitEmergencyCbModeRespAdapter(const ProtocolExitEmergencyCbModeRespAdapter&) = delete;
    ProtocolExitEmergencyCbModeRespAdapter& operator=(ProtocolExitEmergencyCbModeRespAdapter const&) = delete;
    virtual ~ProtocolExitEmergencyCbModeRespAdapter();

public:
    bool GetResult() const;
    RIL_Errno GetRilErrorCode() const;
    UINT GetErrorCode() const { return protocolExitEmergencyCbModeRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolUnsolOnSSAdapter
 */
class ProtocolUnsolOnSSAdapter : public ProtocolUnsolOnSSAdapterInterface {
private:
    ProtocolUnsolOnSSAdapterInterface *protocolUnsolOnSSAdapterInterface = NULL;

public:
    ProtocolUnsolOnSSAdapter(const ModemData *pModemData);
    ProtocolUnsolOnSSAdapter(const ProtocolUnsolOnSSAdapter&) = delete;
    ProtocolUnsolOnSSAdapter& operator=(ProtocolUnsolOnSSAdapter const&) = delete;
    virtual ~ProtocolUnsolOnSSAdapter();

public:
    int GetServiceType() const;
    int GetRequestType() const;
    int GetTeleServiceType() const;
    int GetServiceClass() const;
    int GetResult() const;
    int GetDataType() const;
    bool GetData(void *pData) const;
};

/**
 * ProtocolSrvccHoIndAdapter
 */
class ProtocolSrvccHoIndAdapter : public ProtocolSrvccHoIndAdapterInterface {
private:
    ProtocolSrvccHoIndAdapterInterface *protocolSrvccHoIndAdapterInterface = NULL;

public:
    ProtocolSrvccHoIndAdapter(const ModemData *pModemData);
    ProtocolSrvccHoIndAdapter(const ProtocolSrvccHoIndAdapter&) = delete;
    ProtocolSrvccHoIndAdapter& operator=(ProtocolSrvccHoIndAdapter const&) = delete;
    virtual ~ProtocolSrvccHoIndAdapter();

public:
    BYTE GetSrvccHoType() const;
    BYTE GetSrvccHoStatus() const;
    BYTE GetSrvccHoFailCause() const;
};
#endif /* __PROTOCOL_CALL_ADAPTER_H__ */
