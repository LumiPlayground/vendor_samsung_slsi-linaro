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
import com.shannon.qualifiednetworksservice.Transport.IVendorTransportHandler;
import java.util.Timer;
import junit.framework.TestCase;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static android.telephony.TelephonyManager.NETWORK_TYPE_UNKNOWN;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportCellularTest extends TestCase {
    @Mock
    Context mockContext;
    @Mock
    Timer mockTimer;
    @Mock
    ConfigurationManager mockConfigurationManager;
    @Mock
    HandoverConditions.IConditionHelper mockConditionHelper;
    @Mock
    TransportState.IStateChanged mockCallback;
    @Mock
    IVendorTransportHandler mockVendorHandler;
    private TransportCellular mTransportCellular;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mTransportCellular = new TransportCellular(mockContext, 1,
                mockConfigurationManager, mockConditionHelper, mockCallback, mockVendorHandler);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetNetworkType() {
        //Then
        assertEquals(NETWORK_TYPE_UNKNOWN, mTransportCellular.getNetworkType());
    }

    @Test
    public void testisRoaming() {
        assertFalse(mTransportCellular.isRoaming());
    }

    @Test
    public void testonConnected() {
        // When
        mTransportCellular.onConnected();

        // Then
        verify(mockConditionHelper, times(1)).getCellularNetworkType();
    }

    @Test
    public void testonHandoverCompleted() {
        // When
        mTransportCellular.onHandoverCompleted();

        // Then
        verify(mockConditionHelper, times(1)).getCellularNetworkType();
    }

    @Test
    public void testonRatChanged() {
        // When
        mTransportCellular.onRatChanged();

        // Then
        verify(mockConditionHelper, times(1)).getCellularNetworkType();
    }

    @Test
    public void testonOperatorChanged() {
        // When
        mTransportCellular.onOperatorChanged();

        // Then
        verify(mockConditionHelper, times(1)).isRoaming();
    }
}
