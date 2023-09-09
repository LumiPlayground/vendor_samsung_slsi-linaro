/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <telephony/librilutils.h>
#include "timerlist.h"

#define MAX_TIMEOUT     (5 * 60 * 1000) // 5 min.

using namespace std;

int TimerList::compare(struct timeval &l, struct timeval &r)
{
    // l > r : positive
    // l = r : 0
    // l < r : negative
    if (l.tv_sec == r.tv_sec) {
        return l.tv_usec - r.tv_usec;
    }

    return l.tv_sec - r.tv_sec;
}

void TimerList::add(struct timeval &l, struct timeval &r)
{
    l.tv_sec = l.tv_sec + r.tv_sec;
    l.tv_usec = l.tv_usec + r.tv_usec;
    if (l.tv_usec > 1000000) {
        l.tv_usec -= 1000000;
        l.tv_sec += 1;
    }
}

void TimerList::add(struct timeval &tv, unsigned int millis)
{
    long usec = millis * 1000;
    struct timeval diff = {usec / 1000000, usec % 1000000};
    TimerList::add(tv, diff);
}

void TimerList::sub(struct timeval &l, struct timeval &r)
{
    if (TimerList::compare(l, r) <= 0) {
        l.tv_sec = l.tv_usec = 0;
    }
    else {
        l.tv_sec -= r.tv_sec;
        l.tv_usec -= r.tv_usec;
        if (l.tv_usec < 0) {
            l.tv_usec += 1000000;
            l.tv_sec -= 1;
        }
    }
}

struct timeval TimerList::getNow()
{
    struct timeval tv;
    uint64_t now = ril_nano_time();
    tv.tv_sec = (time_t)(now / 1000000000LL);
    tv.tv_usec = (suseconds_t)((now % 1000000000LL) / 1000);  // nsec to usec
    return tv;
}

struct timeval TimerList::getAtTime(unsigned int elapse)
{
    struct timeval now = TimerList::getNow();
    TimerList::add(now, elapse);
    return now;
}

TimerList::TimerList()
    : mMaxTimeout(MAX_TIMEOUT)
{
}

TimerEvent TimerList::front()
{
    TimerEvent ret = {{0, 0}, NULL};
    mLock.lock();
    if (mList.size() > 0) {
        ret = mList.front();
    }
    mLock.unlock();
    return ret;
}

TimerEvent TimerList::pop()
{
    TimerEvent ret = {{0, 0}, NULL};
    mLock.lock();
    if (mList.size() > 0) {
        ret = mList.front();
        mList.pop_front();
    }
    mLock.unlock();
    return ret;
}

void TimerList::put(Message *msg, unsigned int millis)
{
    if (msg != NULL) {
        // erase if already existed
        erase(msg);

        if (millis > mMaxTimeout) {
            millis = mMaxTimeout;
        }

        struct timeval tv = getNow();
        TimerList::add(tv, millis);
        TimerEvent te = {tv, msg};

        // sorted ASC
        list<TimerEvent>::iterator iter;
        mLock.lock();
        for (iter = mList.begin(); iter != mList.end(); iter++) {
            TimerEvent &cur = *iter;
            if (TimerList::compare(cur.timeout, te.timeout) > 0) {
                mList.insert(iter, te);
                break;
            }
        } // end for iter ~
        if (iter == mList.end()) {
            mList.push_back(te);
        }
        mLock.unlock();
    }
}

void TimerList::erase(Message *msg)
{
    list<TimerEvent>::iterator iter;
    mLock.lock();
    for (iter = mList.begin(); iter != mList.end(); iter++) {
        TimerEvent &te = *iter;
        if (te.msg == msg) {
            mList.erase(iter);
            break;
        }
    } // end for iter ~
    mLock.unlock();
}

int TimerList::size()
{
    mLock.lock();
    int size = mList.size();
    mLock.unlock();
    return size;
}
