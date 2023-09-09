/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import java.util.Timer;
import java.util.TimerTask;

public class DnsResourceRecord {
    public interface DnsResourceCallback {
        public void onResourceExpired(DnsResourceRecord record);
    }

    protected class DnsResourceTimerTask extends TimerTask {
        private final DnsResourceRecord mRecord;

        DnsResourceTimerTask(DnsResourceRecord record) {
            mRecord = record;
        }

        @Override
        public void run() {
            if (mCallback != null) {
                mCallback.onResourceExpired(mRecord);
            }
        }
    }

    private final DnsResourceCallback mCallback;
    private final Timer mTimer;

    DnsResourceRecord(DnsResourceCallback callback, int ttl) {
        mCallback = callback;

        mTimer = new Timer(DnsResourceRecord.class.getSimpleName());
        if (ttl > 0) {
            mTimer.schedule(new DnsResourceTimerTask(this), ttl * 1000);
        }
    }

    public void destroy() {
        mTimer.cancel(); mTimer.purge();
    }
}
