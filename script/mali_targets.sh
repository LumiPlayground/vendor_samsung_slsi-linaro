#!/bin/bash

PROD_LIST=( \
	universal9830_mali \
	universal2100_r_mali \
	universal9810_q_launching_mali \
	universal9820_mali \
	universal9825_r_mali \
	erd8825_s_mali \
	universal3830_s_mali \
	universal3830_qrs_mr_mali \
	erd8535_t_mali \
)

BOOT_IMG_LIST_2100_R=( \
	boot.img \
	dtbo.img \
	vendor_boot.img \
)

PLAT_IMG_LIST_2100_R=( \
	vbmeta.img \
	vbmeta_system.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST_8825_S=( \
	boot.img \
	dtbo.img \
	vendor_boot.img \
)

PLAT_IMG_LIST_8825_S=( \
	vbmeta.img \
	vbmeta_system.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST_9810_Q_LAUNCHING=( \
	boot.img \
	dtb.img \
	dtbo.img \
	ramdisk.img \
)

PLAT_IMG_LIST_9810_Q_LAUNCHING=( \
	vbmeta.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST_9820=( \
	boot.img \
	dtb.img \
	dtbo.img \
)

PLAT_IMG_LIST_9820=( \
	vbmeta.img \
	recovery.img \
	system.img \
	vendor.img \
)

BOOT_IMG_LIST_9825=( \
	boot.img \
	dtb.img \
	dtbo.img \
)

PLAT_IMG_LIST_9825=( \
	vbmeta.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST_9830=( \
	boot.img \
	dtbo.img \
)

PLAT_IMG_LIST_9830=( \
	vbmeta.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST_3830=( \
	boot.img \
	dtbo.img \
	dtb.img \
)

PLAT_IMG_LIST_3830=( \
	vbmeta.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST_8535_T=( \
	boot.img \
	dtbo.img \
	vendor_boot.img \
)

PLAT_IMG_LIST_8535_T=( \
	vbmeta.img \
	vbmeta_system.img \
	recovery.img \
	super.img \
)

BOOT_IMG_LIST=""
PLAT_IMG_LIST=""

function select_img_list()
{
	case "$1" in
		universal2100_r_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_2100_R[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_2100_R[@]})
			;;
		erd8825_s_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_8825_S[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_8825_S[@]})
			;;
		universal9830_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_9830[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_9830[@]})
			;;
		universal9820_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_9820[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_9820[@]})
			;;
		universal9825_r_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_9825[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_9825[@]})
			;;
		universal9810_q_launching_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_9810_Q_LAUNCHING[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_9810_Q_LAUNCHING[@]})
			;;
		universal3830_s_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_3830[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_3830[@]})
			;;
		universal3830_qrs_mr_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_3830[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_3830[@]})
			;;
		erd8535_s_mali)
			BOOT_IMG_LIST=(${BOOT_IMG_LIST_8535_T[@]})
			PLAT_IMG_LIST=(${PLAT_IMG_LIST_8535_T[@]})
			;;
		*)
			BOOT_IMG_LIST=""
			PLAT_IMG_LIST=""
			;;
	esac
}

