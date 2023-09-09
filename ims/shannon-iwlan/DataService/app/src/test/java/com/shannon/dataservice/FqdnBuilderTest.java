package com.shannon.dataservice;

import android.telephony.CellIdentityLte;
import android.telephony.CellIdentityNr;
import android.telephony.CellIdentityWcdma;
import android.telephony.CellInfo;
import android.telephony.CellInfoLte;
import android.telephony.CellInfoNr;
import android.telephony.CellInfoWcdma;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.LinkedList;
import java.util.List;

import static com.shannon.dataservice.ConfigurationParameters.SERVER_SIM;
import static org.junit.Assert.*;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class FqdnBuilderTest {
    private static final String TEST_SIM_OPERATOR_STRING = "310260";

    private static final int APN_BITMASK = 1;

    @Mock TelephonyManager mMockTelephonyManager;
    @Mock DnsResolver mMockDnsResolver;
    @Mock SimRecords mMockSimRecords;
    @Mock ConfigurationParameters mMockConfigurationParameters;

    /* target class */
    private FqdnBuilder mFqdnBuilder;


    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        // default
        mFqdnBuilder = new FqdnBuilder(mMockConfigurationParameters, mMockTelephonyManager,
                mMockDnsResolver, mMockSimRecords);
    }

    @After
    public void tearDown() throws Exception {

    }

    /**
     * SERVER_3GPP_OPERATOR = "3gpp-operator";
     * SERVER_3GPP_LOCATION = "3gpp-location"
     * SERVER_3GPP_5G_LOCATION = "3gpp-location-5g"
     * SERVER_3GPP_VISITED = "3gpp-visited"
     * SERVER_IMS_OPERATOR = "ims-operator"
     * SERVER_SIM = "sim"
     */

    @Test
    public void testBuildLocationFqdnWithLac() {
        ConfigurationParameters configurationParameters = new ConfigurationParameters();
        configurationParameters.mRoamingFqdnEmc = SERVER_SIM; // "3gpp-operator:sim";
        mFqdnBuilder = new FqdnBuilder(configurationParameters, mMockTelephonyManager, mock(DnsResolver.class), mock(SimRecords.class));
        when(mMockTelephonyManager.getSimOperator()).thenReturn(String.valueOf(TEST_SIM_OPERATOR_STRING));

        LinkedList<String> buildList = mFqdnBuilder.build(ApnSetting.TYPE_EMERGENCY, false);

        buildList.get(0);
        assertEquals(buildList.get(0), "epdg.epc.mnc260.mcc310.pub.3gppnetwork.org");
    }

    @Test
    public void buildWithTypeEmergency_HOME_SERVER_3GPP_LOCATION_LTE() {

        // given
        CellInfo mMockCellInfo = mock(CellInfoLte.class);
        when(mMockCellInfo.isRegistered())
                .thenReturn(true);

        CellIdentityLte mMockCellIdentityLte = mock(CellIdentityLte.class);
        when(mMockCellIdentityLte.getTac()).thenReturn(TEST_VALUES.TAC_VALUE);
        when(mMockCellInfo.getCellIdentity())
                .thenReturn(mMockCellIdentityLte);

        List<CellInfo> cellInfoList = new LinkedList<CellInfo>();
        cellInfoList.add(mMockCellInfo);

        when(mMockTelephonyManager.getAllCellInfo())
                .thenReturn(cellInfoList);
        when(mMockTelephonyManager.getDataState())
                .thenReturn(TelephonyManager.DATA_CONNECTED);
        when(mMockTelephonyManager.getDataNetworkType())
                .thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mMockTelephonyManager.getNetworkOperator())
                .thenReturn("310083");
        mMockConfigurationParameters.mHomeFqdnIms = "3gpp-location:plmn";

        mFqdnBuilder = new FqdnBuilder(mMockConfigurationParameters, mMockTelephonyManager,
                mMockDnsResolver, mMockSimRecords);

        // when
        LinkedList<String> resultStrings = mFqdnBuilder.build(ApnSetting.TYPE_EMERGENCY, false);

        // then
        assertEquals("tac-lb14.tac-hb00.epdg.epc.mnc083.mcc310.pub.3gppnetwork.org",
                resultStrings.get(0));
    }

    @Test
    public void buildWithTypeEmergency_HOME_SERVER_3GPP_LOCATION_WCDMA() {
        // This method should called "buildLocationFqdnWith5gsTac".

        // given
        CellInfo mMockCellInfo = mock(CellInfoWcdma.class);
        when(mMockCellInfo.isRegistered())
                .thenReturn(true);

        CellIdentityWcdma mMockCellIdentityWcdma = mock(CellIdentityWcdma.class);
        when(mMockCellIdentityWcdma.getLac()).thenReturn(TEST_VALUES.LAC_VALUE);
        when(mMockCellInfo.getCellIdentity())
                .thenReturn(mMockCellIdentityWcdma);

        List<CellInfo> cellInfoList = new LinkedList<CellInfo>();
        cellInfoList.add(mMockCellInfo);

        when(mMockTelephonyManager.getAllCellInfo())
                .thenReturn(cellInfoList);
        when(mMockTelephonyManager.getDataState())
                .thenReturn(TelephonyManager.DATA_CONNECTED);
        when(mMockTelephonyManager.getDataNetworkType())
                .thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mMockTelephonyManager.getNetworkOperator())
                .thenReturn("310083");
        mMockConfigurationParameters.mHomeFqdnIms = "3gpp-location:plmn";

        mFqdnBuilder = new FqdnBuilder(mMockConfigurationParameters, mMockTelephonyManager,
                mMockDnsResolver, mMockSimRecords);

        // when
        LinkedList<String> resultStrings = mFqdnBuilder.build(ApnSetting.TYPE_EMERGENCY, false);

        // then
        assertEquals("lac0014.epdg.epc.mnc083.mcc310.pub.3gppnetwork.org",
                resultStrings.get(0));
    }

    @Test
    public void buildWithTypeEmergency_HOME_SERVER_3GPP_5G_LOCATION_LTE() {
        // This method should called "buildLocationFqdnWith5gsTac".

        // given
        CellInfo mMockCellInfo = mock(CellInfoLte.class);
        when(mMockCellInfo.isRegistered())
                .thenReturn(true);

        CellIdentityLte mMockCellIdentityLte = mock(CellIdentityLte.class);
        when(mMockCellIdentityLte.getTac()).thenReturn(TEST_VALUES.TAC_VALUE);
        when(mMockCellInfo.getCellIdentity())
                .thenReturn(mMockCellIdentityLte);

        List<CellInfo> cellInfoList = new LinkedList<CellInfo>();
        cellInfoList.add(mMockCellInfo);

        when(mMockTelephonyManager.getAllCellInfo())
                .thenReturn(cellInfoList);
        when(mMockTelephonyManager.getDataState())
                .thenReturn(TelephonyManager.DATA_CONNECTED);
        when(mMockTelephonyManager.getDataNetworkType())
                .thenReturn(TelephonyManager.NETWORK_TYPE_LTE);
        when(mMockTelephonyManager.getNetworkOperator())
                .thenReturn("310083");
        mMockConfigurationParameters.mHomeFqdnIms = "3gpp-location-5g:plmn";

        mFqdnBuilder = new FqdnBuilder(mMockConfigurationParameters, mMockTelephonyManager,
                mMockDnsResolver, mMockSimRecords);

        // when
        LinkedList<String> resultStrings = mFqdnBuilder.build(ApnSetting.TYPE_EMERGENCY, false);

        // then
        assertEquals("tac-lb14.tac-mb00.tac-hb00.5gstac.epdg.epc.mnc083.mcc310.pub.3gppnetwork.org",
                resultStrings.get(0));
    }

    @Test
    public void buildWithTypeEmergency_HOME_SERVER_3GPP_5G_LOCATION_NR() {
        // This method should called "buildLocationFqdnWith5gsTac".

        // given
        CellInfo mMockCellInfo = mock(CellInfoNr.class);
        when(mMockCellInfo.isRegistered())
                .thenReturn(true);

        CellIdentityNr mMockCellIdentityNr = mock(CellIdentityNr.class);
        when(mMockCellIdentityNr.getTac()).thenReturn(TEST_VALUES.TAC_VALUE);
        when(mMockCellInfo.getCellIdentity())
                .thenReturn(mMockCellIdentityNr);

        List<CellInfo> cellInfoList = new LinkedList<CellInfo>();
        cellInfoList.add(mMockCellInfo);

        when(mMockTelephonyManager.getAllCellInfo())
                .thenReturn(cellInfoList);
        when(mMockTelephonyManager.getDataState())
                .thenReturn(TelephonyManager.DATA_CONNECTED);
        when(mMockTelephonyManager.getDataNetworkType())
                .thenReturn(TelephonyManager.NETWORK_TYPE_NR);
        when(mMockTelephonyManager.getNetworkOperator())
                .thenReturn("310083");
        mMockConfigurationParameters.mHomeFqdnIms = "3gpp-location-5g:plmn";

        mFqdnBuilder = new FqdnBuilder(mMockConfigurationParameters, mMockTelephonyManager,
                mMockDnsResolver, mMockSimRecords);

        // when
        LinkedList<String> resultStrings = mFqdnBuilder.build(ApnSetting.TYPE_EMERGENCY, false);

        // then
        assertEquals("tac-lb14.tac-mb00.tac-hb00.5gstac.epdg.epc.mnc083.mcc310.pub.3gppnetwork.org",
                resultStrings.get(0));
    }
}
