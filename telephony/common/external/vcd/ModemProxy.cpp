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
 * ModemProxy.cpp
 *
 *  Created on: 2020. 2. 12.
 */
#include <errno.h>
#include "ModemProxy.h"
#include "base/log.h"

extern int get_modem_fd(void);

ModemProxy::ModemProxy()
{

}

void ModemProxy::Init()
{

}

int ModemProxy::Write(void *data, size_t datalen)
{
    if (data == NULL || datalen == 0) {
        return 0;
    }

    int fd = get_modem_fd();
    size_t write_len = 0;
    do {
        int len = 0;
        if ((len = write(fd, (char *)data + write_len, (datalen - write_len))) < 0) {
            ALOGE("%s : Fail to write", __func__);
            if (errno == EINTR || errno == EAGAIN)
                continue;
            return -1;
        }
        write_len += (size_t)len;
    } while (write_len < datalen);
    return write_len;
}

void ModemProxy::StopForcibly()
{
    // do nothing
}

static ModemProxy sInst;
ModemProxy *ModemProxy::GetInstance() {
    return &sInst;
}
