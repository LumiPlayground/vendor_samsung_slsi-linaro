source $ANDROID_BUILD_TOP/system/tools/hidl/update-makefiles-helper.sh

PACKAGE=vendor.samsung_slsi.hardware.audio_dump@1.0
LOC=vendor/samsung_slsi/hardware/audio_dump/1.0/default

options="-r vendor.samsung_slsi.hardware:vendor/samsung_slsi/hardware \
         -r android.hidl:system/libhidl/transport \
         -r android.hardware:hardware/interfaces"

# Generate Android.bp for .hal files
hidl-gen -o $LOC -L c++-impl $options $PACKAGE;
hidl-gen -o $LOC -L androidbp-impl $options $PACKAGE;
#hidl-gen -o $LOC -L java $options $PACKAGE;
