#define LOG_TAG "ExynosDisplayHDR-Hal"
#include "../../../ExynosDisplayUtils.h"
#include "ExynosDisplayHDR.h"
#include <utils/Log.h>

struct IntToConstString hdrIdToPath[1] = {
	{3,	SYSFS_HDR3_PATH},
};

static int handleHDRAPS(int value)
{
	int ret = 0;
	ALOGD("handleHDRCGC: %d", value);

	vector<string> stream;
	ret = parserXML(CALIB_HDR_APSSET_PATH, stream, "HDR_APS", "aps");
	if (ret < 0) {
		ret = parserXML(DEFAULT_HDR_APS_PATH, stream, "HDR_APS", "aps");
		if (ret < 0)
			return ret;
	}

	/* printVector(stream); */

	value = 0;
	setAps(stream[value].c_str());
	stream.clear();
	return ret;
}

static int handleHDRCGC(int value)
{
	int ret = 0;
	ALOGD("handleHDRCGC: %d", value);

	vector<string> stream;
	ret = parserXML(CALIB_HDR_CGCSET_PATH, stream, "HDR_CGC", "cgc");
	if (ret < 0) {
		ret = parserXML(DEFAULT_HDR_CGC_PATH, stream, "HDR_CGC", "cgc");
		if (ret < 0)
			return ret;
	}

	/* printVector(stream); */

	value = 0;
	setCgc(stream[value].c_str());
	stream.clear();
	return ret;
}

static int handleHDRHSC(int value)
{
	int ret = 0;
	ALOGD("handleHDRHSC: %d", value);

	vector<string> stream;
	ret = parserXML(CALIB_HDR_HSCSET_PATH, stream, "HDR_HSC", "hsc");
	if (ret < 0) {
		ret = parserXML(DEFAULT_HDR_HSC_PATH, stream, "HDR_HSC", "hsc");
		if (ret < 0)
			return ret;
	}

	/* printVector(stream); */

	value = 0;
	setHsc(stream[value].c_str());
	stream.clear();
	return ret;
}

static int handleHDRGAMMA(int value)
{
	int ret = 0;
	ALOGD("handleHDRGAMMA: %d", value);

	vector<string> stream;
	ret = parserXML(CALIB_HDR_GAMMASET_PATH, stream, "HDR_GAMMA", "gamma");
	if (ret < 0) {
		ret = parserXML(DEFAULT_HDR_GAMMA_PATH, stream, "HDR_GAMMA", "gamma");
		if (ret < 0)
			return ret;
	}

	/* printVector(stream); */

	value = 0;
	setGamma(stream[value].c_str());
	stream.clear();
	return ret;
}

static int printPPM(void)
{
	int ret = 0;
	struct ppm_info_ __ppm_info__;

	ret = read_ppm_file(CALIB_HDR_PPMIMG_PATH, &__ppm_info__);
	if (ret) {
		ret = read_ppm_file(DEFAULT_HDR_PPM_PATH, &__ppm_info__);
		if (ret)
			goto err;
		ret = 0;
	}

	print_ppm_file(&__ppm_info__);
err:
	return ret;
}

static int printPNG(void)
{
	int ret = 0;
	struct png_info_ __png_info__;

	ret = read_png_file(CALIB_HDR_PNGIMG_PATH, &__png_info__);
	if (ret) {
		ret = read_png_file(DEFAULT_HDR_PNG_PATH, &__png_info__);
		if (ret)
			goto err;
		ret = 0;
	}

	ALOGD("%s (width=%d, height=%d, color_type=%d, bit_depth=%d, row_pointer=%lx)",
			__func__, __png_info__.width, __png_info__.height,
			__png_info__.color_type, __png_info__.bit_depth,
			__png_info__.row_pointers);
	print_png_file(&__png_info__);
err:
	return ret;
}

static int printRAW(int modeId, int cookie, int bpp)
{
	int ret = 0;

	switch (bpp) {
		case 8:
			ret = print_raw_data_RGBA8(modeId, cookie);
			break;
		case 10:
			ret = print_raw_data_RGBA10(modeId, cookie);
			break;
		default:
			ret = print_raw_data_RGBA10(modeId, cookie);
			break;
	}
err:
	return ret;
}

static int handle_dqeset(int caseId, int modeId, int cookie)
{
	int ret = 0;

	ALOGD("handle_dqeset (%d %d %d)", caseId, modeId, cookie);
	switch(caseId) {
		case HDR_APS:
			ret = handleHDRAPS(modeId);
			break;

		case HDR_CGC:
			ret = handleHDRCGC(modeId);
			break;

		case HDR_HSC:
			ret = handleHDRHSC(modeId);
			break;

		case HDR_GAMMA:
			ret = handleHDRGAMMA(modeId);
			break;

		default:
			ALOGD("case id not defined(%d)", caseId);
			ret = -1;
			break;
	}
	return ret;
}

static int populateHDRstream(struct dpp_hdr_params *hdr_p, vector<string>& stream, int en)
{
	int ret = 0;
	enum hdrpar {CON = 0, TM_X, TM_Y, TM_COEF, TM_RNGX, TM_RNGY,
		GM, EOTF_X, EOTF_Y, OETF_X, OETF_Y};
	int index[] = {5, 33, 33, 3, 2, 2, 9, 129, 129, 33, 33};
	vector<string> out;
	int i;

	out = split(stream[CON], ',');
	if (out.size() != index[CON]) {
		ret = -1;
		ALOGE("<COM></COM> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[CON]);
		goto err;
	}

	if (!en) {
		hdr_p->hdr_en = en;
	} else {
		hdr_p->hdr_en = atoi(out[0].c_str());
	}
	hdr_p->eotf_en = atoi(out[1].c_str());
	hdr_p->gm_en = atoi(out[2].c_str());
	hdr_p->oetf_en = atoi(out[3].c_str());
	hdr_p->tm_en = atoi(out[4].c_str());
	out.clear();

	out = split(stream[TM_X], ',');
	if (out.size() != index[TM_X]) {
		ret = -1;
		ALOGE("<TM_X></TM_X> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[TM_X]);
		goto err;
	}
	for (i = 0; i < index[TM_X]; i++) {
		hdr_p->tm_x[i] = atoi(out[i].c_str());
	}
	out.clear();

	out = split(stream[TM_Y], ',');
	if (out.size() != index[TM_Y]) {
		ret = -1;
		ALOGE("<TM_Y></TM_Y> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[TM_Y]);
		goto err;
	}
	for (i = 0; i < index[TM_Y]; i++) {
		hdr_p->tm_y[i] = atoi(out[i].c_str());
	}
	out.clear();

	out = split(stream[TM_COEF], ',');
	if (out.size() != index[TM_COEF]) {
		ret = -1;
		ALOGE("<TM_COEF></TM_COEF> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[TM_COEF]);
		goto err;
	}
	hdr_p->tm_coef[0] = atoi(out[0].c_str());
	hdr_p->tm_coef[1] = atoi(out[1].c_str());
	hdr_p->tm_coef[2] = atoi(out[2].c_str());
	out.clear();

	out = split(stream[TM_RNGX], ',');
	if (out.size() != index[TM_RNGX]) {
		ret = -1;
		ALOGE("<TM_RNGX></TM_RNGX> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[TM_RNGX]);
		goto err;
	}
	hdr_p->tm_rngx[0] = atoi(out[0].c_str());
	hdr_p->tm_rngx[1] = atoi(out[1].c_str());
	out.clear();

	out = split(stream[TM_RNGY], ',');
	if (out.size() != index[TM_RNGY]) {
		ret = -1;
		ALOGE("<TM_RNGY></TM_RNGY> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[TM_RNGY]);
		goto err;
	}
	hdr_p->tm_rngy[0] = atoi(out[0].c_str());
	hdr_p->tm_rngy[1] = atoi(out[1].c_str());
	out.clear();

	out = split(stream[GM], ',');
	if (out.size() != index[GM]) {
		ret = -1;
		ALOGE("<GM></GM> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[GM]);
		goto err;
	}
	for (i = 0; i < index[GM]; i++) {
		hdr_p->gm_coef[i] = atoi(out[i].c_str());
	}
	out.clear();

	out = split(stream[EOTF_X], ',');
	if (out.size() != index[EOTF_X]) {
		ret = -1;
		ALOGE("<EOTF_X></EOTF_X> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[EOTF_X]);
		goto err;
	}
	for (i = 0; i < index[EOTF_X]; i++) {
		hdr_p->eotf_x[i] = atoi(out[i].c_str());
	}
	out.clear();

	out = split(stream[EOTF_Y], ',');
	if (out.size() != index[EOTF_Y]) {
		ret = -1;
		ALOGE("<EOTF_Y></EOTF_Y> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[EOTF_Y]);
		goto err;
	}
	for (i = 0; i < index[EOTF_Y]; i++) {
		hdr_p->eotf_y[i] = atoi(out[i].c_str());
	}
	out.clear();

	out = split(stream[OETF_X], ',');
	if (out.size() != index[OETF_X]) {
		ret = -1;
		ALOGE("<OETF_X></OETF_X> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[OETF_X]);
		goto err;
	}
	for (i = 0; i < index[OETF_X]; i++) {
		hdr_p->oetf_x[i] = atoi(out[i].c_str());
	}
	out.clear();

	out = split(stream[OETF_Y], ',');
	if (out.size() != index[OETF_Y]) {
		ret = -1;
		ALOGE("<OETF_Y></OETF_Y> parameter number error (parsed = %d, should-be %d)",
				out.size(), index[OETF_Y]);
		goto err;
	}
	for (i = 0; i < index[OETF_Y]; i++) {
		hdr_p->oetf_y[i] = atoi(out[i].c_str());
	}
	out.clear();

err:
	return ret;
}

static void print_hdr_par(struct dpp_hdr_params *hdr_p)
{
	enum hdrpar {CON = 0, TM_X, TM_Y, TM_COEF, TM_RNGX, TM_RNGY,
		GM, EOTF_X, EOTF_Y, OETF_X, OETF_Y};
	int index[] = {5, 33, 33, 3, 2, 2, 9, 129, 129, 33, 33};
	int i, j;

	ALOGD("CON : hdr_en (%d) eotf_en(%d) gm_en(%d) oetf(%d) tm_en(%d)",
		hdr_p->hdr_en, hdr_p->eotf_en, hdr_p->gm_en,
		hdr_p->oetf_en, hdr_p->tm_en);
	for (i = 0; i < index[TM_X] - 10; i+=10) {
		ALOGD("TM_X[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
			i, hdr_p->tm_x[i], i+1, hdr_p->tm_x[i+1], i+2, hdr_p->tm_x[i+2],
			i+3, hdr_p->tm_x[i+3], i+4, hdr_p->tm_x[i+4],
			i+5, hdr_p->tm_x[i+5], i+6, hdr_p->tm_x[i+6], i+7, hdr_p->tm_x[i+7],
			i+8, hdr_p->tm_x[i+8], i+9, hdr_p->tm_x[i+9]);
	}
	ALOGD("TM_X[%3d]:%7d, [%3d]:%7d, [%3d]:%7d", i, hdr_p->tm_x[i], i+1, hdr_p->tm_x[i+1], i+2, hdr_p->tm_x[i+2]);
	for (i = 0; i < index[TM_Y] - 10; i+=10) {
		ALOGD("TM_Y[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
			i, hdr_p->tm_y[i],	i+1, hdr_p->tm_y[i+1],	i+2, hdr_p->tm_y[i+2],
			i+3, hdr_p->tm_y[i+3],	i+4, hdr_p->tm_y[i+4],
			i+5, hdr_p->tm_y[i+5],	i+6, hdr_p->tm_y[i+6],	i+7, hdr_p->tm_y[i+7],
			i+8, hdr_p->tm_y[i+8],	i+9, hdr_p->tm_y[i+9]);
	}
	ALOGD("TM_Y[%3d]:%7d, [%3d]:%7d, [%3d]:%7d", i, hdr_p->tm_y[i], i+1, hdr_p->tm_y[i+1], i+2, hdr_p->tm_y[i+2]);

	i = 0;
	ALOGD("TMCOEF[%3d]:%7d, TMCOEF[%3d]:%7d, TMCOEF[%3d]:%7d", i, hdr_p->tm_coef[i], i+1, hdr_p->tm_coef[i+1], i+2, hdr_p->tm_coef[i+2]);
	ALOGD("TMRNG_X[%3d]:%7d, TMRNG_X[%3d]:%7d", i, hdr_p->tm_rngx[i], i+1, hdr_p->tm_rngx[i+1]);
	ALOGD("TMRNG_Y[%3d]:%7d, TMRNG_Y[%3d]:%7d", i, hdr_p->tm_rngy[i], i+1, hdr_p->tm_rngy[i+1]);

	i = 0;
	ALOGD("GM[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
		i, hdr_p->gm_coef[i],		i+1, hdr_p->gm_coef[i+1],	i+2, hdr_p->gm_coef[i+2],
		i+3, hdr_p->gm_coef[i+3],	i+4, hdr_p->gm_coef[i+4],
		i+5, hdr_p->gm_coef[i+5],	i+6, hdr_p->gm_coef[i+6],	i+7, hdr_p->gm_coef[i+7],
		i+8, hdr_p->gm_coef[i+8]);

	for (i = 0; i < index[EOTF_X] - 10; i+=10) {
		ALOGD("EOTF_X[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
			i,	hdr_p->eotf_x[i],	i+1, hdr_p->eotf_x[i+1],	i+2, hdr_p->eotf_x[i+2],
			i+3, 	hdr_p->eotf_x[i+3],	i+4, hdr_p->eotf_x[i+4],
			i+5, 	hdr_p->eotf_x[i+5],	i+6, hdr_p->eotf_x[i+6],	i+7, hdr_p->eotf_x[i+7],
			i+8, 	hdr_p->eotf_x[i+8],	i+9, hdr_p->eotf_x[i+9]);
	}
	ALOGD("EOTF_X[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
		i, hdr_p->eotf_x[i], i+1, hdr_p->eotf_x[i+1], i+2, hdr_p->eotf_x[i+2],
		i+3, hdr_p->eotf_x[i+3], i+4, hdr_p->eotf_x[i+4], i+5, hdr_p->eotf_x[i+5],
		i+6, hdr_p->eotf_x[i+6], i+7, hdr_p->eotf_x[i+7], i+8, hdr_p->eotf_x[i+8]);
	for (i = 0; i < index[EOTF_Y] - 10; i+=10) {
		ALOGD("EOTF_Y[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
			i,	hdr_p->eotf_y[i],	i+1, hdr_p->eotf_y[i+1],	i+2, hdr_p->eotf_y[i+2],
			i+3, 	hdr_p->eotf_y[i+3],	i+4, hdr_p->eotf_y[i+4],
			i+5, 	hdr_p->eotf_y[i+5],	i+6, hdr_p->eotf_y[i+6],	i+7, hdr_p->eotf_y[i+7],
			i+8, 	hdr_p->eotf_y[i+8],	i+9, hdr_p->eotf_y[i+9]);
	}
	ALOGD("EOTF_Y[%3d]:%7d, [%3d]:%7d, [%d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
		i, hdr_p->eotf_y[i], i+1, hdr_p->eotf_y[i+1], i+2, hdr_p->eotf_y[i+2],
		i+3, hdr_p->eotf_y[i+3], i+4, hdr_p->eotf_y[i+4], i+5, hdr_p->eotf_y[i+5],
		i+6, hdr_p->eotf_y[i+6], i+7, hdr_p->eotf_y[i+7], i+8, hdr_p->eotf_y[i+8]);

	for (i = 0; i < index[OETF_X]-10; i+=10) {
		ALOGD("OETF_X[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
			i,	hdr_p->oetf_x[i],	i+1, hdr_p->oetf_x[i+1],	i+2, hdr_p->oetf_x[i+2],
			i+3, 	hdr_p->oetf_x[i+3],	i+4, hdr_p->oetf_x[i+4],
			i+5, 	hdr_p->oetf_x[i+5],	i+6, hdr_p->oetf_x[i+6],	i+7, hdr_p->oetf_x[i+7],
			i+8, 	hdr_p->oetf_x[i+8],	i+9, hdr_p->oetf_x[i+9]);
	}
	ALOGD("OETF_X[%3d]:%7d, [%3d]:%7d, [%3d]:%7d", i, hdr_p->oetf_x[i], i+1, hdr_p->oetf_x[i+1], i+2, hdr_p->oetf_x[i+2]);
	for (i = 0; i < index[OETF_Y] - 10; i+=10) {
		ALOGD("OETF_Y[%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d, [%3d]:%7d",
			i,	hdr_p->oetf_y[i],	i+1, hdr_p->oetf_y[i+1],	i+2, hdr_p->oetf_y[i+2],
			i+3, 	hdr_p->oetf_y[i+3],	i+4, hdr_p->oetf_y[i+4],
			i+5, 	hdr_p->oetf_y[i+5],	i+6, hdr_p->oetf_y[i+6],	i+7, hdr_p->oetf_y[i+7],
			i+8, 	hdr_p->oetf_y[i+8],	i+9, hdr_p->oetf_y[i+9]);
	}
	ALOGD("OETF_Y[%3d]:%7d, [%3d]:%7d, [%3d]:%7d", i, hdr_p->oetf_y[i], i+1, hdr_p->oetf_y[i+1], i+2, hdr_p->oetf_y[i+2]);
}

static int handle_hdr(vector<string>& stream, int en)
{
	int ret = 0;
	struct dpp_hdr_params hdr_p;

	ret = populateHDRstream(&hdr_p, stream, en);
	if (ret < 0)
		goto err;

	print_hdr_par(&hdr_p);

	ret = setHDR(3, hdrIdToPath, ARRSIZE(hdrIdToPath), (char*)&hdr_p);
	if (ret < 0)
		goto err;
err:
	return ret;
}

static int handle_hdrset(int caseId, int modeId, int cookie)
{
	int ret = 0, i;
	const char *arr[] = {"CON", "TM_X", "TM_Y", "TM_COEF", "TM_RNGX", "TM_RNGY", "GM",
		"EOTF_X", "EOTF_Y", "OETF_X", "OETF_Y"};
	char *path = CALIB_HDR_LUTSET_PATH;
	vector<string> stream;
	int hdr_en = cookie;

	ALOGD("handle_hdrset (%d %d %d)", caseId, modeId, cookie);

	for (i = 0; i < 11; i++) {
		ret = parserXML(path, stream, "HDR", arr[i]);
		if (ret < 0) {
			path = DEFAULT_HDR_LUT_PATH;
			ret = parserXML(path, stream, "HDR", arr[i]);
			if (ret < 0)
				return ret;
		}

	}
	/* printVector(stream); */

	handle_hdr(stream, hdr_en);

	return ret;
}

static int handle_frmupdate(int caseId, int modeId, int cookie)
{
	int ret = 0;
	ALOGD("handle_frmupdate (%d %d %d)", caseId, modeId, cookie);

	switch (caseId) {
		case FORMAT_PPM:
			ret = printPPM();
			break;
		case FORMAT_PNG:
			ret = printPNG();
			break;
		case FORMAT_RAW:
			ret = printRAW(modeId, cookie, 10);
			break;
		case FORMAT_RAW8:
			ret = printRAW(modeId, cookie, 8);
			break;
		default:
			ALOGD("case id not defined(%d)", caseId);
			ret = -1;
			break;
	}
	return ret;
}

static int handle_backlight(int caseId, int modeId, int cookie)
{
	int ret = 0;
	ALOGD("handle_backlight (%d %d %d)", caseId, modeId, cookie);
	ret = set_backlight(cookie);
	return ret;
}

static int handle_unblank(int caseId, int modeId, int cookie)
{
	int ret = 0;
	ALOGD("handle_unblank (%d %d %d)", caseId, modeId, cookie);
	ret = set_unblank();
	return ret;
}

static int handle_blank(int caseId, int modeId, int cookie)
{
	int ret = 0;
	ALOGD("handle_blank (%d %d %d)", caseId, modeId, cookie);
	ret = set_blank();
	return ret;
}

int handleHDR(int displayId, int caseId, int modeId, int cookie)
{
	int ret = 0;
	ALOGD("handleFeature (%d %d %d %d)", displayId, caseId, modeId, cookie);

	switch (displayId) {
		case BLANK:
			ret = handle_blank(caseId, modeId, cookie);
			break;
		case UNBLANK:
			ret = handle_unblank(caseId, modeId, cookie);
			break;
		case SET_BACKLIGHT:
			ret = handle_backlight(caseId, modeId, cookie);
			break;
		case FRAME_UPDATE:
			ret = handle_frmupdate(caseId, modeId, cookie);
			break;
		case HDR_SET:
			ret = handle_hdrset(caseId, modeId, cookie);
			break;
		case DQE_SET:
			ret = handle_dqeset(caseId, modeId, cookie);
			break;
		default:
			ALOGD("not defined displayId(%d)", displayId);
			break;
	}

	return ret;
}
