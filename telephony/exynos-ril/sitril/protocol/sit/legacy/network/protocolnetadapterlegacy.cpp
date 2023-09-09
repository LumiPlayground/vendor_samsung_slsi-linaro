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
 * protocolnetadapterlegacy.cpp
 *
 *  Created on: 2014. 6. 27.
 *      Author: sungwoo48.choi
 */
#include "mcctable.h"
#include "protocolnetadapterlegacy.h"
#include "rillog.h"
#include "modemdata.h"

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NET, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

static bool debug = true;

/**
 * ProtocolNetVoiceRegStateAdapterLegacy
 */
int ProtocolNetVoiceRegStateAdapterLegacy::GetRegState() const
{
    int regStatus = UNKNOWN;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            regStatus = NetProtocolUtils::ConvertSitRegState2Ril(data->reg_state);
        }
    }

    return regStatus;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetRejectCause() const
{
    int rejCause = SIT_NET_REJ_CAUSE_GENERAL;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            rejCause = data->rej_cause;
        }
    }

    return rejCause;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetRadioTech() const
{
    int rat = (int)RADIO_TECH_UNKNOWN;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            rat = NetProtocolUtils::ConvertSitRat2RilRat(data->rat);
        }
    }

    return rat;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetLAC() const
{
    int lac = 0xFFFF;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            lac = data->gw_lac & 0xFFFF;
        }
    }

    return lac;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetCellId() const
{
    int cid = 0xFFFFFFFF;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            cid = data->gw_cid;
        }
    }

    return cid;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetPSC() const
{
    int psc = 0xFF;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            psc = (int)(data->gw_psc & 0xFF);
        }
    }

    return psc;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetTAC() const
{
    int tac = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_CS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_cs_reg_state_v1_1)) {
            sit_cs_reg_state_v1_1 *data = (sit_cs_reg_state_v1_1 *)GetParameter();
            if (data->rat == SIT_RAT_TYPE_5G) {
                tac = data->nr_tac;
            } else {
                tac = data->lte_tac;
            }
        } else {
            sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
            if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
                tac = data->lte_tac;
            }
        }
    }

    return tac;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetPCID() const
{
    int pcid = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_CS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_cs_reg_state_v1_1)) {
            sit_cs_reg_state_v1_1 *data = (sit_cs_reg_state_v1_1 *)GetParameter();
            if (data->rat == SIT_RAT_TYPE_5G) {
                pcid = data->nr_pcid;
            } else {
                pcid = data->lte_pcid;
            }
        } else {
            sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
            if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
                pcid = data->lte_pcid;
            }
        }
    }

    return pcid;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetECI() const
{
    int eci = 0xFFFFFFFF;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            eci = data->lte_eci;
        }
    }

    return eci;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetStationId() const
{
    int stationId = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            stationId = data->basestationid;
        }
    }
    return stationId;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetStationLat() const
{
    int stationLat = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            stationLat = data->basestation_latitude;
        }
    }
    return stationLat;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetStationLong() const
{
    int stationLong = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            stationLong = data->basestation_longitude;
        }
    }
    return stationLong;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetConCurrent() const
{
    int conCur = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            conCur = data->concurrent;
        }
    }
    return conCur;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetSystemId() const
{
    int systemId = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            systemId = data->sid;
        }
    }
    return systemId;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetNetworkId() const
{
    int networkId = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            networkId = data->nid;
        }
    }
    return networkId;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetRoamingInd() const
{
    int roamingInd = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            roamingInd = data->roamingindicator;
        }
    }
    return roamingInd;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetRegPrl() const
{
    int regPrl = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            regPrl = data->registered_prl;
        }
    }
    return regPrl;
}

int ProtocolNetVoiceRegStateAdapterLegacy::GetRoamingIndPrl() const
{
    int roamingPrl = 0;
    if (m_pModemData != NULL) {
        sit_net_get_cs_reg_state_rsp *data = (sit_net_get_cs_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CS_REG_STATE) {
            roamingPrl = data->roaming_indi_prl;
        }
    }
    return roamingPrl;
}

int ProtocolNetVoiceRegStateAdapterLegacy::getChannelNumber() const {
    int channelNumber = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_CS_REG_STATE && GetParameter() != NULL) {
            if (GetParameterLength() >= sizeof(sit_cs_reg_state_v1_1)) {
                sit_cs_reg_state_v1_1 *data = (sit_cs_reg_state_v1_1 *)GetParameter();
                if (data->rat == SIT_RAT_TYPE_5G) {
                    channelNumber = data->nr_arfcn;
                } else {
                    channelNumber = data->channel;
                }
            } else if (GetParameterLength() >= sizeof(sit_cs_reg_state_v1_0)) {
                sit_cs_reg_state_v1_0 *data = (sit_cs_reg_state_v1_0 *)GetParameter();
                channelNumber = data->channel;
            }
        }
    }
    return channelNumber;
}

long int ProtocolNetVoiceRegStateAdapterLegacy::getNrCid() const {
    long int cid = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_CS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_cs_reg_state_v1_1)) {
            sit_cs_reg_state_v1_1 *data = (sit_cs_reg_state_v1_1 *)GetParameter();
            cid = data->nr_cid;
        }
    }
    return cid;
}

/**
 * ProtocolNetDataRegStateAdapterLegacy
 */
int ProtocolNetDataRegStateAdapterLegacy::GetRegState() const
{
    int regStatus = NOT_REGISTERED;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            regStatus = NetProtocolUtils::ConvertSitRegState2Ril(data->reg_state);
        }
    }
    return regStatus;
}

int ProtocolNetDataRegStateAdapterLegacy::GetRejectCause() const
{
    int regCause = SIT_NET_REJ_CAUSE_GENERAL;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            regCause = data->rej_cause;
        }
    }
    return regCause;
}

int ProtocolNetDataRegStateAdapterLegacy::GetMaxSDC() const
{
    int sdc = 4;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            sdc = data->max_sdc;
        }
    }
    return sdc;
}

int ProtocolNetDataRegStateAdapterLegacy::GetRadioTech() const
{
    int rat = (int)RADIO_TECH_UNKNOWN;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            rat = NetProtocolUtils::ConvertSitRat2RilRat(data->rat);
        }
    }
    return rat;
}

int ProtocolNetDataRegStateAdapterLegacy::GetLAC() const
{
    int lac = 0;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            lac = data->gw_lac & 0xFFFF;
        }
    }

    return lac;
}

int ProtocolNetDataRegStateAdapterLegacy::GetCellId() const
{
    int cid = 0;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            cid = data->gw_cid;
        }
    }
    return cid;
}

int ProtocolNetDataRegStateAdapterLegacy::GetPSC() const
{
    int psc = 0xFF;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            psc = (int)(data->gw_psc & 0xFF);
        }
    }

    return psc;
}

int ProtocolNetDataRegStateAdapterLegacy::GetTAC() const
{
    int tac = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_2)) {
            sit_ps_reg_state_v1_2 *data = (sit_ps_reg_state_v1_2 *)GetParameter();
            if (data->rat == SIT_RAT_TYPE_5G) {
                tac = data->nr_tac;
            } else {
                tac = data->lte_tac;
            }
        } else {
            sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
            if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
                tac = data->lte_tac;
            }
        }
    }

    return tac;
}

int ProtocolNetDataRegStateAdapterLegacy::GetPCID() const
{
    int pcid = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_2)) {
            sit_ps_reg_state_v1_2 *data = (sit_ps_reg_state_v1_2 *)GetParameter();
            if (data->rat == SIT_RAT_TYPE_5G) {
                pcid = data->nr_pcid;
            } else {
                pcid = data->lte_pcid;
            }
        } else {
            sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
            if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
                pcid = data->lte_pcid;
            }
        }
    }

    return pcid;
}

int ProtocolNetDataRegStateAdapterLegacy::GetECI() const
{
    int eci = 0xFFFFFFFF;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            eci = data->lte_eci;
        }
    }

    return eci;
}

int ProtocolNetDataRegStateAdapterLegacy::GetCSGID() const
{
    int csgid = 0xFFFFFFFF;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            csgid = data->lte_csgid;
        }
    }

    return csgid;
}

int ProtocolNetDataRegStateAdapterLegacy::GetTADV() const
{
    int tadv = 0xFFFFFFFF;
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            tadv = data->lte_tadv;
        }
    }

    return tadv;
}

int ProtocolNetDataRegStateAdapterLegacy::GetImsVops() const
{
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            return (int)(data->ims_vops & 0xFF);
        }
    }
    return VOPS_NOT_SUPPORTED;
}

int ProtocolNetDataRegStateAdapterLegacy::GetEmcService() const
{
    if (m_pModemData != NULL) {
        sit_net_get_ps_reg_state_rsp *data = (sit_net_get_ps_reg_state_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_REG_STATE) {
            return (int)(data->emc_service & 0xFF);
        }
    }
    return EMC_NOT_SUPPORTED;
}

int ProtocolNetDataRegStateAdapterLegacy::getChannelNumber() const {
    int channelNumber = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE && GetParameter() != NULL) {
            if (GetParameterLength() >= sizeof(sit_ps_reg_state_v1_2)) {
                sit_ps_reg_state_v1_2 *data = (sit_ps_reg_state_v1_2 *)GetParameter();
                if (data->rat == SIT_RAT_TYPE_5G) {
                    channelNumber = data->nr_arfcn;
                } else {
                    channelNumber = data->channel;
                }
            } else if (GetParameterLength() >= sizeof(sit_ps_reg_state_v1_0)) {
                sit_ps_reg_state_v1_0 *data = (sit_ps_reg_state_v1_0 *)GetParameter();
                channelNumber = data->channel;
            }
        }
    }
    return channelNumber;
}

bool ProtocolNetDataRegStateAdapterLegacy::IsEndcAvailable() const {
    bool ret = false;    // E-UTRA-NR Dual Connectivity (EN-DC) is not supported. (not available)
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_1)) {
            sit_ps_reg_state_v1_1 *data = (sit_ps_reg_state_v1_1 *)GetParameter();
            return (data->endc == SERVICE_AVAILABLE);
        }
    }
    return ret;
}

bool ProtocolNetDataRegStateAdapterLegacy::IsDcNrRestricted() const {
    int ret = false;    // use of dual connectivity with NR is restricted.
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_1)) {
            sit_ps_reg_state_v1_1 *data = (sit_ps_reg_state_v1_1 *)GetParameter();
            return (data->dcnr_restricted == SERVICE_AVAILABLE);
        }
    }
    return ret;
}

bool ProtocolNetDataRegStateAdapterLegacy::IsNrAvailable() const {
    int ret = false;    // NR is not available
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_1)) {
            sit_ps_reg_state_v1_1 *data = (sit_ps_reg_state_v1_1 *)GetParameter();
            return (data->nr_available == SERVICE_AVAILABLE);
        }
    }
    return ret;
}

long int ProtocolNetDataRegStateAdapterLegacy::getNrCid() const {
    long int cid = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_2)) {
            sit_ps_reg_state_v1_2 *data = (sit_ps_reg_state_v1_2 *)GetParameter();
            cid = data->nr_cid;
        }
    }
    return cid;
}

int ProtocolNetDataRegStateAdapterLegacy::GetEmf() const
{
    int emf = 0;
    if (m_pModemData != NULL) {
        if (GetId() == SIT_GET_PS_REG_STATE &&
                GetParameter() != NULL &&
                GetParameterLength() >= sizeof(sit_ps_reg_state_v1_3)) {
            sit_ps_reg_state_v1_3 *data = (sit_ps_reg_state_v1_3 *)GetParameter();
            return data->emf;
        }
    }
    return emf;
}

/**
 * ProtocolNetOperatorAdapterLegacy
 */
void ProtocolNetOperatorAdapterLegacy::Init()
{
    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_net_get_operator_rsp *data = (sit_net_get_operator_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_OPERATOR) {
            if (*data->plmn != 0) {
                memcpy(m_szPlmn, data->plmn, 6);
                if (m_szPlmn[5] == '#') {
                    m_szPlmn[5] = 0;
                }
                if (m_szPlmn[0] == '#') {
                    m_szPlmn[0] = 0;
                }
            }

            if (*data->short_name != 0) {
                strncpy(m_szShortPlmn, data->short_name, MAX_SHORT_NAME_LEN);
            }

            if (*data->long_name != 0) {
                strncpy(m_szLongPlmn, data->long_name, MAX_FULL_NAME_LEN);
            }

            if(GetParameter() != NULL && GetParameterLength() >= sizeof(sit_net_operator_v1_0)) {
                sit_net_operator_v1_0 *param = (sit_net_operator_v1_0 *)GetParameter();
                m_regState = param->reg_state;
            }

            if(GetParameter() != NULL && GetParameterLength() >= sizeof(sit_net_operator_v1_1)) {
                sit_net_operator_v1_1 *param = (sit_net_operator_v1_1 *)GetParameter();
                m_lac = param->lac;
            }
        }
    }
}

ProtocolNetOperatorAdapterLegacy::ProtocolNetOperatorAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
    memset(m_szPlmn, 0, sizeof(m_szPlmn));
    memset(m_szShortPlmn, 0, sizeof(m_szShortPlmn));
    memset(m_szLongPlmn, 0, sizeof(m_szLongPlmn));
    m_regState = OPERATOR_REG_UNKNOWN;
    m_lac = -1;

    Init();
}

const char *ProtocolNetOperatorAdapterLegacy::GetPlmn() const
{
    return (m_szPlmn[0] == 0 ? NULL : m_szPlmn);
}

const char *ProtocolNetOperatorAdapterLegacy::GetShortPlmn() const
{
    return (m_szShortPlmn[0] == 0 ? NULL : m_szShortPlmn);
}

const char *ProtocolNetOperatorAdapterLegacy::GetLongPlmn() const
{
    return (m_szLongPlmn[0] == 0 ? NULL : m_szLongPlmn);
}

int ProtocolNetOperatorAdapterLegacy::GetRegState() const
{
    return m_regState;
}

int ProtocolNetOperatorAdapterLegacy::GetLac() const
{
    return m_lac;
}

/**
 * ProtocolNetSelModeAdapterLegacy
 */
int ProtocolNetSelModeAdapterLegacy::GetNetworkSelectionMode() const
{
    if (m_pModemData != NULL) {
        sit_net_get_network_mode_rsp *data = (sit_net_get_network_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_NTW_MODE) {
            int mode = data->network_mode & 0xFF;
            RilLogV("NetworkSelectionMode=%s(0x%02x)", mode == 0 ? "Automatic" : "Manual", mode);
            return mode;
        }
    }
    return SIT_NET_NETWORK_MODE_AUTOMATIC;
}

/**
 * ProtocolGetRadioStateRespAdapterLegacy
 */
int ProtocolGetRadioStateRespAdapterLegacy::GetRadioState() const {
    if (m_pModemData != NULL) {
        sit_pwr_get_radio_power_rsp *data = (sit_pwr_get_radio_power_rsp *) m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_RADIO_POWER) {
            RilLogV("RadioState=%s(0x%02x)", NetProtocolUtils::ConvertPwrRadioSimState2String(data->radio_state), data->radio_state);
            return ConvertRilRadioState(data->radio_state);
        }
    }
    return RADIO_STATE_UNAVAILABLE;
}

int ProtocolGetRadioStateRespAdapterLegacy::ConvertRilRadioState(int radioState) const {
    if (radioState == SIT_PWR_RADIO_SIM_STATE_OFF) {
        return RADIO_STATE_OFF;
    }
    else if (radioState >= SIT_PWR_RADIO_SIM_STATE_SIM_NOT_READY && radioState <= SIT_PWR_RADIO_SIM_STATE_ON) {
        return RADIO_STATE_ON;
    }
    return RADIO_STATE_UNAVAILABLE;
}


/**
 * ProtocolRadioPowerAdapterLegacy
 */
UINT ProtocolRadioPowerAdapterLegacy::GetErrorCode() const
{
    return ProtocolRespAdapter::GetErrorCode();
}

/**
 * ProtocolRadioStateAdapterLegacy
 */
int ProtocolRadioStateAdapterLegacy::GetRadioState() const
{
    if (m_pModemData != NULL) {
        sit_pwr_radio_state_changed_ind *data = (sit_pwr_radio_state_changed_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_STATE_CHANGED) {
            RilLogV("RadioState=%s(0x%02x)", NetProtocolUtils::ConvertRadioState2String(data->radio_state), data->radio_state);
            return ConvertRilRadioState(data->radio_state);
        }
    }
    return RADIO_STATE_UNAVAILABLE;
}

int ProtocolRadioStateAdapterLegacy::ConvertRilRadioState(int radioState) const {
    switch (radioState) {
    case SIT_PWR_RADIO_STATE_INITIALIZED:
    case SIT_PWR_RADIO_STATE_STOP_NETWORK:
        return RADIO_STATE_OFF;
    case SIT_PWR_RADIO_STATE_START_NETWORK:
        return RADIO_STATE_ON;
    case SIT_PWR_RADIO_STATE_POWER_OFF:
        return RADIO_STATE_UNAVAILABLE;
    default:
        return RADIO_STATE_UNAVAILABLE;
    }
}

/**
 * ProtocolNetPrefNetTypeAdapterLegacy
 */
int ProtocolNetPrefNetTypeAdapterLegacy::GetPreferredNetworkType() const
{
    if (m_pModemData != NULL) {
        sit_net_get_pref_network_rsp *data = (sit_net_get_pref_network_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PREFERRED_NTW_TYPE) {
            RilLogV("PreferredNetworkType=%s(0x%02x)", NetProtocolUtils::ConvertSitPreferredNetType2String(data->pref_net_type), data->pref_net_type);
            int prefNetType = NetProtocolUtils::ConvertPrefNetworkType2Ril(data->pref_net_type);
            return prefNetType;
        }
    }
    return PREF_NET_TYPE_GSM_WCDMA;
}

/**
 * ProtocolNetBandModeAdapterLegacy
 */
ProtocolNetBandModeAdapterLegacy::ProtocolNetBandModeAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), m_count(0)
{
    memset(m_bandMode, 0, sizeof(m_bandMode));
    Init();
}

void ProtocolNetBandModeAdapterLegacy::Init()
{
    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_net_get_band_mode_rsp *data = (sit_net_get_band_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BAND_MODE) {
            m_count = MIN(MIN(data->band_info_num, MAX_NET_INFO_COUNT), SIT_NET_BAND_MAX);
            for (int i = 0; i < m_count; i++) {
                m_bandMode[i] = data->band[i];
            } // end for i ~
        }
    }
}

/**
 * ProtocolNetAvailableNetworkAdapterLegacy
 */
ProtocolNetAvailableNetworkAdapterLegacy::ProtocolNetAvailableNetworkAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
}

ProtocolNetAvailableNetworkAdapterLegacy::~ProtocolNetAvailableNetworkAdapterLegacy()
{
}

int ProtocolNetAvailableNetworkAdapterLegacy::GetCount()
{
    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_net_get_available_networks_rsp *data = (sit_net_get_available_networks_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_AVAILABLE_NETWORKS) {
            return data->network_info_num;
        }
    }
    return 0;
}

bool ProtocolNetAvailableNetworkAdapterLegacy::GetNetwork(NetworkInfo &nwkInfo, int index, const char * /* simPlmn */, char * /* simSpn */)
{
    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_net_get_available_networks_rsp *data = (sit_net_get_available_networks_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_AVAILABLE_NETWORKS) {
            int count = data->network_info_num;
            if (index < 0 || index >= count) {
                return false;
            }

            sit_net_network_info_item *p = &data->network_info[index];
            if (p == NULL) {
                return false;
            }

            // convert SIT RAT to Android RAT
            nwkInfo.rat = NetProtocolUtils::ConvertSitRat2RilRat(p->rat);
            if (nwkInfo.rat == RADIO_TECH_IWLAN) nwkInfo.rat = RADIO_TECH_UNKNOWN;
            if (nwkInfo.rat == RADIO_TECH_LTE_CA) nwkInfo.rat = RADIO_TECH_LTE;

            // copy mcc/mnc
            memset(nwkInfo.plmn, 0, sizeof(nwkInfo.plmn));
            memcpy(nwkInfo.plmn, p->plmn, 6);

            if (nwkInfo.plmn[5] == '#' /*|| mncLength == 2*/) {
                nwkInfo.plmn[5] = 0;
            }

            // check UNKNOW network
            if(MccTable::isUnknowNetwork(nwkInfo.plmn) > 0) return false;

            nwkInfo.status = NetProtocolUtils::ConvertPlmnStatus2Ril(p->plmn_status);

            // fill Long/Short EONS
            *nwkInfo.shortPlmn = 0;
            *nwkInfo.longPlmn = 0;
        }
    }

    return true;
}

ProtocolNetGetPsServiceAdapterLegacy::ProtocolNetGetPsServiceAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
}

int ProtocolNetGetPsServiceAdapterLegacy::GetState()
{
    if (m_pModemData != NULL && GetErrorCode() == RCM_E_SUCCESS) {
        sit_net_get_ps_service_rsp *data = (sit_net_get_ps_service_rsp *) m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_PS_SERVICE) {
            return (data->state == 1 ? ALLOW_DATA_CALL : DISALLOW_DATA_CALL);
        }
    }
    return ALLOW_DATA_CALL;
}

/**
 * ProtocolNetDuplexModeRespAdapterLegacy
 */
int ProtocolNetDuplexModeRespAdapterLegacy::Get4gDuplexMode() const
{
    sit_net_get_duplex_mode_rsp *data = (sit_net_get_duplex_mode_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_GET_DUPLEX_MODE) {
        return data->duplex_mode_4g;
    }
    return SIT_NET_DUPLEX_MODE_MAX;
}

int ProtocolNetDuplexModeRespAdapterLegacy::Get3gDuplexMode() const
{
    sit_net_get_duplex_mode_rsp *data = (sit_net_get_duplex_mode_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_GET_DUPLEX_MODE) {
        return data->duplex_mode_3g;
    }
    return SIT_NET_DUPLEX_MODE_MAX;
}

int ProtocolNetDuplexModeRespAdapterLegacy::GetDuplexMode() const
{
    int mode = DUPLEX_MODE_INVALID;
    int duplex_mode_4g = Get4gDuplexMode();
    int duplex_mode_3g = Get3gDuplexMode();

    if(duplex_mode_4g == SIT_NET_DUPLEX_MODE_TDD && duplex_mode_3g == SIT_NET_DUPLEX_MODE_TDD) {
        mode = DUPLEX_MODE_LTG;
    }
    else if (duplex_mode_4g == SIT_NET_DUPLEX_MODE_FDD_TDD && duplex_mode_3g == SIT_NET_DUPLEX_MODE_FDD) {
        mode = DUPLEX_MODE_LWG;
    }
    else if (duplex_mode_4g == SIT_NET_DUPLEX_MODE_FDD_TDD && duplex_mode_3g == SIT_NET_DUPLEX_MODE_FDD_TDD) {
        mode = DUPLEX_MODE_GLOBAL;
    }
    return mode;
}

/**
 * ProtocolNetEmergencyActInfoAdapterLegacy
 */
ProtocolNetEmergencyActInfoAdapterLegacy::ProtocolNetEmergencyActInfoAdapterLegacy(const ModemData *pModemData) : ProtocolIndAdapter(pModemData)
{
}

int ProtocolNetEmergencyActInfoAdapterLegacy::GetRat() const
{
    sit_net_emergency_act_info_ind *data = (sit_net_emergency_act_info_ind *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_IND_EMERGENCY_ACT_INFO) {
        int rat = NetProtocolUtils::ConvertSitRat2RilRat(data->rat);
        // consider as RADIO_TECH_LTE
        if (rat == RADIO_TECH_LTE_CA) rat = RADIO_TECH_LTE;
        return rat;
    }
    return RADIO_TECH_UNKNOWN;
}

int ProtocolNetEmergencyActInfoAdapterLegacy::GetActStatus() const
{
    sit_net_emergency_act_info_ind *data = (sit_net_emergency_act_info_ind *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_IND_EMERGENCY_ACT_INFO) {
        if ((int)data->act_status == SIT_NET_CURRENT_ACT_EMERGENCY_CALL) {
            return EMERGENCY_CALL_AVAILABLE;
        } else if ((int)data->act_status == SIT_NET_RETRY_ACT_EMERGENCY_CALL) {
            return EMERGENCY_CALL_RETRY;
        }
    }
    return EMERGENCY_CALL_NOT_AVAILABLE;
}

/**
 * ProtocolNetMcSrchRespAdapterLegacy
 */
ProtocolNetMcSrchRespAdapterLegacy::ProtocolNetMcSrchRespAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
    memset(m_szPlmn, 0, sizeof(m_szPlmn));

    sit_net_set_micro_cell_search_rsp *data = (sit_net_set_micro_cell_search_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_SET_MICRO_CELL_SEARCH) {
        if (*data->plmn != 0) {
            memcpy(m_szPlmn, data->plmn, MAX_PLMN_LEN);
            if (m_szPlmn[5] == '#') {
                m_szPlmn[5] = 0;
            }
        }
    }
}

int ProtocolNetMcSrchRespAdapterLegacy::GetMcSrchResult() const
{
    sit_net_set_micro_cell_search_rsp *data = (sit_net_set_micro_cell_search_rsp *)m_pModemData->GetRawData();
    if (data != NULL && data->hdr.id == SIT_SET_MICRO_CELL_SEARCH) {
        return data->srch_result;
    }
    return -1;
}

const char *ProtocolNetMcSrchRespAdapterLegacy::GetMcSrchPlmn() const
{
    return (m_szPlmn[0] == 0 ? NULL : m_szPlmn);
}

/**
 * ProtocolGetNetworkRCRespAdapterLegacy
 */
int ProtocolGetNetworkRCRespAdapterLegacy::GetVersion() const
{
    int ret = RIL_RADIO_CAPABILITY_VERSION;
    if (m_pModemData != NULL) {
        sit_net_get_rc_rsp *data = (sit_net_get_rc_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_RADIO_CAPABILITY) {
            ret = RIL_RADIO_CAPABILITY_VERSION;
        }
    }
    return ret;
}

int ProtocolGetNetworkRCRespAdapterLegacy::GetSession() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_net_get_rc_rsp *data = (sit_net_get_rc_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_RADIO_CAPABILITY) {
            ret = 0;
        }
    }
    return ret;
}

int ProtocolGetNetworkRCRespAdapterLegacy::GetPhase() const
{
    int ret = RC_PHASE_CONFIGURED;
    if (m_pModemData != NULL) {
        sit_net_get_rc_rsp *data = (sit_net_get_rc_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_RADIO_CAPABILITY) {
            ret = RC_PHASE_CONFIGURED;
        }
    }
    return ret;
}

int ProtocolGetNetworkRCRespAdapterLegacy::GetRafType() const
{
    int ret = RAF_CP_UNKNOWN;
    if (m_pModemData != NULL) {
        sit_net_get_rc_rsp *data = (sit_net_get_rc_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_RADIO_CAPABILITY) {
            ret = NetProtocolUtils::ConvertSitRaf2Ril(data->rc_raf, true);
        }
    }
    return ret;
}

BYTE *ProtocolGetNetworkRCRespAdapterLegacy::GetUuid() const
{
    BYTE *pRet = NULL;
    return pRet;
}

int ProtocolGetNetworkRCRespAdapterLegacy::GetStatus() const
{
    int ret = RC_STATUS_NONE;
    return ret;
}

/**
 * ProtocolNetworkRCIndAdapterLegacy
 */
int ProtocolNetworkRCIndAdapterLegacy::GetVersion() const
{
    int ret = RIL_RADIO_CAPABILITY_VERSION;
    if (m_pModemData != NULL) {
        sit_net_radio_capability_ind *data = (sit_net_radio_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_CAPABILITY) {
            ret = data->version;
        }
    }
    return ret;
}

int ProtocolNetworkRCIndAdapterLegacy::GetSession() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_radio_capability_ind *data = (sit_net_radio_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_CAPABILITY) {
            ret = data->session_id;
        }
    }
    return ret;
}

int ProtocolNetworkRCIndAdapterLegacy::GetPhase() const
{
    int ret = RC_PHASE_CONFIGURED;
    if (m_pModemData != NULL) {
        sit_net_radio_capability_ind *data = (sit_net_radio_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_CAPABILITY) {
            ret = data->phase;
        }
    }
    return ret;
}

int ProtocolNetworkRCIndAdapterLegacy::GetRafType() const
{
    int ret = RAF_CP_UNKNOWN;
    if (m_pModemData != NULL) {
        sit_net_radio_capability_ind *data = (sit_net_radio_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_CAPABILITY) {
            ret = NetProtocolUtils::ConvertSitRaf2Ril(data->rc_raf, true);
        }
    }
    return ret;
}

BYTE *ProtocolNetworkRCIndAdapterLegacy::GetUuid() const
{
    BYTE *pRet = NULL;
    if (m_pModemData != NULL) {
        sit_net_radio_capability_ind *data = (sit_net_radio_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_CAPABILITY) {
            pRet = data->uuid;
        }
    }
    return pRet;
}

int ProtocolNetworkRCIndAdapterLegacy::GetStatus() const
{
    int ret = RC_STATUS_NONE;
    if (m_pModemData != NULL) {
        sit_net_radio_capability_ind *data = (sit_net_radio_capability_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RADIO_CAPABILITY) {
            ret = data->status;
        }
    }
    return ret;
}

/**
 * ProtocolNetworkSgcBearerAllocIndAdapterLegacy
 */
ProtocolNetworkSgcBearerAllocIndAdapterLegacy::ProtocolNetworkSgcBearerAllocIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    mRat = RADIO_TECH_UNKNOWN;
    mConnectionStatus = NONE;

    if (m_pModemData != NULL) {
        sit_net_sgc_bearer_allocation_ind *data = (sit_net_sgc_bearer_allocation_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SCG_BEARER_ALLOCATION) {
            mRat = NetProtocolUtils::ConvertSitRat2RilRat((int)data->rat);
            mConnectionStatus = (data->scg_status == SIT_SCG_ADDED) ? SECONDARY_SERVING : PRIMARY_SERVING;
            if (mConnectionStatus == SECONDARY_SERVING) {
                mRat = RADIO_TECH_NR;
            }
        }
    }
}

/*
 * ProtocolNetCdmaQueryRoamingTypeAdapterLegacy
 */
int ProtocolNetCdmaQueryRoamingTypeAdapterLegacy::QueryRoamingType() const
{
    if (m_pModemData != NULL) {
        sit_net_query_cdma_roaming_rsp *data = (sit_net_query_cdma_roaming_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CDMA_ROAMING_PREFERENCE) {
            RilLogV("Cdma Query RoamingType=%d", data->cdma_roaming_type);
            return NetProtocolUtils::ConvertSitCdmaRoamingType2Ril(data->cdma_roaming_type);
        }
    }
    return CDMA_ROAMING_HOME_ONLY;
}

/*
 * ProtocolNetCdmaHybridModeAdapterLegacy
 */
int ProtocolNetCdmaHybridModeAdapterLegacy::GetCdmaHybridMode() const
{
    if (m_pModemData != NULL) {
        sit_net_get_cdma_hybrid_mode_rsp *data = (sit_net_get_cdma_hybrid_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_CDMA_HYBRID_MODE) {
            RilLogV("Get CdmaHybridMode=%s(0x%02x)", NetProtocolUtils::ConvertSitCdmaHybridMode2String(data->hybrid_mode), data->hybrid_mode);
            return data->hybrid_mode;
        }
    }
    RilLogV("Get default CdmaHybridMode=%s(0x%02x)", NetProtocolUtils::ConvertSitCdmaHybridMode2String(HYBRID_MODE_1X_HRPD), HYBRID_MODE_1X_HRPD);
    return HYBRID_MODE_1X_HRPD;
}

/*
 * ProtocolNetMccAdapterLegacy
 */
ProtocolNetMccAdapterLegacy::ProtocolNetMccAdapterLegacy(const ModemData *pModemData) : ProtocolIndAdapter (pModemData) {
    memset(mMcc, 0, sizeof(mMcc));
    if (m_pModemData != NULL) {
        sit_net_mcc_ind *data = (sit_net_mcc_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_MCC) {
            strncpy(mMcc, data->mcc, 3);
        }
    }
}

int ProtocolNetMccAdapterLegacy::GetCurrentPrefNetworkMode() const
{
    if (m_pModemData != NULL) {
        sit_net_mcc_ind *data = (sit_net_mcc_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_MCC) {
            RilLogV("Get Pref.NetMode in Mcc=%s(0x%02x)", NetProtocolUtils::ConvertSitPreferredNetType2String(data->pref_net_type), data->pref_net_type);
            return (int)(data->pref_net_type);
        }
    }
    return SIT_NET_PREF_NET_TYPE_GSM_WCDMA;
}

struct ProtocolCellIdentityParserV3 {
public:
    size_t Parse(RIL_CellIdentity_V1_5& cellIdentity, void *data) {
        size_t ret = 0;
        if (data != NULL) {
            char *pcid = (char *)data;
            int type = (int)*(pcid++);
            ret = 1;
            switch (type) {
            case SIT_NET_CELL_INFO_TYPE_GSM:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_GSM;
                CellInfoProtocolUtils::Legacy::FillCellIdentityGsm(cellIdentity.gsm, *((cell_identity_gsm_v3 *)pcid));
                ret += sizeof(cell_identity_gsm_v3);
                break;
            case SIT_NET_CELL_INFO_TYPE_CDMA:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_CDMA;
                CellInfoProtocolUtils::Legacy::FillCellIdentityCdma(cellIdentity.cdma, *((cell_identity_cdma_v3 *)pcid));
                ret += sizeof(cell_identity_cdma_v3);
                break;
            case SIT_NET_CELL_INFO_TYPE_LTE:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_LTE;
                CellInfoProtocolUtils::Legacy::FillCellIdentityLte(cellIdentity.lte, *((cell_identity_lte_v3 *)pcid));
                ret += sizeof(cell_identity_lte_v3);
                break;
            case SIT_NET_CELL_INFO_TYPE_WCDMA:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_WCDMA;
                CellInfoProtocolUtils::Legacy::FillCellIdentityWcdma(cellIdentity.wcdma, *((cell_identity_wcdma_v3 *)pcid));
                ret += sizeof(cell_identity_wcdma_v3);
                break;
            case SIT_NET_CELL_INFO_TYPE_TDSCDMA:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_TD_SCDMA;
                CellInfoProtocolUtils::Legacy::FillCellIdentityTdscdma(cellIdentity.tdscdma, *((cell_identity_tdscdma_v3 *)pcid));
                ret += sizeof(cell_identity_tdscdma_v3);
                break;
            case SIT_NET_CELL_INFO_TYPE_NR:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_NR;
                CellInfoProtocolUtils::Legacy::FillCellIdentityNr(cellIdentity.nr, *((cell_identity_nr_v3 *)pcid));
                ret += sizeof(cell_identity_nr_v3);
                break;
            default:
                RilLogE("ProtocolCellIdentityParserV3: unknown cellInfoType=%d", type);
                cellIdentity = {};
                ret = 0;
                break;
            }
        }
        return ret;
    }
};

struct ProtocolCellIdentityParserV4 {
public:
    size_t Parse(RIL_CellIdentity_V1_5& cellIdentity, void *data) {
        size_t ret = 0;
        if (data != NULL) {
            char *pcid = (char *)data;
            int type = (int)*(pcid++);
            ret = 1;
            switch (type) {
            case SIT_NET_CELL_INFO_TYPE_GSM:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_GSM;
                CellInfoProtocolUtils::Legacy::FillCellIdentityGsm(cellIdentity.gsm, *((cell_identity_gsm_v4 *)pcid));
                ret += sizeof(cell_identity_gsm_v4);
                break;
            case SIT_NET_CELL_INFO_TYPE_CDMA:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_CDMA;
                CellInfoProtocolUtils::Legacy::FillCellIdentityCdma(cellIdentity.cdma, *((cell_identity_cdma_v4 *)pcid));
                ret += sizeof(cell_identity_cdma_v4);
                break;
            case SIT_NET_CELL_INFO_TYPE_LTE:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_LTE;
                CellInfoProtocolUtils::Legacy::FillCellIdentityLte(cellIdentity.lte, *((cell_identity_lte_v4 *)pcid));
                ret += sizeof(cell_identity_lte_v4);
                break;
            case SIT_NET_CELL_INFO_TYPE_WCDMA:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_WCDMA;
                CellInfoProtocolUtils::Legacy::FillCellIdentityWcdma(cellIdentity.wcdma, *((cell_identity_wcdma_v4 *)pcid));
                ret += sizeof(cell_identity_wcdma_v4);
                break;
            case SIT_NET_CELL_INFO_TYPE_TDSCDMA:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_TD_SCDMA;
                CellInfoProtocolUtils::Legacy::FillCellIdentityTdscdma(cellIdentity.tdscdma, *((cell_identity_tdscdma_v4 *)pcid));
                ret += sizeof(cell_identity_tdscdma_v4);
                break;
            case SIT_NET_CELL_INFO_TYPE_NR:
                cellIdentity.cellInfoType = (RIL_CellInfoType)RIL_CELL_INFO_TYPE_NR;
                CellInfoProtocolUtils::Legacy::FillCellIdentityNr(cellIdentity.nr, *((cell_identity_nr_v4 *)pcid));
                ret += sizeof(cell_identity_nr_v4);
                break;
            default:
                RilLogE("ProtocolCellIdentityParserV4: unknown cellInfoType=%d", type);
                cellIdentity = {};
                ret = 0;
                break;
            }
        }
        return ret;
    }
};

struct ProtocolCellIdentityParser {
    int mCellIdentityVer;
public:
    ProtocolCellIdentityParser(int cellIdentityVer) : mCellIdentityVer(cellIdentityVer) {
    }

    size_t Parse(RIL_CellIdentity_V1_5& cellIdentity, void *data) {
        if (mCellIdentityVer == 4) {
            ProtocolCellIdentityParserV4 parser;
            return parser.Parse(cellIdentity, data);

        } else if (mCellIdentityVer == 3) {
            ProtocolCellIdentityParserV3 parser;
            return parser.Parse(cellIdentity, data);
        } else {
            RilLogE("ProtocolCellIdentityParser: cellIdentityVer=%d", mCellIdentityVer);
            return 0;
        }
    }
};

/**
 * ProtocolNetCellInfoListAdapterLegacy
 */
ProtocolNetCellInfoListAdapterLegacy::ProtocolNetCellInfoListAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData)
{
}

list<RIL_CellInfo_V1_6>& ProtocolNetCellInfoListAdapterLegacy::GetCellInfoList(int cellInfoVer)
{
    mCellInfoList.resize(0);
    if (m_pModemData != NULL) {
        sit_net_get_cell_info_list_rsp *response = (sit_net_get_cell_info_list_rsp *)m_pModemData->GetRawData();
        if (response != NULL && response->hdr.id == SIT_GET_CELL_INFO_LIST) {
            int cellInfoSize = response->cell_info_num;
            void *data = response->cell_info_list;
            int dataLen = GetParameterLength() - sizeof(int);

            CellInfoAdapterFactory cellInfoAdapterFactory;
            CellInfoListAdapter *cellInfoListAdapter = cellInfoAdapterFactory.GetCellInfoListAdapter(cellInfoVer);
            if (cellInfoListAdapter != NULL) {
                cellInfoListAdapter->Init(data, dataLen, cellInfoSize);
                mCellInfoList = cellInfoListAdapter->GetCellInfoList();
            }
        }
    }
    return mCellInfoList;
}

/**
 * ProtocolNetCellInfoListIndAdapterLegacy
 */
ProtocolNetCellInfoListIndAdapterLegacy::ProtocolNetCellInfoListIndAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
}

list<RIL_CellInfo_V1_6>& ProtocolNetCellInfoListIndAdapterLegacy::GetCellInfoList(int cellInfoVer)
{
    mCellInfoList.resize(0);
    if (m_pModemData != NULL) {
        sit_net_cell_info_list_ind *response = (sit_net_cell_info_list_ind *)m_pModemData->GetRawData();
        if (response != NULL && response->hdr.id == SIT_IND_CELL_INFO_LIST) {
            int cellInfoSize = response->cell_info_num;
            void *data = response->cell_info_list;
            int dataLen = GetParameterLength() - sizeof(int);

            CellInfoAdapterFactory cellInfoAdapterFactory;
            CellInfoListAdapter *cellInfoListAdapter = cellInfoAdapterFactory.GetCellInfoListAdapter(cellInfoVer);
            if (cellInfoListAdapter != NULL) {
                cellInfoListAdapter->Init(data, dataLen, cellInfoSize);
                mCellInfoList = cellInfoListAdapter->GetCellInfoList();
            }
        }
    }
    return mCellInfoList;
}

/**
 * ProtocolNetScanResultAdapterLegacy
 */
ProtocolNetScanResultAdapterLegacy::ProtocolNetScanResultAdapterLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData)
{
}

int ProtocolNetScanResultAdapterLegacy::GetScanStatus() const
{
    int ret = COMPLETE;
    if (m_pModemData != NULL) {
        sit_net_scanning_network_ind *data = (sit_net_scanning_network_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SCANNING_NETWORKS) {
            if (data->scan_status == SIT_NET_SCAN_STATUS_PARTIAL)
                ret = PARTIAL;
        }
    }
    return ret;
}

int ProtocolNetScanResultAdapterLegacy::GetScanResult() const
{
    // TODO need to get result from modem
    return RIL_E_SUCCESS;
}

list<RIL_CellInfo_V1_6>& ProtocolNetScanResultAdapterLegacy::GetCellInfoList(int cellInfoVer)
{
    mCellInfoList.resize(0);
    if (m_pModemData != NULL) {
        sit_net_scanning_network_ind *response = (sit_net_scanning_network_ind *)m_pModemData->GetRawData();
        if (response != NULL && response->hdr.id == SIT_IND_SCANNING_NETWORKS) {
            int cellInfoSize = response->cell_info_num;
            void *data = response->cell_info_list;
            int dataLen = GetParameterLength() - (sizeof(int) + sizeof(char));

            CellInfoAdapterFactory cellInfoAdapterFactory;
            CellInfoListAdapter *cellInfoListAdapter = cellInfoAdapterFactory.GetCellInfoListAdapter(cellInfoVer);
            if (cellInfoListAdapter != NULL) {
                cellInfoListAdapter->Init(data, dataLen, cellInfoSize);
                mCellInfoList = cellInfoListAdapter->GetCellInfoList();
            }
        }
    }
    return mCellInfoList;
}

/**
 * ProtocolNetSimFileInfoAdapterLegacy
 */
ProtocolNetSimFileInfoAdapterLegacy::ProtocolNetSimFileInfoAdapterLegacy(const ModemData *pModemData) : ProtocolIndAdapter (pModemData) {
    mSimFileId = 0;
    mRecordLen = 0;
    mNumRecords = 0;
    mppData = NULL;

    if (m_pModemData != NULL) {
        sit_sim_file_data_info_ind *data = (sit_sim_file_data_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_SIM_DATA_INFO) {
            mSimFileId = data->sim_file_id;
            mRecordLen = data->record_len;
            mNumRecords = data->num_of_records;
            mppData = new BYTE *[mNumRecords];
            BYTE *pTmp = data->data_info;
            for (int i = 0; i < mNumRecords; i++) {
                BYTE *pInputRecord = pTmp + (i * mRecordLen);
                BYTE *record = new BYTE[mRecordLen];
                memcpy(record, pInputRecord, mRecordLen);
                *(mppData + i) = record;
            } // end for i ~
        }
    }
}

ProtocolNetSimFileInfoAdapterLegacy::~ProtocolNetSimFileInfoAdapterLegacy()
{
    // delete memory
    if (mppData != NULL) {
        BYTE **ppTmp = mppData;
        BYTE *pDel = NULL;
        int i = mNumRecords;
        while(i > 0) {
            pDel = *ppTmp++;
            if(pDel != NULL) delete [] pDel;
            i--;
        }
        delete [] mppData;
    }
}

int ProtocolNetSimFileInfoAdapterLegacy::GetSimFileId() const
{
    return mSimFileId;
}

int ProtocolNetSimFileInfoAdapterLegacy::GetRecordLen() const
{
    return mRecordLen;
}

int ProtocolNetSimFileInfoAdapterLegacy::GetNumOfRecords() const
{
    return mNumRecords;
}

BYTE **ProtocolNetSimFileInfoAdapterLegacy::GetSimFileData() const
{
    return mppData;
}

/**
 * ProtocolNetPhysicalChannelConfigsLegacy
 */
ProtocolNetPhysicalChannelConfigsLegacy::ProtocolNetPhysicalChannelConfigsLegacy(const ModemData *pModemData)
    : ProtocolIndAdapter(pModemData) {
    Init();
}

void ProtocolNetPhysicalChannelConfigsLegacy::Init()
{
    // fill data with considering legacy
    if (m_pModemData != NULL && m_pModemData->GetRawData() != NULL) {
        int messageId = m_pModemData->GetMessageId();
        if (messageId == SIT_IND_SCG_BEARER_ALLOCATION) {
            ProtocolNetworkSgcBearerAllocIndAdapterLegacy nrScgAdapter(m_pModemData);
            PhysicalChannelConfigsData pcc = {};
            pcc.setCurrentStatusRat(nrScgAdapter.GetConnectionStatus(), nrScgAdapter.GetRat());
            mPcc.push_back(pcc);
            // others, default
        } else if (messageId == SIT_IND_PHYSICAL_CHANNEL_CONFIG) {
            int dataSize = m_pModemData->GetLength() - sizeof(RCM_IND_HEADER);
            if ((dataSize - sizeof(int)) % sizeof(sit_physical_channel_config_v1_6) == 0) {
                // sit_net_physical_channel_config_ind_v1_6
                FillPhysicalChannelConfigV1_6();
            } else {
                // sit_net_physical_channel_config_ind
                FillPhysicalChannelConfig();
            }
        }
    }
}

void ProtocolNetPhysicalChannelConfigsLegacy::FillPhysicalChannelConfigV1_6() {
    sit_net_physical_channel_config_ind_v1_6 *data =
        (sit_net_physical_channel_config_ind_v1_6 *)m_pModemData->GetRawData();
    int size = (data->config_len > 0) ? data->config_len : 0;
    if (size > MAX_PHYSICAL_CHANNEL_CONFIGS) {
        size = MAX_PHYSICAL_CHANNEL_CONFIGS;
    }

    for (int i = 0; i < size; i++) {
        PhysicalChannelConfigsData pcc = {};
        sit_physical_channel_config_v1_6 &sitData = data->configs[i];
        pcc.setCurrentStatusRat(sitData.cell_status, NetProtocolUtils::ConvertSitRat2RilRat(sitData.rat));
        pcc.setDownLinkInfo(sitData.channel, sitData.cell_bandwidth_downlink, sitData.frequency_range, sitData.physical_cellid);
        pcc.setContextIds(sitData.context_len, sitData.context_id);
        pcc.setUplinkInfo(sitData.channel_uplink, sitData.cell_bandwidth_uplink, sitData.band_number);
        mPcc.push_back(pcc);
    } // end for i ~
}

void ProtocolNetPhysicalChannelConfigsLegacy::FillPhysicalChannelConfig() {
    sit_net_physical_channel_config_ind *data =
        (sit_net_physical_channel_config_ind *)m_pModemData->GetRawData();
    int size = (data->config_len > 0) ? data->config_len : 0;
    if (size > MAX_PHYSICAL_CHANNEL_CONFIGS) {
        size = MAX_PHYSICAL_CHANNEL_CONFIGS;
    }

    for (int i = 0; i < size; i++) {
        PhysicalChannelConfigsData pcc = {};
        sit_physical_channel_config &sitData = data->configs[i];
        pcc.setCurrentStatusRat(sitData.cell_status, NetProtocolUtils::ConvertSitRat2RilRat(sitData.rat));
        pcc.setDownLinkInfo(sitData.channel, sitData.cell_bandwidth_downlink, sitData.frequency_range, sitData.physical_cellid);
        pcc.setContextIds(sitData.context_len, sitData.context_id);
        mPcc.push_back(pcc);
    } // end for i ~
}

/**
 * ProtocolNetGetManualRatModeAdapterLegacy
 */

ProtocolNetGetManualRatModeAdapterLegacy::ProtocolNetGetManualRatModeAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), m_manual_rat_mode_set(0), m_rat(0)
{
    if (m_pModemData != NULL) {
        sit_net_get_manual_rat_mode_rsp *data = (sit_net_get_manual_rat_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_MANUAL_RAT_MODE) {
            m_manual_rat_mode_set = (int)data->manual_rat_mode_set;
            m_rat = data->rat;
        }
    }
}

void ProtocolNetGetManualRatModeAdapterLegacy::GetManualRatMode(void *data)
{
    *((int *)data+0) = m_manual_rat_mode_set;
    *((int *)data+1) = m_rat;
}

/**
 * ProtocolNetSetManualRatModeAdapterLegacy
 */
int ProtocolNetSetManualRatModeAdapterLegacy::GetCause() const
{
    if (m_pModemData != NULL) {
        sit_net_set_manual_rat_mode_rsp *data = (sit_net_set_manual_rat_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_MANUAL_RAT_MODE) {
            RilLogV("GetCause=%d", data->cause);
            return (int)data->cause;
        }
    }
    return 0;
}

/**
 * ProtocolNetGetFreqLockAdapterLegacy
 */

ProtocolNetGetFreqLockAdapterLegacy::ProtocolNetGetFreqLockAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData), m_freq_mode_set(0), m_rat(0), m_lte_pcid(0), m_lte_earfcn(0), m_gsm_arfcn(0), m_wcdma_psc(0), m_wcdma_uarfcn(0)
{
    if (m_pModemData != NULL) {
        sit_net_get_freq_lock_rsp *data = (sit_net_get_freq_lock_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_LOCK) {
            m_freq_mode_set = (int)data->freq_mode_set;
            m_rat = (int)data->rat;
            m_lte_pcid = data->lte_pci;
            m_lte_earfcn = data->lte_earfcn;
            m_gsm_arfcn = data->gsm_arfcn;
            m_wcdma_psc = data->wcdma_psc;
            m_wcdma_uarfcn = data->wcdma_uarfcn;
        }
    }
}

void ProtocolNetGetFreqLockAdapterLegacy::GetFrequencyLock(void *data)
{
    *((int *)data+0) = m_freq_mode_set;
    *((int *)data+1) = m_rat;
    *((int *)data+2) = m_lte_pcid;
    *((int *)data+3) = m_lte_earfcn;
    *((int *)data+4) = m_gsm_arfcn;
    *((int *)data+5) = m_wcdma_psc;
    *((int *)data+6) = m_wcdma_uarfcn;
}

/**
 * ProtocolNetSetFreqLockAdapterLegacy
 */
int ProtocolNetSetFreqLockAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_net_set_freq_lock_rsp *data = (sit_net_set_freq_lock_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_SET_FREQUENCY_LOCK) {
            RilLogV("GetResult=%d", data->result);
            return (int)data->result;
        }
    }
    return 0;
}

/**
 * ProtocolNetGetEndcModeAdapterLegacy
 */
int ProtocolNetGetEndcModeAdapterLegacy::GetEndcMode() const
{
    if (m_pModemData != NULL) {
        sit_net_get_endc_mode_rsp *data = (sit_net_get_endc_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_ENDC_MODE) {
            RilLogV("Get endc mode=%d", data->mode);
            return (int)data->mode;
        }
    }
    return 0;
}

/**
 * ProtocolNetworkFrequencyInfoIndAdapterLegacy
 */
int ProtocolNetworkFrequencyInfoIndAdapterLegacy::GetPrimaryRat() const
{
    int ret = 0;    // RAT_NONE
    if (m_pModemData != NULL) {
        sit_net_frequency_info_ind *data = (sit_net_frequency_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_FREQUENCY_INFO) {
            ret = (int)data->primary_rat;
        }
    }
    return ret;
}

int ProtocolNetworkFrequencyInfoIndAdapterLegacy::GetPrimaryBand() const
{
    int ret = 0;    // NONE
    if (m_pModemData != NULL) {
        sit_net_frequency_info_ind *data = (sit_net_frequency_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_FREQUENCY_INFO) {
            ret = (int)data->primary_band;
        }
    }
    return ret;
}

int ProtocolNetworkFrequencyInfoIndAdapterLegacy::GetPrimaryFrequency() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_net_frequency_info_ind *data = (sit_net_frequency_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_FREQUENCY_INFO) {
            ret = (int)data->primary_frequency;
        }
    }
    return ret;
}

int ProtocolNetworkFrequencyInfoIndAdapterLegacy::GetSecondaryRat() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_frequency_info_ind *data = (sit_net_frequency_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_FREQUENCY_INFO && m_pModemData->GetLength() == sizeof(sit_net_frequency_info_ind)) {
            ret = (int)data->secondary_rat;
        } else {
            return ret;
        }
    }
    return ret;
}

int ProtocolNetworkFrequencyInfoIndAdapterLegacy::GetSecondaryBand() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_frequency_info_ind *data = (sit_net_frequency_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_FREQUENCY_INFO && m_pModemData->GetLength() == sizeof(sit_net_frequency_info_ind)) {
            ret = (int)data->secondary_band;
        } else {
            return ret;
        }
    }
    return ret;
}

int ProtocolNetworkFrequencyInfoIndAdapterLegacy::GetSecondaryFrequency() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_frequency_info_ind *data = (sit_net_frequency_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_FREQUENCY_INFO && m_pModemData->GetLength() == sizeof(sit_net_frequency_info_ind)) {
            ret = (int)data->secondary_frequency;
        } else {
            return ret;
        }
    }
    return ret;
}

/*
 * ProtocolNetAcBarringInfoLegacy
 */
ProtocolNetAcBarringInfoLegacy::ProtocolNetAcBarringInfoLegacy(const ModemData *pModemData) : ProtocolIndAdapter(pModemData)
{
    NetProtocolUtils::InitAcBarringInfo(mAcBarringInfo);

    if(m_pModemData != NULL) {
        sit_net_ac_barring_info_ind *data = (sit_net_ac_barring_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_AC_BARRING_INFO) {
            mAcBarringInfo.for_emc = data->for_emc;

            mAcBarringInfo.for_mo_sig_factor = data->for_mo_sig_factor;
            mAcBarringInfo.for_mo_sig_time = data->for_mo_sig_time;
            memcpy(mAcBarringInfo.for_mo_sig_ac_list, data->for_mo_sig_ac_list, sizeof(data->for_mo_sig_ac_list));

            mAcBarringInfo.for_mo_data_factor = data->for_mo_data_factor;
            mAcBarringInfo.for_mo_data_time = data->for_mo_data_time;
            memcpy(mAcBarringInfo.for_mo_data_ac_list, data->for_mo_data_ac_list, sizeof(data->for_mo_data_ac_list));

            mAcBarringInfo.for_mmtel_voice_factor = data->for_mmtel_voice_factor;
            mAcBarringInfo.for_mmtel_voice_time = data->for_mmtel_voice_time;
            memcpy(mAcBarringInfo.for_mmtel_voice_ac_list, data->for_mmtel_voice_ac_list, sizeof(data->for_mmtel_voice_ac_list));

            mAcBarringInfo.for_mmtel_video_factor = data->for_mmtel_video_factor;
            mAcBarringInfo.for_mmtel_video_time = data->for_mmtel_video_time;
            memcpy(mAcBarringInfo.for_mmtel_video_ac_list, data->for_mmtel_video_ac_list, sizeof(data->for_mmtel_video_ac_list));
        }
    }
}

void ProtocolNetAcBarringInfoLegacy::GetAcBarringInfo(void *data, unsigned int /* size */)
{
    *((char *)data+0) = mAcBarringInfo.for_emc;
    *((char *)data+1) = mAcBarringInfo.for_mo_sig_factor;
    *((char *)data+2) = mAcBarringInfo.for_mo_data_factor;
    *((char *)data+3) = mAcBarringInfo.for_mmtel_voice_factor;
    *((char *)data+4) = mAcBarringInfo.for_mmtel_video_factor;
}

/*
 * ProtocolNetGetFrequencyInfoAdapterLegacy
 */
int ProtocolNetGetFrequencyInfoAdapterLegacy::GetPrimaryRat() const
{
    int ret = 0;    // RAT_NONE
    if (m_pModemData != NULL) {
        sit_net_get_frequency_info_rsp *data = (sit_net_get_frequency_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_INFO) {
            ret = (int)data->primary_rat;
        }
    }
    return ret;
}

int ProtocolNetGetFrequencyInfoAdapterLegacy::GetPrimaryBand() const
{
    int ret = 0;    // NONE
    if (m_pModemData != NULL) {
        sit_net_get_frequency_info_rsp *data = (sit_net_get_frequency_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_INFO) {
            ret = (int)data->primary_band;
        }
    }
    return ret;
}

int ProtocolNetGetFrequencyInfoAdapterLegacy::GetPrimaryFrequency() const
{
    int ret = 0;
    if (m_pModemData != NULL) {
        sit_net_get_frequency_info_rsp *data = (sit_net_get_frequency_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_INFO) {
            ret = (int)data->primary_frequency;
        }
    }
    return ret;
}

int ProtocolNetGetFrequencyInfoAdapterLegacy::GetSecondaryRat() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_get_frequency_info_rsp *data = (sit_net_get_frequency_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_INFO && m_pModemData->GetLength() == sizeof(sit_net_get_frequency_info_rsp)) {
            ret = (int)data->secondary_rat;
        } else {
            return ret;
        }
    }
    return ret;
}

int ProtocolNetGetFrequencyInfoAdapterLegacy::GetSecondaryBand() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_get_frequency_info_rsp *data = (sit_net_get_frequency_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_INFO && m_pModemData->GetLength() == sizeof(sit_net_get_frequency_info_rsp)) {
            ret = (int)data->secondary_band;
        } else {
            return ret;
        }
    }
    return ret;
}

int ProtocolNetGetFrequencyInfoAdapterLegacy::GetSecondaryFrequency() const
{
    int ret = -1;
    if (m_pModemData != NULL) {
        sit_net_get_frequency_info_rsp *data = (sit_net_get_frequency_info_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_FREQUENCY_INFO && m_pModemData->GetLength() == sizeof(sit_net_get_frequency_info_rsp)) {
            ret = (int)data->secondary_frequency;
        } else {
            return ret;
        }
    }
    return ret;
}

/*
 * ProtocolNetRrcInfoAdapterLegacy
 */
int ProtocolNetRrcInfoAdapterLegacy::GetRat() const
{
    int ret = 0;    // RAT_NONE
    if (m_pModemData != NULL) {
        sit_net_rrc_info_ind *data = (sit_net_rrc_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RRC_INFO) {
            ret = NetProtocolUtils::ConvertSitRat2RilRat(data->rat);
        }
    }
    return ret;
}

int ProtocolNetRrcInfoAdapterLegacy::GetState() const
{
    int ret = 0;    // RCC_IDLE
    if (m_pModemData != NULL) {
        sit_net_rrc_info_ind *data = (sit_net_rrc_info_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_RRC_INFO) {
            ret = (int)data->state;
        }
    }
    return ret;
}

/**
 * ProtocolNetGetNrModeAdapterLegacy
 */
int ProtocolNetGetNrModeAdapterLegacy::GetNrMode() const
{
    if (m_pModemData != NULL) {
        sit_net_get_nr_mode_rsp *data = (sit_net_get_nr_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_NR_MODE) {
            RilLogV("Get NR mode=%d", data->mode);
            return (int)data->mode;
        }
    }
    return 0;
}

/**
 * ProtocolNetGetNrSilenceModeAdapterLegacy
 */
bool ProtocolNetGetNrSilenceModeAdapterLegacy::GetNsaMode() const {
    if (m_pModemData != NULL) {
        sit_get_nr_silence_mode_rsp *data = (sit_get_nr_silence_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_NR_SILENCE_MODE
                && GetErrorCode() == RIL_E_SUCCESS) {
            return (data->nsa_mode != 0);
        }
    }
    return false;
}

bool ProtocolNetGetNrSilenceModeAdapterLegacy::GetSaMode() const {
    if (m_pModemData != NULL) {
        sit_get_nr_silence_mode_rsp *data = (sit_get_nr_silence_mode_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_NR_SILENCE_MODE
                && GetErrorCode() == RIL_E_SUCCESS) {
            return (data->sa_mode != 0);
        }
    }
    return false;
}

/*
 * ProtocolRegistrationFailedAdapterLegacy
 */
ProtocolRegistrationFailedAdapterLegacy::ProtocolRegistrationFailedAdapterLegacy(const ModemData *pModemData,
        int cellIdentityVer/* = 3*/)
    : ProtocolIndAdapter(pModemData), mCellIdentityVer(cellIdentityVer)
{
    Init();
}

void ProtocolRegistrationFailedAdapterLegacy::Init()
{
    mRegFailedInfo = {};
    if (m_pModemData != NULL && m_pModemData->GetRawData() != NULL) {
        sit_net_reg_failed_ind *data = (sit_net_reg_failed_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_REG_FAILED) {
            mRegFailedInfo.chosenPlmn = new char[MAX_PLMN_LEN + 1];
            memcpy(mRegFailedInfo.chosenPlmn, "000000", (MAX_PLMN_LEN + 1));

            if (data->plmn[0] != 0 && data->plmn[0] != '#') {
                memcpy(mRegFailedInfo.chosenPlmn, data->plmn, MAX_PLMN_LEN);
                if (mRegFailedInfo.chosenPlmn[5] == '#') {
                    mRegFailedInfo.chosenPlmn[5] = 0;
                }
            }

            switch (data->service_domain) {
                case SERVICE_DOMAIN_CSPS:
                    mRegFailedInfo.domainBitmap = DOMAIN_CS | DOMAIN_PS;
                    break;
                case SERVICE_DOMAIN_CS:
                    mRegFailedInfo.domainBitmap = DOMAIN_CS;
                    break;
                case SERVICE_DOMAIN_PS:
                    mRegFailedInfo.domainBitmap = DOMAIN_PS;
                    break;
            }

            mRegFailedInfo.causeCode = (data->rej_cause == REG_REJ_CAUSE_UNUSED) ? INT_MAX : data->rej_cause;
            mRegFailedInfo.additionalCauseCode = (data->rej_cause_additional == REG_REJ_CAUSE_UNUSED) ? INT_MAX : data->rej_cause_additional;
            ProtocolCellIdentityParser cellIdentityParser(mCellIdentityVer);
            size_t ret = cellIdentityParser.Parse(mRegFailedInfo.cellIdentity, data->cell_identity);
            if (ret == 0) {
                // fail to parse cell identity
                RilLogW("fail to read cell identity");
            }
        }
    }
}

ProtocolRegistrationFailedAdapterLegacy::~ProtocolRegistrationFailedAdapterLegacy() {
    if (mRegFailedInfo.chosenPlmn != NULL) delete[] mRegFailedInfo.chosenPlmn;
}

/**
 * ProtocolBarringInfosParser
 */
class ProtocolBarringInfosParser {
public:
    static int GetNumberOfBarringInfos(uint32_t *pDst, void *pSrc) {
        if (pDst == nullptr || pSrc == nullptr) {
            return 0;
        }
        uint32_t numOfInfos = *(static_cast<uint32_t *>(pSrc));
        constexpr uint32_t maxNumOfInfos = 20;
        *pDst = MIN(numOfInfos, maxNumOfInfos);
        return sizeof(uint32_t);
    }
    static int GetBarringInfo(RIL_BarringInfo *pDst, sit_barring_info *pSrc) {
        pDst->service = (RIL_BarringServiceType)pSrc->service_type;
        pDst->type = (RIL_BarringType)pSrc->barring_type;
        pDst->typeSpecificInfo.conditionalBarringInfo.barringFactor = pSrc->barring_factor;
        pDst->typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds = pSrc->time_second;
        pDst->typeSpecificInfo.conditionalBarringInfo.isBarred = pSrc->is_barred > 0 ? TRUE:FALSE;
        return sizeof(sit_barring_info);
    }
    static void FreeMemory(UINT32 /* numberOfRecords */, RIL_BarringInfo *pRecords) {
        if (pRecords != NULL) {
            delete[] pRecords;
        }
    }
};

/**
 * ProtocolBarringInfosLegacy
 */
ProtocolBarringInfosLegacy::ProtocolBarringInfosLegacy()
{
    memset(&mCellIdAndBarringInfo, 0, sizeof(mCellIdAndBarringInfo));
}

ProtocolBarringInfosLegacy::~ProtocolBarringInfosLegacy() {
    ProtocolBarringInfosParser::FreeMemory(mCellIdAndBarringInfo.numOfrecord, mCellIdAndBarringInfo.precords);
    memset(&mCellIdAndBarringInfo, 0, sizeof(mCellIdAndBarringInfo));
}

void ProtocolBarringInfosLegacy::DecodingBarringInfos(BYTE *pBarringInfos, UINT len,
        int cellIdentityVer /*=3*/)
{
    if (pBarringInfos == NULL || len == 0) return;

    unsigned int pos = 0;
    BYTE *inputData = pBarringInfos;
    ProtocolCellIdentityParser cellIdentityParser(cellIdentityVer);
    size_t ret = cellIdentityParser.Parse(mCellIdAndBarringInfo.cellIdentity, inputData);
    if (ret == 0) {
        // fail to read cell identity
        RilLogW("fail to read cell identity");
        mCellIdAndBarringInfo.cellIdentity = {};
        return ;
    }
    pos += ret;
    pos += ProtocolBarringInfosParser::GetNumberOfBarringInfos(&(mCellIdAndBarringInfo.numOfrecord), (void *)(inputData + pos));
    RilLogI("ProtocolBarringInfosLegacy:: %d BarringInfos", mCellIdAndBarringInfo.numOfrecord);

    if (pos < len && mCellIdAndBarringInfo.numOfrecord > 0) {
        mCellIdAndBarringInfo.precords = new RIL_BarringInfo[mCellIdAndBarringInfo.numOfrecord] {};
        if (mCellIdAndBarringInfo.precords != NULL) {
            for (uint32_t i = 0; pos < len && i < mCellIdAndBarringInfo.numOfrecord; ++i) {
                sit_barring_info *pSrc = (sit_barring_info *)(inputData + pos);
                pos += ProtocolBarringInfosParser::GetBarringInfo(mCellIdAndBarringInfo.precords+i, pSrc);
                RilLogV("[%d] RIL_BarringServiceType=%d BarringType=%d barringFactor=%d barringTimeSeconds=%d isBarred=%d",
                        i, mCellIdAndBarringInfo.precords[i].service, mCellIdAndBarringInfo.precords[i].type,
                        mCellIdAndBarringInfo.precords[i].typeSpecificInfo.conditionalBarringInfo.barringFactor,
                        mCellIdAndBarringInfo.precords[i].typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds,
                        mCellIdAndBarringInfo.precords[i].typeSpecificInfo.conditionalBarringInfo.isBarred);
            }
        }
    }
}

RIL_CellIdAndBarringInfo *ProtocolBarringInfosLegacy::GetCellIdAndBarringInfo() {
    if (mCellIdAndBarringInfo.numOfrecord == 0 || mCellIdAndBarringInfo.precords == NULL) {
        return NULL;
    }
    return &mCellIdAndBarringInfo;
}

/**
 * ProtocolGetBarringInfoRspAdapterLegacy
 */
ProtocolGetBarringInfoRspAdapterLegacy::ProtocolGetBarringInfoRspAdapterLegacy(const ModemData *pModemData,
        int cellIdentityVer/* = 3*/)
    : ProtocolRespAdapter(pModemData), ProtocolBarringInfosLegacy(), mCellIdentityVer(cellIdentityVer)
{
    UINT errorCode = GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) Init();
}

void ProtocolGetBarringInfoRspAdapterLegacy::Init()
{
    if (m_pModemData != NULL && m_pModemData->GetRawData() != NULL) {
        sit_net_get_barring_infos_rsp *data = (sit_net_get_barring_infos_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_BARRING_INFOS) {
            DecodingBarringInfos(data->barring_info_data, GetParameterLength(), mCellIdentityVer);
        }
    }
}

UINT ProtocolGetBarringInfoRspAdapterLegacy::GetErrorCode() const {
    return ProtocolRespAdapter::GetErrorCode();
}

RIL_CellIdAndBarringInfo *ProtocolGetBarringInfoRspAdapterLegacy::GetCellIdAndBarringInfo() {
    return ProtocolBarringInfosLegacy::GetCellIdAndBarringInfo();
}

/**
 * ProtocolBarringInfoChangedAdapterLegacy
 */
ProtocolBarringInfoChangedAdapterLegacy::ProtocolBarringInfoChangedAdapterLegacy(const ModemData *pModemData,
        int cellIdentityVer/* = 3*/)
    : ProtocolIndAdapter(pModemData), ProtocolBarringInfosLegacy(), mCellIdentityVer(cellIdentityVer)
{
    Init();
}

void ProtocolBarringInfoChangedAdapterLegacy::Init()
{
    if (m_pModemData != NULL && m_pModemData->GetRawData() != NULL) {
        sit_net_barring_chagned_ind *data = (sit_net_barring_chagned_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_BARRING_INFO_CHANGED) {
            DecodingBarringInfos(data->barring_info_data, GetParameterLength(), mCellIdentityVer);
        }
    }
}

RIL_CellIdAndBarringInfo *ProtocolBarringInfoChangedAdapterLegacy::GetCellIdAndBarringInfo() {
    return ProtocolBarringInfosLegacy::GetCellIdAndBarringInfo();
}

/**
 * ProtocolNetGetVonrCapaAdapterLegacy
 */
int ProtocolNetGetVonrCapaAdapterLegacy::GetVonrCapa() const {
    if (m_pModemData != NULL) {
        sit_net_get_vonr_capa_rsp *data = (sit_net_get_vonr_capa_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_VONR_CAPABILITY) {
            RilLogV("Get VoNR capability=%d", data->vonr_capa);
            return data->vonr_capa;
        }
    }
    return 0;
}

/*
 * ProtocolNetGetAllowNetworkAdapterLegacy
 */
UINT32 ProtocolNetGetAllowNetworkAdapterLegacy::GetRat() const
{
    UINT32 ret = 0;
    if (m_pModemData != NULL) {
        sit_net_get_allow_network_rsp *data = (sit_net_get_allow_network_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_ALLOW_NETWORK) {
            ret = NetProtocolUtils::ConvertSitRaf2Ril(data->rat, false);
        }
    }
    return ret;
}

/*
 * CellInfoAdapterFactory
 */
CellInfoListAdapter *CellInfoAdapterFactory::GetCellInfoListAdapter(int cellInfoVersion)
{
    if (cellInfoVersion == 1) {
        RilLogI("CellInfoAdapterFactory::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV1;
    } else if (cellInfoVersion == 2) {
        RilLogI("CellInfoAdapterFactory::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV2;
    } else if (cellInfoVersion == 3) {
        RilLogI("CellInfoAdapterFactory::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV3;
    } else if (cellInfoVersion == 4) {
        RilLogI("CellInfoAdapterFactory::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV4;
    } else {
        // if cellInfoVersion is invalid, return default CellInfo Version
        RilLogW("CellInfoAdapterFactory::GetCellInfoListAdapter cellInfoVersion=%d", cellInfoVersion);
        return &mCellInfoV4;
    }
}


/*
 * CellInfoListAdapter
 */
CellInfoListAdapter::CellInfoListAdapter() : mCellInfoNum(0), mData(NULL), mDataLen(0)
{
}

void CellInfoListAdapter::Init(void *data, int dataLen, int cellInfoNum)
{
    mCellInfoNum = cellInfoNum;
    RilLog("mCellInfoNum=%d", mCellInfoNum);
    if (cellInfoNum > 0 && data != NULL && dataLen > 0) {
        mData = new char[dataLen];
        if (mData != NULL) {
            memcpy(mData, data, dataLen);
            mDataLen = dataLen;
        }
    }
}

/*
 * CellInfoListParserV1
 */
list<RIL_CellInfo_V1_6>& CellInfoListParserV1::GetCellInfoList()
{
    FillCellInfo();
    return mCellInfoList;
}

void CellInfoListParserV1::FillCellInfo()
{
    if (mData == NULL || mDataLen <= 0 || mCellInfoNum <= 0) {
        return ;
    }

    int pos = 0;
    for (int i = 0; i < mCellInfoNum; i++) {
        sit_net_cell_info_item_v12 *p_cur = (sit_net_cell_info_item_v12 *)(mData + pos);
        RIL_CellInfo_V1_6 rilCellInfo;
        memset(&rilCellInfo, 0, sizeof(rilCellInfo));
        rilCellInfo.cellInfoType = (RIL_CellInfoType)(p_cur->cell_info_type + 1);
        rilCellInfo.registered = ((int)p_cur->reg_status == 1 ? true : false);
        rilCellInfo.timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
        if (rilCellInfo.registered) {
            rilCellInfo.connectionStatus = PRIMARY_SERVING;
        }
        CellInfoProtocolUtils::InitCellInfo(rilCellInfo, (int)rilCellInfo.cellInfoType);
        pos += 2;   // cellInfoType, reg_status
        switch ((int)rilCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            CellInfoProtocolUtils::Legacy::FillGsmCellInfo(rilCellInfo.CellInfo.gsm, p_cur->cell_info.gsm);
            pos += sizeof(cell_info_gsm_v12);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(rilCellInfo.CellInfo.cdma, p_cur->cell_info.cdma);
            pos += sizeof(cell_info_cdma);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            CellInfoProtocolUtils::Legacy::FillLteCellInfo(rilCellInfo.CellInfo.lte, p_cur->cell_info.lte);
            pos += sizeof(cell_info_lte_v12);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(rilCellInfo.CellInfo.wcdma, p_cur->cell_info.wcdma);
            pos += sizeof(cell_info_wcdma_v12);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(rilCellInfo.CellInfo.tdscdma, p_cur->cell_info.tdscdma);
            pos += sizeof(cell_info_tdscdma);
            break;
        default:
            // TODO stop parsing because of untrusted data type
            // TODO prepare NR
            RilLogW("[%s][%d] Unsupported CellInfoType %d", __FUNCTION__, (int)rilCellInfo.cellInfoType);
            return;
        }

        if (NetProtocolUtils::IsValidCellInfo(rilCellInfo)) {
            mCellInfoList.push_back(rilCellInfo);
            if (debug) {
                CellInfoProtocolUtils::PrintCellInfo(i, rilCellInfo);
            }
        } else {
            RilLogW("[%d] Drop a cell information", i);
        }
    } // end for i ~
}

/*
 * CellInfoListParserV2
 */
list<RIL_CellInfo_V1_6>& CellInfoListParserV2::GetCellInfoList()
{
    FillCellInfo();
    return mCellInfoList;
}

template <typename T>
void fillCellInfo_V1_6(list<RIL_CellInfo_V1_6> &cellInfoList, T *p_cur, int &pos, int i) {
    RIL_CellInfo_V1_6 rilCellInfo;
    memset(&rilCellInfo, 0, sizeof(rilCellInfo));
    rilCellInfo.cellInfoType = (RIL_CellInfoType)(p_cur->cell_info_type + 1);
    rilCellInfo.registered = ((int)p_cur->reg_status == 1 ? true : false);
    rilCellInfo.timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
    rilCellInfo.connectionStatus = (RIL_CellConnectionStatus)p_cur->cell_connection_status;
    CellInfoProtocolUtils::InitCellInfo(rilCellInfo, (int)rilCellInfo.cellInfoType);

    pos += 3;  // cellInfoType, reg_status, cell_connection_status
    switch ((int)rilCellInfo.cellInfoType) {
    case RIL_CELL_INFO_TYPE_GSM:
        CellInfoProtocolUtils::Legacy::FillGsmCellInfo(rilCellInfo.CellInfo.gsm, p_cur->cell_info.gsm);
        pos += sizeof(cell_info_gsm_v12);
        break;
    case RIL_CELL_INFO_TYPE_CDMA:
        CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(rilCellInfo.CellInfo.cdma, p_cur->cell_info.cdma);
        pos += sizeof(cell_info_cdma_v14);
        break;
    case RIL_CELL_INFO_TYPE_LTE:
        CellInfoProtocolUtils::Legacy::FillLteCellInfo(rilCellInfo.CellInfo.lte, p_cur->cell_info.lte);
        pos += sizeof(cell_info_lte_v14);
        break;
    case RIL_CELL_INFO_TYPE_WCDMA:
        CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(rilCellInfo.CellInfo.wcdma, p_cur->cell_info.wcdma);
        pos += sizeof(cell_info_wcdma_v14);
        break;
    case RIL_CELL_INFO_TYPE_TD_SCDMA:
        CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(rilCellInfo.CellInfo.tdscdma, p_cur->cell_info.tdscdma);
        pos += sizeof(cell_info_tdscdma_v14);
        break;
    case RIL_CELL_INFO_TYPE_NR:
        CellInfoProtocolUtils::Legacy::FillNrCellInfo(rilCellInfo.CellInfo.nr, p_cur->cell_info.nr);
        pos += sizeof(cell_info_nr);
        break;
    default:
        // TODO stop parsing because of untrusted data type
        RilLogW("[%s][%d] Unsupported CellInfoType %d", __FUNCTION__, (int)rilCellInfo.cellInfoType);
        return;
    }

    if (NetProtocolUtils::IsValidCellInfo(rilCellInfo)) {
        cellInfoList.push_back(rilCellInfo);
        if (debug) {
            CellInfoProtocolUtils::PrintCellInfo(i, rilCellInfo);
        }
    } else {
        RilLogW("[%d] Drop a cell information", i);
    }
}

void CellInfoListParserV2::FillCellInfo()
{
    if (mData == NULL || mDataLen <= 0 || mCellInfoNum <= 0) {
            return ;
        }

        int pos = 0;
        for (int i = 0; i < mCellInfoNum; i++) {
            sit_net_cell_info_item_v14 *p_cur = (sit_net_cell_info_item_v14 *)(mData + pos);
            fillCellInfo_V1_6(mCellInfoList, p_cur, pos, i);
        } // end for i ~
}

/*
 * CellInfoListParserV3
 */
list<RIL_CellInfo_V1_6>& CellInfoListParserV3::GetCellInfoList()
{
    FillCellInfo();
    return mCellInfoList;
}

void CellInfoListParserV3::FillCellInfo()
{
    if (mData == NULL || mDataLen <= 0 || mCellInfoNum <= 0) {
        return ;
    }

    int pos = 0;
    for (int i = 0; i < mCellInfoNum; i++) {
        cell_information_v3 *p_cur = (cell_information_v3 *)(mData + pos);
        fillCellInfo_V1_6(mCellInfoList, p_cur, pos, i);
    } // end for i ~
}

/*
 * CellInfoListParserV4
 */
list<RIL_CellInfo_V1_6>& CellInfoListParserV4::GetCellInfoList()
{
    FillCellInfo();
    return mCellInfoList;
}

void CellInfoListParserV4::FillCellInfo()
{
    if (mData == NULL || mDataLen <= 0 || mCellInfoNum <= 0) {
        return ;
    }

    int pos = 0;
    for (int i = 0; i < mCellInfoNum; i++) {
        cell_information_v4 *p_cur = (cell_information_v4 *)(mData + pos);
        RIL_CellInfo_V1_6 rilCellInfo;
        memset(&rilCellInfo, 0, sizeof(rilCellInfo));
        rilCellInfo.cellInfoType = (RIL_CellInfoType)(p_cur->cell_info_type + 1);
        rilCellInfo.registered = ((int)p_cur->reg_status == 1 ? true : false);
        rilCellInfo.timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
        rilCellInfo.connectionStatus = (RIL_CellConnectionStatus)p_cur->cell_connection_status;
        CellInfoProtocolUtils::InitCellInfo(rilCellInfo, (int)rilCellInfo.cellInfoType);

        pos += 3;   // cellInfoType, reg_status, cell_connection_status
        switch ((int)rilCellInfo.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
            CellInfoProtocolUtils::Legacy::FillGsmCellInfo(rilCellInfo.CellInfo.gsm, p_cur->cell_info.gsm);
            pos += sizeof(cell_info_type_gsm_v4);
            break;
        case RIL_CELL_INFO_TYPE_CDMA:
            CellInfoProtocolUtils::Legacy::FillCdmaCellInfo(rilCellInfo.CellInfo.cdma, p_cur->cell_info.cdma);
            pos += sizeof(cell_info_type_cdma_v4);
            break;
        case RIL_CELL_INFO_TYPE_LTE:
            CellInfoProtocolUtils::Legacy::FillLteCellInfo(rilCellInfo.CellInfo.lte, p_cur->cell_info.lte);
            pos += sizeof(cell_info_type_lte_v4);
            break;
        case RIL_CELL_INFO_TYPE_WCDMA:
            CellInfoProtocolUtils::Legacy::FillWcdmaCellInfo(rilCellInfo.CellInfo.wcdma, p_cur->cell_info.wcdma);
            pos += sizeof(cell_info_type_wcdma_v4);
            break;
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
            CellInfoProtocolUtils::Legacy::FillTdscdmaCellInfo(rilCellInfo.CellInfo.tdscdma, p_cur->cell_info.tdscdma);
            pos += sizeof(cell_info_type_tdscdma_v4);
            break;
        case RIL_CELL_INFO_TYPE_NR:
            CellInfoProtocolUtils::Legacy::FillNrCellInfo(rilCellInfo.CellInfo.nr, p_cur->cell_info.nr);
            pos += sizeof(cell_info_type_nr_v4);
            break;
        default:
            // TODO stop parsing because of untrusted data type
            RilLogW("[%s][%d] Unsupported CellInfoType %d", __FUNCTION__, (int)rilCellInfo.cellInfoType);
            return;
        }

        if (NetProtocolUtils::IsValidCellInfo(rilCellInfo)) {
            mCellInfoList.push_back(rilCellInfo);
            if (debug) {
                CellInfoProtocolUtils::PrintCellInfo(i, rilCellInfo);
            }
        } else {
            RilLogW("[%d] Drop a cell information", i);
        }
    } // end for i ~
}

/**
 * ProtocolNetSystemSelectionChannelsAdapterLegacy
 */
ProtocolNetSystemSelectionChannelsAdapterLegacy::ProtocolNetSystemSelectionChannelsAdapterLegacy(const ModemData *pModemData)
    : ProtocolRespAdapter(pModemData) {
    memset(m_radioAccessSpecifiers, 0, sizeof(m_radioAccessSpecifiers));
    m_specifiersLength = 0;
    int errorCode = GetErrorCode();
    if (errorCode == RIL_E_SUCCESS)
        Init();
}

void ProtocolNetSystemSelectionChannelsAdapterLegacy::Init() {
    if (m_pModemData == nullptr) {
        return;
    }

    sit_net_get_system_selection_channels_rsp *data =
        (sit_net_get_system_selection_channels_rsp *)m_pModemData->GetRawData();

    if (data == nullptr || data->hdr.id != SIT_GET_SYSTEM_SELECTION_CHANNELS) {
        return;
    }

    int specifiersLength = MIN(MIN(data->radio_access_specifier_len, SIT_MAX_NO_RAS), MAX_RADIO_ACCESS_NETWORKS);
    int dstIndex = 0;
    for (int srcIndex = 0; srcIndex < specifiersLength; srcIndex++) {
        const int sitNetworkType = data->radio_access_specifier_array[srcIndex].radio_access_network;
        if (NetProtocolUtils::IsSysSelSupported(sitNetworkType) == false) {
            continue;
        }
        FillRasSitSrcToRilDst(sitNetworkType, data->radio_access_specifier_array[srcIndex], m_radioAccessSpecifiers[dstIndex]);
        dstIndex++;
    }
    m_specifiersLength = dstIndex;
}

void ProtocolNetSystemSelectionChannelsAdapterLegacy::FillRasSitSrcToRilDst(int sitNetworkType,
                                                                            sit_net_radio_access_specifier &sitSrcRas,
                                                                            RIL_RadioAccessSpecifier_V1_5 &rilDstRas) {
    switch (sitNetworkType) {
    case SIT_NET_ACCESS_RADIO_TYPE_GERAN:
        FillRasSitSrcToRilDst(GERAN_TYPE(), sitSrcRas, rilDstRas);
        break;
    case SIT_NET_ACCESS_RADIO_TYPE_UTRAN:
        FillRasSitSrcToRilDst(UTRAN_TYPE(), sitSrcRas, rilDstRas);
        break;
    case SIT_NET_ACCESS_RADIO_TYPE_EUTRAN:
        FillRasSitSrcToRilDst(EUTRAN_TYPE(), sitSrcRas, rilDstRas);
        break;
    case SIT_NET_ACCESS_RADIO_TYPE_NGRAN:
        FillRasSitSrcToRilDst(NGRAN_TYPE(), sitSrcRas, rilDstRas);
        break;
    default:
        //assert(false);
        break;
    }
}

void ProtocolNetSystemSelectionChannelsAdapterLegacy::FillRasSitSrcToRilDst(GERAN_TYPE,
                                                                            sit_net_radio_access_specifier &sitSrcRas,
                                                                            RIL_RadioAccessSpecifier_V1_5 &rilDstRas) {
    rilDstRas.radio_access_network = RIL_RAN_GERAN;
    rilDstRas.bands_length = (uint32_t)(MIN(MIN(sitSrcRas.bands_len, SIT_MAX_NO_SSC_GERAN_BANDS), MAX_BANDS));
    rilDstRas.channels_length = (uint32_t)(MIN(MIN(sitSrcRas.channels_len, SIT_MAX_NO_SSC_GERAN_CHANNELS), MAX_CHANNELS));
    CopyArrayDataSrcToDst(sitSrcRas.bands.geran_bands, rilDstRas.bands.geran_bands, rilDstRas.bands_length);
    CopyArrayDataSrcToDst(sitSrcRas.channels.geran_channels, rilDstRas.channels, rilDstRas.channels_length);
}

void ProtocolNetSystemSelectionChannelsAdapterLegacy::FillRasSitSrcToRilDst(UTRAN_TYPE,
                                                                            sit_net_radio_access_specifier &sitSrcRas, RIL_RadioAccessSpecifier_V1_5 &rilDstRas) {
    rilDstRas.radio_access_network = RIL_RAN_UTRAN;
    rilDstRas.bands_length = (uint32_t)(MIN(MIN(sitSrcRas.bands_len, SIT_MAX_NO_SSC_UTRAN_BANDS), MAX_BANDS));
    rilDstRas.channels_length = (uint32_t)(MIN(MIN(sitSrcRas.channels_len, SIT_MAX_NO_SSC_UTRAN_CHANNELS), MAX_CHANNELS));
    CopyArrayDataSrcToDst(sitSrcRas.bands.geran_bands, rilDstRas.bands.geran_bands, rilDstRas.bands_length);
    CopyArrayDataSrcToDst(sitSrcRas.channels.geran_channels, rilDstRas.channels, rilDstRas.channels_length);
}

void ProtocolNetSystemSelectionChannelsAdapterLegacy::FillRasSitSrcToRilDst(EUTRAN_TYPE,
                                                                            sit_net_radio_access_specifier &sitSrcRas,
                                                                            RIL_RadioAccessSpecifier_V1_5 &rilDstRas) {
    rilDstRas.radio_access_network = RIL_RAN_EUTRAN;
    rilDstRas.bands_length = (uint32_t)(MIN(MIN(sitSrcRas.bands_len, SIT_MAX_NO_SSC_EUTRAN_BANDS), MAX_BANDS));
    rilDstRas.channels_length = (uint32_t)(MIN(MIN(sitSrcRas.channels_len, SIT_MAX_NO_SSC_EUTRAN_CHANNELS), MAX_CHANNELS));
    CopyArrayDataSrcToDst(sitSrcRas.bands.geran_bands, rilDstRas.bands.geran_bands, rilDstRas.bands_length);
    CopyArrayDataSrcToDst(sitSrcRas.channels.geran_channels, rilDstRas.channels, rilDstRas.channels_length);
}

void ProtocolNetSystemSelectionChannelsAdapterLegacy::FillRasSitSrcToRilDst(NGRAN_TYPE,
                                                                            sit_net_radio_access_specifier &sitSrcRas,
                                                                            RIL_RadioAccessSpecifier_V1_5 &rilDstRas) {
    rilDstRas.radio_access_network = (RIL_RadioAccessNetworks)RIL_RAN_NGRAN;
    rilDstRas.bands_length = (uint32_t)(MIN(MIN(sitSrcRas.bands_len, SIT_MAX_NO_SSC_NGRAN_BANDS), MAX_BANDS));
    rilDstRas.channels_length = (uint32_t)(MIN(MIN(sitSrcRas.channels_len, SIT_MAX_NO_SSC_NGRAN_CHANNELS), MAX_CHANNELS));
    CopyArrayDataSrcToDst(sitSrcRas.bands.geran_bands, rilDstRas.bands.geran_bands, rilDstRas.bands_length);
    CopyArrayDataSrcToDst(sitSrcRas.channels.geran_channels, rilDstRas.channels, rilDstRas.channels_length);
}

/**
 * ProtocolNetGetDeviceServiceAdapterLegacy
 */
int ProtocolNetGetDeviceServiceAdapterLegacy::GetUsageSetting() const {
    if (m_pModemData != NULL) {
        sit_net_get_device_service_rsp *data = (sit_net_get_device_service_rsp *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_GET_DEVICE_SERVICE) {
            RilLogV("Get Device Service=%d", data->usage_setting);
            return data->usage_setting;
        }
    }
    return 0;
}

/**
 * ProtocolNetDsdsStatusIndAdapterLegacy
 */
int ProtocolNetDsdsStatusIndAdapterLegacy::GetStatus() const {
    int status = DSDS_STATUS_RESUME;
    if (m_pModemData != nullptr) {
        sit_net_dsds_status_ind *data = (sit_net_dsds_status_ind *)m_pModemData->GetRawData();
        if (data != nullptr && data->hdr.id == SIT_IND_DSDS_STATUS) {
            status = data->status;
        }
    }
    return status;
}

bool ProtocolNetDsdsStatusIndAdapterLegacy::IsPsRestricted() const {
    return GetStatus() == DSDS_STATUS_SUSPEND;
}
