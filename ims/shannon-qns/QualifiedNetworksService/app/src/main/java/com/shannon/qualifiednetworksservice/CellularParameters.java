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
package com.shannon.qualifiednetworksservice;

import static com.shannon.qualifiednetworksservice.ConditionCellular.MIN_LTE_RSRP;

public class CellularParameters {
    private int mSignal;
    private int mNoise;
    private int mQuality;

    public CellularParameters() {
        mSignal = MIN_LTE_RSRP;
        mNoise = 0;
        mQuality = 0;
    }

    public CellularParameters(int signal) {
        mSignal = signal;
        mNoise = 0;
        mQuality = 0;
    }

    public CellularParameters(int signal, int noise, int quality) {
        mSignal = signal;
        mNoise = noise;
        mQuality = quality;
    }

    public void setSignalThreshold(int signal) { mSignal = signal; }
    public void setNoiseThreshold(int noise) { mNoise = noise; }
    public void setQualityThreshold(int quality) { mQuality = quality; }
    public int getSignalThreshold() { return mSignal; }
    public int getSignal() { return mSignal; }
    public int getNoiseThreshold() { return mNoise; }
    public int getNoise() { return mNoise; }
    public int getQualityThreshold() { return mQuality; }
    public int getQuality() { return mQuality; }
    public boolean isNoiseNotValid() { return mNoise == 0; }
    public boolean isNoiseValid() { return mNoise != 0; }
    public boolean isQualityNotValid() { return mQuality == 0; }
    public boolean isQualityValid() { return mQuality != 0; }

    public boolean isBetter(CellularParameters other) {
        return (getSignal() >= other.getSignalThreshold()) &&
                (other.isNoiseNotValid() || isNoiseNotValid() || getNoise() >= other.getNoiseThreshold()) &&
                (other.isQualityNotValid() || isQualityNotValid() || getQuality() >= other.getQualityThreshold());
    }

    public boolean isWorse(CellularParameters other) {
        return (getSignal() <= other.getSignalThreshold()) ||
                (other.isNoiseValid() && isNoiseValid() && getNoise() <= other.getNoiseThreshold()) ||
                (other.isQualityValid() && isQualityValid() && getQuality() <= other.getQualityThreshold());
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof CellularParameters) {
            if (obj == this) {
                return true;
            }
            CellularParameters other = (CellularParameters)obj;
            return ((getSignal() == other.getSignal()) &&
                    (getQuality() == other.getQuality()) &&
                    (getNoise() == other.getNoise()));
        }
        return false;
    }

    @Override
    public String toString() {
        return ("Parameters: signal=" + mSignal + " noise=" + mNoise + " quality=" + mQuality);
    }
}
