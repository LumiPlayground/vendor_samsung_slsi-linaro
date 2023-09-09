/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.exynos.memlogcontroller;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.nio.channels.FileChannel;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.StringTokenizer;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Toast;

import com.samsung.slsi.exynos.memlogservice.MemlogService;
/**
 * Created by jin-h.shin on 2015-12-07.
 */
public class MemlogControllerControlInterface {

    private static final String TAG = "MemlogControllerControlInterface";
    private ProgressDialog progressDialog;
    private static Context mContext = null;
    private static Toast mToast = null;
    static final int RETRY_MILLIS = 4 * 1000;
    private static boolean initFlag = false;

    private MemlogService mMemlogService;

    private boolean mEnforcing = true;

    private static class ControlInterfaceLoader {
        public static MemlogControllerControlInterface sInstance = new MemlogControllerControlInterface();
    }

    public static MemlogControllerControlInterface getInstance() {
        return ControlInterfaceLoader.sInstance;
    }

    public static void setContext(Context context) {
        mContext = context;
    }

    public MemlogService getMemlogService() {
        return mMemlogService;
    }

    public void initialize() {
        Log.i(TAG, "initial flag : " + initFlag);
        if (!initFlag) {
            connectToMemlogService();
            initFlag = true;
        }
    }

    private void connectToMemlogService() {
        mMemlogService = MemlogService.init(mContext);
        if (mMemlogService == null) {
            Log.e(TAG, "connectToMemlogService : mMemlogService is null");
        }
    }

    public static void showToastMessage(String str) {
        if (mToast == null)
            mToast = Toast.makeText(mContext, str, Toast.LENGTH_LONG);
        else
            mToast.setText(str);
        mToast.show();
    }

    private void delay(int delay) {
        try {
            Thread.sleep(delay);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void setLevel(int level, String str) {
        mMemlogService.setLevel(level, str);
    }

    public void enableItem(boolean enabled, String str) {
        mMemlogService.enableFile(enabled, str);
    }

    public void saveDumpstate() { mMemlogService.saveDumpstate(); }

    public boolean loadItemsFromDB() {
        mMemlogService.updateFileList();
        return true ;
    }

    public boolean addItem(boolean enabled, byte level, byte type, String str) {
        ObjBtnItem item;
        DescBtnAdapter adapter = MemlogControllerActivity.adapter;
        StringTokenizer tkn = new StringTokenizer(str, "$");

        String group = tkn.nextToken();
        String dummy = tkn.nextToken(); // skip directory number
        String child = tkn.nextToken();

        ((MemlogControllerActivity) mContext).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                adapter.addChild(group, child, enabled, level, type, str);
            }
        });

        if (adapter.DescList.isEmpty()) {
            MemlogControllerActivity.listview.setVisibility(View.GONE);
            MemlogControllerActivity.emptyView.setVisibility(View.VISIBLE);
        } else {
            MemlogControllerActivity.listview.setVisibility(View.VISIBLE);
            MemlogControllerActivity.emptyView.setVisibility(View.GONE);
        }

        return true;
    }

    public boolean removeItem(String str) {
        ObjBtnItem item;
        String group, child;
        StringTokenizer tkn = new StringTokenizer(str, "$");
        DescBtnAdapter adapter = MemlogControllerActivity.adapter;

        group = tkn.nextToken();
        child = tkn.nextToken();

        DescItem grp = adapter.getGroup(group, false);
        if (grp == null) {
            Log.i(TAG, "removeItem: group not found: " + group);
            return false;
        }

        grp.removeChild(child);
        adapter.notifyDataSetChanged();
        return true;
    }
}
