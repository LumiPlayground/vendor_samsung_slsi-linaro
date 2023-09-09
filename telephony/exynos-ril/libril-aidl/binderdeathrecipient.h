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

#include <android/binder_auto_utils.h>
#include <android/binder_ibinder.h>
#include <string>

struct BinderDiedListener {
    virtual ~BinderDiedListener()=default;
    virtual void onBinderDied(uint64_t cookie)=0;
};

struct BinderDeathRecipient {
    BinderDiedListener *mBinderDiedListener;
    AIBinder *mBinder;
    AIBinder_DeathRecipient *mRecipient;
    bool mLinked;
    uint64_t mCookie;

    BinderDeathRecipient(BinderDiedListener *binderDiedListener);
    ~BinderDeathRecipient();

    bool linkToDeath(AIBinder *binder, uint64_t cookie);
    void unlinkToDeath();
    void onBinderDied();
};

struct RemoteDiedHelper : public BinderDiedListener {
    uint64_t mCookie = 0UL;
    BinderDeathRecipient mDeathRecipient;
    struct OnRemoteDiedListener {
        virtual ~OnRemoteDiedListener()=default;
        virtual void onRemoteDied()=0;
    };
    OnRemoteDiedListener *mOnRemoteDiedListener = nullptr;

    RemoteDiedHelper() : mDeathRecipient(this) {}
    virtual ~RemoteDiedHelper()=default;

    uint64_t nextCookie() { return ++mCookie; }
    uint64_t getCookie() { return mCookie; }

    void linkToDeath(AIBinder* binder) {
        mDeathRecipient.linkToDeath(binder, nextCookie());
    }

    void unlinkToDeath() {
        mDeathRecipient.unlinkToDeath();
    }

    void onBinderDied(uint64_t cookie) {
        // onBinderDied is called by a main-thread.
        if (cookie != mCookie) {
            if (mOnRemoteDiedListener) {
                mOnRemoteDiedListener->onRemoteDied();
            } else {
                this->onRemoteDied();
            }
        }
    }

    virtual void onRemoteDied() {}
};