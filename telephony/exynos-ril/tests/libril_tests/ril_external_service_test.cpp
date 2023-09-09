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
 * ril_external_service_test.cpp
 *
 *  Created on: 2021. 7. 20.
 */
#include <vendor/samsung_slsi/telephony/hardware/radioExternal/1.1/IOemSlsiRadioExternal.h>
#include <vendor/samsung_slsi/telephony/hardware/radioExternal/1.1/IOemSlsiRadioExternalRes.h>
#include <vendor/samsung_slsi/telephony/hardware/radioExternal/1.1/IOemSlsiRadioExternalInd.h>

#include <gtest/gtest.h>
#include "slsi/ril_external_service.h"
#include <librilutils/systemproperty.h>
#include <slsi/ril_oem.h>
#include "ril_service_test.h"
#include "testutils.h"

using namespace vendor::samsung_slsi::telephony::hardware::radioExternal;
using namespace vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::hidl_vec;
using android::sp;

using libriltest::GetRandomSerialNumber;

typedef int (*RadioExternalResponseHandler)(int, int, int, int, RIL_Errno, void *, size_t);
typedef int (*RadioExternalIndicationHandler)(int, int, int, const void *, size_t);

TEST(ril_external_service, IS_DEBUGABLE) {
    bool vdbg = SystemProperty::Get("persist.vendor.config.vdbg") == "1";
    bool user = SystemProperty::Get("ro.vendor.build.type") == "user";
    EXPECT_EQ(IS_DEBUGABLE(), (vdbg || !user));
}

TEST(ril_external_service, IsMultiSimEnabled) {
    std::string multisim = SystemProperty::Get("persist.radio.multisim.config");
    EXPECT_EQ(IsMultiSimEnabled(), "dsds" == multisim);
    EXPECT_EQ(GetPhoneCount(), "dsds" == multisim ? 2 : 1);
}

TEST(ril_external_service, getRadioExternalServiceRwlock) {
    EXPECT_NE(radioExternal::getRadioExternalServiceRwlock(), nullptr);
}

TEST(ril_external_service, radio_external_response) {
    RadioExternalResponseHandler testVector[] = {
        radioExternal::sendRequestRawResponse,
        radioExternal::getAvailableNtworksResponse,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioExternalResponseHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        //
        EXPECT_TRUE(f(-1, INT_MAX, 0, i, RIL_E_SUCCESS, nullptr, 0) <= 0);
    }
}

TEST(ril_external_service, radio_external_indication) {
    RadioExternalIndicationHandler testVector[] = {
        radioExternal::rilExternalRawIndication,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);

    for (size_t i = 0; i < size; i++) {
        RadioExternalIndicationHandler f = testVector[i];
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(f(-1, 0, 0, nullptr, 0) <= 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
extern "C" void RIL_External_onRequestComplete(RIL_External_Token t, RIL_Errno e,
        void *response, size_t responselen);
static VendorFunctionExcutor sVendorFunctionExcutor;

static void onTestRequest(int reqOemId, RIL_External_Token externalToken,
        void *data, size_t datalen, RIL_SOCKET_ID) {
    sVendorFunctionExcutor.onRequest(reqOemId, data, datalen, externalToken, RIL_SOCKET_1);
}

#if defined(ANDROID_MULTI_SIM)
static RIL_RadioState onTestStateRequest(RIL_SOCKET_ID socket_id) {
    return sVendorFunctionExcutor.onStateRequest(socket_id);
}
#else
static RIL_RadioState onTestStateRequest() {
    return sVendorFunctionExcutor.onStateRequest(RIL_SOCKET_1);
}
#endif

static RIL_RadioExternalFunctions sTestVendorFunctions = {
    RIL_VERSION,
    onTestRequest,
    onTestStateRequest,
};

static sp<V1_1::IOemSlsiRadioExternal> setupRadioExternal(OnRequestListener *listener = nullptr) {
    radioExternal::initVendorRadio();
    sp<V1_1::IOemSlsiRadioExternal> radio = radioExternal::getRadio();
    EXPECT_FALSE(radio.get() == nullptr);

    radioExternal::setVendorFunctions(&sTestVendorFunctions);
    sVendorFunctionExcutor.setOnRequestListener(listener);
    return radio;
}

static void tearDown(sp<V1_1::IOemSlsiRadioExternal>& radio) {
    radio = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
    radioExternal::setVendorFunctions(nullptr);
}

TEST(ril_external_service, setResponseFunctions) {
    struct VendorRadioResponse : public V1_0::IOemSlsiRadioExternalRes {
        VendorRadioResponse() = default;
        virtual ~VendorRadioResponse() = default;
        virtual Return<void> sendRequestRawResponse(const RadioExternalResponseInfo&,
                const hidl_vec<uint8_t>&) {
            return Void();
        }
        virtual Return<void> sendRequestRawResponseSeg(const RadioExternalResponseInfo&,
                const hidl_vec<uint8_t>&,
                int32_t, int32_t) {
            return Void();
        }
    };

    struct VendorRadioIndication : public V1_0::IOemSlsiRadioExternalInd {
        bool mConnected = false;
        VendorRadioIndication() = default;
        virtual ~VendorRadioIndication() = default;
        virtual Return<void> rilExternalRawIndication(int32_t rilcMsgId, int32_t,
                const hidl_vec<uint8_t>&, int32_t) {
            if (rilcMsgId == RILC_UNSOL_CONNECTED) {
                mConnected = true;
            }
            return Void();
        }
        virtual Return<void> rilExternalRawIndicationSeg(int32_t, int32_t,
                const hidl_vec<uint8_t>&, int32_t, int32_t, int32_t) {
            return Void();
        }
    };
    sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal();
    sp<VendorRadioResponse> responseCallback = new VendorRadioResponse;
    sp<VendorRadioIndication> indicationCallabck = new VendorRadioIndication;
    int clientId = radio->setResponseFunctions(responseCallback, indicationCallabck);
    EXPECT_TRUE(clientId >= 0);
    EXPECT_TRUE(indicationCallabck->mConnected);
    radio->setIndicationFilter(clientId, { 1000, 1001, 1002, });
    responseCallback = nullptr;
    indicationCallabck = nullptr;
    tearDown(radio);
}

static void run_sendRequestRaw(int from, int to) {
    static bool onComplete;
    size_t countRequested = 0;
    size_t countNotRequested = 0;
    ASSERT_TRUE(to >= from);
    for (int requestId = from; requestId <= to; requestId++) {
        struct OnRequestListenerImpl : public OnRequestListener {
            void onRequest(int, void *, size_t, RIL_Token t, RIL_SOCKET_ID) {
                onComplete = true;
                RIL_External_onRequestComplete((RIL_External_Token)t, RIL_E_INTERNAL_ERR, 0, 0);
            }
        } callback;
        int32_t serial = GetRandomSerialNumber();
        sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal(&callback);
        onComplete = false;;
        uint8_t tmp[100];
        hidl_vec<uint8_t> testdata;
        testdata.setToExternal(tmp, serial % sizeof(testdata));
        radio->sendRequestRaw(serial, INT_MAX, requestId, 0, testdata.size(), testdata);
        onComplete ? countRequested++ : countNotRequested++;
        tearDown(radio);
    }
    EXPECT_EQ(countRequested + countNotRequested, (unsigned long)/* NEED TO CHECK */ (to - from + 1));
}

TEST(ril_external_service, sendRequestRaw_1_to_1000) {
    run_sendRequestRaw(1, 1000);
}

TEST(ril_external_service, sendRequestRaw_1001_to_2000) {
    run_sendRequestRaw(1001, 2000);
}

TEST(ril_external_service, sendRequestRaw_2001_to_3000) {
    run_sendRequestRaw(2001, 3000);
}

TEST(ril_external_service, sendRequestRaw_3001_to_4000) {
    run_sendRequestRaw(3001, 4000);
}

TEST(ril_external_service, sendRequestRaw_4001_to_6000) {
    run_sendRequestRaw(4001, 6000);
}

TEST(ril_external_service, sendRequestRaw_6001_to_9999) {
    run_sendRequestRaw(6001, 9999);
}

struct VendorRadioResponse : public V1_1::IOemSlsiRadioExternalRes {
    VendorRadioResponse() = default;
    virtual ~VendorRadioResponse() = default;
    virtual Return<void> sendRequestRawResponse(const RadioExternalResponseInfo&,
            const hidl_vec<uint8_t>&) {
        return Void();
    }
    virtual Return<void> sendRequestRawResponseSeg(const RadioExternalResponseInfo&,
            const hidl_vec<uint8_t>&,
            int32_t, int32_t) {
        return Void();
    }
};

struct VendorRadioIndication : public V1_1::IOemSlsiRadioExternalInd {
    VendorRadioIndication() = default;
    virtual ~VendorRadioIndication() = default;
    virtual Return<void> rilExternalRawIndication(int32_t, int32_t,
            const hidl_vec<uint8_t>&, int32_t) {
        return Void();
    }
    virtual Return<void> rilExternalRawIndicationSeg(int32_t, int32_t,
            const hidl_vec<uint8_t>&, int32_t, int32_t, int32_t) {
        return Void();
    }
};

TEST(ril_external_service, sendRequestRawSegment) {
    const size_t maxSegmentSize = (2 * 1024);
    static const size_t numOfSize = maxSegmentSize + INTrand(false) % maxSegmentSize;  // 10 KiB + a
    static uint8_t *testVector = new uint8_t[numOfSize];
    ASSERT_NE(testVector, nullptr);
    for (size_t i = 0; i < numOfSize; i++) {
        *(testVector + i) = INTrand(false) % 0xFF;
    }

    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("onRequest sendRequestRawSegment");
            EXPECT_EQ(numOfSize, datalen);
            bool match = false;
            for (size_t i = 0; i < datalen; i++) {
                if (!(match = (testVector[i] == ((uint8_t *)data)[i]))) break;
            }
            EXPECT_TRUE(match);
            RIL_External_onRequestComplete((RIL_External_Token)t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal(&callback);
    sp<VendorRadioResponse> responseCallback = new VendorRadioResponse;
    sp<VendorRadioIndication> indicationCallabck = new VendorRadioIndication;
    int clientId = radio->setResponseFunctions_1_1(responseCallback, indicationCallabck,
            "ril_external_service.sendRequestRawSegment");
    // clientId must be valid when testing sendRequestRawSegment.
    ASSERT_TRUE(clientId >= 0);

    size_t datalen = numOfSize;
    uint8_t *p = testVector;
    int segIndex = 0;
    while (datalen > 0) {
        size_t segSize = datalen > maxSegmentSize ? maxSegmentSize : datalen % maxSegmentSize;
        hidl_vec<uint8_t> testdata;
        testdata.setToExternal(p, segSize);
        radio->sendRequestRawSegment(serial, clientId, 1, 0, testdata.size(), testdata, segIndex++, (int)numOfSize);
        p += segSize;
        datalen -= segSize;
    }
    delete[] testVector;
    radio->clearResponseFunctions(clientId);
    responseCallback = nullptr;
    indicationCallabck = nullptr;
    tearDown(radio);
}


TEST(ril_external_service, sendRequestRawSegment_invalid_segIndex) {
    const size_t maxSegmentSize = (2 * 1024);
    static const size_t numOfSize = maxSegmentSize + INTrand(false) % maxSegmentSize;  // 10 KiB + a
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal();
    struct TestVendorRadioResponseImp : public VendorRadioResponse {
        std::vector<uint8_t> mData;
        virtual Return<void> sendRequestRawResponse(const RadioExternalResponseInfo& info,
                const hidl_vec<uint8_t>&) {
            dlog("TestVendorRadioResponseImp#sendRequestRawResponse");
            EXPECT_TRUE(info.error == RadioExternalError::RADIO_EXTERNAL_INVALID_ARGUMENTS);
            return Void();
        }
    };
    sp<TestVendorRadioResponseImp> responseCallback = new TestVendorRadioResponseImp;
    sp<VendorRadioIndication> indicationCallabck = new VendorRadioIndication;
    int clientId = radio->setResponseFunctions_1_1(responseCallback, indicationCallabck,
            "ril_external_service.sendRequestRawSegment");
    // clientId must be valid when testing sendRequestRawSegment.
    ASSERT_TRUE(clientId >= 0);
    hidl_vec<uint8_t> testdata = { 0x1, 0x2, 0x3, 0x4, 0x5 };
    int lastIndex = (int)((numOfSize + maxSegmentSize - 1) /  maxSegmentSize) - 1;
    radio->sendRequestRawSegment(serial, clientId, 1, 0, testdata.size(), testdata, lastIndex, (int)numOfSize);
    radio->clearResponseFunctions(clientId);
    responseCallback = nullptr;
    indicationCallabck = nullptr;
    tearDown(radio);
}

TEST(ril_external_service, sendRequestRawSegment_invalid_clientId) {
    const size_t maxSegmentSize = (2 * 1024);
    static const size_t numOfSize = maxSegmentSize + INTrand(false) % maxSegmentSize;  // 10 KiB + a
    static uint8_t *testVector = new uint8_t[numOfSize];
    ASSERT_NE(testVector, nullptr);
    for (size_t i = 0; i < numOfSize; i++) {
        *(testVector + i) = INTrand(false) % 0xFF;
    }

    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal();
    size_t datalen = numOfSize;
    uint8_t *p = testVector;
    int segIndex = 0;
    while (datalen > 0) {
        size_t segSize = datalen > maxSegmentSize ? maxSegmentSize : datalen % maxSegmentSize;
        hidl_vec<uint8_t> testdata;
        testdata.setToExternal(p, segSize);
        radio->sendRequestRawSegment(serial, -1, 1, 0, testdata.size(), testdata, segIndex++, (int)numOfSize);
        p += segSize;
        datalen -= segSize;
    }
    delete[] testVector;
    tearDown(radio);
}

TEST(ril_external_service, sendRequestRawResponseSeg) {
    const size_t maxSegmentSize = (2 * 1024);
    static const size_t numOfSize = maxSegmentSize * 5 + INTrand(false) % maxSegmentSize;  // 10 KiB + a
    static uint8_t *testVector = new uint8_t[numOfSize];
    ASSERT_NE(testVector, nullptr);
    for (size_t i = 0; i < numOfSize; i++) {
        *(testVector + i) = INTrand(false) % 0xFF;
    }

    static bool onComplete = false;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int, void *, size_t, RIL_Token t, RIL_SOCKET_ID) {
            // return a large size of data
            RIL_External_onRequestComplete((RIL_External_Token)t, RIL_E_SUCCESS,
                    testVector, numOfSize);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal(&callback);

    struct TestVendorRadioResponseImp : public VendorRadioResponse {
        std::vector<uint8_t> mData;
        Return<void> sendRequestRawResponseSeg(const RadioExternalResponseInfo&,
                const hidl_vec<uint8_t>& data, int32_t segIndex, int32_t totalLen) {
            dlog("TestVendorRadioResponseImp#sendRequestRawResponseSeg: %d", segIndex);
            EXPECT_TRUE((unsigned long)/* NEED TO CHECK */ segIndex < (totalLen + maxSegmentSize - 1) / maxSegmentSize);
            for (size_t i = 0; i < data.size(); i++) {
                mData.push_back(data[i]);
            }

            if ((unsigned long)/* NEED TO CHECK */ totalLen == mData.size()) {
                bool match = false;
                for (size_t i = 0; i < (size_t) /* NEED TO CHECK */ totalLen; i++) {
                    if (!(match = (mData[i] == testVector[i]))) break;
                }
                EXPECT_TRUE(match);
                onComplete = true;
            } else if (mData.size() > (unsigned long)/* NEED TO CHECK */ totalLen) {
                EXPECT_TRUE(false);
            }
            return Void();
        }
    };
    sp<TestVendorRadioResponseImp> responseCallback = new TestVendorRadioResponseImp;
    sp<VendorRadioIndication> indicationCallabck = new VendorRadioIndication;
    int clientId = radio->setResponseFunctions_1_1(responseCallback, indicationCallabck,
            "ril_external_service.sendRequestRawSegment");
    // clientId must be valid when testing sendRequestRawSegment.
    ASSERT_TRUE(clientId >= 0);

    radio->sendRequestRaw(serial, clientId, 1, 0, 0, {});
    EXPECT_TRUE(onComplete);
    delete[] testVector;
    radio->clearResponseFunctions(clientId);
    responseCallback = nullptr;
    indicationCallabck = nullptr;
    tearDown(radio);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
extern "C" void
RIL_External_onUnsolicitedResponse(int unsolResponse, const void *data, size_t datalen, int slotId);

TEST(ril_external_service, RIL_External_onUnsolicitedResponse) {
    const int from = 20000;
    const int to = 22000;
    sp<V1_1::IOemSlsiRadioExternal> radio = setupRadioExternal();
    for (int i = from; i <= to; i++) {
        char data[] = { 0x1, 0x2, 0x3, 0x4, 0x5 };
        RIL_External_onUnsolicitedResponse(from, data, sizeof(data), 0);
    }
    tearDown(radio);
}
