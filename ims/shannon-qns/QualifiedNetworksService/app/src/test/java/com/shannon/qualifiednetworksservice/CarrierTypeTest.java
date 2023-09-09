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

import android.telephony.TelephonyManager;
import junit.framework.TestCase;
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

import static com.shannon.qualifiednetworksservice.CarrierType.CARRIER_TYPE_ATT;
import static com.shannon.qualifiednetworksservice.CarrierType.CARRIER_TYPE_EE;
import static com.shannon.qualifiednetworksservice.CarrierType.CARRIER_TYPE_TMO;
import static org.mockito.Mockito.when;

@FixMethodOrder(MethodSorters.NAME_ASCENDING)
@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class CarrierTypeTest extends TestCase {
    @Mock
    private TelephonyManager mockTelephonyManager;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    @Before
    public void setUp() {
        MockitoAnnotations.openMocks(this);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void getEnumTest() {
        // Given
        when(mockTelephonyManager.getSimOperator()).thenReturn("310332");
        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        // Then
        assertNull(CarrierType.getEnum(mockTelephonyManager));

        // Given
        // findById
        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(1949);
        // Then
        assertEquals(CARRIER_TYPE_TMO, CarrierType.getEnum(mockTelephonyManager));

        // Given
        // findByOperator
        when(mockTelephonyManager.getSimOperator()).thenReturn("25851");
        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getSimCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getCarrierIdFromSimMccMnc()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        // Then
        assertEquals(CARRIER_TYPE_ATT, CarrierType.getEnum(mockTelephonyManager));

        // Given
        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.CDMA_ROAMING_MODE_ANY);
        // Then
        assertEquals(CARRIER_TYPE_EE, CarrierType.getEnum(mockTelephonyManager));
    }
}