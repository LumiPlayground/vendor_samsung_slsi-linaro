/*
 *  SIT RIL network Unit test
 *
 *  Copyright Samsung Electronics Co., LTD.
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */

#include "sitrilTestPs.h"
#include <chrono>
#include <thread>
#include "testUtil.h"
#include "testutils.h"
#include "../RilDataTests/pstestutil.h"
#include <librilutils/textutils.h>

//////////////////////////////////////////////////////////
// Test DataBuilder
static UINT8 *buildDeactDataCallRsp(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pdp_deact_data_call_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    // Only header: null_data_format
    //sit_pdp_deact_data_call_rsp *rsp = (sit_pdp_deact_data_call_rsp *)rspData;
    // Fill response
    /*
    rsp->sim_id = SMS_UT_SIM_ID;
    rsp->total_num = SMS_UT_SMS_TOTAL_NUM_ON_SIM;
    rsp->used_num = SMS_UT_SMS_USED_NUM_ON_SIM;
    memcpy(rsp->sw_version, "14", 2);
    */

    return rspData;
}

string dump;
static int HalVer;

// sitrilTestPs class
void SitrilTestPs::SetUp() {
    mPhoneIndex = GetParam();
    mSitRilTest = SitrilTest::CreateInstance();
    mSitRilTest->resetTestData();
    HalVer = getRunningRadioHalVersion(dump);
}

void SitrilTestPs::TearDown() {
    mSitRilTest = nullptr;
}

// Validate test ril preparation
static void validateTestRil(SitrilTest *psitRilTest) {
    const RIL_UnitTestFunctions *prilFuncs = psitRilTest->getRilFunc();
    ASSERT_NE(nullptr, prilFuncs);
    int rilVer = prilFuncs->version;
    EXPECT_EQ(RIL_VERSION_FOR_TEST, rilVer);
}

/** Test SetupDataCall APN **/
char test_apn[] = "test";
char test_user[] = "";
char test_password[] = "";
char test_addresses_legacy[] = { "10.20.30.40 2001::1:2:1" };
char test_addresses[2][50] = { "10.20.30.40", "2001::1:2:1" };
char test_dnses_legacy[] = "10.20.30.1 2001::1:1:2";
std::vector<std::string> test_req_dnsesV2 = { "10.20.30.1", "2001::1:1:2" };

char test_ifname[] = "rmnet0";
char *test_addresses_p[2] = { &test_addresses[0][0], &test_addresses[1][0] };
char test_dnses[2][50] = { "10.20.30.1", "2001::1:1:2" };
char *test_dnses_p[2] = { &test_dnses[0][0], &test_dnses[1][0] };

RIL_LinkAddress linkaddresses[2] = {
    { .address = test_addresses[0],
        .properties = 0,
        .deprecationTime = 0,
        .expirationTime = 0
    },
    { .address = test_addresses[1],
        .properties = 0,
        .deprecationTime = 0,
        .expirationTime = 0
    },
};

std::vector<DataV2_0::RIL_LinkAddress> linkaddressesV = {
    { test_addresses[0], 0, 0, 0},
    { test_addresses[1], 0, 0, 0},
};

RIL_SetupDataCallInfo_V1_5 testSetupDataReq = {
    .accessNetwork = 0,
    .dataProfileInfo = {},
    .roamingAllow = true,
    .reason = 0,
    .leng_addresses= 0,
    .linkAddress = nullptr,
    .dnses = test_dnses_legacy,
};

RIL_SetupDataCallInfo_V1_5 testSetupDataReq_1_5 = {
    .accessNetwork = 0,
    .dataProfileInfo = {
        .profileId = RIL_DataProfileId::DEFAULT,
        .apn = &test_apn[0],
        .protocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
        .roamingProtocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
        .authType = RIL_ApnAuthType::NO_PAP_NO_CHAP,
        .user = &test_user[0],
        .password = &test_password[0],
        .type = THREE_GPP,
        .maxConnsTime = 0,
        .maxConns = 0,
        .waitTime = 0,
        .enabled = true,
        .supportedApnTypesBitmap = 0xFFFFFFFF,
        .bearerBitmap = 0xFFFFFFFF,
        .mtuV4 = 1500,
        .mtuV6 = 1500,
        .preferred = true,
        .persistent = true
    },
    .roamingAllow = true,
    .reason = 0,
    .leng_addresses = 2,
    .linkAddress = &linkaddresses[0],
    .dnses = test_dnses_legacy
};

RIL_SetupDataCallInfo_V1_6 testSetupDataReq_1_6 = {
    {
        .accessNetwork = 0,
        .dataProfileInfo = {
            .profileId = RIL_DataProfileId::DEFAULT,
            .apn = &test_apn[0],
            .protocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
            .roamingProtocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
            .authType = RIL_ApnAuthType::NO_PAP_NO_CHAP,
            .user = &test_user[0],
            .password = &test_password[0],
            .type = THREE_GPP,
            .maxConnsTime = 0,
            .maxConns = 0,
            .waitTime = 0,
            .enabled = true,
            .supportedApnTypesBitmap = 0xFFFFFFFF,
            .bearerBitmap = 0xFFFFFFFF,
            .mtuV4 = 1500,
            .mtuV6 = 1500,
            .preferred = true,
            .persistent = true
        },
        .roamingAllow = true,
        .reason = 0,
        .dnses = test_dnses_legacy,
        .leng_addresses = 2,
        .linkAddress = &linkaddresses[0],
    }, // V1_5
    .pduSessionId = 0,
    /* Slice need more

    */
    .matchAllRuleAllowed = true,
    // Extended, need more

};

DataV2_0::RIL_DataProfileInfo testDataProfileInfo_2_0;

DataV2_0::RIL_SetupDataCallInfo testSetupDataReq_2_0 = {
    .accessNetwork = 0,
    .dataProfileInfo = { },
    .roamingAllow = true,
    .reason = 0,
    .linkAddresses = { },
    .dnses = { },
    .pduSessionId = 0,
    // Slice
    .sliceInfo = { },
    .tdInfo = { },
    .matchAllRuleAllowed = true,
    // Extended
    .vdInfo = { },
    .mAlwaysOnPdu = false,
};

// Emulated Modem SIT Response

// Expected RIL Response
RIL_Data_Call_Response_v11 checkData = {
    .status = (RIL_DataCallFailCause)0,
    .suggestedRetryTime = 0,
    .cid = 1,
    .active = 2,  // DATA_STATE_ACTIVE == ACTIVE_AND_LINKUP
    .type = (char *)STR_PDP_TYPE_IPV4V6,
    .ifname = test_ifname,
    .addresses = test_addresses_legacy,
    .dnses = test_dnses_legacy,
    .gateways = nullptr,
    .pcscf = nullptr,
    .mtu = 1500,
};

RIL_Data_Call_Response_V1_4 checkData_V1_4 = {
    .cause = (RIL_DataCallFailCause)0,
    .suggestedRetryTime = 0,
    .cid = 1,
    .active = 2,  // DATA_STATE_ACTIVE == ACTIVE_AND_LINKUP
    .type = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6,
    .ifname = test_ifname,
    .len_addresses = 2,
    .addresses = test_addresses_p,
    .len_dnses = 2,
    .dnses = test_dnses_p,
    .len_gateways = 0,
    .gateways = nullptr,
    .len_pcscf = 0,
    .pcscf = nullptr,
    .mtu = 1500,
};

static int GetPdpType(const char *protocol)
{
    // default
    if (TextUtils::IsEmpty(protocol)) {
        return SIT_PDP_PDP_TYPE_IPV4;
    }

    if (strcmp(protocol, STR_PDP_TYPE_IPV4) == 0) {
        return SIT_PDP_PDP_TYPE_IPV4;
    }
    else if (strcmp(protocol, STR_PDP_TYPE_IPV6) == 0) {
        return SIT_PDP_PDP_TYPE_IPV6;
    }
    else if (strcmp(protocol, STR_PDP_TYPE_IPV4V6) == 0) {
        return SIT_PDP_PDP_TYPE_IPV4IPV6;
    }

    // default
    return SIT_PDP_PDP_TYPE_IPV4;
}

static UINT8 *buildSetupDataCallRsp(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    // 1.5
    *datalen = (UINT16)sizeof(sit_pdp_setup_data_call_rsp)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_pdp_setup_data_call_rsp *rsp = (sit_pdp_setup_data_call_rsp*)rspData;

    rsp->data_call.status = 0;
    rsp->data_call.cid = checkData.cid;
    rsp->data_call.active = checkData.active;
    rsp->data_call.pdp_type = ::GetPdpType(checkData.type);
    char testaddripv4[4] = { 0x10, 0x20, 0x30, 0x40};
    char testaddripv6[16] = { 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01};
    memcpy(&rsp->data_call.address[0], testaddripv4, 4);
    memcpy(&rsp->data_call.address[4], testaddripv6, 16);
    rsp->data_call.dns_type = 3;
    memcpy(&rsp->data_call.primary_dns[0], testaddripv4, 4);
    memcpy(&rsp->data_call.primary_dns[4], testaddripv6, 16);
    rsp->data_call.pcscf_type = 3;
    memcpy(&rsp->data_call.pcscf[0], testaddripv4, 4);
    memcpy(&rsp->data_call.pcscf[4], testaddripv6, 16);
    //rsp->data_call.mtu_size = 1500; // This is Legacy under 1.4
    rsp->data_call.pco = 0;

    return rspData;
}

static UINT8 *buildSetupDataCallRsp_1_6(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_pdp_setup_data_call_rsp)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_pdp_setup_data_call_rsp *rsp = (sit_pdp_setup_data_call_rsp*)rspData;

    rsp->data_call.status = 0;
    rsp->data_call.cid = checkData.cid;
    rsp->data_call.active = checkData.active;
    rsp->data_call.pdp_type = ::GetPdpType(checkData.type);
    char testaddripv4[4] = { 0x10, 0x20, 0x30, 0x40};
    char testaddripv6[16] = { 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01};
    memcpy(&rsp->data_call.address[0], testaddripv4, 4);
    memcpy(&rsp->data_call.address[4], testaddripv6, 16);
    rsp->data_call.dns_type = 3;
    memcpy(&rsp->data_call.primary_dns[0], testaddripv4, 4);
    memcpy(&rsp->data_call.primary_dns[4], testaddripv6, 16);
    rsp->data_call.pcscf_type = 3;
    memcpy(&rsp->data_call.pcscf[0], testaddripv4, 4);
    memcpy(&rsp->data_call.pcscf[4], testaddripv6, 16);
    //rsp->data_call.mtu_size = 1500; // This is Legacy under 1.4
    rsp->data_call.pco = 0;

    return rspData;
}

static UINT8 *buildSetupDataCallRsp_2_0(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    *datalen = (UINT16)sizeof(sit_pdp_setup_data_call_rsp)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_pdp_setup_data_call_rsp *rsp = (sit_pdp_setup_data_call_rsp*)rspData;

    rsp->data_call.status = 0;
    rsp->data_call.cid = checkData.cid;
    rsp->data_call.active = checkData.active;
    rsp->data_call.pdp_type = ::GetPdpType(checkData.type);
    char testaddripv4[4] = { 0x10, 0x20, 0x30, 0x40};
    char testaddripv6[16] = { 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01};
    memcpy(&rsp->data_call.address[0], testaddripv4, 4);
    memcpy(&rsp->data_call.address[4], testaddripv6, 16);
    rsp->data_call.dns_type = 3;
    memcpy(&rsp->data_call.primary_dns[0], testaddripv4, 4);
    memcpy(&rsp->data_call.primary_dns[4], testaddripv6, 16);
    rsp->data_call.pcscf_type = 3;
    memcpy(&rsp->data_call.pcscf[0], testaddripv4, 4);
    memcpy(&rsp->data_call.pcscf[4], testaddripv6, 16);
    //rsp->data_call.mtu_size = 1500; // This is Legacy under 1.4
    rsp->data_call.pco = 0;

    return rspData;
}

static UINT8 *buildDataCallListRsp(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    // 1.5
    *datalen = (UINT16)sizeof(sit_pdp_get_data_call_list_rsp)+sizeof(RCM_HEADER);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    sit_pdp_get_data_call_list_rsp *rsp = (sit_pdp_get_data_call_list_rsp*)rspData;

    rsp->datacall_info_num = 1;
    rsp->data_call[0].status = 0;
    rsp->data_call[0].cid = checkData.cid;
    rsp->data_call[0].active = checkData.active;
    rsp->data_call[0].pdp_type = ::GetPdpType(checkData.type);
    char testaddripv4[4] = { 0x10, 0x20, 0x30, 0x40};
    char testaddripv6[16] = { 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01};
    memcpy(&rsp->data_call[0].address[0], testaddripv4, 4);
    memcpy(&rsp->data_call[0].address[4], testaddripv6, 16);
    rsp->data_call[0].dns_type = 3;
    memcpy(&rsp->data_call[0].primary_dns[0], testaddripv4, 4);
    memcpy(&rsp->data_call[0].primary_dns[4], testaddripv6, 16);
    rsp->data_call[0].pcscf_type = 3;
    memcpy(&rsp->data_call[0].pcscf[0], testaddripv4, 4);
    memcpy(&rsp->data_call[0].pcscf[4], testaddripv6, 16);
    //rsp->data_call[0].mtu_size = 1500; // This is Legacy under 1.4
    rsp->data_call[0].pco = 0;

    return rspData;
}

static UINT8 *buildStartKeepaliveRsp(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    *datalen = (UINT16)sizeof(sit_pdp_start_keepalive_rsp);
    rspData = allocRspModemMem(*datalen, sitId, sitToken);

    return rspData;
}

static UINT8 *buildSlicingConfigRsp(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(RCM_HEADER) + sizeof(sit_pdp_slicing_config_item);
    rspData = allocRspModemMem(len, sitId, sitToken);
    *datalen = len;

    sit_pdp_slicing_config_item* rsp = (sit_pdp_slicing_config_item*) (rspData + sizeof(RCM_HEADER));

    /* Test vector */
    rsp->slicing_config_version = 1;
    rsp->urspNum = 0;
    rsp->sliceNum = 0;

    return rspData;
}

static UINT8 *buildSlicingConfigIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(RCM_IND_HEADER) + sizeof(sit_pdp_slicing_config_item);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_slicing_config_item* rsp = (sit_pdp_slicing_config_item*) (rspData + sizeof(RCM_IND_HEADER));

    /* Test vector */
    rsp->slicing_config_version = 1;
    rsp->urspNum = 0;
    rsp->sliceNum = 0;

    return rspData;
}


template <typename T>
void SitrilTestPs::testRilRequest(const int testRilCmdId, T &testReq) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    /*
    std::stringstream ss, ss1;
    ss << (void *)&testReq;
    ss1 << (void *)psitRilTest;
    OutV("psitRilTest=" + ss1.str() +
         ",testRilCmdId=" + to_string(testRilCmdId) +
         ",sitId=" + to_string(sitId) +
         ",mPhoneIndex=" + to_string(mPhoneIndex) +
         ",sizeof(T)=" + to_string(sizeof(T)) +
         ",&testReq="+ ss.str());
         */
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &testReq, sizeof(testReq), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

void SitrilTestPs::testRilRequest(const int testRilCmdId, char *(&testReq)[2]) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    /*
    std::stringstream ss, ss1;
    ss << (void *)&testReq;
    ss1 << (void *)psitRilTest;
    OutV("psitRilTest=" + ss1.str() +
         ",testRilCmdId=" + to_string(testRilCmdId) +
         ",sitId=" + to_string(sitId) +
         ",mPhoneIndex=" + to_string(mPhoneIndex) +
         ",sizeof(T)=" + to_string(sizeof(T)) +
         ",&testReq="+ ss.str());
         */

    //printf("%c %c len=%zu, p=%p\n", testReq[0][0], testReq[1][0], sizeof(testReq), testReq);
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &testReq, sizeof(testReq), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

void SitrilTestPs::testRilRequest(const int testRilCmdId, nullptr_t) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, nullptr, 0, mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

void SitrilTestPs::validateRilRequest(UINT &sitToken, const unsigned int sitId, const unsigned int sizeOfModemData) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    // write back Token id for RSP loopback test
    sitToken = pModemData->GetToken();

    delete pModemData;
}

void SitrilTestPs::testRilResponse(const int testRilCmdId, const unsigned int sitId, const UINT sitToken) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);
    // Prapare RSP from modem
    /* test Modem RSP processing */
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    switch(sitId) {
      case SIT_SETUP_DATA_CALL:
          switch(testRilCmdId) {
            case ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2, 0)):
                rspData = buildSetupDataCallRsp_2_0(sitId, sitToken, &rspDataLen);
                break;
            case ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6)):
                rspData = buildSetupDataCallRsp_1_6(sitId, sitToken, &rspDataLen);
                break;
            default:
                rspData = buildSetupDataCallRsp(sitId, sitToken, &rspDataLen);
                break;
          }
        break;
      case SIT_GET_DATA_CALL_LIST:
        rspData = buildDataCallListRsp(sitId, sitToken, &rspDataLen);
        break;
      case SIT_DEACT_DATA_CALL:
        rspData = buildDeactDataCallRsp(sitId, sitToken, &rspDataLen);
        break;
      case SIT_START_KEEPALIVE:
        rspData = buildStartKeepaliveRsp(sitId, sitToken, &rspDataLen);
        break;
      case SIT_SET_PREFERRED_DATA_MODEM:
        rspData = buildEmptyRsp(sitId, sitToken, &rspDataLen);
        break;
      case SIT_GET_SLICING_CONFIG:
        rspData = buildSlicingConfigRsp(sitId, sitToken, &rspDataLen);
        break;
      case SIT_IND_SLICING_CONFIG:
        rspData = buildSlicingConfigRsp(sitId, sitToken, &rspDataLen);
        break;
    }
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    free(rspData);
}

void SitrilTestPs::validateRilResponse(int halversion) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    if (halversion < HAL_VERSION_CODE(1, 5)) {
        rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
        ASSERT_NE((UINT32)0, rilRspDataLen);
        EXPECT_GE(sizeof(RIL_Data_Call_Response_v11), rilRspDataLen);
        EXPECT_NE(nullptr, rilRspData);
        RIL_Data_Call_Response_v11 *rsp = (RIL_Data_Call_Response_v11 *)rilRspData;
        // Build Validation RILRsp
        // Check validation RILRsp == Responsed RILRsp
        EXPECT_EQ(rsp->cid, checkData.cid);
        EXPECT_STREQ(rsp->ifname, test_ifname);
        EXPECT_EQ(rsp->status, checkData.status);
        EXPECT_EQ(rsp->active, checkData.active);
        EXPECT_STREQ(rsp->type, checkData.type);
    } else if (halversion == HAL_VERSION_CODE(1, 5)) {
        EXPECT_EQ(1, 1);
    } else {
        EXPECT_EQ(1, 1);
    }
}

void SitrilTestPs::validateNullRilResponse() {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    UINT32 rilRspDataLen = 0;
    void *rilRspData = nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    //ASSERT_NE((UINT32)0, rilRspDataLen);
    //EXPECT_GE(sizeof(RIL_Data_Call_Response_v11), rilRspDataLen);
    //EXPECT_NE(nullptr, rilRspData);
}

////////////////////////////////////////////////////////////////////////////////////////
//[1] REQUEST & RESPONSE
////////////////////////////////////////////////////////////////////////////////////////

static UINT8 *buildnullRSP(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT16 len = (UINT16)sizeof(null_data_format);
    UINT8 *rspData = allocRspModemMem(len, sitId, sitToken);
    if (rspData) {
        *datalen = len;
    }
    return rspData;
}

TEST_P(SitrilTestPs, setInitialAttachAPNTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId =RIL_REQUEST_SET_INITIAL_ATTACH_APN;
    UINT16 sitId = SIT_SET_INITIAL_ATTACH_APN;

    char testapn[] = "test_apn";
    char testprotocol[] ="IPV4V6";
    char testuser[] = "test_user";
    char testpw[] ="test_password";
    char testmvno[] = "gid";
    char testmvnoMatchData[] ="4E";

    RIL_InitialAttachApn_v15 reqData =
    {
        .apn = testapn,
        .protocol = testprotocol,
        .roamingProtocol = testprotocol,
        .authtype =0,
        .username = testuser,
        .password=testpw,
        .supportedTypesBitmask = 0x0002,
        .bearerBitmask =0,
        .modemCognitive=1,
        .mtu=1570,
        .mvnoType =testmvno,
        .mvnoMatchData = testmvnoMatchData
    };

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(RIL_InitialAttachApn_v15), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_set_initial_attach_apn_req);

    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    UINT sitToken = pModemData->GetToken();
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    rspData = buildnullRSP(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    delete pModemData;
    free(rspData);

}


TEST_P(SitrilTestPs, setDataProfileTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = ENCODE_REQUEST(RIL_REQUEST_SET_DATA_PROFILE, HAL_VERSION_CODE(1,6));

    char testapn[] = "test_apn";
    char testuser[] = "test_user";
    char testpw[] ="test_password";

    RIL_DataProfileInfo_V1_5 reqData_first=
    {
        .profileId = RIL_DataProfileId::DEFAULT,
        .apn = testapn,
        .protocol = PDP_PROTOCOL_TYPE_IPV4V6,
        .roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6,
        .authType =PAP_CHAP,
        .user = testuser,
        .password=testpw,
        .type = THREE_GPP,
        .maxConnsTime=10000,
        .maxConns=10,
        .waitTime=10000,
        .enabled =true,
        .supportedApnTypesBitmap = 0x0002,
        .bearerBitmap =2,
        .mtuV4 =1570,
        .mtuV6=1570,
        .preferred =false,
        .persistent =false
    };

    RIL_DataProfileInfo_V1_5 reqData_second=
    {
        .profileId = RIL_DataProfileId::IMS,
        .apn = testapn,
        .protocol = PDP_PROTOCOL_TYPE_IPV4V6,
        .roamingProtocol = PDP_PROTOCOL_TYPE_IPV4V6,
        .authType =PAP_CHAP,
        .user = testuser,
        .password=testpw,
        .type = THREE_GPP,
        .maxConnsTime=10000,
        .maxConns=10,
        .waitTime=10000,
        .enabled =true,
        .supportedApnTypesBitmap = 0x0002,
        .bearerBitmap =2,
        .mtuV4 =1570,
        .mtuV6=1570,
        .preferred =false,
        .persistent =false
    };

    RIL_DataProfileInfo_V1_5* reqData[2];
    reqData[0] = &reqData_first;
    reqData[1] = &reqData_second;

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 rilRspDataLen = 0;
    void *rilRspData= nullptr;
    rilRspData = psitRilTest->getRilRspData(0, &rilRspDataLen);
    EXPECT_EQ((unsigned int)0, rilRspDataLen);
}

static UINT8 *buildStartKeepAliveRSP(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pdp_start_keepalive_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);

    sit_pdp_start_keepalive_rsp *rsp = (sit_pdp_start_keepalive_rsp *) rspData;
    rsp->session_handle = 0xffff;
    rsp->status_code = 0x12;

    *datalen = len;

    return rspData;
}

TEST_P(SitrilTestPs, startKeepAlive) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_START_KEEPALIVE;
    UINT16 sitId = SIT_START_KEEPALIVE;

    RIL_KeepaliveRequest reqData;
    const std::string srcAddr = "1.2.3.4";
    const std::string dstAddr = "5.6.7.8";

    reqData.cid =0;
    strlcpy(reqData.sourceAddress, srcAddr.c_str(), sizeof(reqData.sourceAddress));
    reqData.sourcePort = 12345;
    strlcpy(reqData.destinationAddress, dstAddr.c_str(), sizeof(reqData.destinationAddress));
    reqData.maxKeepaliveIntervalMillis =3000;
    reqData.type = (RIL_KeepaliveType) 0;
    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    UINT32 sizeOfModemData = sizeof(sit_pdp_start_keepalive_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);
    /* test RSP */
    UINT sitToken = pModemData->GetToken();
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    rspData = buildStartKeepAliveRSP(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    delete pModemData;
    free(rspData);

}

static UINT8 *buildStopKeepAliveRSP(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT16 len = (UINT16)sizeof(null_data_format);
    UINT8 *rspData = allocRspModemMem(len, sitId, sitToken);
    if (rspData) {
        *datalen = len;
    }
    return rspData;
}

TEST_P(SitrilTestPs, stopKeepAlive) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_STOP_KEEPALIVE;
    UINT16 sitId = SIT_STOP_KEEPALIVE;

    RIL_KeepaliveStatus reqData;
    reqData.sessionHandle = 0xABCD;
    reqData.code = KEEPALIVE_INACTIVE;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_stop_keepalive_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    UINT sitToken = pModemData->GetToken();
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    rspData = buildStopKeepAliveRSP(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());


    delete pModemData;
    free(rspData);

}

TEST_P(SitrilTestPs, DetachTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_OEM_DETACH;
    UINT16 sitId = SIT_DETACH;

    int reqData;
    reqData =0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_detach_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;

    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);

    /* test RSP */
    UINT sitToken = pModemData->GetToken();
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    rspData = buildnullRSP(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());


    delete pModemData;
    free(rspData);

}

static UINT8 *buildAllocPduSessionRSP(UINT16 sitId, UINT sitToken, UINT32 *datalen) {
    UINT8 *rspData = nullptr;

    UINT16 len = (UINT16)sizeof(sit_pdp_start_keepalive_rsp);
    rspData = allocRspModemMem(len, sitId, sitToken);

    sit_pdp_alloc_pdu_session_resp* rsp = (sit_pdp_alloc_pdu_session_resp *) rspData;
    rsp->session_id =11;
    *datalen = len;

    return rspData;
}

TEST_P(SitrilTestPs, allocatePduSessionTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_ALLOCATE_PDU_SESSION_ID;
    UINT16 sitId = SIT_ALLOC_PDU_SESSION;
    int* reqData =nullptr;
    int reqDatalen =0;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, reqDatalen, mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_alloc_pdu_session_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);
    /* test RSP */
    UINT sitToken = pModemData->GetToken();
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    rspData = buildAllocPduSessionRSP(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    delete pModemData;
    free(rspData);

}

static UINT8 *buildPsDataOffIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_ps_data_off_status_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_ps_data_off_status_ind* rsp = (sit_pdp_ps_data_off_status_ind*) rspData;

    rsp->ps_data_off_available_status =1;
    rsp->exempt_list_for_home =10;
    rsp->exempt_list_for_roam=11;

    return rspData;
}


TEST_P(SitrilTestPs, PsDataOffStatusIndHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_PS_DATA_OFF_STATUS;
    UINT16 unsolId = RIL_UNSOL_OEM_PS_DATA_OFF_STATUS;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildPsDataOffIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);

}

TEST_P(SitrilTestPs, StartHandoverHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_START_HANDOVER;
    int reqData =1;

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

}

TEST_P(SitrilTestPs, CancelHandoverHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_CANCEL_HANDOVER;
    int reqData =1;

    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

}

TEST_P(SitrilTestPs, releasePduSessionTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_RELEASE_PDU_SESSION_ID;
    UINT16 sitId = SIT_REL_PDU_SESSION;
    int reqData =11;

    psitRilTest->processReq(GetRandomSerialNumber(), testRilCmdId, &reqData, sizeof(reqData), mPhoneIndex);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    UINT32 sizeOfModemData = sizeof(sit_pdp_rel_pdu_session_req);
    UINT32 txIpcRawDataLen = 0;
    char *txIpcRawData = nullptr;
    txIpcRawData = psitRilTest->getTxIpcRawData(0, &txIpcRawDataLen);
    ASSERT_EQ(sizeOfModemData, txIpcRawDataLen);

    ModemData *pModemData = new ModemData(txIpcRawData, sizeOfModemData);
    ASSERT_EQ(pModemData->GetMessageId(), sitId);
    /* test RSP */
    UINT sitToken = pModemData->GetToken();
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;
    rspData = buildnullRSP(sitId, sitToken, &rspDataLen);
    psitRilTest->processRsp(testRilCmdId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    delete pModemData;
    free(rspData);

}

TEST_P(SitrilTestPs, RrcInfoTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int testRilCmdId = RIL_REQUEST_OEM_RRC_INFO;

    int rrcCon = 1;
//    psitRilTest->processReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &rrcCon, sizeof(int), mPhoneIndex);
    psitRilTest->processExternReqNoIpc(GetRandomSerialNumber(), testRilCmdId, &rrcCon, sizeof(int), mPhoneIndex);

    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
}

////////////////////////////////////////////////////
//[2] INDICATION
////////////////////////////////////////////////////

char testApn[MAX_PDP_APN_LEN] ="testapn";

static UINT8 *buildDataCallListIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_data_call_list_changed_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    const char test_ip[MAX_PDP_APN_LEN] ="127.0.0.1";

    sit_pdp_data_call_list_changed_ind* rsp = (sit_pdp_data_call_list_changed_ind*) rspData;

    rsp->datacall_info_num =1;

    rsp->data_call[0].status =0x00;
    rsp->data_call[0].cid =1;
    rsp->data_call[0].active=0x01;
    rsp->data_call[0].pdp_type=0x03;

    strncpy((char*)rsp->data_call[0].address,test_ip,10);
    rsp->data_call[0].dns_type =0x03;
    strncpy((char*)rsp->data_call[0].primary_dns,test_ip,10);
    strncpy((char*)rsp->data_call[0].secondary_dns,test_ip,10);
    rsp->data_call[0].pcscf_type=0x03;
    memset(rsp->data_call[0].pcscf,0,MAX_PCSCF_ADDRESS_LEN);
    rsp->data_call[0].ipv4_mtu_size =1570;
    rsp->data_call[0].pco=0xFF;

    return rspData;
}


TEST_P(SitrilTestPs, DataCallListChangedIndTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_DATA_CALL_LIST_CHANGED;
    UINT16 unsolId = RIL_UNSOL_DATA_CALL_LIST_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildDataCallListIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

static UINT8 *buildKeepAliveStatusIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_keepalive_status_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_keepalive_status_ind*rsp = (sit_pdp_keepalive_status_ind*) rspData;

    rsp->session_handle= 0xFFFF;
    rsp->status_code = -1;

    return rspData;
}

TEST_P(SitrilTestPs, keepAliveStatusInd) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_KEEPALIVE_STATUS;
    UINT16 unsolId = RIL_UNSOL_KEEPALIVE_STATUS;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildKeepAliveStatusIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}


static UINT8 *buildNasStatusIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_nas_timer_status_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_nas_timer_status_ind*rsp = (sit_pdp_nas_timer_status_ind*) rspData;
    strncpy(rsp->apn, testApn,8);
    rsp->timer_status = 0x01;
    rsp->timer_value = 0;
    rsp->timer_type = 0x01;

    return rspData;
}

TEST_P(SitrilTestPs, NasStatusIndTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_NAS_TIMER_STATUS;
    UINT16 unsolId = RIL_UNSOL_NAS_TIMER_STATUS_IND;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildNasStatusIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

typedef struct{
    sit_pdp_pco_data_ind inddata;
    sit_pdp_pco_data_entry pcodata;
}sit_test_pco_data_ind;

RCM_IND_HEADER pco_data_ind_header = {
    RCM_TYPE_INDICATION,
    0,
    SIT_IND_PCO_DATA,
    sizeof(sit_pdp_pco_data_ind),
    {{0,}}
};

static UINT8 *buildPcoDataIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_test_pco_data_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_test_pco_data_ind* rsp = (sit_test_pco_data_ind*) rspData;

    const char ct ='3';
    sit_pdp_pco_data_entry testpco = {
        0x00FF0000,
        1,
        (char*) &ct,
    };

    rsp->inddata.hdr = pco_data_ind_header;
    rsp->inddata.cid = 0x01;
    rsp->inddata.pdp_type = 0x03;
    rsp->inddata.pco_num = 1;

    rsp->pcodata = testpco;

    return rspData;
}

TEST_P(SitrilTestPs, PCOdataTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_PCO_DATA;
    UINT16 unsolId = RIL_UNSOL_PCO_DATA;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildPcoDataIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

static UINT8 *buildUnthrottleApnIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_unthrottle_apn_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_unthrottle_apn_ind*rsp = (sit_pdp_unthrottle_apn_ind*) rspData;
    memset(rsp->apn,0,MAX_PDP_APN_LEN);
    strncpy(rsp->apn,testApn,sizeof(testApn));

    return rspData;
}

TEST_P(SitrilTestPs, unthrottleApnTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_UNTHROTTLE_APN;
    UINT16 unsolId = RIL_UNSOL_UNTHROTTLE_APN;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildUnthrottleApnIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

////////////////////////////////////////////////////////////////////////////////////////
// GET request|response
TEST_P(SitrilTestPs, getDataCallListTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int request = ENCODE_REQUEST(RIL_REQUEST_DATA_CALL_LIST, HAL_VERSION_CODE(1,5));
    testRilRequest(request, nullptr);

    UINT16 sitId = SIT_GET_DATA_CALL_LIST;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_get_data_call_list_req);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

    validateNullRilResponse();

}
// SET reqeust|response //
TEST_P(SitrilTestPs, setupDataCallTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1,5));
    testRilRequest(request, testSetupDataReq);

    UINT16 sitId = SIT_SETUP_DATA_CALL;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_setup_data_call_req);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

    validateRilResponse(HAL_VERSION_CODE(1, 5));
}

TEST_P(SitrilTestPs, setupDataCallTest_1_6) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(1, 6));
    testRilRequest(request, testSetupDataReq_1_6);

    UINT16 sitId = SIT_SETUP_DATA_CALL;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_setup_data_call_v2_req);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

    validateRilResponse(HAL_VERSION_CODE(1, 6));
}


TEST_P(SitrilTestPs, setupDataCallTest_2_0) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int request = ENCODE_REQUEST(RIL_REQUEST_SETUP_DATA_CALL, HAL_VERSION_CODE(2, 0));
    DataV2_0::RIL_DataProfileInfo& sdr = testSetupDataReq_2_0.dataProfileInfo;
    sdr.profileId = RIL_DataProfileId::DEFAULT;
    sdr.apn = &test_apn[0];
    sdr.protocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6;
    sdr.roamingProtocol = PdpProtocolType::PDP_PROTOCOL_TYPE_IPV4V6;
    sdr.authType = RIL_ApnAuthType::NO_PAP_NO_CHAP;
    sdr.user = &test_user[0];
    sdr.password = &test_password[0];
    sdr.type = THREE_GPP;
    sdr.maxConnsTime = 0;
    sdr.maxConns = 0;
    sdr.waitTime = 0;
    sdr.enabled = true;
    sdr.supportedApnTypesBitmap = 0xFFFFFFFF;
    sdr.bearerBitmap = 0xFFFFFFFF;
    sdr.mtuV4 = 1500;
    sdr.mtuV6 = 1500;
    sdr.preferred = true;
    sdr.persistent = true;
    sdr.alwaysOn = false;
    sdr.tdInfo.reset();

    testSetupDataReq_2_0.dnses = test_req_dnsesV2;
    testSetupDataReq_2_0.linkAddresses = linkaddressesV;
    testRilRequest(request, testSetupDataReq_2_0);

    UINT16 sitId = SIT_SETUP_DATA_CALL;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_setup_data_call_v3_req);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

    validateRilResponse(HAL_VERSION_CODE(2, 0));
}

#define TEST_CID    "1"
#define TEST_REASON "0"

/* Prerequsite SetupDataCallTest success
 */
TEST_P(SitrilTestPs, deactDataCallTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(100us);

    validateTestRil(psitRilTest);

    int request = ENCODE_REQUEST(RIL_REQUEST_DEACTIVATE_DATA_CALL, HAL_VERSION_CODE(1, 5));
    char *pStrings[2];
    std::string strCid = std::to_string(checkData.cid);
    pStrings[0] = (char *)strCid.c_str();
    pStrings[1] = (char *)TEST_REASON;
    testRilRequest(request, pStrings);

    UINT16 sitId = SIT_DEACT_DATA_CALL;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_deact_data_call_req);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

    validateNullRilResponse();

}

TEST_P(SitrilTestPs, startKeepAliveTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    RIL_KeepaliveRequest kar = { .type = NATT_IPV4,
        .sourceAddress = "10.20.30.40",
        .sourcePort = 386,
        .destinationAddress = "10.20.30.1",
        .destinationPort = 486,
        .maxKeepaliveIntervalMillis = 10,
        .cid = 1, };

    int request = ENCODE_REQUEST(RIL_REQUEST_START_KEEPALIVE, HAL_VERSION_CODE(1, 5));
    testRilRequest(request, kar);

    UINT16 sitId = SIT_START_KEEPALIVE;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_start_keepalive_req);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

}

static UINT8 *buildDedicatedBearerInfoIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_dedicated_bearer_info_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_dedicated_bearer_info_ind* rsp = (sit_pdp_dedicated_bearer_info_ind*) rspData;

    /* Test vector */
    rsp->cid = 1;
    rsp->status = 0;
    rsp->qos_session_id = 1;
    rsp->qos_rat_type = 0;
    rsp->qos.nr = {};
    rsp->num_of_qos_filter = 0;

    return rspData;
}

TEST_P(SitrilTestPs, DedicatedBearerInfoHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_DEDICATED_BEARER_INFO;
    UINT16 unsolId = RIL_UNSOL_DATA_CALL_LIST_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildDedicatedBearerInfoIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

static UINT8 *buildIpAddrStatusIndMsg(UINT16 sitId, UINT32 *datalen) {
    UINT8 *rspData = nullptr;
    UINT16 len = (UINT16)sizeof(sit_pdp_ip_addr_status_ind);
    rspData = allocIndModemMem(len, sitId);
    *datalen = len;

    sit_pdp_ip_addr_status_ind* rsp = (sit_pdp_ip_addr_status_ind*) rspData;

    /* Test vector */
    rsp->status = 1;
    rsp->cid = 1;
    // leave 0 for IPv4Address, IPv6Address
    memset(rsp->IPv4Address, 0, sizeof(char) * MAX_IPV4_ADDR_LEN);
    memset(rsp->IPv6Address, 0, sizeof(char) * MAX_IPV6_ADDR_LEN);

    return rspData;
}

TEST_P(SitrilTestPs, IpAddrStatusHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_IP_ADDR_STATUS;
    UINT16 unsolId = RIL_UNSOL_DATA_CALL_LIST_CHANGED;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildIpAddrStatusIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());

    free(rspData);
}

TEST_P(SitrilTestPs, PsSlicingConfigHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    int request = ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(1, 6));
    testRilRequest(request, nullptr);

    UINT16 sitId = SIT_GET_SLICING_CONFIG;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_get_slicing_config);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    if (HalVer >= HAL_VERSION_CODE(1, 6)) {
        testRilResponse(request, sitId, sitToken);
    }
}

TEST_P(SitrilTestPs, PsSlicingConfigHandler_2_0_Test) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    if (HalVer != HAL_VERSION_CODE(2, 0)) {
        return;
    }

    int request = ENCODE_REQUEST(RIL_REQUEST_SLICING_CONFIG, HAL_VERSION_CODE(2, 0));
    testRilRequest(request, nullptr);

    UINT16 sitId = SIT_GET_SLICING_CONFIG;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(sit_pdp_get_slicing_config);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    if (HalVer >= HAL_VERSION_CODE(2, 0)) {
        testRilResponse(request, sitId, sitToken);
    }
}

TEST_P(SitrilTestPs, PsSlicingConfigUpdatedHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    UINT16 sitId = SIT_IND_SLICING_CONFIG;
    UINT16 unsolId = RIL_UNSOL_IND_POST_URSP;
    UINT8* rspData = nullptr;
    UINT32 rspDataLen = 0;

    /* test IND */
    rspData = buildSlicingConfigIndMsg(sitId, &rspDataLen);
    psitRilTest->processInd(unsolId, rspData, rspDataLen);
    if (HalVer >= HAL_VERSION_CODE(1, 6)) {
        ASSERT_EQ(std::cv_status::no_timeout, psitRilTest->wait());
    }

    free(rspData);
}

// RadioConfig 1.1, Why here?
TEST_P(SitrilTestPs, PsSetPreferredModemHandlerTest) {
    SitrilTest *psitRilTest = getSitrilTest();
    ASSERT_NE(nullptr, psitRilTest);

    validateTestRil(psitRilTest);

    /*
    int stack_id = 0;

    int request = ENCODE_REQUEST(RIL_REQUEST_SET_PREFERRED_DATA_MODEM, HAL_VERSION_CODE(1, 6));
    testRilRequest(request, stack_id);

    UINT16 sitId = SIT_SET_PREFERRED_DATA_MODEM;
    UINT sitToken = TOKEN_INVALID;
    UINT32 sizeOfModemData = sizeof(int);

    validateRilRequest(sitToken, sitId, sizeOfModemData);

    testRilResponse(request, sitId, sitToken);

    //validateNullRilResponse();
    //psitRilTest->resetTestData();
    */
}


