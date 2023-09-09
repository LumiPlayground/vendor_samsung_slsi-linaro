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
//import java.io;
import java.util.ArrayList;

import org.json.*;

public class JsonParser {
    private JSONObject rootObj;
    public JsonParser(String data) {
        try {
            rootObj = new JSONObject(data);
        } catch(JSONException e){
            e.printStackTrace();
        }
    }

    public JSONObject getRootJsonObject() {
        return rootObj;
    }

    public int getInteger()  throws JSONException {
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            return jObj.getInt("integer");
        }
        return -1;
    }

    public String getString()  throws JSONException {
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            return jObj.getString("string");
        }
        return "";
    }

    public boolean getBoolean() throws JSONException {
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            return jObj.getBoolean("bool");
        }
        return false;
    }

    public double getDouble() throws JSONException {
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            return jObj.getDouble("double");
        }
        return 0;
    }

    public long getLong() throws JSONException{
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            return jObj.getLong("long");
        }
        return 0;
    }

    public ArrayList<Integer> getIntArray()  throws JSONException{
        ArrayList<Integer> list = new ArrayList<Integer>();
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            JSONArray jArray = jObj.getJSONArray("IntArray");
            for(int i = 0; i < jArray.length(); i++){
                int intValue = jArray.getInt(i);
                list.add(intValue);
            }
        }
        return list;
    }

    public ArrayList<String> getStringArray()  throws JSONException{
        ArrayList<String> list = new ArrayList<String>();
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            JSONArray jArray = jObj.getJSONArray("StringArray");
            for(int i = 0; i < jArray.length(); i++){
                String value = jArray.getString(i);
                list.add(value);
            }
        }
        return list;
    }

    public ArrayList<Boolean> getBoolArray () throws JSONException {
        ArrayList<Boolean> list = new ArrayList<Boolean>();
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            JSONArray jArray = jObj.getJSONArray("BoolArray");
            for(int i = 0; i < jArray.length(); i++){
                boolean value = jArray.getBoolean(i);
                list.add(value);
            }
        }
        return list;
    }

    public ArrayList<Double> getDoubleArray() throws JSONException {
        ArrayList<Double> list = new ArrayList<Double>();
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            JSONArray jArray = jObj.getJSONArray("DoubleArray");
            for(int i = 0; i < jArray.length(); i++){
                double value = jArray.getDouble(i);
                list.add(value);
            }
        }
        return list;
    }

    public ArrayList<Long> getLongArray () throws JSONException{
        ArrayList<Long> list = new ArrayList<Long>();
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            JSONArray jArray = jObj.getJSONArray("LongArray");
            for(int i = 0; i < jArray.length(); i++){
                long value = jArray.getLong(i);
                list.add(value);
            }
        }
        return list;
    }

    public ArrayList<JSONObject> getObjectArray () throws JSONException{
        ArrayList<JSONObject> list = new ArrayList<JSONObject> ();
        JSONObject jObj = getRootJsonObject();
        if(jObj != null) {
            JSONArray jArray = jObj.getJSONArray("objectArray");
            for(int i = 0; i < jArray.length(); i++){
                JSONObject value = jArray.getJSONObject(i);
                list.add(value);
            }
        }
        return list;
    }
}

