#!/bin/bash

color_title=$'\E'"[0;33m"
color_success=$'\E'"[0;32m"
color_error=$'\E'"[0;31m"
color_reset=$'\E'"[00m"

function print_error() {
	echo ${color_error}ERROR${color_reset}: $1
}

function array_contains() {
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

function print_title() {
	echo
	echo "[[[[[[[${color_title} $TITLE ${color_reset}]]]]]]]"
}

function check_exit()
{
  RET=$?
  if [ $RET != 0 ]
  then
    exit $RET
  fi
}

function elapsed_time() {
	let "ELAPSED_TIME=$2-$1"
	echo "${color_success}[BUILD TOOL]${color_reset} $FUNC: ${color_title}$ELAPSED_TIME seconds${color_reset}"
}

