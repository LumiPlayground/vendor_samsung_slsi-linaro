/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_EMBMS_ADAPTER_H__
#define __PROTOCOL_EMBMS_ADAPTER_H__

#include "protocolembmsadapterinterface.h"

class ModemData;

class ProtocolEmbmsCoverageAdapter : public ProtocolEmbmsCoverageAdapterInterface {
private:
    ProtocolEmbmsCoverageAdapterInterface *protocolEmbmsCoverageAdapterInterface = NULL;
public:
    ProtocolEmbmsCoverageAdapter(const ModemData *pModemData);
    ProtocolEmbmsCoverageAdapter(const ProtocolEmbmsCoverageAdapter &) = delete;
    ProtocolEmbmsCoverageAdapter& operator=(ProtocolEmbmsCoverageAdapter const&) = delete;
    virtual ~ProtocolEmbmsCoverageAdapter();
public:
    int GetCoverage() const;
};

class ProtocolEmbmsSessionListAdapter : public ProtocolEmbmsSessionListAdapterInterface {
private:
    ProtocolEmbmsSessionListAdapterInterface *protocolEmbmsSessionListAdapterInterface = NULL;
public:
    ProtocolEmbmsSessionListAdapter(const ModemData *pModemData);
    ProtocolEmbmsSessionListAdapter(const ProtocolEmbmsSessionListAdapter &) = delete;
    ProtocolEmbmsSessionListAdapter& operator=(ProtocolEmbmsSessionListAdapter const&) = delete;
    virtual ~ProtocolEmbmsSessionListAdapter();
public:
    int GetState() const;
    int GetOosReason() const;
    int GetRecordNum() const;
    const BYTE *GetTMGI() const;
    UINT GetErrorCode() const { return protocolEmbmsSessionListAdapterInterface->GetErrorCode(); }
};

class ProtocolEmbmsSessionListIndAdapter : public ProtocolEmbmsSessionListIndAdapterInterface {
private:
    ProtocolEmbmsSessionListIndAdapterInterface *protocolEmbmsSessionListIndAdapterInterface = NULL;
public:
    ProtocolEmbmsSessionListIndAdapter(const ModemData *pModemData);
    ProtocolEmbmsSessionListIndAdapter(const ProtocolEmbmsSessionListIndAdapter &) = delete;
    ProtocolEmbmsSessionListIndAdapter& operator=(ProtocolEmbmsSessionListIndAdapter const&) = delete;
    virtual ~ProtocolEmbmsSessionListIndAdapter();
public:
    int GetState() const;
    int GetOosReason() const;
    int GetRecordNum() const;
    const BYTE *GetTMGI() const;
};

class ProtocolEmbmsNetworkTimeAdapter : public ProtocolEmbmsNetworkTimeAdapterInterface {
private:
    ProtocolEmbmsNetworkTimeAdapterInterface *protocolEmbmsNetworkTimeAdapterInterface = NULL;
public:
    ProtocolEmbmsNetworkTimeAdapter(const ModemData *pModemData);
    ProtocolEmbmsNetworkTimeAdapter(const ProtocolEmbmsNetworkTimeAdapter &) = delete;
    ProtocolEmbmsNetworkTimeAdapter& operator=(ProtocolEmbmsNetworkTimeAdapter const&) = delete;
    virtual ~ProtocolEmbmsNetworkTimeAdapter();
public:
    uint64_t GetNetworkTime() const;
    int TimeInfoType() const;
    int DayLightValid() const;
    int Year() const;
    int Month() const;
    int Day() const;
    int Hour() const;
    int Minute() const;
    int Second() const;
    int TimeZone() const;
    int DayLightAdjust() const;
    int DayofWeek() const;

    UINT GetErrorCode() const { return  protocolEmbmsNetworkTimeAdapterInterface->GetErrorCode(); }
};

class ProtocolEmbmsNetworkTimeIndAdapter : public ProtocolEmbmsNetworkTimeIndAdapterInterface {
private:
    ProtocolEmbmsNetworkTimeIndAdapterInterface *protocolEmbmsNetworkTimeIndAdapterInterface = NULL;
public:
    ProtocolEmbmsNetworkTimeIndAdapter(const ModemData *pModemData);
    ProtocolEmbmsNetworkTimeIndAdapter(const ProtocolEmbmsNetworkTimeIndAdapter &) = delete;
    ProtocolEmbmsNetworkTimeIndAdapter& operator=(ProtocolEmbmsNetworkTimeIndAdapter const&) = delete;
    virtual ~ProtocolEmbmsNetworkTimeIndAdapter();
public:
    uint64_t GetNetworkTime() const;
    int TimeInfoType() const;
    int DayLightValid() const;
    int Year() const;
    int Month() const;
    int Day() const;
    int Hour() const;
    int Minute() const;
    int Second() const;
    int TimeZone() const;
    int DayLightAdjust() const;
    int DayofWeek() const;
};

class ProtocolEmbmsSignalStrengthAdapter : public ProtocolEmbmsSignalStrengthAdapterInterface {
private:
    ProtocolEmbmsSignalStrengthAdapterInterface *protocolEmbmsSignalStrengthAdapterInterface = NULL;
public:
    ProtocolEmbmsSignalStrengthAdapter(const ModemData *pModemData);
    ProtocolEmbmsSignalStrengthAdapter(const ProtocolEmbmsSignalStrengthAdapter &) = delete;
    ProtocolEmbmsSignalStrengthAdapter& operator=(ProtocolEmbmsSignalStrengthAdapter const&) = delete;
    virtual ~ProtocolEmbmsSignalStrengthAdapter();
public:
    int GetCount() const;
    const UINT32 *GetSnrList() const;
    UINT GetErrorCode() const { return protocolEmbmsSignalStrengthAdapterInterface->GetErrorCode();  }
};

class ProtocolEmbmsSignalStrengthIndAdapter : public ProtocolEmbmsSignalStrengthIndAdapterInterface {
private:
    ProtocolEmbmsSignalStrengthIndAdapterInterface *protocolEmbmsSignalStrengthIndAdapterInterface = NULL;
public:
    ProtocolEmbmsSignalStrengthIndAdapter(const ModemData *pModemData);
    ProtocolEmbmsSignalStrengthIndAdapter(const ProtocolEmbmsSignalStrengthIndAdapter &) = delete;
    ProtocolEmbmsSignalStrengthIndAdapter& operator=(ProtocolEmbmsSignalStrengthIndAdapter const&) = delete;
    virtual ~ProtocolEmbmsSignalStrengthIndAdapter();
public:
    int GetCount() const;
    const UINT32 *GetSnrList() const;
};

class ProtocolEmbmsSaiListAdapter : public ProtocolEmbmsSaiListAdapterInterface {
private:
    ProtocolEmbmsSaiListAdapterInterface *protocolEmbmsSaiListAdapterInterface = NULL;
public:
    ProtocolEmbmsSaiListAdapter(const ModemData *pModemData);
    ProtocolEmbmsSaiListAdapter(const ProtocolEmbmsSaiListAdapter &) = delete;
    ProtocolEmbmsSaiListAdapter& operator=(ProtocolEmbmsSaiListAdapter const&) = delete;
    virtual ~ProtocolEmbmsSaiListAdapter();
public:
    int GetIntraSaiListLen() const;
    int GetInterSaiListLen() const;
    const UINT16 *GetIntraSaiList() const;
    const EMBMS_InterSaiList *GetInterSaiList() const;
};

class ProtocolEmbmsGlobalCellIdAdapter : public ProtocolEmbmsGlobalCellIdAdapterInterface {
private:
    ProtocolEmbmsGlobalCellIdAdapterInterface *protocolEmbmsGlobalCellIdAdapterInterface = NULL;
public:
    ProtocolEmbmsGlobalCellIdAdapter(const ModemData *pModemData);
    ProtocolEmbmsGlobalCellIdAdapter(const ProtocolEmbmsGlobalCellIdAdapter &) = delete;
    ProtocolEmbmsGlobalCellIdAdapter& operator=(ProtocolEmbmsGlobalCellIdAdapter const&) = delete;
    virtual ~ProtocolEmbmsGlobalCellIdAdapter();
public:
    const char* GetMcc() const;
    const char* GetMnc() const;
    UINT32 GetCellId() const;
};

#endif /* __PROTOCOL_EMBMS_ADAPTER_H__ */
