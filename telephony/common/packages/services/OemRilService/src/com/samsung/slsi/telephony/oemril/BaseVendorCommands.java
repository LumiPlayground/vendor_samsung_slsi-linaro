/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.samsung.slsi.telephony.oemril;

import android.content.Context;
import android.os.Handler;
import android.os.RegistrantList;

import android.os.Message;

public abstract class BaseVendorCommands implements VendorCommandsInterface {

    protected Context mContext;
    protected Object mStateMonitor = new Object();
    protected RegistrantList mIccIdInfoRegistrants = new RegistrantList();
    protected RegistrantList mNrDualConnectivityStateRegistrants = new RegistrantList();
    protected RegistrantList mSimTrayStatusRegistrants = new RegistrantList();
    protected RegistrantList mDataNasTimerStatusRegistrants = new RegistrantList();
    protected RegistrantList mUnthrottleApnRegistrants = new RegistrantList();

    public BaseVendorCommands(Context context) {
        mContext = context;
    }

    @Override
    public void registerForIccIdInfo(Handler h, int what, Object obj) {
        synchronized (mStateMonitor) {
            mIccIdInfoRegistrants.addUnique(h, what, obj);
        }
    }

    @Override
    public void unregisterForIccIdInfo(Handler h) {
        synchronized (mStateMonitor) {
            mIccIdInfoRegistrants.remove(h);
        }
    }

    @Override
    public void registerForNrDualConnectivityState(Handler h, int what, Object obj) {
        synchronized (mStateMonitor) {
            mNrDualConnectivityStateRegistrants.addUnique(h, what, obj);
        }
    }

    @Override
    public void unregisterForNrDualConnectivityState(Handler h) {
        synchronized (mStateMonitor) {
            mNrDualConnectivityStateRegistrants.remove(h);
        }
    }

    @Override
    public void registerForSimTrayStatus(Handler h, int what, Object obj) {
        synchronized (mStateMonitor) {
            mSimTrayStatusRegistrants.addUnique(h, what, obj);
        }
    }

    @Override
    public void unregisterForSimTrayStatus(Handler h) {
        synchronized (mStateMonitor) {
            mSimTrayStatusRegistrants.remove(h);
        }
    }

    @Override
    public void registerForDataNasTimerStatusChanged(Handler h, int what, Object obj) {
        synchronized (mStateMonitor) {
            mDataNasTimerStatusRegistrants.addUnique(h, what, obj);
        }
    }

    @Override
    public void unregisterForDataNasTimerStatusChanged(Handler h) {
        synchronized (mStateMonitor) {
            mDataNasTimerStatusRegistrants.remove(h);
        }
    }

    @Override
    public void registerForUnthrottleApn(Handler h, int what, Object obj) {
        synchronized (mStateMonitor) {
            mUnthrottleApnRegistrants.addUnique(h, what, obj);
        }
    }

    @Override
    public void unregisterForUnthrottleApn(Handler h) {
        synchronized (mStateMonitor) {
            mUnthrottleApnRegistrants.remove(h);
        }
    }
}
