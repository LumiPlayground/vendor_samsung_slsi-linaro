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

import android.telephony.TelephonyManager;

import java.util.LinkedList;

public enum CarrierType {
    CARRIER_TYPE_ATT(1187, 1779, 2023, 2119, 10013, 10021, 10028, "25851", "262800"),
    CARRIER_TYPE_TMO(1, 1949, 2078, 10001, "310332"),
    CARRIER_TYPE_VZW(1839, 1849, 2032, 2126, 2146, 10008, "310028"),
    CARRIER_TYPE_ORANGE(32, 678, 742, 897, 1011, 1366, 1659, 1676, 1713, 2369),
    CARRIER_TYPE_SWISSCOM(16, 1595, 2366),
    CARRIER_TYPE_DTAG(3, 4, 5, 6, 7, 8, 9, 11, 13, 747, 1010, 2088, 2091, 2092, 2386),
    CARRIER_TYPE_ROGERS(1403, 1962, 10025),
    CARRIER_TYPE_EE(2),
    CARRIER_TYPE_TELSTRA(1345),
    CARRIER_TYPE_RJIO(2018);

    private final LinkedList<String> mOperators;
    private final LinkedList<Integer> mCarrierIds;

    private CarrierType(Object ... ids) {
        mOperators = new LinkedList<>();
        mCarrierIds = new LinkedList<>();

        for (Object o : ids) {
            if (o instanceof Integer) {
                mCarrierIds.push((int)o);
            } else if (o instanceof String) {
                mOperators.push((String)o);
            }
        }
    }

    private boolean findById(int carrierId) {
        if (mCarrierIds.isEmpty()) {
            return false;
        }
        for (int id : mCarrierIds) {
            if (id == carrierId) {
                return true;
            }
        }
        return false;
    }

    private boolean findByOperator(String operator) {
        if (mOperators.isEmpty()) {
            return false;
        }
        for (String op : mOperators) {
            if (op.equals(operator)) {
                return true;
            }
        }
        return false;
    }

    public static CarrierType getEnum(TelephonyManager tm) {
        if (tm == null) {
            return null;
        }
        String operator = tm.getSimOperator();
        int carrierId = tm.getSimSpecificCarrierId();
        if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
            carrierId = tm.getSimCarrierId();
            if (carrierId == TelephonyManager.UNKNOWN_CARRIER_ID) {
                carrierId = tm.getCarrierIdFromSimMccMnc();
            }
        }
        for (CarrierType e : CarrierType.values()) {
            if (carrierId != TelephonyManager.UNKNOWN_CARRIER_ID) {
                if (e.findById(carrierId)) {
                    return e;
                }
            } else {
                if (e.findByOperator(operator)) {
                    return e;
                }
            }
        }
        return null;
    }
}
