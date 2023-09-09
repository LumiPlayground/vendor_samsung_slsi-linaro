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

#include <radioproxy.h>
#include <radioresponse.h>

#include <functional>
#include <map>

struct RilAidlService {
    uint32_t mSlot;
    std::shared_ptr<RadioProxy> mRadio;
    std::shared_ptr<RadioResponse> mRadioResponse;
    std::shared_ptr<RadioIndication> mRadioIndication;

    RilAidlService(uint32_t slot, const std::shared_ptr<RadioProxy>& radio,
            const std::shared_ptr<RadioResponse>& radioResponse,
            const std::shared_ptr<RadioIndication>& radioIndication)
            : mSlot(slot), mRadio(radio), mRadioResponse(radioResponse), mRadioIndication(radioIndication) {}
    virtual ~RilAidlService() = default;
    virtual bool publish() = 0;
};