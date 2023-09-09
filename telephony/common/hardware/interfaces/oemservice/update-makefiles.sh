#!/bin/bash

options="-r vendor.samsung_slsi.telephony.hardware:vendor/samsung_slsi/telephony/common/hardware/interfaces \
         -r android.hidl:system/libhidl/transport \
         -r android.hardware:hardware/interfaces"

hidl-gen -Landroidbp $options vendor.samsung_slsi.telephony.hardware.oemservice@1.0;
