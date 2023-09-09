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
 * pshandlerfactory.h
 *
 *  Created on: 2020. 8. 26.
 *      Author: juneho.kwon
 */

#ifndef __PS_HANDLER_FACTORY_H__
#define __PS_HANDLER_FACTORY_H__

#include "servicehandler.h"

class Service;

class pshandlerfactory : public ServiceHandlerFactory {
    DECLARE_SERVICE_HANDLER_FACTORY_INIT(pshandlerfactory)
};

#endif /* __PS_HANDLER_FACTORY_H__ */

