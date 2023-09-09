# Copyright (C) 2015 The Android Open Source Project
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

ifneq ($(filter exynos, $(TARGET_SOC_NAME)),)
ifeq ($(BOARD_MODULE_USES_GDC), true)
build_dirs += libgdc
endif

ifeq ($(BOARD_USE_HDR10PLUS_STAT_ENC), true)
build_dirs += libHDRMetaGenerator
endif

ifeq ($(BOARD_MODULE_USES_VRA), true)
build_dirs += libvra
endif

include $(call all-named-subdir-makefiles,$(build_dirs))
else
PREFIX := $(shell echo $(TARGET_BOARD_PLATFORM) | head -c 6)
ifneq ($(filter exynos, $(PREFIX)),)
ifeq ($(BOARD_MODULE_USES_GDC), true)
build_dirs += libgdc
endif

ifeq ($(BOARD_USE_HDR10PLUS_STAT_ENC), true)
build_dirs += libHDRMetaGenerator
endif

ifeq ($(BOARD_MODULE_USES_VRA), true)
build_dirs += libvra
endif

include $(call all-named-subdir-makefiles,$(build_dirs))
endif
endif
