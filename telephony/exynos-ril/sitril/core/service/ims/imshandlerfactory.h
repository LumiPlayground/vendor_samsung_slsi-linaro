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
 * imshandlerfactory.h
 *
 *  Created on: 2020. 7. 8.
 *      Author: sungwoo48.choi
 */

#ifndef __IMS_HANDLER_FACTORY_H__
#define __IMS_HANDLER_FACTORY_H__

#include "servicehandler.h"
#include <map>
#include <string>

class Service;

class ImsHandlerFactory : public ServiceHandlerFactory {
    DECLARE_SERVICE_HANDLER_FACTORY_INIT(ImsHandlerFactory)
private:
    std::map<std::string, std::shared_ptr<ServiceHandler>> mHandler;
protected:
    void OnCreate();
    void OnDestroy();
    ServiceHandler *OnPostload(int messageId, Service *service);
private:
    ServiceHandler *Find(const std::string& key);
    bool Contain(const std::string& key) const;
    void Put(const std::string& key, std::shared_ptr<ServiceHandler> h);
};

#endif /* __IMS_HANDLER_FACTORY_H__ */
