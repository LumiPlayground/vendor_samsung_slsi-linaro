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
#include "ril_service_1_6.h"
#include "ril_service_legacy.h"
#include "ril_service_common.h"
#include "ril_service_test.h"
#include "slsi/radio_v1_6.h"
#include <librilutils/textutils.h>

using namespace android::hardware::radio;
using ::android::hardware::Return;
using ::android::hardware::Status;
using ::android::hardware::hidl_bitfield;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Void;
using android::sp;

using libriltest::GetRandomSerialNumber;

TEST(ril_service_1_6, checkReturnStatus) {
    radio::impl::v1_6::initRadios();
    int slotId = 0;
    Return<void> ret(Status::fromExceptionCode(Status::EX_TRANSACTION_FAILED));
    radio::impl::v1_6::checkReturnStatus(slotId, ret);
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

static sp<V1_6::IRadio> setupRadio(OnRequestListener *listener = nullptr) {
    radio::impl::v1_6::initRadios();
    sp<V1_0::IRadio> radio = radio::impl::v1_6::getRadio(0);
    EXPECT_FALSE(radio.get() == nullptr);
    sp<V1_6::IRadio> radioV16 = V1_6::IRadio::castFrom(radio).withDefault(nullptr);
    EXPECT_FALSE(radioV16.get() == nullptr);

    s_vendorFunctions = &sTestVendorFunctions;
    sVendorFunctionExcutor.setOnRequestListener(listener);

    return radioV16;
}

static void tearDown(sp<V1_6::IRadio>& radio) {
    radio = nullptr;
    sVendorFunctionExcutor.setOnRequestListener(nullptr);
}

TEST(ril_service_1_6, radio_proxy_body) {
    sp<V1_6::IRadio> radio = setupRadio();
    // using a proxy of a legacy radio interface
    EXPECT_TRUE(radio->getIccCardStatus(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->supplyIccPinForApp(GetRandomSerialNumber(), "1234", "aid").isOk());
    EXPECT_TRUE(radio->supplyIccPukForApp(GetRandomSerialNumber(), "12345678", "1234", "aid").isOk());
    EXPECT_TRUE(radio->supplyIccPin2ForApp(GetRandomSerialNumber(), "1234", "aid").isOk());
    EXPECT_TRUE(radio->supplyIccPuk2ForApp(GetRandomSerialNumber(), "12345678", "1234", "aid").isOk());
    EXPECT_TRUE(radio->changeIccPinForApp(GetRandomSerialNumber(), "1234", "0000", "aid").isOk());
    EXPECT_TRUE(radio->changeIccPin2ForApp(GetRandomSerialNumber(), "1234", "0000", "aid").isOk());
    EXPECT_TRUE(radio->supplyNetworkDepersonalization(GetRandomSerialNumber(), "1234").isOk());
    EXPECT_TRUE(radio->getCurrentCalls(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->dial(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->getImsiForApp(GetRandomSerialNumber(), "aid").isOk());
    EXPECT_TRUE(radio->hangup(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->hangupWaitingOrBackground(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->hangupForegroundResumeBackground(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->switchWaitingOrHoldingAndActive(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->conference(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->rejectCall(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getLastCallFailCause(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getSignalStrength(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getVoiceRegistrationState(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getDataRegistrationState(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getOperator(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setRadioPower(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->sendDtmf(GetRandomSerialNumber(), "0").isOk());
    EXPECT_TRUE(radio->sendSms(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->sendSMSExpectMore(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setupDataCall(GetRandomSerialNumber(), (V1_0::RadioTechnology)0,
            {}, false, true, false).isOk());
    EXPECT_TRUE(radio->iccIOForApp(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->sendUssd(GetRandomSerialNumber(), "uusd").isOk());
    EXPECT_TRUE(radio->cancelPendingUssd(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getClir(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setClir(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->getCallForwardStatus(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setCallForward(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->getCallWaiting(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setCallWaiting(GetRandomSerialNumber(), true, 0).isOk());
    EXPECT_TRUE(radio->acknowledgeLastIncomingGsmSms(GetRandomSerialNumber(), true,
            V1_0::SmsAcknowledgeFailCause::UNSPECIFIED_ERROR).isOk());
    EXPECT_TRUE(radio->acceptCall(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->deactivateDataCall(GetRandomSerialNumber(), 0, false).isOk());
    EXPECT_TRUE(radio->getFacilityLockForApp(GetRandomSerialNumber(),
            "1234", "1234", 0, "appid").isOk());
    EXPECT_TRUE(radio->setFacilityLockForApp(GetRandomSerialNumber(),
            "1234", false, "1234", 0, "appid").isOk());
    EXPECT_TRUE(radio->setBarringPassword(GetRandomSerialNumber(), "1234", "1234", "0000").isOk());
    EXPECT_TRUE(radio->getNetworkSelectionMode(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setNetworkSelectionModeAutomatic(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setNetworkSelectionModeManual(GetRandomSerialNumber(), "00101").isOk());
    EXPECT_TRUE(radio->getAvailableNetworks(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->startNetworkScan(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->stopNetworkScan(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->startDtmf(GetRandomSerialNumber(), "1").isOk());
    EXPECT_TRUE(radio->stopDtmf(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getBasebandVersion(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->separateConnection(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setMute(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->getMute(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getClip(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getDataCallList(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setSuppServiceNotifications(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->writeSmsToSim(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->deleteSmsOnSim(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setBandMode(GetRandomSerialNumber(), (V1_0::RadioBandMode)0).isOk());
    EXPECT_TRUE(radio->getAvailableBandModes(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->sendEnvelope(GetRandomSerialNumber(), "123").isOk());
    EXPECT_TRUE(radio->sendTerminalResponseToSim(GetRandomSerialNumber(), "123").isOk());
    EXPECT_TRUE(radio->handleStkCallSetupRequestFromSim(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->explicitCallTransfer(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setPreferredNetworkType(GetRandomSerialNumber(),
            (V1_0::PreferredNetworkType)0).isOk());
    EXPECT_TRUE(radio->getPreferredNetworkType(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getNeighboringCids(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setLocationUpdates(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->setCdmaSubscriptionSource(GetRandomSerialNumber(),
            (V1_0::CdmaSubscriptionSource)0).isOk());
    EXPECT_TRUE(radio->setCdmaRoamingPreference(GetRandomSerialNumber(),
            (V1_0::CdmaRoamingType)0).isOk());
    EXPECT_TRUE(radio->getCdmaRoamingPreference(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setTTYMode(GetRandomSerialNumber(), (V1_0::TtyMode)0).isOk());
    EXPECT_TRUE(radio->getTTYMode(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setPreferredVoicePrivacy(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->getPreferredVoicePrivacy(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->sendCDMAFeatureCode(GetRandomSerialNumber(), "code").isOk());
    EXPECT_TRUE(radio->sendBurstDtmf(GetRandomSerialNumber(), "1234", 1, 0).isOk());
    EXPECT_TRUE(radio->sendCdmaSms(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->acknowledgeLastIncomingCdmaSms(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->getGsmBroadcastConfig(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setGsmBroadcastConfig(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setGsmBroadcastActivation(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->getCdmaBroadcastConfig(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setCdmaBroadcastConfig(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setCdmaBroadcastActivation(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->getCDMASubscription(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->writeSmsToRuim(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->deleteSmsOnRuim(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->getDeviceIdentity(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->exitEmergencyCallbackMode(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getSmscAddress(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setSmscAddress(GetRandomSerialNumber(), "123").isOk());
    EXPECT_TRUE(radio->reportSmsMemoryStatus(GetRandomSerialNumber(), false).isOk());
    EXPECT_TRUE(radio->reportStkServiceIsRunning(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getCdmaSubscriptionSource(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->requestIsimAuthentication(GetRandomSerialNumber(), "123").isOk());
    EXPECT_TRUE(radio->acknowledgeIncomingGsmSmsWithPdu(GetRandomSerialNumber(), true, "1234").isOk());
    EXPECT_TRUE(radio->sendEnvelopeWithStatus(GetRandomSerialNumber(), "1234").isOk());
    EXPECT_TRUE(radio->getVoiceRadioTechnology(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getCellInfoList(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setCellInfoListRate(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setInitialAttachApn(GetRandomSerialNumber(), {}, false, false).isOk());
    EXPECT_TRUE(radio->getImsRegistrationState(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->sendImsSms(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->iccTransmitApduBasicChannel(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->iccOpenLogicalChannel(GetRandomSerialNumber(), "aid", 0).isOk());
    EXPECT_TRUE(radio->iccCloseLogicalChannel(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->iccTransmitApduLogicalChannel(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->nvReadItem(GetRandomSerialNumber(), (V1_0::NvItem)0).isOk());
    EXPECT_TRUE(radio->nvWriteItem(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->nvWriteCdmaPrl(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->nvResetConfig(GetRandomSerialNumber(), (V1_0::ResetNvType)0).isOk());
    EXPECT_TRUE(radio->setUiccSubscription(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setDataAllowed(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->getHardwareConfig(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->requestIccSimAuthentication(GetRandomSerialNumber(), 0, "1234", "aid").isOk());
    EXPECT_TRUE(radio->setDataProfile(GetRandomSerialNumber(), {}, false).isOk());
    EXPECT_TRUE(radio->requestShutdown(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getRadioCapability(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setRadioCapability(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->startLceService(GetRandomSerialNumber(), 0, false).isOk());
    EXPECT_TRUE(radio->stopLceService(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->pullLceData(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getModemActivityInfo(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setAllowedCarriers(GetRandomSerialNumber(), true, {}).isOk());
    EXPECT_TRUE(radio->getAllowedCarriers(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->sendDeviceState(GetRandomSerialNumber(), (V1_0::DeviceStateType)0, true).isOk());
    EXPECT_TRUE(radio->setIndicationFilter(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setSimCardPower(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->responseAcknowledgement().isOk());
    EXPECT_TRUE(radio->setSimCardPower_1_1(GetRandomSerialNumber(), (V1_1::CardPowerState)0).isOk());
    EXPECT_TRUE(radio->setCarrierInfoForImsiEncryption(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->startKeepalive(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->stopKeepalive(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->startNetworkScan_1_2(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setIndicationFilter_1_2(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setSignalStrengthReportingCriteria(GetRandomSerialNumber(), 0, 0, {},
            (V1_2::AccessNetwork)0).isOk());
    EXPECT_TRUE(radio->setLinkCapacityReportingCriteria(GetRandomSerialNumber(), 0, 0, 0,
            {}, {}, (V1_2::AccessNetwork)0).isOk());
    EXPECT_TRUE(radio->setupDataCall_1_2(GetRandomSerialNumber(), (V1_2::AccessNetwork)0,
            {}, false, true,
            false, (V1_2::DataRequestReason)0, {}, {}).isOk());
    EXPECT_TRUE(radio->deactivateDataCall_1_2(GetRandomSerialNumber(), 0,
            (V1_2::DataRequestReason)1).isOk());
    EXPECT_TRUE(radio->setSystemSelectionChannels(GetRandomSerialNumber(), true, {}).isOk());
    EXPECT_TRUE(radio->enableModem(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->getModemStackStatus(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setupDataCall_1_4(GetRandomSerialNumber(), (V1_4::AccessNetwork)0, {}, false,
            (V1_2::DataRequestReason)0, {}, {}).isOk());
    EXPECT_TRUE(radio->setInitialAttachApn_1_4(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setDataProfile_1_4(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->emergencyDial(GetRandomSerialNumber(), {}, 0, {},
            (V1_4::EmergencyCallRouting)0,
            true, false).isOk());
    EXPECT_TRUE(radio->startNetworkScan_1_4(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->getPreferredNetworkTypeBitmap(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setPreferredNetworkTypeBitmap(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->setAllowedCarriers_1_4(GetRandomSerialNumber(), {},
            (V1_4::SimLockMultiSimPolicy)0).isOk());
    EXPECT_TRUE(radio->getAllowedCarriers_1_4(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getSignalStrength_1_4(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setSignalStrengthReportingCriteria_1_5(GetRandomSerialNumber(), {},
            (V1_5::AccessNetwork)0).isOk());
    EXPECT_TRUE(radio->enableUiccApplications(GetRandomSerialNumber(), true).isOk());
    EXPECT_TRUE(radio->areUiccApplicationsEnabled(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setSystemSelectionChannels_1_5(GetRandomSerialNumber(), true, {}).isOk());
    EXPECT_TRUE(radio->startNetworkScan_1_5(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setupDataCall_1_5(GetRandomSerialNumber(), (V1_5::AccessNetwork)0, {}, false,
            (V1_2::DataRequestReason)0, {}, {}).isOk());
    EXPECT_TRUE(radio->setInitialAttachApn_1_5(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setDataProfile_1_5(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->setRadioPower_1_5(GetRandomSerialNumber(), false, false, false).isOk());
    EXPECT_TRUE(radio->setIndicationFilter_1_5(GetRandomSerialNumber(), 0).isOk());
    EXPECT_TRUE(radio->getBarringInfo(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getVoiceRegistrationState_1_5(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->getDataRegistrationState_1_5(GetRandomSerialNumber()).isOk());
    EXPECT_TRUE(radio->setNetworkSelectionModeManual_1_5(GetRandomSerialNumber(), "00101",
            (V1_5::RadioAccessNetworks)0).isOk());
    EXPECT_TRUE(radio->sendCdmaSmsExpectMore(GetRandomSerialNumber(), {}).isOk());
    EXPECT_TRUE(radio->supplySimDepersonalization(GetRandomSerialNumber(), (V1_5::PersoSubstate)0, "").isOk());
    EXPECT_TRUE(radio->setLinkCapacityReportingCriteria_1_5(GetRandomSerialNumber(), 0, 0, 0,
            {}, {}, (V1_5::AccessNetwork)0).isOk());
    tearDown(radio);
}

TEST(ril_service_1_6, getDataCallList_1_6) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getDataCallList_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST,
                            HAL_VERSION_CODE(1, 6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getDataCallList_1_6(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, setupDataCall_1_6) {
    V1_5::DataProfileInfo testVector = {};
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::setupDataCall_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_SetupDataCallInfo_V1_6));
            if (data) {
                RIL_SetupDataCallInfo_V1_6& setupDataCallInfo = *(RIL_SetupDataCallInfo_V1_6 *)data;
                EXPECT_EQ(setupDataCallInfo.accessNetwork, (int)V1_5::AccessNetwork::EUTRAN);
                EXPECT_EQ(setupDataCallInfo.roamingAllow, true);
                EXPECT_EQ(setupDataCallInfo.reason, (int)V1_2::DataRequestReason::NORMAL);
            }

            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->setupDataCall_1_6(serial, V1_5::AccessNetwork::EUTRAN,
            testVector, true, V1_2::DataRequestReason::NORMAL,
            {}, {}, 0, {}, {}, true);
    tearDown(radio);
}

TEST(ril_service_1_6, setNrDualConnectivityState) {
    static struct {
        V1_6::NrDualConnectivityState test;
        int nrState;
    } testVector[] = {
        { V1_6::NrDualConnectivityState::DISABLE, 0 },
        { V1_6::NrDualConnectivityState::ENABLE, 1 },
        { V1_6::NrDualConnectivityState::DISABLE_IMMEDIATE, 2 },
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    static size_t i;
    for (i = 0; i < size; i++) {
        struct OnRequestListenerImpl : public OnRequestListener {
            void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
                dlog("V1_6::IRadio::setNrDualConnectivityState");
                EXPECT_EQ(request, RIL_REQUEST_ENABLE_NR_DUAL_CONNECTIVITY);
                EXPECT_NE(data, nullptr);
                EXPECT_EQ(datalen, sizeof(int));
                EXPECT_EQ(*(int *)data, testVector[i].nrState);
                RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
            }
        } callback;
        int32_t serial = GetRandomSerialNumber();
        sp<V1_6::IRadio> radio = setupRadio(&callback);
        radio->setNrDualConnectivityState(serial, testVector[i].test);
        tearDown(radio);
    }
}

TEST(ril_service_1_6, isNrDualConnectivityEnabled) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::isNrDualConnectivityEnabled");
            EXPECT_EQ(request, RIL_REQUEST_IS_NR_DUAL_CONNECTIVITY_ENABLED);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->isNrDualConnectivityEnabled(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, setRadioPower_1_6) {
    static struct {
        bool powerOn;
        bool forEmergencyCall;
        bool preferredForEmergencyCall;
    } testVector[] = {
        { false, false, false },
        { true, true, false },
        { true, false, true },
        { true, true, true },
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    static size_t i;
    for (i = 0; i < size; i++) {
        struct OnRequestListenerImpl : public OnRequestListener {
            void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
                dlog("V1_6::IRadio::setRadioPower_1_6");
                EXPECT_EQ(request,
                        ENCODE_REQUEST(RIL_REQUEST_RADIO_POWER, HAL_VERSION_CODE(1,6)));
                EXPECT_NE(data, nullptr);
                EXPECT_EQ(datalen, sizeof(int) * 3);
                if (data) {
                    EXPECT_EQ(((int *)data)[0], testVector[i].powerOn);
                    EXPECT_EQ(((int *)data)[1], testVector[i].forEmergencyCall);
                    EXPECT_EQ(((int *)data)[2], testVector[i].preferredForEmergencyCall);
                }
                RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
            }
        } callback;
        int32_t serial = GetRandomSerialNumber();
        sp<V1_6::IRadio> radio = setupRadio(&callback);
        radio->setRadioPower_1_6(serial, testVector[i].powerOn,
                testVector[i].forEmergencyCall,
                testVector[i].preferredForEmergencyCall);
        tearDown(radio);
    }
}

TEST(ril_service_1_6, sendSms_1_6) {
    static V1_0::GsmSmsMessage testVector = {
        "0123456", "987654321"
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::sendSms_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SEND_SMS, HAL_VERSION_CODE(1,6)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            if (data) {
                EXPECT_TRUE(((char **)data)[0] == testVector.smscPdu);
                EXPECT_TRUE(((char **)data)[1] == testVector.pdu);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->sendSms_1_6(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, sendSmsExpectMore_1_6) {
    static V1_0::GsmSmsMessage testVector = {
        "0123456", "987654321"
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::sendSmsExpectMore_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SEND_SMS_EXPECT_MORE, HAL_VERSION_CODE(1,6)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(char *) * 2);
            if (data) {
                EXPECT_TRUE(((char **)data)[0] == testVector.smscPdu);
                EXPECT_TRUE(((char **)data)[1] == testVector.pdu);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->sendSmsExpectMore_1_6(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, sendCdmaSms_1_6) {
    static V1_0::CdmaSmsMessage testVector = {
        1, true, 2, {}, {}, {},
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::sendCdmaSms_1_6");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SEND_SMS);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_SMS_Message));
            if (data) {
                RIL_CDMA_SMS_Message& rcsm = *(RIL_CDMA_SMS_Message *)data;
                EXPECT_EQ(rcsm.uTeleserviceID, testVector.teleserviceId);
                EXPECT_EQ(rcsm.bIsServicePresent, testVector.isServicePresent);
                EXPECT_EQ(rcsm.uServicecategory, testVector.serviceCategory);
                EXPECT_EQ((unsigned long)/* NEED TO CHECK */ rcsm.uBearerDataLen, testVector.bearerData.size());
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->sendCdmaSms_1_6(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, sendCdmaSmsExpectMore_1_6) {
    static V1_0::CdmaSmsMessage testVector = {
        1, true, 2, {}, {}, {},
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::sendCdmaSmsExpectMore_1_6");
            EXPECT_EQ(request, RIL_REQUEST_CDMA_SEND_SMS_EXPECT_MORE);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CDMA_SMS_Message));
            if (data) {
                RIL_CDMA_SMS_Message& rcsm = *(RIL_CDMA_SMS_Message *)data;
                EXPECT_EQ(rcsm.uTeleserviceID, testVector.teleserviceId);
                EXPECT_EQ(rcsm.bIsServicePresent, testVector.isServicePresent);
                EXPECT_EQ(rcsm.uServicecategory, testVector.serviceCategory);
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ rcsm.uBearerDataLen, testVector.bearerData.size());
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->sendCdmaSmsExpectMore_1_6(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, setSimCardPower_1_6) {
    static V1_1::CardPowerState testVector = V1_1::CardPowerState::POWER_UP;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::setSimCardPower_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SET_SIM_CARD_POWER, HAL_VERSION_CODE(1,6)));
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            if (data) {
                EXPECT_EQ(((int *)data)[0], (int)testVector);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->setSimCardPower_1_6(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, allocatePduSessionId) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::allocatePduSessionId");
            EXPECT_EQ(request, RIL_REQUEST_ALLOCATE_PDU_SESSION_ID);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->allocatePduSessionId(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, releasePduSessionId) {
    static int testVector = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::releasePduSessionId");
            EXPECT_EQ(request, RIL_REQUEST_RELEASE_PDU_SESSION_ID);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            if (data) {
                EXPECT_EQ(((int *)data)[0], (int)testVector);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->releasePduSessionId(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, startHandover) {
    static int testVector = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::startHandover");
            EXPECT_EQ(request, RIL_REQUEST_START_HANDOVER);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            if (data) {
                EXPECT_EQ(((int *)data)[0], (int)testVector);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->startHandover(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, cancelHandover) {
    static int testVector = 1;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::cancelHandover");
            EXPECT_EQ(request, RIL_REQUEST_CANCEL_HANDOVER);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            if (data) {
                EXPECT_EQ(((int *)data)[0], (int)testVector);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->cancelHandover(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, setAllowedNetworkTypesBitmap) {
    static hidl_bitfield<V1_4::RadioAccessFamily> testVector = 0x1FFFF;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::setAllowedNetworkTypesBitmap");
            EXPECT_EQ(request, RIL_REQUEST_SET_ALLOWED_NETWORK_TYPES_BITMAP);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(int));
            if (data) {
                EXPECT_EQ(((int *)data)[0], (int)testVector);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->setAllowedNetworkTypesBitmap(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, setDataThrottling) {
    static V1_6::DataThrottlingAction testVector =
            V1_6::DataThrottlingAction::THROTTLE_SECONDARY_CARRIER;
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::setDataThrottling");
            EXPECT_EQ(request, RIL_REQUEST_SET_DATA_THROTTLING);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_DataThrottling));
            if (data) {
                RIL_DataThrottling& dataThrottling = *(RIL_DataThrottling *)data;
                EXPECT_EQ(dataThrottling.dataThrottlingAction, (int)testVector);
                EXPECT_TRUE(dataThrottling.completionDurationMillis == 1000LL);
            }
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->setDataThrottling(serial, testVector, 1000LL);
    tearDown(radio);
}

TEST(ril_service_1_6, getSystemSelectionChannels) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getSystemSelectionChannels");
            EXPECT_EQ(request, RIL_REQUEST_GET_SYSTEM_SELECTION_CHANNELS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getSystemSelectionChannels(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getCurrentCalls_1_6) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getCurrentCalls_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_GET_CURRENT_CALLS, HAL_VERSION_CODE(1,6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getCurrentCalls_1_6(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getVoiceRegistrationState_1_6) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getVoiceRegistrationState_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_VOICE_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getVoiceRegistrationState_1_6(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getDataRegistrationState_1_6) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getDataRegistrationState_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_DATA_REGISTRATION_STATE, HAL_VERSION_CODE(1,6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getDataRegistrationState_1_6(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getCellInfoList_1_6) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getCellInfoList_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_GET_CELL_INFO_LIST, HAL_VERSION_CODE(1,6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getCellInfoList_1_6(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getSignalStrength_1_6) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getSignalStrength_1_6");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SIGNAL_STRENGTH, HAL_VERSION_CODE(1,6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getSignalStrength_1_6(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getAllowedNetworkTypesBitmap) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getAllowedNetworkTypesBitmap");
            EXPECT_EQ(request,RIL_REQUEST_GET_ALLOWED_NETWORK_TYPES_BITMAP);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getAllowedNetworkTypesBitmap(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, emergencyDial_1_6) {
    static struct {
        V1_0::Dial dialInfo;
        hidl_bitfield<V1_4::EmergencyServiceCategory> categories;
        hidl_vec<hidl_string> urns;
        V1_4::EmergencyCallRouting routing;
        bool hasKnownUserIntentEmergency;
        bool isTesting;
    } testVector[] = {
        {
            {
                "1234567", V1_0::Clir::DEFAULT,
                { {V1_0::UusType::TYPE1_IMPLICIT, V1_0::UusDcs::USP, "0000"}, }
            },
            (int)V1_4::EmergencyServiceCategory::POLICE,
            { "urns1", "urns2" },
            V1_4::EmergencyCallRouting::EMERGENCY,
            true, false,
        },
        {
            {
                "1234567", V1_0::Clir::DEFAULT,
                { {V1_0::UusType::TYPE1_IMPLICIT, V1_0::UusDcs::USP, ""}, }
            },
            (int)V1_4::EmergencyServiceCategory::POLICE,
            { "urns1", "urns2" },
            V1_4::EmergencyCallRouting::EMERGENCY,
            true, false,
        },
    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    static size_t i;
    for (i = 0; i < size; i++) {
        struct OnRequestListenerImpl : public OnRequestListener {
            void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
                dlog("V1_6::IRadio::emergencyDial_1_6");
                EXPECT_EQ(request, RIL_REQUEST_EMERGENCY_DIAL);
                EXPECT_NE(data, nullptr);
                EXPECT_EQ(datalen, sizeof(RIL_EmergencyDial));
                if (data) {
                    RIL_EmergencyDial& emergencyDialInfo = *(RIL_EmergencyDial *)data;
                    RIL_Dial &dial = emergencyDialInfo.dialInfo;
                    EXPECT_TRUE(dial.address == testVector[i].dialInfo.address);
                }

                RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
            }
        } callback;
        int32_t serial = GetRandomSerialNumber();
        sp<V1_6::IRadio> radio = setupRadio(&callback);
        radio->emergencyDial_1_6(serial, testVector[i].dialInfo, testVector[i].categories,
                testVector[i].urns, testVector[i].routing, testVector[i].hasKnownUserIntentEmergency,
                testVector[i].isTesting);
        tearDown(radio);
    }
}

TEST(ril_service_1_6, getSlicingConfig) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getSlicingConfig");
            EXPECT_EQ(request,
                    ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1,6)));
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getSlicingConfig(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, setCarrierInfoForImsiEncryption_1_6) {
    static V1_6::ImsiEncryptionInfo testVector = {
        {
            "450", "05",
            { 0x1, 0x2, 0x3, 0x4, 0x5 },
            "keyIdentifier",
            (int64_t)7777,
        },
        V1_6::PublicKeyType::EPDG,
    };
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::setCarrierInfoForImsiEncryption_1_6");
            EXPECT_EQ(request, RIL_REQUEST_SET_CARRIER_INFO_IMSI_ENCRYPTION);
            EXPECT_NE(data, nullptr);
            EXPECT_EQ(datalen, sizeof(RIL_CarrierInfoForImsiEncryption_1_6));
            RIL_CarrierInfoForImsiEncryption_1_6& imsiEncryption =
                    *(RIL_CarrierInfoForImsiEncryption_1_6 *)data;
            test(*(RIL_CarrierInfoForImsiEncryption *)&imsiEncryption, testVector.base);
            EXPECT_EQ((int)imsiEncryption.keyType, (int)testVector.keyType);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }

        void test(const RIL_CarrierInfoForImsiEncryption& test,
                const V1_1::ImsiEncryptionInfo& imsiEncryption) {
            EXPECT_TRUE(test.mcc ==  imsiEncryption.mcc);
            EXPECT_TRUE(test.mnc ==  imsiEncryption.mnc);
            EXPECT_TRUE(test.keyIdentifier ==  imsiEncryption.keyIdentifier);
            EXPECT_EQ((unsigned long)/* NEED TO CHECK */ test.carrierKeyLength, imsiEncryption.carrierKey.size());
            if (test.carrierKey) {
                hidl_vec<uint8_t> tmp;
                tmp.setToExternal(test.carrierKey, test.carrierKeyLength);
                EXPECT_TRUE(tmp == imsiEncryption.carrierKey);
            }
            EXPECT_TRUE(test.expirationTime == imsiEncryption.expirationTime);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->setCarrierInfoForImsiEncryption_1_6(serial, testVector);
    tearDown(radio);
}

TEST(ril_service_1_6, getSimPhonebookRecords) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getSimPhonebookRecords");
            EXPECT_EQ(request, RIL_REQUEST_GET_SIM_PHONEBOOK_RECORDS);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getSimPhonebookRecords(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, getSimPhonebookCapacity) {
    struct OnRequestListenerImpl : public OnRequestListener {
        void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID) {
            dlog("V1_6::IRadio::getSimPhonebookCapacity");
            EXPECT_EQ(request, RIL_REQUEST_GET_SIM_PHONEBOOK_CAPACITY);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(datalen, 0UL);
            RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
        }
    } callback;
    int32_t serial = GetRandomSerialNumber();
    sp<V1_6::IRadio> radio = setupRadio(&callback);
    radio->getSimPhonebookCapacity(serial);
    tearDown(radio);
}

TEST(ril_service_1_6, updateSimPhonebookRecords) {
    static V1_6::PhonebookRecordInfo testVector[] = {
        {
            0,
            "contact1",
            "1234567",
            { "test@samsung.com", "test2@google.com" },
            { "000000", "111111" },
        },
        {
            1,
            "",
            "",
            {},
            {},
        },
        {
            2,
            "contact2",
            "9876543",
            { "update1@samsung.com", "update2@google.com" },
            { "000000", "111111" },
        },

    };
    static size_t size = NUM_OF_ELEMENTS(testVector);
    static size_t i;
    for (i = 0; i < size; i++) {
        struct OnRequestListenerImpl : public OnRequestListener {
            void onRequest(int request, void *data, size_t /*datalen*/, RIL_Token t, RIL_SOCKET_ID) {
                EXPECT_EQ(request, RIL_REQUEST_UPDATE_SIM_PHONEBOOK_RECORDS);
                EXPECT_NE(data, nullptr);
                RIL_UpdatePbEntry *updateReq = (RIL_UpdatePbEntry *)data;
                EXPECT_EQ(updateReq->type, PB_ADN_3G);
                //EXPECT_EQ(datalen, 16); // ??? length = sizeof(int) * 4
                EXPECT_EQ((unsigned int)/* NEED TO CHECK */ updateReq->recordId, testVector[i].recordId);
                if (testVector[i].recordId == 0) {
                    EXPECT_EQ(updateReq->mode, PB_INSERT);
                } else {
                    EXPECT_TRUE(updateReq->mode == PB_DELETE || updateReq->mode == PB_UPDATE);
                }
                RIL_onRequestComplete(t, RIL_E_INTERNAL_ERR, 0, 0);
            }

        } callback;
        int32_t serial = GetRandomSerialNumber();
        sp<V1_6::IRadio> radio = setupRadio(&callback);
        radio->updateSimPhonebookRecords(serial, testVector[i]);
        tearDown(radio);
    }
}
