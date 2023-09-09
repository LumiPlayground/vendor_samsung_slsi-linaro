#!/bin/bash

#
# Util & help functions
#
function build_dpm_binary()
{
  echo "Build DPM binary (${TARGET_KERNEL}/${BUILD_DPM_BINARY_FILE})"
  BUILD_DPM_BINARY_OUT=${BUILD_DPM_BINARY_OUT:-dpm}
  DPM_CLANG=${PRODUCT_MANIFEST}/${CLANG_PREBUILT_BIN}
  DPM_INPUT_FILE=${TARGET_KERNEL}/${BUILD_DPM_BINARY_FILE}
  DPM_INPUT_INCLUDE_PATH=${TARGET_KERNEL}/include
  DPM_OUTPUT_TEMP=${DPM_OUTPUT_PATH}/${BUILD_DPM_BINARY_OUT}.dts.tmp
  DPM_OUTPUT_DTBO=${DPM_OUTPUT_PATH}/${BUILD_DPM_BINARY_OUT}.dtbo
  DPM_OUTPUT_FILE=${DPM_OUTPUT_PATH}/${BUILD_DPM_BINARY_OUT}.img

  DPM_SB_TOOL_PATH=${PRODUCT_MANIFEST}/prebuilts/kernel-build-tools/linux-x86/exynos-tools
  DPM_SB_BIN_LEN=8192
  DPM_SB_SIGN_TYPE=4
  DPM_SB_KEY_TYPE=0
  DPM_SB_RB_COUNT=0

  ${DPM_CLANG}/clang -I ${DPM_INPUT_INCLUDE_PATH} \
			-E -nostdinc -undef -D__DTS__ -x assembler-with-cpp \
			-o ${DPM_OUTPUT_TEMP} ${DPM_INPUT_FILE}
  if [ $? -ne 0 ]; then
    exit 1
  fi

  ${PRIBUILT_DTC_PATH}/dtc -O dtb -o ${DPM_OUTPUT_DTBO} -b 0 -@ \
						${DPM_OUTPUT_TEMP}
  if [ $? -ne 0 ]; then
    exit 1
  fi

  ${DPM_SB_TOOL_PATH}/Makepad_sb40 ${DPM_OUTPUT_DTBO} ${DPM_SB_BIN_LEN}
  if [ $? -ne 0 ]; then
    exit 1
  fi

  ${DPM_SB_TOOL_PATH}/cslv/cslv_64 -infile ${DPM_OUTPUT_DTBO} \
					-outfile ${DPM_OUTPUT_FILE} \
					-sign_type ${DPM_SB_SIGN_TYPE} \
					-key_type ${DPM_SB_KEY_TYPE} \
					-rb_count ${DPM_SB_RB_COUNT} \
					-dynamic_length no
  if [ $? -ne 0 ]; then
    exit 1
  fi
}

function build_dpm()
{
  echo "Build DPM"
  export DPM_OUTPUT_PATH=${DPM_OUTPUT_PATH:-$(pwd)}

  if [ "${BUILD_DPM_BINARY_FILES}" != "" ]; then
    IFS=' ' read -ra DPM_BINARR <<< $(echo "${BUILD_DPM_BINARY_FILES}" | tr "\n" " ")
    for dpm_bin in "${DPM_BINARR[@]}"; do
      IFS=':' read -ra split <<< ${dpm_bin}
      BUILD_DPM_BINARY_FILE=${split[0]}
      BUILD_DPM_BINARY_OUT=${split[1]}
      build_dpm_binary
    done
    if [ "eng" == "$BUILD_VARIANT" ]; then
      if [ "${DPM_ENG_IMG}" != "" ]; then
        cp ${DPM_OUTPUT_PATH}/${DPM_ENG_IMG} ${DPM_OUTPUT_PATH}/dpm.img
      fi
    elif [ "userdebug" == "$BUILD_VARIANT" ]; then
      if [ "${DPM_USERDEBUG_IMG}" != "" ]; then
        cp ${DPM_OUTPUT_PATH}/${DPM_USERDEBUG_IMG} ${DPM_OUTPUT_PATH}/dpm.img
      fi
    elif [ "user" == "$BUILD_VARIANT" ]; then
      if [ "${DPM_USER_IMG}" != "" ]; then
        cp ${DPM_OUTPUT_PATH}/${DPM_USER_IMG} ${DPM_OUTPUT_PATH}/dpm.img
      fi
    fi
  else
    if [ "${BUILD_DPM_BINARY_FILE}" != "" ]; then
      build_dpm_binary
    fi
  fi
}
