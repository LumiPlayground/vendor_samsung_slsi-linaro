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
 * protocolsimadapterinterface.h
 *
 *  Created on: 2021. 06. 09.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SIM_ADAPTER_INTERFACE_H__
#define __PROTOCOL_SIM_ADAPTER_INTERFACE_H__

#include <telephony/ril.h>
#include <base/types.h>

class ProtocolSimResponseAdapterInterface {
public:
    ProtocolSimResponseAdapterInterface() { }
    virtual ~ProtocolSimResponseAdapterInterface() = default;

    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimStatusAdapterInterface {
public:
    ProtocolSimStatusAdapterInterface() { }
    virtual ~ProtocolSimStatusAdapterInterface() = default;

public:
    virtual int GetCardState() const  = 0;
    virtual int GetUniversalPinState() const  = 0;
    virtual int GetApplicationCount() const = 0;
    // Applications Status Information
    virtual int GetAppsType(int nIndex) const = 0;
    virtual int GetAppsState(int nIndex) const = 0;
    virtual int GetPersonalSubstate(int nIndex) const = 0;
    virtual char *GetAID(int nIndex) const = 0;
    virtual int GetApplicationLabel(int nIndex, BYTE *pAppLabel) const = 0;
    virtual int GetPin1Replaced(int nIndex) const = 0;
    virtual int GetPinState(int nIndex, int nPinIndex) const = 0;
    virtual int GetPinRemainCount(int nIndex, int nPinIndex) const = 0;
    virtual int GetPukRemainCount(int nIndex, int nPukIndex) const = 0;
    // Extension (1.2)
    virtual bool GetEsimNoProfile() const = 0;
    virtual int GetPhysicalSlotId() const = 0;
    virtual int GetAtrLength() const = 0;
    virtual char *GetAtr() const = 0;
    virtual int GetIccidLength() const = 0;
    virtual char *GetIccid() const = 0;

    // Extension (1.4)
    virtual int GetEidLength() const = 0;
    virtual char *GetEid() const = 0;

    // Extension (2.0)
    virtual int GetPortId() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimVerifyPinAdapterInterface {
public:
    ProtocolSimVerifyPinAdapterInterface() { }
    virtual ~ProtocolSimVerifyPinAdapterInterface() = default;
public:
    virtual int GetPinIndex() const = 0;
    virtual int GetRemainCount() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimVerifyPukAdapterInterface {
public:
    ProtocolSimVerifyPukAdapterInterface() { }
    virtual ~ProtocolSimVerifyPukAdapterInterface() = default;
public:
    virtual int GetPukIndex() const = 0;
    virtual int GetRemainCount() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimChangePinAdapterInterface {
public:
    ProtocolSimChangePinAdapterInterface() { }
    virtual ~ProtocolSimChangePinAdapterInterface() = default;

public:
    virtual int GetPinIndex() const = 0;
    virtual int GetRemainCount() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimVerifyNetLockAdapterInterface {
public:
    ProtocolSimVerifyNetLockAdapterInterface() { }
    virtual ~ProtocolSimVerifyNetLockAdapterInterface() = default;

    virtual int GetRemainCount() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimIOAdapterInterface {
public:
    ProtocolSimIOAdapterInterface() { }
    virtual ~ProtocolSimIOAdapterInterface() = default;

public:
    virtual BYTE GetSw1() const = 0;
    virtual BYTE GetSw2() const = 0;
    virtual BYTE *GetResponse() const = 0;
    virtual int GetResponseLength() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimGetFacilityLockAdapterInterface {
public:
    ProtocolSimGetFacilityLockAdapterInterface() { }
    virtual ~ProtocolSimGetFacilityLockAdapterInterface() = default;

public:
    virtual int GetServiceClass() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimSetFacilityLockAdapterInterface {
public:
    ProtocolSimSetFacilityLockAdapterInterface() { }
    virtual ~ProtocolSimSetFacilityLockAdapterInterface() = default;

public:
    virtual int GetRemainCount() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimGetSimAuthAdapterInterface {
public:
    ProtocolSimGetSimAuthAdapterInterface() { }
    virtual ~ProtocolSimGetSimAuthAdapterInterface() = default;

public:
    virtual int GetAuthType() const = 0;
    virtual int GetAuthLength() const = 0;
    virtual BYTE *GetAuth() const = 0;
    virtual UINT GetErrorCode() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

class ProtocolSimTransmitApduBasicAdapterInterface {
public:
    ProtocolSimTransmitApduBasicAdapterInterface() { }
    virtual ~ProtocolSimTransmitApduBasicAdapterInterface() = default;

public:
    virtual BYTE GetSw1() const = 0;
    virtual BYTE GetSw2() const = 0;
    virtual int GetApduLength() const = 0;
    virtual BYTE *GetApdu() const = 0;
	virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimOpenChannelAdapterInterface {
public:
    ProtocolSimOpenChannelAdapterInterface() { }
    virtual ~ProtocolSimOpenChannelAdapterInterface() = default;

public:
    virtual int GetSessionID() const = 0;
    virtual BYTE GetSw1() const = 0;
    virtual BYTE GetSw2() const = 0;
    virtual BYTE *GetResponse() const = 0;
    virtual int GetResponseLength() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimTransmitApduChannelAdapterInterface {
public:
    ProtocolSimTransmitApduChannelAdapterInterface() { }
    virtual ~ProtocolSimTransmitApduChannelAdapterInterface() = default;

public:
    virtual BYTE GetSw1() const = 0;
    virtual BYTE GetSw2() const = 0;
    virtual int GetApduLength() const = 0;
    virtual BYTE *GetApdu() const = 0;
    virtual UINT GetErrorCode() const = 0;
    virtual UINT GetParameterLength() const = 0;
    virtual const char *GetParameter() const = 0;
};

class ProtocolSimImsiAdapterInterface {
public:
    ProtocolSimImsiAdapterInterface() { }
    virtual ~ProtocolSimImsiAdapterInterface() = default;
public:
    virtual const char *GetImsi() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimGetGbaAuthAdapterInterface {
public:
    ProtocolSimGetGbaAuthAdapterInterface() { }
    virtual ~ProtocolSimGetGbaAuthAdapterInterface() = default;
public:
    virtual int GetGbaAuthLength() const = 0;
    virtual BYTE *GetGbaAuth() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimATRAdapterInterface {
public:
    ProtocolSimATRAdapterInterface() {  }
    virtual ~ProtocolSimATRAdapterInterface() = default;
public:
    virtual BYTE GetResult() const = 0;
    virtual BYTE GetATRLength() const = 0;
    virtual const char *GetATR() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimReadPbEntryInterface {
public:
    ProtocolSimReadPbEntryInterface() {  }
    virtual ~ProtocolSimReadPbEntryInterface() = default;
    virtual int GetRecordId() const = 0;
    virtual int GetPbType() const = 0;
    virtual int GetDataLength() const = 0;
    virtual char *GetEntryData() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimUpdatePbEntryInterface {
public:
    ProtocolSimUpdatePbEntryInterface() { }
    virtual ~ProtocolSimUpdatePbEntryInterface() = default;
    virtual BYTE GetMode() = 0;
    virtual BYTE GetPbtype() = 0;
    virtual UINT16 GetRecordId() = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimPbStorageInfoAdapterInterface {
public:
    ProtocolSimPbStorageInfoAdapterInterface() { }
    virtual ~ProtocolSimPbStorageInfoAdapterInterface() = default;
    virtual int GetPbType() const  = 0;
    virtual int GetTotalCount() const = 0;
    virtual int GetUsedCount() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimPbCapaAdapterInterface {
public:
    ProtocolSimPbCapaAdapterInterface() { }
    virtual ~ProtocolSimPbCapaAdapterInterface() = default;
public:
    virtual int GetEntryNum() const = 0;
    virtual bool GetPbCapa(int *pb, int entryNum) = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSimPbReadyAdapterInterface {
public:
    ProtocolSimPbReadyAdapterInterface() { }
    virtual ~ProtocolSimPbReadyAdapterInterface() = default;
    virtual int GetPbReady() const = 0;
};

class ProtocolSimIccidInfoAdapterInterface {
public:
    ProtocolSimIccidInfoAdapterInterface() { }
    virtual ~ProtocolSimIccidInfoAdapterInterface() = default;
    virtual int GetIccIdLen() const = 0;
    virtual const BYTE *GetIccId() const = 0;
};

class ProtocolSimGetCarrierRestrictionsAdapterInterface {
public:
    ProtocolSimGetCarrierRestrictionsAdapterInterface() { }
    virtual ~ProtocolSimGetCarrierRestrictionsAdapterInterface() = default;
    virtual int GetAllowedCarrierLength() const = 0;
    virtual int GetExcludedCarrierLength() const = 0;
    virtual int GetAllowedCarriers(RIL_Carrier *pCarriers, int nSize) const = 0;
    virtual int GetExcludedCarriers(RIL_Carrier *pCarriers, int nSize) const = 0;
    virtual bool GetAllowedCarriersPrioritized() const = 0;
    virtual INT32 GetMultiSimPolicy() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolUiccSubStatusChangeAdapterInterface {
public:
    ProtocolUiccSubStatusChangeAdapterInterface() { }
    virtual ~ProtocolUiccSubStatusChangeAdapterInterface() = default;
    virtual int GetState() const = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

class ProtocolSimLockInfoAdapterInterface {
public:
    ProtocolSimLockInfoAdapterInterface() { }
    virtual ~ProtocolSimLockInfoAdapterInterface() = default;
public:
    virtual int GetPolicy() const = 0;
    virtual int GetStatus() const = 0;
    virtual int GetLockType() const = 0;
    virtual int GetMaxRetryCount() const = 0;
    virtual int GetRemainCount() const = 0;
    virtual int GetLockCodeCount() const = 0;
    virtual const char *GetLockCode() const = 0;
    virtual int GetLockCodeSize() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

#ifdef AUTO_VERIFY_PIN_WITH_CP
class ProtocolSimEncryptedPinAdapterInterface {
public:
    ProtocolSimEncryptedPinAdapterInterface() {}
    virtual ~ProtocolSimEncryptedPinAdapterInterface() = default;
    virtual int GetEncryptedPinLen() const = 0;
    virtual int GetPinLen() const = 0;
    virtual const char *GetEncryptedPin() const = 0;
};
#endif

class ProtocolSimTrayStatusChangeAdapterInterface {
public:
    ProtocolSimTrayStatusChangeAdapterInterface() { }
    virtual ~ProtocolSimTrayStatusChangeAdapterInterface() = default;
    virtual int GetState() const = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

#endif /* __PROTOCOL_SIM_ADAPTER_INTERFACE_H__ */
