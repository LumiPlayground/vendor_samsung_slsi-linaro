#!/bin/bash

#
# Build argument & Environment vairables
#
print_info "Set build_env with args: $@"

if [ "z${CPU_JOB_NUM}" == "z" ] ; then
  CPU_JOB_NUM=$(grep processor /proc/cpuinfo | awk '{field=$NF};END{print (field+1)/2}')
fi

BUILDSH=${BUILD_SCRIPT_BASE}/build.sh
BUILD_PRODUCT=$1
BUILD_VARIANT=$2
BUILD_OPTION=$3
BUILD_OPTION2=$4

if [ -z $OUT_DIR ]; then
  OUT_DIR="out_${1}_${2}"
fi

ESSI_SYSTEM_IMG=""
AVBTOOL=$ROOT_DIR/external/avb/avbtool

#LAUNCHING_VERSION=$(echo "$BUILD_PRODUCT" | grep -e "_r" -e "_s" -e "_t")

if [ -n "${BUILD_VARIANT}" ]; then
  req_build_variant=(eng user userdebug)
  if [ $(array_contains $2 "${req_build_variant[@]}") == 1 ]; then
    print_error "\"$2\" is not a build variant"
  fi
else
  BUILD_VARIANT="eng"
fi

# Load Mali on Keystone module
if [ -f ${BUILD_SCRIPT_BASE}/mali_helper.sh ]; then
    set +e
    . vendor/samsung_slsi/script/mali_helper.sh
    set -e
fi

# When we make symbolic link for product_$BUILD_PRODUCT or product,
# then it must be PRODUCT_MANIFEST. DO NOT MODIFY comparing sequence.
if [ -d "$ROOT_DIR/product_$BUILD_PRODUCT" ]; then
  PRODUCT_MANIFEST=$ROOT_DIR/product_$BUILD_PRODUCT
elif [ -d "$ROOT_DIR/product" ]; then
  PRODUCT_MANIFEST=$ROOT_DIR/product
elif [ -d "$ROOT_DIR/products/$BUILD_PRODUCT" ]; then
  PRODUCT_MANIFEST=$ROOT_DIR/products/$BUILD_PRODUCT
fi
print_title "Exynos Product Folder Name is $PRODUCT_MANIFEST"

IFS='_' read -ra PNAME <<< $(echo "$BUILD_PRODUCT")
PNAME=${PNAME[0]}

TARGET_KERNEL=$PRODUCT_MANIFEST/kernel
PRODUCT_TOOLCHAIN_BASE=$PRODUCT_MANIFEST/toolchain

KERNEL_CONFIG=$TARGET_KERNEL/build.config.$BUILD_PRODUCT
SUBCONFIG_BASE=$PRODUCT_MANIFEST/script/configs
KERNEL_SUBCONFIG=$SUBCONFIG_BASE/kernel.build.config.$BUILD_PRODUCT

EXTERNAL_KERNEL_CONFIG=$PRODUCT_MANIFEST/external/kernel/config/${PNAME}/build_config/build.config.$BUILD_PRODUCT
KERNEL_CLANG_BASE=$ROOT_DIR/prebuilts/clang/host/linux-x86
KERNEL_GCC_PATH=$ROOT_DIR/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin
PRIBUILT_DTC_PATH=$ROOT_DIR/prebuilts/misc/linux-x86/dtc
KERNEL_MERGE_CONFIG=$TARGET_KERNEL/scripts/kconfig/merge_config.sh
BUILD_SUPER_IMAGE=$ROOT_DIR/build/make/tools/releasetools/build_super_image.py
HOST_TOOL_DIR=$ROOT_DIR/host_tools/host/linux-x86/bin
MKDTIMG=$ROOT_DIR/prebuilts/misc/linux-x86/libufdt/mkdtimg
LPMAKE=$HOST_TOOL_DIR/lpmake
UFDT_APPLY_OVERLAY=$ROOT_DIR/prebuilts/misc/linux-x86/libufdt/ufdt_apply_overlay
MERGE_TARGER_FILES=$HOST_TOOL_DIR/merge_target_files

DIST=$ROOT_DIR/device/samsung/${BUILD_PRODUCT}-prebuilts
PRODUCT_DIST=$PRODUCT_MANIFEST/out_$BUILD_PRODUCT
export TARGET_BUILD_CARRIER=
if [ $(array_contains $BUILD_OPTION2 "${build_carrier[@]}") == 0 ]; then
    export TARGET_BUILD_CARRIER=$BUILD_OPTION2
    echo "1.TARGET_BUILD_CARRIER is $BUILD_OPTION2: $TARGET_BUILD_CARRIER"
fi
