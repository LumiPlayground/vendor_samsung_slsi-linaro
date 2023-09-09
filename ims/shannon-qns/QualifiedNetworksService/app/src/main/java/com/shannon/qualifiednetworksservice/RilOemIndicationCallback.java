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
package com.shannon.qualifiednetworksservice;

import android.os.RemoteException;

import com.shannon.qualifiednetworksservice.util.StringUtils;

import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.IOemSlsiRadioExternalInd;
import vendor.samsung_slsi.telephony.hardware.radioExternal.V1_0.RadioExternalResponseInfo;

import java.util.ArrayList;

public class RilOemIndicationCallback extends IOemSlsiRadioExternalInd.Stub {
    private final RilOemInterface mRilInterface;

    RilOemIndicationCallback(RilOemInterface oemInterface) {
        mRilInterface = oemInterface;
    }

    @Override
    public void rilExternalRawIndication(int rilcMsgId, int slotId, ArrayList<Byte> data, int dataLength) throws RemoteException {
        mRilInterface.onIndication(slotId, rilcMsgId, StringUtils.arrayListToPrimitiveArray(data), dataLength);
    }

    @Override
    public void rilExternalRawIndicationSeg(int rilcMsgId, int slotId, ArrayList<Byte> data, int dataLength, int segIndex, int totalLen) {
        mRilInterface.onIndication(slotId, rilcMsgId, data, segIndex, totalLen);
    }
}

