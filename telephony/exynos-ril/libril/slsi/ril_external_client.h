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
 * ril_external_client.h
 *
 *  Created on: 2020. 2. 10.
 */

#ifndef __RIL_EXTERNAL_CLIENT_H__
#define __RIL_EXTERNAL_CLIENT_H__

//#include "ril_external_hal_common.h"
#include <vendor/samsung_slsi/telephony/hardware/radioExternal/1.1/IOemSlsiRadioExternal.h>

#include <set>
#include <string>

/**
 * RequestRawSegmentData
 */
#define MAX_RADIO_DATA_SIZE (4 * 1024) // 4KB, segmented unit radio data size
#define MAX_RADIO_DATA_TOTAL_SIZE (16 * MAX_RADIO_DATA_SIZE) // 16 * MAX_RADIO_DATA_SIZE = 64 KB

class RequestRawSegmentData {
public:
    char *mData;
    int mSerial;
    int mClientId;
    int mRequestId;
    int mSlotId;
    size_t mTotalLen;
    size_t mLen;
    int mFlag = 0;
    bool mTransaction;
public:
    RequestRawSegmentData();
    RequestRawSegmentData(const RequestRawSegmentData&) = delete;
    RequestRawSegmentData& operator=(RequestRawSegmentData const&) = delete;
    ~RequestRawSegmentData();
    void reset();
    bool init(int serial, int clientId, int requestId, int slotId, size_t totalLen);
    int setToExternal(int index, const void *data, size_t datalen);
    void setTransaction(bool transaction) { mTransaction = transaction; }
    bool isTransaction() { return mTransaction; }
    bool assembled() { return (mFlag == 0) && (mTotalLen > 0) && (mTotalLen == mLen); }
public:
    static RequestRawSegmentData *obtain(int serial, int clientId, int requestId, int slotId, size_t totalLen);
};

/**
 * RilExternalClient
 */
class RilExternalClient {
private:
    int mClientId;
    std::string mName;
    android::sp<vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::IOemSlsiRadioExternalRes> mResponse;
    android::sp<vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::IOemSlsiRadioExternalInd> mIndication;
    std::set<int32_t> mIndicationFilter;
    RequestRawSegmentData *mSegmentBuffer;
private:
    RilExternalClient(int32_t clientId, std::string name);

public:
    int setResponseFunctions(const android::sp<vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::IOemSlsiRadioExternalRes> &response,
            const android::sp<vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::IOemSlsiRadioExternalInd> &indication);
    void clearResponseFunctions();
    int getClientId() { return mClientId; }
    const std::string &getName() const { return mName; }
    std::string &getName() { return mName; }
    void clear();
    RequestRawSegmentData *allocateSegmentBuffer(int serial, int requestId, int slotId, size_t totalLen);

    bool sendRequestRawResponse(vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::RadioExternalResponseInfo &responseInfo, void *response, size_t responseLen);
    bool rilExternalRawIndication(int indication, int slotId, const void *unsolResp, size_t responseLen);

    // for v1.1
    void setIndicationFilter(const ::android::hardware::hidl_vec<int32_t>& indicationFilter);
    bool isFilteredIndication(int32_t indication);

    std::string toString() const;

private:
    bool sendRequestRawResponseSeg(vendor::samsung_slsi::telephony::hardware::radioExternal::V1_0::RadioExternalResponseInfo &responseInfo, void *response, size_t responseLen);
    bool rilExternalRawIndicationSeg(int indication, int slotId, const void *unsolResp, size_t responseLen);
    void onNewCommandConnect();
    void onNewCommandConnect(int slot);
    bool checkReturnStatus(::android::hardware::Return<void>& ret);
    static int getNextClientId();

public:
    static RilExternalClient *newInstance(std::string name);
};

#endif /* __RIL_EXTERNAL_CLIENT_H__ */
