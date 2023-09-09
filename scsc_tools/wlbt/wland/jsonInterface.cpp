/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <utils/Log.h>

#include "jsonInterface.h"

bool jsonInterface::parserJsonString(char* buf) {
    if (buf == NULL) {
        ALOGD("jsonInterface : buf null");
        return false;
    }

    bool is_success = reader.parse(buf, root);
    if (!is_success) {
        ALOGD("jsonInterface : Json Parsing error");
        return false;
    }
    return true;
}

std::string jsonInterface::getStringValue(std::string key) {
    if (key.length() == 0) {
        return "";
    }

    bool is_member = root.isMember(key);
    if (!is_member) {
        return "";
    }
    return root.get(key, "").asString();
}

