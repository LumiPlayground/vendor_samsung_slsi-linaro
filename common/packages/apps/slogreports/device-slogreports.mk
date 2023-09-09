#$(warning "Included device-slogreports.mk")

ifneq ($(DISABLE_SLOGREPORTS), true)
# only for userdebug and eng builds
ifneq (, $(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
PRODUCT_PACKAGES += \
    sLogReports

endif
endif
