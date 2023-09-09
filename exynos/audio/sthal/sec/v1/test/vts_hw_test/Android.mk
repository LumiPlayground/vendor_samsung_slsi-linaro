# Copyright (C) 2021 The Android Open Source Project
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
# VTS Unit Test Console application
#
ifeq ($(BOARD_USE_STHAL_SEC_V1),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vtshw-test-v1

LOCAL_SRC_FILES := \
    vts_hw_test.c \
    ../../../../unified/v1/st_xml_parser.c \
    ../../../../unified/v1/st_pcm_dump.c

LOCAL_C_INCLUDES += \
    external/tinyalsa/include \
    $(TOP)/vendor/samsung_slsi/exynos/audio/sthal/unified/v1 \
    $(TOP)/vendor/samsung_slsi/exynos/audio/sthal/sec/v1 \
    external/expat/lib

LOCAL_HEADER_LIBRARIES := libaudio_exynos_headers

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libtinyalsa \
    libhardware \
    libexpat

LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function

LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
endif
