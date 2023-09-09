/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __SIM_LOCK_HANDLER_H__
#define __SIM_LOCK_HANDLER_H__

class SimService;
class Message;
class SimAutoVerifyPinHandler;

#define BUFFER_SIZE (128)

class SimLockHandler
{
    friend SimService;
    friend SimIoService;
	friend SimAutoVerifyPinHandler;

public:
    typedef enum {
        SIM_PIN,
        SIM_PUK
    } SIM_PIN_PUK;

    typedef enum {
        PIN1_PUK1 = 0,
        PIN2_PUK2
    } SIM_PIN_PUK_INDEX;

private:
    SimService *m_pSimService;

    // PIN2 for FDN
    ModemData *m_pEnableFdnData;
	SimAutoVerifyPinHandler *m_pSimAutoVerifyPinHandler;

protected:
    int m_nPinRemain[2];
    int m_nPukRemain[2];

protected:
    SimLockHandler();
    SimLockHandler(const SimLockHandler&) = delete;
    SimLockHandler& operator=(SimLockHandler const&) = delete;
    virtual ~SimLockHandler();
    virtual void Init(SimService* pSimService);

    virtual int DoSetFacilityLock(Message *pMsg);
    virtual int OnSetFacilityLockDone(Message *pMsg);

    virtual int DoVerifyPin(Message *pMsg);
    virtual int OnVerifyPinDone(Message *pMsg);

    virtual int DoVerifyPin2(Message *pMsg);
    virtual int OnVerifyPin2Done(Message *pMsg);

    virtual int DoVerifyPuk(Message *pMsg);
    virtual int OnVerifyPukDone(Message *pMsg);

    virtual int DoVerifyPuk2(Message *pMsg);
    virtual int OnVerifyPuk2Done(Message *pMsg);

    virtual int DoChangePin(Message *pMsg);
    virtual int OnChangePinDone(Message *pMsg);

    virtual int DoChangePin2(Message *pMsg);
    virtual int OnChangePin2Done(Message *pMsg);

    virtual int DoVerifyNetworkLock(Message *pMsg);
    virtual int OnVerifyNetworkLockDone(Message *pMsg);

    BOOL IsValidPinPuk(int nSimPinPuk, const char *pszPinPuk);
    int SendPasswordInvalid(int nSimPinPuk, int nPinPukIndex);
    void SetPinPukRemain(int nSimPinPuk, int nPinPukIndex, int nRemainCount);
    void InitializePinPuk();
    int GetPinState();
};

#endif /* __SIM_LOCK_HANDLER_H__ */
