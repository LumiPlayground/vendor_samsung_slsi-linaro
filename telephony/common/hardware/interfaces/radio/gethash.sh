#!/bin/bash

options="-r vendor.samsung_slsi.telephony.hardware:vendor/samsung_slsi/telephony/common/hardware/interfaces \
         -r android.hidl:system/libhidl/transport \
         -r android.hardware:hardware/interfaces"

hidl-gen -Lhash $options vendor.samsung_slsi.telephony.hardware.radio@1.0;
hidl-gen -Lhash $options vendor.samsung_slsi.telephony.hardware.radio@1.1;
hidl-gen -Lhash $options vendor.samsung_slsi.telephony.hardware.radio@1.2;
