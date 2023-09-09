#
# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)
ifneq ($(filter exynos, $(TARGET_SOC_NAME)),)
include $(CLEAR_VARS)

ifndef TARGET_SOC_BASE
	TARGET_SOC_BASE := $(TARGET_SOC)
endif
ifneq ($(filter exynos9830, $(TARGET_SOC_BASE)),)

LOCAL_MODULE := vendor.samsung_slsi.hardware.geoTransService@1.0-service
LOCAL_INIT_RC := vendor.samsung_slsi.hardware.geoTransService@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
  GeoTransService.cpp \
  service.cpp

LOCAL_CFLAGS := -fno-exceptions

LOCAL_C_INCLUDES += \
    $(TOP)/vendor/samsung_slsi/exynos/modules/libgdc/include \
    $(TOP)/hardware/samsung_slsi/exynos/include

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libhwbinder \
    libhidlbase \
    libhidlmemory \
    libhidltransport \
    liblog \
    libcutils \
    android.hidl.memory@1.0 \
    vendor.samsung_slsi.hardware.geoTransService@1.0 \
    libexynosgdc \
    libcsc \
    libexynosutils \
    libexynosv4l2 \

LOCAL_MULTILIB := 32

include $(BUILD_EXECUTABLE)
endif
else
PREFIX := $(shell echo $(TARGET_BOARD_PLATFORM) | head -c 6)
ifneq ($(filter exynos, $(PREFIX)),)
include $(CLEAR_VARS)

ifndef TARGET_SOC_BASE
	TARGET_SOC_BASE := $(TARGET_SOC)
endif
ifneq ($(filter exynos9830, $(TARGET_SOC_BASE)),)

LOCAL_MODULE := vendor.samsung_slsi.hardware.geoTransService@1.0-service
LOCAL_INIT_RC := vendor.samsung_slsi.hardware.geoTransService@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
  GeoTransService.cpp \
  service.cpp

LOCAL_CFLAGS := -fno-exceptions

LOCAL_C_INCLUDES += \
    $(TOP)/vendor/samsung_slsi/exynos/modules/libgdc/include \
    $(TOP)/hardware/samsung_slsi/exynos/include

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libhwbinder \
    libhidlbase \
    libhidlmemory \
    libhidltransport \
    liblog \
    libcutils \
    android.hidl.memory@1.0 \
    vendor.samsung_slsi.hardware.geoTransService@1.0 \
    libexynosgdc \
    libcsc \
    libexynosutils \
    libexynosv4l2 \

LOCAL_MULTILIB := 32

include $(BUILD_EXECUTABLE)
endif
endif
endif
