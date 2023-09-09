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
 * networkregistrationinfo.cpp
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#include "networkregistrationinfo.h"
#include <librilutils/radio_conv_utils.h>
#include <librilutils/textutils.h>
#include "mcctable.h"
#include "netdatabuilder.h"
#include "networkserviceproxy.h"
#include "networkutils.h"
#include "eonsResolver.h"
#include "operatortable.h"
#include "operatorinforesolver.h"
#include "protocolnetadapter.h"
#include "protocolnetbuilder.h"
#include "rilapplication.h"
#include "rillog.h"
#include "service.h"
#include "servicestate.h"
#include "operatorinfo.h"
#include <librilutils/systemproperty.h>
#include "../../../base/modemconfiguration.h"
#include "util.h"

#define TIMEOUT_NET_DEFAULT 30000

#define CHECK_NULL_MSG(msg)                                                          \
    {                                                                                \
        if (msg == nullptr) {                                                        \
            RilLogE("[%s] %s %s() msg is nullptr", mServiceName, TAG, __FUNCTION__); \
            return -1;                                                               \
        }                                                                            \
    }

#define CHECK_NULL_RSP(msg)                                                                       \
    {                                                                                             \
        if (msg == nullptr || msg->GetModemData() == nullptr) {                                   \
            RilLogE("[%s] %s %s() msg or ModemData is nullptr", mServiceName, TAG, __FUNCTION__); \
            return -1;                                                                            \
        }                                                                                         \
    }

static const char *ConvertRegStateToString(int regState) {
    switch (regState) {
    case NOT_REGISTERED:
        return "NOT_REGISTERED";
    case REGISTERED_HOME:
        return "REGISTERED_HOME";
    case SEARCHING:
        return "SEARCHING";
    case DENIED:
        return "DENIED";
    case UNKNOWN:
        return "UNKNOWN";
    case REGISTERED_ROAMING:
        return "REGISTERED_ROAMING";

    case NOT_REGISTERED_EMERGENCY_ONLY:
        return "NOT_REGISTERED_EMERGENCY_ONLY";
    case SEARCHING_EMERGENCY_ONLY:
        return "SEARCHING_EMERGENCY_ONLY";
    case DENIED_EMERGENCY_ONLY:
        return "DENIED_EMERGENCY_ONLY";
    case UNKNOWN_EMERGENCY_ONLY:
        return "UNKNOWN_EMERGENCY_ONLY";

    case DENIED_ROAMING:
        return "DENIED_ROAMING";
    }
    return "<NOT-DEFINED>";
}

static const char *ConvertRadioTechToString(int rat) {
    switch (rat) {
    case RADIO_TECH_GPRS:
        return "RADIO_TECH_GPRS";
    case RADIO_TECH_EDGE:
        return "RADIO_TECH_EDGE";
    case RADIO_TECH_UMTS:
        return "RADIO_TECH_UMTS";
    case RADIO_TECH_IS95A:
        return "RADIO_TECH_IS95A";
    case RADIO_TECH_IS95B:
        return "RADIO_TECH_IS95B";
    case RADIO_TECH_1xRTT:
        return "RADIO_TECH_1xRTT";
    case RADIO_TECH_EVDO_0:
        return "RADIO_TECH_EVDO_0";
    case RADIO_TECH_EVDO_A:
        return "RADIO_TECH_EVDO_A";
    case RADIO_TECH_HSDPA:
        return "RADIO_TECH_HSDPA";
    case RADIO_TECH_HSUPA:
        return "RADIO_TECH_HSUPA";
    case RADIO_TECH_HSPA:
        return "RADIO_TECH_HSPA";
    case RADIO_TECH_EVDO_B:
        return "RADIO_TECH_EVDO_B";
    case RADIO_TECH_EHRPD:
        return "RADIO_TECH_EHRPD";
    case RADIO_TECH_LTE:
        return "RADIO_TECH_LTE";
    case RADIO_TECH_HSPAP:
        return "RADIO_TECH_HSPAP";
    case RADIO_TECH_GSM:
        return "RADIO_TECH_GSM";
    case RADIO_TECH_IWLAN:
        return "RADIO_TECH_IWLAN";
    case RADIO_TECH_TD_SCDMA:
        return "RADIO_TECH_TD_SCDMA";
    case RADIO_TECH_LTE_CA:
        return "RADIO_TECH_LTE_CA";
    case RADIO_TECH_NR:
        return "RADIO_TECH_NR";
    default:
        return "RADIO_TECH_UNKNOWN";
    }
}

static bool IsRegisteredState(int regState) {
    return (regState == REGISTERED_HOME || regState == REGISTERED_ROAMING);
}

static void ModifyRegStateOnDenied(int &regState, int rejectCause, const char *servicName) {
    if (regState == DENIED) {
        if (rejectCause == NET_REJ_CAUSE_NO_SUITABLE_CELLS_IN_THIS_LOCATION_AREA) {
            regState = NOT_REGISTERED;
            RilLogI("[%s] RegState from [DENIED] to [NOT_REGISTERED] due to reject cause", servicName);
        } else {
            regState = DENIED_EMERGENCY_ONLY;
        }
    }
}

/**
 * VoiceRegistrationStateHandler
 */
static void ModifyVoiceRegStateVoiceRatUsingImsRegState(NetworkServiceProxy &network, int &regState, int &rat, const char *servicName) {
    if ((regState == NOT_REGISTERED || regState == SEARCHING) && network.GetImsRegistrationState() == RIL_IMS_REGISTERED) {
        regState = REGISTERED_HOME;
        rat = network.GetDataRadioTech();
        RilLogI("[%s] Voice RegStat to [REGISTERED_HOME] due to IMS registration", servicName);
    }
}

static void ModifyRegStateOnSearching(int &regState) {
    if (regState == SEARCHING) {
        regState = SEARCHING_EMERGENCY_ONLY;
    }
}

static void ModifyRatOnUnknown(int regState, int &rat) {
    if (regState == NOT_REGISTERED || regState == SEARCHING || regState == DENIED || regState == UNKNOWN || regState == DENIED_ROAMING) {
        rat = RADIO_TECH_UNKNOWN;
    }
}

static void FakePsRestricted(Service *pService, int &regState) {
    if (pService == nullptr)
        return;
    bool isPsRestrictedByModem = false;
    RilProperty *property = pService->GetRilContextProperty();
    if (property != nullptr) {
        isPsRestrictedByModem = property->GetBool(RIL_CONTEXT_NET_PS_RESTRICTED, false);
    }
    // Fake NOT_REGISTERED due to suspended
    if (isPsRestrictedByModem) {
        RilLogW("[%d] NOT_REGISTERED(%d) by PS restricted from %d", pService->GetRilSocketId(), NOT_REGISTERED, regState);
        regState = NOT_REGISTERED;
    }
}

static void UpdateVoiceNetworkInfo(NetworkServiceProxy &network, int regState, int rat) {
    int curVoiceRegState = network.GetVoiceRegistrationState();
    if (curVoiceRegState != regState) {
        network.SetVoiceRegistrationState(regState);
        network.UpdateDbEccInfo();
    }
    network.SetVoiceRadioTech(rat);
}

static void UpdateCdmaSystemProperty(int socketId, int regState, int sid, int nid, int cid, int stationid) {
    char sidKeyName[32];
    char nidKeyName[32];
    char cidKeyName[32];
    char bssidKeyName[32];
    char sidValue[16];
    char nidValue[16];
    char cidValue[16];
    char bssidValue[16];

    snprintf(sidKeyName, sizeof(sidKeyName), "vendor.ril.cdma.sid%d", socketId);
    snprintf(nidKeyName, sizeof(nidKeyName), "vendor.ril.cdma.nid%d", socketId);
    snprintf(cidKeyName, sizeof(nidKeyName), "vendor.ril.gsm.cid%d", socketId);
    snprintf(bssidKeyName, sizeof(bssidKeyName), "vendor.ril.cdma.bssid%d", socketId);
    if (IsRegisteredState(regState)) {
        snprintf(sidValue, 16, "%d", sid);
        snprintf(nidValue, 16, "%d", nid);
        snprintf(cidValue, 16, "%d", cid);
        snprintf(bssidValue, 16, "%d", stationid);
    } else {
        snprintf(sidValue, 16, "%d", -1);
        snprintf(nidValue, 16, "%d", -1);
        snprintf(cidValue, 16, "%d", -1);
        snprintf(bssidValue, 16, "%d", -1);
    }
    SystemProperty::Set(sidKeyName, sidValue);
    SystemProperty::Set(nidKeyName, nidValue);
    SystemProperty::Set(bssidKeyName, bssidValue);
    SystemProperty::Set(cidKeyName, cidValue);
}

static void NotifyUnsolVoiceRadioTechChanged(Service *pService, NetworkServiceProxy &network, int regState, int rat) {
    if (pService == nullptr)
        return;

    int curVoiceRegState = network.GetVoiceRegistrationState();
    int curVoiceRat = network.GetVoiceRadioTech();
    if ((IsRegisteredState(regState) && curVoiceRat != rat) || (!IsRegisteredState(curVoiceRegState) && IsRegisteredState(regState))) {
        pService->OnUnsolicitedResponse(RIL_UNSOL_VOICE_RADIO_TECH_CHANGED, &rat, sizeof(rat));
    }
}

static void NotifyVoiceRegistrationFailed(Service *pService, NetworkServiceProxy &network, int regState, int rejectCause) {
    if (pService == nullptr)
        return;

    RilProperty *property = pService->GetRilApplicationProperty();
    if (property != nullptr) {
        int target_op = property->GetInt(RIL_APP_TARGET_OPER);
        RilLogV("[%s] Check target operator=%d", pService->GetServiceName(), target_op);
        if (TARGET_OPER_ATT == target_op) {
            RilLogI("[%s] ATT specification - request to show notification to user according to rejectCause",
                    pService->GetServiceName());
            network.NotifyVoiceRegistrationFailed(regState, rejectCause);
        }
    }
}

IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(VoiceRegistrationStateHandler, MSG_NET_VOICE_REGISTRATION_STATE)

int VoiceRegistrationStateHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    int halVer = RilApplication::RIL_HalVersionCode;
    int radioState = network.GetRadioState();
    if (radioState == RADIO_STATE_UNAVAILABLE) {
        VoiceRegStateResultBuilder builder(halVer);
        builder.SetRegistrationState(NOT_REGISTERED, RADIO_TECH_UNKNOWN, 0);
        auto rildata = builder.Build();
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_RADIO_NOT_AVAILABLE);
        }
        return 0;
    }

    ProtocolNetworkRegistrationBuilder builder;
    ModemData *pModemData = builder.BuildNetworkRegistrationState(DOMAIN_VOICE_NETWORK);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int VoiceRegistrationStateHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    ProtocolNetVoiceRegStateAdapter adapter(msg->GetModemData());
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int regState = adapter.GetRegState();
        int lac = adapter.GetLAC();
        int cid = adapter.GetCellId();
        int rat = (RIL_RadioTechnology)adapter.GetRadioTech();
        int rejectCause = adapter.GetRejectCause();
        int psc = adapter.GetPSC();
        int tac = adapter.GetTAC();
        int pcid = adapter.GetPCID();
        int eci = adapter.GetECI();
        int stationid = adapter.GetStationId();
        int stationidLat = adapter.GetStationLat();
        int stationidLong = adapter.GetStationLong();
        int concur = adapter.GetConCurrent();
        int sid = adapter.GetSystemId();
        int nid = adapter.GetNetworkId();
        int roaming = adapter.GetRoamingInd();
        int regPrl = adapter.GetRegPrl();
        int roaming_prl = adapter.GetRoamingIndPrl();
        int channel = adapter.getChannelNumber();
        long int nrCid = adapter.getNrCid();

        // The information from CP is modified based on condition.
        ModifyVoiceRegStateVoiceRatUsingImsRegState(network, regState, rat, mServiceName);
        ModifyRegStateOnDenied(regState, rejectCause, mServiceName);
        ModifyRegStateOnSearching(regState);
        ModifyRatOnUnknown(regState, rat);

        NotifyUnsolVoiceRadioTechChanged(mService, network, regState, rat);
        NotifyVoiceRegistrationFailed(mService, network, regState, rejectCause);

        UpdateVoiceNetworkInfo(network, regState, rat);
        UpdateCdmaSystemProperty(mService->GetRilSocketId(), regState, sid, nid, cid, stationid);

        RilLogI("[%s] Voice RegState=%s(0x%02x), RAT=%s(0x%02x), RejectCause=%d",
                mServiceName, ConvertRegStateToString(regState), regState, ConvertRadioTechToString(rat), rat, rejectCause);

        ServiceState ss = mService->GetRadioServiceState();
        VoiceRegStateResultBuilder builder(mService->GetCurrentRequestDataHalVersion(RilApplication::RIL_HalVersionCode));
        builder.SetRegistrationState(regState, rat, rejectCause);
        builder.SetRegisteredPlmn(ss.getOperatorNumeric().c_str());
        builder.SetCellIdentity(ss.getOperatorNumericRaw().c_str(),
                                ss.getOperatorAlphaLongRaw().c_str(), ss.getOperatorAlphaShortRaw().c_str());
        builder.SetCellIdentity(lac, cid, psc, tac, pcid, eci, channel, nrCid);
        if (ACCESS_NETWORK_CDMA2000 ==
            NetworkUtils::getRadioTechnologyToAccessNetworkType(rat)) {
            builder.SetCdmaState(concur, roaming, regPrl, roaming_prl);
            builder.SetCdmaCellIdentity(stationid, stationidLat, stationidLong, sid, nid);
        }
        auto rildata = builder.Build();
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * DataRegistrationStateHandler
 */
static void ModifyRegStateOnDeniedRoaming(int &regState) {
    if (DENIED_ROAMING == regState) {
        regState = DENIED;
    }
}

static void ModifyNrIndicatorsForTest(NetworkServiceProxy &network, bool &isNrAvailable, bool &isDcNrRestricted, bool &isEndcAvailable) {
    if (network.IsNrTestMode()) {
        const string keyNrAvail{"persist.vendor.radio.nr_avail"};
        const string keyDcNrRestricted{"persist.vendor.radio.dcnr_restrict"};
        const string keyEndcAvail{"persist.vendor.radio.endc_avail"};

        int val = SystemProperty::GetInt(keyNrAvail, 0);
        RilLogV("%s [%d]", keyNrAvail.c_str(), val);
        isNrAvailable = (val == 1);

        val = SystemProperty::GetInt(keyDcNrRestricted, 0);
        RilLogV("%s [%d]", keyDcNrRestricted.c_str(), val);
        isDcNrRestricted = (val == 1);

        val = SystemProperty::GetInt(keyEndcAvail, 0);
        RilLogV("%s [%d]", keyEndcAvail.c_str(), val);
        isEndcAvailable = (val == 1);
    }
}

static void UpdateDeniedRoamSystemProperty(int regState, int phoneId) {
    string propName = string{RIL_NET_DINIED_ROAM} + to_string(phoneId);
    const int notApplied = 0;
    //const int csDeniedRoam = 1;
    const int psDeniedRoam = 2;

    if (DENIED_ROAMING == regState) {
        RilLogV("Reg state : denied_roaming");
        SystemProperty::Set(propName, psDeniedRoam);
    } else {
        SystemProperty::Set(propName, notApplied);
    }
}

static void UpdateCellIdSystemProperty(int regState, int cid, int phoneId) {
    char data_cidKeyName[32];
    char data_cidValue[16];
    snprintf(data_cidKeyName, sizeof(data_cidKeyName), "vendor.ril.gsm.data_cid%d", phoneId);
    if (IsRegisteredState(regState))
        snprintf(data_cidValue, 16, "%d", cid);
    else
        snprintf(data_cidValue, 16, "%d", -1);
    SystemProperty::Set(data_cidKeyName, data_cidValue);
}

static void UpdateVolteEmcServiceStatus(NetworkServiceProxy &network, bool volteAvailable, bool emcService,
                                        int rat, int regState) {
    network.WriteVolteEmcServiceStatus(volteAvailable, emcService, rat, regState, true);
}

static void UpdateDataNetworkInfo(NetworkServiceProxy &network, int regState, int rat) {
    network.SetDataRadioTech(rat);
    network.SetDataRegistrationState(regState);
}

IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(DataRegistrationStateHandler, MSG_NET_DATA_REGISTRATION_STATE)

int DataRegistrationStateHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    int radioState = network.GetRadioState();
    if (radioState == RADIO_STATE_UNAVAILABLE) {
        DataRegStateResultBuilder builder(mService->GetCurrentRequestDataHalVersion(RilApplication::RIL_HalVersionCode));
        builder.SetRegistrationState(NOT_REGISTERED, RADIO_TECH_UNKNOWN, 0, 0);
        auto rildata = builder.Build();
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_RADIO_NOT_AVAILABLE);
        }
        return 0;
    }

    ProtocolNetworkRegistrationBuilder builder;
    ModemData *pModemData = builder.BuildNetworkRegistrationState(DOMAIN_DATA_NETWORK);
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int DataRegistrationStateHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    ProtocolNetDataRegStateAdapter adapter(msg->GetModemData());
    int errorCode = adapter.GetErrorCode();
    if (errorCode == RIL_E_SUCCESS) {
        int regState = adapter.GetRegState();
        int lac = adapter.GetLAC();
        int cid = adapter.GetCellId();
        int rat = (RIL_RadioTechnology)adapter.GetRadioTech();
        int sdc = adapter.GetMaxSDC();
        int rejectCause = adapter.GetRejectCause();
        int psc = adapter.GetPSC();
        int tac = adapter.GetTAC();
        int pcid = adapter.GetPCID();
        int eci = adapter.GetECI();
        int imsVops = adapter.GetImsVops();
        int emcService = adapter.GetEmcService();
        int channel = adapter.getChannelNumber();
        long int nrCid = adapter.getNrCid();
        // NR (> 1.4)
        bool isEndcAvailable = adapter.IsEndcAvailable();
        bool isDcNrRestricted = adapter.IsDcNrRestricted();
        bool isNrAvailable = adapter.IsNrAvailable();
        int emfSupported = adapter.GetEmf();

        ModifyNrIndicatorsForTest(network, isNrAvailable, isDcNrRestricted, isEndcAvailable);

        UpdateDeniedRoamSystemProperty(regState, mService->GetRilSocketId());
        ModifyRegStateOnDeniedRoaming(regState);
        ModifyRegStateOnDenied(regState, rejectCause, mServiceName);
        ModifyRatOnUnknown(regState, rat);
        FakePsRestricted(mService, regState);

        bool volteAvailable = (imsVops > VOPS_NOT_SUPPORTED);
        bool isNrEmcBearerSupported = (emcService == EMC_NR_CONNECTED_TO_5GCN || emcService == EMC_BOTH_NR_EUTRA_CONNECTED_TO_5GCN);
        RilLogI("[%s] Data RegState=%s(0x%02x), RAT=%s(0x%02x), RejectCause=%d",
                mServiceName, ConvertRegStateToString(regState), regState, ConvertRadioTechToString(rat), rat, rejectCause);
        RilLogV("Data Registration State{VoLTE=%d,EMC=%d,EMC_NR=%s)}", volteAvailable, emcService, isNrEmcBearerSupported ? "true" : "false");
        RilLogV("[%s] NrIndicators status (isEndcAvailable=%s,isDcNrRestricted=%s,isNrAvailable=%s)",
                mServiceName,
                isEndcAvailable ? "true" : "false",
                isDcNrRestricted ? "true" : "false",
                isNrAvailable ? "true" : "false");
        UpdateDataNetworkInfo(network, regState, rat);
        UpdateVolteEmcServiceStatus(network, volteAvailable, emcService, adapter.GetRadioTech(), regState);
        UpdateCellIdSystemProperty(regState, cid, mService->GetRilSocketId());

        ServiceState ss = mRilContext->GetRadioServiceState();
        DataRegStateResultBuilder builder(RilApplication::RIL_HalVersionCode);
        builder.SetRegistrationState(regState, rat, rejectCause, sdc);
        builder.SetRegisteredPlmn(ss.getOperatorNumeric().c_str());
        builder.SetCellIdentity(ss.getOperatorNumericRaw().c_str(),
                                ss.getOperatorAlphaLongRaw().c_str(),
                                ss.getOperatorAlphaShortRaw().c_str());
        builder.SetCellIdentity(lac, cid, psc, tac, pcid, eci, channel, nrCid);

        int ran = NetworkUtils::getRadioTechnologyToAccessNetworkType(rat);
        if (ran == ACCESS_NETWORK_NGRAN) {
            builder.SetNrVopsInfo(imsVops, emcService, emfSupported);
        } else if (ran == ACCESS_NETWORK_EUTRAN) {
            builder.SetLteVopsInfo(volteAvailable, isNrEmcBearerSupported);
            builder.SetNrIndicators(isEndcAvailable, isDcNrRestricted, isNrAvailable);
        }

        auto rildata = builder.Build();
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_INTERNAL_ERR);
        }
    } else {
        OnRequestComplete(errorCode);
    }

    return 0;
}

/**
 * NetworkStateChangedHandler
 */
IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(NetworkStateChangedHandler, MSG_NET_NETWORK_STATE_CHANGED)

int NetworkStateChangedHandler::OnIndication(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED);
    return 0;
}

/**
 * OperatorHandler
 */
static bool IsNetworkRegOrDenied(Service *pService) {
    if (pService->GetRilContext() != nullptr) {
        const ServiceState ss = pService->GetRilContext()->GetRadioServiceState();
        return !(ss.isVoiceOos() && ss.isDataOos());
    }
    return false;
}

static string GetIccId(Service *pService) {
    RilProperty *property = pService->GetRilContextProperty();
    if (property != nullptr) {
        return property->GetString(RIL_CONTEXT_SIM_ICC_ID);
    } else {
        return string{""};
    }
}

static void UpdateNameProvider(Service *pService, const NetworkOperator &netOperator) {
    string iccId = GetIccId(pService);
    OperatorNameProvider *provider = OperatorNameProvider::GetInstance();

    if(provider == nullptr || !netOperator.IsRegStateInRegRange() || !netOperator.AreBothLongPlmnShortPlmnAvailable()) {
        return;
    }

    if (provider->UpdateEons(iccId, netOperator) == false) {
        if (provider->InsertEons(iccId, netOperator) == true) {
            RilLogV("Operator: insert to Eons cache for finalIccId=%s", iccId.c_str());
        }
    }

    if (provider->Contains(netOperator.GetMcc(), netOperator.GetMnc(), netOperator.GetGid1())) {
        if (provider->Update(netOperator)) {
            RilLogV("Operator: update to DB");
        }
    } else {
        provider->Insert(netOperator);
        RilLogV("Operator: insert to DB");
    }
}

static void UpdateRilProperty(Service *pService, const NetworkOperator &netOperator) {
    const string numericPlmn = netOperator.GetNumeric();
    RilProperty *pRilContextProperty = pService->GetRilContextProperty();
    if (pRilContextProperty != nullptr) {
        pRilContextProperty->Put(RIL_CONTEXT_NET_CURRENT_PLMN, numericPlmn.c_str());
    }

    RilProperty *pRilAppProperty = pService->GetRilApplicationProperty();
    if (pRilAppProperty != nullptr) {
        int overallNetState = pRilAppProperty->GetInt(RIL_OVERALL_NET_CONN_PRESENCE, 0);

        if (MccTable::isValidPlmn(numericPlmn.c_str()) &&
            (numericPlmn.compare(INVALID_PLMN))) {
            overallNetState |= (0x01 << pService->GetRilSocketId());
        } else {
            overallNetState &= ~(0x01 << pService->GetRilSocketId());
        }
        pRilAppProperty->Put(RIL_OVERALL_NET_CONN_PRESENCE, overallNetState);
    }
}

string GetSimGid1Num(RilProperty *pProperty) {
    if (pProperty == nullptr) {
        return GID_DEFAULT_VALUE_ANY;
    }

    string gid1 = pProperty->GetString(RIL_CONTEXT_SIM_GID1);
    if (TextUtils::IsEmpty(gid1)) {
        return GID_DEFAULT_VALUE_ANY;
    }

    return gid1;
}

OperatorInfo decideOperatorName(const NetworkOperator &netOperator, const SimOperator &simOperator, int phoneId) {
    OperatorInfo opname = RegisteredOperatorInfoResolver().resolve(
            netOperator,
            simOperator,
            EonsResolver::createForPhoneId(phoneId));

    // TOE processing
    int typeOfLongEnhancement = netOperator.GetTypeOfLongEnhancement();
    if (typeOfLongEnhancement > 0) {
        if (NetworkUtils::isLteEnhancedAvailable(opname.alphaLong) == 0) {
            opname.alphaLong = NetworkUtils::addLteEnhancedAvailableToAlphaTag(opname.alphaLong, typeOfLongEnhancement);
        }
    }

    int typeOfShortEnhancement = netOperator.GetTypeOfLongEnhancement();
    if (typeOfShortEnhancement > 0) {
        if (NetworkUtils::isLteEnhancedAvailable(opname.alphaShort) == 0) {
            opname.alphaShort = NetworkUtils::addLteEnhancedAvailableToAlphaTag(opname.alphaShort, typeOfShortEnhancement);
        }
    }

    return opname;
}

IMPLEMENT_TINY_SERVICE_HANDLER_CLASS_INIT(OperatorHandler, MSG_NET_OPERATOR)

int OperatorHandler::OnRequest(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_MSG(msg);

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    ProtocolNetworkRegistrationBuilder builder;
    ModemData *pModemData = builder.BuildOperator();
    if (SendRequest(pModemData, TIMEOUT_NET_DEFAULT, mMessageId) < 0) {
        return -1;
    }
    return 0;
}

int OperatorHandler::OnResponse(Message *msg) {
    RilLogI("[%s] %s %s()", mServiceName, TAG, __FUNCTION__);
    CHECK_NULL_RSP(msg);
    ProtocolNetOperatorAdapter adapter(msg->GetModemData());

    NetworkServiceProxy network(mService);
    if (!network.HasService()) {
        return -1;
    }

    const bool isIwlanEn = network.IsIwlanEnabled();
    const SimOperator simOperator(GetSimOperatorNum(mPhoneId), GetSimSpn(mPhoneId), GetIccId(mService));
    const int errorCode = adapter.GetErrorCode();
    const string strSimGid1 = (adapter.GetRegState() == OPERATOR_REG_HOME) ?
        GetSimGid1Num(mRilContext->GetProperty()) : GID_DEFAULT_VALUE_ANY;

    if (errorCode == RIL_E_SUCCESS) {
        const NetworkOperator netOperator(adapter.GetPlmn(), adapter.GetLongPlmn(), adapter.GetShortPlmn(), adapter.GetRegState(), adapter.GetLac(), strSimGid1.c_str());

        RilLogV("Operator {%s, %s, isIwlanEn=%d}",
                netOperator.PrintToString().c_str(), simOperator.PrintToString().c_str(), isIwlanEn);

        UpdateNameProvider(mService, netOperator);
        UpdateRilProperty(mService, netOperator);

        OperatorInfo resultOpName;
        OperatorInfo rawOpnName;
        if (isIwlanEn && simOperator.IsNumericAvailable()) {
            if (!netOperator.IsNumericAvailable() || netOperator.IsRegNot()) {
                resultOpName = WifiOperatorInfoResolver().resolve(simOperator.GetNumeric());
            } else {
                resultOpName = WifiOperatorInfoResolver().resolve(netOperator.GetNumeric());
            }
        } else {
            rawOpnName = decideOperatorName(netOperator, simOperator, mPhoneId);
            if (IsNetworkRegOrDenied(mService) || !(netOperator.IsRegNot())) {
                resultOpName = rawOpnName;
            }
        }

        NetworkDataBuilder builder;
        auto rildata = builder.BuildOperatorResponse(resultOpName, rawOpnName);
        if (rildata != nullptr) {
            OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
        } else {
            OnRequestComplete(RIL_E_GENERIC_FAILURE);
        }

        network.UpdateDbEccInfo(false /*forceToUpdate*/);
    } else {
        if (isIwlanEn && simOperator.IsNumericAvailable()) {
            RilLogV("Operator {%s, isIwlanEn=%d}", simOperator.PrintToString().c_str(), isIwlanEn);
            OperatorInfo opname = WifiOperatorInfoResolver().resolve(simOperator.GetNumeric());
            NetworkDataBuilder builder;
            auto rildata = builder.BuildOperatorResponse(opname, OperatorInfo());
            if (rildata != nullptr) {
                OnRequestComplete(RIL_E_SUCCESS, rildata->GetData(), rildata->GetDataLength());
                return 0;
            }
        }
        OnRequestComplete(errorCode);
    }

    return 0;
}
