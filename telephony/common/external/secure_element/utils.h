/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __SE_UTILS__
#define __SE_UTILS__

int HexString2Value(unsigned char *pValueOut, const char *pszHexStrIn);
int Value2HexString(char *pszHexStrOut, const unsigned char *pValueIn, int nLength);

#endif  // #ifndef __SE_UTILS__
