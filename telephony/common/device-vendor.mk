# Copyright (C) 2014 The Android Open Source Project
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

# Including order
# full_[product].mk -> device.mk (include BoardConfig.mk at the first line) -> telephony/device_vendor.mk

# Origianlly placed at last lines in device.mk
# Some device specific line should be reviewed
# It's better if we can use some variable holding device path or name

# Informative code For Debugging
# we can use these variables that's tested

# exynos RIL
TARGET_EXYNOS_RIL_SOURCE := true
ENABLE_VENDOR_RIL_SERVICE := true

# Multi SIM(DSDS)
SIM_COUNT := 2

$(call inherit-product-if-exists, vendor/samsung_slsi/telephony/common/device/samsung/conf_copy.mk)

######
# Expand AOSP definition from
#$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base_telephony.mk)
##$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
# aosp_base.mk live in device.mk
#$(call inherit-product, $(SRC_TARGET_DIR)/product/aosp_base.mk)
### Excluding AOSP apns copy

PRODUCT_PROPERTY_OVERRIDES += keyguard.no_require_sim=true

$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony.mk)

PRODUCT_PACKAGES += \
	messaging

# Vendor ext (custom platform dependency)
ifneq ($(PRODUCT_VENDOR_TELEPHONY_EXT), )
# these packages has dependency to IVendorTelephony* interface which was
# applied into framework/base/ to be used as hidden system api currently
# injection
PRODUCT_PACKAGES += \
        exynos-telephony-common \
        exynos-telephony-common.xml

# For slsi systemUI.
# System_slsi is implemented based on frameworks/base/packages/SystemUI
PRODUCT_PACKAGES += SystemUI_slsi

PRODUCT_PACKAGES += \
        OemRilService
endif
#
######

# soong configruations for vendor RIL
$(call soong_config_set,vendor_ril_feature,multisim,)
$(call soong_config_set,vendor_ril_feature,use_radio_hal_1_6,)
$(call soong_config_set,vendor_ril_feature,use_radio_hal_2_0,)
$(call soong_config_set,vendor_ril_feature,auto_verify_pin_with_cp,)
$(call soong_config_set,vendor_ril_feature,disable_emulate_ims_permanent_failure,)

ifeq ($(SIM_COUNT), 2)
$(warning "Building with an option SIM_COUNT=$(SIM_COUNT)")
$(call soong_config_set,vendor_ril_feature,multisim,true)
endif

$(call soong_config_set,vendor_ril_feature,auto_verify_pin_with_cp,false)

# Modify RIL daemon (rild-exynos)
# We no more need to copy and override init.rc
# rild.rc will takes this role

VENDOR_DEVICE_BASE := vendor/samsung_slsi/telephony/common/device/samsung

# init.vendor_telephony.rc
PRODUCT_COPY_FILES += \
    $(VENDOR_DEVICE_BASE)/conf/init.vendor_telephony.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.vendor_telephony.rc

# Need to move common area, then split common/variable parts
# For SPN display
PRODUCT_COPY_FILES += \
    $(VENDOR_DEVICE_BASE)/spn-conf.xml:system/etc/spn-conf.xml

# For APN
PRODUCT_COPY_FILES += \
    $(VENDOR_DEVICE_BASE)/apns-conf.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/apns-conf.xml

# copy open carrier info file to system folder
PRODUCT_COPY_FILES += \
    $(VENDOR_DEVICE_BASE)/open_carrier_info.dat:$(TARGET_COPY_OUT_VENDOR)/etc/open_carrier_info.dat

# multisim config for frameworks
ifeq ($(SUPPORT_DS), true)
PRODUCT_PROPERTY_OVERRIDES += \
        persist.radio.multisim.config=dsds
endif

# default network
ifeq ($(SUPPORT_NR), true)
ifeq ($(SUPPORT_CDMA), true)
PRODUCT_PROPERTY_OVERRIDES += \
        ro.telephony.default_network=27
else
PRODUCT_PROPERTY_OVERRIDES += \
        ro.telephony.default_network=26
endif
else
PRODUCT_PROPERTY_OVERRIDES += \
        ro.telephony.default_network=10
endif

# support nr ds
ifeq ($(SUPPORT_NR_DS), true)
PRODUCT_PROPERTY_OVERRIDES += \
        persist.vendor.ril.support_nr_ds=1
else
PRODUCT_PROPERTY_OVERRIDES += \
        persist.vendor.ril.support_nr_ds=0
endif

# Log level (only for userdebug and eng builds)
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
PRODUCT_PROPERTY_OVERRIDES += \
        ro.logd.size=16M \
        log.tag.NetworkController=DEBUG \
        log.tag.PhoneConfig=VERBOSE

# RIL logcapture
PRODUCT_PROPERTY_OVERRIDES += \
        persist.vendor.ril.log_mask=3 \
        persist.vendor.ril.log.chunk_size=5000000 \
        persist.vendor.ril.log.base_dir=/data/vendor/slog
endif

# Device Manifest, Device Compatibility Matrix for Treble
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += \
	$(VENDOR_DEVICE_BASE)/framework_compatibility_matrix.xml

DEVICE_MATRIX_FILE += \
	$(VENDOR_DEVICE_BASE)/compatibility_matrix.xml

# 2.0
ifeq ($(USE_RADIO_HAL_2_0), true)
ifeq ($(SUPPORT_DS), true)
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/aidl/manifest_radio_ds.xml
else
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/aidl/manifest_radio.xml
endif
PRODUCT_PACKAGES += libril-aidl

PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.ril.use_radio_hal=2.0
$(warning "Building with an option USE_RADIO_HAL_2_0 := true")
$(call soong_config_set,vendor_ril_feature,use_radio_hal_2_0,true)
$(call soong_config_set,vendor_ril_feature,disable_emulate_ims_permanent_failure,true)
# 1.6
else ifeq ($(USE_RADIO_HAL_1_6), true)
ifeq ($(SUPPORT_DS), true)
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/1.6/manifest_radio_ds.xml
else
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/1.6/manifest_radio.xml
endif
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/manifest_radioconfig.xml
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.ril.use_radio_hal=1.6
$(warning "Building with an option USE_RADIO_HAL_1_6 := true")
$(call soong_config_set,vendor_ril_feature,use_radio_hal_1_6,true)
# Expect to be used in T version build and later
$(call soong_config_set,vendor_ril_feature,disable_emulate_ims_permanent_failure,true)
# 1.5
else
ifeq ($(SUPPORT_DS), true)
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/1.5/manifest_radio_ds.xml
else
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/1.5/manifest_radio.xml
endif
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/manifest_radioconfig.xml
$(warning "Building with an option USE_RADIO_HAL_1_5 := true")
$(call soong_config_set,vendor_ril_feature,disable_emulate_ims_permanent_failure,true)
endif

# for secure_element
ifeq ($(SUPPORT_DS), true)
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/secureelement/1.2/manifest_ds.xml
else
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/secureelement/1.2/manifest.xml
endif

# OEM Radio HAL(HIDL)
ifeq ($(SUPPORT_DS), true)
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/oem/manifest_oem_radio_ds.xml
else
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/oem/manifest_oem_radio.xml
endif
DEVICE_MANIFEST_FILE += $(VENDOR_DEVICE_BASE)/manifests/oem/manifest_oem_external.xml

# multiple vendor
ifeq ($(TARGET_BUILD_CARRIER),)
$(warning No definition of TARGET_BUILD_CARRIER, set europen at default)
CARRIER := europen
else
$(warning Found TARGET_BUILD_CARRIER=$(TARGET_BUILD_CARRIER))
CARRIER := $(TARGET_BUILD_CARRIER)
endif

PRODUCT_PROPERTY_OVERRIDES += \
    ro.carrier=$(CARRIER) \
    ro.vendor.config.build_carrier=$(CARRIER)

ifeq ($(MODEM_NOT_USE_CP_PARTION), true)
PRODUCT_COPY_FILES += \
	vendor/samsung_slsi/telephony/common/radio/$(DEVICE_PRODUCT)/modem.bin:$(TARGET_COPY_OUT_VENDOR)/firmware/modem.bin
endif

DEVICE_PACKAGE_OVERLAYS += \
    vendor/samsung_slsi/telephony/common/device/samsung/carriers/overlay

# Enable 4GDS at default from Q
PRODUCT_PROPERTY_OVERRIDES += \
        persist.vendor.radio.dual.volte=1

PRODUCT_PROPERTY_OVERRIDES += \
        ro.ril.ecclist=911,112 \
        persist.vendor.ril.ecc.db_id.type=4 \
        persist.vendor.ril.ecc.use.last.db_id=1 \

ECC_DB_TYPE := xml
ifeq ($(ECC_DB_TYPE), xml)
PRODUCT_PROPERTY_OVERRIDES += \
        persist.vendor.ril.ecc.use.xml=1
PRODUCT_COPY_FILES += \
        vendor/samsung_slsi/telephony/common/device/samsung/init.radio.sh:$(TARGET_COPY_OUT_VENDOR)/bin/init.radio.sh \
        vendor/samsung_slsi/telephony/common/device/samsung/conf/init.vendor_telephony.rc:vendor/etc/init/init.vendor_telephony.rc
PRODUCT_COPY_FILES += $(call find-copy-subdir-files,*,vendor/samsung_slsi/telephony/exynos-ril/sitril/database/xml,$(TARGET_COPY_OUT_VENDOR)/etc/database)
endif

# Possible options
#  [ default | AP-assisted | legacy ]
# 'default' is same to 'AP-assisted'
#PRODUCT_PROPERTY_OVERRIDES += \
#	    ro.telephony.iwlan_operation_mode=legacy

PRODUCT_PROPERTY_OVERRIDES += \
        telephony.lteOnCdmaDevice=1

PRODUCT_PROPERTY_OVERRIDES += \
        vendor.rild.libpath=libsitril.so

# Originally placed at last lines in full_[product].mk
# Telephony feature

# eng only
PRODUCT_PACKAGES_ENG += \
        KeyString

# debug only
PRODUCT_PACKAGES_DEBUG += \
        EngineerMode \
        NetworkTestMode \
        UARTSwitch \
        SysDebugMode \
        USBModeManager \
        USBModeSwitch \
        DataTestMode \
        TestMode \
        AutoAnswer \
        CarrierConfigOverride \
        tcpdump_vendor

# optional
PRODUCT_PACKAGES += \
        vcd \
        rfsd \
        bipchmgr

# Build rild_exynos and its family
ifeq ($(TARGET_EXYNOS_RIL_SOURCE),true)
PRODUCT_PACKAGES += \
        rild_exynos \
        libsitril \
        libjsoncpp \
        libril_sitril \
        libsitril-client \
        libsitril-audio \
        libsitril-gps
endif

PRODUCT_PACKAGES += \
        vendor.samsung_slsi.telephony.hardware.radio@1.0 \
        vendor.samsung_slsi.telephony.hardware.radioExternal@1.0

PRODUCT_PACKAGES += \
        vendor.samsung_slsi.telephony.hardware.radio@1.1 \
        vendor.samsung_slsi.telephony.hardware.radio@1.2

PRODUCT_PACKAGES += \
        android.hardware.secure_element@1.2-service-uicc

PRODUCT_PACKAGES += \
        android.hardware.radio.config@1.2-service

# STK
PRODUCT_PACKAGES += \
        Stk

PRODUCT_PACKAGES += privapp-permissions \
        com.android.contacts_slsi.xml \
        com.android.dialer_slsi.xml

PRODUCT_PACKAGES += version_info_exynos_ril_etc

BOARD_SEPOLICY_DIRS += \
        vendor/samsung_slsi/telephony/common/device/samsung/sepolicy \
        vendor/samsung_slsi/telephony/common/external/tools

# telephony features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml \
    frameworks/native/data/etc/android.hardware.telephony.cdma.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/native/data/etc/android.hardware.se.omapi.uicc.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.se.omapi.uicc.xml

# Telephony logging
$(call inherit-product-if-exists, vendor/samsung_slsi/telephony/common/telephony_logging.mk)
