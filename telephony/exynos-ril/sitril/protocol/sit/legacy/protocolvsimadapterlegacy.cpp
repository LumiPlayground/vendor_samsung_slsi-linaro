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
 * protocolvsimadapterlegacy.cpp
 *
 *  Created on: 2016. 02. 26.
 */

#include "protocolvsimadapterlegacy.h"
#include "vsimprotocolutils.h"
#include "util.h"
#include "sitdef.h"
#include "modemdata.h"

/**
 * ProtocolVsimOperationAdapterLegacy
 */
ProtocolVsimOperationAdapterLegacy::ProtocolVsimOperationAdapterLegacy(const ModemData *pModemData)
    : ProtocolVsimOperationAdapterInterface(), ProtocolIndAdapter(pModemData), mOperationData(NULL), mOperationDataLength(0)
{
    if (m_pModemData != NULL) {
        sit_vsim_opertaion_ind *data = (sit_vsim_opertaion_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_VSIM_OPERATION) {
            int dataLen = data->datalength;
            if (dataLen > 0) {
                mOperationDataLength = dataLen * 2 + 1;
                mOperationData = new char[mOperationDataLength];
                if (mOperationData != NULL) {
                    memset(mOperationData, 0, mOperationDataLength);
                    int ret = Value2HexString(mOperationData, (const BYTE*)data->data, dataLen);
                    if (ret < 0) {
                        delete[] mOperationData;
                        mOperationData = NULL;
                        mOperationDataLength = 0;
                    }
                }
            }
        }
    }
}

ProtocolVsimOperationAdapterLegacy::~ProtocolVsimOperationAdapterLegacy()
{
    if (mOperationData != NULL) {
        delete[] mOperationData;
    }
}

int ProtocolVsimOperationAdapterLegacy::GetTransactionId() const
{
    if (m_pModemData != NULL) {
        sit_vsim_opertaion_ind *data = (sit_vsim_opertaion_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_VSIM_OPERATION) {
            return data->tid;
        }
    }
    return -1;
}

int ProtocolVsimOperationAdapterLegacy::GetEventId() const
{
    if (m_pModemData != NULL) {
        sit_vsim_opertaion_ind *data = (sit_vsim_opertaion_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_VSIM_OPERATION) {
            int eventid = VsimProtocolUtils::ConvertVsimOpSitEventIdToRilEventId(data->event_id);
            return eventid;
        }
    }
    return -1;
}

int ProtocolVsimOperationAdapterLegacy::GetResult() const
{
    if (m_pModemData != NULL) {
        sit_vsim_opertaion_ind *data = (sit_vsim_opertaion_ind *)m_pModemData->GetRawData();
        if (data != NULL && data->hdr.id == SIT_IND_VSIM_OPERATION) {
            return data->result;
        }
    }
    return -1;
}
