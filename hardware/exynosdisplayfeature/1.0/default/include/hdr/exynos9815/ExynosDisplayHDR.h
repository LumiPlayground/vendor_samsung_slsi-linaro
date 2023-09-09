#ifndef VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYHDR_H
#define VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYHDR_H

#define SYSFS_HDR0_PATH "/sys/class/hdr0/hdr0/hdr"
#define SYSFS_HDR1_PATH "/sys/class/hdr1/hdr1/hdr"
#define SYSFS_HDR2_PATH "/sys/class/hdr2/hdr2/hdr"
#define SYSFS_HDR3_PATH "/sys/class/hdr3/hdr3/hdr"
#define SYSFS_HDR4_PATH "/sys/class/hdr4/hdr4/hdr"
#define SYSFS_HDR5_PATH "/sys/class/hdr5/hdr5/hdr"
#define SYSFS_HDR6_PATH "/sys/class/hdr6/hdr6/hdr"
#define SYSFS_HDR7_PATH "/sys/class/hdr7/hdr7/hdr"

struct IntToConstString {
	int integer;
	const char *string;
};

#define DEFAULT_HDR_PNG_PATH	"/vendor/etc/dqe/HDR_default_img.png"
#define DEFAULT_HDR_PPM_PATH	"/vendor/etc/dqe/HDR_default_img.ppm"
#define DEFAULT_HDR_LUT_PATH	"/vendor/etc/dqe/HDR_default_lut.xml"
#define DEFAULT_HDR_APS_PATH	"/vendor/etc/dqe/HDR_default_aps.xml"
#define DEFAULT_HDR_CGC_PATH	"/vendor/etc/dqe/HDR_default_cgc.xml"
#define DEFAULT_HDR_HSC_PATH	"/vendor/etc/dqe/HDR_default_hsc.xml"
#define DEFAULT_HDR_GAMMA_PATH	"/vendor/etc/dqe/HDR_default_gamma.xml"
#define CALIB_HDR_PNGIMG_PATH	"/data/vendor/hdr/HDR_calib_img.png"
#define CALIB_HDR_PPMIMG_PATH	"/data/vendor/hdr/HDR_calib_img.ppm"
#define CALIB_HDR_LUTSET_PATH	"/data/vendor/hdr/HDR_calib_lut.xml"
#define CALIB_HDR_APSSET_PATH	"/data/vendor/hdr/HDR_calib_aps.xml"
#define CALIB_HDR_CGCSET_PATH	"/data/vendor/hdr/HDR_calib_cgc.xml"
#define CALIB_HDR_HSCSET_PATH	"/data/vendor/hdr/HDR_calib_hsc.xml"
#define CALIB_HDR_GAMMASET_PATH	"/data/vendor/hdr/HDR_calib_gamma.xml"
#define CALIB_HDR_DEGAMMA_PATH	"/data/vendor/hdr/HDR_calib_degamma.xml"
#define CALIB_HDR_GAMMAMAT_PATH	"/data/vendor/hdr/HDR_calib_gamma_mat.xml"
#define CALIB_HDR_SCL_PATH	"/data/vendor/hdr/HDR_calib_scl.xml"

#define CONTRAST_WARM_VAL	0x0
#define CONTRAST_NORMAL_VAL	0x5
#define CONTRAST_COLD_VAL	0xa

enum DISPLAY_ID : int32_t {
	BLANK,
	UNBLANK,
	FRAME_UPDATE,
	SET_BACKLIGHT,
	HDR_SET,
	DQE_SET,
	MAX_DISPLAY_ID,
};

enum CASE_ID : int32_t {
	EYECARE_STATE,
	AUTOMATIC_CONTRAST_STATE,
	STANDARD_STATE,
	INCREASED_CONTRAST_STATE,
	NIGHT_STATE,
	HDR_APS,
	HDR_CGC,
	HDR_HSC,
	HDR_GAMMA,
	HDR_DEGAMMA,
	HDR_GAMMA_MAT,
	HDR_SCL,
	HDR_SAMPLE,
	FORMAT_PPM,
	FORMAT_PNG,
	FORMAT_RAW,
	FORMAT_RAW8,
	MAX_CASE_ID,
};

enum MODE_ID : int32_t {
	AUTOMATIC_CONTRAST_WARM,
	AUTOMATIC_CONTRAST_NORMAL,
	AUTOMATIC_CONTRAST_COLD,
	MAX_MODE_ID,
};

#define ID_L1	1
#define ID_L3	3
#define ID_L7	7

#define HDR_OETF_H_POSX_LUT_REG_CNT	(65)
#define HDR_OETF_H_POSX_LUT_CNT	(HDR_OETF_H_POSX_LUT_REG_CNT)
#define HDR_OETF_H_POSY_LUT_REG_CNT	(33)
#define HDR_OETF_H_POSY_LUT_CNT	(HDR_OETF_H_POSY_LUT_REG_CNT * 2 - 1)
struct hdr_lut_oetf {
	unsigned int en;
	unsigned int lut_x[HDR_OETF_H_POSX_LUT_CNT];
	unsigned int lut_y[HDR_OETF_H_POSY_LUT_CNT];
};

#define HDR_EOTF_POSX_LUT_REG_CNT	(33)
#define HDR_EOTF_POSX_LUT_CNT		(HDR_EOTF_POSX_LUT_REG_CNT * 2 - 1)
#define HDR_EOTF_POSY_LUT_REG_CNT	(65)
#define HDR_EOTF_POSY_LUT_CNT		(HDR_EOTF_POSY_LUT_REG_CNT)
struct hdr_lut_eotf {
	unsigned int en;
	unsigned int lut_x[HDR_EOTF_POSX_LUT_CNT];
	unsigned int lut_y[HDR_EOTF_POSY_LUT_CNT];
	/* only valid at HDR10PFLH */
	unsigned int pq_en;
	unsigned int coef;
	unsigned int shift;
};

struct hdr_lut_gm {
	unsigned int en;
	int coef[3][3];
	int offs[3];
};

#define HDR_TM_POSX_LUT_REG_CNT		(33)
#define HDR_TM_POSX_LUT_CNT		(HDR_TM_POSX_LUT_REG_CNT)
#define HDR_TM_POSY_LUT_REG_CNT		(33)
#define HDR_TM_POSY_LUT_CNT		(HDR_TM_POSY_LUT_REG_CNT)
struct hdr_lut_tm {
	unsigned int en;
	unsigned int lut_x[HDR_TM_POSX_LUT_CNT];
	unsigned int lut_y[HDR_TM_POSY_LUT_CNT];
	unsigned int coef_r;
	unsigned int coef_g;
	unsigned int coef_b;
	unsigned int min_x;
	unsigned int max_x;
	unsigned int min_y;
	unsigned int max_y;
};

struct dpp_hdr_params {
	unsigned int en;
	unsigned int mul_en; /* de-multiplication -> HDR -> re-multiplication */
	struct hdr_lut_oetf oetf;
	struct hdr_lut_eotf eotf;
	struct hdr_lut_gm gm;
	struct hdr_lut_tm tm;
};

int handleHDR(int displayId, int caseId, int modeId, int cookie);
#endif  // VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYHDR_H
