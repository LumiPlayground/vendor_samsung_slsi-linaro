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
 * datautils.h
 *
 *  Created on: 2021. 9. 6.
 *      Author: sungwoo48.choi
 */

#ifndef __DATA_UTILS_H__
#define __DATA_UTILS_H__

#include "sdmstream.h"

#include <string>

struct DataWriter {
private:
    SdmOutputStream *mOutputStream;

public:
    DataWriter(SdmOutputStream *outputStream);
    ~DataWriter();

    DataWriter& append(uint8_t b);
    DataWriter& append(int8_t b);
    DataWriter& append(uint16_t val);
    DataWriter& append(int16_t val);
    DataWriter& append(uint32_t val);
    DataWriter& append(int32_t val);
    DataWriter& append(uint64_t val);
    DataWriter& append(int64_t val);
    DataWriter& append(const std::string& str);
    DataWriter& append(const void *data, size_t datalen);

private:
    int writeInternal(const void *data, size_t datalen);
};

DataWriter& operator<< (DataWriter& builder, uint8_t b);
DataWriter& operator<< (DataWriter& builder, int8_t b);
DataWriter& operator<< (DataWriter& builder, uint16_t val);
DataWriter& operator<< (DataWriter& builder, int16_t val);
DataWriter& operator<< (DataWriter& builder, uint32_t val);
DataWriter& operator<< (DataWriter& builder, int32_t val);
DataWriter& operator<< (DataWriter& builder, uint64_t val);
DataWriter& operator<< (DataWriter& builder, int64_t val);
DataWriter& operator<< (DataWriter& builder, const std::string& str);

/**
 * DataReader
 */
struct DataReader {
private:
    SdmInputStream *mInputStream;

public:
    DataReader(SdmInputStream *inputStream);
    ~DataReader();
    int read(uint8_t& b);
    int read(int8_t& b);
    int read(uint16_t& s);
    int read(int16_t& s);
    int read(uint32_t& val);
    int read(int32_t& val);
    int read(uint64_t& val);
    int read(int64_t& val);
    int read(void *buf, size_t readSize);
};

#endif /* __DATA_UTILS_H__ */
