/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <stdio.h>
#include <string.h>

int Value2HexString(char *pszHexStrOut, const unsigned char *pValueIn, int nLength)
{
    int nResult = -1;
    if (pszHexStrOut && pValueIn && 0 < nLength) {
        int nSrcOffset = 0;
        for (int i = 0; i < nLength; i++) {
            nSrcOffset +=
                    snprintf(&pszHexStrOut[nSrcOffset], 3, "%02X", (unsigned int) pValueIn[i]);
        }
        pszHexStrOut[nSrcOffset] = '\0';
        nResult = nSrcOffset;
    }
    return nResult;
}

int HexChar2Value(char ch)
{
    if ('0' <= ch && ch <= '9')
        return (int)(ch - '0');
    else if ('a' <= ch && ch <= 'f')
        return (int)(ch - 'a') + 10;
    else if ('A' <= ch && ch <= 'F')
        return (int)(ch - 'A') + 10;
    return -1;
}

int HexString2Value(unsigned char *pValueOut, const char *pszHexStrIn)
{
    int nResult = 0;
    if (pValueOut == NULL || pszHexStrIn == NULL) {
        return nResult;
    }

    int nLength = strlen(pszHexStrIn);
    if (pValueOut && pszHexStrIn && (nLength % 2) == 0) {
        int i = 0;
        for (i = 0; i<nLength && pszHexStrIn[i*2] != '\0'; i++) {
            int nVal = HexChar2Value(pszHexStrIn[i*2]) << 4;
            nVal |= HexChar2Value(pszHexStrIn[(i*2) + 1]);
            pValueOut[i] = (unsigned char) (nVal & 0xFF);
        }
        nResult = i;
    }
    return nResult;
}

