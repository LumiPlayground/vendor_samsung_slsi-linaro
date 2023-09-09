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
 * RilHidlServiceManager.h
 *
 *  Created on: 2020. 11. 19.
 *      Author: sungwoo48.choi
 */

#ifndef __RIL_HIDL_SERVICE_MANAGER_H__
#define __RIL_HIDL_SERVICE_MANAGER_H__

#include <librilutils/vendor.h>

class RilHidlServiceManager {
private:
    static RilHidlServiceManager *instance;

    RilHidlServiceManager();
public:
    ~RilHidlServiceManager();
    void init();
    int getCurrentRadioHalVersionCode();
    int getMaxRadioHalVersionCode();

public:
    static RilHidlServiceManager *getInstance();
};

#endif /* __RIL_HIDL_SERVICE_MANAGER_H__ */
