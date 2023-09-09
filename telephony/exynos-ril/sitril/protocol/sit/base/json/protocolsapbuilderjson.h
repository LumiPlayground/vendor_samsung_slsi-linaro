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
 * protocolsapbuilderjson.h
 *
 *  Created on: 2021. 8. 3.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SAP_BUILDER_JSON_H__
#define __PROTOCOL_SAP_BUILDER_JSON_H__

#include "protocoljsonbuilder.h"
#include "protocolsapbuilderinterface.h"

class ProtocolSapBuilderJson : public ProtocolJsonBuilder, public ProtocolSapBuilderInterface {
  public:
    ProtocolSapBuilderJson()
        : ProtocolJsonBuilder(), ProtocolSapBuilderInterface() {}
    virtual ~ProtocolSapBuilderJson() {}

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

#endif /* __PROTOCOL_SAP_BUILDER_JSON_H__ */
