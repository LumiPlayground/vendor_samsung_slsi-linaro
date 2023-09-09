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
 * sdmdoc.h
 *
 *  Created on: 2021. 9. 3.
 */

#ifndef __SDM_DOC_H__
#define __SDM_DOC_H__

#include "datautils.h"
#include "memblock.h"
#include "sdmstream.h"

#include <string>

enum {
    SDM_TYPE1,
    SDM_TYPE2,
};

#define SDM_TYPE1_EXT   ".sdm"
#define SDM_TYPE2_EXT   ".sdm2"

class SdmDoc {
protected:
    std::string mFilepath;

public:
    SdmDoc() : mFilepath("") {}
    virtual ~SdmDoc() = default;
public:
    bool create(const std::string& filepath) {
        mFilepath = filepath;
        return onCreate();
    }
    void close() {
        onClose();
    }
    const std::string& getFilepath() const { return mFilepath; }
    virtual int write(const void *data, size_t datalen)=0;
    virtual size_t size()=0;
    virtual std::string toString() const { return "SdmDoc"; }

protected:
    virtual bool onCreate()=0;
    virtual void onClose()=0;
};

/**
 * BaseSdmDoc (abstract)
 */
class BaseSdmDoc : public SdmDoc {
protected:
    SdmOutputStream *mOutputStream;
    bool mOwn;
public:
    BaseSdmDoc() : BaseSdmDoc(nullptr) {}
    BaseSdmDoc(SdmOutputStream *outputStream) : mOutputStream(0), mOwn(false) {
        setOutputStream(outputStream);
    }
    BaseSdmDoc(const BaseSdmDoc &) = delete;
    BaseSdmDoc& operator=(BaseSdmDoc const&) = delete;
    virtual ~BaseSdmDoc() {
        setOutputStream(nullptr);
    }
    virtual int write(const void *data, size_t datalen) {
        int ret = -1;
        if (mOutputStream) {
            ret = mOutputStream->write(data, datalen);
        }
        return ret;
    }
    virtual size_t size() {
        if (mOutputStream)
            return mOutputStream->tell();
        return 0;
    }
    void setOutputStream(SdmOutputStream *outputStream, bool own = false) {
        if (mOutputStream) {
            mOutputStream->close();

            if (mOwn) {
                delete mOutputStream;
                mOutputStream = nullptr;
            }
        }
        mOutputStream = outputStream;
        mOwn = own;
    }
protected:
    bool onCreate() {
        if (mOutputStream == nullptr || mOwn) {
            setOutputStream(createOutputStream(), true);
        }

        if (mOutputStream) {
            if (mOutputStream->open(mFilepath)) {
                createHeader();
                return true;
            } else {
                setOutputStream(nullptr);
            }
        }
        return false;
    }
    void onClose() {
        setOutputStream(nullptr);
    }
    virtual SdmOutputStream *createOutputStream() { return nullptr; }
    virtual void createHeader() {}
};

/**
 * SdmDocV1
 */
class SdmDocV1 : public BaseSdmDoc {
public:
    SdmDocV1();
    SdmDocV1(SdmOutputStream *outputStream);
    virtual ~SdmDocV1();
    int write(const void *data, size_t datalen);
    std::string toString() const;
protected:
    SdmOutputStream *createOutputStream();
    void createHeader();
    void createLocalHeader();

private:
    static MemBlock sVersion;
    static bool hasVersion();

public:
    static SdmDocV1 *newInstance();
};

struct SdmV2Header{
    std::string tag;
    std::string version;
    uint32_t logset;
    uint32_t seq;
    std::string codebook;
};

struct SdmV2HeaderLogInfo {
    uint64_t timestamp;
    std::string tzid;
    uint64_t tzoffset;
};

struct SdmDocV2HeaderBuilder {
    enum {
        MAX_TAG_SIZE = 5,
        MAX_VERSION_SIZE = 4,
        MAX_CODEBOOK_SIZE = 4,
        MAX_RESERVED_SIZE = 4,
    };
private:
    SdmV2Header mHeader;
    SdmV2HeaderLogInfo mLogInfo;
    bool mFlag;
    MemBlock mData;

public:
    SdmDocV2HeaderBuilder();
    ~SdmDocV2HeaderBuilder() = default;

    SdmDocV2HeaderBuilder& tag(const std::string& tag);
    SdmDocV2HeaderBuilder& version(const std::string& version);
    SdmDocV2HeaderBuilder& uid(uint32_t uid, uint32_t seq);
    SdmDocV2HeaderBuilder& codebook(const std::string& codebook);
    SdmDocV2HeaderBuilder& timestamp(uint64_t timestamp);
    SdmDocV2HeaderBuilder& tzid(const std::string& tzid);
    SdmDocV2HeaderBuilder& tzoffset(uint64_t tzoffset);
    void build();
    void *data();
    size_t size();
private:
    void buildInternal();
};

class SdmDocV2 : public BaseSdmDoc {
protected:
    uint32_t mUid;
    uint32_t mSeq;
public:
    SdmDocV2();
    SdmDocV2(SdmOutputStream *outputStream);
    virtual ~SdmDocV2();

public:
    int write(const void *data, size_t datalen);
    virtual std::string toString() const;
    uint32_t uid() { return mUid; }
    uint32_t seq() { return mSeq; }
protected:
    SdmOutputStream *createOutputStream();
    void createHeader();

public:
    static SdmDocV2 *newInstance(uint32_t uid, uint32_t seq);
};

#endif /* __SDM_DOC_H__ */
