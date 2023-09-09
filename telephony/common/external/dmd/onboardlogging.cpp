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
 * onboardlogging.cpp
 *
 *  Created on: 2021. 10. 13.
 */
#include <stdio.h>
#include "onboardlogging.h"

OnBoardLogging::OnBoardLogging(OnBoardResponseCallback *callback) : mModemProxy(0),
        mCallback(callback) {
}

void OnBoardLogging::onRawRequest(const void *data, size_t datalen) {
    if (mModemProxy) {
        mModemProxy->WriteSafely(data, datalen);
    }
}
