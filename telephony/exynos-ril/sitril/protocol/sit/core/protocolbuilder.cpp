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
 * protocolbuilder.cpp
 *
 *  Created on: 2014. 6. 27.
 *      Author: sungwoo48.choi
 */

#include "protocolbuilder.h"
#include <telephony/ril.h>
#include <slsi/radio_v1_4.h>

/**
 * ProtocolBuilder
 */
ProtocolBuilder::ProtocolBuilder()
    : m_pTokenGen(NULL) {
    m_pTokenGen = TokenGen::GetInstacne();
}

ProtocolBuilder::~ProtocolBuilder() {
}

void ProtocolBuilder::InitRequestHeader(RCM_HEADER *hdr, int id) {
    if (hdr != NULL) {
        memset(hdr, 0, sizeof(RCM_HEADER));
        hdr->type = RCM_TYPE_REQUEST;
        hdr->id = (UINT16)(id & 0xFFFF);
        hdr->ext.req.token = m_pTokenGen->GetNext();
    }
}

void ProtocolBuilder::InitRequestHeader(RCM_HEADER *hdr, int id, int length) {
    if (hdr != NULL) {
        memset(hdr, 0, sizeof(RCM_HEADER));
        hdr->type = RCM_TYPE_REQUEST;
        hdr->id = (UINT16)(id & 0xFFFF);
        hdr->length = (UINT16)(length & 0xFFFF);
        hdr->ext.req.token = m_pTokenGen->GetNext();
    }
}

/**
 * For GPS IND type request to CP
 */
void ProtocolBuilder::InitIndRequestHeader(RCM_IND_HEADER *hdr, int id, int length) {
    if (hdr != NULL) {
        memset(hdr, 0, sizeof(RCM_IND_HEADER));
        hdr->type = RCM_TYPE_INDICATION;
        hdr->id = (UINT16)(id & 0xFFFF);
        hdr->length = (UINT16)(length & 0xFFFF);
    }
}

void ProtocolBuilder::InitRequestHeader(RCM_HEADER *hdr, int id, int length, RCM_TOKEN token) {
    if (hdr != NULL) {
        memset(hdr, 0, sizeof(RCM_HEADER));
        hdr->type = RCM_TYPE_REQUEST;
        hdr->id = (UINT16)(id & 0xFFFF);
        hdr->length = (UINT16)(length & 0xFFFF);
        hdr->ext.req.token = token;
    }
}

int ProtocolBuilder::switchRafValueForCP(int networkTypeBitmap) {
    int result = 0;

    if (networkTypeBitmap & RAF_UNKNOWN) {
        result = result | RAF_CP_UNKNOWN;
    }
    if (networkTypeBitmap & RAF_GPRS) {
        result = result | RAF_CP_GPRS;
    }
    if (networkTypeBitmap & RAF_EDGE) {
        result = result | RAF_CP_EDGE;
    }
    if (networkTypeBitmap & RAF_UMTS) {
        result = result | RAF_CP_UMTS;
    }
    if (networkTypeBitmap & RAF_IS95A) {
        result = result | RAF_CP_IS95A;
    }
    if (networkTypeBitmap & RAF_IS95B) {
        result = result | RAF_CP_IS95B;
    }
    if (networkTypeBitmap & RAF_1xRTT) {
        result = result | RAF_CP_1xRTT;
    }
    if (networkTypeBitmap & RAF_EVDO_0) {
        result = result | RAF_CP_EVDO_0;
    }
    if (networkTypeBitmap & RAF_EVDO_A) {
        result = result | RAF_CP_EVDO_A;
    }
    if (networkTypeBitmap & RAF_HSDPA) {
        result = result | RAF_CP_HSDPA;
    }
    if (networkTypeBitmap & RAF_HSUPA) {
        result = result | RAF_CP_HSUPA;
    }
    if (networkTypeBitmap & RAF_HSPA) {
        result = result | RAF_CP_HSPA;
    }
    if (networkTypeBitmap & RAF_EVDO_B) {
        result = result | RAF_CP_EVDO_B;
    }
    if (networkTypeBitmap & RAF_EHRPD) {
        result = result | RAF_CP_EHRPD;
    }
    if (networkTypeBitmap & (RAF_LTE | RAF_LTE_CA)) {
        result = result | RAF_CP_LTE;
    }
    if (networkTypeBitmap & RAF_HSPAP) {
        result = result | RAF_CP_HSPAP;
    }
    if (networkTypeBitmap & RAF_GSM) {
        result = result | RAF_CP_GSM;
    }
    if (networkTypeBitmap & RAF_TD_SCDMA) {
        result = result | RAF_CP_TD_SCDMA;
    }
    if (networkTypeBitmap & RAF_NR) {
        result = result | RAF_CP_5G;
    }

    return result;
}
