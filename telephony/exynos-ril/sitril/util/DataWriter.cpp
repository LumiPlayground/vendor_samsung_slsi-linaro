/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */

#include "DataWriter.h"
#include <string>

DataWriter::DataWriter() {
}

DataWriter::DataWriter(const DataWriter & /* orig */) {
}

DataWriter::~DataWriter() {
}

DataWriter & DataWriter::writeByte(UINT8 value) {
    mBuffer.push_back(value);
    return *this;
}

DataWriter & DataWriter::writeShort(UINT16 value) {
    mBuffer.push_back(value & 0xFF);
    mBuffer.push_back((value >> 8) & 0xFF);
    return *this;
}

DataWriter & DataWriter::writeInt(UINT32 value) {
    mBuffer.push_back(value & 0xFF);
    mBuffer.push_back((value >> 8) & 0xFF);
    mBuffer.push_back((value >> 16) & 0xFF);
    mBuffer.push_back((value >> 24) & 0xFF);
    return *this;
}

DataWriter & DataWriter::writeInt64(ULONG value) {
    mBuffer.push_back(value & 0xFF);
    mBuffer.push_back((value >> 8) & 0xFF);
    mBuffer.push_back((value >> 16) & 0xFF);
    mBuffer.push_back((value >> 24) & 0xFF);
    mBuffer.push_back((value >> 32) & 0xFF);
    mBuffer.push_back((value >> 40) & 0xFF);
    mBuffer.push_back((value >> 48) & 0xFF);
    mBuffer.push_back((value >> 56) & 0xFF);
    return *this;
}

DataWriter & DataWriter::writeBytes(UINT8 * value, int length) {
    for (int i = 0; i < length; i++) {
        mBuffer.push_back(value[i]);
    }
    return *this;
}

unsigned int DataWriter::length() {
    return mBuffer.size();
}

void DataWriter::clear() {
    mBuffer.clear();
}

unsigned char *DataWriter::getBytes() {
    return mBuffer.data();
}

unsigned int DataWriter::getSize() {
    return mBuffer.size();
}
