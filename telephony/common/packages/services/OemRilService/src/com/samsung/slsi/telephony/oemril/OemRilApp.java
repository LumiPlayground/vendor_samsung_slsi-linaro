/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.samsung.slsi.telephony.oemril;

import android.app.Application;
import android.os.UserHandle;
import android.util.Log;

public class OemRilApp extends Application {

    private static final String TAG = "OemRilApp";

    OemRilGlobals mOemRilGlobals;

    public OemRilApp() {

    }

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate: " + UserHandle.myUserId());
        if (UserHandle.myUserId() == 0) {
            // We are running as the primary user.
            mOemRilGlobals = new OemRilGlobals(this);
            mOemRilGlobals.onCreate();
        }
    }
}
