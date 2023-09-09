#include <gtest/gtest.h>

/*
#include "modemdatabuilder.h"
#include "protocoladapter.h"
#include "legacy/ps/protocolpsbuilderlegacy.h"

#include "mcctable.h"
*/

//#include "service.h"
//#include "core/service/ps/psservice.h"

#include "datacallreqdata.h"
#include "pdpcontext.h"
#include "protocolpsbuilder.h"
#include "../../../base/modemconfiguration.h"
#include "mcctable.h"

#include "protocoljsontestadapter.h"
#include "json/ps/protocolpsbuilderjson.h"
#include "psjsondef.h"
#include "modemdata.h"
#include "../../sitril/protocol/sit/ps/protocolpsutil.h"

#include <stdio.h>

using namespace std;

#if 0
TEST(ProtocolPsBuilderJson, BuildSetupDataCall) {
    int size = 1;

    int rat = 14;
    int dataProfile = 0;

    int slotId = 0;
    int pduSessionId = 0;
    int sliceServiceType = 0;
    int sliceDifferentiator = -1;
    int mappedHplmnSst = 0;
    int mappedHplmnSd = -1;
    bool matchAllRule = 1;

    //const char * testString[1] ={"5g.sktelecom.com"};

    char test_crrier[] = "00101";
    char test_apn[] = "test";
    char test_user[] = "";
    char test_password[] = "";
    char protocol[] = "IPV4V6";
    char roamingProtocol[] = "IPV4V6";

    int supportedApnTypesBitmap = 0xB7;

    int authType = SETUP_DATA_AUTH_PAP_CHAP;

    const DataV2_0::RIL_TrafficDescriptor tdInfo = {RIL_OPT_INFO_VALID, {'t','e','s','t', 0}, RIL_OPT_INFO_INVALID, {0, }};

    PdpContext * m_PdpContext = new PdpContext(9, NULL, -1);
    ApnSetting * pNewApnSetting = ApnSetting::NewInstance(test_crrier, test_apn, supportedApnTypesBitmap, test_user, test_password, protocol, roamingProtocol, authType);


    m_PdpContext->SetApnSetting(pNewApnSetting);
    m_PdpContext->SetDataProfileId(dataProfile);

    for (int i = 0; i < size; i++) {
        ProtocolPsBuilderJson testBuilder;

        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetupDataCall( rat, m_PdpContext, slotId, 1, NULL,
                                                  pduSessionId, 1, sliceServiceType, sliceDifferentiator, mappedHplmnSst,
                                                  mappedHplmnSd, (&tdInfo), matchAllRule));

        ASSERT_NE(modemData, nullptr);

        if (m_PdpContext != NULL) {
            delete m_PdpContext;
            m_PdpContext = NULL;
        }

        ProtocolJsonReqAdapter testAdapter(modemData.get());

        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SETUP_DATA_CALL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        ASSERT_NE(false, parsed);

        EXPECT_EQ(9, jsonReqData[ns_ps_setup_data_call_req::key_cid].asInt());
        EXPECT_EQ(rat, jsonReqData[ns_ps_setup_data_call_req::key_rat].asInt());

        char ch[100];
        Json::Value jsonApnInfo = jsonReqData[ns_ps_setup_data_call_req::key_apn_info];
        strlcpy(ch,jsonApnInfo[ns_ps_apn_info::key_apn].asString().c_str(), strlen(test_apn) + 1);
        EXPECT_STREQ(test_apn, ch);

        strlcpy(ch,jsonApnInfo[ns_ps_apn_info::key_username].asString().c_str(), strlen(test_user) + 1);
        EXPECT_STREQ(test_user, ch);

        strlcpy(ch,jsonApnInfo[ns_ps_apn_info::key_password].asString().c_str(), strlen(test_password) + 1);
        EXPECT_STREQ(test_password, ch);

        EXPECT_EQ(SIT_PDP_PDP_TYPE_IPV4IPV6, jsonApnInfo[ns_ps_apn_info::key_pdp_type].asInt());
        EXPECT_EQ(0, jsonApnInfo[ns_ps_apn_info::key_apn_type].asInt());
        EXPECT_EQ(authType, jsonApnInfo[ns_ps_apn_info::key_auth_type].asInt());

        EXPECT_EQ(pduSessionId, jsonReqData[ns_ps_setup_data_call_req::key_pdu_session_id].asInt());

        EXPECT_EQ(sliceServiceType, jsonReqData[ns_ps_setup_data_call_req::key_slice_service_type].asInt());
        EXPECT_EQ(sliceDifferentiator, jsonReqData[ns_ps_setup_data_call_req::key_slice_differentiator].asInt());
        EXPECT_EQ(mappedHplmnSst, jsonReqData[ns_ps_setup_data_call_req::key_mapped_hplmn_sst].asInt());
        EXPECT_EQ(mappedHplmnSd, jsonReqData[ns_ps_setup_data_call_req::key_mapped_hplmn_sd].asInt());
        EXPECT_EQ(matchAllRule, jsonReqData[ns_ps_setup_data_call_req::key_match_all_rule_allowed].asInt());

        Json::Value jsonTd = jsonReqData[ns_ps_setup_data_call_req::key_traffic_descriptor];
        strlcpy(ch,jsonTd[ns_ps_data_call_traffic_descriptor::key_dnn].asString().c_str(), strlen(test_apn) + 1);
        EXPECT_STREQ(test_apn, ch);

    }
}
#endif

TEST(ProtocolPsBuilderJson, BuildDeactDataCall) {
    struct {
        int cid;
        int reason;
    } testVector[] = {
        { 1, DEACT_REASON_NORMAL},
        { 2, DEACT_REASON_PDP_RESET},
    };
    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolPsBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildDeactDataCall(testVector[i].cid, testVector[i].reason));
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_DEACT_DATA_CALL);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

        ASSERT_NE(false, parsed);

        EXPECT_EQ(testVector[i].cid, jsonReqData[ns_ps_deact_data_call_req::key_cid].asInt());
        EXPECT_EQ(testVector[i].reason, jsonReqData[ns_ps_deact_data_call_req::key_reason].asInt());

    }
}

TEST(ProtocolPsBuilderJson, BuildGetDataCallList) {
    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildGetDataCallList());

    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_GET_DATA_CALL_LIST);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);
}

/** Test initialAttach APN **/
TEST(ProtocolPsBuilderJson, BuildSetInitialAttach) {

    char test_crrier[] = "00101";
    char test_apn[] = "test";
    char test_user[] = "";
    char test_password[] = "";
    char protocol[] = "IPV4V6";
    char roamingProtocol[] = "IPV4V6";

    int dataProfileId = DATA_PROFILE_INVALID;
    int supportedApnTypesBitmap = 0xB7;

    int authType = SETUP_DATA_AUTH_PAP_CHAP;

    PdpContext * m_PdpContext = new PdpContext(9, NULL, -1);
    ApnSetting * pNewApnSetting = ApnSetting::NewInstance(test_crrier, test_apn, supportedApnTypesBitmap, test_user, test_password, protocol, roamingProtocol, authType);

    m_PdpContext->SetApnSetting(pNewApnSetting);
    m_PdpContext->SetDataProfileId(dataProfileId);

    bool isEsmFlagZero = MccTable::IsEsmFlagZeroOperator(test_crrier);

    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetInitialAttachApn(m_PdpContext, isEsmFlagZero));

    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_INITIAL_ATTACH_APN);
    EXPECT_NE(testAdapter.GetParameterLength(), 0U);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);

    if (m_PdpContext != NULL) {
        delete m_PdpContext;
        m_PdpContext = NULL;
    }
    char ch[100];
    Json::Value jsonApnInfo = jsonReqData[ns_ps_set_initial_attach_apn_req::key_apn_info];
    strlcpy(ch,jsonApnInfo[ns_ps_apn_info::key_apn].asString().c_str(), strlen(test_apn) + 1);
    EXPECT_STREQ(test_apn, ch);

    strlcpy(ch,jsonApnInfo[ns_ps_apn_info::key_username].asString().c_str(), strlen(test_user) + 1);
    EXPECT_STREQ(test_user, ch);

    strlcpy(ch,jsonApnInfo[ns_ps_apn_info::key_password].asString().c_str(), strlen(test_password) + 1);
    EXPECT_STREQ(test_password, ch);

    EXPECT_EQ(SIT_PDP_PDP_TYPE_IPV4IPV6, jsonApnInfo[ns_ps_apn_info::key_pdp_type].asInt());
    EXPECT_EQ(SIT_PDP_PDP_TYPE_IPV4IPV6, jsonReqData[ns_ps_set_initial_attach_apn_req::key_roaming_pdp_type].asInt());
    EXPECT_EQ(SIT_IA_PROFILE_ID_UNKNOWN, jsonReqData[ns_ps_set_initial_attach_apn_req::key_profile_id].asInt());
    EXPECT_EQ(0, jsonApnInfo[ns_ps_apn_info::key_apn_type].asInt());
    EXPECT_EQ(authType, jsonApnInfo[ns_ps_apn_info::key_auth_type].asInt());
}

TEST(ProtocolPsBuilderJson, BuildSetFastDormancyInfo) {
    BYTE lcdOn = 1;
    BYTE lcdOff = 2;
    BYTE rel8LcdOnl = 3;
    BYTE rel8LcdOff = 4;

    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetFastDormancyInfo(lcdOn, lcdOff, rel8LcdOnl, rel8LcdOff));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_FD_INFO);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);

    EXPECT_EQ(lcdOn, jsonReqData[ns_ps_set_fd_info_req::key_lcd_on_timer].asInt());
    EXPECT_EQ(lcdOff, jsonReqData[ns_ps_set_fd_info_req::key_lcd_off_timer].asInt());
    EXPECT_EQ(rel8LcdOnl, jsonReqData[ns_ps_set_fd_info_req::key_rel8_lcd_on_timer].asInt());
    EXPECT_EQ(rel8LcdOff, jsonReqData[ns_ps_set_fd_info_req::key_rel8_lcd_off_timer].asInt());
}

TEST(ProtocolPsBuilderJson, BuildDetach) {
    BYTE reason = 0;

    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildDetach(reason));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_DETACH);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
    ASSERT_NE(false, parsed);

    EXPECT_EQ(reason, jsonReqData[ns_ps_detach_req::key_detach_reason].asInt());
}

TEST(ProtocolPsBuilderJson, BuildStartKeepalive) {
    RIL_KeepaliveRequest testVector = {
        .type = NATT_IPV4,
        .sourceAddress = {0x01, 0x02, 0x03, 0x04, },
        .sourcePort = 9,
        .destinationAddress = {0x05, 0x06, 0x07, 0x08, },
        .destinationPort = 9,
        .maxKeepaliveIntervalMillis = 9,
        .cid = 1,
    };

    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildStartKeepAlive(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_START_KEEPALIVE);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);

    EXPECT_EQ(testVector.type, jsonReqData[ns_ps_start_keepalive_req::key_keepalive_type].asInt());
    for (int i = 0; i < MAX_IPV6_ADDR_LEN; i++) {
        EXPECT_EQ(testVector.sourceAddress[i], jsonReqData[ns_ps_start_keepalive_req::key_source_addr][i].asInt());
        EXPECT_EQ(testVector.destinationAddress[i], jsonReqData[ns_ps_start_keepalive_req::key_dest_addr][i].asInt());
    }

    EXPECT_EQ(testVector.sourcePort, jsonReqData[ns_ps_start_keepalive_req::key_source_port].asInt());
    EXPECT_EQ(testVector.destinationPort, jsonReqData[ns_ps_start_keepalive_req::key_dest_port].asInt());
    EXPECT_EQ(testVector.maxKeepaliveIntervalMillis, jsonReqData[ns_ps_start_keepalive_req::key_max_interval].asInt());
    EXPECT_EQ(testVector.cid, jsonReqData[ns_ps_start_keepalive_req::key_cid].asInt());
}

TEST(ProtocolPsBuilderJson, BuildStopKeepalive) {
    int sessionHande = 10;

    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildStopKeepAlive(sessionHande));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_STOP_KEEPALIVE);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);

    EXPECT_EQ(sessionHande, jsonReqData[ns_ps_stop_keepalive_req::key_keepalive_handle].asInt());
}


TEST(ProtocolPsBuilderJson, BuildAllocatePduSessionId) {
    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildAllocatePduSessionId());
    ASSERT_NE(modemData, nullptr);

    ProtocolReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_ALLOC_PDU_SESSION);
    EXPECT_EQ(testAdapter.GetParameterLength(), 0U);
    ASSERT_EQ(testAdapter.GetParameter(), nullptr);
}

TEST(ProtocolPsBuilderJson, BuildSetDataProfile_v15) {
    const char * apn = "testapn";
    const char * pdptype = "IPV4V6";
    const char * roamingPdptype = "IPV4";
    const char * testUser = "testUser";
    const char * password = "testPassword";
    const char * mvnoType = "imsi";
    const char * mvnoMatchData = "302720x94";
    const RIL_DataProfileInfo_v15 testVector[] = {
        {0, (char *)apn, (char *)pdptype, (char *)roamingPdptype, 3,
            (char *)testUser, (char *)password, 0, 123, 321, 222, 0,
            0x0002, 0x04, 222, (char *)mvnoType, (char *)mvnoMatchData},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolPsBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetDataProfile(&testVector[i], 0));

        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DATA_PROFILE);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        ASSERT_NE(false, parsed);

        EXPECT_EQ(::GetPdpType(testVector[i].protocol), jsonReqData[ns_ps_set_data_profile_req::key_pdp_type].asInt());
        EXPECT_EQ(::GetPdpType(testVector[i].roamingProtocol), jsonReqData[ns_ps_set_data_profile_req::key_roaming_pdp_type].asInt());
        EXPECT_EQ(::ConvertAuthTypeToProtocolAuthType(testVector[i].authType), jsonReqData[ns_ps_set_data_profile_req::key_auth_type].asInt());
        EXPECT_EQ(testVector[i].bearerBitmask, jsonReqData[ns_ps_set_data_profile_req::key_radio_access_family].asInt());
        EXPECT_EQ(testVector[i].supportedTypesBitmask, jsonReqData[ns_ps_set_data_profile_req::key_apn_type].asInt());

        EXPECT_EQ(testVector[i].profileId, jsonReqData[ns_ps_set_data_profile_req::key_profile_id].asInt());

        char ch[100];
        strlcpy(ch,jsonReqData[ns_ps_set_data_profile_req::key_apn].asString().c_str(), strlen(apn) + 1);
        EXPECT_STREQ(testVector[i].apn, ch);

        strlcpy(ch,jsonReqData[ns_ps_set_data_profile_req::key_username].asString().c_str(), strlen(testUser) + 1);
        EXPECT_STREQ(testVector[i].user, ch);

        strlcpy(ch,jsonReqData[ns_ps_set_data_profile_req::key_password].asString().c_str(), strlen(password) + 1);
        EXPECT_STREQ(testVector[i].password, ch);

        EXPECT_EQ(testVector[i].enabled, jsonReqData[ns_ps_set_data_profile_req::key_enabled].asInt());
        EXPECT_EQ(testVector[i].maxConnsTime, jsonReqData[ns_ps_set_data_profile_req::key_max_conn_time].asInt());
        EXPECT_EQ(testVector[i].maxConns, jsonReqData[ns_ps_set_data_profile_req::key_max_conn].asInt());
        EXPECT_EQ(testVector[i].waitTime, jsonReqData[ns_ps_set_data_profile_req::key_wait_time].asInt());
    }
}

#if 0
TEST(ProtocolPsBuilderJson, BuildSetDataProfile_v1_5) {
    const char * apn = "testapn";
    const char * testUser = "testUser";
    const char * password = "testPassword";

    const RIL_DataProfileInfo_V1_5 testVector[] = {
        {DEFAULT, (char *)apn, PDP_PROTOCOL_TYPE_IPV4V6, PDP_PROTOCOL_TYPE_IPV4V6, NO_PAP_CHAP,
            (char *)testUser, (char *)password, COMMON, 123, 321, 222, 0,
            0x0002, 0x04, 222, 333, 0, 1},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);

    ProtocolPsBuilderJson testBuilder;
    for (int i = 0; i < size; i++) {
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetDataProfile(&testVector[i], 0));
        ASSERT_NE(modemData, nullptr);

        ProtocolJsonReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), SIT_SET_DATA_PROFILE);
        EXPECT_NE(testAdapter.GetParameterLength(), 0U);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        Json::Value jsonReqData;
        bool parsed =  testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());
        ASSERT_NE(false, parsed);

        EXPECT_EQ(::ConvertPdpProtocolTypeToSitPdpType(testVector[i].protocol), jsonReqData[ns_ps_set_data_profile_req::key_pdp_type].asInt());
        EXPECT_EQ(::ConvertPdpProtocolTypeToSitPdpType(testVector[i].roamingProtocol), jsonReqData[ns_ps_set_data_profile_req::key_roaming_pdp_type].asInt());
        EXPECT_EQ(::ConvertAuthTypeToProtocolAuthType(testVector[i].authType), jsonReqData[ns_ps_set_data_profile_req::key_auth_type].asInt());
        EXPECT_EQ(testVector[i].bearerBitmap, (uint32_t)jsonReqData[ns_ps_set_data_profile_req::key_radio_access_family].asInt());
        EXPECT_EQ(testVector[i].supportedApnTypesBitmap, (uint32_t)jsonReqData[ns_ps_set_data_profile_req::key_apn_type].asInt());

        char ch[100];
        strlcpy(ch,jsonReqData[ns_ps_set_data_profile_req::key_apn].asString().c_str(), strlen(apn) + 1);
        EXPECT_STREQ(testVector[i].apn, ch);

        strlcpy(ch,jsonReqData[ns_ps_set_data_profile_req::key_username].asString().c_str(), strlen(testUser) + 1);
        EXPECT_STREQ(testVector[i].user, ch);

        strlcpy(ch,jsonReqData[ns_ps_set_data_profile_req::key_password].asString().c_str(), strlen(password) + 1);
        EXPECT_STREQ(testVector[i].password, ch);


        EXPECT_EQ(testVector[i].maxConnsTime, jsonReqData[ns_ps_set_data_profile_req::key_max_conn_time].asInt());
        EXPECT_EQ(testVector[i].maxConns, jsonReqData[ns_ps_set_data_profile_req::key_max_conn].asInt());
        EXPECT_EQ(testVector[i].waitTime, jsonReqData[ns_ps_set_data_profile_req::key_wait_time].asInt());

        if(jsonReqData[ns_ps_set_data_profile_req::key_pdp_type].asInt() & RIL_APN_TYPE_DEFAULT){
            EXPECT_EQ((RIL_DataProfileId)INTERNET_FOR_CP, (RIL_DataProfileId)jsonReqData[ns_ps_set_data_profile_req::key_profile_id].asInt());
            EXPECT_EQ(SIT_PDP_DATA_PROFILE_ENABLED, jsonReqData[ns_ps_set_data_profile_req::key_enabled].asInt());
        }else{
            EXPECT_EQ(testVector[i].profileId, (RIL_DataProfileId)jsonReqData[ns_ps_set_data_profile_req::key_profile_id].asInt());
            EXPECT_EQ(testVector[i].enabled, jsonReqData[ns_ps_set_data_profile_req::key_enabled].asInt());
        }
    }
}

#endif

TEST(ProtocolPsBuilderJson, BuildReleasePduSessionId) {

    int sessionId = 9887;

    ProtocolPsBuilderJson testBuilder;

    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildReleasePduSessionId(sessionId));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_REL_PDU_SESSION);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);

    EXPECT_EQ(sessionId, jsonReqData[ns_ps_rel_pdu_session_req::key_session_id].asInt());
}

/******************************************************************************
 * NET
 *****************************************************************************/

TEST(ProtocolPsBuilderJson, BuildSetPreferredDataModem) {
    BYTE stackId = 0;

    ProtocolPsBuilderJson testBuilder;
    auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildSetPreferredDataModem(stackId));
    ASSERT_NE(modemData, nullptr);

    ProtocolJsonReqAdapter testAdapter(modemData.get());
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
    EXPECT_EQ(testAdapter.GetId(), SIT_SET_PREFERRED_DATA_MODEM);
    EXPECT_NE(testAdapter.GetParameter(), nullptr);

    Json::Value jsonReqData;
    bool parsed = testAdapter.CheckParsing(jsonReqData, testAdapter.GetId());

    ASSERT_NE(false, parsed);
    EXPECT_EQ(stackId, jsonReqData[ns_ps_net_set_preferred_data_modem_req::key_stack_id].asInt());
}

