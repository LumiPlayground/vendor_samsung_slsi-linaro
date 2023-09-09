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

import android.system.ErrnoException;
import android.system.Os;
import android.system.OsConstants;
import android.system.StructTimeval;

import com.shannon.qualifiednetworksservice.util.LogUtils;

import java.io.FileDescriptor;
import java.io.IOException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;

import static android.system.OsConstants.*;

public class IcmpEcho {
    private static final String TAG = "ShannonQNS-Icmp";
    private static int mSeqNum = 0;

    public static long isHostReachable(InetAddress src, InetAddress dst, int length, int timeoutMs) {
        FileDescriptor fd = null;
        try {
            LogUtils.d(TAG, "Ping to " + dst + " from " + src + " with " + length + " bytes. Timeout: " + timeoutMs);

            boolean isIPv4 = dst instanceof Inet4Address;
            int domain = isIPv4 ? AF_INET : AF_INET6;
            int icmpProto = isIPv4 ? IPPROTO_ICMP : IPPROTO_ICMPV6;
            fd = Os.socket(domain, SOCK_DGRAM, icmpProto);
            if (src != null) {
                Os.bind(fd, src, 0);
            }
            StructTimeval tv = StructTimeval.fromMillis(timeoutMs);
            Os.setsockoptTimeval(fd, OsConstants.SOL_SOCKET, OsConstants.SO_RCVTIMEO, tv);

            /* send */
            int seq = mSeqNum++; mSeqNum &= 0xFFFF;
            byte[] packet = IcmpPacket.IcmpEchoRequest(isIPv4, seq, length).getBytes();
            Os.sendto(fd, packet, 0, packet.length, 0, dst, 0);
            InetSocketAddress socketAddress = (InetSocketAddress) Os.getsockname(fd);
            final int icmpId = socketAddress.getPort();

            /* receive */
            byte[] received = new byte[packet.length];
            InetSocketAddress srcAddress = new InetSocketAddress(0);
            int size = Os.recvfrom(fd, received, 0, received.length, 0, srcAddress);
            if (size == packet.length) {
                if (srcAddress.getAddress().equals(dst)
                        && IcmpPacket.isResponse(isIPv4, received[0])
                        && received[4] == (byte) (icmpId >> 8)
                        && received[5] == (byte) icmpId
                        && received[6] == (byte) (seq >> 8)
                        && received[7] == (byte) seq) {
                    // This is the packet we're expecting.
                    long diff = System.currentTimeMillis() - IcmpPacket.getTimestamp(received);
                    LogUtils.d(TAG, "Got ICMP response: " + diff + " ms");
                    return diff;
                }
            }
        } catch (Exception e) {
            LogUtils.e(TAG, "Ping failed: " + e);
        } finally {
            try {
                if (fd != null) {
                    closeAndSignalBlockedThreads(fd);
                }
            } catch (IOException ignored) {
                // nothing
            }
        }
        return timeoutMs;
    }

    private static void closeAndSignalBlockedThreads(FileDescriptor fd) throws IOException {
        if (fd == null || !fd.valid()) {
            return;
        }
        try {
            Os.close(fd);
        } catch (ErrnoException errnoException) {
            throw errnoException.rethrowAsIOException();
        }
    }
}
