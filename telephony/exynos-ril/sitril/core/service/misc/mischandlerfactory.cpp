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
 * mischandlerfactory.cpp
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#include "mischandlerfactory.h"
#include "cdmainfo.h"
#include "deviceinfo.h"
#include "dtmf.h"
#include "messageid.h"
#include "miscdebug.h"
#include "oem_deviceinfo.h"
#include "oem_modeminfo.h"
#include "service.h"
#include "ttymode.h"
#include "carrierconfighandler.h"
#include "nvhandler.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(MiscHandlerFactory)

BEGIN_FACTORY_ENTRY(MiscHandlerFactory)
    ADD_FACTORY_ENTRY(ScreenStateHandler, MSG_MISC_SCREEN)
    ADD_FACTORY_ENTRY(DeviceStateHandler, MSG_MISC_SEND_DEVICE_STATE)
    ADD_FACTORY_ENTRY(DtmfHandler, MSG_MISC_DTMF)
    ADD_FACTORY_ENTRY(DtmfStartHandler, MSG_MISC_DTMF_START)
    ADD_FACTORY_ENTRY(DtmfStopHandler, MSG_MISC_DTMF_STOP)
    ADD_FACTORY_ENTRY(ModemRestartHandler, MSG_MISC_IND_MODEM_RESTART)
    ADD_FACTORY_ENTRY(SetIndicationFilterHandler, MSG_MISC_SET_INDICATION_FILTER)
    ADD_FACTORY_ENTRY(SendATCommandHandler, MSG_MISC_FORWARDING_AT_COMMAND)
    ADD_FACTORY_ENTRY(UnsolATCommandHandler, MSG_MISC_UNSOL_AT_COMMAND)
    ADD_FACTORY_ENTRY(PhoneResetHandler, MSG_MISC_PHONE_RESET)
    ADD_FACTORY_ENTRY(GetCdmaSubscriptionSourceHandler, MSG_MISC_CDMA_GET_SUBSCRIPT_SOURCE)
    ADD_FACTORY_ENTRY(SetCdmaSubscriptionSourceHandler, MSG_MISC_CDMA_SET_SUBSCRIPT_SOURCE)
    ADD_FACTORY_ENTRY(CdmaSubscriptionHandler, MSG_MISC_GET_CDMA_SUBSCRIPTION)
    ADD_FACTORY_ENTRY(CdmaPrlChangedHandler, MSG_MISC_IND_CDMA_PRL_CHANGED)
    ADD_FACTORY_ENTRY(SetPSensorStatusHandler, MSG_MISC_SET_PSENSOR_STATUS)
    ADD_FACTORY_ENTRY(SetSarStateHandler, MSG_MISC_SET_SAR_STATE)
    ADD_FACTORY_ENTRY(GetSarStateHandler, MSG_MISC_GET_SAR_STATE)
    ADD_FACTORY_ENTRY(UnsolSarControlStateHandler, MSG_MISC_SAR_CONTROL_STATE_IND)
    ADD_FACTORY_ENTRY(UnsolSarRfConnectionHandler, MSG_MISC_SAR_RF_CONNECTION_IND)
    ADD_FACTORY_ENTRY(RfConnectionStateHandler, MSG_MISC_RF_CONNECTION_STATE_IND)
    ADD_FACTORY_ENTRY(RssiScanHandler, MSG_MISC_SCAN_RSSI)
    ADD_FACTORY_ENTRY(RssiScanResultHandler, MSG_MISC_SCAN_RSSI_RESULT_RECEIVED)
    ADD_FACTORY_ENTRY(SetFeatureInfoHandler, MSG_MISC_SET_FEATURE_INFO)
    ADD_FACTORY_ENTRY(GetTtyModeHandler, MSG_MISC_QUERY_TTY)
    ADD_FACTORY_ENTRY(SetTtyModeHandler, MSG_AUDIO_SET_TTY_MODE)
    ADD_FACTORY_ENTRY(DummyRequestHandler, MSG_MISC_SET_TTY)
    ADD_FACTORY_ENTRY(UnsolSelflogStatusHandler, MSG_MISC_UNSOL_SELFLOG_STATUS)
    ADD_FACTORY_ENTRY(SetActivateVsimHandler, MSG_MISC_SET_ACTIVATE_VSIM)
    ADD_FACTORY_ENTRY(SetModemLogDumpHandler, MSG_MISC_SET_MODEM_LOG_DUMP)
    ADD_FACTORY_ENTRY(SetApSystemTimeHandler, MSG_MISC_SET_AP_SYSTEM_TIME)
    ADD_FACTORY_ENTRY(SetCpCarrierConfigHandler, MSG_MISC_SET_CP_CARRIER_CONFIG)
    ADD_FACTORY_ENTRY(UnsolCpCarrierConfigSimInfoHandler, MSG_MISC_CARRIER_CONFIG_SIM_INFO_IND)
    ADD_FACTORY_ENTRY(UnsolCpCarrierConfigStatusHandler, MSG_MISC_CARRIER_CONFIG_STATUS_IND)
    ADD_FACTORY_ENTRY(SetSrsPwrLimitHandler, MSG_MISC_OEM_SET_SRS_PWR_LIMIT)
    ADD_FACTORY_ENTRY(SetTraceDumpCfgHandler, MSG_MISC_OEM_SET_TRACE_DUMP_CFG)
    ADD_FACTORY_ENTRY(GetTraceDumpCfgHandler, MSG_MISC_OEM_GET_TRACE_DUMP_CFG)
    ADD_FACTORY_ENTRY(NvResetConfigHandler, MSG_MISC_NV_RESET_CONFIG)
END_FACTORY_ENTRY()
