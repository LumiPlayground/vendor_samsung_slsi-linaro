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

public class IkeNotifyN1ModeCapability extends IkeNotifyData {
    public IkeNotifyN1ModeCapability(int pduSessionId) {
        super(IkeNotifyProcessor.N1_MODE_CAPABILITY, convertToBytes(pduSessionId));
    }

    private static byte[] convertToBytes(int pduSessionId) {
        /*
         * Length
         * PDU Session ID
         */
        byte[] bytes = new byte[2];
        bytes[0] = 1; // length
        bytes[1] = Integer.valueOf(pduSessionId).byteValue();
        return bytes;
    }
}
