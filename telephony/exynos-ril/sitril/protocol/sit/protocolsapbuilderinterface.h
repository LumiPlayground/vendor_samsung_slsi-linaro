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
 * protocolsapbuilderinterface.h
 *
 *  Created on: 2021. 08. 2.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_SAP_BUILDER_INTERFACE_H__
#define __PROTOCOL_SAP_BUILDER_INTERFACE_H__

class ModemData;

class ProtocolSapBuilderInterface {
public:
    ProtocolSapBuilderInterface() {}
    virtual ~ProtocolSapBuilderInterface() {}
public:
    virtual ModemData *BuildSapConnect(int maxMsgSize) = 0;
    virtual ModemData *BuildSapDisconnect() = 0;
    virtual ModemData *BuildSapTransferApdu(int apduLeng, char *apdu) = 0;
    virtual ModemData *BuildSapTransferAtr() = 0;
    virtual ModemData *BuildSapPowerSimOnOff(bool state) = 0;
    virtual ModemData *BuildSapResetSim() = 0;
    virtual ModemData *BuildSapTransferCardReaderStatus() = 0;
    virtual ModemData *BuildSapTranportProtocol(int protocol) = 0;
};

#endif /* __PROTOCOL_SAP_BUILDER_INTERFACE_H__ */
