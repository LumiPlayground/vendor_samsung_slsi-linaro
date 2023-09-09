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
LOCAL_MODULE := libwland
LOCAL_SRC_FILES := wlbtlog.cpp filedir.cpp common.cpp mxlog.cpp udilog.cpp hip4_profile.cpp \
		commandDispatcher.cpp wlandServer.cpp forkManager.cpp jsonInterface.cpp

LOCAL_SHARED_LIBRARIES := liblog libcutils libnl
LOCAL_WHOLE_STATIC_LIBRARIES := libjsoncpp
LOCAL_C_INCLUDES := \
		system/core/libcutils/include \
		system/core/libutils/include

ifndef SEC_PRODUCT_SHIP
LOCAL_PROPRIETARY_MODULE := true
else
LOCAL_CPPFLAGS +=-DCONFIG_SYSTEM_WLAND
endif

LOCAL_CPPFLAGS += -Wno-error=date-time
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -O2
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE               := slsi_wlan_udi_log_sys
LOCAL_MODULE_CLASS         := EXECUTABLES
LOCAL_SRC_FILES            := slsi_wlan_udi_log
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := wland
LOCAL_SRC_FILES := wland.cpp

LOCAL_SHARED_LIBRARIES := liblog libcutils libnl
LOCAL_STATIC_LIBRARIES := libwland
LOCAL_C_INCLUDES := \
		system/core/libcutils/include \
		system/core/libutils/include


ifndef SEC_PRODUCT_SHIP
$(info "vendor partition, wland.rc")
LOCAL_PROPRIETARY_MODULE := true
LOCAL_INIT_RC := wland.rc
else
$(info "system partition, wland_sys.rc")
LOCAL_INIT_RC := wland_sys.rc
endif

LOCAL_CPPFLAGS += -Wno-error=date-time
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -O2
include $(BUILD_EXECUTABLE)

ifneq ($(filter eng userdebug, $(TARGET_BUILD_VARIANT)),)
include $(call all-makefiles-under, $(LOCAL_PATH))
endif
