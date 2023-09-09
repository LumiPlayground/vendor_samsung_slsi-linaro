package com.shannon.vendorcarrierconfig.settings;

import static org.junit.Assert.*;
import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.when;

import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import com.shannon.qualifiednetworksservice.R;
import com.shannon.qualifiednetworksservice.ShannonQualifiedNetworksService;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.Robolectric;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class IwlanSettingActivityTest {
    private IwlanSettingActivity iwlanSettingActivity;

    private static MockedStatic<ShannonQualifiedNetworksService> sMockedQualifiedNetworksService;
    private static MockedStatic<ShannonVendorCarrierConfig> sMockedShannonVendorCarrierConfig;

    @BeforeClass
    public static void beforeClass() {
        sMockedQualifiedNetworksService = mockStatic(ShannonQualifiedNetworksService.class);
        sMockedShannonVendorCarrierConfig = mockStatic(ShannonVendorCarrierConfig.class);
    }

    @AfterClass
    public static void afterClass() {
        sMockedShannonVendorCarrierConfig.close();
        sMockedQualifiedNetworksService.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        Bundle bundle = new Bundle();
        bundle.putInt("test_int", 1);
        bundle.putBoolean("test_boolean", true);
        bundle.putString("test_string", "string");
        when(ShannonVendorCarrierConfig.getVendorCarrierConfigForSlot(anyInt())).thenReturn(bundle);

        // Create instance without real Activity for method test only
        iwlanSettingActivity = Robolectric.buildActivity(IwlanSettingActivity
                .class).create().start().resume().get();
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnReceive() {
        Bundle savedInstanceState = mock(Bundle.class);
        Menu menu = mock(Menu.class);

        iwlanSettingActivity.assembleBundle();
        iwlanSettingActivity.onBackPressed();
        iwlanSettingActivity.onDestroy();
    }

    @Test
    public void testOnOptionsItemSelected() {
        MenuItem item = mock(MenuItem.class);
        when(item.getItemId()).thenReturn(R.id.setting_menu_sim1_policy);
        iwlanSettingActivity.onOptionsItemSelected(item);

        when(item.getItemId()).thenReturn(R.id.setting_menu_sim2_policy);
        iwlanSettingActivity.onOptionsItemSelected(item);

        when(item.getItemId()).thenReturn(R.id.setting_menu_reset);
        iwlanSettingActivity.onOptionsItemSelected(item);
    }

    @Test
    public void testSettingItem() {
        // given
        String testString = "apple.banana";
        Object mockObject = mock(Object.class);

        // when
        IwlanSettingActivity.SettingItem settingItem = iwlanSettingActivity
                .new SettingItem(testString, mockObject);

        // then
        assertEquals("banana", settingItem.getTitle());
        assertEquals("",  settingItem.getDescription());
    }

    @Test
    public void testSettingItem_getValue() {
        // given
        String testString = "apple.banana";
        Object testObject = "testAsString";

        // when
        IwlanSettingActivity.SettingItem settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);

        // then
        assertEquals("testAsString", settingItem.getValue());
        assertEquals("SettingItem{key=apple.banana value=testAsString}", settingItem.toString());
        assertEquals("apple.banana", settingItem.getKey());
    }

    @Test
    public void testSettingItem_getValue_when_value_is_boolean() {
        // given
        String testString = "apple.banana";
        Object testObject = true;

        // when
        IwlanSettingActivity.SettingItem settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);

        // then
        assertEquals("true", settingItem.getValue());
    }

    @Test
    public void testSettingItem_updateHandler() {
        String testString = "apple.banana";
        Object testObject = "testAsString";
        IwlanSettingActivity.SettingItem settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);

        String input1 = "testAsString";
        settingItem.updateHandler(input1);
    }

    @Test
    public void testSettingItem_updateHandler_asBoolean() {
        String testString = "apple.banana";
        Object testObject = true;

        IwlanSettingActivity.SettingItem settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        String input = "inputAsString";
        settingItem.updateHandler(input);

        testObject = true;
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "1";
        settingItem.updateHandler(input);

        testObject = true;
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "false";
        settingItem.updateHandler(input);

        testObject = true;
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "0";
        settingItem.updateHandler(input);

        testObject = 1;
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "0";
        settingItem.updateHandler(input);

        testObject = 1;
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "0a";
        settingItem.updateHandler(input);

        testObject = "1a";
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "0"; // intentionaly, occur error
        settingItem.updateHandler(input);

        testObject = new Object();
        settingItem = iwlanSettingActivity
                .new SettingItem(testString, testObject);
        input = "0"; // intentionaly, occur error
        settingItem.updateHandler(input);
    }
}