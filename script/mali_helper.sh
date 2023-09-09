####### Mali on Keystone Environment setup #######

BUILD_MALI_CORE_SH=vendor/samsung_slsi/script/build_mali_core.sh

if [ -z $MALI_DIR ] ; then
	MALI_DIR=vendor/arm/mali
fi

if [ -f $MALI_DIR/setup_android ]; then
	export MALI_PREBUILT=false
else
	export MALI_PREBUILT=true
fi

MALI_DIR=$MALI_DIR OUT_DIR=$OUT_DIR $BUILD_MALI_CORE_SH $BUILD_PRODUCT $BUILD_VARIANT prepare_build

if [ $? -eq 0 ]; then
	MALI_ON_KEYSTONE=true
	export MALI_ON_KEYSTONE=$MALI_ON_KEYSTONE
else
	OUT_DIR=$OUT_DIR MALI_DIR=$MALI_DIR $BUILD_MALI_CORE_SH $BUILD_PRODUCT $BUILD_VARIANT restore_kbase;
fi

##### Mali on Keystone Functions #####
function setup_mali_build_host_tools()
{
	if [ "true" == "$MALI_ON_KEYSTONE" ] && [ "false" == "$MALI_PREBUILT" ] ; then
		OUT_DIR=host_tools lunch full_$BUILD_PRODUCT-$BUILD_VARIANT > /dev/null
		MALI_DIR=$MALI_DIR OUT_DIR=$OUT_DIR $BUILD_MALI_CORE_SH $BUILD_PRODUCT $BUILD_VARIANT setup_android;
	fi
}

function build_mali_ddk()
{
	if [ "true" == "$MALI_ON_KEYSTONE" ]; then
		OUT_DIR=$OUT_DIR MALI_DIR=$MALI_DIR $BUILD_MALI_CORE_SH $BUILD_PRODUCT $BUILD_VARIANT build_umd;
		check_exit
	fi
}
