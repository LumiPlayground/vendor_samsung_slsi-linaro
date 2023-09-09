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
 * protocolsoundadapterjson.h
 *
 *  Created on: 2021. 08. 10.
 *      Author: lnarayana.k
 */

#ifndef __PROTOCOL_SOUND_ADAPTER_JSON_H__
#define __PROTOCOL_SOUND_ADAPTER_JSON_H__

#include "protocolsoundadapterinterface.h"
#include "protocoljsonadapter.h"

class ProtocolSoundGetMuteRespAdapterJson : public ProtocolSoundGetMuteRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSoundGetMuteRespAdapterJson(const ModemData *pModemData);

  public:
    int GetMuteState() const;
    // For ProtocolRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundRingbackToneIndAdapterJson : public ProtocolSoundRingbackToneIndAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolSoundRingbackToneIndAdapterJson(const ModemData *pModemData);

  public:
    int GetRingbackToneState() const;
    int GetFlag() const;
};

class ProtocolSoundGetVolumeRespAdapterJson : public ProtocolSoundGetVolumeRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSoundGetVolumeRespAdapterJson(const ModemData *pModemData);

  public:
    int GetVolume() const;
    // For ProtocolRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundGetAudiopathRespAdapterJson : public ProtocolSoundGetAudiopathRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSoundGetAudiopathRespAdapterJson(const ModemData *pModemData);

  public:
    int GetAudiopath() const;
    // For ProtocolRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundGetMultiMICRespAdapterJson : public ProtocolSoundGetMultiMICRespAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSoundGetMultiMICRespAdapterJson(const ModemData *pModemData);

  public:
    int GetMultimicmode() const;
    // For ProtocolRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundWBAMRReportAdapterJson : public ProtocolSoundWBAMRReportAdapterInterface, public ProtocolJsonIndAdapter {
  public:
    ProtocolSoundWBAMRReportAdapterJson(const ModemData *pModemData);

  public:
    int GetStatus() const;
    int GetCallType() const;
};

class ProtocolSoundGetWBAMRCapabilityAdapterJson : public ProtocolSoundGetWBAMRCapabilityAdapterInterface, public ProtocolJsonRespAdapter {
  public:
    ProtocolSoundGetWBAMRCapabilityAdapterJson(const ModemData *pModemData);

  public:
    int GetWbAmr() const;
    // For ProtocolRespAdapter
    UINT GetErrorCode() const {
        return ProtocolJsonRespAdapter::GetErrorCode();
    }
};

#endif /* __PROTOCOL_SOUND_ADAPTER_JSON_H__ */
