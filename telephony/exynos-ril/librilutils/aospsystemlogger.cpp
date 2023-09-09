/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <librilutils/aospsystemlogger.h>

#define LOG_NDEBUG 0  // To enable VERBOSE LOG
#define LOG_TAG "RIL"
#include <utils/Log.h>

void AospSystemLogger::PrintRadioLogError(const std::ostringstream &oss) const {
    RLOGE("%s", oss.str().c_str());
}

void AospSystemLogger::PrintRadioLogWarning(const std::ostringstream &oss) const {
    RLOGW("%s", oss.str().c_str());
}

void AospSystemLogger::PrintRadioLogDebug(const std::ostringstream &oss) const {
    RLOGD("%s", oss.str().c_str());
}

void AospSystemLogger::PrintRadioLogInfo(const std::ostringstream &oss) const {
    RLOGI("%s", oss.str().c_str());
}

void AospSystemLogger::PrintRadioLogVerbose(const std::ostringstream &oss) const {
    RLOGV("%s", oss.str().c_str());
};
