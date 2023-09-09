/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class LocationManagerTest extends TestCase {
    private LocationManager locationManager;
    private LocationManager.CountryListenerCallback countryListenerCallback;
    @Mock
    Context mockContext;

    @Mock
    CountryDetector mockCountryService;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mockCountryService = mock(CountryDetector.class);
        when(mockContext.getSystemService((Class<Object>) any())).thenReturn(mockCountryService);
        locationManager = new LocationManager(mockContext);
    }

    @After
    public void tearDown() throws Exception {
        locationManager.destroy();
    }

    @Test
    public void testUpdateLocationOnNetworkConnected() {
        //Given
        String countryName = "ABCD";
        Country country = mock(Country.class);
        when(mockCountryService.detectCountry()).thenReturn(country);
        when(country.getCountryIso()).thenReturn(countryName);

        //When
        locationManager.updateLocationOnNetworkConnected();

        //Then
        assertEquals(countryName, locationManager.getCountryCode());
    }

    @Test
    public void testGetInstance() {
        //Given

        //When

        //Then
        assertNotNull(locationManager.getInstance());
    }

    @Test
    public void testOnCountryDetected() {
        //Given
        String countryName = "ABCD";
        countryListenerCallback = locationManager.new CountryListenerCallback();
        Country country = mock(Country.class);
        when(country.getCountryIso()).thenReturn(countryName);

        //When
        countryListenerCallback.onCountryDetected(country);

        //Then
        assertEquals(countryName, locationManager.getCountryCode());
    }
}
