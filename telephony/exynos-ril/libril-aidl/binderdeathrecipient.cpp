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
#include "binderdeathrecipient.h"

static void binderDied(void *cookie) {
    auto recipient = static_cast<BinderDeathRecipient *>(cookie);
    recipient->onBinderDied();
}

BinderDeathRecipient::BinderDeathRecipient(BinderDiedListener *binderDiedListener) :
    mBinderDiedListener(binderDiedListener),
    mBinder(nullptr), mRecipient(nullptr), mLinked(false), mCookie(0UL) {
    mRecipient = AIBinder_DeathRecipient_new(binderDied);
}

BinderDeathRecipient::~BinderDeathRecipient() {
    unlinkToDeath();
    AIBinder_DeathRecipient_delete(mRecipient);
}

bool BinderDeathRecipient::linkToDeath(AIBinder *binder, uint64_t cookie) {
    if (!mBinder) {
        if (AIBinder_linkToDeath(binder, mRecipient, static_cast<void *>(this)) == STATUS_OK) {
            mBinder = binder;
            mCookie = cookie;
            return true;
        }
    }
    return false;
}

void BinderDeathRecipient::unlinkToDeath() {
    if (mBinder) {
        AIBinder_unlinkToDeath(mBinder, mRecipient, nullptr);
        mBinder = nullptr;
    }
}

void BinderDeathRecipient::onBinderDied() {
    mBinder = nullptr;
    if (mBinderDiedListener) {
        mBinderDiedListener->onBinderDied(mCookie);
    }
}