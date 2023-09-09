/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MEMLOG_SERVICE_H__
#define __MEMLOG_SERVICE_H__

enum {
    TYPE_COMMAND,
    TYPE_RAW,
};

typedef void * HANDLE;

typedef struct {
    void (*onRequest)(int type, int id, void *data, unsigned int datalen);
} Memlog_ServiceFunctions;

typedef HANDLE (*Memlog_RegisterService)(const char *, Memlog_ServiceFunctions *);
typedef void (*Memlog_ReleaseService)(HANDLE);
typedef void (*Memlog_NotifyCallback)(HANDLE, int, int, void *, unsigned int);

#ifdef __cplusplus
extern "C" {
#endif

HANDLE registerService(const char *serviceName, Memlog_ServiceFunctions *func);
void releaseService(HANDLE h);
void notifyCallback(HANDLE h, int type, int id, void *data, unsigned int datalen);

#ifdef __cplusplus
}
#endif

#endif /* __Memlog_SERVICE_H__ */
