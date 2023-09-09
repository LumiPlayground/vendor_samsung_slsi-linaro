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
 * DataWriter.h
 *
 *  Created on: 2021. 10. 28.
 *      Author: jinyu.yi
 */

#ifndef __DATAWRITER_H_
#define __DATAWRITER_H_

#include <vector>
#include "types.h"

using namespace std;

class DataWriter {
public:
    // Constructor & Destructor
    DataWriter();
    DataWriter(const DataWriter& orig);
    virtual ~DataWriter();

    // public methods
    DataWriter & writeByte(UINT8);
    DataWriter & writeShort(UINT16);
    DataWriter & writeInt(UINT32);
    DataWriter & writeInt64(ULONG);
    DataWriter & writeBytes(UINT8 *, int length);
    unsigned int length();
    void clear();
    unsigned char* getBytes() ;
    unsigned int getSize();
private:
    vector<unsigned char> mBuffer;
};

#endif /* __DATAWRITER_H_ */