 /*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#ifndef __MCCMNCCHANGERLOADER_H__
#define __MCCMNCCHANGERLOADER_H__

#include <string>

typedef struct
{
    std::string srcMccMnc;
    std::string desMccMnc;
} MccMncItem;

class MccMncChanger {
private:
    bool changeMccMncList(const char* mccmnc, char* changedMccMnc);
public:
    bool checkMccMncToChange(const char* mccmnc, char* changedMccMnc);
};
#endif
