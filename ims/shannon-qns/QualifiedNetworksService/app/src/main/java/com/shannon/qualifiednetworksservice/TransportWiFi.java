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
package com.shannon.qualifiednetworksservice;

import android.content.Context;
import android.telephony.TelephonyManager;

import java.util.Timer;

public class TransportWiFi extends Transport {
    public TransportWiFi(Context context, int subId, ConfigurationManager configurationManager,
                         TransportState.IStateChanged callback, IVendorTransportHandler vendorHandler) {
        super(TransportType.TRANSPORT_WIFI, context, subId, configurationManager, callback, vendorHandler);
    }

    @Override
    public int getNetworkType() { return TelephonyManager.NETWORK_TYPE_IWLAN; }
}
