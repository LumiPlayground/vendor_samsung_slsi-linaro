ifeq ($(TARGET_SOC),exynos9630)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils libc \
	libui \
	libexpat \

LOCAL_SRC_FILES := \
	FmRadioMptool/FmRadioController_slsi.cpp \
	FmRadioMptool/FmRadioMptool.cpp

LOCAL_LDLIBS := -llog

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libFmRadioMptool

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils libc \
	libui \
	libexpat \

LOCAL_SRC_FILES := \
        fmradio_test/FmRadioController_slsi.cpp \
        fmradio_test/FmRadioController_test.cpp

LOCAL_LDLIBS := -llog

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := fmradio_app

include $(BUILD_EXECUTABLE)

endif
