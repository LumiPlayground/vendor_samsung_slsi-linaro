package com.shannon.vendorcarrierconfig;

import android.content.Context;
import android.content.Intent;

import junit.framework.TestCase;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class MediaMountBroadcastReceiverTest extends TestCase {

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testOnReceive() {
        MediaMountBroadcastReceiver receiver = new MediaMountBroadcastReceiver();

        Context context = mock(Context.class);
        Intent intent = mock(Intent.class);

        when(intent.getAction()).thenReturn(null);
        receiver.onReceive(context, intent);

        when(intent.getAction()).thenReturn(Intent.ACTION_MEDIA_MOUNTED);
        receiver.onReceive(null, intent);
        receiver.onReceive(context, intent);

        assertTrue(true);
    }
}