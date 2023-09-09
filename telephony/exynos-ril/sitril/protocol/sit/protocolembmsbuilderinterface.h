/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_EMBMS_BUILDER_INTERFACE_H__
#define __PROTOCOL_EMBMS_BUILDER_INTERFACE_H__

#include <base/types.h>

class ModemData;

class ProtocolEmbmsBuilderInterface {
public:
    ProtocolEmbmsBuilderInterface() {}
    virtual ~ProtocolEmbmsBuilderInterface() {}
public:
    virtual ModemData *BuildSetService(int netType) = 0;
    virtual ModemData *BuildSetSession(int state, uint64_t tmgi, int saiListLen,
            const uint32_t *pSaiList, int freqListLen, const uint32_t *pFreqList) = 0;
    virtual ModemData *BuildGetSessionList(int state) = 0;
    virtual ModemData *BuildSignalStrength() = 0;
    virtual ModemData *BuildNetworkTime() = 0;
};

#endif /* __PROTOCOL_EMBMS_BUILDER_INTERFACE_H__ */
