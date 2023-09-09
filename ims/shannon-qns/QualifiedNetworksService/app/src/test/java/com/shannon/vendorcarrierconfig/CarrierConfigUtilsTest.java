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

package com.shannon.vendorcarrierconfig;

import android.os.Bundle;
import android.os.PersistableBundle;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.MethodSorters;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static com.shannon.vendorcarrierconfig.CarrierConfigUtils.*;
import static org.junit.Assert.assertNotNull;
import static org.mockito.ArgumentMatchers.anyString;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class CarrierConfigUtilsTest {
    private CarrierConfigUtils mCarrierConfigUtils, spyCarrierConfigUtils;

    private PersistableBundle mBundle;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);

        mBundle = new PersistableBundle();
        mBundle.putString(KEY_SUPPORTED_RAT_LIST, "2G,3G,NR,LTE,NR,LTE,WIFI,NR,WIFI");
        int[] cellIntArray = {-105, -3, 10};
        mBundle.putIntArray(KEY_LTE_ROVE_IN_IDLE_CELLPREF_INT_ARRAY, cellIntArray);
        int[] nrIntArray = {-100, -5, 5};
        mBundle.putIntArray(KEY_NR_ROVE_IN_IDLE_CELLPREF_INT_ARRAY, nrIntArray);
        mBundle.putInt(KEY_WIFI_RSSI_ROVE_IN_IDLE_CELLPREF_INT, -70);
        int[] noVopsOnRoam = {1};
        mBundle.putIntArray(KEY_IMS_PDN_ENABLED_IN_NO_VOPS_SUPPORT_INT_ARRAY, noVopsOnRoam);

        mCarrierConfigUtils = new CarrierConfigUtils(mBundle);
        spyCarrierConfigUtils = spy(mCarrierConfigUtils);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void convertToLocalTest() {
        // Given
        when(spyCarrierConfigUtils.getKey(KEY_SUPPORTED_RAT_LIST)).thenReturn(KEY_SUPPORTED_RAT_LIST);
        when(spyCarrierConfigUtils.getKey(KEY_LTE_ROVE_IN_IDLE_CELLPREF_INT_ARRAY)).
                thenReturn(KEY_LTE_ROVE_IN_IDLE_CELLPREF_INT_ARRAY);
        when(spyCarrierConfigUtils.getKey(KEY_NR_ROVE_IN_IDLE_CELLPREF_INT_ARRAY)).
                thenReturn(KEY_NR_ROVE_IN_IDLE_CELLPREF_INT_ARRAY);
        when(spyCarrierConfigUtils.getKey(KEY_WIFI_RSSI_ROVE_IN_IDLE_CELLPREF_INT)).
                thenReturn(KEY_WIFI_RSSI_ROVE_IN_IDLE_CELLPREF_INT);
        when(spyCarrierConfigUtils.getKey(KEY_IMS_PDN_ENABLED_IN_NO_VOPS_SUPPORT_INT_ARRAY)).
                thenReturn(KEY_IMS_PDN_ENABLED_IN_NO_VOPS_SUPPORT_INT_ARRAY);

        // When
        Bundle testBundle = spyCarrierConfigUtils.convertToLocal();

        // Then
        assertNotNull(testBundle);
    }
}
