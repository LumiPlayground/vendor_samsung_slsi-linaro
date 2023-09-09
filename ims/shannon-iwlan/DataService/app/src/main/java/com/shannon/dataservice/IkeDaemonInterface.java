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

import android.content.Context;
import android.os.SystemProperties;
import android.telephony.Rlog;

import java.net.InetAddress;
import java.util.HashMap;
import java.util.LinkedList;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.dataservice.StrokeAuthenticationResponse.AuthenticationStatus;
import com.shannon.dataservice.StrokeRequestMessage.RequestMessageType;

public class IkeDaemonInterface {
    private static final boolean DBG = true;
    private static final String DAEMON_EXEC = "vendor.charon.exec";
    private static final String DAEMON_STATUS = "vendor.charon.status";
    private static final int REQUEST_ID_START = 21;

    private static IkeDaemonInterface sMe = null;

    protected class DataReceiver implements StrokeServer.SocketDataReceiver {
        public byte[] onDataAvailable(byte[] bytes) {
            if (bytes == null || bytes.length == 0) {
                return null;
            }

            StrokeResponseMessage message = new StrokeResponseMessage(bytes);
            switch (message.getType()) {
                case STR_IND_SIM_AUTH: {
                    return getAuthenticationResponse(message);
                }
                case STR_IND_TERMINATED: {
                    onTunnelTerminatedFromNetwork(message);
                    break;
                }
                default:
                    log("Unsupported message received: " + message.getType());
            }
            return null;
        }
    }

    private class RouteEntry {
        private final InetAddress mSource;
        private final InetAddress mDestination;
        private final String mInterface;
        private int mUsageCount;

        RouteEntry(InetAddress src, InetAddress dst, String name) {
            mSource = src;
            mDestination = dst;
            mInterface = name;
            mUsageCount = 1;
        }

        void add() {
            synchronized (mRoutes) {
                for (RouteEntry entry : mRoutes) {
                    if (equals(entry)) {
                        entry.mUsageCount++;
                        log("Exists " + entry + " count=" + entry.mUsageCount);
                        return;
                    }
                }
                mRoutes.add(this);
            }
            log("Create " + this);
            addRoute(mSource, mDestination, mInterface);
        }

        void delete() {
            if (--mUsageCount == 0) {
                synchronized (mRoutes) {
                    mRoutes.remove(this);
                }
                log("Delete " + this);
                deleteRoute(mSource, mDestination, mInterface);
            } else {
                log("Delete " + this + " count=" + mUsageCount);
            }
        }

        @Override
        public boolean equals(Object obj) {
            if (obj instanceof RouteEntry) {
                RouteEntry entry = (RouteEntry) obj;
                return (mSource.equals(entry.mSource) &&
                        mDestination.equals(entry.mDestination) &&
                        mInterface.equals(entry.mInterface));
            }
            return false;
        }

        @Override
        public String toString() {
            return ("route to " + mDestination + " from " + mSource + " on " + mInterface);
        }
    }

    private final String TAG;
    private boolean mIsDaemonStarted;
    private HashMap<String, TunnelConnection> mConnections;
    private int mRequestId;
    private StrokeServer mServer;
    private final LinkedList<RouteEntry> mRoutes;

    public static IkeDaemonInterface getInstance() { return  sMe; };

    IkeDaemonInterface(Context context) {
        mRequestId = REQUEST_ID_START;
        mConnections = new HashMap<>();
        mRoutes = new LinkedList<>();

        TAG = ShannonDataService.getServiceTAG("-IKE");

        sMe = this;

        stopIkeDaemon();

        StrokeAddRequest msg = new StrokeAddRequest(new ConnectionParameters());
        msg.setMinimumLength();

        mServer = new StrokeServer(new DataReceiver());
    }

    public void destroy() {
        mServer.destroy();
        stopIkeDaemon();
    }

    public void openTunnel(ConnectionParameters parameters, TunnelConnectionCallback callback) {
        startIkeDaemon();

        TunnelConnection c = mConnections.get(parameters.mName);
        if (c != null) {
            /* rare case, reuse same tunnel */
            loge("Connection with same name found (" + parameters.mName + ")");
            c.addCallback(callback);
            return;
        }
        parameters.mRequestId = mRequestId++;

        log("openTunnel for " + parameters.mName);
        c = new TunnelConnection(parameters, callback);
        mConnections.put(parameters.mName, c);
        c.connect();
    }

    public void closeTunnel(String name) {
        log("closeTunnel for " + name);
        TunnelConnection c = deleteTunnel(name);
        if (c != null) {
            c.disconnect();
        }
    }

    public TunnelConnection deleteTunnel(String name) {
        TunnelConnection c = mConnections.remove(name);
        if (c != null) {
            --mRequestId;
        }
        return c;
    }

    public void createRoute(ConnectionParameters parameters) {
        RouteEntry entry = new RouteEntry(parameters.mLocalAddress, parameters.mRemoteAddress,
                parameters.mInterface);
        entry.add();
    }

    public void deleteRoute(ConnectionParameters parameters) {
        boolean found = false;

        RouteEntry entry = new RouteEntry(parameters.mLocalAddress, parameters.mRemoteAddress,
                parameters.mInterface);
        synchronized (mRoutes) {
            for (RouteEntry loopEntry : mRoutes) {
                if (entry.equals(loopEntry)) {
                    entry = loopEntry;
                    found = true;
                    break;
                }
            }
        }
        if (found) {
            entry.delete();
        } else {
            log("deleteRoute not found for " + entry);
        }
    }

    private void addRoute(InetAddress src, InetAddress dst, String name) {
        RequestMessageType type = RequestMessageType.STR_REQ_ADD_ROUTE;
        StrokeRouteRequest request = new StrokeRouteRequest(type, src.getHostAddress(), dst.getHostAddress(), name);
        StrokeClientSocket socket = new StrokeClientSocket(new StrokeClientSocketCallback(type));
        socket.send(request.getBytes());
    }

    private void deleteRoute(InetAddress src, InetAddress dst, String name) {
        RequestMessageType type = RequestMessageType.STR_REQ_DEL_ROUTE;
        StrokeRouteRequest request = new StrokeRouteRequest(type, src.getHostAddress(), dst.getHostAddress(), name);
        StrokeClientSocket socket = new StrokeClientSocket(new StrokeClientSocketCallback(type));
        socket.send(request.getBytes());
    }

    public void setNewInterface(String name, String interfaceName) {
        log("setNewInterface for " + name + " to " + interfaceName);

        StrokeSetInterfaceRequest request = new StrokeSetInterfaceRequest(name, interfaceName);
        StrokeClientSocket socket = new StrokeClientSocket(new StrokeClientSocketCallback(RequestMessageType.STR_REQ_SET_INTERFACE));
        socket.send(request.getBytes());
    }

    public void startIkeDaemon() {
        if (!mIsDaemonStarted) {
            try {
                SystemProperties.set(DAEMON_EXEC, "1");
                mIsDaemonStarted = true;
                log("IKE daemon started");
            } catch (Exception e) {
                loge("Failed to start daemon: " + e);
            }
        }
    }

    public void stopIkeDaemon() {
        try {
            SystemProperties.set(DAEMON_EXEC, "0");
            SystemProperties.set(DAEMON_STATUS, "0");
            mIsDaemonStarted = false;
            log("IKE daemon stopped");
        } catch (Exception e) {
            loge("Failed to stop daemon: " + e);
        }
    }

    public boolean isIkeDaemonStarted() {
        try {
            String value = SystemProperties.get(DAEMON_STATUS, "0");
            return ("1".equalsIgnoreCase(value));
        } catch (Exception e) {
            return false;
        }
    }

    /*
    * Server callbacks
    */
    private byte[] getAuthenticationResponse(StrokeResponseMessage message) {
        StrokeAuthenticationIndication indication = new StrokeAuthenticationIndication(message);
        log("Received " + indication);

        TunnelConnection c = mConnections.get(indication.getName());
        if (c != null) {
            StrokeAuthenticationResponse response;

            byte[] bytes = c.onAuthenticationRequest(indication.getNonce());
            if (bytes != null && bytes.length > 0) {
                byte[] CK = null;
                byte[] IK = null;
                byte[] RES = null;

                AuthenticationStatus status = AuthenticationStatus.getEnum(bytes[0] & 0xFF);

                int RES_len = bytes[1] & 0xFF;
                if (RES_len > 0) {
                    RES = new byte[RES_len];
                    System.arraycopy(bytes, 2, RES, 0, RES_len);
                }
                if (status == AuthenticationStatus.AUTH_STATUS_SUCCESS) {
                    int CK_len = bytes[2 + RES_len] & 0xFF;
                    CK = new byte[CK_len];
                    System.arraycopy(bytes, 3 + RES_len, CK, 0, CK_len);

                    int IK_len = bytes[3 + RES_len + CK_len] & 0xFF;
                    IK = new byte[IK_len];
                    System.arraycopy(bytes, 4 + RES_len + CK_len, IK, 0, IK_len);
                }
                response = new StrokeAuthenticationResponse(status, CK, IK, RES);
            } else {
                response = new StrokeAuthenticationResponse(AuthenticationStatus.AUTH_STATUS_ERROR, null, null, null);
            }
            return response.getBytes();
        }
        return null;
    }

    private void onTunnelTerminatedFromNetwork(StrokeResponseMessage message) {
        StrokeTerminatedIndication indication = new StrokeTerminatedIndication(message);
        log("Received " + indication);

        TunnelConnection c = deleteTunnel(indication.getName());
        if (c != null) {
            c.close(indication);
        }
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    protected void loge(String s) {
        Rlog.e(TAG, s);
    }

    @VisibleForTesting
    boolean getIsDaemonStarted() { return mIsDaemonStarted; }

    @VisibleForTesting
    int getRequestId() { return mRequestId; }

    @VisibleForTesting
    LinkedList<RouteEntry> getRoutes() { return mRoutes; }
}
