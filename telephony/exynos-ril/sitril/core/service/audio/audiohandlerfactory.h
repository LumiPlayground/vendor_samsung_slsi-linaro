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
 * audiohandlerfactory.h
 *
 *  Created on: 2020. 7. 21.
 *      Author: sungwoo48.choi
 */

#ifndef __AUDIO_HANDLER_FACTORY_H__
#define __AUDIO_HANDLER_FACTORY_H__

#include "servicehandler.h"

class Service;

class AudioHandlerFactory : public ServiceHandlerFactory {
    DECLARE_SERVICE_HANDLER_FACTORY_INIT(AudioHandlerFactory)
};

#endif /* __AUDIO_HANDLER_FACTORY_H__ */
