/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

import android.telephony.data.NetworkSliceInfo;

public class IkeNotifyN1ModeInformation extends IkeNotifyN1ModeData {
    private final int DATA_LENGTH_SST = 1;
    private final int DATA_LENGTH_SD = 3;

    public IkeNotifyN1ModeInformation(IkeNotifyData from) {
        super(from);
    }

    public NetworkSliceInfo getSliceInfo() {
        byte[] data = getData();
        if (data != null) {
            NetworkSliceInfo.Builder builder = new NetworkSliceInfo.Builder();
            int offs = 0;

            final int BUFFER_LENGTH_SST_H_SST = DATA_LENGTH_SST * 2;
            final int BUFFER_LENGTH_SST_SD = DATA_LENGTH_SST + DATA_LENGTH_SD;
            final int BUFFER_LENGTH_SST_SD_H_SST = (DATA_LENGTH_SST * 2) + DATA_LENGTH_SD;
            final int BUFFER_LENGTH_ALL = (DATA_LENGTH_SST * 2) + (DATA_LENGTH_SD * 2);
            switch (data.length) {
                case DATA_LENGTH_SST: {
                    builder.setSliceServiceType(StringUtils.getByteAsInt(data, offs));
                    break;
                }
                case BUFFER_LENGTH_SST_H_SST: {
                    builder.setSliceServiceType(StringUtils.getByteAsInt(data, offs));
                    builder.setMappedHplmnSliceServiceType(StringUtils.getByteAsInt(data, ++offs));
                    break;
                }
                case BUFFER_LENGTH_SST_SD: {
                    int value = StringUtils.getInt(data, offs);
                    builder.setSliceServiceType((value >> 24) & 0xFF);
                    try {
                        builder.setSliceDifferentiator(value & 0x00FFFFFF);
                    } catch (Exception e) { /* empty */ }
                    break;
                }
                case BUFFER_LENGTH_SST_SD_H_SST: {
                    int value = StringUtils.getInt(data, offs);
                    builder.setSliceServiceType((value >> 24) & 0xFF);
                    try {
                        builder.setSliceDifferentiator(value & 0x00FFFFFF);
                    } catch (Exception e) { /* empty */ }
                    offs += (DATA_LENGTH_SST + DATA_LENGTH_SD);
                    builder.setMappedHplmnSliceServiceType(StringUtils.getByteAsInt(data, offs));
                    break;
                }
                case BUFFER_LENGTH_ALL: {
                    int value = StringUtils.getInt(data, offs);
                    builder.setSliceServiceType((value >> 24) & 0xFF);
                    try {
                        builder.setSliceDifferentiator(value & 0x00FFFFFF);
                    } catch (Exception e) { /* empty */ }
                    offs += (DATA_LENGTH_SST + DATA_LENGTH_SD);
                    value = StringUtils.getInt(data, offs);
                    builder.setMappedHplmnSliceServiceType((value >> 24) & 0xFF);
                    try {
                        builder.setMappedHplmnSliceDifferentiator(value & 0x00FFFFFF);
                    } catch (Exception e) { /* empty */ }
                    break;
                }
                default: return null;
            }
            return builder.build();
        }
        return null;
    }
}
