/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
package com.samsung.slsi.telephony.oemril;

import android.os.Message;

public interface OemCommandsHelperInterface {
    public void setVoiceOperation(boolean enable, Message result);
    public void getVoiceOperation(Message result);
    public void setMobileDataState(boolean mobileData, boolean roamingData, Message result);
}
