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
 * sdmstream.cpp
 *
 *  Created on: 2021. 9. 3.
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "sdmstream.h"
#include "dmd_test_util.h"
#include "base/log.h"


/**
 * FileOutputStream
 */
FileOutputStream::FileOutputStream() : mFile(0) {

}

FileOutputStream::FileOutputStream(const std::string& filepath) : mFile(0), mFilepath(filepath) {
    onOpen();
}

FileOutputStream::~FileOutputStream() {
    onClose();
}

bool FileOutputStream::open(const std::string& filepath) {
    mFilepath = filepath;
    onOpen();
    return (mFile != nullptr);
}

int FileOutputStream::write(const void *data, size_t datalen) {
    size_t ret = 0;
    if (mFile) {
        ret = fwrite(data, 1, datalen, mFile);
        flush();
    }
    return ret;
}

size_t FileOutputStream::tell() {
    size_t ret = 0;
    if (mFile) {
        ret = ftell(mFile);
    }
    return ret;
}

void FileOutputStream::flush() {
    if (mFile) {
        fflush(mFile);
    }
}

void FileOutputStream::close() {
    onClose();
}

void FileOutputStream::onOpen() {
    if (mFilepath.length() > 0) {
        mFile = fopen(mFilepath.c_str(), "we");
    }
}

void FileOutputStream::onClose() {
    if (mFile) {
        fclose(mFile);
        mFile = nullptr;
    }
}

/**
 * FileInputStream
 */
FileInputStream::FileInputStream() : mFile(0) {

}

FileInputStream::FileInputStream(const std::string& filepath) : mFile(0), mFilepath(filepath) {
    onOpen();
}

FileInputStream::~FileInputStream() {
    onClose();
}

bool FileInputStream::open(const std::string& filepath) {
    mFilepath = filepath;
    onOpen();
    return (mFile != nullptr);
}


void FileInputStream::close() {
    onClose();
}

int FileInputStream::read(void *buf, size_t size) {
    if (buf == nullptr || size == 0) {
        return -1;
    }

    int ret = 0;
    if (mFile != nullptr) {
        if (feof(mFile) > 0) {
            ret = -1;
        } else {
            ret = fread(buf, 1, size, mFile);
        }
    }
    return ret;
}

void FileInputStream::onOpen() {
    if (mFilepath.length() > 0) {
        mFile = fopen(mFilepath.c_str(), "re");
    }
}

void FileInputStream::onClose() {
    if (mFile) {
        fclose(mFile);
        mFile = nullptr;
    }
}

enum {
    MEM_BUF_SIZE = 2048,
    MAX_MEM_BUF_SIZE = (2048 * MEM_BUF_SIZE),
};

/**
 * RawFileOutputStream
 */
RawFileOutputStream::RawFileOutputStream() : mFd(-1), mSize(0) {}

RawFileOutputStream::RawFileOutputStream(const std::string& filepath)
    : mFd(-1), mFilepath(filepath), mSize(0)  {
    onOpen();
}

RawFileOutputStream::~RawFileOutputStream() {
    onClose();
}

bool RawFileOutputStream::open(const std::string& filepath) {
    mFilepath = filepath;
    onOpen();
    return (mFd != -1);
}

void RawFileOutputStream::close() {
    onClose();
}

int RawFileOutputStream::write(const void *data, size_t datalen) {
    int ret = 0;
    if (mFd != -1) {
        ret = ::write(mFd, data, datalen);
        if (ret < 0) {
            LOGE("RawFileOutputStream write failure: %d", errno);
        } else {
            flush();
            mSize += ret;
        }
    }
    return ret;
}

void RawFileOutputStream::flush() {
    if (mFd != -1) {
        fsync(mFd);
    }
}

size_t RawFileOutputStream::tell() {
    return mSize;
}

void RawFileOutputStream::onOpen() {
    mSize = 0;
    if (mFilepath.length() > 0) {
        mFd = ::open(mFilepath.c_str(), (O_CREAT | O_WRONLY | O_TRUNC | O_SYNC | O_CLOEXEC),
                                (S_IRWXU | S_IRGRP | S_IROTH));
        if (mFd < 0) {
            LOGE("RawFileOutputStream open failure: %d", errno);
        }
    }
}

void RawFileOutputStream::onClose() {
    if (mFd != -1) {
        fsync(mFd);
        ::close(mFd);
        mFd = -1;
    }
    mSize = 0;
}

/**
 * MemoryBufferOutputStream
 */
MemoryBufferOutputStream::MemoryBufferOutputStream() : MemoryBufferOutputStream(MEM_BUF_SIZE) {

}

MemoryBufferOutputStream::MemoryBufferOutputStream(size_t capacity) :
        mMem(0), mPos(0), mCapacity(0) {
    resize(capacity);
}

MemoryBufferOutputStream::~MemoryBufferOutputStream() {
    if (mMem) {
        delete[] mMem;
    }
}

bool MemoryBufferOutputStream::expandIfNeeded(size_t datalen) {
    while (mPos + datalen >= mCapacity) {
        if (!expand()) {
            return false;
        }
    }
    return true;
}

bool MemoryBufferOutputStream::expand() {
    // already max size
    if (mCapacity == MAX_MEM_BUF_SIZE) {
        return false;
    }

    size_t capacity = ((mCapacity + MEM_BUF_SIZE) / MEM_BUF_SIZE) * MEM_BUF_SIZE * 2;
    if (capacity > MAX_MEM_BUF_SIZE) {
        capacity = MAX_MEM_BUF_SIZE;
    }

    bool needToCopy = false;
    if (mMem && mPos > 0) {
        needToCopy = true;
    }

    uint8_t *p = new uint8_t[capacity]{};
    if (p) {
        if (needToCopy) {
            memcpy(p, mMem, mPos);
            delete[] mMem;
        }
        mMem = p;
        mCapacity = capacity;
    }

    return true;
}

bool MemoryBufferOutputStream::open(const std::string& /*filepath*/) {
    return mMem != nullptr;
}

void MemoryBufferOutputStream::close() {}

int MemoryBufferOutputStream::write(const void *data, size_t datalen) {
    int ret = 0;
    if (data && datalen > 0) {
        if (expandIfNeeded(datalen)) {
            memcpy(mMem + mPos, data, datalen);
            mPos += datalen;
            ret = datalen;
        }
    }
    return ret;
}

void MemoryBufferOutputStream::reset() {
    if (mMem) {
        memset(mMem, 0, mCapacity);
        mPos = 0;
    }
}

void MemoryBufferOutputStream::resize(size_t size) {
    if (size > MAX_MEM_BUF_SIZE) {
        size = MAX_MEM_BUF_SIZE;
    }

    if (mMem) {
        delete[] mMem;
        mCapacity = 0;
    }

    mMem = new uint8_t[size] {};
    if (mMem) {
        mCapacity = size;
    }
    mPos = 0;
}

/**
 * MemoryBufferInputStream
 */
MemoryBufferInputStream::MemoryBufferInputStream() : MemoryBufferInputStream(0, 0) {

}

MemoryBufferInputStream::MemoryBufferInputStream(void *mem, size_t size) :
    mMem(0), mSize(0), mPos(0) {
    setBuffer(mem, size);
}

MemoryBufferInputStream::~MemoryBufferInputStream() {
    if (mMem) {
        delete[] mMem;
    }
}

void MemoryBufferInputStream::setBuffer(void *mem, size_t size) {
    if (mMem) {
        delete[] mMem;
        mMem = 0;
        mSize = 0;
        mPos = 0;
    }

    if (mem != nullptr && size > 0) {
        mMem = new uint8_t[size] {};
        if (mMem) {
            memcpy(mMem, mem, size);
            mSize = size;
            mPos = 0;
        }
    }
}

bool MemoryBufferInputStream::open(const std::string& /*filepath*/) {
    if (mMem && mSize > 0) {
        return true;
    }
    return false;
}

int MemoryBufferInputStream::read(void *buf, size_t size) {
    int ret = 0;
    if (mMem && mPos < mSize && buf && size > 0) {
        ret = size;
        if (mPos + size > mSize) {
            ret = mSize - mPos;
        }
        memcpy(buf, mMem + mPos, ret);
        mPos += ret;
    }
    return ret;
}

/**
 * BufferedOutputStream
 */
#define DEFAULT_BUF_SIZE (2 * 1024 * 1024)
#define MAX_BUF_SIZE (DEFAULT_BUF_SIZE * 4)
BufferedOutputStream::BufferedOutputStream(SdmOutputStream *outputStream) :
        BufferedOutputStream(outputStream, DEFAULT_BUF_SIZE) {
}

BufferedOutputStream::BufferedOutputStream(SdmOutputStream *outputStream, size_t capacity) :
    mOutputStream(outputStream), mCapacity(capacity) {
    if (mCapacity > MAX_BUF_SIZE) {
        mCapacity = MAX_BUF_SIZE;
    }
    mBuf.resize(mCapacity);
}

BufferedOutputStream::~BufferedOutputStream() {
    if (mOutputStream) {
        mOutputStream->close();
        delete mOutputStream;
    }
}

bool BufferedOutputStream::open(const std::string& filepath) {
    bool ret = false;
    if (mOutputStream) {
        ret = mOutputStream->open(filepath);
    }
    return ret;
}

void BufferedOutputStream::close() {
    if (mOutputStream) {
        if (mBuf.size() > 0) {
            mOutputStream->write(mBuf.buf(), mBuf.size());
        }
        mOutputStream->close();
    }
}

int BufferedOutputStream::write(const void *data, size_t datalen) {
    int ret = 0;
    if (data && datalen > 0) {
        if (mOutputStream) {
            ret = mBuf.write(data, datalen);
            // flush
            if (ret > 0 && mBuf.size() > mCapacity) {
                mOutputStream->write(mBuf.buf(), mBuf.size());
                mBuf.reset();
            }
        }
    }
    return ret;
}

size_t BufferedOutputStream::tell() {
    size_t ret = 0;
    if (mOutputStream) {
        ret = mOutputStream->tell() + mBuf.size();
    }
    return ret;
}
