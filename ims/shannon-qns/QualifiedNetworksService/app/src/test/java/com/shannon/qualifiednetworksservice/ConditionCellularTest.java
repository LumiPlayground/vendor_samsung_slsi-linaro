package com.shannon.qualifiednetworksservice;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.telephony.AccessNetworkConstants;
import android.telephony.CellSignalStrength;
import android.telephony.CellSignalStrengthCdma;
import android.telephony.CellSignalStrengthGsm;
import android.telephony.CellSignalStrengthLte;
import android.telephony.CellSignalStrengthNr;
import android.telephony.CellSignalStrengthTdscdma;
import android.telephony.CellSignalStrengthWcdma;
import android.telephony.DataSpecificRegistrationInfo;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;
import android.telephony.VopsSupportInfo;
import android.telephony.ims.ImsMmTelManager;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.ArrayList;
import java.util.List;

import static android.content.Intent.ACTION_AIRPLANE_MODE_CHANGED;
import static com.shannon.qualifiednetworksservice.RilOemInterface.RIL_UNSOLICITED_MESSAGE;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class ConditionCellularTest extends TestCase {
    private ConditionCellular mConditions;

    @Mock
    Context mockContext;

    @Mock
    Intent mockIntent;

    @Mock
    RilOemInterface mockRil;

    @Mock
    ConfigurationManager mockConfiguration;

    @Mock
    HandoverConditions.IConditionChanged mockCallback;

    @Mock
    SimManager mockSimManager;

    @Mock
    TelephonyManager mockTelephonyManager;

    @Mock
    ServiceState mockServiceState;

    @Mock
    NetworkRegistrationInfo mockPsState;

    @Mock
    NetworkRegistrationInfo mockCsState;

    @Mock
    SignalStrength mockSignalStrength;

    @Mock
    CellSignalStrength mockStrength;

    @Mock
    CellSignalStrengthCdma mockStrengthCdma;

    @Mock
    CellSignalStrengthTdscdma mockStrengthTdscdma;

    @Mock
    CellSignalStrengthGsm mockStrengthGsm;

    @Mock
    CellSignalStrengthWcdma mockStrengthWcdma;

    @Mock
    CellSignalStrengthLte mockStrengthLte;

    @Mock
    CellSignalStrengthNr mockStrengthNr;

    @Mock
    DataSpecificRegistrationInfo mockDataRegistrationInfo;

    @Mock
    VopsSupportInfo mockVopsSupportInfo;

    private static MockedStatic<SimManager> mockStaticSimManager;

    private final String IN_STR = "cdma{rssi=-100};geran{rssi=-100};utran{rscp=-105};eutran{rsrp=-115};ngran{rsrp=-115};iwlan{rssi=-75}";
    private final String OUT_STR = "cdma{rssi=-105};geran{rssi=-105};utran{rscp=-115};eutran{rsrp=-120};ngran{rsrp=-120};iwlan{rssi=-80}";

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        mockStaticSimManager = Mockito.mockStatic(SimManager.class);
        when(SimManager.getInstance()).thenReturn(mockSimManager);
        when(mockSimManager.getTelephonyManager(0)).thenReturn(mockTelephonyManager);
        when(mockSimManager.getSubId(0)).thenReturn(1);
        when(mockConfiguration.getDefaultImsHandoverIn()).thenReturn(IN_STR);
        when(mockConfiguration.getImsRoveIn()).thenReturn(IN_STR);
        when(mockConfiguration.getImsHandIn()).thenReturn(IN_STR);
        when(mockConfiguration.getImsRoveInWiFiPreferred()).thenReturn(IN_STR);
        when(mockConfiguration.getImsRoveInCellPreferred()).thenReturn(IN_STR);
        when(mockConfiguration.getImsHandInWiFiPreferred()).thenReturn(IN_STR);
        when(mockConfiguration.getImsHandInCellPreferred()).thenReturn(IN_STR);
        when(mockConfiguration.getImsRoveInHysteresisOn()).thenReturn(IN_STR);
        when(mockConfiguration.getImsHandInHysteresisOn()).thenReturn(IN_STR);
        when(mockConfiguration.getDefaultImsHandoverOut()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsRoveOut()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsHandOut()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsRoveOutWiFiPreferred()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsRoveOutCellPreferred()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsHandOutWiFiPreferred()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsHandOutCellPreferred()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsRoveOutHysteresisOn()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsHandOutHysteresisOn()).thenReturn(OUT_STR);
        when(mockConfiguration.getImsCellMid()).thenReturn("eutran{rsrp=-115,rsrq=-18};ngran{rsrp=-115,rsrq=-18}");

        ArrayList<CellSignalStrength> list = new ArrayList<>();
        list.add(mockStrengthCdma);
        when(mockStrengthCdma.isValid()).thenReturn(false);
        list.add(mockStrengthTdscdma);
        when(mockStrengthTdscdma.isValid()).thenReturn(false);
        list.add(mockStrengthGsm);
        when(mockStrengthGsm.isValid()).thenReturn(true);
        list.add(mockStrengthWcdma);
        when(mockStrengthWcdma.isValid()).thenReturn(true);
        list.add(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(true);
        list.add(mockStrengthNr);
        when(mockStrengthNr.isValid()).thenReturn(true);
        when(mockTelephonyManager.getSignalStrength()).thenReturn(mockSignalStrength);
        when(mockSignalStrength.getCellSignalStrengths()).thenReturn(list);

        when(mockTelephonyManager.getServiceState()).thenReturn(mockServiceState);
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_PS,
                AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(null);
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_CS,
                AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(null);
        ConditionCellular badConditions = new ConditionCellular(0, mockContext, mockRil, mockConfiguration, mockCallback);

        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_PS,
                AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(mockPsState);
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_CS,
                AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(mockCsState);
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        mConditions = new ConditionCellular(0, mockContext, mockRil, mockConfiguration, mockCallback);
    }

    @After
    public void tearDown() throws Exception {
        mockStaticSimManager.close();
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testDestroy() {
        mConditions.destroy();
        assertTrue(true);
    }

    @Test
    public void testCellBroadCastReceiver() {
        ConditionCellular.CellBroadcastReceiver cellBroadcastReceiver =
                mConditions.getBroadcastReceiver();

        cellBroadcastReceiver.onReceive(mockContext, mockIntent);

        when(mockIntent.getAction()).thenReturn(ACTION_AIRPLANE_MODE_CHANGED);
        cellBroadcastReceiver.onReceive(mockContext, mockIntent);
    }

    @Test
    public void testCellularStateListener() {
        ConditionCellular.CellularStateListener cellularStateListener =
                mConditions.getStateListener();

        cellularStateListener.onServiceStateChanged(mockServiceState);
    }

    @Test
    public void testCellularStateListener1() {
        ConditionCellular.CellularStateListener cellularStateListener =
                mConditions.getStateListener();

        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_HOME);
        when(mockCsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_HOME);

        cellularStateListener.onServiceStateChanged(mockServiceState);
    }

    @Test
    public void testCellularStateListener2() {
        ConditionCellular.CellularStateListener cellularStateListener =
                mConditions.getStateListener();

        NetworkRegistrationInfo newPsState = mock(NetworkRegistrationInfo.class);
        NetworkRegistrationInfo newCsState = mock(NetworkRegistrationInfo.class);

        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_PS,
                AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(newPsState);
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_CS,
                AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(newCsState);

        cellularStateListener.onServiceStateChanged(mockServiceState);
    }

    @Test
    public void testOnSignalStrengthsChanged() {
        ConditionCellular.CellularStateListener cellularStateListener =
                mConditions.getStateListener();
        List<CellSignalStrength> cellSignalStrengths = new ArrayList<>();
        cellSignalStrengths.add(mockStrength);
        mConditions.setSignalStrength(mockStrength);

        when(mockSignalStrength.getCellSignalStrengths()).thenReturn(cellSignalStrengths);
        when(mockStrength.isValid()).thenReturn(true);

        cellularStateListener.onSignalStrengthsChanged(mockSignalStrength);
    }

    @Test
    public void testOnSignalStrengthsChanged1() {
        ConditionCellular.CellularStateListener cellularStateListener =
                mConditions.getStateListener();
        List<CellSignalStrength> cellSignalStrengths = new ArrayList<>();

        when(mockSignalStrength.getCellSignalStrengths()).thenReturn(cellSignalStrengths);

        cellularStateListener.onSignalStrengthsChanged(mockSignalStrength);
    }

    @Test
    public void testOnSignalStrengthsChanged2() {
        ConditionCellular.CellularStateListener cellularStateListener =
                mConditions.getStateListener();
        CellSignalStrength testCellSignalStrength = mock(CellSignalStrength.class);
        List<CellSignalStrength> cellSignalStrengths = new ArrayList<>();
        cellSignalStrengths.add(testCellSignalStrength);
        mConditions.setSignalStrength(mockStrength);

        when(mockSignalStrength.getCellSignalStrengths()).thenReturn(cellSignalStrengths);

        cellularStateListener.onSignalStrengthsChanged(mockSignalStrength);
    }

    @Test
    public void testIsConnected() {
        when(mockPsState.isRegistered()).thenReturn(false);
        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_UNKNOWN);
        assertFalse(mConditions.isConnected());

        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_DENIED);
        assertTrue(mConditions.isConnected());

        when(mockPsState.isRegistered()).thenReturn(true);
        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_UNKNOWN);
        assertTrue(mConditions.isConnected());

        when(mockPsState.isRegistered()).thenReturn(true);
        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_DENIED);
        assertTrue(mConditions.isConnected());
    }

    @Test
    public void testIsOutOfService() {
        when(mockPsState.isRegistered()).thenReturn(false);
        assertTrue(mConditions.isOutOfService());

        when(mockPsState.isRegistered()).thenReturn(true);
        assertFalse(mConditions.isOutOfService());
    }

    @Test
    public void testIsRegistered() {
        when(mockPsState.isRegistered()).thenReturn(true);
        assertTrue(mConditions.isRegistered());
    }

    @Test
    public void testGetNetworkType() {
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        assertEquals(TelephonyManager.NETWORK_TYPE_LTE, mConditions.getNetworkType());
    }

    @Test
    public void testIsAirplaneMode() {
        assertFalse(mConditions.isAirplaneMode());
    }

    @Test
    public void testIsDisconnectedFromNetwork() {
        assertFalse(mConditions.isDisconnectedFromNetwork());
    }

    @Test
    public void testSetDisconnectedFromNetwork() {
        mConditions.setDisconnectedFromNetwork(true);
        assertTrue(mConditions.isDisconnectedFromNetwork());
    }

    @Test
    public void testIsNotLTE() {
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_GSM);
        assertTrue(mConditions.isNotLTE());

        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        assertFalse(mConditions.isNotLTE());
    }

    @Test
    public void testIsRoaming() {
        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_ROAMING);
        assertTrue(mConditions.isRoaming());

        when(mockPsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_HOME);
        assertFalse(mConditions.isRoaming());
    }

    @Test
    public void testIsRoamedInternationally() {
        when(mockPsState.getRoamingType()).thenReturn(ServiceState.ROAMING_TYPE_NOT_ROAMING);
        assertFalse(mConditions.isRoamedInternationally());

        when(mockPsState.getRoamingType()).thenReturn(ServiceState.ROAMING_TYPE_INTERNATIONAL);
        assertTrue(mConditions.isRoamedInternationally());
    }

    @Test
    public void testIsLTE() {
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        assertTrue(mConditions.isLTE());
    }

    @Test
    public void testIsVoiceSupported() {
        when(mockPsState.getDataSpecificInfo()).thenReturn(null);
        assertFalse(mConditions.isVoiceSupported());

        when(mockPsState.getDataSpecificInfo()).thenReturn(mockDataRegistrationInfo);
        when(mockDataRegistrationInfo.getVopsSupportInfo()).thenReturn(null);
        assertFalse(mConditions.isVoiceSupported());

        when(mockPsState.getDataSpecificInfo()).thenReturn(mockDataRegistrationInfo);
        when(mockDataRegistrationInfo.getVopsSupportInfo()).thenReturn(mockVopsSupportInfo);
        when(mockVopsSupportInfo.isVopsSupported()).thenReturn(false);
        assertFalse(mConditions.isVoiceSupported());

        when(mockPsState.getDataSpecificInfo()).thenReturn(mockDataRegistrationInfo);
        when(mockDataRegistrationInfo.getVopsSupportInfo()).thenReturn(mockVopsSupportInfo);
        when(mockVopsSupportInfo.isVopsSupported()).thenReturn(true);
        assertTrue(mConditions.isVoiceSupported());
    }

    @Test
    public void testIsVoiceBarred() {
        Handler handler = mConditions.getHandler();
        Message message = Message.obtain();
        byte[] mRawData = new byte[]{
                (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* number of info : 1 */
                (byte) 0x06, (byte) 0x00, (byte) 0x00, (byte) 0x00, /* SERVICE_TYPE_MMTEL_VOICE */
                (byte) 0x02, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00,
                (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x00};

        RilUnsolicitedMessage rilUnsolicitedMessage =
                new RilUnsolicitedMessage(RilUnsolicitedMessage.RIL_UNSOL_BARRING, mRawData);
        message.what = RIL_UNSOLICITED_MESSAGE;
        message.obj = rilUnsolicitedMessage;

        handler.handleMessage(message);
        assertTrue(mConditions.isVoiceBarred());
    }

    @Test
    public void testIsEmergencySupported() {
        when(mockPsState.getDataSpecificInfo()).thenReturn(null);
        assertFalse(mConditions.isEmergencySupported());

        when(mockPsState.getDataSpecificInfo()).thenReturn(mockDataRegistrationInfo);
        when(mockDataRegistrationInfo.getVopsSupportInfo()).thenReturn(null);
        assertFalse(mConditions.isEmergencySupported());

        when(mockPsState.getDataSpecificInfo()).thenReturn(mockDataRegistrationInfo);
        when(mockDataRegistrationInfo.getVopsSupportInfo()).thenReturn(mockVopsSupportInfo);
        when(mockVopsSupportInfo.isEmergencyServiceSupported()).thenReturn(false);
        assertFalse(mConditions.isEmergencySupported());

        when(mockPsState.getDataSpecificInfo()).thenReturn(mockDataRegistrationInfo);
        when(mockDataRegistrationInfo.getVopsSupportInfo()).thenReturn(mockVopsSupportInfo);
        when(mockVopsSupportInfo.isEmergencyServiceSupported()).thenReturn(true);
        assertTrue(mConditions.isEmergencySupported());
    }

    @Test
    public void testIsAllowedRat() {
        when(mockPsState.getAccessNetworkTechnology()).thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        assertFalse(mConditions.isAllowedRat("GSM"));
    }

    @Test
    public void testTestIsAllowedRat() {
        assertTrue(mConditions.isAllowedRat(TelephonyManager.NETWORK_TYPE_LTE, "LTE"));
    }

    @Test
    public void testIsAllowedNetwork() {
        assertTrue(mConditions.isAllowedNetwork(""));

        when(mockPsState.getRegisteredPlmn()).thenReturn("");
        assertFalse(mConditions.isAllowedNetwork("310"));

        when(mockPsState.getRegisteredPlmn()).thenReturn("311480");
        assertFalse(mConditions.isAllowedNetwork("3"));
        assertFalse(mConditions.isAllowedNetwork("310"));
        assertFalse(mConditions.isAllowedNetwork("310260"));
        assertTrue(mConditions.isAllowedNetwork("311"));
        assertTrue(mConditions.isAllowedNetwork("311480"));
    }

    @Test
    public void testIsRoveIn() {
        mConditions.setSignalStrength(mockStrength);
        when(mockStrength.isValid()).thenReturn(true);
        assertFalse(mConditions.isRoveIn());

        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(false);
        assertFalse(mConditions.isRoveIn());

        when(mockStrengthLte.isValid()).thenReturn(true);
        when(mockStrengthLte.getRsrp()).thenReturn(-120);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertFalse(mConditions.isRoveIn());

        when(mockStrengthLte.getRsrp()).thenReturn(-90);
        assertTrue(mConditions.isRoveIn());
    }

    @Test
    public void testTestIsRoveIn() {
        mConditions.setSignalStrength(mockStrengthNr);
        when(mockStrengthNr.getDbm()).thenReturn(-90);
        when(mockStrengthNr.getSsSinr()).thenReturn(1);
        when(mockStrengthNr.getSsRsrq()).thenReturn(-3);
        assertTrue(mConditions.isRoveIn(false));

        mConditions.setSignalStrength(mockStrengthWcdma);
        when(mockStrengthWcdma.getDbm()).thenReturn(-90);
        when(mockStrengthWcdma.getEcNo()).thenReturn(1);
        assertTrue(mConditions.isRoveIn(true));
    }

    @Test
    public void testTestIsRoveIn1() {
        mConditions.setSignalStrength(mockStrengthGsm);
        when(mockStrengthGsm.getDbm()).thenReturn(-90);
        assertTrue(mConditions.isRoveIn(ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED));

        mConditions.setSignalStrength(mockStrengthTdscdma);
        when(mockStrengthTdscdma.isValid()).thenReturn(true);
        when(mockStrengthTdscdma.getDbm()).thenReturn(-90);
        assertTrue(mConditions.isRoveIn(ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED));
    }

    @Test
    public void testIsHandIn() {
        mConditions.setSignalStrength(mockStrength);
        when(mockStrength.isValid()).thenReturn(true);
        assertFalse(mConditions.isHandIn());

        mConditions.setSignalStrength(mockStrengthCdma);
        assertFalse(mConditions.isHandIn());

        when(mockStrengthCdma.isValid()).thenReturn(true);
        when(mockStrengthCdma.getDbm()).thenReturn(-120);
        assertFalse(mConditions.isHandIn());

        when(mockStrengthCdma.getDbm()).thenReturn(-90);
        assertTrue(mConditions.isHandIn());
    }

    @Test
    public void testTestIsHandIn() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-90);
        assertTrue(mConditions.isHandIn(false));
        assertTrue(mConditions.isHandIn(true));
    }

    @Test
    public void testTestIsHandIn1() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-90);
        assertTrue(mConditions.isHandIn(ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED));
        assertTrue(mConditions.isHandIn(ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED));
    }

    @Test
    public void testIsRoveOut() {
        mConditions.setSignalStrength(mockStrength);
        when(mockStrength.isValid()).thenReturn(true);
        assertTrue(mConditions.isRoveOut());

        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(false);
        assertTrue(mConditions.isRoveOut());

        when(mockStrengthLte.isValid()).thenReturn(true);
        when(mockStrengthLte.getRsrp()).thenReturn(-100);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertFalse(mConditions.isRoveOut());

        when(mockStrengthLte.getRsrp()).thenReturn(-125);
        assertTrue(mConditions.isRoveOut());
    }

    @Test
    public void testTestIsRoveOut() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-125);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertTrue(mConditions.isRoveOut(true));
        assertTrue(mConditions.isRoveOut(false));
    }

    @Test
    public void testTestIsRoveOut1() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-125);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertTrue(mConditions.isRoveOut(ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED));
        assertTrue(mConditions.isRoveOut(ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED));
    }

    @Test
    public void testIsHandOut() {
        mConditions.setSignalStrength(mockStrength);
        when(mockStrength.isValid()).thenReturn(true);
        assertTrue(mConditions.isHandOut());

        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(false);
        assertTrue(mConditions.isHandOut());

        when(mockStrengthLte.isValid()).thenReturn(true);
        when(mockStrengthLte.getRsrp()).thenReturn(-100);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertFalse(mConditions.isHandOut());

        when(mockStrengthLte.getRsrp()).thenReturn(-125);
        assertTrue(mConditions.isHandOut());
    }

    @Test
    public void testTestIsHandOut() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-125);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertTrue(mConditions.isHandOut(true));
        assertTrue(mConditions.isHandOut(false));
    }

    @Test
    public void testTestIsHandOut1() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-125);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertTrue(mConditions.isHandOut(ImsMmTelManager.WIFI_MODE_CELLULAR_PREFERRED));
        assertTrue(mConditions.isHandOut(ImsMmTelManager.WIFI_MODE_WIFI_PREFERRED));
    }

    @Test
    public void testIsRoveOutMid() {
        mConditions.setSignalStrength(mockStrength);
        when(mockStrength.isValid()).thenReturn(false);
        assertTrue(mConditions.isRoveOutMid());

        when(mockStrength.isValid()).thenReturn(true);
        assertFalse(mConditions.isRoveOutMid());

        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.getRsrp()).thenReturn(-100);
        when(mockStrengthLte.getRssnr()).thenReturn(1);
        when(mockStrengthLte.getRsrq()).thenReturn(-3);
        assertFalse(mConditions.isRoveOutMid());

        mConditions.setSignalStrength(mockStrengthNr);
        when(mockStrengthNr.getDbm()).thenReturn(-120);
        when(mockStrengthNr.getSsSinr()).thenReturn(1);
        when(mockStrengthNr.getSsRsrq()).thenReturn(-3);
        assertTrue(mConditions.isRoveOutMid());
    }

    @Test
    public void testGetRsrp() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(false);
        assertEquals(-140, mConditions.getRsrp());

        when(mockStrengthLte.isValid()).thenReturn(true);
        when(mockStrengthLte.getDbm()).thenReturn(-100);
        assertEquals(-100, mConditions.getRsrp());
    }

    @Test
    public void testIsLowerThanThreshold() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(false);
        assertTrue(mConditions.isLowerThanThreshold(-120));

        when(mockStrengthLte.isValid()).thenReturn(true);
        when(mockStrengthLte.getDbm()).thenReturn(-100);
        assertFalse(mConditions.isLowerThanThreshold(-120));

        when(mockStrengthLte.getDbm()).thenReturn(-130);
        assertTrue(mConditions.isLowerThanThreshold(-120));
    }

    @Test
    public void isBetterThanThreshold() {
        mConditions.setSignalStrength(mockStrengthLte);
        when(mockStrengthLte.isValid()).thenReturn(false);
        assertFalse(mConditions.isBetterThanThreshold(-115));

        when(mockStrengthLte.isValid()).thenReturn(true);
        when(mockStrengthLte.getDbm()).thenReturn(-100);
        assertTrue(mConditions.isBetterThanThreshold(-115));

        when(mockStrengthLte.getDbm()).thenReturn(-130);
        assertFalse(mConditions.isBetterThanThreshold(-120));
    }

    @Test
    public void testStopCellReports() {
        mConditions.stopCellReports();
        assertTrue(true);

        mConditions.requestCellReports(false, false, false, false, false);
        mConditions.stopCellReports();
        assertTrue(true);
    }

    @Test
    public void testRequestCellReports() {
        mConditions.requestCellReports(false, false, false, false, false);
        mConditions.requestCellReports(false, false, true, false, false);
        mConditions.requestCellReports(false, true, false, false, false);
        mConditions.requestCellReports(false, true, true, false, false);
        mConditions.requestCellReports(true, false, false, false, false);
        mConditions.requestCellReports(true, false, true, false, false);
        mConditions.requestCellReports(true, true, false, false, false);
        mConditions.requestCellReports(true, true, true, false, false);
    }

    @Test
    public void testIsCsRegistered() {
        when(mockCsState.isInService()).thenReturn(true);
        assertTrue(mConditions.isCsRegistered());
    }

    @Test
    public void testIsCsRegistered_exception() {
        when(mockCsState.isInService()).thenThrow(new NullPointerException());
        assertFalse(mConditions.isCsRegistered());
    }

    @Test
    public void testIsCsRoaming() {
        when(mockCsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_ROAMING);
        assertTrue(mConditions.isCsRoaming());

        when(mockCsState.getRegistrationState()).thenReturn(NetworkRegistrationInfo.REGISTRATION_STATE_HOME);
        assertFalse(mConditions.isCsRoaming());
    }

    @Test
    public void testOnConfigurationChanged() {
        when(mockConfiguration.getDefaultImsHandoverIn()).thenReturn(IN_STR);
        when(mockConfiguration.getImsRoveIn()).thenReturn("CDMA;GERAN;UTRAN{ecno=-5,rscp=-100};EUTRAN{rssnr=2,rsrp=-110,rsrq=-10}");
        when(mockConfiguration.getImsHandIn()).thenReturn("");
        when(mockConfiguration.getImsRoveInWiFiPreferred()).thenReturn("");
        when(mockConfiguration.getImsRoveInCellPreferred()).thenReturn("");
        when(mockConfiguration.getImsHandInWiFiPreferred()).thenReturn("");
        when(mockConfiguration.getImsHandInCellPreferred()).thenReturn("");
        when(mockConfiguration.getImsRoveInHysteresisOn()).thenReturn("");
        when(mockConfiguration.getImsHandInHysteresisOn()).thenReturn("");
        when(mockConfiguration.getDefaultImsHandoverOut()).thenReturn("");
        when(mockConfiguration.getImsRoveOut()).thenReturn("UTRAN{rscp=-110,ecno=-18};EUTRAN{rsrq=-18,rssnr=-3,rsrp=-125}");
        when(mockConfiguration.getImsHandOut()).thenReturn("");
        when(mockConfiguration.getImsRoveOutWiFiPreferred()).thenReturn("");
        when(mockConfiguration.getImsRoveOutCellPreferred()).thenReturn("");
        when(mockConfiguration.getImsHandOutWiFiPreferred()).thenReturn("");
        when(mockConfiguration.getImsHandOutCellPreferred()).thenReturn("");
        when(mockConfiguration.getImsRoveOutHysteresisOn()).thenReturn("");
        when(mockConfiguration.getImsHandOutHysteresisOn()).thenReturn("");
        when(mockConfiguration.getImsCellMid()).thenReturn("");
        mConditions.onConfigurationChanged();
        assertTrue(true);
    }

    @Test
    public void testGetRsrpRoveIn() {
        assertEquals(-115, mConditions.getRsrpRoveIn());
    }

    @Test
    public void testGetRsrpRoveOut() {
        assertEquals(-120, mConditions.getRsrpRoveOut());
    }

    @Test
    public void testGetRsrpMid() {
        assertEquals(-115, mConditions.getRsrpMid());
    }

    @Test
    public void testSetRsrpRoveIn() {
        mConditions.setRsrpRoveIn(-110);
        assertTrue(true);
    }

    @Test
    public void testSetRsrpRoveOut() {
        mConditions.setRsrpRoveOut(-119);
        assertTrue(true);
    }

    @Test
    public void testSetRsrpMid() {
        mConditions.setRsrpMid(-113);
        assertTrue(true);
    }

    @Test
    public void testTestToString() {
        assertNotNull(mConditions.toString());
    }
}