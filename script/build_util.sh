#!/bin/bash

#
# Functions for Print
#
color_title=$'\E'"[0;33m"
color_success=$'\E'"[0;32m"
color_error=$'\E'"[0;31m"
color_reset=$'\E'"[00m"

#
# Constant Environment vairables
#
build_carrier=("chnopen" "cmcc" "europen" "vzw" "tmo" "att" "spr" "ctc" "cu" "kddi" "ntt" "global")
CLIENT=$(whoami)

# BUILD SCRIPT is not in Debug mode
# BUILD_DEBUG=1

#
# Util & help functions
#

function elapsed_time() {
  let "ELAPSED_TIME=$END_TIME-$START_TIME"
  echo "${color_success}[BUILD TOOL]${color_reset} $FUNC: ${color_title}$ELAPSED_TIME seconds${color_reset}"
}

function print_title() {
  echo
  echo "[[[[[[[${color_title} ${TITLE}$1 ${color_reset}]]]]]]]"
  TITLE=
}

function print_error() {
  echo ${color_error}ERROR${color_reset}: $1
}

function print_warning() {
  echo ${color_title}WARNING${color_reset}: $1
}

function print_info() {
  echo ${color_title}INFO${color_reset}: $1
}

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

function check_exit()
{
  RET=$?
  if [ $RET != 0 ]
  then
    exit $RET
  fi
}

function dbg_pause()
{
  if [ "${BUILD_DEBUG}" == "1" ]; then
    echo
    read -s -n1 -p "Paused for debugging #dbg [[$1]]" keypress
    echo
  fi
}

####### Collection Sequencess #######
function copy_binlist()
{
  if [ -z "${BINLIST}" ]; then
    print_error "NO BINLIST variable defined"
    exit 2
  fi

  IFS=' ' read -ra BINARR <<< $(echo "$BINLIST" | tr "\n" " ")
  for bin in "${BINARR[@]}"; do
    IFS=':' read -ra split <<< $bin
    if [ ! -f $BINBASE/${split[0]} ]; then
      print_error "$BINBASE/${split[0]}: No such file"
      exit 2
    fi
    cp -f $BINBASE/${split[0]} $DIST/${split[1]}
    print_info "copy file: $DIST/${split[1]}"
  done
}

function copy_misc_bin()
{
  print_title "Copy Product DIST to Main DIST "
  echo
  echo $DIST
  ls -la $DIST
}
