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
 * sdmdocdirector.cpp
 *
 *  Created on: 2021. 9. 15.
 */
#include "sdmdocdirector.h"
#include "sdmdocenv.h"
#include "log.h"

SdmDocDirector::SdmDocDirector(SdmDocManager *docManager, DMFileManager *fileManager) :
    mDocManager(docManager),
    mFileManager(fileManager),
    mStarted(false),
    mDoc(0) {
}

SdmDocDirector::SdmDocDirector(SdmDocManager *docManager, DMFileManager *fileManager,
                const PostAction<std::function<void()>>& postAction)
    : SdmDocDirector(docManager, fileManager) {
    mPostAction = postAction;
}

SdmDocDirector::~SdmDocDirector() {
    closeDocument();
}
void SdmDocDirector::writeData(const void *data, size_t datalen) {
    std::lock_guard<std::mutex> lock(mMutex);
    if (mStarted) {
        if (mDocManager == nullptr) {
            LOGE("mDocManager is nullptr");
            return ;
        }

        if (mDoc == nullptr) {
            mDoc = mDocManager->newDocument();
            if (mDoc) {
                LOGD("Create New SDM Document");
                LOGD("NewDocument: %s", mDoc->toString().c_str());

                // Post actions as soon as new SDM file is created.
                LOGD("mPostAction.doAction");
                mPostAction.doAction();
            } else {
                LOGE("Failed to create new document.");
            }
        }

        if (mDoc) {
            int ret = mDoc->write(data, datalen);
            if (ret > 0) {
                if (mDoc->size() > SdmDocEnv::getMaxDocSize()) {
                    LOGD("Doc is exceeded a max size.");
                    closeDocument();
                }
            }
        }
    } else {
        LOGV("SdmDocDirector is not started. Drop packets (%zu bytes)", datalen);
    }
}

void SdmDocDirector::onStart() {
    std::lock_guard<std::mutex> lock(mMutex);
    mStarted = true;
    LOGD("SdmDocDirector: started");
}

void SdmDocDirector::onStop() {
    std::lock_guard<std::mutex> lock(mMutex);
    mStarted = false;
    LOGD("SdmDocDirector: stopped");
    closeDocument();
    if (mDocManager) {
        mDocManager->increaseUid();
    }
}

void SdmDocDirector::closeDocument() {
    if (mDoc) {
        LOGD("Document(%s) is closing", mDoc->toString().c_str());
        mDoc->close();
        if (mFileManager) {
            mFileManager->add(mDoc);
        }
        delete mDoc;
        mDoc = nullptr;
    } else {
        LOGE("LoggingDirector::%s no active document", __FUNCTION__);
    }
}
