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
 * ril_external_client.cpp
 *
 *  Created on: 2020. 2. 25.
 */
#define LOG_TAG "RIL_EXTERNAL"

#include <telephony/ril.h>
#include <slsi/ril_client.h>
#include <utils/Log.h>
#include "ril_external_client.h"
#include "ril_external_manager.h"
#include "ril_external_service.h"

using namespace vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0;
using namespace vendor::samsung_slsi::telephony::hardware::radioExternal;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using android::sp;

static int s_globalClientId = 0;

/**
 * RequestRawSegmentData
 */
RequestRawSegmentData::RequestRawSegmentData()
{
    mData = NULL;
    reset();
}

RequestRawSegmentData::~RequestRawSegmentData()
{
    reset();
}

void RequestRawSegmentData::reset()
{
    if (mData != NULL) {
        delete[] mData;
    }
    mData = NULL;
    mSerial = 0;
    mClientId = -1;
    mRequestId = -1;
    mSlotId = 0;
    mTotalLen = mLen = 0;
    mTransaction = false;
    mFlag = 0;
}

bool RequestRawSegmentData::init(int serial, int clientId, int requestId, int slotId, size_t totalLen)
{
    reset();
    if (serial < 0 || clientId < 0 || requestId < 0 || slotId < 0 || totalLen == 0 || totalLen > MAX_RADIO_DATA_TOTAL_SIZE) {
        if (totalLen > MAX_RADIO_DATA_TOTAL_SIZE) RLOGW("%s: totalLen %lu is over the limit", __FUNCTION__, (unsigned long)totalLen);
        return false;
    }
    mSerial = serial;
    mClientId = clientId;
    mRequestId = requestId;
    mSlotId = slotId;
    mTotalLen = totalLen;
    // MAX of num is 32, mFlag is 32 bit.
    size_t num = (mTotalLen + MAX_RADIO_DATA_SIZE - 1) / MAX_RADIO_DATA_SIZE;
    mData = new char[MAX_RADIO_DATA_SIZE * num];
    if (num < 32) mFlag = ~(0xFFFFFFFF << num);
    else mFlag = 0xFFFFFFFF;
    setTransaction(true);

    return true;
}

int RequestRawSegmentData::setToExternal(int index, const void *data, size_t datalen)
{
    if (index < 0 || data == NULL || datalen == 0 || datalen > MAX_RADIO_DATA_SIZE) {
        return -1;
    }

    if ((mFlag & (0x1 << index)) == 0) {
        // already copied or out of index.
        return -2;
    }
    memcpy(mData + (MAX_RADIO_DATA_SIZE * index), data, datalen);
    mFlag &= ~(0x1 << index);
    mLen += datalen;
    return datalen;
}

RequestRawSegmentData *RequestRawSegmentData::obtain(int serial, int clientId,
        int requestId, int slotId, size_t totalLen)
{
    RequestRawSegmentData *p = new RequestRawSegmentData();
    if (p != NULL) {
        bool ret = true;
        ret = p->init(serial, clientId, requestId, slotId, totalLen);
        if (ret == false) {
            delete p;
            p = NULL;
        }
    }
    return p;
}

int RilExternalClient::getNextClientId()
{
    int newClientId = s_globalClientId++;
    return newClientId;
}

RilExternalClient::RilExternalClient(int32_t clientId, std::string name)
    : mClientId(-1), mName(""), mSegmentBuffer(NULL) {
    mClientId = clientId;
    mName = name;
}

int RilExternalClient::setResponseFunctions(const sp<IOemSlsiRadioExternalRes> &response,
        const sp<IOemSlsiRadioExternalInd> &indication) {
    mResponse = response;
    mIndication = indication;

    if (mResponse == nullptr && mIndication == nullptr) {
        clear();
    }
    return getClientId();
}

void RilExternalClient::clear() {
    mClientId = -1;
    mName = "";
    mResponse = nullptr;
    mIndication = nullptr;
    if (mSegmentBuffer != NULL) {
        delete mSegmentBuffer;
    }
}

RequestRawSegmentData *RilExternalClient::allocateSegmentBuffer(
        int serial, int requestId, int slotId, size_t totalLen)
{
    if (mSegmentBuffer != NULL) {
        if (mSegmentBuffer->isTransaction() &&
            (mSegmentBuffer->mSerial == serial) &&
            (mSegmentBuffer->mRequestId == requestId)) {
            return mSegmentBuffer;
        }
        delete mSegmentBuffer;
    }
    mSegmentBuffer = RequestRawSegmentData::obtain(serial, mClientId, requestId, slotId, totalLen);
    return mSegmentBuffer;
}

bool RilExternalClient::checkReturnStatus(Return<void>& ret)
{
    if (!ret.isOk()) {
        RLOGW("RilExternalClient(%s) may be dead?", toString().c_str());
        clear();
        return false;
    }
    return true;
}

bool RilExternalClient::sendRequestRawResponse(RadioExternalResponseInfo &responseInfo,
        void *response, size_t responseLen)
{
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (responseLen > MAX_RADIO_DATA_SIZE) {
        return sendRequestRawResponseSeg(responseInfo, response, responseLen);
    }

    if (mResponse != NULL) {
        hidl_vec<uint8_t> data;
        data.resize(0);
        if (response != NULL && responseLen > 0) {
            data.setToExternal((uint8_t *) response, responseLen);
        }
        Return<void> retStatus = mResponse->sendRequestRawResponse(responseInfo, data);
        return checkReturnStatus(retStatus);
    }
    return true;
}

bool RilExternalClient::sendRequestRawResponseSeg(RadioExternalResponseInfo &responseInfo,
        void *response, size_t responseLen)
{
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mResponse != NULL) {
        int segIndex;
        int num = (responseLen + MAX_RADIO_DATA_SIZE - 1) / MAX_RADIO_DATA_SIZE;
        const uint8_t *rspData = (const uint8_t *)response;

        for (segIndex = 0; segIndex < num; ++segIndex) {
            int segLength = (segIndex == num - 1) ? (responseLen % MAX_RADIO_DATA_SIZE) : MAX_RADIO_DATA_SIZE;
            hidl_vec<uint8_t> sendData;
            if (segLength > 0) sendData.setToExternal((uint8_t *)rspData, segLength);
            Return<void> retStatus = mResponse->sendRequestRawResponseSeg(responseInfo, sendData, segIndex, responseLen);
            if (!checkReturnStatus(retStatus)) {
                return false;
            }
            rspData += segLength;
        }
    }
    return true;
}

bool RilExternalClient::rilExternalRawIndication(int indication, int slotId,
        const void *unsolResp, size_t responseLen)
{
#if VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (responseLen > MAX_RADIO_DATA_SIZE) {
        return rilExternalRawIndicationSeg(indication, slotId, unsolResp, responseLen);
    }

    if (mIndication != NULL) {
        hidl_vec<uint8_t> data;
        data.resize(0);
        if (unsolResp != NULL && responseLen > 0) {
            data.setToExternal((uint8_t *) unsolResp, responseLen);
        }
        Return<void> retStatus = mIndication->rilExternalRawIndication(indication, slotId, data, data.size());
        return checkReturnStatus(retStatus);
    }
    return true;
}

bool RilExternalClient::rilExternalRawIndicationSeg(int indication, int slotId, const void *unsolResp, size_t responseLen)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    if (mIndication != NULL) {
        int segIndex;
        int num = (responseLen + MAX_RADIO_DATA_SIZE - 1) / MAX_RADIO_DATA_SIZE;
        const uint8_t *data = (const uint8_t *)unsolResp;
        for (segIndex = 0; segIndex < num; ++segIndex) {
            hidl_vec<uint8_t> sendData;
            int segLength = (segIndex == num - 1) ? (responseLen % MAX_RADIO_DATA_SIZE) : MAX_RADIO_DATA_SIZE;
            sendData.setToExternal((uint8_t *)data, segLength);
                Return<void> retStatus = mIndication->rilExternalRawIndicationSeg(indication, slotId,
                        sendData, segLength, segIndex, responseLen);
                if (!checkReturnStatus(retStatus)) {
                    return false;
                }
                data += segLength;
        } // end for segIndex ~
    }
    return true;
}

void RilExternalClient::setIndicationFilter(const hidl_vec<int32_t>& indicationFilter)
{
#ifdef VDBG
    RLOGD("%s", __FUNCTION__);
#endif
    mIndicationFilter.clear();

    // default in 1.1
    mIndicationFilter.insert(RIL_UNSOL_RIL_CONNECTED);
    mIndicationFilter.insert(RILC_UNSOL_CONNECTED);
    mIndicationFilter.insert(RILC_UNSOL_RADIO_STATE_CHANGED);

    // user added filters
    for (size_t i = 0; i < indicationFilter.size(); i++) {
        if (IS_DEBUGABLE()) {
            RLOGD("[%zu] indication=%d", i, indicationFilter[i]);
        }
        mIndicationFilter.insert(indicationFilter[i]);
    } // end for i ~
}

bool RilExternalClient::isFilteredIndication(int32_t indication)
{
    bool isFiltered = !(mIndicationFilter.empty() ||
                       (mIndicationFilter.find(indication) != mIndicationFilter.end()));
    if (IS_DEBUGABLE() /*&& isFiltered*/) {
        RLOGI("[%s] clientId=%d name=%s indication=%d isFiltered=%s", __FUNCTION__,
                mClientId, mName.c_str(), indication, (isFiltered ? "true" : "false"));
    }
    return isFiltered;
}

RilExternalClient *RilExternalClient::newInstance(std::string name)
{
    int id = getNextClientId();
    RilExternalClient *newClient = new RilExternalClient(id, name);
    return newClient;
}

std::string RilExternalClient::toString() const
{
    std::stringstream ss;
    ss << "RilExternalClient:{";
    ss << "clientId=" << mClientId;
    ss << ", name=" << mName;
    ss << "}";
    return ss.str();
}
