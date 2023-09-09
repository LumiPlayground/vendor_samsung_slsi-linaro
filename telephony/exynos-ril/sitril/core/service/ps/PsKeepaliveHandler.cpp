/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsKeepaliveHandler.h"
#include "rillog.h"
#include "psservice.h"
#include "protocolpsbuilder.h"
#include "protocolpsadapter.h"

PsKeepaliveHandler::PsKeepaliveHandler()
{
    m_pPsService = NULL;
}

PsKeepaliveHandler::~PsKeepaliveHandler()
{
}

void PsKeepaliveHandler::Init(PsService* pPsService)
{
    if(pPsService != NULL)
        m_pPsService = pPsService;
}

bool PsKeepaliveHandler::isValidMessage(Message *pMsg)
{
    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return false;
    }
    return true;
}

bool PsKeepaliveHandler::isValidRespMessage(Message *pMsg)
{
    if (!isValidMessage(pMsg)) return false;

    ModemData *pModemData = pMsg->GetModemData();
    if (pModemData == NULL) {
        RilLogE("pModemData is NULL");
        return false;
    }
    return true;
}
