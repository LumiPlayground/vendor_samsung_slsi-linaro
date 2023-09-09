/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include "protocoljsontestadapter.h"
#include "protocolutils.h"
#include <string>

/**
 * ProtocolJsonReqAdapter
 */
bool ProtocolJsonReqAdapter::CheckParsing(Json::Value &jsonReqData, unsigned int id, bool debug_print) const {
    if(GetParameter() != NULL && GetId() == id) {
        std::stringstream sstream(GetParameter());
        Json::CharReaderBuilder jsonReader;
        std::string errs;
        if(!Json::parseFromStream(jsonReader, sstream, &jsonReqData, &errs)) {
            printf("Error while parsing JSON data = (%s) for %s", errs.c_str(),
                    ProtocolUtils::rcmMsgToString(id));
            return false;
        }
    } else {
        printf("Error in GetParameter() or GetId() for %s", ProtocolUtils::rcmMsgToString(id));
        return false;
    }
    if (debug_print) {
        printf("[%s] (%s) JsonRespData = %s",  __FUNCTION__, ProtocolUtils::rcmMsgToString(id),
                GetParameter());
    }
    return true;
}
