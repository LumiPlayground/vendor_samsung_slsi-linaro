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
 * protocolcalladapterlegacy.h
 *
 *  Created on: 2014. 6. 27.
 *      Author: jhdaniel.kim
 */

#ifndef __PROTOCOL_CALL_ADAPTER_LEGACY_H__
#define __PROTOCOL_CALL_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolcalladapterinterface.h"

/**
 * ProtocolGetCurrentCallAdapterLegacy
 */
class ProtocolGetCurrentCallAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetCurrentCallAdapterInterface {
  public:
    ProtocolGetCurrentCallAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetCurrentCallAdapterInterface() {}
    virtual ~ProtocolGetCurrentCallAdapterLegacy() = default;

  public:
    bool HasValidLength();
    int GetCallNum();
    int GetCallInfo(CallInfo *pCallInfo, int index);
    void DebugPrintCallInfo(CallInfo *pCallInfo);

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    int ConvertSitToUusInfo(UusInfo *pUusInfo, int uusType, int uus_dcs, int len, BYTE *pData);
    template <typename T>
    int SetCallInfo(CallInfo *(&pCallInfo), int index, T *(&pcallinfo));
    int GetCallInfoV1_1(CallInfo *pCallInfo, int index);
    int GetCallInfoV1_2(CallInfo *pCallInfo, int index);
    int GetCallInfoV1_6(CallInfo *pCallInfo, int index);
};

/**
 * ProtocolGetLastCallFailCauseAdapterLegacy
 */
class ProtocolGetLastCallFailCauseAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetLastCallFailCauseAdapterInterface {
  public:
    ProtocolGetLastCallFailCauseAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetLastCallFailCauseAdapterInterface() {}
    virtual ~ProtocolGetLastCallFailCauseAdapterLegacy() = default;

  public:
    int GetLastCallFailCause();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetClipAdapterLegacy
 */
class ProtocolGetClipAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetClipAdapterInterface {
  public:
    ProtocolGetClipAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetClipAdapterInterface() {}
    virtual ~ProtocolGetClipAdapterLegacy() = default;

  public:
    int GetClipStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetClirAdapterLegacy
 */
class ProtocolGetClirAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetClirAdapterInterface {
  public:
    ProtocolGetClirAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetClirAdapterInterface() {}
    virtual ~ProtocolGetClirAdapterLegacy() = default;

  public:
    int GetClirStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetCallForwardingStatusAdapterLegacy
 */
class ProtocolGetCallForwardingStatusAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetCallForwardingStatusAdapterInterface {
  public:
    ProtocolGetCallForwardingStatusAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetCallForwardingStatusAdapterInterface() {}
    virtual ~ProtocolGetCallForwardingStatusAdapterLegacy() = default;

  public:
    int GetCfNum();
    bool HasValidLength();
    int GetCfInfo(RIL_CallForwardInfo *pCfInfo, int index);
    void DebugPrintCfInfo(RIL_CallForwardInfo *pCfInfo);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetCallWaitingAdapterLegacy
 */
class ProtocolGetCallWaitingAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetCallWaitingAdapterInterface {
  public:
    ProtocolGetCallWaitingAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetCallWaitingAdapterInterface() {}
    virtual ~ProtocolGetCallWaitingAdapterLegacy() = default;

  public:
    int GetServiceStatus();
    int GetServiceClass();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolUssdIndAdapterLegacy
 */
class ProtocolUssdIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolUssdIndAdapterInterface {
  public:
    ProtocolUssdIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolUssdIndAdapterInterface() {}
    virtual ~ProtocolUssdIndAdapterLegacy() = default;

  public:
    int GetDecodedUssd(char *decodedUssd, size_t buf_size, int &dcs);
    int GetUssdStatus();
};

/**
 * ProtocolSsSvcIndAdapterLegacy
 */
class ProtocolSsSvcIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolSsSvcIndAdapterInterface {
  public:
    ProtocolSsSvcIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolSsSvcIndAdapterInterface() {}
    virtual ~ProtocolSsSvcIndAdapterLegacy() = default;

  public:
    int GetNotificationType();
    int GetCode();
    int GetCugIndex();
    UINT GetSSType();
    int GetNumberLength();
    char *GetNumber();
};

/**
 * ProtocolGetColpAdapterLegacy
 */
class ProtocolGetColpAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetColpAdapterInterface {
  public:
    ProtocolGetColpAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetColpAdapterInterface() {}
    virtual ~ProtocolGetColpAdapterLegacy() = default;

  public:
    int GetColpStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetColrAdapterLegacy
 */
class ProtocolGetColrAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetColrAdapterInterface {
  public:
    ProtocolGetColrAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetColrAdapterInterface() {}
    virtual ~ProtocolGetColrAdapterLegacy() = default;

  public:
    int GetColrStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolSendCallConfirmRespAdapterLegacy
 */
class ProtocolSendCallConfirmRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolSendCallConfirmRespAdapterInterface {
  public:
    ProtocolSendCallConfirmRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolSendCallConfirmRespAdapterInterface() {}
    virtual ~ProtocolSendCallConfirmRespAdapterLegacy() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetPreferredVoicePrivacyModeAdapterLegacy
 */
class ProtocolGetPreferredVoicePrivacyModeAdapterLegacy : public ProtocolRespAdapter, public ProtocolGetPreferredVoicePrivacyModeAdapterInterface {
  public:
    ProtocolGetPreferredVoicePrivacyModeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolGetPreferredVoicePrivacyModeAdapterInterface() {}
    virtual ~ProtocolGetPreferredVoicePrivacyModeAdapterLegacy() = default;

  public:
    int GetPreferredVoicePrivacyMode();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolCdmaCallWaitingIndAdapterLegacy
 */
class ProtocolCdmaCallWaitingIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolCdmaCallWaitingIndAdapterInterface {
  public:
    ProtocolCdmaCallWaitingIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolCdmaCallWaitingIndAdapterInterface() {}
    virtual ~ProtocolCdmaCallWaitingIndAdapterLegacy() = default;

  public:
    int GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo);
};

/**
 * ProtocolCdmaInfoListIndAdapterLegacy
 */
class ProtocolCdmaInfoListIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolCdmaInfoListIndAdapterInterface {
  public:
    ProtocolCdmaInfoListIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolCdmaInfoListIndAdapterInterface() {}
    virtual ~ProtocolCdmaInfoListIndAdapterLegacy() = default;

  public:
    int GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const;
    int GetNumberOfInfoRecs() const;
};

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapterLegacy
 */
class ProtocolCdmaOtaProvisionStatusIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolCdmaOtaProvisionStatusIndAdapterInterface {
  public:
    ProtocolCdmaOtaProvisionStatusIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolCdmaOtaProvisionStatusIndAdapterInterface() {}
    virtual ~ProtocolCdmaOtaProvisionStatusIndAdapterLegacy() = default;

  public:
    int GetOtaProvisionStatus() const;
};

/**
 * ProtocolEmergencyCallListIndAdapterLegacy
 */
class ProtocolEmergencyCallListIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmergencyCallListIndAdapterInterface {
  private:
    int mVersion;
    char mMcc[MAX_MCC_LEN + 1];
    char mMnc[MAX_MNC_LEN + 1];
    int mNum;

  private:
    void Init();
    int GetNumV1_6();
    int findEmergencyNumber(int numLen, const char *targetNumber, EccInfo *eccInfo);

  public:
    ProtocolEmergencyCallListIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolEmergencyCallListIndAdapterInterface() {
        Init();
    }
    virtual ~ProtocolEmergencyCallListIndAdapterLegacy() = default;

  public:
    int GetVersion() const;
    const char *GetMcc() const;
    const char *GetMnc() const;
    int GetNum() const;
    void GetEccInfo(EccInfo *eccInfo);
};

/**
 * ProtocolEmergencySupportRatModeIndAdapterLegacy
 */
class ProtocolEmergencySupportRatModeIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmergencySupportRatModeIndAdapterInterface {
  public:
    ProtocolEmergencySupportRatModeIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolEmergencySupportRatModeIndAdapterInterface() {}
    virtual ~ProtocolEmergencySupportRatModeIndAdapterLegacy() = default;

  public:
    int GetSupportRatMode() const;
};

/**
 * ProtocolExitEmergencyCbModeRespAdapterLegacy
 */
class ProtocolExitEmergencyCbModeRespAdapterLegacy : public ProtocolRespAdapter, public ProtocolExitEmergencyCbModeRespAdapterInterface {
  public:
    ProtocolExitEmergencyCbModeRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData), ProtocolExitEmergencyCbModeRespAdapterInterface() {}
    virtual ~ProtocolExitEmergencyCbModeRespAdapterLegacy() = default;

  public:
    bool GetResult() const;
    RIL_Errno GetRilErrorCode() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolUnsolOnSSAdapterLegacy
 */
class ProtocolUnsolOnSSAdapterLegacy : public ProtocolIndAdapter, public ProtocolUnsolOnSSAdapterInterface {
  public:
    ProtocolUnsolOnSSAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData), ProtocolUnsolOnSSAdapterInterface() {}
    virtual ~ProtocolUnsolOnSSAdapterLegacy() = default;

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
 * ProtocolSrvccHoIndAdapterLegacy
 */
class ProtocolSrvccHoIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolSrvccHoIndAdapterInterface {
  private:
    BYTE srvccInfo[3];

  private:
    void Init();

  public:
    ProtocolSrvccHoIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolSrvccHoIndAdapterLegacy() = default;

  public:
    BYTE GetSrvccHoType() const;
    BYTE GetSrvccHoStatus() const;
    BYTE GetSrvccHoFailCause() const;
};
#endif /* __PROTOCOL_CALL_ADAPTER_LEGACY_H__ */
