/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any    means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
/*
 * supphandlerfactory.h
 */

#ifndef __SUPP_HANDLER_FACTORY_H__
#define __SUPP_HANDLER_FACTORY_H__

#include "servicehandler.h"

class Service;

class SuppHandlerFactory : public ServiceHandlerFactory {
    DECLARE_SERVICE_HANDLER_FACTORY_INIT(SuppHandlerFactory)
};

#endif /* __SUPP_HANDLER_FACTORY_H__ */
