package com.shannon.vendorcarrierconfig.settings;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyBoolean;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.os.Bundle;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ListView;

import androidx.fragment.app.FragmentActivity;
import androidx.fragment.app.FragmentManager;

import com.shannon.qualifiednetworksservice.R;
import com.shannon.qualifiednetworksservice.ShannonQualifiedNetworksService;
import com.shannon.vendorcarrierconfig.ShannonVendorCarrierConfig;
import com.shannon.vendorcarrierconfig.settings.IwlanSettingActivity.SettingItem;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedStatic;
import org.mockito.MockitoAnnotations;
import org.mockito.Spy;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import java.util.ArrayList;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimSettingFragmentTest extends TestCase {
    private SimSettingFragment mSimSettingFragment;
    private SimSettingFragment mSpySimSettingFragment;

    private static final int testSlotId = 0;

    private static MockedStatic<LayoutInflater> sLayoutInflater;
    private static MockedStatic<ShannonQualifiedNetworksService> sShannonQualifiedNetworksServiceMockedStatic;
    private static MockedStatic<ShannonVendorCarrierConfig> mMockShannonVendorCarrierConfig;

    @Mock
    Context mockContext = mock(Context.class);

    @Mock
    Bundle mockBundle = mock(Bundle.class);

    @Mock
    View mockView = mock(View.class);

    @BeforeClass
    public static void beforeClass() {
        sLayoutInflater = mockStatic(LayoutInflater.class);
        sShannonQualifiedNetworksServiceMockedStatic =  mockStatic(ShannonQualifiedNetworksService.class);
        mMockShannonVendorCarrierConfig = mockStatic(ShannonVendorCarrierConfig.class);
    }

    @AfterClass
    public static void afterClass() {
        mMockShannonVendorCarrierConfig.close();
        sLayoutInflater.close();
        sShannonQualifiedNetworksServiceMockedStatic.close();
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        SettingItem settingItem = mock(SettingItem.class);
        ArrayList<SettingItem> settingItems = new ArrayList<>();
        settingItems.add(settingItem);
        when(ShannonVendorCarrierConfig.getVendorCarrierConfigForSlot(testSlotId))
                .thenReturn(mockBundle);
        mSimSettingFragment = SimSettingFragment.newInstance(testSlotId, settingItems);
        mSpySimSettingFragment = spy(mSimSettingFragment);
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testOnCreateView() {
        // Given
        LayoutInflater layoutInflater = mock(LayoutInflater.class);
        ViewGroup mockViewGroup = mock(ViewGroup.class);
        EditText mMockeditText = mock(EditText.class);

        // When
        sLayoutInflater.when(() -> LayoutInflater.from(any())).thenReturn(layoutInflater);
        when(layoutInflater.inflate(anyInt(), any(), anyBoolean())).thenReturn(mockView);
        doReturn(mMockeditText).when(mockView).findViewById(R.id.search_edittext);
        doNothing().when(mMockeditText).addTextChangedListener(any());

        // Then
        mSimSettingFragment.onCreateView(layoutInflater, mockViewGroup, mockBundle);
    }


    @Test
    public void testOnAttach() {
        mSimSettingFragment.onAttach(mockContext);
    }


    @Test
    public void testOnViewCreated() {
        ListView listView = mock(ListView.class);
        when(mockView.findViewById(anyInt())).thenReturn(listView);

        mSimSettingFragment.onAttach(mockContext);

        FragmentActivity mMockFragmentActivity = mock(FragmentActivity.class);
        FragmentManager mMockFragmentManager = mock(FragmentManager.class);
        doReturn(mMockFragmentManager).when(mMockFragmentActivity).getSupportFragmentManager();
        doReturn(mMockFragmentActivity).when(mSpySimSettingFragment).getActivity();
        mSpySimSettingFragment.onViewCreated(mockView, mockBundle);
    }

    @Test
    public void testOnDetach() {
        mSimSettingFragment.onDetach();
    }
}