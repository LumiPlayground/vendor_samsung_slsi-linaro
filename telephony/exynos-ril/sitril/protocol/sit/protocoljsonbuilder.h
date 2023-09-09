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
 * protocoljsonbuilder.h
 *
 *  Created on: 2021. 07. 22.
 *      Author: p1.bansal
 */

#ifndef __PROTOCOL_JSON_BUILDER_H__
#define __PROTOCOL_JSON_BUILDER_H__

#include "protocolbuilder.h"
#include "json/json.h"

class ModemData;

class ProtocolJsonBuilder : public ProtocolBuilder
{
public:
    ProtocolJsonBuilder() : ProtocolBuilder() {}
    virtual ~ProtocolJsonBuilder() = default;

protected:
    ModemData *GetJsonReqModemData(Json::Value &jsonReqData, int id);
    ModemData *GetJsonNullReqModemData(int id);
};

#endif /* __PROTOCOL_JSON_BUILDER_H__ */
