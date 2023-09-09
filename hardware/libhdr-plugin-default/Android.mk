LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wno-unused-function -DLOG_TAG=\"libhdrinterface\"
LOCAL_SRC_FILES := \
    libhdr_plugin_default.cpp

LOCAL_HEADER_LIBRARIES := libhdrinterface_header libhdr10p_meta_interface_header libsystem_headers
LOCAL_SHARED_LIBRARIES := liblog

LOCAL_MODULE := libhdr_plugin_default
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
