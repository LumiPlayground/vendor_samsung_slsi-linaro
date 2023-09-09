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
 * protocolsmsadapter.cpp
 *
 *  Created on: 2014. 7. 3.
 *      Author: sungwoo48.choi
 */

#include "protocolsmsadapter.h"
#include "factory/sms/protocolsmsadapterfactory.h"
#include "legacy/protocolsmsadapterlegacy.h"
#include "json/protocolsmsadapterjson.h"

/**
 * ProtocolSendSmsRespAdapter
 */
ProtocolSendSmsRespAdapter::ProtocolSendSmsRespAdapter(const ModemData *pModemData)
    : ProtocolSendSmsRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolSendSmsRespAdapterInterface = factory->createProtocolSendSmsRespAdapter(pModemData);
}

ProtocolSendSmsRespAdapter::~ProtocolSendSmsRespAdapter() {
    delete protocolSendSmsRespAdapterInterface;
    protocolSendSmsRespAdapterInterface = NULL;
}

int ProtocolSendSmsRespAdapter::GetRef() const {
    return protocolSendSmsRespAdapterInterface->GetRef();
}

int ProtocolSendSmsRespAdapter::GetSmsRspErrorCode() const {
    return protocolSendSmsRespAdapterInterface->GetSmsRspErrorCode();
}

int ProtocolSendSmsRespAdapter::GetPduSize() const {
    return protocolSendSmsRespAdapterInterface->GetPduSize();
}

const char *ProtocolSendSmsRespAdapter::GetPdu() const {
    return protocolSendSmsRespAdapterInterface->GetPdu();
}

/**
 * ProtocolWriteSmsRespAdapter
 */
ProtocolWriteSmsToSimRespAdapter::ProtocolWriteSmsToSimRespAdapter(const ModemData *pModemData)
    : ProtocolWriteSmsToSimRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolWriteSmsToSimRespAdapterInterface = factory->createProtocolWriteSmsToSimRespAdapter(pModemData);
}

ProtocolWriteSmsToSimRespAdapter::~ProtocolWriteSmsToSimRespAdapter() {
    delete protocolWriteSmsToSimRespAdapterInterface;
    protocolWriteSmsToSimRespAdapterInterface = NULL;
}

int ProtocolWriteSmsToSimRespAdapter::GetIndex() const {
    return protocolWriteSmsToSimRespAdapterInterface->GetIndex();
}

/**
 * ProtocolNewBcstSmsAdapter
 */
ProtocolNewBcstSmsAdapter::ProtocolNewBcstSmsAdapter(const ModemData *pModemData)
    : ProtocolNewBcstSmsAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolNewBcstSmsAdapterInterface = factory->createProtocolNewBcstSmsAdapter(pModemData);
}

ProtocolNewBcstSmsAdapter::~ProtocolNewBcstSmsAdapter() {
    delete protocolNewBcstSmsAdapterInterface;
    protocolNewBcstSmsAdapterInterface = NULL;
}

BYTE *ProtocolNewBcstSmsAdapter::GetBcst() const {
    return protocolNewBcstSmsAdapterInterface->GetBcst();
}

UINT16 ProtocolNewBcstSmsAdapter::GetBcstLen() const {
    return protocolNewBcstSmsAdapterInterface->GetBcstLen();
}

/**
 * ProtocolSmscAddrRespAdapter
 */
ProtocolSmscAddrRespAdapter::ProtocolSmscAddrRespAdapter(const ModemData *pModemData)
    : ProtocolSmscAddrRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolSmscAddrRespAdapterInterface = factory->createProtocolSmscAddrRespAdapter(pModemData);
}

ProtocolSmscAddrRespAdapter::~ProtocolSmscAddrRespAdapter() {
    delete protocolSmscAddrRespAdapterInterface;
    protocolSmscAddrRespAdapterInterface = NULL;
}

const char *ProtocolSmscAddrRespAdapter::GetSmsc() const {
    return protocolSmscAddrRespAdapterInterface->GetSmsc();
}

int ProtocolSmscAddrRespAdapter::GetSmscLength() const {
    return protocolSmscAddrRespAdapterInterface->GetSmscLength();
}

/**
 * ProtocolNewSmsIndAdapter
 */
ProtocolNewSmsIndAdapter::ProtocolNewSmsIndAdapter(const ModemData *pModemData)
    : ProtocolNewSmsIndAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolNewSmsIndAdapterInterface = factory->createProtocolNewSmsIndAdapter(pModemData);
}

ProtocolNewSmsIndAdapter::~ProtocolNewSmsIndAdapter() {
    delete protocolNewSmsIndAdapterInterface;
    protocolNewSmsIndAdapterInterface = NULL;
}

int ProtocolNewSmsIndAdapter::GetPduSize() const {
    return protocolNewSmsIndAdapterInterface->GetPduSize();
}

const char *ProtocolNewSmsIndAdapter::GetPdu() const {
    return protocolNewSmsIndAdapterInterface->GetPdu();
}

int ProtocolNewSmsIndAdapter::GetTpid() const {
    return protocolNewSmsIndAdapterInterface->GetTpid();
}

/**
 * ProtocolNewSmsOnSimIndAdapter
 */
ProtocolNewSmsOnSimIndAdapter::ProtocolNewSmsOnSimIndAdapter(const ModemData *pModemData)
    : ProtocolNewSmsOnSimIndAdapterInterface() {
    protocolNewSmsOnSimIndAdapterInterface = new ProtocolNewSmsOnSimIndAdapterLegacy(pModemData);
}

ProtocolNewSmsOnSimIndAdapter::~ProtocolNewSmsOnSimIndAdapter() {
    delete protocolNewSmsOnSimIndAdapterInterface;
    protocolNewSmsOnSimIndAdapterInterface = NULL;
}

int ProtocolNewSmsOnSimIndAdapter::GetIndex() const {
    return protocolNewSmsOnSimIndAdapterInterface->GetIndex();
}

/**
 * ProtocolGetBcstSmsConfRespAdapter
 */
ProtocolGetBcstSmsConfRespAdapter::ProtocolGetBcstSmsConfRespAdapter(const ModemData *pModemData)
    : ProtocolGetBcstSmsConfRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolGetBcstSmsConfRespAdapterInterface = factory->createProtocolGetBcstSmsConfRespAdapter(pModemData);
}

ProtocolGetBcstSmsConfRespAdapter::~ProtocolGetBcstSmsConfRespAdapter() {
    delete protocolGetBcstSmsConfRespAdapterInterface;
    protocolGetBcstSmsConfRespAdapterInterface = NULL;
}

UINT8 ProtocolGetBcstSmsConfRespAdapter::GetConfigsNumber() {
    return protocolGetBcstSmsConfRespAdapterInterface->GetConfigsNumber();
}

RIL_GSM_BroadcastSmsConfigInfo **ProtocolGetBcstSmsConfRespAdapter::GetConfigsInfoPointers() {
    return protocolGetBcstSmsConfRespAdapterInterface->GetConfigsInfoPointers();
}

/**
 * ProtocolSmsCapacityOnSimRespAdapter
 */
ProtocolSmsCapacityOnSimRespAdapter::ProtocolSmsCapacityOnSimRespAdapter(const ModemData *pModemData)
    : ProtocolSmsCapacityOnSimRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolSmsCapacityOnSimRespAdapterInterface = factory->createProtocolSmsCapacityOnSimRespAdapter(pModemData);
}

ProtocolSmsCapacityOnSimRespAdapter::~ProtocolSmsCapacityOnSimRespAdapter() {
    delete protocolSmsCapacityOnSimRespAdapterInterface;
    protocolSmsCapacityOnSimRespAdapterInterface = NULL;
}

int ProtocolSmsCapacityOnSimRespAdapter::GetSimId() const {
    return protocolSmsCapacityOnSimRespAdapterInterface->GetSimId();
}

int ProtocolSmsCapacityOnSimRespAdapter::GetTotalNum() const {
    return protocolSmsCapacityOnSimRespAdapterInterface->GetTotalNum();
}

int ProtocolSmsCapacityOnSimRespAdapter::GetUsedNum() const {
    return protocolSmsCapacityOnSimRespAdapterInterface->GetUsedNum();
}

/**
 * ProtocolCdmaSendSmsRespAdapter
 */
ProtocolCdmaSendSmsRespAdapter::ProtocolCdmaSendSmsRespAdapter(const ModemData *pModemData)
    : ProtocolCdmaSendSmsRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolCdmaSendSmsRespAdapterInterface = factory->createProtocolCdmaSendSmsRespAdapter(pModemData);
}

ProtocolCdmaSendSmsRespAdapter::~ProtocolCdmaSendSmsRespAdapter() {
    delete protocolCdmaSendSmsRespAdapterInterface;
    protocolCdmaSendSmsRespAdapterInterface = NULL;
}

int ProtocolCdmaSendSmsRespAdapter::GetRef() {
    return protocolCdmaSendSmsRespAdapterInterface->GetRef();
}

int ProtocolCdmaSendSmsRespAdapter::GetSmsRspErrorClass() {
    return protocolCdmaSendSmsRespAdapterInterface->GetSmsRspErrorClass();
}

int ProtocolCdmaSendSmsRespAdapter::GetSmsRspCauseCode() {
    return protocolCdmaSendSmsRespAdapterInterface->GetSmsRspCauseCode();
}

/**
 * ProtocolCdmaNewSmsIndAdapter
 */
ProtocolCdmaNewSmsIndAdapter::ProtocolCdmaNewSmsIndAdapter(const ModemData *pModemData)
    : ProtocolCdmaNewSmsIndAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolCdmaNewSmsIndAdapterInterface = factory->createProtocolCdmaNewSmsIndAdapter(pModemData);
}

ProtocolCdmaNewSmsIndAdapter::~ProtocolCdmaNewSmsIndAdapter() {
    delete protocolCdmaNewSmsIndAdapterInterface;
    protocolCdmaNewSmsIndAdapterInterface = NULL;
}

int ProtocolCdmaNewSmsIndAdapter::GetMessageLength() {
    return protocolCdmaNewSmsIndAdapterInterface->GetMessageLength();
}

const RIL_CDMA_SMS_Message *ProtocolCdmaNewSmsIndAdapter::GetRilCdmaSmsMsg() const {
    return protocolCdmaNewSmsIndAdapterInterface->GetRilCdmaSmsMsg();
}

int ProtocolCdmaNewSmsIndAdapter::GetTpid() const {
    return protocolCdmaNewSmsIndAdapterInterface->GetTpid();
}

/**
 * ProtocolCdmaWriteSmsToRuimRespAdapter
 */
ProtocolCdmaWriteSmsToRuimRespAdapter::ProtocolCdmaWriteSmsToRuimRespAdapter(const ModemData *pModemData)
    : ProtocolCdmaWriteSmsToRuimRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolCdmaWriteSmsToRuimRespAdapterInterface = factory->createProtocolCdmaWriteSmsToRuimRespAdapter(pModemData);
}

ProtocolCdmaWriteSmsToRuimRespAdapter::~ProtocolCdmaWriteSmsToRuimRespAdapter() {
    delete protocolCdmaWriteSmsToRuimRespAdapterInterface;
    protocolCdmaWriteSmsToRuimRespAdapterInterface = NULL;
}

int ProtocolCdmaWriteSmsToRuimRespAdapter::GetIndex() {
    return protocolCdmaWriteSmsToRuimRespAdapterInterface->GetIndex();
}

/**
 * ProtocolGetCdmaBcstSmsConfRespAdapter
 */
ProtocolGetCdmaBcstSmsConfRespAdapter::ProtocolGetCdmaBcstSmsConfRespAdapter(const ModemData *pModemData)
    : ProtocolGetCdmaBcstSmsConfRespAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolGetCdmaBcstSmsConfRespAdapterInterface = factory->createProtocolGetCdmaBcstSmsConfRespAdapter(pModemData);
}

ProtocolGetCdmaBcstSmsConfRespAdapter::~ProtocolGetCdmaBcstSmsConfRespAdapter() {
    delete protocolGetCdmaBcstSmsConfRespAdapterInterface;
    protocolGetCdmaBcstSmsConfRespAdapterInterface = NULL;
}

UINT8 ProtocolGetCdmaBcstSmsConfRespAdapter::GetConfigsNumber() {
    return protocolGetCdmaBcstSmsConfRespAdapterInterface->GetConfigsNumber();
}

RIL_CDMA_BroadcastSmsConfigInfo **ProtocolGetCdmaBcstSmsConfRespAdapter::GetConfigsInfoPointers() {
    return protocolGetCdmaBcstSmsConfRespAdapterInterface->GetConfigsInfoPointers();
}

/**
 * ProtocolCdmaVoiceMsgWaitingInfoIndAdapter
 */
ProtocolCdmaVoiceMsgWaitingInfoIndAdapter::ProtocolCdmaVoiceMsgWaitingInfoIndAdapter(const ModemData *pModemData)
    : ProtocolCdmaVoiceMsgWaitingInfoIndAdapterInterface() {
    auto factory = ProtocolSmsAdapterFactory::getInstance();
    protocolCdmaVoiceMsgWaitingInfoIndAdapterInterface =
        factory->createProtocolCdmaVoiceMsgWaitingInfoIndAdapter(pModemData);
}

ProtocolCdmaVoiceMsgWaitingInfoIndAdapter::~ProtocolCdmaVoiceMsgWaitingInfoIndAdapter() {
    delete protocolCdmaVoiceMsgWaitingInfoIndAdapterInterface;
    protocolCdmaVoiceMsgWaitingInfoIndAdapterInterface = NULL;
}

int ProtocolCdmaVoiceMsgWaitingInfoIndAdapter::GetMessageLength() {
    return protocolCdmaVoiceMsgWaitingInfoIndAdapterInterface->GetMessageLength();
}

const RIL_CDMA_SMS_Message *ProtocolCdmaVoiceMsgWaitingInfoIndAdapter::GetRilCdmaSmsMsg() const {
    return protocolCdmaVoiceMsgWaitingInfoIndAdapterInterface->GetRilCdmaSmsMsg();
}
