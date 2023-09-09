#!/bin/bash

. vendor/samsung_slsi/script/util.sh
. vendor/samsung_slsi/script/mali_targets.sh

####### Parse input parameters #######
BUILD_PRODUCT=$1
BUILD_VARIANT=$2
BUILD_OPTION=$3

##### Check input parameters #####

req_build_variant=(eng user userdebug)
req_build_options=( \
	build_umd \
	move_umd \
	setup_android \
	restore_kbase \
	prepare_build \
	move_fullimage \
	move_bootimage \
)

function print_usage_exit () {
	print_error "$1"
	echo 'Usage: ./build_mali_core.sh <PRODUCT> [ eng | userdebug | user ] <OPTION>'
	echo "OPTION:"
	for option in "${req_build_options[@]}"
	do
		echo -e "\t${option}"

	done
	exit 2
}

if [ $# -lt 3 ]; then
	print_usage_exit "need at least three arguments"
fi

if [ $(array_contains $BUILD_VARIANT "${req_build_variant[@]}") == 1 ]; then
	print_usage_exit "\"$BUILD_VARIANT\" is not a build variant"
fi

if [ $(array_contains $BUILD_OPTION "${req_build_options[@]}") == 1 ]; then
	print_usage_exit "\"$BUILD_OPTION\" is not a valid option"
fi

function check_project_is_mok()
{
	if [ $(array_contains $BUILD_PRODUCT ${PROD_LIST[@]}) == 1 ]; then
		# Early exit if not Mali on Keystone build product
		echo "$BUILD_PRODUCT is not a Mali on Keystone project"
		exit 2
	fi
}

if [ -z $OUT_DIR ]; then
	echo "OUT_DIR not set. Exiting"
	exit 1
fi

if [ -z $MALI_DIR ]; then
	echo "MALI_DIR not set. Exiting"
	exit 1
fi

####### Set Env Variables #######
ROOT_DIR=$(pwd)
PRODUCT_MANIFEST=$ROOT_DIR/products/$BUILD_PRODUCT

if [ "z${CPU_JOB_NUM}" == "z" ] ; then
	CPU_JOB_NUM=$(grep processor /proc/cpuinfo | awk '{field=$NF};END{print (field+1)/2}')
fi

SRC_DIR=$ROOT_DIR/$OUT_DIR/target/product/$BUILD_PRODUCT
MALI_INTEGRATION_DIR=$ROOT_DIR/vendor/arm/integration

if	[ ! -d $MALI_INTEGRATION_DIR/scripts ]; then
	MALI_INTEGRATION_DIR=$ROOT_DIR/vendor/arm/snapshot_mali/integration
fi

if	[ ! -d $MALI_INTEGRATION_DIR ]; then
	echo "Mali Integration directory not found"
	exit 1
fi

function symlink_if_file_exists()
{
	[ -f $1 ] && ln -snf $1 $2;
}
####### Mali on Keystone build target environment functions #######
function setup_build_env()
{
	TITLE="Prepare Mali on Keystone build environement for $BUILD_PRODUCT"
	print_title

	case "$BUILD_PRODUCT" in
		universal9810_q_launching_mali)
			local base_build_product="universal9810_q_launching"
			local device_dir=device/samsung/universal9810
			;;
		universal9820_mali)
			local base_build_product="universal9820"
			local device_dir=device/samsung/universal9820
			;;
		universal9825_r_mali)
			local base_build_product="universal9825_r"
			local device_dir=device/samsung/universal9820
			;;
		universal9830_mali)
			local base_build_product="universal9830"
			local device_dir=device/samsung/universal9830
			export PATH="${ROOT_DIR}/products/${base_build_product}/toolchain/aarch64-linux-android-4.9-2014/bin:$PATH"
			;;
		universal2100_r_mali)
			local base_build_product="universal2100_r"
			local device_dir=device/samsung/universal2100
			;;
		erd8825_s_mali)
			local base_build_product="erd8825_s"
			local device_dir=device/samsung/erd8825
			;;
		universal3830_s_mali)
			local base_build_product="universal3830_s"
			local device_dir=device/samsung/universal3830
			;;
		universal3830_qrs_mr_mali)
			local base_build_product="universal3830_qrs_mr"
			local device_dir=device/samsung/universal3830
			;;
		erd8535_t_mali)
			local base_build_product="erd8535_t"
			local device_dir=device/samsung/erd8535
			;;
		*)
			echo "Not a Mali on Keystone project"
			exit 2
	esac

	MALI_ON_KEYSTONE=true
	ln -snf $ROOT_DIR/$device_dir $ROOT_DIR/device/samsung/$BUILD_PRODUCT;
	ln -snf $ROOT_DIR/products/$base_build_product $PRODUCT_MANIFEST;

	symlink_if_file_exists $PRODUCT_MANIFEST/kernel/build.config.$base_build_product \
	        $PRODUCT_MANIFEST/kernel/build.config.$BUILD_PRODUCT;

	symlink_if_file_exists $PRODUCT_MANIFEST/bootloader/build.config.$base_build_product \
	        $PRODUCT_MANIFEST/bootloader/build.config.$BUILD_PRODUCT;

	symlink_if_file_exists $PRODUCT_MANIFEST/bl2/build.config.$base_build_product \
	        $PRODUCT_MANIFEST/bl2/build.config.$BUILD_PRODUCT;

	symlink_if_file_exists $PRODUCT_MANIFEST/script/configs/kernel.build.config.$base_build_product \
	        $PRODUCT_MANIFEST/script/configs/kernel.build.config.$BUILD_PRODUCT;

	symlink_if_file_exists $PRODUCT_MANIFEST/script/configs/bootloader.build.config.$base_build_product \
	        $PRODUCT_MANIFEST/script/configs/bootloader.build.config.$BUILD_PRODUCT;

	symlink_if_file_exists $PRODUCT_MANIFEST/script/configs/bl2.build.config.$base_build_product \
	        $PRODUCT_MANIFEST/script/configs/bl2.build.config.$BUILD_PRODUCT;
}


##### Mali UMD related functions #####
function mali_setup_android()
{
	TITLE="Run setup_android for Mali UMD $PLATFORM_BUILD_TARGET"
	print_title

	if [ "$MALI_BUILD_MK" == true ] ; then
		if [ -f "$ROOT_DIR/$MALI_DIR/setup_android" ]; then
		    OUT_DIR=$OUT_DIR $ROOT_DIR/$MALI_DIR/setup_android -k \
		    $ROOT_DIR/$MALI_DIR/bldsys/profiles/$BUILD_PRODUCT.config;
		fi
	else
		if [ -f "$ROOT_DIR/$MALI_DIR/setup_android" ]; then
		    OUT_DIR=$OUT_DIR $ROOT_DIR/$MALI_DIR/setup_android -b \
		    $ROOT_DIR/$MALI_DIR/bldsys/profiles/$BUILD_PRODUCT.config;
		fi
	fi
}

function prepare_lunch()
{
	TITLE="Prepare Lunch for Mali UMD $PLATFORM_BUILD_TARGET"
	print_title

	echo "source build/envsetup.sh"
	source build/envsetup.sh
	echo

	echo "lunch full_$BUILD_PRODUCT-$BUILD_VARIANT"
	OUT_DIR=$OUT_DIR lunch full_$BUILD_PRODUCT-$BUILD_VARIANT >> /dev/null;
	check_exit
}

function build_mali_umd()
{
	local start_time=`date +%s`

	TITLE="Make Mali user libraries $BUILD_PRODUCT-$BUILD_VARIANT"
	print_title

	prepare_lunch

	mali_setup_android
	check_exit

	if [ "$MALI_BUILD_MK" == true ] ; then
		MALI_PREBUILT=false OUT_DIR=$OUT_DIR make -j$CPU_JOB_NUM libGLES_mali libOpenCL vulkan.mali libgpudataproducer;
	else
		MALI_PREBUILT=false OUT_DIR=$OUT_DIR make -j$CPU_JOB_NUM libmali libOpenCL vulkan.mali libgpudataproducer;
	fi

	check_exit

	local end_time=`date +%s`
	FUNC=${FUNCNAME[0]}
	elapsed_time $start_time $end_time
}

function move_mali_umd()
{
	TITLE="Moving Mali UMD to upload directory for $BUILD_PRODUCT"
	print_title

	local src_vendor_32_dir=$SRC_DIR/vendor/lib
	local src_vendor_64_dir=$SRC_DIR/vendor/lib64
	local src_symbol_32_dir=$SRC_DIR/symbols/vendor/lib
	local src_symbol_64_dir=$SRC_DIR/symbols/vendor/lib64

	local dest_dir=$ROOT_DIR/vendor/arm/mali/$BUILD_PRODUCT
	local dest_lib_32_dir=$dest_dir/lib
	local dest_lib_64_dir=$dest_dir/lib64

	local commit_msg_path=$dest_dir/commit_msg

	# Moving libs
	cp -v $src_vendor_32_dir/egl/libGLES_mali.so   $dest_lib_32_dir/egl/libGLES_mali.so;
	cp -v $src_vendor_32_dir/hw/vulkan.mali.so     $dest_lib_32_dir/hw/vulkan.mali.so;
	cp -v $src_vendor_32_dir/libOpenCL.so          $dest_lib_32_dir/libOpenCL.so;
	cp -v $src_vendor_32_dir/libgpudataproducer.so $dest_lib_32_dir/libgpudataproducer.so;
	cp -v $src_vendor_64_dir/egl/libGLES_mali.so   $dest_lib_64_dir/egl/libGLES_mali.so;
	cp -v $src_vendor_64_dir/hw/vulkan.mali.so     $dest_lib_64_dir/hw/vulkan.mali.so;
	cp -v $src_vendor_64_dir/libOpenCL.so          $dest_lib_64_dir/libOpenCL.so;
	cp -v $src_vendor_64_dir/libgpudataproducer.so $dest_lib_64_dir/libgpudataproducer.so;

	# Moving symbol libs
	cp -v $src_symbol_32_dir/egl/libGLES_mali.so $dest_lib_32_dir/egl/libGLES_mali_symbols.so;
	cp -v $src_symbol_64_dir/egl/libGLES_mali.so $dest_lib_64_dir/egl/libGLES_mali_symbols.so;

	cp -v $MALI_DIR/snap_logs $commit_msg_path;
}

function move_bootimage()
{
	if [ -z $OUT_IMAGE_DIR ] ; then
		echo "OUT_IMAGE_DIR is not set. Exiting (1)"
		exit 1
	fi

	local dst_image_dir=${OUT_IMAGE_DIR}/${BUILD_PRODUCT}

	TITLE="Moving bootimage files of $BUILD_PRODUCT to $dst_image_dir"
	print_title

	# create destination directory
	[[ ! -d $dst_image_dir ]] && mkdir -p $dst_image_dir;

	# build list of images to move in BOOT_IMG_LIST array
	select_img_list $BUILD_PRODUCT

	# move images to destination
	for image in "${BOOT_IMG_LIST[@]}"
	do
		mv -v ${SRC_DIR}/${image} ${dst_image_dir};
	done
}

function move_platimage()
{
	if [ -z $OUT_IMAGE_DIR ] ; then
		echo "OUT_IMAGE_DIR is not set. Exiting (1)"
		exit 1
	fi

	local dst_image_dir=${OUT_IMAGE_DIR}/${BUILD_PRODUCT}

	TITLE="Moving platform image files of $BUILD_PRODUCT to $dst_image_dir"
	print_title

	# create destination directory
	[[ ! -d $dst_image_dir ]] && mkdir -vp $dst_image_dir;

	mkdir -p ${dst_image_dir}/vendor/lib/egl;
	mkdir -p ${dst_image_dir}/vendor/lib/hw;
	mkdir -p ${dst_image_dir}/vendor/lib64/egl;
	mkdir -p ${dst_image_dir}/vendor/lib64/hw;

	mv -v $SRC_DIR/vendor/lib/egl/libGLES_mali.so ${dst_image_dir}/vendor/lib/egl/libGLES_mali.so;
	mv -v $SRC_DIR/vendor/lib/hw/vulkan.mali ${dst_image_dir}/vendor/lib/hw/vulkan.mali;
	mv -v $SRC_DIR/vendor/lib/libOpenCL.so ${dst_image_dir}/vendor/lib/libOpenCL.so;

	mv -v $SRC_DIR/vendor/lib64/egl/libGLES_mali.so ${dst_image_dir}/vendor/lib64/egl/libGLES_mali.so;
	mv -v $SRC_DIR/vendor/lib64/hw/vulkan.mali ${dst_image_dir}/vendor/lib64/hw/vulkan.mali;
	mv -v $SRC_DIR/vendor/lib64/libOpenCL.so ${dst_image_dir}/vendor/lib64/libOpenCL.so;

	# build list of images to move in BOOT_IMG_LIST array
	select_img_list $BUILD_PRODUCT

	# move images to destination
	for image in "${PLAT_IMG_LIST[@]}"
	do
		mv -v ${SRC_DIR}/${image} ${dst_image_dir};
	done
}

function move_umd_external()
{
	if [ -z $OUT_IMAGE_DIR ] ; then
		echo "OUT_IMAGE_DIR is not set. Exiting (1)"
		exit 1
	fi

	local dst_image_dir=${OUT_IMAGE_DIR}/${BUILD_PRODUCT}

	TITLE="Moving UMD libs of $BUILD_PRODUCT to $dst_image_dir"
	print_title

	local src_vendor_32_dir=$SRC_DIR/vendor/lib
	local src_vendor_64_dir=$SRC_DIR/vendor/lib64
	local src_symbol_32_dir=$SRC_DIR/symbols/vendor/lib
	local src_symbol_64_dir=$SRC_DIR/symbols/vendor/lib64

	local dest_dir=$dst_image_dir
	local dest_lib_32_dir=$dest_dir/vendor/lib
	local dest_lib_64_dir=$dest_dir/vendor/lib64

	if [ ! -d $dest_lib_32_dir ] ; then
		mkdir -p $dest_lib_32_dir/egl
		mkdir -p $dest_lib_32_dir/hw
	fi
	if [ ! -d $dest_lib_64_dir ] ; then
		mkdir -p $dest_lib_64_dir/egl
		mkdir -p $dest_lib_64_dir/hw
	fi

	local commit_msg_path=$dest_dir/commit_msg

	# Moving libs
	cp -v $src_vendor_32_dir/egl/libGLES_mali.so $dest_lib_32_dir/egl/libGLES_mali.so
	cp -v $src_vendor_32_dir/hw/vulkan.mali.so   $dest_lib_32_dir/hw/vulkan.mali.so
	cp -v $src_vendor_32_dir/libOpenCL.so        $dest_lib_32_dir/libOpenCL.so
	cp -v $src_vendor_64_dir/egl/libGLES_mali.so $dest_lib_64_dir/egl/libGLES_mali.so
	cp -v $src_vendor_64_dir/hw/vulkan.mali.so   $dest_lib_64_dir/hw/vulkan.mali.so
	cp -v $src_vendor_64_dir/libOpenCL.so        $dest_lib_64_dir/libOpenCL.so

	# Moving symbol libs
	cp -v $src_symbol_32_dir/egl/libGLES_mali.so $dest_lib_32_dir/egl/libGLES_mali_symbols.so
	cp -v $src_symbol_64_dir/egl/libGLES_mali.so $dest_lib_64_dir/egl/libGLES_mali_symbols.so
}


####### Mali KMD setup functions #######

TARGET_KERNEL=$PRODUCT_MANIFEST/kernel
MALI_KCONFIG_PATH=$TARGET_KERNEL/drivers/gpu/arm/Kconfig
MALI_KBUILD_PATH=$TARGET_KERNEL/drivers/gpu/arm/Kbuild

function restore_mali_kbase()
{
	local misc_integration_path=$MALI_INTEGRATION_DIR/$BUILD_PRODUCT

	# restoring mali kbuild and kconfig files
	cp $misc_integration_path/mali_kconfig_kbuild/Kbuild $MALI_KBUILD_PATH;
	cp $misc_integration_path/mali_kconfig_kbuild/Kconfig $MALI_KCONFIG_PATH;
}

function prepare_mali_kbase()
{
	# Mali path in platform
	local mali_src_path=$ROOT_DIR/$MALI_DIR
	local kbase_src_path=$mali_src_path/kernel/drivers/gpu/arm
	local mali_linux_header_src_path=$mali_src_path/kernel/include/linux
	local mali_uapi_header_src_path=$mali_src_path/kernel/include/uapi
	local kbase_buildconfig_src_path=$mali_src_path/bldsys/profiles

	local kbase_dst_path=$TARGET_KERNEL/drivers/gpu/arm

	TITLE="Prepare Mali Kbase kernel driver for Mali on Keystone"
	print_title

	# copying def config extension file
	cp -vf $kbase_buildconfig_src_path/${BUILD_PRODUCT}_kbase.cfg \
		$TARGET_KERNEL/arch/arm64/configs/mali_keystone.cfg;

	# copying mali midgard folder
	[ -d "$kbase_dst_path/midgard" ] && rm -rf $kbase_dst_path/midgard;
	[ -d "$kbase_dst_path/platform_exynos" ] && rm -rf $kbase_dst_path/platform_exynos;
	cp -rf $kbase_src_path/* $kbase_dst_path;

	# copying mali header folder
	cp -rf $mali_linux_header_src_path $TARGET_KERNEL/include;
	rm -rf $TARGET_KERNEL/include/uapi/gpu/arm
	cp -rf $mali_uapi_header_src_path $TARGET_KERNEL/include;

	if [ "universal2100_r_mali" = ${BUILD_PRODUCT} ]; then
		local misc_integration_path=$MALI_INTEGRATION_DIR/$BUILD_PRODUCT
		pushd .;
		cd $TARGET_KERNEL;
		git apply $misc_integration_path/kernel_patches/2100_decon_dsi.patch
		git apply $misc_integration_path/kernel_patches/0001-add-NULL-check-in-gpu_profiler.patch
		cd $PRODUCT_MANIFEST/build
		git apply $misc_integration_path/build_patches/0001-build.sh-apply-mali-keystone-cfg-for-mali-on-keyston.patch
		popd;
	fi

	if [ "erd8825_s_mali" = ${BUILD_PRODUCT} ]; then
		local misc_integration_path=$MALI_INTEGRATION_DIR/$BUILD_PRODUCT
		pushd .;
		cd $PRODUCT_MANIFEST/build
		git apply $misc_integration_path/build_patches/0001-8825_product-build.sh-apply-mali-keystone-cfg-for-ma.patch
		popd;
	fi
}

####### Main Routines #######
case "$BUILD_OPTION" in
	restore_kbase)
		;;
	*)
		check_project_is_mok
		;;
esac

case "$BUILD_OPTION" in
	build_umd)
		build_mali_umd
		;;
	move_umd)
		move_mali_umd
		;;
	setup_android)
		# Needed by host tools build
		mali_setup_android
		;;
	restore_kbase)
		restore_mali_kbase
		;;
	prepare_build)
		# setup_build_env will exit if not Mali on Keystone project
		setup_build_env
		prepare_mali_kbase
		prepare_lunch
		mali_setup_android
		;;
	move_fullimage)
		move_bootimage
		move_platimage
		cp -v $MALI_INTEGRATION_DIR/scripts/mok_flash.ps1 $OUT_IMAGE_DIR;
		;;
	move_bootimage)
		move_bootimage
		cp -v $MALI_INTEGRATION_DIR/scripts/mok_flash.ps1 $OUT_IMAGE_DIR;
		;;
	move_platimage)
		move_platimage
		cp -v $MALI_INTEGRATION_DIR/scripts/mok_flash.ps1 $OUT_IMAGE_DIR;
		;;
esac

