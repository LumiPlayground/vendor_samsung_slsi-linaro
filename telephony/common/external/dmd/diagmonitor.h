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
 * diagmonitor.h
 *
 *  Created on: 2021. 10. 13.
 *      Author: sungwoo48.choi
 */

#ifndef __DIAG_MONITOR_H__
#define __DIAG_MONITOR_H__

/**
 * OnRawRequestListener
 */
struct OnRawRequestListener {
    virtual ~OnRawRequestListener()=default;
    virtual void onRawRequest(const void *data, size_t datalen)=0;
};

#endif /* __DIAG_MONITOR_H__ */
