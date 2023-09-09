/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.JsonRilTest;

import java.util.ArrayList;

import org.json.*;


public class JsonBuilder {
    private int intValue;
    private String strValue;
    private boolean boolValue;
    private double doubleValue;
    private long longValue;
    public JsonBuilder(int intValue, String strValue, boolean boolValue, double doubleValue, long longValue) {
        this.intValue = intValue;
        this.strValue = strValue;
        this.boolValue = boolValue;
        this.doubleValue = doubleValue;
        this.longValue = longValue;
    }

    public JSONObject genarateSingleObject() throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put("integer", intValue);
        obj.put("string", strValue);
        obj.put("bool", boolValue);
        obj.put("double", doubleValue);
        obj.put("long", longValue);
        return obj;
    }

    public JSONArray genarateObjectArray() throws JSONException {
        JSONArray jArray = new JSONArray();
        jArray.put(genarateSingleObject());
        jArray.put(genarateSingleObject());
        jArray.put(genarateSingleObject());
        return jArray;
    }

    public JSONArray genarateIntArray() throws JSONException {
        JSONArray jArray = new JSONArray();
        jArray.put(intValue);
        jArray.put(intValue);
        jArray.put(intValue);
        return jArray;
    }

    public JSONArray genarateStringArray() throws JSONException {
        JSONArray jArray = new JSONArray();
        jArray.put(strValue);
        jArray.put(strValue);
        jArray.put(strValue);
        return jArray;
    }

    public JSONArray genarateBoolArray() throws JSONException {
        JSONObject obj = new JSONObject();
        JSONArray jArray = new JSONArray();
        jArray.put(boolValue);
        jArray.put(boolValue);
        jArray.put(boolValue);
        return jArray;
    }

    public JSONArray genarateDoubleArray() throws JSONException {
        JSONArray jArray = new JSONArray();
        jArray.put(doubleValue);
        jArray.put(doubleValue);
        jArray.put(doubleValue);
        return jArray;
    }

    public JSONArray genarateLongArray() throws JSONException {
        JSONArray jArray = new JSONArray();
        jArray.put(longValue);
        jArray.put(longValue);
        jArray.put(longValue);
        return jArray;
    }

    public JSONObject genarateJsonObject() throws JSONException {
        JSONObject obj = genarateSingleObject();
        obj.put("IntArray", genarateIntArray());
        obj.put("StringArray", genarateStringArray());
        obj.put("BoolArray", genarateBoolArray());
        obj.put("DoubleArray", genarateDoubleArray());
        obj.put("LongArray", genarateLongArray());
        obj.put("objectArray", genarateObjectArray());
        return obj;
    }

    public String genarateJsonString() {
        try {
            JSONObject obj = genarateJsonObject();
            return obj.toString();
        } catch (JSONException e) {
            return "";
        }
    }

    public String genarateJsonString(int intentSpace) {
        try {
            JSONObject obj = genarateJsonObject();
            return obj.toString(intentSpace);
        } catch (JSONException e) {
            return "";
        }
    }
}
