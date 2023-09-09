package com.shannon.networkservice;

import org.junit.runner.RunWith;
import org.robolectric.RobolectricTestRunner;
import org.robolectric.annotation.Config;

public class ConfigurationManagerBoat extends ConfigurationManager {

    public ConfigurationManagerBoat(int slotId, String tag) {
        super(slotId, tag);
    }

    public ConfigurationReceiverCallback createConfigurationReceiverCallback() {
        return new ConfigurationReceiverCallback();
    }

}
