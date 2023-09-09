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
 * DMFileManager.h
 *
 *  Created on: 2018. 5. 18.
 *  Modified on: 2021. 9. 10.
 */

#ifndef __DM_FILE_MANAGER_H__
#define __DM_FILE_MANAGER_H__

#include <fstream>
#include <string>
#include <list>
#include "sdmdoc.h"
#include "zipworker.h"

/**
 * DMFileManager
 */
class DMFileManager {
private:
    struct DMFile {
        std::string fileName;
        unsigned int fileSize;
    };
    std::list<DMFile> mFileList;
    unsigned int mCapacity; /// disk used percent
    unsigned int mManagedFileCount;  // file limit
    unsigned int mTotalSize;
    unsigned int mMaxSize;
    OnZipWorkerListener *mZipWorkListener;

private:
    DMFileManager();
    DMFileManager(const DMFileManager &) = delete;
    DMFileManager& operator=(DMFileManager const&) = delete;

public:
    virtual ~DMFileManager();

public:
    void init();
    void add(const std::string& fileName);
    void add(const SdmDoc *doc);
    void removeAll();
    void refreshFileList();
    void shrink();
    void setLimit(unsigned int capacity, unsigned int managedFileCount);
    void setLimit(unsigned int managedFileCount) {
        setLimit(mCapacity, managedFileCount);
    }
protected:
    void createManagedFileList();

public:
    static DMFileManager *getInstance();
};

#endif // __DM_FILE_MANAGER_H__
