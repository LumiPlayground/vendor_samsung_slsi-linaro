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
 * sdmdocdirector.h
 *
 *  Created on: 2021. 9. 15.
 */

#ifndef __SDM_DOC_DIRECTOR_H__
#define __SDM_DOC_DIRECTOR_H__

#include "sdmdocmanager.h"
#include "DMFileManager.h"
#include "postaction.h"

#include <functional>
#include <mutex>

struct SdmDocDirector {
    SdmDocManager *mDocManager;
    DMFileManager *mFileManager;
    std::mutex mMutex;
    bool mStarted;
    SdmDoc *mDoc;
    PostAction<std::function<void()>> mPostAction;
    SdmDocDirector(const SdmDocDirector &) = delete;
    SdmDocDirector& operator=(SdmDocDirector const&) = delete;
    SdmDocDirector(SdmDocManager *docManager, DMFileManager *fileManager);
    SdmDocDirector(SdmDocManager *docManager, DMFileManager *fileManager,
                    const PostAction<std::function<void()>>& postAction);
    virtual ~SdmDocDirector();

    void writeData(const void *data, size_t datalen);
    void onStart();
    void onStop();
    void closeDocument();
};

#endif /* __SDM_DOC_DIRECTOR_H__ */
