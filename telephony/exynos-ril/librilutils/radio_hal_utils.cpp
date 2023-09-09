/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <librilutils/radio_hal_utils.h>
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include <telephony/ril_mcc.h>
#include <telephony/ril_mnc.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "Radio2HalUtils"
#endif
#include <utils/Log.h>
#include <vector>

using namespace std;

namespace Radio2HalUtils {

hidl_string convertCharPtrToHidlString(const char *ptr) {
    hidl_string ret;
    if (ptr != NULL) {
        // TODO: replace this with strnlen
        ret.setToExternal(ptr, strlen(ptr));
    }
    return ret;
}

void parseStrings(vector<string>&dst1, hidl_vec<hidl_string>&dst2, char *str)
{
    string src(str ? str : "");
    stringstream ss(src);
    copy(istream_iterator<string>(ss),
         istream_iterator<string>(),
         back_inserter(dst1));

    dst2.resize(dst1.size());
    for(size_t i = 0; i < dst1.size(); i++)
    {
        dst2[i] = convertCharPtrToHidlString(dst1[i].c_str());
    }
}

void convertLinkAddress(vector<string>&container, hidl_vec<V1_5::LinkAddress>&dst, char *str)
{
    string src(str ? str : "");
    string ip;
    stringstream ss(src);
    while (getline(ss, ip, ' '))
        container.push_back(ip);
    /*
    copy(istream_iterator<string>(ss),
         istream_iterator<string>(),
         back_inserter(container));
         */

    dst.resize(container.size());
    RLOGD("container size:%lu", (unsigned long)container.size());
    for(size_t i = 0; i < container.size(); i++)
    {
        RLOGD("container[%zu] = %s", i, container[i].c_str());
        dst[i].address = convertCharPtrToHidlString(container[i].c_str());
        dst[i].properties = NONE;
        dst[i].deprecationTime = -1;
        dst[i].expirationTime = -1;
    }
}

int convertResponseStringEntryToInt(char **response, int index, int numStrings) {
    if ((response != NULL) &&  (numStrings > index) && (response[index] != NULL)) {
        return strtol(response[index], NULL, 10);
    }

    return -1;
}

int convertResponseHexStringEntryToInt(char **response, int index, int numStrings) {
    const int hexBase = 16;
    if ((response != NULL) &&  (numStrings > index) && (response[index] != NULL)) {
        return strtol(response[index], NULL, hexBase);
    }

    return -1;
}

int convertOperatorStatusToInt(const char *str) {
    if (str != nullptr) {
        if (strncmp("unknown", str, 9) == 0) {
            return (int) OperatorStatus::UNKNOWN;
        } else if (strncmp("available", str, 9) == 0) {
            return (int) OperatorStatus::AVAILABLE;
        } else if (strncmp("current", str, 9) == 0) {
            return (int) OperatorStatus::CURRENT;
        } else if (strncmp("forbidden", str, 9) == 0) {
            return (int) OperatorStatus::FORBIDDEN;
        }
    }
    return -1;
}

///////////////////////////////////////////////////////////////////////////
// radio 1.0  RIL => HAL convert function
///////////////////////////////////////////////////////////////////////////
bool convertRilCardStatusToHal(RIL_CardStatus_v6 *cardStatus, size_t cardStatusLen, CardStatus& out) {
    if (cardStatus == NULL || cardStatusLen < sizeof(RIL_CardStatus_v6)) {
        return false;
    }

    if (cardStatus->gsm_umts_subscription_app_index >= cardStatus->num_applications
            || cardStatus->cdma_subscription_app_index >= cardStatus->num_applications
            || cardStatus->ims_subscription_app_index >= cardStatus->num_applications) {
        RLOGE("convertRilCardStatusToHal: Invalid response");
        return false;
    }

    out.cardState = (CardState) cardStatus->card_state;
    out.universalPinState = (PinState) cardStatus->universal_pin_state;
    out.gsmUmtsSubscriptionAppIndex = cardStatus->gsm_umts_subscription_app_index;
    out.cdmaSubscriptionAppIndex = cardStatus->cdma_subscription_app_index;
    out.imsSubscriptionAppIndex = cardStatus->ims_subscription_app_index;

    const RIL_AppStatus *rilAppStatus = cardStatus->applications;
    out.applications.resize(cardStatus->num_applications);
    AppStatus *appStatus = out.applications.data();

#if VDBG
    RLOGD("convertRilCardStatusToHal: num_applications %d", cardStatus->num_applications);
#endif
    for (int i = 0; i < cardStatus->num_applications; i++) {
        appStatus[i].appType = (AppType) rilAppStatus[i].app_type;
        appStatus[i].appState = (AppState) rilAppStatus[i].app_state;
        appStatus[i].persoSubstate = (PersoSubstate) rilAppStatus[i].perso_substate;
        appStatus[i].aidPtr = convertCharPtrToHidlString(rilAppStatus[i].aid_ptr);
        appStatus[i].appLabelPtr = convertCharPtrToHidlString(
                rilAppStatus[i].app_label_ptr);
        appStatus[i].pin1Replaced = rilAppStatus[i].pin1_replaced;
        appStatus[i].pin1 = (PinState) rilAppStatus[i].pin1;
        appStatus[i].pin2 = (PinState) rilAppStatus[i].pin2;
    }

    return true;
}

bool convertRilCallToHal(RIL_Call *rilCall, size_t rilCallLen, Call& call) {
    if (rilCall == NULL || rilCallLen != sizeof(RIL_Call)) {
        return false;
    }

    call.state = (CallState) rilCall->state;
    call.index = rilCall->index;
    call.toa = rilCall->toa;
    call.isMpty = rilCall->isMpty;
    call.isMT = rilCall->isMT;
    call.als = rilCall->als;
    call.isVoice = rilCall->isVoice;
    call.isVoicePrivacy = rilCall->isVoicePrivacy;
    call.number = convertCharPtrToHidlString(rilCall->number);
    call.numberPresentation = (CallPresentation) rilCall->numberPresentation;
    call.name = convertCharPtrToHidlString(rilCall->name);
    call.namePresentation = (CallPresentation) rilCall->namePresentation;
    if (rilCall->uusInfo != NULL && rilCall->uusInfo->uusData != NULL) {
        RIL_UUS_Info *uusInfo = rilCall->uusInfo;
        call.uusInfo.resize(1);
        call.uusInfo[0].uusType = (UusType) uusInfo->uusType;
        call.uusInfo[0].uusDcs = (UusDcs) uusInfo->uusDcs;
        // convert uusInfo->uusData to a null-terminated string
        char *nullTermStr = strndup(uusInfo->uusData, uusInfo->uusLength);
        call.uusInfo[0].uusData = nullTermStr;
        free(nullTermStr);
    }
    return true;
}

void fillCellIdentityResponse(CellIdentity &cellIdentity, RIL_CellIdentity_v16 &rilCellIdentity) {

    cellIdentity.cellIdentityGsm.resize(0);
    cellIdentity.cellIdentityWcdma.resize(0);
    cellIdentity.cellIdentityCdma.resize(0);
    cellIdentity.cellIdentityTdscdma.resize(0);
    cellIdentity.cellIdentityLte.resize(0);
    cellIdentity.cellInfoType = (CellInfoType)rilCellIdentity.cellInfoType;
    switch(rilCellIdentity.cellInfoType) {

        case RIL_CELL_INFO_TYPE_GSM: {
            cellIdentity.cellIdentityGsm.resize(1);
            cellIdentity.cellIdentityGsm[0].mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityGsm.mcc);
            cellIdentity.cellIdentityGsm[0].mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityGsm.mnc);
            cellIdentity.cellIdentityGsm[0].lac = rilCellIdentity.cellIdentityGsm.lac;
            cellIdentity.cellIdentityGsm[0].cid = rilCellIdentity.cellIdentityGsm.cid;
            cellIdentity.cellIdentityGsm[0].arfcn = rilCellIdentity.cellIdentityGsm.arfcn;
            cellIdentity.cellIdentityGsm[0].bsic = rilCellIdentity.cellIdentityGsm.bsic;
            break;
        }

        case RIL_CELL_INFO_TYPE_WCDMA: {
            cellIdentity.cellIdentityWcdma.resize(1);
            cellIdentity.cellIdentityWcdma[0].mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityWcdma.mcc);
            cellIdentity.cellIdentityWcdma[0].mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityWcdma.mnc);
            cellIdentity.cellIdentityWcdma[0].lac = rilCellIdentity.cellIdentityWcdma.lac;
            cellIdentity.cellIdentityWcdma[0].cid = rilCellIdentity.cellIdentityWcdma.cid;
            cellIdentity.cellIdentityWcdma[0].psc = rilCellIdentity.cellIdentityWcdma.psc;
            cellIdentity.cellIdentityWcdma[0].uarfcn = rilCellIdentity.cellIdentityWcdma.uarfcn;
            break;
        }

        case RIL_CELL_INFO_TYPE_CDMA: {
            cellIdentity.cellIdentityCdma.resize(1);
            cellIdentity.cellIdentityCdma[0].networkId = rilCellIdentity.cellIdentityCdma.networkId;
            cellIdentity.cellIdentityCdma[0].systemId = rilCellIdentity.cellIdentityCdma.systemId;
            cellIdentity.cellIdentityCdma[0].baseStationId =
                    rilCellIdentity.cellIdentityCdma.basestationId;
            cellIdentity.cellIdentityCdma[0].longitude = rilCellIdentity.cellIdentityCdma.longitude;
            cellIdentity.cellIdentityCdma[0].latitude = rilCellIdentity.cellIdentityCdma.latitude;
            break;
        }

        case RIL_CELL_INFO_TYPE_LTE: {
            cellIdentity.cellIdentityLte.resize(1);
            cellIdentity.cellIdentityLte[0].mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityLte.mcc);
            cellIdentity.cellIdentityLte[0].mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityLte.mnc);
            cellIdentity.cellIdentityLte[0].ci = rilCellIdentity.cellIdentityLte.ci;
            cellIdentity.cellIdentityLte[0].pci = rilCellIdentity.cellIdentityLte.pci;
            cellIdentity.cellIdentityLte[0].tac = rilCellIdentity.cellIdentityLte.tac;
            cellIdentity.cellIdentityLte[0].earfcn = rilCellIdentity.cellIdentityLte.earfcn;
            break;
        }

        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            cellIdentity.cellIdentityTdscdma.resize(1);
            cellIdentity.cellIdentityTdscdma[0].mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityTdscdma.mcc);
            cellIdentity.cellIdentityTdscdma[0].mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityTdscdma.mnc);
            cellIdentity.cellIdentityTdscdma[0].lac = rilCellIdentity.cellIdentityTdscdma.lac;
            cellIdentity.cellIdentityTdscdma[0].cid = rilCellIdentity.cellIdentityTdscdma.cid;
            cellIdentity.cellIdentityTdscdma[0].cpid = rilCellIdentity.cellIdentityTdscdma.cpid;
            break;
        }

        default: {
            break;
        }
    }
}

void convertRilSignalStrengthToHal(void *response, size_t responseLen,
        SignalStrength& signalStrength) {
    if (response && responseLen >= sizeof(RIL_SignalStrength_v10)) {
        RIL_SignalStrength_v10 *rilSignalStrength = (RIL_SignalStrength_v10 *) response;

        // Fixup LTE for backwards compatibility
        // signalStrength: -1 -> 99
        if (rilSignalStrength->LTE_SignalStrength.signalStrength == -1) {
            rilSignalStrength->LTE_SignalStrength.signalStrength = 99;
        }
        // rsrp: -1 -> INT_MAX all other negative value to positive.
        // So remap here
        if (rilSignalStrength->LTE_SignalStrength.rsrp == -1) {
            rilSignalStrength->LTE_SignalStrength.rsrp = INT_MAX;
        } else if (rilSignalStrength->LTE_SignalStrength.rsrp < -1) {
            rilSignalStrength->LTE_SignalStrength.rsrp = -rilSignalStrength->LTE_SignalStrength.rsrp;
        }
        // rsrq: -1 -> INT_MAX
        if (rilSignalStrength->LTE_SignalStrength.rsrq == -1) {
            rilSignalStrength->LTE_SignalStrength.rsrq = INT_MAX;
        }
        // Not remapping rssnr is already using INT_MAX
        // cqi: -1 -> INT_MAX
        if (rilSignalStrength->LTE_SignalStrength.cqi == -1) {
            rilSignalStrength->LTE_SignalStrength.cqi = INT_MAX;
        }

        signalStrength.gw.signalStrength = rilSignalStrength->GW_SignalStrength.signalStrength;
        signalStrength.gw.bitErrorRate = rilSignalStrength->GW_SignalStrength.bitErrorRate;
        // RIL_SignalStrength_v10 not support gw.timingAdvance. Set to INT_MAX as
        // invalid value.
        signalStrength.gw.timingAdvance = INT_MAX;

        signalStrength.cdma.dbm = rilSignalStrength->CDMA_SignalStrength.dbm;
        signalStrength.cdma.ecio = rilSignalStrength->CDMA_SignalStrength.ecio;
        signalStrength.evdo.dbm = rilSignalStrength->EVDO_SignalStrength.dbm;
        signalStrength.evdo.ecio = rilSignalStrength->EVDO_SignalStrength.ecio;
        signalStrength.evdo.signalNoiseRatio =
                rilSignalStrength->EVDO_SignalStrength.signalNoiseRatio;
        signalStrength.lte.signalStrength = rilSignalStrength->LTE_SignalStrength.signalStrength;
        signalStrength.lte.rsrp = rilSignalStrength->LTE_SignalStrength.rsrp;
        signalStrength.lte.rsrq = rilSignalStrength->LTE_SignalStrength.rsrq;
        signalStrength.lte.rssnr = rilSignalStrength->LTE_SignalStrength.rssnr;
        signalStrength.lte.cqi = rilSignalStrength->LTE_SignalStrength.cqi;
        signalStrength.lte.timingAdvance = rilSignalStrength->LTE_SignalStrength.timingAdvance;
        signalStrength.tdScdma.rscp = rilSignalStrength->TD_SCDMA_SignalStrength.rscp;

    }
}

void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<CellInfo>& records) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_CellInfo_v12);
    records.resize(num);

    RIL_CellInfo_v12 *rillCellInfo = (RIL_CellInfo_v12 *) response;
    for (int i = 0; i < num; i++) {
        records[i].cellInfoType = (CellInfoType) rillCellInfo->cellInfoType;
        records[i].registered = rillCellInfo->registered;
        records[i].timeStampType = (TimeStampType) rillCellInfo->timeStampType;
        records[i].timeStamp = rillCellInfo->timeStamp;
        // All vectors should be size 0 except one which will be size 1. Set everything to
        // size 0 initially.
        records[i].gsm.resize(0);
        records[i].wcdma.resize(0);
        records[i].cdma.resize(0);
        records[i].lte.resize(0);
        records[i].tdscdma.resize(0);
        switch(rillCellInfo->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                records[i].gsm.resize(1);
                CellInfoGsm *cellInfoGsm = &records[i].gsm[0];
                cellInfoGsm->cellIdentityGsm.mcc =
                        ril::util::mcc::decode(rillCellInfo->CellInfo.gsm.cellIdentityGsm.mcc);
                cellInfoGsm->cellIdentityGsm.mnc =
                        ril::util::mnc::decode(rillCellInfo->CellInfo.gsm.cellIdentityGsm.mnc);
                cellInfoGsm->cellIdentityGsm.lac =
                        rillCellInfo->CellInfo.gsm.cellIdentityGsm.lac;
                cellInfoGsm->cellIdentityGsm.cid =
                        rillCellInfo->CellInfo.gsm.cellIdentityGsm.cid;
                cellInfoGsm->cellIdentityGsm.arfcn =
                        rillCellInfo->CellInfo.gsm.cellIdentityGsm.arfcn;
                cellInfoGsm->cellIdentityGsm.bsic =
                        rillCellInfo->CellInfo.gsm.cellIdentityGsm.bsic;
                cellInfoGsm->signalStrengthGsm.signalStrength =
                        rillCellInfo->CellInfo.gsm.signalStrengthGsm.signalStrength;
                cellInfoGsm->signalStrengthGsm.bitErrorRate =
                        rillCellInfo->CellInfo.gsm.signalStrengthGsm.bitErrorRate;
                cellInfoGsm->signalStrengthGsm.timingAdvance =
                        rillCellInfo->CellInfo.gsm.signalStrengthGsm.timingAdvance;
                break;
            }

            case RIL_CELL_INFO_TYPE_WCDMA: {
                records[i].wcdma.resize(1);
                CellInfoWcdma *cellInfoWcdma = &records[i].wcdma[0];
                cellInfoWcdma->cellIdentityWcdma.mcc =
                        ril::util::mcc::decode(rillCellInfo->CellInfo.wcdma.cellIdentityWcdma.mcc);
                cellInfoWcdma->cellIdentityWcdma.mnc =
                        ril::util::mnc::decode(rillCellInfo->CellInfo.wcdma.cellIdentityWcdma.mnc);
                cellInfoWcdma->cellIdentityWcdma.lac =
                        rillCellInfo->CellInfo.wcdma.cellIdentityWcdma.lac;
                cellInfoWcdma->cellIdentityWcdma.cid =
                        rillCellInfo->CellInfo.wcdma.cellIdentityWcdma.cid;
                cellInfoWcdma->cellIdentityWcdma.psc =
                        rillCellInfo->CellInfo.wcdma.cellIdentityWcdma.psc;
                cellInfoWcdma->cellIdentityWcdma.uarfcn =
                        rillCellInfo->CellInfo.wcdma.cellIdentityWcdma.uarfcn;
                cellInfoWcdma->signalStrengthWcdma.signalStrength =
                        rillCellInfo->CellInfo.wcdma.signalStrengthWcdma.signalStrength;
                cellInfoWcdma->signalStrengthWcdma.bitErrorRate =
                        rillCellInfo->CellInfo.wcdma.signalStrengthWcdma.bitErrorRate;
                break;
            }

            case RIL_CELL_INFO_TYPE_CDMA: {
                records[i].cdma.resize(1);
                CellInfoCdma *cellInfoCdma = &records[i].cdma[0];
                cellInfoCdma->cellIdentityCdma.networkId =
                        rillCellInfo->CellInfo.cdma.cellIdentityCdma.networkId;
                cellInfoCdma->cellIdentityCdma.systemId =
                        rillCellInfo->CellInfo.cdma.cellIdentityCdma.systemId;
                cellInfoCdma->cellIdentityCdma.baseStationId =
                        rillCellInfo->CellInfo.cdma.cellIdentityCdma.basestationId;
                cellInfoCdma->cellIdentityCdma.longitude =
                        rillCellInfo->CellInfo.cdma.cellIdentityCdma.longitude;
                cellInfoCdma->cellIdentityCdma.latitude =
                        rillCellInfo->CellInfo.cdma.cellIdentityCdma.latitude;
                cellInfoCdma->signalStrengthCdma.dbm =
                        rillCellInfo->CellInfo.cdma.signalStrengthCdma.dbm;
                cellInfoCdma->signalStrengthCdma.ecio =
                        rillCellInfo->CellInfo.cdma.signalStrengthCdma.ecio;
                cellInfoCdma->signalStrengthEvdo.dbm =
                        rillCellInfo->CellInfo.cdma.signalStrengthEvdo.dbm;
                cellInfoCdma->signalStrengthEvdo.ecio =
                        rillCellInfo->CellInfo.cdma.signalStrengthEvdo.ecio;
                cellInfoCdma->signalStrengthEvdo.signalNoiseRatio =
                        rillCellInfo->CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio;
                break;
            }

            case RIL_CELL_INFO_TYPE_LTE: {
                records[i].lte.resize(1);
                CellInfoLte *cellInfoLte = &records[i].lte[0];
                cellInfoLte->cellIdentityLte.mcc =
                        ril::util::mcc::decode(rillCellInfo->CellInfo.lte.cellIdentityLte.mcc);
                cellInfoLte->cellIdentityLte.mnc =
                        ril::util::mnc::decode(rillCellInfo->CellInfo.lte.cellIdentityLte.mnc);
                cellInfoLte->cellIdentityLte.ci =
                        rillCellInfo->CellInfo.lte.cellIdentityLte.ci;
                cellInfoLte->cellIdentityLte.pci =
                        rillCellInfo->CellInfo.lte.cellIdentityLte.pci;
                cellInfoLte->cellIdentityLte.tac =
                        rillCellInfo->CellInfo.lte.cellIdentityLte.tac;
                cellInfoLte->cellIdentityLte.earfcn =
                        rillCellInfo->CellInfo.lte.cellIdentityLte.earfcn;
                cellInfoLte->signalStrengthLte.signalStrength =
                        rillCellInfo->CellInfo.lte.signalStrengthLte.signalStrength;
                cellInfoLte->signalStrengthLte.rsrp =
                        rillCellInfo->CellInfo.lte.signalStrengthLte.rsrp;
                cellInfoLte->signalStrengthLte.rsrq =
                        rillCellInfo->CellInfo.lte.signalStrengthLte.rsrq;
                cellInfoLte->signalStrengthLte.rssnr =
                        rillCellInfo->CellInfo.lte.signalStrengthLte.rssnr;
                cellInfoLte->signalStrengthLte.cqi =
                        rillCellInfo->CellInfo.lte.signalStrengthLte.cqi;
                cellInfoLte->signalStrengthLte.timingAdvance =
                        rillCellInfo->CellInfo.lte.signalStrengthLte.timingAdvance;
                break;
            }

            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                records[i].tdscdma.resize(1);
                CellInfoTdscdma *cellInfoTdscdma = &records[i].tdscdma[0];
                cellInfoTdscdma->cellIdentityTdscdma.mcc =
                        ril::util::mcc::decode(rillCellInfo->CellInfo.tdscdma.cellIdentityTdscdma.mcc);
                cellInfoTdscdma->cellIdentityTdscdma.mnc =
                        ril::util::mnc::decode(
                                rillCellInfo->CellInfo.tdscdma.cellIdentityTdscdma.mnc);
                cellInfoTdscdma->cellIdentityTdscdma.lac =
                        rillCellInfo->CellInfo.tdscdma.cellIdentityTdscdma.lac;
                cellInfoTdscdma->cellIdentityTdscdma.cid =
                        rillCellInfo->CellInfo.tdscdma.cellIdentityTdscdma.cid;
                cellInfoTdscdma->cellIdentityTdscdma.cpid =
                        rillCellInfo->CellInfo.tdscdma.cellIdentityTdscdma.cpid;
                cellInfoTdscdma->signalStrengthTdscdma.rscp =
                        rillCellInfo->CellInfo.tdscdma.signalStrengthTdscdma.rscp;
                break;
            }
            default: {
                break;
            }
        }
        rillCellInfo += 1;
    }
}

void convertRilKeepaliveStatusToHal(const RIL_KeepaliveStatus *rilStatus,
        V1_1::KeepaliveStatus& halStatus) {
    if (rilStatus) {
        halStatus.sessionHandle = rilStatus->sessionHandle;
        halStatus.code = static_cast<V1_1::KeepaliveStatusCode>(rilStatus->code);
    }
}

void convertRilDataCallToHal(RIL_Data_Call_Response_v11 *dcResponse,
        SetupDataCallResult& dcResult) {
    if (dcResponse) {
        dcResult.status = (DataCallFailCause) dcResponse->status;
        dcResult.suggestedRetryTime = dcResponse->suggestedRetryTime;
        dcResult.cid = dcResponse->cid;
        dcResult.active = dcResponse->active;
        dcResult.type = convertCharPtrToHidlString(dcResponse->type);
        dcResult.ifname = convertCharPtrToHidlString(dcResponse->ifname);
        dcResult.addresses = convertCharPtrToHidlString(dcResponse->addresses);
        dcResult.dnses = convertCharPtrToHidlString(dcResponse->dnses);
        dcResult.gateways = convertCharPtrToHidlString(dcResponse->gateways);
        dcResult.pcscf = convertCharPtrToHidlString(dcResponse->pcscf);
        dcResult.mtu = dcResponse->mtu;
    }
}

void convertRilDataCallListToHal(void *response, size_t responseLen,
        hidl_vec<SetupDataCallResult>& dcResultList) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_Data_Call_Response_v11);

    RIL_Data_Call_Response_v11 *dcResponse = (RIL_Data_Call_Response_v11 *) response;
    dcResultList.resize(num);
    for (int i = 0; i < num; i++) {
        convertRilDataCallToHal(&dcResponse[i], dcResultList[i]);
    }
}

void convertRilCdmaSignalInfoRecordToHal(RIL_CDMA_SignalInfoRecord *signalInfoRecord, CdmaSignalInfoRecord& record) {
    if (signalInfoRecord) {
        record.isPresent = signalInfoRecord->isPresent;
        record.signalType = signalInfoRecord->signalType;
        record.alertPitch = signalInfoRecord->alertPitch;
        record.signal = signalInfoRecord->signal;
    }
}


void convertRilHardwareConfigListToHal(void *response, size_t responseLen,
        hidl_vec<HardwareConfig>& records) {
    int num = response ? responseLen / sizeof(RIL_HardwareConfig) : 0;
    records.resize(num);

    RIL_HardwareConfig *rilHardwareConfig = (RIL_HardwareConfig *) response;
    for (int i = 0; i < num; i++) {
        records[i].type = (HardwareConfigType) rilHardwareConfig[i].type;
        records[i].uuid = convertCharPtrToHidlString(rilHardwareConfig[i].uuid);
        records[i].state = (HardwareConfigState) rilHardwareConfig[i].state;
        switch (rilHardwareConfig[i].type) {
            case RIL_HARDWARE_CONFIG_MODEM: {
                records[i].modem.resize(1);
                records[i].sim.resize(0);
                HardwareConfigModem *hwConfigModem = &records[i].modem[0];
                hwConfigModem->rilModel = rilHardwareConfig[i].cfg.modem.rilModel;
                hwConfigModem->rat = rilHardwareConfig[i].cfg.modem.rat;
                hwConfigModem->maxVoice = rilHardwareConfig[i].cfg.modem.maxVoice;
                hwConfigModem->maxData = rilHardwareConfig[i].cfg.modem.maxData;
                hwConfigModem->maxStandby = rilHardwareConfig[i].cfg.modem.maxStandby;
                break;
            }

            case RIL_HARDWARE_CONFIG_SIM: {
                records[i].sim.resize(1);
                records[i].modem.resize(0);
                records[i].sim[0].modemUuid =
                        convertCharPtrToHidlString(rilHardwareConfig[i].cfg.sim.modemUuid);
                break;
            }
        }
    }
}

void convertRilRadioCapabilityToHal(void *response, size_t /* responseLen */, RadioCapability &rc) {
    if (response) {
        RIL_RadioCapability *rilRadioCapability = (RIL_RadioCapability *) response;
        rc.session = rilRadioCapability->session;
        rc.phase = (V1_0::RadioCapabilityPhase) rilRadioCapability->phase;
        rc.raf = rilRadioCapability->rat;
        rc.logicalModemUuid = convertCharPtrToHidlString(rilRadioCapability->logicalModemUuid);
        rc.status = (V1_0::RadioCapabilityStatus) rilRadioCapability->status;
    }
}

void convertRilLceDataInfoToHal(void *response, size_t /* responseLen */, LceDataInfo &lce) {
    if (response) {
        RIL_LceDataInfo *rilLceDataInfo = (RIL_LceDataInfo *)response;
        lce.lastHopCapacityKbps = rilLceDataInfo->last_hop_capacity_kbps;
        lce.confidenceLevel = rilLceDataInfo->confidence_level;
        lce.lceSuspended = rilLceDataInfo->lce_suspended;
    }
}

///////////////////////////////////////////////////////////////////////////
// radio 1.2  RIL => HAL convert function
///////////////////////////////////////////////////////////////////////////
bool convertRilCardStatusToHal(RIL_CardStatus_V1_2 *cardStatus, size_t cardStatusLen, V1_2::CardStatus& out) {
    if (cardStatus == NULL || cardStatusLen != sizeof(RIL_CardStatus_V1_2)) {
        return false;
    }

    if (!convertRilCardStatusToHal((RIL_CardStatus_v6 *)cardStatus, sizeof(RIL_CardStatus_v6), out.base)) {
        return false;
    }

    out.physicalSlotId = cardStatus->physicalSlotId;
    out.atr = convertCharPtrToHidlString((char *) cardStatus->atr);
    out.iccid = convertCharPtrToHidlString((char *) cardStatus->iccid);

    return true;
}

bool convertRilCallToHal(RIL_Call_V1_2 *rilCall, size_t rilCallLen, V1_2::Call& call) {
    if (rilCall == NULL || rilCallLen != sizeof(RIL_Call_V1_2)) {
        return false;
    }

    if (!convertRilCallToHal((RIL_Call *)rilCall, sizeof(RIL_Call), call.base)) {
        return false;
    }

    call.audioQuality = (V1_2::AudioQuality) rilCall->audioQuality;
    return true;
}

void fillCellIdentityResponse(V1_2::CellIdentity &cellIdentity, RIL_CellIdentity_V1_2 &rilCellIdentity) {

    cellIdentity.cellIdentityGsm.resize(0);
    cellIdentity.cellIdentityWcdma.resize(0);
    cellIdentity.cellIdentityCdma.resize(0);
    cellIdentity.cellIdentityTdscdma.resize(0);
    cellIdentity.cellIdentityLte.resize(0);
    cellIdentity.cellInfoType = (CellInfoType)rilCellIdentity.cellInfoType;
    switch(rilCellIdentity.cellInfoType) {

        case RIL_CELL_INFO_TYPE_GSM: {
            cellIdentity.cellIdentityGsm.resize(1);
            cellIdentity.cellIdentityGsm[0].base.mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityGsm.mcc);
            cellIdentity.cellIdentityGsm[0].base.mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityGsm.mnc);
            cellIdentity.cellIdentityGsm[0].base.lac = rilCellIdentity.cellIdentityGsm.lac;
            cellIdentity.cellIdentityGsm[0].base.cid = rilCellIdentity.cellIdentityGsm.cid;
            cellIdentity.cellIdentityGsm[0].base.arfcn = rilCellIdentity.cellIdentityGsm.arfcn;
            cellIdentity.cellIdentityGsm[0].base.bsic = rilCellIdentity.cellIdentityGsm.bsic;
            cellIdentity.cellIdentityGsm[0].operatorNames.alphaLong
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityGsm.operatorNames.alphaLong);
            cellIdentity.cellIdentityGsm[0].operatorNames.alphaShort
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityGsm.operatorNames.alphaShort);
            break;
        }

        case RIL_CELL_INFO_TYPE_WCDMA: {
            cellIdentity.cellIdentityWcdma.resize(1);
            cellIdentity.cellIdentityWcdma[0].base.mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityWcdma.mcc);
            cellIdentity.cellIdentityWcdma[0].base.mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityWcdma.mnc);
            cellIdentity.cellIdentityWcdma[0].base.lac = rilCellIdentity.cellIdentityWcdma.lac;
            cellIdentity.cellIdentityWcdma[0].base.cid = rilCellIdentity.cellIdentityWcdma.cid;
            cellIdentity.cellIdentityWcdma[0].base.psc = rilCellIdentity.cellIdentityWcdma.psc;
            cellIdentity.cellIdentityWcdma[0].base.uarfcn = rilCellIdentity.cellIdentityWcdma.uarfcn;
            cellIdentity.cellIdentityWcdma[0].operatorNames.alphaLong
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityWcdma.operatorNames.alphaLong);
            cellIdentity.cellIdentityWcdma[0].operatorNames.alphaShort
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityWcdma.operatorNames.alphaShort);
            break;
        }

        case RIL_CELL_INFO_TYPE_CDMA: {
            cellIdentity.cellIdentityCdma.resize(1);
            cellIdentity.cellIdentityCdma[0].base.networkId = rilCellIdentity.cellIdentityCdma.networkId;
            cellIdentity.cellIdentityCdma[0].base.systemId = rilCellIdentity.cellIdentityCdma.systemId;
            cellIdentity.cellIdentityCdma[0].base.baseStationId =
                    rilCellIdentity.cellIdentityCdma.basestationId;
            cellIdentity.cellIdentityCdma[0].base.longitude = rilCellIdentity.cellIdentityCdma.longitude;
            cellIdentity.cellIdentityCdma[0].base.latitude = rilCellIdentity.cellIdentityCdma.latitude;
            cellIdentity.cellIdentityCdma[0].operatorNames.alphaLong
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityCdma.operatorNames.alphaLong);
            cellIdentity.cellIdentityCdma[0].operatorNames.alphaShort
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityCdma.operatorNames.alphaShort);
            break;
        }

        case RIL_CELL_INFO_TYPE_LTE: {
            cellIdentity.cellIdentityLte.resize(1);
            cellIdentity.cellIdentityLte[0].base.mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityLte.mcc);
            cellIdentity.cellIdentityLte[0].base.mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityLte.mnc);
            cellIdentity.cellIdentityLte[0].base.ci = rilCellIdentity.cellIdentityLte.ci;
            cellIdentity.cellIdentityLte[0].base.pci = rilCellIdentity.cellIdentityLte.pci;
            cellIdentity.cellIdentityLte[0].base.tac = rilCellIdentity.cellIdentityLte.tac;
            cellIdentity.cellIdentityLte[0].base.earfcn = rilCellIdentity.cellIdentityLte.earfcn;
            cellIdentity.cellIdentityLte[0].operatorNames.alphaLong
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityLte.operatorNames.alphaLong);
            cellIdentity.cellIdentityLte[0].operatorNames.alphaShort
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityLte.operatorNames.alphaShort);
            cellIdentity.cellIdentityLte[0].bandwidth = rilCellIdentity.cellIdentityLte.bandwidth;
            break;
        }

        case RIL_CELL_INFO_TYPE_TD_SCDMA: {
            cellIdentity.cellIdentityTdscdma.resize(1);
            cellIdentity.cellIdentityTdscdma[0].base.mcc =
                    ril::util::mcc::decode(rilCellIdentity.cellIdentityTdscdma.mcc);
            cellIdentity.cellIdentityTdscdma[0].base.mnc =
                    ril::util::mnc::decode(rilCellIdentity.cellIdentityTdscdma.mnc);
            cellIdentity.cellIdentityTdscdma[0].base.lac = rilCellIdentity.cellIdentityTdscdma.lac;
            cellIdentity.cellIdentityTdscdma[0].base.cid = rilCellIdentity.cellIdentityTdscdma.cid;
            cellIdentity.cellIdentityTdscdma[0].base.cpid = rilCellIdentity.cellIdentityTdscdma.cpid;
            cellIdentity.cellIdentityTdscdma[0].uarfcn = rilCellIdentity.cellIdentityTdscdma.uarfcn;
            cellIdentity.cellIdentityTdscdma[0].operatorNames.alphaLong
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityTdscdma.operatorNames.alphaLong);
            cellIdentity.cellIdentityTdscdma[0].operatorNames.alphaShort
                = convertCharPtrToHidlString(rilCellIdentity.cellIdentityTdscdma.operatorNames.alphaShort);
            break;
        }

        default: {
            break;
        }
    }
}


bool convertRilDataRegStateToHal(RIL_DataRegistrationStateResponse_V1_2 *dataRegState, size_t responselen,
                                        V1_2::DataRegStateResult& dataRegResponse) {
    if (dataRegState == NULL ||
        responselen != sizeof(RIL_DataRegistrationStateResponse_V1_2)) {
        RLOGE("convertRilDataRegStateToHal Invalid response: NULL");
        return false;
    }

    dataRegResponse.regState = (RegState) dataRegState->regState;
    dataRegResponse.rat = dataRegState->rat;;
    dataRegResponse.reasonDataDenied = dataRegState->reasonDataDenied;
    dataRegResponse.maxDataCalls = dataRegState->maxDataCalls;
    fillCellIdentityResponse(dataRegResponse.cellIdentity, dataRegState->cellIdentity);
    return true;
}

template<typename S, typename T>
void convertRilSignalStrengthToHal_V2_V4_Common(S & rilSignalStrength, T & signalStrength) {
    // Fixup LTE for backwards compatibility
    // signalStrength: -1 -> 99
    if (rilSignalStrength->LTE_SignalStrength.signalStrength == -1) {
        rilSignalStrength->LTE_SignalStrength.signalStrength = 99;
    }
    // rsrp: -1 -> INT_MAX all other negative value to positive.
    // So remap here
    if (rilSignalStrength->LTE_SignalStrength.rsrp == -1) {
        rilSignalStrength->LTE_SignalStrength.rsrp = INT_MAX;
    } else if (rilSignalStrength->LTE_SignalStrength.rsrp < -1) {
        rilSignalStrength->LTE_SignalStrength.rsrp = -rilSignalStrength->LTE_SignalStrength.rsrp;
    }
    // rsrq: -1 -> INT_MAX
    if (rilSignalStrength->LTE_SignalStrength.rsrq == -1) {
        rilSignalStrength->LTE_SignalStrength.rsrq = INT_MAX;
    }
    // Not remapping rssnr is already using INT_MAX
    // cqi: -1 -> INT_MAX
    if (rilSignalStrength->LTE_SignalStrength.cqi == -1) {
        rilSignalStrength->LTE_SignalStrength.cqi = INT_MAX;
    }

    signalStrength.gsm.signalStrength = rilSignalStrength->GSM_SignalStrength.signalStrength;
    signalStrength.gsm.bitErrorRate = rilSignalStrength->GSM_SignalStrength.bitErrorRate;
    signalStrength.gsm.timingAdvance = rilSignalStrength->GSM_SignalStrength.timingAdvance;
    signalStrength.cdma.dbm = rilSignalStrength->CDMA_SignalStrength.dbm;
    signalStrength.cdma.ecio = rilSignalStrength->CDMA_SignalStrength.ecio;
    signalStrength.evdo.dbm = rilSignalStrength->EVDO_SignalStrength.dbm;
    signalStrength.evdo.ecio = rilSignalStrength->EVDO_SignalStrength.ecio;
    signalStrength.evdo.signalNoiseRatio =
            rilSignalStrength->EVDO_SignalStrength.signalNoiseRatio;
    signalStrength.lte.signalStrength = rilSignalStrength->LTE_SignalStrength.signalStrength;
    signalStrength.lte.rsrp = rilSignalStrength->LTE_SignalStrength.rsrp;
    signalStrength.lte.rsrq = rilSignalStrength->LTE_SignalStrength.rsrq;
    signalStrength.lte.rssnr = rilSignalStrength->LTE_SignalStrength.rssnr;
    signalStrength.lte.cqi = rilSignalStrength->LTE_SignalStrength.cqi;
    signalStrength.lte.timingAdvance = rilSignalStrength->LTE_SignalStrength.timingAdvance;
    signalStrength.wcdma.base.signalStrength = rilSignalStrength->WCDMA_SignalStrength.signalStrength;
    signalStrength.wcdma.base.bitErrorRate = rilSignalStrength->WCDMA_SignalStrength.bitErrorRate;
    signalStrength.wcdma.rscp = rilSignalStrength->WCDMA_SignalStrength.rscp;
    signalStrength.wcdma.ecno = rilSignalStrength->WCDMA_SignalStrength.ecno;
}

void convertRilSignalStrengthToHal(void *response, size_t /* responseLen */,
                                   V1_2::SignalStrength &signalStrength) {
    if (response) {
        RIL_SignalStrength_V1_2 *rilSignalStrength = (RIL_SignalStrength_V1_2 *) response;

        convertRilSignalStrengthToHal_V2_V4_Common(rilSignalStrength, signalStrength);

        // To do: Typo should be fixed from TdscdmaSignalStrength to TdScdmaSignalStrength in type.hal
        // signalStrength.tdScdma.signalStrength = rilSignalStrength->TD_SCDMA_SignalStrength.signalStrength;
        // signalStrength.tdScdma.bitErrorRate = rilSignalStrength->TD_SCDMA_SignalStrength.bitErrorRate;
        signalStrength.tdScdma.rscp = rilSignalStrength->TD_SCDMA_SignalStrength.rscp;
    }
}

void convertRilCellInfoGsmToHal(RIL_CellInfoGsm_V1_2 *rillCellInfo, V1_2::CellInfoGsm& cellInfo) {
    if (rillCellInfo != NULL) {
        cellInfo.cellIdentityGsm.base.mcc =
                ril::util::mcc::decode(rillCellInfo->cellIdentityGsm.mcc);
        cellInfo.cellIdentityGsm.base.mnc =
                ril::util::mnc::decode(rillCellInfo->cellIdentityGsm.mnc);
        cellInfo.cellIdentityGsm.base.lac =
                rillCellInfo->cellIdentityGsm.lac;
        cellInfo.cellIdentityGsm.base.cid =
                rillCellInfo->cellIdentityGsm.cid;
        cellInfo.cellIdentityGsm.base.arfcn =
                rillCellInfo->cellIdentityGsm.arfcn;
        cellInfo.cellIdentityGsm.base.bsic =
                rillCellInfo->cellIdentityGsm.bsic;
        cellInfo.cellIdentityGsm.operatorNames.alphaLong
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityGsm.operatorNames.alphaLong);
        cellInfo.cellIdentityGsm.operatorNames.alphaShort
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityGsm.operatorNames.alphaShort);
        cellInfo.signalStrengthGsm.signalStrength =
                rillCellInfo->signalStrengthGsm.signalStrength;
        cellInfo.signalStrengthGsm.bitErrorRate =
                rillCellInfo->signalStrengthGsm.bitErrorRate;
        cellInfo.signalStrengthGsm.timingAdvance =
                rillCellInfo->signalStrengthGsm.timingAdvance;
    }
}

void convertRilCellInfoWcdmaToHal(RIL_CellInfoWcdma_V1_2 *rillCellInfo, V1_2::CellInfoWcdma& cellInfo) {
    if (rillCellInfo != NULL) {
        cellInfo.cellIdentityWcdma.base.mcc =
                ril::util::mcc::decode(rillCellInfo->cellIdentityWcdma.mcc);
        cellInfo.cellIdentityWcdma.base.mnc =
                ril::util::mnc::decode(rillCellInfo->cellIdentityWcdma.mnc);
        cellInfo.cellIdentityWcdma.base.lac =
                rillCellInfo->cellIdentityWcdma.lac;
        cellInfo.cellIdentityWcdma.base.cid =
                rillCellInfo->cellIdentityWcdma.cid;
        cellInfo.cellIdentityWcdma.base.psc =
                rillCellInfo->cellIdentityWcdma.psc;
        cellInfo.cellIdentityWcdma.base.uarfcn =
                rillCellInfo->cellIdentityWcdma.uarfcn;
        cellInfo.cellIdentityWcdma.operatorNames.alphaLong
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityWcdma.operatorNames.alphaLong);
        cellInfo.cellIdentityWcdma.operatorNames.alphaShort
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityWcdma.operatorNames.alphaShort);
        cellInfo.signalStrengthWcdma.base.signalStrength =
                rillCellInfo->signalStrengthWcdma.signalStrength;
        cellInfo.signalStrengthWcdma.base.bitErrorRate =
                rillCellInfo->signalStrengthWcdma.bitErrorRate;
        cellInfo.signalStrengthWcdma.rscp =
                rillCellInfo->signalStrengthWcdma.rscp;
        cellInfo.signalStrengthWcdma.ecno=
                rillCellInfo->signalStrengthWcdma.ecno;
    }
}

void convertRilCellInfoCdmaToHal(RIL_CellInfoCdma_V1_2 *rillCellInfo, V1_2::CellInfoCdma& cellInfo) {
    if (rillCellInfo != NULL) {
        cellInfo.cellIdentityCdma.base.networkId =
                rillCellInfo->cellIdentityCdma.networkId;
        cellInfo.cellIdentityCdma.base.systemId =
                rillCellInfo->cellIdentityCdma.systemId;
        cellInfo.cellIdentityCdma.base.baseStationId =
                rillCellInfo->cellIdentityCdma.basestationId;
        cellInfo.cellIdentityCdma.base.longitude =
                rillCellInfo->cellIdentityCdma.longitude;
        cellInfo.cellIdentityCdma.base.latitude =
                rillCellInfo->cellIdentityCdma.latitude;
        cellInfo.signalStrengthCdma.dbm =
                rillCellInfo->signalStrengthCdma.dbm;
        cellInfo.signalStrengthCdma.ecio =
                rillCellInfo->signalStrengthCdma.ecio;
        cellInfo.signalStrengthEvdo.dbm =
                rillCellInfo->signalStrengthEvdo.dbm;
        cellInfo.signalStrengthEvdo.ecio =
                rillCellInfo->signalStrengthEvdo.ecio;
        cellInfo.signalStrengthEvdo.signalNoiseRatio =
                rillCellInfo->signalStrengthEvdo.signalNoiseRatio;
    }
}

void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_2 *rillCellInfo, V1_2::CellInfoLte& cellInfo) {
    if (rillCellInfo != NULL) {
        cellInfo.cellIdentityLte.base.mcc =
                ril::util::mcc::decode(rillCellInfo->cellIdentityLte.mcc);
        cellInfo.cellIdentityLte.base.mnc =
                ril::util::mnc::decode(rillCellInfo->cellIdentityLte.mnc);
        cellInfo.cellIdentityLte.base.ci =
                rillCellInfo->cellIdentityLte.ci;
        cellInfo.cellIdentityLte.base.pci =
                rillCellInfo->cellIdentityLte.pci;
        cellInfo.cellIdentityLte.base.tac =
                rillCellInfo->cellIdentityLte.tac;
        cellInfo.cellIdentityLte.base.earfcn =
                rillCellInfo->cellIdentityLte.earfcn;
        cellInfo.cellIdentityLte.bandwidth =
                rillCellInfo->cellIdentityLte.bandwidth;
        cellInfo.cellIdentityLte.operatorNames.alphaLong
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityLte.operatorNames.alphaLong);
        cellInfo.cellIdentityLte.operatorNames.alphaShort
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityLte.operatorNames.alphaShort);
        cellInfo.signalStrengthLte.signalStrength =
                rillCellInfo->signalStrengthLte.signalStrength;
        cellInfo.signalStrengthLte.rsrp =
                rillCellInfo->signalStrengthLte.rsrp;
        cellInfo.signalStrengthLte.rsrq =
                rillCellInfo->signalStrengthLte.rsrq;
        cellInfo.signalStrengthLte.rssnr =
                rillCellInfo->signalStrengthLte.rssnr;
        cellInfo.signalStrengthLte.cqi =
                rillCellInfo->signalStrengthLte.cqi;
        cellInfo.signalStrengthLte.timingAdvance =
                rillCellInfo->signalStrengthLte.timingAdvance;
    }
}

void convertRilCellInfoTdscdmaToHal(RIL_CellInfoTdscdma_V1_2 *rillCellInfo, V1_2::CellInfoTdscdma& cellInfo) {
    if (rillCellInfo != NULL) {
        cellInfo.cellIdentityTdscdma.base.mcc =
                ril::util::mcc::decode(rillCellInfo->cellIdentityTdscdma.mcc);
        cellInfo.cellIdentityTdscdma.base.mnc =
                ril::util::mnc::decode(rillCellInfo->cellIdentityTdscdma.mnc);
        cellInfo.cellIdentityTdscdma.base.lac =
                rillCellInfo->cellIdentityTdscdma.lac;
        cellInfo.cellIdentityTdscdma.base.cid =
                rillCellInfo->cellIdentityTdscdma.cid;
        cellInfo.cellIdentityTdscdma.base.cpid =
                rillCellInfo->cellIdentityTdscdma.cpid;
        cellInfo.cellIdentityTdscdma.operatorNames.alphaLong
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityTdscdma.operatorNames.alphaLong);
        cellInfo.cellIdentityTdscdma.operatorNames.alphaShort
            = convertCharPtrToHidlString(rillCellInfo->cellIdentityTdscdma.operatorNames.alphaShort);
        cellInfo.signalStrengthTdscdma.signalStrength =
                rillCellInfo->signalStrengthTdscdma.signalStrength;
        cellInfo.signalStrengthTdscdma.bitErrorRate =
                rillCellInfo->signalStrengthTdscdma.bitErrorRate;
        cellInfo.signalStrengthTdscdma.rscp =
                rillCellInfo->signalStrengthTdscdma.rscp;
    }
}

void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_2::CellInfo>& records) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_CellInfo_V1_2);
    records.resize(num);

    RIL_CellInfo_V1_2 *rillCellInfo = (RIL_CellInfo_V1_2 *) response;
    for (int i = 0; i < num; i++) {
        records[i].cellInfoType = (CellInfoType) rillCellInfo->cellInfoType;
        records[i].registered = rillCellInfo->registered;
        records[i].timeStampType = (TimeStampType) rillCellInfo->timeStampType;
        records[i].timeStamp = rillCellInfo->timeStamp;
        records[i].connectionStatus = (V1_2::CellConnectionStatus) rillCellInfo->connectionStatus;
        // All vectors should be size 0 except one which will be size 1. Set everything to
        // size 0 initially.
        records[i].gsm.resize(0);
        records[i].wcdma.resize(0);
        records[i].cdma.resize(0);
        records[i].lte.resize(0);
        records[i].tdscdma.resize(0);
        switch(rillCellInfo->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                records[i].gsm.resize(1);
                convertRilCellInfoGsmToHal(&rillCellInfo->CellInfo.gsm, records[i].gsm[0]);
                break;
            }

            case RIL_CELL_INFO_TYPE_WCDMA: {
                records[i].wcdma.resize(1);
                convertRilCellInfoWcdmaToHal(&rillCellInfo->CellInfo.wcdma, records[i].wcdma[0]);
                break;
            }

            case RIL_CELL_INFO_TYPE_CDMA: {
                records[i].cdma.resize(1);
                convertRilCellInfoCdmaToHal(&rillCellInfo->CellInfo.cdma, records[i].cdma[0]);
                break;
            }

            case RIL_CELL_INFO_TYPE_LTE: {
                records[i].lte.resize(1);
                convertRilCellInfoLteToHal(&rillCellInfo->CellInfo.lte, records[i].lte[0]);
                break;
            }

            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                records[i].tdscdma.resize(1);
                convertRilCellInfoTdscdmaToHal(&rillCellInfo->CellInfo.tdscdma, records[i].tdscdma[0]);
                break;
            }
            default: {
                break;
            }
        }
        rillCellInfo += 1;
    }
}

///////////////////////////////////////////////////////////////////////////
// radio 1.4  RIL => HAL convert function
///////////////////////////////////////////////////////////////////////////
bool convertRilCardStatusToHal(RIL_CardStatus_V1_4 *cardStatus, size_t cardStatusLen, V1_4::CardStatus& out) {
    if (cardStatus == NULL || cardStatusLen != sizeof(RIL_CardStatus_V1_4)) {
        return false;
    }

    if (!convertRilCardStatusToHal((RIL_CardStatus_V1_2 *)cardStatus, sizeof(RIL_CardStatus_V1_2), out.base)) {
        return false;
    }
    out.eid = convertCharPtrToHidlString((char *) cardStatus->eid);

    return true;
}

bool convertRilDataRegStateToHal(RIL_DataRegistrationStateResponse_V1_4 *dataRegState, size_t responselen,
                                        V1_4::DataRegStateResult& dataRegResponse) {
    if (dataRegState == NULL ||
        responselen != sizeof(RIL_DataRegistrationStateResponse_V1_4)) {
        RLOGE("convertRilDataRegStateToHal Invalid response: NULL");
        return false;
    }

    if (!convertRilDataRegStateToHal((RIL_DataRegistrationStateResponse_V1_2 *)dataRegState,
                                      sizeof(RIL_DataRegistrationStateResponse_V1_2),
                                      dataRegResponse.base)) {
        return false;
    }
    V1_4::LteVopsInfo lteVopsInfo = {
        dataRegState->lteVopsInfo.isVopsSupported,
        dataRegState->lteVopsInfo.isEmcBearerSupported
    };

    // V1_4::LteVopsInfo is only valid the current rat is LTE/LTE_CA
    // others, must be noinit state
    if (dataRegState->rat == RADIO_TECH_LTE || dataRegState->rat == RADIO_TECH_LTE_CA) {
        dataRegResponse.vopsInfo.lteVopsInfo(lteVopsInfo);
    }
    dataRegResponse.nrIndicators.isEndcAvailable = dataRegState->nrIndicators.isEndcAvailable;
    dataRegResponse.nrIndicators.isDcNrRestricted = dataRegState->nrIndicators.isDcNrRestricted;
    dataRegResponse.nrIndicators.isNrAvailable = dataRegState->nrIndicators.isNrAvailable;

    return true;
}

V1_4::PdpProtocolType getPdpType(const char *protocol)
{
    std::string str = protocol ? protocol : "";
    V1_4::PdpProtocolType ret = V1_4::PdpProtocolType::IP;

    if (str.empty()) {
        ret = V1_4::PdpProtocolType::IP;
    } else if (str.compare("IP") == 0) {
        ret = V1_4::PdpProtocolType::IP;
    } else if (str.compare("IPV6") == 0) {
        ret = V1_4::PdpProtocolType::IPV6;
    } else if (str.compare("IPV4V6") == 0) {
        ret = V1_4::PdpProtocolType::IPV4V6;
    }

    return ret;
}

void convertRilDataCallToHal(RIL_Data_Call_Response_v11 *dcResponse,
    V1_4::SetupDataCallResult& dcResult, vector<vector<string>> &tmp, int idx) {
    if (dcResponse) {
        dcResult.cause = (V1_4::DataCallFailCause) dcResponse->status;
        dcResult.suggestedRetryTime = dcResponse->suggestedRetryTime;
        dcResult.cid = dcResponse->cid;
        dcResult.active = (V1_4::DataConnActiveStatus)dcResponse->active;
        dcResult.type = (V1_4::PdpProtocolType) getPdpType(dcResponse->type);
        dcResult.ifname = convertCharPtrToHidlString(dcResponse->ifname);
        parseStrings(tmp[idx*4+0], dcResult.addresses, dcResponse->addresses);
        parseStrings(tmp[idx*4+1], dcResult.dnses, dcResponse->dnses);
        parseStrings(tmp[idx*4+2], dcResult.gateways, dcResponse->gateways);
        parseStrings(tmp[idx*4+3], dcResult.pcscf, dcResponse->pcscf);

        dcResult.mtu = dcResponse->mtu;
    }
}

// Use v11 version then convert it to V1_4:: Result
void convertRilDataCallListToHal(void *response, size_t responseLen,
        hidl_vec<V1_4::SetupDataCallResult>& dcResultList, vector<vector<string>>& t) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_Data_Call_Response_v11);

    RIL_Data_Call_Response_v11 *dcResponse = (RIL_Data_Call_Response_v11 *) response;
    dcResultList.resize(num);
    t.resize(num*4);
    for (int i = 0; i < num; i++) {
        convertRilDataCallToHal(&dcResponse[i], dcResultList[i], t, i);
    }
}

// Use v11 version then convert it to V1_5:: Result
void convertRilDataCallListToHal(void *response, size_t responseLen,
        hidl_vec<V1_5::SetupDataCallResult>& dcResultList, vector<vector<string>>& t) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_Data_Call_Response_v11);

    RIL_Data_Call_Response_v11 *dcResponse = (RIL_Data_Call_Response_v11 *) response;
    dcResultList.resize(num);
    t.resize(num*4);
    for (int i = 0; i < num; i++) {
        convertRilDataCallToHal(&dcResponse[i], dcResultList[i], t, i);
    }
}

void convertRilSignalStrengthToHal(void *response, size_t responseLen, V1_4::SignalStrength& signalStrength) {
    if (response == NULL || responseLen != sizeof(RIL_SignalStrength_V1_4)) {
        return ;
    }

    RIL_SignalStrength_V1_4 *rilSignalStrength = (RIL_SignalStrength_V1_4 *)response;

    convertRilSignalStrengthToHal_V2_V4_Common(rilSignalStrength, signalStrength);

    signalStrength.tdscdma.signalStrength = rilSignalStrength->TD_SCDMA_SignalStrength.signalStrength;
    signalStrength.tdscdma.bitErrorRate = rilSignalStrength->TD_SCDMA_SignalStrength.bitErrorRate;
    signalStrength.tdscdma.rscp = rilSignalStrength->TD_SCDMA_SignalStrength.rscp;

    signalStrength.nr.ssRsrp = rilSignalStrength->NR_SignalStrength.ssRsrp;
    signalStrength.nr.ssRsrq = rilSignalStrength->NR_SignalStrength.ssRsrq;
    signalStrength.nr.ssSinr = rilSignalStrength->NR_SignalStrength.ssSinr;
    signalStrength.nr.csiRsrp = rilSignalStrength->NR_SignalStrength.csiRsrp;
    signalStrength.nr.csiRsrq = rilSignalStrength->NR_SignalStrength.csiRsrq;
    signalStrength.nr.csiSinr = rilSignalStrength->NR_SignalStrength.csiSinr;
}

void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_4 *rillCellInfo, V1_4::CellInfoLte& cellInfo) {
    if (rillCellInfo != NULL) {
        convertRilCellInfoLteToHal(&rillCellInfo->cellInfo, cellInfo.base);
        cellInfo.cellConfig.isEndcAvailable =
                rillCellInfo->cellConfig.isEndcAvailable;
    }
}

void convertRilCellInfoNrToHal(RIL_CellInfoNr_V1_4 *rillCellInfo, V1_4::CellInfoNr& cellInfo) {
    if (rillCellInfo != NULL) {
        cellInfo.cellidentity.mcc =
            ril::util::mcc::decode(rillCellInfo->cellidentityNr.mcc);
        cellInfo.cellidentity.mnc =
            ril::util::mnc::decode(rillCellInfo->cellidentityNr.mnc);
        cellInfo.cellidentity.nci = rillCellInfo->cellidentityNr.nci;
        cellInfo.cellidentity.pci = rillCellInfo->cellidentityNr.pci;
        cellInfo.cellidentity.tac = rillCellInfo->cellidentityNr.tac;
        cellInfo.cellidentity.operatorNames.alphaLong
            = convertCharPtrToHidlString(rillCellInfo->cellidentityNr.operatorNames.alphaLong);
        cellInfo.cellidentity.operatorNames.alphaShort
            = convertCharPtrToHidlString(rillCellInfo->cellidentityNr.operatorNames.alphaShort);
        cellInfo.signalStrength.ssRsrp = rillCellInfo->signalStrengthNr.ssRsrp; ;
        cellInfo.signalStrength.ssRsrq = rillCellInfo->signalStrengthNr.ssRsrq; ;
        cellInfo.signalStrength.ssSinr = rillCellInfo->signalStrengthNr.ssSinr; ;
        cellInfo.signalStrength.csiRsrp = rillCellInfo->signalStrengthNr.csiRsrp; ;
        cellInfo.signalStrength.csiRsrq = rillCellInfo->signalStrengthNr.csiRsrq; ;
        cellInfo.signalStrength.csiSinr = rillCellInfo->signalStrengthNr.csiSinr; ;
    }
}

void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_4::CellInfo>& records) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_CellInfo_V1_4);
    records.resize(num);

    RIL_CellInfo_V1_4 *rillCellInfo = (RIL_CellInfo_V1_4 *) response;
    for (int i = 0; i < num; i++) {
        records[i].isRegistered = rillCellInfo->registered;
        records[i].connectionStatus = (V1_2::CellConnectionStatus)rillCellInfo->connectionStatus;
        switch((int)rillCellInfo->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                V1_2::CellInfoGsm cellInfo{};
                convertRilCellInfoGsmToHal(&rillCellInfo->CellInfo.gsm, cellInfo);
                records[i].info.gsm(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_WCDMA: {
                V1_2::CellInfoWcdma cellInfo{};
                convertRilCellInfoWcdmaToHal(&rillCellInfo->CellInfo.wcdma, cellInfo);
                records[i].info.wcdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_CDMA: {
                V1_2::CellInfoCdma cellInfo{};
                convertRilCellInfoCdmaToHal(&rillCellInfo->CellInfo.cdma, cellInfo);
                records[i].info.cdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_LTE: {
                V1_4::CellInfoLte cellInfo{};
                convertRilCellInfoLteToHal(&rillCellInfo->CellInfo.lte, cellInfo);
                records[i].info.lte(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                V1_2::CellInfoTdscdma cellInfo{};
                convertRilCellInfoTdscdmaToHal(&rillCellInfo->CellInfo.tdscdma, cellInfo);
                records[i].info.tdscdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_NR: {
                V1_4::CellInfoNr cellInfo{};
                convertRilCellInfoNrToHal(&rillCellInfo->CellInfo.nr, cellInfo);
                records[i].info.nr(cellInfo);
                break;
            }

            default: {
                break;
            }
        }
        rillCellInfo += 1;
    }
}

void convertRilPhysicalChannelConfigToHal(void *response, size_t responseLen,
    V1_2::PhysicalChannelConfig& config) {
    if (response == NULL || responseLen != sizeof(RIL_PhysicalChannelConfig)) {
        return ;
    }
    RIL_PhysicalChannelConfig *rilConfig = (RIL_PhysicalChannelConfig *)response;
    config.status = (V1_2::CellConnectionStatus)rilConfig->status;
    config.cellBandwidthDownlink = rilConfig->cellBandwidthDownlink;
}

void convertRilPhysicalChannelConfigToHal(void *response, size_t responseLen,
    V1_4::PhysicalChannelConfig& config) {
    if (response == NULL || responseLen != sizeof(RIL_PhysicalChannelConfig_V1_4)) {
        return ;
    }
    convertRilPhysicalChannelConfigToHal(response, sizeof(RIL_PhysicalChannelConfig), config.base);

    RIL_PhysicalChannelConfig_V1_4 *rilConfig = (RIL_PhysicalChannelConfig_V1_4 *)response;
    config.rat = (V1_4::RadioTechnology)rilConfig->rat;
    if (rilConfig->rfInfoType == 1) {
        config.rfInfo.range((V1_4::FrequencyRange)rilConfig->rfInfo.range);
    }
    else if (rilConfig->rfInfoType == 2) {
        config.rfInfo.channelNumber(rilConfig->rfInfo.channelNumber);
    }
    config.contextIds.resize(0);
    if (rilConfig->len_contextIds > 0) {
        config.contextIds.resize(rilConfig->len_contextIds);
        for (int i = 0; i < rilConfig->len_contextIds; i++) {
            config.contextIds[i] = rilConfig->contextIds[i];
        }
    }
    config.physicalCellId = rilConfig->physicalCellId;
}

void convertRilEmergencyNumberToHal(void *response, size_t responselen,
        V1_4::EmergencyNumber& emergencyNumber) {
    if (response == NULL || responselen != sizeof(RIL_EmergencyNumber)) {
        return ;
    }

    RIL_EmergencyNumber *rilInfo = (RIL_EmergencyNumber *)response;
    emergencyNumber.number = convertCharPtrToHidlString(rilInfo->number);
    emergencyNumber.mcc = convertCharPtrToHidlString(rilInfo->mcc);
    emergencyNumber.mnc = convertCharPtrToHidlString(rilInfo->mnc);
    emergencyNumber.categories = rilInfo->categories;
    emergencyNumber.urns.resize(0);
    if (rilInfo->len_urns > 0) {
        emergencyNumber.urns.resize(rilInfo->len_urns);
        for (int i = 0; i < rilInfo->len_urns; i++) {
            emergencyNumber.urns[i] = convertCharPtrToHidlString(rilInfo->urns[i]);
        }
    }
    emergencyNumber.sources = rilInfo->sources;
}

void convertRilEmergencyNumberListToHal(void *response, size_t responselen,
        hidl_vec<V1_4::EmergencyNumber>& emergencyNumberList) {
    if (response == NULL || responselen % sizeof(RIL_EmergencyNumber) != 0) {
        return ;
    }

    int num = responselen / sizeof(RIL_EmergencyNumber);
    emergencyNumberList.resize(num);
    for (int i = 0; i < num; i++) {
        RIL_EmergencyNumber *p_cur = &((RIL_EmergencyNumber *)response)[i];
        convertRilEmergencyNumberToHal(p_cur, sizeof(RIL_EmergencyNumber), emergencyNumberList[i]);
    }
}

///////////////////////////////////////////////////////////////////////////
// radio 1.5  RIL => HAL convert function
///////////////////////////////////////////////////////////////////////////
void convertRilBarringInfoToHal(V1_5::BarringInfo& to, RIL_BarringInfo& from) {
    to.serviceType = (V1_5::BarringInfo::ServiceType)from.service;
    to.barringType = (V1_5::BarringInfo::BarringType)from.type;
    V1_5::BarringInfo::BarringTypeSpecificInfo::Conditional cbi = {
        from.typeSpecificInfo.conditionalBarringInfo.barringFactor,
        from.typeSpecificInfo.conditionalBarringInfo.barringTimeSeconds,
        from.typeSpecificInfo.conditionalBarringInfo.isBarred
    };
    to.barringTypeSpecificInfo.conditional(cbi);
}

void convertRilBarringInfosToHal(hidl_vec<V1_5::BarringInfo>& halRecords,
        RIL_BarringInfo *prilBarringInfos, size_t numOfInfos) {
    if (prilBarringInfos == NULL || numOfInfos == 0) {
        halRecords.resize(0);
        return ;
    }

    halRecords.resize(numOfInfos);
    for (uint32_t i = 0; i < numOfInfos; i++) {
        convertRilBarringInfoToHal(halRecords[i], prilBarringInfos[i]);
    }
}

void convertRilCsgInfoToHal(V1_5::ClosedSubscriberGroupInfo& out, const RIL_ClosedSubscriberGroupInfo& csg) {
    out.csgIndication = csg.csgIndication;
    out.homeNodebName = convertCharPtrToHidlString(csg.homeNodebName);
    out.csgIdentity = csg.csgIdentity;
}

// Optional CSG info
// valid only if csgIndication is true.
void convertRilOptionalCsgInfoToHal(V1_5::OptionalCsgInfo& out, const RIL_OptionalCsgInfo& csg) {
    if (!csg.csgInfo.csgIndication && csg.csgInfo.csgIdentity == INT_MAX) {
        out.noinit();
    } else {
        V1_5::ClosedSubscriberGroupInfo csgInfo;
        convertRilCsgInfoToHal(csgInfo, csg.csgInfo);
        out.csgInfo(csgInfo);
    }
}

void convertRilCellIdentityGsmToHal(V1_0::CellIdentityGsm& out, RIL_CellIdentityGsm_V1_5& rilCellId) {
    out.mcc = ril::util::mcc::decode(rilCellId.mcc);
    out.mnc = ril::util::mnc::decode(rilCellId.mnc);
    out.lac = rilCellId.lac;
    out.cid = rilCellId.cid;
    out.arfcn = rilCellId.arfcn;
    out.bsic = rilCellId.bsic;
}

void convertRilCellIdentityGsmToHal(V1_2::CellIdentityGsm& out, RIL_CellIdentityGsm_V1_5& rilCellId) {
    convertRilCellIdentityGsmToHal(out.base, rilCellId);
    out.operatorNames.alphaLong = convertCharPtrToHidlString(rilCellId.operatorNames.alphaLong);
    out.operatorNames.alphaShort = convertCharPtrToHidlString(rilCellId.operatorNames.alphaShort);
}

void convertRilCellIdentityGsmToHal(V1_5::CellIdentityGsm &out, RIL_CellIdentityGsm_V1_5 &rilCellId) {
    convertRilCellIdentityGsmToHal(out.base, rilCellId);
    out.additionalPlmns.resize(rilCellId.leng_additionalPlmns);
    if (rilCellId.additionalPlmns != NULL) {
        for (int i = 0; i < rilCellId.leng_additionalPlmns; i ++) {
            out.additionalPlmns[i] = convertCharPtrToHidlString(rilCellId.additionalPlmns[i]);
        }
    } else {
        out.additionalPlmns.resize(0);
    }
}

void convertRilCellIdentityWcdmaToHal(V1_0::CellIdentityWcdma& out, RIL_CellIdentityWcdma_V1_5& rilCellId) {
    out.mcc = ril::util::mcc::decode(rilCellId.mcc);
    out.mnc = ril::util::mnc::decode(rilCellId.mnc);
    out.lac = rilCellId.lac;
    out.cid = rilCellId.cid;
    out.psc = rilCellId.psc;
    out.uarfcn = rilCellId.uarfcn;
}

void convertRilCellIdentityWcdmaToHal(V1_2::CellIdentityWcdma& out, RIL_CellIdentityWcdma_V1_5& rilCellId) {
    convertRilCellIdentityWcdmaToHal(out.base, rilCellId);
    out.operatorNames.alphaLong = convertCharPtrToHidlString(rilCellId.operatorNames.alphaLong);
    out.operatorNames.alphaShort = convertCharPtrToHidlString(rilCellId.operatorNames.alphaShort);
}

void convertRilCellIdentityWcdmaToHal(V1_5::CellIdentityWcdma& out, RIL_CellIdentityWcdma_V1_5& rilCellId) {
    convertRilCellIdentityWcdmaToHal(out.base, rilCellId);
    out.additionalPlmns.resize(rilCellId.leng_additionalPlmns);
    if (rilCellId.additionalPlmns != NULL) {
        for (int i = 0; i < rilCellId.leng_additionalPlmns; i ++) {
            out.additionalPlmns[i] = convertCharPtrToHidlString(rilCellId.additionalPlmns[i]);
        }
    } else {
        out.additionalPlmns.resize(0);
    }

    // Optional CSG info
    // valid only if csgIndication is true.
    convertRilOptionalCsgInfoToHal(out.optionalCsgInfo, rilCellId.optionalCsgInfo);
}

void convertRilCellIdentityTdscdmaToHal(V1_0::CellIdentityTdscdma& out, RIL_CellIdentityTdscdma_V1_5& rilCellId) {
    out.mcc = ril::util::mcc::decode(rilCellId.mcc);
    out.mnc = ril::util::mnc::decode(rilCellId.mnc);
    out.lac = rilCellId.lac;
    out.cid = rilCellId.cid;
    out.cpid = rilCellId.cpid;
}

void convertRilCellIdentityTdscdmaToHal(V1_2::CellIdentityTdscdma& out, RIL_CellIdentityTdscdma_V1_5& rilCellId) {
    convertRilCellIdentityTdscdmaToHal(out.base, rilCellId);
    out.uarfcn = rilCellId.uarfcn;
    out.operatorNames.alphaLong = convertCharPtrToHidlString(rilCellId.operatorNames.alphaLong);
    out.operatorNames.alphaShort = convertCharPtrToHidlString(rilCellId.operatorNames.alphaShort);
}

void convertRilCellIdentityTdscdmaToHal(V1_5::CellIdentityTdscdma& out, RIL_CellIdentityTdscdma_V1_5& rilCellId) {
    convertRilCellIdentityTdscdmaToHal(out.base, rilCellId);
    out.additionalPlmns.resize(rilCellId.leng_additionalPlmns);
    if (rilCellId.additionalPlmns != NULL) {
        for (int i = 0; i < rilCellId.leng_additionalPlmns; i ++) {
            out.additionalPlmns[i] = convertCharPtrToHidlString(rilCellId.additionalPlmns[i]);
        }
    } else {
        out.additionalPlmns.resize(0);
    }

    // Optional CSG info
    // valid only if csgIndication is true.
    convertRilOptionalCsgInfoToHal(out.optionalCsgInfo, rilCellId.optionalCsgInfo);
}

void convertRilCellIdentityCdmaToHal(V1_0::CellIdentityCdma& out, RIL_CellIdentityCdma_V1_2& rilCellId) {
    out.networkId = rilCellId.networkId;
    out.systemId = rilCellId.systemId;
    out.baseStationId = rilCellId.basestationId;
    out.longitude = rilCellId.longitude;
    out.latitude = rilCellId.latitude;
}

void convertRilCellIdentityCdmaToHal(V1_2::CellIdentityCdma& out, RIL_CellIdentityCdma_V1_2& rilCellId) {
    convertRilCellIdentityCdmaToHal(out.base, rilCellId);
    out.operatorNames.alphaLong = convertCharPtrToHidlString(rilCellId.operatorNames.alphaLong);
    out.operatorNames.alphaShort = convertCharPtrToHidlString(rilCellId.operatorNames.alphaShort);
}

void convertRilCellIdentityLteToHal(V1_0::CellIdentityLte& out, RIL_CellIdentityLte_V1_5& rilCellId) {
    out.mcc = ril::util::mcc::decode(rilCellId.mcc);
    out.mnc = ril::util::mnc::decode(rilCellId.mnc);
    out.ci = rilCellId.ci;
    out.pci = rilCellId.pci;
    out.tac = rilCellId.tac;
    out.earfcn = rilCellId.earfcn;
}

void convertRilCellIdentityLteToHal(V1_2::CellIdentityLte& out, RIL_CellIdentityLte_V1_5& rilCellId) {
    convertRilCellIdentityLteToHal(out.base, rilCellId);
    out.operatorNames.alphaLong = convertCharPtrToHidlString(rilCellId.operatorNames.alphaLong);
    out.operatorNames.alphaShort = convertCharPtrToHidlString(rilCellId.operatorNames.alphaShort);
    out.bandwidth = rilCellId.bandwidth;
}

void convertRilCellIdentityLteToHal(V1_5::CellIdentityLte& out, RIL_CellIdentityLte_V1_5& rilCellId) {
    convertRilCellIdentityLteToHal(out.base, rilCellId);
    out.additionalPlmns.resize(rilCellId.leng_additionalPlmns);
    if (rilCellId.additionalPlmns != NULL) {
        for (int i = 0; i < rilCellId.leng_additionalPlmns; i ++) {
            out.additionalPlmns[i] = convertCharPtrToHidlString(rilCellId.additionalPlmns[i]);
        }
    } else {
        out.additionalPlmns.resize(0);
    }

    convertRilOptionalCsgInfoToHal(out.optionalCsgInfo, rilCellId.optionalCsgInfo);

    out.bands.resize(rilCellId.leng_bands);
    for (int i = 0; i < rilCellId.leng_bands; i ++) {
        out.bands[i] = (V1_5::EutranBands)rilCellId.bands[i];
    }
}

void convertRilCellIdentityNrToHal(V1_4::CellIdentityNr& out, RIL_CellIdentityNr_V1_5& rilCellId) {
    out.mcc = ril::util::mcc::decode(rilCellId.mcc);
    out.mnc = ril::util::mnc::decode(rilCellId.mnc);
    out.nci = rilCellId.nci;
    out.pci = rilCellId.pci;
    out.tac = rilCellId.tac;
    out.nrarfcn = rilCellId.nrarfcn;
    out.operatorNames.alphaLong = convertCharPtrToHidlString(rilCellId.operatorNames.alphaLong);
    out.operatorNames.alphaShort = convertCharPtrToHidlString(rilCellId.operatorNames.alphaShort);
}

void convertRilCellIdentityNrToHal(V1_5::CellIdentityNr& out, RIL_CellIdentityNr_V1_5& rilCellId) {
    convertRilCellIdentityNrToHal(out.base, rilCellId);
    out.additionalPlmns.resize(rilCellId.leng_additionalPlmns);
    if (rilCellId.additionalPlmns != NULL) {
        for (int i = 0; i < rilCellId.leng_additionalPlmns; i ++) {
            out.additionalPlmns[i] = convertCharPtrToHidlString(rilCellId.additionalPlmns[i]);
        }
    } else {
        out.additionalPlmns.resize(0);
    }

    out.bands.resize(rilCellId.leng_bands);
    for (int i = 0; i < rilCellId.leng_bands; i ++) {
        out.bands[i] = (V1_5::NgranBands)rilCellId.bands[i];
    }
}

void convertRilCellIdentityToHal(V1_5::CellIdentity& out, RIL_CellIdentity_V1_5& rilCellIdentity) {
    switch((int)rilCellIdentity.cellInfoType) {
        case RIL_CELL_INFO_TYPE_GSM:
        {
            V1_5::CellIdentityGsm gsmCellId = {};
            convertRilCellIdentityGsmToHal(gsmCellId, rilCellIdentity.gsm);
            out.gsm(gsmCellId);
            break;
        }
        case RIL_CELL_INFO_TYPE_WCDMA:
        {
            V1_5::CellIdentityWcdma wcdmaCellId = {};
            convertRilCellIdentityWcdmaToHal(wcdmaCellId, rilCellIdentity.wcdma);
            out.wcdma(wcdmaCellId);
            break;
        }
        case RIL_CELL_INFO_TYPE_TD_SCDMA:
        {
            V1_5::CellIdentityTdscdma tdscdmaCellId = {};
            convertRilCellIdentityTdscdmaToHal(tdscdmaCellId, rilCellIdentity.tdscdma);
            out.tdscdma(tdscdmaCellId);
            break;
        }
        case RIL_CELL_INFO_TYPE_CDMA:
        {
            V1_2::CellIdentityCdma cdmaCellId = {};
            convertRilCellIdentityCdmaToHal(cdmaCellId, rilCellIdentity.cdma);
            out.cdma(cdmaCellId);
            break;
        }
        case RIL_CELL_INFO_TYPE_LTE:
        {
            V1_5::CellIdentityLte lteCellId = {};
            convertRilCellIdentityLteToHal(lteCellId, rilCellIdentity.lte);
            out.lte(lteCellId);
            break;
        }
        case RIL_CELL_INFO_TYPE_NR:
        {
            V1_5::CellIdentityNr nrCellId = {};
            convertRilCellIdentityNrToHal(nrCellId, rilCellIdentity.nr);
            out.nr(nrCellId);
            break;
        }
        default:
        {
            // explicit call of "noinit"
            out.noinit();
            break;
        }
    }
}

void convertRilCellInfoGsmToHal(RIL_CellInfoGsm_V1_5 *rillCellInfo, V1_5::CellInfoGsm& cellInfo) {
    if (rillCellInfo != NULL) {
        convertRilCellIdentityGsmToHal(cellInfo.cellIdentityGsm, rillCellInfo->cellIdentityGsm);
        cellInfo.signalStrengthGsm.signalStrength =
                rillCellInfo->signalStrengthGsm.signalStrength;
        cellInfo.signalStrengthGsm.bitErrorRate =
                rillCellInfo->signalStrengthGsm.bitErrorRate;
        cellInfo.signalStrengthGsm.timingAdvance =
                rillCellInfo->signalStrengthGsm.timingAdvance;
    }
}

void convertRilCellInfoWcdmaToHal(RIL_CellInfoWcdma_V1_5 *rillCellInfo, V1_5::CellInfoWcdma& cellInfo) {
    if (rillCellInfo != NULL) {
        convertRilCellIdentityWcdmaToHal(cellInfo.cellIdentityWcdma, rillCellInfo->cellIdentityWcdma);
        cellInfo.signalStrengthWcdma.base.signalStrength =
                rillCellInfo->signalStrengthWcdma.signalStrength;
        cellInfo.signalStrengthWcdma.base.bitErrorRate =
                rillCellInfo->signalStrengthWcdma.bitErrorRate;
        cellInfo.signalStrengthWcdma.rscp =
                rillCellInfo->signalStrengthWcdma.rscp;
        cellInfo.signalStrengthWcdma.ecno=
                rillCellInfo->signalStrengthWcdma.ecno;
    }
}

void convertRilCellInfoLteToHal(RIL_CellInfoLte_V1_5 *rillCellInfo, V1_5::CellInfoLte& cellInfo) {
    if (rillCellInfo != NULL) {
        convertRilCellIdentityLteToHal(cellInfo.cellIdentityLte, rillCellInfo->cellIdentityLte);
        cellInfo.signalStrengthLte.signalStrength =
                rillCellInfo->signalStrengthLte.signalStrength;
        cellInfo.signalStrengthLte.rsrp =
                rillCellInfo->signalStrengthLte.rsrp;
        cellInfo.signalStrengthLte.rsrq =
                rillCellInfo->signalStrengthLte.rsrq;
        cellInfo.signalStrengthLte.rssnr =
                rillCellInfo->signalStrengthLte.rssnr;
        cellInfo.signalStrengthLte.cqi =
                rillCellInfo->signalStrengthLte.cqi;
        cellInfo.signalStrengthLte.timingAdvance =
                rillCellInfo->signalStrengthLte.timingAdvance;
    }
}

void convertRilCellInfoTdscdmaToHal(RIL_CellInfoTdscdma_V1_5 *rillCellInfo, V1_5::CellInfoTdscdma& cellInfo) {
    if (rillCellInfo != NULL) {
        convertRilCellIdentityTdscdmaToHal(cellInfo.cellIdentityTdscdma, rillCellInfo->cellIdentityTdscdma);
        cellInfo.signalStrengthTdscdma.signalStrength =
                rillCellInfo->signalStrengthTdscdma.signalStrength;
        cellInfo.signalStrengthTdscdma.bitErrorRate =
                rillCellInfo->signalStrengthTdscdma.bitErrorRate;
        cellInfo.signalStrengthTdscdma.rscp =
                rillCellInfo->signalStrengthTdscdma.rscp;
    }
}

void convertRilCellInfoNrToHal(RIL_CellInfoNr_V1_5 *rillCellInfo, V1_5::CellInfoNr& cellInfo) {
    if (rillCellInfo != NULL) {
        convertRilCellIdentityNrToHal(cellInfo.cellIdentityNr, rillCellInfo->cellIdentityNr);
        cellInfo.signalStrengthNr.ssRsrp = rillCellInfo->signalStrengthNr.ssRsrp;
        cellInfo.signalStrengthNr.ssRsrq = rillCellInfo->signalStrengthNr.ssRsrq;
        cellInfo.signalStrengthNr.ssSinr = rillCellInfo->signalStrengthNr.ssSinr;
        cellInfo.signalStrengthNr.csiRsrp = rillCellInfo->signalStrengthNr.csiRsrp;
        cellInfo.signalStrengthNr.csiRsrq = rillCellInfo->signalStrengthNr.csiRsrq;
        cellInfo.signalStrengthNr.csiSinr = rillCellInfo->signalStrengthNr.csiSinr;
    }
}

void convertRilCellInfoListToHal(void *response, size_t responseLen, hidl_vec<V1_5::CellInfo>& records) {
    int num = (response == NULL) ? 0 : responseLen / sizeof(RIL_CellInfo_V1_5);
    records.resize(num);

    RIL_CellInfo_V1_5 *rillCellInfo = (RIL_CellInfo_V1_5 *) response;
    for (int i = 0; i < num; i++) {
        records[i].registered = rillCellInfo->registered;
        records[i].connectionStatus = (V1_2::CellConnectionStatus)rillCellInfo->connectionStatus;
        switch((int)rillCellInfo->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                V1_5::CellInfoGsm cellInfo;
                convertRilCellInfoGsmToHal(&rillCellInfo->CellInfo.gsm, cellInfo);
                records[i].ratSpecificInfo.gsm(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_WCDMA: {
                V1_5::CellInfoWcdma cellInfo;
                convertRilCellInfoWcdmaToHal(&rillCellInfo->CellInfo.wcdma, cellInfo);
                records[i].ratSpecificInfo.wcdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_CDMA: {
                V1_2::CellInfoCdma cellInfo;
                convertRilCellInfoCdmaToHal(&rillCellInfo->CellInfo.cdma, cellInfo);
                records[i].ratSpecificInfo.cdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_LTE: {
                V1_5::CellInfoLte cellInfo;
                convertRilCellInfoLteToHal(&rillCellInfo->CellInfo.lte, cellInfo);
                records[i].ratSpecificInfo.lte(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                V1_5::CellInfoTdscdma cellInfo;
                convertRilCellInfoTdscdmaToHal(&rillCellInfo->CellInfo.tdscdma, cellInfo);
                records[i].ratSpecificInfo.tdscdma(cellInfo);
                break;
            }

            case RIL_CELL_INFO_TYPE_NR: {
                V1_5::CellInfoNr cellInfo;
                convertRilCellInfoNrToHal(&rillCellInfo->CellInfo.nr, cellInfo);
                records[i].ratSpecificInfo.nr(cellInfo);
                break;
            }

            default: {
                break;
            }
        }
        rillCellInfo += 1;
    }
}

// string -> LinkAddress
void convertRilDataCallToHal(RIL_Data_Call_Response_v11 *dcResponse,
    V1_5::SetupDataCallResult& dcResult, vector<vector<string>> &tmp, int idx) {
    if (dcResponse) {
        dcResult.cause = (V1_4::DataCallFailCause) dcResponse->status;
        dcResult.suggestedRetryTime = dcResponse->suggestedRetryTime;
        dcResult.cid = dcResponse->cid;
        dcResult.active = (V1_4::DataConnActiveStatus)dcResponse->active;
        dcResult.type = (V1_4::PdpProtocolType) getPdpType(dcResponse->type);
        dcResult.ifname = convertCharPtrToHidlString(dcResponse->ifname);
        convertLinkAddress(tmp[idx*4+0], dcResult.addresses, dcResponse->addresses);
        parseStrings(tmp[idx*4+1], dcResult.dnses, dcResponse->dnses);
        parseStrings(tmp[idx*4+2], dcResult.gateways, dcResponse->gateways);
        parseStrings(tmp[idx*4+3], dcResult.pcscf, dcResponse->pcscf);
        dcResult.mtuV4 = dcResponse->mtu;
        dcResult.mtuV6 = dcResponse->mtu;
    }
}

bool convertRilRegStateToHal(RIL_RegStateResult *from, size_t responselen, V1_5::RegStateResult& to) {
    if (from == NULL ||
            responselen != sizeof(RIL_RegStateResult)) {
        RLOGE("convertRilRegStateToHal Invalid resonse: NULL");
        return false;
    }

    to.regState = (RegState) from->regState;
    to.rat = (V1_4::RadioTechnology) from->rat;
    to.reasonForDenial = (V1_5::RegistrationFailCause) from->reasonForDenial;
    to.registeredPlmn = convertCharPtrToHidlString(from->registeredPlmn);

    switch (RadioConvUtils::getRadioTechnologyToAccessNetworkType((int)from->rat)) {
        case ACCESS_NETWORK_EUTRAN:
        case ACCESS_NETWORK_NGRAN:
            V1_5::RegStateResult::AccessTechnologySpecificInfo::EutranRegistrationInfo eutranReginfo;
            eutranReginfo.lteVopsInfo.isVopsSupported
                = from->accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isVopsSupported;
            eutranReginfo.lteVopsInfo.isEmcBearerSupported
                = from->accessTechnologySpecificInfo.eutranInfo.lteVopsInfo.isEmcBearerSupported;
            eutranReginfo.nrIndicators.isEndcAvailable
                = from->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isEndcAvailable;
            eutranReginfo.nrIndicators.isDcNrRestricted
                = from->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isDcNrRestricted;
            eutranReginfo.nrIndicators.isNrAvailable
                = from->accessTechnologySpecificInfo.eutranInfo.nrIndicators.isNrAvailable;
            to.accessTechnologySpecificInfo.eutranInfo(eutranReginfo);
            break;
        case ACCESS_NETWORK_CDMA2000:
            V1_5::RegStateResult::AccessTechnologySpecificInfo::Cdma2000RegistrationInfo cdmaReginfo;
            cdmaReginfo.cssSupported
                = from->accessTechnologySpecificInfo.cdmaInfo.cssSupported;
            cdmaReginfo.roamingIndicator
                = from->accessTechnologySpecificInfo.cdmaInfo.roamingIndicator;
            cdmaReginfo.systemIsInPrl
                = (V1_5::PrlIndicator) from->accessTechnologySpecificInfo.cdmaInfo.systemIsInPrl;
            cdmaReginfo.defaultRoamingIndicator
                = from->accessTechnologySpecificInfo.cdmaInfo.defaultRoamingIndicator;
            to.accessTechnologySpecificInfo.cdmaInfo(cdmaReginfo);
            break;
        default:
            break;
    }

    convertRilCellIdentityToHal(to.cellIdentity, from->cellIdentity);
    return true;
}

int ConvertUtf8ToUcs2(const unsigned char *src, const int srcLen,
        unsigned short *dst, const int dstLen) {
    int i = 0, char_size = 0, ucs2_idx = 0;
    bool isOutOfBoundForSrcArr = false;
    auto isValidIndexForSrc = [] (int idx, int len, int dstSize) {
        return idx + len < dstSize;
    };

    while ( i < srcLen ) {
        if ((src[i] & 0x80) == 0) {            /* 0xxxxxxx : 000000 - 00007f */                             /* 127 : ASCII */
            char_size = 1;
            if (isValidIndexForSrc(i, char_size, srcLen)) {
                dst[ucs2_idx++] = (unsigned short) (src[i] & 0x7f);
            } else {
                isOutOfBoundForSrcArr = true;
            }
        } else if ((src[i] & 0xe0) == 0xc0) {  /* 110xxxxx 10xxxxxx : 000080 - 0007ff */                    /* 2047 */
            char_size = 2;
            if (isValidIndexForSrc(i, char_size, srcLen)) {
                dst[ucs2_idx++] = (unsigned short) (((src[i] & 0x1f) << 6) | (src[i + 1] | 0x3f));
            } else {
                isOutOfBoundForSrcArr = true;
            }
        } else if ((src[i] & 0xf0) == 0xe0) {  /* 1110xxxx 10xxxxxx 10xxxxxx : 000800 - 00ffff */           /* 65535 : UCS2 */
            char_size = 3;
            if (isValidIndexForSrc(i, char_size, srcLen)) {
                dst[ucs2_idx++] = (unsigned short) (((src[i] & 0xf) << 12)
                    | ((src[i + 1] & 0x3f) << 6) | (src[i + 2] & 0x3f));
            } else {
                isOutOfBoundForSrcArr = true;
            }
        } else {                               /* 1110zzz 10zzxxxx 10xxxxxx 10xxxxxx : 010000 - 10ffff */   /* 1114111 : Unicode */
            RLOGE("[%s] UCS2 cannot supported case", __FUNCTION__);
            break;
        }

        if (isOutOfBoundForSrcArr) {
            RLOGE("[%s] current idx[%d] can overflow max size[%d] because of char_size[%d]",
                    __FUNCTION__, i, srcLen, char_size);
            break;
        }

        if (ucs2_idx >= dstLen) {
            RLOGE("[%s] ucs2_idx[%d] is bigger than maximum size[%d]", __FUNCTION__,
                    ucs2_idx, dstLen);
            break;
        }

        i += char_size;
    }

    return ucs2_idx;
}

int convertStringToAdnString(const string &data, unsigned char *pDst, int dstLen) {
    int length = data.size();
    unsigned char srcData[MAX_NAME_LEN + 1] = { 0, };
    if (length > MAX_NAME_LEN) length = MAX_NAME_LEN;
    memcpy(srcData, data.c_str(), length);

    int decLen = ConvertUtf8ToUcs2(srcData, length, (unsigned short *) pDst, dstLen);
    RLOGD("[%s] converted UCS2 format string len:%d", __FUNCTION__, decLen);

    return decLen;
}

bool convertRilRadioAccessSpecifierToHal(void *response, size_t responseLen,
        V1_5::RadioAccessSpecifier& ras) {
    if (response == NULL || responseLen != sizeof(RIL_RadioAccessSpecifier_V1_5)) {
        RLOGE("convertRilRadioAccessSpecifierToHal Invalid response: NULL");
        return false;
    }

    RIL_RadioAccessSpecifier_V1_5 *rilRas = (RIL_RadioAccessSpecifier_V1_5 *)response;
    ras.radioAccessNetwork = (V1_5::RadioAccessNetworks)rilRas->radio_access_network;

    int bandsLen = rilRas->bands_length;

    switch(ras.radioAccessNetwork) {
        case V1_5::RadioAccessNetworks::GERAN:
        {
            hidl_vec<V1_1::GeranBands> geran;
            geran.resize(bandsLen);
            for (int i = 0; i < bandsLen; i++) {
                geran[i] = (V1_1::GeranBands)rilRas->bands.geran_bands[i];
            }
            ras.bands.geranBands(geran);
            break;
        }
        case V1_5::RadioAccessNetworks::UTRAN:
        {
            hidl_vec<V1_5::UtranBands> utran;
            utran.resize(bandsLen);
            for (int i = 0; i < bandsLen; i++) {
                utran[i] = (V1_5::UtranBands)rilRas->bands.utran_bands[i];
            }
            ras.bands.utranBands(utran);
            break;
        }
        case V1_5::RadioAccessNetworks::EUTRAN:
        {
            hidl_vec<V1_5::EutranBands> eutran;
            eutran.resize(bandsLen);
            for (int i = 0; i < bandsLen; i++) {
                eutran[i] = (V1_5::EutranBands)rilRas->bands.eutran_bands[i];
            }
            ras.bands.eutranBands(eutran);
            break;
        }
        case V1_5::RadioAccessNetworks::NGRAN:
        {
            hidl_vec<V1_5::NgranBands> ngran;
            ngran.resize(bandsLen);
            for (int i = 0; i < bandsLen; i++) {
                ngran[i] = (V1_5::NgranBands)rilRas->bands.ngran_bands[i];
            }
            ras.bands.ngranBands(ngran);
            break;
        }
        default:
        RLOGE("convertRilRadioAccessSpecifierToHal Invalid RIL_RadioAccessNetwork");
        return false;
    }

    ras.channels.resize(0);
    if (rilRas->channels_length > 0) {
        ras.channels.resize(rilRas->channels_length);
        for (uint32_t i = 0; i < rilRas->channels_length; i++) {
            ras.channels[i] = (int32_t)rilRas->channels[i];
        }
    }
    return true;
}

} // namespace Radio2HalUtils
