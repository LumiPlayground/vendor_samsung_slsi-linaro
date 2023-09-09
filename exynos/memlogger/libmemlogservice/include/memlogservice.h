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
 * memlogservice.h
 *
 *  Created on: 2018. 5. 3.
 */

#ifndef __MEMLOG_SERVICE_H__
#define __MEMLOG_SERVICE_H__

enum {
    TYPE_COMMAND,
    TYPE_RAW,
    TYPE_FACTORY_COMMAND,
};

typedef void * HANDLE;

typedef struct {
    void (*onRequest)(int type, int id, void *data, unsigned int datalen);
} MEMLOG_ServiceFunctions;

typedef HANDLE (*MEMLOG_RegisterService)(const char *, MEMLOG_ServiceFunctions *);
typedef void (*MEMLOG_ReleaseService)(HANDLE);
typedef void (*MEMLOG_NotifyCallback)(HANDLE, int, int, void *, unsigned int);

#ifdef __cplusplus
extern "C" {
#endif

HANDLE registerService(const char *serviceName, MEMLOG_ServiceFunctions *func);
void releaseService(HANDLE h);
void notifyCallback(HANDLE h, int type, int id, void *data, unsigned int datalen);

#ifdef __cplusplus
}
#endif

#endif /* __MEMLOG_SERVICE_H__ */
