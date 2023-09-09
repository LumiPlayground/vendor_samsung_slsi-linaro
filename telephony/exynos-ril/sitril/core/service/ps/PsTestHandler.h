/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PsTestHandler_H__
#define __PsTestHandler_H__

class PsService;
class Message;
class ModemData;

class PsTestHandler  // Maybe inherit Handler with better abstraction
{
private:
    // Test Control flags
    const bool m_bTestNewIPCFormat = false;
    bool m_bForceFailAfterSuccess = false;

protected:
    PsService *m_pPsService;

public:
    PsTestHandler();
    virtual ~PsTestHandler();
    virtual void Init(PsService* pPsService);
    // type 0 = SetupDataCallDone, 1 = GetDataCallListDone, 2 = DataCallListChangedInd
    ModemData *replaceModemDataForTest(ModemData *oldData, int type);
    // One time Force fail
    int forceFailOnceForTest(int errorCode);
    void setForceFailOnceForTest() { m_bForceFailAfterSuccess = true; }
    static ModemData *generateModemDataForTest(int type);
    template<typename T>
    static int fillTestPayload(char *(&new_payload), T *header, const char *testdata, int len, bool &isTx);
};
#endif // __PsTestHandler_H__
