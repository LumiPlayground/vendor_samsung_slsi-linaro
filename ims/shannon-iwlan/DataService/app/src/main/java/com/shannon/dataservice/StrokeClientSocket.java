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

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.telephony.Rlog;

import java.io.InputStream;
import java.io.OutputStream;

import com.shannon.dataservice.StrokeRequestMessage.RequestMessageType;

public class StrokeClientSocket {
    private static final boolean DBG = true;
    private static final String FILE_LINK = "/data/vendor/misc/vpn/charon.ctl";
    private static final int SOCKET_CONNECT_TIMEOUT = 1000; // 1 second
    private static final int SOCKET_READ_TIMEOUT = 600000; // 10 minutes
    private static final int SOCKET_READ_BUFFER_SIZE = 1500;

    protected class ResponseThread extends Thread {
        @Override
        public void run() {
            boolean success = false;

            try {
                mSocket.setSoTimeout(SOCKET_READ_TIMEOUT);

                InputStream input = mSocket.getInputStream();
                if (input != null) {
                    byte[] bytes = new byte[SOCKET_READ_BUFFER_SIZE];
                    if (input.read(bytes) > 0) {
                        mCallback.onResponse(bytes);
                        success = true;
                    }
                    input.close();
                }
            } catch (Exception e) {
                loge("Failed to process server data: " + e);
            }

            /* close socket */
            try {
                synchronized (mSocket) {
                    mSocket.close();
                }
            } catch (Exception e) {
                // nothing
            }

            /* failure: notify */
            if (!success) {
                mCallback.onFailure();
            }
        }
    }

    private final String TAG;
    private final StrokeClientSocketCallback mCallback;
    private final LocalSocket mSocket;
    private final ResponseThread mThread;

    StrokeClientSocket(StrokeClientSocketCallback callback) {
        TAG = ShannonDataService.getServiceTAG("-STR-C");

        mCallback = callback;
        mSocket = new LocalSocket();
        mThread = new ResponseThread();
    }

    public boolean send(byte[] bytes) {
        try {
            mSocket.connect(new LocalSocketAddress(FILE_LINK, LocalSocketAddress.Namespace.FILESYSTEM));

            synchronized (mSocket) {
                OutputStream output = mSocket.getOutputStream();
                if (output != null) {
                    mThread.start();
                    output.write(bytes);
                    return true;
                }
            }
        } catch (Exception e) {
            loge("Failed to send client data: " + e);
        }

        /* failure: end thread */
        try {
            if (mThread.isAlive()) {
                mThread.interrupt();
            }
        } catch (Exception e) {
            // nothing
        }
        return false;
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
