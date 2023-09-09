#
# Copyright (C) 2013 The Android Open Source Project
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
#

# camera external plugIn
ifeq ($(BOARD_MODULE_USES_GDC), true)
PRODUCT_PACKAGES += \
    libexynosgdc
endif

ifeq ($(BOARD_USE_HDR10PLUS_STAT_ENC), true)
PRODUCT_PACKAGES += \
    libHDRMetaGenerator
endif

ifeq ($(BOARD_MODULE_USES_VRA), true)
PRODUCT_PACKAGES += \
    libvra
endif
