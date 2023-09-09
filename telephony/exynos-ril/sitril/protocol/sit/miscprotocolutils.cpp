/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "miscprotocolutils.h"
#include "commondef.h"
#include "../../base/modemconfiguration.h"
#include "rillog.h"
#include <slsi/radio_v1_2.h>
#include <slsi/radio_v1_5.h>

namespace MiscProtocolUtils {

int ConvertAccessNetworkType2Sit(int accessNetworkType)
{
    switch (accessNetworkType) {
    case ACCESS_NETWORK_GERAN:
        return SIT_NET_ACCESS_RADIO_TYPE_GERAN;
    case ACCESS_NETWORK_UTRAN:
        return SIT_NET_ACCESS_RADIO_TYPE_UTRAN;
    case ACCESS_NETWORK_EUTRAN:
        return SIT_NET_ACCESS_RADIO_TYPE_EUTRAN;
    case ACCESS_NETWORK_CDMA2000:
        return SIT_NET_ACCESS_RADIO_TYPE_CDMA2000;
    case ACCESS_NETWORK_IWLAN:
        return SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN;
    case ACCESS_NETWORK_NGRAN:
        return SIT_NET_ACCESS_RADIO_TYPE_NGRAN;
    }
    return SIT_NET_ACCESS_RADIO_TYPE_UNKNOWN_ALLRAN;
}

UINT32 MappingSGCValue(int targetOp)
{
    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    static const int sgcMap[][2] = {
        // Global SGC
        { TARGET_OPER_CHNOPEN, SIT_ID_SGC_SP_CHNOPEN },
        { TARGET_OPER_CHNOPEN_GCF, SIT_ID_SGC_SP_CHNOPEN_GCF },
        { TARGET_OPER_CMCC, SIT_ID_SGC_SP_CMCC },
        { TARGET_OPER_CTC, SIT_ID_SGC_SP_CTC },
        { TARGET_OPER_CU, SIT_ID_SGC_SP_CUCC },
        { TARGET_OPER_ATT, SIT_ID_SGC_SP_ATT },
        { TARGET_OPER_TMO, SIT_ID_SGC_SP_TMOUSA },
        { TARGET_OPER_VZW, SIT_ID_SGC_SP_VZW },
        { TARGET_OPER_SPR, SIT_ID_SGC_SP_SPR },
        { TARGET_OPER_NA_GCF, SIT_ID_SGC_SP_NA_GCF },
        { TARGET_OPER_DISH, SIT_ID_SGC_SP_DISH },
        { TARGET_OPER_LATIN, SIT_ID_SGC_SP_LATIN },
        { TARGET_OPER_LATIN_GCF, SIT_ID_SGC_SP_LATIN_GCF },
        { TARGET_OPER_EUROPEN, SIT_ID_SGC_SP_EUROPEN },
        { TARGET_OPER_EUROPEN_GCF, SIT_ID_SGC_SP_GCF },
        { TARGET_OPER_VIOLET, SIT_ID_SGC_SP_VIOLET },
        { TARGET_OPER_NTT, SIT_ID_SGC_SP_NTT },
        { TARGET_OPER_KDDI, SIT_ID_SGC_SP_KDDI },
        { TARGET_OPER_SBM, SIT_ID_SGC_SP_SBM },
        { TARGET_OPER_RKT, SIT_ID_SGC_SP_RKT },

        // Latam specific
        { TARGET_OPER_CLARO_AR, SIT_ID_SGC_CLARO_AR },
        { TARGET_OPER_MOV_AR, SIT_ID_SGC_MOV_AR },
        { TARGET_OPER_TUENTI_AR, SIT_ID_SGC_TUENTI_AR },
        { TARGET_OPER_NII_AR, SIT_ID_SGC_NII_AR },
        { TARGET_OPER_NUESTRO_AR, SIT_ID_SGC_NUESTRO_AR },
        { TARGET_OPER_PERSONAL_AR, SIT_ID_SGC_PERSONAL_AR },
        { TARGET_OPER_TIGO_BO, SIT_ID_SGC_TIGO_BO },
        { TARGET_OPER_VIVA_BO, SIT_ID_SGC_VIVA_BO },
        { TARGET_OPER_CLARO_BR, SIT_ID_SGC_CLARO_BR },
        { TARGET_OPER_VIVO_BR, SIT_ID_SGC_VIVO_BR },
        { TARGET_OPER_NII_BR, SIT_ID_SGC_NII_BR },
        { TARGET_OPER_OI_BR, SIT_ID_SGC_OI_BR },
        { TARGET_OPER_PORTO_CONECTA_BR, SIT_ID_SGC_PORTO_CONECTA_BR },
        { TARGET_OPER_SURF_BR, SIT_ID_SGC_SURF_BR },
        { TARGET_OPER_TIM_BR, SIT_ID_SGC_TIM_BR },
        { TARGET_OPER_CLARO_CL, SIT_ID_SGC_CLARO_CL },
        { TARGET_OPER_MOV_CL, SIT_ID_SGC_MOV_CL },
        { TARGET_OPER_ENTEL_CL, SIT_ID_SGC_ENTEL_CL },
        { TARGET_OPER_CLARO_BR, SIT_ID_SGC_WOM_CL },
        { TARGET_OPER_CLARO_CO, SIT_ID_SGC_CLARO_CO },
        { TARGET_OPER_MOV_CO, SIT_ID_SGC_MOV_CO },
        { TARGET_OPER_AVANTEL_CO, SIT_ID_SGC_AVANTEL_CO },
        { TARGET_OPER_ETB_CO, SIT_ID_SGC_ETB_CO },
        { TARGET_OPER_TIGO_CO, SIT_ID_SGC_TIGO_CO },
        { TARGET_OPER_CLARO_CR, SIT_ID_SGC_CLARO_CR },
        { TARGET_OPER_CLARO_DO, SIT_ID_SGC_CLARO_DO },
        { TARGET_OPER_CLARO_EC, SIT_ID_SGC_CLARO_EC },
        { TARGET_OPER_MOV_EC, SIT_ID_SGC_MOV_EC },
        { TARGET_OPER_CNT_EC, SIT_ID_SGC_CNT_EC },
        { TARGET_OPER_CLARO_SV, SIT_ID_SGC_CLARO_SV },
        { TARGET_OPER_TIGO_SV, SIT_ID_SGC_TIGO_SV },
        { TARGET_OPER_CLARO_GT, SIT_ID_SGC_CLARO_GT },
        { TARGET_OPER_TIGO_GT, SIT_ID_SGC_TIGO_GT },
        { TARGET_OPER_CLARO_HN, SIT_ID_SGC_CLARO_HN },
        { TARGET_OPER_TIGO_HO, SIT_ID_SGC_TIGO_HO },
        { TARGET_OPER_TELCEL_MX, SIT_ID_SGC_TELCEL_MX },
        { TARGET_OPER_MOV_MX, SIT_ID_SGC_MOV_MX },
        { TARGET_OPER_ALTAN_MX, SIT_ID_SGC_ALTAN_MX },
        { TARGET_OPER_ATT_MX, SIT_ID_SGC_ATT_MX },
        { TARGET_OPER_CLARO_NI, SIT_ID_SGC_CLARO_NI },
        { TARGET_OPER_CLARO_PA, SIT_ID_SGC_CLARO_PA },
        { TARGET_OPER_CLARO_PY, SIT_ID_SGC_CLARO_PY },
        { TARGET_OPER_PERSONAL_PY, SIT_ID_SGC_PERSONAL_PY },
        { TARGET_OPER_TIGO_PY, SIT_ID_SGC_TIGO_PY },
        { TARGET_OPER_CLARO_PE, SIT_ID_SGC_CLARO_PE },
        { TARGET_OPER_MOV_PE, SIT_ID_SGC_MOV_PE },
        { TARGET_OPER_ENTEL_PE, SIT_ID_SGC_ENTEL_PE },
        { TARGET_OPER_CLARO_PR, SIT_ID_SGC_CLARO_PR },
        { TARGET_OPER_OPEN_MOBILE_PR, SIT_ID_SGC_OPEN_MOBILE_PR },
        { TARGET_OPER_CLARO_UY, SIT_ID_SGC_CLARO_UY },
        { TARGET_OPER_MOV_UY, SIT_ID_SGC_MOV_UY },
        { TARGET_OPER_ANTEL_UY, SIT_ID_SGC_ANTEL_UY },
        { TARGET_OPER_MOV_UZ, SIT_ID_SGC_MOV_UZ },
        { TARGET_OPER_BELL, SIT_ID_SGC_SP_BMC },
        { TARGET_OPER_TELUS, SIT_ID_SGC_SP_TELUS },
        { TARGET_OPER_ROGERS, SIT_ID_SGC_SP_ROGERS },
        { TARGET_OPER_FREEDOM, SIT_ID_SGC_SP_FREEDOM },

        // Kor
        { TARGET_OPER_KT, SIT_ID_SGC_SP_KT },
        { TARGET_OPER_LGU, SIT_ID_SGC_SP_LGU },
        { TARGET_OPER_KOROPEN, SIT_ID_SGC_SP_KOROPEN },
        { TARGET_OPER_SKT, SIT_ID_SGC_SP_SKT },
    };

    if (targetOp < 0) {
        return SIT_ID_SGC_SP_CHNOPEN;
    }

    UINT32 sgcId = SIT_ID_SGC_SP_CHNOPEN; // default
    int size = sizeof(sgcMap) / sizeof(sgcMap[0]);
    for (int i = 0; i < size; i++) {
        if (sgcMap[i][0] == targetOp) {
            sgcId = sgcMap[i][1];
            break;
        }
    } // end for i ~

    return sgcId;
}

} // namespace MiscProtocolUtils
