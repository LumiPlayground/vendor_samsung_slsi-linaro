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

import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.os.Bundle;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import android.app.Activity;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.text.method.ScrollingMovementMethod;

import com.samsung.slsi.telephony.oem.io.DataReader;
import com.samsung.slsi.telephony.oem.io.DataWriter;
import com.samsung.slsi.telephony.oem.OemRil;
import com.samsung.slsi.telephony.oem.OemRilConstants;
import com.samsung.slsi.telephony.oem.util.StringUtil;

import com.samsung.slsi.telephony.testmode.R;


import org.json.*;

public class JsonRilTestActivity extends Activity {

    private static final String TAG = "JsonRilTestActivity";
    private static final int EVENT_RIL_CONNECTED = 100;
    private static final int EVENT_RIL_DISCONNECTED = 101;
    private static final int EVENT_RIL_LOOPBACK_DATA_RECIVED = 100;
    private static final int EVENT_SEND_RIL_DATA_DONE = 103;
    private static final int EVENT_CP_RESPONSE_TIMEOUT = 104;
    private static final int EVENT_TITLE_CHANGED = 105;

    /* RIL request */
    private static final int RILC_REQ_MISC_SEND_TEST_JSON_DATA = 811;    // Engineer Mode Set Command

    private LinearLayout mLLayInput;
    private LinearLayout mLLayDisplay;
    private EditText mEditJsonInt;
    private EditText mEditJsonString;
    private EditText mEditJsonBool;
    private EditText mEditJsonDouble;
    private EditText mEditJsonLong;

    private Button mBtnGenerate;


    private TextView mTextViewJson;
    private TextView mTextViewJsonHumanReadable;
    private Button mBtnSend;

    private OemRil mOemRil;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_SEND_RIL_DATA_DONE: {
                    Log.d(TAG, "EVENT_SEND_RIL_DATA_DONE");
                    break;
                }
                case EVENT_RIL_LOOPBACK_DATA_RECIVED: {
                    Log.d(TAG, "Receive Json data from RIL");
                    AsyncResult ar = (AsyncResult) msg.obj;
                    byte[] data = (byte[])ar.result;
                    if(data != null) {
                        displayLoopbackData(new String(data));
                    } else {
                        displayLoopbackData("{integer:321,\"string\":\"hello\"}");
                    }
                    break;
                }
                default:
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle saveInstance) {
        super.onCreate(saveInstance);
        setContentView(R.layout.jsonriltest_activity);
        this.setTitle("JSON RIL loopback test");
        mEditJsonInt = (EditText) findViewById(R.id.edit_json_integer);
        mEditJsonString = (EditText) findViewById(R.id.edit_json_string);
        mEditJsonBool = (EditText) findViewById(R.id.edit_json_bool);
        mEditJsonDouble = (EditText) findViewById(R.id.edit_json_double);
        mEditJsonLong = (EditText) findViewById(R.id.edit_json_long);
        mBtnGenerate = (Button) findViewById(R.id.btn);
        mLLayInput = (LinearLayout) findViewById(R.id.llay_input);
        mLLayDisplay = (LinearLayout) findViewById(R.id.llay_display);
        mTextViewJson = (TextView) findViewById(R.id.text_view_json);
        mTextViewJsonHumanReadable  = (TextView) findViewById(R.id.text_view_json_human);;
        mBtnSend = (Button) findViewById(R.id.btn_send);
        mBtnGenerate.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                clickGenerateButton();
                InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(mBtnGenerate.getWindowToken(), 0);
            }
        });

        connectToOemRilService();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mOemRil != null) {
            mOemRil.unregisterDisplayEng(mHandler);
            mOemRil.detach();
        }
    }

    private void connectToOemRilService() {
        ArrayList<Integer> filters = new ArrayList();
        filters.add(OemRilConstants.RILC_REQ_MISC_SEND_TEST_JSON_DATA);
        mOemRil = OemRil.init(getApplicationContext(), 0, "JOSN", filters); // default SIM1
        if (mOemRil == null) {
            Log.d(TAG, "connectToOemRilService mOemRil is null");
        } else {
            mOemRil.registerDisplayEng(mHandler, RILC_REQ_MISC_SEND_TEST_JSON_DATA, null);
        }
    }

    private void sendRilData(int requestId, String data) {
        DataWriter dr = new DataWriter();
        try {
            dr.writeBytes(data.getBytes());
        } catch (IOException e) {
            Log.i(TAG, "SendData() IOException" + e);
        }
        mOemRil.invokeRequestRaw(requestId, dr.toByteArray(), mHandler.obtainMessage(EVENT_RIL_LOOPBACK_DATA_RECIVED));
    }

    public boolean isNumeric(String str){
        Pattern pattern = Pattern.compile("[0-9]+");
        Matcher isNum = pattern.matcher(str);
        if( !isNum.matches() ){
            return false;
        }
        return true;
    }

    public boolean isDouble(String str){
        Pattern pattern = Pattern.compile("[0-9]+.{0,1}[0-9]+");
        Matcher isNum = pattern.matcher(str);
        if( !isNum.matches() ){
            return false;
        }
        return true;
    }

    public boolean getBoolean(String str){
        if(str == null || str.startsWith("t") || str.startsWith("T") || str.startsWith("1")){
            return true;
        }
        return false;
    }

    public String getJsonString(int intValue, String str, boolean boolValue,
            double doubleValue, long longValue){
          JsonBuilder builder = new JsonBuilder(intValue, str, boolValue, doubleValue, longValue);
          return builder.genarateJsonString();
    }

    public String getHumanEasyReadJsonString(int intValue, String str, boolean boolValue,
            double doubleValue, long longValue) {
        JsonBuilder builder = new JsonBuilder(intValue, str, boolValue, doubleValue, longValue);
        return builder.genarateJsonString(4);
    }

    public void clickGenerateButton() {
        int intValue = 1234;
        long longValue = 0xFFFFFFF;
        boolean boolValue = false;
        double doubleValue = 4321.12;

        String tmpStr = mEditJsonInt.getText().toString();
        if(isNumeric(tmpStr)) {
            intValue = new Integer(tmpStr);
        }
        String strValue = mEditJsonString.getText().toString();

        tmpStr = mEditJsonBool.getText().toString();
        boolValue = getBoolean(tmpStr);

        tmpStr = mEditJsonDouble.getText().toString();
        if(isDouble(tmpStr)) {
            doubleValue = new Double(tmpStr);
        }

        tmpStr = mEditJsonLong.getText().toString();
        if(isNumeric(tmpStr)) {
            longValue = new Long(tmpStr);
        }

        String jsonStr = getJsonString(intValue, strValue, boolValue, doubleValue, longValue);

        String jsonStrHuman = getHumanEasyReadJsonString(intValue, strValue, boolValue, doubleValue, longValue);

        mLLayInput.setVisibility(View.GONE);

        mTextViewJson.setText(jsonStr);
        mTextViewJson.setMovementMethod(ScrollingMovementMethod.getInstance());
        mTextViewJsonHumanReadable.setText(jsonStrHuman);
        mTextViewJsonHumanReadable.setMovementMethod(ScrollingMovementMethod.getInstance());

        mBtnSend.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                sendRilData(RILC_REQ_MISC_SEND_TEST_JSON_DATA, jsonStr);
            }
        });

        mLLayDisplay.setVisibility(View.VISIBLE);
    }

    public void displayLoopbackData(String jsonData) {
        TextView textViewHint1 = (TextView) findViewById(R.id.text_view_hint1);
        TextView textViewHint2 = (TextView) findViewById(R.id.text_view_hint2);
        textViewHint1.setText("recieved data from RIL");
        mTextViewJson.setText(jsonData);
        mTextViewJson.setMovementMethod(ScrollingMovementMethod.getInstance());

        mTextViewJsonHumanReadable.setVisibility(View.GONE);
        textViewHint2.setVisibility(View.GONE);
        mBtnSend.setText("Parse Json");

        mBtnSend.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                String parsedStr = "";
                JsonParser parser = new JsonParser(jsonData);
                try {
                    parsedStr = parsedStr + "Integer:" + parser.getInteger();
                    parsedStr += "\n";
                    parsedStr = parsedStr + "String:" + parser.getString();
                    parsedStr += "\n";
                    parsedStr = parsedStr + "Bool:" + parser.getBoolean();
                    parsedStr += "\n";
                    parsedStr = parsedStr + "Double:" + parser.getDouble();
                    parsedStr += "\n";
                    parsedStr = parsedStr + "Long:" + parser.getLong();
                    parsedStr += "\n";
                    parsedStr = parsedStr + "String Array:" +
                         parser.getStringArray().toString();
                    parsedStr += "\n===========\n";
                    parsedStr = parsedStr + "object Array:\n";
                    ArrayList<JSONObject> objList = parser.getObjectArray();
                    for(int i = 0; i < objList.size(); i++) {
                        parsedStr = parsedStr + "    object[" + i + "]:" +
                             objList.get(i).toString() + "\n";
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                textViewHint2.setText("Parsed value");
                mTextViewJsonHumanReadable.setText(parsedStr);
                textViewHint2.setVisibility(View.VISIBLE);
                mTextViewJsonHumanReadable.setVisibility(View.VISIBLE);
                mBtnSend.setText("return");
                mBtnSend.setOnClickListener(new OnClickListener() {
                    @Override
                    public void onClick(View arg0) {
                        onCreate(null);
                    }
                });
            }
        });

        mLLayDisplay.setVisibility(View.VISIBLE);
    }
}
