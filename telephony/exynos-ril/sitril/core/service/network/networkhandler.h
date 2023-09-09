/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * networkhandler.h
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#ifndef __NETWORK_HANDLER_H__
#define __NETWORK_HANDLER_H__

#include "servicecallback.h"
#include "servicehandlerbase.h"

/**
 * GetVoiceRadioTechHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetVoiceRadioTechHandler)

/**
 * SetBandModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetBandModeHandler)

/**
 * QueryAvailableBandModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(QueryAvailableBandModeHandler)

/**
 * AllowDataModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(AllowDataModeHandler)

/**
 * SetUnsolCellInfoListRateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetUnsolCellInfoListRateHandler)

/**
 * GetDuplexModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetDuplexModeHandler)

/**
 * SetDuplexModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetDuplexModeHandler)

/**
 * QueryNetworkSelectionModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(QueryNetworkSelectionModeHandler)

/**
 * SetNetworkSelectionAutoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetNetworkSelectionAutoHandler)

/**
 * GetCellInfoListHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCellInfoListHandler)

/**
 * CellInfoListReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CellInfoListReceivedHandler)

/**
 * SetDSNetworkTypeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetDSNetworkTypeHandler)

/**
 * GetPreferredNetworkTypeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetPreferredNetworkTypeHandler)

/**
 * SetPreferredNetworkTypeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPreferredNetworkTypeHandler)

/**
 * SetDualNetworkTypeAndAllowDataHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetDualNetworkTypeAndAllowDataHandler)

/**
 * StartNetworkScanHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(StartNetworkScanHandler)

/**
 * StopNetworkScanHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(StopNetworkScanHandler)

/**
 * NetworkScanResultHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(NetworkScanResultHandler)

/**
 * SetEndcModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetEndcModeHandler)

/**
 * GetEndcModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetEndcModeHandler)

/**
 * SetNrModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetNrModeHandler)

/**
 * GetNrModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetNrModeHandler)

/**
 * SetNrSilenceModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetNrSilenceModeHandler)
\
/**
 * GetNrSilenceModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetNrSilenceModeHandler)

/**
 * SetPsServiceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPsServiceHandler)

/**
 * GetPsServiceHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetPsServiceHandler)

/**
 * SetMicroCellSearchHandler
 */
class SetMicroCellSearchHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(SetMicroCellSearchHandler)
protected:
    int OnTimeout(Message *msg);
};

/**
 * GetRadioCapabilityHandler
 */
class GetRadioCapabilityHandler : public ServiceHandlerBase {
    DECLARE_TINY_SERVICE_HANDLER_INIT(GetRadioCapabilityHandler)
protected:
    int OnTimeout(Message *msg);
};

/**
 * SetRadioCapabilityHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetRadioCapabilityHandler)

/**
 * RadioCapabilityChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(RadioCapabilityChangedHandler)

/**
 * TotalOostHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(TotalOostHandler)

/**
 * MccIndtHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(MccIndtHandler)

/**
 * SetSysSelChannelsHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSysSelChannelsHandler)

/**
 * GetBarringInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetBarringInfoHandler)

/**
 * BarringInfoChangedHandler
 */
class BarringInfoChangedHandler : public ServiceHandlerBase,
        public ServiceCallback::RadioStateListener {
    DECLARE_TINY_UNSOL_SERVICE_HANDLER_INIT(BarringInfoChangedHandler)
protected:
    void OnCreate();
    void OnDestroy();
public:
    void OnRadioOffOrNotAvailable();
};

/**
 * UnsolRegistrationFailedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolRegistrationFailedHandler)

/**
 * QueryEmergencyCallAvailableRadioTechHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(QueryEmergencyCallAvailableRadioTechHandler)

/**
 * SetEmergencyCallStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetEmergencyCallStatusHandler)

/**
 * EmergencyActInfoReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(EmergencyActInfoReceivedHandler)

/**
 * SetAllowedNetworkTypeBitmapHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetAllowedNetworkTypeBitmapHandler)

/**
 * GetAllowedNetworkTypeBitmapHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetAllowedNetworkTypeBitmapHandler)

/**
 * SetPlmnListHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPlmnListHandler)

/**
 * GetPlmnListHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetPlmnListHandler)

/**
 * SetImsRegistrationStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetImsRegistrationStateHandler)

/**
 * SetMobileDataStateHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetMobileDataStateHandler)

/**
 * CurrentPhysicalChannelConfigsHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CurrentPhysicalChannelConfigsHandler)

/**
 * DsdsStatusChangedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(DsdsStatusChangedHandler)

#endif /* __NETWORK_HANDLER_H__ */
