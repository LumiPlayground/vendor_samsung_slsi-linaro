/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_EMBMS_ADAPTER_INTERFACE_H__
#define __PROTOCOL_EMBMS_ADAPTER_INTERFACE_H__

#include <base/rildef.h>

class ProtocolEmbmsCoverageAdapterInterface {
public:
    ProtocolEmbmsCoverageAdapterInterface() { }
    virtual ~ProtocolEmbmsCoverageAdapterInterface() { }
public:
    virtual int GetCoverage() const = 0;
};

class ProtocolEmbmsSessionListAdapterInterface {
public:
    ProtocolEmbmsSessionListAdapterInterface() { }
    virtual ~ProtocolEmbmsSessionListAdapterInterface() { }
public:
    virtual int GetState() const = 0;
    virtual int GetOosReason() const = 0;
    virtual int GetRecordNum() const = 0;
    virtual  const BYTE *GetTMGI() const = 0;

    virtual UINT GetErrorCode() const = 0;
};

class ProtocolEmbmsSessionListIndAdapterInterface {
public:
    ProtocolEmbmsSessionListIndAdapterInterface() { }
    virtual ~ProtocolEmbmsSessionListIndAdapterInterface() { }
public:
    virtual int GetState() const = 0;
    virtual int GetOosReason() const = 0;
    virtual int GetRecordNum() const = 0;
    virtual const BYTE *GetTMGI() const = 0;
};

class ProtocolEmbmsNetworkTimeAdapterInterface {
public:
    ProtocolEmbmsNetworkTimeAdapterInterface() { }
    virtual ~ProtocolEmbmsNetworkTimeAdapterInterface() { }
public:
    virtual uint64_t GetNetworkTime() const = 0;
    virtual int TimeInfoType() const = 0;
    virtual int DayLightValid() const = 0;
    virtual int Year() const = 0;
    virtual int Month() const = 0;
    virtual int Day() const = 0;
    virtual int Hour() const = 0;
    virtual int Minute() const = 0;
    virtual int Second() const = 0;
    virtual int TimeZone() const = 0;
    virtual int DayLightAdjust() const = 0;
    virtual int DayofWeek() const = 0;

    virtual UINT GetErrorCode() const = 0;
};

class ProtocolEmbmsNetworkTimeIndAdapterInterface {
public:
    ProtocolEmbmsNetworkTimeIndAdapterInterface() { }
    virtual ~ProtocolEmbmsNetworkTimeIndAdapterInterface() { }
public:
    virtual uint64_t GetNetworkTime() const = 0;
    virtual int TimeInfoType() const = 0;
    virtual int DayLightValid() const = 0;
    virtual int Year() const = 0;
    virtual int Month() const = 0;
    virtual int Day() const = 0;
    virtual int Hour() const = 0;
    virtual int Minute() const = 0;
    virtual int Second() const = 0;
    virtual int TimeZone() const = 0;
    virtual int DayLightAdjust() const = 0;
    virtual int DayofWeek() const = 0;
};

class ProtocolEmbmsSignalStrengthAdapterInterface {
public:
    ProtocolEmbmsSignalStrengthAdapterInterface() { }
    virtual ~ProtocolEmbmsSignalStrengthAdapterInterface() { }
public:
    virtual int GetCount() const = 0;
    virtual const UINT32 *GetSnrList() const = 0;

    virtual UINT GetErrorCode() const = 0;
};

class ProtocolEmbmsSignalStrengthIndAdapterInterface {
public:
    ProtocolEmbmsSignalStrengthIndAdapterInterface() { }
    virtual ~ProtocolEmbmsSignalStrengthIndAdapterInterface() { }
public:
    virtual int GetCount() const = 0;
    virtual const UINT32 *GetSnrList() const = 0;
};

class ProtocolEmbmsSaiListAdapterInterface {
public:
    ProtocolEmbmsSaiListAdapterInterface()  { }
    virtual ~ProtocolEmbmsSaiListAdapterInterface() { }
public:
    virtual int GetIntraSaiListLen() const = 0;
    virtual int GetInterSaiListLen() const = 0;
    virtual const UINT16 *GetIntraSaiList() const = 0;
    virtual const EMBMS_InterSaiList *GetInterSaiList() const = 0;
};

class ProtocolEmbmsGlobalCellIdAdapterInterface {
public:
    ProtocolEmbmsGlobalCellIdAdapterInterface() { }
    virtual ~ProtocolEmbmsGlobalCellIdAdapterInterface() { }
public:
    virtual const char* GetMcc() const = 0;
    virtual const char* GetMnc() const = 0;
    virtual UINT32 GetCellId() const = 0;
};

#endif /* __PROTOCOL_EMBMS_ADAPTER_INTERFACE_H__ */
