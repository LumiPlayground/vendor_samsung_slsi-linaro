LOCAL_PATH := $(call my-dir)

################################################
include $(CLEAR_VARS)
LOCAL_MODULE := com.android.contacts_slsi.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_PRODUCT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

################################################
include $(CLEAR_VARS)
LOCAL_MODULE := com.android.dialer_slsi.xml
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_PRODUCT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
