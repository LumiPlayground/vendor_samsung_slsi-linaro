#include <gtest/gtest.h>
#include "settingsprovider.h"
#include <librilutils/textutils.h>
#include "dataprofileinfo.h"
#include "settingsprovider.h"
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include <string>

using namespace std;

#if 0
#define DLOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "SettingsTestMulti"
#endif
#define DLOG(format, ...)
#endif

#define NUM_THREAD  3

void *proc(void *arg);

pthread_t tid[NUM_THREAD];

struct SimInfo {
    const char *iccid;
    const char *dp;
};

SimInfo simInfo[] = {
    { "89886972012770043771", "-1/2/0/internet///0/0/0/0/1/21/0/0/0/0/0/1" },
    { "89886017157803912406", "-1/2/0/5ginternet///0/0/0/0/1/21/0/0/0/0/0/1" },
    { "8982051508438165020F", "-1/2/0/lte.sktelecom.com///0/0/0/0/1/183/0/0/0/0/0/1" },
};
const size_t simInfoSize = sizeof(simInfo) / sizeof(simInfo[0]);

enum { READ = 0, INSERT = 1 };
const int commands[] = {
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
    READ, INSERT, INSERT,
};
const size_t commandsSize = sizeof(commands) / sizeof(commands[0]);

TEST(SettingsProvider, SettingsProvider_create_destroy) {
    string tesfilepath = "/data/local/tmp/settings_provider_db";
    SettingsProvider::TestTouch(tesfilepath);
    EXPECT_TRUE(access(tesfilepath.c_str(), F_OK) == 0);

    SettingsProvider testSettings;
    testSettings.Destroy();
    EXPECT_TRUE(access(tesfilepath.c_str(), F_OK) == -1);
}

TEST(SettingsProvider, SettingsProvider_integer) {
    string tesfilepath = "/data/local/tmp/settings_provider_db2";
    SettingsProvider::TestTouch(tesfilepath);

    SettingsProvider testSettings;
    for (int i = 0; i < 10; i++) {
        string key = "test_key";
        key += to_string(i);
        testSettings.Put(key, i);
        EXPECT_EQ(testSettings.GetInt(key), i);
    }  // end for i ~
    testSettings.Destroy();
}

TEST(SettingsProvider, SettingsProvider_string) {
    string tesfilepath = "/data/local/tmp/settings_provider_db3";
    SettingsProvider::TestTouch(tesfilepath);

    SettingsProvider testSettings;
    for (int i = 0; i < 10; i++) {
        string key = "test_key";
        key += to_string(i);
        string value = "test_value";
        value += to_string(i);
        testSettings.Put(key, value);
        EXPECT_TRUE(TextUtils::Equals(testSettings.GetString(key), value));
    }  // end for i ~
    testSettings.Destroy();
}

TEST(SettingsProvider, SettingsProvider_integer_defval) {
    string tesfilepath = "/data/local/tmp/settings_provider_db4";
    SettingsProvider::TestTouch(tesfilepath);

    SettingsProvider testSettings;
    for (int i = 0; i < 10; i++) {
        string key = "test_key";
        key += to_string(i);
        EXPECT_EQ(testSettings.GetInt(key, i), i);
    }  // end for i ~
    testSettings.Destroy();
}

TEST(SettingsProvider, SettingsProvider_string_defval) {
    string tesfilepath = "/data/local/tmp/settings_provider_db5";
    SettingsProvider::TestTouch(tesfilepath);

    SettingsProvider testSettings;
    for (int i = 0; i < 10; i++) {
        string key = "test_key";
        key += to_string(i);
        string defval = "test_defval";
        defval += to_string(i);
        EXPECT_TRUE(TextUtils::Equals(testSettings.GetString(key, defval), defval));
    }  // end for i ~

    testSettings.Destroy();
}

void *proc(void *arg) {
    size_t id = (size_t)arg;
    if (id >= simInfoSize) {
        DLOG("id(%zu) is over simInfoSize", id);
        return NULL;
    }
    DLOG("Start tid=%ld id=%zu", tid[id], id);

    const SimInfo& info = simInfo[id];
    size_t count = commandsSize;
    for (size_t i = 0; i < count; i++) {
        if (commands[i] == READ) {
            DLOG("id=%zu READ iccid=%s", id, info.iccid);
        } else if (commands[i] == INSERT) {
            DLOG("id=%zu INSERT (%s, %s)", id, info.iccid, info.dp);
        }
        usleep(100);
    }
    DLOG("Exit tid=%ld id=%zu", tid[id], id);

    return NULL;
}

TEST(SettingsProvider, SettingsProvider_multi) {
    SettingsProvider::TestTouch("/data/local/tmp/settings_provider_multi");

    SettingsProvider testSettings;
    for (size_t i = 0; i < NUM_THREAD; i++) {
        pthread_create(&tid[i], NULL, proc, (void *)i);
    }

    for (int i = 0; i < NUM_THREAD; i++) {
        pthread_join(tid[i], NULL);
    }

    testSettings.Destroy();
}

