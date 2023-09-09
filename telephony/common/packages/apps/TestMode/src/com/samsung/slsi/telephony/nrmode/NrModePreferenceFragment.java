/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.nrmode;

import java.io.IOException;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.SystemProperties;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceFragment;
import android.preference.SwitchPreference;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.WindowManager;
import android.widget.Toast;

import com.samsung.slsi.telephony.testmode.R;
import com.samsung.slsi.telephony.oem.OemRil;
import com.samsung.slsi.telephony.oem.io.DataWriter;

public class NrModePreferenceFragment extends PreferenceFragment {

    private static final String TAG = "NrTest";
    private ListPreference mPhoneIdPref, mNrModeListPref, mEndcModeListPref, mVonrCapaListPref;
    private SwitchPreference mNrDsModePref;
    private TelephonyManager mTelephonyManager;
    private SubscriptionManager mSubscriptionManager;
    private int[] mNrModeType = new int[2];
    private int[] mEndcModeType = new int[2];
    private int[] mVonrCapa = new int[2];
    private int mSubId = SubscriptionManager.INVALID_SUBSCRIPTION_ID;
    private int mPhoneId = -1;
    private OemRil[] mOemRil = new OemRil[2];

    private static final int EVENT_RIL_CONNECTED        = 100;
    private static final int EVENT_RIL_DISCONNECTED     = 101;
    private static final int EVENT_SET_NR_MODE          = 102;
    private static final int EVENT_GET_NR_MODE_0        = 103;
    private static final int EVENT_GET_NR_MODE_1        = 104;
    private static final int EVENT_SET_ENDC_MODE        = 105;
    private static final int EVENT_GET_ENDC_MODE_0      = 106;
    private static final int EVENT_GET_ENDC_MODE_1      = 107;
    private static final int EVENT_SET_VONR_CAPA        = 108;
    private static final int EVENT_GET_VONR_CAPA_0      = 109;
    private static final int EVENT_GET_VONR_CAPA_1      = 110;

    private static final int RILC_REQ_MODEM_RESET       = 22;
    private static final int RILC_REQ_SET_ENDC_MODE     = 27;
    private static final int RILC_REQ_GET_ENDC_MODE     = 28;
    private static final int RILC_REQ_SET_NR_MODE       = 43;
    private static final int RILC_REQ_GET_NR_MODE       = 44;
    private static final int RILC_REQ_SET_VONR_CAPA     = 57;
    private static final int RILC_REQ_GET_VONR_CAPA     = 58;

    private static final String PROPERTY_SUPPORT_NR_DS = "persist.vendor.ril.support_nr_ds";

    private Handler mHandler = new Handler() {
        @Override
            public void handleMessage(Message msg) {
                AsyncResult ar = (AsyncResult) msg.obj;
                switch (msg.what) {
                    case EVENT_SET_NR_MODE:
                        if (ar.exception == null) {
                            Log.d(TAG, "Success");
                        } else {
                            Log.d(TAG, "Fail: " + msg.what);
                        }
                        getNrMode();
                        // Don't need to reset a modmen due to SOC-94770
                        //resetRadioPower();
                        break;
                    case EVENT_SET_ENDC_MODE:
                        if (ar.exception == null) {
                            Log.d(TAG, "Success");
                        } else {
                            Log.d(TAG, "Fail: " + msg.what);
                        }
                        getEndcMode();
                        break;
                    case EVENT_GET_NR_MODE_0:
                        if (ar.exception == null) {
                            mNrModeType[0] = ((byte []) ar.result)[0];
                            Log.d(TAG, "[0]Success to get NR mode: " + mNrModeType[0]);
                        } else {
                            Log.d(TAG, "[0]Fail: " + msg.what);
                        }
                        updateUI();
                        break;
                    case EVENT_GET_NR_MODE_1:
                        if (ar.exception == null) {
                            mNrModeType[1] = ((byte []) ar.result)[0];
                            Log.d(TAG, "[1]Success to get NR mode: " + mNrModeType[1]);
                        } else {
                            Log.d(TAG, "[1]Fail: " + msg.what);
                        }
                        updateUI();
                        break;
                    case EVENT_GET_ENDC_MODE_0:
                        if (ar.exception == null) {
                            mEndcModeType[0] = ((byte []) ar.result)[0];
                            Log.d(TAG, "[0]Success to get ENDC mode: " + mEndcModeType[0]);
                        } else {
                            Log.d(TAG, "[0]Fail: " + msg.what);
                        }
                        updateUI();
                        break;
                    case EVENT_GET_ENDC_MODE_1:
                        if (ar.exception == null) {
                            mEndcModeType[1] = ((byte []) ar.result)[0];
                            Log.d(TAG, "[1]Success to get ENDC mode: " + mEndcModeType[1]);
                        } else {
                            Log.d(TAG, "[1]Fail: " + msg.what);
                        }
                        updateUI();
                        break;
                    case EVENT_SET_VONR_CAPA:
                        if (ar.exception == null) {
                            Log.d(TAG, "Success");
                        } else {
                            Log.d(TAG, "Fail: " + msg.what);
                        }
                        getVonrCapa();
                        // Don't need to reset a modmen due to SOC-94770
                        //resetRadioPower();
                        break;
                    case EVENT_GET_VONR_CAPA_0:
                        if (ar.exception == null) {
                            mVonrCapa[0] = ((byte []) ar.result)[0];
                            Log.d(TAG, "[0]Success to get VoNR Capability: " + mVonrCapa[0]);
                        } else {
                            Log.d(TAG, "[0]Fail: " + msg.what);
                        }
                        updateUI();
                        break;
                    case EVENT_GET_VONR_CAPA_1:
                        if (ar.exception == null) {
                            mVonrCapa[1] = ((byte []) ar.result)[0];
                            Log.d(TAG, "[1]Success to get VoNR Capability: " + mVonrCapa[1]);
                        } else {
                            Log.d(TAG, "[1]Fail: " + msg.what);
                        }
                        updateUI();
                        break;
                }
            }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.layout.preference_nrmode);

        mTelephonyManager = (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
        mSubscriptionManager = (SubscriptionManager) getActivity().getSystemService(Context.TELEPHONY_SUBSCRIPTION_SERVICE);

        mPhoneIdPref = (ListPreference) findPreference("key_phoneid_nr_mode");
        mPhoneIdPref.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                preference.setSummary("Select phone");
                int pos = mPhoneId = mPhoneIdPref.findIndexOfValue(newValue.toString());
                if (pos >= 0) {
                    mPhoneIdPref.setSummary(mPhoneIdPref.getEntries()[pos]);
                    mPhoneIdPref.setValueIndex(pos);
                }
                updateSubId(Integer.parseInt(newValue.toString()));
                return true;
            }
        });

        mNrModeListPref = (ListPreference) findPreference("key_nr_mode");
        mNrModeListPref.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                preference.setSummary("");
                int pos = mNrModeListPref.findIndexOfValue(newValue.toString());
                if (pos >= 0) {
                    setNrMode(mPhoneId, Integer.parseInt(newValue.toString()));
                }
                return true;
            }
        });

        mEndcModeListPref = (ListPreference) findPreference("key_endc_mode");
        mEndcModeListPref.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                preference.setSummary("");
                int pos = mEndcModeListPref.findIndexOfValue(newValue.toString());
                if (pos >= 0) {
                    setEndcMode(mPhoneId, Integer.parseInt(newValue.toString()));
                }
                return true;
            }
        });

        mVonrCapaListPref = (ListPreference) findPreference("key_vonr_capa");
        mVonrCapaListPref.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                preference.setSummary("");
                int pos = mVonrCapaListPref.findIndexOfValue(newValue.toString());
                if (pos >= 0) {
                    setVonrCapa(mPhoneId, Integer.parseInt(newValue.toString()));
                }
                return true;
            }
        });

        mNrDsModePref = (SwitchPreference) findPreference("key_nr_ds_mode");
        mNrDsModePref.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {

            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {
                String mode = (Boolean)newValue ? "1" : "0";
                SystemProperties.set(PROPERTY_SUPPORT_NR_DS, mode);
                reboot();
                return true;
            }
        });

        connectToOemRilService();
        getNrMode();
        getEndcMode();
        getVonrCapa();
        updateUI();
    }


    public static NrModePreferenceFragment newInstance() {
        NrModePreferenceFragment frag = new NrModePreferenceFragment();
        return frag;
    }

    @Override
    public void onResume() {
        boolean isNrDsSupported = SystemProperties.getInt(PROPERTY_SUPPORT_NR_DS, 0) == 1;
        mNrDsModePref.setChecked(isNrDsSupported);
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        for (int i = 0; i < 2; i++) {
            if (mOemRil[i] != null) {
            mOemRil[i].unregisterForOemRilConnected(mHandler);
            mOemRil[i].unregisterForOemRilDisconnected(mHandler);
            mOemRil[i].detach();
            }
        }
    }

    private void connectToOemRilService() {
        for (int i = 0; i < 2; i++) {
            mOemRil[i] = OemRil.init(getActivity(), i);
            if (mOemRil[i] == null) {
                Log.d(TAG, "connectToOemRilService mOemRil"+i+" is null");
            } else {
                mOemRil[i].registerForOemRilConnected(mHandler, EVENT_RIL_CONNECTED);
                mOemRil[i].registerForOemRilDisconnected(mHandler, EVENT_RIL_DISCONNECTED);
            }
        }
    }

    private void updateUI() {
        String stringValue = mPhoneIdPref.getValue();
        int pos = mPhoneId = mPhoneIdPref.findIndexOfValue(stringValue);
        mPhoneIdPref.setSummary(pos >= 0 ? mPhoneIdPref.getEntries()[pos] : "Select phone");
        if (!TextUtils.isEmpty(stringValue))
            updateSubId(Integer.parseInt(stringValue.toString()));

        mNrModeListPref.setSummary("Phone0: " + mNrModeListPref.getEntries()[mNrModeType[0]]
                + "\nPhone1: " + mNrModeListPref.getEntries()[mNrModeType[1]]);
        mEndcModeListPref.setSummary("Phone0: " + mEndcModeListPref.getEntries()[mEndcModeType[0]]
                + "\nPhone1: " + mEndcModeListPref.getEntries()[mEndcModeType[1]]);
        mVonrCapaListPref.setSummary("Phone0: " + mVonrCapaListPref.getEntries()[mVonrCapa[0]]
                + "\nPhone1: " + mVonrCapaListPref.getEntries()[mVonrCapa[1]]);
    }

    private void updateSubId(int phoneId) {
        int subId = SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        if (SubscriptionManager.isValidPhoneId(phoneId)) {
            int[] subIds = SubscriptionManager.getSubId(phoneId);
            if (subIds != null) {
                mSubId = subIds[0];
            } else {
                mSubId = -1;
            }
        } else {
            mSubId = -1;
        }
        Log.d(TAG, "updateSubId() : phone("+phoneId+"), subId("+mSubId+")");
    }

    private void setNrMode(int phoneId, int nrMode) {
        Log.d(TAG, "Set NR mode phone("+phoneId+")="+nrMode);
        if (mSubId < 0) {
            Log.d(TAG, "Invalid SubId (" + mSubId + "), phoneId("+mPhoneId+")");
            Toast.makeText(getActivity().getApplicationContext(),
                    "Invalid Phone(" + mPhoneId + "), Please check SIM status", Toast.LENGTH_LONG).show();
            getNrMode();
            return ;
        }
        DataWriter dr = new DataWriter();
        try {
            dr.writeInt(nrMode);
        } catch (IOException e) {
            Log.i(TAG, "SendData() IOException" + e);
        }

        if (mOemRil[phoneId] != null) {
            mOemRil[phoneId].invokeRequestRaw(RILC_REQ_SET_NR_MODE,
                    dr.toByteArray(), mHandler.obtainMessage(EVENT_SET_NR_MODE));
        }
    }

    private void getNrMode() {
        if (mOemRil[0] != null) {
            mOemRil[0].invokeRequestRaw(RILC_REQ_GET_NR_MODE,
                    null, mHandler.obtainMessage(EVENT_GET_NR_MODE_0));
        }

        if (mOemRil[1] != null) {
            mOemRil[1].invokeRequestRaw(RILC_REQ_GET_NR_MODE,
                    null, mHandler.obtainMessage(EVENT_GET_NR_MODE_1));
        }
    }

    private void setEndcMode(int phoneId, int endc) {
        Log.d(TAG, "Set Endc mode phone("+phoneId+")="+endc);
        if (mSubId < 0) {
            Log.d(TAG, "Invalid SubId (" + mSubId + "), phoneId("+mPhoneId+")");
            Toast.makeText(getActivity().getApplicationContext(),
                    "Invalid Phone(" + mPhoneId + "), Please check SIM status", Toast.LENGTH_LONG).show();
            getEndcMode();
            return ;
        }

        DataWriter dr = new DataWriter();
        try {
            dr.writeInt(endc);
        } catch (IOException e) {
            Log.i(TAG, "SendData() IOException" + e);
        }

        if (mOemRil[phoneId] != null) {
            mOemRil[phoneId].invokeRequestRaw(RILC_REQ_SET_ENDC_MODE,
                    dr.toByteArray(), mHandler.obtainMessage(EVENT_SET_ENDC_MODE));
        }
    }

    private void getEndcMode() {
        if (mOemRil[0] != null) {
            mOemRil[0].invokeRequestRaw(RILC_REQ_GET_ENDC_MODE,
                    null, mHandler.obtainMessage(EVENT_GET_ENDC_MODE_0));
        }
        if (mOemRil[1] != null) {
            mOemRil[1].invokeRequestRaw(RILC_REQ_GET_ENDC_MODE,
                    null, mHandler.obtainMessage(EVENT_GET_ENDC_MODE_1));
        }
    }

    private void resetRadioPower() {
        Log.d(TAG, "Set Radio Power");
        DataWriter dr = new DataWriter();
        try {
            dr.writeByte((byte)(0xff & 4));
        } catch (IOException e) {
            Log.i(TAG, "SendData() IOException" + e);
        }

        for (int i = 0; i < 2; i++) {
            if (mOemRil[i] != null) {
                mOemRil[i].invokeRequestRaw(RILC_REQ_MODEM_RESET, null, null);
            }
        }
    }

    private void setVonrCapa(int phoneId, int vonrCapa) {
        Log.d(TAG, "Set VoNR Capability phone("+phoneId+")="+vonrCapa);
        if (mSubId < 0) {
            Log.d(TAG, "Invalid SubId (" + mSubId + "), phoneId("+mPhoneId+")");
            Toast.makeText(getActivity().getApplicationContext(),
                    "Invalid Phone(" + mPhoneId + "), Please check SIM status", Toast.LENGTH_LONG).show();
            getVonrCapa();
            return ;
        }
        DataWriter dr = new DataWriter();
        try {
            dr.writeInt(vonrCapa);
        } catch (IOException e) {
            Log.i(TAG, "SendData() IOException" + e);
        }

        if (mOemRil[phoneId] != null) {
            mOemRil[phoneId].invokeRequestRaw(RILC_REQ_SET_VONR_CAPA,
                    dr.toByteArray(), mHandler.obtainMessage(EVENT_SET_VONR_CAPA));
        }
    }

    private void getVonrCapa() {
        if (mOemRil[0] != null) {
            mOemRil[0].invokeRequestRaw(RILC_REQ_GET_VONR_CAPA,
                    null, mHandler.obtainMessage(EVENT_GET_VONR_CAPA_0));
        }

        if (mOemRil[1] != null) {
            mOemRil[1].invokeRequestRaw(RILC_REQ_GET_VONR_CAPA,
                    null, mHandler.obtainMessage(EVENT_GET_VONR_CAPA_1));
        }
    }

    private void reboot() {
        final PowerManager pm = (PowerManager) getContext().getSystemService(Context.POWER_SERVICE);
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setMessage("Reboot the device")
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        pm.reboot(null);
                    }
                }).setNegativeButton("Cancel", null);
        final AlertDialog resetDialog = builder.create();
        resetDialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        resetDialog.show();
    }
}
