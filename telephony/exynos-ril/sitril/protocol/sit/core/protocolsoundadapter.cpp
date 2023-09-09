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
 * protocolsoundadapter.cpp
 *
 *  Created on: 2021. 8. 25.
 *      Author: lnarayana.k
 */

#include "protocolsoundadapter.h"
#include "legacy/protocolsoundadapterlegacy.h"
#include "json/protocolsoundadapterjson.h"
#include "commondef.h"

/**
 * ProtocolSoundGetMuteRespAdapter
 */
ProtocolSoundGetMuteRespAdapter::ProtocolSoundGetMuteRespAdapter(const ModemData *pModemData)
    : ProtocolSoundGetMuteRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundGetMuteRespAdapterInterface = new ProtocolSoundGetMuteRespAdapterJson(pModemData);
    } else {
        m_pProtocolSoundGetMuteRespAdapterInterface = new ProtocolSoundGetMuteRespAdapterLegacy(pModemData);
    }
}

ProtocolSoundGetMuteRespAdapter::~ProtocolSoundGetMuteRespAdapter() {
    delete m_pProtocolSoundGetMuteRespAdapterInterface;
    m_pProtocolSoundGetMuteRespAdapterInterface = NULL;
}

int ProtocolSoundGetMuteRespAdapter::GetMuteState() const {
    return m_pProtocolSoundGetMuteRespAdapterInterface->GetMuteState();
}

/**
 * ProtocolSoundRingbackToneIndAdapter
 */
ProtocolSoundRingbackToneIndAdapter::ProtocolSoundRingbackToneIndAdapter(const ModemData *pModemData)
    : ProtocolSoundRingbackToneIndAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundRingbackToneIndAdapterInterface = new ProtocolSoundRingbackToneIndAdapterJson(pModemData);
    } else {
        m_pProtocolSoundRingbackToneIndAdapterInterface = new ProtocolSoundRingbackToneIndAdapterLegacy(pModemData);
    }
}

ProtocolSoundRingbackToneIndAdapter::~ProtocolSoundRingbackToneIndAdapter() {
    delete m_pProtocolSoundRingbackToneIndAdapterInterface;
    m_pProtocolSoundRingbackToneIndAdapterInterface = NULL;
}

int ProtocolSoundRingbackToneIndAdapter::GetRingbackToneState() const {
    return m_pProtocolSoundRingbackToneIndAdapterInterface->GetRingbackToneState();
}

int ProtocolSoundRingbackToneIndAdapter::GetFlag() const {
    return m_pProtocolSoundRingbackToneIndAdapterInterface->GetFlag();
}

/**
 * ProtocolSoundGetVolumeRespAdapter
 */
ProtocolSoundGetVolumeRespAdapter::ProtocolSoundGetVolumeRespAdapter(const ModemData *pModemData)
    : ProtocolSoundGetVolumeRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundGetVolumeRespAdapterInterface = new ProtocolSoundGetVolumeRespAdapterJson(pModemData);
    } else {
        m_pProtocolSoundGetVolumeRespAdapterInterface = new ProtocolSoundGetVolumeRespAdapterLegacy(pModemData);
    }
}

ProtocolSoundGetVolumeRespAdapter::~ProtocolSoundGetVolumeRespAdapter() {
    delete m_pProtocolSoundGetVolumeRespAdapterInterface;
    m_pProtocolSoundGetVolumeRespAdapterInterface = NULL;
}

int ProtocolSoundGetVolumeRespAdapter::GetVolume() const {
    return m_pProtocolSoundGetVolumeRespAdapterInterface->GetVolume();
}

/**
 * ProtocolSoundGetAudiopathRespAdapter
 */
ProtocolSoundGetAudiopathRespAdapter::ProtocolSoundGetAudiopathRespAdapter(const ModemData *pModemData)
    : ProtocolSoundGetAudiopathRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundGetAudiopathRespAdapterInterface = new ProtocolSoundGetAudiopathRespAdapterJson(pModemData);
    } else {
        m_pProtocolSoundGetAudiopathRespAdapterInterface = new ProtocolSoundGetAudiopathRespAdapterLegacy(pModemData);
    }
}

ProtocolSoundGetAudiopathRespAdapter::~ProtocolSoundGetAudiopathRespAdapter() {
    delete m_pProtocolSoundGetAudiopathRespAdapterInterface;
    m_pProtocolSoundGetAudiopathRespAdapterInterface = NULL;
}

int ProtocolSoundGetAudiopathRespAdapter::GetAudiopath() const {
    return m_pProtocolSoundGetAudiopathRespAdapterInterface->GetAudiopath();
}

/**
 * ProtocolSoundGetMultiMICRespAdapter
 */
ProtocolSoundGetMultiMICRespAdapter::ProtocolSoundGetMultiMICRespAdapter(const ModemData *pModemData)
    : ProtocolSoundGetMultiMICRespAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundGetMultiMICRespAdapterInterface = new ProtocolSoundGetMultiMICRespAdapterJson(pModemData);
    } else {
        m_pProtocolSoundGetMultiMICRespAdapterInterface = new ProtocolSoundGetMultiMICRespAdapterLegacy(pModemData);
    }
}

ProtocolSoundGetMultiMICRespAdapter::~ProtocolSoundGetMultiMICRespAdapter() {
    delete m_pProtocolSoundGetMultiMICRespAdapterInterface;
    m_pProtocolSoundGetMultiMICRespAdapterInterface = NULL;
}

int ProtocolSoundGetMultiMICRespAdapter::GetMultimicmode() const {
    return m_pProtocolSoundGetMultiMICRespAdapterInterface->GetMultimicmode();
}

/**
 * ProtocolSoundWBAMRReportAdapter
 */
ProtocolSoundWBAMRReportAdapter::ProtocolSoundWBAMRReportAdapter(const ModemData *pModemData)
    : ProtocolSoundWBAMRReportAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundWBAMRReportAdapterInterface = new ProtocolSoundWBAMRReportAdapterJson(pModemData);
    } else {
        m_pProtocolSoundWBAMRReportAdapterInterface = new ProtocolSoundWBAMRReportAdapterLegacy(pModemData);
    }
}
ProtocolSoundWBAMRReportAdapter::~ProtocolSoundWBAMRReportAdapter() {
    delete m_pProtocolSoundWBAMRReportAdapterInterface;
    m_pProtocolSoundWBAMRReportAdapterInterface = NULL;
}
int ProtocolSoundWBAMRReportAdapter::GetStatus() const {
    return m_pProtocolSoundWBAMRReportAdapterInterface->GetStatus();
}

int ProtocolSoundWBAMRReportAdapter::GetCallType() const {
    return m_pProtocolSoundWBAMRReportAdapterInterface->GetCallType();
}

/**
 * ProtocolSoundGetWBAMRCapabilityAdapter
 */
ProtocolSoundGetWBAMRCapabilityAdapter::ProtocolSoundGetWBAMRCapabilityAdapter(const ModemData *pModemData)
    : ProtocolSoundGetWBAMRCapabilityAdapterInterface() {
    if (IsPayloadTypeJson()) {
        m_pProtocolSoundGetWBAMRCapabilityAdapterInterface = new ProtocolSoundGetWBAMRCapabilityAdapterJson(pModemData);
    } else {
        m_pProtocolSoundGetWBAMRCapabilityAdapterInterface = new ProtocolSoundGetWBAMRCapabilityAdapterLegacy(pModemData);
    }
}

ProtocolSoundGetWBAMRCapabilityAdapter::~ProtocolSoundGetWBAMRCapabilityAdapter() {
    delete m_pProtocolSoundGetWBAMRCapabilityAdapterInterface;
    m_pProtocolSoundGetWBAMRCapabilityAdapterInterface = NULL;
}

int ProtocolSoundGetWBAMRCapabilityAdapter::GetWbAmr() const {
    return m_pProtocolSoundGetWBAMRCapabilityAdapterInterface->GetWbAmr();
}
