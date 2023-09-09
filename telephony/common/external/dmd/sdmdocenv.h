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
 * sdmdocenv.h
 *
 *  Created on: 2021. 9. 9.
 */

#ifndef __SDM_DOC_ENV_H__
#define __SDM_DOC_ENV_H__

#include <string>

namespace SdmDocEnv {
    std::string getBaseDir();
    void setBaseDir(const std::string& dir);
    std::string getFilenamePrefix();
    void setFilenamePrefix(const std::string& prefix);
    std::string getBaseDirDefault();
    int getSdmType();
    std::string getSdmType1Ext();
    std::string getSdmType2Ext();
    size_t getMaxDocSize();
    void setMaxDocSize(size_t size);
    bool isZipArachiveRequiredByConfig();
    bool isZipArachiveRequired(const std::string& filepath);
    uint64_t getCurrentTime();
}

#endif /* __SDM_DOC_ENV_H__ */
