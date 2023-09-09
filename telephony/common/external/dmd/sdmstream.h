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
 * sdmstream.h
 *
 *  Created on: 2021. 9. 3.
 *      Author: sungwoo48.choi
 */

#ifndef __SDM_STREAM_H__
#define __SDM_STREAM_H__

#include <string>

struct SdmOutputStream {
    SdmOutputStream() = default;
    virtual ~SdmOutputStream() = default;
    virtual bool open(const std::string& filepath)=0;
    virtual void close()=0;
    virtual int write(const void *data, size_t datalen)=0;
    virtual size_t tell()=0;
};

struct SdmInputStream {
    SdmInputStream() = default;
    virtual ~SdmInputStream() = default;
    virtual bool open(const std::string& filepath)=0;
    virtual void close()=0;
    virtual int read(void *buf, size_t size)=0;
};

/**
 * FileOutputStream
 */
struct FileOutputStream : public SdmOutputStream {
protected:
    FILE *mFile;
    std::string mFilepath;

public:
    FileOutputStream();
    FileOutputStream(const FileOutputStream &) = delete;
    FileOutputStream& operator=(FileOutputStream const&) = delete;
    FileOutputStream(const std::string& filepath);
    virtual ~FileOutputStream();
    bool open(const std::string& filepath);
    void close();
    int write(const void *data, size_t datalen);
    size_t tell();
    void flush();

protected:
    void onOpen();
    void onClose();
};

/**
 * FileInputStream
 */
struct FileInputStream : public SdmInputStream {
protected:
    FILE *mFile;
    std::string mFilepath;

public:
    FileInputStream();
    FileInputStream(const FileInputStream &) = delete;
    FileInputStream& operator=(FileInputStream const&) = delete;
    FileInputStream(const std::string& filepath);
    virtual ~FileInputStream();
    bool open(const std::string& filepath);
    void close();
    int read(void *buf, size_t size);

protected:
    void onOpen();
    void onClose();
};

/**
 * RawFileOutputStream
 */
struct RawFileOutputStream : public SdmOutputStream {
protected:
    int mFd;
    std::string mFilepath;
    size_t mSize;

public:
    RawFileOutputStream();
    RawFileOutputStream(const std::string& filepath);
    ~RawFileOutputStream();
    bool open(const std::string& filepath);
    void close();
    int write(const void *data, size_t datalen);
    size_t tell();
private:
    void flush();

protected:
    void onOpen();
    void onClose();
};

/**
 * MemoryBufferOutputStream
 */
struct MemoryBufferOutputStream : public SdmOutputStream {
private:
    uint8_t *mMem;
    size_t mPos;
    size_t mCapacity;
public:
    MemoryBufferOutputStream();
    MemoryBufferOutputStream(const MemoryBufferOutputStream &) = delete;
    MemoryBufferOutputStream& operator=(MemoryBufferOutputStream const&) = delete;
    MemoryBufferOutputStream(size_t capacity);
    virtual ~MemoryBufferOutputStream();
    bool open(const std::string& filepath);
    void close();
    int write(const void *data, size_t datalen);
    size_t tell() { return size(); }
    void *buf() { return mMem; }
    void *cur() { return mMem + mPos; }
    size_t size() { return mPos; }
    size_t capacity() { return mCapacity; }
    void reset();
    void resize(size_t size);

private:
    bool expandIfNeeded(size_t datalen);
    bool expand();
};

/**
 * MemoryBufferInputStream
 */
struct MemoryBufferInputStream : public SdmInputStream {
private:
    uint8_t *mMem;
    size_t mSize;
    size_t mPos;

public:
    MemoryBufferInputStream();
    MemoryBufferInputStream(const MemoryBufferInputStream &) = delete;
    MemoryBufferInputStream& operator=(MemoryBufferInputStream const&) = delete;
    MemoryBufferInputStream(void *mem, size_t size);
    ~MemoryBufferInputStream();

    void setBuffer(void *mem, size_t size);
    bool open(const std::string& filepath);
    void close() {}
    int read(void *buf, size_t size);
};

/**
 * BufferedOutputStream
 */
struct BufferedOutputStream : public SdmOutputStream {
private:
    SdmOutputStream *mOutputStream;
    MemoryBufferOutputStream mBuf;
    size_t mCapacity;

public:
    BufferedOutputStream(SdmOutputStream *outputStream);
    BufferedOutputStream(const BufferedOutputStream &) = delete;
    BufferedOutputStream& operator=(BufferedOutputStream const&) = delete;
    BufferedOutputStream(SdmOutputStream *outputStream, size_t capacity);
    ~BufferedOutputStream();
    bool open(const std::string& filepath);
    void close();
    int write(const void *data, size_t datalen);
    size_t tell();
};

#endif /* __SDM_STREAM_H__ */
