#!/bin/bash

#
# Build core functions
#

function build_ispcpu()
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

  ISPCPU_TARGET=$(OUT_DIR=$OUT_DIR get_build_var ISPCPU_TARGET)
  ISPCPU_PATH=$(OUT_DIR=$OUT_DIR get_build_var ISPCPU_PATH)
  TARGET_DEVICE=$(OUT_DIR=$OUT_DIR get_build_var TARGET_DEVICE)

  TITLE="Build ISPCPU $ISPCPU_TARGET"
  print_title

  ISPCPU_START_TIME=`date +%s`

  if [[ "$ISPCPU_TARGET" == "" || "$ISPCPU_PATH" == "" ]]
  then
    echo "***********************************************************************"
    echo " >>> warning : ispcpu build target or ispcpu project path is not specified"
    echo " >>> ISPCPU_TARGET = $ISPCPU_TARGET"
    echo " >>> ISPCPU_PATH = $ISPCPU_PATH"
    echo "***********************************************************************"
    return 1
  fi

  if [ ! -f $ISPCPU_PATH/build.sh ]
  then
    echo "***********************************************************************"
    echo " >>> warning: $ISPCPU_PATH/build.sh does not exist !!!"
    echo "***********************************************************************"
    return 2
  fi

  if [[ $1 == 0 ]]
  then
    export ISPCPU_FIRMWARE_PATH=$ROOT_DIR/device/samsung/$TARGET_DEVICE/firmware/camera
  fi

  echo "*****************************************"
  echo "PATH = $ISPCPU_PATH"
  echo "TARGET = $ISPCPU_TARGET"
  echo "FIRMWARE PATH = $ISPCPU_FIRMWARE_PATH"
  echo "*****************************************"

  pushd $ISPCPU_PATH
  ./build.sh -prj=$ISPCPU_TARGET -src=fw
  popd

  ISPCPU_END_TIME=`date +%s`
  FUNC=${FUNCNAME[0]}
  let "ELAPSED_TIME=$ISPCPU_END_TIME-$ISPCPU_START_TIME"
  echo "${color_success}[BUILD TOOL]${color_reset} $FUNC: ${color_title}$ELAPSED_TIME seconds${color_reset}"
}


