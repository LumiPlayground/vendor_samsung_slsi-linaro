#!/bin/bash
#
# Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
# PROPRIETARY/CONFIDENTIAL
#
# This software is the confidential and proprietary information of
# SAMSUNG ELECTRONICS ("Confidential Information").
#
# You shall not disclose such Confidential Information and shall use it
# only in accordance with the terms of the license agreement
# you entered into with SAMSUNG ELECTRONICS.
#
# SAMSUNG make no representations or warranties about the suitability of
# the software, either express or implied, including but not limited to
# the implied warranties of merchantability, fitness for a particular
# purpose, or non-infringement. SAMSUNG shall not be liable for any
# damages suffered by license as a result of using, modifying or
# distributing this software or its derivatives.

# This will be called by run_sam.sh script
# this requires running lunch command at default
# when ANDROID_BUILD_TOP is detected, it's used
# next if preset is configures in this file or system enviromenent it will be used
# Each ANDROID_ROOT and lunch parameter shall be changed for each build dtarget

###################
# HELP           ##
###################
Help()
{
    # Display Help
    echo " Syntax: $0 [-c|h|o|a]"
    echo "options:"
    echo "h    Help. This screen"
    echo "c    Clean Build"
    echo "o    Build with Coverage Env"
    echo "a    Build with HWASAN Env"
    echo
}

BUILD_OPT=$1
CLEAN_OPT=
#export ALLOW_NINJA_ENV=1
while getopts ":hcoa" option; do
    case $option in
        h) # display Help
           Help
           exit;;
        c) echo "Do Clean Build, This will ignore other options"
           export CLEAN_OPT="clean";;
        o) echo "Build with Coverage Env"
           export CLANG_COVERAGE=true;
           export NATIVE_COVERAGE_PATHS="vendor/samsung_slsi/telephony/exynos-ril";;
        a) echo "Build with HWASAN Env"
           export SANITIZE_TARGET="hwaddress";;
        \?) # Invalid option
           echo "Error: Invalid option"
           Help
           exit;;
    esac
done

LUNCH_MENU="full_erd9935_t-eng"
#ANDROID_ROOT="/home2/android13_platform_cicd"
if [ -z $ANDROID_BUILD_TOP ]; then
  # uncomment, for preset without global envsetup
  if [ ! -z $ANDROID_ROOT ]; then
    source $ANDROID_ROOT/build/envsetup.sh
    lunch $LUNCH_MENU
  else
    echo "You need to source and lunch before you can use this script"
    exit -1
  fi
else
  echo Detected ANDROID_BUILD_TOP = $ANDROID_BUILD_TOP
  source $ANDROID_BUILD_TOP/build/envsetup.sh
  if [ ! -z $TARGET_PRODUCT ]; then
    echo Detected TARGET_PRODUCT = $TARGET_PRODUCT
    echo Use Lunch Menu : $TARGET_PRODUCT-$TARGET_BUILD_VARIANT
    lunch $TARGET_PRODUCT-$TARGET_BUILD_VARIANT
  else
    echo "You need to lunch before you try to use this script"
    exit -1
  fi
fi

# for older build system or to build specific module build
#m libsitril libril_sitril librilutils rild_exynos RilDataTests libril_tests librilutils_tests SitProtocolTests libvendortestbase SitrilTest PsServiceTest -j36
# For build all sub projects, traversing Android.bp and Android.mk
mm $CLEAN_OPT -j$(nproc)
