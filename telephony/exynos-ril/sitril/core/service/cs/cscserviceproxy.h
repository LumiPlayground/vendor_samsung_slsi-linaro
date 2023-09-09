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
 * cscserviceproxy.h
 *
 *  Created on: 2020. 7. 17.
 *      Author: sungwoo48.choi
 */

#ifndef __CSC_SERVICE_PROXY_H__
#define __CSC_SERVICE_PROXY_H__

class AudioService;
class CallId;
class CallList;
class ClirInfo;
class CscService;
class EccInfo;
class NetworkService;
class Service;
class EmergencyCallHelper;

class CscServiceProxy {
private:
    CscService *mService;

public:
    CscServiceProxy(Service *service);
    ~CscServiceProxy();
public:
    bool HasService() { return mService != NULL; }
    CallList *GetCallList();
    CallId *GetCallId();
    NetworkService *FindNetworkService();
    AudioService *FindAudioService();
    bool IsRadioOffOrNotAvailable();
    EmergencyCallHelper *GetEmergencyCallHelper();
};

#endif /* __CS_SERVICE_PROXY_H__ */
