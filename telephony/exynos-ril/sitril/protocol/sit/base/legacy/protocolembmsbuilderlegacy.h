/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __PROTOCOL_EMBMS_BUILDER_LEGACY_H__
#define __PROTOCOL_EMBMS_BUILDER_LEGACY_H__

#include "protocolbuilder.h"
#include "protocolembmsbuilderinterface.h"

class ModemData;

class ProtocolEmbmsBuilderLegacy : public ProtocolEmbmsBuilderInterface, public ProtocolBuilder {
  private:
    char m_szTmgi[7];
    char *getStringTypeTmgi(uint64_t tmgi);

  public:
    ProtocolEmbmsBuilderLegacy() {}
    virtual ~ProtocolEmbmsBuilderLegacy() {}

  public:
    ModemData *BuildSetService(int netType);
    ModemData *BuildSetSession(int state, uint64_t tmgi, int saiListLen, const uint32_t *pSaiList,
                               int freqListLen, const uint32_t *pFreqList);
    ModemData *BuildGetSessionList(int state);
    ModemData *BuildSignalStrength();
    ModemData *BuildNetworkTime();
};

#endif /* __PROTOCOL_EMBMS_BUILDER_LEGACY_H__ */
