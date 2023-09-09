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
 * operatortable.h
 *
 *  Created on: 2014. 7. 5.
 *      Author: sungwoo48.choi
 */
#pragma once

#include "operatorinfo.h"

class OperatorContentValue
{
public:
    int m_mcc;
    int m_mnc;
    std::string m_shortPlmn;
    std::string m_longPlmn;
    std::string m_gid1;

public:
    OperatorContentValue();
    OperatorContentValue(int mcc, int mnc, const std::string &longPlmn, const std::string &shortPlmn, const std::string &gid1);
    virtual ~OperatorContentValue();

public:
    bool AreBothLongPlmnShortPlmnAvailable() const;
    int GetMcc() const {
        return m_mcc;
    }
    int GetMnc() const {
        return m_mnc;
    }
    const std::string GetShortPlmn() const {
        return m_shortPlmn;
    }
    const std::string GetLongPlmn() const {
        return m_longPlmn;
    }
    const std::string GetGid1() const {
        return m_gid1;
    }

    void SetShortPlmn(const std::string &shortPlmn);
    void SetLongPlmn(const std::string &longPlmn);
    void SetGid1(const std::string &gid1);
    void Update(const NetworkOperator &networkOperator);

public:
    // operator override
    virtual bool operator==(const OperatorContentValue &rhs);
    virtual bool operator!=(const OperatorContentValue &rhs);
    virtual OperatorContentValue &operator=(const OperatorContentValue &rhs);

    static OperatorContentValue *NewInstance(int mcc, int mnc, const char *longPlmn, const char *shortPlmn, const char *gid1);
};

class EonsContentsValue : public OperatorContentValue
{
public:
    std::string iccId;

public:
    EonsContentsValue();
    EonsContentsValue(const std::string &iccId, const NetworkOperator &networkOperator);
    virtual ~EonsContentsValue();

public:
    void SetIccId(const std::string &iccId);
    const std::string GetIccId() const;

public:
    static EonsContentsValue *NewInstance(const std::string &iccId, const NetworkOperator &networkOperator);
};

class OperatorNameProvider {
public:
    OperatorNameProvider() {}
    virtual ~OperatorNameProvider() {}

    bool isValidPlmn(const std::string &plmn);
    bool isValidMccMnc(int mcc, int mnc);

public:
    virtual bool Contains(int mcc, int mnc, const std::string &gid1) = 0;
    virtual OperatorContentValue *Find(int mcc, int mnc, const std::string &gid1) = 0;
    virtual bool Update(int mcc, int mnc, const std::string &longPlmn, const std::string &shortPlmn, const std::string &gid1) = 0;
    virtual bool Update(const OperatorContentValue *contentValue) = 0;
    virtual bool Update(const NetworkOperator &networkOperator) = 0;
    virtual bool Insert(int mcc, int mnc, const std::string &longPlmn, const std::string &shortPlmn, const std::string &gid1) = 0;
    virtual bool Insert(const OperatorContentValue *contentValue) = 0;
    virtual bool Insert(const NetworkOperator &networkOperator) = 0;

    virtual bool InsertEons(const std::string &iccId, const NetworkOperator &networkOperator) = 0;
    virtual bool UpdateEons(const std::string &iccId, const NetworkOperator &networkOperator) = 0;
    virtual EonsContentsValue *FindEons(const std::string &iccId, const std::string &netPlmn) = 0;

public:
    static OperatorNameProvider *MakeInstance();
    static OperatorNameProvider *GetInstance();
    static void ReleaseInstance();

    static OperatorContentValue GetVendorCustomOperatorName(const std::string &simOperatorNumeric, const std::string &operatorNumeric);
};
