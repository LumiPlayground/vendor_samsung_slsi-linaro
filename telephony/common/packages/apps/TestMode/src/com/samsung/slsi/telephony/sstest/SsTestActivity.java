/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.telephony.sstest;

import android.app.Activity;
import android.content.Context;
import android.graphics.drawable.Icon;
import android.net.Uri;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;

import com.samsung.slsi.telephony.testmode.R;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

public class SsTestActivity extends PreferenceActivity {

    private static final String TAG = "SsTestActivity";
    private static final int ACCOUNT_ORDERING_START_VALUE = 100;

    private PreferenceScreen mPref;
    private TelecomManager mTelecomManager;
    private TelephonyManager mTelephonyManager;
    private SubscriptionManager mSubscriptionManager;

    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        mTelecomManager = TelecomManager.from(this);
        mTelephonyManager = TelephonyManager.from(this);
        mSubscriptionManager = SubscriptionManager.from(this);
        // mTelecomManager = (TelecomManager) this.getSystemService(Context.TELECOM_SERVICE);
        // mTelephonyManager = (TelephonyManager) this.getSystemService(Context.TELEPHONY_SERVICE);
        // mSubscriptionManager = (SubscriptionManager) this.getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE);
    }

    @Override
    public void onResume() {
        super.onResume();
        addPreferencesFromResource(R.layout.ss_test);
        mPref = getPreferenceScreen();
        updateAccounts();
    }

    private void updateAccounts() {
        if (mPref != null) {
            Log.i(TAG, "mPref is not null, remove all");
            mPref.removeAll();
        }

        List<PhoneAccountHandle> enabledAccounts =
            getCallingAccounts(true /* includeSims */, false /* includeDisabled */);

        // Obtain the list of phone accounts.
        List<PhoneAccount> accounts = new ArrayList<>();
        for (PhoneAccountHandle handle : enabledAccounts) {
            PhoneAccount account = mTelecomManager.getPhoneAccount(handle);
            if (account != null) {
                Log.i(TAG, "add accounts: " + account);
                accounts.add(account);
            } else {
                Log.i(TAG, "account is null");
            }
        }

        // Sort the accounts according to how we want to display them.
        Collections.sort(accounts, new Comparator<PhoneAccount>() {
            @Override
            public int compare(PhoneAccount account1, PhoneAccount account2) {
                int retval = 0;

                // SIM accounts go first
                boolean isSim1 = account1.hasCapabilities(PhoneAccount.CAPABILITY_SIM_SUBSCRIPTION);
                boolean isSim2 = account2.hasCapabilities(PhoneAccount.CAPABILITY_SIM_SUBSCRIPTION);
                if (isSim1 != isSim2) {
                    retval = isSim1 ? -1 : 1;
                }

                int subId1 = mTelephonyManager.getSubIdForPhoneAccount(account1);
                int subId2 = mTelephonyManager.getSubIdForPhoneAccount(account2);
                if (subId1 != SubscriptionManager.INVALID_SUBSCRIPTION_ID &&
                        subId2 != SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
                    retval = (mSubscriptionManager.getSlotIndex(subId1) <
                        mSubscriptionManager.getSlotIndex(subId2)) ? -1 : 1;
                }

                // Then order by package
                if (retval == 0) {
                    String pkg1 = account1.getAccountHandle().getComponentName().getPackageName();
                    String pkg2 = account2.getAccountHandle().getComponentName().getPackageName();
                    retval = pkg1.compareTo(pkg2);
                }

                // Finally, order by label
                if (retval == 0) {
                    String label1 = nullToEmpty(account1.getLabel().toString());
                    String label2 = nullToEmpty(account2.getLabel().toString());
                    retval = label1.compareTo(label2);
                }

                // Then by hashcode
                if (retval == 0) {
                    retval = account1.hashCode() - account2.hashCode();
                }
                return retval;
            }
        });

        int order = ACCOUNT_ORDERING_START_VALUE;

        for (PhoneAccount account : accounts) {
            PhoneAccountHandle handle = account.getAccountHandle();

            // SIM phone accounts use a different setting intent and are thus handled differently.
            if (account.hasCapabilities(PhoneAccount.CAPABILITY_SIM_SUBSCRIPTION)) {
                Log.i(TAG, "has CAPABILITY_SIM_SUBSCRIPTION");
                SubscriptionInfo subInfo = mSubscriptionManager.getActiveSubscriptionInfo(
                    mTelephonyManager.getSubIdForPhoneAccount(account));

                if (subInfo == null) {
                    Log.i(TAG, "subInfo is null");
                    continue;
                }

                // Create the preference & add the label
                SsEditTextPreference accountPreference = new SsEditTextPreference(this, subInfo);
                CharSequence accountLabel = account.getLabel();
                boolean isSimAccount =
                    account.hasCapabilities(PhoneAccount.CAPABILITY_SIM_SUBSCRIPTION);
                accountPreference.setTitle((TextUtils.isEmpty(accountLabel) && isSimAccount)
                            ? getString(R.string.phone_accounts_default_account_label) : accountLabel);

                // Add an icon.
                Icon icon = account.getIcon();
                if (icon != null) {
                    accountPreference.setIcon(icon.loadDrawable(this));
                }
                accountPreference.setDialogTitle(R.string.sstest_dialog_title);
                accountPreference.setOrder(order++);
                Log.i(TAG, "add EditTextPreference");
                mPref.addPreference(accountPreference);
            } else {
                Log.i(TAG, "no CAPABILITY_SIM_SUBSCRIPTION");
            }
        }
    }

    private List<PhoneAccountHandle> getCallingAccounts(
            boolean includeSims, boolean includeDisabledAccounts) {
        List<PhoneAccountHandle> accountHandles =
                mTelecomManager.getCallCapablePhoneAccounts(includeDisabledAccounts);
        for (Iterator<PhoneAccountHandle> i = accountHandles.iterator(); i.hasNext();) {
            PhoneAccountHandle handle = i.next();
            if (handle.getId().equals("E")) {
                Log.i(TAG, "getCallingAccounts has Emergency account");
                // never include emergency call accounts in this piece of code.
                i.remove();
                continue;
            }

            PhoneAccount account = mTelecomManager.getPhoneAccount(handle);
            if (account == null) {
                i.remove();
            } else if (!includeSims &&
                    account.hasCapabilities(PhoneAccount.CAPABILITY_SIM_SUBSCRIPTION)) {
                Log.i(TAG, "getCallingAccounts no CAPABILITY_SIM_SUBSCRIPTION");
                i.remove();
            }
        }
        return accountHandles;
    }

    private String nullToEmpty(String str) {
        return str == null ? "" : str;
    }
}
