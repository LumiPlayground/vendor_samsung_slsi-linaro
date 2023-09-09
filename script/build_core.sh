#!/bin/bash

#
# Clean up Sequences
#

function clean_dist()
{
  print_title "Clean Main DIST"
  echo "rm -rf $DIST"
  rm -rf $DIST
  mkdir -p $DIST
}

function clean_kernel()
{
  print_title "Clean kernel"
  PATH=$KERNEL_GCC_PATH:$PATH
  CROSS_COMPILE=aarch64-linux-android-
  echo "make -C $TARGET_KERNEL -j distclean"
  make -C $TARGET_KERNEL -j distclean
}

function clean_android()
{
  print_title "Clean android platform"
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

#
# Check functions
#

function check_environment()
{
  if [[ ! -z "${TARGET_PRODUCT}" ]]; then
    print_error "\"build/envsetup.sh\" and \"lunch\" should not be executed. Please re-create your shell"
    exit 2
  fi
}

function check_kernel()
{
  if [ "essi" == "$BUILD_PRODUCT" ]; then
    print_info "BUILD for essi. Skipping check_kernel"

  elif [ ! -d $TARGET_KERNEL ]; then
    print_error "$TARGET_KERNEL: No kernel source"
    exit 2

  elif [ ! -f $KERNEL_CONFIG ]; then
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

function check_prod_build
{
  if [ "essi" != "$BUILD_PRODUCT" ]; then
    $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT check
    check_exit
  fi
}

#
# Kernel Build functions
#

function build_defconfig()
{
  print_title "Build kernel(1) defconfig"

  KERNEL_DEFCONFIG_PATH=$KERNEL_CONFIG_BASE/$DEFCONFIG
  KERNEL_DEFCONFIG_BASE=$KERNEL_CONFIG_BASE/${TARGET_SOC}-base_defconfig
  KERNEL_GKI_DEFCONFIG_PATH=$KERNEL_CONFIG_BASE/$GKI_DEFCONFIG
  if [ ! -f $KERNEL_DEFCONFIG_BASE ]; then
    KERNEL_DEFCONFIG_BASE=""
  fi

  if [ -z $GKI_DEFCONFIG ]; then
    KERNEL_GKI_DEFCONFIG_PATH=""
  fi

  if [ "eng" == "$BUILD_VARIANT" ]; then
    KERNEL_USER_CFG=""
  elif [ "userdebug" == "$BUILD_VARIANT" ]; then
    KERNEL_USER_CFG=$KERNEL_CONFIG_BASE/${TARGET_SOC}_userdebug.cfg
  elif [ "user" == "$BUILD_VARIANT" ]; then
    KERNEL_USER_CFG=$KERNEL_CONFIG_BASE/${TARGET_SOC}_user.cfg
  fi

  if [ ! -z "${MALI_CONFIG}" ]; then
    KERNEL_MALI_CFG=${KERNEL_CONFIG_BASE}/${MALI_CONFIG}
  fi

  if [ "true" == "$MALI_ON_KEYSTONE" ]; then
    KERNEL_MALI_CFG=$KERNEL_CONFIG_BASE/mali_keystone.cfg
  fi

  if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
    echo "mkdir -p $KERNEL_OBJ"
    bash -c "mkdir -p $KERNEL_OBJ"
    MAKE_CONFIG_CMD="$KERNEL_MERGE_CONFIG -m -O $KERNEL_OBJ $KERNEL_GKI_DEFCONFIG_PATH $KERNEL_DEFCONFIG_BASE $KERNEL_DEFCONFIG_PATH $KERNEL_MALI_CFG $KERNEL_USER_CFG &> /dev/null;"
    MAKE_CONFIG_CMD="$MAKE_CONFIG_CMD make -C $TARGET_KERNEL O=$KERNEL_OBJ KCONFIG_ALLCONFIG=$KERNEL_OBJ/.config alldefconfig -j$CPU_JOB_NUM $CC_CLANG;"
  else
    MAKE_CONFIG_CMD="$KERNEL_MERGE_CONFIG -m -O $TARGET_KERNEL $KERNEL_GKI_DEFCONFIG_PATH $KERNEL_DEFCONFIG_BASE $KERNEL_DEFCONFIG_PATH $KERNEL_MALI_CFG $KERNEL_USER_CFG &> /dev/null;"
    MAKE_CONFIG_CMD="$MAKE_CONFIG_CMD make -C $TARGET_KERNEL KCONFIG_ALLCONFIG=.config alldefconfig -j$CPU_JOB_NUM $CC_CLANG;"
  fi

  echo "$MAKE_CONFIG_CMD"
  bash -c "$MAKE_CONFIG_CMD"
  check_exit
  echo
}

function check_defconfig()
{
  TITLE="Build kernel(1-1) check_defconfig"
  print_title
  KERNEL_DEFCONFIG_PATH=$KERNEL_CONFIG_BASE/$DEFCONFIG
  KERNEL_DEFCONFIG_BASE=$KERNEL_CONFIG_BASE/${TARGET_SOC}-base_defconfig
  KERNEL_GKI_DEFCONFIG_PATH=$KERNEL_CONFIG_BASE/$GKI_DEFCONFIG

  if [ -z $GKI_DEFCONFIG ]; then
    KERNEL_GKI_DEFCONFIG_PATH=""
  fi

  if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
    CHECK_CONFIG_CMD="make -C $TARGET_KERNEL O=$KERNEL_OBJ savedefconfig $CC_CLANG;"
    CHECK_DEFCONFIG="$KERNEL_OBJ/defconfig"
  else
    CHECK_CONFIG_CMD="make -C $TARGET_KERNEL O=$TARGET_KERNEL savedefconfig $CC_CLANG;"
    CHECK_DEFCONFIG="$TARGET_KERNEL/defconfig"
  fi
  if [ ! -f $KERNEL_GKI_DEFCONFIG_PATH ]; then
    if [ ! -f $KERNEL_DEFCONFIG_BASE ]; then
       if [ -f $KERNEL_USER_CFG ]; then
        echo Verifying that savedefconfig matches $KERNEL_DEFCONFIG_PATH
        bash -c "$CHECK_CONFIG_CMD"
        diff -uN $CHECK_DEFCONFIG $KERNEL_DEFCONFIG_PATH
        check_exit
      else
        echo "KERNEL_USER_CFG=$KERNEL_USER_CFG BUILD_VARIANT=$BUILD_VARIANT"
        echo "Skip verifying savedefconfig"
      fi
    else
      echo "KERNEL_DEFCONFIG_BASE=$KERNEL_DEFCONFIG_BASE is present"
      echo "Skip verifying savedefconfig"
    fi
  else
    echo "KERNEL_GKI_DEFCONFIG_BASE=$KERNEL_GKI_DEFCONFIG_PATH is present"
    echo "Skip verifying savedefconfig"
  fi
  echo
}

function build_kernel_core()
{
  TITLE="Build kernel(2)"
  print_title
  if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
    echo "make -C $TARGET_KERNEL -j$CPU_JOB_NUM O=$KERNEL_OBJ $CC_CLANG"
    make -C $TARGET_KERNEL -j$CPU_JOB_NUM O=$KERNEL_OBJ $CC_CLANG
    if [ ! -z ${BUILD_KERNEL_MODULES} ]; then
      echo "make INSTALL_MOD_STRIP=1 INSTALL_MOD_PATH=$MODULE_OBJ modules_install -C $TARGET_KERNEL -j$CPU_JOB_NUM O=$KERNEL_OBJ $CC_CLANG"
      make INSTALL_MOD_STRIP=1 INSTALL_MOD_PATH=$MODULE_OBJ modules_install -C $TARGET_KERNEL -j$CPU_JOB_NUM O=$KERNEL_OBJ $CC_CLANG
      MODULE_OBJ=$MODULE_OBJ/lib/modules/$(cat $KERNEL_OBJ/include/config/kernel.release)
    fi
  else
    echo "make -C $TARGET_KERNEL -j$CPU_JOB_NUM $CC_CLANG"
    make -C $TARGET_KERNEL -j$CPU_JOB_NUM $CC_CLANG
  fi
  check_exit
  echo
}

function build_dtbo()
{
  TITLE="Build kernel(3) dtbo"
  print_title
  if [ "None" == "${TARGET_DTBO_CFG}" ]; then
    return 0;
  fi
  if [ ! -z ${TARGET_DTBO_CFG} ]; then
    KERNEL_DTBO_CFG=$KERNEL_DTB_SOURCE_DIR/${TARGET_DTBO_CFG}
  else
    KERNEL_DTBO_CFG=$KERNEL_DTB_SOURCE_DIR/${TARGET_SOC}_dtboimg.cfg
  fi
  if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
    echo "$MKDTIMG cfg_create $KERNEL_OBJ/dtbo.img $KERNEL_DTBO_CFG -d $KERNEL_OBJ"
    $MKDTIMG cfg_create $KERNEL_OBJ/dtbo.img $KERNEL_DTBO_CFG -d $KERNEL_OBJ
  else
    echo "$MKDTIMG cfg_create $TARGET_KERNEL/dtbo.img $KERNEL_DTBO_CFG -d $TARGET_KERNEL"
    $MKDTIMG cfg_create $TARGET_KERNEL/dtbo.img $KERNEL_DTBO_CFG -d $TARGET_KERNEL
  fi
  check_exit
  echo
}

function check_dtbo_merge()
{
  TITLE="Build kernel(3-1) dtbo merge verification"
  print_title
  ## find dtb/dtbo file name in kernel build.config ##
  IFS=' ' read -ra BINARR <<< $(echo "$BINLIST" | tr "\n" " ")
  for bin in "${BINARR[@]}"; do
    IFS=':' read -ra split <<< $bin
    if [ ! -f $BINBASE/${split[0]} ]; then
      print_error "$BINBASE/${split[0]}: No such file"
      exit 2
    fi
    if [ "dtb.img" == "${split[1]}" ]; then
      KERNEL_DTB=$BINBASE/${split[0]}
    fi
    if [ "dtbo.img" == "${split[1]}" ]; then
      KERNEL_DTBO=$BINBASE/${split[0]}
    fi
  done

  ## error check ##
  if [ -z ${KERNEL_DTB} ]; then
      print_error "ERROR: Can not find dtb file name in build.config"
      exit 2
  fi

  if [ -z ${KERNEL_DTBO} ]; then
      print_error "ERROR: Can not find dtbo file name in build.config"
      exit 2
  fi

  ## delete pre dtbo_dump.* files ##
  rm -f $BINBASE/dtbo_dump.*

  ## dump dtbo entry files from dtbo.img ##
  echo "$MKDTIMG dump $KERNEL_DTBO -b $BINBASE/dtbo_dump"
  $MKDTIMG dump $KERNEL_DTBO -b $BINBASE/dtbo_dump > /dev/null
  check_exit

  ## verify to merge with dtb and dtbo ##
  local merge_ret
  local dtbo
  for dtbo in $(ls $BINBASE/dtbo_dump.*)
  do
    echo "$dtbo"
    merge_ret=$($UFDT_APPLY_OVERLAY $KERNEL_DTB $dtbo $dtbo.out 2>&1)
    echo  $merge_ret
    if [[ $merge_ret =~ "ERROR" ]]; then
      exit 2
    fi
  done

  ## clean dtbo_dump.* files ##
  rm -f $BINBASE/dtbo_dump.*
}

function build_kernel_with_make()
{
  check_kernel
  START_TIME=`date +%s`
  set -a
  . ${KERNEL_CONFIG}
  set +a
  KERNEL_CONFIG_BASE=$TARGET_KERNEL/arch/$ARCH/configs
  KERNEL_DTB_SOURCE_DIR=$TARGET_KERNEL/arch/$ARCH/boot/dts/exynos
  if [ ! -z ${BUILD_KERNEL_MODULES} ]; then
    if [ -z ${SEPERATE_KERNEL_OBJ} ]; then
      print_error "BUILD_KERNEL_MODULES depends on SEPERATE_KERNEL_OBJ."
      exit -2
    fi
  fi
  if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
    KERNEL_OBJ=$PRODUCT_MANIFEST/KERNEL_OBJ
    if [ ! -z ${BUILD_KERNEL_MODULES} ]; then
      MODULE_OBJ=$PRODUCT_MANIFEST/KERNEL_MODULES
    fi
  fi
  echo
  PATH=$KERNEL_GCC_PATH:$PATH
  if [ ! -z ${CLANG_PREBUILT_BIN} ]; then
    CLANG_VERSION=$(echo $CLANG_PREBUILT_BIN | grep -Eo 'clang-[a-z0-9]+')
  fi
  if [ ! -z ${CLANG_VERSION} ]; then
    CC_CLANG="CC=clang"
    if [ ! "z${KERNEL_LTO_ON_4_19}" == "z" ] ; then
      CC_CLANG="CC=clang LD=ld.lld"
    fi
    CLANG_CANDIDATE=$PRODUCT_TOOLCHAIN_BASE/$CLANG_VERSION
    if [ -d ${CLANG_CANDIDATE} ]; then
      PATH=$CLANG_CANDIDATE/bin:$PATH
    else
      CLANG_CANDIDATE=$KERNEL_CLANG_BASE/$CLANG_VERSION
      if [ -d ${CLANG_CANDIDATE} ]; then
        PATH=$CLANG_CANDIDATE/bin:$PATH
      else
        print_error "$PRODUCT_TOOLCHAIN_BASE/$CLANG_VERSION: No clang in product/toolchain"
        print_error "$KERNEL_CLANG_BASE/$CLANG_VERSION: No clang in platform/prebuilts"
        exit -2
      fi
    fi

    if [ ! "z${KERNEL_LTO_ON}" == "z" ] ; then
      export LLVM_AR=$CLANG_CANDIDATE/bin/llvm-ar
      export LLVM_DIS=$CLANG_CANDIDATE/bin/llvm-dis
      export LTO_LLVM_LIB_BASE=$CLANG_CANDIDATE/lib64/
    fi
  fi

  # Version 5.4 or later
  if [[ ! -z "${GKI_DEFCONFIG}" ]] || [[ ! -z "${SET_TOOLCHAIN}" ]] ; then
    if [ ! -z ${CC} ] ; then
      CC_CLANG="CC=${CC}"
    fi
    if [ ! -z ${LD} ] ; then
      CC_CLANG="LD=${LD} ${CC_CLANG}"
    fi
    if [ ! -z ${NM} ] ; then
      CC_CLANG="NM=${NM} ${CC_CLANG}"
    fi
    if [ ! -z ${OBJCOPY} ] ; then
      CC_CLANG="OBJCOPY=${OBJCOPY} ${CC_CLANG}"
    fi
  fi

  build_defconfig
  if [ ! -z ${POST_CHECK_DEFCONFIG} ]; then
    check_defconfig
  fi

  build_kernel_core
  build_dtbo

  export DPM_OUTPUT_PATH=${KERNEL_OBJ}/arch/arm64/boot/dts
  build_dpm

  BINLIST=$FILES
  if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
    BINBASE=$KERNEL_OBJ
    if [ ! -z ${BUILD_KERNEL_MODULES} ]; then
      echo "Copy kernel modules to DIST"
      mkdir -p $DIST/modules_symbols
      mkdir -p $DIST/modules
      find $KERNEL_OBJ -name \*.ko | xargs cp -t $DIST/modules_symbols
      find $MODULE_OBJ/kernel -name \*.ko | xargs cp -t $DIST/modules
      cp $MODULE_OBJ/modules.order $DIST/modules
      if [ ! -z ${VENDOR_KERNEL_MODULE_LIST} ]; then
        cp $TARGET_KERNEL/$VENDOR_KERNEL_MODULE_LIST $DIST/vendor_module_list.cfg
      fi
    fi
  else
    BINBASE=$TARGET_KERNEL
  fi
  if [ "z${SKIP_DTBO_MERGE_VERIFICATION}" == "z" ]; then
      check_dtbo_merge
  fi
  copy_binlist
  check_exit
  END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  elapsed_time
}

function build_kernel_with_gki_script()
{
  WITHOUT_SIGN=1 $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT kernel $BUILD_VARIANT
  check_exit

  $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT copy_modules $BUILD_VARIANT $DIST

  echo "SUPPORT_ATP=$SUPPORT_ATP" > $DIST/build_info.txt
}

function build_kernel()
{
  ## find dtb/dtbo file name in kernel build.config ##
  if [ -n "${KERNEL_CONFIG}" ]; then
    IFS=' ' read -ra BINARR <<< $(cat "$KERNEL_CONFIG" | tr "\n" " ")
    for bin in "${BINARR[@]}"; do
      IFS='=' read -ra split <<< $bin
      if [[ "BUILD_USE_GKI_SCRIPT" == ${split[0]} ]] && [[ "1" == ${split[1]} ]]; then
        BUILD_KERNLE_WITH_GKI_SCRIPT="1"
	print_info "Build kernel with GKI build script"
      fi
    done
  fi

  if [ "1" == "${BUILD_KERNLE_WITH_GKI_SCRIPT}" ]; then
    build_kernel_with_gki_script
  else
    build_kernel_with_make
  fi
}

#
# Android Build
#
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
    set +e
    build_chub 0
    build_ispcpu 0
    set -e
  fi

  echo
  echo "make -j$CPU_JOB_NUM $PLATFORM_BUILD_TARGET"
  echo
  MAKE_DIST=$(OUT_DIR=$OUT_DIR get_build_var PRODUCT_USE_DYNAMIC_PARTITIONS)
  # do not dist build if BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT
  MAKE_SUPER=$(OUT_DIR=$OUT_DIR get_build_var BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT)
  if [ "$MAKE_SUPER" == "true" ] ; then
    MAKE_DIST=false
  fi
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
  if [ "$PLATFORM_BUILD_TARGET" == "dist" ] && [ "essi" != "$BUILD_PRODUCT" ] ; then
    unzip -o $OUT_DIR/dist/full_$BUILD_PRODUCT-img* -d $PRODUCT_OUT/
    if  [ "$WITH_ESSI" != "true" ] ; then
      cp -f $OUT_DIR/dist/super.img $PRODUCT_OUT/
    fi
  fi
  # if [ ! -z ${SEPERATE_KERNEL_OBJ} ]; then
  #   TARGET_OUT_INTERMEDIATES=$(OUT_DIR=$OUT_DIR get_build_var TARGET_OUT_INTERMEDIATES)
  #   mv $PRODUCT_MANIFEST/KERNEL_OBJ $TARGET_OUT_INTERMEDIATES
  # fi
  echo "$PRODUCT_OUT"
  if [ "essi" != "$BUILD_PRODUCT" ]; then
    ls -l $PRODUCT_OUT/*.img $PRODUCT_OUT/ff_* 2> /dev/null
  else
    ls -l $PRODUCT_OUT/*.img 2> /dev/null
  fi	  
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

#
# Product Build
#

function build_product()
{
  $PRODUCT_MANIFEST/prod_build.sh $BUILD_PRODUCT
  check_exit
  cp $PRODUCT_DIST/* $DIST

  echo "SUPPORT_ATP=$SUPPORT_ATP" > $DIST/build_info.txt
}

#
# Partial build
#

function build_bootimage()
{
  START_TIME=`date +%s`
  clean_dist
  clean_android "_${BUILD_PRODUCT}_${BUILD_VARIANT}"
  $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT kernel
  check_exit
  copy_misc_bin
  dbg_pause "$1  bootimage build"
  PLATFORM_BUILD_TARGET="$1" $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT platform
  END_TIME=`date +%s`
  FUNC="Total"
  elapsed_time
}

#
# Full Build
#

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

    if [ "true" == "$MALI_ON_KEYSTONE" ] && [ "$BUILD_OPTION" == ""  ] ; then
	echo "skipped build_host_tools function call for mali keystone build option $BUILD_OPTION"
    else
	build_host_tools
    fi
    $BUILDSH $BUILD_PRODUCT $BUILD_VARIANT platform $TARGET_BUILD_CARRIER
    check_exit
    END_TIME=`date +%s`
    FUNC="Total"
    elapsed_time
}
