/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_EMBMS_ADAPTER_LEGACY_H__
#define __PROTOCOL_EMBMS_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolembmsadapterinterface.h"
#include "sitdef.h"

class ProtocolEmbmsCoverageAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmbmsCoverageAdapterInterface {
  public:
    ProtocolEmbmsCoverageAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolEmbmsCoverageAdapterLegacy() = default;

  public:
    int GetCoverage() const;
};

class ProtocolEmbmsSessionListAdapterLegacy : public ProtocolRespAdapter, public ProtocolEmbmsSessionListAdapterInterface {
  public:
    ProtocolEmbmsSessionListAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSessionListAdapterLegacy() = default;

  private:
    int m_state;
    int m_oos_reason;
    int m_record_num;
    BYTE m_tmgi[EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN];

    void Init();

  public:
    int GetState() const {
        return m_state;
    }
    int GetOosReason() const {
        return m_oos_reason;
    }
    int GetRecordNum() const {
        return m_record_num;
    }
    const BYTE *GetTMGI() const;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolEmbmsSessionListIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmbmsSessionListIndAdapterInterface {
  public:
    ProtocolEmbmsSessionListIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSessionListIndAdapterLegacy() = default;

  private:
    int m_state;
    int m_oos_reason;
    int m_record_num;
    BYTE m_tmgi[EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN];

    void Init();

  public:
    int GetState() const {
        return m_state;
    }
    int GetOosReason() const {
        return m_oos_reason;
    }
    int GetRecordNum() const {
        return m_record_num;
    }
    const BYTE *GetTMGI() const;
};

class ProtocolEmbmsNetworkTimeAdapterLegacy : public ProtocolRespAdapter, public ProtocolEmbmsNetworkTimeAdapterInterface {
  public:
    ProtocolEmbmsNetworkTimeAdapterLegacy(const ModemData *pModemData)
        : ProtocolRespAdapter(pModemData) {}
    virtual ~ProtocolEmbmsNetworkTimeAdapterLegacy() = default;

  public:
    uint64_t GetNetworkTime() const;
    // TODO: think about removing this unused API
    // providing empty definition for now to avoid linking error
    int TimeInfoType() const {
        return 0;
    }
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

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolEmbmsNetworkTimeIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmbmsNetworkTimeIndAdapterInterface {
  public:
    ProtocolEmbmsNetworkTimeIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {}
    virtual ~ProtocolEmbmsNetworkTimeIndAdapterLegacy() = default;

  public:
    uint64_t GetNetworkTime() const;
    // TODO: think about removing this unused API
    // providing empty definition for now to avoid linking error
    int TimeInfoType() const {
        return 0;
    }
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

class ProtocolEmbmsSignalStrengthAdapterLegacy : public ProtocolRespAdapter, public ProtocolEmbmsSignalStrengthAdapterInterface {
  public:
    ProtocolEmbmsSignalStrengthAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSignalStrengthAdapterLegacy() = default;

  private:
    int m_number_record;
    UINT32 m_arrSnrList[8];
    void Init();

  public:
    int GetCount() const {
        return m_number_record;
    }
    const UINT32 *GetSnrList() const;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolEmbmsSignalStrengthIndAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmbmsSignalStrengthIndAdapterInterface {
  public:
    ProtocolEmbmsSignalStrengthIndAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSignalStrengthIndAdapterLegacy() = default;

  private:
    int m_number_record;
    UINT32 m_arrSnrList[8];
    void Init();

  public:
    int GetCount() const {
        return m_number_record;
    }
    const UINT32 *GetSnrList() const;
};

class ProtocolEmbmsSaiListAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmbmsSaiListAdapterInterface {
  public:
    ProtocolEmbmsSaiListAdapterLegacy(const ModemData *pModemData)
        : ProtocolIndAdapter(pModemData) {
        Init();
    }
    virtual ~ProtocolEmbmsSaiListAdapterLegacy() = default;

  private:
    EMBMS_SaiList m_saiList;

    virtual void Init();

  public:
    int GetIntraSaiListLen() const {
        return m_saiList.IntraSaiListNum;
    }
    int GetInterSaiListLen() const {
        return m_saiList.InterSaiListNum;
    }
    const UINT16 *GetIntraSaiList() const;
    const EMBMS_InterSaiList *GetInterSaiList() const;
};

class ProtocolEmbmsGlobalCellIdAdapterLegacy : public ProtocolIndAdapter, public ProtocolEmbmsGlobalCellIdAdapterInterface {
  public:
    ProtocolEmbmsGlobalCellIdAdapterLegacy(const ModemData *pModemData);
    virtual ~ProtocolEmbmsGlobalCellIdAdapterLegacy() = default;

  private:
    char m_mcc[MAX_MCC_LEN + 1];
    char m_mnc[MAX_MNC_LEN + 1];
    UINT32 m_cellId;

    void Init();

  public:
    const char *GetMcc() const;
    const char *GetMnc() const;
    UINT32 GetCellId() const;
};

#endif /* __PROTOCOL_EMBMS_ADAPTER_LEGACY_H__ */
