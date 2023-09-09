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

import android.telephony.Rlog;

import java.util.LinkedList;

import com.android.internal.annotations.VisibleForTesting;
import com.shannon.dataservice.StrokeRequestMessage.RequestMessageType;
import com.shannon.dataservice.StrokeResponseMessage.ProcessedNotifyType;
import com.shannon.dataservice.StrokeResponseMessage.ResponseErrorStatus;
import com.shannon.dataservice.StrokeResponseMessage.ResponseMessageType;

import static com.shannon.dataservice.TunnelError.ERR_UNKNOWN;

public class TunnelConnection {
    private static final boolean DBG = true;

    protected static final int STATE_INIT = 0;
    protected static final int STATE_ADD = 1;
    protected static final int STATE_OPENING = 2;
    protected static final int STATE_OPENED = 3;
    protected static final int STATE_TERMINATING = 4;
    protected static final int STATE_TERMINATED = 5;

    private int mState;
    private final String TAG;
    private final String mName;
    private final ConnectionParameters mParameters;
    private final IkeDaemonInterface mDaemonInterface;
    private final LinkedList<TunnelConnectionCallback> mCallbackList;
    private int mUsageCount;
    // connection information
    private String mAdapter;
    private int mMtu;
    private String mIpAddress;
    private byte[] mAttributes;
    private byte[] mNotifies;

    TunnelConnection(ConnectionParameters parameters, TunnelConnectionCallback callback) {
        mState = STATE_INIT;
        mName = parameters.mName;
        mParameters = parameters;
        mCallbackList = new LinkedList<>();
        mCallbackList.add(callback);
        mUsageCount = 1;
        mDaemonInterface = IkeDaemonInterface.getInstance();
        TAG = ShannonDataService.getServiceTAG("-" + mName);
    }

    public void addCallback(TunnelConnectionCallback callback) {
        mCallbackList.add(callback); mUsageCount++;
        if (mState == STATE_OPENED) {
            notifyConnected();
        }
    }

    public void connect() {
        mState = STATE_ADD;
        addConnection(mParameters);
    }

    public void disconnect() {
        if (--mUsageCount > 0) {
            log("Connection usage count " + mUsageCount + " != 0");
            return;
        }
        switch (mState) {
            case STATE_OPENED:
                mState = STATE_TERMINATING;
                terminateConnection(mName);
                break;
            case STATE_OPENING:
                mState = STATE_TERMINATING;
                terminateConnection(mName);
            case STATE_ADD:
                mState = STATE_TERMINATED;
                deleteConnection(mName);
        }
    }

    public void close(StrokeTerminatedIndication indication) {
        if (mState != STATE_INIT && mState != STATE_TERMINATED) {
            mState = STATE_TERMINATED;
            deleteConnection(mName);

            notifyTerminated(indication.getCause());
        }
    }

    protected void onAdded(String name) {
        log("onAdded for " + name);

        if (mState == STATE_ADD) {
            mState = STATE_OPENING;
            initiateConnection(mName);
        }
    }

    private void notifyDisconnected() {
        for (TunnelConnectionCallback callback : mCallbackList) {
            callback.onDisconnected(mName);
        }
        mCallbackList.clear();
    }

    private void notifyTerminated(StrokeTerminatedIndication.Cause cause) {
        for (TunnelConnectionCallback callback : mCallbackList) {
            callback.onTerminated(mName, cause);
        }
        mCallbackList.clear();
    }

    private void onAddFailure(String name) {
        loge("onAddFailure for " + name);

        if (mState == STATE_ADD) {
            mState = STATE_TERMINATED;

            mDaemonInterface.deleteTunnel(mName);
            notifyConnectError(ERR_UNKNOWN, null);
        }
    }

    private void notifyConnected() {
        for (TunnelConnectionCallback callback : mCallbackList) {
            callback.onConnected(mName, mAdapter, mMtu, mIpAddress, mAttributes, mNotifies);
        }
    }

    protected void onConnected(String name, String adapter, int mtu, String tunnelIP, byte[] attributes, byte[] notifies) {
        log("onConnected for " + name);

        if (mState == STATE_OPENING) {
            mState = STATE_OPENED;

            mAdapter = adapter;
            mMtu = mtu;
            mIpAddress = tunnelIP;
            mAttributes = attributes;
            mNotifies = notifies;
            notifyConnected();
        }
    }

    private void notifyConnectError(TunnelError error, byte[] notifies) {
        for (TunnelConnectionCallback callback : mCallbackList) {
            callback.onConnectionError(mName, error, notifies);
        }
    }

    protected void onConnectFailure(String name, TunnelError error, byte[] notifies) {
        loge("onConnectFailure for " + name + " code:" + error);

        if (mState == STATE_OPENING) {
            mState = STATE_TERMINATED;

            mDaemonInterface.deleteTunnel(mName);
            deleteConnection(mName);
            notifyConnectError(error, notifies);
        }
    }

    protected void onDisconnected(String name) {
        log("onDisconnected for " + name);

        if (mState == STATE_TERMINATING) {
            mState = STATE_TERMINATED;
            deleteConnection(mName);

            notifyDisconnected();
        }
    }

    private void onDisconnectFailure(String name) {
        loge("onDisconnectFailure for " + name);

        if (mState == STATE_TERMINATING) {
            mState = STATE_TERMINATED;
            deleteConnection(mName);

            notifyDisconnected();
        }
    }

    protected void onDeleted(String name) {
        log("onDeleted for " + name);
    }

    private void onDeleteFailure(String name) {
        log("onDeleteFailure for " + name);
    }

    public byte[] onAuthenticationRequest(byte[] input) {
        TunnelConnectionCallback callback = mCallbackList.getFirst();
        return callback.onAuthenticationRequest(input);
    }

    private void addConnection(ConnectionParameters params) {
        final String name = params.mName;

        StrokeAddRequest request = new StrokeAddRequest(params);
        StrokeClientSocketCallback callback = new StrokeClientSocketCallback(RequestMessageType.STR_REQ_ADD_CONN) {
            @Override
            public void onResponse(byte[] bytes) {
                StrokeResponseMessage message = new StrokeResponseMessage(bytes);
                if (message.getType() == ResponseMessageType.STR_RES_ADD_CONN) {
                    StrokeGenericResponse response = new StrokeGenericResponse(message);
                    log("Received " + response);

                    onAdded(response.getName());
                } else {
                    loge("Unexpected message type received: " + message.getType());
                }
            }

            @Override
            public void onFailure() {
                onAddFailure(name);
            }
        };

        StrokeClientSocket socket = new StrokeClientSocket(callback);
        if (!socket.send(request.getBytes())) {
            onAddFailure(name);
        }
    }

    protected void initiateConnection(final String name) {
        RequestMessageType type = RequestMessageType.STR_REQ_INITIATE;

        StrokeNamedRequest request = new StrokeNamedRequest(type, name);
        StrokeClientSocketCallback callback = new StrokeClientSocketCallback(type) {
            @Override
            public void onResponse(byte[] bytes) {
                StrokeResponseMessage message = new StrokeResponseMessage(bytes);
                if (message.getType() == ResponseMessageType.STR_RES_INITIATE) {
                    StrokeInitiateResponse response = new StrokeInitiateResponse(message);
                    log("Received " + response);

                    ResponseErrorStatus status = response.getStatus();
                    if (status != ResponseErrorStatus.STR_ERROR_SUCCESS) {
                        TunnelError error;

                        ProcessedNotifyType notify = response.getNotify();
                        if (notify != null) {
                            error = TunnelError.getEnum(notify.getCode());
                        } else {
                            error = TunnelError.getEnum(status.getCode());
                        }
                        onConnectFailure(response.getName(), error, response.getNotifies());
                    } else {
                        onConnected(response.getName(), response.getAdapter(), response.getMtu(),
                                response.getIpAddresses(), response.getAttributes(), response.getNotifies());
                    }
                } else {
                    loge("Unexpected message type received: " + message.getType());
                }
            }

            @Override
            public void onFailure() {
                TunnelError error = TunnelError.getEnum(ResponseErrorStatus.STR_ERROR_UNKNOWN.getCode());
                onConnectFailure(name, error, null);
            }
        };

        StrokeClientSocket socket = new StrokeClientSocket(callback);
        if (!socket.send(request.getBytes())) {
            TunnelError error = TunnelError.getEnum(ResponseErrorStatus.STR_ERROR_UNKNOWN.getCode());
            onConnectFailure(name, error, null);
        }
    }

    private void terminateConnection(final String name) {
        RequestMessageType type = RequestMessageType.STR_REQ_TERMINATE;

        StrokeNamedRequest request = new StrokeNamedRequest(type, name);
        StrokeClientSocketCallback callback = new StrokeClientSocketCallback(type) {
            @Override
            public void onResponse(byte[] bytes) {
                StrokeResponseMessage message = new StrokeResponseMessage(bytes);
                if (message.getType() == ResponseMessageType.STR_RES_TERMINATE) {
                    StrokeGenericResponse response = new StrokeGenericResponse(message);
                    log("Received " + response);

                    onDisconnected(response.getName());
                } else {
                    loge("Unexpected message type received: " + message.getType());
                }
            }

            @Override
            public void onFailure() {
                onDisconnectFailure(name);
            }
        };

        StrokeClientSocket socket = new StrokeClientSocket(callback);
        if (!socket.send(request.getBytes())) {
            onDisconnectFailure(name);
        }
    }

    private void deleteConnection(final String name) {
        RequestMessageType type = RequestMessageType.STR_REQ_DEL_CONN;

        StrokeNamedRequest request = new StrokeNamedRequest(type, name);
        StrokeClientSocketCallback callback = new StrokeClientSocketCallback(type) {
            @Override
            public void onResponse(byte[] bytes) {
                StrokeResponseMessage message = new StrokeResponseMessage(bytes);
                if (message.getType() == ResponseMessageType.STR_RES_DEL_CONN) {
                    StrokeGenericResponse response = new StrokeGenericResponse(message);
                    log("Received " + response);

                    onDeleted(response.getName());
                } else {
                    loge("Unexpected message type received: " + message.getType());
                }
            }

            @Override
            public void onFailure() {
                onDeleteFailure(name);
            }
        };

        StrokeClientSocket socket = new StrokeClientSocket(callback);
        if (!socket.send(request.getBytes())) {
            onDeleteFailure(name);
        }
    }

    private void log(String s) {
        if (DBG) {
            Rlog.d(TAG, s);
        }
    }

    private void loge(String s) {
        Rlog.e(TAG, s);
    }

    @VisibleForTesting
    int getState() { return mState; }

    @VisibleForTesting
    void setState(int State) { mState = State; }

    @VisibleForTesting
    LinkedList<TunnelConnectionCallback> getCallbackList() { return mCallbackList; }
}
