/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/**
 * Thread wrapper class
 *
 * refer java Thread and thread-runnable model
 *
 * @author sungwoo48.choi@samsung.com
 *
 */


#ifndef __THREAD_H__
#define __THREAD_H__

#include <thread>

class Runnable {
public:
    virtual ~Runnable()=default;
    virtual void Run()=0;
};

class Thread {
protected:
    bool m_bStarted;
    Runnable *mRunnable;
    std::thread mWorker;

private:
    static void *ThreadProc(void *arg);

public:
    Thread(Runnable *pRunnable = NULL);
    virtual ~Thread();

    int Start();
    int Stop();
    void Detach();
    virtual void Run();
    std::thread::id GetTid() const;

    template<class F, class... Args>
    static std::thread Execute(F&& f, Args&&... args) {
        return std::thread(f, args...);
    }
};
#endif
