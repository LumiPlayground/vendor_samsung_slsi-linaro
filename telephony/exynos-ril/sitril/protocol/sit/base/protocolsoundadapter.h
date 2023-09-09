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
 * protocolsoundadapter.h
 *
 *  Created on: 2021. 8. 19.
 *      Author: lnarayana.k
 */

#ifndef __PROTOCOL_SOUND_ADAPTER_H__
#define __PROTOCOL_SOUND_ADAPTER_H__

#include "protocolsoundadapterinterface.h"

class ModemData;

class ProtocolSoundGetMuteRespAdapter : public ProtocolSoundGetMuteRespAdapterInterface
{
private:
    ProtocolSoundGetMuteRespAdapterInterface *m_pProtocolSoundGetMuteRespAdapterInterface = NULL;

public:
    ProtocolSoundGetMuteRespAdapter(const ModemData *pModemData);
    ProtocolSoundGetMuteRespAdapter(const ProtocolSoundGetMuteRespAdapter&) = delete;
    ProtocolSoundGetMuteRespAdapter& operator=(ProtocolSoundGetMuteRespAdapter const&) = delete;
    virtual ~ProtocolSoundGetMuteRespAdapter();
public:
    int GetMuteState() const;
    UINT GetErrorCode() const { return m_pProtocolSoundGetMuteRespAdapterInterface->GetErrorCode(); }
};

class ProtocolSoundRingbackToneIndAdapter : public ProtocolSoundRingbackToneIndAdapterInterface
{
private:
    ProtocolSoundRingbackToneIndAdapterInterface *m_pProtocolSoundRingbackToneIndAdapterInterface = NULL;

public:
    ProtocolSoundRingbackToneIndAdapter(const ModemData *pModemData);
    ProtocolSoundRingbackToneIndAdapter(const ProtocolSoundRingbackToneIndAdapter&) = delete;
    ProtocolSoundRingbackToneIndAdapter& operator=(ProtocolSoundRingbackToneIndAdapter const&) = delete;
    virtual ~ProtocolSoundRingbackToneIndAdapter();
public:
    int GetRingbackToneState() const;
    int GetFlag() const;
};

class ProtocolSoundGetVolumeRespAdapter : public ProtocolSoundGetVolumeRespAdapterInterface
{
private:
    ProtocolSoundGetVolumeRespAdapterInterface *m_pProtocolSoundGetVolumeRespAdapterInterface = NULL;
public:
    ProtocolSoundGetVolumeRespAdapter(const ModemData *pModemData);
    ProtocolSoundGetVolumeRespAdapter(const ProtocolSoundGetVolumeRespAdapter&) = delete;
    ProtocolSoundGetVolumeRespAdapter& operator=(ProtocolSoundGetVolumeRespAdapter const&) = delete;
    virtual ~ProtocolSoundGetVolumeRespAdapter();
public:
    int GetVolume() const;
    UINT GetErrorCode() const { return m_pProtocolSoundGetVolumeRespAdapterInterface->GetErrorCode(); }
};

class ProtocolSoundGetAudiopathRespAdapter : public ProtocolSoundGetAudiopathRespAdapterInterface
{
private:
    ProtocolSoundGetAudiopathRespAdapterInterface *m_pProtocolSoundGetAudiopathRespAdapterInterface = NULL;
public:
    ProtocolSoundGetAudiopathRespAdapter(const ModemData *pModemData);
    ProtocolSoundGetAudiopathRespAdapter(const ProtocolSoundGetAudiopathRespAdapter&) = delete;
    ProtocolSoundGetAudiopathRespAdapter& operator=(ProtocolSoundGetAudiopathRespAdapter const&) = delete;
    virtual ~ProtocolSoundGetAudiopathRespAdapter();
public:
    int GetAudiopath() const;
    UINT GetErrorCode() const { return m_pProtocolSoundGetAudiopathRespAdapterInterface->GetErrorCode(); }
};

class ProtocolSoundGetMultiMICRespAdapter : public ProtocolSoundGetMultiMICRespAdapterInterface
{
private:
    ProtocolSoundGetMultiMICRespAdapterInterface *m_pProtocolSoundGetMultiMICRespAdapterInterface = NULL;
public:
    ProtocolSoundGetMultiMICRespAdapter(const ModemData *pModemData);
    ProtocolSoundGetMultiMICRespAdapter(const ProtocolSoundGetMultiMICRespAdapter&) = delete;
    ProtocolSoundGetMultiMICRespAdapter& operator=(ProtocolSoundGetMultiMICRespAdapter const&) = delete;
    virtual ~ProtocolSoundGetMultiMICRespAdapter();
public:
    int GetMultimicmode() const;
    UINT GetErrorCode() const { return m_pProtocolSoundGetMultiMICRespAdapterInterface->GetErrorCode(); }
};

class ProtocolSoundWBAMRReportAdapter : public ProtocolSoundWBAMRReportAdapterInterface
{
private:
    ProtocolSoundWBAMRReportAdapterInterface *m_pProtocolSoundWBAMRReportAdapterInterface = NULL;
public:
    ProtocolSoundWBAMRReportAdapter(const ModemData *pModemData);
    ProtocolSoundWBAMRReportAdapter(const ProtocolSoundWBAMRReportAdapter&) = delete;
    ProtocolSoundWBAMRReportAdapter& operator=(ProtocolSoundWBAMRReportAdapter const&) = delete;
    virtual ~ProtocolSoundWBAMRReportAdapter();

public:
    int GetStatus() const;
    int GetCallType() const;
};

class ProtocolSoundGetWBAMRCapabilityAdapter : public ProtocolSoundGetWBAMRCapabilityAdapterInterface
{
private:
    ProtocolSoundGetWBAMRCapabilityAdapterInterface *m_pProtocolSoundGetWBAMRCapabilityAdapterInterface = NULL;
public:
    ProtocolSoundGetWBAMRCapabilityAdapter(const ModemData *pModemData);
    ProtocolSoundGetWBAMRCapabilityAdapter(const ProtocolSoundGetWBAMRCapabilityAdapter&) = delete;
    ProtocolSoundGetWBAMRCapabilityAdapter& operator=(ProtocolSoundGetWBAMRCapabilityAdapter const&) = delete;
    virtual ~ProtocolSoundGetWBAMRCapabilityAdapter();
public:
    int GetWbAmr() const;
    UINT GetErrorCode() const { return m_pProtocolSoundGetWBAMRCapabilityAdapterInterface->GetErrorCode(); }
};
#endif /* __PROTOCOL_SOUND_ADAPTER_H__ */
