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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.AccessNetworkConstants;
import android.telephony.CarrierConfigManager;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

import com.shannon.qualifiednetworksservice.util.DeviceUtils;
import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import com.shannon.qualifiednetworksservice.ConfigurationManager.DisallowedHandoverRule;

import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.BROADCAST_ACTION_VENDOR_CONFIG_CHANGED;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.BROADCAST_EXTRA_VENDOR_CONFIG;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_EPDG_CONNECTION_RETRY_PARAMETERS;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_VOLTE_ROAMING_ENABLED;
import static com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig.KEY_VOMOBILE_ENABLED;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import androidx.collection.ArraySet;
import androidx.test.platform.app.InstrumentationRegistry;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConfigurationManagerTest extends TestCase {
    @BeforeClass
    public static void beforeClass() {
        sSimManager = mockStatic(SimManager.class);
        sDeviceUtils = mockStatic(DeviceUtils.class);
        sCarrierType = mockStatic(CarrierType.class);
    }

    @AfterClass
    public static void afterClass() {
        sSimManager.close();
        sDeviceUtils.close();
        sCarrierType.close();
        sSimManager = null;
        sDeviceUtils = null;
    }

    private ConfigurationManager mConfigurationManager;
    private static MockedStatic<SimManager> sSimManager;
    private static MockedStatic<DeviceUtils> sDeviceUtils;
    private static MockedStatic<CarrierType> sCarrierType;

    private Context mContext = InstrumentationRegistry.getInstrumentation().getContext();

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        int VENDOR_CONFIG_UPDATED = 3;

        SimManager mockSimManager = mock(SimManager.class);

        sSimManager.when(() -> SimManager.getInstance()).thenReturn(mockSimManager);

        sDeviceUtils.when(() -> DeviceUtils.isConnectionCallbackFixed()).thenReturn(true);

        int slotId = 0;
        Handler handler = mock(Handler.class);
        Message message = Message.obtain(handler, VENDOR_CONFIG_UPDATED);
        mConfigurationManager = new ConfigurationManager(slotId, mContext, message);
    }

    @After
    public void tearDown() throws Exception {
        mConfigurationManager.destroy();
    }

    @Test
    public void testPutInt() {
        //Given
        int param = -1;
        //When
        mConfigurationManager.putInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, param);

        //Then
        assertEquals(param, mConfigurationManager.getInt(KEY_IMS_HANDOVER_HYSTERESIS_TIMER_VOLTE, -1));
    }

    @Test
    public void testPutBoolean() {
        //Given
        boolean param = false;

        //When
        mConfigurationManager.putBoolean(KEY_VOLTE_ROAMING_ENABLED, param);
        mConfigurationManager.putBooleanPersisted(KEY_VOMOBILE_ENABLED, param);

        //Then
        assertEquals(param, mConfigurationManager.getBoolean(KEY_VOLTE_ROAMING_ENABLED, false));
        assertEquals(param, mConfigurationManager.getBoolean(KEY_VOMOBILE_ENABLED, false));
    }

    @Test
    public void testIsVoLteVoPsRequired() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isVoLteVoPsRequired();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoLteVoPsNotRequiredActiveCall() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isVoLteVoPsNotRequiredActiveCall();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetVoLteAllowedRat() {
        //Given
        String param = "LTE,LTE_CA,NR";

        //When
        String retVal = mConfigurationManager.getVoLteAllowedRat();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoLteRoamingSupported() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isVoLteRoamingSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoLteVoPsRequiredRoaming() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isVoLteVoPsRequiredRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsWfcOemProvisioningRequired() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isWfcOemProvisioningRequired();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsWfcRoamingSupported() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isWfcRoamingSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsImsHandoverAllowed() {
        //Given
        boolean param = true;
        boolean roaming = true;

        //When
        boolean retVal = mConfigurationManager.isImsHandoverAllowed(roaming);

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsImsHandoverAllowedInCall() {
        //Given
        boolean param = true;
        boolean roaming = true;

        //When
        boolean retVal = mConfigurationManager.isImsHandoverAllowedInCall(roaming);

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandoverAllowedRatList() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandoverAllowedRatList();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandoverNotAllowedRatList() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandoverNotAllowedRatList();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHysteresisTimerToLte() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getImsHysteresisTimerToLte();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHysteresisTimerToWiFi() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getImsHysteresisTimerToWiFi();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testCanIgnoreCallDowngrade() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.canIgnoreCallDowngrade();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testCanIgnoreCallUpgrade() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.canIgnoreCallUpgrade();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetRtpThreshold() {
        //Given
        int param = 40;

        //When
        int retVal = mConfigurationManager.getRtpThreshold();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetRtpInterval() {
        //Given
        int param = 5;

        //When
        int retVal = mConfigurationManager.getRtpInterval();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetRtpJitter() {
        //Given
        int param = 120;

        //When
        int retVal = mConfigurationManager.getRtpJitter();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetNoRtpInterval() {
        //Given
        int param = 2;

        //When
        int retVal = mConfigurationManager.getNoRtpInterval();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetWiFiApHandoverTimeout() {
        //Given
        int param = 10;

        //When
        int retVal = mConfigurationManager.getWiFiApHandoverTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsWiFiValidationRequired() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isWiFiValidationRequired();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testCanUseWiFiIPv6() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.canUseWiFiIPv6();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsWiFiApHandoverSupported() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isWiFiApHandoverSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetPingDestination() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getPingDestination();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetPingPeriod() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getPingPeriod();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetPingInterval() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getPingInterval();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetPingCount() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getPingCount();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetPingPacketSize() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getPingPacketSize();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetPingMaxRtt() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getPingMaxRtt();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetDefaultImsHandoverIn() {
        //Given
        String param = "cdma{rssi=-100};geran{rssi=-100};utran{rscp=-105};eutran{rsrp=-115};iwlan{rssi=-75}";

        //When
        String retVal = mConfigurationManager.getDefaultImsHandoverIn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveIn() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveIn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveInRoaming() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveInRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveInHysteresisOn() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveInHysteresisOn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveInWiFiPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveInWiFiPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveInCellPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveInCellPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandIn() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandIn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandInRoaming() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandInRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandInHysteresisOn() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandInHysteresisOn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandInWiFiPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandInWiFiPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandInCellPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandInCellPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetDefaultImsHandoverOut() {
        //Given
        String param = "cdma{rssi=-105};geran{rssi=-105};utran{rscp=-115};eutran{rsrp=-120};iwlan{rssi=-80}";

        //When
        String retVal = mConfigurationManager.getDefaultImsHandoverOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveOut() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveOutHysteresisOn() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveOutHysteresisOn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveOutWiFiPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveOutWiFiPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsRoveOutCellPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsRoveOutCellPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandOut() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandOutHysteresisOn() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandOutHysteresisOn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandOutWiFiPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandOutWiFiPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsHandOutCellPreferred() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsHandOutCellPreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetImsCellMid() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getImsCellMid();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellularNetworkInterfaceName() {
        //Given
        String param = "rmnet";

        //When
        String retVal = mConfigurationManager.getCellularNetworkInterfaceName();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetIwlanNetworkInterfaceName() {
        //Given
        String param = "epdg";

        //When
        String retVal = mConfigurationManager.getIwlanNetworkInterfaceName();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetEmcHandoverAllowedRatList() {
        //Given
        String param = "LTE,LTE_CA,NR,IWLAN";

        //When
        String retVal = mConfigurationManager.getEmcHandoverAllowedRatList();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetWiFiEmcRoveIn() {
        //Given
        int param = -65;

        //When
        int retVal = mConfigurationManager.getWiFiEmcRoveIn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetWiFiEmcRoveOut() {
        //Given
        int param = -80;

        //When
        int retVal = mConfigurationManager.getWiFiEmcRoveOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellEmcRoveIn() {
        //Given
        int param = -100;

        //When
        int retVal = mConfigurationManager.getCellEmcRoveIn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellEmcRoveOut() {
        //Given
        int param = -112;

        //When
        int retVal = mConfigurationManager.getCellEmcRoveOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellEmcRsrpMid() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getCellEmcRsrpMid();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsCbsOverWiFiSupported() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isCbsOverWiFiSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsMmsOverWiFiSupported() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isMmsOverWiFiSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsWiFiPreferredForMms() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isWiFiPreferredForMms();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsXcapOverWiFiSupported() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isXcapOverWiFiSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsWiFiPreferredForXcap() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isWiFiPreferredForXcap();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsXcapHandoverAllowed() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isXcapHandoverAllowed();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsXcapImsPreconditionForWiFi() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isXcapImsPreconditionForWiFi();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetXcapAllowedRatList() {
        //Given
        String param = "GSM,GPRS,EDGE,UMTS,HSDPA,HSUPA,HSPA,HSPA+,LTE,LTE_CA,NR";

        //When
        String retVal = mConfigurationManager.getXcapAllowedRatList();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetHysteresisTimerToLte() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getHysteresisTimerToLte();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetHysteresisTimerToWiFi() {
        //Given
        int param = 0;

        //When
        int retVal = mConfigurationManager.getHysteresisTimerToWiFi();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoMobileEnabled() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isVoMobileEnabled();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoMobileEnabledOnRoamingNetwork() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isVoMobileEnabledOnRoamingNetwork();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetVoMobileAllowedRatList() {
        //Given
        String param = "LTE,LTE_CA,NR";

        //When
        String retVal = mConfigurationManager.getVoMobileAllowedRatList();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetVoMobileOtherSignalIn() {
        //Given
        int param = -110;

        //When
        int retVal = mConfigurationManager.getVoMobileOtherSignalIn();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetVoMobileOtherSignalOut() {
        //Given
        int param = -116;

        //When
        int retVal = mConfigurationManager.getVoMobileOtherSignalOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetVoMobileSignalOut() {
        //Given
        int param = -125;

        //When
        int retVal = mConfigurationManager.getVoMobileSignalOut();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoMobileRoamingOverrideSignalStrength() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isVoMobileRoamingOverrideSignalStrength();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoMobileIgnoreCsState() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isVoMobileIgnoreCsState();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoMobilePreferred() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isVoMobilePreferred();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetVoMobileForbiddenNetworksInRoaming() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getVoMobileForbiddenNetworksInRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsVoMobileInterStackHandoverSupported() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isVoMobileInterStackHandoverSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellularPermanentErrorRetryCount() {
        //Given
        int param = 3;

        //When
        int retVal = mConfigurationManager.getCellularPermanentErrorRetryCount();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellularPermanentErrorRetryTimeout() {
        //Given
        int param = 720;

        //When
        int retVal = mConfigurationManager.getCellularPermanentErrorRetryTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetCellularPermanentErrorHandoverRetryTimeout() {
        //Given
        int param = 120;

        //When
        int retVal = mConfigurationManager.getCellularPermanentErrorHandoverRetryTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetIwlanPermanentErrorRetryCount() {
        //Given
        int param = 3;

        //When
        int retVal = mConfigurationManager.getIwlanPermanentErrorRetryCount();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetIwlanPermanentErrorRetryTimeout() {
        //Given
        int param = 720;

        //When
        int retVal = mConfigurationManager.getIwlanPermanentErrorRetryTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetIwlanPermanentErrorHandoverRetryTimeout() {
        //Given
        int param = 120;

        //When
        int retVal = mConfigurationManager.getIwlanPermanentErrorHandoverRetryTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetInitialNonPermanentErrorRetryCount() {
        //Given
        int param = 3;

        //When
        int retVal = mConfigurationManager.getInitialNonPermanentErrorRetryCount();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetInitialNonPermanentErrorRetryTimeout() {
        //Given
        int param = 30;

        //When
        int retVal = mConfigurationManager.getInitialNonPermanentErrorRetryTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetHandoverNonPermanentErrorRetryCount() {
        //Given
        int param = 10;

        //When
        int retVal = mConfigurationManager.getHandoverNonPermanentErrorRetryCount();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetHandoverNonPermanentErrorRetryTimeout() {
        //Given
        int param = 30;

        //When
        int retVal = mConfigurationManager.getHandoverNonPermanentErrorRetryTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetInitialAttachTimeout() {
        //Given
        int param = 5;

        //When
        int retVal = mConfigurationManager.getInitialAttachTimeout();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsProfileDefaultNetworkSet() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isProfileDefaultNetworkSet();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetAllowedRoamingNetworks() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getAllowedRoamingNetworks();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetAllowedDomesticRoamingNetworks() {
        //Given
        String param = "";

        //When
        String retVal = mConfigurationManager.getAllowedDomesticRoamingNetworks();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testCanUseWfcRoamingSettings() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.canUseWfcRoamingSettings();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetUnknownLocationAsRoaming() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.getUnknownLocationAsRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testGetRetryParameters() {
        //Given
        String param = null;

        //When
        String retVal = mConfigurationManager.getRetryParameters();

        //Then
        assertEquals(param, retVal);

        Bundle bundle = new Bundle();
        bundle.putString(KEY_EPDG_CONNECTION_RETRY_PARAMETERS, "retry");

        mConfigurationManager.setBundle(bundle);
        assertEquals("retry", mConfigurationManager.getRetryParameters());
    }

    @Test
    public void testGetMmsAllowedRatList() {
        //Given
        String param = "GSM,GPRS,EDGE,UMTS,HSDPA,HSUPA,HSPA,HSPA+,LTE,LTE_CA,NR";

        //When
        String retVal = mConfigurationManager.getMmsAllowedRatList();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsEmcOverWiFiSupported() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isEmcOverWiFiSupported();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsEmcOverWiFiSupportedRoaming() {
        //Given
        boolean param = true;

        //When
        boolean retVal = mConfigurationManager.isEmcOverWiFiSupportedRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsEmcOemProvisioningRequired() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isEmcOemProvisioningRequired();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsEmcHandoverAllowed() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isEmcHandoverAllowed();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testIsEmcHandoverAllowedAtRoaming() {
        //Given
        boolean param = false;

        //When
        boolean retVal = mConfigurationManager.isEmcHandoverAllowedAtRoaming();

        //Then
        assertEquals(param, retVal);
    }

    @Test
    public void testHandleVendorConfigChanged() {
        //Given
        ConfigurationManager.ConfigurationReceiverCallback configurationReceiverCallback;
        Intent intent = mock(Intent.class);
        when(intent.getAction()).thenReturn(BROADCAST_ACTION_VENDOR_CONFIG_CHANGED);

        // original
        Bundle bundle = new Bundle();
        bundle.putInt("test_int", 1);
        mConfigurationManager.setBundle(bundle);
        // update
        bundle = new Bundle();
        bundle.putInt("test_int", 5);
        when(intent.getBundleExtra(BROADCAST_EXTRA_VENDOR_CONFIG)).thenReturn(bundle);

        configurationReceiverCallback = mConfigurationManager.new ConfigurationReceiverCallback();
        assertNotNull(configurationReceiverCallback);

        //When
        configurationReceiverCallback.onReceive(intent);

        //Then
        verify(intent, times(1)).getIntExtra(any(), anyInt());
    }

    @Test
    public void testGetInt() {
        Bundle bundle = new Bundle();
        bundle.putInt("test_int", 1);

        mConfigurationManager.setBundle(bundle);
        assertEquals(1, mConfigurationManager.getInt("test_int", 0));
    }

    @Test
    public void testGetBoolean() {
        Bundle bundle = new Bundle();
        bundle.putBoolean("test_bool", true);

        mConfigurationManager.setBundle(bundle);
        assertTrue(mConfigurationManager.getBoolean("test_bool", false));
    }

    @Test
    public void testGetString() {
        Bundle bundle = new Bundle();
        bundle.putString("test_string", "true");

        mConfigurationManager.setBundle(bundle);
        assertEquals("true", mConfigurationManager.getString("test_string", "false"));
    }

    @Test
    public void testPutBooleanPersisted() {
        mConfigurationManager.putBooleanPersisted("test_key", false);
    }

    @Test
    public void testGetImsRegistrationTimer() {
        assertEquals(5, mConfigurationManager.getImsRegistrationTimer());
    }

    @Test
    public void testIsHandoverNotAllowed() {
        String currentAccessNetwork = AccessNetworkConstants.AccessNetworkType
                .toString(AccessNetworkConstants.AccessNetworkType.EUTRAN);
        String preferredAccessNetwork = AccessNetworkConstants.AccessNetworkType
                .toString(AccessNetworkConstants.AccessNetworkType.IWLAN);

        assertFalse(mConfigurationManager.isHandoverNotAllowed(ApnType.getCapabilityNameOf(ApnType.APN_IMS.getType()),
                AccessNetworkConstants.AccessNetworkType.EUTRAN,
                AccessNetworkConstants.AccessNetworkType.IWLAN, false));

        List<DisallowedHandoverRule> list = new ArrayList<>();

        List<String> source = new ArrayList<>();
        List<String> target = new ArrayList<>();
        List<String> capabilities = new ArrayList<>();

        DisallowedHandoverRule rule = new DisallowedHandoverRule(source, target, capabilities, true);
        list.add(rule);

        source.add(currentAccessNetwork);
        target.add(preferredAccessNetwork);
        rule = new DisallowedHandoverRule(source, target, capabilities, false);
        list.add(rule);

        mConfigurationManager.setHandoverRulesList(list);

        assertTrue(mConfigurationManager.isHandoverNotAllowed(ApnType.getCapabilityNameOf(
                        ApnType.APN_IMS.getType()),
                AccessNetworkConstants.AccessNetworkType.EUTRAN,
                AccessNetworkConstants.AccessNetworkType.IWLAN,
                false)
        );
    }

    @Test
    public void testCarrierConfigurationBroadcastReceiver() {
        BroadcastReceiver receiver = mConfigurationManager.getCarrierConfigBroadcastReceiver();

        Context context = mock(Context.class);
        Intent intent = mock(Intent.class);

        when(intent.getAction()).thenReturn(null);
        receiver.onReceive(context, intent);

        when(intent.getAction()).thenReturn(CarrierConfigManager.ACTION_CARRIER_CONFIG_CHANGED);
        when(intent.getIntExtra(CarrierConfigManager.EXTRA_SLOT_INDEX, -1)).thenReturn(5);
        receiver.onReceive(context, intent);
    }
}
