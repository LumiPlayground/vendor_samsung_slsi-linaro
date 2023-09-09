 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __NETWORK_SERVICE_H__
#define __NETWORK_SERVICE_H__

#include "networkservicebase.h"

class NetworkServiceProxy;
class RilContext;

class NetworkService : public NetworkServiceBase {
    friend NetworkServiceProxy;
    INTERFACE_AS_SERVICE(NetworkService, RIL_SERVICE_NETWORK)

protected:
    int m_lteRrcState = 0;
    bool mIsWfcEnabled;

public:
    NetworkService(RilContext* pRilContext);
    virtual ~NetworkService() = default;

public:
    int GetRegState(void);

private:

protected:
    virtual void OnRadioOffOrNotAvailable();

    virtual BOOL OnHandleRequest(Message* pMsg);
    virtual BOOL OnHandleSolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleUnsolicitedResponse(Message* pMsg);
    virtual BOOL OnHandleRequestTimeout(Message* pMsg);

    virtual int DoSetNetworkSelectionManual(Message *pMsg);
    virtual int OnSetNetworkSelectionManualDone(Message *pMsg);
    virtual int DoSetNetworkSelectionManualWithRat(Message *pMsg);

    virtual int DoQueryAvailableNetwork(Message *pMsg);
    virtual int OnQueryAvailableNetworkDone(Message *pMsg);
    virtual int OnQueryAvailableNetworkTimeout(Message *pMsg);
    virtual int DoQueryBplmnSearch(Message *pMsg);
    virtual int OnQueryBplmnSearchDone(Message *pMsg);

    virtual void OnVoiceRegistrationCustomNotification(int regState, int rejectCause);

    virtual int OnRrcInfo(Message *pMsg);
    void UpdateLteRrcState(int state, bool notify = true);

public:
    void UpdateNetworkNameOfNetworkScanResult(NetworkInfo &nwkInfo,
            const char *simOperatorNumeric, const char *operatorNumeric,
            const char *simSpn);
    BOOL IsPlmnSearching(void);
    UINT IsOppsiteStackOccupyRF();
    UINT IsCurrentStackOccupyRF();
};
#endif /*__NETWORK_SERVICE_H__*/
