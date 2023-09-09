/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __TESTUTIL_H__
#define __TESTUTIL_H__

#include <string>

#define MAG "\033[38;5;127m"
#define GRN "\033[38;5;35m"
#define SCL "\033[38;2;255;128;128m"
#define DFT "\033[0;0m"

extern void OutV(std::string a);
extern bool isSameIp(char *a, char *b, int len);

#define TRACE()   (OutV("Run " + string(__PRETTY_FUNCTION__) + " @ "+ to_string(__LINE__) ) )

#endif  // __TESTUTIL_H__
