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
 * oem_modeminfo.h
 *
 *  Created on: 2020. 7. 9.
 *      Author: sungwoo48.choi
 */

#ifndef __OEM_MODEM_INFO_H__
#define __OEM_MODEM_INFO_H__

#include "servicehandlerbase.h"

/**
 * ModemInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ModemInfoHandler)

/**
 * ModemInfoReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(ModemInfoReceivedHandler)

/**
 * ModemResetHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(ModemResetHandler)

/**
 * SwitchModemFunctionHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SwitchModemFunctionHandler)

/**
 * SetPdcpDiscardTimerHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPdcpDiscardTimerHandler)

/**
 * SetSelflogHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSelflogHandler)

/**
 * SetSelfLogProfileHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSelfLogProfileHandler)

/**
 * GetSelfLogStatusHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetSelfLogStatusHandler)

/**
 * UnsolSelflogStatusHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(UnsolSelflogStatusHandler)

/**
 * SetActivateVsimHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetActivateVsimHandler)

/**
 * GetCqiInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCqiInfoHandler)

/**
 * SetSarSettingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSarSettingHandler)

/**
 * SetGmoSwitchHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetGmoSwitchHandler)

/**
 * SetTcsFciHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetTcsFciHandler)

/**
 * GetTcsFciHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetTcsFciHandler)

/**
 * SetCABandwidthFilterHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetCABandwidthFilterHandler)

/**
 * CABandwidthFilterIndHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(CABandwidthFilterIndHandler)

/**
 * SetModemLogDumpHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetModemLogDumpHandler)

/**
 * SetElevatortSensorHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetElevatortSensorHandler)

/**
 * EndcCapabilityReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(EndcCapabilityReceivedHandler)

/**
 * SetForbidLteCellHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetForbidLteCellHandler)

/**
 * SetApSystemTimeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetApSystemTimeHandler)

/**
 * BaseStationReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(BaseStationReceivedHandler)

/**
 * GetModemActivityInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetModemActivityInfoHandler)

/**
 * SetSvnHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSvnHandler)

/**
 * DroppedMtCallInfoReceivedHandler
 */
DECLARE_TINY_UNSOL_SERVICE_HANDLER_CLASS(DroppedMtCallInfoReceivedHandler)

/**
 * SetForbidFrequencyHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetForbidFrequencyHandler)

/**
 * SetForbidLteBandHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetForbidLteBandHandler)

/**
 * SetForbidLteTacHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetForbidLteTacHandler)

/**
 * SetSelflogTcpDumpModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSelflogTcpDumpModeHandler)

/**
 * SetAllDivSettingHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetAllDivSettingHandler)

/**
 * SetBsrParaHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetBsrParaHandler)

/**
 * SetAheadRreHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetAheadRreHandler)

/**
 * SetDrdvHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetDrdvHandler)

/**
 * SetEmcLabTestModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetEmcLabTestModeHandler)

/**
 * UpdateEmergencyNumberListHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(UpdateEmergencyNumberListHandler)

/**
 * SetForbidNrCellHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetForbidNrCellHandler)

/**
 * GetCdmaCapabilityInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetCdmaCapabilityInfoHandler)

/**
 * SetSrsPwrLimitHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetSrsPwrLimitHandler)

/**
 * SetTraceDumpCfgHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetTraceDumpCfgHandler)

/**
 * GetTraceDumpCfgHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetTraceDumpCfgHandler)

/**
 * SetUeOperationModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetUeOperationModeHandler)

/**
 * GetUeOperationModeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetUeOperationModeHandler)

/**
 * SetNrrrmMinReqEnableHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetNrrrmMinReqEnableHandler)

/**
 * GetNrrrmMinReqEnableHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetNrrrmMinReqEnableHandler)

/**
 * SetPwrDetectThrHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetPwrDetectThrHandler)

/**
 * SetVcrtHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetVcrtHandler)

/**
 * SetEnableN28
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetEnableN28)

/**
 * SetUlMimo
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetUlMimo)

/**
 * GetBinaryVersionInfoHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetBinaryVersionInfoHandler)

/**
 * SetBtlBufMaxSizeHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetBtlBufMaxSizeHandler)

/**
 * GetVcrtHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(GetVcrtHandler)

/*
* SetNrDisableBandHandler
 */
DECLARE_TINY_SERVICE_HANDLER_CLASS(SetNrDisableBandHandler)

#endif /* __OEM_MODEM_INFO_H__ */
