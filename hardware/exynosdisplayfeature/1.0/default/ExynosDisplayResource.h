#ifndef VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYRESOURCE_H
#define VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYRESOURCE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

#if defined(__EXYNOS9610_DPU__)
#include "include/dpu/exynos9610/DeconCommonHeader.h"
#include "include/dpu/exynos9610/DeconFbHeader.h"
#include "include/dpu/exynos9610/DeconHeader.h"
#include "include/hdr/exynos9610/ExynosDisplayHDR.h"
#elif defined(__EXYNOS9630_DPU__)
#include "include/dpu/exynos9630/DeconCommonHeader.h"
#include "include/dpu/exynos9630/DeconFbHeader.h"
#include "include/dpu/exynos9630/DeconHeader.h"
#include "include/hdr/exynos9630/ExynosDisplayHDR.h"
#elif defined(__EXYNOS9815_DPU__)
#include "include/dpu/exynos9815/DeconCommonHeader.h"
#include "include/dpu/exynos9815/DeconFbHeader.h"
#include "include/dpu/exynos9815/DeconHeader.h"
#include "include/hdr/exynos9815/ExynosDisplayHDR.h"
#else
#include "include/dpu/exynos9815/DeconCommonHeader.h"
#include "include/dpu/exynos9815/DeconFbHeader.h"
#include "include/dpu/exynos9815/DeconHeader.h"
#include "include/hdr/exynos9815/ExynosDisplayHDR.h"
#endif

#include "include/ion/ion_exynos.h"

#define DEVNAME_FB0		"/dev/graphics/fb0"

#define NUM_COLOR	(3)
#define DEDICATED_WIN	(3)

#define RGB_8888_RED		0xFF0000
#define RGB_8888_ORANGE		0xFF7F00
#define RGB_8888_YELLOW		0xFFD400
#define RGB_8888_GREEN		0x00FF00
#define RGB_8888_BLUE		0x0000FF
#define RGB_8888_NAVY		0x000080
#define RGB_8888_PURPLE		0x8B00FF
#define RGB_8888_WHITE		0xFFFFFF
#define RGB_8888_GRAY		0x808080
#define XRGB_8888_RED		0x0000FF00
#define XRGB_8888_GREEN		0x00FF0000
#define XRGB_8888_BLUE		0xFF000000

#endif  // VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYRESOURCE_H
