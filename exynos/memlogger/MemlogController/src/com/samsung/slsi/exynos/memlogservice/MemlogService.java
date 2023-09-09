/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.exynos.memlogservice;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicInteger;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import vendor.samsung_slsi.hardware.memlogservice.V1_0.*;

public class MemlogService {

	private Context mContext;
	private static MemlogService sInstance = null;
	private static final String TAG = "MemlogService";

    private IMemlogService mMemlogServiceMemlogdProxy = null;
    private IMemlogServiceCallback mMemlogServiceMemlogdCallback = null;

    public static MemlogService init(Context context) {
        Log.i(TAG, "Create new MemlogService instance");
        return new MemlogService(context);
    }

    public static MemlogService getInstance() {
        return sInstance;
    }

    public Context getContext() {
        return mContext;
    }

    public MemlogService(Context context) {
        Log.i(TAG, "MemlogService");
        mContext = context;
        mMemlogServiceMemlogdCallback = new MemlogServiceMemlogdCallback(this);
        getMemlogServiceMemlogdProxy();
    }

    private IMemlogService getMemlogServiceMemlogdProxy() {

        Log.i(TAG, "getMemlogServiceMemlogdProxy");
        if (mMemlogServiceMemlogdProxy != null) {
            return mMemlogServiceMemlogdProxy;
        }

        try {
            mMemlogServiceMemlogdProxy = IMemlogService.getService("memlog");
            if (mMemlogServiceMemlogdProxy != null) {
                // not calling linkToDeath() as ril service runs in the same process and death
                // notification for that should be sufficient
                Log.d(TAG, "getMemlogServiceMemlogdProxy: register callback");
                mMemlogServiceMemlogdProxy.setCallback(mMemlogServiceMemlogdCallback);
            } else {
                Log.e(TAG, "getMemlogServiceMemlogdProxy: mMemlogServiceMemlogdProxy == null");
            }
        } catch (RemoteException | RuntimeException e) {
            mMemlogServiceMemlogdProxy = null;
            Log.e(TAG, "MemlogServiceMemlogdProxy getService/setCallback: " + e);
        }
        return mMemlogServiceMemlogdProxy;
    }

    public void setLevel(int level, String str) {
        Log.i(TAG, "setLevel");
        ArrayList<Byte> data = primitiveArrayToArrayList(str.getBytes());
        IMemlogService memlogServiceProxy = getMemlogServiceMemlogdProxy();
        try {
            memlogServiceProxy.sendRequestRaw(MemlogServiceConstants.TYPE_COMMAND, level + MemlogServiceConstants.COMMAND_LEVEL_BASE, data);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void enableFile(boolean enabled, String str) {
        Log.i(TAG, "enableFile");
        ArrayList<Byte> data = primitiveArrayToArrayList(str.getBytes());
        IMemlogService memlogServiceProxy = getMemlogServiceMemlogdProxy();
        try {
            if (enabled)
                memlogServiceProxy.sendRequestRaw(MemlogServiceConstants.TYPE_COMMAND, MemlogServiceConstants.COMMAND_ENABLE_FILE, data);
            else
                memlogServiceProxy.sendRequestRaw(MemlogServiceConstants.TYPE_COMMAND, MemlogServiceConstants.COMMAND_DISABLE_FILE, data);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void updateFileList() {
        int i = 0;
        ArrayList<Byte> data = new ArrayList<>(1);
        Log.i(TAG, "updateFileList");
        IMemlogService memlogServiceProxy = getMemlogServiceMemlogdProxy();
        if (mMemlogServiceMemlogdProxy != null) {
            try {
                memlogServiceProxy.sendRequestRaw(MemlogServiceConstants.TYPE_COMMAND, MemlogServiceConstants.COMMAND_REFRESH_FILE_LIST, data);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }

    public void saveDumpstate() {
        int i = 0;
        ArrayList<Byte> data = new ArrayList<>(1);
        Log.i(TAG, "saveDumpstate");
        IMemlogService memlogServiceProxy = getMemlogServiceMemlogdProxy();
        if (mMemlogServiceMemlogdProxy != null) {
            try {
                memlogServiceProxy.sendRequestRaw(MemlogServiceConstants.TYPE_COMMAND, MemlogServiceConstants.COMMAND_SAVE_DUMPSTATE, data);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }

    public static ArrayList<Byte> primitiveArrayToArrayList(byte[] arr) {
        ArrayList<Byte> arrayList = new ArrayList<>(arr.length);
        for (byte b : arr) {
            arrayList.add(b);
        }
        return arrayList;
    }
}
