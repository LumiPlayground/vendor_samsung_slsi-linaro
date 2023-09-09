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


ifneq ($(filter exynos,$(TARGET_SOC_NAME)),)
LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_USES_HWC_SERVICES),true)

include $(CLEAR_VARS)

ifndef TARGET_SOC_BASE
	TARGET_SOC_BASE := $(TARGET_SOC)
endif

LOCAL_MODULE := vendor.samsung_slsi.hardware.ExynosHWCServiceTW@1.0-service
LOCAL_INIT_RC := vendor.samsung_slsi.hardware.ExynosHWCServiceTW@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
  ExynosHWCServiceTW.cpp \
  service.cpp

LOCAL_CFLAGS := -fno-exceptions

ifdef BOARD_HWC_VERSION
LOCAL_CFLAGS += -DUSE_LIBHWC21
LOCAL_CFLAGS += -DENABLE_DDI_SCALER
LOCAL_CFLAGS += -DSUPPORT_WFD_COMMAND
ifeq ($(BOARD_USES_HWC_CPU_PERF_MODE),true)
LOCAL_CFLAGS += -DUSE_CPU_PERF_MODE
endif
LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/graphics/base/$(BOARD_HWC_VERSION)/libhwcService \
endif

LOCAL_SHARED_LIBRARIES := \
  libutils \
  libcutils \
  libhidlbase \
  libhidltransport \
  liblog \
  libbinder \
  libsync \
  libhardware \
  android.hardware.graphics.composer@2.1 \
  android.hardware.graphics.allocator@2.0 \
  vendor.samsung_slsi.hardware.ExynosHWCServiceTW@1.0 \
  libExynosHWCService \
  libion \
  libdrm

LOCAL_HEADER_LIBRARIES := libhardware_legacy_headers libbinder_headers libutils_headers libexynos_headers libdrm
ifeq ($(BOARD_USES_HDR_INTERFACE), true)
LOCAL_HEADER_LIBRARIES += libhdrinterface_header libhdr10p_meta_interface_header
endif
ifeq ($(BOARD_USES_DQE_INTERFACE), true)
LOCAL_HEADER_LIBRARIES += libdqeInterface_headers
endif
include $(TOP)/hardware/samsung_slsi/graphics/base/BoardConfigCFlags.mk
include $(BUILD_EXECUTABLE)

endif
endif
endif
