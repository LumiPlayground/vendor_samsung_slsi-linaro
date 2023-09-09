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
 * ts25table.cpp
 *
 *  Created on: 2019. 3. 5.
 */

#include "ts25table.h"
#include <sqlite3.h>
#include <sstream>
#include "rillog.h"
#include <librilutils/textutils.h>
#include <unistd.h>

#define SQL_BUF_SIZE             512
#define DB_NAME                  "/data/vendor/rild/.ts25table.db"
#define DB_DUMP_PATH             "/data/vendor/rild/ts25table_dump.txt"
#define TABLE_NAME_SETTINGS      "settings"
#define COL_VERSION              "version"
#define COL_STATUS               "status"
#define COL_INDEX_VERSION        (0)
#define COL_INDEX_STATUS         (1)
#define STATUS_DONE              "done"

#define TABLE_NAME_TS25          "ts25table"
#define COL_ID                   "_id"
#define COL_MCC                  "mcc"
#define COL_MNC                  "mnc"
#define COL_ABREV_NET_NAME       "abrev_net_name"
#define COL_PPCIN                "ppcin"
#define COL_INDEX_ID             0
#define COL_INDEX_MCC            1
#define COL_INDEX_MNC            2
#define COL_INDEX_ABREV_NET_NAME 3
#define COL_INDEX_PPCIN          4

#define TABLE_VERSION_11         11   // Apr. 25. 2022 (Published TS25 Database 25 April 2022.xlsx from GSMA)
#define TABLE_VERSION_11_DATE    "4/25/2022"
#define TABLE_VERSION_12         12   // Aug. 15. 2022 (Published TS.25 Database 15 August 2022.xlsx from GSMA)
#define TABLE_VERSION_12_DATE    "8/15/2022"
#define TABLE_VERSION_13         13   // Test for next version

#define TABLE_CURRENT_VERSION    TABLE_VERSION_12
#define TABLE_CURRENT_DATE       TABLE_VERSION_12_DATE

#define CURRENT_TS25_DATA        "ts25_aug_15_2022.txt"
#define CURRENT_NON_TS25_DATA    "non_ts25.txt"

static bool debug = false;
static pthread_mutex_t s_ts25DbMutex = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////////////////////////////////////////
// TS25TableDatabaseHelper
/////////////////////////////////////////////////////////////////////
class TS25TableDatabaseHelper {
    DECLARE_MODULE_TAG()
public:
    TS25TableDatabaseHelper() {
        m_database = NULL;
        mVersion = TABLE_CURRENT_VERSION;
    }

    virtual ~TS25TableDatabaseHelper() {
        Close();
    }

private:
    sqlite3 *m_database;
    int mVersion;

protected:
    sqlite3 *Open() {
        if (sqlite3_open(DB_NAME, &m_database) != SQLITE_OK) {
            RilLogE("%s : Failed to open database", __FUNCTION__);
            m_database = NULL;
        }
        return m_database;
    }
    void Close() {
        if (m_database != NULL) {
            sqlite3_close(m_database);
            m_database = NULL;
        }
    }

public:
    bool Contains(int mcc, int mnc);
    TS25Record Find(int mcc, int mnc);
    bool Update(int mcc, int mnc, const char *networkName, const char *ppcin);
    bool Insert(int mcc, int mnc, const char *networkName, const char *ppcin);
    bool InsertOrUpdate(int mcc, int mnc, const char *networkName, const char *ppcin);
public:
    bool CreateSettingsTable();
    bool CreateTS25Table();
    void DropDB();
    int CreateAllRecords();
    int GetTS25TableCount();

private:
    bool TestDatabaseVersion();
    bool TestTS25Table();
    static TS25TableDatabaseHelper *instance;
public:
    static TS25TableDatabaseHelper *GetInstance();
    static void ReleaseInstance();
};

IMPLEMENT_MODULE_TAG(TS25TableDatabaseHelper, TS25TableDatabaseHelper)

TS25TableDatabaseHelper *TS25TableDatabaseHelper::instance = NULL;

TS25TableDatabaseHelper *TS25TableDatabaseHelper::GetInstance()
{
    if (instance == NULL) {
        instance = new TS25TableDatabaseHelper();
        if (instance != NULL) {
            if (!instance->TestDatabaseVersion() || !instance->TestTS25Table()) {
                // drop first
                instance->DropDB();

                if (!instance->CreateTS25Table() || !instance->CreateSettingsTable()) {
                    instance->DropDB();
                    RilLogW("%s %s Failed to create database table : Drop database.", TAG, __FUNCTION__);

                    delete instance;
                    instance = NULL;
                }
            }
        }
    }

    return instance;
}

void TS25TableDatabaseHelper::ReleaseInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

int TS25TableDatabaseHelper::GetTS25TableCount()
{
    int counts = -1;
    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = NULL;
    sqlite3_stmt *statement = NULL;
    int err = SQLITE_OK;
    pthread_mutex_lock(&s_ts25DbMutex);
    if ((err = sqlite3_open_v2(DB_NAME, &database, SQLITE_OPEN_READONLY, NULL)) == SQLITE_OK) {
        // check database version
        snprintf(sql, sizeof(sql), "SELECT count(*) FROM %s", TABLE_NAME_TS25);
        if ((err = sqlite3_prepare(database, sql, -1, &statement, NULL)) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                counts = sqlite3_column_int(statement, 0);
                RilLogV("%s %s Total records is(are) = %d", TAG, __FUNCTION__, counts);
            }
            else {
                RilLogW("%s %s : No record(s) in database", TAG, __FUNCTION__);
            }
            sqlite3_finalize(statement);
        }
        else {
            RilLogE("%s %s : sqlite3_prepare error : error code(%d)", TAG, __FUNCTION__, err);
        }
        sqlite3_close(database);
    }
    pthread_mutex_unlock(&s_ts25DbMutex);
    return counts;
}

bool TS25TableDatabaseHelper::TestDatabaseVersion()
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = NULL;
    sqlite3_stmt *statement = NULL;
    bool test = false;
    int err = SQLITE_OK;
    pthread_mutex_lock(&s_ts25DbMutex);
    if ((err = sqlite3_open_v2(DB_NAME, &database, SQLITE_OPEN_READONLY, NULL)) == SQLITE_OK) {

        // check database version
        snprintf(sql, sizeof(sql), "SELECT * FROM %s", TABLE_NAME_SETTINGS);
        if ((err = sqlite3_prepare(database, sql, -1, &statement, NULL)) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                int version = sqlite3_column_int(statement, COL_INDEX_VERSION);
                char *status = (char *)sqlite3_column_text(statement, COL_INDEX_STATUS);
                RilLogV("%s %s current version=%d, new version=%d, status=%s",
                        TAG, __FUNCTION__, version, mVersion, status);
                if (version >= mVersion && TextUtils::Equals(status, STATUS_DONE)) {
                    test = true;
                }
            }
            sqlite3_finalize(statement);
        }
        else {
            RilLogE("%s %s : sqlite3_prepare error : error code(%d)", TAG, __FUNCTION__, err);
        }
        sqlite3_close(database);
    }
    pthread_mutex_unlock(&s_ts25DbMutex);
    return test;
}

bool TS25TableDatabaseHelper::TestTS25Table()
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    if (GetTS25TableCount() < 0) {
        return false;
    }
    return true;
}

bool TS25TableDatabaseHelper::CreateSettingsTable()
{
    //int err = 0;
    bool ret = false;
    char sql[SQL_BUF_SIZE] = { 0, };
    sqlite3 *database = NULL;
    char *errMsg = NULL;

    if (sqlite3_open(DB_NAME, &database) != SQLITE_OK) {
        /*  DB create error */
        RilLogE("%s %s Failed to create DB file %s", TAG, __FUNCTION__, DB_NAME);
        return false;
    }

    // create settings table
    snprintf(sql, sizeof(sql)-1, "CREATE TABLE %s (" \
                    "%s INTEGER PRIMARY KEY, " \
                    "%s TEXT);",
            TABLE_NAME_SETTINGS, COL_VERSION, COL_STATUS);
    if (sqlite3_exec(database, sql, NULL, NULL, &errMsg) == SQLITE_OK) {
        snprintf(sql, sizeof(sql)-1, "INSERT INTO %s (%s, %s) VALUES(\"%d\", \"%s\")",
                TABLE_NAME_SETTINGS, COL_VERSION, COL_STATUS, mVersion, STATUS_DONE);
        if (sqlite3_exec(database, sql, NULL, NULL, &errMsg) == SQLITE_OK) {
            RilLogV("%s %s set database version=%d", TAG, __FUNCTION__, mVersion);
            ret = true;
        }
        else {
            RilLogW("%s %s Failed to insert content : error(%s)", TAG, __FUNCTION__, errMsg);
            sqlite3_free(errMsg);
        }
    }
    else {
        // create table error
        RilLogE("%s %s Failed to create %s table : error(%s)", TAG, __FUNCTION__, TABLE_NAME_SETTINGS, errMsg);
        sqlite3_free(errMsg);
    }
    sqlite3_close(database);

    return ret;
}

bool TS25TableDatabaseHelper::CreateTS25Table()
{
    sqlite3 *database = NULL;
    char *errMsg = NULL;

    if (sqlite3_open(DB_NAME, &database) != SQLITE_OK) {
        /*  DB create error */
        RilLogE("%s %s Failed to create DB file %s", TAG, __FUNCTION__, DB_NAME);
        return false;
    }

    // create operator table
    std::stringstream ss;
    ss << "CREATE TABLE " << TABLE_NAME_TS25 << " (";
    ss << COL_ID << " INTEGER PRIMARY KEY, ";
    ss << COL_MCC << " INTEGER, ";
    ss << COL_MNC << " INTEGER, ";
    ss << COL_ABREV_NET_NAME << " TEXT, ";
    ss << COL_PPCIN << " TEXT);";
    std::string sql = ss.str();
    if (debug)
        RilLogV("%s %s %s", TAG, __FUNCTION__, sql.c_str());

    if (sqlite3_exec(database, sql.c_str(), NULL, NULL, &errMsg) == SQLITE_OK) {
        int records = CreateAllRecords();
        RilLogV("%s %s total %d record(s) is(are) added.", TAG, __FUNCTION__, records);
    }
    else {
        // create table error
        RilLogE("%s %s Failed to create %s table : error(%s)", TAG, __FUNCTION__, TABLE_NAME_TS25, errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(database);
    return true;
}

void TS25TableDatabaseHelper::DropDB()
{
    // drop database file
    int ret = unlink(DB_NAME);
    RilLogV("%s %s Drop database(%d)", TAG, __FUNCTION__, ret);
}

int TS25TableDatabaseHelper::CreateAllRecords()
{
    RilLog("%s TS.25 Table version %d", TAG, TABLE_CURRENT_VERSION);
    int records = 0;

#include CURRENT_TS25_DATA

    RilLog("%s Non-TS.25 Table", TAG);
#include CURRENT_NON_TS25_DATA

    records = GetTS25TableCount();
    return records;
}

bool TS25TableDatabaseHelper::Contains(int mcc, int mnc)
{
    if (mcc < 0 || mcc >= 1000) {
        return false;
    }

    if (mnc < 0 || mnc >= 1000) {
        return false;
    }

    TS25Record record = Find(mcc, mnc);
    return record.IsValid();
}

TS25Record TS25TableDatabaseHelper::Find(int mcc, int mnc)
{
    sqlite3_stmt *statement = NULL;
    char sql[SQL_BUF_SIZE] = { 0, };
    TS25Record record;

    if (mcc < 0 || mcc >= 1000) {
        return record;
    }

    if (mnc < 0 || mnc >= 1000) {
        return record;
    }

    Open();

    if (m_database == NULL) {
        RilLogW("%s : Invalid database handle", __FUNCTION__);
        return record;
    }

    snprintf(sql, sizeof(sql), "SELECT * FROM %s WHERE %s=%d and %s =%d", TABLE_NAME_TS25, COL_MCC, mcc, COL_MNC, mnc);
    int err = SQLITE_OK;
    pthread_mutex_lock(&s_ts25DbMutex);
    if ((err = sqlite3_prepare(m_database, sql, -1, &statement, NULL)) == SQLITE_OK) {
        // select the first record only
        if (sqlite3_step(statement) == SQLITE_ROW) {
            char *networkName = (char *)sqlite3_column_text(statement, COL_INDEX_ABREV_NET_NAME);
            char *ppcin = (char *)sqlite3_column_text(statement, COL_INDEX_PPCIN);
            if (networkName != NULL && ppcin != NULL) {
                record.mcc = mcc;
                record.mnc = mnc;
                record.networkName = networkName;
                record.ppcin = ppcin;
            }
            RilLogV("%s : Found record %s for carrier %d/%d", __FUNCTION__, networkName, mcc, mnc);
        }
        else {
            RilLogV("%s : No record for carrier %d/%d", __FUNCTION__, mcc, mnc);
        }
    }
    else {
        RilLogE("%s : sqlite3_prepare error : error code(%d)", __FUNCTION__, err);
        RilLogE("%s : sql=%s", __FUNCTION__, sql);

        // TODO may re-create DB file.
    }
    sqlite3_finalize(statement);
    Close();
    pthread_mutex_unlock(&s_ts25DbMutex);
    return record;
}

bool TS25TableDatabaseHelper::Update(int mcc, int mnc, const char *networkName, const char *ppcin)
{
    if (mcc < 0 || mcc >= 1000) {
        return false;
    }

    if (mnc < 0 || mnc >= 1000) {
        return false;
    }

    if (TextUtils::IsEmpty(networkName)) {
        return false;
    }

    if (TextUtils::IsEmpty(ppcin)) {
        return false;
    }

    Open();

    char sql[SQL_BUF_SIZE] = { 0, };
    if (m_database == NULL) {
        return false;
    }

    snprintf(sql, SQL_BUF_SIZE, "UPDATE %s SET %s=\"%s\", %s=\"%s\" WHERE %s=\"%d\" AND %s=\"%d\"",
            TABLE_NAME_TS25,
            COL_ABREV_NET_NAME, networkName,
            COL_PPCIN, ppcin,
            COL_MCC, mcc, COL_MNC, mnc);

    char *errMsg = NULL;
    if (sqlite3_exec(m_database, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        // TODO log error
        RilLogE("%s : sqlite3_exec error : %s", __FUNCTION__, errMsg);
        RilLogE("%s : sql=%s", __FUNCTION__, sql);

        sqlite3_free(errMsg);
        Close();
        return false;
    }
    Close();
    if (debug)
        RilLogV("%s %s", TAG, sql);
    return true;
}

bool TS25TableDatabaseHelper::Insert(int mcc, int mnc, const char *networkName, const char *ppcin)
{
    if (mcc < 0 || mcc >= 1000) {
        return false;
    }

    if (mnc < 0 || mnc >= 1000) {
        return false;
    }

    if (TextUtils::IsEmpty(networkName)) {
        return false;
    }

    if (TextUtils::IsEmpty(ppcin)) {
        return false;
    }

    Open();

    char sql[SQL_BUF_SIZE] = { 0, };
    if (m_database == NULL) {
        return false;
    }

    snprintf(sql, SQL_BUF_SIZE, "INSERT INTO %s (%s, %s, %s, %s) VALUES(\"%d\", \"%d\", \"%s\", \"%s\")",
            TABLE_NAME_TS25,
            COL_MCC, COL_MNC, COL_ABREV_NET_NAME, COL_PPCIN, mcc, mnc, networkName, ppcin);

    char *errMsg = NULL;
    if (sqlite3_exec(m_database, sql, NULL, NULL, &errMsg) != SQLITE_OK) {
        // log error
        RilLogE("%s : sqlite3_exec error : %s", __FUNCTION__, errMsg);
        RilLogE("%s : sql=%s", __FUNCTION__, sql);

        sqlite3_free(errMsg);
        Close();
        return false;
    }

    if (debug)
        RilLogV("%s %s)", TAG, sql);
    Close();
    return true;
}

bool TS25TableDatabaseHelper::InsertOrUpdate(int mcc, int mnc, const char *networkName, const char *ppcin)
{
    bool ret = false;
    if (!Contains(mcc, mnc)) {
        ret = Insert(mcc, mnc, networkName, ppcin);
    }
    else {
        ret = Update(mcc, mnc, networkName, ppcin);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////
// TS25TableDumpHelper
/////////////////////////////////////////////////////////////////////
class TS25TableDumpHelper {
private:
    sqlite3 *database;
    sqlite3_stmt *statement;

public:
    TS25TableDumpHelper() {
        database = NULL;
        statement = NULL;
    }
    virtual ~TS25TableDumpHelper() {
        Close();
    }

    bool Prepare() {
        // reset
        Close();

        // open database
        int errorCode = 0;
        if ((errorCode = sqlite3_open_v2(DB_NAME, &database, SQLITE_OPEN_READONLY, NULL)) != SQLITE_OK) {
            RilLogE("%s() db open error : %d", __FUNCTION__, errorCode);
            return false;
        }

        std::string sql = "SELECT * from ";
        sql += TABLE_NAME_TS25;

        if (debug)
            RilLogV("[TS25TableDumpHelper] sql=%s", sql.c_str());

        int cnt = 0;
        do {
            if  ( cnt > 0 ) {
                RilLogV("%s SQLITE_BUSY(5), retry(%d) after x ms", __FUNCTION__, cnt);
                WaitForTimeUs(__FUNCTION__, 200'000U);
            }
            errorCode = sqlite3_prepare(database, sql.c_str(), -1, &statement, NULL);
        }while( errorCode == SQLITE_BUSY && ++cnt < 10);

        if ( errorCode  != SQLITE_OK ) {
              RilLogE("%s() db prepare error : %d", __FUNCTION__, errorCode);
              return false;
        }

        return true;
    }

    bool HasNext() {
        if (statement == NULL) {
            RilLogW("%s() statement id NULL", __FUNCTION__);
            return false;
        }

        bool ret = sqlite3_step(statement) == SQLITE_ROW;
        if (!ret) {
            sqlite3_finalize(statement);
            statement = NULL;
        }
        return ret;
    }

    TS25Record Next() {
        TS25Record record;
        if (statement == NULL) {
            RilLogW("%s() statement id NULL", __FUNCTION__);
            return record;
        }

        int mcc = sqlite3_column_int(statement, COL_INDEX_MCC);
        int mnc = sqlite3_column_int(statement, COL_INDEX_MNC);
        char *networkName = (char *)sqlite3_column_text(statement, COL_INDEX_ABREV_NET_NAME);
        char *ppcin = (char *)sqlite3_column_text(statement, COL_INDEX_PPCIN);

        record.mcc = mcc;
        record.mnc = mnc;
        record.networkName = networkName;
        record.ppcin = ppcin;

        return record;
    }

    void Close() {
        if(statement != NULL) {
            sqlite3_finalize(statement);
            statement = NULL;
        }
        if (database != NULL) {
            sqlite3_close(database);
            database = NULL;
        }
    }

    void Dump() {
        FILE *fp = fopen(DB_DUMP_PATH, "we");
        if (fp == NULL) {
            return ;
        }

        int count = 0;
        TS25Record record;
        fprintf(fp, "MCC/MNC\t\t\tAbrev. Net. Name/PPCI&N\n");
        fprintf(fp, "-------------------------------------------------------------------------------\n");
        //pthread_mutex_lock(&s_ts25DbMutex);
        if (Prepare()) {
            while (HasNext()) {
                record = Next();
                if (record.IsValid()) {
                    fprintf(fp, "%d/%d\t\t\t%s/%s\n", record.mcc, record.mnc,
                            record.networkName.c_str(), record.ppcin.c_str());
                    count++;
                }
            } // end while ~
            fprintf(fp, "\n");
            fprintf(fp, "TS.25 update date: %s\n", TABLE_CURRENT_DATE);
            fprintf(fp, "number of records: %d\n", count);
            fflush(fp);
        }
        //pthread_mutex_unlock(&s_ts25DbMutex);
        fclose(fp);
        fp = NULL;
    }
};

/////////////////////////////////////////////////////////////////////
// TS25Record
/////////////////////////////////////////////////////////////////////
int TS25Record::MCCMNC_CODES_USE_LONG_NAME_IN_TS25[][2] = {
    // GR COSMOTE
    {202, 1},
};

TS25Record::TS25Record() {
    networkName = "";
    ppcin = "";
    mcc = 0;
    mnc = 0;
}

bool TS25Record::IsValid() const {
    if (mcc < 0 || mcc >= 1000) {
        return false;
    }

    if (mnc < 0 || mnc >= 1000) {
        return false;
    }

    if (mcc == 0 && mnc == 0) {
        return false;
    }

    if (TextUtils::IsEmpty(networkName) && TextUtils::IsEmpty(ppcin)) {
        return false;
    }

    return true;
}

bool TS25Record::isUsingLongNameOfT32Table(int _mcc, int _mnc) {
    int size = sizeof(MCCMNC_CODES_USE_LONG_NAME_IN_TS25) / sizeof(MCCMNC_CODES_USE_LONG_NAME_IN_TS25[0]);
    for (int i = 0; i < size; i++) {
        if (MCCMNC_CODES_USE_LONG_NAME_IN_TS25[i][0] == _mcc && MCCMNC_CODES_USE_LONG_NAME_IN_TS25[i][1] == _mnc) {
            return true;
        }
    }

    return false;
}

TS25Record &TS25Record::operator=(const TS25Record &rhs) {
    if (this == &rhs)
        return *this;
    this->mcc = rhs.mcc;
    this->mnc = rhs.mnc;
    this->networkName = rhs.networkName;
    this->ppcin = rhs.ppcin;
    return *this;
}

/**
 * TS25Table
 */
TS25Table *TS25Table::instance = NULL;

TS25Table::TS25Table()
{
}

TS25Table::~TS25Table()
{
}

bool TS25Table::Init()
{
    TS25TableDatabaseHelper *ts25table = TS25TableDatabaseHelper::GetInstance();
    bool ret = (ts25table != NULL);
    return ret;
}

TS25Record TS25Table::GetRecord(int mcc, int mnc)
{
    TS25Record record;
    TS25TableDatabaseHelper *ts25table = TS25TableDatabaseHelper::GetInstance();
    if (ts25table != NULL) {
        record = ts25table->Find(mcc, mnc);
    }
    return record;
}

void TS25Table::Dump()
{
    RilLogV("TS25Table::Dump start");
    TS25TableDumpHelper dumpHelper;
    dumpHelper.Dump();
    RilLogV("TS25Table::Dump end");
}

TS25Table *TS25Table::MakeInstance()
{
    if (instance == NULL) {
        instance = new TS25Table();
        if (instance != NULL) {
            instance->Init();
        }
    }
    return instance;
}

TS25Table *TS25Table::GetInstance()
{
    return instance;
}

void TS25Table::ReleaseInstance() {
    if (instance) {
        TS25TableDatabaseHelper::ReleaseInstance();
        delete instance;
        instance = nullptr;
    }
}
