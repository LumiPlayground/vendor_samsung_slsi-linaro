/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.qualifiednetworksservice.util.ping;

import java.nio.ByteBuffer;

import static android.system.OsConstants.ICMP6_ECHO_REQUEST;
import static android.system.OsConstants.ICMP6_ECHO_REPLY;
import static android.system.OsConstants.ICMP_ECHO;
import static android.system.OsConstants.ICMP_ECHOREPLY;

public class IcmpPacket {
    private byte[] bytes;

    private IcmpPacket(int size) {
        if (size < Long.BYTES) {
            size = Long.BYTES;
        }
        bytes =  new byte[8 + size];
    }

    /*
     * Echo or Echo Reply Message
     *
     * 0                   1                   2                   3
     * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |     Type      |     Code      |          Checksum             |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |           Identifier          |        Sequence Number        |
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |     Data ...
     * +-+-+-+-+-
     */
    public static IcmpPacket IcmpEchoRequest(boolean ipv4, int seq, int size) {
        IcmpPacket p = new IcmpPacket(size);
        p.bytes[0] = ipv4 ? (byte) ICMP_ECHO : (byte) ICMP6_ECHO_REQUEST;
        // bytes[1]: Code is always zero.
        // bytes[2,3]: Checksum is computed by kernel.
        // bytes[4,5]: ID (= port) inserted by kernel.
        p.bytes[6] = (byte) (seq >> 8);
        p.bytes[7] = (byte) seq;
        /* current timestamp */
        ByteBuffer buf = ByteBuffer.wrap(p.bytes, 8, Long.BYTES);
        buf.putLong(System.currentTimeMillis());
        return p;
    }

    public static boolean isResponse(boolean ipv4, byte type) {
        byte expectedType = ipv4 ? (byte) ICMP_ECHOREPLY : (byte) ICMP6_ECHO_REPLY;
        return (type == expectedType);
    }

    public static long getTimestamp(byte[] bytes) {
        ByteBuffer buf = ByteBuffer.wrap(bytes, 8, Long.BYTES);
        return buf.getLong();
    }

    public byte[] getBytes() {
        return bytes.clone();
    }
}
