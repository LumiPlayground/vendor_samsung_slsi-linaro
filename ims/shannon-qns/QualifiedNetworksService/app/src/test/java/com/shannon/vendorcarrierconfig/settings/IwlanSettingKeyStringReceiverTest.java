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

package com.shannon.vendorcarrierconfig.settings;

import android.content.Context;
import android.content.Intent;

import android.view.LayoutInflater;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IwlanSettingKeyStringReceiverTest extends TestCase {
    private IwlanSettingKeyStringReceiver mIwlanSettingKeyStringReceiver;

    @BeforeClass
    public static void beforeClass() {
    }

    @AfterClass
    public static void afterClass() {
    }

    private static MockedStatic<LayoutInflater> sLayoutInflater;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        mIwlanSettingKeyStringReceiver = new IwlanSettingKeyStringReceiver();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnReceive() {
        //Given
        Context context = mock(Context.class);
        Intent intent = mock(Intent.class);
        when(intent.getAction()).thenReturn("android.provider.Telephony.SECRET_CODE");

        //When
        mIwlanSettingKeyStringReceiver.onReceive(context, intent);

        //Then
        verify(context, times(1)).startActivity(any());
    }
}


