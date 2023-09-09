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
 * ril_vendor_internal.h
 *
 *  Created on: 2019. 2. 19.
 */

#ifndef __RIL_VENDOR_INTERNAL_H__
#define __RIL_VENDOR_INTERNAL_H__


// The value for SAP shall be matched to librilutils/proto/sap-api.proto
#ifndef RIL_REQUEST_SAP_BASE
#define RIL_REQUEST_SAP_BASE 60000
#define RIL_REQUEST_SAP_UNSOL_RESPONSESAP 61000
#endif

#define RIL_REQUEST_SIM_SAP_CONNECT                     (RIL_REQUEST_SAP_BASE + 1)
#define RIL_REQUEST_SIM_SAP_DISCONNECT                  (RIL_REQUEST_SAP_BASE + 2)
#define RIL_REQUEST_SIM_SAP_APDU                        (RIL_REQUEST_SAP_BASE + 3)
#define RIL_REQUEST_SIM_SAP_TRANSFER_ATR                (RIL_REQUEST_SAP_BASE + 4)
#define RIL_REQUEST_SIM_SAP_POWER                       (RIL_REQUEST_SAP_BASE + 5)
#define RIL_REQUEST_SIM_SAP_RESET_SIM                   (RIL_REQUEST_SAP_BASE + 6)
#define RIL_REQUEST_SIM_SAP_TRANSFER_CARD_READER_STATUS (RIL_REQUEST_SAP_BASE + 8)
#define RIL_REQUEST_SIM_SAP_SET_TRANSFER_PROTOCOL       (RIL_REQUEST_SAP_BASE + 10)

#define RIL_UNSOL_RESPONSE_SIM_SAP_DISCONNECT           (RIL_REQUEST_SAP_UNSOL_RESPONSESAP + 2)
#define RIL_UNSOL_RESPONSE_SIM_SAP_STATUS               (RIL_REQUEST_SAP_UNSOL_RESPONSESAP + 7)
#define RIL_UNSOL_RESPONSE_SIM_SAP_ERROR_RESP           (RIL_REQUEST_SAP_UNSOL_RESPONSESAP + 9)


#endif /* __RIL_VENDOR_INTERNAL_H__ */
