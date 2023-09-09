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

import android.os.RemoteException;

import java.util.ArrayList;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalRes;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalError;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalResponseInfo;

public class RilOemResponseCallback extends IOemSlsiRadioExternalRes.Stub {
    private final RilOemInterface mRilInterface;

    RilOemResponseCallback(RilOemInterface oemInterface) {
        mRilInterface = oemInterface;
    }

    @Override
    public void sendRequestRawResponse(RadioExternalResponseInfo responseInfo, ArrayList<Byte> data) throws RemoteException {
        mRilInterface.onResponse(responseInfo.serial, responseInfo.error, StringUtils.arrayListToPrimitiveArray(data), data.size());
    }

    @Override
    public void sendRequestRawResponseSeg(RadioExternalResponseInfo responseInfo, ArrayList<Byte> data, int segIndex, int totalLen) throws RemoteException {
    }
}
