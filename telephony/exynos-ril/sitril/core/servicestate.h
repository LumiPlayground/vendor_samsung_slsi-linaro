 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <slsi/radio_v1_5.h>
#include <string>

class ServiceState
{
private:
    int mVoiceRegState;
    int mDataRegState;
    std::string mOperatorNumeric;
    std::string mOperatorAlphaLong;
    std::string mOperatorAlphaShort;
    std::string mOperatorNumericRaw;
    std::string mOperatorAlphaLongRaw;
    std::string mOperatorAlphaShortRaw;
    bool mIsManualNetworkSelection;
    int mVoiceRadioTechnology;
    int mDataRadioTechnology;
    int mCssIndicator;
    int mNetworkId;
    int mSystemId;
    int mCdmaRoamingIndicator;
    int mCdmaDefaultRoamingIndicator;
    int mCdmaEriIconIndex;
    int mCdmaEriIconMode;
    int mIsEmergencyOnly;
    int mIsUsingCarrierAggregation;
    int mChannelNumber;
    RIL_LteVopsInfo mLteVopsInfo;
    RIL_NrIndicators mNrIndicators;
    int mNrStatus;
    int mNrFrequencyRange;
public:
    ServiceState();
    void setNullState();
    ServiceState& operator=(const ServiceState& rhs);
    bool operator==(const ServiceState& rhs) const;
    bool operator!=(const ServiceState& rhs) const;
    std::string toString() const;

    int getVoiceRegState() const { return mVoiceRegState; }
    int getDataRegState() const { return mDataRegState; }
    const std::string& getOperatorNumeric() const {
        return mOperatorNumeric;
    }
    const std::string& getOperatorAlphaLong() const {
        return mOperatorAlphaLong;
    }
    const std::string& getOperatorAlphaShort() const {
        return mOperatorAlphaShort;
    }
    const std::string& getOperatorNumericRaw() const {
        return mOperatorNumericRaw;
    }
    const std::string& getOperatorAlphaLongRaw() const {
        return mOperatorAlphaLongRaw;
    }
    const std::string& getOperatorAlphaShortRaw() const {
        return mOperatorAlphaShortRaw;
    }
    bool getIsManualSelection() { return mIsManualNetworkSelection; }
    int getVoiceRadioTechnology() const { return mVoiceRadioTechnology; }
    int getDataRadioTechnology() const { return mDataRadioTechnology; }
    bool isEmergencyOnly() const { return mIsEmergencyOnly; }
    bool isUsingCarrierAggregation() const { return mIsUsingCarrierAggregation; }
    int getChannelNumber() const { return mChannelNumber; }
    bool getLteVopsSupport() const { return mLteVopsInfo.isVopsSupported; }
    bool getLteEmcBearerSupport() const { return mLteVopsInfo.isEmcBearerSupported; }
    int getNrStatus() const { return mNrStatus; }
    bool isEndcAvailable() const { return mNrIndicators.isEndcAvailable; }
    bool isDcNrRestricted() const { return mNrIndicators.isDcNrRestricted; }
    bool isNrAvailable() const { return mNrIndicators.isNrAvailable; }
    void setVoiceRegState(int voiceRegState);
    void setDataRegState(int dataRegState);
    void setOperatorNumeric(const std::string& numeric) {
        mOperatorNumeric = numeric;
    }
    void setOperatorNumericRaw(const std::string& numericRaw) {
        mOperatorNumericRaw = numericRaw;
    }
    void setIsManualSelection(bool isManual) { mIsManualNetworkSelection = isManual; }
    void setVoiceRadioTechnology(int rt);
    void setDataRadioTechnology(int rt);
    void setOperatorName(const std::string& longName, const std::string& shortName);
    void setOperatorNameRaw(const std::string& longNameRaw, const std::string& shortNameRaw);
    void setEmergencyOnly(bool emergencyOnly) { mIsEmergencyOnly = emergencyOnly; }
    void setChannelNumber(int channelNumber) { mChannelNumber = channelNumber; }
    void setLteVopsSupport(bool vopsSupport) { mLteVopsInfo.isVopsSupported = vopsSupport; }
    void setLteEmcBearerSupport(bool emcBearerSupport) { mLteVopsInfo.isEmcBearerSupported = emcBearerSupport; }
    void updateNrStatus(bool isEndcAvailable, bool isDcNrRestricted, bool isNrAvailable);
    void setNrStatus(int nrStatus) { mNrStatus = nrStatus; }
    bool isVoiceOos() const;
    bool isDataOos() const;
};
