/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include "protocolsmsadapterinterface.h"
#include "modemdata.h"

class ProtocolSmsAdapterFactory {
private:
    static std::shared_ptr<ProtocolSmsAdapterFactory> protocolSmsAdapterFactory;

public:
    virtual ~ProtocolSmsAdapterFactory() { }

    static void createInstance();
    static std::shared_ptr<ProtocolSmsAdapterFactory> getInstance();

    virtual ProtocolSendSmsRespAdapterInterface*
         createProtocolSendSmsRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolWriteSmsToSimRespAdapterInterface*
         createProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolSmsCapacityOnSimRespAdapterInterface*
         createProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolSmscAddrRespAdapterInterface*
         createProtocolSmscAddrRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolNewSmsIndAdapterInterface*
         createProtocolNewSmsIndAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolGetBcstSmsConfRespAdapterInterface*
         createProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolNewBcstSmsAdapterInterface*
         createProtocolNewBcstSmsAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolCdmaSendSmsRespAdapterInterface*
         createProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolCdmaNewSmsIndAdapterInterface*
         createProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolCdmaWriteSmsToRuimRespAdapterInterface*
         createProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolGetCdmaBcstSmsConfRespAdapterInterface*
         createProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData) = 0;
    virtual ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface*
         createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData) = 0;

    // SIT_IND_NEW_SMS_ON_SIM is not in use as per CP JSON doc
    // hence there is no JSON adapter for this IPC, providing
    // implementation in parent class by redirecting to legacy adapter
    ProtocolNewSmsOnSimIndAdapterInterface*
        createProtocolNewSmsOnSimIndAdapter(const ModemData *pModemData);
};
