/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

package com.samsung.slsi.exynos.memlogservice;

import java.util.ArrayList;
import java.io.ObjectInputStream;
import java.io.ByteArrayInputStream;
import vendor.samsung_slsi.hardware.memlogservice.V1_0.IMemlogServiceCallback;
import android.os.AsyncResult;
import android.os.Message;
import android.util.Log;

import com.samsung.slsi.exynos.memlogcontroller.MemlogControllerControlInterface;

class MemlogServiceMemlogdCallback extends IMemlogServiceCallback.Stub {

	private static final String TAG = "MemlogServiceMemlogdCallback";
	MemlogService mMemlogService;


	MemlogServiceMemlogdCallback(MemlogService memlogService)
    {
        Log.i(TAG, "new MemlogServiceMemlogdCallback");
		mMemlogService = memlogService;
	}

	static void sendMessageResponse(Message msg, Object ret) {
        if (msg != null) {
            AsyncResult.forMessage(msg, ret, null);
            msg.sendToTarget();
        }
    }

	@Override
    public void onCallback(int type, int id, ArrayList<Byte> data) {
        //Log.i(TAG, "onCallback() : type=" + type + " id=" + id);
        switch (id) {
            case MemlogServiceConstants.COMMAND_ADD_LIST:
                try {
                    byte[] packet = arrayListToPrimitiveArray(data);
                    byte[] name = new byte[250];
                    //Log.i(TAG, byteArrayToHex(packet));
                    byte length = packet[0];
                    boolean enabled = packet[1] != 0;
                    byte level = packet[2];
                    byte objtype = packet[3];
                    System.arraycopy(packet, 4, name, 0, length - 4);
                    MemlogControllerControlInterface.getInstance().addItem(enabled, level, objtype, new String(name).substring(0, length - 4));
                } catch (Throwable e) {
                    e.printStackTrace();
                }
                break;
            case MemlogServiceConstants.COMMAND_REMOVE_LIST:
                try {
                    byte[] packet = arrayListToPrimitiveArray(data);
                    byte[] name = new byte[250];
                    byte length = packet[0];
                    System.arraycopy(packet, 4, name, 0, length - 4);
                    MemlogControllerControlInterface.getInstance().removeItem(new String(name).substring(0, length - 4));
                } catch (Throwable e) {
                    e.printStackTrace();
                }
            default:
                break;
        }
	}

    public String byteArrayToHex(byte[] a) {
        StringBuilder sb = new StringBuilder();
        for(final byte b: a)
            sb.append(String.format("%02x ", b&0xff));
        return sb.toString();
    }

    public static byte[] arrayListToPrimitiveArray(ArrayList<Byte> bytes) {
        byte[] ret = new byte[bytes.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = bytes.get(i);
        }
        return ret;
    }

}
