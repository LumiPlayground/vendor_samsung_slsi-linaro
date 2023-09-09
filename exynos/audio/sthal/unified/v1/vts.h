/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UAPI__SOUND_VTS_H
#define _UAPI__SOUND_VTS_H

#if defined(__KERNEL__) || defined(__linux__)
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#endif

#ifndef __KERNEL__
#include <stdlib.h>
#endif

#define VTSDRV_MISC_IOCTL_LOAD_SOUND_MODEL    _IOW('V', 0x00, struct vts_model_bin_info)
#define VTSDRV_MISC_IOCTL_READ_GOOGLE_VERSION _IOR('V', 0x01, int)
#define VTSDRV_MISC_IOCTL_READ_EVENT_TYPE     _IOR('V', 0x02, int)
#define VTSDRV_MISC_IOCTL_WRITE_EXIT_POLLING  _IOW('V', 0x03, int)
#define VTSDRV_MISC_IOCTL_SET_PARAM           _IOW('V', 0x04, struct vts_ipc_msg)
#define VTSDRV_MISC_IOCTL_READ_GOOGLE_UUID    _IOR('V', 0x05, char[40])

#endif /* _UAPI__SOUND_VTS_H */
