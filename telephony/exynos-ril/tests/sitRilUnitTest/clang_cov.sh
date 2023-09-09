#!/bin/bash
#
# Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
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

# How to get clang's coverage data for vendor-ril
#
# 0) Initialize the environment
#   $ . build/envsetup.sh && lunch TARGET
#
# 1) Build coverage enabled Android image for vendor-ril
#   $ BOARD_BUILD_SUPER_IMAGE_BY_DEFAULT=true \
#     CLANG_COVERAGE=true \
#     NATIVE_COVERAGE_PATHS="vendor/samsung_slsi/telephony/exynos-ril" \
#     make -j4
#
#   Note: *.profraw filename for executables or shared library can be checked as follows
#   E.g.
#       $ cd PATH_TO_VENDOR_LIBRARY && strings * -f | grep profraw
#       FILE_NAME: /data/misc/trace/clang-%p-%m.profraw
#       FILE_NAME: default.profraw
#
# 2) Flash a device
#
# 3) Build a test code
#   $ CLANG_COVERAGE=true \
#     NATIVE_COVERAGE_PATHS="vendor/samsung_slsi/telephony/exynos-ril" \
#     mm TEST_MODULE
#
# 4) run this script
#   $ ./clang_cov.sh [options]
#
#   Options:
#   -r  : Generate the html formatted report
#   -e  : Show expanded source regions(Only available with -r)
#   -f  : Set filter strings for --gtesst_filter
#   https://github.com/google/googletest/blob/master/docs/advanced.md#running-a-subset-of-the-tests
#
#   E.g.
#       $ ./clang_cov.sh // Show code coverage result
#       $ ./clang_cov.sh -r // Generate the html report
#       $ ./clang_cov.sh -f=*Sms* // Only run the tests whose full names match the filter.
#       $ ./clang_cov.sh -l // Generate the lcov tracefile output and html report based lcov
#       $ ./clang_cov.sh -u // upload new built ril libraries and replace libraries then finally killing previous rild
#       $ ./clang_cov.sh -b // Assume first boot, so try to reboot to disable verity to remount
#                           // and replace libraries then finally killing previous rild
#

#######################################################################
TEST_MODULE="SitrilTest"
SOURCE_PATH="vendor/samsung_slsi/telephony/exynos-ril"
MODULE_INSTALL_PATH="$ANDROID_PRODUCT_OUT/testcases/$TEST_MODULE/arm64"
REMOTE_DIR_BASE=""
REMOTE_DIR_BASE_OLD="/data/local/tmp"
REMOTE_DIR_BASE_T="/data/local/tests/vendor"
REMOTE_COVERAGE_OUTPUT_LOC="clang_cov/$TEST_MODULE"
REMOTE_COVERAGE_OUTPUT_DIR="" #"$REMOTE_DIR_BASE/clang_cov/$TEST_MODULE"
OUTPUT_DIR_BASE="$ANDROID_PRODUCT_OUT/coverage/$TEST_MODULE"
OUTPUT_CLANG_REPORT="$OUTPUT_DIR_BASE/clang_report"
OUTPUT_LCOV_REPORT="$OUTPUT_DIR_BASE/lcov_report"
#######################################################################

if [ -z $ANDROID_BUILD_TOP ]; then
  echo "You need to source and lunch before you can use this script"
  exit 1
fi

adb wait-for-device
adb root
adb wait-for-device

# Check SDK version
SDKVERSION=$(adb shell 'getprop ro.build.version.sdk | grep -o "[0-9]*[A-Z]*"')
echo SDK_VERSION = $SDKVERSION

if [ $SDKVERSION -ge 33 ]; then
  echo "T or later"
  REMOTE_DIR_BASE=$REMOTE_DIR_BASE_T
else
  echo "S or eariler"
  REMOTE_DIR_BASE=$REMOTE_DIR_BASE_OLD
fi
REMOTE_COVERAGE_OUTPUT_DIR="$REMOTE_DIR_BASE/$REMOTE_COVERAGE_OUTPUT_LOC"

EXTRA_OPTS=""
GTEST_FILTER="*"
COMMAND="report"

VENDOR_PATH="vendor/lib64"
SYMBOL_PATH="symbols/$VENDOR_PATH"
TARGET_OBJECTS=("libsitril.so" "libril_sitril.so" "libsitril-audio.so" "libsitril-client.so" "libsitril-gps.so")
#  libsitril-se.so" # Removed on HAL 2.0 T
#  libsitril-common.so # Removed before T

remount_trial() {
    adb root
    adb remount
    adb reboot
    echo "Rebooting after remount trial for sure disabling verity"
    adb wait-for-device shell 'while [[ -z $(getprop sys.boot_completed) ]]; do sleep 1; done;'
    echo "BOOT_COMPLETED"
}

replace_rillibs() {
    echo "push libs"
    adb root
    adb remount
    for tgt in ${TARGET_OBJECTS[@]}; do
        adb push $ANDROID_PRODUCT_OUT/$VENDOR_PATH/$tgt /vendor/lib64/
    done
    victim=$(adb shell "pgrep rild");
    adb shell "kill $victim"
    echo "Wait for 15 seconds to recover rild"
    sleep 15;
}

for i in "$@"; do
  case $i in
    -e)
      EXTRA_OPTS="${EXTRA_OPTS} --show-expansions=true"
      shift
      ;;
    -f=*)
      GTEST_FILTER="${i#*=}"
      shift
      ;;
    -r)
      if [ "$COMMAND" = "export" ]; then
        COMMAND="all"
      else
        COMMAND="show"
      fi
      EXTRA_OPTS="${EXTRA_OPTS} \
        --format=html \
        --project-title=$TEST_MODULE \
        --output-dir=$OUTPUT_CLANG_REPORT"
      shift
      ;;
    -l)
      if [ "$COMMAND" = "show" ]; then
        COMMAND="all"
      else
        COMMAND="export"
      fi
      shift
      ;;
    -u)
      replace_rillibs
      shift
      ;;
    -b)
      remount_trial
      replace_rillibs
      shift
      ;;
  esac
done

echo "Running native tests and generating coverage report"

adb shell mkdir -p $REMOTE_COVERAGE_OUTPUT_DIR

# Clean old profraw files from device
adb shell find $REMOTE_COVERAGE_OUTPUT_DIR -iname "*.profraw" -delete 1> /dev/null

# Push the test module and make it executable
adb push $MODULE_INSTALL_PATH/$TEST_MODULE $REMOTE_DIR_BASE
adb shell chmod +x $REMOTE_DIR_BASE/$TEST_MODULE

# Execute the test module
adb shell \
  LLVM_PROFILE_FILE=$REMOTE_COVERAGE_OUTPUT_DIR/profraw/$TEST_MODULE-%p-%m.profraw \
  $REMOTE_DIR_BASE/$TEST_MODULE --gtest_color=yes --gtest_filter=$GTEST_FILTER

if [ -d "$OUTPUT_DIR_BASE" ]; then
  # Clean previous data files and report if any
  rm -rf $OUTPUT_DIR_BASE/profraw
else
  mkdir -p $OUTPUT_DIR_BASE
fi

# Pull coverage files and post-process
adb pull $REMOTE_COVERAGE_OUTPUT_DIR/profraw $OUTPUT_DIR_BASE

llvm-profdata merge --failure-mode=all --output=$OUTPUT_DIR_BASE/profraw/$TEST_MODULE.profdata \
  $OUTPUT_DIR_BASE/profraw/*.profraw

# removed option : This will make the report more lcov-like
# --show-branch-summary=false \
# --show-region-summary=false \
COMMON_OPTS="--Xdemangler=c++filt --Xdemangler=-n \
  --path-equivalence=/proc/self/cwd/,$ANDROID_BUILD_TOP \
  --instr-profile=$OUTPUT_DIR_BASE/profraw/$TEST_MODULE.profdata"

OBJECTS=
for i in "${!TARGET_OBJECTS[@]}"; do
   if [ "$i" -eq "0" ]; then
       OBJECTS="$ANDROID_PRODUCT_OUT/$SYMBOL_PATH/${TARGET_OBJECTS[$i]} "
   else
       OBJECTS="$OBJECTS--object=$ANDROID_PRODUCT_OUT/$SYMBOL_PATH/${TARGET_OBJECTS[$i]} "
   fi
done

# Generate a coverage report(Refer to "llvm-cov show/report -h")
echo Generate a coverage report
if [ "$COMMAND" = "show" ] || [ "$COMMAND" = "all" ]; then
  if [ -d "$OUTPUT_CLANG_REPORT" ]; then
    # Clean previous report if any
    rm -rf $OUTPUT_CLANG_REPORT
  fi
  llvm-cov show \
  $EXTRA_OPTS \
  $COMMON_OPTS \
  $OBJECTS \
  $ANDROID_BUILD_TOP/$SOURCE_PATH \
  && echo "Report path: $OUTPUT_CLANG_REPORT"

  echo -e "\033[32m==== Reporting Clang ====\033[0m"
  llvm-cov report \
  $COMMON_OPTS \
  $OBJECTS \
  $ANDROID_BUILD_TOP/$SOURCE_PATH \
  | grep TOTAL | awk '{print "\033[34mFunctions=" $7, "Lines=" $10} \
  $7 ~ /%/ {$7=$7/100} $10 ~ /%/ { if ($10>80) $10=80/100
    else $10=$10/100 } {print "\033[32mTEM=" ($7*$10*5)/0.8 "\033[0m"}'
fi

# Generate a lcov tracefile output and html report based lcov
if [ "$COMMAND" = "export" ] || [ "$COMMAND" = "all" ]; then
  if [ -d "$OUTPUT_LCOV_REPORT" ]; then
    # Clean previous report if any
    rm -rf $OUTPUT_LCOV_REPORT
  fi
  llvm-cov export \
  "--format=lcov" \
  "--Xdemangler=c++filt" \
  "--path-equivalence=/proc/self/cwd/,$ANDROID_BUILD_TOP" \
  "--instr-profile=$OUTPUT_DIR_BASE/profraw/$TEST_MODULE.profdata" \
  $OBJECTS \
  $ANDROID_BUILD_TOP/$SOURCE_PATH \
  > $OUTPUT_DIR_BASE/lcov.info

  sed -i "s|\/proc\/self\/cwd|$ANDROID_BUILD_TOP|g" $OUTPUT_DIR_BASE/lcov.info
  lcov --remove $OUTPUT_DIR_BASE/lcov.info "*.txt" -o $OUTPUT_DIR_BASE/filtered_lcov.info
  genhtml --branch-coverage $OUTPUT_DIR_BASE/filtered_lcov.info -o $OUTPUT_LCOV_REPORT > tmp.txt

  echo -e "\033[32m==== Reporting Lcov ====\033[0m"
  echo "Overall coverage rate:"
  echo "  "`cat tmp.txt | egrep "lines\.\."`
  echo "  "`cat tmp.txt | egrep "functions\.\."`
  echo "  "`cat tmp.txt | egrep "branches\.\.\."`

  LINES=$(echo `cat tmp.txt | egrep "lines\.\." | awk '$2 ~ /%/ { if ($2>80) $2=80/100
    else $2=$2/100 } {print $2}'`)
  FUNCTIONS=$(echo `cat tmp.txt | egrep "functions\.\.:" | awk '$2 ~ /%/ {$2=$2/100} {print $2}'`)
  TEM_SCORE=$(echo "scale=2;($LINES*$FUNCTIONS*5/0.8)" | bc -l)
  echo "   tem........:" $TEM_SCORE

  rm tmp.txt
  rm $OUTPUT_DIR_BASE/lcov.info
  rm $OUTPUT_DIR_BASE/filtered_lcov.info
fi

