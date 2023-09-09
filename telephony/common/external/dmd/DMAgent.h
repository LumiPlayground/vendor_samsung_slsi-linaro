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
 * DMAgent.h
 *
 *  Created on: 2018. 5. 18.
 */

#ifndef __DM_AGENT_H__
#define __DM_AGENT_H__

#include "DMConstants.h"
#include <string>

struct AutologCallback;
struct DMAgent;
struct DMAgentCallback;
struct FileOperationCallback;

DMAgent *DMAgent_Init();
int DMAgent_GetActiveMode();
void DMAgent_SetActiveMode(int activeMode);
int DMAgent_OnResponse(const void *data, size_t datalen);

/**
 * DMAgent
 */
struct DMAgent {
    virtual ~DMAgent()=default;
    virtual void onResponse(const void *data, size_t datalen)=0;
    virtual void registerCallback(DMAgentCallback *callback)=0;
    virtual void registerCallback(AutologCallback *callback)=0;
    virtual void registerCallback(FileOperationCallback *callback)=0;
};

/**
 * DMAgentCallback
 */
struct DMAgentCallback {
    virtual ~DMAgentCallback()=default;
    virtual void onActiveModeChange(int oldMode, int newMode)=0;
    virtual void onStart()=0;
    virtual void onStop()=0;
    virtual void onSendProfile(const void *data, size_t datalen)=0;
    /*
     * TBD
    virtual void onSaveSnapshot()=0;
    */
    virtual void onCommand(int id, const void *data, size_t datalen)=0;
    virtual void onRawRequest(const void *data, size_t datalen)=0;
};

/**
 * FileOperationCallback
 */
struct FileOperationCallback {
    virtual ~FileOperationCallback()=default;
    virtual void onRefreshManageFileList()=0;
    virtual void onDocumentSizeChanged(size_t size)=0;
    virtual void onManagedFileCountChanged(size_t count)=0;
    virtual void onBaseDirectoryChanged(const std::string& dir)=0;
    virtual void onSetFilenamePrefix(const std::string& prefix)=0;
};

/**
 * AutologCallback
 */
struct AutologCallback {
    virtual ~AutologCallback()=default;
    virtual int onSaveAutolog()=0;
};

struct FactoryDMCallback;

/**
 * FactoryDMAgent
 */
struct FactoryDMAgent {
    virtual ~FactoryDMAgent()=default;
    virtual void registerCallback(FactoryDMCallback *callback)=0;
};

struct FactoryDMCallback {
    virtual ~FactoryDMCallback()=default;
    virtual void onActiveModeChange(int oldMode, int newMode)=0;
    virtual void onStart()=0;
    virtual void onStop()=0;
    virtual void onSendProfile(const void *data, size_t datalen)=0;
};

FactoryDMAgent *FactoryDMAgent_Init();

#endif /* __DM_AGENT_H__ */
