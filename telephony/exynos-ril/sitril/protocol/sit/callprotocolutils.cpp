/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include "callprotocolutils.h"
#include "commondef.h"
#include "sitdef.h" // TODO: Remove it once all enums(SS) are shifted to commondef.h
#include "rildef.h"
#include "rillog.h"

sit_ril_match_table SitRilValueTable[] =
{
    {
        SIT_CALL_TB_IDX_STATE,
        -1,
        -1,
        {
            {SIT_CALL_STATE_DIALING,      RIL_CALL_DIALING},
            {SIT_CALL_STATE_INCOMING,     RIL_CALL_INCOMING},
            {SIT_CALL_STATE_WAITING,      RIL_CALL_WAITING},
            {SIT_CALL_STATE_ALERTING,     RIL_CALL_ALERTING},
            {SIT_CALL_STATE_ACTIVE,       RIL_CALL_ACTIVE},
            {SIT_CALL_STATE_HOLDING,      RIL_CALL_HOLDING}
        }
    },
    {
        SIT_CALL_TB_IDX_PRESENTATION,
        -1,
        -1,
        {
            {SIT_CALL_PRESENTATION_ALLOWED,          RIL_CALL_NAME_PRESENTATION_ALLOW},
            {SIT_CALL_PRESENTATION_RESTRICTED,       RIL_CALL_NAME_PRESENTATION_RESTRICT},
            {SIT_CALL_PRESENTATION_NOT_SPECIFIED,    RIL_CALL_NAME_PRESENTATION_UNKNOWN},
            {SIT_CALL_PRESENTATION_PAYPHONE,         RIL_CALL_NAME_PRESENTATION_PAYPHONE}
        }
    },
    {
        SIT_CALL_TB_IDX_UUS_TYPE,
        -1,
        -1,
        {
            {SIT_CALL_UUS_TYPE_TYPE1_IMPLICIT,            RIL_UUS_TYPE1_IMPLICIT},
            {SIT_CALL_UUS_TYPE_TYPE1_REQUIRED,            RIL_UUS_TYPE1_REQUIRED},
            {SIT_CALL_UUS_TYPE_TYPE1_NOT_REQUIRED,        RIL_UUS_TYPE1_NOT_REQUIRED},
            {SIT_CALL_UUS_TYPE_TYPE2_REQUIRED,            RIL_UUS_TYPE2_REQUIRED},
            {SIT_CALL_UUS_TYPE_TYPE2_NOT_REQUIRED,        RIL_UUS_TYPE2_NOT_REQUIRED},
            {SIT_CALL_UUS_TYPE_TYPE3_REQUIRED,            RIL_UUS_TYPE3_REQUIRED},
            {SIT_CALL_UUS_TYPE_TYPE3_NOT_REQUIRED,        RIL_UUS_TYPE3_NOT_REQUIRED}
        }
    },
    {
        SIT_CALL_TB_IDX_UUS_DCS,
        -1,
        -1,
        {
            {SIT_CALL_UUS_DCS_USP,         RIL_UUS_DCS_USP},
            {SIT_CALL_UUS_DCS_OSIHLP,      RIL_UUS_DCS_OSIHLP},
            {SIT_CALL_UUS_DCS_X244RMCF,    RIL_UUS_DCS_X244},
            {SIT_CALL_UUS_DCS_RMCF,        RIL_UUS_DCS_RMCF},
            {SIT_CALL_UUS_DCS_IA5C,        RIL_UUS_DCS_IA5c}
        }
    },
    {
        SIT_CALL_TB_IDX_TYPE,
        -1,
        -1,
        {
            {SIT_CALL_CALL_TYPE_VOICE,             CALL_TYPE_VOICE},
            {SIT_CALL_CALL_TYPE_VIDEO,             CALL_TYPE_VIDEO},
            {SIT_CALL_CALL_TYPE_EMERGENCY,         CALL_TYPE_EMERGENCY},
            {SIT_CALL_CALL_TYPE_CDMA_VOICE,        CALL_TYPE_CDMA_VOICE},
            {SIT_CALL_CALL_TYPE_CDMA_EMERGENCY,    CALL_TYPE_CDMA_EMERGENCY}
        }
    },
    {
        SIT_CALL_TB_IDX_CLIR,
        -1,
        -1,
        {
            {SIT_CALL_CLIR_DEFAULT,         CLIR_DEFAULT},
            {SIT_CALL_CLIR_INVOCATION,      CLIR_INVOCATION},
            {SIT_CALL_CLIR_SUPPRESSION,     CLIR_SUPPRESSION}
        }
    },
    /*    {
          SIT_CALL_TB_IDX_RELEASECAUSE,
          SIT_CALL_LAST_CALL_FAIL_NORMAL,
          RET_VALUE_SAME_AS_SIT_COMMAND,
          {
          {SIT_CALL_LAST_CALL_FAIL_UNOBTAINABLE_NUMBER,    CALL_FAIL_UNOBTAINABLE_NUMBER},
          {SIT_CALL_LAST_CALL_FAIL_NORMAL,                 CALL_FAIL_NORMAL},
          {SIT_CALL_LAST_CALL_FAIL_BUSY,                   CALL_FAIL_BUSY},
          {SIT_CALL_LAST_CALL_FAIL_NORMAL_UNSPECIFIED,     CALL_FAIL_NORMAL_UNSPECIFIED},
          {SIT_CALL_LAST_CALL_FAIL_CONGESTION,             CALL_FAIL_CONGESTION},
          {SIT_CALL_LAST_CALL_FAIL_ACM_LIMIT_EXCEEDED,     CALL_FAIL_ACM_LIMIT_EXCEEDED},
          {SIT_CALL_LAST_CALL_FAIL_CALL_BARRED,            CALL_FAIL_CALL_BARRED},
          {SIT_CALL_LAST_CALL_FAIL_FDN_BLOCKED,            CALL_FAIL_FDN_BLOCKED},
          {SIT_CALL_LAST_CALL_FAIL_IMSI_UNKNOWN_IN_VLR,    CALL_FAIL_IMSI_UNKNOWN_IN_VLR},
          {SIT_CALL_LAST_CALL_FAIL_IMEI_NOT_ACCEPTED,      CALL_FAIL_IMEI_NOT_ACCEPTED},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE, CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_DROP, CALL_FAIL_CDMA_DROP},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_INTERCEPT, CALL_FAIL_CDMA_INTERCEPT},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_REORDER, CALL_FAIL_CDMA_REORDER},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_SO_REJECT, CALL_FAIL_CDMA_SO_REJECT},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_RETRY_ORDER, CALL_FAIL_CDMA_RETRY_ORDER},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_ACCESS_FAILURE, CALL_FAIL_CDMA_ACCESS_FAILURE},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_PREEMPTED, CALL_FAIL_CDMA_PREEMPTED},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_NOT_EMERGENCY, CALL_FAIL_CDMA_NOT_EMERGENCY},
          {SIT_CALL_LAST_CALL_FAIL_CDMA_ACCESS_BLOCKED, CALL_FAIL_CDMA_ACCESS_BLOCKED},
    // SIT_CALL_LAST_CALL_FAIL_CDMA_xxx ...

    {SIT_CALL_LAST_CALL_FAIL_ERROR_UNSPECIFIED,      CALL_FAIL_ERROR_UNSPECIFIED},
    }
    },
     */
    {
        SIT_CALL_TB_IDX_GET_CLIP_STATE,
            -1,
            -1,
            {
                {SIT_SS_GET_CLIP_STATUS_NOT_PROVISIONED,         CLIP_NOT_PROVISIONED},
                {SIT_SS_GET_CLIP_STATUS_PROVISIONED,             CLIP_PROVISIONED},
                {SIT_SS_GET_CLIP_STATUS_UNKNOWN,                 CLIP_UNKNOWN},
            }
    },
    {
        SIT_CALL_TB_IDX_SERVICE_CLASS_CF_SET,
        -1,
        -1,
        {
            {SIT_SS_CALL_FARWARD_STATUS_DISABLE,             RIL_SS_MODE_DISABLE},
            {SIT_SS_CALL_FARWARD_STATUS_ENABLE,              RIL_SS_MODE_ENABLE},
            {SIT_SS_CALL_FARWARD_STATUS_INTERROGATE,         RIL_SS_MODE_INTERROGATE},
            {SIT_SS_CALL_FARWARD_STATUS_REGISTRATION,        RIL_SS_MODE_REGISTRATION},
            {SIT_SS_CALL_FARWARD_STATUS_ERASURE,             RIL_SS_MODE_ERASURE},
        }
    },
    {
        SIT_CALL_TB_IDX_SERVICE_CLASS_CF_GET,
        -1,
        -1,
        {
            {SIT_SS_CALL_FARWARD_STATUS_NOT_ACTIVE,          RIL_SS_STATUS_NOT_ACTIVE},
            {SIT_SS_CALL_FARWARD_STATUS_ACTIVE,              RIL_SS_STATUS_ACTIVE},
        }
    },
    {
        SIT_CALL_TB_IDX_SERVICE_STATUS_CALL_WAITING,
        -1,
        -1,
        {
            {SIT_SS_CALL_FARWARD_STATUS_DISABLE,             RIL_SS_MODE_DISABLE},
            {SIT_SS_CALL_FARWARD_STATUS_ENABLE,              RIL_SS_MODE_ENABLE},
        }
    },
    {
        SIT_CALL_TB_IDX_USSD_STATUS,
        -1,
        -1,
        {
            {SIT_SS_USSD_IND_STATUS_USSD_NOTIFY,                       RIL_USSD_NOTIFY},
            {SIT_SS_USSD_IND_STATUS_USSD_REQUEST,                      RIL_USSD_REQUEST},
            {SIT_SS_USSD_IND_STATUS_SESSION_TERMINATED_BY_NETWORK,     RIL_USSD_SESSION_TERMINATED_BY_NET},
            {SIT_SS_USSD_IND_STATUS_OTHER_LOCAL_CLIENT_HAS_RESPONDED,    RIL_USSD_OTHER_LOCAL_CLIENT_HAS_RESPONDED},
            {SIT_SS_USSD_IND_STATUS_OPERATION_NOT_SUPPORTED,           RIL_USSD_OPERATION_NOT_SUPPORT},
            {SIT_SS_USSD_IND_STATUS_NETWORK_TIMEOUT,                   RIL_USSD_NETWORK_TIMEOUT},
        }
    },
    {
        SIT_CALL_TB_IDX_SSNOTI_TYPE,
        -1,
        -1,
        {
            {SIT_SS_SSNOTI_TYPE_MO,    RIL_SSNOTI_TYPE_MO},
            {SIT_SS_SSNOTI_TYPE_MT,    RIL_SSNOTI_TYPE_MT},
        }
    },
};

namespace CallProtocolUtils {

void PrintHexBytes(const char* pTitle, unsigned char* data, unsigned int len)
{
    char szBuffer[256] = {0,};
    char szTemp[4];

    snprintf(szBuffer, sizeof(szBuffer)-1, "(%d) : ", len);

    for  ( unsigned int i = 0; i < len; i++ )
    {
        if ( i > 64 )
        {
            SECURELIB::strncat(szBuffer, sizeof(szBuffer), "...", 3);
            break;
        }
        snprintf(szTemp, sizeof(szTemp)-1, "%02x ", data[i]);
        SECURELIB::strncat(szBuffer, sizeof(szBuffer), szTemp, sizeof(szTemp));

        if ( i %16 == 15 )
        {
            SECURELIB::strncat(szBuffer, sizeof(szBuffer), "\n", 1);
        }
    }
    RilLogV("%s : \n%s", pTitle, szBuffer);
}

int ConvertSitDefineToRilDefine(int TableIndex, int SitValue)
{
    if ( TableIndex >= 0 && TableIndex < SIT_CALL_TB_IDX__MAX
            && SitRilValueTable[TableIndex].Index == TableIndex )
    {

        for ( int i = 0; i < MAX_MATCH_TABLE; i++ )
        {
            if ( SitRilValueTable[TableIndex].match_table[i].SitVal == SitValue )
            {
                return SitRilValueTable[TableIndex].match_table[i].RilVal;
            }
        }
        RilLogW("[%s] cannot find matched ril constant value with sit value(%d),"
                " return default value(%d)", __FUNCTION__, SitValue, SitRilValueTable[TableIndex].DefRilValue);
        return SitRilValueTable[TableIndex].DefRilValue;
    }
    RilLogW("[%s] cannot find matched table with index(%d)", __FUNCTION__, TableIndex);
    return -1;
}

int ConvertRilDefineToSitDefine(int TableIndex, int RilValue)
{
    if ( TableIndex >= 0 && TableIndex < SIT_CALL_TB_IDX__MAX
        && SitRilValueTable[TableIndex].Index == TableIndex )
    {

        for ( int i = 0; i < MAX_MATCH_TABLE; i++ )
        {
            if ( SitRilValueTable[TableIndex].match_table[i].RilVal == RilValue )
            {
                return SitRilValueTable[TableIndex].match_table[i].SitVal;
            }
        }
        RilLogE("[%s] cannot find matched sit constant value with ril value(%d), return default value(%d)", __FUNCTION__, RilValue, SitRilValueTable[TableIndex].DefSitValue);
        return SitRilValueTable[TableIndex].DefSitValue;
    }
    RilLogE("[%s] cannot find matched table with index(%d)", __FUNCTION__, TableIndex);
    return -1;
}

int getProvisionStatus(int otaType, int otaStatus) {
    int provisionStatus = -1;
    if (otaType == SIT_OTA_TYPE_OTASP) {
        switch (otaStatus) {
        case SIT_OTASP_STATUS_OK_SPL_UNLOCKED:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED;
            break;
        case SIT_OTASP_STATUS_OK_AKEYEX:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_A_KEY_EXCHANGED;
            break;
        case SIT_OTASP_STATUS_OK_SSDUPDT:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_SSD_UPDATED;
            break;
        case SIT_OTASP_STATUS_OK_NAMDWNLD:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_NAM_DOWNLOADED;
            break;
        case SIT_OTASP_STATUS_OK_MDNDWNLD:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_MDN_DOWNLOADED;
            break;
        case SIT_OTASP_STATUS_OK_IMSIDWNLD:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED;
            break;
        case SIT_OTASP_STATUS_OK_PRLDWNLD:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_PRL_DOWNLOADED;
            break;
        case SIT_OTASP_STATUS_OK_COMMIT:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_COMMITTED;
            break;
        case SIT_OTASP_STATUS_OK_PROGRAMMING:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_COMMITTED;
            break;
        case SIT_OTASP_STATUS_SUCCESSFUL:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_COMMITTED;
            break;
        case SIT_OTASP_STATUS_UNSUCCESSFUL:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED;
            break;
        case SIT_OTASP_STATUS_OK_OTAPAVERIFY:
            //provisionStatus = -1;
            break;
        case SIT_OTASP_STATUS_PROGRESS:
            //provisionStatus = -1;
            break;
        case SIT_OTASP_STATUS_FAILURES_EXCESS_SPC:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_SPC_RETRIES_EXCEEDED;
            break;
        case SIT_OTASP_STATUS_LOCK_CODE_PASSWORD_SET:
            //provisionStatus = -1;
            break;
        default:
            break;
        }
    } else if (otaType == SIT_OTA_TYPE_OTAPA) {
        switch (otaStatus) {
        case SIT_OTAPA_STATUS_CALL_STOP_MODE:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED;
            break;
        case SIT_OTAPA_STATUS_CALL_START_MODE:
            provisionStatus = CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED;
            break;
        default:
            break;
        }
    }

    return provisionStatus;
}

int getRilLastCallFailCause(int sitLastCallFailCause) {
    switch (sitLastCallFailCause) {
    case SIT_CALL_LAST_CALL_FAIL_UNOBTAINABLE_NUMBER:
        return CALL_FAIL_UNOBTAINABLE_NUMBER;
    case SIT_CALL_LAST_CALL_FAIL_NORMAL:
        return CALL_FAIL_NORMAL;
    case SIT_CALL_LAST_CALL_FAIL_BUSY:
        return CALL_FAIL_BUSY;
    case SIT_CALL_LAST_CALL_FAIL_NORMAL_UNSPECIFIED:
        return CALL_FAIL_NORMAL_UNSPECIFIED;
    case SIT_CALL_LAST_CALL_FAIL_CONGESTION:
        return CALL_FAIL_CONGESTION;
    case SIT_CALL_LAST_CALL_FAIL_ACM_LIMIT_EXCEEDED:
        return CALL_FAIL_ACM_LIMIT_EXCEEDED;
    case SIT_CALL_LAST_CALL_FAIL_CALL_BARRED:
        return CALL_FAIL_CALL_BARRED;
    case SIT_CALL_LAST_CALL_FAIL_FDN_BLOCKED:
        return CALL_FAIL_FDN_BLOCKED;
    case SIT_CALL_LAST_CALL_FAIL_IMSI_UNKNOWN_IN_VLR:
        return CALL_FAIL_IMSI_UNKNOWN_IN_VLR;
    case SIT_CALL_LAST_CALL_FAIL_IMEI_NOT_ACCEPTED:
        return CALL_FAIL_IMEI_NOT_ACCEPTED;

    case SIT_CALL_LAST_CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE:
        return CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_DROP:
        return CALL_FAIL_CDMA_DROP;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_INTERCEPT:
        return CALL_FAIL_CDMA_INTERCEPT;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_REORDER:
        return CALL_FAIL_CDMA_REORDER;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_SO_REJECT:
        return CALL_FAIL_CDMA_SO_REJECT;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_RETRY_ORDER:
        return CALL_FAIL_CDMA_RETRY_ORDER;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_ACCESS_FAILURE:
        return CALL_FAIL_CDMA_ACCESS_FAILURE;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_PREEMPTED:
        return CALL_FAIL_CDMA_PREEMPTED;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_NOT_EMERGENCY:
        return CALL_FAIL_CDMA_NOT_EMERGENCY;
    case SIT_CALL_LAST_CALL_FAIL_CDMA_ACCESS_BLOCKED:
        return CALL_FAIL_CDMA_ACCESS_BLOCKED;

    case SIT_CALL_LAST_CALL_FAIL_ERROR_UNSPECIFIED:
        return CALL_FAIL_ERROR_UNSPECIFIED;
    default:
        RilLogW("[%s] cannot find matched ril constant value with sit value(%d), return SIT value instead", __FUNCTION__, sitLastCallFailCause);
        return sitLastCallFailCause;
    }
}
} // namespace CallProtocolUtils
