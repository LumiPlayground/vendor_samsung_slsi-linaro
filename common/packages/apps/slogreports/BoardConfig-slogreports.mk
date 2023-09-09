#$(warning "Included BoardConfig-slogreports.mk")
#$(warning "#### DISABLE_SLOGREPORTS=$(DISABLE_SLOGREPORTS)")

ifneq ($(DISABLE_SLOGREPORTS), true)
# only for userdebug and eng builds
ifneq (, $(filter userdebug eng, $(TARGET_BUILD_VARIANT)))
SLOGREPORTS_SEPOLICY_DIR := $(SSCR_MP_TOOL_APP_DIR)/slogreports/sepolicy
# slogreports SE
ifeq (, $(findstring $(SLOGREPORTS_SEPOLICY_DIR), $(BOARD_VENDOR_SEPOLICY_DIRS)))
BOARD_VENDOR_SEPOLICY_DIRS += $(SLOGREPORTS_SEPOLICY_DIR)
endif
endif
endif
