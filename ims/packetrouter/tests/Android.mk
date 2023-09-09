LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	src/PacketRouterTest.cpp

# Dependable files
LOCAL_SHARED_LIBRARIES := \
	liblog\
	libnetutils

LOCAL_STATIC_LIBRARIES += \
	libip_checksum

LOCAL_C_INCLUDES := \
	system/core/include\
	system/core/libnetutils/include\
	vendor/samsung_slsi/ims/packetrouter/src

LOCAL_CFLAGS += -DPKTRT_TEST

LOCAL_MODULE := PacketRouterTest
LOCAL_MODULE_TAGS := tests
LOCAL_PROPRIETARY_MODULE := true

#ALLOW_MISSING_DEPENDENCIES=true


include $(BUILD_NATIVE_TEST)
