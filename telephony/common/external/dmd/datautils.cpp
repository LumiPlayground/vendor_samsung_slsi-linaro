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
 * datautils.cpp
 *
 *  Created on: 2021. 9. 6.
 *      Author: sungwoo48.choi
 */
#include "datautils.h"

/**
 * DataBuilder
 */
DataWriter::DataWriter(SdmOutputStream *outputStream) : mOutputStream(outputStream) {
}

DataWriter::~DataWriter() {
}

int DataWriter::writeInternal(const void *data, size_t datalen) {
    int ret = 0;
    if (mOutputStream) {
        ret = mOutputStream->write(data, datalen);
    }
    return ret;
}

DataWriter& DataWriter::append(uint8_t b) {
    writeInternal(&b, sizeof(b));
    return *this;
}

DataWriter& DataWriter::append(int8_t b) {
    writeInternal(&b, sizeof(b));
    return *this;
}

DataWriter& DataWriter::append(uint16_t val) {
    writeInternal(&val, sizeof(val));
    return *this;
}

DataWriter& DataWriter::append(int16_t val) {
    writeInternal(&val, sizeof(val));
    return *this;
}

DataWriter& DataWriter::append(uint32_t val) {
    writeInternal(&val, sizeof(val));
    return *this;
}

DataWriter& DataWriter::append(int32_t val) {
    writeInternal(&val, sizeof(val));
    return *this;
}

DataWriter& DataWriter::DataWriter::append(uint64_t val) {
    writeInternal(&val, sizeof(val));
    return *this;
}

DataWriter& DataWriter::append(int64_t val) {
    writeInternal(&val, sizeof(val));
    return *this;
}

DataWriter& DataWriter::append(const void *data, size_t datalen) {
    writeInternal(data, datalen);
    return *this;
}

DataWriter& DataWriter::append(const std::string& str) {
    return append(str.c_str(), str.length());
}

DataWriter& operator<< (DataWriter& builder, uint8_t b) {
    return builder.append(b);
}

DataWriter& operator<< (DataWriter& builder, int8_t b) {
    return builder.append(b);
}

DataWriter& operator<< (DataWriter& builder, uint16_t val) {
    return builder.append(val);
}

DataWriter& operator<< (DataWriter& builder, int16_t val) {
    return builder.append(val);
}

DataWriter& operator<< (DataWriter& builder, uint32_t val) {
    return builder.append(val);
}

DataWriter& operator<< (DataWriter& builder, int32_t val) {
    return builder.append(val);
}

DataWriter& operator<< (DataWriter& builder, uint64_t val) {
    return builder.append(val);
}

DataWriter& operator<< (DataWriter& builder, int64_t val) {
    return builder.append(val);
}

DataWriter& operator<< (DataWriter& builder, const std::string& str) {
    return builder.append(str);
}

/**
 * DataReader
 */
DataReader::DataReader(SdmInputStream *inputStream) : mInputStream(inputStream) {

}

DataReader::~DataReader() {
}

int DataReader::read(uint8_t& b) {
    int ret = -1;
    if (mInputStream) {
        char buf[1] {};
        ret = mInputStream->read(buf, sizeof(buf));
        if (ret == 1) {
            b = (uint8_t)buf[0];
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(int8_t& b) {
    int ret = -1;
    if (mInputStream) {
        char buf[1] {};
        ret = mInputStream->read(buf, sizeof(buf));
        if (ret == 1) {
            b = (int8_t)buf[0];
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(uint16_t& s) {
    int ret = -1;
    if (mInputStream) {
        char buf[8] {};
        ret = mInputStream->read(buf, sizeof(uint16_t));
        if (ret == sizeof(uint16_t)) {
            memcpy(&s, buf, sizeof(uint16_t));
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(int16_t& s) {
    int ret = -1;
    if (mInputStream) {
        char buf[8] {};
        ret = mInputStream->read(buf, sizeof(int16_t));
        if (ret == sizeof(int16_t)) {
            memcpy(&s, buf, sizeof(int16_t));
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(uint32_t& val) {
    int ret = -1;
    if (mInputStream) {
        char buf[8] {};
        ret = mInputStream->read(buf, sizeof(uint32_t));
        if (ret == sizeof(uint32_t)) {
            memcpy(&val, buf, sizeof(uint32_t));
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(int32_t& val) {
    int ret = -1;
    if (mInputStream) {
        char buf[8] {};
        ret = mInputStream->read(buf, sizeof(int32_t));
        if (ret == sizeof(int32_t)) {
            memcpy(&val, buf, sizeof(int32_t));
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(uint64_t& val) {
    int ret = -1;
    if (mInputStream) {
        char buf[8] {};
        ret = mInputStream->read(buf, sizeof(uint64_t));
        if (ret == sizeof(uint64_t)) {
            memcpy(&val, buf, sizeof(uint64_t));
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(int64_t& val) {
    int ret = -1;
    if (mInputStream) {
        char buf[8] {};
        ret = mInputStream->read(buf, sizeof(int64_t));
        if (ret == sizeof(int64_t)) {
            memcpy(&val, buf, sizeof(int64_t));
        } else {
            ret = -1;
        }
    }
    return ret;
}

int DataReader::read(void *buf, size_t readSize) {
    int ret = -1;
    if (mInputStream && buf && readSize > 0) {
        ret = mInputStream->read(buf, readSize);
    }
    return ret;
}
