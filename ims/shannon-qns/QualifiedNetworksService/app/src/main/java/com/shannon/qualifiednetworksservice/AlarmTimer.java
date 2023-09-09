/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice;

import android.app.AlarmManager;

public class AlarmTimer {
    private class AlarmTimerCallback implements AlarmManager.OnAlarmListener {
        public void onAlarm() {
            if (mRunnable != null) {
                mRunnable.run();
            }
        }
    }

    private final AlarmTimerCallback mCallback;
    private Runnable mRunnable;

    public AlarmTimer() {
        mRunnable = null;
        mCallback = new AlarmTimerCallback();
    }

    public void schedule(Runnable task, long timeout) {
        mRunnable = task;

        AlarmTimerManager manager = AlarmTimerManager.getInstance();
        if (manager != null) {
            manager.schedule(timeout, mCallback);
        }
    }

    public void cancel() {
        AlarmTimerManager manager = AlarmTimerManager.getInstance();
        if (manager != null) {
            manager.cancel(mCallback);
        }
    }
}
