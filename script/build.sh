#!/bin/bash

set -e

ROOT_DIR=$(pwd)
BUILD_SCRIPT_BASE=${ROOT_DIR}/vendor/samsung_slsi/script

# Include build helper functions
source "${BUILD_SCRIPT_BASE}/build_util.sh"

if [ $# -lt 1 ]; then
  print_usage_exit "need at least two arguments"
fi

# Set build env variables
source "${BUILD_SCRIPT_BASE}/build_env.sh"

source "${BUILD_SCRIPT_BASE}/build_core.sh"
source "${BUILD_SCRIPT_BASE}/build_dpm.sh"
source "${BUILD_SCRIPT_BASE}/build_chub.sh"
source "${BUILD_SCRIPT_BASE}/build_ispcpu.sh"

if [ "clean" == "$1" ]; then
  clean_dist
  clean_kernel
  clean_android "*"
  exit 0
fi

check_kernel
check_environment
#TODO : is it necessary?
check_prod_build

if [ "essi" != "$BUILD_PRODUCT" ]; then
  $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT check
  check_exit
fi

####### Main Routines #######
print_info "[Only Info For debugging] -- Start Main Routines with ${BUILD_OPTION}"
case "$BUILD_OPTION" in
  platform)
    build_android
    echo "TARGET_BUILD_CARRIER is $TARGET_BUILD_CARRIER"
    ;;
  host-tools)
    build_host_tools
    ;;
  product)
    build_product
    ;;
  kernel)
    $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT host-tools
    build_kernel
    ;;
  vendorbootimage)
    build_bootimage "vendorbootimage"
    ;;
  bootimage)
    build_bootimage "bootimage"
   ;;
  dpm)
    if [ "${BUILD_DPM_BINARY_FILE}" == "" ]; then
      exit 1
    fi
    build_dpm
    ;;
  audit)
    audit
    ;;
  mali)
    build_mali_ddk
    ;;
  clean)
    clean_dist
    ;;
  chub)
    build_chub 1
    ;;
  ispcpu)
    build_ispcpu 1
    ;;
#### Work as BUILD_CARRIER ####
  "")
    build_all
    ;;
  *)
    if [ $(array_contains $BUILD_OPTION "${build_carrier[@]}") == 0 ]; then
        export TARGET_BUILD_CARRIER=$BUILD_OPTION
        echo "2.TARGET_BUILD_CARRIER is $BUILD_OPTION : $TARGET_BUILD_CARRIER"
        build_all
        exit 0
    fi
    print_usage_exit "$BUILD_OPTION is not vaild build option"
esac

exit 0
