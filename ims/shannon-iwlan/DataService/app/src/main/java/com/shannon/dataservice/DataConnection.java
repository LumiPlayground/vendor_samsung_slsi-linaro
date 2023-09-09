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
import android.net.ConnectivityManager;
import android.net.IpPrefix;
import android.net.IpSecManager;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.Network;
import android.net.RouteInfo;
import android.net.SocketKeepalive;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Message;
import android.system.Os;
import android.telephony.DataFailCause;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.telephony.data.ApnSetting;
import android.telephony.data.DataCallResponse;
import android.telephony.data.DataProfile;
import android.telephony.data.NetworkSliceInfo;

import android.text.TextUtils;

import java.io.FileDescriptor;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.NetworkInterface;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.Executors;

import static android.system.OsConstants.F_SETFL;
import static android.system.OsConstants.SOCK_DGRAM;
import static android.system.OsConstants.SOCK_NONBLOCK;
import static android.telephony.data.ApnSetting.PROTOCOL_IP;
import static android.telephony.data.ApnSetting.PROTOCOL_IPV4V6;
import static android.telephony.data.ApnSetting.PROTOCOL_IPV6;
import static android.telephony.data.DataService.REQUEST_REASON_HANDOVER;

import com.android.internal.annotations.VisibleForTesting;

public class DataConnection {
    private static final boolean DBG = true;
    private static final String DEFAULT_MAC_ADDRESS = "02-00-00-00-00-00";

    public interface ConnectionCallback {
        public void onConnectionCompleted(ApnType type, Message message, DataCallResponse response);
        public void onConnectionTerminated(ApnType type, Message message);
        public void onConnectionTerminated(ApnType type, Message message, StrokeTerminatedIndication.Cause cause);
    }

    protected class DaemonCallback implements TunnelConnectionCallback {
        public void onConnected(String connectionName, String adapterName, int mtu, String tunnelIP, byte[] attributes, byte[] notifies) {
            log("onConnected " + connectionName + " on " + adapterName + " with mtu " + mtu + " IP:" + tunnelIP);
            try {
                mRetryHandler.reset();

                mIkeConfiguration.processReceivedAttributes(attributes);
                mNofifyProcessor.processReceivedNotifies(notifies);

                if (mtu < 1280) {
                    mtu = 1280;
                }
                /* check for handover */
                if (mLinkProperties != null) {
                    mLinkProperties.setInterfaceName(adapterName);
                    mLinkProperties.setMtu(mtu);
                } else {
                    mLinkProperties = new LinkProperties();
                    mLinkProperties.setInterfaceName(adapterName);
                    mLinkProperties.setMtu(mtu);

                    boolean hasIPv4 = false;
                    boolean hasIPv6 = false;

                    String[] sa = tunnelIP.split(",");
                    for (String s : sa) {
                        InetAddress a = InetAddress.getByName(s);
                        if (a instanceof Inet4Address) {
                            hasIPv4 = true;

                            LinkAddress la = new LinkAddress(a, 32);
                            mLinkProperties.addLinkAddress(la);
                        } else if (a instanceof Inet6Address) {
                            hasIPv6 = true;

                            LinkAddress la = new LinkAddress(a, 64);
                            mLinkProperties.addLinkAddress(la);
                        }
                    }
                    if (hasIPv4) {
                        InetAddress gateway = Inet4Address.getByName("0.0.0.0");
                        RouteInfo route = new RouteInfo(
                                (IpPrefix) null, gateway, null, RouteInfo.RTN_UNICAST);
                        mLinkProperties.addRoute(route);
                    }
                    if (hasIPv6) {
                        InetAddress gateway = new InetSocketAddress(0).getAddress();
                        RouteInfo route = new RouteInfo(
                                (IpPrefix) null, gateway, null, RouteInfo.RTN_UNICAST);
                        mLinkProperties.addRoute(route);
                    }
                    /* IPv4 P-CSCF */
                    LinkedList<IkeIpConfigurationAttribute> list =
                            mIkeConfiguration.getIpConfigurationAttributes(mConfigurationParameters.mIkeAttrPcscf4);
                    for (IkeIpConfigurationAttribute attribute : list) {
                        InetAddress address = attribute.getIp();
                        if (address != null) {
                            mLinkProperties.addPcscfServer(address);
                        }
                    }
                    /* IPv6 P-CSCF */
                    list = mIkeConfiguration.getIpConfigurationAttributes(mConfigurationParameters.mIkeAttrPcscf6);
                    for (IkeIpConfigurationAttribute attribute : list) {
                        InetAddress address = attribute.getIp();
                        if (address != null) {
                            mLinkProperties.addPcscfServer(address);
                        }
                    }
                    /* DNSv4 */
                    list = mIkeConfiguration.getIpConfigurationAttributes(IkeConfiguration.INTERNAL_IP4_DNS);
                    for (IkeIpConfigurationAttribute attribute : list) {
                        InetAddress address = attribute.getIp();
                        if (address != null) {
                            mLinkProperties.addDnsServer(address);
                        }
                    }
                    /* DNSv6 */
                    list = mIkeConfiguration.getIpConfigurationAttributes(IkeConfiguration.INTERNAL_IP6_DNS);
                    for (IkeIpConfigurationAttribute attribute : list) {
                        InetAddress address = attribute.getIp();
                        if (address != null) {
                            mLinkProperties.addDnsServer(address);
                        }
                    }
                }
                IkeNotifyN1ModeInformation n1info = mNofifyProcessor.getN1ModeInformation();
                if (n1info != null) {
                    NetworkSliceInfo sliceInfo = n1info.getSliceInfo();
                    if ((sliceInfo != null) && !sliceInfo.equals(mSliceInfo)) {
                        log("New S-NSSAI received. " + sliceInfo);
                        mSliceInfo = sliceInfo;
                    }
                }
                if (isUseOffloadedKeepalive()) {
                    startOffloadedKeepAlive();
                }

                Message message = mRequestMessage;
                mRequestMessage = null;

                DataCallResponse r = getDataCallResponse();
                mConnectionCallback.onConnectionCompleted(mApnType, message, r);
            } catch (Exception e) {
                loge("Failed to process connection response");
            }
        }

        public void onConnectionError(String connectionName, TunnelError error, byte[] notifies) {
            log("onConnectionError " + connectionName + " error: " + error);
            mDaemonInterface.deleteRoute(mConnectionParameters);
            mRetryHandler.removeAddress(error, mConnectionParameters.mRemoteAddress);

            mNofifyProcessor.processReceivedNotifies(notifies);

            DataCallResponse r;
            int timerValue = 0;
            int cause = error.toDataFailCause();

            IkeNotifyBackoffTimer timer = mNofifyProcessor.getBackoffTimer();
            if (timer.isDeactivatedOrInvalid()) {
                InetAddress nextRemoteAddress = mRetryHandler.getNextAddress(mLocalAddresses);
                if (nextRemoteAddress != null) {
                    InetAddress localIP;
                    if (nextRemoteAddress instanceof Inet4Address) {
                        localIP = getLocalIpv4();
                    } else {
                        localIP = getLocalIpv6();
                    }
                    mConnectionParameters.mLocalAddress = localIP;
                    mConnectionParameters.mMeAddress = localIP.getHostAddress();
                    mConnectionParameters.mRemoteAddress = nextRemoteAddress;
                    mConnectionParameters.mOtherAddress = nextRemoteAddress.getHostAddress();
                    mDaemonInterface.createRoute(mConnectionParameters);

                    log("Retry data connection with " + mConnectionParameters);
                    mDaemonInterface.openTunnel(mConnectionParameters, mDaemonCallback);
                    return;
                }
            } else {
                timerValue = timer.getTimerValue();
            }
            r = createFailureResponse(cause, timerValue);
            mRetryHandler.clearAddresses();

            Message message = mRequestMessage;
            mRequestMessage = null;

            log("Response: " + r);
            mConnectionCallback.onConnectionCompleted(mApnType, message, r);
        }

        public void onDisconnected(String connectionName) {
            stopOffloadedKeepAlive();
            mDaemonInterface.deleteRoute(mConnectionParameters);

            Message message = mRequestMessage;
            mRequestMessage = null;

            log("onDisconnected " + connectionName);
            mConnectionCallback.onConnectionTerminated(mApnType, message);
        }

        public void onTerminated(String connectionName, StrokeTerminatedIndication.Cause cause) {
            stopOffloadedKeepAlive();
            mDaemonInterface.deleteRoute(mConnectionParameters);

            Message message = mRequestMessage;
            mRequestMessage = null;

            log("onTerminated " + connectionName + " cause: " + cause);
            mConnectionCallback.onConnectionTerminated(mApnType, message, cause);
        }

        public byte[] onAuthenticationRequest(byte[] input) {
            boolean isUsim = mConfigurationParameters.mUseUsim || !mSimInformation.isIsimCard();
            return mRilInterface.getAuthenticationResponse(input, isUsim);
        }
    }

    private final String TAG;
    private final int mSlotId;
    private final ApnType mApnType;
    private final int mApnBitmask;
    private final WifiManager mWiFiManager;
    private final IkeDaemonInterface mDaemonInterface;
    private final RilOemInterface mRilInterface;
    private final SimInformation mSimInformation;
    private final ConnectionCallback mConnectionCallback;
    private final ConfigurationManager mConfigurationManager;
    private final ConfigurationParameters mConfigurationParameters;
    private final int mProtocol;
    private final IkeConfiguration mIkeConfiguration;
    private final IkeNotifyProcessor mNofifyProcessor;
    private final RetryHandler mRetryHandler;
    private ConnectionParameters mConnectionParameters;
    private LinkProperties mLinkProperties;
    private String mLastBssid;
    private ArrayList<InetAddress> mLocalAddresses;
    private Message mRequestMessage;
    private DaemonCallback mDaemonCallback;
    /* for keep-alive offloading */
    private final ConnectivityManager mConnectivityManager;
    private final IpSecManager mIpSecManager;
    private Network mNetwork;
    private IpSecManager.UdpEncapsulationSocket mUdpSocket;
    private SocketKeepalive mKeepAliveSocket;
    private int mKeepAliveInterval;
    /* session ID */
    private int mPduSessionId;
    private NetworkSliceInfo mSliceInfo;

    public DataConnection(int slotId, Context context, ApnType type, DataProfile dataProfile, int reason,
                          LinkProperties linkProperties, int pduSessionId, NetworkSliceInfo sliceInfo, boolean isFirstConnection,
                          RilOemInterface rilInterface, ConfigurationManager configurationManager, RetryManager retryManager,
                          ConnectionCallback callback) {
        mSlotId = slotId;
        mApnType = type;
        mApnBitmask = dataProfile.getSupportedApnTypesBitmask();
        mConnectionCallback = callback;
        mRilInterface = rilInterface;
        mConfigurationManager = configurationManager;
        mSimInformation = SimManager.getInstance().getSimInformation(mSlotId);
        mWiFiManager = context.getSystemService(WifiManager.class);
        mConnectivityManager = context.getSystemService(ConnectivityManager.class);
        mIpSecManager = context.getSystemService(IpSecManager.class);
        mDaemonInterface = IkeDaemonInterface.getInstance();
        mLocalAddresses = new ArrayList<>();
        mRetryHandler = retryManager.getApnRetryHandler(mApnType, dataProfile.getApn());

        mRequestMessage = null;
        mLastBssid = null;
        mDaemonCallback = new DaemonCallback();
        TelephonyManager tm = mSimInformation.getTelephonyManager();

        TAG = ShannonDataService.getServiceTAG("-DC-" + mApnType.getUserFriendlyName() + "-" + mSlotId);
        mIkeConfiguration = new IkeConfiguration(TAG);
        mNofifyProcessor = new IkeNotifyProcessor(TAG);

        mDaemonInterface.startIkeDaemon();

        mKeepAliveSocket = null;
        mUdpSocket = null;
        mKeepAliveInterval = 0;

        int mcc = mSimInformation.getMcc();
        int mnc = mSimInformation.getMnc();
        mConfigurationParameters = mConfigurationManager.getConnectionConfiguration();

        String apnName = getApnName(dataProfile);
        log("Create connection for " + apnName + mSimInformation.getSubId());

        mConnectionParameters = new ConnectionParameters(mConfigurationParameters);
        mConnectionParameters.setIkeConfiguration(mIkeConfiguration);
        mConnectionParameters.setIkeNotifyProcessor(mNofifyProcessor);
        mConnectionParameters.mIsFirstConnection = isFirstConnection;
        /* DEVICE ID */
        mConnectionParameters.mName = apnName + mSimInformation.getSubId();
        String str1 = "0" + mSimInformation.getImsi() + "@";
        String str2 = "";
        if (mConfigurationParameters.mUseMacForNai) {
            String sMac = DEFAULT_MAC_ADDRESS;
            try {
                WifiInfo info = mWiFiManager.getConnectionInfo();
                if (info != null) {
                    mLastBssid = info.getBSSID();
                    if (mLastBssid != null) {
                        sMac = mLastBssid.replace(":", "-");
                    }
                }
            } catch (Exception e) {
                loge("Failed to get MAC address");
            }
            if (sMac != null) {
                str2 = sMac + ":";
            }
        }
        String str3 = String.format(Locale.US, "nai.epc.mnc%03d.mcc%03d.3gppnetwork.org", mnc, mcc);
        mConnectionParameters.mMeId = str1 + str2 + str3;
        if (!str2.isEmpty()) {
            mConnectionParameters.mEapId = str1 + str3;
        }
        /* SERVER ID */
        mConnectionParameters.mOtherId = getResponderId(apnName, mcc, mnc);
        /* DEVICE IMEI */
        processDeviceIdentity(tm);
        /* HANDOVER */
        mProtocol = dataProfile.getProtocolType();
        if (reason == REQUEST_REASON_HANDOVER) {
            mLinkProperties = new LinkProperties(linkProperties);
            mConnectionParameters.mIsHandover = true;
            String s = "";
            for (LinkAddress address : linkProperties.getLinkAddresses()) {
                s += address.getAddress().getHostAddress() + ",";
            }
            mConnectionParameters.mMeSourceIP = s.substring(0, s.length() - 1);
        } else {
            mLinkProperties = null;
            boolean requestPcscf = (hasType(ApnSetting.TYPE_IMS) || hasType(ApnSetting.TYPE_EMERGENCY) || mConfigurationParameters.mRequestPcscfForAllPdn);
            switch (mProtocol) {
                case PROTOCOL_IP:
                    mConnectionParameters.mMeSourceIP = ConnectionParameters.PROTO_IPv4;
                    mConnectionParameters.mMeDns = ConnectionParameters.PROTO_IPv4;
                    mIkeConfiguration.addExpectedAttribute(IkeConfiguration.INTERNAL_IP4_DNS);
                    if (requestPcscf) {
                        mIkeConfiguration.addPcscfAttribute(mConfigurationParameters.mIkeAttrPcscf4, null);
                    }
                    break;
                case PROTOCOL_IPV6:
                    mConnectionParameters.mMeSourceIP = ConnectionParameters.PROTO_IPv6;
                    mConnectionParameters.mMeDns = ConnectionParameters.PROTO_IPv6;
                    mIkeConfiguration.addExpectedAttribute(IkeConfiguration.INTERNAL_IP6_DNS);
                    if (requestPcscf) {
                        mIkeConfiguration.addPcscfAttribute(mConfigurationParameters.mIkeAttrPcscf6, null);
                    }
                    break;
                case PROTOCOL_IPV4V6:
                    mConnectionParameters.mMeSourceIP = ConnectionParameters.PROTO_IPv4v6;
                    mConnectionParameters.mMeDns = ConnectionParameters.PROTO_IPv4v6;
                    mIkeConfiguration.addExpectedAttribute(IkeConfiguration.INTERNAL_IP4_DNS);
                    mIkeConfiguration.addExpectedAttribute(IkeConfiguration.INTERNAL_IP6_DNS);
                    if (requestPcscf) {
                        mIkeConfiguration.addPcscfAttribute(mConfigurationParameters.mIkeAttrPcscf4, null);
                        mIkeConfiguration.addPcscfAttribute(mConfigurationParameters.mIkeAttrPcscf6, null);
                    }
                    break;
            }
        }
        /* REMAINED */
        switch (mProtocol) {
            case PROTOCOL_IP:
                mConnectionParameters.mOtherSubnets = ConnectionParameters.SUBNET_IPv4;
                break;
            case PROTOCOL_IPV6:
                mConnectionParameters.mOtherSubnets = ConnectionParameters.SUBNET_IPv6;
                break;
            case PROTOCOL_IPV4V6:
                mConnectionParameters.mOtherSubnets = ConnectionParameters.SUBNET_IPv4v6;
                break;
        }
        /* don't use for non IMS */
        if (!hasType(ApnSetting.TYPE_IMS)) {
            mConnectionParameters.mDpdInterval = 0;
            mConnectionParameters.mKeepAliveInterval = 0;
        } else {
            mIkeConfiguration.addLivenessCheckAttribute();
            /* use local keep-alive value */
            if (isUseOffloadedKeepalive()) {
                mKeepAliveInterval = mConnectionParameters.mKeepAliveInterval;
                mConnectionParameters.mKeepAliveInterval = 0;
            }
        }
        mNofifyProcessor.addExpectedNotify(IkeNotifyProcessor.BACKOFF_TIMER);
        /* add 5GS over ePDG access */
        mSliceInfo = sliceInfo;
        mPduSessionId = pduSessionId;
        if (mPduSessionId != DataCallResponse.PDU_SESSION_ID_NOT_SET) {
            mNofifyProcessor.sendN1ModeCapability(mPduSessionId);
            mNofifyProcessor.addExpectedNotify(IkeNotifyProcessor.N1_MODE_INFORMATION);
            mNofifyProcessor.addExpectedNotify(IkeNotifyProcessor.N1_MODE_S_NSSAI_PLMN_ID);
        }
    }

    public void destroyEstablished() {
        log("Destroy established " + mApnType);
        if (mRequestMessage != null) {
            Message message = mRequestMessage;
            mRequestMessage = null;

            mConnectionCallback.onConnectionTerminated(mApnType, message);
        } else {
            mDaemonInterface.deleteTunnel(mConnectionParameters.mName);
        }
    }

    public void destroyPending() {
        log("Destroy pending " + mApnType);
        if (mRequestMessage != null) {
            DataCallResponse r = createFailureResponse(DataFailCause.SIM_CARD_CHANGED);
            Message message = mRequestMessage;
            mRequestMessage = null;

            mConnectionCallback.onConnectionCompleted(mApnType, message, r);
        }
        mDaemonInterface.deleteTunnel(mConnectionParameters.mName);
    }

    public void closePending() {
        log("Close pending " + mApnType + " ," + mConnectionParameters.mName);
        if (mRequestMessage != null) {
            DataCallResponse r = createFailureResponse(DataFailCause.ERROR_UNSPECIFIED);
            Message message = mRequestMessage;
            mRequestMessage = null;

            mConnectionCallback.onConnectionCompleted(mApnType, message, r);
        }
        mDaemonInterface.closeTunnel(mConnectionParameters.mName);
    }

    public DataCallResponse getDataCallResponse() {
        DataCallResponse r;

        List<InetAddress> gateways = new ArrayList<>();
        for (RouteInfo ri : mLinkProperties.getRoutes()) {
            gateways.add(ri.getGateway());
        }
        int mtu = mLinkProperties.getMtu();
        DataCallResponse.Builder builder = new DataCallResponse.Builder()
                .setCause(DataFailCause.NONE)
                .setRetryDurationMillis(0)
                .setId(mApnType.getCode(mSlotId))
                .setLinkStatus(DataCallResponse.LINK_STATUS_ACTIVE)
                .setProtocolType(mProtocol)
                .setInterfaceName(mLinkProperties.getInterfaceName())
                .setAddresses(mLinkProperties.getLinkAddresses())
                .setDnsAddresses(mLinkProperties.getDnsServers())
                .setGatewayAddresses(gateways)
                .setPcscfAddresses(mLinkProperties.getPcscfServers())
                .setMtu(mtu)
                .setMtuV4(mtu)
                .setMtuV6(mtu)
                .setPduSessionId(mPduSessionId)
                .setDefaultQos(null)
                .setSliceInfo(mSliceInfo);
        r = builder.build();

        log("Response: " + r);
        return r;
    }

    public void activate(Network network, LinkProperties linkProperties, Message message) {
        if (network == null || linkProperties == null) {
            MobileNetworkCallback callback = SimManager.getInstance().getMobileNetwork(mSlotId, mConfigurationManager);
            if (callback != null) {
                network = callback.getNetwork();
                linkProperties = callback.getLinkProperties();
            } else {
                loge("APN " + mApnType + " could not be activated, no network found");

                DataCallResponse r = createFailureResponse(DataFailCause.ERROR_UNSPECIFIED);
                log("Response: " + r);
                mConnectionCallback.onConnectionCompleted(mApnType, message, r);
                return;
            }
        }

        mNetwork = network;

        /* copy to local structure */
        for (LinkAddress la : linkProperties.getLinkAddresses()) {
            mLocalAddresses.add(la.getAddress());
        }

        InetAddress selectedRemoteIP = mRetryHandler.getAddress(mLocalAddresses);
        if (selectedRemoteIP != null) {
            InetAddress selectedLocalIP;
            if (selectedRemoteIP instanceof Inet4Address) {
                selectedLocalIP = getLocalIpv4();
            } else {
                selectedLocalIP = getLocalIpv6();
            }
            mConnectionParameters.mLocalAddress = selectedLocalIP;
            mConnectionParameters.mMeAddress = selectedLocalIP.getHostAddress();
            mConnectionParameters.mRemoteAddress = selectedRemoteIP;
            mConnectionParameters.mOtherAddress = selectedRemoteIP.getHostAddress();
        } else {
            mRetryHandler.clearAddresses();

            DataCallResponse r = createFailureResponse(DataFailCause.INVALID_DNS_ADDR);
            log("Response: " + r);
            mConnectionCallback.onConnectionCompleted(mApnType, message, r);
            return;
        }
        try {
            /* check IKE daemon max 1 sec*/
            int tries = 0;
            while (!mDaemonInterface.isIkeDaemonStarted()) {
                Thread.sleep(50);
                if (++tries > 20) {
                    loge("Failed to wait daemon");

                    DataCallResponse r = createFailureResponse(DataFailCause.ERROR_UNSPECIFIED);
                    log("Response: " + r);
                    mConnectionCallback.onConnectionCompleted(mApnType, message, r);
                    return;
                }
            }
        } catch (InterruptedException e) {
            /* not required */
        }
        mConnectionParameters.mInterface = linkProperties.getInterfaceName();
        if (TextUtils.isEmpty(mConnectionParameters.mInterface)) {
            mConnectionParameters.mInterface = getInterfaceByAddress(mConnectionParameters.mLocalAddress);
        }
        mDaemonInterface.createRoute(mConnectionParameters);

        mRequestMessage = message;

        log("Open data connection with " + mConnectionParameters);
        mDaemonInterface.openTunnel(mConnectionParameters, mDaemonCallback);
    }

    public void deactivate(Message message) {
        if (message != null) {
            mRequestMessage = message;
        }

        log("Close " + mConnectionParameters.mName);
        mDaemonInterface.closeTunnel(mConnectionParameters.mName);
    }

    public void updateLocalAddresses(LinkProperties linkProperties) {
        List<LinkAddress> addresses = linkProperties.getLinkAddresses();
        /* copy to local structure */
        mLocalAddresses.clear();
        for (LinkAddress la : addresses) {
            mLocalAddresses.add(la.getAddress());
        }
        /* check */
        InetAddress currentLocalIP;
        if (mConnectionParameters.mRemoteAddress instanceof Inet4Address) {
            currentLocalIP = getLocalIpv4();
        } else {
            currentLocalIP = getLocalIpv6();
        }
        if (currentLocalIP == null) {
            loge("Could not find IP address matched to server IP");
            return;
        }
        if (!mConnectionParameters.mLocalAddress.equals(currentLocalIP)) {
            mDaemonInterface.deleteRoute(mConnectionParameters);

            String interfaceName = linkProperties.getInterfaceName();
            if (TextUtils.isEmpty(interfaceName)) {
                interfaceName = getInterfaceByAddress(currentLocalIP);
            }
            if (!interfaceName.equals(mConnectionParameters.mInterface)) {
                log("Interface was changed from " + mConnectionParameters.mInterface + " to " + interfaceName);
                mConnectionParameters.mInterface = interfaceName;
                mDaemonInterface.setNewInterface(mConnectionParameters.mName, interfaceName);
            }
            log("Local IP address updated to " + currentLocalIP);
            mConnectionParameters.mLocalAddress = currentLocalIP;
            mDaemonInterface.createRoute(mConnectionParameters);
        }
        /* mobility */
        if (mConfigurationParameters.mUseMacForNai) {
            try {
                WifiInfo info = mWiFiManager.getConnectionInfo();
                if (info != null) {
                    String bssid = info.getBSSID();
                    if ((bssid != null) && !bssid.equalsIgnoreCase(mLastBssid)) {
                        log("BSSID updated, moved to " + bssid);
                        mLastBssid = bssid;
                    }
                }
            } catch (Exception e) {
                loge("Failed to get MAC address");
            }
        }
    }

    private DataCallResponse createFailureResponse(int reason) {
        return mRetryHandler.createFailureResponse(reason, mProtocol,
                mConnectionParameters.mIsHandover);
    }

    private DataCallResponse createFailureResponse(int reason, int timeout) {
        if (timeout == 0) {
            timeout = DataCallResponse.RETRY_DURATION_UNDEFINED;
        }
        return mRetryHandler.createFailureResponse(reason, timeout,
                mProtocol, mConnectionParameters.mIsHandover);
    }

    private InetAddress getLocalIpv4() {
        for (InetAddress a : mLocalAddresses) {
            if (a instanceof Inet4Address) {
                return a;
            }
        }
        return null;
    }

    private InetAddress getLocalIpv6() {
        for (InetAddress a : mLocalAddresses) {
            if ((a instanceof Inet6Address) && !((Inet6Address)a).isLinkLocalAddress()) {
                return a;
            }
        }
        return null;
    }

    private boolean hasType(int type) {
        return ((mApnBitmask & type) != 0);
    }

    private String getApnName(DataProfile dataProfile) {
        String name;

        if (hasType(ApnSetting.TYPE_EMERGENCY) && !mConfigurationParameters.mEmcEpdgName.isEmpty()) {
            name = mConfigurationParameters.mEmcEpdgName;
        } else if (hasType(ApnSetting.TYPE_MMS) && !mConfigurationParameters.mMmsEpdgName.isEmpty()) {
            name = mConfigurationParameters.mMmsEpdgName;
        } else if (hasType(ApnSetting.TYPE_XCAP) && !mConfigurationParameters.mXcapEpdgName.isEmpty()) {
            name = mConfigurationParameters.mXcapEpdgName;
        } else {
            name = dataProfile.getApn().toLowerCase(Locale.US);
        }
        return name;
    }

    protected class KeepAliveCallback extends SocketKeepalive.Callback {
        @Override
        public void onStarted() {
            log("Keep-alive started for " + mConnectionParameters.mName + " with duration " + mKeepAliveInterval);
        }

        @Override
        public void onStopped() {
            log("Keep-alive stopped for " + mConnectionParameters.mName);
        }

        @Override
        public void onError(int error) {
            final String sError;
            switch (error) {
                case SocketKeepalive.SUCCESS: sError = "NO_ERROR"; break;
                case SocketKeepalive.ERROR_INVALID_NETWORK: sError = "ERROR_INVALID_NETWORK"; break;
                case SocketKeepalive.ERROR_INVALID_IP_ADDRESS: sError = "ERROR_INVALID_IP_ADDRESS"; break;
                case SocketKeepalive.ERROR_INVALID_PORT: sError = "ERROR_INVALID_PORT"; break;
                case SocketKeepalive.ERROR_INVALID_LENGTH: sError = "ERROR_INVALID_LENGTH"; break;
                case SocketKeepalive.ERROR_INVALID_INTERVAL: sError = "ERROR_INVALID_INTERVAL"; break;
                case SocketKeepalive.ERROR_INVALID_SOCKET: sError = "ERROR_INVALID_SOCKET"; break;
                case SocketKeepalive.ERROR_SOCKET_NOT_IDLE: sError = "ERROR_SOCKET_NOT_IDLE"; break;
                case SocketKeepalive.ERROR_UNSUPPORTED: sError = "ERROR_UNSUPPORTED"; break;
                case SocketKeepalive.ERROR_HARDWARE_ERROR: sError = "ERROR_HARDWARE_ERROR"; break;
                case SocketKeepalive.ERROR_INSUFFICIENT_RESOURCES: sError = "ERROR_INSUFFICIENT_RESOURCES"; break;
                default: sError = "UNKNOWN(" + error + ")";
            }
            loge("Keep-alive error " + sError + " for " + mConnectionParameters.mName);
        }
    }

    private boolean isUseOffloadedKeepalive() {
        return mConfigurationParameters.mUseOffloadedKeepalive;
    }

    private IpSecManager.UdpEncapsulationSocket openKeepAliveSocket() {
        IpSecManager.UdpEncapsulationSocket socket = null;
        try {
            socket = mIpSecManager.openUdpEncapsulationSocket(4500);
            /* UDP */
            FileDescriptor fd = socket.getFileDescriptor();
            Os.fcntlInt(fd, F_SETFL, SOCK_DGRAM | SOCK_NONBLOCK);
            /* bind to network */
            mNetwork.bindSocket(fd);
        } catch (Exception e) {
            if (socket != null) {
                try { socket.close(); } catch (Exception e1) { /* nothing */ }
            }
        }
        return socket;
    }

    private void startOffloadedKeepAlive() {
        if (hasType(ApnSetting.TYPE_IMS) && (mKeepAliveInterval > 0)) {
            stopOffloadedKeepAlive();
            try {
                mUdpSocket = openKeepAliveSocket();
                if (mUdpSocket == null) {
                    return;
                }
                mKeepAliveSocket = mConnectivityManager.createSocketKeepalive(mNetwork, mUdpSocket,
                        mConnectionParameters.mLocalAddress, mConnectionParameters.mRemoteAddress,
                        Executors.newSingleThreadExecutor(), new KeepAliveCallback());
                mKeepAliveSocket.start(mKeepAliveInterval);
            } catch (Exception e) {
                loge("Exception at keep-alive start " + e);
                stopOffloadedKeepAlive();
            }
        }
    }

    private void stopOffloadedKeepAlive() {
        if (hasType(ApnSetting.TYPE_IMS)) {
            try {
                if (mKeepAliveSocket != null) {
                    mKeepAliveSocket.stop();
                }
                if (mUdpSocket != null) {
                    mUdpSocket.close();
                }
            } catch (Exception e) {
                loge("Exception at keep-alive stop " + e);
            }
            mKeepAliveSocket = null;
            mUdpSocket = null;
        }
    }

    /* if network was changed keep-alive restart required */
    public void updateNetwork(Network network) {
        if (!mNetwork.equals(network)) {
            log("Network changed from " + mNetwork + " to " + network);
            mNetwork = network;
            if (isUseOffloadedKeepalive()) {
                startOffloadedKeepAlive();
            }
        }
    }

    private String getInterfaceByAddress(InetAddress address) {
        String name = "wlan0";
        try {
            NetworkInterface ni = NetworkInterface.getByInetAddress(address);
            if (ni != null) {
                name = ni.getName();
            }
        } catch (Exception e) {
            loge("Failed to get interface name");
        }
        return name;
    }

    private void processDeviceIdentity(TelephonyManager tm) {
        if (TextUtils.isEmpty(mConfigurationParameters.mIkeDeviceIdentity)) {
            return;
        }
        try {
            String imei = tm.getImei();
            if (TextUtils.isEmpty(imei)) {
                return;
            }
            String configValue = mConfigurationParameters.mIkeDeviceIdentity;
            if (ConfigurationParameters.IDENTITY_AS_NOTIFY.equals(configValue)) {
                mNofifyProcessor.sendDeviceIdentity(imei);
            } else if (configValue.startsWith(ConfigurationParameters.IDENTITY_AS_ATTRIBUTE)) {
                String[] strings = configValue.split(":");
                if (strings.length > 1) {
                    String s = strings[1];
                    if (ConfigurationParameters.ATTR_AT_DEVICE_IDENTITY.equals(s)) {
                        // attribute type not defined by IANA yet
                    } else if (ConfigurationParameters.ATTR_DEVICE_IDENTITY.equals(s)) {
                        mIkeConfiguration.addDeviceIdentityAttribute(imei);
                    } else {
                        int attrType = Integer.parseInt(s);
                        mIkeConfiguration.addImeiAttribute(attrType, imei);
                    }
                }
            }
        } catch (Exception e) {
            loge("Failed to build device identity");
        }
    }

    private String getResponderId(String apnName, int mcc, int mnc) {
        String response;

        try {
            String idFormat = getResponderIdFormat();
            if (idFormat.startsWith(ConfigurationParameters.ID_KEY_ID)) {
                response = String.format(Locale.US, idFormat, apnName);
            } else if (idFormat.startsWith(ConfigurationParameters.ID_FQDN)) {
                String[] strings = apnName.split("\\.");
                if (idFormat.contains("mnc") && idFormat.contains("mcc")) {
                    response = String.format(Locale.US, idFormat, strings[0], mnc, mcc);
                } else {
                    response = String.format(Locale.US, idFormat, strings[0]);
                }
            } else {
                /* default */
                response = ConfigurationParameters.ID_KEY_ID + ":" + apnName;
            }
        } catch (Exception e) {
            response = ConfigurationParameters.ID_KEY_ID + ":" + apnName;
        }
        return response;
    }

    private String getResponderIdFormat() {
        if (hasType(ApnSetting.TYPE_EMERGENCY)
                && !TextUtils.isEmpty(mConfigurationParameters.mResponderIdFormatForEmergency)) {
            return mConfigurationParameters.mResponderIdFormatForEmergency;
        } else if (hasType(ApnSetting.TYPE_MMS)
                && !TextUtils.isEmpty(mConfigurationParameters.mResponderIdFormatForMms)) {
            return mConfigurationParameters.mResponderIdFormatForMms;
        } else if (hasType(ApnSetting.TYPE_XCAP)
                && !TextUtils.isEmpty(mConfigurationParameters.mResponderIdFormatForXcap)) {
            return mConfigurationParameters.mResponderIdFormatForXcap;
        } else {
            return mConfigurationParameters.mResponderIdFormat;
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
    protected Message getRequestMessage() {
        return mRequestMessage;
    }

    @VisibleForTesting
    protected void setLinkProperties(LinkProperties linkProperties) {
        mLinkProperties = linkProperties;
    }

    @VisibleForTesting
    protected void setKeepAliveInterval(int keepAliveInterval) {
        mKeepAliveInterval = keepAliveInterval;
    }

    @VisibleForTesting
    protected DataConnection.DaemonCallback getDaemonCallback() {
        return mDaemonCallback;
    }
}
