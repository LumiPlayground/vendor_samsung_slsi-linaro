#
# Copyright 2021 The Android Open Source Project
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

ifeq ($(BOARD_USE_STHAL_ERD_V1),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

STHAL_SEC_PATH := $(TOP)/vendor/samsung_slsi/exynos/audio/sthal/erd
STHAL_UNIFIED_PATH := $(TOP)/vendor/samsung_slsi/exynos/audio/sthal/unified
DEVICE_BASE_PATH := $(TOP)/device/samsung

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
    sound_trigger_hw_odm.c \
    ../../unified/v1/sound_trigger_hw.c \
    ../../unified/v1/st_xml_parser.c \
    ../../unified/v1/st_pcm_dump.c

LOCAL_C_INCLUDES += \
    external/tinyalsa/include \
    ./ \
    $(STHAL_UNIFIED_PATH)/v1 \
    external/expat/lib

LOCAL_HEADER_LIBRARIES := libaudio_exynos_headers

LOCAL_SHARED_LIBRARIES := \
    liblog libcutils libtinyalsa libhardware libexpat

LOCAL_CFLAGS := -coverage
LOCAL_LDFLAGS := -coverage
LOCAL_NATIVE_COVERAGE := true
GCOV_PREFIX := /data/coverage
LOCAL_CFLAGS += -fprofile-dir="/data/coverage"

ifeq ($(BOARD_USE_STHAL_AUDIOROUTE),true)
LOCAL_CFLAGS += -DSUPPORT_AUDIO_ROUTE
endif

LOCAL_MODULE := sound_trigger.primary.$(TARGET_SOC)
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
