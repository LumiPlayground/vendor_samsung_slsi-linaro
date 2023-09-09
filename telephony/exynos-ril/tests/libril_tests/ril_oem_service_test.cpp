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
 * ril_oem_service_test.cpp
 *
 *  Created on: 2021. 7. 20.
 */
#include <gtest/gtest.h>
#include <vendor/samsung_slsi/telephony/hardware/radio/1.1/IOemSamsungslsi.h>
#include "slsi/ril_oem_service.h"
#include <telephony/ril_ext.h>
#include "ril_internal.h"

#include "ril_service_test.h"

using namespace android::hardware::radio;
using namespace android::hardware::radio::V1_0;
using namespace vendor::samsung_slsi::telephony::hardware::radio;
using namespace vendor::samsung_slsi::telephony::hardware::radio::V1_0;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Void;
using android::sp;

using libriltest::GetRandomSerialNumber;

typedef int (*RadioResponseHandler)(int, int, int, RIL_Errno, void *, size_t);
typedef int (*RadioIndicationHandler)(int, int, int, RIL_Errno, void *, size_t);

TEST(ril_oem_service, vendor_radio_response_v1_0) {
    vendor::radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        vendor::radio::queryCOLPResponse,
        vendor::radio::queryCOLRResponse,
        vendor::radio::iccGetAtrResponse,
        vendor::radio::sendUSSDWithDcsResponse,
        vendor::radio::setEmcStatusResponse,
        vendor::radio::setFemtoCellSearchResponse,
        vendor::radio::setCdmaHybridModeResponse,
        vendor::radio::getCdmaHybridModeResponse,
        vendor::radio::queryBplmnSearchResponse,
        vendor::radio::setNetworkSelectionModeManualResponse,
        vendor::radio::dialWithCallTypeResponse,
        vendor::radio::setVoiceOperationResponse,
        vendor::radio::setBarringPasswordOverMmiResponse,
        vendor::radio::setDualNetworkTypeAndAllowDataResponse,
        vendor::radio::setDsNetworkTypeResponse,
        vendor::radio::deactivateDataCallWithReasonResponse,
        vendor::radio::getSimLockStatusResponse,
        vendor::radio::sendVsimNotificationResponse,
        vendor::radio::sendVsimOperationResponse,
        vendor::radio::sendRequestRawResponse,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_oem_service, vendor_radio_indication_v1_0) {
    vendor::radio::impl::legacy::initRadios();

    RadioIndicationHandler testVector[] = {
        vendor::radio::suppSvcReturnResult,
        vendor::radio::callPresentInd,
        vendor::radio::wbAmrReportInd,
        vendor::radio::vsimOperationInd,
        vendor::radio::nasTimerStatusInd,
        vendor::radio::emergencyActInd,
        vendor::radio::onUssdWithDcsInd,
        vendor::radio::volteAvailableInfoInd,
        vendor::radio::iccIdInfoInd,
        vendor::radio::emergencySupportRatModeInd,
        vendor::radio::ussdCanceledInd,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        RadioIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_UNSOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_oem_service, vendor_radio_response_v1_1) {
    vendor::radio::impl::legacy::initRadios();

    RadioResponseHandler testVector[] = {
        vendor::radio::setActivateVsimResponse,
        vendor::radio::setEndcModeResponse,
        vendor::radio::getEndcModeResponse,
        vendor::radio::getSmsStorageOnSimResponse,
        vendor::radio::setNrModeResponse,
        vendor::radio::getNrModeResponse,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_SOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_oem_service, vendor_radio_indication_v1_1) {
    vendor::radio::impl::legacy::initRadios();

    RadioIndicationHandler testVector[] = {
        vendor::radio::currentPhysicalChannelConfigInd,
        vendor::radio::endcCapabilityInd,
        vendor::radio::simTrayStatusChangedInd,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        RadioIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(0, RESPONSE_UNSOLICITED, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
extern RIL_RadioFunctions *s_vendorFunctions;
static VendorFunctionExcutor sVendorFunctionExcutor;

#if defined(ANDROID_MULTI_SIM)
static void onTestRequest(int request, void *data, size_t datalen, RIL_Token t,
        RIL_SOCKET_ID socket_id) {
    sVendorFunctionExcutor.onRequest(request, data, datalen, t, socket_id);
}
static RIL_RadioState onTestStateRequest(RIL_SOCKET_ID socket_id) {
    return sVendorFunctionExcutor.onStateRequest(socket_id);
}
#else
static void onTestRequest(int request, void *data, size_t datalen, RIL_Token t) {
    sVendorFunctionExcutor.onRequest(request, data, datalen, t, RIL_SOCKET_1);
}
static RIL_RadioState onTestStateRequest() {
    return sVendorFunctionExcutor.onStateRequest(RIL_SOCKET_1);
}
#endif

static RIL_RadioFunctions sTestVendorFunctions = {
    RIL_VERSION,
    onTestRequest,
    onTestStateRequest,
    nullptr,
    nullptr,
    nullptr,
};

static sp<V1_1::IOemSamsungslsi> setupRadio(OnRequestListener *listener = nullptr) {
    vendor::radio::impl::legacy::initRadios();
    sp<V1_1::IOemSamsungslsi> radio = vendor::radio::impl::legacy::getRadio(0);
    EXPECT_FALSE(radio.get() == nullptr);

    s_vendorFunctions = &sTestVendorFunctions;
    sVendorFunctionExcutor.setOnRequestListener(listener);

    return radio;
}

static void tearDown(sp<V1_1::IOemSamsungslsi>& radio) {
    radio = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
}

TEST(ril_oem_service, queryCOLP) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::queryCOLP");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_COLP);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->queryCOLP(serial);
    tearDown(radio);
}

TEST(ril_oem_service, queryCOLR) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::queryCOLR");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_COLR);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->queryCOLR(serial);
    tearDown(radio);
}

TEST(ril_oem_service, iccGetAtr) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::iccGetAtr");
            EXPECT_EQ(request, RIL_REQUEST_SIM_GET_ATR);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->iccGetAtr(serial);
    tearDown(radio);
}

TEST(ril_oem_service, sendUSSDWithDcs) {
    static hidl_string dcs = "dcs1234";
    static hidl_string ussd = "ussd1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::sendUSSDWithDcs");
            EXPECT_EQ(request, RIL_REQUEST_SEND_ENCODED_USSD);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE(dcs == ((char **)data)[0]);
            EXPECT_TRUE(ussd == ((char **)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->sendUSSDWithDcs(serial, dcs, ussd);
    tearDown(radio);
}

TEST(ril_oem_service, setPreferredUplmn) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int, void *, size_t, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setPreferredUplmn");
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setPreferredUplmn(serial, 0, 0, 0, "empty");
    tearDown(radio);
}

TEST(ril_oem_service, getPreferredUplmn) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int, void *, size_t, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::getPreferredUplmn");
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->getPreferredUplmn(serial);
    tearDown(radio);
}

TEST(ril_oem_service, setEmcStatus) {
    static int status = 1;
    static int rat = 10;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setEmcStatus");
            EXPECT_EQ(request, RIL_REQUEST_SET_EMERGENCY_CALL_STATUS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 2);
            EXPECT_EQ(status, ((int *)data)[0]);
            EXPECT_EQ(rat, ((int *)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setEmcStatus(serial, status, rat);
    tearDown(radio);
}

TEST(ril_oem_service, setFemtoCellSearch) {
    static int searchMode = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setFemtoCellSearch");
            EXPECT_EQ(request, RIL_REQUEST_SET_FEMTO_CELL_SRCH);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(searchMode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setFemtoCellSearch(serial, searchMode);
    tearDown(radio);
}

TEST(ril_oem_service, setCdmaHybridMode) {
    static int hybridMode = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setCdmaHybridMode");
            EXPECT_EQ(request, RIL_REQUEST_SET_CDMA_HYBRID_MODE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(hybridMode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setCdmaHybridMode(serial, hybridMode);
    tearDown(radio);
}

TEST(ril_oem_service, getCdmaHybridMode) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::getCdmaHybridMode");
            EXPECT_EQ(request, RIL_REQUEST_GET_CDMA_HYBRID_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->getCdmaHybridMode(serial);
    tearDown(radio);
}

TEST(ril_oem_service, queryBplmnSearch) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::queryBplmnSearch");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_BPLMN_SEARCH);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->queryBplmnSearch(serial);
    tearDown(radio);
}

TEST(ril_oem_service, setNetworkSelectionModeManual) {
    static hidl_string operatorNumeric = "00101";
    static hidl_string rat = "14";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setNetworkSelectionModeManual");
            EXPECT_EQ(request, RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_WITH_RAT);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE(operatorNumeric == ((char **)data)[0]);
            EXPECT_TRUE(rat == ((char **)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setNetworkSelectionModeManual(serial, operatorNumeric, rat);
    tearDown(radio);
}

TEST(ril_oem_service, dialWithCallType) {
    static Dial dialInfo = {
        "01012345678",
        Clir::DEFAULT,
        {},
    };
    static int callType = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::dialWithCallType");
            EXPECT_EQ(request, RIL_REQUEST_DIAL_WITH_CALL_TYPE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_Dial_Ext));
            const RIL_Dial_Ext& dial = *(RIL_Dial_Ext *)data;
            EXPECT_TRUE(dialInfo.address == dial.address);
            EXPECT_EQ((int)dialInfo.clir, dial.clir);
            EXPECT_EQ(callType, dial.callType);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->dialWithCallType(serial, dialInfo, callType);
    tearDown(radio);
}

TEST(ril_oem_service, setVoiceOperation) {
    static int prefMode = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setVoiceOperation");
            EXPECT_EQ(request, RIL_REQUEST_SET_VOICE_OPERATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(prefMode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setVoiceOperation(serial, prefMode);
    tearDown(radio);
}

TEST(ril_oem_service, setBarringPasswordOverMmi) {
    static hidl_string facility = "facility";
    static hidl_string oldPassword = "oldPassword";
    static hidl_string newPassword = "newPassword";
    static hidl_string newPasswordAgain = "newPasswordAgain";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setBarringPasswordOverMmi");
            EXPECT_EQ(request, RIL_REQUEST_CHANGE_BARRING_PASSWORD_OVER_MMI);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 4);
            EXPECT_TRUE(facility == ((char **)data)[0]);
            EXPECT_TRUE(oldPassword == ((char **)data)[1]);
            EXPECT_TRUE(newPassword == ((char **)data)[2]);
            EXPECT_TRUE(newPasswordAgain == ((char **)data)[3]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setBarringPasswordOverMmi(serial, facility, oldPassword, newPassword,
            newPasswordAgain);
    tearDown(radio);
}

TEST(ril_oem_service, setDualNetworkTypeAndAllowData) {
    static int networkTypeForPrimary = 9;
    static int networkTypeForSecondary = 0;
    static int allowDataForPrimary = 1;
    static int allowDataForSecondary = 0;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setDualNetworkTypeAndAllowData");
            EXPECT_EQ(request, RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 4);
            EXPECT_EQ(networkTypeForPrimary, ((int *)data)[0]);
            EXPECT_EQ(networkTypeForSecondary, ((int *)data)[1]);
            EXPECT_EQ(allowDataForPrimary, ((int *)data)[2]);
            EXPECT_EQ(allowDataForSecondary, ((int *)data)[3]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setDualNetworkTypeAndAllowData(serial, networkTypeForPrimary, networkTypeForSecondary,
            allowDataForPrimary, allowDataForSecondary);
    tearDown(radio);
}

TEST(ril_oem_service, setDsNetworkType) {
    static int networkType = 9;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::setDsNetworkType");
            EXPECT_EQ(request, RIL_REQUEST_SET_DS_NETWORK_TYPE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(networkType, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setDsNetworkType(serial, networkType);
    tearDown(radio);
}

TEST(ril_oem_service, deactivateDataCallWithReason) {
    static int cid = 9;
    static int reason = 0;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::deactivateDataCallWithReason");
            EXPECT_EQ(request, RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_EQ(cid, std::stoi(((char **)data)[0]));
            EXPECT_EQ(reason, std::stoi(((char **)data)[1]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->deactivateDataCallWithReason(serial, cid, reason);
    tearDown(radio);
}

TEST(ril_oem_service, emulateInd) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int, void *, size_t, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::emulateInd");
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->emulateInd(serial, 0, 0);
    tearDown(radio);
}

TEST(ril_oem_service, getSimLockStatus) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::getSimLockStatus");
            EXPECT_EQ(request, RIL_REQUEST_GET_SIM_LOCK_STATUS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->getSimLockStatus(serial);
    tearDown(radio);
}

TEST(ril_oem_service, sendVsimNotification) {
    static int transactionId = 9;
    static int eventId = 0;
    static int simType = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::sendVsimNotification");
            EXPECT_EQ(request, RIL_LOCAL_REQUEST_VSIM_NOTIFICATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 3);
            EXPECT_EQ(transactionId, ((int *)data)[0]);
            EXPECT_EQ(eventId, ((int *)data)[1]);
            EXPECT_EQ(simType, ((int *)data)[2]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->sendVsimNotification(serial, transactionId, eventId, simType);
    tearDown(radio);
}

TEST(ril_oem_service, sendVsimOperation) {
    static int transactionId = 9;
    static int eventId = 0;
    static int result = 1;
    static hidl_string tpdu = "0123456789ABCDEF";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IOemSamsungslsi::sendVsimOperation");
            EXPECT_EQ(request, RIL_LOCAL_REQUEST_VSIM_OPERATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_VsimOperationEvent));
            const RIL_VsimOperationEvent& event = *(RIL_VsimOperationEvent *)data;
            EXPECT_EQ(transactionId, event.transaction_id);
            EXPECT_EQ(eventId, event.eventId);
            EXPECT_EQ(result, event.result);
            EXPECT_EQ(tpdu.size(), (unsigned long)/* NEED TO CHECK */ event.data_length);
            EXPECT_TRUE(tpdu == event.data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->sendVsimOperation(serial, transactionId, eventId, result, tpdu);
    tearDown(radio);
}

TEST(ril_oem_service, setActivateVsim) {
    static int slot = 0;
    static hidl_string iccid = "123456789";
    static hidl_string imsi = "0010145865455";
    static hidl_string hplmn = "00101";
    static int vsimState = 1;
    static int vsimCardType = 2;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IOemSamsungslsi::setActivateVsim");
            EXPECT_EQ(request, RIL_REQUEST_SET_ACTIVATE_VSIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SetActivateVsim));
            const RIL_SetActivateVsim& event = *(RIL_SetActivateVsim *)data;
            EXPECT_EQ(slot, event.simSlot);
            EXPECT_TRUE(iccid == event.iccid);
            EXPECT_TRUE(imsi == event.imsi);
            EXPECT_TRUE(hplmn == event.hplmn);
            EXPECT_EQ(vsimState, event.vsimState);
            EXPECT_EQ(vsimCardType, event.vsimCardType);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setActivateVsim(serial, slot, iccid, imsi, hplmn, vsimState, vsimCardType);
    tearDown(radio);
}

TEST(ril_oem_service, setEndcMode) {
    static bool enable = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IOemSamsungslsi::setEndcMode");
            EXPECT_EQ(request, RIL_REQUEST_SET_ENDC_MODE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(enable, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setEndcMode(serial, enable);
    tearDown(radio);
}

TEST(ril_oem_service, getEndcMode) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IOemSamsungslsi::getEndcMode");
            EXPECT_EQ(request, RIL_REQUEST_GET_ENDC_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->getEndcMode(serial);
    tearDown(radio);
}

TEST(ril_oem_service, getSmsStorageOnSim) {
    static int simId = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IOemSamsungslsi::getSmsStorageOnSim");
            EXPECT_EQ(request, RIL_REQUEST_GET_SMS_STORAGE_ON_SIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(simId, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->getSmsStorageOnSim(serial, simId);
    tearDown(radio);
}

TEST(ril_oem_service, setNrMode) {
    static int mode = 4;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IOemSamsungslsi::setNrMode");
            EXPECT_EQ(request, RIL_REQUEST_SET_NR_MODE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(mode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->setNrMode(serial, mode);
    tearDown(radio);
}

TEST(ril_oem_service, getNrMode) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IOemSamsungslsi::getNrMode");
            EXPECT_EQ(request, RIL_REQUEST_GET_NR_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSamsungslsi> radio = setupRadio(&callback);
    radio->getNrMode(serial);
    tearDown(radio);
}
