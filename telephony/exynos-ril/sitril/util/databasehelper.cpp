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
 * databasehelper.cpp
 *
 *  Created on: 2019. 12. 16.
 *  Modified on: 2021. 1. 21.
 */
#include <sstream>
#include <unistd.h>
#include "databasehelper.h"
#include "rillog.h"
#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>

using android::sp;

#ifdef RilLogV
#undef RilLogV
static bool VDBG = (SystemProperty::GetInt("persist.vendor.config.vdbg", 0) != 0);
#define RilLogV(format, ...) if (VDBG) CRilLog::Log(CRilLog::E_RIL_VERBOSE_LOG, format, ##__VA_ARGS__)
#endif

IMPLEMENT_MODULE_TAG(Database, Database)

bool Database::Open()
{
    bool ret = false;
    if (!IsOpen()) {
        bool exists = access(mName.c_str(), F_OK) == 0;
        RilLogV("[%s] %s database %s", TAG, __FUNCTION__, (exists ? "existed" : "not existed"));

        int flag = 0;
        if (mWritable || !exists) {
            RilLogV("[%s] %s try to open a writable database", TAG, __FUNCTION__);
            flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
        }
        else {
            //open read-only
            RilLogV("[%s] %s try to open a read-only database", TAG, __FUNCTION__);
            flag = SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX;
        }

        int err = sqlite3_open_v2(mName.c_str(), &mDb, flag, NULL);
        if (err == SQLITE_OK) {
            RilLogV("[%s] %s open database(%s) success", TAG, __FUNCTION__, mName.c_str());
            ret = true;
        }
        else {
            RilLogW("[%s] %s open database failed. err=%d", TAG, __FUNCTION__, err);
        }
    }
    else {
        // already opened
        RilLogV("[%s] %s database already opened", TAG, __FUNCTION__);
        ret = true;
    }
    return ret;
}

void Database::ReopenReadWrite()
{
    RilLogV("[%s] %s ", TAG, __FUNCTION__);
    if (IsOpen() && !mWritable) {
        Close();
        mWritable = true;
    }
    Open();
}

void Database::Close()
{
    RilLogV("[%s] %s ", TAG, __FUNCTION__);
    if (mDb != NULL) {
        sqlite3_close(mDb);
        mDb = NULL;
    }
}

sqlite3_stmt *Database::Query(const string& sql)
{
    if (TextUtils::IsEmpty(sql)) {
        RilLogW("[%s] %s invalid parameter", TAG, __FUNCTION__);
        return NULL;
    }

    if (!IsOpen()) {
        RilLogW("[%s] %s database not opened", TAG, __FUNCTION__);
        return NULL;
    }

    RilLogV("[%s] %s sql=%s", TAG, __FUNCTION__, sql.c_str());

    sqlite3_stmt *statement = NULL;
    int err = SQLITE_OK;
    if ((err = sqlite3_prepare(mDb, sql.c_str(), -1, &statement, NULL)) != SQLITE_OK) {
        RilLogW("[%s] %s sqlite3_prepare error", TAG, __FUNCTION__);
        sqlite3_finalize(statement);
        statement = NULL;
    }
    return statement;
}

bool Database::Exec(const string& sql)
{
    if (TextUtils::IsEmpty(sql)) {
        RilLogW("[%s] %s invalid parameter", TAG, __FUNCTION__);
        return false;
    }

    if (!IsOpen()) {
        RilLogW("[%s] %s database not opened", TAG, __FUNCTION__);
        return false;
    }

    RilLogV("[%s] %s sql=%s", TAG, __FUNCTION__, sql.c_str());

    char *errMsg = NULL;
    if (sqlite3_exec(mDb, sql.c_str(), NULL, NULL, &errMsg) != SQLITE_OK) {
        // log error
        if (VDBG) {
            RilLogW("[%s] %s sqlite3_exec error: %s", TAG, __FUNCTION__, errMsg);
        }
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int Database::GetVersion()
{
    if (VDBG) RilLogV("[%s] %s ", TAG, __FUNCTION__);
    // get current version using PRAGMA
    int version = 0;   // unknown
    string sql = "PRAGMA user_version;";
    sqlite3_stmt *stmt = Query(sql);
    if (stmt != NULL) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            version = sqlite3_column_int(stmt, 0);
            RilLogV("[%s] %s version=%d", TAG, __FUNCTION__, version);
        }
        sqlite3_finalize(stmt);
    }
    return version;
}

void Database::SetVersion(int version)
{
    if (VDBG) RilLogV("[%s] %s version=%d", TAG, __FUNCTION__, version);
    // set current version using PRAGMA
    stringstream ss;
    ss << "PRAGMA user_version = " << version << ";";
    string sql = ss.str();
    if (!Exec(sql)) {
        RilLogV("[%s] %s failed", TAG, __FUNCTION__);
    }
}

void Database::BeginTransaction()
{
    if (!mInTransaction) {
        string sql = "BEGIN TRANSACTION;";
        mInTransaction = true;
        if (!Exec(sql)) {
            if (VDBG) {
                RilLogV("[%s] %s failed", TAG, __FUNCTION__);
            }
        }
    }
}

void Database::EndTransaction()
{
    if (mInTransaction) {
        string sql = "END TRANSACTION;";
        if (!Exec(sql)) {
            RilLogV("[%s] %s failed", TAG, __FUNCTION__);
        }
        mInTransaction = false;
    }
}

void Database::RollbackTransaction()
{
    if (mInTransaction) {
        string sql = "ROLLBACK TRANSACTION;";
        if (!Exec(sql)) {
            RilLogV("[%s] %s failed", TAG, __FUNCTION__);
        }
        mInTransaction = false;
    }
}


Database *Database::OpenDatabase(const string& name, bool writable)
{
    RilLogV("[%s] %s name=%s writable=%d", TAG, __FUNCTION__, name.c_str(), writable);
    Database *db = new Database();
    if (db != NULL) {
        db->mName = name;
        db->mWritable = writable;
        db->Open();
    }
    return db;
}

/**
 * DatabaseOpenHelper
 */
IMPLEMENT_MODULE_TAG(DatabaseOpenHelper, DatabaseOpenHelper)

DatabaseOpenHelper::DatabaseOpenHelper(const string& name, int version)
    : mDatabase(NULL)
{
    mName = name;
    mVersion = version;
    if (mVersion <= 0) {
        mVersion = DATABASE_MIN_VERSION;
    }
}

DatabaseOpenHelper::~DatabaseOpenHelper()
{
    Close();

    // do not delete
    // decrease a reference count
    mDatabase = NULL;
}

void DatabaseOpenHelper::Close()
{
    if (mDatabase != NULL) {
        mDatabase->Close();
    }
}

sp<Database> DatabaseOpenHelper::GetDatabase(bool writable)
{
    if (mDatabase != NULL) {
        if (!mDatabase->IsOpen()) {
            mDatabase = NULL;
        }
        else if (!writable || !mDatabase->IsReadOnly()) {
            return mDatabase;
        }
    }

    Database *db = mDatabase.get();
    RilLogV("[%s] %s mDatabase.get()=%p", TAG, __FUNCTION__, db);
    if (db != NULL) {
        if (writable && db->IsReadOnly()) {
            RilLogV("[%s] %s ReopenReadWrite", TAG, __FUNCTION__);
            db->ReopenReadWrite();
        }
    }
    else {
        RilLogV("[%s] %s Open database(name=%s, writable=%d)", TAG, __FUNCTION__, mName.c_str(), writable);
        db = Database::OpenDatabase(mName, writable);
    }

    if (db != NULL) {
        int version = db->GetVersion();
        if (VDBG) {
            RilLogV("[%s] %s oldVersion=%d newVersion=%d", TAG, __FUNCTION__, version, mVersion);
        }
        if (version != mVersion) {
            db->BeginTransaction();
            if (version == 0) {
                OnCreate(db);
            }
            else {
                if (version > mVersion) {
                    RilLogV("[%s] %s OnDowngrade(%d->%d)", TAG, __FUNCTION__, version, mVersion);
                    OnDowngrade(db, version, mVersion);
                }
                else {
                    RilLogV("[%s] %s OnUpgrade(%d->%d)", TAG, __FUNCTION__, version, mVersion);
                    OnUpgrade(db, version, mVersion);
                }
            }
            db->SetVersion(mVersion);
            db->EndTransaction();
        }
        mDatabase = db;
    }
    return mDatabase;
}

sp<Database> DatabaseOpenHelper::GetReadableDatabase()
{
    return GetDatabase(false);
}

sp<Database> DatabaseOpenHelper::GetWritableDatabase()
{
    return GetDatabase(true);
}
