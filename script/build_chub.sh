#!/bin/bash

#
# Build core functions
#

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


