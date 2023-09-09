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
 * protocolsimadapter.h
 *
 *  Created on: 2021. 06. 09.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SIM_ADAPTER_H__
#define __PROTOCOL_SIM_ADAPTER_H__

#include "protocolsimadapterinterface.h"

class ModemData;

class ProtocolSimResponseAdapter : public ProtocolSimResponseAdapterInterface {
private:
    ProtocolSimResponseAdapterInterface *protocolSimResponseAdapterInterface = NULL;

public:
    ProtocolSimResponseAdapter(const ModemData *pModemData);
    ProtocolSimResponseAdapter(const ProtocolSimResponseAdapter&) = delete;
    ProtocolSimResponseAdapter& operator=(ProtocolSimResponseAdapter const&) = delete;
    virtual ~ProtocolSimResponseAdapter();
    const char *GetParameter() const { return protocolSimResponseAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolSimResponseAdapterInterface->GetParameterLength(); }
    UINT GetErrorCode() const { return protocolSimResponseAdapterInterface->GetErrorCode(); }
};

class ProtocolSimStatusAdapter : public ProtocolSimStatusAdapterInterface {
private:
    ProtocolSimStatusAdapterInterface *protocolSimStatusAdapterInterface = NULL;

public:
    ProtocolSimStatusAdapter(const ModemData *pModemData);
    ProtocolSimStatusAdapter(const ProtocolSimStatusAdapter&) = delete;
    ProtocolSimStatusAdapter& operator=(ProtocolSimStatusAdapter const&) = delete;
    virtual ~ProtocolSimStatusAdapter();
    int GetCardState() const;
    int GetUniversalPinState() const;
    int GetApplicationCount() const;
    // Applications Status Information
    int GetAppsType(int nIndex) const;
    int GetAppsState(int nIndex) const;
    int GetPersonalSubstate(int nIndex) const;
    char *GetAID(int nIndex) const;
    int GetApplicationLabel(int nIndex, BYTE *pAppLabel) const;
    int GetPin1Replaced(int nIndex) const;
    int GetPinState(int nIndex, int nPinIndex) const;
    int GetPinRemainCount(int nIndex, int nPinIndex) const;
    int GetPukRemainCount(int nIndex, int nPukIndex) const;
    // Extension (1.2)
    bool GetEsimNoProfile() const;
    int GetPhysicalSlotId() const;
    int GetAtrLength() const;
    char *GetAtr() const;
    int GetIccidLength() const;
    char *GetIccid() const;

    // Extension (1.4)
    int GetEidLength() const;
    char *GetEid() const;

    // Extension (2.0)
    int GetPortId() const { return protocolSimStatusAdapterInterface->GetPortId(); }

    UINT GetErrorCode() const { return protocolSimStatusAdapterInterface->GetErrorCode(); }
};

class ProtocolSimVerifyPinAdapter : public ProtocolSimVerifyPinAdapterInterface {
private:
    ProtocolSimVerifyPinAdapterInterface *protocolSimVerifyPinAdapterInterface = NULL;

public:
    ProtocolSimVerifyPinAdapter(const ModemData *pModemData);
    ProtocolSimVerifyPinAdapter(const ProtocolSimVerifyPinAdapter&) = delete;
    ProtocolSimVerifyPinAdapter& operator=(ProtocolSimVerifyPinAdapter const&) = delete;
    virtual ~ProtocolSimVerifyPinAdapter();
    int GetPinIndex() const;
    int GetRemainCount() const;
    UINT GetErrorCode() const { return protocolSimVerifyPinAdapterInterface->GetErrorCode(); }
};

class ProtocolSimVerifyPukAdapter : public ProtocolSimVerifyPukAdapterInterface {
private:
    ProtocolSimVerifyPukAdapterInterface *protocolSimVerifyPukAdapterInterface = NULL;

public:
    ProtocolSimVerifyPukAdapter(const ModemData *pModemData);
    ProtocolSimVerifyPukAdapter(const ProtocolSimVerifyPukAdapter&) = delete;
    ProtocolSimVerifyPukAdapter& operator=(ProtocolSimVerifyPukAdapter const&) = delete;
    virtual ~ProtocolSimVerifyPukAdapter();
    int GetPukIndex() const;
    int GetRemainCount() const;
    UINT GetErrorCode() const { return protocolSimVerifyPukAdapterInterface->GetErrorCode(); }
};

class ProtocolSimVerifyNetLockAdapter : public ProtocolSimVerifyNetLockAdapterInterface {
private:
    ProtocolSimVerifyNetLockAdapterInterface *protocolSimVerifyNetLockAdapterInterface = NULL;

public:
    ProtocolSimVerifyNetLockAdapter(const ModemData *pModemData);
    ProtocolSimVerifyNetLockAdapter(const ProtocolSimVerifyNetLockAdapter&) = delete;
    ProtocolSimVerifyNetLockAdapter& operator=(ProtocolSimVerifyNetLockAdapter const&) = delete;
    virtual ~ProtocolSimVerifyNetLockAdapter();
    int GetRemainCount() const;
    UINT GetErrorCode() const { return protocolSimVerifyNetLockAdapterInterface->GetErrorCode(); }
};

class ProtocolSimIOAdapter : public ProtocolSimIOAdapterInterface {
private:
    ProtocolSimIOAdapterInterface *protocolSimIOAdapterInterface = NULL;

public:
    ProtocolSimIOAdapter(const ModemData *pModemData);
    ProtocolSimIOAdapter(const ProtocolSimIOAdapter&) = delete;
    ProtocolSimIOAdapter& operator=(ProtocolSimIOAdapter const&) = delete;
    virtual ~ProtocolSimIOAdapter();
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    BYTE *GetResponse() const;
    int GetResponseLength() const;
    UINT GetErrorCode() const { return protocolSimIOAdapterInterface->GetErrorCode(); }
};

class ProtocolSimGetFacilityLockAdapter : public ProtocolSimGetFacilityLockAdapterInterface {
private:
    ProtocolSimGetFacilityLockAdapterInterface *protocolSimGetFacilityLockAdapterInterface = NULL;

public:
    ProtocolSimGetFacilityLockAdapter(const ModemData *pModemData);
    ProtocolSimGetFacilityLockAdapter(const ProtocolSimGetFacilityLockAdapter&) = delete;
    ProtocolSimGetFacilityLockAdapter& operator=(ProtocolSimGetFacilityLockAdapter const&) = delete;
    virtual ~ProtocolSimGetFacilityLockAdapter();
    int GetServiceClass() const;
    UINT GetErrorCode() const { return protocolSimGetFacilityLockAdapterInterface->GetErrorCode(); }
};

class ProtocolSimSetFacilityLockAdapter : public ProtocolSimSetFacilityLockAdapterInterface {
private:
    ProtocolSimSetFacilityLockAdapterInterface *protocolSimSetFacilityLockAdapterInterface = NULL;

public:
    ProtocolSimSetFacilityLockAdapter(const ModemData *pModemData);
    ProtocolSimSetFacilityLockAdapter(const ProtocolSimSetFacilityLockAdapter&) = delete;
    ProtocolSimSetFacilityLockAdapter& operator=(ProtocolSimSetFacilityLockAdapter const&) = delete;
    virtual ~ProtocolSimSetFacilityLockAdapter();
    int GetRemainCount() const;
    UINT GetErrorCode() const { return protocolSimSetFacilityLockAdapterInterface->GetErrorCode(); }
};

class ProtocolSimGetSimAuthAdapter : public ProtocolSimGetSimAuthAdapterInterface {
private:
    ProtocolSimGetSimAuthAdapterInterface *protocolSimGetSimAuthAdapterInterface = NULL;

public:
    ProtocolSimGetSimAuthAdapter(const ModemData *pModemData);
    ProtocolSimGetSimAuthAdapter(const ProtocolSimGetSimAuthAdapter&) = delete;
    ProtocolSimGetSimAuthAdapter& operator=(ProtocolSimGetSimAuthAdapter const&) = delete;
    virtual ~ProtocolSimGetSimAuthAdapter();
    int GetAuthType() const;
    int GetAuthLength() const;
    BYTE *GetAuth() const;
    UINT GetErrorCode() const { return protocolSimGetSimAuthAdapterInterface->GetErrorCode(); }
    UINT GetParameterLength() const { return protocolSimGetSimAuthAdapterInterface->GetParameterLength(); }
};

class ProtocolSimTransmitApduBasicAdapter : public ProtocolSimTransmitApduBasicAdapterInterface {
private:
    ProtocolSimTransmitApduBasicAdapterInterface *protocolSimTransmitApduBasicAdapterInterface = NULL;

public:
    ProtocolSimTransmitApduBasicAdapter(const ModemData *pModemData);
    ProtocolSimTransmitApduBasicAdapter(const ProtocolSimTransmitApduBasicAdapter&) = delete;
    ProtocolSimTransmitApduBasicAdapter& operator=(ProtocolSimTransmitApduBasicAdapter const&) = delete;
    virtual ~ProtocolSimTransmitApduBasicAdapter();
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    int GetApduLength() const;
    BYTE *GetApdu() const;
    UINT GetErrorCode() const { return protocolSimTransmitApduBasicAdapterInterface->GetErrorCode(); }
};

class ProtocolSimOpenChannelAdapter : public ProtocolSimOpenChannelAdapterInterface {
private:
    ProtocolSimOpenChannelAdapterInterface *protocolSimOpenChannelAdapterInterface = NULL;

public:
    ProtocolSimOpenChannelAdapter(const ModemData *pModemData);
    ProtocolSimOpenChannelAdapter(const ProtocolSimOpenChannelAdapter&) = delete;
    ProtocolSimOpenChannelAdapter& operator=(ProtocolSimOpenChannelAdapter const&) = delete;
    virtual ~ProtocolSimOpenChannelAdapter();
    int GetSessionID() const;
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    BYTE *GetResponse() const;
    int GetResponseLength() const;
    UINT GetErrorCode() const { return protocolSimOpenChannelAdapterInterface->GetErrorCode(); }
};

class ProtocolSimTransmitApduChannelAdapter : public ProtocolSimTransmitApduChannelAdapterInterface {
private:
    ProtocolSimTransmitApduChannelAdapterInterface *protocolSimTransmitApduChannelAdapterInterface = NULL;

public:
    ProtocolSimTransmitApduChannelAdapter(const ModemData *pModemData);
    ProtocolSimTransmitApduChannelAdapter(const ProtocolSimTransmitApduChannelAdapter&) = delete;
    ProtocolSimTransmitApduChannelAdapter& operator=(ProtocolSimTransmitApduChannelAdapter const&) = delete;
    virtual ~ProtocolSimTransmitApduChannelAdapter();
    BYTE GetSw1() const;
    BYTE GetSw2() const;
    int GetApduLength() const;
    BYTE *GetApdu() const;
    UINT GetErrorCode() const { return protocolSimTransmitApduChannelAdapterInterface->GetErrorCode(); }
    UINT GetParameterLength() const { return protocolSimTransmitApduChannelAdapterInterface->GetParameterLength(); }
    const char *GetParameter() const { return protocolSimTransmitApduChannelAdapterInterface->GetParameter(); }
};

class ProtocolSimImsiAdapter : public ProtocolSimImsiAdapterInterface {
private:
    ProtocolSimImsiAdapterInterface *protocolSimImsiAdapterInterface = NULL;

public:
    ProtocolSimImsiAdapter(const ModemData *pModemData);
    ProtocolSimImsiAdapter(const ProtocolSimImsiAdapter&) = delete;
    ProtocolSimImsiAdapter& operator=(ProtocolSimImsiAdapter const&) = delete;
    virtual ~ProtocolSimImsiAdapter();
    const char *GetImsi() const;
    UINT GetErrorCode() const { return protocolSimImsiAdapterInterface->GetErrorCode(); }
};

class ProtocolSimGetGbaAuthAdapter : public ProtocolSimGetGbaAuthAdapterInterface {
private:
    ProtocolSimGetGbaAuthAdapterInterface *protocolSimGetGbaAuthAdapterInterface = NULL;

public:
    ProtocolSimGetGbaAuthAdapter(const ModemData *pModemData);
    ProtocolSimGetGbaAuthAdapter(const ProtocolSimGetGbaAuthAdapter&) = delete;
    ProtocolSimGetGbaAuthAdapter& operator=(ProtocolSimGetGbaAuthAdapter const&) = delete;
    virtual ~ProtocolSimGetGbaAuthAdapter();
    int GetGbaAuthLength() const;
    BYTE *GetGbaAuth() const;
    virtual UINT GetErrorCode() const { return protocolSimGetGbaAuthAdapterInterface->GetErrorCode(); }
};

class ProtocolSimATRAdapter : public ProtocolSimATRAdapterInterface {
private:
    ProtocolSimATRAdapterInterface *protocolSimATRAdapterInterface = NULL;

public:
    ProtocolSimATRAdapter(const ModemData *pModemData);
    ProtocolSimATRAdapter(const ProtocolSimATRAdapter&) = delete;
    ProtocolSimATRAdapter& operator=(ProtocolSimATRAdapter const&) = delete;
    virtual ~ProtocolSimATRAdapter();
    BYTE GetResult() const;
    BYTE GetATRLength() const;
    const char *GetATR() const;
    UINT GetErrorCode() const { return protocolSimATRAdapterInterface->GetErrorCode(); }
};

class ProtocolSimReadPbEntry: public ProtocolSimReadPbEntryInterface {
private:
    ProtocolSimReadPbEntryInterface *protocolSimReadPbEntryInterface = NULL;

public:
    ProtocolSimReadPbEntry(const ModemData *pModemData);
    ProtocolSimReadPbEntry(const ProtocolSimReadPbEntry&) = delete;
    ProtocolSimReadPbEntry& operator=(ProtocolSimReadPbEntry const&) = delete;
    virtual ~ProtocolSimReadPbEntry();
    int GetRecordId() const;
    int GetPbType() const;
    int GetDataLength() const;
    char *GetEntryData();
    UINT GetErrorCode() const { return protocolSimReadPbEntryInterface->GetErrorCode(); }
};

class ProtocolSimUpdatePbEntry: public ProtocolSimUpdatePbEntryInterface {
private:
    ProtocolSimUpdatePbEntryInterface *protocolSimUpdatePbEntryInterface = NULL;

public:
    ProtocolSimUpdatePbEntry(const ModemData *pModemData);
    ProtocolSimUpdatePbEntry(const ProtocolSimUpdatePbEntry&) = delete;
    ProtocolSimUpdatePbEntry& operator=(ProtocolSimUpdatePbEntry const&) = delete;
    virtual ~ProtocolSimUpdatePbEntry();
    BYTE GetMode();
    BYTE GetPbtype();
    UINT16 GetRecordId();
    UINT GetErrorCode() const { return protocolSimUpdatePbEntryInterface->GetErrorCode(); }
};

class ProtocolSimPbStorageInfoAdapter:public ProtocolSimPbStorageInfoAdapterInterface {
private:
    ProtocolSimPbStorageInfoAdapterInterface *protocolSimPbStorageInfoAdapterInterface = NULL;

public:
    ProtocolSimPbStorageInfoAdapter(const ModemData *pModemData);
    ProtocolSimPbStorageInfoAdapter(const ProtocolSimPbStorageInfoAdapter&) = delete;
    ProtocolSimPbStorageInfoAdapter& operator=(ProtocolSimPbStorageInfoAdapter const&) = delete;
    virtual ~ProtocolSimPbStorageInfoAdapter();
    int GetPbType() const;
    int GetTotalCount() const;
    int GetUsedCount() const ;
    UINT GetErrorCode() const { return protocolSimPbStorageInfoAdapterInterface->GetErrorCode(); }
};

class ProtocolSimPbCapaAdapter : public ProtocolSimPbCapaAdapterInterface {
private:
    ProtocolSimPbCapaAdapterInterface *protocolSimPbCapaAdapterInterface = NULL;

public:
    ProtocolSimPbCapaAdapter(const ModemData *pModemData);
    ProtocolSimPbCapaAdapter(const ProtocolSimPbCapaAdapter&) = delete;
    ProtocolSimPbCapaAdapter& operator=(ProtocolSimPbCapaAdapter const&) = delete;
    virtual ~ProtocolSimPbCapaAdapter();
    int GetEntryNum() const;
    bool GetPbCapa(int *pb, int entryNum);
    UINT GetErrorCode() const { return protocolSimPbCapaAdapterInterface->GetErrorCode(); }
};

class ProtocolSimPbReadyAdapter : public ProtocolSimPbReadyAdapterInterface {
private:
    ProtocolSimPbReadyAdapterInterface *protocolSimPbReadyAdapterInterface = NULL;
public:
    ProtocolSimPbReadyAdapter(const ModemData *pModemData);
    ProtocolSimPbReadyAdapter(const ProtocolSimPbReadyAdapter&) = delete;
    ProtocolSimPbReadyAdapter& operator=(ProtocolSimPbReadyAdapter const&) = delete;
    virtual ~ProtocolSimPbReadyAdapter();
    int GetPbReady() const;
};

class ProtocolSimGetCarrierRestrictionsAdapter : public ProtocolSimGetCarrierRestrictionsAdapterInterface {
private:
    ProtocolSimGetCarrierRestrictionsAdapterInterface *protocolSimGetCarrierRestrictionsAdapterInterface = NULL;

public:
    ProtocolSimGetCarrierRestrictionsAdapter(const ModemData *pModemData);
    ProtocolSimGetCarrierRestrictionsAdapter(const ProtocolSimGetCarrierRestrictionsAdapter&) = delete;
    ProtocolSimGetCarrierRestrictionsAdapter& operator=(ProtocolSimGetCarrierRestrictionsAdapter const&) = delete;
    virtual ~ProtocolSimGetCarrierRestrictionsAdapter();
    int GetAllowedCarrierLength() const;
    int GetExcludedCarrierLength() const;
    int GetAllowedCarriers(RIL_Carrier *pCarriers, int nSize) const;
    int GetExcludedCarriers(RIL_Carrier *pCarriers, int nSize) const;
    bool GetAllowedCarriersPrioritized() const;
    INT32 GetMultiSimPolicy() const;
    UINT GetErrorCode() const { return protocolSimGetCarrierRestrictionsAdapterInterface->GetErrorCode(); }
};

class ProtocolUiccSubStatusChangeAdapter : public ProtocolUiccSubStatusChangeAdapterInterface {
private:
    ProtocolUiccSubStatusChangeAdapterInterface *protocolUiccSubStatusChangeAdapterInterface = NULL;

public:
    ProtocolUiccSubStatusChangeAdapter(const ModemData *pModemData);
    ProtocolUiccSubStatusChangeAdapter(const ProtocolUiccSubStatusChangeAdapter&) = delete;
    ProtocolUiccSubStatusChangeAdapter& operator=(ProtocolUiccSubStatusChangeAdapter const&) = delete;
    virtual ~ProtocolUiccSubStatusChangeAdapter();
    int GetState() const;
    const char *GetParameter() const { return protocolUiccSubStatusChangeAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolUiccSubStatusChangeAdapterInterface->GetParameterLength(); }
};

class ProtocolSimLockInfoAdapter : public ProtocolSimLockInfoAdapterInterface {
private:
    ProtocolSimLockInfoAdapterInterface *protocolSimLockInfoAdapterInterface = NULL;

public:
    ProtocolSimLockInfoAdapter(const ModemData *pModemData);
    ProtocolSimLockInfoAdapter(const ProtocolSimLockInfoAdapter&) = delete;
    ProtocolSimLockInfoAdapter& operator=(ProtocolSimLockInfoAdapter const&) = delete;
    virtual ~ProtocolSimLockInfoAdapter();
    int GetPolicy() const;
    int GetStatus() const;
    int GetLockType() const;
    int GetMaxRetryCount() const;
    int GetRemainCount() const;
    int GetLockCodeCount() const;
    const char *GetLockCode() const;
    int GetLockCodeSize() const;
    UINT GetErrorCode() const { return protocolSimLockInfoAdapterInterface->GetErrorCode(); }
};

#ifdef AUTO_VERIFY_PIN_WITH_CP
class ProtocolSimEncryptedPinAdapter : public ProtocolSimEncryptedPinAdapterInterface {
private:
    ProtocolSimEncryptedPinAdapterInterface *protocolSimEncryptedPinAdapterInterface = NULL;

public:
    ProtocolSimEncryptedPinAdapter(const ModemData *pModemData);
    virtual ~ProtocolSimEncryptedPinAdapter();
    int GetEncryptedPinLen() const
			{ return protocolSimEncryptedPinAdapterInterface->GetEncryptedPinLen(); }
    int GetPinLen() const { return protocolSimEncryptedPinAdapterInterface->GetPinLen(); }
    const char *GetEncryptedPin() const;
};
#endif

class ProtocolSimTrayStatusChangeAdapter : public ProtocolSimTrayStatusChangeAdapterInterface {
private:
    ProtocolSimTrayStatusChangeAdapterInterface *protocolSimTrayStatusChangeAdapterInterface = NULL;

public:
    ProtocolSimTrayStatusChangeAdapter(const ModemData *pModemData);
    ProtocolSimTrayStatusChangeAdapter(const ProtocolSimTrayStatusChangeAdapter&) = delete;
    ProtocolSimTrayStatusChangeAdapter& operator=(ProtocolSimTrayStatusChangeAdapter const&) = delete;
    virtual ~ProtocolSimTrayStatusChangeAdapter();
    int GetState() const;
    const char *GetParameter() const { return protocolSimTrayStatusChangeAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolSimTrayStatusChangeAdapterInterface->GetParameterLength(); }
};

#endif /* __PROTOCOL_SIM_ADAPTER_H__ */
