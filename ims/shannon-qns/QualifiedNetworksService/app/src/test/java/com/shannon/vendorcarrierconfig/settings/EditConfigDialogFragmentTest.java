package com.shannon.vendorcarrierconfig.settings;

import static org.junit.Assert.*;
import static org.mockito.Mockito.mock;

import android.os.Bundle;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.robolectric.Robolectric;

public class EditConfigDialogFragmentTest {

    EditConfigDialogFragment editConfigDialogFragment;

    @Before
    public void setUp() throws Exception {
        editConfigDialogFragment = new EditConfigDialogFragment();
    }

    @After
    public void tearDown() throws Exception {
    }

    @Test
    public void testSettingItem() {
        IwlanSettingActivity.SettingItem mockSettingItem = mock(IwlanSettingActivity.SettingItem.class);
        editConfigDialogFragment.setSettingItem(mockSettingItem);

    }

    @Test
    public void testSetListener() {
        EditConfigDialogFragment.DialogEventListener mockListener =
                mock(EditConfigDialogFragment.DialogEventListener.class);
        editConfigDialogFragment.setListener(mockListener);
    }

    @Test
    public void testOnResume() {
        editConfigDialogFragment.onResume();
    }
}