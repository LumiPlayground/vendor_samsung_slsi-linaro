# Copyright (C) 2020 The Android Open Source Project
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

#
# AudioDump Service
#

ifeq ($(BOARD_USES_AUDIO_LOGGING_SERVICE), true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vendor.samsung_slsi.hardware.audio_dump@1.0-service

LOCAL_INIT_RC := vendor.samsung_slsi.hardware.audio_dump@1.0-service.rc
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_SRC_FILES := \
    AudioDump.cpp \
    service.cpp

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidlmemory \
    liblog \
    libutils \
    libcutils \
    libbinder \
    vendor.samsung_slsi.hardware.audio_dump@1.0 \
    android.hardware.audio@7.0

ifeq ($(BOARD_USES_VTS_LOGGING), true)
LOCAL_CFLAGS += -DSUPPORT_VTS_LOGGING
endif

LOCAL_HEADER_LIBRARIES := libaudio_exynos_headers

include $(BUILD_EXECUTABLE)
endif
