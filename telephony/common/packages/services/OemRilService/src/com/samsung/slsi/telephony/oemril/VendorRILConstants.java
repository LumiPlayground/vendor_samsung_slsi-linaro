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

public class VendorRILConstants {
    // @1.0
    static final int RIL_EXTENSION_REQUEST_BASE = 5000;
    static final int RIL_REQUEST_QUERY_COLP = (RIL_EXTENSION_REQUEST_BASE + 2);
    static final int RIL_REQUEST_QUERY_COLR = (RIL_EXTENSION_REQUEST_BASE + 3);
    static final int RIL_REQUEST_SIM_GET_ATR = (RIL_EXTENSION_REQUEST_BASE + 5);
    static final int RIL_REQUEST_SEND_ENCODED_USSD = (RIL_EXTENSION_REQUEST_BASE + 6);
    static final int RIL_REQUEST_SET_UPLMN = (RIL_EXTENSION_REQUEST_BASE + 7);
    static final int RIL_REQUEST_GET_UPLMN = (RIL_EXTENSION_REQUEST_BASE + 8);
    static final int RIL_REQUEST_SET_DS_NETWORK_TYPE = (RIL_EXTENSION_REQUEST_BASE + 17);
    static final int RIL_REQUEST_VSIM_NOTIFICATION = (RIL_EXTENSION_REQUEST_BASE + 22);
    static final int RIL_REQUEST_VSIM_OPERATION = (RIL_EXTENSION_REQUEST_BASE + 23);
    static final int RIL_REQUEST_SET_EMERGENCY_CALL_STATUS = (RIL_EXTENSION_REQUEST_BASE + 25);
    static final int RIL_REQUEST_SET_FEMTO_CELL_SRCH = (RIL_EXTENSION_REQUEST_BASE + 26);
    static final int RIL_REQUEST_SET_CDMA_HYBRID_MODE = (RIL_EXTENSION_REQUEST_BASE + 27);
    static final int RIL_REQUEST_GET_CDMA_HYBRID_MODE = (RIL_EXTENSION_REQUEST_BASE + 28);
    static final int RIL_REQUEST_SET_VOICE_OPERATION = (RIL_EXTENSION_REQUEST_BASE + 30);
    static final int RIL_REQUEST_SET_DUAL_NETWORK_AND_ALLOW_DATA = (RIL_EXTENSION_REQUEST_BASE + 31);
    static final int RIL_REQUEST_QUERY_BPLMN_SEARCH = (RIL_EXTENSION_REQUEST_BASE + 33);
    static final int RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL_WITH_RAT = (RIL_EXTENSION_REQUEST_BASE + 34);
    static final int RIL_REQUEST_DIAL_WITH_CALL_TYPE = (RIL_EXTENSION_REQUEST_BASE + 35);
    static final int RIL_REQUEST_CHANGE_BARRING_PASSWORD_OVER_MMI = (RIL_EXTENSION_REQUEST_BASE + 36);
    static final int RIL_REQUEST_DEACTIVATE_DATA_CALL_WITH_REASON = (RIL_EXTENSION_REQUEST_BASE + 38);
    static final int RIL_REQUEST_EMULATE_IND = (RIL_EXTENSION_REQUEST_BASE + 39);
    static final int RIL_REQUEST_GET_SIM_LOCK_STATUS = (RIL_EXTENSION_REQUEST_BASE + 40);
    static final int RIL_REQUEST_SET_INTPS_SERVICE = (RIL_EXTENSION_REQUEST_BASE + 41);
    static final int RIL_REQUEST_GET_SMS_STORAGE_ON_SIM = (RIL_EXTENSION_REQUEST_BASE + 42);

    static final int RIL_UNSOL_EXTENSION_BASE = 6000;
    static final int RIL_UNSOL_SUPP_SVC_RETURN_RESULT = (RIL_UNSOL_EXTENSION_BASE + 1);
    static final int RIL_UNSOL_PB_READY = (RIL_UNSOL_EXTENSION_BASE + 2);
    static final int RIL_UNSOL_CALL_PRESENT_IND = (RIL_UNSOL_EXTENSION_BASE + 3);
    static final int RIL_UNSOL_WB_AMR_REPORT_IND = (RIL_UNSOL_EXTENSION_BASE + 4);
    static final int RIL_UNSOL_VSIM_OPERATION_INDICATION = (RIL_UNSOL_EXTENSION_BASE + 5);
    static final int RIL_UNSOL_NAS_TIMER_STATUS_IND = (RIL_UNSOL_EXTENSION_BASE + 6);
    static final int RIL_UNSOL_EMERGENCY_ACT_INFO = (RIL_UNSOL_EXTENSION_BASE + 7);
    static final int RIL_UNSOL_ICCID_INFO = (RIL_UNSOL_EXTENSION_BASE + 8);
    static final int RIL_UNSOL_ON_USSD_WITH_DCS = (RIL_UNSOL_EXTENSION_BASE + 9);
    static final int RIL_UNSOL_VOLTE_AVAILABLE_INFO = (RIL_UNSOL_EXTENSION_BASE + 10);
    static final int RIL_UNSOL_EMERGENCY_SUPPORT_RAT_MODE = (RIL_UNSOL_EXTENSION_BASE + 11);
    static final int RIL_UNSOL_USSD_CANCELED = (RIL_UNSOL_EXTENSION_BASE+12);

    // @1.1
    static final int RIL_REQUEST_SET_ACTIVATE_VSIM = (RIL_EXTENSION_REQUEST_BASE + 42);
    static final int RIL_REQUEST_SET_ENDC_MODE = (RIL_EXTENSION_REQUEST_BASE + 43);
    static final int RIL_REQUEST_GET_ENDC_MODE = (RIL_EXTENSION_REQUEST_BASE + 44);
    static final int RIL_REQUEST_SET_NR_MODE = (RIL_EXTENSION_REQUEST_BASE + 46);
    static final int RIL_REQUEST_GET_NR_MODE = (RIL_EXTENSION_REQUEST_BASE + 47);

    static final int RIL_UNSOL_NR_PHYSICAL_CHANNEL_CONFIGS = (RIL_UNSOL_EXTENSION_BASE+13);
    static final int RIL_UNSOL_ENDC_CAPABILITY = (RIL_UNSOL_EXTENSION_BASE+14);
    static final int RIL_UNSOL_SIM_TRAY_STATUS_CHANGED = (RIL_UNSOL_EXTENSION_BASE+15);

    // @1.2
    static final int RIL_UNSOL_IND_POST_URSP = (RIL_UNSOL_EXTENSION_BASE + 16);
}
