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

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;

import android.widget.ExpandableListView;
import android.app.Dialog;
import android.app.Activity;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.StatFs;
import android.text.TextUtils;
import android.text.format.Formatter;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

public class MemlogControllerActivity extends Activity {

    private static final String TAG = "MemlogControllerActivity";
    static ExpandableListView listview ;
    static TextView emptyView;
    static DescBtnAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_silent_logging);

        MemlogControllerControlInterface.getInstance().setContext(this);
        MemlogControllerControlInterface.getInstance().initialize();
        initUI();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_silent_logging, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_transaction) {
            Intent intent = new Intent(this, MemlogControllerSettings.class);
            startActivity(intent);
            return true;
        } else if (id == R.id.action_refresh) {
            adapter.DescList.clear();
            MemlogControllerControlInterface.getInstance().loadItemsFromDB();
            adapter.notifyDataSetChanged();
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        if (adapter != null)
            adapter.setContext(this);
    }

    private void initUI() {
        emptyView = findViewById(R.id.emptylist);
        MemlogControllerControlInterface.getInstance().loadItemsFromDB();
        if (adapter == null)
            adapter = new DescBtnAdapter(this, R.layout.list_group, R.layout.list_item);

        listview = (ExpandableListView) findViewById(R.id.mainlist);
        listview.setEmptyView(emptyView);
        listview.setAdapter(adapter);
    }
}
