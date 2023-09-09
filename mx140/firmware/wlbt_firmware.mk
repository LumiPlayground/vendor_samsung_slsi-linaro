ifeq ($(CONFIG_SAMSUNG_SCSC_WIFIBT),true)

ifeq ($(TARGET_PRODUCT),full_erd9935_t)
FW_PATH_FIND := vendor/samsung_slsi/mx140/firmware/$(SCSC_WLAN_DEVICE_NAME)/mx140_evt*
else
FW_PATH_FIND := vendor/samsung_slsi/mx140/firmware/$(SCSC_WLAN_DEVICE_NAME)
endif
FW_PATH_HOST := vendor/samsung_slsi/mx140/firmware/$(SCSC_WLAN_DEVICE_NAME)
FW_PATH_DEVICE := vendor/etc/wifi

REGDB_PATH_HOST := vendor/samsung_slsi/mx140/firmware/reg_domain/regulatory_db_2_7.bin
REGDB_PATH_DEVICE := vendor/etc/wifi/slsi_reg_database.bin

REGDB_ABSOLUTE_PATH_FW := $(TARGET_COPY_OUT_VENDOR)/etc/wifi/slsi_reg_database.bin

PRODUCT_PACKAGES_ENG += enable_monitor_mode.sh \
                        enable_test_mode.sh
PRODUCT_PACKAGES_DEBUG += enable_monitor_mode.sh \
                        enable_test_mode.sh

# fw dirs: mx140.bin mx140_t.bin mx140/ mx140_t/
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
PRODUCT_COPY_FILES += $(foreach image,\
        $(shell find $(FW_PATH_FIND) -type f),\
        $(image):$(subst $(FW_PATH_HOST),$(FW_PATH_DEVICE),$(image)))
else # don't copy *.sym files in user build
PRODUCT_COPY_FILES += $(foreach image,\
        $(shell find $(FW_PATH_FIND) -type f -not -name '*.sym'),\
        $(image):$(subst $(FW_PATH_HOST),$(FW_PATH_DEVICE),$(image)))
endif

PRODUCT_COPY_FILES += $(REGDB_PATH_HOST):$(REGDB_PATH_DEVICE)
# Moredump binary for this FW
#PRODUCT_PACKAGES_ENG += $(FW_PATH_HOST)/mx140/debug/hardware/moredump/moredump.bin
#PRODUCT_PACKAGES_DEBUG += $(FW_PATH_HOST)/mx140/debug/hardware/moredump/moredump.bin
ifeq ($(TARGET_PRODUCT),full_erd9935_t)
PRODUCT_COPY_FILES += $(FW_PATH_HOST)/mx140_evt0.1/debug/hardware/moredump/moredump.bin:$(TARGET_COPY_OUT_VENDOR)/bin/moredump.bin
else
PRODUCT_COPY_FILES += $(FW_PATH_HOST)/mx140/debug/hardware/moredump/moredump.bin:$(TARGET_COPY_OUT_VENDOR)/bin/moredump.bin
endif

ifeq ($(TARGET_PRODUCT),full_erd8835_t)
PRODUCT_COPY_FILES += $(FW_PATH_HOST)/mx140/wpal:$(TARGET_COPY_OUT_VENDOR)/bin/wpal
endif
endif
