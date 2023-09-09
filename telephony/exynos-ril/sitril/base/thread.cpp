/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "thread.h"
#include "rillog.h"

Thread::Thread(Runnable *pRunnable/* = NULL*/) : m_bStarted(false), mRunnable(nullptr) {
    if (pRunnable) {
        mRunnable = pRunnable;
    }
}

Thread::~Thread() {
    Stop();
}

int Thread::Start() {
    if (!m_bStarted) {
        mWorker = std::thread([this]() { this->Run(); });
        m_bStarted = true;
    }
    return 0;
}

int Thread::Stop() {
    if (m_bStarted && mWorker.joinable()) {
        mWorker.join();
    }
    m_bStarted = false;
    return 0;
}

void Thread::Detach() {
    if (m_bStarted && mWorker.joinable()) {
        mWorker.detach();
    }
}

std::thread::id Thread::GetTid() const {
    return mWorker.get_id();
}

void Thread::Run() {
    if (mRunnable != NULL) {
        mRunnable->Run();
    }
}
