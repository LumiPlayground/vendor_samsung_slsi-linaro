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
 * protocolcalladapterjson.h
 *
 *  Created on: 2021. 07. 15.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_CALL_ADAPTER_JSON_H__
#define __PROTOCOL_CALL_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolcalladapterinterface.h"
#include "json/value.h"
#include "sitdef.h"

/**
 * ProtocolGetCurrentCallAdapterJson
 */
class ProtocolGetCurrentCallAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetCurrentCallAdapterInterface {
  public:
    ProtocolGetCurrentCallAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolGetCurrentCallAdapterJson() = default;

  public:
    bool HasValidLength();
    int GetCallNum();
    int GetCallInfo(CallInfo *pCallInfo, int index);
    void DebugPrintCallInfo(CallInfo *pCallInfo);

    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }

  private:
    int ConvertSitToUusInfo(UusInfo *pUusInfo, int uusType, int uus_dcs, int len, BYTE *pData);
};

/**
 * ProtocolGetLastCallFailCauseAdapterJson
 */
class ProtocolGetLastCallFailCauseAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetLastCallFailCauseAdapterInterface {
  public:
    ProtocolGetLastCallFailCauseAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolGetLastCallFailCauseAdapterJson() = default;

  public:
    int GetLastCallFailCause();
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetCallForwardingStatusAdapterJson
 */
class ProtocolGetCallForwardingStatusAdapterJson : public ProtocolJsonRespAdapter,
                                                   public ProtocolGetCallForwardingStatusAdapterInterface {
  public:
    ProtocolGetCallForwardingStatusAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolGetCallForwardingStatusAdapterJson() = default;

  public:
    int GetCfNum();
    bool HasValidLength();
    int GetCfInfo(RIL_CallForwardInfo *pCfInfo, int index);
    void DebugPrintCfInfo(RIL_CallForwardInfo *pCfInfo);
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    // json object to store call forwarding status
    // see ns_ss_call_forward_status
    Json::Value m_jsonCallFrwrdStatus;
    // array of strings to store numbers
    char m_number[MAX_CALL_FORWARD_STATUS_NUM][MAX_SS_NUM_LEN + 1];

    void Init();
};

/**
 * ProtocolGetCallWaitingAdapterJson
 */
class ProtocolGetCallWaitingAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetCallWaitingAdapterInterface {
  public:
    ProtocolGetCallWaitingAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_CALL_WAITING), ProtocolGetCallWaitingAdapterInterface() {}
    virtual ~ProtocolGetCallWaitingAdapterJson() = default;

  public:
    int GetServiceStatus();
    int GetServiceClass();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetClipAdapterJson
 */
class ProtocolGetClipAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetClipAdapterInterface {
  public:
    ProtocolGetClipAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_CLIP), ProtocolGetClipAdapterInterface() {}
    virtual ~ProtocolGetClipAdapterJson() = default;

  public:
    int GetClipStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetClirAdapterJson
 */
class ProtocolGetClirAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetClirAdapterInterface {
  public:
    ProtocolGetClirAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_CLIR), ProtocolGetClirAdapterInterface() {}
    virtual ~ProtocolGetClirAdapterJson() = default;

  public:
    int GetClirStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolUssdIndAdapterJson
 */
class ProtocolUssdIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolUssdIndAdapterInterface {
  public:
    ProtocolUssdIndAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_ON_USSD), ProtocolUssdIndAdapterInterface() {}
    virtual ~ProtocolUssdIndAdapterJson() = default;

  public:
    int GetDecodedUssd(char *decodedUssd, size_t buf_size, int &dcs);
    int GetUssdStatus();
};

/**
 * ProtocolSsSvcIndAdapterJson
 */
class ProtocolSsSvcIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolSsSvcIndAdapterInterface {
  public:
    ProtocolSsSvcIndAdapterJson(const ModemData *pModemData);
    ProtocolSsSvcIndAdapterJson(const ProtocolSsSvcIndAdapterJson &) = delete;
    ProtocolSsSvcIndAdapterJson& operator=(ProtocolSsSvcIndAdapterJson const&) = delete;
    virtual ~ProtocolSsSvcIndAdapterJson();

  public:
    int GetNotificationType();
    int GetCode();
    int GetCugIndex();
    UINT GetSSType();
    int GetNumberLength();
    char *GetNumber();

  private:
    char *m_pNumber;
};

/**
 * ProtocolGetColpAdapterJson
 */
class ProtocolGetColpAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetColpAdapterInterface {
  public:
    ProtocolGetColpAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_COLP), ProtocolGetColpAdapterInterface() {}
    virtual ~ProtocolGetColpAdapterJson() = default;

  public:
    int GetColpStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetColrAdapterJson
 */
class ProtocolGetColrAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetColrAdapterInterface {
  public:
    ProtocolGetColrAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_COLR), ProtocolGetColrAdapterInterface() {}
    virtual ~ProtocolGetColrAdapterJson() = default;

  public:
    int GetColrStatus();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolSendCallConfirmRespAdapterJson
 */
class ProtocolSendCallConfirmRespAdapterJson : public ProtocolJsonRespAdapter, public ProtocolSendCallConfirmRespAdapterInterface {
  public:
    ProtocolSendCallConfirmRespAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSendCallConfirmRespAdapterJson() = default;

  public:
    int GetResult() const;
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolGetPreferredVoicePrivacyModeAdapterJson
 */
class ProtocolGetPreferredVoicePrivacyModeAdapterJson : public ProtocolJsonRespAdapter, public ProtocolGetPreferredVoicePrivacyModeAdapterInterface {
  public:
    ProtocolGetPreferredVoicePrivacyModeAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolGetPreferredVoicePrivacyModeAdapterJson() = default;

  public:
    int GetPreferredVoicePrivacyMode();
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

/**
 * ProtocolEmergencyCallListIndAdapterJson
 */
class ProtocolEmergencyCallListIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolEmergencyCallListIndAdapterInterface {
  private:
    int mVersion;
    char mMcc[MAX_MCC_LEN + 1];
    char mMnc[MAX_MNC_LEN + 1];
    int mNum;

  private:
    void Init();

  public:
    ProtocolEmergencyCallListIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmergencyCallListIndAdapterJson() = default;

  public:
    int GetVersion() const;
    const char *GetMcc() const;
    const char *GetMnc() const;
    int GetNum() const;
    void GetEccInfo(EccInfo *eccInfo);
};

/**
 * ProtocolSrvccHoIndAdapterJson
 */
class ProtocolSrvccHoIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolSrvccHoIndAdapterInterface {
  public:
    ProtocolSrvccHoIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSrvccHoIndAdapterJson() = default;

  public:
    BYTE GetSrvccHoType() const;
    BYTE GetSrvccHoStatus() const;
    BYTE GetSrvccHoFailCause() const;
};

/**
 * ProtocolCdmaOtaProvisionStatusIndAdapterJson
 */
class ProtocolCdmaOtaProvisionStatusIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolCdmaOtaProvisionStatusIndAdapterInterface {
  public:
    ProtocolCdmaOtaProvisionStatusIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolCdmaOtaProvisionStatusIndAdapterJson() = default;

  public:
    int GetOtaProvisionStatus() const;
};

/**
 * ProtocolEmergencySupportRatModeIndAdapterJson
 */
class ProtocolEmergencySupportRatModeIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolEmergencySupportRatModeIndAdapterInterface {
  public:
    ProtocolEmergencySupportRatModeIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmergencySupportRatModeIndAdapterJson() = default;

  public:
    int GetSupportRatMode() const;
};

/**
 * ProtocolCdmaCallWaitingIndAdapterJson
 */
class ProtocolCdmaCallWaitingIndAdapterJson : public ProtocolJsonIndAdapter,
                                              public ProtocolCdmaCallWaitingIndAdapterInterface {
  public:
    ProtocolCdmaCallWaitingIndAdapterJson(const ModemData *pModemData);
    ProtocolCdmaCallWaitingIndAdapterJson(const ProtocolCdmaCallWaitingIndAdapterJson &) = delete;
    ProtocolCdmaCallWaitingIndAdapterJson& operator=(ProtocolCdmaCallWaitingIndAdapterJson const&) = delete;
    virtual ~ProtocolCdmaCallWaitingIndAdapterJson();

  public:
    int GetCwInfo(RIL_CDMA_CallWaiting_v6 *pCwInfo);

  private:
    char *m_pNumber;
    char *m_pName;
};

/**
 * ProtocolCdmaInfoListIndAdapterJson
 */
class ProtocolCdmaInfoListIndAdapterJson : public ProtocolJsonIndAdapter, public ProtocolCdmaInfoListIndAdapterInterface {
  public:
    ProtocolCdmaInfoListIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolCdmaInfoListIndAdapterJson() = default;

  public:
    int GetCdmaInfo(RIL_CDMA_InformationRecord &cdmaInfo, int index) const;
    int GetNumberOfInfoRecs() const;
};
#endif /* __PROTOCOL_CALL_ADAPTER_JSON_H__ */
