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
 * rillogcapture.h
 *
 *  Created on: 2018. 4. 24.
 *      Author: sungwoo48.choi
 */

#ifndef __RIL_LOG_CAPTURE_H__
#define __RIL_LOG_CAPTURE_H__

#define NAME_SIZE 100

#include <string>
#include "thread.h"
#include "types.h"

class CMutex;

class RilLogCapture : public Runnable
{
    DECLARE_MODULE_TAG()
public:
    RilLogCapture();
    RilLogCapture(const RilLogCapture &) = delete;
    RilLogCapture& operator=(RilLogCapture const&) = delete;
    virtual ~RilLogCapture();
    int Start();
    int Init();
    void Run();
    static RilLogCapture *MakeInstance();
    int OpenMessagePipe();
    int notifyNewRilLog(const char *rilLogMsg);

private:
    Thread *m_pRilLogCaptureThread;
    int m_nRilLogPipeW;
    int m_nRilLogPipeR;
    static bool m_RilLogIsInited;
    std::string mBaseDir;
    size_t mTotalSize;
    size_t mChunkSize;
    size_t mNumOfFile;
    int mPos;
    FILE *mLog;
    size_t mFileSize;
    bool mFirstBoot;
    bool mBaseDirAccessible;

protected:
    virtual void OnClose();
    void LoadConfigs();
    std::string GetValidBaseDirectory();
    bool IsDirectoryExists(std::string path);
    bool MakeDirectory(std::string path);
    std::string GetCurrentLogDir();
    std::string GetPreviousLogDir();
    int GetNextIndex();
    int GetResumeIndex();
    bool ResumeFromCrash();
    std::string GetNextFilename(int index);
    FILE *GetNextFileHandle(int index);
    CMutex *m_pRilLogRMutex;
};

#endif /* __RIL_LOG_CAPTURE_H__ */
