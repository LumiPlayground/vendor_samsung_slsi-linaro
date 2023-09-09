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
 * simcardstatus.h
 *
 *  Created on: 2021. 10. 21.
 */

#ifndef __SIM_CARD_STATUS_H__
#define __SIM_CARD_STATUS_H__

#include <slsi/radio_v1_4.h>
#include <string>
#include <vector>

struct AppStatus {
};

class SimCardStatus {
protected:
    int cardState;
    uint32_t physicalSlotId;
    std::string atr;
    std::string iccid;
    std::string eid;
    // TBD
    std::vector<AppStatus> applications;

public:
    SimCardStatus();
    SimCardStatus(const RIL_CardStatus_V1_4& cardStatus);
    SimCardStatus(const SimCardStatus& o);
    SimCardStatus(SimCardStatus&& o) noexcept;
    void setNoSim();
    SimCardStatus& operator=(const SimCardStatus& o);
    SimCardStatus& operator=(SimCardStatus&& o) noexcept;
    bool operator==(const SimCardStatus& o) const;
    bool operator!=(const SimCardStatus& o) const { return !(*this == o); }
    std::string toString() const;
    int getCardState() const { return cardState; }
    int getPhysicalSlotId() const { return physicalSlotId; }
    const std::string& getAtr() const { return atr; }
    const std::string& getIccid() const { return iccid; }
    const std::string& getEid() const { return eid; }

    static SimCardStatus create(const RIL_CardStatus_V1_4& cardStatus);
};

#endif /* __SIM_CARD_STATUS_H__ */
