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
 * memblock.h
 *
 *  Created on: 2021. 9. 8.
 */

#ifndef __MEM_BLOCK_H__
#define __MEM_BLOCK_H__

#include <stdint.h>
#include <string.h>

struct MemBlock {
    uint8_t *mMem;
    size_t mSize;

    MemBlock() : mMem(0), mSize(0) {}
    MemBlock(const void *data, size_t size);
    MemBlock(const MemBlock& o);
    MemBlock(MemBlock&& o) noexcept;
    ~MemBlock() { clear(); }
    MemBlock& operator=(const MemBlock& o);
    MemBlock& operator=(MemBlock&& o) noexcept;
    bool operator==(const MemBlock& o);
    void setData(const void *data, size_t size);
    void *ptr() { return mMem; }
    const void *ptr() const { return mMem; }
    size_t size() { return mSize; }
    size_t size() const { return mSize; }
    void clear();
    bool hasData() { return mMem && mSize > 0; }
};

#endif /* __MEM_BLOCK_H__ */
