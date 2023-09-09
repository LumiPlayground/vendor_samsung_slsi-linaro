/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.telephony.ursp;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import com.samsung.slsi.telephony.networktestmode.R;

public class UrspSettingActivity extends Activity {

    private UrspSettingPreferenceFragment fragment;

    @Override
    protected void onCreate(Bundle saveInstance) {
        super.onCreate(saveInstance);

        if (saveInstance == null) {
            FragmentTransaction ft = getFragmentManager().beginTransaction();
            Fragment newFrag = SelectSimPreferenceFragment.newInstance();
            ft.add(android.R.id.content, newFrag);
            ft.commit();
        }
    }

    public void onNextFragment(int phoneId) {
        fragment = UrspSettingPreferenceFragment.newInstance(phoneId);
        getFragmentManager().beginTransaction()
                .replace(android.R.id.content, fragment).commit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        getMenuInflater().inflate(R.menu.menu_settings, menu);

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.action_save) {
            if (fragment != null) {
                fragment.saveMode();
            }
        }
        return super.onOptionsItemSelected(item);
    }
}