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


#include <hidl-log.h>
#include <hidl/hidlhalindication_voice.h>
#include <librilutils/radio_hal_utils.h>
#include "ril_service_common.h"
#include "ril_service_legacy.h"

#include <librilutils/radio_conv_utils.h>

#define TAG "HidlHalVoiceIndication"

using ::android::hardware::Return;
using ::android::hardware::hidl_string;

int HidlHalVoiceIndication::callRingInd(int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responselen) {
    if (mRadioIndication != NULL) {
        bool isGsm;
        CdmaSignalInfoRecord record = {};
        if (response == NULL || responselen == 0) {
            isGsm = true;
        } else {
            isGsm = false;
            if (responselen != sizeof (RIL_CDMA_SignalInfoRecord)) {
                RLOGE("%s: invalid response", __FUNCTION__);
                return 0;
            }
            Radio2HalUtils::convertRilCdmaSignalInfoRecordToHal((RIL_CDMA_SignalInfoRecord *) response, record);
        }

#if VDBG
        RLOGD("%s: isGsm %d", __FUNCTION__, isGsm);
#endif
        Return<void> retStatus = mRadioIndication->callRing(
                convertIntToRadioIndicationType(indicationType), isGsm, record);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::callStateChangedInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->callStateChanged(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::cdmaCallWaitingInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(RIL_CDMA_CallWaiting_v6)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        CdmaCallWaiting callWaitingRecord = {};
        RIL_CDMA_CallWaiting_v6 *callWaitingRil = ((RIL_CDMA_CallWaiting_v6 *) response);
        callWaitingRecord.number = Radio2HalUtils::convertCharPtrToHidlString(callWaitingRil->number);
        callWaitingRecord.numberPresentation =
            (CdmaCallWaitingNumberPresentation) callWaitingRil->numberPresentation;
        callWaitingRecord.name = Radio2HalUtils::convertCharPtrToHidlString(callWaitingRil->name);
        Radio2HalUtils::convertRilCdmaSignalInfoRecordToHal(&callWaitingRil->signalInfoRecord,
                callWaitingRecord.signalInfoRecord);
        callWaitingRecord.numberType = (CdmaCallWaitingNumberType) callWaitingRil->number_type;
        callWaitingRecord.numberPlan = (CdmaCallWaitingNumberPlan) callWaitingRil->number_plan;

#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->cdmaCallWaiting(
                convertIntToRadioIndicationType(indicationType), callWaitingRecord);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::cdmaInfoRecInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(RIL_CDMA_InformationRecords)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        CdmaInformationRecords records = {};
        RIL_CDMA_InformationRecords *recordsRil = (RIL_CDMA_InformationRecords *) response;

        char* string8 = NULL;
        int num = MIN(recordsRil->numberOfInfoRecs, RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);
        if (recordsRil->numberOfInfoRecs > RIL_CDMA_MAX_NUMBER_OF_INFO_RECS) {
            RLOGE("%s: received %d recs which is more than %d, dropping "
                    "additional ones", __FUNCTION__, recordsRil->numberOfInfoRecs,
                    RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);
        }
        records.infoRec.resize(num);
        for (int i = 0 ; i < num ; i++) {
            CdmaInformationRecord *record = &records.infoRec[i];
            RIL_CDMA_InformationRecord *infoRec = &recordsRil->infoRec[i];
            record->name = (CdmaInfoRecName) infoRec->name;
            // All vectors should be size 0 except one which will be size 1. Set everything to
            // size 0 initially.
            record->display.resize(0);
            record->number.resize(0);
            record->signal.resize(0);
            record->redir.resize(0);
            record->lineCtrl.resize(0);
            record->clir.resize(0);
            record->audioCtrl.resize(0);
            switch (infoRec->name) {
                case RIL_CDMA_DISPLAY_INFO_REC:
                case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: {
                    if (infoRec->rec.display.alpha_len > RIL_CDMA_ALPHA_INFO_BUFFER_LENGTH) {
                        RLOGE("%s: invalid display info response length %d "
                                "expected not more than %d", __FUNCTION__, (int) infoRec->rec.display.alpha_len,
                                RIL_CDMA_ALPHA_INFO_BUFFER_LENGTH);
                        return 0;
                    }
                    string8 = (char*) malloc((infoRec->rec.display.alpha_len + 1) * sizeof(char));
                    if (string8 == NULL) {
                        RLOGE("%s: Memory allocation failed for "
                                "responseCdmaInformationRecords", __FUNCTION__);
                        return 0;
                    }
                    memcpy(string8, infoRec->rec.display.alpha_buf, infoRec->rec.display.alpha_len);
                    string8[(int)infoRec->rec.display.alpha_len] = '\0';

                    record->display.resize(1);
                    record->display[0].alphaBuf = string8;
                    free(string8);
                    string8 = NULL;
                    break;
                }

                case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
                case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
                case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: {
                    if (infoRec->rec.number.len > RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH) {
                        RLOGE("%s: invalid display info response length %d "
                                "expected not more than %d", __FUNCTION__, (int) infoRec->rec.number.len,
                                RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH);
                        return 0;
                    }
                    string8 = (char*) malloc((infoRec->rec.number.len + 1) * sizeof(char));
                    if (string8 == NULL) {
                        RLOGE("%s: Memory allocation failed for "
                                "responseCdmaInformationRecords", __FUNCTION__);
                        return 0;
                    }
                    memcpy(string8, infoRec->rec.number.buf, infoRec->rec.number.len);
                    string8[(int)infoRec->rec.number.len] = '\0';

                    record->number.resize(1);
                    record->number[0].number = string8;
                    free(string8);
                    string8 = NULL;
                    record->number[0].numberType = infoRec->rec.number.number_type;
                    record->number[0].numberPlan = infoRec->rec.number.number_plan;
                    record->number[0].pi = infoRec->rec.number.pi;
                    record->number[0].si = infoRec->rec.number.si;
                    break;
                }

                case RIL_CDMA_SIGNAL_INFO_REC: {
                    record->signal.resize(1);
                    record->signal[0].isPresent = infoRec->rec.signal.isPresent;
                    record->signal[0].signalType = infoRec->rec.signal.signalType;
                    record->signal[0].alertPitch = infoRec->rec.signal.alertPitch;
                    record->signal[0].signal = infoRec->rec.signal.signal;
                    break;
                }

                case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: {
                    if (infoRec->rec.redir.redirectingNumber.len >
                            RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH) {
                        RLOGE("%s: invalid display info response length %d "
                                "expected not more than %d\n", __FUNCTION__,
                                (int)infoRec->rec.redir.redirectingNumber.len,
                                RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH);
                        return 0;
                    }
                    string8 = (char*) malloc((infoRec->rec.redir.redirectingNumber.len + 1) *
                            sizeof(char));
                    if (string8 == NULL) {
                        RLOGE("%s: Memory allocation failed for "
                                "responseCdmaInformationRecords", __FUNCTION__);
                        return 0;
                    }
                    memcpy(string8, infoRec->rec.redir.redirectingNumber.buf,
                            infoRec->rec.redir.redirectingNumber.len);
                    string8[(int)infoRec->rec.redir.redirectingNumber.len] = '\0';

                    record->redir.resize(1);
                    record->redir[0].redirectingNumber.number = string8;
                    free(string8);
                    string8 = NULL;
                    record->redir[0].redirectingNumber.numberType =
                        infoRec->rec.redir.redirectingNumber.number_type;
                    record->redir[0].redirectingNumber.numberPlan =
                        infoRec->rec.redir.redirectingNumber.number_plan;
                    record->redir[0].redirectingNumber.pi = infoRec->rec.redir.redirectingNumber.pi;
                    record->redir[0].redirectingNumber.si = infoRec->rec.redir.redirectingNumber.si;
                    record->redir[0].redirectingReason =
                        (CdmaRedirectingReason) infoRec->rec.redir.redirectingReason;
                    break;
                }

                case RIL_CDMA_LINE_CONTROL_INFO_REC: {
                    record->lineCtrl.resize(1);
                    record->lineCtrl[0].lineCtrlPolarityIncluded =
                        infoRec->rec.lineCtrl.lineCtrlPolarityIncluded;
                    record->lineCtrl[0].lineCtrlToggle = infoRec->rec.lineCtrl.lineCtrlToggle;
                    record->lineCtrl[0].lineCtrlReverse = infoRec->rec.lineCtrl.lineCtrlReverse;
                    record->lineCtrl[0].lineCtrlPowerDenial =
                        infoRec->rec.lineCtrl.lineCtrlPowerDenial;
                    break;
                }

                case RIL_CDMA_T53_CLIR_INFO_REC: {
                    record->clir.resize(1);
                    record->clir[0].cause = infoRec->rec.clir.cause;
                    break;
                }

                case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: {
                    record->audioCtrl.resize(1);
                    record->audioCtrl[0].upLink = infoRec->rec.audioCtrl.upLink;
                    record->audioCtrl[0].downLink = infoRec->rec.audioCtrl.downLink;
                    break;
                }

                case RIL_CDMA_T53_RELEASE_INFO_REC:
                    RLOGE("%s: RIL_CDMA_T53_RELEASE_INFO_REC: INVALID", __FUNCTION__);
                    return 0;

                default:
                    RLOGE("%s: Incorrect name value", __FUNCTION__);
                    return 0;
            }
        }

#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->cdmaInfoRec(
                convertIntToRadioIndicationType(indicationType), records);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::cdmaOtaProvisionStatusInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t status = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: status %d", __FUNCTION__, status);
#endif
        Return<void> retStatus = mRadioIndication->cdmaOtaProvisionStatus(
                convertIntToRadioIndicationType(indicationType), (CdmaOtaProvisionStatus) status);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::currentEmergencyNumberListInd(int slotId, int indicationType, int token,
        RIL_Errno e, void *response, size_t responselen) {
    if (mRadioIndicationV1_4 != NULL) {
        if ((response == NULL && responselen != 0) || responselen % sizeof(RIL_EmergencyNumber) != 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        hidl_vec<V1_4::EmergencyNumber> emergencyNumberList;
        Radio2HalUtils::convertRilEmergencyNumberListToHal(response, responselen, emergencyNumberList);
        Return<void> retStatus = mRadioIndicationV1_4->currentEmergencyNumberList(
                convertIntToRadioIndicationType(indicationType), emergencyNumberList);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndicationV1_4 == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::enterEmergencyCallbackModeInd(int slotId, int indicationType, int token,
        RIL_Errno e, void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->enterEmergencyCallbackMode(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::exitEmergencyCallbackModeInd(int slotId, int indicationType, int token,
        RIL_Errno e, void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->exitEmergencyCallbackMode(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::indicateRingbackToneInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        bool start = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: start %d", __FUNCTION__, start);
#endif
        Return<void> retStatus = mRadioIndication->indicateRingbackTone(
                convertIntToRadioIndicationType(indicationType), start);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::onSupplementaryServiceIndicationInd(int slotId, int indicationType, int token,
        RIL_Errno e, void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(RIL_StkCcUnsolSsResponse)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }

        RIL_StkCcUnsolSsResponse *rilSsResponse = (RIL_StkCcUnsolSsResponse *) response;
        StkCcUnsolSsResult ss = {};
        ss.serviceType = (SsServiceType) rilSsResponse->serviceType;
        ss.requestType = (SsRequestType) rilSsResponse->requestType;
        ss.teleserviceType = (SsTeleserviceType) rilSsResponse->teleserviceType;
        ss.serviceClass = rilSsResponse->serviceClass;
        ss.result = (RadioError) rilSsResponse->result;

        if (RadioConvUtils::isServiceTypeCfQuery(rilSsResponse->serviceType, rilSsResponse->requestType)) {
#if VDBG
            RLOGD("%s CF type, num of Cf elements %d",
                    __FUNCTION__, rilSsResponse->cfData.numValidIndexes);
#endif
            if (rilSsResponse->cfData.numValidIndexes > RIL_NUM_SERVICE_CLASSES) {
                RLOGE("%s numValidIndexes is greater than "
                        "max value %d, truncating it to max value", __FUNCTION__, RIL_NUM_SERVICE_CLASSES);
                rilSsResponse->cfData.numValidIndexes = RIL_NUM_SERVICE_CLASSES;
            }

            ss.cfData.resize(1);
            ss.ssInfo.resize(0);

            /* number of call info's */
            ss.cfData[0].cfInfo.resize(rilSsResponse->cfData.numValidIndexes);

            for (int i = 0; i < rilSsResponse->cfData.numValidIndexes; i++) {
                RIL_CallForwardInfo cf = rilSsResponse->cfData.cfInfo[i];
                CallForwardInfo *cfInfo = &ss.cfData[0].cfInfo[i];

                cfInfo->status = (CallForwardInfoStatus) cf.status;
                cfInfo->reason = cf.reason;
                cfInfo->serviceClass = cf.serviceClass;
                cfInfo->toa = cf.toa;
                cfInfo->number = Radio2HalUtils::convertCharPtrToHidlString(cf.number);
                cfInfo->timeSeconds = cf.timeSeconds;
#if VDBG
                RLOGD("%s: Data: %d,reason=%d,cls=%d,toa=%d,num=%s,tout=%d],", __FUNCTION__, cf.status,
                        cf.reason, cf.serviceClass, cf.toa, (char*)cf.number, cf.timeSeconds);
#endif
            }
        } else {
            ss.ssInfo.resize(1);
            ss.cfData.resize(0);

            /* each int */
            ss.ssInfo[0].ssInfo.resize(RIL_SS_INFO_MAX);
            for (int i = 0; i < RIL_SS_INFO_MAX; i++) {
#if VDBG
                RLOGD("%s: Data: %d", __FUNCTION__, rilSsResponse->ssInfo[i]);
#endif
                ss.ssInfo[0].ssInfo[i] = rilSsResponse->ssInfo[i];
            }
        }

#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->
            onSupplementaryServiceIndication(convertIntToRadioIndicationType(indicationType), ss);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::onUssdInd(int slotId, int indicationType, int token, RIL_Errno e, void *response,
        size_t responseLen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responseLen != 2 * sizeof(char *)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        char **strings = (char **)response;
        char *mode = strings[0];
        hidl_string msg = Radio2HalUtils::convertCharPtrToHidlString(strings[1]);
        UssdModeType modeType = (UssdModeType)atoi(mode);
#if VDBG
        RLOGD("%s::%s: mode %s", TAG, __FUNCTION__, mode);
#endif
        Return<void> retStatus = mRadioIndication->onUssd(
                convertIntToRadioIndicationType(indicationType), modeType, msg);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::resendIncallMuteInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->resendIncallMute(
                convertIntToRadioIndicationType(indicationType));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::srvccStateNotifyInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t state = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: rat %d", __FUNCTION__, state);
#endif
        Return<void> retStatus = mRadioIndication->srvccStateNotify(
                convertIntToRadioIndicationType(indicationType), (SrvccState) state);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::stkCallControlAlphaNotifyInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen == 0) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
#if VDBG
        RLOGD("%s", __FUNCTION__);
#endif
        Return<void> retStatus = mRadioIndication->stkCallControlAlphaNotify(
                convertIntToRadioIndicationType(indicationType),
                Radio2HalUtils::convertCharPtrToHidlString((char *) response));
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}

int HidlHalVoiceIndication::stkCallSetupInd(int slotId, int indicationType, int token, RIL_Errno e,
        void *response, size_t responselen) {
    if (mRadioIndication != NULL) {
        if (response == NULL || responselen != sizeof(int)) {
            RLOGE("%s: invalid response", __FUNCTION__);
            return 0;
        }
        int32_t timeout = ((int32_t *) response)[0];
#if VDBG
        RLOGD("%s: timeout %d", __FUNCTION__, timeout);
#endif
        Return<void> retStatus = mRadioIndication->stkCallSetup(
                convertIntToRadioIndicationType(indicationType), timeout);
        checkReturnStatus(retStatus);
    } else {
        RLOGE("%s: radioService[%d]->mRadioIndication == NULL", __FUNCTION__, slotId);
    }

    return 0;
}
