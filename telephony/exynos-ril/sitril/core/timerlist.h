/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef    __TIMER_LIST_H__
#define    __TIMER_LIST_H__

#include <sys/time.h>
#include <list>
#include <sys/time.h>
#include "mutex.h"

class Message;

 typedef struct {
     struct timeval timeout;
     Message *msg;
 } TimerEvent;

 // sorted list
 class TimerList {
 private:
   std::list<TimerEvent> mList;
   unsigned int mMaxTimeout;
   CMutex mLock;

 public:
    TimerList();

 public:
     TimerEvent front();
     TimerEvent pop();
     void put(Message *msg, unsigned int millis);
     void erase(Message *msg);
     int size();
 public:
     static int compare(struct timeval &l, struct timeval &r);
     static void add(struct timeval &l, struct timeval &r);
     static void add(struct timeval &tv, unsigned int millis);
     static void sub(struct timeval &l, struct timeval &r);
     static struct timeval getNow();
     static struct timeval getAtTime(unsigned int elapse);
 };
#endif // __TIMER_LIST_H__
