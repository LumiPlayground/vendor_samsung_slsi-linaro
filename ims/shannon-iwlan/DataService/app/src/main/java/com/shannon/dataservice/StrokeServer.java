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
package com.shannon.dataservice;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.system.Os;
import android.system.OsConstants;
import android.telephony.Rlog;

import java.io.InputStream;
import java.io.OutputStream;

public class StrokeServer {
    private static final boolean DBG = true;
    private static final String FILE_LINK = "/data/vendor/misc/vpn/charoncomm.ctl";
    private static final int SOCKET_READ_BUFFER_SIZE = 1500;

    public interface SocketDataReceiver {
        public byte[] onDataAvailable(byte[] bytes);
    }

    private class ServerThread extends Thread {
        @Override
        public void run() {
            do {
                try {
                    LocalSocket sk = mSocket.accept();
                    if (sk != null) {
                        InputStream input = sk.getInputStream();
                        if (input != null) {
                            byte[] bytes = new byte[SOCKET_READ_BUFFER_SIZE];
                            if (input.read(bytes) > 0) {
                                byte[] response = mReceiver.onDataAvailable(bytes);
                                if (response != null) {
                                    OutputStream output = sk.getOutputStream();
                                    if (output != null) {
                                        output.write(response);
                                        output.flush();
                                        output.close();
                                    }
                                }
                            }
                            input.close();
                        }
                        sk.close();
                    }
                } catch (Exception e) {
                    loge("Failed to process server data: " + e);
                }
            } while (!mThreadTerminated);
        }
    }

    private final String TAG;
    private final SocketDataReceiver mReceiver;
    private LocalServerSocket mSocket;
    private ServerThread mThread;
    private boolean mThreadTerminated;

    public StrokeServer(SocketDataReceiver receiver) {
        TAG = ShannonDataService.getServiceTAG("-STR-S");

        mThreadTerminated = false;
        mReceiver = receiver;
        mSocket = null;
        try {
            mSocket = new LocalServerSocket(FILE_LINK);
            mThread = new ServerThread();
            mThread.start();
        } catch (Exception e) {
            loge("Failed to open server socket: " + e);
        }
    }

    public void destroy() {
        try {
            mThreadTerminated = true;
            if (mThread.isAlive()) {
                mThread.interrupt();
            }
            Os.shutdown(mSocket.getFileDescriptor(), OsConstants.SHUT_RDWR);
            mSocket.close();
        } catch (Exception e) {
            // nothing
        }
    }

    protected void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }
}
