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

import android.telephony.DataFailCause;

import com.shannon.dataservice.StrokeResponseMessage.ProcessedNotifyType;
import com.shannon.dataservice.StrokeResponseMessage.ResponseErrorStatus;

public enum TunnelError {
    /* statuses */
    ERR_SUCCESS(ResponseErrorStatus.STR_ERROR_SUCCESS),
    ERR_UNKNOWN(ResponseErrorStatus.STR_ERROR_UNKNOWN),
    ERR_PEER_INIT_UNREACHABLE(ResponseErrorStatus.STR_ERROR_PEER_INIT_UNREACHABLE),
    ERR_CERTIFICATE_EXPIRED(ResponseErrorStatus.STR_ERROR_CERTIFICATE_EXPIRED),
    ERR_CERTIFICATE_REVOKED(ResponseErrorStatus.STR_ERROR_CERTIFICATE_REVOKED),
    ERR_CERTIFICATE_VALIDATION_FAILED(ResponseErrorStatus.STR_ERROR_CERTIFICATE_VALIDATION_FAILED),
    ERR_CERTIFICATE_NO_ISSUER(ResponseErrorStatus.STR_ERROR_CERTIFICATE_NO_ISSUER),
    ERR_CERTIFICATE_UNTRUSTED_ROOT(ResponseErrorStatus.STR_ERROR_CERTIFICATE_UNTRUSTED_ROOT),
    ERR_CERTIFICATE_EXCEEDED_PATH_LEN(ResponseErrorStatus.STR_ERROR_CERTIFICATE_EXCEEDED_PATH_LEN),
    ERR_CERTIFICATE_POLICY_VIOLATION(ResponseErrorStatus.STR_ERROR_CERTIFICATE_POLICY_VIOLATION),
    ERR_PEER_AUTH_FAILED(ResponseErrorStatus.STR_ERROR_CERTIFICATE_AUTH_FAILED),
    ERR_NETWORK_GENERIC_FAILURE(ResponseErrorStatus.STR_ERROR_NETWORK_FAILURE),
    /* internal DNS errors */
    ERR_DNS_UNKNOWN(1000),
    ERR_DNS_TIMEOUT(1001),
    ERR_DNS_NO_DATA(1002),
    /* notifies */
    ERR_NETWORK_AUTH_FAILED(ProcessedNotifyType.STR_NOTIFY_NETWORK_AUTH_FAILED),
    ERR_INTERNAL_ADDR_FAILED(ProcessedNotifyType.STR_NOTIFY_INTERNAL_ADDR_FAILED),
    /* 3GPP TS 24.302 defined notifies */
    ERR_PDN_CONNECTION_REJECTION(ProcessedNotifyType.STR_NOTIFY_PDN_CONNECTION_REJECTION),
    ERR_MAX_CONNECTION_REACHED(ProcessedNotifyType.STR_NOTIFY_MAX_CONNECTION_REACHED),
    ERR_SEMANTIC_ERROR_IN_THE_TFT_OPERATION(ProcessedNotifyType.STR_NOTIFY_SEMANTIC_ERROR_IN_THE_TFT_OPERATION),
    ERR_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION(ProcessedNotifyType.STR_NOTIFY_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION),
    ERR_SEMANTIC_ERRORS_IN_PACKET_FILTERS(ProcessedNotifyType.STR_NOTIFY_SEMANTIC_ERRORS_IN_PACKET_FILTERS),
    ERR_SYNTACTICAL_ERRORS_IN_PACKET_FILTERS(ProcessedNotifyType.STR_NOTIFY_SYNTACTICAL_ERRORS_IN_PACKET_FILTERS),
    ERR_NON_3GPP_ACCESS_NOT_ALLOWED(ProcessedNotifyType.STR_NOTIFY_NON_3GPP_ACCESS_NOT_ALLOWED),
    ERR_USER_UNKNOWN(ProcessedNotifyType.STR_NOTIFY_USER_UNKNOWN),
    ERR_NO_APN_SUBSCRIPTION(ProcessedNotifyType.STR_NOTIFY_NO_APN_SUBSCRIPTION),
    ERR_AUTHORIZATION_REJECTED(ProcessedNotifyType.STR_NOTIFY_AUTHORIZATION_REJECTED),
    ERR_ILLEGAL_ME(ProcessedNotifyType.STR_NOTIFY_ILLEGAL_ME),
    ERR_S2B_MISSING_OR_UNKNOWN_APN(ProcessedNotifyType.STR_NOTIFY_MISSING_OR_UNKNOWN_APN),
    ERR_S2B_NO_SUBSCRIPTION(ProcessedNotifyType.STR_NOTIFY_NO_SUBSCRIPTION),
    ERR_NETWORK_FAILURE(ProcessedNotifyType.STR_NOTIFY_NETWORK_FAILURE),
    ERR_ROAMING_NOT_ALLOWED(ProcessedNotifyType.STR_NOTIFY_ROAMING_NOT_ALLOWED),
    ERR_RAT_TYPE_NOT_ALLOWED(ProcessedNotifyType.STR_NOTIFY_RAT_TYPE_NOT_ALLOWED),
    ERR_PERMANENT_FAILURE(ProcessedNotifyType.STR_NOTIFY_PERMANENT_FAILURE),
    ERR_S2B_RAT_DISALLOWED(ProcessedNotifyType.STR_NOTIFY_RAT_DISALLOWED),
    ERR_IMEI_NOT_ACCEPTED(ProcessedNotifyType.STR_NOTIFY_IMEI_NOT_ACCEPTED),
    ERR_PLMN_NOT_ALLOWED(ProcessedNotifyType.STR_NOTIFY_PLMN_NOT_ALLOWED),
    ERR_S2B_UNSUPPORTED_PDN_TYPE(ProcessedNotifyType.STR_NOTIFY_UNSUPPORTED_PDN_TYPE),
    ERR_UNAUTHENTICATED_EMERGENCY_NOT_SUPPORTED(ProcessedNotifyType.STR_NOTIFY_UNAUTHENTICATED_EMERGENCY_NOT_SUPPORTED),
    ERR_CONGESTION(ProcessedNotifyType.STR_NOTIFY_CONGESTION),
    ERR_NETWORK_TOO_BUSY(ProcessedNotifyType.STR_NOTIFY_NETWORK_TOO_BUSY);

    private int mCode;

    TunnelError(int id) {
        mCode = id;
    }

    TunnelError(ResponseErrorStatus status) {
        mCode = status.getCode();
    }

    TunnelError(ProcessedNotifyType notify) {
        mCode = notify.getCode();
    }

    public static TunnelError getEnum(int code) {
        for (TunnelError e : values()) {
            if (e.mCode == code) {
                return e;
            }
        }
        return ERR_UNKNOWN;
    }

    public int toDataFailCause() {
        int response = DataFailCause.ERROR_UNSPECIFIED;
        switch (this) {
            case ERR_DNS_UNKNOWN:
            case ERR_DNS_NO_DATA:
            case ERR_DNS_TIMEOUT:
                response = DataFailCause.INVALID_DNS_ADDR;
                break;
            case ERR_CERTIFICATE_EXPIRED:
            case ERR_CERTIFICATE_REVOKED:
            case ERR_CERTIFICATE_NO_ISSUER:
            case ERR_CERTIFICATE_UNTRUSTED_ROOT:
            case ERR_CERTIFICATE_EXCEEDED_PATH_LEN:
            case ERR_CERTIFICATE_POLICY_VIOLATION:
                response = DataFailCause.INVALID_MANDATORY_INFO;
                break;
            case ERR_CERTIFICATE_VALIDATION_FAILED:
                response = DataFailCause.ILLEGAL_MS;
                break;
            case ERR_PEER_INIT_UNREACHABLE:
                response = DataFailCause.NO_RESPONSE_FROM_BASE_STATION;
                break;
            case ERR_NETWORK_GENERIC_FAILURE:
            case ERR_MAX_CONNECTION_REACHED:
            case ERR_SEMANTIC_ERROR_IN_THE_TFT_OPERATION:
            case ERR_SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION:
            case ERR_SEMANTIC_ERRORS_IN_PACKET_FILTERS:
            case ERR_SYNTACTICAL_ERRORS_IN_PACKET_FILTERS:
            case ERR_NETWORK_FAILURE:
            case ERR_IMEI_NOT_ACCEPTED:
            case ERR_UNAUTHENTICATED_EMERGENCY_NOT_SUPPORTED:
            case ERR_INTERNAL_ADDR_FAILED:
                response = DataFailCause.NETWORK_FAILURE;
                break;
            case ERR_ROAMING_NOT_ALLOWED:
                response = DataFailCause.APN_DISALLOWED_ON_ROAMING;
                break;
            case ERR_CONGESTION:
                response = DataFailCause.CONGESTION;
                break;
            case ERR_NETWORK_TOO_BUSY:
                response = DataFailCause.OEM_DCFAILCAUSE_1;
                break;

            /* no retry */
            case ERR_PDN_CONNECTION_REJECTION:
                response = DataFailCause.ACTIVATION_REJECT_GGSN;
                break;
            case ERR_PEER_AUTH_FAILED:
            case ERR_NETWORK_AUTH_FAILED:
                response = DataFailCause.USER_AUTHENTICATION;
                break;
            case ERR_NON_3GPP_ACCESS_NOT_ALLOWED:
            case ERR_RAT_TYPE_NOT_ALLOWED:
            case ERR_S2B_RAT_DISALLOWED:
            case ERR_PERMANENT_FAILURE:
                response = DataFailCause.SERVICE_OPTION_NOT_SUPPORTED;
                break;
            case ERR_NO_APN_SUBSCRIPTION:
            case ERR_PLMN_NOT_ALLOWED:
            case ERR_S2B_MISSING_OR_UNKNOWN_APN:
            case ERR_S2B_UNSUPPORTED_PDN_TYPE:
                response = DataFailCause.MISSING_UNKNOWN_APN;
                break;
            case ERR_USER_UNKNOWN:
            case ERR_AUTHORIZATION_REJECTED:
            case ERR_S2B_NO_SUBSCRIPTION:
                response = DataFailCause.SERVICE_OPTION_NOT_SUBSCRIBED;
                break;
            case ERR_ILLEGAL_ME:
                response = DataFailCause.ILLEGAL_ME;
                break;

            case ERR_UNKNOWN:
            default: ;
        }
        return response;
    }
}
