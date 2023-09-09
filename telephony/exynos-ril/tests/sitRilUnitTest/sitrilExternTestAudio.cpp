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

#include <libsitril-audio/rilaudioapi.h>
#include <slsi/ril_client.h>
#include "sitrilExternTestAudio.h"

int onIndCallbackForAudio(HANDLE handle, int event, const void * /*data*/, unsigned int /*datalen*/) {
    int ret = -1;
    //printf("[%s] (%d) Indication comes, size(%u)\n", __FUNCTION__, event, datalen);
    //SitrilExternTest *_externTest = SitrilExternTest::GetInstance();
    SitrilExternTest *_externTest = (SitrilExternTest *)handle;
    if (_externTest != NULL) {
        _externTest->setIndMsgId(event);
        ret = 0;
    } else {
        printf("[%s] externTest is null\n", __FUNCTION__);
    }
    return ret;
}

SitrilExternApiAudio::SitrilExternApiAudio() {
    ril_register_callback = nullptr;
    ril_set_audio_volume = nullptr;
    ril_set_audio_path = nullptr;
    ril_set_multi_mic = nullptr;
    ril_set_mute = nullptr;
    ril_set_audio_clock = nullptr;
    ril_set_audio_loopback = nullptr;
    ril_set_tty_mode = nullptr;
}

void SitrilExternTestAudio::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilExternTest = new SitrilExternTest(RIL_EXTERN_AUDIO, &mApi);
    SitrilExternTest::SetInstance(mSitRilExternTest);

    void *handle = mSitRilExternTest->getHandle();
    if (handle) {
        mApi.ril_open_client        = (void* (*)(const struct RILC_EnvUnitTest *env))dlsym(handle, "RilAudioOpenUnitTest");
        mApi.ril_close_client       = (int (*)(void))dlsym(handle, "RilAudioClose");
        mApi.ril_register_callback  = (int (*)(void *, int *))dlsym(handle, "RegisterEventCallback");
        mApi.ril_set_audio_volume   = (int (*)(int))dlsym(handle, "SetAudioVolume");
        mApi.ril_set_audio_path     = (int (*)(int))dlsym(handle, "SetAudioPath");
        mApi.ril_set_multi_mic      = (int (*)(int))dlsym(handle, "SetMultiMic");
        mApi.ril_set_mute           = (int (*)(int))dlsym(handle, "SetMute");
        mApi.ril_set_audio_clock    = (int (*)(int))dlsym(handle, "SetAudioClock");
        mApi.ril_set_audio_loopback = (int (*)(int, int))dlsym(handle, "SetAudioLoopback");
        mApi.ril_set_tty_mode       = (int (*)(int))dlsym(handle, "SetTtyMode");
    }

    mSitRilExternTest->openClient();
}

void SitrilExternTestAudio::TearDown() {
    mSitRilExternTest->closeClient();
    mSitRilExternTest->resetTestData();

    delete mSitRilExternTest;
    mSitRilExternTest = nullptr;
    SitrilExternTest::SetInstance(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////
TEST_P(SitrilExternTestAudio, setAudioVolume) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_audio_volume);

    int volume = 10;
    int ret =  mApi.ril_set_audio_volume(volume);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_VOLUME, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_VOLUME, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

TEST_P(SitrilExternTestAudio, setAudioPath) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_audio_path);

    int audioPath = RILAUIDO_PATH_NONE;    // invalid value
    int ret =  mApi.ril_set_audio_path(audioPath);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    audioPath = RILAUIDO_PATH_HANDSET;
    ret =  mApi.ril_set_audio_path(audioPath);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_PATH, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_PATH, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

TEST_P(SitrilExternTestAudio, setMutiMic) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_multi_mic);

    int multiMic = -1;    // invalid value
    int ret =  mApi.ril_set_multi_mic(multiMic);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    multiMic = RILAUDIO_MULTI_MIC_OFF;
    ret =  mApi.ril_set_multi_mic(multiMic);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_MIC, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_MIC, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

TEST_P(SitrilExternTestAudio, setMute) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_mute);

    int muteMode = -1;    // invalid value
    int ret =  mApi.ril_set_mute(muteMode);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    muteMode = RILAUDIO_MUTE_DISABLED;
    ret =  mApi.ril_set_mute(muteMode);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_MUTE, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_MUTE, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

TEST_P(SitrilExternTestAudio, setAudioClock) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_audio_clock);

    int clockMode = -1;    // invalid value
    int ret =  mApi.ril_set_audio_clock(clockMode);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    clockMode = RILAUDIO_TURN_OFF_I2S;
    ret =  mApi.ril_set_audio_clock(clockMode);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_AUDIO_CLOCK, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_AUDIO_CLOCK, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

TEST_P(SitrilExternTestAudio, setAudioLoopback) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_audio_loopback);

    int loopbackMode = -1;    // invalid value
    int loopbackPath = -1;    // invalid value
    int ret =  mApi.ril_set_audio_loopback(loopbackMode, loopbackPath);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    loopbackMode = RILAUDIO_LOOPBACK_STOP;
    ret =  mApi.ril_set_audio_loopback(loopbackMode, loopbackPath);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    loopbackPath = RILAUDIO_LOOPBACK_PATH_HEADSET;
    ret =  mApi.ril_set_audio_loopback(loopbackMode, loopbackPath);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_AUDIO_LOOPBACK, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_AUDIO_LOOPBACK, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

TEST_P(SitrilExternTestAudio, setTtyMode) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_set_tty_mode);

    int ttyMode = 10;    // invalid value
    int ret =  mApi.ril_set_tty_mode(ttyMode);
    EXPECT_EQ(RILAUDIO_ERROR_INVALID_PARAM, ret);

    ttyMode = TTY_MODE_OFF;
    ret =  mApi.ril_set_tty_mode(ttyMode);
    EXPECT_EQ(RILAUDIO_ERROR_NONE, ret);

    SitrilExternCmdData *reqData = mSitRilExternTest->getReqData();
    ASSERT_NE(nullptr, reqData);
    EXPECT_EQ((unsigned int)RILC_REQ_AUDIO_SET_TTY_MODE, reqData->mMsgId);

    char data[3] = "AB";
    mSitRilExternTest->sendRespone(RILC_STATUS_SUCCESS, RILC_REQ_AUDIO_SET_TTY_MODE, data, 3, 0);

    //rsp data checking is not needed because data is bypassed in most scenario.
}

////////////////////////////////////////////////////////////////////////////////////////
TEST_P(SitrilExternTestAudio, indProcess) {
    ASSERT_EQ(true, mSitRilExternTest->getConnection());
    ASSERT_NE(nullptr, mApi.ril_register_callback);

    int ret = mApi.ril_register_callback((void*)mSitRilExternTest, (int *)onIndCallbackForAudio);
    ASSERT_EQ(RILAUDIO_ERROR_NONE, ret);

    int data[2] = {1, 2};
    mSitRilExternTest->setIndMsgId(-1);
    mSitRilExternTest->sendIndication(RILC_UNSOL_IMS_SRVCC_HO, data, sizeof(data), 0);
    EXPECT_EQ(RILAUDIO_EVENT_IMS_SRVCC_HANDOVER, mSitRilExternTest->getIndMsgId());

    mSitRilExternTest->setIndMsgId(-1);
    mSitRilExternTest->sendIndication(RILC_UNSOL_AUDIO_RINGBACK, data, sizeof(data), 0);
    EXPECT_EQ(-1, mSitRilExternTest->getIndMsgId());

    mSitRilExternTest->setIndMsgId(-1);
    mSitRilExternTest->sendIndication(RILC_UNSOL_AUDIO_RINGBACK_BY_NETWORK, data, sizeof(data), 0);
    EXPECT_EQ(RILAUDIO_EVENT_RINGBACK_STATE_CHANGED, mSitRilExternTest->getIndMsgId());

    mSitRilExternTest->setIndMsgId(-1);
    mSitRilExternTest->sendIndication(RILC_UNSOL_WB_AMR_REPORT, data, sizeof(data), 0);
    EXPECT_EQ(RILAUDIO_EVENT_WB_AMR_REPORT, mSitRilExternTest->getIndMsgId());
}
