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
import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.SystemClock;

public class AlarmTimerManager {
    private static AlarmTimerManager sMe = null;
    public static AlarmTimerManager getInstance() { return sMe; }

    private final AlarmManager mAlarmManager;
    private final HandlerThread mHandlerThread;
    private final Handler mHandler;

    public AlarmTimerManager(Context context) {
        mAlarmManager = context.getSystemService(AlarmManager.class);
        mHandlerThread = new HandlerThread(AlarmTimerManager.class.getSimpleName());
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper());

        sMe = this;
    }

    public void destroy() {
        mHandlerThread.quitSafely();

        sMe = null;
    }

    public void schedule(long timeout, AlarmManager.OnAlarmListener listener) {
        mAlarmManager.setExact(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock.elapsedRealtime() + timeout, null,
                listener, mHandler);
    }

    public void cancel(AlarmManager.OnAlarmListener listener) {
        mAlarmManager.cancel(listener);
    }
}
