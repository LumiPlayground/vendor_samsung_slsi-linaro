# Copyright (C) 2015 The Android Open Source Project
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

# HDR10+ Profile A/B MetaGenerator

LOCAL_PATH := $(call my-dir)

ifeq ($(BOARD_USE_HDR10PLUS_STAT_ENC), true)

include $(CLEAR_VARS)

LOCAL_MODULE := libHDRMetaGenerator
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_CHECK_ELF_FILES := false

ifeq ($(BOARD_USES_VENDORIMAGE), true)
LOCAL_PROPRIETARY_MODULE := true
endif

LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_MODULE_TARGET_ARCH := $(TARGET_ARCH) $(TARGET_2ND_ARCH)
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib64/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := lib/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MULTILIB := both

include $(BUILD_PREBUILT)

endif
