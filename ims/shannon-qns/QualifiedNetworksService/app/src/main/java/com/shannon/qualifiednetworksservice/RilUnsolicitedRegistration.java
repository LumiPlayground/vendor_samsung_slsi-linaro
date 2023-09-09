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

import com.android.internal.annotations.VisibleForTesting;;

public class RilUnsolicitedRegistration extends RilUnsolicitedMessage {
    private enum RegistrationState {
        UNKNOWN(0),
        IMS_NOT_REGISTERED(1),
        IMS_REGISTERED(2),
        IMS_REGISTERED_LIMITED(3),
        EMERGENCY_NOT_REGISTERED(4),
        EMERGENCY_REGISTERED(5),
        RCS_NOT_REGISTERED(6),
        RCS_REGISTERED(7);

        private int mState;

        RegistrationState(int state) {
            mState = state;
        }

        public static RegistrationState getEnum(int state) {
            for (RegistrationState e : values()) {
                if (e.mState == state) {
                    return e;
                }
            }
            return UNKNOWN;
        }
    }

    @VisibleForTesting
    protected enum StatusCode {
        SUCCESS(0),
        FEATURES_MISMATCH(1),
        DEREG_NWK_TRIGGERED(2),
        DEREG_NORSP_FROMNWK(3),
        DEREG_INTERNAL(4),
        REG_FAIL(5),
        REG_FAIL_500(6),
        REG_FAIL_403(7),
        REG_FAIL_RETRY(8),
        SUCCESS_LIMITED_ACCESS_MODE(9),
        REG_FAIL_TRY_AFTER_CELL_CHANGE(10),         // 0x0A
        REG_FAIL_TIMS_EXPIRY(11),                   // 0x0B
        REG_FAIL_FRESH_REG_START(12),               // 0x0C
        DEREG_FAIL_ONGOING_CALL(13),                // 0x0D
        REG_FAIL_TIMS_START(14),                    // 0x0E
        REG_FAIL_TRY_AFTER_WIFI_REASSOCIATE(15);    // 0x0F

        private int mStatusCode;

        StatusCode(int statusCode) {
            mStatusCode = statusCode;
        }

        public int getInt() {
            return mStatusCode;
        }

        public static StatusCode getEnum(int statusCode) {
            for (StatusCode e : values()) {
                if (e.mStatusCode == statusCode) {
                    return e;
                }
            }
            return SUCCESS;
        }
    }

    private RegistrationState mState;
    private StatusCode mStatusCode;

    public RilUnsolicitedRegistration(RilUnsolicitedMessage from) {
        super(from);

        mState = RegistrationState.UNKNOWN;

        byte[] bytes = getData();
        if (bytes != null && bytes.length > 1) {
            mState = RegistrationState.getEnum(bytes[0]);
            mStatusCode = StatusCode.getEnum(getShort(bytes[6], bytes[7]));
        }
    }

    public boolean isImsIndication() {
        return (mState == RegistrationState.IMS_NOT_REGISTERED ||
                mState == RegistrationState.IMS_REGISTERED ||
                mState == RegistrationState.IMS_REGISTERED_LIMITED);
    }

    public boolean isImsRegistered() {
        return (mState == RegistrationState.IMS_REGISTERED ||
                mState == RegistrationState.IMS_REGISTERED_LIMITED);
    }

    public boolean isEmergencyIndication() {
        return (mState == RegistrationState.EMERGENCY_NOT_REGISTERED ||
                mState == RegistrationState.EMERGENCY_REGISTERED);
    }

    public boolean isEmergencyRegistered() {
        return (mState == RegistrationState.EMERGENCY_REGISTERED);
    }

    public int getStatusCodeAsInt() {
        return mStatusCode.getInt();
    }

    @Override
    public String toString() {
        return ("RIL_UNSOLICITED_REGISTRATION {" + "REG_STATE: " + mState + ", STATUS_CODE: " +
                mStatusCode + "}");
    }
}
