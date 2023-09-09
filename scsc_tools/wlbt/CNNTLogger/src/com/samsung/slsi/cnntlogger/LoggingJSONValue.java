/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.cnntlogger;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

class LoggingJSONValue {
    static final String NAME_KEY = "name";

    static final String EXEC_KEY = "exec";

    static final String DIR_KEY = "dir";

    static final String OPTION_KEY = "option";

    static final String DATA_KEY = "data";

    private JSONArray mJSonArray;

    LoggingJSONValue() {
        String jsonValue = "[{\"name\":\"wifilog\", \"exec\":\"start\", \"option\":\"udilog\", \"dir\":\"\"},"
                + "{\"name\":\"wifilog\", \"exec\":\"start\", \"option\":\"mxlog\", \"dir\":\"\"},"
                + "{\"name\":\"wifilog\", \"exec\":\"start\", \"option\":\"all\", \"dir\":\"\"},"
                + "{\"name\":\"wifilog\", \"exec\":\"stop\", \"option\":\"udilog\", \"dir\":\"\"},"
                + "{\"name\":\"wifilog\", \"exec\":\"stop\", \"option\":\"mxlog\", \"dir\":\"\"},"
                + "{\"name\":\"wifilog\", \"exec\":\"stop\", \"option\":\"all\", \"dir\":\"\"},"
                + "{\"name\":\"wifilog\", \"exec\":\"hip4\", \"option\":\"hip4\", \"dir\":\"\"},"

                + "{\"name\":\"btlog\", \"exec\":\"start\", \"option\":\"general\", \"dir\":\"\"},"
                + "{\"name\":\"btlog\", \"exec\":\"start\", \"option\":\"audio\", \"dir\":\"\"},"
                + "{\"name\":\"btlog\", \"exec\":\"start\", \"option\":\"custom\", \"dir\":\"\", \"data\":\"\"},"
                + "{\"name\":\"btlog\", \"exec\":\"stop\", \"option\":\"\", \"dir\":\"\"}]";

        try {
            mJSonArray = new JSONArray(jsonValue);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    JSONObject getJSonValue(int num, String filepath, String data) {
        JSONObject json = null;
        try {
            json = mJSonArray.getJSONObject(num);
            json.put(DIR_KEY, filepath);
            if (data != null) {
                json.put(DATA_KEY, data);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return json;
    }
}
