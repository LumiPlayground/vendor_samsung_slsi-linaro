LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(TARGET_SOC_BASE), s5e9815)
LOCAL_CFLAGS := -D__EXYNOS9815_DPU__
LOCAL_SRC_FILES := include/hdr/exynos9815/ExynosDisplayHDR.cpp
else
LOCAL_CFLAGS := -D__EXYNOS9815_DPU__
LOCAL_SRC_FILES := include/hdr/exynos9815/ExynosDisplayHDR.cpp
endif
ifeq ($(TARGET_SOC_BASE), exynos9630)
LOCAL_CFLAGS := -D__EXYNOS9630_DPU__
LOCAL_SRC_FILES := include/hdr/exynos9630/ExynosDisplayHDR.cpp
endif

ifeq ($(TARGET_SOC_BASE), exynos9610)
LOCAL_CFLAGS := -D__EXYNOS9610_DPU__
LOCAL_SRC_FILES := include/hdr/exynos9610/ExynosDisplayHDR.cpp
endif

LOCAL_CFLAGS += -DUSES_ARCH_ARM64

LOCAL_MODULE := vendor.samsung_slsi.hardware.exynosdisplayfeature@1.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES += \
    ExynosDisplayFeature.cpp \
    ExynosDisplayUtils.cpp \
    ExynosDisplayColor.cpp \
    include/ion/ion_exynos.c \
    ExynosDisplayPanel.cpp \

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libutils \
    liblog \
    libcutils \
    libhardware \
    libbase \
    libbinder \

LOCAL_SHARED_LIBRARIES += vendor.samsung_slsi.hardware.exynosdisplayfeature@1.0

LOCAL_SHARED_LIBRARIES += libxml2

LOCAL_SHARED_LIBRARIES += libpng

LOCAL_C_INCLUDES := \
    external/libxml2/include \

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vendor.samsung_slsi.hardware.exynosdisplayfeature@1.0-service
LOCAL_INIT_RC := vendor.samsung_slsi.hardware.exynosdisplayfeature@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    service.cpp \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libbinder \
    libhardware \
    libhidlbase \

LOCAL_SHARED_LIBRARIES += vendor.samsung_slsi.hardware.exynosdisplayfeature@1.0

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := exynosdisplayfeature_client
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    client.cpp \

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \
    libbinder \
    libhardware \
    libhidlbase \
    libhidltransport \

LOCAL_SHARED_LIBRARIES += vendor.samsung_slsi.hardware.exynosdisplayfeature@1.0

include $(BUILD_EXECUTABLE)
