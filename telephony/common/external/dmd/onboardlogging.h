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
 * onboardlogging.h
 *
 *  Created on: 2021. 10. 13.
 */
#ifndef __ON_BOARD_LOGGING_H__
#define __ON_BOARD_LOGGING_H__

#include "diagmonitor.h"
#include "ModemProxy.h"

/**
 * OnBoardResponseCallback
 */
struct OnBoardResponseCallback {
    virtual ~OnBoardResponseCallback()=default;
    virtual void onResponse(const void *data, size_t datalen)=0;
};

/**
 * OnBoardLogging
 */
struct OnBoardLogging  : public OnRawRequestListener {
    ModemProxy *mModemProxy;
    OnBoardResponseCallback *mCallback;
    OnBoardLogging(OnBoardResponseCallback *callback);
    void setModemProxy(ModemProxy *modemProxy) { mModemProxy = modemProxy; }
    void onRawRequest(const void *data, size_t datalen);
    void onModemDataReceive(const void *data, size_t datalen) {
        if (mCallback) {
            mCallback->onResponse(data, datalen);
        }
    }
};

#endif /* __ON_BOARD_LOGGING_H__ */
