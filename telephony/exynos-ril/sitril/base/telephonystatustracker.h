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
/*
 * telephonystatustracker.h
 *
 *  Created on: 2021. 10. 19.
 */

#ifndef __TELEPHONYSTATUSTRACKER_H__
#define __TELEPHONYSTATUSTRACKER_H__

#include <string>
#include <sstream>
#include <vector>
#include "basedef.h"

struct MonitoringCallback {
  virtual void dump(std::stringstream& ss) = 0;
  virtual ~MonitoringCallback()=default;
};

struct MonitoringItem {
    MonitoringCallback * callback;
    bool periodic;
};

struct MonitoringItemList {
    std::vector<MonitoringItem> mItems;
    void add(const MonitoringItem& r) {
        mItems.push_back(r);
    }

    void remove(const MonitoringCallback* r) {
        auto cur = std::begin(mItems);
        auto end = std::end(mItems);
        for (; cur != end; cur++) {
            if ((*cur).callback == r) {
                mItems.erase(cur);
                break;
            }
        }
    }

    bool find(const MonitoringCallback *r) {
        auto cur = std::begin(mItems);
        auto end = std::end(mItems);
        for (; cur != end; cur++) {
            if ((*cur).callback == r) {
                return true;
            }
        }
        return false;
    }

    void clear() { mItems.clear(); }
};

class TelephonyStatusTracker
{
    DECLARE_MODULE_TAG()
private:
    MonitoringItemList mList;
    int mPhoneId = -1;

public:
    TelephonyStatusTracker() {}
    virtual ~TelephonyStatusTracker() {}

public:
    void registerMonitoringCallback(MonitoringCallback *callback, bool periodic);
    void unregisterMonitoringCallback(MonitoringCallback *callback);

    void dump(bool forceDump);

    static TelephonyStatusTracker& createForPhoneId(int phoneId);
private:
    static TelephonyStatusTracker sTracker[];
};

#endif /* __TELEPHONYSTATUSTRACKER_H__ */
