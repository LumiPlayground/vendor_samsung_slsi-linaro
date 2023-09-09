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
 * RilContextWrapperHandler.h
 *
 *  Created on: 2021. 02. 19.
 */

#ifndef __RIL_CONTEXT_WRAPPER_HANDLER_H__
#define __RIL_CONTEXT_WRAPPER_HANDLER_H__

class RilContextWrapper;

class RilContextWrapperHandler {
    DECLARE_MODULE_TAG()

    // member
protected:
    RilContextWrapper *m_pRilContextWrapper;

public:
    RilContextWrapperHandler(RilContextWrapper *);
    virtual ~RilContextWrapperHandler();

    //@Overriding
public:
    void ProcessIccCardStatus(const void *data, unsigned int datalen);
    void ProcessImsi(const void *data, unsigned int datalen);
    void ProcessDeviceIdentity(const void *data, unsigned int datalen);
};

#endif  /* __RIL_CONTEXT_WRAPPER_HANDLER_H__ */
