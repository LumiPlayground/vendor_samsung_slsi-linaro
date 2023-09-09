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
 * protocolsoundadapterinterface.h
 *
 *  Created on: 2021.08. 18.
 *      Author: lnarayana.k
 */

#ifndef __PROTOCOL_SOUND_ADAPTER_INTERFACE_H__
#define __PROTOCOL_SOUND_ADAPTER_INTERFACE_H__

#include <base/types.h>

class ProtocolSoundGetMuteRespAdapterInterface
{
public:
    ProtocolSoundGetMuteRespAdapterInterface() { }
    virtual ~ProtocolSoundGetMuteRespAdapterInterface() = default;

public:
    virtual int GetMuteState() const = 0;

    //for ProtocolRspAdapter
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSoundRingbackToneIndAdapterInterface
{
public:
    ProtocolSoundRingbackToneIndAdapterInterface(){ }
    virtual ~ProtocolSoundRingbackToneIndAdapterInterface() = default;

public:
    virtual int GetRingbackToneState() const = 0;
    virtual int GetFlag() const = 0;
};

class ProtocolSoundGetVolumeRespAdapterInterface
{
public:
    ProtocolSoundGetVolumeRespAdapterInterface() { }
    virtual ~ProtocolSoundGetVolumeRespAdapterInterface() = default;

public:
    virtual int GetVolume() const = 0;

    //for ProtocolRspAdapter
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSoundGetAudiopathRespAdapterInterface
{
public:
    ProtocolSoundGetAudiopathRespAdapterInterface() { }
    virtual ~ProtocolSoundGetAudiopathRespAdapterInterface() = default;

public:
    virtual int GetAudiopath() const = 0;

    //for ProtocolRspAdapter
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSoundGetMultiMICRespAdapterInterface
{
public:
    ProtocolSoundGetMultiMICRespAdapterInterface() { }
    virtual ~ProtocolSoundGetMultiMICRespAdapterInterface() = default;

public:
    virtual int GetMultimicmode() const = 0;

    //for ProtocolRspAdapter
    virtual UINT GetErrorCode() const = 0;
};

class ProtocolSoundWBAMRReportAdapterInterface
{
public:
    ProtocolSoundWBAMRReportAdapterInterface() { }
    virtual ~ProtocolSoundWBAMRReportAdapterInterface() = default;

public:
    virtual int GetStatus() const = 0;
    virtual int GetCallType() const = 0;
};

class ProtocolSoundGetWBAMRCapabilityAdapterInterface
{
public:
    ProtocolSoundGetWBAMRCapabilityAdapterInterface() { }
    virtual ~ProtocolSoundGetWBAMRCapabilityAdapterInterface() = default;

public:
    virtual int GetWbAmr() const = 0;

    //for ProtocolRspAdapter
    virtual UINT GetErrorCode() const = 0;
};

#endif /* __PROTOCOL_SOUND_ADAPTER_INTERFACE_H__ */
