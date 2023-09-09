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

import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetAddress;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.NetworkOnMainThreadException;
import android.os.SystemProperties;
import android.preference.EditTextPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.preference.SwitchPreference;
import android.text.TextUtils;
import android.util.Log;
import android.view.inputmethod.InputMethodManager;
import android.widget.Toast;

import com.samsung.slsi.telephony.networktestmode.R;
import com.samsung.slsi.telephony.oem.OemRil;
import com.samsung.slsi.telephony.oem.io.DataWriter;


public class UrspSettingPreferenceFragment extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener {

    private static final String TAG = "UrspSettingPreferenceFragment";
    private SwitchPreference mUrspTestModePref;
    private EditTextPreference mUrspOsIdPref, mUrspAppIdPref, mFqdnPref,
            mIpv4AddrPref, mIpv4MaskPref, mIpv6AddrPref, mPrefixLenPref,
            mNextIpHdrPref, mSinglePortPref,mPortRangeLowLimitPref, mPortRangeHighLimitPref;
    private static int mPhoneId = -1;
    private static final String PROPERTY_URSP_TEST_MODE = "persist.vendor.ril.ursptestmode";
    private OemRil mOemRil;

    private static final int InvalidParam = -1;
    private static final int ValidParam = 3;
    private static final String InvalidStringParam = "INVALID";
    private static final String InvalidIPv4Param = "0.0.0.0";
    private static final String InvalidIPv6Param = "ffee::1";

    private static final int MAX_OS_ID_LEN = 16;
    private static final int MAX_ADDR_STRING_LEN = 256;
    private static final int MAX_IPV4_ADDR_LEN = 4;
    private static final int MAX_IPV6_ADDR_LEN = 16;

    /* Handle Message */
    private static final int EVENT_RIL_CONNECTED = 100;
    private static final int EVENT_RIL_DISCONNECTED = 101;
    private static final int EVENT_SET_URSP_DONE = 102;

    /* RIL Request */
    private static final int RILC_REQ_OEM_SET_URSP = 50;

    private final Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            AsyncResult ar = (AsyncResult) msg.obj;
            switch (msg.what) {
            case EVENT_RIL_CONNECTED:
                Log.d(TAG, "RIL connected");
                break;
            case EVENT_RIL_DISCONNECTED:
                Log.d(TAG, "RIL disconnected");
                getActivity().finish();
                break;
            case EVENT_SET_URSP_DONE:
                // if (ar.exception == null) {
                // }
                break;
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.layout.preference_ursp_setting);

        mUrspTestModePref = (SwitchPreference) getPreferenceScreen().findPreference("key_usrp_test_mode");

        mUrspOsIdPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_ursp_os");
        mUrspAppIdPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_ursp_app");
        mFqdnPref = (EditTextPreference) getPreferenceScreen().findPreference(
                "key_fqdn");
        mIpv4AddrPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_ipv4_addr");
        mIpv4MaskPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_ipv4_mask");
        mIpv6AddrPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_ipv6_addr");
        mPrefixLenPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_prefix_len");
        mNextIpHdrPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_next_ip_hdr");
        mSinglePortPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_single_port_range");
        mPortRangeLowLimitPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_port_range_low_limit");
        mPortRangeHighLimitPref = (EditTextPreference) getPreferenceScreen()
                .findPreference("key_port_range_high_limit");

        mUrspTestModePref.setOnPreferenceChangeListener(this);
        mUrspOsIdPref.setOnPreferenceChangeListener(this);
        mUrspAppIdPref.setOnPreferenceChangeListener(this);
        mFqdnPref.setOnPreferenceChangeListener(this);
        mIpv4AddrPref.setOnPreferenceChangeListener(this);
        mIpv4MaskPref.setOnPreferenceChangeListener(this);
        mIpv6AddrPref.setOnPreferenceChangeListener(this);
        mPrefixLenPref.setOnPreferenceChangeListener(this);
        mNextIpHdrPref.setOnPreferenceChangeListener(this);
        mSinglePortPref.setOnPreferenceChangeListener(this);
        mPortRangeLowLimitPref.setOnPreferenceChangeListener(this);
        mPortRangeHighLimitPref.setOnPreferenceChangeListener(this);

        SharedPreferences sharedPref = PreferenceManager
                .getDefaultSharedPreferences(getContext());
        mUrspOsIdPref.setSummary(sharedPref.getString("key_ursp_os", ""));
        mUrspAppIdPref.setSummary(sharedPref.getString("key_ursp_app", ""));
        mFqdnPref.setSummary(sharedPref.getString("key_fqdn", ""));
        mIpv4AddrPref.setSummary(sharedPref.getString("key_ipv4_addr", ""));
        mIpv4MaskPref.setSummary(sharedPref.getString("key_ipv4_mask", ""));
        mIpv6AddrPref.setSummary(sharedPref.getString("key_ipv6_addr", ""));
        mPrefixLenPref.setSummary(sharedPref.getString("key_prefix_len", ""));
        mNextIpHdrPref.setSummary(sharedPref.getString("key_next_ip_hdr", ""));
        mSinglePortPref.setSummary(sharedPref.getString(
                "key_single_port_range", ""));
        mPortRangeLowLimitPref.setSummary(sharedPref.getString(
                "key_port_range_low_limit", ""));
        mPortRangeHighLimitPref.setSummary(sharedPref.getString(
                "key_port_range_high_limit", ""));
        connectToOemRilService();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mOemRil != null) {
            mOemRil.unregisterForOemRilConnected(mHandler);
            mOemRil.unregisterForOemRilDisconnected(mHandler);
            mOemRil.detach();
        }
    }

    private void connectToOemRilService() {
        mOemRil = OemRil.init(getActivity(), mPhoneId);
        if (mOemRil == null) {
            Log.d(TAG, "connectToOemRilService mOemRil is null");
        } else {
            mOemRil.registerForOemRilConnected(mHandler, EVENT_RIL_CONNECTED);
            mOemRil.registerForOemRilDisconnected(mHandler,
                    EVENT_RIL_DISCONNECTED);
        }
    }

    void saveMode() {
        Log.d(TAG, "saveMode()");

        DataWriter dr = new DataWriter();
        try {
            String osId;
            int osIdLen;
            {
            int osIdFlag = ValidParam;
            if (TextUtils.isEmpty(mUrspOsIdPref.getText())) {
                osIdFlag = InvalidParam;
                osId =InvalidStringParam;
                osIdLen = InvalidStringParam.length();
            }else{
                osId = mUrspOsIdPref.getText().toString();
                osIdLen =  osId.length();
            }
                dr.writeInt(osIdFlag);
                dr.writeBytes(osId.getBytes());
                dr.fillBytes(MAX_OS_ID_LEN - osIdLen);
            }

            String appId;
            int appIdLen;
            {
            int appIdFlag = ValidParam;
            if (TextUtils.isEmpty(mUrspAppIdPref.getText()) ) {
                appIdFlag = InvalidParam;
                appId= InvalidStringParam;
                appIdLen = InvalidStringParam.length();
            }else{
                appId = mUrspAppIdPref.getText().toString();
                appIdLen =appId.length();
            }
                dr.writeInt(appIdFlag);
                dr.writeBytes(appId.getBytes());
                dr.fillBytes(MAX_ADDR_STRING_LEN - appIdLen);
            }

            String fqdn;
            int fqdnLen;
            {

            int fqdnLenFlag = ValidParam;
            if (TextUtils.isEmpty(mFqdnPref.getText())) {
                fqdnLenFlag = InvalidParam;
                fqdn= InvalidStringParam;
                fqdnLen= InvalidStringParam.length();
            }else{
                fqdn = mFqdnPref.getText().toString();
                fqdnLen = fqdn.length();
            }
                dr.writeInt(fqdnLenFlag);
                dr.writeBytes(fqdn.getBytes());
                dr.fillBytes(MAX_ADDR_STRING_LEN - fqdnLen);
            }

            String ipv4Addr;
            try {
            int ipv4AddrFlag =ValidParam;
            if(TextUtils.isEmpty(mIpv4AddrPref.getText())) {
                ipv4AddrFlag =InvalidParam;
                ipv4Addr = InvalidIPv4Param;
            }else{
                ipv4Addr = mIpv4AddrPref.getText().toString();
            }
                byte[] ipv4AddrArray = InetAddress.getByName(ipv4Addr).getAddress();
                dr.writeInt(ipv4AddrFlag);
                dr.writeBytes(ipv4AddrArray);
            } catch(NetworkOnMainThreadException e) {
                Toast.makeText(getActivity(), "Invalid IPv4 Address. Try again.", Toast.LENGTH_SHORT).show();
                return;
            }

            String ipv4Mask;
            try {
            int ipv4MaskFlag =ValidParam;
            if(TextUtils.isEmpty( mIpv4MaskPref.getText())) {
                ipv4MaskFlag =InvalidParam;
                ipv4Mask = InvalidIPv4Param;
            }else{
                ipv4Mask = mIpv4MaskPref.getText().toString();
            }
                byte[] ipv4MaskArray = InetAddress.getByName(ipv4Mask).getAddress();
                dr.writeInt(ipv4MaskFlag);
                dr.writeBytes(ipv4MaskArray);
            } catch(NetworkOnMainThreadException e) {
                Toast.makeText(getActivity(), "Invalid IPv4 Mask Address. Try again.", Toast.LENGTH_SHORT).show();
                return;
            }

            String ipv6Addr ;
            try {
            int ipv6Flag =ValidParam;
            if(TextUtils.isEmpty(mIpv6AddrPref.getText())) {
                ipv6Flag =InvalidParam;
                ipv6Addr = InvalidIPv6Param;
            }else{
                ipv6Addr = mIpv6AddrPref.getText().toString();
            }
                byte[] ipv6AddrArray = InetAddress.getByName(ipv6Addr).getAddress();
                dr.writeInt(ipv6Flag);
                dr.writeBytes(ipv6AddrArray);
            } catch(NetworkOnMainThreadException e) {
                Toast.makeText(getActivity(), "Invalid IPv6 Address!!!! Try again.", Toast.LENGTH_SHORT).show();
                return;
            }

            int prefixLenFlag =InvalidParam;
            int prefixLen =InvalidParam;
            if(!TextUtils.isEmpty(mPrefixLenPref.getText())){
                prefixLenFlag = ValidParam;
                prefixLen = Integer.parseInt(mPrefixLenPref.getText().toString());
            }
            dr.writeInt(prefixLenFlag);
            dr.writeInt(prefixLen);

            int nextIpHdrFlag =InvalidParam;
            int nextIpHdr =InvalidParam;
            if(!TextUtils.isEmpty(mNextIpHdrPref.getText())){
                nextIpHdrFlag = ValidParam;
                nextIpHdr = Integer.parseInt(mNextIpHdrPref.getText().toString());
            }
            dr.writeInt(nextIpHdrFlag);
            dr.writeInt(nextIpHdr);

            int singlePortRangeFlag =InvalidParam;
            int singlePortRange =InvalidParam;
            if(!TextUtils.isEmpty(mSinglePortPref.getText())){
                singlePortRangeFlag = ValidParam;
                singlePortRange = Integer.parseInt(mSinglePortPref.getText().toString());
            }
            dr.writeInt(singlePortRangeFlag);
            dr.writeInt(singlePortRange);

            int portRangeLowLimitFlag =InvalidParam;
            int portRangeLowLimit =InvalidParam;
            if(!TextUtils.isEmpty(mPortRangeLowLimitPref.getText())){
                portRangeLowLimitFlag = ValidParam;
                portRangeLowLimit = Integer.parseInt(mPortRangeLowLimitPref.getText().toString());
            }
            dr.writeInt(portRangeLowLimitFlag);
            dr.writeInt(portRangeLowLimit);

            int portRangeHighLimitFlag =InvalidParam;
            int portRangeHighLimit =InvalidParam;
            if(!TextUtils.isEmpty(mPortRangeHighLimitPref.getText())){
                portRangeHighLimitFlag = ValidParam;
                portRangeHighLimit = Integer.parseInt(mPortRangeHighLimitPref.getText().toString());
            }
            dr.writeInt(portRangeHighLimitFlag);
            dr.writeInt(portRangeHighLimit);

            Log.d(TAG, "ursp setting(" + dr.toByteArray().length
            + "bytes) { prefixlenFlag=" + prefixLenFlag + ", nextiphdrFlag=" + nextIpHdrFlag
            + ", portrangelowlimitFlag=" + portRangeLowLimitFlag + ", portrangehighlimitFlag=" + portRangeHighLimitFlag +"}");

            Log.d(TAG, "ursp setting(" + dr.toByteArray().length
            + "bytes) {osid=" + osId + ", appid=" + appId
            + ", fqdn=" + fqdn + ", ipv4addr=" + ipv4Addr
            + ", ipv4mask=" + ipv4Mask + ", ipv6addr=" + ipv6Addr
            + ", prefixlen=" + prefixLen + ", nextiphdr=" + nextIpHdr
            + ", portrangelowlimit=" + portRangeLowLimit + ", portrangehighlimit=" + portRangeHighLimit +"}");
            Toast.makeText(getContext(), "Save", Toast.LENGTH_SHORT).show();
        }catch (IOException e) {
            Log.i(TAG, "SendData() IOException" + e);
        }

        mOemRil.invokeRequestRaw(RILC_REQ_OEM_SET_URSP, dr.toByteArray(), mHandler.obtainMessage(EVENT_SET_URSP_DONE));
    }

    private void setUrspTestMode(boolean mode) {
        String curMode = String.valueOf(SystemProperties.get(PROPERTY_URSP_TEST_MODE, "0"));
        if (curMode.equals("1") && mode ==false)
            SystemProperties.set(PROPERTY_URSP_TEST_MODE, "0");
        else if(curMode.equals("0") && mode ==true)
            SystemProperties.set(PROPERTY_URSP_TEST_MODE, "1");
    }

    public boolean onPreferenceChange(Preference preference, Object newValue) {
        preference.setSummary(newValue.toString());
        boolean newMode = Boolean.parseBoolean(newValue.toString());
        if(preference == mUrspTestModePref) {
            setUrspTestMode(newMode);
            Log.d(TAG, "onPreferenceChange:false");
        }
        // hide keyboard
        InputMethodManager immhide = (InputMethodManager) getActivity().getSystemService (Activity.INPUT_METHOD_SERVICE);
        immhide.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY, 0);
        return true;
    }
    public static UrspSettingPreferenceFragment newInstance(int phoneId) {
        Log.d(TAG, "phoneId: " + phoneId);
        UrspSettingPreferenceFragment frag = new UrspSettingPreferenceFragment();
        mPhoneId = phoneId;
        return frag;
    }
}
