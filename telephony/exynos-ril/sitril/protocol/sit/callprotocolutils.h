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
 * callprotocolutils.h
 *
 *  Created on: 2021. 06. 15.
 *      Author: p1.bansal
 */

#ifndef __CALL_PROTOCOL_UTILS_H__
#define __CALL_PROTOCOL_UTILS_H__

#define MAX_MATCH_TABLE 20
#define SIGNAL_INFO_REC_PRESENT 1 //non-zero if signal information record is present
typedef struct
{
    int SitVal;
    int RilVal;
}sit_ril_match_item;

typedef struct
{
    int Index;
    int DefSitValue;
    int DefRilValue;
    sit_ril_match_item match_table[MAX_MATCH_TABLE];
}sit_ril_match_table;

typedef enum
{
    SIT_CALL_TB_IDX_STATE,
    SIT_CALL_TB_IDX_PRESENTATION,
    SIT_CALL_TB_IDX_UUS_TYPE,
    SIT_CALL_TB_IDX_UUS_DCS,
    SIT_CALL_TB_IDX_TYPE,
    SIT_CALL_TB_IDX_CLIR,
    //SIT_CALL_TB_IDX_RELEASECAUSE,
    SIT_CALL_TB_IDX_GET_CLIP_STATE,
    SIT_CALL_TB_IDX_SERVICE_CLASS_CF_SET,
    SIT_CALL_TB_IDX_SERVICE_CLASS_CF_GET,
    SIT_CALL_TB_IDX_SERVICE_STATUS_CALL_WAITING,
    SIT_CALL_TB_IDX_USSD_STATUS,
    SIT_CALL_TB_IDX_SSNOTI_TYPE,

    SIT_CALL_TB_IDX__MAX
}sit_call_tb_idx_e_type;

namespace CallProtocolUtils {
void PrintHexBytes(const char* pTitle, unsigned char* data, unsigned int len);
int ConvertSitDefineToRilDefine(int TableIndex, int SitValue);
int ConvertRilDefineToSitDefine(int TableIndex, int RilValue);
int getProvisionStatus(int otaType, int otaStatus);
int getRilLastCallFailCause(int sitLastCallFailCause);
} // namespace CallProtocolUtils

#endif /* __CALL_PROTOCOL_UTILS_H__ */
