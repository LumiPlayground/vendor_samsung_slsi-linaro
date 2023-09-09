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
 * protocolnetadapterinterface.h
 *
 *  Created on: 2021. 6. 23.
 *      Author: aryaman.2019
 */

#ifndef __PROTOCOL_NET_ADAPTER_INTERFACE_H__
#define __PROTOCOL_NET_ADAPTER_INTERFACE_H__

#include "rildef.h"
#include <fw/netdata.h>

/**
 * ProtocolNetVoiceRegStateAdapterInterface
 */
class ProtocolNetVoiceRegStateAdapterInterface {
public:
    ProtocolNetVoiceRegStateAdapterInterface() = default;
    virtual ~ProtocolNetVoiceRegStateAdapterInterface() = default;
public:
    virtual int GetRegState() const = 0;
    virtual int GetRejectCause() const = 0;
    virtual int GetRadioTech() const = 0;
    virtual int GetLAC() const = 0;
    virtual int GetCellId() const = 0;
    virtual int GetPSC() const = 0;
    virtual int GetTAC() const = 0;
    virtual int GetPCID() const = 0;
    virtual int GetECI() const = 0;
    virtual int GetStationId() const = 0;
    virtual int GetStationLat() const = 0;
    virtual int GetStationLong() const = 0;
    virtual int GetConCurrent() const = 0;
    virtual int GetSystemId() const = 0;
    virtual int GetNetworkId() const = 0;
    virtual int GetRoamingInd() const = 0;
    virtual int GetRegPrl() const = 0;
    virtual int GetRoamingIndPrl() const = 0;
    virtual int getChannelNumber() const = 0;
    virtual long int getNrCid() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetDataRegStateAdapterInterface
 */
class ProtocolNetDataRegStateAdapterInterface {
public:
    ProtocolNetDataRegStateAdapterInterface() = default;
    virtual ~ProtocolNetDataRegStateAdapterInterface() = default;
public:
    virtual int GetRegState() const = 0;
    virtual int GetRejectCause() const = 0;
    virtual int GetMaxSDC() const = 0;
    virtual int GetRadioTech() const = 0;
    virtual int GetLAC() const = 0;
    virtual int GetCellId() const = 0;
    virtual int GetPSC() const = 0;
    virtual int GetTAC() const = 0;
    virtual int GetPCID() const = 0;
    virtual int GetECI() const = 0;
    virtual int GetCSGID() const = 0;
    virtual int GetTADV() const = 0;
    virtual int GetImsVops() const = 0;
    virtual int GetEmcService() const = 0;
    virtual int getChannelNumber() const = 0;
    virtual bool IsEndcAvailable() const = 0;
    virtual bool IsDcNrRestricted() const = 0;
    virtual bool IsNrAvailable() const = 0;
    virtual long int getNrCid() const = 0;
    virtual int GetEmf() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetOperatorAdapterInterface
 */
class ProtocolNetOperatorAdapterInterface {
public:
    ProtocolNetOperatorAdapterInterface() = default;
    virtual ~ProtocolNetOperatorAdapterInterface() = default;
public:
    virtual const char *GetPlmn() const = 0;
    virtual const char *GetShortPlmn() const = 0;
    virtual const char *GetLongPlmn() const = 0;
    virtual int GetRegState() const = 0;
    virtual int GetLac() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetSelModeAdapterInterface
 */
class ProtocolNetSelModeAdapterInterface {
public:
    ProtocolNetSelModeAdapterInterface() = default;
    virtual ~ProtocolNetSelModeAdapterInterface() = default;
public:
    virtual int GetNetworkSelectionMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetRadioStateRespAdapterInterface
 */
class ProtocolGetRadioStateRespAdapterInterface {
public:
    ProtocolGetRadioStateRespAdapterInterface() = default;
    virtual ~ProtocolGetRadioStateRespAdapterInterface() = default;
public:
    virtual int GetRadioState() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolRadioPowerAdapterInterface
 */
class ProtocolRadioPowerAdapterInterface {
public:
    ProtocolRadioPowerAdapterInterface() = default;
    virtual ~ProtocolRadioPowerAdapterInterface() = default;
public:
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolRadioStateAdapterInterface
 */
class ProtocolRadioStateAdapterInterface {
public:
    ProtocolRadioStateAdapterInterface() = default;
    virtual ~ProtocolRadioStateAdapterInterface() = default;
public:
    virtual int GetRadioState() const = 0;
};

/**
 * ProtocolNetPrefNetTypeAdapterInterface
 */
class ProtocolNetPrefNetTypeAdapterInterface {
public:
    ProtocolNetPrefNetTypeAdapterInterface() = default;
    virtual ~ProtocolNetPrefNetTypeAdapterInterface() = default;
public:
    virtual int GetPreferredNetworkType() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetBandModeAdapterInterface
 */
class ProtocolNetBandModeAdapterInterface {
public:
    ProtocolNetBandModeAdapterInterface() = default;
    virtual ~ProtocolNetBandModeAdapterInterface() = default;
public:
    virtual int GetCount() const = 0;
    virtual const int * GetAvialableBandMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetAvailableNetworkAdapterInterface
 */
class ProtocolNetAvailableNetworkAdapterInterface {
public:
    ProtocolNetAvailableNetworkAdapterInterface() = default;
    virtual ~ProtocolNetAvailableNetworkAdapterInterface() = default;
public:
    virtual int GetCount() = 0;
    virtual bool GetNetwork(NetworkInfo &nwkInfo, int index, const char *simPlmn, char *simSpn) = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetGetPsServiceAdapterInterface
 */
class ProtocolNetGetPsServiceAdapterInterface {
public:
    ProtocolNetGetPsServiceAdapterInterface() = default;
    virtual ~ProtocolNetGetPsServiceAdapterInterface() = default;
public:
    virtual int GetState() = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetDuplexModeRespAdapterInterface
 */
class ProtocolNetDuplexModeRespAdapterInterface {
public:
    ProtocolNetDuplexModeRespAdapterInterface() = default;
    virtual ~ProtocolNetDuplexModeRespAdapterInterface() = default;
public:
    virtual int Get4gDuplexMode() const = 0;
    virtual int Get3gDuplexMode() const = 0;
    virtual int GetDuplexMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetEmergencyActInfoAdapterInterface
 */
class ProtocolNetEmergencyActInfoAdapterInterface {
public:
    ProtocolNetEmergencyActInfoAdapterInterface() = default;
    virtual ~ProtocolNetEmergencyActInfoAdapterInterface() = default;
public:
    virtual int GetRat() const = 0;
    virtual int GetActStatus() const = 0;
};

/**
 * ProtocolNetMcSrchRespAdapterInterface
 */
class ProtocolNetMcSrchRespAdapterInterface {
public:
    ProtocolNetMcSrchRespAdapterInterface() = default;
    virtual ~ProtocolNetMcSrchRespAdapterInterface() = default;
public:
    virtual int GetMcSrchResult() const = 0;
    virtual const char *GetMcSrchPlmn() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolSetNetworkRCRespAdapterInterface
 */
class ProtocolSetNetworkRCRespAdapterInterface {
public:
    ProtocolSetNetworkRCRespAdapterInterface() = default;
    virtual ~ProtocolSetNetworkRCRespAdapterInterface() = default;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolGetNetworkRCRespAdapterInterface
 */
class ProtocolGetNetworkRCRespAdapterInterface {
public:
    ProtocolGetNetworkRCRespAdapterInterface() = default;
    virtual ~ProtocolGetNetworkRCRespAdapterInterface() = default;
public:
    virtual int GetVersion() const = 0;
    virtual int GetSession() const = 0;
    virtual int GetPhase() const = 0;
    virtual int GetRafType() const = 0;
    virtual BYTE *GetUuid() const = 0;
    virtual int GetStatus() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetworkRCIndAdapterInterface
 */
class ProtocolNetworkRCIndAdapterInterface {
public:
    ProtocolNetworkRCIndAdapterInterface() = default;
    virtual ~ProtocolNetworkRCIndAdapterInterface() = default;
public:
    //need to implement based on SIT definition when it is available
    virtual int GetVersion() const = 0;
    virtual int GetSession() const = 0;
    virtual int GetPhase() const = 0;
    virtual int GetRafType() const = 0;
    virtual BYTE *GetUuid() const = 0;
    virtual int GetStatus() const = 0;
};

/**
 * ProtocolNetworkSgcBearerAllocIndAdapterInterface
 */
class ProtocolNetworkSgcBearerAllocIndAdapterInterface {
public:
    ProtocolNetworkSgcBearerAllocIndAdapterInterface() = default;
    virtual ~ProtocolNetworkSgcBearerAllocIndAdapterInterface() = default;
public:
    virtual int GetRat() const = 0;
    virtual int GetConnectionStatus() const = 0;
};

/**
 * ProtocolNetCdmaQueryRoamingTypeAdapterInterface
 */
class ProtocolNetCdmaQueryRoamingTypeAdapterInterface {
public:
    ProtocolNetCdmaQueryRoamingTypeAdapterInterface() = default;
    virtual ~ProtocolNetCdmaQueryRoamingTypeAdapterInterface() = default;
public:
    virtual int QueryRoamingType() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetCdmaHybridModeAdapterInterface
 */
class ProtocolNetCdmaHybridModeAdapterInterface {
public:
    ProtocolNetCdmaHybridModeAdapterInterface() = default;
    virtual ~ProtocolNetCdmaHybridModeAdapterInterface() = default;
public:
    virtual int GetCdmaHybridMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetMccAdapterInterface
 */
class ProtocolNetMccAdapterInterface {
public:
    ProtocolNetMccAdapterInterface() = default;
    virtual ~ProtocolNetMccAdapterInterface() = default;
public:
    virtual int GetCurrentPrefNetworkMode() const = 0;
    virtual const char* GetMcc() const = 0;
};

/**
 * ProtocolNetCellInfoListAdapterInterface
 */
class ProtocolNetCellInfoListAdapterInterface {
public:
    ProtocolNetCellInfoListAdapterInterface() = default;
    virtual ~ProtocolNetCellInfoListAdapterInterface() = default;
public:
    virtual list<RIL_CellInfo_V1_6>& GetCellInfoList(int cellInfoVer) = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetCellInfoListIndAdapterInterface
 */
class ProtocolNetCellInfoListIndAdapterInterface {
public:
    ProtocolNetCellInfoListIndAdapterInterface() = default;
    virtual ~ProtocolNetCellInfoListIndAdapterInterface() = default;
public:
    virtual list<RIL_CellInfo_V1_6>& GetCellInfoList(int cellInfoVer) = 0;
};

/**
 * ProtocolNetScanResultAdapterInterface
 */
class ProtocolNetScanResultAdapterInterface {
public:
    ProtocolNetScanResultAdapterInterface() = default;
    virtual ~ProtocolNetScanResultAdapterInterface() = default;
public:
    virtual int GetScanStatus() const = 0;
    virtual int GetScanResult() const = 0;
    virtual list<RIL_CellInfo_V1_6>& GetCellInfoList(int cellInfoVer) = 0;
};

/**
 * ProtocolNetSimFileInfoAdapterInterface
 */
class ProtocolNetSimFileInfoAdapterInterface {
public:
    ProtocolNetSimFileInfoAdapterInterface() = default;
    virtual ~ProtocolNetSimFileInfoAdapterInterface() = default;
public:
    virtual int GetSimFileId() const = 0;
    virtual int GetRecordLen() const = 0;
    virtual int GetNumOfRecords() const = 0;
    virtual BYTE **GetSimFileData() const = 0;
};

/**
 * ProtocolNetPhysicalChannelConfigsInterface
 */
class ProtocolNetPhysicalChannelConfigsInterface {
public:
    ProtocolNetPhysicalChannelConfigsInterface() = default;
    virtual ~ProtocolNetPhysicalChannelConfigsInterface() = default;
    virtual list<PhysicalChannelConfigsData>& GetConfigList() = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolNetGetManualRatModeAdapterInterface
 */
class ProtocolNetGetManualRatModeAdapterInterface {
public:
    ProtocolNetGetManualRatModeAdapterInterface() = default;
    virtual ~ProtocolNetGetManualRatModeAdapterInterface() = default;
public:
    virtual void GetManualRatMode(void *data) = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetSetManualRatModeAdapterInterface
 */
class ProtocolNetSetManualRatModeAdapterInterface {
public:
    ProtocolNetSetManualRatModeAdapterInterface() = default;
    virtual ~ProtocolNetSetManualRatModeAdapterInterface() = default;
public:
    virtual int GetCause() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetGetFreqLockAdapterInterface
 */
class ProtocolNetGetFreqLockAdapterInterface {
public:
    ProtocolNetGetFreqLockAdapterInterface() = default;
    virtual ~ProtocolNetGetFreqLockAdapterInterface() = default;
public:
    virtual void GetFrequencyLock(void *data) = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetSetFreqLockAdapterInterface
 */
class ProtocolNetSetFreqLockAdapterInterface {
public:
    ProtocolNetSetFreqLockAdapterInterface() = default;
    virtual ~ProtocolNetSetFreqLockAdapterInterface() = default;
public:
    virtual int GetResult() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetGetEndcModeAdapterInterface
 */
class ProtocolNetGetEndcModeAdapterInterface {
public:
    ProtocolNetGetEndcModeAdapterInterface() = default;
    virtual ~ProtocolNetGetEndcModeAdapterInterface() = default;
public:
    virtual int GetEndcMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetworkFrequencyInfoIndAdapterInterface
 */
class ProtocolNetworkFrequencyInfoIndAdapterInterface {
public:
    ProtocolNetworkFrequencyInfoIndAdapterInterface() = default;
    virtual ~ProtocolNetworkFrequencyInfoIndAdapterInterface() = default;
public:
    virtual int GetPrimaryRat() const = 0;
    virtual int GetPrimaryBand() const = 0;
    virtual int GetPrimaryFrequency() const = 0;
    virtual int GetSecondaryRat() const = 0;
    virtual int GetSecondaryBand() const = 0;
    virtual int GetSecondaryFrequency() const = 0;
};

/**
 * ProtocolNetAcBarringInfoInterface
 */
class ProtocolNetAcBarringInfoInterface {
public:
    ProtocolNetAcBarringInfoInterface() = default;
    virtual ~ProtocolNetAcBarringInfoInterface()= default;
public:
    virtual void GetAcBarringInfo(void *data, unsigned int size) = 0;
};

/**
 * ProtocolNetGetFrequencyInfoAdapterInterface
 */
class ProtocolNetGetFrequencyInfoAdapterInterface {
public:
    ProtocolNetGetFrequencyInfoAdapterInterface() = default;
    virtual ~ProtocolNetGetFrequencyInfoAdapterInterface() = default;
public:
    virtual int GetPrimaryRat() const = 0;
    virtual int GetPrimaryBand() const = 0;
    virtual int GetPrimaryFrequency() const = 0;
    virtual int GetSecondaryRat() const = 0;
    virtual int GetSecondaryBand() const = 0;
    virtual int GetSecondaryFrequency() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetRrcInfoAdapterInterface
 */
class ProtocolNetRrcInfoAdapterInterface {
public:
    ProtocolNetRrcInfoAdapterInterface() = default;
    virtual ~ProtocolNetRrcInfoAdapterInterface() = default;
public:
    virtual int GetRat() const = 0;
    virtual int GetState() const = 0;
};

/**
 * ProtocolNetGetNrModeAdapterInterface
 */
class ProtocolNetGetNrModeAdapterInterface {
public:
    ProtocolNetGetNrModeAdapterInterface() = default;
    virtual ~ProtocolNetGetNrModeAdapterInterface() = default;
public:
    virtual int GetNrMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetGetNrSilenceModeAdapterInterface
 */
class ProtocolNetGetNrSilenceModeAdapterInterface {
public:
    ProtocolNetGetNrSilenceModeAdapterInterface() = default;
    virtual ~ProtocolNetGetNrSilenceModeAdapterInterface() = default;
public:
    virtual bool GetNsaMode() const = 0;
    virtual bool GetSaMode() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolRegistrationFailedAdapterInterface
 */
class ProtocolRegistrationFailedAdapterInterface {
public:
    ProtocolRegistrationFailedAdapterInterface() = default;
    virtual ~ProtocolRegistrationFailedAdapterInterface() = default;
public:
    virtual RIL_RegistrationFailed *GetRegistrationFailed() = 0;
};

/**
 * ProtocolGetBarringInfoRspAdapterInterface
 */
class ProtocolGetBarringInfoRspAdapterInterface {
public:
    ProtocolGetBarringInfoRspAdapterInterface() = default;
    virtual ~ProtocolGetBarringInfoRspAdapterInterface() = default;
    virtual RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo() = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolBarringInfoChangedAdapterInterface
 */
class ProtocolBarringInfoChangedAdapterInterface {
public:
    ProtocolBarringInfoChangedAdapterInterface() = default;
    virtual ~ProtocolBarringInfoChangedAdapterInterface() = default;
    virtual RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo() = 0;
};

/**
 * ProtocolNetGetVonrCapaAdapterInterface
 */
class ProtocolNetGetVonrCapaAdapterInterface {
public:
    ProtocolNetGetVonrCapaAdapterInterface() = default;
    virtual ~ProtocolNetGetVonrCapaAdapterInterface() = default;
public:
    virtual int GetVonrCapa() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetGetAllowNetworkAdapterInterface
 */
class ProtocolNetGetAllowNetworkAdapterInterface {
public:
    ProtocolNetGetAllowNetworkAdapterInterface() = default;
    virtual ~ProtocolNetGetAllowNetworkAdapterInterface() = default;
public:
    virtual UINT32 GetRat() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetSystemSelectionChannelsAdapterInterface
 */
class ProtocolNetSystemSelectionChannelsAdapterInterface {
public:
    ProtocolNetSystemSelectionChannelsAdapterInterface() = default;
    virtual ~ProtocolNetSystemSelectionChannelsAdapterInterface() = default;
public:
    virtual int GetSpecifiersLength() = 0;
    virtual RIL_RadioAccessSpecifier_V1_5 *GetRadioAccessSpecifier() = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetB1B2ConfigIndAdapterInterface
 */
class ProtocolNetB1B2ConfigIndAdapterInterface {
public:
    ProtocolNetB1B2ConfigIndAdapterInterface() = default;
    virtual ~ProtocolNetB1B2ConfigIndAdapterInterface() = default;
public:
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolNetGetDeviceServiceAdapterInterface
 */
class ProtocolNetGetDeviceServiceAdapterInterface {
public:
    ProtocolNetGetDeviceServiceAdapterInterface() = default;
    virtual ~ProtocolNetGetDeviceServiceAdapterInterface() = default;
public:
    virtual int GetUsageSetting() const = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolNetDsdsStatusIndAdapterInterface
 */
class ProtocolNetDsdsStatusIndAdapterInterface {
public:
    ProtocolNetDsdsStatusIndAdapterInterface() = default;
    virtual ~ProtocolNetDsdsStatusIndAdapterInterface() = default;
    // virtual int GetStatus() const = 0;
    virtual bool IsPsRestricted() const = 0;
};

#endif /* __PROTOCOL_NET_ADAPTER_INTERFACE_H__ */
