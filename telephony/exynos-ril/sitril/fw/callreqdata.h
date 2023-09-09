 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _CALL_DIAL_REQ_DATA_H_
#define _CALL_DIAL_REQ_DATA_H_

#include "requestdata.h"

/* User-to-User Signaling Information defined in 3GPP 23.087 v8.0
 * This data is passed in RIL_ExtensionRecord and rec contains this
 * structure when type is RIL_UUS_INFO_EXT_REC */
//typedef struct {
//  RIL_UUS_Type    uusType;    /* UUS Type */
//  RIL_UUS_DCS     uusDcs;     /* UUS Data Coding Scheme */
//  int             uusLength;  /* Length of UUS Data */
//  char *          uusData;    /* UUS Data */
//} RIL_UUS_Info;

class UusInfo
{
public:
    RIL_UUS_Type m_uusType;
    RIL_UUS_DCS m_uusDcs;
    int m_uusLength;
    char m_uusData[MAX_UUS_DATA_LEN];

public:
    UusInfo();
    ~UusInfo() {}
    void Clear();
};

class CallInfo
{
public:
    RIL_CallState GetState() {return m_state;}
    CallInfo();
    void Clear();
public:
    RIL_CallState m_state;
    INT32 m_nIndex;
    INT32 m_toa;
    BOOL m_isMParty;
    BOOL m_isMt;
    BOOL m_isVoice;
    BOOL m_isVideo;
    BOOL m_isVoicePrivacy;
    CallPresentation m_numPresent;
    CallPresentation m_namePresent;
    int8_t m_als;
    char m_number[MAX_DIAL_NUM+1];
    char m_name[MAX_DIAL_NAME+1];
    UusInfo m_uusInfo;
    RIL_AudioQuality m_audioQuality;
    char m_forwardedNumber[MAX_SS_NUM_LEN+1];
};

class CallList
{
public:
    CallList();
    ~CallList();
    void Clear();
    INT32 GetCount() {return m_nCount;}
    CallInfo *GetCallInfo() {return m_szCallInfo;}
public:
    static const int MAX_CALL_LIST_COUNT = 9;
    CallInfo m_szCallInfo[MAX_CALL_LIST_COUNT];
    INT32 m_nCount;
};

typedef enum {CALL_ID_STATE_INACTIVE, CALL_ID_STATE_ACTIVE}CS_CALL_ID_STATE;

typedef struct
{
    int call_id;
    int state;
}call_id_state;

class CallId
{
public:
    CallId();
private:
    static const int MAX_CALL_ID_COUNT = 9;
    call_id_state m_CallId[MAX_CALL_ID_COUNT];
public:
    void Init();
    int GetCpIndex(int ApId);

    void SyncReady();
    int AddCallId(int CpId);
    void SyncDone();
};

class EmergencyNumber {
  private:
    char mMcc[MAX_MCC_LEN + 1];
    char mMnc[MAX_MNC_LEN + 1];
    char mNumber[MAX_EMERGENCY_NUMBER_LEN + 1];
    char mUrn[MAX_URN_COUNT][MAX_URN_LEN + 1];
    char *mUrnP[MAX_URN_COUNT];
    int mNumberLen;
    int mUrnCount;
    int mSource;
    int mCategory;
    int mConditions;
    bool mIsEmergencyType;
    bool mIsEmergencyCallRouting;

  public:
    void Clear();
    void Add(const char *mcc, const char *mnc, const char *number, int numberLen,
             int category, int conditions, int source);
    void Add(const char *mcc, const char *mnc, const char *number, int numberLen,
             const char *urn, int urnLen, int category, int conditions, int source);
    void AddUrn(const char *urn, int urnLen);
    void RemoveUrn(const char *urn);
    char *GetMcc();
    char *GetMnc();
    void SetNumber(const char *number, int len);
    char *GetNumber();
    void SetNumberLen(int numberLen);
    int GetNumberLen();
    void SetSource(int source);
    int GetSource();
    void SetCategory(int category);
    int GetCategory();
    int GetUrnCount();
    char *GetUrn(int idx);
    char **GetUrns();
    int GetConditions();
    void SetEmergencyType(bool isEmergencyType);
    bool IsEmergencyType();
    void SetEmergencyCallRouting(bool isEmergencyCallRouting);
    bool IsEmergencyCallRouting();
};

class EccInfo {
  private:

    /**
     * The number of EmergencyNumber in mEmergencyNumberList.
     */
    int mCount;

    /**
     * EENL validity. Either 0 or 1 is set.
     * Refer to SIT_IND_EMERGENCY_CALL_LIST command.
     * This value is only set into SIT_ECC_INFO_TYPE_EENL array of radio ECC info,
     * when receiving SIT_IND_EMERGENCY_CALL_LIST.
     * It is used to calculate which radio ECC info used.
     * There is usage in GetRadioEccInfoType API of EmergencyNumberUpdater.
     */
    int mEenlv;

    /**
     * The PLMN(MCC + MNC) from radio. This value is set into
     * SIT_ECC_INFO_TYPE_EENL array of radio ECC info,
     * when receiving SIT_IND_EMERGENCY_CALL_LIST.
     * It is used to calculate which radio ECC info used.
     * There is usage in GetRadioEccInfoType API of EmergencyNumberUpdater.
     */
    char mPlmn[MAX_PLMN_LEN];

    /**
     * The list of EmergencyNumber.
     */
    EmergencyNumber mEmergencyNumberList[MAX_EMERGENCY_NUMBER_LIST_COUNT];

  public:
    EccInfo();
    ~EccInfo();
    void SetCount(int count);
    int GetCount();
    void SetEenlv(int eenlv);
    int GetEenlv();
    void SetMcc(char *mcc);
    void SetMnc(char *mnc, int mncLen);
    char *GetPlmn();
    EmergencyNumber *GetEmergencyNumberList();
    EmergencyNumber *GetEmergencyNumber(int idx);

    void Clear();
    void AddEmergencyNumber(const char *mcc, const char *mnc, const char *number, int numberLen,
                            int category, int conditions, int source);
    void AddEmergencyNumber(const char *mcc, const char *mnc, const char *number, int numberLen,
                            const char *urn, int urn_len, int category, int conditions, int source);
};

class DtmfInfo : public RequestData
{
    public:
        char m_szDtmf[MAX_DIAL_NUM+1];

    public:
        DtmfInfo(const int nReq, const Token tok, const ReqType type = REQ_FW);
        DtmfInfo(const int nReq, const Token tok, const char dtmf);
        DtmfInfo(const int nReq, const Token tok, const char *dtmf);
        ~DtmfInfo() {}
};

class CallDialReqData : public RequestData
{
public:
    CallDialReqData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    ~CallDialReqData() {}

public:
    INT32 encode(char *data, unsigned int datalen);
    char * GetNumber()
    {
        return m_number;
    }

    void SetClirType(ClirType type)
    {
        m_clirType = type;
    }

    ClirType GetClirType()
    {
        return m_clirType;
    }
    UusInfo GetUusInfo()
    {
        return m_uusInfo;
    }

    void SetCallType(CallType type)
    {
        m_callType = type;
    }

    CallType GetCallType()
    {
        return m_callType;
    }
    INT32 GetEccCategory()
    {
        return m_eccCategory;
    }

private:
    char m_number[MAX_DIAL_NUM+1];
    ClirType m_clirType;
    UusInfo m_uusInfo;
    CallType m_callType;
    INT32 m_eccCategory;
};

class CallEmergencyDialReqData : public CallDialReqData
{
public:
    CallEmergencyDialReqData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    ~CallEmergencyDialReqData();
public:
    INT32 encode(char *data, unsigned int datalen);

    INT32 GetCategories() { return m_categories; }
    INT32 GetLenUrns() { return m_renUrns; }
    char** GetUrns() { return m_urns; }
    INT32 GetRouting() { return m_routing; }
    BOOL GetHasKnownUserIntentEmergency() { return m_hasKnownUserIntentEmergency; }
    BOOL GetIsTesting() { return m_isTesting; }

private:
    INT32 m_categories;
    INT32 m_renUrns;
    char** m_urns;
    INT32 m_routing;
    BOOL m_hasKnownUserIntentEmergency;
    BOOL m_isTesting;
};

class CallForwardReqData : public RequestData
{
public:
    CallForwardReqData(const int nReq, const Token tok, const ReqType type = REQ_FW);
public:
    INT32 encode(char *data, unsigned int datalen);
    SsModeType GetStatus()
    {
        return m_status;
    }
    SsCfReason GetReason()
    {
        return m_reason;
    }
    SsClassX GetSsClassType()
    {
        return m_classType;
    }
    char* GetNumber()
    {
        return m_number;
    }
    INT32 GetToa()
    {
        return m_toa;
    }
    INT32 GetTimeSeconds()
    {
        return m_timeSeconds;
    }
private:
    SsModeType m_status;
    SsCfReason m_reason;
    SsClassX m_classType;
    INT32 m_toa;
    char m_number[MAX_DIAL_NUM+1];
    INT32 m_timeSeconds;
    INT32 m_serviceClass;
};
#endif /*_CALL_DIAL_REQ_DATA_H_*/
