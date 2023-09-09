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

# ANDROID_BUILD_TOP is redefined in genrule
# OUT_DIR comes from global environment, OUT is not accessible
# TARGET_DEVICE comes from soong environment
# OK : TARGET_PRODUCT
# X : TARGET_DEVICE_NAME, PRODUCT_NAME, ANDROID_PRODUCT_OUT, PRODUT_OUT, PRODUCT_DEVICE
SRC_PATH=${ANDROID_BUILD_TOP}/vendor/samsung_slsi/telephony/exynos-ril
TARGET_DEVICE=$(echo $TARGET_PRODUCT | cut -d '_' -f2)
TARGET_LIBPATH="${ANDROID_BUILD_TOP}/${OUT_DIR}/target/product/${TARGET_DEVICE}/vendor/lib64"
TARGET_BINPATH="${ANDROID_BUILD_TOP}/${OUT_DIR}/target/product/${TARGET_DEVICE}/vendor/bin"

get_date() {
    echo "$(date +%Y%m%d_%H%M%S)"
}

commit_id() {
    TARGET_PATH=$1
    echo "$(git -C ${TARGET_PATH} log -n 1 --pretty=format:%h .)"
}

change_id() {
    TARGET_PATH=$1
    echo "$(git -C ${TARGET_PATH} log -n 1 . | grep Change-Id | tr -d ' ' | cut -d':' -f2 | cut -c 1-17)"
}

# Now allowed readelf and file in soong sandbox
#BUILDID_LIBSITRIL=$(readelf -n ${ANDROID_PRODUCT_OUT}/vendor/lib64/libsitril.so | grep -e "Build ID:" | awk '{print $3}')
#BUILDID_LIBSITRIL=$(file $ANDROID_PRODUCT_OUT/vendor/lib64/libsitril.so | cut -f5 -d',' | cut -f2 -d'=')
# hexdump is allowed but actual working folder is just in sandbox
# md5sum and other tools in /prebuilts/build-tools/path/linux-x86 can work
build_id() {
    TARGET_PATH=$1
    # Currently proper path can't be set
    A="$(hexdump -s64 -n8 -e '8/1 "%02x"' ${TARGET_PATH} | cut -c 1-8)"
    B="$(sha1sum ${TARGET_PATH} | cut -c 1-40)"
    C="$(md5sum ${TARGET_PATH} | cut -c 1-32)"
    echo "$C"
}

# $1 : TargetLocation
# return ID with the form of "CommitID:ChangeID"
generate_id() {
    TARGET_PATH=$1
    TARGET_OBJPATH=$2
    COMMITID=$(commit_id ${TARGET_PATH})
    CHANGEID=$(change_id ${TARGET_PATH})
    BUILDID=$(build_id ${TARGET_OBJPATH})
    echo "${COMMITID}"
#    echo "${COMMITID}:${CHANGEID}:${BUILDID}"
}
BUILD_DATE=$(get_date)
GIT_HASH_TOP=$(commit_id ${SRC_PATH})
ID_EXYNOS_RIL=$(generate_id "${SRC_PATH}" "${TARGET_LIBPATH}/libsitril.so")

ID_LIBSITRIL=$(generate_id "${SRC_PATH}/sitril" "${TARGET_LIBPATH}/libsitril.so")
ID_LIBRIL=$(generate_id "${SRC_PATH}/libril" "${TARGET_LIBPATH}/libril_sitril.so")
ID_LIBRILUTILS=$(generate_id "${SRC_PATH}/librilutils" "${TARGET_LIBPATH}/librilutils.so")
ID_LIBRIL_AIDL=$(generate_id "${SRC_PATH}/libril-aidl" "${TARGET_LIBPATH}/libril-aidl.so")
ID_RILD=$(generate_id "${SRC_PATH}/rild" "${TARGET_BINPATH}/hw/rild_exynos")

VERSION_MAJOR=3
VERSION_MINOR=0
sed \
    -e s/@NEW_VERSION_MAJOR@/${VERSION_MAJOR}/g \
    -e s/@NEW_VERSION_MINOR@/${VERSION_MINOR}/g \
    -e s/@BUILD_DATE@/${BUILD_DATE}/g \
    -e s/@GIT_HASH_TOP@/${GIT_HASH_TOP}/g \
    -e s/@ID_EXYNOS_RIL@/${ID_EXYNOS_RIL}/g \
    -e s/@ID_LIBSITRIL@/${ID_LIBSITRIL}/g \
    -e s/@ID_LIBRIL@/${ID_LIBRIL}/g \
    -e s/@ID_LIBRILUTILS@/${ID_LIBRILUTILS}/g \
    -e s/@ID_LIBRIL_AIDL@/${ID_LIBRIL_AIDL}/g \
    -e s/@ID_RILD@/${ID_RILD}/g
