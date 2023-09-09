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
 * operatortable.cpp
 *
 *  Created on: 2014. 7. 5.
 *      Author: sungwoo48.choi
 */

#include "operatortable.h"
#include <sqlite3.h>
#include <list>
#include <map>
#include "rillog.h"
#include "networkutils.h"
#include <librilutils/textutils.h>
#include "ts25table.h"
#include <unistd.h>
#include "constdef.h"

using std::list;
using std::map;
using std::string;

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_OPERTABLE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_OPERTABLE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_OPERTABLE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_OPERTABLE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define DB_NAME                  "/data/vendor/rild/.operatortable.db"
#define TABLE_NAME_SETTINGS      "settings"
#define COL_VERSION              "version"
#define COL_STATUS               "status"
#define COL_INDEX_VERSION        (0)
#define COL_INDEX_STATUS         (1)
#define STATUS_DONE              "done"

#define TABLE_NAME_OPERATOR      "operator"
#define COL_PLMN_MCC             "plmn_mcc"
#define COL_PLMN_MNC             "plmn_mnc"
#define COL_GID1                 "gid1"
#define COL_LONG_PLMN            "long_plmn"
#define COL_SHORT_PLMN           "short_plmn"
#define COL_INDEX_PLMN_MCC       (0)
#define COL_INDEX_PLMN_MNC       (1)
#define COL_INDEX_GID1           (2)
#define COL_INDEX_LONG_PLMN      (3)
#define COL_INDEX_SHORT_PLMN     (4)
#define SQL_BUF_SIZE             512

// database version
#define OPERATOR_NAME_DB_VERSION  (6)

// MCC, MNC, GID1, Long PLMN, Short PLMN
static const char *GLOBAL_OPERATOR_INFO[][5] = {
#include "global_operator_info.txt"
    { 0, 0, 0, 0, 0 }
};

static bool debug = false;

OperatorContentValue::OperatorContentValue()
    : m_mcc(INT_MAX), m_mnc(INT_MAX), m_shortPlmn(""), m_longPlmn(""), m_gid1(GID_DEFAULT_VALUE_ANY) {
}

OperatorContentValue::~OperatorContentValue() {
}

OperatorContentValue::OperatorContentValue(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const std::string &gid1) {
    m_mcc = mcc;
    m_mnc = mnc;
    m_shortPlmn = shortPlmn;
    m_longPlmn = longPlmn;
    m_gid1 = gid1;
}

OperatorContentValue *OperatorContentValue::NewInstance(int mcc, int mnc, const char *longPlmn, const char *shortPlmn, const char *gid1) {
    if (mcc == INT_MAX && mnc == INT_MAX) {
        return nullptr;
    }
    return new OperatorContentValue(mcc, mnc, (longPlmn == nullptr ? "" : longPlmn),
            (shortPlmn == nullptr ? "" : shortPlmn), (gid1 == nullptr ? GID_DEFAULT_VALUE_ANY : gid1));
}

bool OperatorContentValue::AreBothLongPlmnShortPlmnAvailable() const {
    return !TextUtils::IsEmpty(m_shortPlmn) && !TextUtils::IsEmpty(m_longPlmn);
}

void OperatorContentValue::SetShortPlmn(const string &shortPlmn) {
    m_shortPlmn = shortPlmn;
}

void OperatorContentValue::SetLongPlmn(const string &longPlmn) {
    m_longPlmn = longPlmn;
}

void OperatorContentValue::SetGid1(const string &gid1) {
    m_gid1 = gid1;
}

void OperatorContentValue::Update(const NetworkOperator &networkOperator) {
    m_mcc = networkOperator.GetMcc();
    m_mnc = networkOperator.GetMnc();
    SetShortPlmn(networkOperator.GetAlphaShort());
    SetLongPlmn(networkOperator.GetAlphaLong());
    SetGid1(networkOperator.GetGid1());
}

bool OperatorContentValue::operator==(const OperatorContentValue &rhs) {
    return (m_mcc == rhs.m_mcc &&
            m_mnc == rhs.m_mnc &&
            m_gid1 == rhs.m_gid1 &&
            m_shortPlmn == rhs.m_shortPlmn &&
            m_longPlmn == rhs.m_longPlmn);
}

bool OperatorContentValue::operator!=(const OperatorContentValue &rhs) {
    return (m_mcc != rhs.m_mcc ||
            m_mnc != rhs.m_mnc ||
            m_gid1 != rhs.m_gid1 ||
            m_shortPlmn != rhs.m_shortPlmn ||
            m_longPlmn != rhs.m_longPlmn);
}

OperatorContentValue &OperatorContentValue::operator=(const OperatorContentValue &rhs) {
    if (this == &rhs)
        return *this;
    this->m_mcc = rhs.m_mcc;
    this->m_mnc = rhs.m_mnc;
    this->m_gid1 = rhs.m_gid1;
    this->m_shortPlmn = rhs.m_shortPlmn;
    this->m_longPlmn = rhs.m_longPlmn;
    return *this;
}

EonsContentsValue::EonsContentsValue()
    : OperatorContentValue(), iccId("") {
}

EonsContentsValue::~EonsContentsValue() {
}

EonsContentsValue::EonsContentsValue(const string &_iccId, const NetworkOperator &networkOperator) {
    Update(networkOperator);
    SetIccId(_iccId);
}

void EonsContentsValue::SetIccId(const string &_iccId) {
    iccId = _iccId;
}

const string EonsContentsValue::GetIccId() const {
    return iccId;
}

EonsContentsValue *EonsContentsValue::NewInstance(const string &iccId, const NetworkOperator &networkOperator) {
    return new EonsContentsValue(iccId, networkOperator);
}

/////////////////////////////////////////////////////////////////////
// OperatorNameTableDatabaseHelper
/////////////////////////////////////////////////////////////////////
static pthread_mutex_t s_opDbMutex = PTHREAD_MUTEX_INITIALIZER;

class OperatorNameTableDatabaseHelper : public OperatorNameProvider {
    DECLARE_MODULE_TAG()
public:
    OperatorNameTableDatabaseHelper() {
        m_database = nullptr;
        if (sqlite3_open(DB_NAME, &m_database) != SQLITE_OK) {
            RilLogE("%s : Failed to open database", __FUNCTION__);
            m_database = nullptr;
        }
    }

    virtual ~OperatorNameTableDatabaseHelper() {
        if (m_database != nullptr) {
            sqlite3_close(m_database);
            m_database = nullptr;
        }
    }

private:
    sqlite3 *m_database;

public:
    bool Contains(int mcc, int mnc, const string &gid1);
    OperatorContentValue *Find(int mcc, int mnc, const string &gid1);
    bool Update(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid);
    bool Update(const OperatorContentValue *contentValue);
    bool Update(const NetworkOperator &networkOperator);
    bool Insert(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1);

    bool Insert(const OperatorContentValue *contentValue);
    bool Insert(const NetworkOperator &networkOperator);

    //EONS table access
    bool InsertEons(const string &iccId, const NetworkOperator &networkOperator);
    bool UpdateEons(const string &iccId, const NetworkOperator &networkOperator);
    EonsContentsValue *FindEons(const string &iccId, const string &netPlmn);

public:
    static int MakeDB();
    static void DropDB();

private:
    static bool TestDatabaseVersion();
    static bool TestOperatorTable();
    static bool CreateTableSettings();
    static bool CreateTableOperator();
};

IMPLEMENT_MODULE_TAG(OperatorNameTableDatabaseHelper, OperatorNameTableDatabaseHelper)

bool OperatorNameTableDatabaseHelper::TestDatabaseVersion()
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = nullptr;
    sqlite3_stmt *statement = nullptr;
    bool testResult = false;
    int err = SQLITE_OK;

    pthread_mutex_lock(&s_opDbMutex);
    if ((err = sqlite3_open_v2(DB_NAME, &database, SQLITE_OPEN_READONLY, nullptr)) == SQLITE_OK) {
        // check database version
        snprintf(sql, sizeof(sql), "SELECT * FROM %s", TABLE_NAME_SETTINGS);
        if ((err = sqlite3_prepare(database, sql, -1, &statement, nullptr)) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                int version = sqlite3_column_int(statement, COL_INDEX_VERSION);
                char *status = (char *)sqlite3_column_text(statement, COL_INDEX_STATUS);
                RilLogV("%s %s current version=%d, new version=%d, status=%s", TAG, __FUNCTION__, version, OPERATOR_NAME_DB_VERSION, status);
                if (version >= OPERATOR_NAME_DB_VERSION && TextUtils::Equals(status, STATUS_DONE)) {
                    testResult = true;
                }
            }
            sqlite3_finalize(statement);
        } else {
            RilLogE("%s %s : sqlite3_prepare error : error code(%d)", TAG, __FUNCTION__, err);
        }
    }
    sqlite3_close(database);
    pthread_mutex_unlock(&s_opDbMutex);

    return testResult;
}

bool OperatorNameTableDatabaseHelper::TestOperatorTable()
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = nullptr;
    sqlite3_stmt *statement = nullptr;
    bool testResult = false;
    int err = SQLITE_OK;

    pthread_mutex_lock(&s_opDbMutex);
    if ((err = sqlite3_open_v2(DB_NAME, &database, SQLITE_OPEN_READONLY, nullptr)) == SQLITE_OK) {
        // check database version
        snprintf(sql, sizeof(sql), "SELECT count(*) FROM %s", TABLE_NAME_OPERATOR);
        if ((err = sqlite3_prepare(database, sql, -1, &statement, nullptr)) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                int counts = sqlite3_column_int(statement, 0);
                RilLogV("%s %s Total records is(are) = %d", TAG, __FUNCTION__, counts);
                if (counts > 0) {
                    testResult = true;
                }
            }
            else {
                RilLogW("%s %s : No record(s) in database", TAG, __FUNCTION__);
            }
            sqlite3_finalize(statement);
        } else {
            RilLogE("%s %s : sqlite3_prepare error : error code(%d)", TAG, __FUNCTION__, err);
        }
    }
    sqlite3_close(database);
    pthread_mutex_unlock(&s_opDbMutex);

    return testResult;
}

bool OperatorNameTableDatabaseHelper::CreateTableSettings()
{
    bool ret = false;
    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = nullptr;
    char *errMsg = nullptr;

    if (sqlite3_open(DB_NAME, &database) != SQLITE_OK) {
        RilLogE("%s %s Failed to create DB file %s", TAG, __FUNCTION__, DB_NAME);
        return false;
    }

    // create settings table
    snprintf(sql, sizeof(sql)-1, "CREATE TABLE %s (" \
                    "%s INTEGER PRIMARY KEY, " \
                    "%s TEXT);",
            TABLE_NAME_SETTINGS, COL_VERSION, COL_STATUS);
    if (sqlite3_exec(database, sql, nullptr, nullptr, &errMsg) == SQLITE_OK) {
        snprintf(sql, sizeof(sql)-1, "INSERT INTO %s (%s, %s) VALUES(\"%d\", \"%s\")",
                TABLE_NAME_SETTINGS, COL_VERSION, COL_STATUS, OPERATOR_NAME_DB_VERSION, STATUS_DONE);
        if (sqlite3_exec(database, sql, nullptr, nullptr, &errMsg) == SQLITE_OK) {
            if (debug)
                RilLogV("%s %s set database version=%d", TAG, __FUNCTION__, OPERATOR_NAME_DB_VERSION);
            ret = true;
        } else {
            RilLogW("%s %s Failed to insert content : error(%s)", TAG, __FUNCTION__, errMsg);
            sqlite3_free(errMsg);
        }
    } else {
        RilLogE("%s %s Failed to create %s table : error(%s)", TAG, __FUNCTION__, TABLE_NAME_SETTINGS, errMsg);
        sqlite3_free(errMsg);
    }
    sqlite3_close(database);

    return ret;
}

bool OperatorNameTableDatabaseHelper::CreateTableOperator()
{
    bool ret = false;
    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = nullptr;
    char *errMsg = nullptr;

    if (sqlite3_open(DB_NAME, &database) != SQLITE_OK) {
        RilLogE("%s %s Failed to create DB file %s", TAG, __FUNCTION__, DB_NAME);
        return false;
    }

    // create operator table
    snprintf(sql, sizeof(sql)-1, "CREATE TABLE %s (" \
                    "%s INTEGER, " \
                    "%s INTEGER, " \
                    "%s TEXT, " \
                    "%s TEXT, " \
                    "%s TEXT);",
            TABLE_NAME_OPERATOR, COL_PLMN_MCC, COL_PLMN_MNC, COL_GID1, COL_LONG_PLMN, COL_SHORT_PLMN);

    if (sqlite3_exec(database, sql, nullptr, nullptr, &errMsg) == SQLITE_OK) {
        int records = 0;
        // insert new records
        int size = sizeof(GLOBAL_OPERATOR_INFO) / sizeof(GLOBAL_OPERATOR_INFO[0]);
        for (int i = 0; i < size; i++) {
            const char **entry = GLOBAL_OPERATOR_INFO[i];
            if (entry[0] == nullptr)
                break;

            snprintf(sql, sizeof(sql) - 1, "INSERT INTO %s (%s, %s, %s, %s, %s) VALUES(%ld, %ld, UPPER(\"%s\"), \"%s\", \"%s\")",
                     TABLE_NAME_OPERATOR, COL_PLMN_MCC, COL_PLMN_MNC, COL_GID1, COL_LONG_PLMN, COL_SHORT_PLMN,
                     strtol(entry[0], nullptr, 10), strtol(entry[1], nullptr, 10), entry[2], entry[3], entry[4]);
            if (sqlite3_exec(database, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
                RilLogW("%s %s Failed to insert content : error(%s)", TAG, __FUNCTION__, errMsg);
                RilLogV("%s %s MCC=%d, MNC=%d", TAG, __FUNCTION__, strtol(entry[0], nullptr, 10), strtol(entry[1], nullptr, 10));
                sqlite3_free(errMsg);
                continue;
            }
            records++;
            if (debug) {
                RilLogV("%s %s INSERT VALUES(%d, %d, \"%s\", \"%s\", \"%s\")", TAG, __FUNCTION__,
                        strtol(entry[0], nullptr, 10), strtol(entry[1], nullptr, 10), entry[2], entry[3], entry[4]);
            }
            ret = true;
        }
        RilLogV("%s %s total %d record(s) is(are) added.", TAG, __FUNCTION__, records);
    } else {
        RilLogE("%s %s Failed to create %s table : error(%s)", TAG, __FUNCTION__, TABLE_NAME_OPERATOR, errMsg);
        sqlite3_free(errMsg);
    }
    sqlite3_close(database);

    return ret;
}

int OperatorNameTableDatabaseHelper::MakeDB()
{
    //  make database file
    // 1. check whether database is already existed or not
    // 2-1. if no file, create new database and insert operator information
    // 2-2. if already existed, check open state and counts of records.
    RilLogI("%s %s", TAG, __FUNCTION__);

    if (!TestDatabaseVersion() || !TestOperatorTable()) {
        RilLogV("%s %s Create new database", TAG, __FUNCTION__);

        // drop first
        DropDB();

        if (!CreateTableOperator()) {
            DropDB();
            RilLogW("%s %s Failed to create database operator table : Drop database.", TAG, __FUNCTION__);
            return -1;
        }
        if (!CreateTableSettings()) {
            DropDB();
            RilLogW("%s %s Failed to create database settings table : Drop database.", TAG, __FUNCTION__);
            return -1;
        }
    }
    else {
        RilLogW("%s %s Don't need to create database and tables", TAG, __FUNCTION__);
    }

    return 0;
}

void OperatorNameTableDatabaseHelper::DropDB()
{
    int ret = unlink(DB_NAME);
    RilLogV("%s %s Drop database(%d)", TAG, __FUNCTION__, ret);
}

bool OperatorNameTableDatabaseHelper::Contains(int mcc, int mnc, const string &gid1)
{
    OperatorContentValue *contentValue = Find(mcc ,mnc, gid1);
    if (contentValue != nullptr) {
        delete contentValue;
        return true;
    }

    return false;
}

OperatorContentValue *OperatorNameTableDatabaseHelper::Find(int mcc, int mnc, const string &gid1)
{
    if (m_database == nullptr) {
        RilLogW("%s : Invalid database handle", __FUNCTION__);
        return nullptr;
    }

    if (isValidMccMnc(mcc, mnc) == false) {
        return nullptr;
    }

    sqlite3_stmt *statement = nullptr;
    char sql[SQL_BUF_SIZE] = {
        0,
    };
    OperatorContentValue *result = nullptr;

    snprintf(sql, sizeof(sql), "SELECT * FROM %s WHERE %s=%d and %s=%d and %s=UPPER(\"%s\")",
            TABLE_NAME_OPERATOR, COL_PLMN_MCC, mcc, COL_PLMN_MNC, mnc, COL_GID1, gid1.c_str());

    int err = SQLITE_OK;
    pthread_mutex_lock(&s_opDbMutex);
    if ((err = sqlite3_prepare(m_database, sql, -1, &statement, nullptr)) == SQLITE_OK) {
        // select the first record only
        if (sqlite3_step(statement) == SQLITE_ROW) {
            char *longPlmn = (char *)sqlite3_column_text(statement, COL_INDEX_LONG_PLMN);
            char *shortPlmn = (char *)sqlite3_column_text(statement, COL_INDEX_SHORT_PLMN);
            result = OperatorContentValue::NewInstance(mcc, mnc, longPlmn, shortPlmn, gid1.c_str());
            if (debug)
                RilLogV("SELECT VALUES(%d, %d, \"%s\", \"%s\", \"%s\")", mcc, mnc, gid1.c_str(), longPlmn, shortPlmn);
        }
        else {
            if (debug) RilLogV("%s : No record for carrier %d/%d/%s", __FUNCTION__, mcc, mnc, gid1.c_str());
        }
    } else {
        RilLogE("%s : sqlite3_prepare error : error code(%d)", __FUNCTION__, err);
        RilLogE("%s : sql=%s", __FUNCTION__, sql);

        // TODO may re-create DB file.
    }
    sqlite3_finalize(statement);
    pthread_mutex_unlock(&s_opDbMutex);

    return result;
}

bool OperatorNameTableDatabaseHelper::Insert(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1) {
    if (isValidMccMnc(mcc, mnc) == false ||
        TextUtils::IsEmpty(longPlmn) || TextUtils::IsEmpty(shortPlmn) ||
        m_database == nullptr) {
        return false;
    }

    char sql[SQL_BUF_SIZE + 1] = {};

    snprintf(sql, SQL_BUF_SIZE, "INSERT INTO %s (%s, %s, %s, %s, %s) VALUES(%d, %d, UPPER(\"%s\"), \"%s\", \"%s\")",
             TABLE_NAME_OPERATOR,
             COL_PLMN_MCC, COL_PLMN_MNC, COL_GID1, COL_LONG_PLMN, COL_SHORT_PLMN,
             mcc, mnc, gid1.c_str(), longPlmn.c_str(), shortPlmn.c_str());

    char *errMsg = nullptr;
    if (sqlite3_exec(m_database, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        RilLogE("%s : sqlite3_exec error : %s", __FUNCTION__, errMsg);
        RilLogE("%s : sql=%s", __FUNCTION__, sql);

        sqlite3_free(errMsg);
        return false;
    }

    if (debug)
        RilLogV("INSERT VALUES(%d, %d, \"%s\", \"%s\", \"%s\")", mcc, mnc, gid1.c_str(), longPlmn.c_str(), shortPlmn.c_str());

    return true;
}

bool OperatorNameTableDatabaseHelper::Insert(const NetworkOperator &networkOperator) {
    return Insert(networkOperator.GetMcc(), networkOperator.GetMnc(),
                  networkOperator.GetAlphaLong(), networkOperator.GetAlphaShort(), networkOperator.GetGid1());
}

bool OperatorNameTableDatabaseHelper::Insert(const OperatorContentValue *contentValue) {
    if (contentValue != nullptr) {
        return Insert(contentValue->GetMcc(), contentValue->GetMnc(), contentValue->GetLongPlmn(), contentValue->GetShortPlmn(), contentValue->GetGid1());
    }
    return false;
}

bool OperatorNameTableDatabaseHelper::Update(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1) {
    if (isValidMccMnc(mcc, mnc) == false ||
        TextUtils::IsEmpty(longPlmn) || TextUtils::IsEmpty(shortPlmn) ||
        m_database == nullptr) {
        return false;
    }

    char sql[SQL_BUF_SIZE] = { 0, };

    snprintf(sql, SQL_BUF_SIZE, "UPDATE %s SET %s=\"%s\", %s=\"%s\" WHERE %s=%d and %s=%d and %s=UPPER(\"%s\")",
            TABLE_NAME_OPERATOR, COL_LONG_PLMN, longPlmn.c_str(), COL_SHORT_PLMN, shortPlmn.c_str(),
            COL_PLMN_MCC, mcc, COL_PLMN_MNC, mnc, COL_GID1, gid1.c_str());

    char *errMsg = nullptr;
    if (sqlite3_exec(m_database, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        RilLogE("%s : sqlite3_exec error : %s", __FUNCTION__, errMsg);
        RilLogE("%s : sql=%s", __FUNCTION__, sql);

        sqlite3_free(errMsg);
        return false;
    }

    if (debug)
        RilLogV("UPDATE VALUES(%d, %d, \"%s\", \"%s\", \"%s\")", mcc, mnc, gid1.c_str(), longPlmn.c_str(), shortPlmn.c_str());

    return true;
}

bool OperatorNameTableDatabaseHelper::Update(const OperatorContentValue *contentValue) {
    if (contentValue != nullptr) {
        return Update(contentValue->GetMcc(), contentValue->GetMnc(),
                contentValue->GetLongPlmn(), contentValue->GetShortPlmn(), contentValue->GetGid1());
    }
    return false;
}

bool OperatorNameTableDatabaseHelper::Update(const NetworkOperator &networkOperator) {
    return Update(networkOperator.GetMcc(), networkOperator.GetMnc(),
                  networkOperator.GetAlphaLong(), networkOperator.GetAlphaShort(), networkOperator.GetGid1());
}

bool OperatorNameTableDatabaseHelper::InsertEons(const string & /* iccId */, const NetworkOperator & /* networkOperator */) {
    return true;
}

bool OperatorNameTableDatabaseHelper::UpdateEons(const string & /* iccId */, const NetworkOperator & /* networkOperator */) {
    return true;
}

EonsContentsValue *OperatorNameTableDatabaseHelper::FindEons(const string & /* iccId */, const string & /* netPlmn */) {
    return nullptr;
}

/////////////////////////////////////////////////////////////////////
// OperatorNameTable
/////////////////////////////////////////////////////////////////////
class OperatorNameTable : public OperatorNameProvider {
    DECLARE_MODULE_TAG()
private:
    list<OperatorContentValue *> m_cache;
    list<EonsContentsValue *> m_cacheEons;

public:
    OperatorNameTable();
    virtual ~OperatorNameTable();

public:
    bool Contains(int mcc, int mnc, const string &gid1);
    OperatorContentValue *Find(int mcc, int mnc, const string &gid1);
    bool Update(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1);
    bool Update(const OperatorContentValue *contentValue);
    bool Update(const NetworkOperator &networkOperator);
    bool Insert(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1);

    bool Insert(const OperatorContentValue *contentValue);
    bool Insert(const NetworkOperator &networkOperator);

    // EONS table access
    bool InsertEons(const string &iccId, const NetworkOperator &networkOperator);
    bool UpdateEons(const string &iccId, const NetworkOperator &networkOperator);
    EonsContentsValue *FindEons(const string &iccId, const string &netPlmn);

private:
    void LoadDefaultContentValues();
    void Clear();

    static CMutex lock;
    static OperatorNameTable *_instance;
public:
    static OperatorNameTable *MakeInstance();
    static OperatorNameTable *GetInstance();
    static void ReleaseInstance();
};

IMPLEMENT_MODULE_TAG(OperatorNameTable, OperatorNameTable)

CMutex OperatorNameTable::lock;
OperatorNameTable *OperatorNameTable::_instance = nullptr;

OperatorNameTable *OperatorNameTable::MakeInstance()
{
    OperatorNameTable::lock.lock();
    if (debug) RilLogV("%s::%s lock", TAG, __FUNCTION__);
    if (_instance == nullptr) {
        _instance = new OperatorNameTable();
        if (OperatorNameTableDatabaseHelper::MakeDB() < 0) {
            RilLogE("%s::%s Load default set of operator name without creating database", TAG, __FUNCTION__);
            // Exceptional case, failed to create operator name table
            // maintain all operator name list in OperatorNameTable instead of database
            _instance->LoadDefaultContentValues();
        }
    }
    OperatorNameTable::lock.unlock();
    if (debug) RilLogV("%s::%s unlock", TAG, __FUNCTION__);
    return _instance;
}

OperatorNameTable *OperatorNameTable::GetInstance()
{
    OperatorNameTable::lock.lock();
    do {
        if (debug) RilLogV("%s::%s lock", TAG, __FUNCTION__);
    } while (0);
    OperatorNameTable::lock.unlock();
    if (debug) RilLogV("%s::%s unlock", TAG, __FUNCTION__);
    return _instance;
}

void OperatorNameTable::ReleaseInstance()
{
    OperatorNameTable::lock.lock();
    if (_instance) {
        delete _instance;
        _instance = nullptr;
    }
    OperatorNameTable::lock.unlock();
}

OperatorNameTable::OperatorNameTable() {

}

OperatorNameTable::~OperatorNameTable() {
    Clear();
}

void OperatorNameTable::LoadDefaultContentValues()
{
    RilLogI("%s", __FUNCTION__);
    Clear();

    int size = sizeof(GLOBAL_OPERATOR_INFO) / sizeof(GLOBAL_OPERATOR_INFO[0]);
    for (int i = 0; i < size; i++) {
        const char **entry = GLOBAL_OPERATOR_INFO[i];
        if (entry[0] == nullptr)
            break;

        OperatorContentValue *contentValue = OperatorContentValue::NewInstance(
                strtol(entry[0], nullptr, 10), strtol(entry[1], nullptr, 10), entry[2], entry[3], entry[4]);
        m_cache.push_back(contentValue);
    }
    RilLogV("%s List loaded total %d item(s)", __FUNCTION__, m_cache.size());
}

bool OperatorNameTable::Contains(int mcc, int mnc, const string &gid1)
{
    OperatorContentValue *contentValue = Find(mcc, mnc, gid1);
    return (contentValue != nullptr);
}

OperatorContentValue *OperatorNameTable::Find(int mcc, int mnc, const string &gid1)
{
    if (isValidMccMnc(mcc, mnc) == false) {
        return nullptr;
    }

    list<OperatorContentValue *>::iterator iter;
    for (iter = m_cache.begin(); iter != m_cache.end(); iter++) {
        OperatorContentValue *contentValue = *iter;
        if (contentValue != nullptr && contentValue->GetGid1() == gid1
                && contentValue->GetMcc() == mcc && contentValue->GetMnc() == mnc) {
            return contentValue;
        }
    }

    OperatorNameTableDatabaseHelper database;
    OperatorContentValue *contentValue = database.Find(mcc, mnc, gid1);
    if (contentValue == nullptr) {
        return nullptr;
    }

    m_cache.push_back(contentValue);

    return contentValue;
}

bool OperatorNameTable::Insert(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1) {
    if (isValidMccMnc(mcc, mnc) == false ||
        TextUtils::IsEmpty(longPlmn) || TextUtils::IsEmpty(shortPlmn)) {
        return false;
    }

    if (Contains(mcc, mnc, gid1)) {
        return false;
    }

    OperatorNameTableDatabaseHelper database;
    return database.Insert(mcc, mnc, longPlmn, shortPlmn, gid1);
}

bool OperatorNameTable::Insert(const NetworkOperator &networkOperator) {
    return Insert(networkOperator.GetMcc(), networkOperator.GetMnc(),
                  networkOperator.GetAlphaLong(), networkOperator.GetAlphaShort(), networkOperator.GetGid1());
}

bool OperatorNameTable::Insert(const OperatorContentValue *contentValue) {
    if (contentValue != nullptr) {
        return Insert(contentValue->GetMcc(), contentValue->GetMnc(),
                contentValue->GetLongPlmn(), contentValue->GetShortPlmn(), contentValue->GetGid1());
    }
    return false;
}

bool OperatorNameTable::Update(int mcc, int mnc, const string &longPlmn, const string &shortPlmn, const string &gid1) {
    if (isValidMccMnc(mcc, mnc) == false ||
        TextUtils::IsEmpty(longPlmn) || TextUtils::IsEmpty(shortPlmn)) {
        return false;
    }

    OperatorContentValue *contentValueInCache = Find(mcc, mnc, gid1);
    if (contentValueInCache == nullptr) {
        return false;
    }

    if ((contentValueInCache->GetShortPlmn() == shortPlmn) &&
        (contentValueInCache->GetLongPlmn() == longPlmn)) {
        return false;
    }

    contentValueInCache->SetLongPlmn(longPlmn);
    contentValueInCache->SetShortPlmn(shortPlmn);

    OperatorNameTableDatabaseHelper database;
    return database.Update(contentValueInCache);
}

bool OperatorNameTable::Update(const OperatorContentValue *contentValue) {
    if (contentValue != nullptr) {
        return Update(contentValue->GetMcc(), contentValue->GetMnc(),
                contentValue->GetLongPlmn(), contentValue->GetShortPlmn(), contentValue->GetGid1());
    }
    return false;
}

bool OperatorNameTable::Update(const NetworkOperator &networkOperator) {
    return Update(networkOperator.GetMcc(), networkOperator.GetMnc(),
                  networkOperator.GetAlphaLong(), networkOperator.GetAlphaShort(), networkOperator.GetGid1());
}

bool OperatorNameTable::InsertEons(const string &iccId, const NetworkOperator &networkOperator) {
    if (TextUtils::IsEmpty(iccId) ||
        networkOperator.IsNumericValid() == false ||
        networkOperator.AreBothLongPlmnShortPlmnAvailable() == false) {
        return false;
    }

    EonsContentsValue *contentsValue = EonsContentsValue::NewInstance(iccId, networkOperator);
    m_cacheEons.push_back(contentsValue);

    return true;
}

bool OperatorNameTable::UpdateEons(const string &iccId, const NetworkOperator &networkOperator) {
    EonsContentsValue *contentValueInCache = FindEons(iccId, networkOperator.GetNumeric());
    if (contentValueInCache == nullptr) {
        return false;
    }

    if (networkOperator.IsLongPlmnAvailable()) {
        contentValueInCache->SetLongPlmn(networkOperator.GetAlphaLong());
    }

    if (networkOperator.IsShortPlmnAvailable()) {
        contentValueInCache->SetShortPlmn(networkOperator.GetAlphaShort());
    }

    return true;
}

EonsContentsValue *OperatorNameTable::FindEons(const string &iccId, const string &netPlmn) {
    if (TextUtils::IsEmpty(iccId) || isValidPlmn(netPlmn) == false) {
        return nullptr;
    }

    int mcc = NetworkUtils::fetchMcc(netPlmn.c_str());
    int mnc = NetworkUtils::fetchMncNoEncoding(netPlmn.c_str());

    list<EonsContentsValue *>::iterator iterEons;
    for (iterEons = m_cacheEons.begin(); iterEons != m_cacheEons.end(); iterEons++) {
        EonsContentsValue *contentValue = *iterEons;
        if (contentValue != nullptr) {
            if (contentValue->GetMcc() == mcc && contentValue->GetMnc() == mnc &&
                contentValue->GetIccId() == iccId) {
                return contentValue;
            }
        }
    }

    return nullptr;
}

void OperatorNameTable::Clear()
{
    list<OperatorContentValue *>::iterator iter;
    for (iter = m_cache.begin(); iter != m_cache.end(); iter++) {
        OperatorContentValue * contentValue = *iter;
        if (contentValue != nullptr) {
            delete contentValue;
        }
    }
    m_cache.clear();

    list<EonsContentsValue *>::iterator iterEons;
    for (iterEons = m_cacheEons.begin(); iterEons != m_cacheEons.end(); iterEons++) {
        EonsContentsValue * contentValue = *iterEons;
        if (contentValue != nullptr) {
            delete contentValue;
        }
    }
    m_cacheEons.clear();
}


/////////////////////////////////////////////////////////////////////
// OperatorNameProvider
/////////////////////////////////////////////////////////////////////
bool OperatorNameProvider::isValidPlmn(const std::string &plmn) {
    if (!(plmn.length() == 5 || plmn.length() == 6) || TextUtils::IsDigitsOnly(plmn) == false) {
        RilLogW("Invalid carrier(%s)", plmn.c_str());
        return false;
    }

    return true;
}

bool OperatorNameProvider::isValidMccMnc(int mcc, int mnc) {
    if (mcc != INT_MAX && mnc != INT_MAX) {
        return true;
    }
    return false;
}

OperatorNameProvider *OperatorNameProvider::MakeInstance()
{
    return OperatorNameTable::MakeInstance();
}

OperatorNameProvider *OperatorNameProvider::GetInstance()
{
    return OperatorNameTable::GetInstance();
}

void OperatorNameProvider::ReleaseInstance()
{
    OperatorNameTable::ReleaseInstance();
}

/////////////////////////////////////////////////////////////////////
// Venodr specific
/////////////////////////////////////////////////////////////////////
#define NET_GR_QTELCOM_NUMERIC              "20209"
#define NET_GR_WIND_NUMERIC                 "20210"
#define NET_GR_QTELCOM_LONG_ALPHA_EONS      "Q-TELCOM"
#define NET_GR_QTELCOM_SHORT_ALPHA_EONS     "Q-TELCOM"

static string GetVendorCustomOperatorLongAlpha(
    const string &simOperatorNumeric, const string &operatorNumeric) {
    // vendor customized
    // 1. GR Q-TELCOM(20209) had been disbanded in May 2007.
    //    Q-TELCOM bands are currently used by WIND(20210)
    if (TextUtils::Equals(simOperatorNumeric, NET_GR_QTELCOM_NUMERIC)) {
        if (TextUtils::Equals(operatorNumeric, NET_GR_WIND_NUMERIC)) {
            return string{NET_GR_QTELCOM_LONG_ALPHA_EONS};
        }
    }
    return string{""};
}

static string GetVendorCustomOperatorShortAlpha(
    const string &simOperatorNumeric, const string &operatorNumeric) {
    // vendor customized
    // 1. GR Q-TELCOM(20209) had been disbanded in May 2007.
    //    Q-TELCOM bands are currently used by WIND(20210)
    if (TextUtils::Equals(simOperatorNumeric, NET_GR_QTELCOM_NUMERIC)) {
        if (TextUtils::Equals(operatorNumeric, NET_GR_WIND_NUMERIC)) {
            return string{NET_GR_QTELCOM_SHORT_ALPHA_EONS};
        }
    }
    return string{""};
}

OperatorContentValue OperatorNameProvider::GetVendorCustomOperatorName(const string &simOperatorNumeric, const string &operatorNumeric) {
    string longAlpha = GetVendorCustomOperatorLongAlpha(simOperatorNumeric, operatorNumeric);
    string shortAlpha = GetVendorCustomOperatorShortAlpha(simOperatorNumeric, operatorNumeric);

    OperatorContentValue ret;
    ret.Update(NetworkOperator(operatorNumeric, longAlpha, shortAlpha, 0, 0, GID_DEFAULT_VALUE_ANY));
    return ret;
}
