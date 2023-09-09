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
 * protocolnetadapter.h
 *
 *  Created on: 2021. 06. 23.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_NET_ADAPTER_H__
#define __PROTOCOL_NET_ADAPTER_H__

#include "protocolnetadapterinterface.h"
#include <base/types.h>

class ModemData;

/**
 * ProtocolNetVoiceRegStateAdapter
 */
class ProtocolNetVoiceRegStateAdapter : public ProtocolNetVoiceRegStateAdapterInterface {
private:
    ProtocolNetVoiceRegStateAdapterInterface *protocolNetVoiceRegStateAdapterInterface = NULL;

public:
    ProtocolNetVoiceRegStateAdapter(const ModemData *pModemData);
    ProtocolNetVoiceRegStateAdapter(const ProtocolNetVoiceRegStateAdapter &) = delete;
    ProtocolNetVoiceRegStateAdapter& operator=(ProtocolNetVoiceRegStateAdapter const&) = delete;
    virtual ~ProtocolNetVoiceRegStateAdapter();
public:
    int GetRegState() const;
    int GetRejectCause() const;
    int GetRadioTech() const;
    int GetLAC() const;
    int GetCellId() const;
    int GetPSC() const;
    int GetTAC() const;
    int GetPCID() const;
    int GetECI() const;
    int GetStationId() const;
    int GetStationLat() const;
    int GetStationLong() const;
    int GetConCurrent() const;
    int GetSystemId() const;
    int GetNetworkId() const;
    int GetRoamingInd() const;
    int GetRegPrl() const;
    int GetRoamingIndPrl() const;
    int getChannelNumber() const;
    long int getNrCid() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetVoiceRegStateAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetDataRegStateAdapter
 */
class ProtocolNetDataRegStateAdapter : public ProtocolNetDataRegStateAdapterInterface {
private:
    ProtocolNetDataRegStateAdapterInterface *protocolNetDataRegStateAdapterInterface = NULL;

public:
    ProtocolNetDataRegStateAdapter(const ModemData *pModemData);
    ProtocolNetDataRegStateAdapter(const ProtocolNetDataRegStateAdapter &) = delete;
    ProtocolNetDataRegStateAdapter& operator=(ProtocolNetDataRegStateAdapter const&) = delete;
    virtual ~ProtocolNetDataRegStateAdapter();
public:
    int GetRegState() const;
    int GetRejectCause() const;
    int GetMaxSDC() const;
    int GetRadioTech() const;
    int GetLAC() const;
    int GetCellId() const;
    int GetPSC() const;
    int GetTAC() const;
    int GetPCID() const;
    int GetECI() const;
    int GetCSGID() const;
    int GetTADV() const;
    int GetImsVops() const;
    int GetEmcService() const;
    int getChannelNumber() const;
    bool IsEndcAvailable() const;
    bool IsDcNrRestricted() const;
    bool IsNrAvailable() const;
    long int getNrCid() const;
    int GetEmf() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetDataRegStateAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetOperatorAdapter
 */
class ProtocolNetOperatorAdapter : public ProtocolNetOperatorAdapterInterface {
private:
    ProtocolNetOperatorAdapterInterface *protocolNetOperatorAdapterInterface = NULL;

public:
    ProtocolNetOperatorAdapter(const ModemData *pModemData);
    ProtocolNetOperatorAdapter(const ProtocolNetOperatorAdapter &) = delete;
    ProtocolNetOperatorAdapter& operator=(ProtocolNetOperatorAdapter const&) = delete;
    virtual ~ProtocolNetOperatorAdapter();
public:
    const char *GetPlmn() const;
    const char *GetShortPlmn() const;
    const char *GetLongPlmn() const;
    int GetRegState() const;
    int GetLac() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetOperatorAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetSelModeAdapter
 */
class ProtocolNetSelModeAdapter : public ProtocolNetSelModeAdapterInterface {
private:
    ProtocolNetSelModeAdapterInterface *protocolNetSelModeAdapterInterface = NULL;

public:
    ProtocolNetSelModeAdapter(const ModemData *pModemData);
    ProtocolNetSelModeAdapter(const ProtocolNetSelModeAdapter &) = delete;
    ProtocolNetSelModeAdapter& operator=(ProtocolNetSelModeAdapter const&) = delete;
    virtual ~ProtocolNetSelModeAdapter();
public:
    int GetNetworkSelectionMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetSelModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetRadioStateRespAdapter
 */
class ProtocolGetRadioStateRespAdapter : public ProtocolGetRadioStateRespAdapterInterface {
private:
    ProtocolGetRadioStateRespAdapterInterface *protocolGetRadioStateRespAdapterInterface = NULL;

public:
    ProtocolGetRadioStateRespAdapter(const ModemData *pModemData);
    ProtocolGetRadioStateRespAdapter(const ProtocolGetRadioStateRespAdapter &) = delete;
    ProtocolGetRadioStateRespAdapter& operator=(ProtocolGetRadioStateRespAdapter const&) = delete;
    virtual ~ProtocolGetRadioStateRespAdapter();
public:
    int GetRadioState() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolGetRadioStateRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolRadioPowerAdapter
 */
class ProtocolRadioPowerAdapter : public ProtocolRadioPowerAdapterInterface {
private:
    ProtocolRadioPowerAdapterInterface *protocolRadioPowerAdapterInterface = NULL;

public:
    ProtocolRadioPowerAdapter(const ModemData *pModemData);
    ProtocolRadioPowerAdapter(const ProtocolRadioPowerAdapter &) = delete;
    ProtocolRadioPowerAdapter& operator=(ProtocolRadioPowerAdapter const&) = delete;
    virtual ~ProtocolRadioPowerAdapter();
public:
    virtual UINT GetErrorCode() const;
};

/**
 * ProtocolRadioStateAdapter
 */
class ProtocolRadioStateAdapter : public ProtocolRadioStateAdapterInterface {
private:
    ProtocolRadioStateAdapterInterface *protocolRadioStateAdapterInterface = NULL;

public:
    ProtocolRadioStateAdapter(const ModemData *pModemData);
    ProtocolRadioStateAdapter(const ProtocolRadioStateAdapter &) = delete;
    ProtocolRadioStateAdapter& operator=(ProtocolRadioStateAdapter const&) = delete;
    virtual ~ProtocolRadioStateAdapter();
public:
    int GetRadioState() const;
};

/**
 * ProtocolNetPrefNetTypeAdapter
 */
class ProtocolNetPrefNetTypeAdapter : public ProtocolNetPrefNetTypeAdapterInterface {
private:
    ProtocolNetPrefNetTypeAdapterInterface *protocolNetPrefNetTypeAdapterInterface = NULL;

public:
    ProtocolNetPrefNetTypeAdapter(const ModemData *pModemData);
    ProtocolNetPrefNetTypeAdapter(const ProtocolNetPrefNetTypeAdapter &) = delete;
    ProtocolNetPrefNetTypeAdapter& operator=(ProtocolNetPrefNetTypeAdapter const&) = delete;
    virtual ~ProtocolNetPrefNetTypeAdapter();
public:
    int GetPreferredNetworkType() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetPrefNetTypeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetBandModeAdapter
 */
class ProtocolNetBandModeAdapter : public ProtocolNetBandModeAdapterInterface {
private:
    ProtocolNetBandModeAdapterInterface *protocolNetBandModeAdapterInterface = NULL;

public:
    ProtocolNetBandModeAdapter(const ModemData *pModemData);
    ProtocolNetBandModeAdapter(const ProtocolNetBandModeAdapter &) = delete;
    ProtocolNetBandModeAdapter& operator=(ProtocolNetBandModeAdapter const&) = delete;
    virtual ~ProtocolNetBandModeAdapter();
public:
    int GetCount() const;
    const int * GetAvialableBandMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetBandModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetAvailableNetworkAdapter
 */
class ProtocolNetAvailableNetworkAdapter : public ProtocolNetAvailableNetworkAdapterInterface {
private:
    ProtocolNetAvailableNetworkAdapterInterface *protocolNetAvailableNetworkAdapterInterface = NULL;

public:
    ProtocolNetAvailableNetworkAdapter(const ModemData *pModemData);
    ProtocolNetAvailableNetworkAdapter(const ProtocolNetAvailableNetworkAdapter &) = delete;
    ProtocolNetAvailableNetworkAdapter& operator=(ProtocolNetAvailableNetworkAdapter const&) = delete;
    virtual ~ProtocolNetAvailableNetworkAdapter();

public:
    int GetCount();
    bool GetNetwork(NetworkInfo &nwkInfo, int index, const char *simPlmn, char *simSpn);

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetAvailableNetworkAdapterInterface->GetErrorCode(); }
};


/**
 * ProtocolNetGetPsServiceAdapter
 */
class ProtocolNetGetPsServiceAdapter : public ProtocolNetGetPsServiceAdapterInterface {
private:
    ProtocolNetGetPsServiceAdapterInterface *protocolNetGetPsServiceAdapterInterface = NULL;

public:
    ProtocolNetGetPsServiceAdapter(const ModemData *pModemData);
    ProtocolNetGetPsServiceAdapter(const ProtocolNetGetPsServiceAdapter &) = delete;
    ProtocolNetGetPsServiceAdapter& operator=(ProtocolNetGetPsServiceAdapter const&) = delete;
    virtual ~ProtocolNetGetPsServiceAdapter();

public:
    int GetState();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetPsServiceAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetDuplexModeRespAdapter
 */
class ProtocolNetDuplexModeRespAdapter : public ProtocolNetDuplexModeRespAdapterInterface {
private:
    ProtocolNetDuplexModeRespAdapterInterface *protocolNetDuplexModeRespAdapterInterface = NULL;

public:
    ProtocolNetDuplexModeRespAdapter(const ModemData *pModemData);
    ProtocolNetDuplexModeRespAdapter(const ProtocolNetDuplexModeRespAdapter &) = delete;
    ProtocolNetDuplexModeRespAdapter& operator=(ProtocolNetDuplexModeRespAdapter const&) = delete;
    virtual ~ProtocolNetDuplexModeRespAdapter();

public:
    int Get4gDuplexMode() const;
    int Get3gDuplexMode() const;
    int GetDuplexMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetDuplexModeRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetEmergencyActInfoAdapter
 */
class ProtocolNetEmergencyActInfoAdapter : public ProtocolNetEmergencyActInfoAdapterInterface {
private:
    ProtocolNetEmergencyActInfoAdapterInterface *protocolNetEmergencyActInfoAdapterInterface = NULL;

public:
    ProtocolNetEmergencyActInfoAdapter(const ModemData *pModemData);
    ProtocolNetEmergencyActInfoAdapter(const ProtocolNetEmergencyActInfoAdapter &) = delete;
    ProtocolNetEmergencyActInfoAdapter& operator=(ProtocolNetEmergencyActInfoAdapter const&) = delete;
    virtual ~ProtocolNetEmergencyActInfoAdapter();

public:
    int GetRat() const;
    int GetActStatus() const;
};

/**
 * ProtocolNetMcSrchRespAdapter
 */
class ProtocolNetMcSrchRespAdapter : public ProtocolNetMcSrchRespAdapterInterface {
private:
    ProtocolNetMcSrchRespAdapterInterface *protocolNetMcSrchRespAdapterInterface = NULL;

public:
    ProtocolNetMcSrchRespAdapter(const ModemData *pModemData);
    ProtocolNetMcSrchRespAdapter(const ProtocolNetMcSrchRespAdapter &) = delete;
    ProtocolNetMcSrchRespAdapter& operator=(ProtocolNetMcSrchRespAdapter const&) = delete;
    virtual ~ProtocolNetMcSrchRespAdapter();

public:
    int GetMcSrchResult() const;
    const char *GetMcSrchPlmn() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetMcSrchRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolSetNetworkRCRespAdapter
 */
class ProtocolSetNetworkRCRespAdapter : public ProtocolSetNetworkRCRespAdapterInterface {
private:
    ProtocolSetNetworkRCRespAdapterInterface *protocolSetNetworkRCRespAdapterInterface = NULL;

public:
    ProtocolSetNetworkRCRespAdapter(const ModemData *pModemData);
    ProtocolSetNetworkRCRespAdapter(const ProtocolSetNetworkRCRespAdapter &) = delete;
    ProtocolSetNetworkRCRespAdapter& operator=(ProtocolSetNetworkRCRespAdapter const&) = delete;
    virtual ~ProtocolSetNetworkRCRespAdapter();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolSetNetworkRCRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolGetNetworkRCRespAdapter
 */
class ProtocolGetNetworkRCRespAdapter : public ProtocolGetNetworkRCRespAdapterInterface {
private:
    ProtocolGetNetworkRCRespAdapterInterface *protocolGetNetworkRCRespAdapterInterface = NULL;

public:
    ProtocolGetNetworkRCRespAdapter(const ModemData *pModemData);
    ProtocolGetNetworkRCRespAdapter(const ProtocolGetNetworkRCRespAdapter &) = delete;
    ProtocolGetNetworkRCRespAdapter& operator=(ProtocolGetNetworkRCRespAdapter const&) = delete;
    virtual ~ProtocolGetNetworkRCRespAdapter();

public:
    int GetVersion() const;
    int GetSession() const;
    int GetPhase() const;
    int GetRafType() const;
    BYTE *GetUuid() const;
    int GetStatus() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolGetNetworkRCRespAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetworkRCIndAdapter
 */
class ProtocolNetworkRCIndAdapter : public ProtocolNetworkRCIndAdapterInterface {
private:
    ProtocolNetworkRCIndAdapterInterface *protocolNetworkRCIndAdapterInterface = NULL;

public:
    ProtocolNetworkRCIndAdapter(const ModemData *pModemData);
    ProtocolNetworkRCIndAdapter(const ProtocolNetworkRCIndAdapter &) = delete;
    ProtocolNetworkRCIndAdapter& operator=(ProtocolNetworkRCIndAdapter const&) = delete;
    virtual ~ProtocolNetworkRCIndAdapter();

public:
    //need to implement based on SIT definition when it is available
    int GetVersion() const;
    int GetSession() const;
    int GetPhase() const;
    int GetRafType() const;
    BYTE *GetUuid() const;
    int GetStatus() const;
};

/**
 * ProtocolNetCdmaQueryRoamingTypeAdapter
 */
class ProtocolNetCdmaQueryRoamingTypeAdapter : public ProtocolNetCdmaQueryRoamingTypeAdapterInterface {
private:
    ProtocolNetCdmaQueryRoamingTypeAdapterInterface *protocolNetCdmaQueryRoamingTypeAdapterInterface = NULL;

public:
    ProtocolNetCdmaQueryRoamingTypeAdapter(const ModemData *pModemData);
    ProtocolNetCdmaQueryRoamingTypeAdapter(const ProtocolNetCdmaQueryRoamingTypeAdapter &) = delete;
    ProtocolNetCdmaQueryRoamingTypeAdapter& operator=(ProtocolNetCdmaQueryRoamingTypeAdapter const&) = delete;
    virtual ~ProtocolNetCdmaQueryRoamingTypeAdapter();
public:
    int QueryRoamingType() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetCdmaQueryRoamingTypeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetCdmaHybridModeAdapter
 */
class ProtocolNetCdmaHybridModeAdapter : public ProtocolNetCdmaHybridModeAdapterInterface {
private:
    ProtocolNetCdmaHybridModeAdapterInterface *protocolNetCdmaHybridModeAdapterInterface = NULL;

public:
    ProtocolNetCdmaHybridModeAdapter(const ModemData *pModemData);
    ProtocolNetCdmaHybridModeAdapter(const ProtocolNetCdmaHybridModeAdapter &) = delete;
    ProtocolNetCdmaHybridModeAdapter& operator=(ProtocolNetCdmaHybridModeAdapter const&) = delete;
    virtual ~ProtocolNetCdmaHybridModeAdapter();
public:
    int GetCdmaHybridMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetCdmaHybridModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetMccAdapter
 */
class ProtocolNetMccAdapter : public ProtocolNetMccAdapterInterface {
private:
    ProtocolNetMccAdapterInterface *protocolNetMccAdapterInterface = NULL;

public:
    ProtocolNetMccAdapter(const ModemData *pModemData);
    ProtocolNetMccAdapter(const ProtocolNetMccAdapter &) = delete;
    ProtocolNetMccAdapter& operator=(ProtocolNetMccAdapter const&) = delete;
    virtual ~ProtocolNetMccAdapter();
public:
    int GetCurrentPrefNetworkMode() const;
    const char* GetMcc() const;
};

/**
 * ProtocolNetCellInfoListAdapter
 */
class ProtocolNetCellInfoListAdapter : public ProtocolNetCellInfoListAdapterInterface {
private:
    ProtocolNetCellInfoListAdapterInterface *protocolNetCellInfoListAdapterInterface = NULL;

public:
    ProtocolNetCellInfoListAdapter(const ModemData *pModemData);
    ProtocolNetCellInfoListAdapter(const ProtocolNetCellInfoListAdapter &) = delete;
    ProtocolNetCellInfoListAdapter& operator=(ProtocolNetCellInfoListAdapter const&) = delete;
    virtual ~ProtocolNetCellInfoListAdapter();

public:
    list<RIL_CellInfo_V1_6>& GetCellInfoList(int cellInfoVer);

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetCellInfoListAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetCellInfoListIndAdapter
 */
class ProtocolNetCellInfoListIndAdapter : public ProtocolNetCellInfoListIndAdapterInterface {
private:
    ProtocolNetCellInfoListIndAdapterInterface *protocolNetCellInfoListIndAdapterInterface = NULL;

public:
    ProtocolNetCellInfoListIndAdapter(const ModemData *pModemData);
    ProtocolNetCellInfoListIndAdapter(const ProtocolNetCellInfoListIndAdapter &) = delete;
    ProtocolNetCellInfoListIndAdapter& operator=(ProtocolNetCellInfoListIndAdapter const&) = delete;
    virtual ~ProtocolNetCellInfoListIndAdapter();

public:
    list<RIL_CellInfo_V1_6>& GetCellInfoList(int cellInfoVer);
};

/**
 * ProtocolNetScanResultAdapter
 */
class ProtocolNetScanResultAdapter : public ProtocolNetScanResultAdapterInterface {
private:
    ProtocolNetScanResultAdapterInterface *protocolNetScanResultAdapterInterface = NULL;

public:
    ProtocolNetScanResultAdapter(const ModemData *pModemData);
    ProtocolNetScanResultAdapter(const ProtocolNetScanResultAdapter &) = delete;
    ProtocolNetScanResultAdapter& operator=(ProtocolNetScanResultAdapter const&) = delete;
    virtual ~ProtocolNetScanResultAdapter();

public:
    int GetScanStatus() const;
    int GetScanResult() const;
    list<RIL_CellInfo_V1_6>& GetCellInfoList(int cellInfoVer);
};

/**
 * ProtocolNetSimFileInfoAdapter
 */
class ProtocolNetSimFileInfoAdapter : public ProtocolNetSimFileInfoAdapterInterface {
private:
    ProtocolNetSimFileInfoAdapterInterface *protocolNetSimFileInfoAdapterInterface = NULL;

public:
    ProtocolNetSimFileInfoAdapter(const ModemData *pModemData);
    ProtocolNetSimFileInfoAdapter(const ProtocolNetSimFileInfoAdapter &) = delete;
    ProtocolNetSimFileInfoAdapter& operator=(ProtocolNetSimFileInfoAdapter const&) = delete;
    virtual ~ProtocolNetSimFileInfoAdapter();

public:
    int GetSimFileId() const;
    int GetRecordLen() const;
    int GetNumOfRecords() const;
    BYTE **GetSimFileData() const;
};

/**
 * ProtocolNetPhysicalChannelConfigs
 */
class ProtocolNetPhysicalChannelConfigs : public ProtocolNetPhysicalChannelConfigsInterface {
private:
    ProtocolNetPhysicalChannelConfigsInterface *protocolNetPhysicalChannelConfigsInterface = NULL;

public:
    ProtocolNetPhysicalChannelConfigs(const ModemData *pModemData);
    ProtocolNetPhysicalChannelConfigs(const ProtocolNetPhysicalChannelConfigs &) = delete;
    ProtocolNetPhysicalChannelConfigs& operator=(ProtocolNetPhysicalChannelConfigs const&) = delete;
    virtual ~ProtocolNetPhysicalChannelConfigs();

    std::list<PhysicalChannelConfigsData>& GetConfigList();
    const char *GetParameter() const;
    UINT GetParameterLength() const;
};

/**
 * ProtocolNetGetManualRatModeAdapter
 */
class ProtocolNetGetManualRatModeAdapter : public ProtocolNetGetManualRatModeAdapterInterface {
private:
    ProtocolNetGetManualRatModeAdapterInterface *protocolNetGetManualRatModeAdapterInterface = NULL;

public:
    ProtocolNetGetManualRatModeAdapter(const ModemData *pModemData);
    ProtocolNetGetManualRatModeAdapter(const ProtocolNetGetManualRatModeAdapter &) = delete;
    ProtocolNetGetManualRatModeAdapter& operator=(ProtocolNetGetManualRatModeAdapter const&) = delete;
    virtual ~ProtocolNetGetManualRatModeAdapter();
public:
    void GetManualRatMode(void *data);

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetManualRatModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetSetManualRatModeAdapter
 */
class ProtocolNetSetManualRatModeAdapter : public ProtocolNetSetManualRatModeAdapterInterface {
private:
    ProtocolNetSetManualRatModeAdapterInterface *protocolNetSetManualRatModeAdapterInterface = NULL;

public:
    ProtocolNetSetManualRatModeAdapter(const ModemData *pModemData);
    ProtocolNetSetManualRatModeAdapter(const ProtocolNetSetManualRatModeAdapter &) = delete;
    ProtocolNetSetManualRatModeAdapter& operator=(ProtocolNetSetManualRatModeAdapter const&) = delete;
    virtual ~ProtocolNetSetManualRatModeAdapter();
public:
    int GetCause() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetSetManualRatModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetGetFreqLockAdapter
 */
class ProtocolNetGetFreqLockAdapter : public ProtocolNetGetFreqLockAdapterInterface {
private:
    ProtocolNetGetFreqLockAdapterInterface *protocolNetGetFreqLockAdapterInterface = NULL;

public:
    ProtocolNetGetFreqLockAdapter(const ModemData *pModemData);
    ProtocolNetGetFreqLockAdapter(const ProtocolNetGetFreqLockAdapter &) = delete;
    ProtocolNetGetFreqLockAdapter& operator=(ProtocolNetGetFreqLockAdapter const&) = delete;
    virtual ~ProtocolNetGetFreqLockAdapter();
public:
    void GetFrequencyLock(void *data);

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetFreqLockAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetSetFreqLockAdapter
 */
class ProtocolNetSetFreqLockAdapter : public ProtocolNetSetFreqLockAdapterInterface {
private:
    ProtocolNetSetFreqLockAdapterInterface *protocolNetSetFreqLockAdapterInterface = NULL;

public:
    ProtocolNetSetFreqLockAdapter(const ModemData *pModemData);
    ProtocolNetSetFreqLockAdapter(const ProtocolNetSetFreqLockAdapter &) = delete;
    ProtocolNetSetFreqLockAdapter& operator=(ProtocolNetSetFreqLockAdapter const&) = delete;
    virtual ~ProtocolNetSetFreqLockAdapter();
public:
    int GetResult() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetSetFreqLockAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetGetEndcModeAdapter
 */
class ProtocolNetGetEndcModeAdapter : public ProtocolNetGetEndcModeAdapterInterface {
private:
    ProtocolNetGetEndcModeAdapterInterface *protocolNetGetEndcModeAdapterInterface = NULL;

public:
    ProtocolNetGetEndcModeAdapter(const ModemData *pModemData);
    ProtocolNetGetEndcModeAdapter(const ProtocolNetGetEndcModeAdapter &) = delete;
    ProtocolNetGetEndcModeAdapter& operator=(ProtocolNetGetEndcModeAdapter const&) = delete;
    virtual ~ProtocolNetGetEndcModeAdapter();
public:
    int GetEndcMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetEndcModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetworkFrequencyInfoIndAdapter
 */
class ProtocolNetworkFrequencyInfoIndAdapter : public ProtocolNetworkFrequencyInfoIndAdapterInterface {
private:
    ProtocolNetworkFrequencyInfoIndAdapterInterface *protocolNetworkFrequencyInfoIndAdapterInterface = NULL;

public:
    ProtocolNetworkFrequencyInfoIndAdapter(const ModemData *pModemData);
    ProtocolNetworkFrequencyInfoIndAdapter(const ProtocolNetworkFrequencyInfoIndAdapter &) = delete;
    ProtocolNetworkFrequencyInfoIndAdapter& operator=(ProtocolNetworkFrequencyInfoIndAdapter const&) = delete;
    virtual ~ProtocolNetworkFrequencyInfoIndAdapter();

public:
    int GetPrimaryRat() const;
    int GetPrimaryBand() const;
    int GetPrimaryFrequency() const;
    int GetSecondaryRat() const;
    int GetSecondaryBand() const;
    int GetSecondaryFrequency() const;
};

/**
 * ProtocolNetAcBarringInfo
 */
class ProtocolNetAcBarringInfo : public ProtocolNetAcBarringInfoInterface {
private:
    ProtocolNetAcBarringInfoInterface *protocolNetAcBarringInfoInterface = NULL;

public:
    ProtocolNetAcBarringInfo(const ModemData *pModemData);
    ProtocolNetAcBarringInfo(const ProtocolNetAcBarringInfo &) = delete;
    ProtocolNetAcBarringInfo& operator=(ProtocolNetAcBarringInfo const&) = delete;
    virtual ~ProtocolNetAcBarringInfo();

public:
    void GetAcBarringInfo(void *data, unsigned int size);
};

/**
 * ProtocolNetGetFrequencyInfoAdapter
 */
class ProtocolNetGetFrequencyInfoAdapter : public ProtocolNetGetFrequencyInfoAdapterInterface {
private:
    ProtocolNetGetFrequencyInfoAdapterInterface *protocolNetGetFrequencyInfoAdapterInterface = NULL;

public:
    ProtocolNetGetFrequencyInfoAdapter(const ModemData *pModemData);
    ProtocolNetGetFrequencyInfoAdapter(const ProtocolNetGetFrequencyInfoAdapter &) = delete;
    ProtocolNetGetFrequencyInfoAdapter& operator=(ProtocolNetGetFrequencyInfoAdapter const&) = delete;

    virtual ~ProtocolNetGetFrequencyInfoAdapter();
public:
    int GetPrimaryRat() const;
    int GetPrimaryBand() const;
    int GetPrimaryFrequency() const;
    int GetSecondaryRat() const;
    int GetSecondaryBand() const;
    int GetSecondaryFrequency() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetFrequencyInfoAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetRrcInfoAdapter
 */
class ProtocolNetRrcInfoAdapter : public ProtocolNetRrcInfoAdapterInterface {
private:
    ProtocolNetRrcInfoAdapterInterface *protocolNetRrcInfoAdapterInterface = NULL;

public:
    ProtocolNetRrcInfoAdapter(const ModemData *pModemData);
    ProtocolNetRrcInfoAdapter(const ProtocolNetRrcInfoAdapter &) = delete;
    ProtocolNetRrcInfoAdapter& operator=(ProtocolNetRrcInfoAdapter const&) = delete;
    virtual ~ProtocolNetRrcInfoAdapter();
public:
    int GetRat() const;
    int GetState() const;
};

/**
 * ProtocolNetGetNrModeAdapter
 */
class ProtocolNetGetNrModeAdapter : public ProtocolNetGetNrModeAdapterInterface {
private:
    ProtocolNetGetNrModeAdapterInterface *protocolNetGetNrModeAdapterInterface = NULL;

public:
    ProtocolNetGetNrModeAdapter(const ModemData *pModemData);
    ProtocolNetGetNrModeAdapter(const ProtocolNetGetNrModeAdapter &) = delete;
    ProtocolNetGetNrModeAdapter& operator=(ProtocolNetGetNrModeAdapter const&) = delete;
    virtual ~ProtocolNetGetNrModeAdapter();
public:
    int GetNrMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetNrModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetGetNrSilenceModeAdapter
 */
class ProtocolNetGetNrSilenceModeAdapter : public ProtocolNetGetNrSilenceModeAdapterInterface {
private:
    ProtocolNetGetNrSilenceModeAdapterInterface *protocolNetGetNrSilenceModeAdapterInterface = NULL;

public:
    ProtocolNetGetNrSilenceModeAdapter(const ModemData *pModemData);
    ProtocolNetGetNrSilenceModeAdapter(const ProtocolNetGetNrSilenceModeAdapter &) = delete;
    ProtocolNetGetNrSilenceModeAdapter& operator=(ProtocolNetGetNrSilenceModeAdapter const&) = delete;
    virtual ~ProtocolNetGetNrSilenceModeAdapter();
public:
    bool GetNsaMode() const;
    bool GetSaMode() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetNrSilenceModeAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolRegistrationFailedAdapter
 */
class ProtocolRegistrationFailedAdapter : public ProtocolRegistrationFailedAdapterInterface {
private:
    ProtocolRegistrationFailedAdapterInterface *protocolRegistrationFailedAdapterInterface = NULL;

public:
    ProtocolRegistrationFailedAdapter(const ModemData *pModemData, int cellIdentityVer);
    ProtocolRegistrationFailedAdapter(const ProtocolRegistrationFailedAdapter &) = delete;
    ProtocolRegistrationFailedAdapter& operator=(ProtocolRegistrationFailedAdapter const&) = delete;
    virtual ~ProtocolRegistrationFailedAdapter();

public:
    RIL_RegistrationFailed *GetRegistrationFailed();
};

/**
 * ProtocolGetBarringInfoRspAdapter
 */
class ProtocolGetBarringInfoRspAdapter : public ProtocolGetBarringInfoRspAdapterInterface {
private:
    ProtocolGetBarringInfoRspAdapterInterface *protocolGetBarringInfoRspAdapterInterface = NULL;

public:
    ProtocolGetBarringInfoRspAdapter(const ModemData *pModemData, int cellIdentityVer);
    ProtocolGetBarringInfoRspAdapter(const ProtocolGetBarringInfoRspAdapter &) = delete;
    ProtocolGetBarringInfoRspAdapter& operator=(ProtocolGetBarringInfoRspAdapter const&) = delete;
    virtual ~ProtocolGetBarringInfoRspAdapter();

    // For ProtocolBarringInfos
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolGetBarringInfoRspAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolBarringInfoChangedAdapter
 */
class ProtocolBarringInfoChangedAdapter : public ProtocolBarringInfoChangedAdapterInterface {
private:
    ProtocolBarringInfoChangedAdapterInterface *protocolBarringInfoChangedAdapterInterface = NULL;

public:
    ProtocolBarringInfoChangedAdapter(const ModemData *pModemData, int cellIdentityVer);
    ProtocolBarringInfoChangedAdapter(const ProtocolBarringInfoChangedAdapter &) = delete;
    ProtocolBarringInfoChangedAdapter& operator=(ProtocolBarringInfoChangedAdapter const&) = delete;
    virtual ~ProtocolBarringInfoChangedAdapter();

    // For ProtocolBarringInfos
    RIL_CellIdAndBarringInfo *GetCellIdAndBarringInfo();
};

/**
 * ProtocolNetGetVonrCapaAdapter
 */
class ProtocolNetGetVonrCapaAdapter : public ProtocolNetGetVonrCapaAdapterInterface {
private:
    ProtocolNetGetVonrCapaAdapterInterface *protocolNetGetVonrCapaAdapterInterface = NULL;

public:
    ProtocolNetGetVonrCapaAdapter(const ModemData *pModemData);
    ProtocolNetGetVonrCapaAdapter(const ProtocolNetGetVonrCapaAdapter &) = delete;
    ProtocolNetGetVonrCapaAdapter& operator=(ProtocolNetGetVonrCapaAdapter const&) = delete;
    virtual ~ProtocolNetGetVonrCapaAdapter();
public:
    int GetVonrCapa() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetVonrCapaAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetGetAllowNetworkAdapter
 */
class ProtocolNetGetAllowNetworkAdapter : public ProtocolNetGetAllowNetworkAdapterInterface {
private:
    ProtocolNetGetAllowNetworkAdapterInterface *protocolNetGetAllowNetworkAdapterInterface = NULL;

public:
    ProtocolNetGetAllowNetworkAdapter(const ModemData *pModemData);
    ProtocolNetGetAllowNetworkAdapter(const ProtocolNetGetAllowNetworkAdapter &) = delete;
    ProtocolNetGetAllowNetworkAdapter& operator=(ProtocolNetGetAllowNetworkAdapter const&) = delete;
    virtual ~ProtocolNetGetAllowNetworkAdapter();
public:
    UINT32 GetRat() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetAllowNetworkAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetSystemSelectionChannelsAdapter
 */
class ProtocolNetSystemSelectionChannelsAdapter : public ProtocolNetSystemSelectionChannelsAdapterInterface {
private:
    ProtocolNetSystemSelectionChannelsAdapterInterface *protocolNetSystemSelectionChannelsAdapterInterface = NULL;

public:
    ProtocolNetSystemSelectionChannelsAdapter(const ModemData *pModemData);
    ProtocolNetSystemSelectionChannelsAdapter(const ProtocolNetSystemSelectionChannelsAdapter &) = delete;
    ProtocolNetSystemSelectionChannelsAdapter& operator=(ProtocolNetSystemSelectionChannelsAdapter const&) = delete;
    virtual ~ProtocolNetSystemSelectionChannelsAdapter();
public:
    int GetSpecifiersLength();
    RIL_RadioAccessSpecifier_V1_5 *GetRadioAccessSpecifier();

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetSystemSelectionChannelsAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetB1B2ConfigIndAdapter
 */
class ProtocolNetB1B2ConfigIndAdapter : public ProtocolNetB1B2ConfigIndAdapterInterface {
private:
    ProtocolNetB1B2ConfigIndAdapterInterface *protocolNetB1B2ConfigIndAdapterInterface = NULL;

public:
    ProtocolNetB1B2ConfigIndAdapter(const ModemData *pModemData);
    ProtocolNetB1B2ConfigIndAdapter(const ProtocolNetB1B2ConfigIndAdapter &) = delete;
    ProtocolNetB1B2ConfigIndAdapter& operator=(ProtocolNetB1B2ConfigIndAdapter const&) = delete;
    virtual ~ProtocolNetB1B2ConfigIndAdapter();
public:
    const char *GetParameter() const { return protocolNetB1B2ConfigIndAdapterInterface->GetParameter(); }
    UINT GetParameterLength() const { return protocolNetB1B2ConfigIndAdapterInterface->GetParameterLength(); }
};

/*
 * ProtocolNetGetDeviceService
 */
class ProtocolNetGetDeviceServiceAdapter : public ProtocolNetGetDeviceServiceAdapterInterface {
private:
    ProtocolNetGetDeviceServiceAdapterInterface *protocolNetGetDeviceServiceAdapterInterface = NULL;

public:
    ProtocolNetGetDeviceServiceAdapter(const ModemData *pModemData);
    ProtocolNetGetDeviceServiceAdapter(const ProtocolNetGetDeviceServiceAdapter &) = delete;
    ProtocolNetGetDeviceServiceAdapter& operator=(ProtocolNetGetDeviceServiceAdapter const&) = delete;
    virtual ~ProtocolNetGetDeviceServiceAdapter();

    int GetUsageSetting() const;

    //For ProtocolRespAdapter
    UINT GetErrorCode() const {return protocolNetGetDeviceServiceAdapterInterface->GetErrorCode(); }
};

/**
 * ProtocolNetDsdsStatusIndAdapter
 */
class ProtocolNetDsdsStatusIndAdapter : public ProtocolNetDsdsStatusIndAdapterInterface {
private:
    ProtocolNetDsdsStatusIndAdapterInterface *protocolNetDsdsStatusIndAdapterInterface = nullptr;
public:
    ProtocolNetDsdsStatusIndAdapter(const ModemData *pModemData);
    ProtocolNetDsdsStatusIndAdapter(const ProtocolNetDsdsStatusIndAdapter &) = delete;
    ProtocolNetDsdsStatusIndAdapter& operator=(ProtocolNetDsdsStatusIndAdapter const&) = delete;
    virtual ~ProtocolNetDsdsStatusIndAdapter();

    int GetStatus() const;
    bool IsPsRestricted() const;
};
#endif /* __PROTOCOL_NET_ADAPTER_H__ */
