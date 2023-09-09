 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef _REQUEST_DATA_H_
#define _REQUEST_DATA_H_

#include "rildef.h"

typedef void * Token;

enum ReqType {
    REQ_FW = 0,         //
    REQ_PRIVATE = 1,    //
    REQ_OEM = 2         //
};

/**
 * RequestData
 */
class RequestData
{
public:
    virtual int encode(char *, unsigned int) { return 0; }
    int mHalVer;
public:
    RequestData();
    RequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    virtual ~RequestData() {}
    int GetReqId() const { return m_nReq; }
    ReqType GetReqType() { return m_reqType; }
    Token GetToken() { return m_tok; }
    virtual RequestData *Clone() const;
    int GetHalVersion() const { return mHalVer; }

protected:
    int m_nReq;
    Token m_tok;
    ReqType m_reqType;
};


/**
 * IntRequestData
 */
class IntRequestData : public RequestData
{
public:
    virtual int encode(char *data, unsigned int datalen);
    int GetInt() { return m_nInt; }

public:
    IntRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    virtual ~IntRequestData();
    virtual IntRequestData *Clone() const;

protected:
    int m_nInt;
};

/**
 * IntsRequestData
 */
class IntsRequestData : public RequestData
{
public:
    virtual int encode(char *data, unsigned int datalen);
    int GetInt(int index) const;
    int GetSize() const { return m_nSize; }

public:
    IntsRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    IntsRequestData(const IntsRequestData&) = delete;
    IntsRequestData& operator=(IntsRequestData const&) = delete;
    virtual ~IntsRequestData();
    virtual IntsRequestData *Clone() const;

protected:
    int *m_pInts;
    int m_nSize;
};

/**
 * StringRequestData
 */
class StringRequestData : public RequestData
{
public:
    virtual int encode(char *data, unsigned int datalen);
    const char *GetString() const { return m_szData; }
public:
    StringRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    StringRequestData(const StringRequestData&) = delete;
    StringRequestData& operator=(StringRequestData const&) = delete;
    virtual ~StringRequestData();
    virtual StringRequestData *Clone() const;

protected:
    char *m_szData;
};

/**
 * StringsRequestData
 */
class StringsRequestData : public RequestData
{
public:
    virtual int encode(char *data, unsigned int datalen);
    int GetStringCount() {return m_nStringCount;}
    char **GetStringsContent(){return m_ptrStrings;}
    const char *GetString(int index) const;

public:
    StringsRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    StringsRequestData(const StringsRequestData&) = delete;
    StringsRequestData& operator=(StringsRequestData const&) = delete;
    virtual ~StringsRequestData();
    virtual StringsRequestData *Clone() const;

protected:
    int m_nStringCount;
    char **m_ptrStrings;
};

/**
 * RawRequestData
 */
class RawRequestData : public RequestData
{
public:
    virtual int encode(char *data, unsigned int datalen);
    void *GetRawData() { return m_pRawData; }
    int GetSize() const { return m_nSize; }

public:
    RawRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    RawRequestData(const RawRequestData&) = delete;
    RawRequestData& operator=(RawRequestData const&) = delete;
    virtual ~RawRequestData();
    virtual RawRequestData *Clone() const;

protected:
    char *m_pRawData;
    int m_nSize;
};

/**
 * OemStringsRequestData
 */
class OemStringsRequestData : public RequestData {
  public:
    virtual int encode(char *data, unsigned int datalen);
    const char *GetString(int index) const;

  public:
    OemStringsRequestData(const int nReq, const Token tok, const ReqType type = REQ_FW);
    OemStringsRequestData(const OemStringsRequestData&) = delete;
    OemStringsRequestData& operator=(OemStringsRequestData const&) = delete;
    virtual ~OemStringsRequestData();
    virtual OemStringsRequestData *Clone() const;

  protected:
    int m_nStringCount;
    char **m_ptrStrings;
};

#endif /*_REQUEST_DATA_H_*/
