/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
package com.samsung.slsi.telephony.carrierconfigoverride;

import android.telephony.CarrierConfigManager;

import java.util.ArrayList;
import java.util.Arrays;

public class CarrierConfigList {

    public static final ArrayList<CarrierConfig> ENTRY_CONFIGS = new ArrayList<CarrierConfig>(
            Arrays.asList(
                    new CarrierConfig("VoNR Enabled", "Flag specifying whether VoNR should be enabled for carrier", CarrierConfigManager.KEY_VONR_ENABLED_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, true),
                    new CarrierConfig("VoNR Setting Visibility", "Determines whether Vo5G toggle will be shown in the settings", CarrierConfigManager.KEY_VONR_SETTING_VISIBILITY_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, false),
                    new CarrierConfig("4G Calling", "Use 4G services to improve voice calls", CarrierConfigManager.KEY_EDITABLE_ENHANCED_4G_LTE_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, true),
                    new CarrierConfig("Enhanced 4G", "Determines whether the Enhanced 4G LTE toggle will be shown in the settings", CarrierConfigManager.KEY_HIDE_ENHANCED_4G_LTE_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, true),
                    new CarrierConfig("APN Editor view Visibility", "Flag to hide Preset APN details", CarrierConfigManager.KEY_HIDE_PRESET_APN_DETAILS_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, true),
                    new CarrierConfig("Allow to add APNs", "Allow user to add APNs", CarrierConfigManager.KEY_ALLOW_ADDING_APNS_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, false),
                    new CarrierConfig("Allow to edit APNs", "Allow user to edit current APNs", CarrierConfigManager.KEY_READ_ONLY_APN_TYPES_STRING_ARRAY, ValueType.TYPE_ALLOW_TO_ADD_APNS_PREFERENCE, true),
                    new CarrierConfig("IMS APN Visibility", "Determine whether IMS apn can be shown", CarrierConfigManager.KEY_HIDE_IMS_APN_BOOL, ValueType.TYPE_BASE_SWITCH_PREFERENCE, true)
                    //new CarrierConfig("APN fields", "APN fields that user is not allowed to modify.", CarrierConfigManager.KEY_READ_ONLY_APN_TYPES_STRING_ARRAY, ValueType.TYPE_STRING, true)

            )
    );
}

class CarrierConfig {
    String title;
    String summary;
    String key;
    int subId;
    int type;
    boolean visible;

    CarrierConfig(String title, String summary, String key, int type, boolean visible) {
        this.title = title;
        this.summary = summary;
        this.key = key;
        this.type = type;
        this.visible = visible;
    }

    public void setSubId(int id) {
        subId = id;
    }
}

class ValueType {
    public static final int TYPE_BASE_SWITCH_PREFERENCE = 0;
    public static final int TYPE_INTEGER = 2;
    public static final int TYPE_STRING = 3;

    // custom preference
    public static final int TYPE_ALLOW_TO_ADD_APNS_PREFERENCE = 100;
}