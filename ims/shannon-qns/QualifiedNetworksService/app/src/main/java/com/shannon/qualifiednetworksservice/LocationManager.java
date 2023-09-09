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
package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.location.Country;
import android.location.CountryDetector;
import android.location.CountryListener;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.qualifiednetworksservice.util.LogUtils;

public class LocationManager {
    private static LocationManager sMe = null;
    public static LocationManager getInstance() { return sMe; }

    protected class CountryListenerCallback implements CountryListener {
        public void onCountryDetected(Country country) {
            LogUtils.v(TAG, "Country changed to " + country);
            updateCountry(country);
        }
    }

    private final String TAG;
    private final CountryDetector mCountryService;
    private final CountryListenerCallback mCountryCallback;
    private String mCountry;

    public LocationManager(Context context) {
        mCountryService = context.getSystemService(CountryDetector.class);
        mCountryCallback = new CountryListenerCallback();
        mCountry = null;

        TAG = ShannonQualifiedNetworksService.getServiceTAG("-Location");

        sMe = this;

        try {
            mCountryService.addCountryListener(mCountryCallback, null);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to register listener: " + e);
        }
        try {
            Country country = mCountryService.detectCountry();
            LogUtils.v(TAG, "Current " + country);
            updateCountry(country);
        } catch (Exception e) {
            LogUtils.e(TAG, "Failed to get country: " + e);
        }
    }

    public void destroy() {
        mCountryService.removeCountryListener(mCountryCallback);
    }

    public String getCountryCode() { return mCountry; }

    public void updateLocationOnNetworkConnected() {
        Country country = mCountryService.detectCountry();
        LogUtils.v(TAG, "updateLocationOnNetworkConnected " + country);
        updateCountry(country);
    }

    private void updateCountry(Country country) {
        if (country == null) {
            return;
        }
        /* not interested in SIM and Locale countries */
        if (country.getSource() < Country.COUNTRY_SOURCE_SIM) {
            mCountry = country.getCountryIso();
            LogUtils.i(TAG, "Country set to " + mCountry);
        }
    }
}
