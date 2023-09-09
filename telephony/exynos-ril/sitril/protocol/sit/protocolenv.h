/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#pragma once

enum ProtocolPayloadType {
    STRUCT,
    JSON,
};

struct ProtocolEnv {
    ProtocolPayloadType mPayloadType;

    bool isJsonSupported() const {
        return mPayloadType == ProtocolPayloadType::JSON;
    }

    bool isLegacyOnly() const {
        return mPayloadType == ProtocolPayloadType::STRUCT;
    }

    ProtocolPayloadType getProtocolPayloadType() const {
        return mPayloadType;
    }

    static ProtocolEnv getEnv();
    static ProtocolEnv getDefaultEnv();
    static void overrideEnv(const ProtocolEnv& env);
};