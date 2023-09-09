/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_EMBMS_ADAPTER_JSON_H__
#define __PROTOCOL_EMBMS_ADAPTER_JSON H__

#include "protocoljsonadapter.h"
#include "protocolembmsadapterinterface.h"
#include "sitdef.h"

class ProtocolEmbmsCoverageAdapterJson : public ProtocolJsonIndAdapter,
                                         public ProtocolEmbmsCoverageAdapterInterface {
  public:
    ProtocolEmbmsCoverageAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_EMBMS_COVERAGE) {}
    virtual ~ProtocolEmbmsCoverageAdapterJson() = default;

  public:
    int GetCoverage() const;
};

class ProtocolEmbmsSessionListAdapterJson : public ProtocolJsonRespAdapter,
                                            public ProtocolEmbmsSessionListAdapterInterface {
  public:
    ProtocolEmbmsSessionListAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSessionListAdapterJson() = default;

  private:
   unsigned int m_nRecordNum;
    BYTE m_tmgiList[EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN];

    void Init();
    void SetTMGI();

  public:
    int GetState() const;
    int GetOosReason() const;
    int GetRecordNum() const {
        return m_nRecordNum;
    }
    const BYTE *GetTMGI() const;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolEmbmsSessionListIndAdapterJson : public ProtocolJsonIndAdapter,
                                               public ProtocolEmbmsSessionListIndAdapterInterface {
  public:
    ProtocolEmbmsSessionListIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSessionListIndAdapterJson() = default;

  private:
    int m_nRecordNum;
    BYTE m_tmgiList[EMBMS_MAX_INTRA_SAILIST_NUMBER * EMBMS_TMGI_LEN];

    void Init();
    void SetTMGI();

  public:
    int GetState() const;
    int GetOosReason() const;
    int GetRecordNum() const {
        return m_nRecordNum;
    }
    const BYTE *GetTMGI() const;
};

class ProtocolEmbmsNetworkTimeAdapterJson : public ProtocolJsonRespAdapter,
                                            public ProtocolEmbmsNetworkTimeAdapterInterface {
  public:
    ProtocolEmbmsNetworkTimeAdapterJson(const ModemData *pModemData)
        : ProtocolJsonRespAdapter(pModemData, SIT_GET_EMBMS_NETWORK_TIME) {}
    virtual ~ProtocolEmbmsNetworkTimeAdapterJson() = default;

  public:
    uint64_t GetNetworkTime() const;
    // TODO : Unused API, think to remove in future
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

class ProtocolEmbmsNetworkTimeIndAdapterJson : public ProtocolJsonIndAdapter,
                                               public ProtocolEmbmsNetworkTimeIndAdapterInterface {
  public:
    ProtocolEmbmsNetworkTimeIndAdapterJson(const ModemData *pModemData)
        : ProtocolJsonIndAdapter(pModemData, SIT_IND_EMBMS_NETWORK_TIME) {}
    virtual ~ProtocolEmbmsNetworkTimeIndAdapterJson() = default;

  public:
    uint64_t GetNetworkTime() const;
    // TODO : Unused API, think to remove in future
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

class ProtocolEmbmsSignalStrengthAdapterJson : public ProtocolJsonRespAdapter,
                                               public ProtocolEmbmsSignalStrengthAdapterInterface {
  public:
    ProtocolEmbmsSignalStrengthAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSignalStrengthAdapterJson() = default;

  private:
    unsigned int m_nNumberOfRecord;
    UINT32 m_arrSnrList[8];
    void SetSnrList();

  public:
    int GetCount() const {
        return m_nNumberOfRecord;
    };
    const UINT32 *GetSnrList() const;

    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolEmbmsSignalStrengthIndAdapterJson : public ProtocolJsonIndAdapter,
                                                  public ProtocolEmbmsSignalStrengthIndAdapterInterface {
  public:
    ProtocolEmbmsSignalStrengthIndAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSignalStrengthIndAdapterJson() = default;

  private:
    unsigned int m_nNumberOfRecord;
    UINT32 m_arrSnrList[8];
    void SetSnrList();

  public:
    int GetCount() const {
        return m_nNumberOfRecord;
    };
    const UINT32 *GetSnrList() const;
};

class ProtocolEmbmsSaiListAdapterJson : public ProtocolJsonIndAdapter,
                                        public ProtocolEmbmsSaiListAdapterInterface {
  public:
    ProtocolEmbmsSaiListAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmbmsSaiListAdapterJson() = default;

  private:
    EMBMS_SaiList m_saiList;
    void Init();

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

class ProtocolEmbmsGlobalCellIdAdapterJson : public ProtocolJsonIndAdapter,
                                             public ProtocolEmbmsGlobalCellIdAdapterInterface {
  public:
    ProtocolEmbmsGlobalCellIdAdapterJson(const ModemData *pModemData);
    virtual ~ProtocolEmbmsGlobalCellIdAdapterJson() = default;

  private:
    char m_mcc[MAX_MCC_LEN + 1];
    char m_mnc[MAX_MNC_LEN + 1];

    void Init();
    void SetMccMnc();

  public:
    const char *GetMcc() const;
    const char *GetMnc() const;
    UINT32 GetCellId() const;
};

#endif /* __PROTOCOL_EMBMS_ADAPTER_JSON_H__ */
