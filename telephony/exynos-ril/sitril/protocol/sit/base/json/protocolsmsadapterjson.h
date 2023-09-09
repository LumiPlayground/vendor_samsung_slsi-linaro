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
 * protocolsmsadapterjson.h
 *
 *  Created on: 2021. 8. 27.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SMS_ADAPTER_JSON_H__
#define __PROTOCOL_SMS_ADAPTER_JSON_H__

#include "protocoljsonadapter.h"
#include "protocolsmsadapterinterface.h"
#include "sitdef.h"

class BroadcastSmsConfigs;
class CCdmaBroadcastSmsConfigs;
class CCdmaSmsMessage;

class ProtocolSendSmsRespAdapterJson : public ProtocolSendSmsRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSendSmsRespAdapterJson(const ModemData *pModemData);
    ProtocolSendSmsRespAdapterJson(const ProtocolSendSmsRespAdapterJson&) = delete;
    ProtocolSendSmsRespAdapterJson& operator=(ProtocolSendSmsRespAdapterJson const&) = delete;
    virtual ~ProtocolSendSmsRespAdapterJson() {
        if (m_pPdu) {
            delete[] m_pPdu;
            m_pPdu = NULL;
        }
    }

  public:
    int GetRef() const;
    int GetSmsRspErrorCode() const;
    int GetPduSize() const {
        return m_nPduSize;
    }
    const char *GetPdu() const {
        return m_pPdu;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    char *m_pPdu;
    unsigned int m_nPduSize;

    void SetPdu();
    void Init();
};

class ProtocolWriteSmsToSimRespAdapterJson : public ProtocolWriteSmsToSimRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolWriteSmsToSimRespAdapterJson(const ModemData *pModemData);
    ProtocolWriteSmsToSimRespAdapterJson(const ProtocolWriteSmsToSimRespAdapterJson&) = delete;
    ProtocolWriteSmsToSimRespAdapterJson& operator=(ProtocolWriteSmsToSimRespAdapterJson const&) = delete;
    virtual ~ProtocolWriteSmsToSimRespAdapterJson() = default;

  public:
    int GetIndex() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolGetBcstSmsConfRespAdapterJson : public ProtocolGetBcstSmsConfRespAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    BroadcastSmsConfigs *m_pCBcsc;

  public:
    ProtocolGetBcstSmsConfRespAdapterJson(const ModemData *pModemData);
    ProtocolGetBcstSmsConfRespAdapterJson(const ProtocolGetBcstSmsConfRespAdapterJson&) = delete;
    ProtocolGetBcstSmsConfRespAdapterJson& operator=(ProtocolGetBcstSmsConfRespAdapterJson const&) = delete;
    virtual ~ProtocolGetBcstSmsConfRespAdapterJson();

    UINT8 GetConfigsNumber();
    RIL_GSM_BroadcastSmsConfigInfo **GetConfigsInfoPointers();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void SetBcstConfInfoList();
};

class ProtocolSmsCapacityOnSimRespAdapterJson : public ProtocolSmsCapacityOnSimRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSmsCapacityOnSimRespAdapterJson(const ModemData *pModemData)
        : ProtocolSmsCapacityOnSimRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData, SIT_GET_STORED_SMS_COUNT) {}
    virtual ~ProtocolSmsCapacityOnSimRespAdapterJson() = default;

  public:
    int GetSimId() const;
    int GetTotalNum() const;
    int GetUsedNum() const;
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSmscAddrRespAdapterJson : public ProtocolSmscAddrRespAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    char m_smsc[MAX_GSM_SMS_SERVICE_CENTER_ADDR * 2];
    unsigned int m_nSmscLen;

    void SetSmsc();
    void Init();

  public:
    ProtocolSmscAddrRespAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolSmscAddrRespAdapterJson() = default;

  public:
    const char *GetSmsc() const {
        return m_nSmscLen > 0 ? m_smsc : NULL;
    }
    int GetSmscLength() const {
        return m_nSmscLen;
    }
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolNewSmsIndAdapterJson : public ProtocolNewSmsIndAdapterInterface, public ProtocolJsonIndAdapter {
  private:
    char m_tpdu[MAX_GSM_SMS_TPDU_SIZE * 2 + 1];
    // tpdu len in hex string
    int m_nTpduLen;

    void SetPdu();
    void Init();

  public:
    ProtocolNewSmsIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolNewSmsIndAdapterJson() = default;

  public:
    int GetPduSize() const {
        return m_nTpduLen;
    }
    const char *GetPdu() const;
    int GetTpid() const;
};

class ProtocolNewBcstSmsAdapterJson : public ProtocolNewBcstSmsAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolNewBcstSmsAdapterJson(const ModemData *pModemData);
    ProtocolNewBcstSmsAdapterJson(const ProtocolNewBcstSmsAdapterJson&) = delete;
    ProtocolNewBcstSmsAdapterJson& operator=(ProtocolNewBcstSmsAdapterJson const&) = delete;
    virtual ~ProtocolNewBcstSmsAdapterJson();

  public:
    BYTE *GetBcst() const {
        return m_pBcstMsg;
    }
    UINT16 GetBcstLen() const {
        return m_nBcstMsgLen;
    }

  private:
    BYTE *m_pBcstMsg;
    unsigned int m_nBcstMsgLen;

    void SetBcst();
};

class ProtocolCdmaSendSmsRespAdapterJson : public ProtocolCdmaSendSmsRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolCdmaSendSmsRespAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolCdmaSendSmsRespAdapterJson() = default;

    int GetRef();
    int GetSmsRspErrorClass();
    int GetSmsRspCauseCode();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void Init();
};

class ProtocolCdmaNewSmsIndAdapterJson : public ProtocolCdmaNewSmsIndAdapterInterface, public ProtocolJsonIndAdapter {
  private:
    CCdmaSmsMessage *m_pCCdmaSmsMsg;

  public:
    ProtocolCdmaNewSmsIndAdapterJson(const ModemData *pModemData);
    ProtocolCdmaNewSmsIndAdapterJson(const ProtocolCdmaNewSmsIndAdapterJson&) = delete;
    ProtocolCdmaNewSmsIndAdapterJson& operator=(ProtocolCdmaNewSmsIndAdapterJson const&) = delete;
    virtual ~ProtocolCdmaNewSmsIndAdapterJson();

    int GetMessageLength() {
        return sizeof(RIL_CDMA_SMS_Message);
    }
    const RIL_CDMA_SMS_Message *GetRilCdmaSmsMsg() const;
    int GetTpid() const;

  private:
    void SetCdmaSmsMsg();
};

class ProtocolCdmaWriteSmsToRuimRespAdapterJson : public ProtocolCdmaWriteSmsToRuimRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolCdmaWriteSmsToRuimRespAdapterJson(const ModemData *pModemData)
        : ProtocolCdmaWriteSmsToRuimRespAdapterInterface(), ProtocolJsonRespAdapter(pModemData, SIT_CDMA_WRITE_SMS_TO_RUIM) {}
    virtual ~ProtocolCdmaWriteSmsToRuimRespAdapterJson() = default;

    int GetIndex();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolGetCdmaBcstSmsConfRespAdapterJson : public ProtocolGetCdmaBcstSmsConfRespAdapterInterface, public ProtocolJsonRespAdapter {
  private:
    CCdmaBroadcastSmsConfigs *m_pCCbcsc;

  public:
    ProtocolGetCdmaBcstSmsConfRespAdapterJson(const ModemData *pModemData);
    ProtocolGetCdmaBcstSmsConfRespAdapterJson(const ProtocolGetCdmaBcstSmsConfRespAdapterJson&) = delete;
    ProtocolGetCdmaBcstSmsConfRespAdapterJson& operator=(ProtocolGetCdmaBcstSmsConfRespAdapterJson const&) = delete;
    virtual ~ProtocolGetCdmaBcstSmsConfRespAdapterJson();

    UINT8 GetConfigsNumber();
    RIL_CDMA_BroadcastSmsConfigInfo **GetConfigsInfoPointers();
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }

  private:
    void SetBcstConfInfoList();
};

class ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson : public ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface, public ProtocolJsonIndAdapter {
  private:
    RIL_CDMA_SMS_Message *m_ptRcsm;

  public:
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson(const ModemData *pModemData);
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson(const ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson&) = delete;
    ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson& operator=(ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson const&) = delete;
    virtual ~ProtocolCdmaVoiceMsgWaitingInfoIndAdapterJson();

    int GetMessageLength() {
        return sizeof(RIL_CDMA_SMS_Message);
    }
    const RIL_CDMA_SMS_Message *GetRilCdmaSmsMsg() const {
        return m_ptRcsm;
    }

  private:
    void SetCdmaSmsMsg();
};

#endif  // __PROTOCOL_SMS_ADAPTER_JSON_H__
