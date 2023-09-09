package com.shannon.qualifiednetworksservice.util;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import com.shannon.qualifiednetworksservice.ShannonQualifiedNetworksService;

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

@RunWith(RobolectricTestRunner.class)
@Config(sdk = {32})
public class PackageVersionTest extends TestCase {
    private PackageVersion mPackageVersion;

    @Mock
    Context mockContext = mock(Context.class);

    @Mock
    PackageManager mockPackageManager = mock(PackageManager.class);

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.openMocks(this);

        Context context = Mockito.mock(Context.class);
        PackageManager manager = Mockito.mock(PackageManager.class);

        PackageInfo packageInfo = new PackageInfo();
        packageInfo.versionName = "testVersion01";
        packageInfo.lastUpdateTime = Long.valueOf(123456789);

        ApplicationInfo appInfo = new ApplicationInfo();
        appInfo.targetSdkVersion = 32;

        Mockito.when(context.getPackageManager()).thenReturn(manager);
        Mockito.when(context.getPackageName()).thenReturn("testCode");
        Mockito.when(manager.getPackageInfo("testCode", 0)).thenReturn(packageInfo);
        Mockito.when(context.getApplicationInfo()).thenReturn(appInfo);

        mPackageVersion = new PackageVersion(ShannonQualifiedNetworksService.getServiceTAG("-Version"), context);
    }

    @After
    public void tearDown() throws Exception {
        Mockito.validateMockitoUsage();
    }

    @Test
    public void testAbnormal() {
        when(mockContext.getApplicationInfo()).thenReturn(null);

        mPackageVersion = new PackageVersion(ShannonQualifiedNetworksService.getServiceTAG("-Version"), mockContext);
    }

    @Test
    public void testTestToString() {
        assertNotNull(mPackageVersion.toString());
    }
}