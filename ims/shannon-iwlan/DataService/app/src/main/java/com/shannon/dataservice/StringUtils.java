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
package com.shannon.dataservice;

import java.util.ArrayList;
import java.util.Arrays;

public class StringUtils {
    public static String bytesToHexString(byte[] data) {
        if (data != null) {
            StringBuilder sb = new StringBuilder();
            for (byte b : data) {
                sb.append(String.format("%02X ", b));
            }
            return sb.toString().trim();
        }
        return "";
    }

    public static byte[] hexStringToByteArray(String input) {
        int l = input.length() / 2;
        byte[] b = new byte[l];
        for (int i = 0; i < l; i++) {
            Integer tmp = Integer.parseInt(input.substring(i * 2, (i * 2) + 2), 16);
            b[i] = tmp.byteValue();
        }
        return b;
    }

    public static byte[] stringToNativeString(String input, String charset) {
        if (input == null) {
            return null;
        }
        byte[] empty = new byte[1];
        empty[0] = 0;
        if (input.isEmpty()) {
            return empty;
        }
        try {
            return Arrays.copyOf(input.getBytes(charset), input.length() + 1);
        } catch (Exception e) {
            return empty;
        }
    }

    public static byte[] stringToTBCD(String input) {
        int stringLength = input.length();
        int length = (stringLength / 2) + (stringLength & 1);
        byte[] res = new byte[length];
        for (int i = 0; i < length; i++) {
            int high = 0x0F;
            int low = Integer.parseInt(input.substring(i * 2, i * 2 + 1), 16);
            if (--stringLength > 0) {
                high = Integer.parseInt(input.substring(i * 2 + 1, i * 2 + 2), 16);
                stringLength--;
            }
            res[i] = (byte)((((high & 0x0F) << 4) | (low & 0x0F)) & 0xFF);
        }
        return res;
    }

    public static ArrayList<Byte> primitiveArrayToArrayList(byte[] data) {
        ArrayList<Byte> arrayList = new ArrayList<>(data.length);
        for (byte b : data) {
            arrayList.add(b);
        }
        return arrayList;
    }

    public static byte[] arrayListToPrimitiveArray(ArrayList<Byte> data) {
        byte[] ret = new byte[data.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = data.get(i);
        }
        return ret;
    }

    public static int getByteAsInt(byte[] data, int offset) {
        return (data[offset] & 0xFF);
    }

    public static int getShortAsInt(byte[] data, int offset) {
        return (((getByteAsInt(data, offset) << 8) | getByteAsInt(data, offset + 1)) & 0xFFFF);
    }

    public static int getInt(byte[] data, int offset) {
        return (getByteAsInt(data, offset) << 24) | (getByteAsInt(data,offset + 1) << 16) |
                (getByteAsInt(data, offset + 2) << 8) | getByteAsInt(data, offset + 3);
    }
}
