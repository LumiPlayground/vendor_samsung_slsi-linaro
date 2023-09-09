
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
#include "deviceconfighandlerfactory.h"
#include "deviceinfo.h"
#include "messageid.h"
#include "miscdebug.h"
#include "oem_omadm.h"
#include "service.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(DeviceConfigHandlerFactory)

BEGIN_FACTORY_ENTRY(DeviceConfigHandlerFactory)
    ADD_FACTORY_ENTRY(BasebandVersionHandler, MSG_MISC_BASEBAND_VER)
    ADD_FACTORY_ENTRY(ImeiHandler, MSG_MISC_GET_IMEI)
    ADD_FACTORY_ENTRY(ImeisvHandler, MSG_MISC_GET_IMEISV)
    ADD_FACTORY_ENTRY(DeviceIdentityHandler, MSG_MISC_DEV_IDENTITY)
    ADD_FACTORY_ENTRY(SetEngModeHandler, MSG_MISC_OEM_SET_ENG_MODE)
    ADD_FACTORY_ENTRY(SetScreenLineHandler, MSG_MISC_OEM_SET_SCREEN_LINE)
    ADD_FACTORY_ENTRY(SetEngStringInputHandler, MSG_MISC_OEM_SET_ENG_STRING_INPUT)
    ADD_FACTORY_ENTRY(EngModeDisplayIndHandler, MSG_MISC_UNSOL_OEM_DISPLAY_ENG)
    ADD_FACTORY_ENTRY(DebugTraceHandler, MSG_MISC_OEM_SET_DEBUG_TRACE)
    ADD_FACTORY_ENTRY(SGCHandler, MSG_MISC_OEM_SEND_SGC)
    ADD_FACTORY_ENTRY(DeviceInfoHandler, MSG_MISC_SET_DEVICE_INFO)
    ADD_FACTORY_ENTRY(OpenCarrierInfoHandler, MSG_MISC_SET_OPEN_CARRIER_INFO)
    ADD_FACTORY_ENTRY(HwConfigHandler, MSG_MISC_GET_HARDWARE_CONFIG)
    ADD_FACTORY_ENTRY(HwConfigChangedHandler, MSG_MISC_IND_HARDWARE_CONFIG_CHANGED)
    ADD_FACTORY_ENTRY(SetCarrierInfoImsiEncryptionHandler, MSG_MISC_SET_CARRIER_INFO_IMSI_ENCRYPTION)
    ADD_FACTORY_ENTRY(UnsolCarrierInfoImsiEncryptionHandler, MSG_MISC_IND_CARRIER_INFO_IMSI_ENCRYPTION)
    ADD_FACTORY_ENTRY(SignalStrengthReportingCriteriaHandler, MSG_MISC_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA)
    ADD_FACTORY_ENTRY(OemSignalStrengthReportingCriteriaHandler, MSG_MISC_OEM_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA)
    ADD_FACTORY_ENTRY(LinkCapacityReportingCriteriaHandler, MSG_MISC_SET_LINK_CAPACITY_REPORTING_CRITERIA)
    ADD_FACTORY_ENTRY(RadioConfigResetHandler, MSG_MISC_OEM_RADIO_CONFIG_RESET)
    ADD_FACTORY_ENTRY(GetPlmnNameFromSE13TableHandler, MSG_MISC_GET_PLMN_NAME_FROM_SE13TABLE)
    ADD_FACTORY_ENTRY(Ts25TableDumpHandler, MSG_MISC_TS25TABLE_DUMP)
END_FACTORY_ENTRY()
