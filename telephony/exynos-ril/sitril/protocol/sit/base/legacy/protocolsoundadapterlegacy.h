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
 * protocolsoundadapterlegacy.h
 *
 *  Created on: 2014. 6. 28.
 *      Author: sungwoo48.choi
 */

#ifndef __PROTOCOL_SOUND_ADAPTER_LEGACY_H__
#define __PROTOCOL_SOUND_ADAPTER_LEGACY_H__

#include "protocoladapter.h"
#include "protocolsoundadapterinterface.h"

class ProtocolSoundGetMuteRespAdapterLegacy : public ProtocolSoundGetMuteRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSoundGetMuteRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundGetMuteRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}

  public:
    int GetMuteState() const;

    //forProtocolRspadapter
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundRingbackToneIndAdapterLegacy : public ProtocolSoundRingbackToneIndAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolSoundRingbackToneIndAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundRingbackToneIndAdapterInterface(), ProtocolIndAdapter(pModemData) {}

  public:
    int GetRingbackToneState() const;
    int GetFlag() const;
};

class ProtocolSoundGetVolumeRespAdapterLegacy : public ProtocolSoundGetVolumeRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSoundGetVolumeRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundGetVolumeRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}

  public:
    int GetVolume() const;

    //forProtocolRspadapter
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundGetAudiopathRespAdapterLegacy : public ProtocolSoundGetAudiopathRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSoundGetAudiopathRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundGetAudiopathRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}

  public:
    int GetAudiopath() const;

    //forProtocolRspadapter
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundGetMultiMICRespAdapterLegacy : public ProtocolSoundGetMultiMICRespAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSoundGetMultiMICRespAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundGetMultiMICRespAdapterInterface(), ProtocolRespAdapter(pModemData) {}

  public:
    int GetMultimicmode() const;

    //forProtocolRspadapter
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

class ProtocolSoundWBAMRReportAdapterLegacy : public ProtocolSoundWBAMRReportAdapterInterface, public ProtocolIndAdapter {
  public:
    ProtocolSoundWBAMRReportAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundWBAMRReportAdapterInterface(), ProtocolIndAdapter(pModemData) {}

  public:
    int GetStatus() const;
    int GetCallType() const;
};

class ProtocolSoundGetWBAMRCapabilityAdapterLegacy : public ProtocolSoundGetWBAMRCapabilityAdapterInterface, public ProtocolRespAdapter {
  public:
    ProtocolSoundGetWBAMRCapabilityAdapterLegacy(const ModemData *pModemData)
        : ProtocolSoundGetWBAMRCapabilityAdapterInterface(), ProtocolRespAdapter(pModemData) {}

  public:
    int GetWbAmr() const;

    //forProtocolRspadapter
    UINT GetErrorCode() const {
        return ProtocolRespAdapter::GetErrorCode();
    }
};

#endif /* __PROTOCOL_SOUND_ADAPTER_LEGACY_H__ */
