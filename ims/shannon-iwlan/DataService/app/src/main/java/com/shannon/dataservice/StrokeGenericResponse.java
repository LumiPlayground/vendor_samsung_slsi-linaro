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

public class StrokeGenericResponse extends StrokeResponseMessage {
    private final String mName;
    private final ResponseErrorStatus mStatus;
    private final ProcessedNotifyType mNotify;

    public StrokeGenericResponse(StrokeResponseMessage from) {
        super(from);
        mName = getString();
        mStatus = ResponseErrorStatus.getEnum(getInt());
        mNotify = ProcessedNotifyType.getEnum(getInt());
    }

    public String getName() { return mName; }
    public ResponseErrorStatus getStatus() { return mStatus; }
    public ProcessedNotifyType getNotify() { return mNotify; }

    @Override
    public String toString() {
        return ("" + getType() + " name=" + mName + " status=" + mStatus + " notify=" + mNotify);
    }
}
