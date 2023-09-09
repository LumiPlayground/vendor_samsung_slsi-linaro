/*
 * Copyright (C) 2018 Samsung Electronics Co. LTD
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 *
 */
#ifndef SOURCE_COMMON_INCLUDE_OFI_OS_HAL_H_
#define SOURCE_COMMON_INCLUDE_OFI_OS_HAL_H_

#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief defines OS_Hal layer functions
 *
 * @file ofi_os_hal.h
 * @author hoon98.choi@samsung.com
 * @date 2018-08-21
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OFI_VER_2019
#define CPU_BIG_CLUSTER (240)
#endif

extern pid_t ofi_osal_get_tid(void);
extern int32_t ofi_osal_ion_open(int32_t *heapmask);
extern void ofi_osal_ion_close(int32_t ionFd);
extern int32_t ofi_osal_ion_alloc(int32_t ionClientFd, int32_t heapmask,
                          int32_t size, int32_t flag);
extern void ofi_osal_ion_free(int32_t alloc_fd);

extern int ofi_set_sched_affinity(int32_t);
extern int ofi_adjust_thread_priority_urgent(void);

#ifdef __cplusplus
}
#endif


#endif  // SOURCE_COMMON_INCLUDE_OFI_OS_HAL_H_
