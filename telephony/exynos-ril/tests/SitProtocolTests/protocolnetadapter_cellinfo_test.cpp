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
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "legacy/network/protocolnetadapterlegacy.h"
#include <librilutils/textutils.h>
#include <telephony/ril_mnc.h>

#define CELL_INFO_COMMON_SIZE 3
#define GSM_CELL_INFO_SIZE (CELL_INFO_COMMON_SIZE + sizeof(cell_info_type_gsm_v4))
#define WCDMA_CELL_INFO_SIZE (CELL_INFO_COMMON_SIZE + sizeof(cell_info_type_wcdma_v4))
#define LTE_CELL_INFO_SIZE (CELL_INFO_COMMON_SIZE + sizeof(cell_info_type_lte_v4))
#define NR_CELL_INFO_SIZE (CELL_INFO_COMMON_SIZE + sizeof(cell_info_type_nr_v4))

typedef struct {
    int cell_info_num;
    cell_information_v4 ci[4];
} cell_info_resp;

cell_information_v4 cellInfoGsm = {
    .cell_info_type = 0,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.gsm = {
        .cell_identity = {
            .plmn = {'4', '5', '0', '0', '5', '#'},
            .lac = 1234,
            .cid = 5678,
            .arfcn = 1234,
            .bsic = 11,
            .plmn_info = {
                .plmn_short_name = "SKT",
                .plmn_long_name = "SKT Telecom",
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                }
            }
        },
        .sig_str = 29,
        .sig_ber = 4,
        .sig_ta = 11
    },
};

cell_information_v4 cellInfoLte = {
    .cell_info_type = 2,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.lte = {
        .cell_identity = {
            .plmn = {'4', '5', '0', '0', '5', '#'},
            .cell_id = 1234,
            .phy_cell_id = 501,
            .tac = 119,
            .earfcn = 201,
            .bandwidth = 10000,
            .plmn_info = {
                .plmn_short_name = {
                    0x53, 0x4b, 0x54, 0x65, 0x6c, 0x65, 0x63, 0x6f, 0x6d, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .plmn_long_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            .csg_info = {
                .csg_indication = 0,
                .hnb_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0},
                .csg_identity = INT_MAX
            },
            .band_len = 1,
            .band_info = {0, 0, 0, 0, 0, 0, 0, 0}
        },
        .endc_available = 1,
        .sig_str = 11,
        .sig_rsrp = 111,
        .sig_rsrq = 11,
        .sig_rssnr = 10,
        .cqi_table_index = 5,
        .cqi = 4,
        .ta = 3
    }
};

cell_information_v4 cellInfoWcdma = {
    .cell_info_type = 3,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.wcdma = {
        .cell_identity = {
            .plmn = {'4', '5', '0', '0', '5', '#'},
            .lac = 20000,
            .cid = 9999,
            .psc = 111,
            .uarfcn = 11,
            .plmn_info = {
                .plmn_short_name = "SKT",
                .plmn_long_name = "SKT Telecom",
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            .csg_info = {
                .csg_indication = 1,
                .hnb_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0},
                .csg_identity = 9
            }
        },
        .sig_str = 29,
        .sig_ber = 4,
        .rscp = 89,
        .ecno = 33
    },
};

cell_information_v4 cellInfoNr = {
    .cell_info_type = 5,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.nr = {
        .cell_identity = {
            .plmn = {'4', '5', '0', '0', '5', '#'},
            .cell_id = 12345678,
            .phy_cell_id = 999,
            .tac = 22,
            .arfcn = 1234,
            .plmn_info = {
                .plmn_short_name = "SKT",
                .plmn_long_name = "SKT Telecom",
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            .csg_info = {
                .csg_indication = 1,
                .hnb_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0},
                .csg_identity = 9
            },
            .band_len = 1,
            .band_info = {0, 0, 0, 0, 0, 0, 0, 0}
        },
        .ss_rsrp = 99,
        .ss_rsrq = 11,
        .ss_sinr = 22,
        .csi_rsrp = 89,
        .csi_rsrq = 13,
        .csi_sinr = 33,
        .cqi_table_index = 1,
        .wb_cqi = 1,
        .sb_cqi = {0,}
    },
};

cell_information_v4 cellInfoLte_invalid = {
    .cell_info_type = 2,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.lte = {
        .cell_identity = {
            .plmn = {'0', '0', '0', '0', '0', '#'},
            .cell_id = 1234,
            .phy_cell_id = 501,
            .tac = 119,
            .earfcn = 201,
            .bandwidth = 10000,
            .plmn_info = {
                .plmn_short_name = {
                    0x53, 0x4b, 0x54, 0x65, 0x6c, 0x65, 0x63, 0x6f, 0x6d, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .plmn_long_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            .csg_info = {
                .csg_indication = 0,
                .hnb_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0},
                .csg_identity = INT_MAX
            },
            .band_len = 1,
            .band_info = {0, 0, 0, 0, 0, 0, 0, 0}
        },
        .endc_available = 1,
        .sig_str = 11,
        .sig_rsrp = 111,
        .sig_rsrq = 11,
        .sig_rssnr = 10,
        .cqi_table_index = 5,
        .cqi = 4,
        .ta = 3
    }
};

cell_information_v4 cellInfoWcdma_invalid = {
    .cell_info_type = 3,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.wcdma = {
        .cell_identity = {
            .plmn = {'0', '0', '0', '0', '5', '#'},
            .lac = 20000,
            .cid = 9999,
            .psc = 111,
            .uarfcn = 11,
            .plmn_info = {
                .plmn_short_name = "SKT",
                .plmn_long_name = "SKT Telecom",
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            .csg_info = {
                .csg_indication = 1,
                .hnb_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0},
                .csg_identity = 9
            }
        },
        .sig_str = 29,
        .sig_ber = 4,
        .rscp = 89,
        .ecno = 33
    },
};

cell_information_v4 cellInfoNr_empty_plmn = {
    .cell_info_type = 5,
    .reg_status = 1,
    .cell_connection_status = 1,
    .cell_info.nr = {
        .cell_identity = {
            .plmn = {},
            .cell_id = 12345678,
            .phy_cell_id = 999,
            .tac = 22,
            .arfcn = 1234,
            .plmn_info = {
                .plmn_short_name = "SKT",
                .plmn_long_name = "SKT Telecom",
                .additional_plmn_len = 0,
                .additional_plmn_info = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                }
            },
            .csg_info = {
                .csg_indication = 1,
                .hnb_name = {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0},
                .csg_identity = 9
            },
            .band_len = 1,
            .band_info = {0, 0, 0, 0, 0, 0, 0, 0}
        },
        .ss_rsrp = 99,
        .ss_rsrq = 11,
        .ss_sinr = 22,
        .csi_rsrp = 89,
        .csi_rsrq = 13,
        .csi_sinr = 33,
        .cqi_table_index = 1,
        .wb_cqi = 1,
        .sb_cqi = {0,}
    },
};

int FetchMcc(const char *plmn) {
    char mccStr[4] = {0, };
    if (plmn != NULL) {
        memcpy(mccStr, plmn, 3);
        if (TextUtils::IsDigitsOnly(mccStr)) {
            int mcc = strtol(mccStr, NULL, 10);
#if 0
            int mccLength = 3;
            if (mcc > 0) {
                mcc = ril::util::mnc::encode(mcc, mccLength);
                return mcc;
            }
#else
            return mcc;
#endif
        }
    }
    return INT_MAX;
}

int FetchMnc(const char *plmn) {
    int mcc = FetchMcc(plmn);
    if (mcc == 0 || mcc == INT_MAX) {
        return INT_MAX;
    }

    char mncStr[4] = {0, };
    if (plmn != NULL) {
        memcpy(mncStr, plmn + 3, 3);
        if (mncStr[2] == '#') {
            mncStr[2] = 0;
        }

        if (TextUtils::IsDigitsOnly(mncStr)) {
            int mnc = strtol(mncStr, NULL, 10);
            int mccLength = strlen(mncStr);
            mnc = ril::util::mnc::encode(mnc, mccLength);
            return mnc;
        }
    }
    return INT_MAX;
}

void verifyCellInfoGsm(RIL_CellInfo_V1_6& cur, cell_information_v4 cellInfoGsm) {
    EXPECT_EQ(cur.cellInfoType, cellInfoGsm.cell_info_type + 1);
    EXPECT_EQ(cur.registered, cellInfoGsm.reg_status);
    EXPECT_EQ(cur.connectionStatus, cellInfoGsm.cell_connection_status);
    EXPECT_EQ(cur.CellInfo.gsm.cellIdentityGsm.mcc, FetchMcc(cellInfoGsm.cell_info.gsm.cell_identity.plmn));
    EXPECT_EQ(cur.CellInfo.gsm.cellIdentityGsm.mnc, FetchMnc(cellInfoGsm.cell_info.gsm.cell_identity.plmn));
    EXPECT_EQ(cur.CellInfo.gsm.cellIdentityGsm.lac, cellInfoGsm.cell_info.gsm.cell_identity.lac);
    EXPECT_EQ(cur.CellInfo.gsm.cellIdentityGsm.cid, cellInfoGsm.cell_info.gsm.cell_identity.cid);
    EXPECT_EQ(cur.CellInfo.gsm.cellIdentityGsm.arfcn, cellInfoGsm.cell_info.gsm.cell_identity.arfcn);
    EXPECT_EQ(cur.CellInfo.gsm.cellIdentityGsm.bsic, cellInfoGsm.cell_info.gsm.cell_identity.bsic);
    EXPECT_TRUE(strncmp(cur.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaLong,
        cellInfoGsm.cell_info.gsm.cell_identity.plmn_info.plmn_long_name, sizeof(cellInfoGsm.cell_info.gsm.cell_identity.plmn_info.plmn_long_name)) == 0);
    EXPECT_TRUE(strncmp(cur.CellInfo.gsm.cellIdentityGsm.operatorNames.alphaShort,
        cellInfoGsm.cell_info.gsm.cell_identity.plmn_info.plmn_short_name, sizeof(cellInfoGsm.cell_info.gsm.cell_identity.plmn_info.plmn_short_name)) == 0);

    // SS
    EXPECT_EQ(cur.CellInfo.gsm.signalStrengthGsm.signalStrength, cellInfoGsm.cell_info.gsm.sig_str);
    EXPECT_EQ(cur.CellInfo.gsm.signalStrengthGsm.bitErrorRate, cellInfoGsm.cell_info.gsm.sig_ber);
    EXPECT_EQ(cur.CellInfo.gsm.signalStrengthGsm.timingAdvance, cellInfoGsm.cell_info.gsm.sig_ta);
}

void verifyCellInfoLte(RIL_CellInfo_V1_6& cur, cell_information_v4 cellInfoLte) {
    EXPECT_EQ(cur.cellInfoType, cellInfoLte.cell_info_type + 1);
    EXPECT_EQ(cur.registered, cellInfoLte.reg_status);
    EXPECT_EQ(cur.connectionStatus, cellInfoLte.cell_connection_status);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.mcc, FetchMcc(cellInfoLte.cell_info.lte.cell_identity.plmn));
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.mnc, FetchMnc(cellInfoLte.cell_info.lte.cell_identity.plmn));
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.ci, cellInfoLte.cell_info.lte.cell_identity.cell_id);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.pci, cellInfoLte.cell_info.lte.cell_identity.phy_cell_id);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.tac, cellInfoLte.cell_info.lte.cell_identity.tac);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.earfcn, cellInfoLte.cell_info.lte.cell_identity.earfcn);
    EXPECT_TRUE(strncmp(cur.CellInfo.lte.cellIdentityLte.operatorNames.alphaLong,
        cellInfoLte.cell_info.lte.cell_identity.plmn_info.plmn_long_name, sizeof(cellInfoLte.cell_info.lte.cell_identity.plmn_info.plmn_long_name)) == 0);
    EXPECT_TRUE(strncmp(cur.CellInfo.lte.cellIdentityLte.operatorNames.alphaShort,
        cellInfoLte.cell_info.lte.cell_identity.plmn_info.plmn_short_name, sizeof(cellInfoLte.cell_info.lte.cell_identity.plmn_info.plmn_short_name)) == 0);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.bandwidth, cellInfoLte.cell_info.lte.cell_identity.bandwidth);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.csgIndication, cellInfoLte.cell_info.lte.cell_identity.csg_info.csg_indication);
    EXPECT_TRUE(strncmp(cur.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.homeNodebName,
        cellInfoLte.cell_info.lte.cell_identity.csg_info.hnb_name, sizeof(cellInfoLte.cell_info.lte.cell_identity.csg_info.hnb_name)) == 0);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.optionalCsgInfo.csgInfo.csgIdentity, cellInfoLte.cell_info.lte.cell_identity.csg_info.csg_identity);
    EXPECT_EQ(cur.CellInfo.lte.cellIdentityLte.leng_bands, cellInfoLte.cell_info.lte.cell_identity.band_len);
    EXPECT_TRUE(memcmp(cur.CellInfo.lte.cellIdentityLte.bands, cellInfoLte.cell_info.lte.cell_identity.band_info, MAX_BANDS)==0);

    EXPECT_EQ(cur.CellInfo.lte.isEndcAvailable, cellInfoLte.cell_info.lte.endc_available);

    // SS
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.signalStrength, cellInfoLte.cell_info.lte.sig_str);
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.rsrp, cellInfoLte.cell_info.lte.sig_rsrp);
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.rsrq, cellInfoLte.cell_info.lte.sig_rsrq);
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.rssnr, cellInfoLte.cell_info.lte.sig_rssnr);
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.cqi, cellInfoLte.cell_info.lte.cqi);
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.timingAdvance, cellInfoLte.cell_info.lte.ta);
    EXPECT_EQ(cur.CellInfo.lte.signalStrengthLte.cqiTableIndex, (unsigned int)/* NEED TO CHECK */ cellInfoLte.cell_info.lte.cqi_table_index);
}

void verifyCellInfoWcdma(RIL_CellInfo_V1_6& cur, cell_information_v4 cellInfoWcdma) {
    EXPECT_EQ(cur.cellInfoType, cellInfoWcdma.cell_info_type + 1);
    EXPECT_EQ(cur.registered, cellInfoWcdma.reg_status);
    EXPECT_EQ(cur.connectionStatus, cellInfoWcdma.cell_connection_status);
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.mcc, FetchMcc(cellInfoWcdma.cell_info.wcdma.cell_identity.plmn));
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.mnc, FetchMnc(cellInfoWcdma.cell_info.wcdma.cell_identity.plmn));
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.lac, cellInfoWcdma.cell_info.wcdma.cell_identity.lac);
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.cid, cellInfoWcdma.cell_info.wcdma.cell_identity.cid);
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.psc, cellInfoWcdma.cell_info.wcdma.cell_identity.psc);
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.uarfcn, cellInfoWcdma.cell_info.wcdma.cell_identity.uarfcn);
    EXPECT_TRUE(strncmp(cur.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaLong,
        cellInfoWcdma.cell_info.wcdma.cell_identity.plmn_info.plmn_long_name, sizeof(cellInfoWcdma.cell_info.wcdma.cell_identity.plmn_info.plmn_long_name)) == 0);
    EXPECT_TRUE(strncmp(cur.CellInfo.wcdma.cellIdentityWcdma.operatorNames.alphaShort,
        cellInfoWcdma.cell_info.wcdma.cell_identity.plmn_info.plmn_short_name, sizeof(cellInfoWcdma.cell_info.wcdma.cell_identity.plmn_info.plmn_short_name)) == 0);
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIndication, cellInfoWcdma.cell_info.wcdma.cell_identity.csg_info.csg_indication);
    EXPECT_TRUE(strncmp(cur.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.homeNodebName,
        cellInfoWcdma.cell_info.wcdma.cell_identity.csg_info.hnb_name, sizeof(cellInfoWcdma.cell_info.wcdma.cell_identity.csg_info.hnb_name)) == 0);
    EXPECT_EQ(cur.CellInfo.wcdma.cellIdentityWcdma.optionalCsgInfo.csgInfo.csgIdentity, cellInfoWcdma.cell_info.wcdma.cell_identity.csg_info.csg_identity);

    // SS
    EXPECT_EQ(cur.CellInfo.wcdma.signalStrengthWcdma.signalStrength, cellInfoWcdma.cell_info.wcdma.sig_str);
    EXPECT_EQ(cur.CellInfo.wcdma.signalStrengthWcdma.bitErrorRate, cellInfoWcdma.cell_info.wcdma.sig_ber);
    EXPECT_EQ(cur.CellInfo.wcdma.signalStrengthWcdma.rscp, (unsigned int)/* NEED TO CHECK */ cellInfoWcdma.cell_info.wcdma.rscp);
    EXPECT_EQ(cur.CellInfo.wcdma.signalStrengthWcdma.ecno, (unsigned int)/* NEED TO CHECK */ cellInfoWcdma.cell_info.wcdma.ecno);
}

void verifyCellInfoNr(RIL_CellInfo_V1_6& cur, cell_information_v4 cellInfoNr) {
    EXPECT_EQ(cur.cellInfoType, cellInfoNr.cell_info_type + 1);
    EXPECT_EQ(cur.registered, cellInfoNr.reg_status);
    EXPECT_EQ(cur.connectionStatus, cellInfoNr.cell_connection_status);
    EXPECT_TRUE((cur.CellInfo.nr.cellIdentityNr.mcc == FetchMcc(cellInfoNr.cell_info.nr.cell_identity.plmn))
            || (cur.CellInfo.nr.cellIdentityNr.mcc == INT_MAX));
    EXPECT_TRUE((cur.CellInfo.nr.cellIdentityNr.mnc == FetchMnc(cellInfoNr.cell_info.nr.cell_identity.plmn))
            || (cur.CellInfo.nr.cellIdentityNr.mnc == INT_MAX));
    // Do not use EXPECT_EQ for 64-bit type to support a compatibility on 32 bit machine.
    EXPECT_TRUE(cur.CellInfo.nr.cellIdentityNr.nci == cellInfoNr.cell_info.nr.cell_identity.cell_id);
    EXPECT_EQ(cur.CellInfo.nr.cellIdentityNr.pci, cellInfoNr.cell_info.nr.cell_identity.phy_cell_id);
    EXPECT_EQ(cur.CellInfo.nr.cellIdentityNr.tac, cellInfoNr.cell_info.nr.cell_identity.tac);
    EXPECT_EQ(cur.CellInfo.nr.cellIdentityNr.nrarfcn, cellInfoNr.cell_info.nr.cell_identity.arfcn);
    EXPECT_TRUE(strncmp(cur.CellInfo.nr.cellIdentityNr.operatorNames.alphaLong,
        cellInfoNr.cell_info.nr.cell_identity.plmn_info.plmn_long_name, sizeof(cellInfoNr.cell_info.nr.cell_identity.plmn_info.plmn_long_name)) == 0);
    EXPECT_TRUE(strncmp(cur.CellInfo.nr.cellIdentityNr.operatorNames.alphaShort,
        cellInfoNr.cell_info.nr.cell_identity.plmn_info.plmn_short_name, sizeof(cellInfoNr.cell_info.nr.cell_identity.plmn_info.plmn_short_name)) == 0);
    EXPECT_EQ(cur.CellInfo.nr.cellIdentityNr.leng_bands, cellInfoNr.cell_info.nr.cell_identity.band_len);
    EXPECT_TRUE(memcmp(cur.CellInfo.nr.cellIdentityNr.bands, cellInfoNr.cell_info.nr.cell_identity.band_info, MAX_BANDS)==0);

    // SS
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.ssRsrp, cellInfoNr.cell_info.nr.ss_rsrp);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.ssRsrq, cellInfoNr.cell_info.nr.ss_rsrq);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.ssSinr, cellInfoNr.cell_info.nr.ss_sinr);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.csiRsrp, cellInfoNr.cell_info.nr.csi_rsrp);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.csiRsrq, cellInfoNr.cell_info.nr.csi_rsrq);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.csiSinr, cellInfoNr.cell_info.nr.csi_sinr);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.csiCqiTableIndex, (unsigned int)/* NEED TO CHECK */ cellInfoNr.cell_info.nr.cqi_table_index);
    EXPECT_EQ(cur.CellInfo.nr.signalStrengthNr.csiCqiReportLen, cellInfoNr.cell_info.nr.wb_cqi);
    //EXPECT_TRUE(memcmp(cur.CellInfo.nr.signalStrengthNr.csiCqiReport, cellInfoNr.cell_info.nr.sb_cqi, MAX_NR_BAND_INDEX)==0);
}


void verifyCellInfoAll(list<RIL_CellInfo_V1_6>& cellInfoList) {
    int cellInfoSize = cellInfoList.size();
    EXPECT_TRUE(cellInfoSize > 0);
    if (cellInfoSize > 0) {
        list<RIL_CellInfo_V1_6>::iterator iter;
        for (iter = cellInfoList.begin(); iter != cellInfoList.end(); iter++) {
            RIL_CellInfo_V1_6 &cur = *iter;

            switch((int)cur.cellInfoType) {
                case RIL_CELL_INFO_TYPE_GSM: {
                    verifyCellInfoGsm(cur, cellInfoGsm);
                    break;
                }
                case RIL_CELL_INFO_TYPE_LTE: {
                    verifyCellInfoLte(cur, cellInfoLte);
                    break;
                }
                case RIL_CELL_INFO_TYPE_WCDMA: {
                    verifyCellInfoWcdma(cur, cellInfoWcdma);
                    break;
                }
                case RIL_CELL_INFO_TYPE_NR: {
                    verifyCellInfoNr(cur, cellInfoNr);
                    break;
                }
                default:
                    break;
            } // end switch

        } // end iter ~
    }
}

TEST(ProtocolNetAdapter, CellInfoListAdapter_v4_single) {
    cell_information_v4 *testVector[] = {
        &cellInfoGsm, &cellInfoLte, &cellInfoWcdma, &cellInfoNr
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        cell_info_resp data;
        data.cell_info_num = 1;
        memcpy(&data.ci[0], testVector[i], sizeof(cell_information_v4));

        ModemData *modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_CELL_INFO_LIST, 0, RIL_E_SUCCESS, &data, sizeof(data));
        ASSERT_NE(modemData, nullptr);

        ProtocolNetCellInfoListAdapterLegacy adapter(modemData);
        list<RIL_CellInfo_V1_6> &cellInfoList = adapter.GetCellInfoList(4);
        EXPECT_EQ(cellInfoList.size(), 1U);

        verifyCellInfoAll(cellInfoList);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }
}

TEST(ProtocolNetAdapter, CellInfoListAdapter_v4_list) {
    const int cellInfoSize = GSM_CELL_INFO_SIZE + WCDMA_CELL_INFO_SIZE + LTE_CELL_INFO_SIZE + NR_CELL_INFO_SIZE;
    struct {
        int numOfCellInfo;
        char data[cellInfoSize];
    } testVector;

    testVector.numOfCellInfo = 1;
    int pos = 0;
    memcpy(testVector.data, &cellInfoGsm, GSM_CELL_INFO_SIZE);
    pos += GSM_CELL_INFO_SIZE;
    memcpy(testVector.data + pos, &cellInfoWcdma, WCDMA_CELL_INFO_SIZE);
    pos += WCDMA_CELL_INFO_SIZE;
    memcpy(testVector.data + pos, &cellInfoLte, LTE_CELL_INFO_SIZE);
    pos += LTE_CELL_INFO_SIZE;
    memcpy(testVector.data + pos, &cellInfoNr, NR_CELL_INFO_SIZE);
    pos += NR_CELL_INFO_SIZE;

    ModemData *modemData = ModemDataBuilder::BuildResponse(
        SIT_GET_CELL_INFO_LIST, 0, RIL_E_SUCCESS, &testVector, sizeof(int) + pos);
    ASSERT_NE(modemData, nullptr);

    ProtocolNetCellInfoListAdapterLegacy adapter(modemData);
    list<RIL_CellInfo_V1_6> &cellInfoList = adapter.GetCellInfoList(4);
    EXPECT_EQ(cellInfoList.size(), (unsigned long)/* NEED TO CHECK */ testVector.numOfCellInfo);

    verifyCellInfoAll(cellInfoList);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolNetAdapter, CellInfoListAdapter_v4_list_invalid) {
    const int cellInfoSize = GSM_CELL_INFO_SIZE + WCDMA_CELL_INFO_SIZE + LTE_CELL_INFO_SIZE + NR_CELL_INFO_SIZE;
    struct {
        int numOfCellInfo;
        char data[cellInfoSize];
    } testVector;

    testVector.numOfCellInfo = 4;
    int pos = 0;
    memcpy(testVector.data, &cellInfoGsm, GSM_CELL_INFO_SIZE);
    pos += GSM_CELL_INFO_SIZE;
    memcpy(testVector.data + pos, &cellInfoWcdma_invalid, WCDMA_CELL_INFO_SIZE);
    pos += WCDMA_CELL_INFO_SIZE;
    memcpy(testVector.data + pos, &cellInfoLte_invalid, LTE_CELL_INFO_SIZE);
    pos += LTE_CELL_INFO_SIZE;
    memcpy(testVector.data + pos, &cellInfoNr_empty_plmn, NR_CELL_INFO_SIZE);
    pos += NR_CELL_INFO_SIZE;

    ModemData *modemData = ModemDataBuilder::BuildResponse(
            SIT_GET_CELL_INFO_LIST, 0, RIL_E_SUCCESS, &testVector, sizeof(int) + pos);
    ASSERT_NE(modemData, nullptr);

    ProtocolNetCellInfoListAdapterLegacy adapter(modemData);
    list<RIL_CellInfo_V1_6> &cellInfoList = adapter.GetCellInfoList(4);
    EXPECT_EQ(cellInfoList.size(), (unsigned long)/* NEED TO CHECK */ testVector.numOfCellInfo-2);

    verifyCellInfoAll(cellInfoList);

    if (modemData) {
        delete modemData;
        modemData = nullptr;
    }
}
