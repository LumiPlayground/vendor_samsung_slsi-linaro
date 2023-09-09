/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#define LOG_TAG "UiccHal-SE"

#include <inttypes.h>
#include <utils/Log.h>
#include "sitril-client.h"
#include "interface.h"
#include "secureelement.h"
#include "types.h"
#include "utils.h"

#define LogD(format, ...)    ALOGD("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogE(format, ...)    ALOGE("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogW(format, ...)    ALOGW("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogI(format, ...)    ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)
#define LogV(format, ...)    ALOGI("%s() " format, __FUNCTION__, ##__VA_ARGS__)

#define ENTER_FUNC()        { ALOGD("%s() [<-- ", __FUNCTION__); }
#define LEAVE_FUNC()        { ALOGD("%s() [--> ", __FUNCTION__); }

#define REASON_BUF_SIZE     (24)
#define MAX_SW              (2)
enum {
    kSW1 = 0,
    kSW2
};

using ::android::hardware::secure_element::V1_0::LogicalChannelResponse;
using namespace ::android::hardware::secure_element;

sp<V1_0::ISecureElementHalCallback> SecureElementImpl::mCallbackV1_0[SIM_COUNT] {};
sp<V1_1::ISecureElementHalCallback> SecureElementImpl::mCallbackV1_1[SIM_COUNT] {};
bool SecureElementImpl::mConnected[SIM_COUNT] {};
static bool OpenedRil = false;

const char *SecureElementImpl::statusToString(SecureElementStatus seStatus) {
    switch(seStatus) {
        case SecureElementStatus::SUCCESS:
            return "SUCCESS";
        case SecureElementStatus::FAILED:
            return "FAILED";
        case SecureElementStatus::CHANNEL_NOT_AVAILABLE:
            return "CHANNEL_NOT_AVAILABLE";
        case SecureElementStatus::NO_SUCH_ELEMENT_ERROR:
            return "NO_SUCH_ELEMENT_ERROR";
        case SecureElementStatus::UNSUPPORTED_OPERATION:
            return "UNSUPPORTED_OPERATION";
        case SecureElementStatus::IOERROR:
            return "IOERROR";
        default:
            return "INVALID_STATUS";
    }
}

SecureElementImpl::SecureElementImpl(unsigned int socketId) {
    mOpenedchannelCount = 0;
    memset(mOpenedChannels, 0, sizeof(mOpenedChannels));
    mSocketId = socketId;

    mOpenedChannels[BASIC_CHANNEL] = true;
    mOpenedchannelCount++;

    nMutexCount = 0;
    OpenedRil = false;
    mutex_socket = PTHREAD_MUTEX_INITIALIZER;
}

SecureElementImpl::~SecureElementImpl() {
    if (mCallbackV1_0[mSocketId] != nullptr) {
        mCallbackV1_0[mSocketId]->unlinkToDeath(this);
        mCallbackV1_0[mSocketId] = nullptr;
    }

    if (mCallbackV1_1[mSocketId] != nullptr) {
        mCallbackV1_1[mSocketId]->unlinkToDeath(this);
        mCallbackV1_1[mSocketId] = nullptr;
    }

    for (int i = 0; i < nMutexCount; i++) {
        pthread_mutex_unlock(&mutex_socket);
    }
    OpenedRil = false;
}

Return<void> SecureElementImpl::init(
        const sp<V1_0::ISecureElementHalCallback>&clientCallback) {
    ENTER_FUNC();

    do {
        if (clientCallback == nullptr) {
            LogE("%s: calientCallback is nullptr", __func__);
            break;
        }

        mCallbackV1_0[mSocketId] = clientCallback;
        mCallbackV1_1[mSocketId] = nullptr;
        if (!clientCallback->linkToDeath(this, mSocketId)) {
            LogE("%s: Failed to register death notification", __func__);
        }

        if (OpenedRil == false) {
            if (SITRIL_SE_ERROR_NONE != rilOpen()) {
                LogE("%s: rilOpen failed", __func__);
                break;
            } else {
                OpenedRil = true;
            }
        }

        IsCardPresent(mSocketId, &mConnected[mSocketId]);

        // set unsolicated handler to RIL client
        SetUnsolicitedHandler(mSocketId, (void*)onStateChange);

        Return<void> ret = clientCallback->onStateChange(mConnected[mSocketId]);
        if (ret.isOk() == false) {
            LogI("%s: Remote process may be died", __func__);
        }
    } while (0);

    LEAVE_FUNC();
    return Void();
}

Return<void> SecureElementImpl::init_1_1(
        const sp<V1_1::ISecureElementHalCallback>& clientCallback) {

    ENTER_FUNC();

    do {
        if (clientCallback == nullptr) {
            LogE("%s: calientCallback is nullptr", __func__);
            break;
        }

        mCallbackV1_0[mSocketId] = nullptr;
        mCallbackV1_1[mSocketId] = clientCallback;
        if (!clientCallback->linkToDeath(this, mSocketId)) {
            LogE("%s: Failed to register death notification", __func__);
        }

        if (OpenedRil == false) {
            if (SITRIL_SE_ERROR_NONE != rilOpen()) {
                LogE("%s: rilOpen failed", __func__);
                break;
            } else {
                OpenedRil = true;
            }
        }

        IsCardPresent(mSocketId, &mConnected[mSocketId]);

        // set unsolicated handler to RIL client
        SetUnsolicitedHandler(mSocketId, (void*)onStateChange);

        Return<void> ret = clientCallback->onStateChange_1_1(mConnected[mSocketId], "Initialized");
        if (ret.isOk() == false) {
            LogE("%s: Remote process may be died", __func__);
        }
    } while (0);

    LEAVE_FUNC();
    return Void();
}

Return<void> SecureElementImpl::getAtr(getAtr_cb _hidl_cb) {
    ENTER_FUNC();

    E_STATUS ret = E_STATUS_SUCCESS;
    hidl_vec<uint8_t> resBuf;
    LengthData atr {};

    ret = (E_STATUS) GetAtr(mSocketId, &atr);
    if (ret != E_STATUS_SUCCESS) {
        LogE("internalGetAtr failed!!!");
    } else {
        resBuf.setToExternal(atr.data, atr.length);
    }

    _hidl_cb(resBuf);
    LEAVE_FUNC();
    return Void();
}

Return<bool> SecureElementImpl::isCardPresent() {
    return mConnected[mSocketId];
}

Return<void> SecureElementImpl::transmit(const hidl_vec<uint8_t>& data, transmit_cb _hidl_cb) {
    ENTER_FUNC();

    hidl_vec<uint8_t> resBuf;

    TransmitLengthData *pCommand = nullptr;
    TransmitApduChannelResponse *pResponse = nullptr;
    TransmitLengthData *pTransmitCmd = nullptr;
    TransmitApduChannelResponse *pTransmitRsp = nullptr;

    // buffer for log
    char *pTmpbuf = new char[(MAX_APDU_SIZE * 2) + 1] {};

    do {
        uint8_t sessionId = 0;
        uint8_t status[MAX_SW] {};

        pCommand = new TransmitLengthData {};
        pResponse = new TransmitApduChannelResponse {};

        pCommand->length = data.size();
        if (pCommand->length < MIN_APDU_SIZE) {
            LogE("INVALID APDU SIZE : %d", pCommand->length);
            break;
        }

        memcpy(pCommand->data, data.data(), pCommand->length);
        Value2HexString(pTmpbuf, pCommand->data, pCommand->length);
        LogD("CAPDU : %s", pTmpbuf);

        E_STATUS ret = E_STATUS_SUCCESS;
        ret = (E_STATUS) internalTransmit(pResponse, pCommand);
        if (ret != E_STATUS_SUCCESS) {
            LogE("internalTransmit failed (%d)!!!", ret);
            break;
        }

        sessionId = (uint8_t)parseChannelNumber(pCommand->data[0]);
        status[kSW1] = pResponse->sw1;
        status[kSW2] = pResponse->sw2;

        LogD("transmit received sw1: %d, sw2 : %d, pResponse->nResponseCount : %d",
                status[kSW1], status[kSW2], pResponse->nResponseCount);

        Value2HexString(pTmpbuf, pResponse->aResponse, pResponse->nResponseCount);
        LogD("RAPDU : %s", pTmpbuf);

        // when sw is 62 xx, 63 xx and command is "SELECT" command
        // if data is only status word, request GET_RESPONSE
        if ((pCommand->data[1] == 0xA4 && (status[kSW1]  == 0x62 || status[kSW1]  == 0x63))
                && (pResponse->nResponseCount < 3)) {
            pTransmitCmd = new TransmitLengthData {};
            pTransmitRsp = new TransmitApduChannelResponse {};

            pTransmitCmd->data[0] = setChannelToClassByte(0x00, sessionId);
            pTransmitCmd->data[1] = 0xc0;
            pTransmitCmd->data[2] = 0x00;
            pTransmitCmd->data[3] = 0x00;
            pTransmitCmd->data[4] = 0x00;
            pTransmitCmd->length = 5;

            ret = (E_STATUS) internalTransmit(pTransmitRsp, pTransmitCmd);
            if (ret == E_STATUS_SUCCESS) {
                int tmpSw1 = pTransmitRsp->sw1;
                int tmpSw2 = pTransmitRsp->sw2;

                if (2 < pTransmitRsp->nResponseCount && (tmpSw1 == 0x90 && tmpSw2 == 0x00)) {
                    pTransmitRsp->aResponse[pTransmitRsp->nResponseCount-2] = status[kSW1];
                    pTransmitRsp->aResponse[pTransmitRsp->nResponseCount-1] = status[kSW2];
                    resBuf.setToExternal(pTransmitRsp->aResponse, pTransmitRsp->nResponseCount);
                } else {
                    resBuf.setToExternal(status, MAX_SW);
                }
            } else {
                resBuf.setToExternal(status, MAX_SW);
            }
        } else {
            resBuf.setToExternal(pResponse->aResponse, pResponse->nResponseCount);
        }
    } while (0);

    _hidl_cb(resBuf);

    delete pCommand;
    delete pResponse;
    delete pTransmitCmd;
    delete pTransmitRsp;
    delete[] pTmpbuf;

    LEAVE_FUNC();
    return Void();
}

Return<void> SecureElementImpl::openLogicalChannel(const hidl_vec<uint8_t>& aid, uint8_t p2,
        openLogicalChannel_cb _hidl_cb) {
    ENTER_FUNC();

    SecureElementStatus seStatus = SecureElementStatus::IOERROR;
    LogicalChannelResponse resBuf {};
    resBuf.channelNumber = MAX_CHANNEL_NUM;

    TransmitLengthData *pTransmitCmd = nullptr;
    TransmitApduChannelResponse *pTransmitRsp = nullptr;

    do {
        E_STATUS ret = E_STATUS_FAIL;
        OpenChannelResponse openRsp {};
        LengthData AID {};
        AID.length = aid.size();
        if (0 < AID.length) {
            memcpy(&AID.data[0], aid.data(), aid.size());
        }

        ret = (E_STATUS) OpenLogicalChannel(mSocketId, &openRsp, &AID, p2);
        if (ret != E_STATUS_SUCCESS) {
            /*Transceive failed*/
            if (ret == E_STATUS_SIM_ABSENT) {
                seStatus = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
            } else {
                seStatus = SecureElementStatus::IOERROR;
            }
            break;
        }

        if (openRsp.rilErrno == RILC_STATUS_MISSING_RESOURCE) {
            seStatus = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
        } else if (openRsp.rilErrno == RILC_STATUS_NO_SUCH_ELEMENT) {
            seStatus = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
        } else {
            uint8_t status[MAX_SW] {};
            status[kSW1] = openRsp.sw1;
            status[kSW2] = openRsp.sw2;

            LogD("Response sw1 : 0x%02x, sw2 : 0x%02x, sessionId : %d",
                    status[kSW1], status[kSW2], openRsp.sessionId);

            // Return response on success, empty vector on failure
            // Status is success
            if (status[kSW1] == 0x90 || status[kSW1] == 0x91 || status[kSW1] == 0x62
                    || status[kSW1] == 0x63) {
                seStatus = SecureElementStatus::SUCCESS;
            } else if (status[kSW1] == 0x6A && status[kSW2] == 0x82) {
                seStatus = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
            } else if (status[kSW1] == 0x69 && (status[kSW2] == 0x85
                    || status[kSW2] == 0x99)) {
                if (status[kSW2] == 0x99) {
                    LogI("CRC2: NoSuchElementError");
                    LogI("if the AID on the SE is not available (or cannot be selected) or a logical"
                            " channel is already open to a non-multiselectable applet.");
                } else if (status[kSW2] == 0x85) {
                    LogI("Conditions of use not satisfied");
                }
                seStatus = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
            } else if ((status[kSW1] == 0x6A && (status[kSW2] == 0x86 || status[kSW2] == 0x81)) ||
                (((status[kSW1] == 0x6E) || (status[kSW1] == 0x6D)) && status[kSW2] == 0x00)) {
                seStatus = SecureElementStatus::UNSUPPORTED_OPERATION;
            }

            // when sw is 62 xx, 63, data is only status word, request GET_RESPONSE
            if ((openRsp.nResponseCount < 3) && (status[kSW1] == 0x62 || status[kSW1] == 0x63)) {
                pTransmitCmd = new TransmitLengthData {};
                pTransmitRsp = new TransmitApduChannelResponse {};

                pTransmitCmd->length = 5;
                pTransmitCmd->data[0] = setChannelToClassByte(0x00, openRsp.sessionId);
                pTransmitCmd->data[1] = 0xc0;
                pTransmitCmd->data[2] = 0x00;
                pTransmitCmd->data[3] = 0x00;
                pTransmitCmd->data[4] = 0x00;

                ret = (E_STATUS) internalTransmit(pTransmitRsp, pTransmitCmd);
                if (ret == E_STATUS_SUCCESS) {
                    resBuf.channelNumber = (uint8_t)openRsp.sessionId;
                    int tmpSw1 = pTransmitRsp->sw1;
                    int tmpSw2 = pTransmitRsp->sw2;
                    if (2 < pTransmitRsp->nResponseCount && (tmpSw1 == 0x90 && tmpSw2 == 0x00)) {
                        pTransmitRsp->aResponse[pTransmitRsp->nResponseCount-2] = status[kSW1];
                        pTransmitRsp->aResponse[pTransmitRsp->nResponseCount-1] = status[kSW2];
                        resBuf.selectResponse.setToExternal(pTransmitRsp->aResponse,
                                pTransmitRsp->nResponseCount);
                    } else {
                        resBuf.selectResponse.setToExternal(status, MAX_SW);
                    }
                } else {
                    resBuf.selectResponse.setToExternal(status, MAX_SW);
                }
            } else {
                resBuf.channelNumber = (uint8_t)openRsp.sessionId;
                if (openRsp.nResponseCount) {
                    resBuf.selectResponse.setToExternal(openRsp.aResponse, openRsp.nResponseCount);
                }
            }

            if (seStatus == SecureElementStatus::SUCCESS) {
                if (MAX_CHANNEL_NUM <= resBuf.channelNumber) {
                    seStatus = SecureElementStatus::FAILED;
                    break;
                }
                mOpenedChannels[resBuf.channelNumber] = true;
                mOpenedchannelCount++;
            }
        }
    } while (0);

    _hidl_cb(resBuf, seStatus);
    LogD("return seStatus : %s", statusToString(seStatus));

    delete pTransmitCmd;
    delete pTransmitRsp;

    LEAVE_FUNC();
    return Void();
}

Return<void> SecureElementImpl::openBasicChannel(const hidl_vec<uint8_t>& aid,
        uint8_t p2, openBasicChannel_cb _hidl_cb) {
    ENTER_FUNC();

    hidl_vec<uint8_t> resBuf;
    SecureElementStatus seStatus = SecureElementStatus::IOERROR;

    TransmitLengthData *pCommand = nullptr;
    TransmitApduChannelResponse *pResponse = nullptr;

    TransmitLengthData *pTransmitCmd = nullptr;
    TransmitApduChannelResponse *pTransmitRsp = nullptr;

    do {
        pCommand = new TransmitLengthData {};
        pResponse = new TransmitApduChannelResponse {};

        pCommand->length= (int32_t)(5 + aid.size());
        pCommand->data[0] = BASIC_CHANNEL;          // basic channel
        pCommand->data[1] = (uint8_t)0xA4;          // INS
        pCommand->data[2] = 0x04;                   // P1
        pCommand->data[3] = p2;                     // P2
        pCommand->data[4] = (uint8_t)aid.size();    // Lc
        memcpy(&pCommand->data[5], aid.data(), aid.size());

        E_STATUS ret = E_STATUS_FAIL;
        ret = internalTransmit(pResponse, pCommand);
        if (ret != E_STATUS_SUCCESS) {
            /* Transceive failed */
            if (ret == E_STATUS_SIM_ABSENT) {
                seStatus = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
            } else {
                seStatus = SecureElementStatus::IOERROR;
            }
            break;
        }

        uint8_t status[MAX_SW] {};
        status[kSW1] = pResponse->sw1;
        status[kSW2] = pResponse->sw2;

        /*Return response on success, empty vector on failure*/
        /*Status is success*/
        LogD("Response sw1 : 0x%02x, sw2 : 0x%02x", status[kSW1], status[kSW2]);

        if (status[kSW1] == 0x90 || status[kSW1] == 0x91 || status[kSW1] == 0x62
                || status[kSW1] == 0x63) {
            seStatus = SecureElementStatus::SUCCESS;
        } else if (status[kSW1] == 0x6A && status[kSW2] == 0x82) {
            seStatus = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
        } else if (status[kSW1] == 0x69 && (status[kSW2] == 0x85 || status[kSW2] == 0x99)) {
            if (status[kSW2] == 0x99) {
                LogI("if the AID on the SE is not available (or cannot be selected) or a logical"
                        " channel is already open to a non-multiselectable applet.");
            } else if (status[kSW2] == 0x85) {
                LogI("Conditions of use not satisfied");
            }
            seStatus = SecureElementStatus::NO_SUCH_ELEMENT_ERROR;
        } else if ((status[kSW1] == 0x6A && (status[kSW2] == 0x86 || status[kSW2] == 0x81))
                || (((status[kSW1] == 0x6E) || (status[kSW1] == 0x6D)) 
                && status[kSW2] == 0x00)) {
            seStatus = SecureElementStatus::UNSUPPORTED_OPERATION;
        }

        // when sw is 62 xx, 63 xx and command is "SELECT" command
        // if data is only status word, request GET_RESPONSE
        if ((pCommand->data[1] == 0xA4 && (status[kSW1] == 0x62 || status[kSW1] == 0x63)) &&
                (pResponse->nResponseCount < 3)) {
            pTransmitCmd = new TransmitLengthData {};
            pTransmitRsp = new TransmitApduChannelResponse {};

            pTransmitCmd->data[0] = setChannelToClassByte(0x00, BASIC_CHANNEL);
            pTransmitCmd->data[1] = 0xc0;
            pTransmitCmd->data[2] = 0x00;
            pTransmitCmd->data[3] = 0x00;
            pTransmitCmd->data[4] = 0x00;
            pTransmitCmd->length = 5;

            ret = internalTransmit(pTransmitRsp, pTransmitCmd);
            if (ret == E_STATUS_SUCCESS) {
                int tmpSw1 = pTransmitRsp->sw1;
                int tmpSw2 = pTransmitRsp->sw2;
                if (2 < pTransmitRsp->nResponseCount && (tmpSw1 == 0x90 && tmpSw2 == 0x00)) {
                    pTransmitRsp->aResponse[pTransmitRsp->nResponseCount-2] = status[kSW1];
                    pTransmitRsp->aResponse[pTransmitRsp->nResponseCount-1] = status[kSW2];
                    resBuf.setToExternal(pTransmitRsp->aResponse, pTransmitRsp->nResponseCount);
                } else {
                    resBuf.setToExternal(status, MAX_SW);
                }
            } else {
                resBuf.setToExternal(status, MAX_SW);
            }
        } else {
            if (pResponse->nResponseCount) {
                resBuf.setToExternal(pResponse->aResponse, pResponse->nResponseCount);
            }
        }
    } while (0);

    _hidl_cb(resBuf, seStatus);
    LogD("return seStatus : %s", statusToString(seStatus));

    delete pCommand;
    delete pResponse;
    delete pTransmitCmd;
    delete pTransmitRsp;

    LEAVE_FUNC();
    return Void();
}

Return<SecureElementStatus> SecureElementImpl::closeChannel(uint8_t channelNumber) {
    ENTER_FUNC();

    SecureElementStatus seStatus = SecureElementStatus::FAILED;
    LogI("channelNumber : %d", channelNumber);

    do {
        if (channelNumber == BASIC_CHANNEL) {
            // no close operation because of basic channel
            seStatus = SecureElementStatus::SUCCESS;
            break;
        }

        if (MAX_CHANNEL_NUM <= channelNumber) {
            LogE("invalid channel!!!");
            seStatus = SecureElementStatus::FAILED;
        } else if (mOpenedChannels[channelNumber] == false) {
            seStatus = SecureElementStatus::CHANNEL_NOT_AVAILABLE;
        } else if (BASIC_CHANNEL < channelNumber) {
            E_STATUS ret = (E_STATUS)CloseLogicalChannel(mSocketId, channelNumber);
            if (ret == E_STATUS_SUCCESS) {
                seStatus = SecureElementStatus::SUCCESS;
            } else {
                seStatus = SecureElementStatus::FAILED;
            }
        }

        if (seStatus == SecureElementStatus::SUCCESS) {
            mOpenedChannels[channelNumber] = false;
            mOpenedchannelCount--;
        }
    } while (0);

    LogI("return seStatus : %s", statusToString(seStatus));
    LEAVE_FUNC();
    return seStatus;
}

Return<SecureElementStatus> SecureElementImpl::reset(void) {
    ENTER_FUNC();

    SecureElementStatus seStatus = SecureElementStatus::FAILED;
    E_STATUS ret = (E_STATUS) Reset(mSocketId);
    if (ret == E_STATUS_SUCCESS) {
        seStatus = SecureElementStatus::SUCCESS;
    }

    LEAVE_FUNC();
    return seStatus;
}

E_STATUS SecureElementImpl::internalTransmit(TransmitApduChannelResponse *pResponse,
        TransmitLengthData *pCommand) {
    ENTER_FUNC();
    E_STATUS ret = E_STATUS_FAIL;

    do {
        if (pResponse == nullptr || pCommand == nullptr) {
            if (pResponse == nullptr) LogE("pResonse is nullptr");
            if (pCommand == nullptr) LogE("pCommand is nullptr");
            break;
        }

        LengthData *pApduBuffer = new LengthData {};

        pthread_mutex_lock(&mutex_socket);

        int channelNumber = parseChannelNumber(pCommand->data[0]);
        int cla = pCommand->data[0] & 0xFF;
        int ins = pCommand->data[1] & 0xff;
        int p1 = pCommand->data[2] & 0xff;
        int p2 = pCommand->data[3] & 0xff;
        int p3 = (4 < pCommand->length) ? pCommand->data[4] & 0xff : 0;
        if (5 < pCommand->length) {
            pApduBuffer->length = (pCommand->length - 5);
            memcpy(&pApduBuffer->data[0], &pCommand->data[5], pApduBuffer->length); // 6.5.3 id 6
        }
        ret = (E_STATUS) TransmitApduLogicalChannel(mSocketId, pResponse, channelNumber, cla,
                ins, p1, p2, p3, pApduBuffer);

        pthread_mutex_unlock(&mutex_socket);

        delete pApduBuffer;
    } while (0);

    LEAVE_FUNC();
    return ret;
}

void SecureElementImpl::onStateChange(int socketId, void *data)
{
    ENTER_FUNC();

    do {
        if (socketId == -1 || data == nullptr) {
            LogE("Invalid Paramter!!!!");
            break;
        }

        bool bConnected = *(bool*)data;
        char reasonBuf[REASON_BUF_SIZE] {};

        mConnected[socketId] = bConnected;
        if (mCallbackV1_1[socketId] != nullptr) {
            snprintf(reasonBuf, REASON_BUF_SIZE, "%s SIM card", bConnected ? "Insert" : "Remove");
            Return<void> ret = mCallbackV1_1[socketId]->onStateChange_1_1(bConnected, reasonBuf);
            if (ret.isOk() == false) {
                LogI("%s: Remote process may be died", __func__);
            }
        } else if (mCallbackV1_0[socketId] != nullptr) {
            Return<void> ret = mCallbackV1_0[socketId]->onStateChange(bConnected);
            if (ret.isOk() == false) {
                LogI("%s: Remote process may be died", __func__);
            }
        } else {
            LogE("Callback is not exist!!!!");
        }
    } while (0);

    LEAVE_FUNC();
    return;
}

uint8_t SecureElementImpl::clearChannelNumber(uint8_t cla) {
    bool isFirstInterindustryClassByteCoding = (cla & 0x40) == 0x00;
    if (isFirstInterindustryClassByteCoding) {
        return (uint8_t) (cla & 0xFC);
    }
    return (uint8_t) (cla & 0xF0);
}

int32_t SecureElementImpl::parseChannelNumber(uint8_t cla) {
    bool isFirstInterindustryClassByteCoding = (cla & 0x40) == 0x00;
    if (isFirstInterindustryClassByteCoding) {
        return cla & 0x03;
    }
    return (cla & 0x0F) + 4;
}

uint8_t SecureElementImpl::setChannelToClassByte(uint8_t cla, int channelNumber) {
    if (channelNumber < 4) {
        // b7 = 0 indicates the first interindustry class byte coding
        cla = (uint8_t) ((cla & 0xBC) | channelNumber);
    } else if (channelNumber < 20) {
        // b7 = 1 indicates the further interindustry class byte coding
        bool isSM = (cla & 0x0C) != 0;
        cla = (uint8_t) ((cla & 0xB0) | 0x40 | (channelNumber - 4));
        if (isSM) {
            cla |= 0x20;
        }
    } else {
        LogE("IllegalArgumentException(\"Channel number must be within [0..19]\")");
    }
    return cla;
}

void SecureElementImpl::serviceDied(uint64_t cookie, const android::wp<IBase>& /*who*/) {

    ENTER_FUNC();

    if (cookie == mSocketId) {
        LogI("serviceDied :  %" PRIu64, cookie);

        if (mCallbackV1_0[mSocketId] != nullptr) {
            mCallbackV1_0[mSocketId]->unlinkToDeath(this);
            mCallbackV1_0[mSocketId] = nullptr;
        }
        if (mCallbackV1_1[mSocketId] != nullptr) {
            mCallbackV1_1[mSocketId]->unlinkToDeath(this);
            mCallbackV1_1[mSocketId] = nullptr;
        }
    }

    LEAVE_FUNC();
    return;
}
