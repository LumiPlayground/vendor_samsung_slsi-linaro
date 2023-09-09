# Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
# PROPRIETARY/CONFIDENTIAL
#
# This software is the confidential and proprietary information of
# SAMSUNG ELECTRONICS ("Confidential Information").
#
# You shall not disclose such Confidential Information and shall use it
# only in accordance with the terms of the license agreement
# you entered into with SAMSUNG ELECTRONICS.
#
# SAMSUNG make no representations or warranties about the suitability of
# the software, either express or implied, including but not limited to
# the implied warranties of merchantability, fitness for a particular
# purpose, or non-infringement. SAMSUNG shall not be liable for any
# damages suffered by license as a result of using, modifying or
# distributing this software or its derivatives.

PRODUCT_PACKAGES_DEBUG += \
        SilentLogging \
        sced

PRODUCT_PACKAGES += \
        dmd \
        liboemservice \
        vendor.samsung_slsi.telephony.hardware.oemservice@1.0

##############################################
# SdmDocEnv
##############################################
# A base log directory
# value: a direct path (default: /data/vendor/slog/)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     vendor.sys.exynos.slog.path=

# Enable making ZIP archive
# value: 0 disable / 1 enable (default)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.sys.dm.zip=

# Use SDM type 2 document
# value: 0 use type 1 doc (default) / 1 use type 2 doc
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.config.sdm_type2=

##############################################
# DMFileManager
##############################################
# A threshold of a usage percent of /data partition.
# value: percent (min 1 : max 50, default : 50)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.sys.diag.log.capacity=

# A threshold of a managed file counts.
# value: A number of file to be managed. (min 1 : max 50, default : 50)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.sys.diag.log.managed_size=

# A threshold of a total size of all managed files.
# value: MiB (default : 0 not to use)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.sys.diag.log.maxsize=

##############################################
# SilentLogging
##############################################
# Auto restarting of CP silent logging from booting
# value: 0 disable / 1 enable (default)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.config.dm.autostart=

##############################################
# Test/Debug
##############################################
# Use a self-generated sdm header in higher priority
# value: 0 not to use (default) / 1 use
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.config.sdm_local_header=

# Enable to print VERBOSE logs
# value: 0 disable (default) / 1 enable
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.config.vdbg=

# Enable logging over TCP/IP
# value: 0 enable / 1 disable (default)
#PRODUCT_PROPERTY_OVERRIDES += \
#	     persist.vendor.config.tcp.port.disable=
