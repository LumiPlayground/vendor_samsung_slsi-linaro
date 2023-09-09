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
 * protocolsapbuilderlegacy.h
 *
 *  Created on: 2021. 08. 2.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_SAP_BUILDER_LEGACY_H__
#define __PROTOCOL_SAP_BUILDER_LEGACY_H__

#include "protocolbuilder.h"
#include "protocolsapbuilderinterface.h"

class ProtocolSapBuilderLegacy : public ProtocolBuilder, public ProtocolSapBuilderInterface {
  public:
    ProtocolSapBuilderLegacy()
        : ProtocolBuilder(), ProtocolSapBuilderInterface() {}
    virtual ~ProtocolSapBuilderLegacy() {}

  public:
    ModemData *BuildSapConnect(int maxMsgSize);
    ModemData *BuildSapDisconnect();
    ModemData *BuildSapTransferApdu(int apduLeng, char *apdu);
    ModemData *BuildSapTransferAtr();
    ModemData *BuildSapPowerSimOnOff(bool state);
    ModemData *BuildSapResetSim();
    ModemData *BuildSapTransferCardReaderStatus();
    ModemData *BuildSapTranportProtocol(int protocol);
};

#endif /* __PROTOCOL_SAP_BUILDER_LEGACY_H__ */
