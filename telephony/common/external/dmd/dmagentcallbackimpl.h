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
 * dmagentcallbackimpl.h
 *
 *  Created on: 2021. 10. 12.
 */
#ifndef __DM_AGENT_CALLBACK_IMPL_H__
#define __DM_AGENT_CALLBACK_IMPL_H__

#include "DMAgent.h"

class DMFileManager;

/**
 * AutologImpl
 */
struct AutologImpl : public AutologCallback {
    int onSaveAutolog();
};

/**
 * FileOperationCallbackImpl
 */
struct FileOperationCallbackImpl : public FileOperationCallback {
    DMFileManager *mFileManager;
    FileOperationCallbackImpl() : FileOperationCallbackImpl(0) {}
    FileOperationCallbackImpl(DMFileManager *fileManager) : mFileManager(fileManager) {}
    void onRefreshManageFileList();
    void onDocumentSizeChanged(size_t size);
    void onManagedFileCountChanged(size_t count);
    void onBaseDirectoryChanged(const std::string& dir);
    void onSetFilenamePrefix(const std::string& prefix);
};

#endif  // __DM_AGENT_CALLBACK_IMPL_H__
