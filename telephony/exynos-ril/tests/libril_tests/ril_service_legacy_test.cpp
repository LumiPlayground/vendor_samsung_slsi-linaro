/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <gtest/gtest.h>
#include "ril_service_legacy.h"
#include "ril_service_common.h"
#include "ril_service_test.h"
#include <librilutils/textutils.h>

using namespace android::hardware::radio;
using ::android::hardware::Return;
using ::android::hardware::Status;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Void;
using android::sp;

using libriltest::GetRandomSerialNumber;

TEST(ril_service_legacy, checkReturnStatus) {
    radio::impl::legacy::initRadios();
    int slotId = 0;
    Return<void> ret(Status::fromExceptionCode(Status::EX_TRANSACTION_FAILED));
    radio::impl::legacy::checkReturnStatus(slotId, ret);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
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

static sp<V1_5::IRadio> setupRadio(OnRequestListener *listener = nullptr) {
    radio::impl::legacy::initRadios();
    sp<V1_0::IRadio> radio = radio::impl::legacy::getRadio(0);
    EXPECT_FALSE(radio.get() == nullptr);
    sp<V1_5::IRadio> radioV15 = V1_5::IRadio::castFrom(radio).withDefault(nullptr);
    EXPECT_FALSE(radioV15.get() == nullptr);

    s_vendorFunctions = &sTestVendorFunctions;
    sVendorFunctionExcutor.setOnRequestListener(listener);

    return radioV15;
}

static void tearDown(sp<V1_5::IRadio>& radio) {
    radio = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
}

TEST(ril_service_legacy, getIccCardStatus) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getIccCardStatus");
            EXPECT_EQ(request, RIL_REQUEST_GET_SIM_STATUS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getIccCardStatus(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, supplyIccPinForApp) {
    static const char *pin = "1234";
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::supplyIccPinForApp");
            EXPECT_EQ(request, RIL_REQUEST_ENTER_SIM_PIN);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            size_t size = datalen / sizeof(const char *);
            if (size == 2) {
                EXPECT_TRUE(TextUtils::Equals(pin, ((const char **)data)[0]));
                EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[1]));
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->supplyIccPinForApp(serial, pin, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, supplyIccPukForApp) {
    static const char *puk = "12345678";
    static const char *pin = "1234";
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::supplyIccPukForApp");
            EXPECT_EQ(request, RIL_REQUEST_ENTER_SIM_PUK);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 3);
            size_t size = datalen / sizeof(const char *);
            if (size == 3) {
                EXPECT_TRUE(TextUtils::Equals(puk, ((const char **)data)[0]));
                EXPECT_TRUE(TextUtils::Equals(pin, ((const char **)data)[1]));
                EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[2]));
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->supplyIccPukForApp(serial, puk, pin, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, supplyIccPin2ForApp) {
    static const char *pin = "1234";
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::supplyIccPin2ForApp");
            EXPECT_EQ(request, RIL_REQUEST_ENTER_SIM_PIN2);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            size_t size = datalen / sizeof(const char *);
            if (size == 2) {
                EXPECT_TRUE(TextUtils::Equals(pin, ((const char **)data)[0]));
                EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[1]));
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->supplyIccPin2ForApp(serial, pin, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, supplyIccPuk2ForApp) {
    static const char *puk = "12345678";
    static const char *pin = "1234";
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::supplyIccPuk2ForApp");
            EXPECT_EQ(request, RIL_REQUEST_ENTER_SIM_PUK2);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 3);
            size_t size = datalen / sizeof(const char *);
            if (size == 3) {
                EXPECT_TRUE(TextUtils::Equals(puk, ((const char **)data)[0]));
                EXPECT_TRUE(TextUtils::Equals(pin, ((const char **)data)[1]));
                EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[2]));
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->supplyIccPuk2ForApp(serial, puk, pin, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, changeIccPinForApp) {
    static const char *oldPin = "1234";
    static const char *newPin = "5678";
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::changeIccPinForApp");
            EXPECT_EQ(request, RIL_REQUEST_CHANGE_SIM_PIN);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 3);
            size_t size = datalen / sizeof(const char *);
            if (size == 3) {
                EXPECT_TRUE(TextUtils::Equals(oldPin, ((const char **)data)[0]));
                EXPECT_TRUE(TextUtils::Equals(newPin, ((const char **)data)[1]));
                EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[2]));
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->changeIccPinForApp(serial, oldPin, newPin, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, changeIccPin2ForApp) {
    static const char *oldPin = "1234";
    static const char *newPin = "5678";
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::changeIccPin2ForApp");
            EXPECT_EQ(request, RIL_REQUEST_CHANGE_SIM_PIN2);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 3);
            size_t size = datalen / sizeof(const char *);
            if (size == 3) {
                EXPECT_TRUE(TextUtils::Equals(oldPin, ((const char **)data)[0]));
                EXPECT_TRUE(TextUtils::Equals(newPin, ((const char **)data)[1]));
                EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[2]));
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->changeIccPin2ForApp(serial, oldPin, newPin, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, supplyNetworkDepersonalization) {
    static const char *newPin = "5678";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::supplyNetworkDepersonalization");
            EXPECT_EQ(request, RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 1);
            EXPECT_TRUE(TextUtils::Equals(newPin, ((const char **)data)[0]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->supplyNetworkDepersonalization(serial, newPin);
    tearDown(radio);
}

TEST(ril_service_legacy, getCurrentCalls) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCurrentCalls");
            EXPECT_EQ(request, RIL_REQUEST_GET_CURRENT_CALLS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCurrentCalls(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, dial) {
    static V1_0::Dial testVector = {
        "+821012345678", V1_0::Clir::DEFAULT,
        { {V1_0::UusType::TYPE1_IMPLICIT, V1_0::UusDcs::USP, "0000"}, }
    };

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::dial");
            EXPECT_EQ(request, RIL_REQUEST_DIAL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_Dial));
            RIL_Dial *dial = (RIL_Dial *)data;
            EXPECT_TRUE(testVector.address == dial->address);
            EXPECT_TRUE((int)testVector.clir == dial->clir);
            EXPECT_NE(dial->uusInfo, nullptr);
            EXPECT_EQ((int)dial->uusInfo->uusType, (int)testVector.uusInfo[0].uusType);
            EXPECT_EQ((int)dial->uusInfo->uusDcs, (int)testVector.uusInfo[0].uusDcs);
            EXPECT_EQ((int)dial->uusInfo->uusLength, (int)testVector.uusInfo[0].uusData.size());
            EXPECT_TRUE(dial->uusInfo->uusData == testVector.uusInfo[0].uusData);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->dial(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_legacy, getImsiForApp) {
    static const char *aid = "aid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getImsiForApp");
            EXPECT_EQ(request, RIL_REQUEST_GET_IMSI);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(const char *));
            EXPECT_TRUE(TextUtils::Equals(aid, ((const char **)data)[0]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getImsiForApp(serial, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, hangup) {
    static int gsmIndex = 0;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::hangup");
            EXPECT_EQ(request, RIL_REQUEST_HANGUP);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(gsmIndex, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->hangup(serial, gsmIndex);
    tearDown(radio);
}

TEST(ril_service_legacy, hangupWaitingOrBackground) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::hangupWaitingOrBackground");
            EXPECT_EQ(request, RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->hangupWaitingOrBackground(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, hangupForegroundResumeBackground) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::hangupForegroundResumeBackground");
            EXPECT_EQ(request, RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->hangupForegroundResumeBackground(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, switchWaitingOrHoldingAndActive) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::switchWaitingOrHoldingAndActive");
            EXPECT_EQ(request, RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->switchWaitingOrHoldingAndActive(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, conference) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::conference");
            EXPECT_EQ(request, RIL_REQUEST_CONFERENCE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->conference(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, rejectCall) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::rejectCall");
            EXPECT_EQ(request, RIL_REQUEST_UDUB);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->rejectCall(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getLastCallFailCause) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getLastCallFailCause");
            EXPECT_EQ(request, RIL_REQUEST_LAST_CALL_FAIL_CAUSE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getLastCallFailCause(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getSignalStrength) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getSignalStrength");
            EXPECT_EQ(request, RIL_REQUEST_SIGNAL_STRENGTH);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getSignalStrength(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getVoiceRegistrationState) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getVoiceRegistrationState");
            EXPECT_EQ(request, RIL_REQUEST_VOICE_REGISTRATION_STATE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getVoiceRegistrationState(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getDataRegistrationState) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getDataRegistrationState");
            EXPECT_EQ(request, RIL_REQUEST_DATA_REGISTRATION_STATE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getDataRegistrationState(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getOperator) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getOperator");
            EXPECT_EQ(request, RIL_REQUEST_OPERATOR);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getOperator(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setRadioPower) {
    static bool on = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setRadioPower");
            EXPECT_EQ(request, RIL_REQUEST_RADIO_POWER);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(on ? 1 : 0, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setRadioPower(serial, on);
    tearDown(radio);
}

TEST(ril_service_legacy, sendDtmf) {
    static const char *s = "7";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendDtmf");
            EXPECT_EQ(request, RIL_REQUEST_DTMF);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(TextUtils::Equals(s, (const char *)data));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->sendDtmf(serial, s);
    tearDown(radio);
}

TEST(ril_service_legacy, sendSms) {
    static V1_0::GsmSmsMessage message {};
    message.smscPdu = "821012345";
    message.pdu = "01234567890ABCDEF";

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendSms");
            EXPECT_EQ(request, RIL_REQUEST_SEND_SMS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE(message.smscPdu == hidl_string(((const char **)data)[0]));
            EXPECT_TRUE(message.pdu == hidl_string(((const char **)data)[1]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->sendSms(serial, message);
    tearDown(radio);
}

TEST(ril_service_legacy, sendSMSExpectMore) {
    static V1_0::GsmSmsMessage message {};
    message.smscPdu = "821012345";
    message.pdu = "01234567890ABCDEF";

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendSMSExpectMore");
            EXPECT_EQ(request, RIL_REQUEST_SEND_SMS_EXPECT_MORE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE(message.smscPdu == hidl_string(((const char **)data)[0]));
            EXPECT_TRUE(message.pdu == hidl_string(((const char **)data)[1]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->sendSMSExpectMore(serial, message);
    tearDown(radio);
}

TEST(ril_service_legacy, setupDataCall) {
    static V1_0::RadioTechnology radioTechnology = V1_0::RadioTechnology::LTE;
    static V1_0::DataProfileInfo dataProfileInfo = {};
    static bool modemCognitive = false;
    static bool roamingAllowed = true;
    static bool isRoaming = true;

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setupDataCall");
            EXPECT_EQ(request, RIL_REQUEST_SETUP_DATA_CALL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 15);

            // TODO

            RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->setupDataCall(serial, radioTechnology, dataProfileInfo, modemCognitive, roamingAllowed,
            isRoaming);
    tearDown(radio);
}

TEST(ril_service_legacy, iccIOForApp) {
    static V1_0::IccIo iccIo {};

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::iccIOForApp");
            EXPECT_EQ(request, RIL_REQUEST_SIM_IO);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SIM_IO_v6));

            // TODO
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->iccIOForApp(serial, iccIo);
    tearDown(radio);
}

TEST(ril_service_legacy, sendUssd) {
    static const char *ussd = "ussd0";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendUssd");
            EXPECT_EQ(request, RIL_REQUEST_SEND_USSD);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(TextUtils::Equals(ussd, (char *)data));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendUssd(serial, ussd);
    tearDown(radio);
}

TEST(ril_service_legacy, cancelPendingUssd) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::cancelPendingUssd");
            EXPECT_EQ(request, RIL_REQUEST_CANCEL_USSD);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->cancelPendingUssd(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getClir) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getClir");
            EXPECT_EQ(request, RIL_REQUEST_GET_CLIR);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getClir(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setClir) {
    static int status = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setClir");
            EXPECT_EQ(request, RIL_REQUEST_SET_CLIR);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(status, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setClir(serial, status);
    tearDown(radio);
}

TEST(ril_service_legacy, getCallForwardStatus) {
    static V1_0::CallForwardInfo callInfo = {
        V1_0::CallForwardInfoStatus::ENABLE,
        1,
        2,
        91,
        "+821012345678",
        0,
    };

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCallForwardStatus");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_CALL_FORWARD_STATUS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CallForwardInfo));
            const RIL_CallForwardInfo& cf = *(RIL_CallForwardInfo *)data;
            EXPECT_EQ(cf.status, (int)callInfo.status);
            EXPECT_EQ(cf.reason, callInfo.reason);
            EXPECT_EQ(cf.serviceClass, callInfo.serviceClass);
            EXPECT_EQ(cf.toa, callInfo.toa);
            EXPECT_EQ(cf.timeSeconds, callInfo.timeSeconds);
            EXPECT_TRUE(cf.number == callInfo.number);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCallForwardStatus(serial, callInfo);
    tearDown(radio);
}

TEST(ril_service_legacy, setCallForward) {
    static V1_0::CallForwardInfo callInfo = {
        V1_0::CallForwardInfoStatus::ENABLE,
        1,
        2,
        91,
        "+821012345678",
        0,
    };

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCallForward");
            EXPECT_EQ(request, RIL_REQUEST_SET_CALL_FORWARD);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CallForwardInfo));
            const RIL_CallForwardInfo& cf = *(RIL_CallForwardInfo *)data;
            EXPECT_EQ(cf.status, (int)callInfo.status);
            EXPECT_EQ(cf.reason, callInfo.reason);
            EXPECT_EQ(cf.serviceClass, callInfo.serviceClass);
            EXPECT_EQ(cf.toa, callInfo.toa);
            EXPECT_EQ(cf.timeSeconds, callInfo.timeSeconds);
            EXPECT_TRUE(cf.number == callInfo.number);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCallForward(serial, callInfo);
    tearDown(radio);
}

TEST(ril_service_legacy, getCallWaiting) {
    static int serviceClass = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCallWaiting");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_CALL_WAITING);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(serviceClass, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCallWaiting(serial, serviceClass);
    tearDown(radio);
}

TEST(ril_service_legacy, setCallWaiting) {
    static bool enable = true;
    static int serviceClass = 2;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCallWaiting");
            EXPECT_EQ(request, RIL_REQUEST_SET_CALL_WAITING);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 2);
            EXPECT_EQ(enable, ((int *)data)[0]);
            EXPECT_EQ(serviceClass, ((int *)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCallWaiting(serial, enable, serviceClass);
    tearDown(radio);
}

TEST(ril_service_legacy, acknowledgeLastIncomingGsmSms) {
    static bool success = true;
    static V1_0::SmsAcknowledgeFailCause cause =
            V1_0::SmsAcknowledgeFailCause::MEMORY_CAPACITY_EXCEEDED;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::acknowledgeLastIncomingGsmSms");
            EXPECT_EQ(request, RIL_REQUEST_SMS_ACKNOWLEDGE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 2);
            EXPECT_EQ(success, ((int *)data)[0]);
            EXPECT_EQ((int)cause, ((int *)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->acknowledgeLastIncomingGsmSms(serial, success, cause);
    tearDown(radio);
}

TEST(ril_service_legacy, acceptCall) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::acceptCall");
            EXPECT_EQ(request, RIL_REQUEST_ANSWER);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->acceptCall(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, deactivateDataCall) {
    static int cid = 1;
    static bool reasonRadioShutDown = false;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::deactivateDataCall");
            EXPECT_EQ(request, RIL_REQUEST_DEACTIVATE_DATA_CALL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_EQ(cid, std::stoi(((char **)data)[0]));
            EXPECT_EQ(reasonRadioShutDown, std::stoi(((char **)data)[1]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->deactivateDataCall(serial, cid, reasonRadioShutDown);
    tearDown(radio);
}

TEST(ril_service_legacy, getFacilityLockForApp) {
    static hidl_string facility = "facility";
    static hidl_string password = "0123456";
    static int serviceClass = 1;
    static hidl_string appId = "appid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getFacilityLockForApp");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_FACILITY_LOCK);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 4);
            EXPECT_TRUE(facility == ((const char **)data)[0]);
            EXPECT_TRUE(password == ((const char **)data)[1]);
            EXPECT_TRUE(std::to_string(serviceClass) == ((const char **)data)[2]);
            EXPECT_TRUE(appId == ((const char **)data)[3]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getFacilityLockForApp(serial, facility, password, serviceClass, appId);
    tearDown(radio);
}

TEST(ril_service_legacy, setFacilityLockForApp) {
    static hidl_string facility = "facility";
    static bool lockState = true;
    static hidl_string password = "0123456";
    static int serviceClass = 1;
    static hidl_string appId = "appid1234";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setFacilityLockForApp");
            EXPECT_EQ(request, RIL_REQUEST_SET_FACILITY_LOCK);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 5);
            EXPECT_TRUE(facility == ((const char **)data)[0]);
            EXPECT_TRUE((lockState ? hidl_string("1") : hidl_string("0")) ==
                    ((const char **)data)[1]);
            EXPECT_TRUE(password == ((const char **)data)[2]);
            EXPECT_TRUE(std::to_string(serviceClass) == ((const char **)data)[3]);
            EXPECT_TRUE(appId == ((const char **)data)[4]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setFacilityLockForApp(serial, facility, lockState, password, serviceClass, appId);
    tearDown(radio);
}

TEST(ril_service_legacy, setBarringPassword) {
    static hidl_string facility = "facility";
    static hidl_string oldPassword = "0123456";
    static hidl_string newPassword = "5678901";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setBarringPassword");
            EXPECT_EQ(request, RIL_REQUEST_CHANGE_BARRING_PASSWORD);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 3);
            EXPECT_TRUE(facility == ((const char **)data)[0]);
            EXPECT_TRUE(oldPassword == ((const char **)data)[1]);
            EXPECT_TRUE(newPassword == ((const char **)data)[2]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setBarringPassword(serial, facility, oldPassword, newPassword);
    tearDown(radio);
}

TEST(ril_service_legacy, getNetworkSelectionMode) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getNetworkSelectionMode");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getNetworkSelectionMode(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setNetworkSelectionModeAutomatic) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setNetworkSelectionModeAutomatic");
            EXPECT_EQ(request, RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setNetworkSelectionModeAutomatic(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setNetworkSelectionModeManual) {
    static hidl_string operatorNumeric = "45005";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setNetworkSelectionModeManual");
            EXPECT_EQ(request, RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(operatorNumeric == ((const char **)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setNetworkSelectionModeManual(serial, operatorNumeric);
    tearDown(radio);
}

TEST(ril_service_legacy, getAvailableNetworks) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getAvailableNetworks");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_AVAILABLE_NETWORKS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getAvailableNetworks(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, startNetworkScan) {
    static V1_1::NetworkScanRequest request {

    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadio::startNetworkScan");
            EXPECT_EQ(request, RIL_REQUEST_START_NETWORK_SCAN);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_NetworkScanRequest));
            // TODO
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startNetworkScan(serial, request);
    tearDown(radio);
}

TEST(ril_service_legacy, stopNetworkScan) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::stopNetworkScan");
            EXPECT_EQ(request, RIL_REQUEST_STOP_NETWORK_SCAN);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->stopNetworkScan(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, startDtmf) {
    static hidl_string s = "7";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::startDtmf");
            EXPECT_EQ(request, RIL_REQUEST_DTMF_START);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(s == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startDtmf(serial, s);
    tearDown(radio);
}

TEST(ril_service_legacy, stopDtmf) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::stopDtmf");
            EXPECT_EQ(request, RIL_REQUEST_DTMF_STOP);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->stopDtmf(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getBasebandVersion) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getBasebandVersion");
            EXPECT_EQ(request, RIL_REQUEST_BASEBAND_VERSION);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getBasebandVersion(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, separateConnection) {
    static int gsmIndex = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::separateConnection");
            EXPECT_EQ(request, RIL_REQUEST_SEPARATE_CONNECTION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(gsmIndex, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->separateConnection(serial, gsmIndex);
    tearDown(radio);
}

TEST(ril_service_legacy, setMute) {
    static bool enable = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setMute");
            EXPECT_EQ(request, RIL_REQUEST_SET_MUTE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(enable, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setMute(serial, enable);
    tearDown(radio);
}

TEST(ril_service_legacy, getMute) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getMute");
            EXPECT_EQ(request, RIL_REQUEST_GET_MUTE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getMute(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getClip) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getClip");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_CLIP);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getClip(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getDataCallList) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getDataCallList");
            EXPECT_EQ(request, RIL_REQUEST_DATA_CALL_LIST);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getDataCallList(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setSuppServiceNotifications) {
    static bool enable = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setSuppServiceNotifications");
            EXPECT_EQ(request, RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(enable, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSuppServiceNotifications(serial, enable);
    tearDown(radio);
}

TEST(ril_service_legacy, writeSmsToSim) {
    static V1_0::SmsWriteArgs smsWriteArgs = {
        V1_0::SmsWriteArgsStatus::REC_READ,
        "01234567890ABCDE",
        "0F918112345",
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::writeSmsToSim");
            EXPECT_EQ(request, RIL_REQUEST_WRITE_SMS_TO_SIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SMS_WriteArgs));
            const RIL_SMS_WriteArgs& args = *(RIL_SMS_WriteArgs *)data;
            EXPECT_EQ((int)smsWriteArgs.status, args.status);
            EXPECT_TRUE(smsWriteArgs.pdu == args.pdu);
            EXPECT_TRUE(smsWriteArgs.smsc == args.smsc);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->writeSmsToSim(serial, smsWriteArgs);
    tearDown(radio);
}

TEST(ril_service_legacy, deleteSmsOnSim) {
    static int index = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::deleteSmsOnSim");
            EXPECT_EQ(request, RIL_REQUEST_DELETE_SMS_ON_SIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(index, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->deleteSmsOnSim(serial, index);
    tearDown(radio);
}

TEST(ril_service_legacy, setBandMode) {
    static V1_0::RadioBandMode mode = V1_0::RadioBandMode::BAND_MODE_USA;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setBandMode");
            EXPECT_EQ(request, RIL_REQUEST_SET_BAND_MODE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)mode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setBandMode(serial, mode);
    tearDown(radio);
}

TEST(ril_service_legacy, getAvailableBandModes) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getAvailableBandModes");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getAvailableBandModes(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, sendEnvelope) {
    static hidl_string command = "envelop_command";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendEnvelope");
            EXPECT_EQ(request, RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(command == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendEnvelope(serial, command);
    tearDown(radio);
}

TEST(ril_service_legacy, sendTerminalResponseToSim) {
    static hidl_string commandResponse = "commandResponse";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendTerminalResponseToSim");
            EXPECT_EQ(request, RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(commandResponse == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendTerminalResponseToSim(serial, commandResponse);
    tearDown(radio);
}

TEST(ril_service_legacy, handleStkCallSetupRequestFromSim) {
    static bool accept = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::handleStkCallSetupRequestFromSim");
            EXPECT_EQ(request, RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(accept, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->handleStkCallSetupRequestFromSim(serial, accept);
    tearDown(radio);
}

TEST(ril_service_legacy, explicitCallTransfer) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::explicitCallTransfer");
            EXPECT_EQ(request, RIL_REQUEST_EXPLICIT_CALL_TRANSFER);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->explicitCallTransfer(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setPreferredNetworkType) {
    static V1_0::PreferredNetworkType mode = V1_0::PreferredNetworkType::LTE_GSM_WCDMA;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setPreferredNetworkType");
            EXPECT_EQ(request, RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)mode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setPreferredNetworkType(serial, mode);
    tearDown(radio);
}

TEST(ril_service_legacy, getPreferredNetworkType) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getPreferredNetworkType");
            EXPECT_EQ(request, RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getPreferredNetworkType(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getNeighboringCids) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getNeighboringCids");
            EXPECT_EQ(request, RIL_REQUEST_GET_NEIGHBORING_CELL_IDS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getNeighboringCids(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setLocationUpdates) {
    static bool enable = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setLocationUpdates");
            EXPECT_EQ(request, RIL_REQUEST_SET_LOCATION_UPDATES);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(enable, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setLocationUpdates(serial, enable);
    tearDown(radio);
}

TEST(ril_service_legacy, setCdmaSubscriptionSource) {
    static V1_0::CdmaSubscriptionSource cdmaSub = V1_0::CdmaSubscriptionSource::RUIM_SIM;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCdmaSubscriptionSource");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)cdmaSub, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCdmaSubscriptionSource(serial, cdmaSub);
    tearDown(radio);
}

TEST(ril_service_legacy, setCdmaRoamingPreference) {
    static V1_0::CdmaRoamingType type = V1_0::CdmaRoamingType::AFFILIATED_ROAM;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCdmaRoamingPreference");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)type, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCdmaRoamingPreference(serial, type);
    tearDown(radio);
}

TEST(ril_service_legacy, getCdmaRoamingPreference) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCdmaRoamingPreference");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCdmaRoamingPreference(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setTTYMode) {
    static V1_0::TtyMode mode = V1_0::TtyMode::FULL;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setTTYMode");
            EXPECT_EQ(request, RIL_REQUEST_SET_TTY_MODE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)mode, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setTTYMode(serial, mode);
    tearDown(radio);
}

TEST(ril_service_legacy, getTTYMode) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getTTYMode");
            EXPECT_EQ(request, RIL_REQUEST_QUERY_TTY_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getTTYMode(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setPreferredVoicePrivacy) {
    static bool enable = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setPreferredVoicePrivacy");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(enable, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setPreferredVoicePrivacy(serial, enable);
    tearDown(radio);
}

TEST(ril_service_legacy, getPreferredVoicePrivacy) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getPreferredVoicePrivacy");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getPreferredVoicePrivacy(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, sendCDMAFeatureCode) {
    static hidl_string featureCode = "12345";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendCDMAFeatureCode");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_FLASH);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(featureCode == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendCDMAFeatureCode(serial, featureCode);
    tearDown(radio);
}

TEST(ril_service_legacy, sendBurstDtmf) {
    static hidl_string dtmf = "12345";
    static int on = 1;
    static int off = 0;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendBurstDtmf");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_BURST_DTMF);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 3);
            EXPECT_TRUE(dtmf == ((const char **)data)[0]);
            EXPECT_EQ(on, std::stoi(((const char **)data)[1]));
            EXPECT_EQ(off, std::stoi(((const char **)data)[2]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendBurstDtmf(serial, dtmf, on, off);
    tearDown(radio);
}

TEST(ril_service_legacy, sendCdmaSms) {
    static V1_0::CdmaSmsMessage sms {};

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendCdmaSms");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SEND_SMS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_SMS_Message));

            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->sendCdmaSms(serial, sms);
    tearDown(radio);
}

TEST(ril_service_legacy, acknowledgeLastIncomingCdmaSms) {
    static V1_0::CdmaSmsAck smsAck = {
        V1_0::CdmaSmsErrorClass::ERROR,
        12,
    };

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::acknowledgeLastIncomingCdmaSms");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_SMS_Ack));
            RIL_CDMA_SMS_Ack *rcsa = (RIL_CDMA_SMS_Ack *)data;
            EXPECT_EQ((int)smsAck.errorClass, rcsa->uErrorClass);
            EXPECT_EQ(smsAck.smsCauseCode, rcsa->uSMSCauseCode);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->acknowledgeLastIncomingCdmaSms(serial, smsAck);
    tearDown(radio);
}

TEST(ril_service_legacy, getGsmBroadcastConfig) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getGsmBroadcastConfig");
            EXPECT_EQ(request, RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getGsmBroadcastConfig(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setGsmBroadcastConfig) {
    static const int num = 3;
    static hidl_vec<V1_0::GsmBroadcastSmsConfigInfo> configInfo;
    configInfo.resize(num);

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setGsmBroadcastConfig");
            EXPECT_EQ(request, RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_GSM_BroadcastSmsConfigInfo *) * num);

            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->setGsmBroadcastConfig(serial, configInfo);
    tearDown(radio);
}

TEST(ril_service_legacy, setGsmBroadcastActivation) {
    static bool activate = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setGsmBroadcastActivation");
            EXPECT_EQ(request, RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(!activate, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setGsmBroadcastActivation(serial, activate);
    tearDown(radio);
}

TEST(ril_service_legacy, getCdmaBroadcastConfig) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCdmaBroadcastConfig");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCdmaBroadcastConfig(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setCdmaBroadcastConfig) {
    static const int num = 3;
    static hidl_vec<V1_0::CdmaBroadcastSmsConfigInfo> configInfo;
    configInfo.resize(num);

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCdmaBroadcastConfig");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_BroadcastSmsConfigInfo *) * num);

            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->setCdmaBroadcastConfig(serial, configInfo);
    tearDown(radio);
}

TEST(ril_service_legacy, setCdmaBroadcastActivation) {
    static bool activate = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCdmaBroadcastActivation");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(!activate, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCdmaBroadcastActivation(serial, activate);
    tearDown(radio);
}

TEST(ril_service_legacy, getCDMASubscription) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCDMASubscription");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SUBSCRIPTION);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCDMASubscription(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, writeSmsToRuim) {
    static V1_0::CdmaSmsWriteArgs cdmaSms = {
        V1_0::CdmaSmsWriteArgsStatus::REC_READ,
        {},
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::writeSmsToRuim");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_SMS_WriteArgs));
            //const RIL_CDMA_SMS_WriteArgs& rcsw = *(RIL_CDMA_SMS_WriteArgs *)data;

            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->writeSmsToRuim(serial, cdmaSms);
    tearDown(radio);
}

TEST(ril_service_legacy, deleteSmsOnRuim) {
    static int index = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::deleteSmsOnRuim");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(index, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->deleteSmsOnRuim(serial, index);
    tearDown(radio);
}

TEST(ril_service_legacy, getDeviceIdentity) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getDeviceIdentity");
            EXPECT_EQ(request, RIL_REQUEST_DEVICE_IDENTITY);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getDeviceIdentity(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, exitEmergencyCallbackMode) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::exitEmergencyCallbackMode");
            EXPECT_EQ(request, RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->exitEmergencyCallbackMode(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getSmscAddress) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getSmscAddress");
            EXPECT_EQ(request, RIL_REQUEST_GET_SMSC_ADDRESS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getSmscAddress(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setSmscAddress) {
    static hidl_string smsc = "0F9110123456";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setSmscAddress");
            EXPECT_EQ(request, RIL_REQUEST_SET_SMSC_ADDRESS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(smsc == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSmscAddress(serial, smsc);
    tearDown(radio);
}

TEST(ril_service_legacy, reportSmsMemoryStatus) {
    static bool available = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::reportSmsMemoryStatus");
            EXPECT_EQ(request, RIL_REQUEST_REPORT_SMS_MEMORY_STATUS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(available, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->reportSmsMemoryStatus(serial, available);
    tearDown(radio);
}

TEST(ril_service_legacy, reportStkServiceIsRunning) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::reportStkServiceIsRunning");
            EXPECT_EQ(request, RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->reportStkServiceIsRunning(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getCdmaSubscriptionSource) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCdmaSubscriptionSource");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCdmaSubscriptionSource(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, requestIsimAuthentication) {
    static hidl_string challenge = "0F9110123456";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::requestIsimAuthentication");
            EXPECT_EQ(request, RIL_REQUEST_ISIM_AUTHENTICATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(challenge == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->requestIsimAuthentication(serial, challenge);
    tearDown(radio);
}

TEST(ril_service_legacy, acknowledgeIncomingGsmSmsWithPdu) {
    static bool success = true;
    static hidl_string ackPdu = "0F9110123456";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::acknowledgeIncomingGsmSmsWithPdu");
            EXPECT_EQ(request, RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE(TextUtils::Equals(success ? "1" : "0", ((const char **)data)[0]));
            EXPECT_TRUE(ackPdu == ((const char **)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->acknowledgeIncomingGsmSmsWithPdu(serial, success, ackPdu);
    tearDown(radio);
}

TEST(ril_service_legacy, sendEnvelopeWithStatus) {
    static hidl_string contents = "0F9110123456";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendEnvelopeWithStatus");
            EXPECT_EQ(request, RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *));
            EXPECT_TRUE(contents == (const char *)data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendEnvelopeWithStatus(serial, contents);
    tearDown(radio);
}

TEST(ril_service_legacy, getVoiceRadioTechnology) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getVoiceRadioTechnology");
            EXPECT_EQ(request, RIL_REQUEST_VOICE_RADIO_TECH);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getVoiceRadioTechnology(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getCellInfoList) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getCellInfoList");
            EXPECT_EQ(request, RIL_REQUEST_GET_CELL_INFO_LIST);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getCellInfoList(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setCellInfoListRate) {
    static int rate = 999;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setCellInfoListRate");
            EXPECT_EQ(request, RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(rate, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCellInfoListRate(serial, rate);
    tearDown(radio);
}

TEST(ril_service_legacy, setInitialAttachApn) {
    static V1_0::DataProfileInfo dataProfileInfo = {};
    static bool modemCognitive = false;
    static bool isRoaming = true;

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setInitialAttachApn");
            EXPECT_EQ(request, RIL_REQUEST_SET_INITIAL_ATTACH_APN);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_InitialAttachApn_v15));

            // TODO
            //const RIL_InitialAttachApn_v15& iaa = *(RIL_InitialAttachApn_v15 *)data;

            RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->setInitialAttachApn(serial, dataProfileInfo, modemCognitive, isRoaming);
    tearDown(radio);
}

TEST(ril_service_legacy, getImsRegistrationState) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getImsRegistrationState");
            EXPECT_EQ(request, RIL_REQUEST_IMS_REGISTRATION_STATE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getImsRegistrationState(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, iccTransmitApduBasicChannel) {
    static V1_0::SimApdu message = {
        1, 2, 3, 4, 5, 6, "test_apdu",
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::iccTransmitApduBasicChannel");
            EXPECT_EQ(request, RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SIM_APDU));
            const RIL_SIM_APDU& apdu = *(RIL_SIM_APDU *)data;
            EXPECT_EQ(message.sessionId, apdu.sessionid);
            EXPECT_EQ(message.cla, apdu.cla);
            EXPECT_EQ(message.instruction, apdu.instruction);
            EXPECT_EQ(message.p1, apdu.p1);
            EXPECT_EQ(message.p2, apdu.p2);
            EXPECT_EQ(message.p3, apdu.p3);
            EXPECT_TRUE(message.data == apdu.data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->iccTransmitApduBasicChannel(serial, message);
    tearDown(radio);
}

TEST(ril_service_legacy, iccOpenLogicalChannel) {
    static hidl_string aid = "0F9110123456";
    static int p2 = 10;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::iccOpenLogicalChannel");
            EXPECT_EQ(request, RIL_REQUEST_SIM_OPEN_CHANNEL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_OpenChannelParams));
            const RIL_OpenChannelParams& params = *(RIL_OpenChannelParams *)data;
            EXPECT_EQ(p2, params.p2);
            EXPECT_TRUE(aid == params.aidPtr);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->iccOpenLogicalChannel(serial, aid, p2);
    tearDown(radio);
}

TEST(ril_service_legacy, iccCloseLogicalChannel) {
    static int channelId = 999;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::iccCloseLogicalChannel");
            EXPECT_EQ(request, RIL_REQUEST_SIM_CLOSE_CHANNEL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(channelId, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->iccCloseLogicalChannel(serial, channelId);
    tearDown(radio);
}

TEST(ril_service_legacy, iccTransmitApduLogicalChannel) {
    static V1_0::SimApdu message = {
        1, 2, 3, 4, 5, 6, "test_apdu",
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::iccTransmitApduLogicalChannel");
            EXPECT_EQ(request, RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SIM_APDU));
            const RIL_SIM_APDU& apdu = *(RIL_SIM_APDU *)data;
            EXPECT_EQ(message.sessionId, apdu.sessionid);
            EXPECT_EQ(message.cla, apdu.cla);
            EXPECT_EQ(message.instruction, apdu.instruction);
            EXPECT_EQ(message.p1, apdu.p1);
            EXPECT_EQ(message.p2, apdu.p2);
            EXPECT_EQ(message.p3, apdu.p3);
            EXPECT_TRUE(message.data == apdu.data);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->iccTransmitApduLogicalChannel(serial, message);
    tearDown(radio);
}

TEST(ril_service_legacy, nvReadItem) {
    static V1_0::NvItem itemId = V1_0::NvItem::CDMA_MEID;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::nvReadItem");
            EXPECT_EQ(request, RIL_REQUEST_NV_READ_ITEM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_NV_ReadItem));
            EXPECT_EQ((int)itemId, ((RIL_NV_ReadItem *)data)->itemID);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->nvReadItem(serial, itemId);
    tearDown(radio);
}

TEST(ril_service_legacy, nvWriteItem) {
    static V1_0::NvWriteItem item = {
        V1_0::NvItem::CDMA_MEID,
        "1234567890ABCDEF",
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::nvWriteItem");
            EXPECT_EQ(request, RIL_REQUEST_NV_WRITE_ITEM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_NV_WriteItem));
            EXPECT_EQ((int)item.itemId, ((RIL_NV_WriteItem *)data)->itemID);
            EXPECT_TRUE(item.value == ((RIL_NV_WriteItem *)data)->value);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->nvWriteItem(serial, item);
    tearDown(radio);
}

TEST(ril_service_legacy, nvWriteCdmaPrl) {
    static hidl_vec<uint8_t> prl(std::initializer_list<uint8_t> { 0x01, 0x02, 0x03, 0x04, 0x05 });
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::nvWriteCdmaPrl");
            EXPECT_EQ(request, RIL_REQUEST_NV_WRITE_CDMA_PRL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, prl.size());
            hidl_vec<uint8_t> test;
            test.setToExternal((uint8_t *)data, datalen);
            EXPECT_TRUE(test == prl);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->nvWriteCdmaPrl(serial, prl);
    tearDown(radio);
}

TEST(ril_service_legacy, nvResetConfig) {
    static V1_0::ResetNvType resetType = V1_0::ResetNvType::FACTORY_RESET;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::nvResetConfig");
            EXPECT_EQ(request, RIL_REQUEST_NV_RESET_CONFIG);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(3, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->nvResetConfig(serial, resetType);
    tearDown(radio);
}

TEST(ril_service_legacy, setUiccSubscription) {
    static V1_0::SelectUiccSub uiccSub = {
        0,
        1,
        V1_0::SubscriptionType::SUBSCRIPTION_1,
        V1_0::UiccSubActStatus::ACTIVATE,

    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setUiccSubscription");
            EXPECT_EQ(request, RIL_REQUEST_SET_UICC_SUBSCRIPTION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SelectUiccSub));
            const RIL_SelectUiccSub& rilUiccSub =  *(RIL_SelectUiccSub *)data;
            EXPECT_EQ((int)uiccSub.slot, rilUiccSub.slot);
            EXPECT_EQ((int)uiccSub.appIndex, rilUiccSub.app_index);
            EXPECT_EQ((int)uiccSub.subType, rilUiccSub.sub_type);
            EXPECT_EQ((int)uiccSub.actStatus, rilUiccSub.act_status);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setUiccSubscription(serial, uiccSub);
    tearDown(radio);
}

TEST(ril_service_legacy, setDataAllowed) {
    static bool allow = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setDataAllowed");
            EXPECT_EQ(request, RIL_REQUEST_ALLOW_DATA);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(allow, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setDataAllowed(serial, allow);
    tearDown(radio);
}

TEST(ril_service_legacy, getHardwareConfig) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getHardwareConfig");
            EXPECT_EQ(request, RIL_REQUEST_GET_HARDWARE_CONFIG);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getHardwareConfig(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, requestIccSimAuthentication) {
    static int authContext = 1;
    static hidl_string authData = "012345678";
    static hidl_string aid = "aid123456";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::requestIccSimAuthentication");
            EXPECT_EQ(request, RIL_REQUEST_SIM_AUTHENTICATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SimAuthentication));
            const RIL_SimAuthentication& pf = *(RIL_SimAuthentication *)data;
            EXPECT_EQ(authContext, pf.authContext);
            EXPECT_TRUE(authData == pf.authData);
            EXPECT_TRUE(aid == pf.aid);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->requestIccSimAuthentication(serial, authContext, authData, aid);
    tearDown(radio);
}

TEST(ril_service_legacy, setDataProfile) {
    const size_t num = 3;
    static hidl_vec<V1_0::DataProfileInfo> profiles(num);
    profiles[0] = {
        V1_0::DataProfileId::DEFAULT,
        "apn1",
        "IP",
        "IP",
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        V1_0::MvnoType::NONE,
        "",
    };
    profiles[1] = {
        V1_0::DataProfileId::IMS,
        "apn2",
        "IPV6",
        "IP",
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        V1_0::MvnoType::IMSI,
        "2060188",
    };
    profiles[2] = {
        V1_0::DataProfileId::TETHERED,
        "apn3",
        "IPV4V6",
        "IPV4V6",
        V1_0::ApnAuthType::PAP_NO_CHAP,
        "user",
        "1234",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        V1_0::MvnoType::GID,
        "4E",
    };
    static bool isRoaming = true;

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setDataProfile");
            EXPECT_EQ(request, RIL_REQUEST_SET_DATA_PROFILE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_DataProfileInfo_v15 *) * num);

            RIL_DataProfileInfo_v15 **dataProfilePtrs = (RIL_DataProfileInfo_v15 **)data;
            for (size_t i = 0; i < num; i++) {
                test(*(dataProfilePtrs[i]), profiles[i]);
            }

            RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, 0, 0);
        }

        void test(const RIL_DataProfileInfo_v15& df, const V1_0::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.protocol == dpi.protocol);
            EXPECT_TRUE(df.roamingProtocol == dpi.roamingProtocol);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ(df.profileId, (int)dpi.profileId);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, (int)dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, (int)dpi.maxConns);
            EXPECT_EQ(df.waitTime, (int)dpi.waitTime);
            EXPECT_EQ(df.enabled, (int)dpi.enabled);
            EXPECT_EQ(df.supportedTypesBitmask, (int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmask, (int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtu, (int)dpi.mtu);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->setDataProfile(serial, profiles, isRoaming);
    tearDown(radio);
}

TEST(ril_service_legacy, requestShutdown) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::requestShutdown");
            EXPECT_EQ(request, RIL_REQUEST_SHUTDOWN);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->requestShutdown(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getRadioCapability) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getRadioCapability");
            EXPECT_EQ(request, RIL_REQUEST_GET_RADIO_CAPABILITY);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getRadioCapability(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setRadioCapability) {
    static V1_0::RadioCapability rc {};
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setRadioCapability");
            EXPECT_EQ(request, RIL_REQUEST_SET_RADIO_CAPABILITY);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_RadioCapability));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setRadioCapability(serial, rc);
    tearDown(radio);
}

TEST(ril_service_legacy, startLceService) {
    static int32_t reportInterval = 1000;
    static bool pullMode = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::startLceService");
            EXPECT_EQ(request, RIL_REQUEST_START_LCE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 2);
            EXPECT_EQ(reportInterval, ((int *)data)[0]);
            EXPECT_EQ(pullMode, ((int *)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startLceService(serial, reportInterval, pullMode);
    tearDown(radio);
}

TEST(ril_service_legacy, stopLceService) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::stopLceService");
            EXPECT_EQ(request, RIL_REQUEST_STOP_LCE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->stopLceService(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, pullLceData) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::pullLceData");
            EXPECT_EQ(request, RIL_REQUEST_PULL_LCEDATA);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->pullLceData(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getModemActivityInfo) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getModemActivityInfo");
            EXPECT_EQ(request, RIL_REQUEST_GET_ACTIVITY_INFO);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getModemActivityInfo(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setAllowedCarriers) {
    static bool allAllowed = true;
    std::vector<V1_0::Carrier> allowedCarriers;
    allowedCarriers.push_back({ "450", "05", V1_0::CarrierMatchType::SPN, "spn" });
    allowedCarriers.push_back({ "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" });
    allowedCarriers.push_back({ "310", "480", V1_0::CarrierMatchType::ALL, "" });
    std::vector<V1_0::Carrier> excludedCarriers;
    excludedCarriers.push_back({ "450", "05", V1_0::CarrierMatchType::SPN, "spn" });
    excludedCarriers.push_back({ "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" });
    excludedCarriers.push_back({ "310", "480", V1_0::CarrierMatchType::ALL, "" });
    static V1_0::CarrierRestrictions carriers = { allowedCarriers, excludedCarriers };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setAllowedCarriers");
            EXPECT_EQ(request, RIL_REQUEST_SET_CARRIER_RESTRICTIONS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CarrierRestrictions));
            const RIL_CarrierRestrictions& cr = *(RIL_CarrierRestrictions *)data;
            EXPECT_EQ(carriers.allowedCarriers.size(), (unsigned long)/* NEED TO CHECK */ cr.len_allowed_carriers);
            if (cr.len_allowed_carriers > 0) {
                EXPECT_NE(cr.allowed_carriers, nullptr);
                for (int i = 0; i < cr.len_allowed_carriers; i++) {
                    EXPECT_TRUE(carriers.allowedCarriers[i].mcc == cr.allowed_carriers[i].mcc);
                    EXPECT_TRUE(carriers.allowedCarriers[i].mnc == cr.allowed_carriers[i].mnc);
                    EXPECT_TRUE((int)carriers.allowedCarriers[i].matchType == cr.allowed_carriers[i].match_type);
                    EXPECT_TRUE(carriers.allowedCarriers[i].matchData == cr.allowed_carriers[i].match_data);
                }
            }
            EXPECT_EQ(carriers.excludedCarriers.size(), (unsigned long)/* NEED TO CHECK */ cr.len_excluded_carriers);
            if (cr.len_excluded_carriers > 0) {
                EXPECT_NE(cr.excluded_carriers, nullptr);
                for (int i = 0; i < cr.len_excluded_carriers; i++) {
                    EXPECT_TRUE(carriers.excludedCarriers[i].mcc == cr.excluded_carriers[i].mcc);
                    EXPECT_TRUE(carriers.excludedCarriers[i].mnc == cr.excluded_carriers[i].mnc);
                    EXPECT_TRUE((int)carriers.excludedCarriers[i].matchType == cr.excluded_carriers[i].match_type);
                    EXPECT_TRUE(carriers.excludedCarriers[i].matchData == cr.excluded_carriers[i].match_data);
                }
            }

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setAllowedCarriers(serial, allAllowed, carriers);
    tearDown(radio);
}

TEST(ril_service_legacy, getAllowedCarriers) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::getAllowedCarriers");
            EXPECT_EQ(request, RIL_REQUEST_GET_CARRIER_RESTRICTIONS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getAllowedCarriers(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, sendDeviceState) {
    static V1_0::DeviceStateType deviceStateType = V1_0::DeviceStateType::POWER_SAVE_MODE;
    static bool state = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *, size_t, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::sendDeviceState");
            EXPECT_EQ(request, RIL_REQUEST_SEND_DEVICE_STATE);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->sendDeviceState(serial, deviceStateType, state);
    tearDown(radio);
}

TEST(ril_service_legacy, setIndicationFilter) {
    static int indicationFilter = 0x1F;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setIndicationFilter");
            EXPECT_EQ(request, RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(indicationFilter, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setIndicationFilter(serial, indicationFilter);
    tearDown(radio);
}

TEST(ril_service_legacy, setSimCardPower) {
    static bool powerUp = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_0::IRadio::setSimCardPower");
            EXPECT_EQ(request, RIL_REQUEST_SET_SIM_CARD_POWER);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(powerUp, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSimCardPower(serial, powerUp);
    tearDown(radio);
}

TEST(ril_service_legacy, setSimCardPower_1_1) {
    static V1_1::CardPowerState state = V1_1::CardPowerState::POWER_UP;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadio::setSimCardPower_1_1");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1, 1)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)state, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSimCardPower_1_1(serial, state);
    tearDown(radio);
}

TEST(ril_service_legacy, setCarrierInfoForImsiEncryption) {
    static V1_1::ImsiEncryptionInfo carrier = {
        "450", "05",
        { 0x1, 0x2, 0x3, 0x4, 0x5 },
        "keyIdentifier",
        (int64_t)7777,

    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadio::setCarrierInfoForImsiEncryption");
            EXPECT_EQ(request, RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CarrierInfoForImsiEncryption));
            const RIL_CarrierInfoForImsiEncryption& imsiEncryption =
                    *(RIL_CarrierInfoForImsiEncryption *)data;
            EXPECT_TRUE(carrier.mcc ==  imsiEncryption.mcc);
            EXPECT_TRUE(carrier.mnc ==  imsiEncryption.mnc);
            EXPECT_TRUE(carrier.keyIdentifier ==  imsiEncryption.keyIdentifier);
            EXPECT_EQ(carrier.carrierKey.size(), (unsigned long)/* NEED TO CHECK */ imsiEncryption.carrierKeyLength);
            if (imsiEncryption.carrierKey) {
                hidl_vec<uint8_t> tmp;
                tmp.setToExternal(imsiEncryption.carrierKey, imsiEncryption.carrierKeyLength);
                EXPECT_TRUE(carrier.carrierKey == tmp);
            }
            EXPECT_EQ(carrier.expirationTime, imsiEncryption.expirationTime);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setCarrierInfoForImsiEncryption(serial, carrier);
    tearDown(radio);
}

TEST(ril_service_legacy, startKeepalive) {
    static V1_1::KeepaliveRequest keepalive = {
        V1_1::KeepaliveType::NATT_IPV4,
        { 0x10, 0x20, 0x30, 0x40 },
        6001,
        { 0x11, 0x21, 0x31, 0x41 },
        7001,
        50000,
        1,
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadio::startKeepalive");
            EXPECT_EQ(request, RIL_REQUEST_START_KEEPALIVE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_KeepaliveRequest));
            const RIL_KeepaliveRequest& kaReq = *(RIL_KeepaliveRequest *)data;
            EXPECT_EQ((int)keepalive.type, (int)kaReq.type);
            hidl_vec<uint8_t> tmp;
            tmp.setToExternal((uint8_t *)kaReq.sourceAddress, keepalive.sourceAddress.size());
            EXPECT_TRUE(keepalive.sourceAddress == tmp);
            EXPECT_EQ(keepalive.sourcePort, kaReq.sourcePort);
            tmp.setToExternal((uint8_t *)kaReq.destinationAddress, keepalive.destinationAddress.size());
            EXPECT_TRUE(keepalive.destinationAddress == tmp);
            EXPECT_EQ(keepalive.destinationPort, kaReq.destinationPort);
            EXPECT_EQ(keepalive.maxKeepaliveIntervalMillis, kaReq.maxKeepaliveIntervalMillis);
            EXPECT_EQ(keepalive.cid , kaReq.cid );
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startKeepalive(serial, keepalive);
    tearDown(radio);
}

TEST(ril_service_legacy, stopKeepalive) {
    static int sessionHandle = 2;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_1::IRadio::stopKeepalive");
            EXPECT_EQ(request, RIL_REQUEST_STOP_KEEPALIVE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(sessionHandle, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->stopKeepalive(serial, sessionHandle);
    tearDown(radio);
}

TEST(ril_service_legacy, startNetworkScan_1_2) {
    static V1_2::NetworkScanRequest networkScanRequest {};
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_2::IRadio::startNetworkScan_1_2");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,2)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_NetworkScanRequest_V1_2));

            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startNetworkScan_1_2(serial, networkScanRequest);
    tearDown(radio);
}

TEST(ril_service_legacy, setIndicationFilter_1_2) {
    static int indicationFilter = 0x1F;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_2::IRadio::setIndicationFilter_1_2");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, HAL_VERSION_CODE(1,2)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(indicationFilter, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setIndicationFilter_1_2(serial, indicationFilter);
    tearDown(radio);
}

TEST(ril_service_legacy, setSignalStrengthReportingCriteria) {
    static int32_t hysteresisMs = 1000;
    static int32_t hysteresisDb = 20;
    static hidl_vec<int32_t> thresholdsDbm = { -118, -100, -90, -50 };
    static V1_2::AccessNetwork accessNetwork = V1_2::AccessNetwork::EUTRAN;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_2::IRadio::setSignalStrengthReportingCriteria");
            EXPECT_EQ(request, RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SignalStrengthReportingCriteria_V1_2));
            const RIL_SignalStrengthReportingCriteria_V1_2& ss = *(RIL_SignalStrengthReportingCriteria_V1_2 *)data;
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisMs, ss.hysteresisMs);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisDb, ss.hysteresisDb);
            EXPECT_EQ(thresholdsDbm.size(), ss.numOfThresholdsDbm);
            hidl_vec<int32_t> tmp;
            tmp.setToExternal((int32_t *)ss.thresholdsDbm, ss.numOfThresholdsDbm);
            EXPECT_TRUE(thresholdsDbm == tmp);
            EXPECT_EQ((RIL_AccessNetwork)accessNetwork, ss.accessNetwork);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSignalStrengthReportingCriteria(serial, hysteresisMs, hysteresisDb, thresholdsDbm, accessNetwork);
    tearDown(radio);
}

TEST(ril_service_legacy, setLinkCapacityReportingCriteria) {
    static int32_t hysteresisMs = 3000;
    static int32_t hysteresisDlKbps = 50;
    static int32_t hysteresisUlKbps = 50;
    static hidl_vec<int32_t> thresholdsDownlinkKbps = {
        100,    // VoIP
        500,    // Web browsing
        1000,   // SD video streaming
        5000,   // HD video streaming
        10000,  // file downloading
        20000,  // 4K video streaming
        50000,  // LTE-Advanced speeds
        75000,
        100000,
        200000, // 5G speeds
        500000,
        1000000,
        1500000,
        2000000
    };
    static hidl_vec<int32_t> thresholdsUplinkKbps = {
        100,    // VoIP calls
        500,
        1000,   // SD video calling
        5000,   // HD video calling
        10000,  // file uploading
        20000,  // 4K video calling
        50000,
        75000,
        100000,
        200000,
        500000
    };
    static V1_2::AccessNetwork accessNetwork = V1_2::AccessNetwork::EUTRAN;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_2::IRadio::setLinkCapacityReportingCriteria");
            EXPECT_EQ(request, RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_LinkCapacityReportingCriteria_V1_2));
            const RIL_LinkCapacityReportingCriteria_V1_2& lc = *(RIL_LinkCapacityReportingCriteria_V1_2 *)data;
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisMs, lc.hysteresisMs);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisDlKbps, lc.hysteresisDlKbps);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisUlKbps, lc.hysteresisUlKbps);
            EXPECT_EQ(thresholdsDownlinkKbps.size(), lc.numOfThresholdsDownlinkKbps);
            hidl_vec<int32_t> tmp;
            tmp.setToExternal((int32_t *)lc.thresholdsDownlinkKbps, lc.numOfThresholdsDownlinkKbps);
            EXPECT_TRUE(thresholdsDownlinkKbps == tmp);
            EXPECT_EQ(thresholdsUplinkKbps.size(), lc.numOfThresholdsUplinkKbps);
            tmp.setToExternal((int32_t *)lc.thresholdsUplinkKbps, lc.numOfThresholdsUplinkKbps);
            EXPECT_TRUE(thresholdsUplinkKbps == tmp);
            EXPECT_EQ((RIL_AccessNetwork)accessNetwork, lc.accessNetwork);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setLinkCapacityReportingCriteria(serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps,
            thresholdsDownlinkKbps, thresholdsUplinkKbps, accessNetwork);
    tearDown(radio);
}

TEST(ril_service_legacy, setupDataCall_1_2) {
    static V1_2::AccessNetwork accessNetwork = V1_2::AccessNetwork::EUTRAN;
    static V1_0::DataProfileInfo dataProfileInfo = {
        V1_0::DataProfileId::DEFAULT,
        "apn1",
        "IP",
        "IP",
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        V1_0::MvnoType::NONE,
        "",
    };
    static bool modemCognitive = false;
    static bool roamingAllowed = true;
    static bool isRoaming = true;
    static V1_2::DataRequestReason reason = V1_2::DataRequestReason::NORMAL;
    static hidl_vec<hidl_string> addresses;
    static hidl_vec<hidl_string> dnses;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_2::IRadio::setupDataCall_1_2");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,2)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 18);

            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setupDataCall_1_2(serial, accessNetwork, dataProfileInfo, modemCognitive,
            roamingAllowed, isRoaming, reason, addresses, dnses);
    tearDown(radio);
}

TEST(ril_service_legacy, deactivateDataCall_1_2) {
    static int cid = 2;
    static V1_2::DataRequestReason reason = V1_2::DataRequestReason::NORMAL;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_2::IRadio::deactivateDataCall_1_2");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_DEACTIVATE_DATA_CALL, HAL_VERSION_CODE(1,2)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_EQ(cid, std::stoi(((char **)data)[0]));
            EXPECT_EQ((int)reason, std::stoi(((char **)data)[1]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->deactivateDataCall_1_2(serial, cid, reason);
    tearDown(radio);
}

TEST(ril_service_legacy, setSystemSelectionChannels) {
    static bool specifyChannels = true;
    V1_1::RadioAccessSpecifier ras = {
        V1_1::RadioAccessNetworks::EUTRAN,
        {},
        {},
        { V1_1::EutranBands::BAND_1, V1_1::EutranBands::BAND_3, V1_1::EutranBands::BAND_7 },
        { 1000, 1100, 1200 },
    };
    static hidl_vec<V1_1::RadioAccessSpecifier> specifiers = { ras };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_3::IRadio::setSystemSelectionChannels");
            EXPECT_EQ(request, RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SysSelChannelRequest_V1_5));
            const RIL_SysSelChannelRequest_V1_5& sscr = *(RIL_SysSelChannelRequest_V1_5 *)data;
            EXPECT_EQ(specifyChannels, sscr.isSpecifyChannels);
            EXPECT_EQ(specifiers.size(), sscr.specifiers_length);
            EXPECT_EQ((int)specifiers[0].radioAccessNetwork, sscr.specifiers[0].radio_access_network);
            EXPECT_EQ(specifiers[0].channels.size(), sscr.specifiers[0].channels_length);
            EXPECT_EQ(specifiers[0].eutranBands.size(), sscr.specifiers[0].bands_length);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSystemSelectionChannels(serial, specifyChannels, specifiers);
    tearDown(radio);
}

TEST(ril_service_legacy, enableModem) {
    static bool on = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_3::IRadio::enableModem");
            EXPECT_EQ(request, RIL_REQUEST_ENABLE_MODEM);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(on, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->enableModem(serial, on);
    tearDown(radio);
}

TEST(ril_service_legacy, getModemStackStatus) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_3::IRadio::getModemStackStatus");
            EXPECT_EQ(request, RIL_REQUEST_GET_MODEM_STATUS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getModemStackStatus(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setupDataCall_1_4) {
    static V1_4::AccessNetwork accessNetwork = V1_4::AccessNetwork::EUTRAN;
    static V1_4::DataProfileInfo dataProfileInfo = {
        V1_0::DataProfileId::DEFAULT,
        "apn1",
        V1_4::PdpProtocolType::IP,
        V1_4::PdpProtocolType::IP,
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        true,
        true,
    };
    static bool roamingAllowed = true;
    static V1_2::DataRequestReason reason = V1_2::DataRequestReason::NORMAL;
    static hidl_vec<hidl_string> addresses;
    static hidl_vec<hidl_string> dnses;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::setupDataCall_1_4");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,4)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SetupDataCallInfo_V1_4));
            const RIL_SetupDataCallInfo_V1_4& setupDataCallInfo = *(RIL_SetupDataCallInfo_V1_4 *)data;
            test(setupDataCallInfo.dataProfileInfo, dataProfileInfo);
            EXPECT_EQ((int)accessNetwork, setupDataCallInfo.accessNetwork);
            EXPECT_EQ(roamingAllowed, setupDataCallInfo.roamingAllow);
            EXPECT_EQ((int)reason, setupDataCallInfo.reason);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_DataProfileInfo_V1_4& df, const V1_4::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ(df.profileId, (int)dpi.profileId);
            EXPECT_EQ(df.protocol, (int)dpi.protocol);
            EXPECT_EQ(df.roamingProtocol, (int)dpi.roamingProtocol);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, dpi.maxConns);
            EXPECT_EQ(df.waitTime, dpi.waitTime);
            EXPECT_EQ(df.enabled, dpi.enabled);
            EXPECT_EQ(df.supportedApnTypesBitmap, (unsigned int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmap, (unsigned int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtu, dpi.mtu);
            EXPECT_EQ(df.preferred, dpi.preferred);
            EXPECT_EQ(df.persistent, dpi.persistent);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setupDataCall_1_4(serial, accessNetwork, dataProfileInfo, roamingAllowed,
            reason, addresses, dnses);
    tearDown(radio);
}

TEST(ril_service_legacy, setInitialAttachApn_1_4) {
    static V1_4::DataProfileInfo dataProfileInfo = {
        V1_0::DataProfileId::DEFAULT,
        "apn1",
        V1_4::PdpProtocolType::IP,
        V1_4::PdpProtocolType::IP,
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        true,
        true,
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::setInitialAttachApn_1_4");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1,4)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_DataProfileInfo_V1_4));
            const RIL_DataProfileInfo_V1_4& dataProfile = *(RIL_DataProfileInfo_V1_4 *)data;
            test(dataProfile, dataProfileInfo);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_DataProfileInfo_V1_4& df, const V1_4::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ(df.profileId, (int)dpi.profileId);
            EXPECT_EQ(df.protocol, (int)dpi.protocol);
            EXPECT_EQ(df.roamingProtocol, (int)dpi.roamingProtocol);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, dpi.maxConns);
            EXPECT_EQ(df.waitTime, dpi.waitTime);
            EXPECT_EQ(df.enabled, dpi.enabled);
            EXPECT_EQ(df.supportedApnTypesBitmap, (unsigned int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmap, (unsigned int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtu, dpi.mtu);
            EXPECT_EQ(df.preferred, dpi.preferred);
            EXPECT_EQ(df.persistent, dpi.persistent);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setInitialAttachApn_1_4(serial, dataProfileInfo);
    tearDown(radio);
}

TEST(ril_service_legacy, setDataProfile_1_4) {
    static hidl_vec<V1_4::DataProfileInfo> profiles = {
        {
            V1_0::DataProfileId::DEFAULT,
            "apn1",
            V1_4::PdpProtocolType::IP,
            V1_4::PdpProtocolType::IP,
            V1_0::ApnAuthType::NO_PAP_NO_CHAP,
            "",
            "",
            V1_0::DataProfileInfoType::THREE_GPP,
            3000,
            4,
            -1,
            true,
            -1,
            -1,
            1500,
            true,
            true,
        },
        {
            V1_0::DataProfileId::IMS,
            "apn2",
            V1_4::PdpProtocolType::IPV4V6,
            V1_4::PdpProtocolType::IPV4V6,
            V1_0::ApnAuthType::NO_PAP_NO_CHAP,
            "",
            "",
            V1_0::DataProfileInfoType::THREE_GPP,
            3000,
            4,
            -1,
            true,
            -1,
            -1,
            1500,
            true,
            true,
        },
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::setDataProfile_1_4");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1,4)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_DataProfileInfo_V1_4 *) * profiles.size());
            const RIL_DataProfileInfo_V1_4 **dataProfilePtrs = (const RIL_DataProfileInfo_V1_4 **)data;
            for (size_t i = 0; i < profiles.size(); i++) {
                test(*dataProfilePtrs[i], profiles[i]);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_DataProfileInfo_V1_4& df, const V1_4::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ(df.profileId, (int)dpi.profileId);
            EXPECT_EQ(df.protocol, (int)dpi.protocol);
            EXPECT_EQ(df.roamingProtocol, (int)dpi.roamingProtocol);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, dpi.maxConns);
            EXPECT_EQ(df.waitTime, dpi.waitTime);
            EXPECT_EQ(df.enabled, dpi.enabled);
            EXPECT_EQ(df.supportedApnTypesBitmap, (unsigned int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmap, (unsigned int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtu, dpi.mtu);
            EXPECT_EQ(df.preferred, dpi.preferred);
            EXPECT_EQ(df.persistent, dpi.persistent);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setDataProfile_1_4(serial, profiles);
    tearDown(radio);
}

TEST(ril_service_legacy, emergencyDial) {
    static V1_0::Dial dialInfo {};
    static int categories = 0x1F;
    static hidl_vec<hidl_string> urns;
    static V1_4::EmergencyCallRouting routing = V1_4::EmergencyCallRouting::EMERGENCY;
    static bool hasKnownUserIntentEmergency = true;
    static bool isTesting = false;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::emergencyDial");
            EXPECT_EQ(request, RIL_REQUEST_EMERGENCY_DIAL);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_EmergencyDial));
            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->emergencyDial(serial, dialInfo, categories, urns, routing, hasKnownUserIntentEmergency,
            isTesting);
    tearDown(radio);
}

TEST(ril_service_legacy, startNetworkScan_1_4) {
    static V1_2::NetworkScanRequest networkScanRequest {};
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::startNetworkScan_1_4");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,4)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_NetworkScanRequest_V1_2));
            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startNetworkScan_1_4(serial, networkScanRequest);
    tearDown(radio);
}

TEST(ril_service_legacy, getPreferredNetworkTypeBitmap) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::getPreferredNetworkTypeBitmap");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE, HAL_VERSION_CODE(1, 4)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getPreferredNetworkTypeBitmap(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setPreferredNetworkTypeBitmap) {
    static int networkTypeBitmap = 0x1FFF;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::setPreferredNetworkTypeBitmap");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE, HAL_VERSION_CODE(1, 4)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ((int)networkTypeBitmap, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setPreferredNetworkTypeBitmap(serial, networkTypeBitmap);
    tearDown(radio);
}

TEST(ril_service_legacy, setAllowedCarriers_1_4) {
    static V1_4::CarrierRestrictionsWithPriority carriers = {
        {
            { "450", "05", V1_0::CarrierMatchType::SPN, "spn" },
            { "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" },
            { "310", "480", V1_0::CarrierMatchType::ALL, "" }
        },
        {
            { "450", "05", V1_0::CarrierMatchType::SPN, "spn" },
            { "001", "01", V1_0::CarrierMatchType::IMSI_PREFIX, "imsi" },
            { "310", "480", V1_0::CarrierMatchType::ALL, "" }
        },
        true,
    };
    static V1_4::SimLockMultiSimPolicy multiSimPolicy = V1_4::SimLockMultiSimPolicy::ONE_VALID_SIM_MUST_BE_PRESENT;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::setAllowedCarriers");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1,4)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CarrierRestrictions_V1_4));
            const RIL_CarrierRestrictions_V1_4& cr = *(RIL_CarrierRestrictions_V1_4 *)data;
            EXPECT_EQ(carriers.allowedCarriers.size(), (unsigned int)/* NEED TO CHECK */ cr.carriers.len_allowed_carriers);
            if (cr.carriers.len_allowed_carriers > 0) {
                EXPECT_NE(cr.carriers.allowed_carriers, nullptr);
                for (int i = 0; i < cr.carriers.len_allowed_carriers; i++) {
                    EXPECT_TRUE(carriers.allowedCarriers[i].mcc == cr.carriers.allowed_carriers[i].mcc);
                    EXPECT_TRUE(carriers.allowedCarriers[i].mnc == cr.carriers.allowed_carriers[i].mnc);
                    EXPECT_TRUE((int)carriers.allowedCarriers[i].matchType == cr.carriers.allowed_carriers[i].match_type);
                    EXPECT_TRUE(carriers.allowedCarriers[i].matchData == cr.carriers.allowed_carriers[i].match_data);
                }
            }
            EXPECT_EQ(carriers.excludedCarriers.size(), (unsigned int)/* NEED TO CHECK */ cr.carriers.len_excluded_carriers);
            if (cr.carriers.len_excluded_carriers > 0) {
                EXPECT_NE(cr.carriers.excluded_carriers, nullptr);
                for (int i = 0; i < cr.carriers.len_excluded_carriers; i++) {
                    EXPECT_TRUE(carriers.excludedCarriers[i].mcc == cr.carriers.excluded_carriers[i].mcc);
                    EXPECT_TRUE(carriers.excludedCarriers[i].mnc == cr.carriers.excluded_carriers[i].mnc);
                    EXPECT_TRUE((int)carriers.excludedCarriers[i].matchType == cr.carriers.excluded_carriers[i].match_type);
                    EXPECT_TRUE(carriers.excludedCarriers[i].matchData == cr.carriers.excluded_carriers[i].match_data);
                }
            }
            EXPECT_EQ(carriers.allowedCarriersPrioritized, cr.carriers.allowedCarriersPrioritized);
            EXPECT_EQ((int)multiSimPolicy, cr.multiSimPolicy);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setAllowedCarriers_1_4(serial, carriers, multiSimPolicy);
    tearDown(radio);
}

TEST(ril_service_legacy, getAllowedCarriers_1_4) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::getAllowedCarriers_1_4");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_GET_CARRIER_RESTRICTIONS, HAL_VERSION_CODE(1,4)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getAllowedCarriers_1_4(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getSignalStrength_1_4) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::getSignalStrength_1_4");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,4)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getSignalStrength_1_4(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setSignalStrengthReportingCriteria_1_5) {
    static V1_5::SignalThresholdInfo signalThresholdInfo = {
        V1_5::SignalMeasurementType::SSRSRP,
        1000,
        20,
        { -118, -100, -90, -50 },
        true,
    };
    static V1_5::AccessNetwork accessNetwork = V1_5::AccessNetwork::NGRAN;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setSignalStrengthReportingCriteria_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SignalStrengthReportingCriteria_V1_5));
            const RIL_SignalStrengthReportingCriteria_V1_5& ssrc = *(RIL_SignalStrengthReportingCriteria_V1_5 *)data;
            EXPECT_EQ((int)signalThresholdInfo.signalMeasurement, (int)ssrc.signalThresholdInfo.signalMeasurement);
            EXPECT_EQ(signalThresholdInfo.hysteresisMs, ssrc.signalThresholdInfo.hysteresisMs);
            EXPECT_EQ(signalThresholdInfo.hysteresisDb, ssrc.signalThresholdInfo.hysteresisDb);
            EXPECT_EQ(signalThresholdInfo.thresholds.size(), ssrc.signalThresholdInfo.len_thresholds);
            hidl_vec<int32_t> tmp;
            tmp.setToExternal((int32_t *)ssrc.signalThresholdInfo.thresholds, ssrc.signalThresholdInfo.len_thresholds);
            EXPECT_TRUE(signalThresholdInfo.thresholds == tmp);
            EXPECT_EQ((int)accessNetwork, ssrc.accessNetwork);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSignalStrengthReportingCriteria_1_5(serial, signalThresholdInfo, accessNetwork);
    tearDown(radio);
}

TEST(ril_service_legacy, enableUiccApplications) {
    static bool enable = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::enableUiccApplications");
            EXPECT_EQ(request, RIL_REQUEST_ENABLE_UICC_APPLICATIONS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(enable, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->enableUiccApplications(serial, enable);
    tearDown(radio);
}

TEST(ril_service_legacy, areUiccApplicationsEnabled) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::areUiccApplicationsEnabled");
            EXPECT_EQ(request, RIL_REQUEST_GET_UICC_APPLICATIONS_ENABLEMENT);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->areUiccApplicationsEnabled(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setSystemSelectionChannels_1_5) {
    static bool specifyChannels = true;
    V1_5::RadioAccessSpecifier ras {};
    ras.radioAccessNetwork = V1_5::RadioAccessNetworks::NGRAN;
    ras.bands.ngranBands({ V1_5::NgranBands::BAND_1, V1_5::NgranBands::BAND_12, V1_5::NgranBands::BAND_257 });
    ras.channels = { 1000, 1100, 1200 };
    static hidl_vec<V1_5::RadioAccessSpecifier> specifiers = { ras };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setSystemSelectionChannels_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_SYSTEM_SELECTION_CHANNELS, HAL_VERSION_CODE(1,5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SysSelChannelRequest_V1_5));
            const RIL_SysSelChannelRequest_V1_5& sscr = *(RIL_SysSelChannelRequest_V1_5 *)data;
            EXPECT_EQ(specifyChannels, sscr.isSpecifyChannels);
            EXPECT_EQ(specifiers.size(), sscr.specifiers_length);
            EXPECT_EQ((int)specifiers[0].radioAccessNetwork, sscr.specifiers[0].radio_access_network);
            EXPECT_EQ(specifiers[0].bands.ngranBands().size(), sscr.specifiers[0].bands_length);
            EXPECT_EQ(specifiers[0].channels.size(), sscr.specifiers[0].channels_length);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setSystemSelectionChannels_1_5(serial, specifyChannels, specifiers);
    tearDown(radio);
}

TEST(ril_service_legacy, startNetworkScan_1_5) {
    static V1_5::NetworkScanRequest networkScanRequest {};
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::startNetworkScan_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_START_NETWORK_SCAN, HAL_VERSION_CODE(1,5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_NetworkScanRequest_V1_5));
            // TODO

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->startNetworkScan_1_5(serial, networkScanRequest);
    tearDown(radio);
}

TEST(ril_service_legacy, setupDataCall_1_5) {
    static V1_5::AccessNetwork accessNetwork = V1_5::AccessNetwork::EUTRAN;
    static V1_5::DataProfileInfo dataProfileInfo = {
        V1_0::DataProfileId::DEFAULT,
        "apn1",
        V1_4::PdpProtocolType::IP,
        V1_4::PdpProtocolType::IP,
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        1500,
        true,
        true,
    };
    static bool roamingAllowed = true;
    static V1_2::DataRequestReason reason = V1_2::DataRequestReason::NORMAL;
    static hidl_vec<V1_5::LinkAddress> addresses;
    static hidl_vec<hidl_string> dnses;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_4::IRadio::setupDataCall_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SetupDataCallInfo_V1_5));
            const RIL_SetupDataCallInfo_V1_5& setupDataCallInfo = *(RIL_SetupDataCallInfo_V1_5 *)data;
            test(setupDataCallInfo.dataProfileInfo, dataProfileInfo);
            EXPECT_EQ((int)accessNetwork, setupDataCallInfo.accessNetwork);
            EXPECT_EQ(roamingAllowed, setupDataCallInfo.roamingAllow);
            EXPECT_EQ((int)reason, setupDataCallInfo.reason);

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_DataProfileInfo_V1_5& df, const V1_5::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ((long)df.profileId, (long)dpi.profileId);
            EXPECT_EQ(df.protocol, (int)dpi.protocol);
            EXPECT_EQ(df.roamingProtocol, (int)dpi.roamingProtocol);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, dpi.maxConns);
            EXPECT_EQ(df.waitTime, dpi.waitTime);
            EXPECT_EQ(df.enabled, dpi.enabled);
            EXPECT_EQ(df.supportedApnTypesBitmap, (unsigned int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmap, (unsigned int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtuV4, dpi.mtuV4);
            EXPECT_EQ(df.mtuV6, dpi.mtuV6);
            EXPECT_EQ(df.preferred, dpi.preferred);
            EXPECT_EQ(df.persistent, dpi.persistent);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setupDataCall_1_5(serial, accessNetwork, dataProfileInfo, roamingAllowed,
            reason, addresses, dnses);
    tearDown(radio);
}

TEST(ril_service_legacy, setInitialAttachApn_1_5) {
    static V1_5::DataProfileInfo dataProfileInfo = {
        V1_0::DataProfileId::DEFAULT,
        "apn1",
        V1_4::PdpProtocolType::IP,
        V1_4::PdpProtocolType::IP,
        V1_0::ApnAuthType::NO_PAP_NO_CHAP,
        "",
        "",
        V1_0::DataProfileInfoType::THREE_GPP,
        3000,
        4,
        -1,
        true,
        -1,
        -1,
        1500,
        1500,
        true,
        true,
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setInitialAttachApn_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_INITIAL_ATTACH_APN, HAL_VERSION_CODE(1, 5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_DataProfileInfo_V1_5));
            const RIL_DataProfileInfo_V1_5& dataProfile = *(RIL_DataProfileInfo_V1_5 *)data;
            test(dataProfile, dataProfileInfo);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_DataProfileInfo_V1_5& df, const V1_5::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ((int)df.profileId, (int)dpi.profileId);
            EXPECT_EQ(df.protocol, (int)dpi.protocol);
            EXPECT_EQ(df.roamingProtocol, (int)dpi.roamingProtocol);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, dpi.maxConns);
            EXPECT_EQ(df.waitTime, dpi.waitTime);
            EXPECT_EQ(df.enabled, dpi.enabled);
            EXPECT_EQ(df.supportedApnTypesBitmap, (unsigned int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmap, (unsigned int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtuV4, dpi.mtuV4);
            EXPECT_EQ(df.mtuV6, dpi.mtuV6);
            EXPECT_EQ(df.preferred, dpi.preferred);
            EXPECT_EQ(df.persistent, dpi.persistent);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setInitialAttachApn_1_5(serial, dataProfileInfo);
    tearDown(radio);
}

TEST(ril_service_legacy, setDataProfile_1_5) {
    static hidl_vec<V1_5::DataProfileInfo> profiles = {
        {
            V1_0::DataProfileId::DEFAULT,
            "apn1",
            V1_4::PdpProtocolType::IP,
            V1_4::PdpProtocolType::IP,
            V1_0::ApnAuthType::NO_PAP_NO_CHAP,
            "",
            "",
            V1_0::DataProfileInfoType::THREE_GPP,
            3000,
            4,
            -1,
            true,
            -1,
            -1,
            1500,
            1500,
            true,
            true,
        },
        {
            V1_0::DataProfileId::IMS,
            "apn2",
            V1_4::PdpProtocolType::IPV4V6,
            V1_4::PdpProtocolType::IPV4V6,
            V1_0::ApnAuthType::NO_PAP_NO_CHAP,
            "",
            "",
            V1_0::DataProfileInfoType::THREE_GPP,
            3000,
            4,
            -1,
            true,
            -1,
            -1,
            1500,
            1500,
            true,
            true,
        },
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setDataProfile_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1, 5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_DataProfileInfo_V1_5 *) * profiles.size());
            const RIL_DataProfileInfo_V1_5 **dataProfilePtrs = (const RIL_DataProfileInfo_V1_5 **)data;
            for (size_t i = 0; i < profiles.size(); i++) {
                test(*dataProfilePtrs[i], profiles[i]);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_DataProfileInfo_V1_5& df, const V1_5::DataProfileInfo& dpi) {
            EXPECT_TRUE(df.apn == dpi.apn);
            EXPECT_TRUE(df.user == dpi.user);
            EXPECT_TRUE(df.password == dpi.password);
            EXPECT_EQ((int)df.profileId, (int)dpi.profileId);
            EXPECT_EQ(df.protocol, (int)dpi.protocol);
            EXPECT_EQ(df.roamingProtocol, (int)dpi.roamingProtocol);
            EXPECT_EQ(df.authType, (int)dpi.authType);
            EXPECT_EQ(df.type, (int)dpi.type);
            EXPECT_EQ(df.maxConnsTime, dpi.maxConnsTime);
            EXPECT_EQ(df.maxConns, dpi.maxConns);
            EXPECT_EQ(df.waitTime, dpi.waitTime);
            EXPECT_EQ(df.enabled, dpi.enabled);
            EXPECT_EQ(df.supportedApnTypesBitmap, (unsigned int)dpi.supportedApnTypesBitmap);
            EXPECT_EQ(df.bearerBitmap, (unsigned int)dpi.bearerBitmap);
            EXPECT_EQ(df.mtuV4, dpi.mtuV4);
            EXPECT_EQ(df.mtuV6, dpi.mtuV6);
            EXPECT_EQ(df.preferred, dpi.preferred);
            EXPECT_EQ(df.persistent, dpi.persistent);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setDataProfile_1_5(serial, profiles);
    tearDown(radio);
}

TEST(ril_service_legacy, setRadioPower_1_5) {
    static bool powerOn = true;
    static bool forEmergencyCall = true;
    static bool preferredForEmergencyCall = true;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setRadioPower_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int) * 3);
            EXPECT_EQ(powerOn, ((int *)data)[0]);
            EXPECT_EQ(forEmergencyCall, ((int *)data)[1]);
            EXPECT_EQ(preferredForEmergencyCall, ((int *)data)[2]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setRadioPower_1_5(serial, powerOn, forEmergencyCall, preferredForEmergencyCall);
    tearDown(radio);
}

TEST(ril_service_legacy, setIndicationFilter_1_5) {
    static int indicationFilter = 0x1F;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setIndicationFilter_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER, HAL_VERSION_CODE(1, 5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            EXPECT_EQ(indicationFilter, ((int *)data)[0]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setIndicationFilter_1_5(serial, indicationFilter);
    tearDown(radio);
}

TEST(ril_service_legacy, getVoiceRegistrationState_1_5) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::getVoiceRegistrationState_1_5");
            EXPECT_EQ(request, RIL_REQUEST_VOICE_REGISTRATION_STATE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getVoiceRegistrationState_1_5(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, getDataRegistrationState_1_5) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::getDataRegistrationState_1_5");
            EXPECT_EQ(request, RIL_REQUEST_DATA_REGISTRATION_STATE);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->getDataRegistrationState_1_5(serial);
    tearDown(radio);
}

TEST(ril_service_legacy, setNetworkSelectionModeManual_1_5) {
    static hidl_string operatorNumeric = "45005";
    static V1_5::RadioAccessNetworks ran = V1_5::RadioAccessNetworks::NGRAN;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setNetworkSelectionModeManual_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL, HAL_VERSION_CODE(1,5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE(operatorNumeric == ((const char **)data)[0]);
            EXPECT_TRUE((int)ran == std::stoi(((const char **)data)[1]));
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setNetworkSelectionModeManual_1_5(serial, operatorNumeric, ran);
    tearDown(radio);
}

TEST(ril_service_legacy, sendCdmaSmsExpectMore) {
    static V1_0::CdmaSmsMessage message = {
        1, true, 2, {}, {}, {},
    };

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::sendCdmaSmsExpectMore");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_SMS_Message));
            // TODO
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    int32_t serial = GetRandomSerialNumber();
    radio->sendCdmaSmsExpectMore(serial, message);
    tearDown(radio);
}

TEST(ril_service_legacy, supplySimDepersonalization) {
    static V1_5::PersoSubstate persoType = V1_5::PersoSubstate::SIM_ICCID;
    static hidl_string controlKey = "12345678";
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::supplySimDepersonalization");
            EXPECT_EQ(request, RIL_REQUEST_SUPPLY_SIM_DEPERSONALIZATION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            EXPECT_TRUE((int)persoType == std::stoi(((const char **)data)[0]));
            EXPECT_TRUE(controlKey == ((const char **)data)[1]);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->supplySimDepersonalization(serial, persoType, controlKey);
    tearDown(radio);
}

TEST(ril_service_legacy, setLinkCapacityReportingCriteria_1_5) {
    static int32_t hysteresisMs = 3000;
    static int32_t hysteresisDlKbps = 50;
    static int32_t hysteresisUlKbps = 50;
    static hidl_vec<int32_t> thresholdsDownlinkKbps = {
        100,    // VoIP
        500,    // Web browsing
        1000,   // SD video streaming
        5000,   // HD video streaming
        10000,  // file downloading
        20000,  // 4K video streaming
        50000,  // LTE-Advanced speeds
        75000,
        100000,
        200000, // 5G speeds
        500000,
        1000000,
        1500000,
        2000000
    };
    static hidl_vec<int32_t> thresholdsUplinkKbps = {
        100,    // VoIP calls
        500,
        1000,   // SD video calling
        5000,   // HD video calling
        10000,  // file uploading
        20000,  // 4K video calling
        50000,
        75000,
        100000,
        200000,
        500000
    };
    static V1_5::AccessNetwork accessNetwork = V1_5::AccessNetwork::EUTRAN;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_5::IRadio::setLinkCapacityReportingCriteria_1_5");
            EXPECT_EQ(request, ENCODE_REQUEST(RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA, HAL_VERSION_CODE(1,5)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_LinkCapacityReportingCriteria_V1_5));
            const RIL_LinkCapacityReportingCriteria_V1_5& lc = *(RIL_LinkCapacityReportingCriteria_V1_5 *)data;
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisMs, lc.hysteresisMs);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisDlKbps, lc.hysteresisDlKbps);
            EXPECT_EQ((unsigned int)/* NEED TO CHECK */ hysteresisUlKbps, lc.hysteresisUlKbps);
            EXPECT_EQ(thresholdsDownlinkKbps.size(), lc.numOfThresholdsDownlinkKbps);
            hidl_vec<int32_t> tmp;
            tmp.setToExternal((int32_t *)lc.thresholdsDownlinkKbps, lc.numOfThresholdsDownlinkKbps);
            EXPECT_TRUE(thresholdsDownlinkKbps == tmp);
            EXPECT_EQ(thresholdsUplinkKbps.size(), lc.numOfThresholdsUplinkKbps);
            tmp.setToExternal((int32_t *)lc.thresholdsUplinkKbps, lc.numOfThresholdsUplinkKbps);
            EXPECT_TRUE(thresholdsUplinkKbps == tmp);
            EXPECT_EQ((RIL_AccessNetwork)accessNetwork, lc.accessNetwork);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_5::IRadio> radio = setupRadio(&callback);
    radio->setLinkCapacityReportingCriteria_1_5(serial, hysteresisMs, hysteresisDlKbps, hysteresisUlKbps,
            thresholdsDownlinkKbps, thresholdsUplinkKbps, accessNetwork);
    tearDown(radio);
}
