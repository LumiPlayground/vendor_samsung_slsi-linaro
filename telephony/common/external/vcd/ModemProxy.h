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
 * ModemProxy.h
 *
 *  Created on: 2020. 2. 12.
 */

#ifndef __MODEM_PROXY_H__
#define __MODEM_PROXY_H__

#include <unistd.h>

class ModemProxy {
private:
    void Init();
public:
    ModemProxy();
    int Write(void *data, size_t datalen);
    void StopForcibly();
public:
    static ModemProxy *GetInstance();
};

#endif /* __MODEM_PROXY_H__ */
