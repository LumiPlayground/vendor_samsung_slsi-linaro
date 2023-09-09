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
 * sdmdocmanager.h
 *
 *  Created on: 2021. 9. 7.
 */

#ifndef __SDM_DOC_MANAGER_H__
#define __SDM_DOC_MANAGER_H__

#include "sdmdoc.h"

/**
 * SdmDocManager
 */
class SdmDocManager {
private:
    uint32_t mUid;
    uint32_t mSeq;
    int mType;

public:
    SdmDocManager();
    ~SdmDocManager()=default;
    uint32_t increaseUid();
    SdmDoc *newDocument();
    static std::string getFilename(const std::string& prefix, uint64_t date);
private:
    void init();
    std::string getNewFilename();
};

#endif /* __SDM_DOC_MANAGER_H__ */
