/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "protocolembmsadapter.h"
#include "legacy/protocolembmsadapterlegacy.h"
#include "json/protocolembmsadapterjson.h"
#include "modemdata.h"

/**
 * ProtocolEmbmsCoverageAdapter
 */
ProtocolEmbmsCoverageAdapter::ProtocolEmbmsCoverageAdapter(const ModemData *pModemData)
    : ProtocolEmbmsCoverageAdapterInterface() {
    if (IsPayloadTypeJson()) {
        protocolEmbmsCoverageAdapterInterface = new ProtocolEmbmsCoverageAdapterJson(pModemData);
    } else {
        protocolEmbmsCoverageAdapterInterface = new ProtocolEmbmsCoverageAdapterLegacy(pModemData);
    }
}

ProtocolEmbmsCoverageAdapter::~ProtocolEmbmsCoverageAdapter() {
    delete protocolEmbmsCoverageAdapterInterface;
    protocolEmbmsCoverageAdapterInterface = NULL;
}

int ProtocolEmbmsCoverageAdapter::GetCoverage() const {
    return protocolEmbmsCoverageAdapterInterface->GetCoverage();
}

/**
 * ProtocolEmbmsGlobalCellIdAdapter
 */
ProtocolEmbmsGlobalCellIdAdapter::ProtocolEmbmsGlobalCellIdAdapter(const ModemData *pModemData)
    : ProtocolEmbmsGlobalCellIdAdapterInterface() {
    protocolEmbmsGlobalCellIdAdapterInterface = new ProtocolEmbmsGlobalCellIdAdapterLegacy(pModemData);
}

ProtocolEmbmsGlobalCellIdAdapter::~ProtocolEmbmsGlobalCellIdAdapter() {
    delete protocolEmbmsGlobalCellIdAdapterInterface;
    protocolEmbmsGlobalCellIdAdapterInterface = NULL;
}

const char *ProtocolEmbmsGlobalCellIdAdapter::GetMcc() const {
    return protocolEmbmsGlobalCellIdAdapterInterface->GetMcc();
}

const char *ProtocolEmbmsGlobalCellIdAdapter::GetMnc() const {
    return protocolEmbmsGlobalCellIdAdapterInterface->GetMnc();
}

UINT32 ProtocolEmbmsGlobalCellIdAdapter::GetCellId() const {
    return protocolEmbmsGlobalCellIdAdapterInterface->GetCellId();
}

/**
 * ProtocolEmbmsSessionListIndAdapter
 */
ProtocolEmbmsSessionListIndAdapter::ProtocolEmbmsSessionListIndAdapter(const ModemData *pModemData)
    : ProtocolEmbmsSessionListIndAdapterInterface() {
    protocolEmbmsSessionListIndAdapterInterface = new ProtocolEmbmsSessionListIndAdapterLegacy(pModemData);
}

ProtocolEmbmsSessionListIndAdapter::~ProtocolEmbmsSessionListIndAdapter() {
    delete protocolEmbmsSessionListIndAdapterInterface;
    protocolEmbmsSessionListIndAdapterInterface = NULL;
}

int ProtocolEmbmsSessionListIndAdapter::GetState() const {
    return protocolEmbmsSessionListIndAdapterInterface->GetState();
}

int ProtocolEmbmsSessionListIndAdapter::GetOosReason() const {
    return protocolEmbmsSessionListIndAdapterInterface->GetOosReason();
}

int ProtocolEmbmsSessionListIndAdapter::GetRecordNum() const {
    return protocolEmbmsSessionListIndAdapterInterface->GetRecordNum();
}

const BYTE *ProtocolEmbmsSessionListIndAdapter::GetTMGI() const {
    return protocolEmbmsSessionListIndAdapterInterface->GetTMGI();
}

/**
 * ProtocolEmbmsSessionListAdapter
 */
ProtocolEmbmsSessionListAdapter::ProtocolEmbmsSessionListAdapter(const ModemData *pModemData)
    : ProtocolEmbmsSessionListAdapterInterface() {
    protocolEmbmsSessionListAdapterInterface = new ProtocolEmbmsSessionListAdapterLegacy(pModemData);
}

ProtocolEmbmsSessionListAdapter::~ProtocolEmbmsSessionListAdapter() {
    delete protocolEmbmsSessionListAdapterInterface;
    protocolEmbmsSessionListAdapterInterface = NULL;
}

int ProtocolEmbmsSessionListAdapter::GetState() const {
    return protocolEmbmsSessionListAdapterInterface->GetState();
}

int ProtocolEmbmsSessionListAdapter::GetOosReason() const {
    return protocolEmbmsSessionListAdapterInterface->GetOosReason();
}

int ProtocolEmbmsSessionListAdapter::GetRecordNum() const {
    return protocolEmbmsSessionListAdapterInterface->GetRecordNum();
}
const BYTE *ProtocolEmbmsSessionListAdapter::GetTMGI() const {
    return protocolEmbmsSessionListAdapterInterface->GetTMGI();
}

/**
 * ProtocolEmbmsNetworkTimeAdapter
 */
ProtocolEmbmsNetworkTimeAdapter::ProtocolEmbmsNetworkTimeAdapter(const ModemData *pModemData)
    : ProtocolEmbmsNetworkTimeAdapterInterface() {
    protocolEmbmsNetworkTimeAdapterInterface = new ProtocolEmbmsNetworkTimeAdapterLegacy(pModemData);
}

ProtocolEmbmsNetworkTimeAdapter::~ProtocolEmbmsNetworkTimeAdapter() {
    delete protocolEmbmsNetworkTimeAdapterInterface;
    protocolEmbmsNetworkTimeAdapterInterface = NULL;
}

uint64_t ProtocolEmbmsNetworkTimeAdapter::GetNetworkTime() const {
    return protocolEmbmsNetworkTimeAdapterInterface->GetNetworkTime();
}

int ProtocolEmbmsNetworkTimeAdapter::TimeInfoType() const {
    return protocolEmbmsNetworkTimeAdapterInterface->TimeInfoType();
}

int ProtocolEmbmsNetworkTimeAdapter::DayLightValid() const {
    return protocolEmbmsNetworkTimeAdapterInterface->DayLightValid();
}

int ProtocolEmbmsNetworkTimeAdapter::Year() const {
    return protocolEmbmsNetworkTimeAdapterInterface->Year();
}

int ProtocolEmbmsNetworkTimeAdapter::Month() const {
    return protocolEmbmsNetworkTimeAdapterInterface->Month();
}

int ProtocolEmbmsNetworkTimeAdapter::Day() const {
    return protocolEmbmsNetworkTimeAdapterInterface->Day();
}

int ProtocolEmbmsNetworkTimeAdapter::Hour() const {
    return protocolEmbmsNetworkTimeAdapterInterface->Hour();
}

int ProtocolEmbmsNetworkTimeAdapter::Minute() const {
    return protocolEmbmsNetworkTimeAdapterInterface->Minute();
}

int ProtocolEmbmsNetworkTimeAdapter::Second() const {
    return protocolEmbmsNetworkTimeAdapterInterface->Second();
}

int ProtocolEmbmsNetworkTimeAdapter::TimeZone() const {
    return protocolEmbmsNetworkTimeAdapterInterface->TimeZone();
}
int ProtocolEmbmsNetworkTimeAdapter::DayLightAdjust() const {
    return protocolEmbmsNetworkTimeAdapterInterface->DayLightAdjust();
}

int ProtocolEmbmsNetworkTimeAdapter::DayofWeek() const {
    return protocolEmbmsNetworkTimeAdapterInterface->DayofWeek();
}

/**
 * ProtocolEmbmsNetworkTimeIndAdapter
 */
ProtocolEmbmsNetworkTimeIndAdapter::ProtocolEmbmsNetworkTimeIndAdapter(const ModemData *pModemData)
    : ProtocolEmbmsNetworkTimeIndAdapterInterface() {
    protocolEmbmsNetworkTimeIndAdapterInterface = new ProtocolEmbmsNetworkTimeIndAdapterLegacy(pModemData);
}

ProtocolEmbmsNetworkTimeIndAdapter::~ProtocolEmbmsNetworkTimeIndAdapter() {
    delete protocolEmbmsNetworkTimeIndAdapterInterface;
    protocolEmbmsNetworkTimeIndAdapterInterface = NULL;
}

uint64_t ProtocolEmbmsNetworkTimeIndAdapter::GetNetworkTime() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->GetNetworkTime();
}

int ProtocolEmbmsNetworkTimeIndAdapter::TimeInfoType() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->TimeInfoType();
}

int ProtocolEmbmsNetworkTimeIndAdapter::DayLightValid() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->DayLightValid();
}

int ProtocolEmbmsNetworkTimeIndAdapter::Year() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->Year();
}

int ProtocolEmbmsNetworkTimeIndAdapter::Month() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->Month();
}

int ProtocolEmbmsNetworkTimeIndAdapter::Day() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->Day();
}

int ProtocolEmbmsNetworkTimeIndAdapter::Hour() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->Hour();
}

int ProtocolEmbmsNetworkTimeIndAdapter::Minute() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->Minute();
}

int ProtocolEmbmsNetworkTimeIndAdapter::Second() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->Second();
}

int ProtocolEmbmsNetworkTimeIndAdapter::TimeZone() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->TimeZone();
}
int ProtocolEmbmsNetworkTimeIndAdapter::DayLightAdjust() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->DayLightAdjust();
}

int ProtocolEmbmsNetworkTimeIndAdapter::DayofWeek() const {
    return protocolEmbmsNetworkTimeIndAdapterInterface->DayofWeek();
}

/**
 * ProtocolEmbmsSignalStrengthAdapter
 */
ProtocolEmbmsSignalStrengthAdapter::ProtocolEmbmsSignalStrengthAdapter(const ModemData *pModemData)
    : ProtocolEmbmsSignalStrengthAdapterInterface() {
    protocolEmbmsSignalStrengthAdapterInterface = new ProtocolEmbmsSignalStrengthAdapterLegacy(pModemData);
}

ProtocolEmbmsSignalStrengthAdapter::~ProtocolEmbmsSignalStrengthAdapter() {
    delete protocolEmbmsSignalStrengthAdapterInterface;
    protocolEmbmsSignalStrengthAdapterInterface = NULL;
}

int ProtocolEmbmsSignalStrengthAdapter::GetCount() const {
    return protocolEmbmsSignalStrengthAdapterInterface->GetCount();
}

const UINT32 *ProtocolEmbmsSignalStrengthAdapter::GetSnrList() const {
    return protocolEmbmsSignalStrengthAdapterInterface->GetSnrList();
}

/**
 * ProtocolEmbmsSignalStrengthIndAdapter
 */
ProtocolEmbmsSignalStrengthIndAdapter::ProtocolEmbmsSignalStrengthIndAdapter(const ModemData *pModemData)
    : ProtocolEmbmsSignalStrengthIndAdapterInterface() {
    protocolEmbmsSignalStrengthIndAdapterInterface = new ProtocolEmbmsSignalStrengthIndAdapterLegacy(pModemData);
}

ProtocolEmbmsSignalStrengthIndAdapter::~ProtocolEmbmsSignalStrengthIndAdapter() {
    delete protocolEmbmsSignalStrengthIndAdapterInterface;
    protocolEmbmsSignalStrengthIndAdapterInterface = NULL;
}

int ProtocolEmbmsSignalStrengthIndAdapter::GetCount() const {
    return protocolEmbmsSignalStrengthIndAdapterInterface->GetCount();
}

const UINT32 *ProtocolEmbmsSignalStrengthIndAdapter::GetSnrList() const {
    return protocolEmbmsSignalStrengthIndAdapterInterface->GetSnrList();
}

/**
 * ProtocolEmbmsSaiListAdapter
 */
ProtocolEmbmsSaiListAdapter::ProtocolEmbmsSaiListAdapter(const ModemData *pModemData)
    : ProtocolEmbmsSaiListAdapterInterface() {
    protocolEmbmsSaiListAdapterInterface = new ProtocolEmbmsSaiListAdapterLegacy(pModemData);
}

ProtocolEmbmsSaiListAdapter::~ProtocolEmbmsSaiListAdapter() {
    delete protocolEmbmsSaiListAdapterInterface;
    protocolEmbmsSaiListAdapterInterface = NULL;
}

int ProtocolEmbmsSaiListAdapter::GetIntraSaiListLen() const {
    return protocolEmbmsSaiListAdapterInterface->GetIntraSaiListLen();
}

int ProtocolEmbmsSaiListAdapter::GetInterSaiListLen() const {
    return protocolEmbmsSaiListAdapterInterface->GetInterSaiListLen();
}

const UINT16 *ProtocolEmbmsSaiListAdapter::GetIntraSaiList() const {
    return protocolEmbmsSaiListAdapterInterface->GetIntraSaiList();
}

const EMBMS_InterSaiList *ProtocolEmbmsSaiListAdapter::GetInterSaiList() const {
    return protocolEmbmsSaiListAdapterInterface->GetInterSaiList();
}
