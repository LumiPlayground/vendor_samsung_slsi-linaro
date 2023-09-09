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
 * rwlockholder.h
 *
 *  Created on: 2020. 3. 3.
 */
#ifndef __RW_LOCK_HOLDER_H__
#define __RW_LOCK_HOLDER_H__

class RwlockHolder {
private:
    pthread_rwlock_t mRwlock;
    int mRef;
public:
    RwlockHolder() : mRef(0) {
        pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
        mRwlock = rwlock;
    }
    ~RwlockHolder() {
        if (mRef-- > 0) {
            pthread_rwlock_unlock(&mRwlock);
        }
    }
    void rwlock() {
        pthread_rwlock_wrlock(&mRwlock);
        mRef++;
    }
    void rdlock() {
        pthread_rwlock_rdlock(&mRwlock);
        mRef++;
    }
    void unlock() {
        mRef--;
        pthread_rwlock_unlock(&mRwlock);
    }
};

#endif // __RW_LOCK_HOLDER_H__
