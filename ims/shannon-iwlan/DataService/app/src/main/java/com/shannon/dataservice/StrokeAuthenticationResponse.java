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

public class StrokeAuthenticationResponse extends StrokeResponseMessage {
    private static final int AKA_CK_LENGTH = 16;
    private static final int AKA_IK_LENGTH = 16;
    private static final int AKA_RES_LENGTH = 16;

    public enum AuthenticationStatus {
        AUTH_STATUS_SUCCESS(0, 0),
        AUTH_STATUS_ERROR(1, 0),
        AUTH_STATUS_SYNC_FAILURE(2, 5);

        private int mCode;
        private byte mStatus;

        AuthenticationStatus(int code, int status) {
            mCode = code;
            mStatus = Integer.valueOf(status).byteValue();
        }

        public int getCode() { return mCode; }

        public static AuthenticationStatus getEnum(int status) {
            byte value = Integer.valueOf(status).byteValue();
            for (AuthenticationStatus e : AuthenticationStatus.values()) {
                if (e.mStatus == value) {
                    return e;
                }
            }
            return AUTH_STATUS_ERROR;
        }
    }

    public StrokeAuthenticationResponse(AuthenticationStatus status, byte[] CK, byte[] IK, byte[] RES) {
        super(ResponseMessageType.STR_RES_SIM_AUTH, Short.BYTES + (Integer.BYTES * 2) +
                AKA_CK_LENGTH + AKA_IK_LENGTH + AKA_RES_LENGTH);
        putInt(status.getCode());
        if (CK != null && CK.length > 0) {
            putBytesArray(CK);
        } else {
            putBytesArray(new byte[AKA_CK_LENGTH]);
        }
        if (IK != null && IK.length > 0) {
            putBytesArray(IK);
        } else {
            putBytesArray(new byte[AKA_IK_LENGTH]);
        }
        putInt(RES != null ? RES.length : 0);
        if (RES != null && RES.length > 0) {
            putBytesArray(RES);
        } else {
            putBytesArray(new byte[AKA_RES_LENGTH]);
        }
    }
}
