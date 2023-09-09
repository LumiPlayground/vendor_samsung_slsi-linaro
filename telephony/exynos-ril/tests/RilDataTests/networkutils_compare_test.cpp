/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <testutils.h>
#include <slsi/radio_v1_6.h>
#include "networkutils.h"
#include <vector>

TEST(NetworkUtils_compare, RIL_CellIdentityOperatorNames) {
    RIL_CellIdentityOperatorNames lhs = {
        "test long", "test short"
    };
    decltype(lhs) rhs = {
        "test long", "test short"
    };
    EXPECT_TRUE(lhs == rhs);
    rhs = {"test long", "test long"};
    EXPECT_FALSE(lhs == rhs);
    rhs = {"test short", "test short"};
    EXPECT_FALSE(lhs == rhs);
}

TEST(NetworkUtils_compare, RIL_OptionalCsgInfo) {
    RIL_OptionalCsgInfo lhs = {
        { true, "homeNodebName", 0x12345 }
    };
    decltype(lhs) rhs = lhs;
    EXPECT_TRUE(lhs == rhs);
    rhs = { false, "homeNodebName", 0x12345 };
    EXPECT_FALSE(lhs == rhs);
    rhs = { true, "homeNodebName2", 0x12345 };
    EXPECT_FALSE(lhs == rhs);
    rhs = { true, "homeNodebName", 7777 };
    EXPECT_FALSE(lhs == rhs);
}

TEST(NetworkUtils_compare, RIL_CellIdentityGsm_V1_5) {
    RIL_CellIdentityGsm_V1_5 lhs = {
        450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr,
    };
    decltype(lhs) rhs = {
        450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { 450, 3, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr},
        { 460, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr},
        { 450, 5, 10000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr},
        { 450, 5, 20000, 30000, 1000, 10, {"test long", "test short"}, 0, nullptr},
        { 450, 5, 20000, 40000, 1000, 15, {"test short", "test short"}, 0, nullptr},
        { 450, 5, 20000, 40000, 1000, 10, {"test long", "test long"}, 0, nullptr},
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentityWcdma_V1_5) {
    RIL_CellIdentityWcdma_V1_5 lhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
        {{ true, "homeNodebName", 0x12345 }},
    };
    decltype(lhs) rhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
        {{ true, "homeNodebName", 0x12345 }},
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { 450, 3, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 460, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 30000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 50000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 200, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1500, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test long"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test short", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ false, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName2", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x1234 }} },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentityTdscdma_V1_5) {
    RIL_CellIdentityTdscdma_V1_5 lhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
        {{ true, "homeNodebName", 0x12345 }},
    };
    decltype(lhs) rhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
        {{ true, "homeNodebName", 0x12345 }},
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { 450, 3, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 460, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 30000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 50000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 200, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1500, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test long"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test short", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ false, "homeNodebName", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName2", 0x12345 }} },
        { 450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x1234 }} },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentityLte_V1_5) {
    RIL_CellIdentityLte_V1_5 lhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
        {{ true, "homeNodebName", 0x12345 }},
        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
    };
    decltype(lhs) rhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
        {{ true, "homeNodebName", 0x12345 }},
        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            450, 3, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            460, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 20000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 100, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1500, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test short", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test long"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 300, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ false, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName2", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x2345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            2, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_2, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentityNr_V1_5) {
    RIL_CellIdentityNr_V1_5 lhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
    };
    decltype(lhs) rhs = {
        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            450, 3, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            460, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 20000, 30000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 20000, 40000, 1300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 20000, 40000, 300, 2200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test long"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test short", "test short"}, 0, nullptr,
            2, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_3, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentityCdma_V1_2) {
    RIL_CellIdentityCdma_V1_2 lhs = {
        10, 20, 30, 40, 50, {"test long", "test short"}
    };
    decltype(lhs) rhs = {
        10, 20, 30, 40, 50, {"test long", "test short"}
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { 100, 20, 30, 40, 50, {"test long", "test short"} },
        { 10, 200, 30, 40, 50, {"test long", "test short"} },
        { 10, 20, 300, 40, 50, {"test long", "test short"} },
        { 10, 20, 30, 400, 50, {"test long", "test short"} },
        { 10, 20, 30, 40, 500, {"test long", "test short"} },
        { 10, 20, 30, 40, 50, {"test long", "test long"} },
        { 10, 20, 30, 40, 50, {"test short", "test short"} },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_NONE) {
    RIL_CellIdentity_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE, {}
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE, {}
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, {}
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, {}
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_GSM) {
    RIL_CellIdentity_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
        .gsm = {
            450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr,
        }
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
        .gsm = {
            450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr,
        }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            .gsm = {
                450, 3, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr,
            }
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM,
            .gsm = {
                450, 5, 20000, 40000, 1000, 10, {"test long", "test long"}, 0, nullptr,
            }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_WCDMA) {
    RIL_CellIdentity_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
        .wcdma = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
        }
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
        .wcdma = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
        }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            .wcdma = {
                450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
            }
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA,
            .wcdma = {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName", 0x2345 }},
            }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_TD_SCDMA) {
    RIL_CellIdentity_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
        .tdscdma = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
        }
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
        .tdscdma = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
        }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            .tdscdma = {
                450, 5, 20000, 40000, 200, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
            }
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA,
            .tdscdma = {
                450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName2", 0x12345 }},
            }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_CDMA) {
    RIL_CellIdentity_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
        .cdma = {
            10, 20, 30, 40, 50, {"test long", "test short"}
        }
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
        .cdma = {
            10, 20, 30, 40, 50, {"test long", "test short"}
        }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            .cdma = {
                10, 20, 30, 40, 30, {"test long", "test short"}
            }
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_CDMA,
            .cdma = {
                10, 20, 50, 40, 50, {"test long", "test short"}
            }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_LTE) {
    RIL_CellIdentity_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
        .lte = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        }
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
        .lte = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .lte = {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 110, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            }
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE,
            .lte = {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_3, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellIdentity_V1_5_for_NR) {
    RIL_CellIdentity_V1_5 lhs = {
        (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR,
        .nr = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        }
    };
    decltype(lhs) rhs = {
        (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR,
        .nr = {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR,
            .nr = {
                450, 5, 20000, 30000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
            }
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR,
            .nr = {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                2, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
            }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_GSM_SignalStrength_v12) {
    RIL_GSM_SignalStrength_v12 lhs = { 15, 3, 7 };
    RIL_GSM_SignalStrength_v12 rhs = { 15, 3, 7 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<RIL_GSM_SignalStrength_v12> testVector = {
        { 10, 3, 7 },
        { 15, 2, 7 },
        { 15, 3, 0 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_WCDMA_SignalStrength_V1_2) {
    RIL_WCDMA_SignalStrength_V1_2 lhs = { 15, 3, 255, 255 };
    RIL_WCDMA_SignalStrength_V1_2 rhs = { 15, 3, 255, 255 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<RIL_WCDMA_SignalStrength_V1_2> testVector = {
        { 10, 3, 255, 255 },
        { 15, 0, 255, 255 },
        { 15, 3, 10, 255 },
        { 15, 3, 255, 3 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_TD_SCDMA_SignalStrength_V1_2) {
    RIL_TD_SCDMA_SignalStrength_V1_2 lhs = { 15, 3, 255 };
    RIL_TD_SCDMA_SignalStrength_V1_2 rhs = { 15, 3, 255 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<RIL_TD_SCDMA_SignalStrength_V1_2> testVector = {
        { 5, 3, 255 },
        { 15, 0, 255 },
        { 15, 3, 90 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_LTE_SignalStrength_v8) {
    RIL_LTE_SignalStrength_v8 lhs = { 15, 140, 10, -100, 0, 0x7FFFFFFE };
    RIL_LTE_SignalStrength_v8 rhs = { 15, 140, 10, -100, 0, 0x7FFFFFFE };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<RIL_LTE_SignalStrength_v8> testVector = {
        { 10, 140, 10, -100, 0, 0x7FFFFFFE },
        { 15, 40, 10, -100, 0, 0x7FFFFFFE },
        { 15, 140, 15, -100, 0, 0x7FFFFFFE },
        { 15, 140, 10, 0, 0, 0x7FFFFFFE },
        { 15, 140, 10, -100, 15, 0x7FFFFFFE },
        { 15, 140, 10, -100, 0, 0xFFFFFFE },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_LTE_SignalStrength_V1_6) {
    RIL_LTE_SignalStrength_V1_6 lhs = { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 };
    RIL_LTE_SignalStrength_V1_6 rhs = { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<RIL_LTE_SignalStrength_V1_6> testVector = {
        { {10, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        { {15, 40, 10, -100, 0, 0x7FFFFFFE}, 1 },
        { {15, 140, 15, -100, 0, 0x7FFFFFFE}, 1 },
        { {15, 140, 10, 0, 0, 0x7FFFFFFE}, 1 },
        { {15, 140, 10, -100, 15, 0x7FFFFFFE}, 1 },
        { {15, 140, 10, -100, 0, 0xFFFFFFE}, 1 },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 2 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_NR_SignalStrength_V1_4) {
    RIL_NR_SignalStrength_V1_4 lhs = { 44, 7, -10, 44, 7, -10 };
    RIL_NR_SignalStrength_V1_4 rhs = { 44, 7, -10, 44, 7, -10 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<RIL_NR_SignalStrength_V1_4> testVector = {
        { 100, 7, -10, 44, 7, -10 },
        { 44, 15, -10, 44, 7, -10 },
        { 44, 7, 10, 44, 7, -10 },
        { 44, 7, -10, 120, 7, -10 },
        { 44, 7, -10, 44, 3, -10 },
        { 44, 7, -10, 44, 7, -1 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_NR_SignalStrength_V1_6) {
    RIL_NR_SignalStrength_V1_6 lhs = { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} };
    decltype(lhs) rhs = { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { {100, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} },
        { {44, 15, -10, 44, 7, -10}, 1, 3, {1, 2, 3} },
        { {44, 7, 10, 44, 7, -10}, 1, 3, {1, 2, 3} },
        { {44, 7, -10, 120, 7, -10}, 1, 3, {1, 2, 3} },
        { {44, 7, -10, 44, 3, -10}, 1, 3, {1, 2, 3} },
        { {44, 7, -10, 44, 7, -1}, 1, 3, {1, 2, 3} },
        { {44, 7, -10, 44, 7, -10}, 2, 3, {1, 2, 3} },
        { {44, 7, -10, 44, 7, -10}, 1, 2, {1, 2, 3} },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 5} },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CDMA_SignalStrength) {
    RIL_CDMA_SignalStrength lhs = { 70, 125 };
    decltype(lhs) rhs = { 70, 125 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { 60, 125 },
        { 70, 100 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_EVDO_SignalStrength) {
    RIL_EVDO_SignalStrength lhs = { 70, 125, 0 };
    decltype(lhs) rhs = { 70, 125, 0 };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        { 60, 125, 0 },
        { 70, 100, 0 },
        { 70, 125, 3 },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_SignalStrength_V1_6) {
    RIL_SignalStrength_V1_6 lhs = {
        { 60, 125, 0 },
        { 70, 125 },
        { 70, 125, 3 },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        { 15, 3, 255 },
        { 15, 3, 255, 255 },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
    };
    decltype(lhs) rhs = {
        { 60, 125, 0 },
        { 70, 125 },
        { 70, 125, 3 },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        { 15, 3, 255 },
        { 15, 3, 255, 255 },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
     };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            { 60, 15, 0 },
            { 70, 125 },
            { 70, 125, 3 },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 15, 3, 255 },
            { 15, 3, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            { 60, 125, 0 },
            { 70, 105 },
            { 70, 125, 3 },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 15, 3, 255 },
            { 15, 3, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            { 60, 125, 0 },
            { 70, 125 },
            { 70, 125, 1 },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 15, 3, 255 },
            { 15, 3, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            { 60, 125, 0 },
            { 70, 125 },
            { 70, 125, 3 },
            { {15, 100, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 15, 3, 255 },
            { 15, 3, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            { 60, 125, 0 },
            { 70, 125 },
            { 70, 125, 3 },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 12, 3, 255 },
            { 15, 3, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            { 60, 125, 0 },
            { 70, 125 },
            { 70, 125, 3 },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 15, 3, 255 },
            { 15, 0, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            { 60, 125, 0 },
            { 70, 125 },
            { 70, 125, 3 },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            { 15, 3, 255 },
            { 15, 3, 255, 255 },
            { {44, 7, -10, 44, 7, -10}, 2, 3, {1, 2, 3} }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoGsm_V1_5) {
    RIL_CellInfoGsm_V1_5 lhs = {
        { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
        { 15, 3, 7 }
    };
    decltype(lhs) rhs = {
        { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
        { 15, 3, 7 }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            { 450, 5, 20000, 40000, 1200, 10, {"test long", "test short"}, 0, nullptr },
            { 15, 3, 7 }
        },
        {
            { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
            { 10, 3, 7 }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoWcdma_V1_5) {
    RIL_CellInfoWcdma_V1_5 lhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }}
        },
        { 15, 3, 255, 255 }
    };
    decltype(lhs) rhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }}
        },
        { 15, 3, 255, 255 }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            {
                450, 3, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }}
            },
            { 15, 3, 255, 255 }
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }}
            },
            { 10, 3, 255, 255 }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoTdscdma_V1_5) {
    RIL_CellInfoTdscdma_V1_5 lhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
        },
        { 15, 3, 255 }
    };
    decltype(lhs) rhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
        },
        { 15, 3, 255 }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test long"}, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
            },
            { 15, 3, 255 }
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
            },
            { 15, 3, 9 }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoLte_V1_5) {
    RIL_CellInfoLte_V1_5 lhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        { 15, 140, 10, -100, 0, 0x7FFFFFFE },
        true
    };
    decltype(lhs) rhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        { 15, 140, 10, -100, 0, 0x7FFFFFFE },
        true
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 30, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            },
            { 15, 140, 10, -100, 0, 0x7FFFFFFE },
            true
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            },
            { 15, 50, 10, -100, 0, 0x7FFFFFFE },
            true
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            },
            { 15, 140, 10, -100, 0, 0x7FFFFFFE },
            false
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoLte_V1_6) {
    RIL_CellInfoLte_V1_6 lhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        true
    };
    decltype(lhs) rhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
            {{ true, "homeNodebName", 0x12345 }},
            3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
        },
        { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
        true
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            {
                450, 5, 20000, 10000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            true
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_3, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            },
            { {15, 100, 10, -100, 0, 0x7FFFFFFE}, 1 },
            true
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                {{ true, "homeNodebName", 0x12345 }},
                3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
            },
            { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
            false
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoNr_V1_5) {
    RIL_CellInfoNr_V1_5 lhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        { 44, 7, -10, 44, 7, -10 }
    };
    decltype(lhs) rhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        { 44, 7, -10, 44, 7, -10 }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                2, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
            },
            { 44, 7, -10, 44, 7, -10 }
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
            },
            { 44, 7, 0, 44, 7, -10 }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoNr_V1_6) {
    RIL_CellInfoNr_V1_6 lhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
    };
    decltype(lhs) rhs = {
        {
            450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
            3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
        },
        { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_90}
            },
            { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
        },
        {
            {
                450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
            },
            { {44, 7, -10, 44, 7, -10}, 0, 3, {1, 2, 3} }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfoCdma_V1_2) {
    RIL_CellInfoCdma_V1_2 lhs = {
        { 10, 20, 30, 40, 50, {"test long", "test short"} },
        { 70, 125 },
        { 70, 125, 0 }
    };
    decltype(lhs) rhs = {
        { 10, 20, 30, 40, 50, {"test long", "test short"} },
        { 70, 125 },
        { 70, 125, 0 }
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            { 10, 20, 30, 40, 50, {"test long", "test long"} },
            { 70, 125 },
            { 70, 125, 0 }
        },
        {
            { 10, 20, 30, 40, 50, {"test long", "test short"} },
            { 70, 25 },
            { 70, 125, 0 }
        },
        {
            { 10, 20, 30, 40, 50, {"test long", "test short"} },
            { 70, 125 },
            { 70, 125, 1 }
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_5_for_NONE) {
    RIL_CellInfo_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {},
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {},
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {},
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {},
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_5_for_GSM) {
    RIL_CellInfo_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .gsm = {
                { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                { 15, 3, 7 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .gsm = {
                { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                { 15, 3, 7 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, false, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .gsm = {
                    { 450, 5, 20000, 30000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 2, 7 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
            RIL_CellConnectionStatus::SECONDARY_SERVING,
        },

    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_5_for_WCDMA) {
    RIL_CellInfo_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .wcdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }}
                },
                { 15, 3, 255, 255 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .wcdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }}
                },
                { 15, 3, 255, 255 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, false, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .wcdma = {
                    {
                        450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 10, 3, 255, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
            RIL_CellConnectionStatus::NONE,
        },

    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_5_for_TD_SCDMA) {
    RIL_CellInfo_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .tdscdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                },
                { 15, 3, 255 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .tdscdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                },
                { 15, 3, 255 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName2", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 2, 255 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
            RIL_CellConnectionStatus::SECONDARY_SERVING,
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_5_for_LTE) {
    RIL_CellInfo_V1_5 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .lte = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                    3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                },
                { 15, 140, 10, -100, 0, 0x7FFFFFFE },
                true
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .lte = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                    3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                },
                { 15, 140, 10, -100, 0, 0x7FFFFFFE },
                true
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, false, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { 15, 140, 10, -100, 0, 0x7FFFFFFE },
                    true
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_3, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { 15, 140, 10, -100, 0, 0x7FFFFFFE },
                    true
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { 15, 140, 10, -100, 0, 0x7FFFFFFE },
                    true
                },
            },
            RIL_CellConnectionStatus::NONE,
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_5_for_NR) {
    RIL_CellInfo_V1_5 lhs = {
        (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
        RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .nr = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                },
                { 44, 7, -10, 44, 7, -10 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    decltype(lhs) rhs = {
        (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
        RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
        {
            .nr = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                },
                { 44, 7, -10, 44, 7, -10 }
            },
        },
        RIL_CellConnectionStatus::PRIMARY_SERVING,
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, false,
            RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { 44, 7, -10, 44, 7, -10 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .nr = {
                    {
                        450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { 44, 7, -10, 44, 7, -10 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, 0UL,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { 44, 7, -10, 44, 8, -10 }
                },
            },
            RIL_CellConnectionStatus::PRIMARY_SERVING,
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_6_for_NONE) {
    RIL_CellInfo_V1_6 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {},
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_NONE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {},
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {},
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {},
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_6_for_GSM) {
    RIL_CellInfo_V1_6 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .gsm = {
                { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                { 15, 3, 7 }
            },
        },
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .gsm = {
                { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                { 15, 3, 7 }
            },
        },
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, false, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .gsm = {
                    { 450, 5, 20000, 30000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 2, 7 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_GSM, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            {
                .gsm = {
                    { 450, 5, 20000, 40000, 1000, 10, {"test long", "test short"}, 0, nullptr },
                    { 15, 3, 7 }
                },
            },
        },

    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_6_for_WCDMA) {
    RIL_CellInfo_V1_6 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .wcdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }}
                },
                { 15, 3, 255, 255 }
            },
        },
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .wcdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }}
                },
                { 15, 3, 255, 255 }
            },
        },
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, false, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .wcdma = {
                    {
                        450, 5, 10000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 10, 3, 255, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_WCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::NONE,
            {
                .wcdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }}
                    },
                    { 15, 3, 255, 255 }
                },
            },
        },

    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_6_for_TD_SCDMA) {
    RIL_CellInfo_V1_6 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .tdscdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                },
                { 15, 3, 255 }
            },
        },
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .tdscdma = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                },
                { 15, 3, 255 }
            },
        },
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName2", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 2, 255 }
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_TD_SCDMA, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::SECONDARY_SERVING,
            {
                .tdscdma = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                    },
                    { 15, 3, 255 }
                },
            },
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_6_for_LTE) {
    RIL_CellInfo_V1_6 lhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .lte = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                    3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                },
                { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                true
            },
        },
    };
    decltype(lhs) rhs = {
        RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
        RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .lte = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                    {{ true, "homeNodebName", 0x12345 }},
                    3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                },
                { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                true
            },
        },
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, false, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                    true
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, false, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::NONE,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                    true
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .lte = {
                    {
                        450, 3, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                    true
                },
            },
        },
        {
            RIL_CellInfoType::RIL_CELL_INFO_TYPE_LTE, true, RIL_TIMESTAMP_TYPE_OEM_RIL,
            RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .lte = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 100, 0, nullptr,
                        {{ true, "homeNodebName", 0x12345 }},
                        3, {RIL_EutranBands::EUTRAN_BAND_1, RIL_EutranBands::EUTRAN_BAND_13, RIL_EutranBands::EUTRAN_BAND_20}
                    },
                    { {15, 140, 10, -100, 0, 0x7FFFFFFE}, 1 },
                    false
                },
            },
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}

TEST(NetworkUtils_compare, RIL_CellInfo_V1_6_for_NR) {
    RIL_CellInfo_V1_6 lhs = {
        (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
        RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .nr = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                },
                { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
            },
        },
    };
    decltype(lhs) rhs = {
        (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
        RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
        {
            .nr = {
                {
                    450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                    3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                },
                { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
            },
        },
    };
    EXPECT_TRUE(lhs == rhs);

    std::initializer_list<decltype(lhs)> testVector = {
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, false,
            RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
                },
            },
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .nr = {
                    {
                        460, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { {44, 7, -10, 44, 7, -10}, 1, 3, {1, 2, 3} }
                },
            },
        },
        {
            (RIL_CellInfoType)RIL_CellInfoType_V1_4::RIL_CELL_INFO_TYPE_NR, true,
            RIL_TIMESTAMP_TYPE_OEM_RIL, RIL_CellConnectionStatus::PRIMARY_SERVING,
            {
                .nr = {
                    {
                        450, 5, 20000, 40000, 300, 1200, {"test long", "test short"}, 0, nullptr,
                        3, {RIL_NgranBands::NGRAN_BAND_1, RIL_NgranBands::NGRAN_BAND_8, RIL_NgranBands::NGRAN_BAND_65}
                    },
                    { {44, 7, -1, 44, 7, -10}, 1, 3, {1, 2, 3} }
                },
            },
        },
    };
    for (auto& cur : testVector) {
        EXPECT_FALSE(lhs == cur);
    }
}