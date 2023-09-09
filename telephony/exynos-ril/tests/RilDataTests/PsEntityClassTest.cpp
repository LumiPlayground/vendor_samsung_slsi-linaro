/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

// GoogleTest Dependency
#include <gtest/gtest.h>

/// Ril Service Layer
// #include <telephony/ril.h>
#include "../../sitril/base/rilapplication.h"

// PsService Layer
#include <pdpcontext.h>
#include "PsUtil.h"
#include "../../sitril/fw/psdatabuilder.h"
#include <netifcontroller.h>

// PsServceHandler Layer

// Modem SIT Layer
#include "../../sitril/protocol/sit/sitdef.h"

// Local Test Util
#include <pstestutil.h>
#include <string>


const int PDPCONTEXT_NUM = 4;

/* Test Vectors */
const char testCarrier[] = "00101";
const char testCarrier2[] = "310260";
const char testApn[] = "testapn";
const char testUser[] = "Nouser";
const char testPassword[] = "NoPassword";

void createPdpContext(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM]) {
    for (int i = 0; i < PDPCONTEXT_NUM; i++) {
        pPdpContext[i] = new PdpContext(i+1, "rmnet", i);
        if (pPdpContext[i] != nullptr) {
            pPdpContext[i]->Init();
            // OutV("PdpContext[" + to_string(i) + "]" + SCL + " is initialized");
        }
    }
}

void cleanupPdpContext(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM]) {
    for (int i = 0; i < PDPCONTEXT_NUM; i++) {
        if (pPdpContext[i] != nullptr) {
            delete pPdpContext[i];
        }
    }
}

void CheckRilData(const RilData *rilData) {
    EXPECT_EQ(!rilData, NULL);
    if(rilData) delete rilData;
}

// RadioHal -> RIL Layer
TEST(PdpContextTest, SimpleConstructRilPdpContextTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    for (int i = 0; i < PDPCONTEXT_NUM; i++) {
        pPdpContext[i] = new PdpContext(i+1);
    }

    for (int i = 0; i < PDPCONTEXT_NUM; i++) {
        if (pPdpContext[i] != nullptr) {
            delete pPdpContext[i];
        }
    }
}

TEST(PdpContextTest, ConstructRilPdpContextTest) {
    /* TBD: Unsupported usage, maybe need to change actual code, but not now
       This will cause SEGV
       03-09 18:54:56.359 20914 20914 I PsServiceTest: ==20914==ERROR: AddressSanitizer: SEGV on unknown address 0xbebebebebebebebe (pc 0x007109b94f18 bp 0x007ff4c12290 sp 0x007ff4c121b0 T0)
       03-09 18:54:56.360 20914 20914 I PsServiceTest: ==20914==The signal is caused by a READ memory access.
       03-09 18:54:56.584 20962 20962 F DEBUG   :       #06 pc 000000000000063c  [vdso] (__kernel_rt_sigreturn)
       03-09 18:54:56.584 20962 20962 F DEBUG   :       #07 pc 00000000002d3f14  /vendor/lib64/libsitril.so (PdpContext::~PdpContext()+252) (BuildId: c7e6efa30fa329b80640aa11dd75d8cf)
       03-09 18:54:56.584 20962 20962 F DEBUG   :       #08 pc 00000000000cc140  /data/local/tmp/PsServiceTest (PdpContextTest_ConstructRilPdpContextTest_Test::TestBody()+904) (BuildId: 0a5a2080c0121998e9b1d89b08510784)

    //PdpContext *pEmptyPdpContext = new PdpContext(0);
    //delete pEmptyPdpContext;
    //PdpContext *pEmptyPdpContext = new PdpContext(1);
    //delete pEmptyPdpContext;

    */
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    // Check Initial State
    for (int i = 0; i < PDPCONTEXT_NUM; i++) {
        EXPECT_EQ(pPdpContext[i]->GetCID(), i+1);
        EXPECT_EQ(pPdpContext[i]->GetState(), PDP_CONTEXT_DISCONNECTED);
        EXPECT_EQ(pPdpContext[i]->GetActive(), INACTIVE);
        EXPECT_EQ(pPdpContext[i]->IsAvailable(), true);
        EXPECT_EQ(pPdpContext[i]->GetInterfaceName(), std::string("rmnet") + std::string(to_string(i)));
    }

    cleanupPdpContext(pPdpContext);
}

DataCall dc = { .status = PDP_FAIL_NONE,
                .suggestedRetryTime = -1,
                .cid = 1,
                .active = ACTIVE,
                .pdpType = PDP_TYPE_IPV4V6,
                .ipv4 = {
                    .valid = true,
                    .addr = {0x10, 0x20, 0x30, 0x40},
                    .dns1 = {0x10, 0x20, 0x30, 0x50},
                    .dns2 = {0x10, 0x20, 0x30, 0x60},
                    .gw = {0x10, 0x20, 0x30, 0x1},
                    .pcscf = {0, }
                },
                .ipv6 = {
                    .valid = true,
                    .addr = {0x10, 0x20, 0x30, 0x40, 0xA0, 0xB0},
                    .dns1 = {0x10, 0x20, 0x30, 0x50, 0xA0, 0xB0, 0xB1},
                    .dns2 = {0x10, 0x20, 0x30, 0x60, 0xA0, 0xB0, 0xB2},
                    .gw = {0x10, 0x20, 0x30, 0x40, 0xA0, 0xB0, 0x0, 0x0, 0x0, 0xC, 0x1},
                    .pcscf = {0, }
                },
                //.mtu_size = 1500,
                .pco = 0,
                .IPC_version = 4,
                .pcscf_ext_count = 5,
                .ambr = {
                    .octet2 = 0x04,
                    .octet3 = 0xef,
                    .octet4 = 0xEf,
                    .octet5 = 0xba,
                    .octet6 = 0xba,
                    .octet7 = 0xcd,
                    .octet8 = 0xcd,
                },
};

void CheckupPdpContextState(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM], int cid) {
    int idx = cid - 1;
    PdpContext *p = pPdpContext[idx];
    EXPECT_EQ(p->GetCID(), cid);
    EXPECT_EQ(p->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(p->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(p->GetInterfaceName(), std::string("rmnet") + std::string(to_string(cid - 1)));
    EXPECT_EQ(isSameIp((char *)p->GetDataCallInfo()->ipv4.addr, (char *)dc.ipv4.addr, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(isSameIp((char *)p->GetDataCallInfo()->ipv6.addr, (char *)dc.ipv6.addr, MAX_IPV6_ADDR_LEN), true);
}

void CheckupPdpContextNotAvailableState(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM], int cid) {
    CheckupPdpContextState(pPdpContext, cid);
    int idx = cid - 1;
    PdpContext *p = pPdpContext[idx];
    EXPECT_EQ(p->IsAvailable(), false);  // Already used
}

void CheckupPdpContextAvailableState(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM], int cid) {
    CheckupPdpContextState(pPdpContext, cid);
    int idx = cid - 1;
    PdpContext *p = pPdpContext[idx];
    EXPECT_EQ(p->IsAvailable(), true);
}

void CheckupPdpContextActiveState(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM], int cid) {
    CheckupPdpContextState(pPdpContext, cid);
    int idx = cid - 1;
    PdpContext *p = pPdpContext[idx];
    EXPECT_EQ(p->GetActive(), ACTIVE_AND_LINKUP);
}

void CheckupPdpContextActiveLinkDownState(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM], int cid) {
    CheckupPdpContextState(pPdpContext, cid);
    int idx = cid - 1;
    PdpContext *p = pPdpContext[idx];
    EXPECT_EQ(p->GetActive(), ACTIVE_AND_LINKDOWN);
}

void CheckupPdpContextInitState(PdpContext *(&pPdpContext)[PDPCONTEXT_NUM], int cid) {
    CheckupPdpContextState(pPdpContext, cid);
    int idx = cid - 1;
    PdpContext *p = pPdpContext[idx];
    EXPECT_EQ(p->GetActive(), ACTIVE_AND_LINKDOWN);
    EXPECT_EQ(p->IsAvailable(), true);
}

TEST(PdpContextTest, OnActivatedDeactivatedTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    EXPECT_EQ(pPdpContext[0]->OnActivated(&dc), 0);
    EXPECT_EQ(pPdpContext[0]->OnActivated(), 0);

    CheckupPdpContextActiveState(pPdpContext, 1);

    EXPECT_EQ(pPdpContext[0]->OnDeactivated(), 0);
    EXPECT_EQ(pPdpContext[0]->GetCID(), 1);
    EXPECT_EQ(pPdpContext[0]->GetActive(), 0);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, false);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, false);
    EXPECT_EQ(pPdpContext[0]->IsAvailable(), true);  // Already used
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->suggestedRetryTime, RETRY_NO_SUGGESTED);

    cleanupPdpContext(pPdpContext);
}

TEST(PdpContextTest, updateDataCallInfoTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    EXPECT_EQ(pPdpContext[0]->UpdateDataCallInfo(&dc), true);
    CheckupPdpContextActiveLinkDownState(pPdpContext, 1);
    EXPECT_EQ(pPdpContext[0]->IsAvailable(), true);  // Already used

    pPdpContext[0]->SetState(PDP_CONTEXT_CONNECTED);
    EXPECT_EQ(pPdpContext[0]->GetState(), PDP_CONTEXT_CONNECTED);

    pPdpContext[0]->SetActive(ACTIVE);
    EXPECT_EQ(pPdpContext[0]->GetActive(), ACTIVE);

    //pPdpContext[0]->SetHandoverDnses(string("10.20.30.40"));
    //EXPECT_EQ(strcmp(pPdpContext[0]->GetHandoverDnses().c_str(), "10.20.30.40"), 0);

    cleanupPdpContext(pPdpContext);
}

TEST(PdpContextTest, updateDataCallInfoForLinkLocalTest) {
    // Ipv6 LinkLocal to Global Address update
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);
    DataCall dc_local = dc;
    BYTE newipv6[16] = {0xFE, 0x80, 0x30, 0x40, 0xA0, 0xB0, 0x00, 0x00};
    memcpy(&dc_local.ipv6.addr, newipv6, 16);

    EXPECT_EQ(pPdpContext[0]->UpdateDataCallInfo(&dc_local), true);

    EXPECT_EQ(pPdpContext[0]->GetCID(), 1);
    EXPECT_EQ(pPdpContext[0]->GetActive(), ACTIVE_AND_LINKDOWN);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(pPdpContext[0]->IsAvailable(), true);
    EXPECT_EQ(pPdpContext[0]->GetInterfaceName(), std::string("rmnet") + std::string(to_string(0)));
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv4.addr, (char *)dc_local.ipv4.addr, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, (char *)dc_local.ipv6.addr, MAX_IPV6_ADDR_LEN), true);

    pPdpContext[0]->SetState(PDP_CONTEXT_CONNECTED);
    EXPECT_EQ(pPdpContext[0]->GetState(), PDP_CONTEXT_CONNECTED);

    pPdpContext[0]->SetActive(ACTIVE);
    EXPECT_EQ(pPdpContext[0]->GetActive(), ACTIVE);

    EXPECT_EQ(pPdpContext[0]->UpdateDataCallInfo(&dc), true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, (char *)dc.ipv6.addr, MAX_IPV6_ADDR_LEN), true);

    cleanupPdpContext(pPdpContext);
}

TEST(PdpContextTest, OnUpdatedTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    EXPECT_EQ(pPdpContext[0]->UpdateDataCallInfo(&dc), true);
    pPdpContext[0]->SetState(PDP_CONTEXT_CONNECTED);
    pPdpContext[0]->SetActive(ACTIVE);

    // Now change Stae
    char ipv4[MAX_IPV4_ADDR_LEN] = { 0x10, 0x20, 0x30, 0x40 };
    char ipv6[MAX_IPV6_ADDR_LEN] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                        0, 0, 0, 0, 0, 0, 0, 0x01 };
    pPdpContext[0]->OnUpdated(IP_PREFERRED, ipv4, ipv6);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv4.addr, ipv4, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, ipv6, MAX_IPV6_ADDR_LEN), true);

    char ipv4_2[MAX_IPV4_ADDR_LEN] = { 0x10, 0x20, 0x30, 0x42 };
    char ipv6_2[MAX_IPV6_ADDR_LEN] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                        0, 0, 0, 0, 0, 0, 0, 0x02 };

    pPdpContext[0]->OnUpdated(IP_PREFERRED, ipv4_2, ipv6_2);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv4.addr, ipv4_2, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, ipv6_2, MAX_IPV6_ADDR_LEN), true);
#if 0
    vector<DeprecatedAddress>& dep = pPdpContext[0]->getDeprecatedAddress();
    OutV("DeprecatedAddress.size:" + std::to_string(dep.size()));
    /*
    auto it = dep.begin();
    int i=0;
    for(it != dep.end()) {
        OutV("DeprecatedAddress" + std::to_string(i) + " :" + dep[i]);
        i++;
    }
    */

    pPdpContext[0]->OnUpdated(IP_DEPRECATED, ipv4, ipv6);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv4.addr, ipv4_2, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, ipv6_2, MAX_IPV6_ADDR_LEN), true);
    OutV("DeprecatedAddress.size:" + std::to_string(dep.size()));
    /*
    it = dep.begin();
    i=0;
    for(it != dep.end()) {
        OutV("DeprecatedAddress" + std::to_string(i) + " :" + dep[i]);
        i++;
    }
    */

    pPdpContext[0]->OnUpdated(IP_INVALID, ipv4, ipv6);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv4.addr, ipv4_2, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, ipv6_2, MAX_IPV6_ADDR_LEN), true);
    OutV("DeprecatedAddress.size:" + std::to_string(dep.size()));
#endif
    cleanupPdpContext(pPdpContext);
}

TEST(PdpContextTest, OnChangedTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    EXPECT_EQ(pPdpContext[0]->OnActivated(&dc), 0);
    EXPECT_EQ(pPdpContext[0]->OnChanged(&dc), 0);

    EXPECT_EQ(pPdpContext[0]->GetCID(), 1);
    EXPECT_GE(pPdpContext[0]->GetActive(), ACTIVE_AND_LINKDOWN);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv4.valid, true);
    EXPECT_EQ(pPdpContext[0]->GetDataCallInfo()->ipv6.valid, true);
    EXPECT_EQ(pPdpContext[0]->IsAvailable(), false);  // Already used
    EXPECT_EQ(pPdpContext[0]->GetInterfaceName(), std::string("rmnet") + std::string(to_string(0)));
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv4.addr, (char *)dc.ipv4.addr, MAX_IPV4_ADDR_LEN), true);
    EXPECT_EQ(isSameIp((char *)pPdpContext[0]->GetDataCallInfo()->ipv6.addr, (char *)dc.ipv6.addr, MAX_IPV6_ADDR_LEN), true);

    cleanupPdpContext(pPdpContext);
}

TEST(PdpContextTest, SetApnSettingTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    createPdpContext(pPdpContext);

    ApnSetting *r = ApnSetting::NewInstance(testCarrier, testApn,
                                            APN_TYPE_BIT_DEFAULT|APN_TYPE_BIT_SUPL,
                                            testUser, testPassword,
                                            DATA_PROTOCOL_IP, DATA_PROTOCOL_IPV4V6, SETUP_DATA_AUTH_PAP_CHAP);
    pPdpContext[0]->SetApnSetting(r);

    EXPECT_EQ(pPdpContext[0]->GetApnSetting(), r);
    EXPECT_EQ(pPdpContext[0]->GetApnSetting()->Equals(r), true);

    /*
    ApnSetting *r2 = ApnSetting::NewInstance(testCarrier, testApn,
                                            APN_TYPE_BIT_DEFAULT|APN_TYPE_BIT_SUPL,
                                            testUser, testPassword,
                                            DATA_PROTOCOL_IP, DATA_PROTOCOL_IPV4V6, SETUP_DATA_AUTH_PAP_CHAP);
    pPdpContext[0]->SetApnSetting(r2);
    EXPECT_EQ(pPdpContext[0]->GetApnSetting(), r2);
    EXPECT_EQ(pPdpContext[0]->GetApnSetting()->Equals(r2), true);

    delete r2;
    */

    cleanupPdpContext(pPdpContext);
}

TEST(PdpContextTest, InitDataCallTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);
    for (int i = 0; i < PDPCONTEXT_NUM; i++) {
        pPdpContext[i]->InitDataCall();
    }

    cleanupPdpContext(pPdpContext);
}

// TBD : add proper test parameters and sequences
TEST(PdpContextTest, SetAddrTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    pPdpContext[0]->SetAddr(&dc);
    /* Private member
    pPdpContext[0]->isMatchedIp(false, dc.ipv4.addr, dc.ipv4.addr);
    pPdpContext[0]->isMatchedIp(true, dc.ipv6.addr, dc.ipv6.addr);
    */

    cleanupPdpContext(pPdpContext);
}

// TBD : add proper test parameters and sequences
TEST(PdpContextTest, OnUpdateQosInfoTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    QosSession_t qosSession;

    createPdpContext(pPdpContext);

    pPdpContext[0]->OnUpdateQosInfo(0, qosSession);

    cleanupPdpContext(pPdpContext);
}

// TBD : add proper test parameters and sequences
TEST(PdpContextTest, isConnectedTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    pPdpContext[0]->isConnected();

    cleanupPdpContext(pPdpContext);
}

// TBD : add proper test parameters and sequences
TEST(PdpContextTest, GetStateToStringTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    pPdpContext[0]->GetStateToString();

    cleanupPdpContext(pPdpContext);
}

// TBD : add proper test parameters and sequences
TEST(PdpContextTest, GetActiveStateToStringTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    pPdpContext[0]->GetActiveToString();

    cleanupPdpContext(pPdpContext);
}

// TBD : add proper test parameters and sequences
TEST(PdpContextTest, ToStringTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];

    createPdpContext(pPdpContext);

    pPdpContext[0]->ToString();

    cleanupPdpContext(pPdpContext);
}


TEST(ApnSettingTest, ConstructApnSettingTest) {
    ApnSetting *r = ApnSetting::NewInstance(testCarrier, testApn,
                                            APN_TYPE_BIT_DEFAULT|APN_TYPE_BIT_SUPL,
                                            testUser, testPassword,
                                            DATA_PROTOCOL_IP, DATA_PROTOCOL_IPV4V6, SETUP_DATA_AUTH_PAP_CHAP);
    EXPECT_EQ(strcmp(r->GetCarrier(), testCarrier), 0);
    EXPECT_EQ(r->GetId(), -1);
    EXPECT_EQ(strcmp(r->GetApn(), testApn), 0);
    EXPECT_EQ(strcmp(r->GetUsername(), testUser), 0);
    EXPECT_EQ(strcmp(r->GetPassword(), testPassword), 0);
    EXPECT_EQ(strcmp(r->GetTypeString(), string(APN_TYPE_DEFAULT "," APN_TYPE_SUPL).c_str()), 0);
    EXPECT_EQ(strcmp(r->GetProtocol(), DATA_PROTOCOL_IP), 0);
    EXPECT_EQ(strcmp(r->GetRoamingProtocol(), DATA_PROTOCOL_IPV4V6), 0);
    EXPECT_EQ(r->GetAuthType(), SETUP_DATA_AUTH_PAP_CHAP);
    EXPECT_EQ(r->GetSupportedTypesBitmask(), APN_TYPE_BIT_DEFAULT|APN_TYPE_BIT_SUPL);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_ALL), true); // Only one Bitmask is matched, this can be handled
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_DEFAULT), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_MMS), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_SUPL), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_DUN), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_HIPRI), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_FOTA), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_IMS), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_CBS), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_IA), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_EMERGENCY), false);
    EXPECT_EQ(r->isPcscfRequiredForDefaultPDN(), false);

    ApnSetting *clone = r->Clone();

    EXPECT_EQ(clone->Equals(r), true);
    EXPECT_EQ(r->Equals(clone), true);

    string dumpString("ApnSetting{_id=-1,apn=testapn,type=default,supl,authtype=3,protocol=IP,username=Nouser,password=NoPassword,m_roaming_protocol=IPV4V6}");
    EXPECT_EQ(r->ToString(), dumpString);

    delete r;
    delete clone;

    r = ApnSetting::NewInstance(testCarrier2, testApn,
                                            APN_TYPE_BIT_ALL,
                                            testUser, testPassword,
                                            DATA_PROTOCOL_IP, DATA_PROTOCOL_IPV4V6, SETUP_DATA_AUTH_CHAP);
    EXPECT_EQ(r->GetAuthType(), SETUP_DATA_AUTH_CHAP);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_ALL), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_DEFAULT), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_MMS), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_SUPL), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_DUN), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_HIPRI), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_FOTA), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_IMS), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_CBS), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_IA), true);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_EMERGENCY), true);
    EXPECT_EQ(r->isPcscfRequiredForDefaultPDN(), true);
    delete r;


    r = ApnSetting::NewInstance(testCarrier, testApn,
                                            APN_TYPE_BIT_NONE,
                                            testUser, testPassword,
                                            DATA_PROTOCOL_IP, DATA_PROTOCOL_IP);
    r->UpdateProtocol(DATA_PROTOCOL_IPV4V6);
    EXPECT_EQ(strcmp(r->GetProtocol(), DATA_PROTOCOL_IPV4V6), 0);
    EXPECT_EQ(strcmp(r->GetRoamingProtocol(), DATA_PROTOCOL_IP), 0);
    EXPECT_EQ(r->GetAuthType(), SETUP_DATA_AUTH_NONE);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_ALL), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_DEFAULT), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_MMS), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_SUPL), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_DUN), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_HIPRI), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_FOTA), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_IMS), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_CBS), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_IA), false);
    EXPECT_EQ(r->CanHandleType(APN_TYPE_BIT_EMERGENCY), false);
    EXPECT_EQ(r->isPcscfRequiredForDefaultPDN(), false);

    delete r;
}

// This is Debugging utility, trivial tests for coverage check
TEST(PsUtilTest, DumpTest) {
    char data[100] = { 0x10, 0x20, 0x30, 0x40};
    PsUtil::DumpBuf(data, 100);
    sit_pdp_data_call_item_pcscf_ext testv;
    PsUtil::DumpPcscfExtPayload((char*) &testv);

    DataCall dc_local = dc;
    PsUtil::PrintDataCallInfo(&dc_local, "Test", "DumpTest", RilApplication::RIL_HalVersionCode);
    PsUtil::decodeAMBR(&dc_local);
    dc_local.ambr.octet2 = 6;
    PsUtil::decodeAMBR(&dc_local);
    dc_local.ambr.octet2 = 8;
    PsUtil::decodeAMBR(&dc_local);
    PsUtil::PrintAddressInfo(&dc_local);

    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    createPdpContext(pPdpContext);

    PsDataBuilder builder(RilApplication::RIL_HalVersionCode);
    const RilData *rilData = builder.BuildSetupDataCallResponse(RIL_E_SUCCESS, pPdpContext[0]);
    PsDataBuilder builder_V1_6(HAL_VERSION_CODE(1, 6));
    const RilData *rilData2 = builder.BuildSetupDataCallResponse(RIL_E_SUCCESS, pPdpContext[0]);
    PsUtil::PrintRilDataInfo((RIL_Data_Call_Response_v11 *)rilData, "Test");
    PsUtil::PrintRilDataInfo((RIL_SetupDataCallResult_V1_6 *)rilData2, "Test");
    cleanupPdpContext(pPdpContext);
}

TEST(PsConverterTest, psDataBuilderTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    createPdpContext(pPdpContext);
    EXPECT_EQ(pPdpContext[0]->OnActivated(&dc), 0);

    // Just suppose current HalVersion
    PsDataBuilder builder(RilApplication::RIL_HalVersionCode);
    CheckRilData(builder.BuildSetupDataCallResponse(RIL_E_SUCCESS, pPdpContext[0]));

    CheckRilData(builder.BuildSetupDataCallResponse(RIL_E_SUCCESS, 10));

    CheckRilData(builder.BuildSetupDataCallResponse(pPdpContext[0]));

    char pco_contents[5] = {0x01,0x02,0x04,0x40,0x80};
    CheckRilData(builder.BuildPcoData(1, PDP_TYPE_IPV4V6, 1, 5, pco_contents));

    cleanupPdpContext(pPdpContext);
}

TEST(PsConverterTest, psDataCallListBuilderTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    createPdpContext(pPdpContext);
    EXPECT_EQ(pPdpContext[0]->OnActivated(&dc), 0);

    PsDataCallListBuilder builder(RilApplication::RIL_HalVersionCode);
    builder.AddDataCall(pPdpContext[0]);
    builder.Clear();
    builder.AddDataCall(pPdpContext[0]);

    CheckRilData(builder.Build());

    cleanupPdpContext(pPdpContext);
}

TEST(PsConverterTest, psNasTimerStatusBuilderTest) {
    PsDataNasTimerStatusBuilder builder;
    SitNasTimerStatus snts = { .type = 0, .status = 0, .value = 0, .apn = "testapn", };

    CheckRilData(builder.BuildNasTimerStatus(&snts));
}

TEST(PsNetifControllerTest, PrimitiveFunctionTest) {
    PdpContext *pPdpContext[PDPCONTEXT_NUM];
    createPdpContext(pPdpContext);
    EXPECT_EQ(pPdpContext[0]->OnActivated(&dc), 0);
    char IFN[10] = {"rmnet1"};
    unsigned char IPV6ADDR[20] = {"2001::1/64"};

    NetIfController netifc(pPdpContext[1]);
    netifc.GetIfSockV6();
    netifc.SetIfFlags(netifc.GetIfSock(), IFN, 0);
    netifc.SetIfUp(netifc.GetIfSock(), IFN, netifc.GetIfFlags(netifc.GetIfSock(), IFN));
    netifc.SetIfDown(netifc.GetIfSock(), IFN, netifc.GetIfFlags(netifc.GetIfSock(), IFN));
    netifc.SetIfUp(netifc.GetIfSock(), IFN, netifc.GetIfFlags(netifc.GetIfSock(), IFN));
    // Not available for non physical if
    // netifc.SetIfDormant(netifc.GetIfSock(), IFN, netifc.GetIfFlags(netifc.GetIfSock(), IFN), true);
    // netifc.SetIfDormant(netifc.GetIfSock(), IFN, netifc.GetIfFlags(netifc.GetIfSock(), IFN), false);
    netifc.GetLastError();
    netifc.SetIfAddrIpv6(netifc.GetIfSockV6(), IFN, netifc.GetIfFlags(netifc.GetIfSock(), IFN), IPV6ADDR);
    netifc.SetIfMaxRsCount(IFN, 0);
    netifc.GetIfMaxRsCount(IFN);
    netifc.SetIfRsDelay(IFN, 0);
    netifc.GetIfRsDelay(IFN);
    netifc.SetIfRsInterval(IFN, 0);
    netifc.GetIfRsInterval(IFN);

    cleanupPdpContext(pPdpContext);
}
