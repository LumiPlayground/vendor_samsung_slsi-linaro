package com.samsung.slsi.telephony.JsonRilTest;
//import java.io;
import java.util.ArrayList;
import org.json.*;

public class Main {
    public static void main(String args[]) {
        System.out.println("====main====");
        JsonBuilder builder = new JsonBuilder(123456, "test Json String", true, 123.40, 1234567890);
        try {
            System.out.println("====JsonBuider====");
            String jsonStr = builder.genarateJsonString(4);
            System.out.println(jsonStr);
            JsonParser parser = new JsonParser(builder.genarateJsonString());
            System.out.println("====Json Parser====");
            System.out.println("integer:" + parser.getInteger());
            System.out.println("string:" + parser.getString());
            System.out.println("bool:" + parser.getBoolean());
            System.out.println("double:" + parser.getDouble());
            System.out.println("Long:" + parser.getLong());
            System.out.println("integer array: " + parser.getIntArray());
            System.out.println("string array: " + parser.getStringArray());
            System.out.println("bool array: " + parser.getBoolArray());
            System.out.println("double array: " + parser.getDoubleArray());
            System.out.println("Long array: " + parser.getLongArray());
            ArrayList objList = parser.getObjectArray();
            System.out.println("object array:");
            for(int i = 0; i < objList.size(); i++) {
                JSONObject obj = (JSONObject)objList.get(i);
                System.out.println(obj.toString(4));
            }
        } catch (JSONException e){
            e.printStackTrace();
        }
    }
}
