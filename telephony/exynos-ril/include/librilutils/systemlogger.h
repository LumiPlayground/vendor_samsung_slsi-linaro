/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#pragma once

#include <memory>
#include <sstream>

class SystemLogger {
  public:
    virtual ~SystemLogger() = default;
    virtual void PrintRadioLogError(const std::ostringstream &oss) const = 0;
    virtual void PrintRadioLogWarning(const std::ostringstream &oss) const = 0;
    virtual void PrintRadioLogDebug(const std::ostringstream &oss) const = 0;
    virtual void PrintRadioLogInfo(const std::ostringstream &oss) const = 0;
    virtual void PrintRadioLogVerbose(const std::ostringstream &oss) const = 0;
};

using SystemLoggerPtr = std::shared_ptr<const SystemLogger>;