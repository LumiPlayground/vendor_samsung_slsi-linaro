
/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "defaultservicefactory.h"
#include "serviceinterface.h"
#include "cscservice.h"
#include "psservice.h"
#include "networkservice.h"
#include "simservice.h"
#include "miscservice.h"
#include "smsservice.h"
#include "audioservice.h"
#include "imsservice.h"
#include "vsimservice.h"
#include "gpsservice.h"
#include "stkservice.h"
#include "supplementaryservice.h"
#include "embmsservice.h"
#include "simioservice.h"
#include "sapservice.h"

void DefaultServiceFactory::OnInitialize() {
    AddAvailableService(RIL_SERVICE_NETWORK);
    AddAvailableService(RIL_SERVICE_CSC);
    AddAvailableService(RIL_SERVICE_PS);
    AddAvailableService(RIL_SERVICE_SIM);
    AddAvailableService(RIL_SERVICE_MISC);
    AddAvailableService(RIL_SERVICE_SMS);
    AddAvailableService(RIL_SERVICE_AUDIO);
    AddAvailableService(RIL_SERVICE_IMS);
    AddAvailableService(RIL_SERVICE_GPS);
    AddAvailableService(RIL_SERVICE_STK);
    AddAvailableService(RIL_SERVICE_EMBMS);
    AddAvailableService(RIL_SERVICE_SIM_IO);
    AddAvailableService(RIL_SERVICE_VSIM);
    AddAvailableService(RIL_SERVICE_SAP);
    AddAvailableService(RIL_SERVICE_SUPPLEMENTARY);
}

Service *DefaultServiceFactory::CreateService(RilContext *pRilContext, UINT nServiceId)
{
    Service *pService = NULL;

    switch (nServiceId) {
    case RIL_SERVICE_NETWORK:
        pService = new NetworkService(pRilContext);
        break;
    case RIL_SERVICE_CSC:
        pService = new CscService(pRilContext);
        break;
    case RIL_SERVICE_PS:
        pService = new PsService(pRilContext);
        break;
    case RIL_SERVICE_SIM:
        pService = new SimService(pRilContext);
        break;
    case RIL_SERVICE_MISC:
        pService = new MiscService(pRilContext);
        break;
    case RIL_SERVICE_SMS:
        pService = new SmsService(pRilContext);
        break;
    case RIL_SERVICE_AUDIO:
        pService = new AudioService(pRilContext);
        break;
    case RIL_SERVICE_IMS:
        pService = new ImsService(pRilContext);
        break;
    case RIL_SERVICE_VSIM:
        pService = new VSimService(pRilContext);
        break;
    case RIL_SERVICE_GPS:
        pService = new GpsService(pRilContext);
        break;
    case RIL_SERVICE_STK:
        pService = new StkService(pRilContext);
        break;
    case RIL_SERVICE_SUPPLEMENTARY:
        pService = new SupplementaryService(pRilContext);
        break;
    case RIL_SERVICE_EMBMS:
        pService = new EmbmsService(pRilContext);
        break;
    case RIL_SERVICE_SIM_IO:
        pService = new SimIoService(pRilContext);
        break;
    case RIL_SERVICE_SAP:
        pService = new SapService(pRilContext);
        break;
    default:
        // TODO unsupported service creation
        return NULL;
    } // end switch ~

    if (pService != NULL && pService->GetServiceId() != nServiceId) {
        delete pService;
        pService = NULL;
    }

    return pService;
}
