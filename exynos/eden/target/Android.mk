# ========================================================
# public eden sdk .txt - for P os
# ========================================================
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := public.libraries-edensdk.samsung.txt
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)
ifeq ($(TARGET_SOC),exynos2100)
LOCAL_SRC_FILES := public.libraries-gpuboost.samsung.txt
else ifeq ($(TARGET_SOC),s5e9925)
LOCAL_SRC_FILES := public.libraries-gpuboost.samsung.txt
else ifeq ($(TARGET_SOC),s5e8825)
LOCAL_SRC_FILES := public.libraries-gpuboost.samsung.txt
else
LOCAL_SRC_FILES := $(LOCAL_MODULE)
endif
include $(BUILD_PREBUILT)
