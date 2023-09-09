source $ANDROID_BUILD_TOP/system/tools/hidl/update-makefiles-helper.sh

PACKAGE=vendor.samsung_slsi.hardware.memlogservice@1.0
LOC=vendor/samsung_slsi/hardware/memlogservice/1.0/default

options="-r vendor.samsung_slsi.hardware:vendor/samsung_slsi/hardware \
         -r android.hidl:system/libhidl/transport \
         -r android.hardware:hardware/interfaces"

hidl-gen -Landroidbp $options $PACKAGE;
