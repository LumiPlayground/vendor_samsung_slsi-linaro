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

public class StrokeInitiateResponse extends StrokeGenericResponse {
    private final String mAdapter;
    private final String mIp;
    private final byte[] mAttributes;
    private final byte[] mNotifies;
    private final int mMtu;

    public StrokeInitiateResponse(StrokeResponseMessage from) {
        super(from);

        mAdapter = getString();
        mIp = getString();
        mAttributes = getArray();
        mNotifies = getArray();
        mMtu = getInt();
    }

    public String getAdapter() { return mAdapter; }
    public String getIpAddresses() { return mIp; }
    public byte[] getAttributes() { return mAttributes; }
    public byte[] getNotifies() { return mNotifies; }
    public int getMtu() { return mMtu; }

    @Override
    public String toString() {
        return (super.toString() + " adapter=" + mAdapter + " address=" + mIp + " MTU=" + mMtu);
    }
}
