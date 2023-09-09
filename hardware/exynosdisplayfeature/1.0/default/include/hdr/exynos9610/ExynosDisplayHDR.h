#ifndef VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYHDR_H
#define VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYHDR_H

#define SYSFS_HDR3_PATH "/sys/class/hdr3/hdr3/hdr"

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

struct dpp_hdr_params {
	unsigned int hdr_en;
	unsigned int oetf_en;
	unsigned int oetf_x[33];
	unsigned int oetf_y[33];
	unsigned int eotf_en;
	unsigned int eotf_x[129];
	unsigned int eotf_y[129];
	unsigned int gm_en;
	unsigned int gm_coef[9];
	unsigned int tm_en;
	unsigned int tm_coef[3];
	unsigned int tm_rngx[2];
	unsigned int tm_rngy[2];
	unsigned int tm_x[33];
	unsigned int tm_y[33];
};

int handleHDR(int displayId, int caseId, int modeId, int cookie);
#endif  // VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYHDR_H
