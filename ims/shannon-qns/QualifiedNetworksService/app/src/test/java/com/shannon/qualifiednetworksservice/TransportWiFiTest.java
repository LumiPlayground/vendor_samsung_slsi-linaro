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

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.Timer;

import static android.telephony.TelephonyManager.NETWORK_TYPE_IWLAN;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportWiFiTest extends TestCase {
    private TransportWiFi mTransportWifi;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        Context context = mock(Context.class);
        int subId = 0;
        ConfigurationManager configurationManager = mock(ConfigurationManager.class);
        TransportState.IStateChanged callback = mock(TransportState.IStateChanged.class);
        IVendorTransportHandler vendorHandler = mock(IVendorTransportHandler.class);

        mTransportWifi = new TransportWiFi(context, subId, configurationManager, callback, vendorHandler);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetNetworkType() {
        //Given

        //When

        //Then
        assertEquals(NETWORK_TYPE_IWLAN, mTransportWifi.getNetworkType());
    }
}
