LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform

LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := AudioLogging
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_PROGUARD_ENABLED := disabled
LOCAL_PRIVILEGED_MODULE := true
LOCAL_REQUIRED_MODULES := privapp-permissions-com.samsung.slsi.audiologging.xml

LOCAL_STATIC_JAVA_LIBRARIES += \
        vendor.samsung_slsi.hardware.audio_dump-V1.0-java

include $(BUILD_PACKAGE)

include $(call all-makefiles-under,$(LOCAL_PATH))

# ============================================================
# Install the permissions file into system/etc/permissions
# ============================================================
include $(CLEAR_VARS)
LOCAL_MODULE := privapp-permissions-com.samsung.slsi.audiologging.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
