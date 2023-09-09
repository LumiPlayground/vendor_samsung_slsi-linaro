/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "vsimhandler.h"
#include "vsimhandlerfactory.h"
#include "messageid.h"

IMPLEMENT_SERVICE_HANDLER_FACTORY_INIT(VsimHandlerFactory)

BEGIN_FACTORY_ENTRY(VsimHandlerFactory)
    ADD_FACTORY_ENTRY(VsimNotificationHandler, MSG_VSIM_NOTIFICATION)
    ADD_FACTORY_ENTRY(VsimOperationHandler, MSG_VSIM_OPERATION)

    // Indication
    ADD_FACTORY_ENTRY(VsimOperationIndHandler, MSG_VSIM_OPERATION_IND)
END_FACTORY_ENTRY()
