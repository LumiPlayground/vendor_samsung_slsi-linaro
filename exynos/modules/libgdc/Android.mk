# Copyright (C) 2018 The Android Open Source Project
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
include $(CLEAR_VARS)

COMMIT_ID := $(shell (cd $(LOCAL_PATH);git log --pretty=oneline -1 --abbrev-commit | awk '{print $$1}'))

LOCAL_SRC_FILES := ExynosGDCInterface.cpp ExynosGDC.cpp
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libexynosv4l2 libexynosutils

LOCAL_MODULE := libexynosgdc

LOCAL_C_INCLUDES += \
    $(TOP)/hardware/samsung_slsi/exynos/include \
    $(LOCAL_PATH)/include \

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

LOCAL_CFLAGS += -Wno-error=date-time
LOCAL_CFLAGS += -DCOMMITID=\"$(COMMIT_ID)\"

include $(TOP)/hardware/samsung_slsi/exynos/BoardConfigCFlags.mk
include $(BUILD_SHARED_LIBRARY)
