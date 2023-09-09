ifeq ($(CONFIG_SAMSUNG_SCSC_WIFIBT),true)
PRODUCT_PACKAGES_DEBUG += \
        wland \
        CNNTLogger \
 #       ChartViewer

$(warning "SEC_PRODUCT_SHIP is " $(SEC_PRODUCT_SHIP))
ifneq ($(TARGET_BUILD_VARIANT),user)
DEVICE_PACKAGE_OVERLAYS += \
               vendor/samsung_slsi/scsc_tools/wlbt/device/samsung/overlay

ifdef SEC_PRODUCT_SHIP
PRODUCT_PACKAGES_DEBUG += \
         slsi_wlan_udi_log_sys

endif
endif

endif
