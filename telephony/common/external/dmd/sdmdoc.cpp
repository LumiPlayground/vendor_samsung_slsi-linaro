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
 * sdmdoc.cpp
 *
 *  Created on: 2021. 9. 3.
 */
#include "log.h"
#include "sdmdoc.h"
#include "sdmdocenv.h"
#include "systemproperty.h"
#include "timeutils.h"
#include "dmd_test_util.h"

#include <sstream>

/**
 * OnHook
 */
struct OnHook {
    virtual ~OnHook()=default;
    virtual void onHook(void *data, size_t datalen)=0;
};

/////////////////////////////////////////////////////////////////////////////////
struct SdmDocWriter {
    SdmOutputStream *mOutputStream;
    const uint8_t *mData;
    size_t mSize;

    SdmDocWriter(SdmOutputStream *outputStream, const void *data, size_t size) {
        mOutputStream = outputStream;
        mData = (const uint8_t *)data;
        mSize = size;
    }
    virtual ~SdmDocWriter()=default;
    virtual size_t run(OnHook *hook = nullptr)=0;
};


#pragma pack(1)
enum {
    FLAG_START = 0x7F,
    FLAG_END = 0x7E,
};
#define HDLC_FLAGS_SIZE 2

struct HdlcHeader {
    uint8_t start;
    uint16_t length;  // start/end flags are not included.
    uint8_t control;
    uint8_t message[0];
};
struct HdlcTailer {
    uint8_t end;
};
struct MessageHeader {
    uint16_t length;
    uint8_t seq;
    uint8_t ack;
    uint8_t main;
    uint8_t sub;
    uint8_t type;
    uint8_t param[0];
};
struct SdmDocV1FrameHeader {
    uint16_t length;
    uint8_t flag;
    uint8_t type;
};
#define SDM_FLAG 0x39
#define SDM_TYPE_DM 0x7F
#define SDM_TYPE_LOG_INFO 0xFD
#define SDM_DATA_TIMESTAMP_SIZE 6
#pragma pack()

struct SdmDocV1Writer : public SdmDocWriter {
    SdmDocV1Writer(SdmOutputStream *outputStream, const void *data, size_t size) :
        SdmDocWriter(outputStream, data, size) {}
    ~SdmDocV1Writer()=default;
    size_t run(OnHook *hook = nullptr);
    static size_t writeLogInfo(SdmOutputStream *outputStream, const MessageHeader *data, size_t size, uint64_t timestamp);
};

size_t SdmDocV1Writer::run(OnHook *hook/* = nullptr*/) {
    if (mOutputStream == nullptr || mData == nullptr || mSize == 0) {
        return 0;
    }

    size_t count = 0;
    uint64_t timestamp = timeutils::getTimestamp();
    const uint8_t *cur = mData;
    const uint8_t *end = mData + mSize;
    while (cur != end) {
        while (*cur != FLAG_START && cur != end) {
            cur++;
        }

        // not found
        if (cur == end) break;

        HdlcHeader *hdlc = (HdlcHeader *)cur;
        // data chunked
        if (cur + sizeof(HdlcHeader) + sizeof(MessageHeader) >= end) {
            // drop whole data
            LOGE("cur + sizeof(HdlcHeader) + sizeof(MessageHeader) >= end");
            break;
        }

        const uint8_t *tailer = cur + hdlc->length + 1;
        // incorrect HDLC length
        if (*tailer != FLAG_END) {
            // drop whole data
            LOGE("*tailer != FLAG_END");
            break;
        }

        MessageHeader *message = (MessageHeader *)hdlc->message;
        // incorrect Message length
        if ((uint8_t *)message + message->length != tailer) {
            // drop whole data
            LOGE("message + message->length != tailer");
            break;
        }

        count = SdmDocV1Writer::writeLogInfo(mOutputStream, message, message->length, timestamp);

        // hook DM packet
        if (hook) {
            hook->onHook(message, message->length);
        }

        // the next
        cur = tailer + 1;
    }
    return count;
}

// DM Message to SDM LogInfo
size_t SdmDocV1Writer::writeLogInfo(SdmOutputStream *outputStream,
        const MessageHeader *message, size_t size, uint64_t timestamp) {
    size_t ret = 0;
    if (outputStream && message && size > 0 && message->length <= size) {
        size_t paramsSize = message->length - sizeof(message->length);
        uint8_t *params = (uint8_t *)&message->seq;
        // write to the output stream
        // SDM frame header
        //   - length (2 bytes) - header + data. a size of length field is not included.
        //   - flag (1 byte) - 0x39(fixed)
        //   - type (1 byte) - 0x7F (fixed)
        // data (variable)
        //   - timestamp (6 bytes)
        //   - DM payload (variable)
        //     seq(1 byte)
        //     ack(1 byte)
        //     main(1 byte)
        //     sub(1 byte)
        //     type(1 byte)
        //     param(variable)
        SdmDocV1FrameHeader sdm = {
            (uint16_t)(sizeof(uint8_t) * 2  // flag + type
                +  SDM_DATA_TIMESTAMP_SIZE  // timestamp 6 bytes
                + paramsSize                // DM message. a size of length filed is not included.
            ),
            SDM_FLAG, SDM_TYPE_DM,
        };
        ret += outputStream->write(&sdm, sizeof(sdm));
        ret += outputStream->write(&timestamp, SDM_DATA_TIMESTAMP_SIZE);
        ret += outputStream->write(params, paramsSize);
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////

/**
 * SdmDocV1
 */

// a common data for sdm type 1.
MemBlock SdmDocV1::sVersion;

bool SdmDocV1::hasVersion() {
    return sVersion.hasData();
}

SdmDocV1::SdmDocV1() : SdmDocV1(nullptr) {
}

SdmDocV1::SdmDocV1(SdmOutputStream *outputStream) : BaseSdmDoc(outputStream) {
}

SdmDocV1::~SdmDocV1() {
}

std::string SdmDocV1::toString() const {
    std::stringstream ss;
    ss << "SdmDocV1{";
    ss << "filepath=" << mFilepath;
    ss << ", hasVersion=" << SdmDocV1::hasVersion();
    ss << "}";
    return ss.str();
}

int SdmDocV1::write(const void *data, size_t datalen) {
    if (mOutputStream == nullptr) {
        return -1;
    }

    if (data == nullptr || datalen == 0) {
        return 0;
    }

    struct OnHookImpl : public OnHook {
        ~OnHookImpl()=default;
        void onHook(void *data, size_t datalen) {
            if (data && datalen > 0) {
                MessageHeader *message = (MessageHeader *)data;
                if ((message->main == 0xA0 || message->main == 0xA1) &&
                    message->sub == 0x00 && message->type == 0x01) {
                    LOGD("onHook version info");

                    uint64_t timestamp = timeutils::getTimestamp();
                    MemoryBufferOutputStream out;
                    if (SdmDocV1Writer::writeLogInfo(&out, message,
                            message->length, timestamp) > 0) {
                        SdmDocV1::sVersion = std::move(MemBlock(out.buf(), out.size()));
                    }
                }
            }
        }
    } hook;
    OnHook *hookPtr = nullptr;
    if (!SdmDocV1::hasVersion()) {
        hookPtr = &hook;
    }

    SdmDocV1Writer out(mOutputStream, data, datalen);
    size_t ret = out.run(hookPtr);
    return (ret > 0) ? datalen : 0;
}

void SdmDocV1::createHeader() {
    if (mOutputStream) {
        std::stringstream ss;
        ss << SdmDocEnv::getBaseDir() << ".sbuff_header.sdm";
        FileInputStream in;
        bool testHeader = SystemProperty::GetInt("persist.vendor.config.sdm_local_header") == 1;
        if (!testHeader && in.open(ss.str())) {
            char buf[2000] {};
            int ret = 0;
            while ((ret = in.read(buf, sizeof(buf))) > 0) {
                mOutputStream->write(buf, ret);
            }
            in.close();

            if (SdmDocV1::hasVersion()) {
                mOutputStream->write(sVersion.ptr(), sVersion.size());
            }
        } else {
            // open failure
            // SDM type1 doc header must be generated by SilentLogging app.
            LOGV("Using testHeader or SDM header not existed. (%d)", testHeader);
            createLocalHeader();
        }
    }
}

void SdmDocV1::createLocalHeader() {
    if (mOutputStream) {
        std::string tzid = timeutils::getTimezone();
        uint64_t timestamp = timeutils::getTimestamp();
        uint8_t version[4] =  { 0x02, };
        std::string appVersion = "Silent Log Ver.3 Sep. 14, 2021";
        size_t logInfoLength = sizeof(uint16_t) // Length of Log version
                + sizeof(version)
                + sizeof(uint16_t)  // Length of App version
                + appVersion.length() // App version
                + sizeof(uint16_t)  // Length of Timestamp
                + sizeof(uint64_t)  // Timestamp
                + sizeof(uint16_t)  // Length of Time Zone ID
                + tzid.length();    // Time Zone ID

        // Log Info
        MemoryBufferOutputStream out;
        DataWriter dr(&out);
        dr << (uint16_t)(logInfoLength + 2) << (uint8_t)SDM_FLAG << (uint8_t)SDM_TYPE_LOG_INFO;
        dr << (uint16_t)sizeof(version) << *(uint32_t *)version << (uint16_t)appVersion.length() << appVersion;
        dr << (uint16_t)sizeof(timestamp) << timestamp << (uint16_t)tzid.length();
        if (tzid.length() > 0) {
            dr << tzid;
        }
        mOutputStream->write(out.buf(), out.size());
    }
}

SdmOutputStream *SdmDocV1::createOutputStream() {
    // RawFileOutputStream will be released by BufferedOutputStream
    BufferedOutputStream *bos = new BufferedOutputStream(new RawFileOutputStream());
    return bos;
}

SdmDocV1 *SdmDocV1::newInstance() {
    return new SdmDocV1();
}

/**
 * SdmDocV2HeaderBuilder
 */
SdmDocV2HeaderBuilder::SdmDocV2HeaderBuilder()
    : mFlag(false) {
    mHeader = {};
    mLogInfo = {};
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::tag(const std::string& tag) {
    if (tag.length() > MAX_TAG_SIZE) {
        mHeader.tag = tag.substr(0, MAX_TAG_SIZE);
    } else {
        mHeader.tag = tag;
        mHeader.tag += std::string("00000").substr(0, MAX_TAG_SIZE - tag.length());
    }
    mFlag = true;
    return *this;
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::version(const std::string& version) {
    if (version.length() > MAX_VERSION_SIZE) {
        mHeader.version = version.substr(0, MAX_VERSION_SIZE);
    } else {
        mHeader.version = version;
        mHeader.version += std::string("0000").substr(0, MAX_VERSION_SIZE - version.length());
    }
    mFlag = true;
    return *this;
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::uid(uint32_t uid, uint32_t seq) {
    mHeader.logset = uid;
    mHeader.seq = seq;
    mFlag = true;
    return *this;
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::codebook(const std::string& codebook) {
    if (codebook.length() > MAX_CODEBOOK_SIZE) {
        mHeader.codebook = codebook.substr(0, MAX_CODEBOOK_SIZE);
    } else {
        mHeader.codebook = codebook;
        mHeader.codebook += std::string("0000").substr(0, MAX_CODEBOOK_SIZE - codebook.length());
    }
    mFlag = true;
    return *this;
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::timestamp(uint64_t timestamp) {
    mLogInfo.timestamp = timestamp;
    mFlag = true;
    return *this;
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::tzid(const std::string& tzid) {
    mLogInfo.tzid = tzid;
    mFlag = true;
    return *this;
}

SdmDocV2HeaderBuilder& SdmDocV2HeaderBuilder::tzoffset(uint64_t tzoffset) {
    mLogInfo.tzoffset = tzoffset;
    mFlag = true;
    return *this;
}

void SdmDocV2HeaderBuilder::build() {
    if (mFlag) {
        buildInternal();
    }
}

void *SdmDocV2HeaderBuilder::data() {
    if (mFlag) {
        buildInternal();
    }
    return mData.ptr();
}

size_t SdmDocV2HeaderBuilder::size() {
    if (mFlag) {
        buildInternal();
    }
    return mData.size();
}

void SdmDocV2HeaderBuilder::buildInternal() {
    if (mFlag) {
        size_t logInfoLength = 2 // Length of Timestamp
                + sizeof(uint64_t) // Timestamp
                + 2 // Length of Time Zone ID
                + mLogInfo.tzid.length() // Time Zone ID
                + 2 // Length of CP binary path
                + 2 // Time Zone Offset length
                + sizeof(uint64_t); // Time Zone Offset

        size_t totalLen = MAX_TAG_SIZE     // Tag
                + sizeof(uint32_t)  // a total length: 4 bytes
                + MAX_VERSION_SIZE  // Version
                + sizeof(uint32_t)  // Log set ID: 4 bytes
                + sizeof(uint32_t)  // Sequence ID: 4 bytes
                + sizeof(uint32_t)  // Codebook version: 4 bytes
                + MAX_RESERVED_SIZE // reserved: 4 bytes
                + 2                 // Log Info Length
                + logInfoLength;

        MemoryBufferOutputStream out;
        DataWriter dr(&out);
        dr << mHeader.tag << (uint32_t)totalLen << mHeader.version
                << mHeader.logset << mHeader.seq << mHeader.codebook;
        char reserved[MAX_RESERVED_SIZE] = {};
        dr.append(reserved, sizeof(reserved));

        // Log Info
        dr << (uint16_t)logInfoLength << (uint16_t)sizeof(uint64_t) << mLogInfo.timestamp;
        dr << (uint16_t)mLogInfo.tzid.length();
        if (mLogInfo.tzid.length() > 0) {
            dr.append(mLogInfo.tzid.c_str(), mLogInfo.tzid.length());
        }
        dr << (uint16_t)0;
        dr << (uint16_t)sizeof(uint64_t) << mLogInfo.tzoffset;

        mFlag = false;
        mData.setData(out.buf(), out.size());
    }
}

/**
 * SdmDocV2 - interface
 */
SdmDocV2::SdmDocV2() : SdmDocV2(nullptr) {
}

SdmDocV2::SdmDocV2(SdmOutputStream *outputStream) : BaseSdmDoc(outputStream), mUid(0), mSeq(0) {

}

SdmDocV2::~SdmDocV2() {
}

int SdmDocV2::write(const void *data, size_t datalen) {
    // SDM V2 : dump without additional processing of DM packets
    return BaseSdmDoc::write(data, datalen);
}

std::string SdmDocV2::toString() const {
    std::stringstream ss;
    ss << "SdmDocV2{";
    ss << "filepath=" << mFilepath;
    ss << ", mUid=" << mUid;
    ss << ", mSeq=" << mSeq;
    ss << "}";
    return ss.str();
}

SdmOutputStream *SdmDocV2::createOutputStream() {
    SdmOutputStream *oos = new FileOutputStream();
    return oos;
}

#define SDM2_TAG "SDM2"

void SdmDocV2::createHeader() {
    // fill header info and write into output stream
    std::string tag = "SDM2:";
    std::string version = "0000";
    std::string codebook = "0000";
    std::string tzid = timeutils::getTimezone();
    uint64_t timestamp = timeutils::getTimestamp();
    uint64_t tzoffset = timeutils::getTimezoneOffset();

    SdmDocV2HeaderBuilder builder;
    builder.tag(tag)
        .version(version)
        .uid(1, 0)
        .codebook(codebook)
        .timestamp(timestamp)
        .tzid(tzid)
        .tzoffset(tzoffset)
        .build();
    void *data = builder.data();
    size_t datalen = builder.size();
    if (mOutputStream) {
        if (data && datalen > 0) {
            int ret = mOutputStream->write(data, datalen);
            if (ret < 0 || (size_t)ret != datalen) {
                // write failure
            }
        }
    }
}

SdmDocV2 *SdmDocV2::newInstance(uint32_t uid, uint32_t seq) {
    SdmDocV2 *doc = new SdmDocV2();
    if (doc) {
        doc->mUid = uid;
        doc->mSeq = seq;
    }
    return doc;
}
