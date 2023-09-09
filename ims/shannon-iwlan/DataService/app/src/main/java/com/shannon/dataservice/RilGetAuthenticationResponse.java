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

import android.util.Base64;

import static com.shannon.dataservice.RilOemInterface.RIL_RESPONSE_SUCCESS;

public class RilGetAuthenticationResponse extends RilSolicitedMessage {
    private static final byte SIM_TYPE_ISIM = 0;
    private static final byte SIM_TYPE_USIM = 2;

    RilGetAuthenticationResponse(RilOemInterface oemInterface, byte[] input, boolean isUsim) {
        super(RIL_GET_AUTHENTICATION_RESPONSE, oemInterface);

        byte sim_type = SIM_TYPE_ISIM;
        if (isUsim) {
            sim_type = SIM_TYPE_USIM;
        }
        byte[] tmp = new byte[input.length + 1];
        tmp[0] = sim_type;
        System.arraycopy(input, 0, tmp, 1, input.length);

        String nonce = Base64.encodeToString(tmp, Base64.NO_WRAP);
        try {
            mRawData = nonce.getBytes("UTF-8");
        } catch (Exception e) {
            // nothing
        }
    }
}
