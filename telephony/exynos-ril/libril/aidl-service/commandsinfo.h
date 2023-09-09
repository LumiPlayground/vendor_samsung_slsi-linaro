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
#pragma once

#include "radiomodemresponse.h"
#include "radiodataresponse.h"
#include "radioconfigresponse.h"
#include "radiosimresponse.h"
#include "radionetworkresponse.h"
#include "radiomessagingresponse.h"
#include "radiovoiceresponse.h"

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioModemResponse>& response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioModemIndication>& indication);

void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioDataResponse>& response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioDataIndication>& indication);

/* Radio Config */
void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioConfigResponse>& response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioConfigIndication>& indication);

/* sim */
void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioSimResponse>& response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioSimIndication>& indication);

/* Network */
void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioNetworkResponse>& response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioNetworkIndication>& indication);

/* Messaging */
void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioMessagingResponse> &response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioMessagingIndication> &indication);

/* Voice */
void updateCommandsInfo(uint32_t slot, const std::shared_ptr<RadioVoiceResponse> &response);
void updateUnsolResponseInfo(uint32_t slot, const std::shared_ptr<RadioVoiceIndication> &indication);
