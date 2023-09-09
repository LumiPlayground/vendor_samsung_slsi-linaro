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
 * vsimprotocolutils.h
 *
 *  Created on: 2021. 08. 20.
 *      Author: aryaman.2019
 */

#ifndef __VSIM_PROTOCOL_UTILS_H__
#define __VSIM_PROTOCOL_UTILS_H__

#include "commondef.h"
#include "rildef.h"

namespace VsimProtocolUtils {
// ProtcolVsimBuilder(Legacy/Json)
int ConvertVsimNotiRilEventIdToSitEventId(int eventid);
int ConvertVsimOpRilEventIdToSitEventId(int eventid);

// ProtocolVsimAdapter(Legacy/Json)
int ConvertVsimOpSitEventIdToRilEventId(int eventid);
}

#endif // __VSIM_PROTOCOL_UTILS_H__
