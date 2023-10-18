ifeq ($(TARGET_SOC_BASE),exynos9630)
LOCAL_PATH := $(call my-dir)

###############################################################################
# libhdcp_tx_static.a

include $(CLEAR_VARS)
LOCAL_MODULE := libhdcp_tx_static
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_TARGET_ARCH := arm
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

###############################################################################
# libstagefright_hdcp

include $(CLEAR_VARS)

LOCAL_MODULE:= libstagefright_hdcp

LOCAL_SRC_FILES:= \
	hdcp_module_tx.cpp

LOCAL_C_INCLUDES:= \
	$(TOP)/frameworks/native/include/media/hardware \
	$(TOP)/hardware/samsung_slsi/exynos/include

LOCAL_STATIC_LIBRARIES:= \
	libhdcp_tx_static

LOCAL_SHARED_LIBRARIES:= \
	liblog \
	libcutils \
	libutils \
	libstagefright_foundation \
	libion

LOCAL_MULTILIB := 32
LOCAL_MODULE_OWNER := samsung

include $(BUILD_SHARED_LIBRARY)
endif
