/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#pragma once

#include <radiomodemresponse.h>
#include <radiodataresponse.h>
#include <radioconfigresponse.h>
#include <radiosimresponse.h>
#include <radionetworkresponse.h>
#include <radiomessagingresponse.h>
#include <radiovoiceresponse.h>

struct RadioResponse {
    uint32_t mSlot;
    std::shared_ptr<RadioModemResponse> mModem;
    std::shared_ptr<RadioModemResponse> modem() { return mModem; }
    void modem(const std::shared_ptr<RadioModemResponse>& response) { mModem = response; }

    //DATA
    std::shared_ptr<RadioDataResponse> mData;
    std::shared_ptr<RadioDataResponse> data() { return mData; }
    void data(const std::shared_ptr<RadioDataResponse>& response) { mData = response; }

    //SIM
    std::shared_ptr<RadioSimResponse> mSim;
    std::shared_ptr<RadioSimResponse> sim() { return mSim; }
    void sim(const std::shared_ptr<RadioSimResponse>& response) { mSim = response; }

    //MESSAGING
    std::shared_ptr<RadioMessagingResponse> mMessaging;
    std::shared_ptr<RadioMessagingResponse> messaging() { return mMessaging; }
    void messaging(const std::shared_ptr<RadioMessagingResponse>& response) { mMessaging = response; }

    void config(const std::shared_ptr<RadioConfigResponse>& response) { mConfig = response; }

    //Network
    std::shared_ptr<RadioNetworkResponse> mNetwork;
    std::shared_ptr<RadioNetworkResponse> network() { return mNetwork; }
    void network(const std::shared_ptr<RadioNetworkResponse>& response) { mNetwork = response; }

    // CONFIG
    std::shared_ptr<RadioConfigResponse> mConfig;
    std::shared_ptr<RadioConfigResponse> config() { return mConfig; }

    // VOICE
    std::shared_ptr<RadioVoiceResponse> mVoice;
    std::shared_ptr<RadioVoiceResponse> voice() { return mVoice; }
    void voice(const std::shared_ptr<RadioVoiceResponse>& response) { mVoice = response; }

    static std::shared_ptr<RadioResponse> make(uint32_t slot) {
        auto hook = std::make_shared<RadioResponse>();
        hook->mSlot = slot;
        hook->modem(RadioModemResponse::make());
        hook->data(RadioDataResponse::make());
        hook->config(RadioConfigResponse::make());
        hook->sim(RadioSimResponse::make());
        hook->network(RadioNetworkResponse::make());
        hook->messaging(RadioMessagingResponse::make());
        hook->voice(RadioVoiceResponse::make());

        // TODO others
        return hook;
    }

};

struct RadioIndication {
    uint32_t mSlot;
    std::shared_ptr<RadioModemIndication> mModem;
    std::shared_ptr<RadioConfigIndication> mConfig;

    std::shared_ptr<RadioModemIndication> modem() { return mModem; }
    std::shared_ptr<RadioConfigIndication> config() { return mConfig; }

    void modem(const std::shared_ptr<RadioModemIndication>& response) { mModem = response; }
    void config(const std::shared_ptr<RadioConfigIndication>& response) { mConfig = response; }

    //DATA
    std::shared_ptr<RadioDataIndication> mData;
    std::shared_ptr<RadioDataIndication> data() { return mData; }
    void data(const std::shared_ptr<RadioDataIndication>& response) { mData = response; }

    // SIM
    std::shared_ptr<RadioSimIndication> mSim;
    std::shared_ptr<RadioSimIndication> sim() { return mSim; }
    void sim(const std::shared_ptr<RadioSimIndication>& response) { mSim = response; }

    std::shared_ptr<RadioNetworkIndication> mNetwork;
    std::shared_ptr<RadioNetworkIndication> network() { return mNetwork; }
    void network(const std::shared_ptr<RadioNetworkIndication>& response) { mNetwork = response; }

    //MESSAGING
    std::shared_ptr<RadioMessagingIndication> mMessaging;
    std::shared_ptr<RadioMessagingIndication> messaging() { return mMessaging; }
    void messaging(const std::shared_ptr<RadioMessagingIndication>& response) { mMessaging = response; }

    // VOICE
    std::shared_ptr<RadioVoiceIndication> mVoice;
    std::shared_ptr<RadioVoiceIndication> voice() { return mVoice; }
    void voice(const std::shared_ptr<RadioVoiceIndication>& response) { mVoice = response; }

    static std::shared_ptr<RadioIndication> make(uint32_t slot) {
        auto hook = std::make_shared<RadioIndication>();
        hook->mSlot = slot;
        hook->modem(RadioModemIndication::make());
        hook->data(RadioDataIndication::make());
        hook->config(RadioConfigIndication::make());
        hook->sim(RadioSimIndication::make());
        hook->network(RadioNetworkIndication::make());
        hook->messaging(RadioMessagingIndication::make());
        hook->voice(RadioVoiceIndication::make());

        // TODO others
        return hook;
    }
};
