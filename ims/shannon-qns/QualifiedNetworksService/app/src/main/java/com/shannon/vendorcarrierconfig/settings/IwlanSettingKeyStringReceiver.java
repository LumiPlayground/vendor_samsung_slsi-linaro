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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class IwlanSettingKeyStringReceiver extends BroadcastReceiver {
    final String keyStringAction = "android.provider.Telephony.SECRET_CODE";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent != null && keyStringAction.equals(intent.getAction())) {
            Intent i = new Intent(Intent.ACTION_MAIN);
            i.setClass(context, IwlanSettingActivity.class);
            i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(i);
        }
    }
}
