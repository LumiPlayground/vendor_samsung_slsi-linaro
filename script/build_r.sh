#!/bin/bash

####### Functions for Print #######
color_title=$'\E'"[0;33m"
color_success=$'\E'"[0;32m"
color_error=$'\E'"[0;31m"
color_reset=$'\E'"[00m"

build_carrier=("chnopen" "cmcc" "europen" "vzw" "tmo" "att" "spr" "ctc" "cu" "kddi" "ntt" "global")

function elapsed_time() {
  let "ELAPSED_TIME=$END_TIME-$START_TIME"
  echo "${color_success}[BUILD TOOL]${color_reset} $FUNC: ${color_title}$ELAPSED_TIME seconds${color_reset}"
}

function print_title() {
  echo
  echo "[[[[[[[${color_title} $TITLE ${color_reset}]]]]]]]"
}

function print_error() {
  echo ${color_error}ERROR${color_reset}: $1
}

function print_warning() {
  echo ${color_title}WARNING${color_reset}: $1
}

####### Arguments Check-up #######
function print_usage_exit () {
  print_error "$1"
  echo 'Usage: ./build.sh <PRODUCT> [ eng | userdebug | user ] [ "" | bootimage | vendorbootimage | host-tools | product | platform ] [ <TARGET_BUILD_CARRIER> ]'
  echo "                Available TARGET_BUILD_CARRIER : ${build_carrier[@]}"
  exit 2
}

function array_contains () {
  local seeking=$1; shift
  local in=1
  for element; do
    if [[ $element == $seeking ]]; then
      in=0
      break
    fi
  done
  echo $in
}

if [ $# -lt 1 ]; then
  print_usage_exit "need at least two arguments"
fi

####### Environment setup #######
if [ "z${CPU_JOB_NUM}" == "z" ] ; then
  CPU_JOB_NUM=$(grep processor /proc/cpuinfo | awk '{field=$NF};END{print (field+1)/2}')
fi
CLIENT=$(whoami)

ROOT_DIR=$(pwd)

BUILDSH=vendor/samsung_slsi/script/build.sh

BUILD_PRODUCT=$1
BUILD_VARIANT=$2
BUILD_OPTION=$3
BUILD_OPTION2=$4

if [ -z $OUT_DIR ]; then
  OUT_DIR="out_${1}_${2}"
fi

ESSI_SYSTEM_IMG=""
AVBTOOL=$ROOT_DIR/external/avb/avbtool

LAUNCHING_VERSION=$(echo "$BUILD_PRODUCT" | grep -e "_r" -e "_s" -e "_t")

if [ "${LAUNCHING_VERSION}" != "" ]; then
  BUILDSH=vendor/samsung_slsi/script/build_r.sh
fi

if [ -z $BUILD_VARIANT ]; then
  BUILD_VARIANT=eng
fi

# Load Mali on Keystone module
. vendor/samsung_slsi/script/mali_helper.sh

# When we make symbolic link for product_$BUILD_PRODUCT or product,
# then it must be PRODUCT_MANIFEST. So reorder for that.
if [ -d "$ROOT_DIR/product_$BUILD_PRODUCT" ]; then
  PRODUCT_MANIFEST=$ROOT_DIR/product_$BUILD_PRODUCT
elif [ -d "$ROOT_DIR/product" ]; then
  PRODUCT_MANIFEST=$ROOT_DIR/product
elif [ -d "$ROOT_DIR/products/$BUILD_PRODUCT" ]; then
  PRODUCT_MANIFEST=$ROOT_DIR/products/$BUILD_PRODUCT
fi

TITLE="Exynos Product Folder Name is $PRODUCT_MANIFEST"
print_title

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

####### Clean up Sequences #######
function clean_dist()
{
  TITLE="Clean Main DIST"
  print_title
  echo "rm -rf $DIST"
  rm -rf $DIST
  mkdir -p $DIST
}

function clean_kernel()
{
  TITLE="Clean kernel"
  print_title
  PATH=$KERNEL_GCC_PATH:$PATH
  CROSS_COMPILE=aarch64-linux-android-
  echo "make -C $TARGET_KERNEL -j distclean"
  make -C $TARGET_KERNEL -j distclean
}

function clean_android()
{
  TITLE="Clean android platform"
  print_title
  echo "rm -rf out$1/target/product/*/system"
  rm -rf out$1/target/product/*/system
  echo "rm -rf out$1/target/product/*/vendor"
  rm -rf out$1/target/product/*/vendor
  echo "rm -rf out$1/target/product/*/ramdisk"
  rm -rf out$1/target/product/*/ramdisk
  echo "rm -rf out$1/target/product/*/vendor-ramdisk"
  rm -rf out$1/target/product/*/vendor-ramdisk
  echo "rm -rf out$1/target/product/*/vendor_debug_ramdisk"
  rm -rf out$1/target/product/*/vendor_debug_ramdisk
  echo "rm -rf out$1/target/product/*/recovery"
  rm -rf out$1/target/product/*/recovery
  echo "rm -rf out$1/target/product/*/root"
  rm -rf out$1/target/product/*/root
  echo "rm -rf out$1/target/product/*/*.img"
  rm -rf out$1/target/product/*/*.img
  echo "rm -rf out$1/target/product/*/kernel"
  rm -rf out$1/target/product/*/kernel
  echo "rm -rf out$1/target/product/*/ff_*"
  rm -rf out$1/target/product/*/ff_*
  echo "rm -rf out$1/target/product/*/obj/FAKE/com.android.art_intermediates"
  rm -rf out$1/target/product/*/obj/FAKE/com.android.art_intermediates
}

if [ "clean" == "$1" ]; then
  clean_dist
  clean_kernel
  clean_android "*"
  exit 0
fi

req_build_variant=(eng user userdebug)
if [ $(array_contains $2 "${req_build_variant[@]}") == 1 ]; then
  print_usage_exit "\"$2\" is not a build variant"
fi


####### Error Conditions #######
function check_exit()
{
  RET=$?
  if [ $RET != 0 ]
  then
    exit $RET
  fi
}

function check_kernel()
{
  if [ ! -d $TARGET_KERNEL ]; then
    print_error "$TARGET_KERNEL: No kernel source"
    exit 2
  fi

  if [ ! -f $KERNEL_CONFIG ]; then
    if [ -f $KERNEL_SUBCONFIG ]; then
      KERNEL_CONFIG=$KERNEL_SUBCONFIG
    else
      if [ -f $EXTERNAL_KERNEL_CONFIG ]; then
        KERNEL_CONFIG=$EXTERNAL_KERNEL_CONFIG
      else
        print_error "$KERNEL_CONFIG: No kernel config"
        print_error "$KERNEL_SUBCONFIG: No kernel subconfig"
        print_error "$EXTERNAL_KERNEL_CONFIG: No kernel subconfig"
        exit 2
      fi
    fi
  fi
}

function check_environment()
{
  if [[ ! -z "${TARGET_PRODUCT}" ]]; then
    print_error "\"build/envsetup.sh\" and \"lunch\" should not be executed. Please re-create your shell"
    exit 2
  fi
}

if [ "essi" != "$BUILD_PRODUCT" ]; then
  check_kernel
fi
check_environment
if [ "essi" != "$BUILD_PRODUCT" ]; then
$PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT check
check_exit
fi

####### Collection Sequencess #######
function copy_binlist()
{
  IFS=' ' read -ra BINARR <<< $(echo "$BINLIST" | tr "\n" " ")
  for bin in "${BINARR[@]}"; do
    IFS=':' read -ra split <<< $bin
    if [ ! -f $BINBASE/${split[0]} ]; then
      print_error "$BINBASE/${split[0]}: No such file"
      exit 2
    fi
    cp -f $BINBASE/${split[0]} $DIST/${split[1]}
  done
}

function copy_misc_bin()
{
  TITLE="Copy Product DIST to Main DIST "
  print_title
  echo
  echo $DIST
  ls -la $DIST
}

####### Build Sequencess #######

function build_kernel()
{
  WITHOUT_SIGN=1 $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT kernel $BUILD_VARIANT
  check_exit

  $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT copy_modules $BUILD_VARIANT $DIST


  echo "SUPPORT_ATP=$SUPPORT_ATP" > $DIST/build_info.txt
}

function build_host_tools()
{
  local tools=("lpmake" "merge_target_files")
  local not_build=true
  local tool_list=""

  for tool in "${tools[@]}"; do
    if [ -z $(which ${HOST_TOOL_DIR}/$tool) ]; then
      not_build=false
      break
    fi
  done

  if [ "${SKIP_HOST_TOOLS_BUILD}" == "true" ] && [ $not_build == true ] ; then
    print_warning "Host tools bins exist:[$not_build] and SKIP_HOST_TOOLS_BUILD:[${SKIP_HOST_TOOLS_BUILD}]"
    print_warning "Skip host tools build. no need to build a new one."
    return
  fi

  TITLE="Build Host Tools"
  print_title
  START_TIME=`date +%s`
  echo "source build/envsetup.sh"
  source build/envsetup.sh
  echo

  local tool_list=""
  for list in "${tools[@]}"; do
    tool_list+="$list "
  done

  setup_mali_build_host_tools

  echo "OUT_DIR=host_tools make $tool_list -j$CPU_JOB_NUM"
  OUT_DIR=host_tools make $tool_list -j$CPU_JOB_NUM
  echo
  check_exit

  END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  elapsed_time
}

function build_essi()
{
  TITLE="Build ESSI"
  print_title
  START_TIME=`date +%s`
  echo "source build/envsetup.sh"
  source build/envsetup.sh
  echo
  echo "lunch essi-$BUILD_VARIANT"
  OUT_DIR=out_essi_$BUILD_VARIANT lunch essi-$BUILD_VARIANT > /dev/null
  check_exit
  echo
  echo "make dist -j$CPU_JOB_NUM"
  echo
  OUT_DIR=out_essi_$BUILD_VARIANT make dist -j$CPU_JOB_NUM
  check_exit
  PRODUCT_OUT=$(OUT_DIR=out_essi_$BUILD_VARIANT get_build_var PRODUCT_OUT)
  ESSI_SYSTEM_IMG=$PRODUCT_OUT/system.img
  ls -l $ESSI_SYSTEM_IMG 2> /dev/null
  END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  elapsed_time
}

function merge_target_files()
{
  echo "Merge target files"
  FRAMEWORK_TARGET_FILES=$(ls out_essi_$BUILD_VARIANT/dist/essi-target_files*.zip)
  VENDOR_TARGET_FILES=$(ls $OUT_DIR/dist/full_$BUILD_PRODUCT-target_files*.zip)
  echo "$MERGE_TARGER_FILES --framework-target-files $FRAMEWORK_TARGET_FILES"
  echo "                    --vendor-target-files $VENDOR_TARGET_FILES"
  echo "                    --output-target-files $PRODUCT_OUT/merged-target-files.zip"
  echo "                    --framework-misc-info-keys device/samsung/essi/merge_config_system_misc_info_keys"
  echo "                    --framework-item-list device/samsung/essi/merge_config_system_item_list"
  echo "                    --vendor-item-list device/samsung/essi/merge_config_other_item_list"
  echo "                    --logfile $PRODUCT_OUT/merged-target-files.log"
  $MERGE_TARGER_FILES  --framework-target-files $FRAMEWORK_TARGET_FILES \
                       --vendor-target-files $VENDOR_TARGET_FILES \
		       --output-target-files $PRODUCT_OUT/merged-target-files.zip \
		       --framework-misc-info-keys device/samsung/essi/merge_config_system_misc_info_keys \
		       --framework-item-list device/samsung/essi/merge_config_system_item_list \
		       --vendor-item-list device/samsung/essi/merge_config_other_item_list \
		       --logfile $PRODUCT_OUT/merged-target-files.log
}

function create_vbmeta()
{
  echo "Build vbmeta"
  BOARD_AVB_ALGORITHM=$(OUT_DIR=$OUT_DIR get_build_var BOARD_AVB_ALGORITHM)
  BOARD_AVB_KEY_PATH=$(OUT_DIR=$OUT_DIR get_build_var BOARD_AVB_KEY_PATH)
  echo "ESSI: $ESSI_SYSTEM_IMG"
  echo "Partial Board Build: $PRODUCT_OUT"
  echo "Signing algorithm: $BOARD_AVB_ALGORITHM"
  echo "Key path: $BOARD_AVB_KEY_PATH"
  $AVBTOOL make_vbmeta_image \
	  --output $PRODUCT_OUT/vbmeta.img \
	  --key $BOARD_AVB_KEY_PATH \
	  --algorithm $BOARD_AVB_ALGORITHM \
	  --include_descriptors_from_image $ESSI_SYSTEM_IMG \
	  --include_descriptors_from_image $PRODUCT_OUT/vendor.img \
	  --include_descriptors_from_image $PRODUCT_OUT/boot.img \
	  --include_descriptors_from_image $PRODUCT_OUT/dtbo.img
}

function create_super()
{
  BOARD_SUPER_PARTITION_SIZE=$(OUT_DIR=$OUT_DIR get_build_var BOARD_SUPER_PARTITION_SIZE)
  BOARD_GROUP_BASIC_SIZE=$(OUT_DIR=$OUT_DIR get_build_var BOARD_GROUP_BASIC_SIZE)
  PRODUCT_USE_VIRTUAL_AB=$(OUT_DIR=$OUT_DIR get_build_var PRODUCT_USE_VIRTUAL_AB)
  SUPER_ENV=$PRODUCT_OUT/super_env
  echo "use_dynamic_partitions=true"                          >  $SUPER_ENV
  echo "lpmake=$LPMAKE"                                       >> $SUPER_ENV
  echo "build_super_partition=true"                           >> $SUPER_ENV
  echo "super_metadata_device=super"                          >> $SUPER_ENV
  echo "super_block_devices=super"                            >> $SUPER_ENV
  echo "super_super_device_size=$BOARD_SUPER_PARTITION_SIZE"  >> $SUPER_ENV
  echo "dynamic_partition_list=system vendor"                 >> $SUPER_ENV
  echo "super_partition_groups=group_basic"                   >> $SUPER_ENV
  echo "super_group_basic_group_size=$BOARD_GROUP_BASIC_SIZE" >> $SUPER_ENV
  echo "super_group_basic_partition_list=system vendor"       >> $SUPER_ENV
#  echo "system_image=$ESSI_SYSTEM_IMG"                        >> $SUPER_ENV
  echo "system_image=$PRODUCT_OUT/system.img"                 >> $SUPER_ENV
  echo "vendor_image=$PRODUCT_OUT/vendor.img"                 >> $SUPER_ENV
  if [ "$PRODUCT_USE_VIRTUAL_AB" == "true" ]; then
    echo "ab_update=true"                                     >> $SUPER_ENV
    echo "virtual_ab=true"                                    >> $SUPER_ENV
  fi

  $BUILD_SUPER_IMAGE -v \
        $PRODUCT_OUT/super_env \
        $PRODUCT_OUT/super.img
}

function build_chub()
{
  if [[ $1 == 1 ]]
  then
    echo "***********************************************************************"
    echo "source build/envsetup.sh"
    source build/envsetup.sh
    echo
    echo "lunch full_$BUILD_PRODUCT-$BUILD_VARIANT"
    OUT_DIR=$OUT_DIR lunch full_$BUILD_PRODUCT-$BUILD_VARIANT > /dev/null
    echo "***********************************************************************"
  fi

  NANOHUB_TARGET=$(OUT_DIR=$OUT_DIR get_build_var NANOHUB_TARGET)
  NANOHUB_PATH=$(OUT_DIR=$OUT_DIR get_build_var NANOHUB_PATH)/firmware
  TARGET_DEVICE=$(OUT_DIR=$OUT_DIR get_build_var TARGET_DEVICE)

  TITLE="Build CHUB $NANOHUB_TARGET"
  print_title

  CHUB_START_TIME=`date +%s`

  if [[ "$NANOHUB_TARGET" == "" || "$NANOHUB_PATH" == "" ]]
  then
    echo "***********************************************************************"
    echo " >>> warning : nanohub build target or nanohub project path is not specified"
    echo " >>> NANOHUB_TARGET = $NANOHUB_TARGET"
    echo " >>> NANOHUB_PATH = $NANOHUB_PATH"
    echo "***********************************************************************"
    return 1
  fi

  if [ ! -f $NANOHUB_PATH/build.sh ]
  then
    echo "***********************************************************************"
    echo " >>> warning: $NANOHUB_PATH/build.sh does not exist !!!"
    echo "***********************************************************************"
    return 2
  fi

  if [[ $1 == 0 ]]
  then
    export NANOHUB_FIRMWARE_PATH=$ROOT_DIR/device/samsung/$TARGET_DEVICE/firmware
  fi

  echo "*****************************************"
  echo "PATH = $NANOHUB_PATH"
  echo "TARGET = $NANOHUB_TARGET"
  echo "FIRMWARE PATH = $NANOHUB_FIRMWARE_PATH"
  echo "*****************************************"
  export NANOHUB_TOOLCHAIN=$ROOT_DIR/prebuilts/gcc/linux-x86/arm/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-
  export CROSS_COMPILE=$ROOT_DIR/prebuilts/gcc/linux-x86/arm/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-
  export ARM_NONE_GCC_PATH=$ROOT_DIR/prebuilts/gcc/linux-x86/arm/gcc-arm-none-eabi-5_3-2016q1
  echo "NANOHUB_TOOLCHAIN=$NANOHUB_TOOLCHAIN"
  echo "CROSS_COMPILE=$CROSS_COMPILE"
  echo "ARM_NONE_GCC_PATH=$ARM_NONE_GCC_PATH"
  echo "*****************************************"

  pushd $NANOHUB_PATH
  ./build.sh $NANOHUB_TARGET BUILD_VARIANT=$BUILD_VARIANT
  popd

  unset NANOHUB_TOOLCHAIN
  unset CROSS_COMPILE
  unset ARM_NONE_GCC_PATH

  CHUB_END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  let "ELAPSED_TIME=$CHUB_END_TIME-$CHUB_START_TIME"
  echo "${color_success}[BUILD TOOL]${color_reset} $FUNC: ${color_title}$ELAPSED_TIME seconds${color_reset}"
}

function build_android()
{
  if [ "$ESSI_VENDOR_BUILD" == "true" ] ; then
    WITH_ESSI=true
    export WITH_ESSI=true
  fi
  if [ "$WITH_ESSI" == "true" ] && [ "$ESSI_VENDOR_BUILD" != "true" ] ; then
    build_essi
  fi
  TITLE="Build android platform $PLATFORM_BUILD_TARGET"
  print_title
  START_TIME=`date +%s`
  echo "source build/envsetup.sh"
  source build/envsetup.sh
  echo
  if [ "essi" != "$BUILD_PRODUCT" ]; then
    echo "lunch full_$BUILD_PRODUCT-$BUILD_VARIANT"
    OUT_DIR=$OUT_DIR lunch full_$BUILD_PRODUCT-$BUILD_VARIANT > /dev/null
  else
    echo "lunch $BUILD_PRODUCT-$BUILD_VARIANT"
    OUT_DIR=$OUT_DIR lunch $BUILD_PRODUCT-$BUILD_VARIANT > /dev/null
  fi
  check_exit
  if [ "$PLATFORM_BUILD_TARGET" != "bootimage" ] && [ "$PLATFORM_BUILD_TARGET" != "vendorbootimage" ]; then
  build_chub 0
  fi

  echo
  echo "make -j$CPU_JOB_NUM $PLATFORM_BUILD_TARGET"
  echo
  MAKE_DIST=$(OUT_DIR=$OUT_DIR get_build_var PRODUCT_USE_DYNAMIC_PARTITIONS)
  if [ "$MAKE_DIST" == "true" ] || [ "essi" == "$BUILD_PRODUCT" ] || [ "$WITH_ESSI" == "true" ] || [ "$ESSI_VENDOR_BUILD" == "true" ] ; then
    if [ "$PLATFORM_BUILD_TARGET" != "bootimage" ] && [ "$PLATFORM_BUILD_TARGET" != "vendorbootimage" ] ; then
      PLATFORM_BUILD_TARGET=dist
    fi
  fi
  OUT_DIR=$OUT_DIR make -j$CPU_JOB_NUM $PLATFORM_BUILD_TARGET
  check_exit
  PRODUCT_OUT=$(OUT_DIR=$OUT_DIR get_build_var PRODUCT_OUT)
  if [ "$PLATFORM_BUILD_TARGET" == "bootimage" ] ; then
    cp -f $DIST/* $PRODUCT_OUT/
  fi
  if [ "$PLATFORM_BUILD_TARGET" == "vendorbootimage" ] ; then
    cp -f $DIST/* $PRODUCT_OUT/
  fi
  if [ "$PLATFORM_BUILD_TARGET" == "dist" ] ; then
    unzip -o $OUT_DIR/dist/full_$BUILD_PRODUCT-img* -d $PRODUCT_OUT/
    if  [ "$WITH_ESSI" != "true" ] && [ "essi" != "$BUILD_PRODUCT" ] ; then
      cp -f $OUT_DIR/dist/super.img $PRODUCT_OUT/
    fi
  fi
  # if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
  #   TARGET_OUT_INTERMEDIATES=$(OUT_DIR=$OUT_DIR get_build_var TARGET_OUT_INTERMEDIATES)
  #   mv $PRODUCT_MANIFEST/KERNEL_OBJ $TARGET_OUT_INTERMEDIATES
  # fi
  echo "$PRODUCT_OUT"
  ls -l $PRODUCT_OUT/*.img $PRODUCT_OUT/ff_* 2> /dev/null
  if [ "$WITH_ESSI" == "true" ] && [ "essi" != "$BUILD_PRODUCT" ] ; then
    merge_target_files
    check_exit
    unzip -jo $PRODUCT_OUT/merged-target-files.zip 'IMAGES/*.img' -d $PRODUCT_OUT
#    create_vbmeta
    check_exit
    create_super
    check_exit
  fi
  END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  elapsed_time
}

function audit()
{
  TITLE="launch audit2allow, need audit denined log file name is audit.log and policy file"
  print_title
  START_TIME=`date +%s`
  echo "source build/envsetup.sh"
  source build/envsetup.sh
  echo
  echo "lunch full_$BUILD_PRODUCT-$BUILD_VARIANT"
  OUT_DIR=$OUT_DIR lunch full_$BUILD_PRODUCT-$BUILD_VARIANT &> /dev/null
  cat audit.log | audit2allow -p policy
  check_exit
  END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  elapsed_time
}

function build_product()
{
  $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT
  check_exit
  cp $PRODUCT_DIST/* $DIST

  echo "SUPPORT_ATP=$SUPPORT_ATP" > $DIST/build_info.txt
}

function build_all()
{
    START_TIME=`date +%s`
    clean_dist
    clean_android "_${BUILD_PRODUCT}_${BUILD_VARIANT}"
    if [ "essi" != "$BUILD_PRODUCT" ]; then
      $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT product
      check_exit
      $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT kernel
      check_exit
      copy_misc_bin
    fi
    $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT platform $TARGET_BUILD_CARRIER
    check_exit
    END_TIME=`date +%s`
    FUNC="Total"
    elapsed_time
}

export TARGET_BUILD_CARRIER=
if [ $(array_contains $BUILD_OPTION2 "${build_carrier[@]}") == 0 ]; then
    export TARGET_BUILD_CARRIER=$BUILD_OPTION2
    echo "1.TARGET_BUILD_CARRIER is $BUILD_OPTION2: $TARGET_BUILD_CARRIER"
fi

####### Main Routines #######
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
    START_TIME=`date +%s`
    clean_dist
    clean_android "_${BUILD_PRODUCT}_${BUILD_VARIANT}"
    $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT kernel
    check_exit
    copy_misc_bin
    PLATFORM_BUILD_TARGET=vendorbootimage $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT platform
    END_TIME=`date +%s`
    FUNC="Total"
    elapsed_time
    ;;
  bootimage)
    START_TIME=`date +%s`
    clean_dist
    clean_android "_${BUILD_PRODUCT}_${BUILD_VARIANT}"
    $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT kernel
    check_exit
    copy_misc_bin
    PLATFORM_BUILD_TARGET=bootimage $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT platform
    END_TIME=`date +%s`
    FUNC="Total"
    elapsed_time
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
