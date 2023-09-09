#
# Copyright Samsung Electronics Co., LTD.
#
# This software is proprietary of Samsung Electronics.
# No part of this software, either material or conceptual may be copied or distributed, transmitted,
# transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
# electronic, mechanical, manual or otherwise, or disclosed
# to third parties without the express written permission of Samsung Electronics.
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := wlandTest
LOCAL_MODULE_TAGS := tests
ifndef SEC_PRODUCT_SHIP
LOCAL_PROPRIETARY_MODULE := true
endif

LOCAL_SRC_FILES := jsonInterfaceTest.cpp commandDispatcherTest.cpp wlandServerTest.cpp forkManagerTest.cpp

# Dependable files
LOCAL_STATIC_LIBRARIES := \
	libgmock \
	libgmock_main \
	libgtest \
	libwland

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils

LOCAL_C_INCLUDES := \
		system/core/libcutils/include

include $(BUILD_NATIVE_TEST)
