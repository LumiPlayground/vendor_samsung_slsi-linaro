package com.shannon.qualifiednetworksservice;

import android.telephony.DataFailCause;
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
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.Spy;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ImsHandoverDecisionTest extends TestCase {
    private static MockedStatic<DeviceUtils> sMockedUtils;

    private ImsHandoverDecision mDecision;

    @Mock
    private HandoverConditions mockConditions;

    @Mock
    private ConfigurationManager mockConfiguration;

    @Mock
    private ConditionCellular mockCellularConditions;

    @Mock
    private ConditionWiFi mockWiFiCondition;

    @Mock
    private ConditionUserSetting mockUserConditions;

    @Mock
    private ConditionConnectivity mockTransportConditions;

    @Mock
    private ConditionMobile mockMobileConditions;

    @Mock
    private ConditionCall mockCallConditions;

    @Mock
    private ConditionProvisioning mockProvisioningConditions;

    @Mock
    private Transport mockTransport;

    @BeforeClass
    public static void beforeClass() {
        sMockedUtils = Mockito.mockStatic(DeviceUtils.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedUtils.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mockConditions.getConfigurationManager()).thenReturn(mockConfiguration);
        when(mockConditions.getCellularConditions()).thenReturn(mockCellularConditions);
        when(mockConditions.getWiFiConditions()).thenReturn(mockWiFiCondition);
        when(mockConditions.getUserConditions()).thenReturn(mockUserConditions);
        when(mockConditions.getTransportCondition()).thenReturn(mockTransportConditions);
        when(mockConditions.getMobileConditions()).thenReturn(mockMobileConditions);
        when(mockConditions.getCallConditions()).thenReturn(mockCallConditions);
        when(mockConditions.getProvisioningConditions()).thenReturn(mockProvisioningConditions);
        when(mockConditions.getSlotIndex()).thenReturn(0);
        mDecision = new ImsHandoverDecision(mockConditions);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetPreferredList() {
        setIsImsOnCell(false);
        setIsImsOnWiFi(false);
        setIsImsCallActive(false);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        when(mockWiFiCondition.isWiFiApHandoverActive()).thenReturn(false);
        when(DeviceUtils.isConnectionCallbackFixed()).thenReturn(true);
        enterAttachState(TelephonyManager.NETWORK_TYPE_LTE, 0);
        PreferredList list = mDecision.getPreferredList(
                HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        enterAttachState(TelephonyManager.NETWORK_TYPE_LTE, 10);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        when(mockCellularConditions.isAirplaneMode()).thenReturn(true);
        enterAttachState(TelephonyManager.NETWORK_TYPE_LTE, 10);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertTrue(list.isEmpty());

        when(mockCellularConditions.isAirplaneMode()).thenReturn(false);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        enterAttachState(TelephonyManager.NETWORK_TYPE_LTE, 5);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_TIMEOUT);
        assertTrue(list.isEmpty());

        setIsImsCallActive(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 10);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        when(mockCellularConditions.isRoaming()).thenReturn(false);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        setIsImsCallActive(false);
        setIsImsOnCell(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockCellularConditions.isOutOfService()).thenReturn(false);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_GSM);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        when(mockCellularConditions.isOutOfService()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        when(mockCellularConditions.isOutOfService()).thenReturn(false);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(false);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertTrue(list.isEmpty());

        setIsImsCallActive(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);

        setIsImsOnCell(true);
        setIsImsCallActive(false);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        when(mockCellularConditions.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        setIsImsOnWiFi(true);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        when(mockUserConditions.isWiFiPreferred()).thenReturn(false);
        when(mockCellularConditions.isRoveIn()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        when(DeviceUtils.isConnectionCallbackFixed()).thenReturn(false);
        when(mockTransportConditions.isHandoverFailed(ApnSetting.TYPE_IMS,
                Transport.TransportType.TRANSPORT_CELLULAR)).thenReturn(false);
        when(mockTransportConditions.isHandoverFailed(ApnSetting.TYPE_IMS,
                Transport.TransportType.TRANSPORT_WIFI)).thenReturn(false);

        setIsImsOnWiFi(false);
        when(mockConfiguration.isImsHandoverAllowedInCall(true)).thenReturn(true);
        when(mockConfiguration.isImsHandoverAllowedInCall(false)).thenReturn(true);
        when(mockConfiguration.isImsHandoverAllowed(true)).thenReturn(true);
        when(mockConfiguration.isImsHandoverAllowed(false)).thenReturn(true);
        when(mockConfiguration.getImsHandoverNotAllowedRatList()).thenReturn("");
        when(mockConfiguration.getImsHandoverAllowedRatList()).thenReturn("");
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());
        mDecision.setNewState(mDecision.mIdleState);

        when(mockTransportConditions.isHandoverFailed(ApnSetting.TYPE_IMS,
                Transport.TransportType.TRANSPORT_CELLULAR)).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());
        mDecision.setNewState(mDecision.mIdleState);

        setIsImsOnWiFi(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockCellularConditions.isAirplaneMode()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_AIRPLANE_MODE_CHANGED);
        assertTrue(list.isEmpty());

        setIsImsCallActive(true);
        mDecision.setNewState(mDecision.mIdleState);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        when(mockCellularConditions.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCellularConditions.isAllowedRat(TelephonyManager.NETWORK_TYPE_LTE, "LTE")).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());
        mDecision.setNewState(mDecision.mIdleState);

        setIsImsOnWiFi(false);
        setIsImsOnCell(true);
        setIsImsCallActive(false);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockCellularConditions.isAirplaneMode()).thenReturn(false);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        when(mockCellularConditions.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mockConfiguration.getImsHandoverNotAllowedRatList()).thenReturn("");
        when(mockConfiguration.getImsHandoverAllowedRatList()).thenReturn("");
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        when(mockWiFiCondition.isWiFiApHandoverActive()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());

        setIsImsOnWiFi(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_IMS)).
                thenReturn(TelephonyManager.NETWORK_TYPE_IWLAN);
        when(mockCellularConditions.isAirplaneMode()).thenReturn(true);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        when(mockCellularConditions.isCsRegistered()).thenReturn(true);
        list = mDecision.getPreferredList(HandoverConditions.ConditionEvents.EVENT_APN_IMS_CONNECTION_CHANGED);
        assertFalse(list.isEmpty());
    }

    @Test
    public void testGetPreferredNoCellular() {
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(false, TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(false, TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockWiFiCondition.isConnected()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mDecision.getPreferredNoCellular(true, TelephonyManager.NETWORK_TYPE_UNKNOWN));
    }

    @Test
    public void testGetPreferredNetwork() {
        ImsHandoverDecision mSpyDecision = spy(mDecision);
        setIsImsOnWiFi(false);
        setIsImsOnCell(false);
        // airplane mode on
        when(mockCellularConditions.isAirplaneMode()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockCellularConditions.isAirplaneMode()).thenReturn(false);

        // no cell
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        when(mockCellularConditions.isCsRegistered()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockCellularConditions.isRegistered()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        setIsImsOnCell(true);
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        when(mockCellularConditions.isCsRegistered()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // WFC off, cellular
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        when(mockCellularConditions.getNetworkType()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // WFC on, WiFi preferred, no WiFi
        setIsImsOnWiFi(true);
        when(mockUserConditions.isWiFiPreferred()).thenReturn(true);
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        setIsImsCallActive(false);
        when(mockWiFiCondition.isRoveIn()).thenReturn(true);
        when(mockWiFiCondition.isRoveOut()).thenReturn(false);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        when(mockCellularConditions.isRoveIn()).thenReturn(true);
        when(mockCellularConditions.isRoveOut()).thenReturn(false);
        when(mockConfiguration.isProfileDefaultNetworkSet()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // no cell
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // WiFi connected
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // WiFi not in
        when(mockWiFiCondition.isRoveIn()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // cell connected
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // no cell, no WiFi
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // cell connected
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // WiFi connected
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        when(mockWiFiCondition.isIpReachable()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // WiFi out, cell in
        when(mockWiFiCondition.isRoveOut()).thenReturn(true);
        when(mockCellularConditions.isRoveIn()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // WiFi out, cell not in
        when(mockCellularConditions.isRoveIn()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // RTP event
        setIsImsCallActive(true);
        when(mockWiFiCondition.isRoveOut()).thenReturn(false);
        when(mockWiFiCondition.isHandOut()).thenReturn(false);
        when(mockCellularConditions.isHandIn()).thenReturn(false);
        when(mockCellularConditions.isHandOut()).thenReturn(false);
        doReturn(true).when(mSpyDecision).isRtpEvent(any());
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // cellular preferred
        when(mockUserConditions.isWiFiPreferred()).thenReturn(false);
        setIsImsCallActive(false);
        when(mockCellularConditions.isRoveIn()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // VoLTE off, call on IMS
        setIsImsOnCell(false);
        setIsImsCallActive(true);
        when(mockCellularConditions.isHandIn()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // WiFi off
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // no call
        setIsImsCallActive(false);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // not WiFi
        when(mockCellularConditions.isRoveOut()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // no WiFi, not cell in
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        when(mockCellularConditions.isRoveIn()).thenReturn(false);
        when(mockCellularConditions.isRoveOut()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));
        when(mockCellularConditions.isRoveOut()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));
        setIsImsOnCell(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));

        // WiFi connected
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));
        setIsImsOnCell(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));
        when(mSpyDecision.isRtpEvent(any())).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_IWLAN));
        when(mockCellularConditions.isRoveOut()).thenReturn(true);
        when(mockWiFiCondition.isRoveIn()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));
        when(mockWiFiCondition.isRoveIn()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_IWLAN,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));
        when(mockWiFiCondition.isRoveIn()).thenReturn(false);
        setIsImsOnCell(true);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE,
                mSpyDecision.getPreferredNetwork(TelephonyManager.NETWORK_TYPE_UNKNOWN));
    }

    @Test
    public void testIsCellRoamedInternationally() {
        // not roaming
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        assertFalse(mDecision.isCellRoamedInternationally());

        // domestic
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        assertFalse(mDecision.isCellRoamedInternationally());

        // international
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        assertTrue(mDecision.isCellRoamedInternationally());

        // fake
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        assertFalse(mDecision.isCellRoamedInternationally());
    }

    @Test
    public void testIsImsOnCell() {
        // VoLTE off
        when(mockUserConditions.isVolteEnabled()).thenReturn(false);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnCell());

        // no transport
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(false);
        assertFalse(mDecision.isImsOnCell());

        // not provisioned
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(false);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(false);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(false);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnCell());

        // not roaming, no VoPS
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(false);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(false);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnCell());

        // roaming not supported
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(false);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnCell());

        // network not allowed
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(false);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnCell());

        // RAT not allowed
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(false);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnCell());

        // all true, roaming
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertTrue(mDecision.isImsOnCell());
    }

    @Test
    public void testIsImsOnWiFi() {
        // WFC off
        when(mockUserConditions.isWfcEnabled()).thenReturn(false);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(true);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(false);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockWiFiCondition.isRoaming()).thenReturn(false);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnWiFi());

        // no transport
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(true);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockWiFiCondition.isRoaming()).thenReturn(true);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(false);
        assertFalse(mDecision.isImsOnWiFi());

        // not provisioned
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(false);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockWiFiCondition.isRoaming()).thenReturn(true);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnWiFi());

        // not provisioned, OEM
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(true);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(false);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockWiFiCondition.isRoaming()).thenReturn(true);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnWiFi());

        // WFC roaming not supported
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(true);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(false);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockWiFiCondition.isRoaming()).thenReturn(false);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(false);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(true);
        assertFalse(mDecision.isImsOnWiFi());

        // all true
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(true);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(true);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(true);
        when(mockWiFiCondition.isRoaming()).thenReturn(true);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(true);
        assertTrue(mDecision.isImsOnWiFi());
    }

    @Test
    public void testIsServiceSupported() {
        // invalid network type
        assertFalse(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_UNKNOWN));

        // NETWORK_TYPE_IWLAN - supported
        setIsImsOnWiFi(true);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertTrue(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_IWLAN));

        // NETWORK_TYPE_IWLAN - WFC off
        setIsImsOnWiFi(false);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        assertFalse(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_IWLAN));

        // NETWORK_TYPE_IWLAN - WiFi off
        setIsImsOnWiFi(true);
        when(mockWiFiCondition.isConnected()).thenReturn(false);
        assertFalse(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_IWLAN));

        // NETWORK_TYPE_LTE - supported
        setIsImsOnCell(true);
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        assertTrue(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_LTE));

        // NETWORK_TYPE_LTE - not registered
        setIsImsOnCell(true);
        when(mockCellularConditions.isRegistered()).thenReturn(false);
        assertFalse(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_LTE));

        // NETWORK_TYPE_LTE - not allowed networks
        when(mockCellularConditions.isRegistered()).thenReturn(true);
        when(mockUserConditions.isVolteEnabled()).thenReturn(true);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(true);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(false);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
        assertFalse(mDecision.isServiceSupported(TelephonyManager.NETWORK_TYPE_LTE));
    }

    @Test
    public void testIsImsCallActive() {
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        assertTrue(mDecision.isImsCallActive());
    }

    @Test
    public void testGetHysteresisTimerToLte() {
        when(mockConfiguration.getImsHysteresisTimerToLte()).thenReturn(10);
        assertEquals(10, mDecision.getHysteresisTimerToLte());
    }

    @Test
    public void testGetHysteresisTimerToWiFi() {
        when(mockConfiguration.getImsHysteresisTimerToWiFi()).thenReturn(20);
        assertEquals(20, mDecision.getHysteresisTimerToWiFi());
    }

    @Test
    public void testIsWiFiIn() {
        when(mockWiFiCondition.isRoveIn()).thenReturn(true);
        when(mockWiFiCondition.isHandIn()).thenReturn(false);
        assertTrue(mDecision.isWiFiIn(false));
        assertFalse(mDecision.isWiFiIn(true));
    }

    @Test
    public void testIsWiFiOut() {
        when(mockWiFiCondition.isRoveOut()).thenReturn(false);
        when(mockWiFiCondition.isHandOut()).thenReturn(true);
        assertTrue(mDecision.isWiFiOut(true));
        assertFalse(mDecision.isWiFiOut(false));
    }

    @Test
    public void testIsCellIn() {
        when(mockCellularConditions.isRoveIn()).thenReturn(false);
        when(mockCellularConditions.isHandIn()).thenReturn(true);
        assertFalse(mDecision.isCellIn(false));
        assertTrue(mDecision.isCellIn(true));
    }

    @Test
    public void testIsCellOut() {
        when(mockCellularConditions.isRoveOut()).thenReturn(true);
        when(mockCellularConditions.isHandOut()).thenReturn(false);
        assertFalse(mDecision.isCellOut(true));
        assertTrue(mDecision.isCellOut(false));
    }

    @Test
    public void testProcessAttachState() {
        // EVENT_APN_CONNECTED
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_CONNECTED, timer task started
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 10);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mAttachState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_IMS_REGISTRATION_COMPLETED
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_REGISTRATION_COMPLETED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_DISCONNECTED, not matched transport
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mDecision.setEventInfo(mockTransport);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED);
        assertEquals(mDecision.mAttachState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_APN_DISCONNECTED, matched transport, error
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getLastFailCause()).thenReturn(DataFailCause.UNKNOWN);
        mDecision.setEventInfo(mockTransport);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_DISCONNECTED, matched transport, no error
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getLastFailCause()).thenReturn(DataFailCause.NONE);
        mDecision.setEventInfo(mockTransport);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_DISCONNECTED, matched transport, no error, timer task started
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 10);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTransport.getLastFailCause()).thenReturn(DataFailCause.NONE);
        mDecision.setEventInfo(mockTransport);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED);
        assertEquals(mDecision.mAttachState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_APN_FAILURE, not matched transport
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mDecision.setEventInfo(mockTransport);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_FAILURE);
        assertEquals(mDecision.mAttachState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_APN_FAILURE, matched transport
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mDecision.setEventInfo(mockTransport);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_FAILURE);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // default, idle
        enterAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN, 0);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_CELL_SIGNAL_STRENGTH_CHANGED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // default, keep attach
        enterAttachState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        setIsWiFiServiceSupported(true);
        mDecision.processAttachState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_TRANSPORT_AVAILABLE);
        assertEquals(mDecision.mAttachState, mDecision.mState);
    }

    @Test
    public void testProcessHandoverState() {
        // EVENT_WFC_MODE_CHANGED
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_WFC_MODE_CHANGED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_HYSTERESIS_TIMER_EXPIRED
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_HYSTERESIS_TIMER_EXPIRED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_DISCONNECTED, not matched transport
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_CELLULAR);
        mDecision.setEventInfo(mockTransport);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_APN_DISCONNECTED, matched transport
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mDecision.setEventInfo(mockTransport);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_DISCONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_IMS_REGISTRATION_COMPLETED, keep handover state
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mDecision.setEventInfo(mockTransport);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_REGISTRATION_COMPLETED);
        // because mImsRegistrationTimer is null
        assertEquals(mDecision.mHandoverState, mDecision.mState);

        // EVENT_IMS_REGISTRATION_TIMER_EXPIRED
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        mDecision.setEventInfo(mockTransport);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_REGISTRATION_TIMER_EXPIRED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_CONNECTED, wrong current
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_APN_CONNECTED, same current, no timer
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_APN_CONNECTED, same current, timer, call and no call
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 10);
        setIsImsCallActive(true);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(false);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.mBlockHandoverByHysteresisOnCall = false;
        when(mockCallConditions.isImsCallActive(false)).thenReturn(false);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_APN_CONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_IMS_CALL_STARTED, call and no call
        mDecision.mBlockHandoverByHysteresisOnCall = true;
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        setIsImsCallActive(true);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_STARTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(false);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_STARTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.mBlockHandoverByHysteresisOnCall = false;
        when(mockCallConditions.isImsCallActive(false)).thenReturn(false);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_STARTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(true);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_STARTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_WIFI_DISCONNECTED
        enterHandoverState(TelephonyManager.NETWORK_TYPE_LTE, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_WIFI_DISCONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_WIFI_DISCONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_IWLAN;
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_WIFI_DISCONNECTED);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_WIFI_DISCONNECTED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_IMS_CALL_RTP_EVENT
        enterHandoverState(TelephonyManager.NETWORK_TYPE_LTE, 0);
        setIsImsCallActive(false);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_IWLAN;
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_LTE;
        setIsImsCallActive(true);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.mLastPreferredNetwork = TelephonyManager.NETWORK_TYPE_IWLAN;
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RTP_EVENT);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_WFC_SETTINGS_BROADCAST, same current
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN,
                HandoverConditions.ConditionEvents.EVENT_WFC_SETTINGS_BROADCAST);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // EVENT_WFC_SETTINGS_BROADCAST, stop timer
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 10);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_WFC_SETTINGS_BROADCAST);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // EVENT_WFC_SETTINGS_BROADCAST, no timer
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_LTE,
                HandoverConditions.ConditionEvents.EVENT_WFC_SETTINGS_BROADCAST);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
        mDecision.setNewState(mDecision.mIdleState);

        // default, idle
        enterHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN, 0);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_CELL_SIGNAL_STRENGTH_CHANGED);
        assertEquals(mDecision.mIdleState, mDecision.mState);

        // default, keep handover
        enterHandoverState(TelephonyManager.NETWORK_TYPE_IWLAN, 0);
        setIsWiFiServiceSupported(true);
        mDecision.processHandoverState(TelephonyManager.NETWORK_TYPE_UNKNOWN,
                HandoverConditions.ConditionEvents.EVENT_APN_TRANSPORT_AVAILABLE);
        assertEquals(mDecision.mHandoverState, mDecision.mState);
    }

    @Test
    public void testStartImsRegistrationTimer() {
        // when
        when(mockConfiguration.getImsRegistrationTimer()).thenReturn(5);

        // then
        assertEquals(true, mDecision.startImsRegistrationTimer());

        // when
        mDecision.stopImsRegistrationTimer();
    }

    @Test
    public void testControlCellReports() {
        // when
        // EVENT_WIFI_DISCONNECTED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_WIFI_DISCONNECTED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_CARRIER_CONFIG_CHANGED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_CARRIER_CONFIG_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_CONFIGURATION_CHANGED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_CONFIGURATION_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_DEFAULT_LINK_PROPERTIES_CHANGED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_DEFAULT_LINK_PROPERTIES_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_WIFI_CONNECTED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_WIFI_CONNECTED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_WIFI_CAPABILITIES_CHANGED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_WIFI_CAPABILITIES_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_PHONE_SERVICE_STATE_CHANGED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_PHONE_SERVICE_STATE_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_IMS_CALL_STARTED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_STARTED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_IMS_CALL_RELEASED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_IMS_CALL_RELEASED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_WFC_SETTINGS_CHANGED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_WFC_SETTINGS_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_WFC_SETTINGS_CHANGED:
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_WFC_MODE_CHANGED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_WFC_MODE_CHANGED:
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_APN_HANDOVER_COMPLETED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );

        // EVENT_HYSTERESIS_TIMER_EXPIRED
        mDecision.controlCellReports(
                HandoverConditions.ConditionEvents.EVENT_HYSTERESIS_TIMER_EXPIRED,
                TelephonyManager.NETWORK_TYPE_LTE,true
        );
    }

    @Test
    public void testRequestOrStopCellReports() {
        // when
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockUserConditions.isWiFiPreferred()).thenReturn(true);
        when(mockWiFiCondition.isRoveOut()).thenReturn(true);
        when(mockWiFiCondition.isHandOut()).thenReturn(true);

        // then
        mDecision.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE, true);
        mDecision.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_UNKNOWN, true);

        // when
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockUserConditions.isWiFiPreferred()).thenReturn(false);

        // then
        mDecision.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE, true);
        mDecision.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_UNKNOWN, true);

        // when
        when(mockCallConditions.isImsCallActive(anyBoolean())).thenReturn(false);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
        when(mockUserConditions.isWfcEnabled()).thenReturn(true);
        when(mockUserConditions.isWiFiPreferred()).thenReturn(true);
        when(mockWiFiCondition.isRoveOut()).thenReturn(false);
        when(mockWiFiCondition.isHandOut()).thenReturn(false);

        // then
        mDecision.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_LTE, true);
        mDecision.requestOrStopCellReports(TelephonyManager.NETWORK_TYPE_UNKNOWN, true);
    }

    private void enterAttachState(int networkType, int timeout) {
        when(mockConfiguration.getInitialAttachTimeout()).thenReturn(timeout);
        mDecision.mLastPreferredNetwork = networkType;
        mDecision.setNewState(mDecision.mAttachState);
    }

    private void enterHandoverState(int networkType, int timeout) {
        when(mockConfiguration.getImsHysteresisTimerToLte()).thenReturn(timeout);
        when(mockConfiguration.getImsHysteresisTimerToWiFi()).thenReturn(timeout);
        mDecision.mLastPreferredNetwork = networkType;
        mDecision.setNewState(mDecision.mHandoverState);
    }

    private void setIsWiFiServiceSupported(boolean isEnabled) {
        setIsImsOnWiFi(isEnabled);
        when(mockWiFiCondition.isConnected()).thenReturn(true);
    }

    private void setIsImsOnCell(boolean isEnabled) {
        when(mockUserConditions.isVolteEnabled()).thenReturn(isEnabled);
        when(mockProvisioningConditions.isVoLteProvisioned()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsRequired()).thenReturn(true);
        when(mockConfiguration.isVoLteVoPsNotRequiredActiveCall()).thenReturn(true);
        setIsImsCallActive(false);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockConfiguration.isVoLteRoamingSupported()).thenReturn(true);
        when(mockCellularConditions.isAllowedNetwork("*")).thenReturn(true);
        when(mockConfiguration.getAllowedRoamingNetworks()).thenReturn("*");
        when(mockConfiguration.isVoLteVoPsRequiredRoaming()).thenReturn(true);
        when(mockConfiguration.getAllowedDomesticRoamingNetworks()).thenReturn("*");
        when(mockCellularConditions.isAllowedRat("LTE")).thenReturn(true);
        when(mockConfiguration.getVoLteAllowedRat()).thenReturn("LTE");
        when(mockCellularConditions.isVoiceSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_CELLULAR)).
                thenReturn(true);
    }

    private void setIsImsOnWiFi(boolean isEnabled) {
        when(mockUserConditions.isWfcEnabled()).thenReturn(isEnabled);
        when(mockProvisioningConditions.isWfcProvisioned()).thenReturn(true);
        when(mockConfiguration.isWfcOemProvisioningRequired()).thenReturn(false);
        when(mockProvisioningConditions.isOemProvisioned()).thenReturn(true);
        when(mockCellularConditions.isRoaming()).thenReturn(false);
        when(mockCellularConditions.isRoamedInternationally()).thenReturn(false);
        when(mockWiFiCondition.isRoaming()).thenReturn(false);
        when(mockConfiguration.isWfcRoamingSupported()).thenReturn(true);
        when(mockTransportConditions.isTransportAvailable(ApnSetting.TYPE_IMS, Transport.TransportType.TRANSPORT_WIFI)).
                thenReturn(true);
    }

    private void setIsImsCallActive(boolean active) {
        when(mockTransportConditions.getNetworkType(ApnSetting.TYPE_EMERGENCY)).
                thenReturn(TelephonyManager.NETWORK_TYPE_UNKNOWN);
        when(mockCallConditions.isImsCallActive(false)).thenReturn(active);
    }
}