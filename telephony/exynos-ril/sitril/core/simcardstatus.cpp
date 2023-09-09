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
/*
 * simcardstatus.cpp
 *
 *  Created on: 2021. 10. 21.
 */
#include <telephony/ril.h>
#include "simcardstatus.h"
#include <sstream>

/**
 * SimCardStatus
 */
SimCardStatus::SimCardStatus() : physicalSlotId(-1) {
    setNoSim();
}

SimCardStatus::SimCardStatus(const RIL_CardStatus_V1_4& cardStatus) : cardState((int)cardStatus.card_state),
        physicalSlotId(cardStatus.physicalSlotId) {
    atr = cardStatus.atr ? cardStatus.atr : "";
    iccid = cardStatus.iccid ? cardStatus.iccid : "";
    eid = cardStatus.eid ? cardStatus.eid : "";
}

SimCardStatus::SimCardStatus(const SimCardStatus& o) : cardState(o.cardState),
        physicalSlotId(o.physicalSlotId), atr(o.atr), iccid(o.iccid), eid(o.eid),
        applications(o.applications) {
}

SimCardStatus::SimCardStatus(SimCardStatus&& o) noexcept : cardState(o.cardState),
        physicalSlotId(o.physicalSlotId),
        atr(std::move(o.atr)), iccid(std::move(o.iccid)), eid(std::move(o.eid)),
        applications(std::move(o.applications)) {
}

void SimCardStatus::setNoSim() {
    cardState = RIL_CARDSTATE_ABSENT;
    atr = "";
    iccid = "";
    eid = "";
    applications.clear();
}

SimCardStatus& SimCardStatus::operator=(const SimCardStatus& o) {
    if (this != &o) {
        cardState = o.cardState;
        physicalSlotId = o.physicalSlotId;
        atr = o.atr;
        iccid = o.iccid;
        eid = o.eid;
        applications = o.applications;
    }
    return *this;
}

SimCardStatus& SimCardStatus::operator=(SimCardStatus&& o) noexcept {
    if (this != &o) {
        cardState = o.cardState;
        physicalSlotId = o.physicalSlotId;
        atr = std::move(o.atr);
        iccid = std::move(o.iccid);
        eid = std::move(o.eid);
        applications = std::move(o.applications);
    }
    return *this;
}

bool SimCardStatus::operator==(const SimCardStatus& o) const {
    return (cardState == o.cardState) &&
           (physicalSlotId == o.physicalSlotId) &&
           (atr == o.atr) &&
           (iccid == o.iccid) &&
           (eid == o.eid);
}

std::string SimCardStatus::toString() const {
    std::stringstream ss;
    ss << "SimCardStatus:{cardState=" << cardState << ",physicalSlotId=" << physicalSlotId;
    ss << ",atr=" << atr << ",iccid=" << iccid << ",eid=" << eid;
    ss << "}";
    return ss.str();
}

SimCardStatus SimCardStatus::create(const RIL_CardStatus_V1_4& cardStatus) {
    return { cardStatus };
}
