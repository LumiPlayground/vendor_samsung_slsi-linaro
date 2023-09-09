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
 * netprotocolutils.h
 *
 *  Created on: 2021. 09. 02.
 *      Author: dowon.nam
 */

#ifndef __NET_PROTOCOL_UTILS_H__
#define __NET_PROTOCOL_UTILS_H__

#include "rildef.h"
#include "sitdef.h"
#include "commondef.h"
#include <type_traits>

namespace NetProtocolUtils {
    // for RIL => SIT
    int ConvertNetworktype2Sit(int netType);
    int ConvertRadioAccessNetworkType2Sit(int radioAccessNetworkType);
    int ConvertDsNetworkType2Sit(int dsNetType);
    int ConvertNetworkScanType2Sit(int rilScanType);

    // for SIT => RIL
    int ConvertSitRaf2Ril(int raf, bool needToAdjust );
    int ConvertSitRat2RilRat(int sitRat);
    int ConvertSitRegState2Ril(int sitRegStatus);
    int ConvertPrefNetworkType2Ril(int sitNetType);
    const char *ConvertPlmnStatus2Ril(int plmnStatus);
    int ConvertSitCdmaRoamingType2Ril(int sitValue);

    // etc
    const char *ConvertSitDsNetworkType2String(int sitDsNetType);
    const char *ConvertPwrRadioSimState2String(int pwrRadioSimState);
    const char *ConvertRadioState2String(int radioState);

    const char *ConvertSitPreferredNetType2String(int sitNetType);
    const char *ConvertSitCdmaHybridMode2String(int sitHybridMode);

    int IsSysSelSupported(int sitRan);
    void InitAcBarringInfo(AC_BARRING_INFO &info);
    bool IsValidCellInfo(RIL_CellInfo_V1_6& cellInfo);
    void FreeAdditionalPlmms(RIL_CellInfo_V1_6& cellInfo);
}  // NetProtocolUtils

namespace CellInfoProtocolUtils {
    // CellInfo helper functions
    void InitCellInfo(RIL_CellInfo_V1_6& info, int type);
    void PrintCellInfo(int index, RIL_CellInfo_V1_6& cellInfo);

    namespace Legacy {
        int FetchMcc(const char *plmn);
        int FetchMnc(const char *plmn);
        void FillOperatorName(RIL_CellIdentityOperatorNames& name, int mcc, int encodedMnc);
        void FillGsmCellInfo(RIL_CellInfoGsm_V1_5& out, cell_info_gsm_v12& cellInfo);
        void FillGsmCellInfo(RIL_CellInfoGsm_V1_5& out, cell_info_type_gsm_v3& cellInfo);
        void FillGsmCellInfo(RIL_CellInfoGsm_V1_5& out, cell_info_type_gsm_v4& cellInfo);
        template <typename T>
            void fillCdmaCellInfo_V1_2(RIL_CellInfoCdma_V1_2 &out, T &cellInfo);
        void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_cdma& cellInfo);
        void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_cdma_v14& cellInfo);
        void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_type_cdma_v3& cellInfo);
        void FillCdmaCellInfo(RIL_CellInfoCdma_V1_2& out, cell_info_type_cdma_v4& cellInfo);
        void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_lte_v12& cellInfo);
        void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_lte_v14& cellInfo);
        void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_type_lte_v3& cellInfo);
        void FillLteCellInfo(RIL_CellInfoLte_V1_6& out, cell_info_type_lte_v4& cellInfo);
        void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5& out, cell_info_wcdma_v12& cellInfo);
        void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5& out, cell_info_wcdma_v14& cellInfo);
        void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5& out, cell_info_type_wcdma_v3& cellInfo);
        void FillWcdmaCellInfo(RIL_CellInfoWcdma_V1_5& out, cell_info_type_wcdma_v4& cellInfo);
        void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_tdscdma& cellInfo);
        void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_tdscdma_v14& cellInfo);
        void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_type_tdscdma_v3& cellInfo);
        void FillTdscdmaCellInfo(RIL_CellInfoTdscdma_V1_5& out, cell_info_type_tdscdma_v4& cellInfo);
        void FillNrCellInfo(RIL_CellInfoNr_V1_6& out, cell_info_nr& cellInfo);
        void FillNrCellInfo(RIL_CellInfoNr_V1_6& out, cell_info_type_nr_v3& cellInfo);
        void FillNrCellInfo(RIL_CellInfoNr_V1_6& out, cell_info_type_nr_v4& cellInfo);

        // CellIdentity helper functions
        void FillCellIdentityGsm(RIL_CellIdentityGsm_V1_5& out, cell_identity_gsm_v3& cellIdentity);
        void FillCellIdentityGsm(RIL_CellIdentityGsm_V1_5& out, cell_identity_gsm_v4& cellIdentity);
        void FillCellIdentityCdma(RIL_CellIdentityCdma_V1_2& out, cell_identity_cdma_v3& cellIdentity);
        void FillCellIdentityCdma(RIL_CellIdentityCdma_V1_2& out, cell_identity_cdma_v4& cellIdentity);
        void FillCellIdentityLte(RIL_CellIdentityLte_V1_5& out, cell_identity_lte_v3& cellIdentity);
        void FillCellIdentityLte(RIL_CellIdentityLte_V1_5& out, cell_identity_lte_v4& cellIdentity);
        void FillCellIdentityWcdma(RIL_CellIdentityWcdma_V1_5& out, cell_identity_wcdma_v3& cellIdentity);
        void FillCellIdentityWcdma(RIL_CellIdentityWcdma_V1_5& out, cell_identity_wcdma_v4& cellIdentity);
        void FillCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_5& out, cell_identity_tdscdma_v3& cellIdentity);
        void FillCellIdentityTdscdma(RIL_CellIdentityTdscdma_V1_5& out, cell_identity_tdscdma_v4& cellIdentity);
        void FillCellIdentityNr(RIL_CellIdentityNr_V1_5& out, cell_identity_nr_v3& cellIdentity);
        void FillCellIdentityNr(RIL_CellIdentityNr_V1_5& out, cell_identity_nr_v4& cellIdentity);

        // CellIdentity CSG
        void FillCellIdentityCsgInfo(RIL_OptionalCsgInfo& out, const csg_info_v3& csgInfo);
        void FillCellIdentityCsgInfo(RIL_OptionalCsgInfo& out, const csg_info_v4& csgInfo);
    } // CellInfoProtocolUtils::Legacy

    namespace JSON {
        bool isValidPlmn(const char* plmn);
        int FetchMcc(const char* plmn);
        int FetchMnc(const char* plmn);
    } // CellInfoProtocolUtils::JSON
}  // CellInfoProtocolUtils

using GERAN_TYPE = std::integral_constant<int, SIT_NET_ACCESS_RADIO_TYPE_GERAN>;
using UTRAN_TYPE = std::integral_constant<int, SIT_NET_ACCESS_RADIO_TYPE_UTRAN>;
using EUTRAN_TYPE = std::integral_constant<int, SIT_NET_ACCESS_RADIO_TYPE_EUTRAN>;
using NGRAN_TYPE = std::integral_constant<int, SIT_NET_ACCESS_RADIO_TYPE_NGRAN>;

template <typename Tsrc, typename Tdst>
inline void CopyArrayDataSrcToDst(Tsrc src, Tdst dst, BYTE copyLen) {
    using RemovePtTdst = std::remove_pointer_t<Tdst>;
    static_assert(std::is_pointer_v<Tsrc>);
    static_assert(std::is_pointer_v<Tdst>);

    for (int i = 0; i < copyLen; i++) {
        *dst++ = static_cast<RemovePtTdst>(*src++);
    }
}

#endif
