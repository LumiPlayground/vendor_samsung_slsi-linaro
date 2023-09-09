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
 * timeutils.h
 *
 *  Created on: 2021. 9. 8.
 */

#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#include <string>

namespace timeutils {
    // milliseconds
    uint64_t getTimestamp();
    // seconds
    uint64_t getCurrentTime();
    std::string getTimezone();
    uint64_t getTimezoneOffset();
}

#endif /* __TIME_UTILS_H__ */
