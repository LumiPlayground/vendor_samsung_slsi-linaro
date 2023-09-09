/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import com.shannon.qualifiednetworksservice.R;
import com.shannon.vendorcarrierconfig.settings.IwlanSettingActivity.SettingItem;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.MockedConstruction;
import org.mockito.MockedStatic;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.mockStatic;
import static org.mockito.Mockito.when;

import java.util.ArrayList;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class SimSettingAdapterTest extends TestCase {
    private SimSettingAdapter mSimSettingAdapter;

    @BeforeClass
    public static void beforeClass() {
        sLayoutInflater =  mockStatic(LayoutInflater.class);
    }

    @AfterClass
    public static void afterClass() {
        sLayoutInflater.close();
    }

    private static MockedStatic<LayoutInflater> sLayoutInflater;

    @Mock ArrayList<SettingItem> mMockSimSettingItem;
    @Mock SettingItem mMockSettingItem;
    @Mock View mMockConvertView;
    @Mock TextView mMockTitle;
    @Mock TextView mMockDescription;

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
        Context context = mock(Context.class);
        doReturn(mMockTitle).when(mMockConvertView).findViewById(R.id.textEditorTitle);
        doReturn(mMockDescription).when(mMockConvertView).findViewById(R.id.textEditorValue);
        doNothing().when(mMockTitle).setText(any());
        doNothing().when(mMockDescription).setText(any());

        int resource = 2;

        try (MockedConstruction<ArrayAdapter> mockedConstruction =
            Mockito.mockConstruction(ArrayAdapter.class)) {
        }
        doReturn(mMockSettingItem).when(mMockSimSettingItem).get(anyInt());
        mSimSettingAdapter = new SimSettingAdapter(context, resource, mMockSimSettingItem);

    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testGetView() {
        //Given
        int position = 10;
        TextView textView = mock(TextView.class);
        View convertView = mock(View.class);
        ViewGroup parent = mock(ViewGroup.class);
        when(convertView.findViewById(R.id.textEditorTitle)).thenReturn(textView);
        when(convertView.findViewById(R.id.textEditorValue)).thenReturn(textView);

        //When
        View returnVal = mSimSettingAdapter.getView(position, convertView, parent);

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetViewConvertViewNull() {
        //Given
        int position = 10;
        TextView textView = mock(TextView.class);
        ViewGroup parent = mock(ViewGroup.class);
        View convertView =  mock(View.class);
        LayoutInflater layoutInflater = mock(LayoutInflater.class);
        sLayoutInflater.when(() -> LayoutInflater.from(any())).thenReturn(layoutInflater);
        when(layoutInflater.inflate(R.layout.setting_list_item, parent, false)).thenReturn(convertView);
        when(convertView.findViewById(R.id.textEditorTitle)).thenReturn(textView);
        when(convertView.findViewById(R.id.textEditorValue)).thenReturn(textView);

        //When
        View returnVal = mSimSettingAdapter.getView(position, convertView, parent);

        //Then
        assertNotNull(returnVal);
    }

    @Test
    public void testGetViewNotNull() {
        //Given
        int position = 10;
        ViewGroup parent = mock(ViewGroup.class);
        View view =  mock(View.class);
        SettingItem item = mock(SettingItem.class);
        TextView textView = mock(TextView.class);
        LayoutInflater layoutInflater = mock(LayoutInflater.class);
        sLayoutInflater.when(() -> LayoutInflater.from(any())).thenReturn(layoutInflater);
        when(mSimSettingAdapter.getItem(position)).thenReturn(item);
        when(item.getTitle()).thenReturn("testString1");
        when(item.getValue()).thenReturn("testString2");
        when(layoutInflater.inflate(R.layout.setting_list_item, parent, false)).thenReturn(view);
        when(view.findViewById(R.id.textEditorTitle)).thenReturn(textView);
        when(view.findViewById(R.id.textEditorValue)).thenReturn(textView);

        //When
        View returnVal = mSimSettingAdapter.getView(position, view, parent);

        //Then
        assertNotNull(returnVal);
    }
}

