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
 * uiccphonebookhandler.cpp
 *
 *  Created on: 2021. 5. 11.
 *      Author: yunzhen.kim
 */

#include "uiccphonebookhandler.h"
#include "rillog.h"
#include "simservice.h"
#include "protocolsimbuilder.h"
#include "protocolsimadapter.h"
#include "simdata.h"
#include "simdatabuilder.h"
#include "iccUtil.h"

#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_SIM, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define PARAM_NULL(msg)         { if(msg==NULL) { RilLogE("%s::%s() Parameter = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_REQ(msg)           { if(msg==NULL || msg->GetRequestData()==NULL) { RilLogE("%s::%s() RequestData = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define NULL_RSP(msg)           { if(msg==NULL || msg->GetModemData()==NULL) { RilLogE("%s::%s() ModemData = NULL", m_pSimService->m_szSvcName, __FUNCTION__); return -1; } }
#define ENTER_FUNC()            { RilLogI("%s::%s() [<-- ", m_pSimService->m_szSvcName, __FUNCTION__); }
#define LEAVE_FUNC()            { RilLogI("%s::%s() [--> ", m_pSimService->m_szSvcName, __FUNCTION__); }
#define NOT_IMPLEMENT()         { RilLogE("%s::%s() Not Implemented", m_pSimService->m_szSvcName, __FUNCTION__); }

#define LOGI(format, ...)       RilLogI("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGV(format, ...)       RilLogV("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#undef LOGD
#define LOGD(format, ...)       RilLog("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)
#define LOGE(format, ...)       RilLogE("%s::%s() " format, m_pSimService->m_szSvcName, __FUNCTION__, ##__VA_ARGS__)

UiccPhonebookHandler::UiccPhonebookHandler() {
    mRecordId = 1;
    mIsSimPbLoading = false;
    mIsSimPbSupported = true;
    mIsSimPbInitialized = false;
    mPbResult.receivedStatus = 0;
    mPbResult.loadedCount = 0;
    m_pSimService = NULL;
    mPbResult.mPbRec = NULL;
    memset(&mPbCapa, 0, sizeof(SIM_PhonebookCapacity));
}

UiccPhonebookHandler::~UiccPhonebookHandler() {
    mRecordId = 1;
    mIsSimPbLoading = false;
    mIsSimPbSupported = true;
    mIsSimPbInitialized = false;
    mPbResult.loadedCount = 0;
    mPbResult.receivedStatus = 0;
    m_pSimService = NULL;
    if (mPbResult.mPbRec != NULL) {
        delete[] mPbResult.mPbRec;
        mPbResult.mPbRec = NULL;
    }
    memset(&mPbCapa, 0, sizeof(SIM_PhonebookCapacity));
}

void UiccPhonebookHandler::Init(SimService* pSimService) {
    if (pSimService != NULL) {
        m_pSimService = pSimService;
    }
}

void UiccPhonebookHandler::InitPbFlags() {
    LOGD("InitPbFlags");
    mRecordId = 1;
    mIsSimPbLoading = false;
    mIsSimPbSupported = true;
    mIsSimPbInitialized = false;
    mPbResult.loadedCount = 0;
    mPbResult.receivedStatus = 0;
    if (mPbResult.mPbRec != NULL) {
        delete[] mPbResult.mPbRec;
        mPbResult.mPbRec = NULL;
    }
    memset(&mPbCapa, 0, sizeof(SIM_PhonebookCapacity));
}

void UiccPhonebookHandler::SendLoadingPbError(int error) {
    OnGetPbRecDone(error);
}

int UiccPhonebookHandler::SendMessageForReadPbEntry(int recordId) {
    int inputData[2] = {PB_ADN_3G, recordId};
    RequestData *pData = RilParser::CreateInts(RIL_REQUEST_READ_PB_ENTRY, 0, (char *) inputData,
            sizeof(inputData));

    if (pData != NULL) {
        Message *pMsg = Message::ObtainMessage(pData, RIL_SERVICE_SIM, MSG_SIM_READ_PB_ENTRY);

        if (pMsg != NULL) {
            ServiceMgr *pServiceMgr = m_pSimService->GetRilContext()->GetServiceManager();
            if (pServiceMgr != NULL) {
                if (pServiceMgr->SendMessage(pMsg) < 0) {
                    LOGE("send message failed");
                    if (pMsg != NULL) {
                        delete pMsg;
                        return -1;
                    }
                }
            } else {
                if (pMsg != NULL) {
                    delete pMsg;
                    return -1;
                }
            }
        }
    }

    return 0;
}

int UiccPhonebookHandler::DoReadPbEntry(Message *pMsg) {
    int nResult = -1;
    NULL_REQ(pMsg);

    IntsRequestData *pReq = (IntsRequestData *) pMsg->GetRequestData();

    int pb_type = pReq->GetInt(0);
    int recordId = pReq->GetInt(1);

    LOGV("pb type:%d, recordId:%d", pb_type, recordId);

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimReadPbEntry(pb_type, recordId);
    nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, MSG_SIM_READ_PB_ENTRY_DONE);

    return (nResult < 0) ? -1 : 0;
}

int UiccPhonebookHandler::OnReadPbEntryDone(Message *pMsg) {
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimReadPbEntry adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    if (uErrCode == RIL_E_SUCCESS) {
        RIL_ReadPbEntry pb;
        RIL_3GPb *p;
        int pbLen = 0, data_len = 0, cnt = 0, anrSetCount = 0, emailSetCount = 0;

        pb.type = adapter.GetPbType();
        pb.recordId = adapter.GetRecordId();

        LOGV("mRecordId:%d", mRecordId);

        // set pb rec
        mPbResult.mPbRec[mPbResult.loadedCount].recordId = pb.recordId;
        memset(mPbResult.mPbRec[mPbResult.loadedCount].name, 0,
                sizeof(char) * MAX_NAME_LEN);
        memset(mPbResult.mPbRec[mPbResult.loadedCount].number, 0,
                sizeof(char) * (MAX_NUM_LEN + EFEXT_LEN));
        memset(mPbResult.mPbRec[mPbResult.loadedCount].emails, 0,
                sizeof(char) * MAX_EF_SET * MAX_NAME_LEN);
        memset(mPbResult.mPbRec[mPbResult.loadedCount].additionalNumbers, 0,
                sizeof(char) * MAX_EF_SET * MAX_ANR_LEN);

        int length = adapter.GetDataLength();
        char *entry = adapter.GetEntryData();
        char entry_data[MAX_PB_ENTRY_LEN] = {};

        switch(pb.type) {
        case PB_ADN_3G:
            p = (RIL_3GPb *) entry_data;
            while (length > 0) {
                if (*entry == 0xFF)
                    break;

                p->type3g = *entry;
                p->data_len = (*(entry + 1) & 0xFF) | (*(entry + 2) << 8);
                p->data_type = *(entry + 3);
                if (p->data_type == 0xFF)
                    break;

                p->data = new char[p->data_len - 1];
                memcpy(p->data, entry + 4, p->data_len - 1);
                entry += p->data_len + 3;

                // whole length - (sizeof(BYTE) + sizeof(UINT16) + data_len)
                length -= (p->data_len + 3);
                pbLen += sizeof(int) * 3 + p->data_len;
                data_len = p->data_len - 1;

                LOGV("cnt %d, type3G:%d len:%d dataType:0x%02x data:%s, remain_len:%d",
                        cnt, p->type3g, p->data_len, p->data_type, p->data, length);

                // input entry data to record
                if (p->type3g == NAME) {
                    char *pszName = new char[data_len];
                    memcpy(pszName, p->data, data_len);

                    if (p->data_type == UCS2) {
                        std::string str = IccUtil::adnStringToUtf8String(
                                (unsigned char *) pszName, data_len);
                        strncpy(mPbResult.mPbRec[mPbResult.loadedCount].name,
                                str.c_str(), str.size());
                        pbLen = (pbLen - data_len + str.size());
                    } else {
                        strncpy(mPbResult.mPbRec[mPbResult.loadedCount].name, pszName, data_len);
                    }
                    delete[] pszName;
                }

                if (p->type3g == NUMBER) {
                    char *pszNum = new char[data_len];
                    memcpy(pszNum, p->data, data_len);
                    strncpy(mPbResult.mPbRec[mPbResult.loadedCount].number, pszNum, strlen(pszNum));
                    delete[] pszNum;
                }

                if (p->type3g == EMAIL
                        || p->type3g == EMAILA
                        || p->type3g == EMAILB
                        || p->type3g == EMAILC) {
                    char *pszEmail = new char[data_len];
                    memcpy(pszEmail, p->data, data_len);
                    strncpy(mPbResult.mPbRec[mPbResult.loadedCount].emails[emailSetCount++],
                            pszEmail, strlen(pszEmail));
                    delete[] pszEmail;
                }

                if (p->type3g == ANR
                        || p->type3g == ANRA
                        || p->type3g == ANRB
                        || p->type3g == ANRC) {
                    // TODO: anr type will be handled later.
                    mPbResult.mPbRec[mPbResult.loadedCount].anrType = (int) ((p->data)[0]);
                    char *pszAnr = new char[data_len - 1];
                    memcpy(pszAnr, &((p->data)[1]), data_len - 1);
                    strncpy(mPbResult.mPbRec[mPbResult.loadedCount].additionalNumbers[anrSetCount++],
                            pszAnr, strlen(pszAnr));
                    delete[] pszAnr;
                }

                p++;
                cnt++;
            }
            pbLen += sizeof(int) * 2;

            mPbResult.mPbRec[mPbResult.loadedCount].emailSetCount = emailSetCount;
            mPbResult.mPbRec[mPbResult.loadedCount].anrSetCount = anrSetCount;

            LOGV("copied record ID:%d, size:%d, name:%s, number:%s, emailSetCount:%d, anrSetCount:%d, email:%s, anr type:%d, anr:%s",
                    mPbResult.mPbRec[mPbResult.loadedCount].recordId,
                    sizeof(RIL_PhonebookRecordInfo),
                    mPbResult.mPbRec[mPbResult.loadedCount].name,
                    mPbResult.mPbRec[mPbResult.loadedCount].number,
                    mPbResult.mPbRec[mPbResult.loadedCount].emailSetCount,
                    mPbResult.mPbRec[mPbResult.loadedCount].anrSetCount,
                    mPbResult.mPbRec[mPbResult.loadedCount].emails[0],
                    mPbResult.mPbRec[mPbResult.loadedCount].anrType,
                    mPbResult.mPbRec[mPbResult.loadedCount].additionalNumbers[0]);
            mPbResult.loadedCount++;

            while (cnt > 0) {
                delete p[cnt].data;
                cnt--;
            }
            break;
        default:
            break;
        }
    }

    m_pSimService->OnRequestComplete(uErrCode);
    if (mPbResult.loadedCount < mPbCapa.usedAdnRecords && mRecordId < mPbCapa.maxAdnRecords) {
        if (SendMessageForReadPbEntry(++mRecordId) < 0) {
            LOGE("cannot send read pb entry msg");
            OnGetPbRecDone(PB_RECEIVED_ABORT);
        }
    } else {
        LOGV("read pb records done. current record ID(%d), loadedPbCnt(%d), usedAdnRecords(%d)",
                mRecordId, mPbResult.loadedCount, mPbCapa.usedAdnRecords);

        if (mPbResult.loadedCount < mPbCapa.usedAdnRecords) {
            OnGetPbRecDone(PB_RECEIVED_OK);
        } else {
            OnGetPbRecDone(PB_RECEIVED_FINAL);
        }
    }

    return 0;
}

int UiccPhonebookHandler::DoUpdatePbEntry(Message *pMsg) {
    NULL_REQ(pMsg);
    V2_0::RIL_CardStatus status = m_pSimService->GetRilCardStatus();
    if (RIL_CARDSTATE_PRESENT != status.card_state) {
        m_pSimService->OnRequestComplete(RIL_E_INVALID_SIM_STATE);
        return 0;
    }

    if (false == mIsSimPbSupported) {
        m_pSimService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    int nResult = -1;
    SimUpdatePbEntry *pReq = (SimUpdatePbEntry *) pMsg->GetRequestData();
    ProtocolSimBuilder builder;
    ModemData *pModemData= NULL;

    int mode = pReq->GetMode();
    int type = pReq->GetPbType();
    int recordId = pReq->GetRecordId();

    /* delete */
    if (mode == 2) {
        pModemData = builder.BuildSimUpdatePbDelete(mode, type, recordId);
        nResult = m_pSimService->SendRequest(pModemData,
                TIMEOUT_SIM_DEFAULT, MSG_SIM_UPDATE_PB_ENTRY_DONE);
        return (nResult < 0) ? -1 : 0;
    }

    LOGV("mode:%d, type:%d, recordId:%d, length:%d",
            mode, type, recordId, pReq->GetLength());

    if (!pReq->GetLength()) {
        LOGE("data length is zero");
        return -1;
    }

    switch(type) {
    case PB_ADN_3G:
        pModemData = builder.BuildSimUpdatePb3gEntry(
                mode, type, recordId, pReq->GetLength(), pReq->GetEntryData());
        break;
    default:
        break;
    }

    nResult = m_pSimService->SendRequest(pModemData,
            TIMEOUT_SIM_DEFAULT, MSG_SIM_UPDATE_PB_ENTRY_DONE);

    return (nResult < 0) ? -1 : 0;
}

int UiccPhonebookHandler::OnUpdatePbEntryDone(Message *pMsg) {
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimUpdatePbEntry adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    if (uErrCode == RIL_E_SUCCESS) {
        RIL_UpdatePbRsp resp;
        resp.mode = adapter.GetMode();
        resp.type = adapter.GetPbtype();
        resp.recordId = adapter.GetRecordId();
        LOGV("mode:%d, type:%d, recordId:%d",
                resp.mode, resp.type, resp.recordId);
        m_pSimService->OnRequestComplete(RIL_E_SUCCESS, &resp, sizeof(resp));
        m_pSimService->OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED);

    } else m_pSimService->OnRequestComplete(uErrCode);

    return 0;
}

int UiccPhonebookHandler::DoGetPbStorageInfo() {
    int nResult = -1;
    SimDataBuilder simDataBuilder;

    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSimGetPbStorageInfo(PB_ADN_3G);
    nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, MSG_SIM_GET_PB_STORAGE_INFO_DONE);

    return (nResult < 0) ? -1 : 0;
}

int UiccPhonebookHandler::OnGetPbStorageInfoDone(Message *pMsg) {
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimPbStorageInfoAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    if (uErrCode == RIL_E_SUCCESS) {
        mPbCapa.maxAdnRecords = adapter.GetTotalCount();
        mPbCapa.usedAdnRecords = adapter.GetUsedCount();
        if (mPbResult.mPbRec == NULL) {
            mPbResult.mPbRec = new RIL_PhonebookRecordInfo[mPbCapa.maxAdnRecords];
        }

        LOGV("type:%d, total count:%d used count:%d ",
                adapter.GetPbType(), mPbCapa.maxAdnRecords, mPbCapa.usedAdnRecords);
        return DoGet3GPbCapa();
    } else {
        if (uErrCode == RIL_E_GENERIC_FAILURE) {
            m_pSimService->OnRequestComplete(RIL_E_INTERNAL_ERR);
        } else {
            m_pSimService->OnRequestComplete(uErrCode);
        }
    }

    return 0;
}

int UiccPhonebookHandler::DoGet3GPbCapa() {
    V2_0::RIL_CardStatus status = m_pSimService->GetRilCardStatus();
    if (RIL_CARDSTATE_PRESENT != status.card_state) {
        m_pSimService->OnRequestComplete(RIL_E_INVALID_SIM_STATE);
        return 0;
    }

    if (false == mIsSimPbSupported) {
        m_pSimService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    int nResult = -1;
    ProtocolSimBuilder builder;
    ModemData *pModemData = builder.BuildSim3GPbCapa();
    nResult = m_pSimService->SendRequest(pModemData, TIMEOUT_SIM_DEFAULT, MSG_SIM_GET_3G_PB_CAPA_DONE);

    return (nResult<0)? -1: 0;
}

int UiccPhonebookHandler::OnGet3GPbCapaDone(Message *pMsg) {
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimPbCapaAdapter adapter(pModemData);
    UINT uErrCode = adapter.GetErrorCode();

    if (uErrCode == RIL_E_SUCCESS) {
        int entry_num = adapter.GetEntryNum();
        int pb_capa[MAX_PB_ENTRY_NUM * 4] = {0};

        LOGV("EntryNum:%d", entry_num);
        adapter.GetPbCapa(pb_capa, entry_num);

        int j = 0;
        for (int i=0; i < entry_num; i++) {
            LOGV("No:%d, type:%d, index:%d, entry:%d, usedCnt:%d",
                    i, pb_capa[j], pb_capa[j+1], pb_capa[j+2], pb_capa[j+3]);
            if (pb_capa[j] == PB_NAME) {
                mPbCapa.maxNameLen = pb_capa[j + 2]; // entry length of NAME
            }

            if (pb_capa[j] == PB_NUMBER) {
                mPbCapa.maxNumberLen = pb_capa[j + 2]; // entry length of NUMBER
            }

            // make a representative of PB_EMAIL info as email capa.
            if (pb_capa[j] == PB_EMAIL) {
                mPbCapa.maxEmailRecords = pb_capa[j + 1]; // maximum index
                mPbCapa.maxEmailLen = pb_capa[j + 2]; // entry length of EF_EMAIL
                mPbCapa.usedEmailRecords = pb_capa[j + 3]; // used count
            }

            // make a representative of PB_ANR info as AdditionalNumber capa.
            if (pb_capa[j] == PB_ANR) {
                mPbCapa.maxAdditionalNumberRecords = pb_capa[j + 1]; // maximum index
                mPbCapa.maxAdditionalNumberLen = pb_capa[j + 2]; // entry length of EF_ANR
                mPbCapa.usedAdditionalNumberRecords = pb_capa[j + 3]; // used count
            }
            j +=4;
        }

        LOGV("pbcapa maxNameLen:%d, maxNumLen:%d, maxEmailRecs:%d, usedEmailRecs:%d, maxEmailLen:%d, maxAnrRecs:%d, usedAnrRecs:%d, maxAnrLen:%d",
                mPbCapa.maxNameLen, mPbCapa.maxNumberLen,
                mPbCapa.maxEmailRecords, mPbCapa.usedEmailRecords, mPbCapa.maxEmailLen,
                mPbCapa.maxAdditionalNumberRecords, mPbCapa.usedAdditionalNumberRecords,
                mPbCapa.maxAdditionalNumberLen);
    }

    return OnGetPbCapaDone(uErrCode);
}

int UiccPhonebookHandler::DoGetPbCapa(Message * /* pMsg */) {
    V2_0::RIL_CardStatus status = m_pSimService->GetRilCardStatus();
    if (RIL_CARDSTATE_PRESENT != status.card_state) {
        m_pSimService->OnRequestComplete(RIL_E_INVALID_SIM_STATE);
        return 0;
    }

    if (false == mIsSimPbSupported) {
        m_pSimService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    return DoGetPbStorageInfo();
}

int UiccPhonebookHandler::OnGetPbCapaDone(UINT uErrCode) {
    if (uErrCode == RIL_E_SUCCESS) {
        SimDataBuilder builder;
        const RilData *pRilData = builder.BuildPbCapaResponse((int *) &mPbCapa);
        m_pSimService->OnRequestComplete(RIL_E_SUCCESS, pRilData->GetData(), pRilData->GetDataLength());
    } else {
        if (uErrCode == RIL_E_GENERIC_FAILURE) {
            m_pSimService->OnRequestComplete(RIL_E_INTERNAL_ERR);
        } else {
            m_pSimService->OnRequestComplete(uErrCode);
        }
    }

    return 0;
}

int UiccPhonebookHandler::DoGetPbRec(Message * /* pMsg */) {
    V2_0::RIL_CardStatus status = m_pSimService->GetRilCardStatus();
    if (RIL_CARDSTATE_PRESENT != status.card_state) {
        m_pSimService->OnRequestComplete(RIL_E_INVALID_SIM_STATE);
        return 0;
    }

    if (false == mIsSimPbSupported) {
        m_pSimService->OnRequestComplete(RIL_E_REQUEST_NOT_SUPPORTED);
        return 0;
    }

    if (mPbCapa.maxAdnRecords <= 0) {
        m_pSimService->OnRequestComplete(RIL_E_INTERNAL_ERR);
        return 0;
    }

    LOGV("Start to load PB from SIM");

    // Start with index 1 if usedAdnRecords is upper than 0.
    mIsSimPbLoading = true;
    if (mPbCapa.usedAdnRecords > 0 && SendMessageForReadPbEntry(mRecordId) < 0) {
        LOGE("cannot send read pb entry msg");
        m_pSimService->OnRequestComplete(RIL_E_INTERNAL_ERR);
    } else {
        m_pSimService->OnRequestComplete(RIL_E_SUCCESS);
    }

    return 0;
}

int UiccPhonebookHandler::OnGetPbRecDone(int status) {
    int length = sizeof(int) + (mPbCapa.maxAdnRecords * sizeof(RIL_PhonebookRecordInfo));

    LOGV("PB Read done - receivedStatus:%d, length:%d", status, length);

    mIsSimPbLoading = false;
    mPbResult.receivedStatus = status;
    m_pSimService->OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_RECORDS_RECEIVED, &mPbResult, length);

    mRecordId = 1;
    mPbResult.loadedCount = 0;

    if (status == PB_RECEIVED_ERROR) {
        if (mPbResult.mPbRec != NULL) {
            delete[] mPbResult.mPbRec;
            mPbResult.mPbRec = NULL;
        }
        memset(&mPbCapa, 0, sizeof(SIM_PhonebookCapacity));
    }

    return 0;
}

int UiccPhonebookHandler::OnReadPbEntryTimeout(Message *pMsg) {
    NULL_RSP(pMsg);

    m_pSimService->OnRequestComplete(RIL_E_SUCCESS);

    if (mPbCapa.usedAdnRecords > 0 && mPbResult.loadedCount > 0
            && mPbResult.loadedCount < mPbCapa.usedAdnRecords) {
        LOGV("timeout occured while loading PB entries");
        OnGetPbRecDone(PB_RECEIVED_ABORT);
    }

    return 0;
}

int UiccPhonebookHandler::OnPbReady(Message *pMsg) {
    NULL_RSP(pMsg);

    ModemData *pModemData = pMsg->GetModemData();
    ProtocolSimPbReadyAdapter adapter(pModemData);
    int pb_ready = adapter.GetPbReady();
    LOGV("PB Ready indication %d", pb_ready);

    if (pb_ready == PB_READY) {
        mIsSimPbInitialized = true;
        m_pSimService->OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_PHONEBOOK_CHANGED);
        return 0;
    } else {
        // PB_DISABLED case
        mIsSimPbSupported = false;
        return 0;
    }
}

