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
 * oemmischandlerfactory.cpp
 *
 *  Created on: 2021. 3.23.
 */

#include "oemmischandlerfactory.h"
#include "deviceinfo.h"
#include "messageid.h"
#include "miscdebug.h"
#include "oem_omadm.h"
#include "oem_deviceinfo.h"
#include "oem_modeminfo.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(OemMiscHandlerFactory)

BEGIN_FACTORY_ENTRY(OemMiscHandlerFactory)
    ADD_FACTORY_ENTRY(PinControlHandler, MSG_MISC_OEM_SET_PIN_CONTROL)
    ADD_FACTORY_ENTRY(UnsolPinControlHandler, MSG_MISC_UNSOL_OEM_PIN_CONTROL)
    ADD_FACTORY_ENTRY(GetRadioNodeHandler, MSG_MISC_OEM_GET_RADIO_NODE)
    ADD_FACTORY_ENTRY(SetRadioNodeHandler, MSG_MISC_OEM_SET_RADIO_NODE)
    ADD_FACTORY_ENTRY(GetProvisionUpdateRequestHandler, MSG_MISC_OEM_GET_PROVISION_UPDATE_REQUEST)
    ADD_FACTORY_ENTRY(SetProvisionUpdateDoneRequestHandler, MSG_MISC_OEM_SET_PROVISION_UPDATE_DONE_REQUEST)
    ADD_FACTORY_ENTRY(MslCodeHandler, MSG_MISC_OEM_GET_MSL_CODE)
    ADD_FACTORY_ENTRY(GetRfDesenseModeHandler, MSG_MISC_OEM_GET_RF_DESENSE_MODE)
    ADD_FACTORY_ENTRY(SetRfDesenseModeHandler, MSG_MISC_OEM_SET_RF_DESENSE_MODE)
    ADD_FACTORY_ENTRY(GetManualBandModeHandler, MSG_MISC_OEM_GET_MANUAL_BAND_MODE)
    ADD_FACTORY_ENTRY(SetManualBandModeHandler, MSG_MISC_OEM_SET_MANUAL_BAND_MODE)
    ADD_FACTORY_ENTRY(ModemInfoHandler, MSG_MISC_OEM_MODEM_INFO)
    ADD_FACTORY_ENTRY(ModemInfoReceivedHandler, MSG_MISC_OEM_MODEM_INFO_RECEIVED)
    ADD_FACTORY_ENTRY(ModemResetHandler, MSG_MISC_OEM_MODEM_RESET)
    ADD_FACTORY_ENTRY(SwitchModemFunctionHandler, MSG_MISC_OEM_SET_FUNC_SWITCH_REQ)
    ADD_FACTORY_ENTRY(SetPdcpDiscardTimerHandler, MSG_MISC_OEM_SET_PDCP_DISCARD_TIMER)
    ADD_FACTORY_ENTRY(SetSelflogHandler, MSG_MISC_OEM_SET_SELFLOG)
    ADD_FACTORY_ENTRY(SetSelfLogProfileHandler, MSG_MISC_OEM_SET_SELFLOG_PROFILE)
    ADD_FACTORY_ENTRY(GetSelfLogStatusHandler, MSG_MISC_OEM_GET_SELFLOG_STATUS)
    ADD_FACTORY_ENTRY(GetCqiInfoHandler, MSG_MISC_OEM_GET_CQI_INFO)
    ADD_FACTORY_ENTRY(SetSarSettingHandler, MSG_MISC_OEM_SET_SAR_SETTING)
    ADD_FACTORY_ENTRY(SetGmoSwitchHandler, MSG_MISC_OEM_SET_GMO_SWITCH)
    ADD_FACTORY_ENTRY(SetTcsFciHandler, MSG_MISC_OEM_SET_TCS_FCI_REQ)
    ADD_FACTORY_ENTRY(GetTcsFciHandler, MSG_MISC_OEM_GET_TCS_FCI_INFO)
    ADD_FACTORY_ENTRY(SetCABandwidthFilterHandler, MSG_MISC_OEM_SET_CA_BW_FILTER)
    ADD_FACTORY_ENTRY(CABandwidthFilterIndHandler, MSG_MISC_OEM_CA_BW_FILTER_IND)
    ADD_FACTORY_ENTRY(SetElevatortSensorHandler, MSG_MISC_OEM_SET_ELEVATOR_SENSOR)
    ADD_FACTORY_ENTRY(EndcCapabilityReceivedHandler, MSG_MISC_OEM_IND_NTW_ENDC_CAPABILITY)
    ADD_FACTORY_ENTRY(SetForbidLteCellHandler, MSG_MISC_OEM_SET_FORBID_LTE_CELL)
    ADD_FACTORY_ENTRY(BaseStationReceivedHandler, MSG_MISC_OEM_IND_BASESTATION_INFO)
    ADD_FACTORY_ENTRY(GetModemActivityInfoHandler, MSG_MISC_OEM_GET_MODEM_ACTIVITY_INFO)
    ADD_FACTORY_ENTRY(SetSvnHandler, MSG_MISC_OEM_SET_SVN)
    ADD_FACTORY_ENTRY(DroppedMtCallInfoReceivedHandler, MSG_MISC_OEM_IND_DROPPED_MT_CALL_INFO)
    ADD_FACTORY_ENTRY(SetForbidFrequencyHandler, MSG_MISC_OEM_SET_FORBID_FREQUENCY)
    ADD_FACTORY_ENTRY(SetForbidLteBandHandler, MSG_MISC_OEM_SET_FORBID_LTE_BAND)
    ADD_FACTORY_ENTRY(SetForbidLteTacHandler, MSG_MISC_OEM_SET_FORBID_LTE_TAC)
    ADD_FACTORY_ENTRY(SetSelflogTcpDumpModeHandler, MSG_MISC_OEM_SET_SELFLOG_TCP_DUMP_MODE)
    ADD_FACTORY_ENTRY(SetAllDivSettingHandler, MSG_MISC_OEM_SET_ALL_DIV_SETTING)
    ADD_FACTORY_ENTRY(SetBsrParaHandler, MSG_MISC_OEM_SET_BSR_PARA)
    ADD_FACTORY_ENTRY(SetAheadRreHandler, MSG_MISC_OEM_SET_AHEAD_RRE)
    ADD_FACTORY_ENTRY(SetDrdvHandler, MSG_MISC_OEM_SET_DRDV)
    ADD_FACTORY_ENTRY(SetEmcLabTestModeHandler, MSG_MISC_OEM_SET_EMC_LAB_TEST_MODE)
    ADD_FACTORY_ENTRY(SetForbidNrCellHandler, MSG_MISC_OEM_SET_FORBID_NR_CELL)
    ADD_FACTORY_ENTRY(GetCdmaCapabilityInfoHandler, MSG_MISC_OEM_GET_CDMA_CAPABILITY_INFO)
    ADD_FACTORY_ENTRY(SetTelephonyStatusLoggingHandler, MSG_MISC_OEM_SET_TELEPHONY_STATUS_LOGGING)
    ADD_FACTORY_ENTRY(TestUnsolHandler, MSG_MISC_OEM_TEST_UNSOL)
    ADD_FACTORY_ENTRY(UpdateEmergencyNumberListHandler, MSG_MISC_OEM_UPDATE_EMERGENCY_NUMBER_LIST)
    ADD_FACTORY_ENTRY(SetUeOperationModeHandler, MSG_MISC_OEM_SET_UE_OPERATION_MODE)
    ADD_FACTORY_ENTRY(GetUeOperationModeHandler, MSG_MISC_OEM_GET_UE_OPERATION_MODE)
    ADD_FACTORY_ENTRY(SetNrrrmMinReqEnableHandler, MSG_MISC_OEM_SET_NRRRM_MIN_REQ_ENABLE)
    ADD_FACTORY_ENTRY(GetNrrrmMinReqEnableHandler, MSG_MISC_OEM_GET_NRRRM_MIN_REQ_ENABLE)
    ADD_FACTORY_ENTRY(SetPwrDetectThrHandler, MSG_MISC_OEM_SET_PWR_DETECT_THR)
    ADD_FACTORY_ENTRY(SetVcrtHandler, MSG_MISC_OEM_SET_VCRT)
    ADD_FACTORY_ENTRY(SetEnableN28, MSG_MISC_OEM_SET_ENABLE_N28)
    ADD_FACTORY_ENTRY(SetUlMimo, MSG_MISC_SET_UL_MIMO)
    ADD_FACTORY_ENTRY(GetBinaryVersionInfoHandler, MSG_MISC_GET_BINARY_VERSION_INFO)
    ADD_FACTORY_ENTRY(SetBtlBufMaxSizeHandler, MSG_MISC_OEM_SET_BTL_BUFMAX_SIZE)
    ADD_FACTORY_ENTRY(GetVcrtHandler, MSG_MISC_OEM_GET_VCRT)
    ADD_FACTORY_ENTRY(SetNrDisableBandHandler, MSG_MISC_SET_NR_DISABLE_BAND)
END_FACTORY_ENTRY()
