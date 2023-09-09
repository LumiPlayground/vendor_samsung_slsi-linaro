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

public interface TunnelConnectionCallback {
    public void onConnected(String connectionName, String adapterName, int mtu, String tunnelIP, byte[] attributes, byte[] notifies);
    public void onConnectionError(String connectionName, TunnelError error, byte[] notifies);
    public void onDisconnected(String connectionName);
    public void onTerminated(String connectionName, StrokeTerminatedIndication.Cause cause);
    public byte[] onAuthenticationRequest(byte[] input);
}