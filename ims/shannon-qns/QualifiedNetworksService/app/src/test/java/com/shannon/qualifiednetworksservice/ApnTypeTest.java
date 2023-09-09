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

import android.net.NetworkCapabilities;
import android.telephony.data.ApnSetting;

import com.android.internal.telephony.PhoneConstants;
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

import static com.android.internal.telephony.PhoneConstants.APN_TYPE_IMS;
import static com.shannon.qualifiednetworksservice.ApnType.APN_CBS;
import static com.shannon.qualifiednetworksservice.ApnType.APN_DEFAULT;
import static com.shannon.qualifiednetworksservice.ApnType.APN_DUN;
import static com.shannon.qualifiednetworksservice.ApnType.APN_EMERGENCY;
import static com.shannon.qualifiednetworksservice.ApnType.APN_FOTA;
import static com.shannon.qualifiednetworksservice.ApnType.APN_HIPRI;
import static com.shannon.qualifiednetworksservice.ApnType.APN_IA;
import static com.shannon.qualifiednetworksservice.ApnType.APN_IMS;
import static com.shannon.qualifiednetworksservice.ApnType.APN_MCX;
import static com.shannon.qualifiednetworksservice.ApnType.APN_MMS;
import static com.shannon.qualifiednetworksservice.ApnType.APN_SUPL;
import static com.shannon.qualifiednetworksservice.ApnType.APN_XCAP;
import static com.shannon.qualifiednetworksservice.ApnType.getBroadcastType;
import static com.shannon.qualifiednetworksservice.ApnType.getEnum;
import static org.mockito.Mockito.mock;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ApnTypeTest extends TestCase {
    @Mock private ShannonQualifiedNetworksService mShannonQualifiedNetworksService = mock(ShannonQualifiedNetworksService.class);

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        int type = ApnSetting.TYPE_IMS;
        int capability = NetworkCapabilities.NET_CAPABILITY_IMS;
        String name = PhoneConstants.APN_TYPE_IMS;
        boolean supported = true;
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testApnType() {
        final int incorrectApnType = 3;
        assertEquals(getBroadcastType(ApnSetting.TYPE_IMS), APN_TYPE_IMS);
        assertFalse(getBroadcastType(incorrectApnType) == APN_TYPE_IMS);
        assertEquals(getEnum(ApnSetting.TYPE_IMS),  APN_IMS);
        assertFalse(getEnum(incorrectApnType) == APN_IMS);
    }

    @Test
    public void testGetType() {
        assertEquals(ApnSetting.TYPE_DEFAULT, APN_DEFAULT.getType());
        assertEquals(ApnSetting.TYPE_MMS, APN_MMS.getType());
        assertEquals(ApnSetting.TYPE_SUPL, APN_SUPL.getType());
        assertEquals(ApnSetting.TYPE_DUN, APN_DUN.getType());
        assertEquals(ApnSetting.TYPE_HIPRI, APN_HIPRI.getType());
        assertEquals(ApnSetting.TYPE_FOTA, APN_FOTA.getType());
        assertEquals(ApnSetting.TYPE_IMS, APN_IMS.getType());
        assertEquals(ApnSetting.TYPE_CBS, APN_CBS.getType());
        assertEquals(ApnSetting.TYPE_IA, APN_IA.getType());
        assertEquals(ApnSetting.TYPE_EMERGENCY, APN_EMERGENCY.getType());
        assertEquals(ApnSetting.TYPE_MCX, APN_MCX.getType());
        assertEquals(ApnSetting.TYPE_XCAP, APN_XCAP.getType());
    }

    @Test
    public void testGetCapability() {
        assertEquals(NetworkCapabilities.NET_CAPABILITY_INTERNET, APN_DEFAULT.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_MMS, APN_MMS.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_SUPL, APN_SUPL.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_DUN, APN_DUN.getCapability());
        assertEquals(-1, APN_HIPRI.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_FOTA, APN_FOTA.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_IMS, APN_IMS.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_CBS, APN_CBS.getCapability());
        assertEquals(-1, APN_IA.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_EIMS, APN_EMERGENCY.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_MCX, APN_MCX.getCapability());
        assertEquals(NetworkCapabilities.NET_CAPABILITY_XCAP, APN_XCAP.getCapability());
    }

    @Test
    public void testIsSupported() {
        assertEquals(false, APN_DEFAULT.isSupported());
        assertEquals(true, APN_MMS.isSupported());
        assertEquals(false, APN_SUPL.isSupported());
        assertEquals(false, APN_DUN.isSupported());
        assertEquals(false, APN_HIPRI.isSupported());
        assertEquals(false, APN_FOTA.isSupported());
        assertEquals(true, APN_IMS.isSupported());
        assertEquals(true, APN_CBS.isSupported());
        assertEquals(false, APN_IA.isSupported());
        assertEquals(true, APN_EMERGENCY.isSupported());
        assertEquals(false, APN_MCX.isSupported());
        assertEquals(true, APN_XCAP.isSupported());
    }

    @Test
    public void testGetModemType() {
        int MODEM_TYPE_DEFAULT = 0;
        int MODEM_TYPE_EMERGENCY = 1;
        int MODEM_TYPE_IMS = 2;

        assertEquals(MODEM_TYPE_IMS, APN_IMS.getModemType());
        assertEquals(MODEM_TYPE_EMERGENCY, APN_EMERGENCY.getModemType());
        assertEquals(MODEM_TYPE_DEFAULT, APN_XCAP.getModemType());
    }

    @Test
    public void testGetApnBitmask() {
        Assert.assertEquals(2754, ApnType.getApnBitmask());
    }

    @Test
    public void testGetApnBitmaskWithoutImsAndEmergency() {
        assertEquals(2178, ApnType.getApnBitmaskWithoutImsAndEmergency());
    }

    @Test
    public void testGetSupportedApnType() {
        assertEquals(64/* ims */, ApnType.getSupportedApnType(64));
        assertEquals(0 /* None */, ApnType.getSupportedApnType(1));
    }

    @Test
    public void testGetApnBitmaskForCall() {
        assertEquals(576, ApnType.getApnBitmaskForCall());
    }

    @Test
    public void testGetCapabilityNameOf() {
        assertEquals("IMS", ApnType.getCapabilityNameOf(APN_IMS.getType()));
    }
}
