#define LOG_TAG "ExynosDisplayHDR-Hal"
#include "../../../ExynosDisplayUtils.h"
#include "ExynosDisplayHDR.h"
#include "ExynosDisplayHDRDefaultLUT.h"
#include <utils/Log.h>

struct IntToConstString hdrIdToPath[8] = {
	{0,	SYSFS_HDR0_PATH},
	{1,	SYSFS_HDR1_PATH},
	{2,	SYSFS_HDR2_PATH},
	{3,	SYSFS_HDR3_PATH},
	{4,	SYSFS_HDR4_PATH},
	{5,	SYSFS_HDR5_PATH},
	{6,	SYSFS_HDR6_PATH},
	{7,	SYSFS_HDR7_PATH},
};

static int handleHDRAPS(int value)
{
	int ret = 0;
	ALOGD("handleHDRAPS: %d", value);
	string xml_path = CALIB_HDR_APSSET_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = DEFAULT_HDR_APS_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_APS", "aps", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/
	setAps(stream[0].c_str());
	stream.clear();
	return ret;
}

static int handleHDRCGC(int value)
{
	int ret = 0;
	ALOGD("handleHDRCGC: %d", value);
	string xml_path = CALIB_HDR_CGCSET_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = DEFAULT_HDR_CGC_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	for (int rgb = 0; rgb < 3; rgb++) {
		for(int idx = 0; idx < 17; idx++) {
			ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_CGC", "cgc17_enc", rgb, idx);
			if (ret < 0)
				return ret;

			/*printVector(stream);*/

			string str_idx;
			str_idx.clear();
			str_idx.append(std::to_string(rgb));
			str_idx.append(" ");
			str_idx.append(std::to_string(idx));

			setCgc17_idx(str_idx.c_str());
			setCgc17_enc(stream[0].c_str());
			str_idx.clear();
			stream.clear();
		}
	}

	setCgc17_dec("7");

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_CGC", "cgc17_con", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/
	setCgc17_con(stream[0].c_str());
	stream.clear();

	return ret;
}

static int handleHDRHSC(int value)
{
	int ret = 0;
	ALOGD("handleHDRHSC: %d", value);
	string xml_path = CALIB_HDR_HSCSET_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = DEFAULT_HDR_HSC_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	for(int idx = 0; idx < 3; idx++) {
		ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_HSC", "hsc48_lcg",idx, 0);
		if (ret < 0)
			return ret;

		/*printVector(stream);*/

		string str_idx;
		str_idx.clear();
		str_idx.append(std::to_string(idx));

		setHsc48_idx(str_idx.c_str());
		setHsc48_lcg(stream[0].c_str());
		stream.clear();
	}

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_HSC", "hsc", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/

	setHsc(stream[0].c_str());
	stream.clear();

	return ret;
}

static int handleHDRGAMMA(int value)
{
	int ret = 0;
	ALOGD("handleHDRGAMMA: %d", value);
	string xml_path = CALIB_HDR_GAMMASET_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = CALIB_HDR_GAMMASET_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_GAMMA", "gamma", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/
	setGamma(stream[0].c_str());
	stream.clear();
	return ret;
}

static int handleHDRDEGAMMA(int value)
{
	int ret = 0;
	ALOGD("handleHDRDEGAMMA: %d", value);
	string xml_path = CALIB_HDR_DEGAMMA_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = CALIB_HDR_DEGAMMA_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_DEGAMMA", "degamma", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/
	setDegamma(stream[0].c_str());
	stream.clear();
	return ret;
}

static int handleHDRGAMMA_MAT(int value)
{
	int ret = 0;
	ALOGD("handleHDRGAMMA_MAT: %d", value);
	string xml_path = CALIB_HDR_GAMMAMAT_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = CALIB_HDR_GAMMAMAT_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_GAMMAMAT", "gamma_matrix", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/
	setGammaMat(stream[0].c_str());
	stream.clear();
	return ret;
}

static int handleHDRSCL(int value)
{
	int ret = 0;
	ALOGD("handleHDRSCL: %d", value);
	string xml_path = CALIB_HDR_SCL_PATH;

	ret = existXML(xml_path.c_str());
	if (ret < 0) {
		xml_path = CALIB_HDR_SCL_PATH;
		ret = existXML(xml_path.c_str());
		if (ret < 0)
			return ret;
	}

	vector<string> stream;

	ret = parserFactoryXMLText(xml_path.c_str(), stream, "HDR_SCL", "scl", 0, 0);
	if (ret < 0)
		return ret;

	/*printVector(stream);*/
	setScl(stream[0].c_str());
	stream.clear();
	return ret;
}

static int printPPM(void)
{
	int ret = 0;
	struct ppm_info_ __ppm_info__;

	ret = read_ppm_file((char *)CALIB_HDR_PPMIMG_PATH, &__ppm_info__);
	if (ret) {
		ret = read_ppm_file((char *)DEFAULT_HDR_PPM_PATH, &__ppm_info__);
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

	ret = read_png_file((char *)CALIB_HDR_PNGIMG_PATH, &__png_info__);
	if (ret) {
		ret = read_png_file((char *)DEFAULT_HDR_PNG_PATH, &__png_info__);
		if (ret)
			goto err;
		ret = 0;
	}

	ALOGD("%s (width=%d, height=%d, color_type=%d, bit_depth=%d, row_pointer=%lx)",
			__func__, __png_info__.width, __png_info__.height,
			__png_info__.color_type, __png_info__.bit_depth,
			(unsigned long)__png_info__.row_pointers);
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

		case HDR_DEGAMMA:
			ret = handleHDRDEGAMMA(modeId);
			break;

		case HDR_GAMMA_MAT:
			ret = handleHDRGAMMA_MAT(modeId);
			break;

		case HDR_SCL:
			ret = handleHDRSCL(modeId);
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
	enum hdrpar {
		HDR_EN = 0,
		EOTF_EN, EOTF_X, EOTF_Y, NORMAL_EN, NORMAL_COEF, NORMAL_SHIFT,
		OETF_EN, OETF_X, OETF_Y,
		GM_EN, GM,  GM_OFFSET,
		TM_EN,  TM_X, TM_Y, TM_COEF, TM_RNGX, TM_RNGY,
	};
	int index[] = {
		1,
		1, 65, 65, 1, 1, 1,
		1, 65, 65,
		1, 9, 3,
		1, 33, 33, 3, 2, 2,
	};
	vector<string> out;
	int i;

	out = split(stream[HDR_EN], ',');
	if (out.size() != index[HDR_EN]) {
		ret = -1;
		ALOGE("<HDR_EN> number error (parsed %d, should-be %d, %d)", (int)out.size(), index[HDR_EN], en);
		goto err;
	}
	hdr_p->en = atoi(out[0].c_str());
	hdr_p->mul_en = atoi(out[0].c_str());
	out.clear();

	out = split(stream[EOTF_EN], ',');
	if (out.size() != index[EOTF_EN]) {
		ret = -1;
		ALOGE("<EOTF_EN> number error (parsed %d, should-be %d)", (int)out.size(), index[EOTF_EN]);
		goto err;
	}
	hdr_p->eotf.en = atoi(out[0].c_str());
	out.clear();

	out = split(stream[EOTF_X], ',');
	if (out.size() != index[EOTF_X]) {
		ret = -1;
		ALOGE("<EOTF_X> number error (parsed %d, should-be %d)", (int)out.size(), index[EOTF_X]);
		goto err;
	}
	for (i = 0; i < index[EOTF_X]; i++)
		hdr_p->eotf.lut_x[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[EOTF_Y], ',');
	if (out.size() != index[EOTF_Y]) {
		ret = -1;
		ALOGE("<EOTF_Y> number error (parsed %d, should-be %d)", (int)out.size(), index[EOTF_Y]);
		goto err;
	}
	for (i = 0; i < index[EOTF_Y]; i++)
		hdr_p->eotf.lut_y[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[NORMAL_EN], ',');
	if (out.size() != index[NORMAL_EN]) {
		ret = -1;
		ALOGE("<NORMAL_EN> number error (parsed %d, should-be %d)", (int)out.size(), index[NORMAL_EN]);
		goto err;
	}
	hdr_p->eotf.pq_en = atoi(out[0].c_str());
	out.clear();

	out = split(stream[NORMAL_COEF], ',');
	if (out.size() != index[NORMAL_COEF]) {
		ret = -1;
		ALOGE("<NORMAL_COEF> number error (parsed %d, should-be %d)", (int)out.size(), index[NORMAL_COEF]);
		goto err;
	}
	hdr_p->eotf.coef = atoi(out[0].c_str());
	out.clear();

	out = split(stream[NORMAL_SHIFT], ',');
	if (out.size() != index[NORMAL_SHIFT]) {
		ret = -1;
		ALOGE("<NORMAL_SHIFT> number error (parsed %d, should-be %d)", (int)out.size(), index[NORMAL_SHIFT]);
		goto err;
	}
	hdr_p->eotf.shift = atoi(out[0].c_str());
	out.clear();

	out = split(stream[OETF_EN], ',');
	if (out.size() != index[OETF_EN]) {
		ret = -1;
		ALOGE("<OETF_EN> number error (parsed %d, should-be %d)", (int)out.size(), index[OETF_EN]);
		goto err;
	}
	hdr_p->oetf.en = atoi(out[0].c_str());
	out.clear();

	out = split(stream[OETF_X], ',');
	if (out.size() != index[OETF_X]) {
		ret = -1;
		ALOGE("<OETF_X> number error (parsed %d, should-be %d)", (int)out.size(), index[OETF_X]);
		goto err;
	}
	for (i = 0; i < index[OETF_X]; i++)
		hdr_p->oetf.lut_x[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[OETF_Y], ',');
	if (out.size() != index[OETF_Y]) {
		ret = -1;
		ALOGE("<OETF_Y> number error (parsed %d, should-be %d)", (int)out.size(), index[OETF_Y]);
		goto err;
	}
	for (i = 0; i < index[OETF_Y]; i++)
		hdr_p->oetf.lut_y[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[GM_EN], ',');
	if (out.size() != index[GM_EN]) {
		ret = -1;
		ALOGE("<GM_EN> number error (parsed %d, should-be %d)", (int)out.size(), index[GM_EN]);
		goto err;
	}
	hdr_p->gm.en = atoi(out[0].c_str());
	out.clear();

	out = split(stream[GM], ',');
	if (out.size() != index[GM]) {
		ret = -1;
		ALOGE("<GM> number error (parsed %d, should-be %d)", (int)out.size(), index[GM]);
		goto err;
	}
	for (i = 0; i < index[GM]; i++)
		hdr_p->gm.coef[(i / 3)][(i % 3)] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[GM_OFFSET], ',');
	if (out.size() != index[GM_OFFSET]) {
		ret = -1;
		ALOGE("<GM_OFFSET> number error (parsed %d, should-be %d)", (int)out.size(), index[GM_OFFSET]);
		goto err;
	}
	for (i = 0; i < index[GM_OFFSET]; i++)
		hdr_p->gm.offs[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[TM_EN], ',');
	if (out.size() != index[TM_EN]) {
		ret = -1;
		ALOGE("<TM_EN> number error (parsed %d, should-be %d)", (int)out.size(), index[TM_EN]);
		goto err;
	}
	hdr_p->tm.en = atoi(out[0].c_str());
	out.clear();

	out = split(stream[TM_X], ',');
	if (out.size() != index[TM_X]) {
		ret = -1;
		ALOGE("<TM_X> number error (parsed %d, should-be %d)", (int)out.size(), index[TM_X]);
		goto err;
	}
	for (i = 0; i < index[TM_X]; i++)
	hdr_p->tm.lut_x[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[TM_Y], ',');
	if (out.size() != index[TM_Y]) {
		ret = -1;
		ALOGE("<TM_Y> number error (parsed %d, should-be %d)", (int)out.size(), index[TM_Y]);
		goto err;
	}
	for (i = 0; i < index[TM_Y]; i++)
		hdr_p->tm.lut_y[i] = atoi(out[i].c_str());
	out.clear();

	out = split(stream[TM_COEF], ',');
	if (out.size() != index[TM_COEF]) {
		ret = -1;
		ALOGE("<TM_COEF> number error (parsed %d, should-be %d)", (int)out.size(), index[TM_COEF]);
		goto err;
	}
	hdr_p->tm.coef_r = atoi(out[0].c_str());
	hdr_p->tm.coef_g = atoi(out[1].c_str());
	hdr_p->tm.coef_b = atoi(out[2].c_str());
	out.clear();

	out = split(stream[TM_RNGX], ',');
	if (out.size() != index[TM_RNGX]) {
		ret = -1;
		ALOGE("<TM_RNGX> number error (parsed %d, should-be %d)", (int)out.size(), index[TM_RNGX]);
		goto err;
	}
	hdr_p->tm.min_x = atoi(out[0].c_str());
	hdr_p->tm.max_x = atoi(out[1].c_str());
	out.clear();

	out = split(stream[TM_RNGY], ',');
	if (out.size() != index[TM_RNGY]) {
		ret = -1;
		ALOGE("<TM_RNGY> number error (parsed %d, should-be %d)", (int)out.size(), index[TM_RNGY]);
		goto err;
	}
	hdr_p->tm.min_y = atoi(out[0].c_str());
	hdr_p->tm.max_y = atoi(out[1].c_str());
	out.clear();

	err:
	return ret;
}

static void print_hdr_par(struct dpp_hdr_params *hdr_p)
{
	enum hdrpar {
		HDR_EN = 0,
		EOTF_EN, EOTF_X, EOTF_Y, NORMAL_EN, NORMAL_COEF, NORMAL_SHIFT,
		OETF_EN, OETF_X, OETF_Y,
		GM_EN, GM,  GM_OFFSET,
		TM_EN,  TM_X, TM_Y, TM_COEF, TM_RNGX, TM_RNGY,
	};
	int index[] = {
		1,
		1, 65, 65, 1, 1, 1,
		1, 65, 65,
		1, 9, 3,
		1, 33, 33, 3, 2, 2,
	};
	int i;
	int *ptr = NULL;
	string outstr;
	outstr.clear();

	ALOGD("<HDR_EN>%d</HDR_EN>", hdr_p->en);
	ALOGD("<EOTF_EN>%d</EOTF_EN>", hdr_p->eotf.en);

	ptr = (int *)(&hdr_p->eotf.lut_x[0]);
	outstr.append("<EOTF_X>");
	for (i = 0; i < index[EOTF_X]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</EOTF_X>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ptr = (int *)(&hdr_p->eotf.lut_y[0]);
	outstr.append("<EOTF_Y>");
	for (i = 0; i < index[EOTF_Y]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</EOTF_Y>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ALOGD("<NORMAL_EN>%d</NORMAL_EN>", hdr_p->eotf.pq_en);
	ALOGD("<NORMAL_COEF>%d</NORMAL_COEF>", hdr_p->eotf.coef);
	ALOGD("<NORMAL_SHIFT>%d</NORMAL_SHIFT>", hdr_p->eotf.shift);

	ALOGD("<OETF_EN>%d</OETF_EN>", hdr_p->oetf.en);

	ptr = (int *)(&hdr_p->oetf.lut_x[0]);
	outstr.append("<OETF_X>");
	for (i = 0; i < index[OETF_X]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</OETF_X>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ptr = (int *)(&hdr_p->oetf.lut_y[0]);
	outstr.append("<OETF_Y>");
	for (i = 0; i < index[OETF_Y]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</OETF_Y>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ALOGD("<GM_EN>%d</GM_EN>", hdr_p->gm.en);

	ptr = (int *)(&hdr_p->gm.coef[0][0]);
	outstr.append("<GM>");
	for (i = 0; i < index[GM]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</GM>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ptr = (int *)(&hdr_p->gm.offs[0]);
	outstr.append("<GM_OFFSET>");
	for (i = 0; i < index[GM_OFFSET]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</GM_OFFSET>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ALOGD("<TM_EN>%d</TM_EN>", hdr_p->tm.en);

	ptr = (int *)(&hdr_p->tm.lut_x[0]);
	outstr.append("<TM_X>");
	for (i = 0; i < index[TM_X]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</TM_X>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ptr = (int *)(&hdr_p->tm.lut_y[0]);
	outstr.append("<TM_Y>");
	for (i = 0; i < index[TM_Y]; i++) {
		outstr.append(std::to_string(*ptr++).c_str());
		outstr.append(",");
	}
	outstr.pop_back();
	outstr.append("</TM_Y>");
	ALOGD("%s", outstr.c_str());
	outstr.clear();

	ALOGD("<TM_COEF>%d,%d,%d</TM_COEF>", hdr_p->tm.coef_r, hdr_p->tm.coef_g, hdr_p->tm.coef_b);
	ALOGD("<TM_RNGX>%d,%d</TM_RNGX>", hdr_p->tm.min_x, hdr_p->tm.max_x);
	ALOGD("<TM_RNGY>%d,%d</TM_RNGY>", hdr_p->tm.min_y, hdr_p->tm.max_y);

	return;
}

static int handle_hdr(int id, vector<string>& stream, int en)
{
	int ret = 0;
	struct dpp_hdr_params hdr_p;

	ret = populateHDRstream(&hdr_p, stream, en);
	if (ret < 0)
		goto err;

	print_hdr_par(&hdr_p);

	ret = setHDR(id, hdrIdToPath, ARRSIZE(hdrIdToPath), (char*)&hdr_p);
	if (ret < 0)
		goto err;
err:
	return ret;
}

static void getDefaultHdrLut(int id, struct dpp_hdr_params *hdr_p)
{
	int i, j;
	hdr_p->en = 1;
	hdr_p->mul_en = 1;

	if (ID_L1 == id || ID_L3 == id || ID_L7 == id) {
		hdr_p->oetf.en = 1;
		memcpy(hdr_p->oetf.lut_x,
			hdr_h_oetf_lut_x,
			sizeof(unsigned int)*HDR_OETF_H_POSX_LUT_CNT);
		memcpy(hdr_p->oetf.lut_y,
			hdr_h_oetf_lut_y,
			sizeof(unsigned int)*HDR_OETF_H_POSY_LUT_CNT);

		hdr_p->eotf.en = 1;
		hdr_p->eotf.pq_en = 1;
		hdr_p->eotf.coef = 39321;
		hdr_p->eotf.shift = 16;

		hdr_p->gm.en = 1;
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				hdr_p->gm.coef[i][j] = hdr_h_gm_coef[i][j];
			}
			hdr_p->gm.offs[i] = hdr_h_gm_offs[i];
		}

		hdr_p->tm.en = 1;
		memcpy(hdr_p->tm.lut_x,
			hdr_tm_lut_x,
			sizeof(unsigned int)*HDR_TM_POSX_LUT_CNT);
		memcpy(hdr_p->tm.lut_y,
			hdr_tm_lut_y,
			sizeof(unsigned int)*HDR_TM_POSY_LUT_CNT);
		hdr_p->tm.coef_r = 214;
		hdr_p->tm.coef_g = 740;
		hdr_p->tm.coef_b = 70;
		hdr_p->tm.min_x = 0;
		hdr_p->tm.max_x = 0;
		hdr_p->tm.min_y = 256;
		hdr_p->tm.max_y = 256;
	} else {
		hdr_p->oetf.en = 1;
		memcpy(hdr_p->oetf.lut_x,
			hdr_l_oetf_lut_x,
			sizeof(unsigned int)*((HDR_OETF_H_POSX_LUT_CNT+1)/2));
		memcpy(hdr_p->oetf.lut_y,
			hdr_l_oetf_lut_y,
			sizeof(unsigned int)*((HDR_OETF_H_POSY_LUT_CNT+1)/2));

		hdr_p->eotf.en = 1;
		memcpy(hdr_p->eotf.lut_x,
			hdr_eotf_lut_x,
			sizeof(unsigned int)*HDR_EOTF_POSX_LUT_CNT);
		memcpy(hdr_p->eotf.lut_y,
			hdr_eotf_lut_y,
			sizeof(unsigned int)*HDR_EOTF_POSY_LUT_CNT);

		hdr_p->gm.en = 1;
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				hdr_p->gm.coef[i][j] = hdr_l_gm_coef[i][j];
			}
			hdr_p->gm.offs[i] = hdr_l_gm_offs[i];
		}

		hdr_p->tm.en = 0;
	}
}

static int handle_hdr_default(void)
{
	int ret = 0;
	struct dpp_hdr_params hdr_p[ARRSIZE(hdrIdToPath)];
	int i;

	for (i = 0; i < ARRSIZE(hdrIdToPath); i++) {
		getDefaultHdrLut(i, &hdr_p[i]);
		/*print_hdr_par(&hdr_p[i]);*/
	}

	for (i = 0; i < ARRSIZE(hdrIdToPath); i++)
		setHDR(i, hdrIdToPath, ARRSIZE(hdrIdToPath), (char*)&hdr_p[i]);

	return ret;
}

static int handle_hdrset(int caseId, int modeId, int cookie)
{
	int ret = 0, i;
	const char *arr[] = {
		"HDR_EN",
		"EOTF_EN", "EOTF_X", "EOTF_Y", "NORMAL_EN", "NORMAL_COEF", "NORMAL_SHIFT",
		"OETF_EN","OETF_X", "OETF_Y",
		"GM_EN","GM", "GM_OFFSET",
		"TM_EN", "TM_X", "TM_Y", "TM_COEF", "TM_RNGX", "TM_RNGY",
	};
	string xml_path = CALIB_HDR_LUTSET_PATH;
	vector<string> stream;
	int hdr_id = modeId;
	int hdr_en = cookie;

	ALOGD("handle_hdrset (%d %d %d)", caseId, modeId, cookie);

	switch (caseId) {
		case HDR_SAMPLE:
			handle_hdr_default();
		break;

		default:
			ret = existXML(xml_path.c_str());
			if (ret < 0) {
				xml_path = DEFAULT_HDR_LUT_PATH;
				ret = existXML(xml_path.c_str());
				if (ret < 0)
					return ret;
			}

			for (i = 0; i < 19; i++) {
				ret = parserXML(xml_path.c_str(), stream, "HDR", arr[i]);
			if (ret < 0)
				return ret;
			}

			/*printVector(stream);*/
			handle_hdr(hdr_id, stream, hdr_en);
		break;
	}

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
