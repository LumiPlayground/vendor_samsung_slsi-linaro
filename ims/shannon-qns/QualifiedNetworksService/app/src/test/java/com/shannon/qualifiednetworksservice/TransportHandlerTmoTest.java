package com.shannon.qualifiednetworksservice;

import android.telephony.AccessNetworkConstants;
import android.telephony.DataFailCause;
import android.telephony.NetworkRegistrationInfo;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import com.shannon.qualifiednetworksservice.Transport.TransportType;
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

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class TransportHandlerTmoTest extends TestCase {
    private TransportHandlerTmo mTransportHandlerTmo;

    @Mock
    private TelephonyManager mockTelephonyManager;

    @Mock
    private Transport mockTransport;

    @Mock
    private TransportState mockTransportState;

    @Mock
    private ServiceState mockServiceState;

    @Mock
    private NetworkRegistrationInfo mockPsRegState, mockCsRegState;

    @Mock
    private ConfigurationManager mockConfigurationManager;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getSimCarrierId()).thenReturn(TelephonyManager.UNKNOWN_CARRIER_ID);
        when(mockTelephonyManager.getCarrierIdFromSimMccMnc()).thenReturn(678);


        when(mockTelephonyManager.getSimSpecificCarrierId()).thenReturn(32);

        when(mockConfigurationManager.getRetryParameters()).thenReturn("N_fqdn");

        mTransportHandlerTmo = new TransportHandlerTmo(ApnSetting.TYPE_IMS, mockTelephonyManager, mockConfigurationManager);
    }

    @After
    public void tearDown() {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void onHandoverFailureTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onHandoverFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 2);

        // Then
        assertFalse(test_false);

        // Given
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);

        // When
        boolean test_true_NETWORK_FAILURE = mTransportHandlerTmo.onHandoverFailure(mockTransport, DataFailCause.NETWORK_FAILURE, 0);

        // Then
        assertTrue(test_true_NETWORK_FAILURE);

        // When
        boolean test_true_OEM_DCFAILCAUSE_1 = mTransportHandlerTmo.onHandoverFailure(mockTransport, DataFailCause.OEM_DCFAILCAUSE_1, 0);

        // Then
        assertTrue(test_true_OEM_DCFAILCAUSE_1);

        // When
        boolean test_true_SERVICE_OPTION_NOT_SUPPORTED = mTransportHandlerTmo.onHandoverFailure(mockTransport, DataFailCause.SERVICE_OPTION_NOT_SUPPORTED, 0);

        // Then
        assertTrue(test_true_SERVICE_OPTION_NOT_SUPPORTED);

        // When
        boolean test_true_APN_DISALLOWED_ON_ROAMING = mTransportHandlerTmo.onHandoverFailure(mockTransport, DataFailCause.APN_DISALLOWED_ON_ROAMING, 0);

        // Then
        assertTrue(test_true_APN_DISALLOWED_ON_ROAMING);

        // When
        boolean test_true_CONGESTION = mTransportHandlerTmo.onHandoverFailure(mockTransport, DataFailCause.CONGESTION, 0);

        // Then
        assertTrue(test_true_CONGESTION);

        // When
        boolean test_true_default = mTransportHandlerTmo.onHandoverFailure(mockTransport, 2060, 0);

        // Then
        assertTrue(test_true_default);

        // Given
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);
        when(mockTransport.getType()).thenReturn(TransportType.TRANSPORT_CELLULAR);

        // When
        boolean test_false_no_wifi = mTransportHandlerTmo.onHandoverFailure(mockTransport, 2060, 0);

        // Then
        assertFalse(test_false_no_wifi);
    }

    @Test
    public void onConnectionFailureTest() {
        // Given

        // When
        boolean test_true_default = mTransportHandlerTmo.onConnectionFailure(mockTransport, 2060, 2);

        // Then
        assertFalse(test_true_default);
    }


    @Test
    public void onAirplaneModeChangedTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onAirplaneModeChanged(mockTransport);

        // Then
        assertFalse(test_false);
        // Given
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);

        // isImsOnWiFi
        mTransportHandlerTmo.setBlockedUntilWifiAssociated(true);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);

        // When
        boolean test_true_default = mTransportHandlerTmo.onAirplaneModeChanged(mockTransport);

        // Then
        assertTrue(test_true_default);
    }

    @Test
    public void onOperatorChangedTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onOperatorChanged(mockTransport);

        // Then
        assertFalse(test_false);
        // Given
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);

        // isRegisteredHome
        mTransportHandlerTmo.setBlockedUntilHome(true);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);
        when(mockTelephonyManager.getServiceState()).thenReturn(mockServiceState);
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_PS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(mockPsRegState);
        when(mockServiceState.getNetworkRegistrationInfo(NetworkRegistrationInfo.DOMAIN_CS, AccessNetworkConstants.TRANSPORT_TYPE_WWAN)).thenReturn(mockCsRegState);
        when(mockTelephonyManager.isNetworkRoaming()).thenReturn(false);

        // isRegistered
        when(mockPsRegState.isRegistered()).thenReturn(true);
        when(mockCsRegState.isRegistered()).thenReturn(true);

        // When
        boolean test_false_default = mTransportHandlerTmo.onOperatorChanged(mockTransport);

        // Then
        assertFalse(test_false_default);
    }


    @Test
    public void onWiFiStateChangedTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onWiFiStateChanged(mockTransport);

        // Then
        assertFalse(test_false);
        // Given
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);

        // isImsOnWiFi
        mTransportHandlerTmo.setBlockedUntilWifiAssociated(true);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);

        // When
        boolean test_true = mTransportHandlerTmo.onWiFiStateChanged(mockTransport);

        // Then
        assertTrue(test_true);
    }

    @Test
    public void onWiFiAccessPointChangedTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onWiFiAccessPointChanged(mockTransport);

        // Then
        assertFalse(test_false);
        // Given
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);

        // isImsOnWiFi
        mTransportHandlerTmo.setBlockedUntilWifiAssociated(true);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);

        // When
        boolean test_true = mTransportHandlerTmo.onWiFiAccessPointChanged(mockTransport);

        // Then
        assertTrue(test_true);
    }

    @Test
    public void onWfcStateChangedTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onWfcStateChanged(mockTransport);

        // Then
        assertFalse(test_false);
        // Given
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);

        // isImsOnWiFi
        mTransportHandlerTmo.setBlockedUntilWifiAssociated(true);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);

        // When
        boolean test_true = mTransportHandlerTmo.onWfcStateChanged(mockTransport);

        // Then
        assertTrue(test_true);
    }

    @Test
    public void onCarrierConfigChangedTest() {

        // Given
        when(mockConfigurationManager.getRetryParameters()).thenReturn("N_fast_try");

        // When
        boolean test_true_N_fast_try = mTransportHandlerTmo.onCarrierConfigChanged(mockTransport);

        // Then
        assertTrue(test_true_N_fast_try);

        // Given
        when(mockConfigurationManager.getRetryParameters()).thenReturn("N_max");

        // When
        boolean test_true_N_max = mTransportHandlerTmo.onCarrierConfigChanged(mockTransport);

        // Then
        assertTrue(test_true_N_max);

        // Given
        when(mockConfigurationManager.getRetryParameters()).thenReturn("T_fast");

        // When
        boolean test_true_T_fast = mTransportHandlerTmo.onCarrierConfigChanged(mockTransport);

        // Then
        assertTrue(test_true_T_fast);

        // Given
        when(mockConfigurationManager.getRetryParameters()).thenReturn("T_start");

        // When
        boolean test_true_T_start = mTransportHandlerTmo.onCarrierConfigChanged(mockTransport);

        // Then
        assertTrue(test_true_T_start);
    }


    @Test
    public void onRegistrationFailedTest() {
        // Default case
        // When
        boolean test_false = mTransportHandlerTmo.onRegistrationFailed(mockTransport, 15);

        // Then
        assertFalse(test_false);
        // Given
        when(mockTransport.getInitialState()).thenReturn(mockTransportState);
        when(mockTransport.getHandoverState()).thenReturn(mockTransportState);

        // isImsOnWiFi
        mTransportHandlerTmo.setBlockedUntilWifiAssociated(true);
        when(mockTransport.getType()).thenReturn(Transport.TransportType.TRANSPORT_WIFI);

        // When
        boolean test_true = mTransportHandlerTmo.onRegistrationFailed(mockTransport, 15);

        // Then
        assertTrue(test_true);
    }
}