#!/bin/bash

. vendor/samsung_slsi/script/util.sh
. vendor/samsung_slsi/script/mali_targets.sh

BUILD_PRODUCT=$1
BUILD_VARIANT=$2
BUILD_OPTION=$3


##### Helper Functions #####
function print_usage_exit () {
	print_error "$1"
	echo 'Usage: ./build_mali.sh [ all | BUILD_PRODUCT ] [ eng | userdebug | user ] <OPTION>'
	echo "OPTION:"
	for option in "${req_build_options[@]}"
	do
		echo -e "\t${option}"

	done
	exit 2
}

##### Check input parameters #####
if [ $# -lt 3 ]; then
	print_usage_exit "need at least three arguments"
fi

req_build_variant=(eng user userdebug)
req_build_options=( \
	create_snapshot \
	export_snapshot \
	umd \
	bootimage \
	vendorbootimage \
	fullimage \
	move_bootimage \
	move_fullimage \
)

if [ $(array_contains $BUILD_VARIANT "${req_build_variant[@]}") == 1 ]; then
	print_usage_exit "\"$BUILD_VARIANT\" is not a build variant"
fi

if [ $(array_contains $BUILD_OPTION "${req_build_options[@]}") == 1 ]; then
	print_usage_exit "\"$BUILD_OPTION\" is not a valid option"
fi

if [ "$BUILD_PRODUCT" != "all" ] && [ $(array_contains $BUILD_PRODUCT "${PROD_LIST[@]}") == 1 ]; then
	print_usage_exit "\"$BUILD_PRODUCT\" is not a support build target"
fi

####### Set Env Variables #######
ROOT_DIR=$(pwd)

if [ -z $OUT_DIR ]; then
	OUT_DIR="out_mali"
fi

BUILDSH=vendor/samsung_slsi/script/build.sh
BUILD_MALI_CORE_SH=vendor/samsung_slsi/script/build_mali_core.sh

SCRIPT_START_TIME=`date +%y%m%d_%H%M%S`

if [ -z $MOK_BASE_DIR ] ; then
	echo "MOK_BASE_DIR is not set. Exiting (1)"
	exit 1
fi

MOK_OUT_DIR=${MOK_BASE_DIR}/${SCRIPT_START_TIME}-${BUILD_PRODUCT}-${BUILD_VARIANT}-${BUILD_OPTION}

# If OUT_LOG_DIR is not defined, log to standard directory
if [ -z $OUT_LOG_DIR ] ; then
	OUT_LOG_DIR=${MOK_OUT_DIR}/logs
fi

# If OUT_LOG_DIR is not defined, log to standard directory
if [ -z $OUT_IMAGE_DIR ] ; then
	OUT_IMAGE_DIR=${MOK_OUT_DIR}/images
fi

# If OUT_LOG_DIR is not defined, log to standard directory
if [ -z $MALI_SNAPSHOT_EXPORT_DIR ] ; then
	MALI_SNAPSHOT_EXPORT_DIR=${MOK_OUT_DIR}/mali_snapshot
fi

MALI_DIR=vendor/arm/snapshot_mali
MALI_SNAPSHOT_DIR=$ROOT_DIR/$MALI_DIR

##### Functions #####
function create_log_file()
{
	LOG_FILE_STDOUT=${OUT_LOG_DIR}/$1_${BUILD_VARIANT}
	LOG_FILE_STDERR=${OUT_LOG_DIR}/$1_${BUILD_VARIANT}_err
	RESULT_FILE=$OUT_LOG_DIR/result

	if [ "$MALI_LOG_STDOUT" == "true" ] ; then
		STDOUT=/dev/stdout
		STDERR=/dev/stderr
	else
		STDOUT=/dev/null
		STDERR=/dev/null
	fi
}

function log_build_result()
{
	let "ELAPSED_TIME=$5-$4"

	if [ $3 -ne 0 ]
	then
		echo
		echo "${color_error}Mali on Keystone $2 build failed for $1 elapsed_time: ${ELAPSED_TIME}${color_reset}"
		echo

		echo "[FAILED] $1 $2 elapsed_time: $ELAPSED_TIME" >> $RESULT_FILE
		return 1
	else
		echo
		echo "${color_success}Mali on Keystone $2 built for $1 elapsed_time: ${ELAPSED_TIME}${color_reset}"
		echo

		echo "[SUCCESS] $1 elapsed_time: $ELAPSED_TIME" >> $RESULT_FILE
		return 0
	fi
}

####### Functions #######
function build_multi()
{
	local prod_list=("$@")

	TITLE="Build Mali on Keystone $BUILD_OPTION"
	print_title

	for build_product in "${prod_list[@]}"
	do
		echo "* ${build_product} ${BUILD_VARIANT}"
	done
	echo

	mkdir -p $OUT_LOG_DIR

	for build_product in "${prod_list[@]}"
	do
		echo "Building Mali on Keystone $BUILD_OPTION for $build_product"

		create_log_file $build_product

		export OUT_DIR=$OUT_DIR
		export MALI_DIR=$MALI_DIR
		local build_start_time=`date +%s`

		### Clean Mali UMD Android BP temp files ###
		MALI_BP_INTERM_DIR=$OUT_DIR/target/product/$build_product/gen/STATIC_LIBRARIES/mali-android*
		rm -r $MALI_BP_INTERM_DIR

		echo "deleted $MALI_BP_INTERM_DIR"

		case "$build_product" in
			universal2100_r_mali)
				local build_vendorbootimage=true
				;;
			erd8825_s_mali)
				local build_vendorbootimage=true
				;;
		esac

		case "$BUILD_OPTION" in
			umd)
				$BUILDSH $build_product $BUILD_VARIANT mali \
					> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
					2> >(tee -a $LOG_FILE_STDERR >$STDERR);
				local ret=$?
				;;
			bootimage)
				$BUILDSH $build_product $BUILD_VARIANT bootimage \
					> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
					2> >(tee -a $LOG_FILE_STDERR >$STDERR);
				local ret=$?
				if [ "$build_vendorbootimage" == true ] ; then
					$BUILDSH $build_product $BUILD_VARIANT vendorbootimage \
						> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
						2> >(tee -a $LOG_FILE_STDERR >$STDERR);

				fi
				;;
			vendorbootimage)
				if [ "$build_vendorbootimage" == true ] ; then
					$BUILDSH $build_product $BUILD_VARIANT vendorbootimage \
						> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
						2> >(tee -a $LOG_FILE_STDERR >$STDERR);
					local ret=$?
				fi
				;;
			fullimage)
				$BUILDSH $build_product $BUILD_VARIANT \
					> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
					2> >(tee -a $LOG_FILE_STDERR >$STDERR);
				local ret=$?
				;;
		esac

		local build_end_time=`date +%s`
		log_build_result $build_product $BUILD_OPTION $ret $build_start_time $build_end_time

		if [ "$BUILD_OPTION" == umd ] && [ $ret -eq "0" ]
		then
			$BUILD_MALI_CORE_SH $build_product $BUILD_VARIANT move_umd \
				> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
				2> >(tee -a $LOG_FILE_STDERR >$STDERR);
		fi

		if [ "$BUILD_OPTION" == bootimage ] && [ $ret -eq "0" ]
		then
			OUT_IMAGE_DIR=$OUT_IMAGE_DIR $BUILD_MALI_CORE_SH $build_product $BUILD_VARIANT move_bootimage \
				> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
				2> >(tee -a $LOG_FILE_STDERR >$STDERR);
		fi

		if [ "$BUILD_OPTION" == vendorbootimage ] && [ $ret -eq "0" ]
		then
			OUT_IMAGE_DIR=$OUT_IMAGE_DIR $BUILD_MALI_CORE_SH $build_product $BUILD_VARIANT move_bootimage \
				> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
				2> >(tee -a $LOG_FILE_STDERR >$STDERR);
		fi

		if [ "$BUILD_OPTION" == fullimage ] && [ $ret -eq "0" ]
		then
			OUT_IMAGE_DIR=$OUT_IMAGE_DIR $BUILD_MALI_CORE_SH $build_product $BUILD_VARIANT move_fullimage \
				> >(tee -a $LOG_FILE_STDOUT >$STDOUT) \
				2> >(tee -a $LOG_FILE_STDERR >$STDERR);
		fi

	done
}

####### Mali Source Snapshot Functions #######
function create_snapshot()
{
	# Remove pre-existing snapshot directory
	rm -rf vendor/arm/snapshot_*

	TITLE="Create Mali Source Snapshot"
	print_title

	if [ -z $MALI_WORKING_DIR ] ; then
		echo 'MALI_WORKING_DIR not set. Please set it manually or in bashrc.'
		exit 1
	fi

	### Create and rsync snapshot mali dir ###
	if [ ! -d $MALI_SNAPSHOT_DIR ] ; then
		mkdir -p $MALI_SNAPSHOT_DIR;
	else
		rm -rf $MALI_SNAPSHOT_DIR/*;
	fi

	if [ -n "$MALI_WORKING_DIR" ] ; then
		rsync -a --exclude='.git/' $MALI_WORKING_DIR/ $MALI_SNAPSHOT_DIR;
	fi

	if [ -n "$MALI_KBASE_INTEGRATION_DIR" ] ; then
		rsync -a --exclude='.git/' $MALI_KBASE_INTEGRATION_DIR/ $MALI_SNAPSHOT_DIR;
	fi

	### Create snapshot information ###
	MALI_SNAPSHOT_LOG_ABS=$MALI_SNAPSHOT_DIR/snap_logs
	touch $MALI_SNAPSHOT_LOG_ABS;

	cat $MALI_SNAPSHOT_DIR/release_name | grep default | awk '{print $2}' | sed 's/\"//g' > $MALI_SNAPSHOT_LOG_ABS;
	echo >> $MALI_SNAPSHOT_LOG_ABS;

	echo $SCRIPT_START_TIME >> $MALI_SNAPSHOT_LOG_ABS;
	echo >> $MALI_SNAPSHOT_LOG_ABS;

	pushd . >> /dev/null;
	cd $MALI_WORKING_DIR;
	git log --oneline >> $MALI_SNAPSHOT_LOG_ABS;
	echo >> $MALI_SNAPSHOT_LOG_ABS;
	git status >> $MALI_SNAPSHOT_LOG_ABS;
	echo >> $MALI_SNAPSHOT_LOG_ABS;
	git diff >> $MALI_SNAPSHOT_LOG_ABS;
	popd >> /dev/null;
}

function export_snapshot()
{
	if [ ! -d $MALI_SNAPSHOT_DIR ] ; then
		# No snapshot directory to export
		return 0
	fi

	TITLE="Export Mali Source Snapshot to $MALI_SNAPSHOT_EXPORT_DIR"
	print_title

	if [ -d $MALI_SNAPSHOT_EXPORT_DIR ] ; then
		echo "$MALI_SNAPSHOT_EXPORT_DIR already exists... Exiting(1)"
		exit 1
	fi

	mv $MALI_SNAPSHOT_DIR $MALI_SNAPSHOT_EXPORT_DIR;
}

function move_fullimage()
{
	local prod_list=("$@")

	TITLE="Move Mali on Keystone Images"
	print_title

	for build_product in "${prod_list[@]}"
	do
		echo "* ${build_product} ${BUILD_VARIANT}"
	done
	echo

	for build_product in "${prod_list[@]}"
	do
		echo "Moving Mali on Keystone images for $build_product (time: $SCRIPT_START_TIME)"

		export OUT_DIR=$OUT_DIR
		export MALI_DIR=$MALI_DIR

		OUT_IMAGE_DIR=$OUT_IMAGE_DIR $BUILD_MALI_CORE_SH $build_product $BUILD_VARIANT move_fullimage
	done
}

function move_bootimage()
{
	local prod_list=("$@")

	TITLE="Move Mali on Keystone Boot Images"
	print_title

	for build_product in "${prod_list[@]}"
	do
		echo "* ${build_product} ${BUILD_VARIANT}"
	done
	echo

	for build_product in "${prod_list[@]}"
	do
		echo "Moving Mali on Keystone boot images for $build_product (time: $SCRIPT_START_TIME)"

		export OUT_DIR=$OUT_DIR
		export MALI_DIR=$MALI_DIR

		OUT_IMAGE_DIR=$OUT_IMAGE_DIR $BUILD_MALI_CORE_SH $build_product $BUILD_VARIANT move_bootimage
	done
}

####### Main Routines #######
case "$BUILD_PRODUCT" in
	all)
		PROD_LIST=( \
			universal9830_mali \
			universal2100_r_mali \
			universal9810_q_launching_mali \
			universal9820_mali \
		)
		;;
	*)
		PROD_LIST=($BUILD_PRODUCT)
esac

case "$BUILD_OPTION" in
	create_snapshot)
		create_snapshot
		exit 0
		;;
	export_snapshot)
		export_snapshot
		exit 0
		;;
	umd)
		;;
	bootimage)
		;;
	vendorbootimage)
		;;
	fullimage)
		;;
	move_fullimage)
		move_fullimage "${PROD_LIST[@]}"
		exit 0
		;;
	move_bootimage)
		move_bootimage "${PROD_LIST[@]}"
		exit 0
		;;
	"")
		BUILD_OPTION=fullimage
		;;
esac

if [ "$MALI_PREBUILT" == true ] ; then
	MALI_DIR=vendor/arm/mali
	build_multi "${PROD_LIST[@]}"
	check_exit
else
	create_snapshot
	check_exit
	build_multi "${PROD_LIST[@]}"
	export_snapshot
	check_exit
fi

exit 0
