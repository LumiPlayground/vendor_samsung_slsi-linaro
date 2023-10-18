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
ifeq ($(TARGET_SOC_BASE),s5e9815)
ifneq ($(MALI_BUILD),true)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libOpenCL
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true
LOCAL_POST_INSTALL_CMD := \
	ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so; \
	ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1; \
	ln -sf /vendor/lib64/egl/libGLES_mali.so $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1.1; \
	ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so; \
	ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1; \
	ln -sf /vendor/lib/egl/libGLES_mali.so $(2ND_TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libOpenCL.so.1.1;

#overlay by the special mali library only when build with SANITIZE_TARGET=hwaddress
ifneq ($(filter hwaddress,$(SANITIZE_TARGET)),)
LOCAL_POST_INSTALL_CMD += \
    cp -v $(LOCAL_PATH)/lib64/egl/libGLES_mali_hwasan_64.so $(TARGET_OUT_VENDOR)/lib64/egl/libGLES_mali.so; \
    cp -v $(LOCAL_PATH)/lib/egl/libGLES_mali_hwasan_32.so   $(TARGET_OUT_VENDOR)/lib/egl/libGLES_mali.so;
endif

include $(BUILD_SHARED_LIBRARY)
endif
endif
