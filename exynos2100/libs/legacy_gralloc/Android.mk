#
# Copyright (C) 2012 The Android Open Source Project
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
ifeq ($(TARGET_SOC_BASE),exynos2100)
ifeq ($(BOARD_USES_EXYNOS_GRALLOC_VERSION),3)
ifneq ($(MALI_BUILD),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libGLES_mali
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MULTILIB := both
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := samsung_arm
LOCAL_SRC_FILES_$(TARGET_ARCH) := lib64/egl/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_SRC_FILES_$(TARGET_2ND_ARCH) := lib/egl/$(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_SHARED_LIBRARIES := \
        liblog \
        libz \
        libnativewindow \
        libcutils \
        android.hardware.graphics.common@1.0 \
        libdrm \
        android.hardware.graphics.common-ndk_platform \
        android.hardware.graphics.common@1.2 \
        libion_exynos \
        libhidlbase \
        libutils
LOCAL_CHECK_ELF_FILES := false
LOCAL_MODULE_PATH_64 := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/egl
LOCAL_MODULE_PATH_32 := $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/egl
LOCAL_POST_INSTALL_CMD := $(hide) \
    mkdir -p $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw; \
    ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw/vulkan.$(TARGET_BOARD_PLATFORM).so; \
    ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so; \
    ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1; \
    ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1.1; \
    mkdir -p $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw; \
    ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/hw/vulkan.$(TARGET_BOARD_PLATFORM).so; \
    ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so; \
    ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1; \
    ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1.1;
include $(BUILD_PREBUILT)

endif
endif
endif
