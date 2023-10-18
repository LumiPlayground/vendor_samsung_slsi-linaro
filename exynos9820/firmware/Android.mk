#
# Copyright (C) 2013 The Android Open Source Project
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

LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_SOC),exynos9820)

ifeq ($(BOARD_USE_DIVX_ENABLE), false)
include $(CLEAR_VARS)

LOCAL_MODULE := mfc_fw.bin
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := mfc_fw_v13.0.bin

include $(BUILD_PREBUILT)
endif

endif

ifneq (,$(filter exynos9820 exynos9825, $(TARGET_SOC)))

include $(CLEAR_VARS)

LOCAL_MODULE := NPU.bin
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/firmware
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := npu_fw_v9.094.bin

include $(BUILD_PREBUILT)

endif
