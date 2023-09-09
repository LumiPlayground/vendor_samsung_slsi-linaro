/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */

package com.shannon.vendorcarrierconfig.settings;

import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.telephony.Rlog;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import android.widget.ListView;

import com.shannon.qualifiednetworksservice.R;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig;

import java.util.ArrayList;
import java.util.Comparator;
import androidx.fragment.app.FragmentManager;

public class IwlanSettingActivity extends AppCompatActivity {
    static void setDialogOnListView(ListView listView, EditConfigDialogFragment.DialogEventListener dialogEventListener, String tag,
        FragmentManager fragmentManager) {
        listView.setOnItemClickListener((adapterView, itemView, position, id) -> {
            EditConfigDialogFragment dialogFragment = new EditConfigDialogFragment();
            dialogFragment.setSettingItem((SettingItem) adapterView.getItemAtPosition(position));
            dialogFragment.setListener(dialogEventListener);
            dialogFragment.show(fragmentManager, tag);
        });
    }
    private String TAG;
    private int mPhoneId;
    private Bundle globalBundle;
    private Button applyBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_iwlan_setting);

        if (findViewById(R.id.settingContainer) != null) {
            if (savedInstanceState != null) {
                return;
            }

            globalBundle = new Bundle();

            mPhoneId = 0;
            TAG = ShannonVendorCarrierConfig.getServiceTAG("-" + mPhoneId + "-UI");
            createSimPolicySettingFragment(mPhoneId);
        }

        applyBtn = (Button) findViewById(R.id.apply_button);

        applyBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                assembleBundle();
                Toast.makeText(getApplicationContext(), "Apply this Configurations", Toast.LENGTH_LONG).show();
                applyBtn.setEnabled(false);
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onResumeFragments() {
        super.onResumeFragments();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        getMenuInflater().inflate(R.menu.menu_setting, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        super.onOptionsItemSelected(item);
        switch (item.getItemId()) {
            case R.id.setting_menu_sim1_policy:
                mPhoneId = 0;
                globalBundle.clear();
                TAG = ShannonVendorCarrierConfig.getServiceTAG("-" + mPhoneId + "-UI");
                createSimPolicySettingFragment(0);
                break;

            case R.id.setting_menu_sim2_policy:
                mPhoneId = 1;
                globalBundle.clear();
                TAG = ShannonVendorCarrierConfig.getServiceTAG("-" + mPhoneId + "-UI");
                createSimPolicySettingFragment(1);
                break;

            case R.id.setting_menu_reset:
                resetSharedPreferences();
                break;
        }
        return true;
    }

    private void createSimPolicySettingFragment(int phoneId) {
        createConfigFragment(phoneId, getSimPolicySettingItems(phoneId));
    }

    private void resetSharedPreferences() {
        Log.d(TAG, "IwlanSetting - resetSharedPreferences");
        globalBundle.clear();
        ShannonVendorCarrierConfig.deleteSharedPreferencesFiles();

        Toast.makeText(getApplicationContext(), "Reset finished. Please reboot a device.", Toast.LENGTH_LONG).show();
    }

    @NonNull
    private ArrayList<SettingItem> getSimPolicySettingItems(int phoneId) {
        Bundle bundle = ShannonVendorCarrierConfig.getVendorCarrierConfigForSlot(phoneId);

        ArrayList<SettingItem> settingItems = new ArrayList<>();
        for (String key : bundle.keySet()) {
            settingItems.add(new SettingItem(key, bundle.get(key)));
        }

        settingItems.sort(new Comparator<SettingItem>() {
            @Override
            public int compare(SettingItem o1, SettingItem o2) {
                return o1.getTitle().compareTo(o2.getTitle());
            }
        });

        return settingItems;
    }

    private void createConfigFragment(int phoneId, ArrayList<SettingItem> settingItems) {
        SimSettingFragment fragment = SimSettingFragment.newInstance(phoneId, settingItems);
        getSupportFragmentManager().beginTransaction().replace(R.id.settingContainer, fragment).commit();
    }

    public void assembleBundle() {
        ShannonVendorCarrierConfig.updateVendorCarrierConfigForSlot(mPhoneId, globalBundle);
    }

    class SettingItem {
        final String mKey;
        Object mValue;

        SettingItem(String key, Object value) {
            mKey = key;
            mValue = value;
        }

        public String getKey() {
            return mKey;
        }

        public String getTitle() {
            return mKey.substring(mKey.lastIndexOf('.') + 1);
        }

        public String getDescription() {
            return "";
        }

        public String getValue() {
            if (mValue instanceof Boolean || mValue instanceof Integer) {
                return String.valueOf(mValue);
            }
            return (String)mValue;
        }

        public void updateHandler(String input) {
            Log.d(TAG, "IwlanSetting - key: " + mKey + " value: " + mValue + " by updateHandler( " + input + " )");

            boolean configChanged = false;

            if (input.equals(mValue.toString())) {
                return;
            } else if (("true".equals(mValue.toString()) && input.equals("1")) || ("false".equals(mValue.toString())
                    && input.equals("0"))) {
                return;
            } else {
                if (mValue instanceof Boolean) {
                    if ("false".equalsIgnoreCase(input) || "true".equalsIgnoreCase(input)) {
                        mValue = Boolean.valueOf(input);
                        globalBundle.putBoolean(mKey, Boolean.valueOf(input));
                        configChanged = true;
                    } else if ("0".equalsIgnoreCase(input) || "1".equalsIgnoreCase(input)) {
                        mValue = "1".equalsIgnoreCase(input);
                        globalBundle.putBoolean(mKey, "1".equalsIgnoreCase(input));
                        configChanged = true;
                    } else {
                        Toast.makeText(getApplicationContext(), "Please input correct value", Toast.LENGTH_LONG).show();
                    }
                } else if (mValue instanceof Integer) {
                    if (isNumeric(input)) {
                        mValue = Integer.parseInt(input);
                        globalBundle.putInt(mKey, Integer.parseInt(input));
                        configChanged = true;
                    } else {
                        Toast.makeText(getApplicationContext(), "Please input correct value", Toast.LENGTH_LONG).show();
                    }
                } else if (mValue instanceof String) {
                    mValue = input;
                    globalBundle.putString(mKey, input);
                    configChanged = true;
                } else {
                    Toast.makeText(getApplicationContext(), "Please input correct value", Toast.LENGTH_LONG).show();
                }
            }
            if (configChanged == true) {
                applyBtn.setEnabled(true);
            }
        }

        public String toString() {
            return "SettingItem{" +
                    "key=" + mKey + " value=" + mValue +
                    '}';
        }

        private void loge(String s) {
            Rlog.e(TAG, s);
        }
    }

    public static boolean isNumeric(String input) {
        try {
            Double.parseDouble(input);
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }
}
