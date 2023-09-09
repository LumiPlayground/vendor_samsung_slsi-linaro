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
 * memblock.cpp
 *
 *  Created on: 2021. 9. 8.
 */
#include "memblock.h"

#include "dmd_test_util.h"

MemBlock::MemBlock(const void *data, size_t size) : mMem(0), mSize(0) {
    setData(data, size);
}

MemBlock::MemBlock(const MemBlock& o) : mMem(0), mSize(0) {
    setData(o.mMem, o.mSize);
}

MemBlock::MemBlock(MemBlock&& o) noexcept {
    mMem = o.mMem;
    mSize = o.mSize;
    o.mMem = nullptr;
    o.mSize = 0;
}

void MemBlock::setData(const void *data, size_t size) {
    if (mMem) {
        delete[] mMem;
        mMem = 0;
        mSize = 0;
    }

    if (data && size > 0) {
        mMem = new uint8_t[size]{};
        if (mMem) {
            memcpy(mMem, data, size);
            mSize = size;
        }
    }
}

MemBlock& MemBlock::operator=(const MemBlock& o) {
    if (&o == this) {
        return *this;
    }
    setData(o.mMem, o.mSize);
    return *this;
}

MemBlock& MemBlock::operator=(MemBlock&& o) noexcept {
    if (mMem) {
        delete[] mMem;
    }
    mMem = o.mMem;
    mSize = o.mSize;
    o.mMem = nullptr;
    o.mSize = 0;
    return *this;
}

bool MemBlock::operator==(const MemBlock& o) {
    if (mMem == nullptr && mSize == 0 && o.mMem == nullptr && o.mSize == 0) {
        return true;
    }
    return (mSize == o.mSize && mMem && o.mMem) &&
           (memcmp(mMem, o.mMem, mSize) == 0);
}

void MemBlock::clear() {
    if (mMem) {
        delete[] mMem;
        mMem = nullptr;
    }
    mSize = 0;
}
