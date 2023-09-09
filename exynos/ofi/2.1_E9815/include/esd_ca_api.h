#ifndef __ESD_CA_H__
#define __ESD_CA_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
    int32_t caDSP_Init(void);
    int32_t caDSP_Deinit(void);
    int32_t caOpen(void);
    void    caClose(void);
    int32_t caDSP_LoadKernel(const char *kernel_name);
    int32_t caDSP_UnloadKernel(const char *kernel_name);
#ifdef __cplusplus
}
#endif

#endif // __ESD_CA_H__
