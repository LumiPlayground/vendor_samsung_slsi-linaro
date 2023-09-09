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
 * miscnetworkhandlerfactory.h
 *
 *  Created on: 2020. 7. 6.
 *      Author: sungwoo48.choi
 */

#ifndef __MISC_NETWORK_HANDLER_FACTORY_H__
#define __MISC_NETWORK_HANDLER_FACTORY_H__

#include "servicehandler.h"

class Service;

class MiscNetworkHandlerFactory : public ServiceHandlerFactory {
    DECLARE_SERVICE_HANDLER_FACTORY_INIT(MiscNetworkHandlerFactory)
};

#endif /* __MISC_NETWORK_HANDLER_FACTORY_H__ */
