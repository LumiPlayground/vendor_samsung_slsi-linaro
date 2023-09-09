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
package com.shannon.qualifiednetworksservice.util;

import java.util.ArrayList;

public class StringUtils {
    public static String bytesToHexString(byte[] data) {
        if (data != null) {
            StringBuilder sb = new StringBuilder();
            for (byte b : data) {
                sb.append(String.format("%02X ", b));
            }

            return sb.toString();
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
}
