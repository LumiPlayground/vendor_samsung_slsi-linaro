/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __SIM_AUTOVERIFYPIN_HANDLER_H__
#define __SIM_AUTOVERIFYPIN_HANDLER_H__

#include <base/types.h>

class Message;
class SimIoService;
class SimLockHandler;
class SimService;

#define PROPERTY_NAME_LEN (40)

class SimAutoVerifyPinHandler
{
    friend SimService;
    friend SimIoService;
    friend SimLockHandler;

public:
    typedef enum {
        BOOT_STATE_NORMAL,
        BOOT_STATE_KERNEL_PANIC,
        BOOT_STATE_UNKNOWN,
    } BOOT_STATE;

    typedef enum {
        SIM_PIN,
        SIM_PUK
    } SIM_PIN_PUK;

    typedef enum {
        PIN1_PUK1 = 0,
        PIN2_PUK2
    } SIM_PIN_PUK_INDEX;

    typedef enum {
        AUTO_PIN_STATE_DISABLED,       // Not Set PIN.
        AUTO_PIN_STATE_DISABLING,   // Disabing PIN
        AUTO_PIN_STATE_ENABLED,     // Setting auto pin is done.
        AUTO_PIN_STATE_ENABLING,      // Ready to set auto pin.
        AUTO_PIN_STATE_CP_CRASH,       // Ready to use auto verifying pin operation.
        AUTO_PIN_STATE_RECOVERY,    // Start Automatic Verify PIN.
    } AUTO_VERIFY_PIN_STATE;

private:
    SimService *m_pSimService;
    char m_szAutoPinPropertyKey[PROPERTY_NAME_LEN];

protected:
    int m_nAutoPinState;

public:
    int GetAutoPinState() { return m_nAutoPinState; }

protected:
    SimAutoVerifyPinHandler();
    virtual ~SimAutoVerifyPinHandler();
    virtual void Init(SimService* pSimService);

    // APIs
    int DoAutoVerifyPin();
    void SavePin(const char *pszPin);
    void GetAutoVerifyPin(char *pszPin);

    void ClearEncryptedPin();
    void EncryptAutoVerifyPin(const char *pszPin);
    void DecryptAutoVerifyPin(char *pszPin);

    void SetAutoPinState(int autoPinState);
    int CheckBootReason();
    bool IsEncryptedPinPropertyExist();

#ifdef AUTO_VERIFY_PIN_WITH_CP
    int OnReceiveEncryptedPin(Message *pMsg);
    int OnVerifyEncryptedPinDone(Message *pMsg);
#endif
};

#endif /* __SIM_AUTOVERIFYPIN_HANDLER_H__ */
