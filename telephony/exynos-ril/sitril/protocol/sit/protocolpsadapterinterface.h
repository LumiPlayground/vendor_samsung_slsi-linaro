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
 * protocolpsadapterinterface.h
 *
 *  Created on: 2021. 10. 18.
 *      Author: juneho.kwon
 */

#ifndef ProtocolPsAdapterInterface_H
#define ProtocolPsAdapterInterface_H

#include <telephony/ril.h>
#include <base/rildef.h>
#include "sitdef.h"

/**
 * ProtocolPsSetupDataCallAdapterInterface
 */
class ProtocolPsSetupDataCallAdapterInterface {
public:
    ProtocolPsSetupDataCallAdapterInterface() = default;
    virtual ~ProtocolPsSetupDataCallAdapterInterface() = default;

public:
    virtual int GetStatus() const = 0;
    virtual int GetCid() const  = 0;
    virtual int GetActiveStatus() const  = 0;
    virtual int GetPdpType() const = 0;
    virtual int GetAddrInfo(PDP_ADDR *pAddr, int pdpType) const = 0;
    virtual int GetAddrInfo(PDP_ADDR *pAddr) const = 0;
    //int GetMTU() const { return m_dataCall.mtu_size; }
    virtual int GetMTUv4() const = 0;
    virtual int GetMTUv6() const = 0;
    virtual int GetPCO() const = 0;
    virtual int64_t GetSuggestedRetryTime() const = 0;
    virtual const DataCall *GetDataCall() const = 0;
    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;

};

/**
 * ProtocolPsDataCallListAdapterInterface
 */
class ProtocolPsDataCallListAdapterInterface {
public:
    ProtocolPsDataCallListAdapterInterface() = default;
    virtual ~ProtocolPsDataCallListAdapterInterface() = default;

public:
    virtual unsigned int GetDataCallNum() const = 0;
    virtual const DataCall *GetDataCallList() const = 0;
    virtual const DataCall *GetDataCallByIndex(unsigned int index) const = 0;
    virtual const DataCall *GetDataCallByCid(int cid) const = 0;
    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolPsDataCallListChangedAdapterInterface
 */
class ProtocolPsDataCallListChangedAdapterInterface {
public:
    ProtocolPsDataCallListChangedAdapterInterface() = default;
    virtual ~ProtocolPsDataCallListChangedAdapterInterface() = default;

public:
    virtual unsigned int GetDataCallNum() const = 0;
    virtual const DataCall *GetDataCallList() const = 0;
    virtual const DataCall *GetDataCallByIndex(unsigned int index) const = 0;
    virtual const DataCall *GetDataCallByCid(int cid) const = 0;
    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolPsDedicatedBearInfoAdapterInterface
 */
class ProtocolPsDedicatedBearInfoAdapterInterface {
public:
    ProtocolPsDedicatedBearInfoAdapterInterface() = default;
    virtual ~ProtocolPsDedicatedBearInfoAdapterInterface() = default;

public:
    virtual const DedicatedBearerInfo *GetDedicatedBearerInfo() = 0;
    virtual int GetCid() const  = 0;
    virtual int GetStatus() const  = 0;
    virtual const QosSession_t &GetQosSessionInfo() const  = 0;
};

/**
 * ProtocolPsNasTimerStatusAdapterInterface
 */
class ProtocolPsNasTimerStatusAdapterInterface {
public:
    ProtocolPsNasTimerStatusAdapterInterface() = default;
    virtual ~ProtocolPsNasTimerStatusAdapterInterface() = default;

public:
    virtual const SitNasTimerStatus *GetNasTimerStatus() = 0;
    virtual const char *GetParameter() const = 0;
    virtual UINT GetParameterLength() const = 0;
};

/**
 * ProtocolPsStartKeepAliveAdapterInterface
 */
class ProtocolPsStartKeepAliveAdapterInterface {
public:
    ProtocolPsStartKeepAliveAdapterInterface() = default;
    virtual ~ProtocolPsStartKeepAliveAdapterInterface() = default;

public:
    virtual unsigned int getSessionHandle() const = 0;
    virtual int getCode() const = 0;
    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolPsKeepAliveStatusAdapterInterface
 */
class ProtocolPsKeepAliveStatusAdapterInterface {
public:
    ProtocolPsKeepAliveStatusAdapterInterface() = default;
    virtual ~ProtocolPsKeepAliveStatusAdapterInterface() = default;

public:
    virtual unsigned int getSessionHandle() const = 0;
    virtual int getCode() const = 0;
};

/**
 * ProtocolPsPcoDataAdapterInterface
 */
class ProtocolPsPcoDataAdapterInterface {
public:
    ProtocolPsPcoDataAdapterInterface() = default;
    virtual ~ProtocolPsPcoDataAdapterInterface() = default;

public:
    // Per CID and SIT_IND
    virtual int GetCid() const = 0;
    virtual int GetPdpType() const = 0;
    virtual int GetPcoNum() const = 0;

    // Per PCO Block
    virtual int GetPcoData(sit_pdp_pco_data_entry &) = 0;
};

/**
 * ProtocolPsDataOffStatusAdapterInterface
 */
class ProtocolPsDataOffStatusAdapterInterface {
public:
    ProtocolPsDataOffStatusAdapterInterface() = default;
    virtual ~ProtocolPsDataOffStatusAdapterInterface() = default;

public:
    virtual int GetPsDataOffAvailableStatus() const = 0;
    virtual int GetExemptListForHome() const = 0;
    virtual int GetExemptListForRoam() const = 0;
};

/**
 * ProtocolPsIpAddrStatusAdapterInterface
 */
class ProtocolPsIpAddrStatusAdapterInterface {
public:
    ProtocolPsIpAddrStatusAdapterInterface() = default;
    virtual ~ProtocolPsIpAddrStatusAdapterInterface() = default;

public:
    virtual int GetCid() const = 0;
    virtual int GetStatus() const = 0;
    virtual void GetAddrInfo(char (&)[MAX_IPV4_ADDR_LEN], char (&)[MAX_IPV6_ADDR_LEN]) const = 0;
};

/**
 * ProtocolPsUnthrottleApnInterface
 */
class ProtocolPsUnthrottleApnInterface {
public:
    ProtocolPsUnthrottleApnInterface() = default;
    virtual ~ProtocolPsUnthrottleApnInterface() = default;

public:
    virtual const char* GetApn() const = 0;
};

/**
 * ProtocolPsAllocatePduSessionIdInterface
 */
class ProtocolPsAllocatePduSessionIdInterface {
public:
    ProtocolPsAllocatePduSessionIdInterface() = default;
    virtual ~ProtocolPsAllocatePduSessionIdInterface() = default;

    virtual int GetSessionId() const = 0;
    //For ProtocolResAdatper
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolPsSlicingConfigInterface
 */
class ProtocolPsSlicingConfigInterface {
public:
    ProtocolPsSlicingConfigInterface() = default;
    virtual ~ProtocolPsSlicingConfigInterface() = default;

public:
    virtual DataV2_0::RIL_SlicingConfig GetSlicingConfig() const = 0;
    virtual int GetPayloadSize() const  = 0;
    virtual UINT GetErrorCode() const = 0;
};

/**
 * ProtocolPsSlicingConfigUpdatedInterface
 */
class ProtocolPsSlicingConfigUpdatedInterface {
public:
    ProtocolPsSlicingConfigUpdatedInterface() = default;
    virtual ~ProtocolPsSlicingConfigUpdatedInterface() = default;

public:
    virtual DataV2_0::RIL_SlicingConfig GetSlicingConfig() const = 0;
    virtual int GetPayloadSize() const  = 0;
    virtual UINT GetErrorCode() const = 0;
};

#endif
