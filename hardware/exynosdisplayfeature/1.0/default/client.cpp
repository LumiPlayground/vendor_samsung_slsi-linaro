#define LOG_TAG "exynosdisplayfeature_client"

#include <vendor/samsung_slsi/hardware/exynosdisplayfeature/1.0/IExynosDisplayFeature.h>
#include <android/log.h>
#include <binder/ProcessState.h>
#include <hidl/LegacySupport.h>
#include "ExynosDisplayFeature.h"

#if defined(__EXYNOS9610_DPU__)
#include "include/hdr/exynos9610/ExynosDisplayHDR.h"
#elif defined(__EXYNOS9630_DPU__)
#include "include/hdr/exynos9630/ExynosDisplayHDR.h"
#elif defined(__EXYNOS9815_DPU__)
#include "include/hdr/exynos9815/ExynosDisplayHDR.h"
#else
#include "include/hdr/exynos9815/ExynosDisplayHDR.h"
#endif

using vendor::samsung_slsi::hardware::exynosdisplayfeature::V1_0::IExynosDisplayFeature;
using android::hardware::defaultPassthroughServiceImplementation;
using vendor::samsung_slsi::hardware::exynosdisplayfeature::V1_0::implementation::ExynosDisplayFeature;
using namespace vendor::samsung_slsi::hardware::exynosdisplayfeature::V1_0;

static int32_t convert_displayId(char *arg)
{
	int32_t displayId = MAX_DISPLAY_ID;

	if (!strcmp("BLANK", arg))
		displayId = BLANK;
	else if (!strcmp("UNBLANK", arg))
		displayId = UNBLANK;
	else if (!strcmp("FRM_UPDATE", arg))
		displayId = FRAME_UPDATE;
	else if (!strcmp("SET_BL", arg))
		displayId = SET_BACKLIGHT;
	else if (!strcmp("DQE_SET", arg))
		displayId = DQE_SET;
	else if (!strcmp("HDR_SET", arg))
		displayId = HDR_SET;
	else
		displayId = atoi(arg);

	return displayId;
}

static int32_t convert_caseId(char *arg)
{
	int32_t caseId = MAX_CASE_ID;

	if (!strcmp("PPM", arg))
		caseId = FORMAT_PPM;
	else if (!strcmp("PNG", arg))
		caseId = FORMAT_PNG;
	else if (!strcmp("RAW", arg))
		caseId = FORMAT_RAW;
	else if (!strcmp("RAW8", arg))
		caseId = FORMAT_RAW8;
	else if (!strcmp("EYECARE", arg))
		caseId = EYECARE_STATE;
	else if (!strcmp("AUTO_CONTRAST", arg))
		caseId = AUTOMATIC_CONTRAST_STATE;
	else if (!strcmp("INC_CONTRAST", arg))
		caseId = INCREASED_CONTRAST_STATE;
	else if (!strcmp("NIGHT", arg))
		caseId = NIGHT_STATE;
	else if (!strcmp("HDR_APS", arg))
		caseId = HDR_APS;
	else if (!strcmp("HDR_CGC", arg))
		caseId = HDR_CGC;
	else if (!strcmp("HDR_HSC", arg))
		caseId = HDR_HSC;
	else if (!strcmp("HDR_GAMMA", arg))
		caseId = HDR_GAMMA;
	else if (!strcmp("HDR_DEGAMMA", arg))
		caseId = HDR_DEGAMMA;
	else if (!strcmp("HDR_GAMMA_MAT", arg))
		caseId = HDR_GAMMA_MAT;
	else if (!strcmp("HDR_SCL", arg))
		caseId = HDR_SCL;
	else if (!strcmp("HDR_SAMPLE", arg))
		caseId = HDR_SAMPLE;
	else
		caseId = atoi(arg);

	return caseId;
}

static int32_t convert_modeId(char *arg)
{
	int32_t modeId = MAX_MODE_ID;

	if (!strcmp("WARM", arg))
		modeId = AUTOMATIC_CONTRAST_WARM;
	else if (!strcmp("NORMAL", arg))
		modeId = AUTOMATIC_CONTRAST_NORMAL;
	else if (!strcmp("COLD", arg))
		modeId = AUTOMATIC_CONTRAST_COLD;
	else
		modeId = atoi(arg);

	return modeId;
}

static int32_t convert_cookie(char *arg)
{
	int32_t cookie = -1;

	cookie = atoi(arg);

	return cookie;
}

int convert_argument(int32_t *out, char *in[])
{
	int32_t displayId = 0;
	int32_t caseId = 0;
	int32_t modeId = 0;
	int32_t cookie = 0;
	int ret = 0;

	displayId = convert_displayId(in[0]);

	switch (displayId) {
		case BLANK:
		case UNBLANK:
			break;
		case FRAME_UPDATE:
			caseId = convert_caseId(in[1]);
			modeId = convert_modeId(in[2]);
			cookie = convert_cookie(in[3]);
			break;
		case SET_BACKLIGHT:
			cookie = convert_cookie(in[3]);
			break;
		case HDR_SET:
			caseId = convert_caseId(in[1]);
			modeId = convert_modeId(in[2]);
			cookie = convert_cookie(in[3]);
			break;
		case DQE_SET:
			caseId = convert_caseId(in[1]);
			modeId = convert_modeId(in[2]);
			break;
		case MAX_DISPLAY_ID:
		default:
			ALOGD("displayId not defined (%s)\n", in[0]);
			ret = -1;
			break;
	}

	out[0] = displayId;
	out[1] = caseId;
	out[2] = modeId;
	out[3] = cookie;

	return ret;
}

int main(int argc, char *argv[]) {
	static android::sp<IExynosDisplayFeature> sExynosDisplayFeature;
	int ret = 0;

	if (argc != 5) {
		ALOGD("usage: ./exynosdisplayfeature_client 0 1 2 3\n");
		return -1;
	}

	ALOGD("exynosdisplayfeature_client %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);

	int32_t param[5] = {0,};

	ret = convert_argument(&param[1], &argv[1]);
	if (ret < 0)
		goto err;

	sExynosDisplayFeature = IExynosDisplayFeature::getService();
	sExynosDisplayFeature->setHDR(param[1], param[2], param[3], param[4]);

err:
	return ret;
}
