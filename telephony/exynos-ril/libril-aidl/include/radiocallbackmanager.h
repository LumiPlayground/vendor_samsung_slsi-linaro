/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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

template<
    typename RadioCallbackType,
    typename RadioResponseCallbackType,
    typename RadioIndicationCallbackType
>
struct RadioCallbackManager {
    std::shared_ptr<RadioCallbackType> mRadioCallback;
    std::shared_ptr<RadioResponseCallbackType> mRadioResponseCallback;
    std::shared_ptr<RadioIndicationCallbackType> mRadioIndicationCallback;

    RadioCallbackManager()=default;
    RadioCallbackManager(const std::shared_ptr<RadioCallbackType>& radioCallback)
            : mRadioCallback(radioCallback) {}
    RadioCallbackManager(const std::shared_ptr<RadioCallbackType>& radioCallback,
            const std::shared_ptr<RadioResponseCallbackType>& radioResponseCallback,
            const std::shared_ptr<RadioIndicationCallbackType>& radioIndicationCallback)
            : mRadioCallback(radioCallback) {
        setCallbacks(radioResponseCallback, radioIndicationCallback);
    }

    void setCallbacks(const std::shared_ptr<RadioResponseCallbackType>& radioResponseCallback,
            const std::shared_ptr<RadioIndicationCallbackType>& radioIndicationCallback) {
        mRadioResponseCallback = radioResponseCallback;
        mRadioIndicationCallback = radioIndicationCallback;
    }

    template<typename RadioResponseType, typename RadioIndicationType>
    void setResponseFunctions(const std::shared_ptr<RadioResponseType>& radioResponse,
            const std::shared_ptr<RadioIndicationType>& radioIndication) {
        if (mRadioResponseCallback) {
            mRadioResponseCallback->setResponseFunction(radioResponse);
        }
        if (mRadioIndicationCallback) {
            mRadioIndicationCallback->setResponseFunction(radioIndication);
        }
        if (mRadioCallback) {
            mRadioCallback->setResponseFunctions(mRadioResponseCallback, mRadioIndicationCallback);
        }
    }
};