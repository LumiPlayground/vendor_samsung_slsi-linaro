/*
 *  SIT RIL network Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include <librilutils/build.h>
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>
#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include <librilutils/osallog.h>
#include <rilversioninfo.h>
#include <util/util.h>
#include <base/rillog.h>
#include <base/rillogcapture.h>
#include <util/networkutils.h>
#include <util/open_carrier.h>
#include <librilutils/radio_conv_utils.h>
#include "sitrilTestUtil.h"
#include "testUtil.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// sitrilTestMisc class
void SitrilTestUtil::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();

    mpModemData = nullptr;
    mRspData = nullptr;
    mRspDataLen = 0;
}

void SitrilTestUtil::TearDown() {
    mSitRilTest = nullptr;

    if (mpModemData) {
        delete mpModemData;
        mpModemData = nullptr;
    }
    if (mRspData) {
        free(mRspData);
        mRspData = nullptr;
    }
    mRspDataLen = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
TEST_P(SitrilTestUtil, rilApi) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);

    // check function sall be implemeted later if it is needed.

    const RIL_RadioFunctions *(*initRil)(const void *, int, char **) = nullptr;
    const RIL_RadioExternalFunctions *(*initRilExternal)(const void *) = nullptr;

    void *handle = psitRilTest->getHandle();
    if (handle) {
        initRil = (const RIL_RadioFunctions *(*)(const void *, int, char **))dlsym(handle, "RIL_Init");
        initRilExternal = (const RIL_RadioExternalFunctions *(*)(const void *))dlsym(handle, "RIL_External_Init");
    }

    const RIL_RadioFunctions *radioFunc = nullptr;
    const RIL_RadioExternalFunctions *radioExternFunc = nullptr;
    int intRet = 0;
    const char *pRet = nullptr;

    ASSERT_NE(nullptr, initRil);
    radioFunc = initRil(nullptr, 0, nullptr);
    ASSERT_NE(nullptr, radioFunc);

    if (radioFunc->onStateRequest) {
#if (SIM_COUNT >= 2)
        intRet = radioFunc->onStateRequest(RIL_SOCKET_1);
#else
        intRet = radioFunc->onStateRequest();
#endif
        EXPECT_EQ(RADIO_STATE_ON, intRet);
    }

    if (radioFunc->supports) {
        intRet = radioFunc->supports(0);
        EXPECT_EQ(0, intRet);
    }

    if (radioFunc->onCancel) {
        radioFunc->onCancel(nullptr);
    }

    if (radioFunc->getVersion) {
        pRet = radioFunc->getVersion();
        EXPECT_NE(nullptr, pRet);
    }

    ASSERT_NE(nullptr, initRilExternal);
    radioExternFunc = initRilExternal(nullptr);
    EXPECT_NE(nullptr, radioExternFunc);
}

TEST_P(SitrilTestUtil, basicUtilClass) {
    string retString;
    int retInt;
    bool retBool;
    char *pretChar;
    char srcCharArray[10] = "test";
    int srcCharArraySize = 4;
    char dstCharArray[10] = {0,};
    int dstCharArraySize = sizeof(dstCharArray);

    // check function sall be implemeted later if it is needed.

    // Build class
    retString = Build::ID();
    retString = Build::Display();
    retString = Build::Product();
    retString = Build::Device();
    retString = Build::Board();
    retString = Build::Manufacturer();
    retString = Build::Brand();
    retString = Build::Model();
    retString = Build::Bootloader();
    retString = Build::Hardware();
    retString = Build::Serial();
    retString = Build::SupportedABIS();
    retString = Build::Supported32bitABIS();
    retString = Build::Supported64bitABIS();

    // ril_mcc.h
    retString = ril::util::mcc::decode(1000);
    retString = ril::util::mcc::decode(2);

    // ril_mnc.h
    retString = ril::util::mnc::decode(0x00000001);
    retString = ril::util::mnc::decode(0x50000001);
    retString = ril::util::mnc::decode(0x20000011);
    retString = ril::util::mnc::decode(0x30000111);
    retInt = ril::util::mnc::encode(1, 3);
    retInt = ril::util::mnc::encode(1, 5);

    // TextUtil class
    retBool = TextUtils::Equals(nullptr, nullptr);
    EXPECT_EQ(true, retBool);
    retBool = TextUtils::Equals(nullptr, srcCharArray);
    EXPECT_EQ(false, retBool);
    retBool = TextUtils::DupString(nullptr, srcCharArray, false);
    EXPECT_EQ(false, retBool);

    // RILVersionInfo class
    RILVersionInfo *pVerserInfo = new RILVersionInfo();
    if (pVerserInfo)
        delete pVerserInfo;
    pVerserInfo = nullptr;

    // SECURELIB
    pretChar = SECURELIB::strncpy(nullptr, 0, nullptr, 0);
    pretChar = SECURELIB::strncpy(dstCharArray, 0, srcCharArray, srcCharArraySize);
    pretChar = (char *)SECURELIB::memcpy(nullptr, 0, nullptr, 0);
    pretChar = (char *)SECURELIB::memcpy(dstCharArray, 0, srcCharArray, srcCharArraySize);
    pretChar = (char *)SECURELIB::memcpy(dstCharArray, dstCharArraySize, srcCharArray, srcCharArraySize);
    pretChar = SECURELIB::strncat(nullptr, 0, srcCharArray, srcCharArraySize);
    pretChar = SECURELIB::strncat(dstCharArray, 0, srcCharArray, srcCharArraySize);

    // util
    char rawData[10] = {
        10,
        20,
        30,
        40,
    };
    pretChar = ConvertToHexString(rawData, 4);
    EXPECT_NE(nullptr, pretChar);
    if (pretChar)
        delete[] pretChar;

    char hexData[10] = "0A1E4A";
    retInt = ConvertToRaw(nullptr, nullptr, 10);
    EXPECT_EQ(0, retInt);
    ConvertToRaw(nullptr, rawData, 10);
    EXPECT_EQ(0, retInt);
    ConvertToRaw(hexData, rawData, 1);
    EXPECT_EQ(0, retInt);
    ConvertToRaw(hexData, rawData, &retInt);
    EXPECT_EQ(3, retInt);

    retString = GetSimOperatorNumericCdma(0);

    retString = bcdToString((const BYTE *)srcCharArray, srcCharArraySize);

    retBool = IsPlmnMatchedSimPlmn("450001", 0);
}

TEST_P(SitrilTestUtil, SystemProperty) {
    string key = "vendor.ril.net.rejectcause";
    string setVal = "10";
    string orgVal = SystemProperty::Get(key);

    SystemProperty::Set(key, setVal);
    EXPECT_TRUE(setVal == SystemProperty::Get(key));

    SystemProperty::Set(key, orgVal);
    SystemProperty::Set(key, stoi(setVal));
    EXPECT_TRUE(stoi(setVal) == SystemProperty::GetInt(key));

    SystemProperty::Set(key, orgVal);
}

TEST_P(SitrilTestUtil, OsalLog) {
    CREATE_OSAL_LOG_INSTANCE;
    ASSERT_TRUE(OsalLog::GetInstance() != nullptr);

    const int numOfLoop = 2;
    for (int i = 0; i < numOfLoop; ++i) {
        OSAL_RADIO_LOGE("[OsalLogUnitTest][err][red] %d, %d, %d, %d", 1, 2, 3, 4);
        OSAL_RADIO_LOGW("[OsalLogUnitTest][warning][orange] test log");
        OSAL_RADIO_LOGD("[OsalLogUnitTest][debug][blue] %s, %s", "test", "log");
        OSAL_RADIO_LOGI("[OsalLogUnitTest][info][grean] test log");
        OSAL_RADIO_LOGV("[OsalLogUnitTest][verbose][black] test log");
    }
}

TEST_P(SitrilTestUtil, CrilLog) {
    // CRilLog
    char testChars[] = "SitrilTestUtil_rilLog";
    char testFormat[] = "1:%s, 2:%d, 3:%s";
    int testCategory = RIL_LOG_CAT_CALL;
    int tetsLogLevel[] = {CRilLog::E_RIL_CRITICAL_LOG,
                          CRilLog::E_RIL_WARNING_LOG,
                          CRilLog::E_RIL_INFO_LOG,
                          CRilLog::E_RIL_VERBOSE_LOG};

    char **ppLog = CRilLog::BufferedLog(nullptr, "%s bufferedLog", testChars);
    EXPECT_TRUE(ppLog != nullptr);
    CRilLog::BufferedLogFlash(testCategory, CRilLog::E_RIL_VERBOSE_LOG, ppLog);

    for (auto logLevel : tetsLogLevel) {
        CRilLog::Log(logLevel, testFormat, testChars, logLevel, testChars);
        CRilLog::Log(logLevel, testChars);
        CRilLog::Log(testCategory, logLevel, testFormat, testChars, logLevel, testChars);
        CRilLog::Log(testCategory, logLevel, testChars);
    }

    CRilLog::DumpResetLog("SitrilTestUtil");

    string retString = CRilLog::GetParentLogPath();
    EXPECT_TRUE(TextUtils::IsEmpty(retString) == false);

    // RilLogCapture
    RilLogCapture *ptr = new RilLogCapture();
    if (ptr) {
        delete ptr;
    }
}

TEST_P(SitrilTestUtil, networkUtil) {
    string retString;
    int retInt;
    bool retBool;

    // check function sall be implemeted later if it is needed.

    // NetworkUtils
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(ACCESS_NETWORK_UNKNOWN);
    EXPECT_EQ(RADIO_TECH_UNKNOWN, retInt);
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(ACCESS_NETWORK_GERAN);
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(ACCESS_NETWORK_UTRAN);
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(ACCESS_NETWORK_EUTRAN);
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(ACCESS_NETWORK_CDMA2000);
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(ACCESS_NETWORK_NGRAN);
    retInt = NetworkUtils::getRadioTechnologyFromAccessNetwork(200);

    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(RIL_RAN_UNKNOWN);
    EXPECT_EQ(RADIO_TECH_UNKNOWN, retInt);
    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(RIL_RAN_GERAN);
    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(RIL_RAN_UTRAN);
    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(RIL_RAN_EUTRAN);
    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(RIL_RAN_NGRAN);
    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(RIL_RAN_CDMA2000);
    retInt = NetworkUtils::getRadioTechnologyFromRadioAccessNetwork(200);

    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_GPRS);
    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_UMTS);
    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_IS95A);
    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_LTE);
    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_IWLAN);
    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_NR);
    retInt = NetworkUtils::getRadioTechnologyToAccessNetworkType(200);

    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_UNKNOWN);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_GPRS);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_EDGE);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_UMTS);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_IS95A);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_IS95B);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_1xRTT);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_EVDO_0);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_EVDO_A);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_HSDPA);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_HSUPA);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_HSPA);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_EVDO_B);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_EHRPD);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_LTE);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_HSPAP);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_GSM);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_TD_SCDMA);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_IWLAN);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_LTE_CA);
    retString = NetworkUtils::getRadioTechnologyString(RADIO_TECH_NR);
    retString = NetworkUtils::getRadioTechnologyString(200);

    retString = NetworkUtils::getRegStateString(RIL_NOT_REG_AND_NOT_SEARCHING);
    retString = NetworkUtils::getRegStateString(RIL_REG_HOME);
    retString = NetworkUtils::getRegStateString(RIL_NOT_REG_AND_SEARCHING);
    retString = NetworkUtils::getRegStateString(RIL_REG_DENIED);
    retString = NetworkUtils::getRegStateString(RIL_UNKNOWN);
    retString = NetworkUtils::getRegStateString(RIL_REG_ROAMING);
    retString = NetworkUtils::getRegStateString(RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING);
    retString = NetworkUtils::getRegStateString(RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_SEARCHING);
    retString = NetworkUtils::getRegStateString(RIL_REG_DENIED_AND_EMERGENCY_AVAILABLE);
    retString = NetworkUtils::getRegStateString(RIL_UNKNOWN_AND_EMERGENCY_AVAILABLE);
    retString = NetworkUtils::getRegStateString(200);

    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_UNKNOWN);
    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_GERAN);
    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_UTRAN);
    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_EUTRAN);
    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_CDMA2000);
    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_IWLAN);
    retString = NetworkUtils::getAccessNewtorkString(ACCESS_NETWORK_NGRAN);
    retString = NetworkUtils::getAccessNewtorkString(200);

    retBool = NetworkUtils::isInService(RIL_REG_HOME);
    retBool = NetworkUtils::isEmergencyOnly(RIL_NOT_REG_AND_EMERGENCY_AVAILABLE_AND_NOT_SEARCHING);
    retBool = NetworkUtils::isGsm(RADIO_TECH_GPRS);
    retBool = NetworkUtils::isCdma(RADIO_TECH_IS95A);

    retInt = NetworkUtils::fetchMncNoEncoding("450001");

    retString = NetworkUtils::addLteEnhancedAvailableToAlphaTag("test...1", 1);

    retInt = NetworkUtils::getDefaultDataSubscription();
    NetworkUtils::setDefaultDataSubscription(retInt);

    retInt = GetOcNameByMccMnc("450001");
}

TEST_P(SitrilTestUtil, radioConvUtil) {
    int retInt;
    bool retBool;

    // check function sall be implemeted later if it is needed.

    // for radio_conv_utils.h
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_GSM_WCDMA);
    EXPECT_EQ(PREF_NET_TYPE_GSM_WCDMA, retInt);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_GSM_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_WCDMA_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_GSM_WCDMA_AUTO);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_CDMA_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_EVDO_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_GSM_WCDMA_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_LTE_CDMA_EVDO);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_LTE_GSM_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_LTE_CMDA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_LTE_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_LTE_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_GSM);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_WCDMA_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_ONLY);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_LTE_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_LTE_CDMA_GSM_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_LTE_CMDA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_LTE_WCDMA);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_GSM);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_TD_SCDMA_WCDMA_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_TD_SCDMA_GSM_WCDMA_LTE);
    retInt = RadioConvUtils::getNetworkTypeFromRaf(NETWORK_TYPE_BITMAP_NR_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA);

    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_GSM_WCDMA);
    EXPECT_EQ(NETWORK_TYPE_BITMAP_GSM_WCDMA, retInt);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_GSM_ONLY);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_GSM_WCDMA_AUTO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_CDMA_ONLY);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_EVDO_ONLY);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_LTE_CDMA_EVDO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_LTE_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_LTE_ONLY);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_LTE_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_ONLY);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_LTE);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_GSM);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_GSM_LTE);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_WCDMA_LTE);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_LTE);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_GSM_WCDMA_CDMA_EVDO_AUTO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_ONLY);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_CDMA_EVDO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_CDMA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_TDSCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_TDSCDMA_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_TDSCDMA_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_NR_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_CDMA);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_CDMA_NO_EVDO);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_CDMA_EVDO_LTE);
    retInt = RadioConvUtils::getRafFromNetworkType(PREF_NET_TYPE_TD_SCDMA_EVDO_NO_CDMA);

    RadioConvUtils::printSupportedNetworkTypeBitmap();

    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_GPRS);
    EXPECT_EQ(RIL_CELL_INFO_TYPE_GSM, retInt);
    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_UMTS);
    EXPECT_EQ(RIL_CELL_INFO_TYPE_WCDMA, retInt);
    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_IS95A);
    EXPECT_EQ(RIL_CELL_INFO_TYPE_CDMA, retInt);
    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_LTE);
    EXPECT_EQ(RIL_CELL_INFO_TYPE_LTE, retInt);
    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_TD_SCDMA);
    EXPECT_EQ(RIL_CELL_INFO_TYPE_TD_SCDMA, retInt);
    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_NR);
    EXPECT_EQ(RIL_CELL_INFO_TYPE_NR, retInt);
    retInt = RadioConvUtils::getCellInfoTypeRadioTechnology(RADIO_TECH_UNKNOWN);

    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_GPRS);
    EXPECT_EQ(ACCESS_NETWORK_GERAN, retInt);
    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_UMTS);
    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_IS95A);
    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_LTE);
    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_IWLAN);
    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_NR);
    retInt = RadioConvUtils::getRadioTechnologyToAccessNetworkType(RADIO_TECH_UNKNOWN);

    retBool = RadioConvUtils::isServiceTypeCfQuery(SS_CFU, SS_INTERROGATION);
    retBool = RadioConvUtils::isServiceTypeCfQuery(SS_CF_ALL_CONDITIONAL, SS_INTERROGATION);
    retBool = RadioConvUtils::isServiceTypeCfQuery(SS_CFU, SS_ACTIVATION);

    // Cell Identity
    RIL_CellIdentityOperatorNames opNameSrc = {"test1", "test2"};
    RIL_CellIdentityOperatorNames opNamedst = {"test3", "test4"};

    RIL_OptionalCsgInfo csgInfo = {{false, "test", 1}};

    RIL_CellIdentityGsm_v12 cidGsmV12 = {1, 1, 1, 1, 1, 1};
    RIL_CellIdentityGsm_V1_2 cidGsmV1_2 = {1, 1, 1, 1, 1, 1, opNameSrc};
    RIL_CellIdentityGsm_V1_5 cidGsmV1_5 = {1, 1, 1, 1, 1, 1, opNameSrc, 0, nullptr};
    RIL_CellIdentityCdma cidCdma = {1, 1, 1, 1, 1};
    RIL_CellIdentityCdma_V1_2 cidCdmaV1_2 = {1, 1, 1, 1, 1, opNameSrc};
    RIL_CellIdentityTdscdma cidTdscdma = {1, 1, 1, 1, 1};
    RIL_CellIdentityTdscdma_V1_2 cidTdscdmaV1_2 = {1, 1, 1, 1, 1, 1, opNameSrc};
    RIL_CellIdentityTdscdma_V1_5 cidTdscdmaV1_5 = {1, 1, 1, 1, 1, 1, opNameSrc, 0, nullptr, csgInfo};
    RIL_CellIdentityWcdma_v12 cidWcdmaV12 = {1, 1, 1, 1, 1, 1};
    RIL_CellIdentityWcdma_V1_2 cidWcdmaV1_2 = {1, 1, 1, 1, 1, 1, opNameSrc};
    RIL_CellIdentityWcdma_V1_5 cidWcdmaV1_5 = {1, 1, 1, 1, 1, 1, opNameSrc, 0, nullptr, csgInfo};
    RIL_CellIdentityLte_v12 cidLteV12 = {1, 1, 1, 1, 1, 1};
    RIL_CellIdentityLte_V1_2 cidLteV1_2 = {1, 1, 1, 1, 1, 1, opNameSrc, 0};
    RIL_CellIdentityLte_V1_5 cidLteV1_5 = {1, 1, 1, 1, 1, 1, opNameSrc, 0, 0, nullptr, csgInfo, 0, {EUTRAN_BAND_1,}};
    RIL_CellIdentityNr_V1_4 cidNrV1_4 = {1, 1, 1, 1, 1, 1, opNameSrc};
    RIL_CellIdentityNr_V1_5 cidNrV1_5 = {1, 1, 1, 1, 1, 1, opNameSrc, 0, nullptr, 0, {NGRAN_BAND_1,}};

    RIL_CellIdentity_v16 cidV16 = {RIL_CELL_INFO_TYPE_GSM, {cidGsmV12}};
    RIL_CellIdentity_V1_2 cidV1_2 = {RIL_CELL_INFO_TYPE_GSM, {cidGsmV1_2}};
    RIL_CellIdentity_V1_5 cidV1_5 = {RIL_CELL_INFO_TYPE_GSM, {cidGsmV1_5}};

    RIL_VoiceRegistrationStateResponse voiceReg = {RIL_REG_HOME, RADIO_TECH_GSM, 1, 1, 1, 1, 1, cidV16};
    RIL_VoiceRegistrationStateResponse_V1_2 voiceRegV1_2 = {RIL_REG_HOME, RADIO_TECH_GSM, 1, 1, 1, 1, 1, cidV1_2};
    RIL_DataRegistrationStateResponse dataReg = {RIL_REG_HOME, RADIO_TECH_GPRS, 0, 0, cidV16};
    RIL_DataRegistrationStateResponse_V1_2 dataRegV1_2 = {RIL_REG_HOME, RADIO_TECH_GPRS, 0, 0, cidV1_2};
    RIL_DataRegistrationStateResponse_V1_4 dataRegV1_4 = {RIL_REG_HOME, RADIO_TECH_GPRS, 0, 0, cidV1_2, {false, false}, {false, false, false}};
    RIL_RegStateResult regResult = {RIL_REG_HOME, RADIO_TECH_GSM, REG_FAIL_CAUSE_NONE, cidV1_5, "45000", {{0, 0, PRL_IND_NOT_IN_PRL, 0}}};

    RIL_SignalStrength_v10 ssV10 = {{0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0}};
    RIL_SignalStrength_V1_2 ssV1_2 = {{0, 0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0}, {0, 0, 0, 0}};
    RIL_SignalStrength_V1_4 ssV1_4 = {{0, 0, 0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

    RIL_CellInfoGsm_v12 cInfoGsmV12 = {cidGsmV12, {0, 0, 0}};
    RIL_CellInfoGsm_V1_2 cInfoGsmV1_2 = {cidGsmV1_2, {0, 0, 0}};
    RIL_CellInfoGsm_V1_5 cInfoGsmV1_5 = {cidGsmV1_5, {0, 0, 0}};
    RIL_CellInfoWcdma_v12 cInfoWcdmaV12 = {cidWcdmaV12, {0, 0}};
    RIL_CellInfoWcdma_V1_2 cInfoWcdmaV1_2 = {cidWcdmaV1_2, {0, 0, 0, 0}};
    RIL_CellInfoCdma cInfoCdma = {cidCdma, {0, 0}, {0, 0, 0}};
    RIL_CellInfoCdma_V1_2 cInfoCdmaV1_2 = {cidCdmaV1_2, {0, 0}, {0, 0, 0}};
    RIL_CellInfoTdscdma cInfoTdscdma = {cidTdscdma, {0}};
    RIL_CellInfoTdscdma_V1_2 cInfoTdscdmaV1_2 = {cidTdscdmaV1_2, {0, 0, 0}};
    RIL_CellInfoLte_v12 cInfoLteV12 = {cidLteV12, {0, 0, 0, 0, 0, 0}};
    RIL_CellInfoLte_V1_2 cInfoLteV1_2 = {cidLteV1_2, {0, 0, 0, 0, 0, 0}};
    RIL_CellInfoLte_V1_4 cInfoLteV1_4 = {cInfoLteV1_2, {false}};

    RIL_CellInfo_v12 cInfoV12 = {RIL_CELL_INFO_TYPE_GSM, 0, RIL_TIMESTAMP_TYPE_UNKNOWN, 0, {cInfoGsmV12}};
    RIL_CellInfo_V1_2 cInfoV1_2 = {RIL_CELL_INFO_TYPE_GSM, 0, RIL_TIMESTAMP_TYPE_UNKNOWN, 0, {cInfoGsmV1_2}, NONE};
    RIL_CellInfo_V1_4 cInfoV1_4 = {RIL_CELL_INFO_TYPE_GSM, 0, RIL_TIMESTAMP_TYPE_UNKNOWN, 0, {cInfoGsmV1_2}, NONE};
    RIL_CellInfo_V1_5 cInfoV1_5 = {RIL_CELL_INFO_TYPE_GSM, 0, RIL_TIMESTAMP_TYPE_UNKNOWN, 0, {cInfoGsmV1_5}, NONE};
    list<RIL_CellInfo_v12> cInfoV12List;
    list<RIL_CellInfo_V1_2> cInfoV1_2List;
    list<RIL_CellInfo_V1_4> cInfoV1_4List;
    cInfoV1_4List.push_back(cInfoV1_4);

    RadioConvUtils::convertCellIdentityGsm(cidGsmV12, cidGsmV1_2);
    RadioConvUtils::convertCellIdentityCdma(cidCdma, cidCdmaV1_2);
    RadioConvUtils::convertCellIdentityWcdma(cidWcdmaV12, cidWcdmaV1_2);
    RadioConvUtils::convertCellIdentityLte(cidLteV12, cidLteV1_2);
    RadioConvUtils::convertCellIdentityTdscdma(cidTdscdma, cidTdscdmaV1_2);

    // 1.5
    RadioConvUtils::convertCellIdentity(cidV1_2, cidV1_5);
    RadioConvUtils::convertCellIdentity(cidV16, cidV1_5);
    RadioConvUtils::convertCellIdentity(cidV16, cidV1_2);
    RadioConvUtils::fillOperatorName(opNamedst, opNameSrc);
    RadioConvUtils::convertCellIdentityGsm(cidGsmV1_2, cidGsmV1_5);
    RadioConvUtils::convertCellIdentityGsm(cidGsmV12, cidGsmV1_5);
    RadioConvUtils::convertCellIdentityWcdma(cidWcdmaV1_2, cidWcdmaV1_5);
    RadioConvUtils::convertCellIdentityWcdma(cidWcdmaV12, cidWcdmaV1_5);
    RadioConvUtils::convertCellIdentityTdscdma(cidTdscdmaV1_2, cidTdscdmaV1_5);
    RadioConvUtils::convertCellIdentityTdscdma(cidTdscdma, cidTdscdmaV1_5);
    RadioConvUtils::convertCellIdentityLte(cidLteV1_2, cidLteV1_5);
    RadioConvUtils::convertCellIdentityLte(cidLteV12, cidLteV1_5);
    RadioConvUtils::convertCellIdentityNr(cidNrV1_4, cidNrV1_5);

    // Reg result
    RadioConvUtils::convertDataRegistrationStateResult(dataRegV1_4, regResult);
    RadioConvUtils::convertDataRegistrationStateResult(dataRegV1_2, dataRegV1_4);
    RadioConvUtils::convertDataRegistrationStateResult(dataReg, dataRegV1_2);
    RadioConvUtils::convertVoiceRegistrationStateResult(voiceRegV1_2, regResult);
    RadioConvUtils::convertVoiceRegistrationStateResult(voiceReg, voiceRegV1_2);

    // Signal strength
    RadioConvUtils::convertSignalStrengthResult(ssV10, ssV1_2);
    RadioConvUtils::convertSignalStrengthResult(ssV10, ssV1_4);
    RadioConvUtils::convertSignalStrengthResult(ssV1_2, ssV1_4);

    // Cell Info
    RadioConvUtils::convertCellInfoGsm(cInfoGsmV12, cInfoGsmV1_2);
    RadioConvUtils::convertCellInfoCdma(cInfoCdma, cInfoCdmaV1_2);
    RadioConvUtils::convertCellInfoWcdma(cInfoWcdmaV12, cInfoWcdmaV1_2);
    RadioConvUtils::convertCellInfoTdscdma(cInfoTdscdma, cInfoTdscdmaV1_2);
    RadioConvUtils::convertCellInfoLte(cInfoLteV12, cInfoLteV1_2);
    RadioConvUtils::convertCellInfoLte(cInfoLteV12, cInfoLteV1_4);
    RadioConvUtils::convertCellInfoLte(cInfoLteV1_2, cInfoLteV1_4);
    RadioConvUtils::convertCellInfo(cInfoV12, cInfoV1_2);
    RadioConvUtils::convertCellInfo(cInfoV12, cInfoV1_4);
    RadioConvUtils::convertCellInfo(cInfoV1_2, cInfoV1_4);
    RadioConvUtils::convertCellInfoList(cInfoV12List, cInfoV1_4List);
    RadioConvUtils::convertCellInfoList(cInfoV1_2List, cInfoV1_4List);

    // 1.5
    RadioConvUtils::convertCellInfo(cInfoV1_4, cInfoV1_5);

    // Physical channel config
    RIL_PhysicalChannelConfig pcCfg = {NONE, 0};
    RIL_PhysicalChannelConfig_V1_4 pcCfgV1_4_src = {0, 0, 0, 0, {0}, 0, nullptr, 0};
    RIL_PhysicalChannelConfig_V1_4 pcCfgV1_4_dst = {0, 0, 0, 0, {0}, 0, nullptr, 0};
    RadioConvUtils::convertPhysicalChannelConfig(pcCfg, pcCfgV1_4_src);
    RadioConvUtils::dupPhysicalChannelConfig(pcCfgV1_4_dst, pcCfgV1_4_src);

    // Data Call list
    RIL_DataProfileInfo dpi = {0, nullptr, nullptr, 0, nullptr, nullptr, 0, 0, 0, 0, 0};
    RIL_DataProfileInfo_v15 dpiV15_src, dpiV15_dst;
    dpiV15_dst = {0, nullptr, nullptr, nullptr, 0, nullptr, nullptr, 0, 0, 0, 0, 0, 0, 0, 0, nullptr, nullptr};
    RIL_DataProfileInfo_V1_4 dpiV1_4 = {0, nullptr, 0, 0, 0, nullptr, nullptr, 0, 0, 0, 0, false, 0, 0, 0, false, false};
    RIL_DataProfileInfo_V1_5 dpiV1_5_src, dpiV1_5_dst;
    dpiV1_5_src = {DEFAULT, nullptr, PDP_PROTOCOL_TYPE_IP, PDP_PROTOCOL_TYPE_IP, NO_PAP_NO_CHAP,
                   nullptr, nullptr, COMMON, 0, 0, 0, false, 0, 0, 0, 0, false, false};
    RIL_LinkAddress la_src, la_dst;
    la_src = {nullptr, 0, 0, 0};

    retBool = RadioConvUtils::CopyDataProfileInfo(dpiV1_5_dst, dpiV1_5_src);
    retBool = RadioConvUtils::CopyDataProfileInfo(dpiV1_5_dst, dpiV1_4);
    retBool = RadioConvUtils::CopyDataProfileInfo(dpiV1_5_dst, dpiV15_src);
    retBool = RadioConvUtils::CopyDataProfileInfo(dpiV15_dst, dpiV15_src);
    retBool = RadioConvUtils::CopyDataProfileInfo(dpiV15_dst, dpi);
    RadioConvUtils::FreeDataProfileInfo(dpiV1_4);
    RadioConvUtils::FreeDataProfileInfo(dpiV1_5_dst);
    RadioConvUtils::CopyLinkAddress(la_dst, la_src);
    RadioConvUtils::FreeLinkAddress(la_dst);

    // RIL_Data_Call_Response_v11 dcRsp = {0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0};
    // RIL_Data_Call_Response_V1_4 dcRspV1_4 = {0, 0, 0, 0, 0, nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0}; // not used, no need to test
    RIL_SetupDataCallInfo_V1_4 sdcV1_4 = {0, dpiV1_4, false, 0, nullptr, nullptr};
    RIL_SetupDataCallInfo_V1_5 sdcV1_5 = {0, dpiV1_5_src, false, 0, 0, nullptr, nullptr};

    RadioConvUtils::convertSetupDataCallInfo(sdcV1_5, sdcV1_4);
    // RadioConvUtils::convertDataCall(dcRsp, dcRspV1_4); // not used, removed
    // RadioConvUtils::convertDataCallList(nullptr, nullptr, 0); // not used, removed

    //Network
    RIL_RadioAccessSpecifier ras = {RIL_RAN_GERAN, 0, {{GERAN_BAND_T380}}, 0, {0,}};
    RIL_RadioAccessSpecifier_V1_5 rasV1_5;
    RadioConvUtils::convertRadioAccessSpecifiers(rasV1_5, ras);
}

TEST_P(SitrilTestUtil, radioHalUtil) {
    // for radio_hal_utils.h
    // need to implement
}
