#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>

#include "FmRadioController_slsi.h"

/*******************************************************************************
 *
 * Global Variables
 *
 ******************************************************************************/
static FmRadioController_slsi * pFMRadio = NULL;
static fm_radio_state test_status;

static long current_channel;
static int current_band;
static int tuner_mode;
static int mute_onoff;
static int softmute_onoff;

static bool print_option_display;
static bool print_option_clear;

pthread_t thread_seek;
pthread_t thread_seekaging;

bool stop_seek;
bool test_seek;
bool stop_seekaging;
bool test_seekaging;

unsigned char test_read_data[TEST_DATA_SIZE][12];
radio_data_t test_radio_data[TEST_DATA_SIZE];
struct PIECC_data test_piecc[TEST_DATA_SIZE];
struct RTPlus_data test_rtplus[TEST_DATA_SIZE];
struct Final_RDS_data final_rds_data[10];

RadioText RT_Final;
RadioText RT_Buffered;
ServiceName PS_Final;
ServiceName PS_Buffered;

unsigned char rt_flag;
unsigned char ps_flag;

unsigned int test_data_index;
unsigned int test_data_index_rtplus;
unsigned int test_data_index_rds;

int print_mode;

/*******************************************************************************
 *
 * PRINT FUNCTIONS - Menu
 *
 ******************************************************************************/
void clear_screen(void)
{
	printf("\033[H\033[J");
}

void print_title(int subtitle)
{
	if (print_option_clear)
		clear_screen();

	printf("\n======================================================================\n\n");
	printf(" FM RADIO TEST");
	switch (subtitle) {
		case FM_CMD_MENU_SEEKAGING:
			printf(" - SEEK AGING\n\n");
			break;
		case FM_CMD_MENU_NOISESCAN:
			printf(" - NOISE SCAN\n\n");
			break;
		case FM_CMD_MENU_CHANNEL:
			printf(" - CHANNEL & SEEK TEST\n\n");
			break;
		case FM_CMD_MENU_MODE:
			printf(" - MODE SETTING\n\n");
			break;
		case FM_CMD_MENU_RDS:
			printf(" - RDS CONTROL\n\n");
			break;
		case FM_CMD_MENU_STATUS:
			printf(" - STATUS\n\n");
			break;
		case FM_CMD_MENU_OPTION:
			printf(" - OPTION SETTING\n\n");
			break;
		default:
			printf("\n\n");
			break;
	}
	printf("----------------------------------------------------------------------\n\n");
}

void print_sub_seekaging(void)
{
	print_title(FM_CMD_MENU_SEEKAGING);
	printf("  1. START - FOREVER\n");
	printf("  2. START - COUNT (100*COUNT)\n");
	printf("  3. STOP\n\n");
	printf(" 99. EXIT\n\n");
	printf("======================================================================\n\n");
}

void print_menu_channel(void)
{
	print_title(FM_CMD_MENU_CHANNEL);
	printf("  1. Set Channel\n");
	printf("  2. Get Channel [%ldKHz]\n\n", current_channel);
	printf("  3. Seek Up\n");
	printf("  4. Seek Down\n");
	printf("  5. Search All\n\n");
	printf("  6. Full Scan\n");
	printf("  7. Seek Cancel\n\n");
	printf(" 99. EXIT\n\n");
	printf("======================================================================\n\n");
}

void print_menu_mode(void)
{
	int value;

	print_title(FM_CMD_MENU_MODE);

	printf(" BAND (SPACING)   : ");
	if (current_band == 1)
		printf("European/U.S. [87.5MHz - 108MHz]");
	else if (current_band == 3)
		printf("Japanese [76MHz - 90MHz]");
	else
		printf("UNKNOWN");
	value = pFMRadio->GetChannelSpacing();
	if (value == FM_FAILURE)
		printf(" (0KHz)\n\n");
	else
		printf(" (%dKHz)\n\n", value*10);

	printf(" MUTE (SOFTMUTE)  : ");
	if (mute_onoff == 0)
		printf("OFF");
	else if (mute_onoff == 1)
		printf("ON");
	else
		printf("UNKNOWN");
	if (softmute_onoff == 0)
		printf(" (OFF)\n\n");
	else if (softmute_onoff == 1)
		printf(" (ON)\n\n");
	else
		printf(" (UNKNOWN)\n\n");

	printf(" MODE             : ");
	if (tuner_mode == 0)
		printf("MONO\n\n");
	else if (tuner_mode == 1)
		printf("STEREO\n\n");
	else
		printf("UNKNOWN\n\n");

	printf("----------------------------------------------------------------------\n\n");
	printf("  1. Set BAND\n");
	printf("  2. Set Channel Spacing\n");
	printf("  3. Get Channel Spacing\n\n");
	printf("  4. Set Stereo\n");
	printf("  5. Set Mono\n\n");
	printf("  6. Mute On\n");
	printf("  7. Mute Off\n");
	printf("  8. Set Softmute\n");
	printf("  9. Get Softmute\n");
	printf(" 10. Set Deconstant\n\n");
	printf(" 99. EXIT\n\n");
	printf("======================================================================\n\n");
}

void print_menu_RDS(void)
{
	print_title(FM_CMD_MENU_RDS);
	printf("  1. RDS On\n");
	printf("  2. RDS Off\n");
	printf("  3. DNS On\n");
	printf("  4. DNS Off\n");
	printf("  5. AF  On\n");
	printf("  6. AF  Off\n\n");
	printf("  7. Cancel AF Switching\n\n");
	printf("  8. Set AF Threshold\n");
	printf("  9. Get AF Threshold\n");
	printf(" 10. Set AF Valid Threshold\n");
	printf(" 11. Get AF Valid Threshold\n\n");
	printf(" 99. Test EXIT\n\n");
	printf("======================================================================\n\n");
}

void print_menu_status(void)
{
	print_title(FM_CMD_MENU_STATUS);
	printf("  1. Get Current RSSI\n");
	printf("  2. Get Current SNR\n\n");
	printf("  3. Get Soft Stereo Blend Coeff\n");
	printf("  4. Set Soft Stereo Blend Coeff\n");
	printf("  5. Get Soft Mute Coeff\n");
	printf("  6. Set Soft Mute Coeff\n\n");
	printf("  7. Set RSSI Threshold\n");
	printf("  8. Get RSSI Threshold\n\n");
	printf("  9. Set IF Count 1\n");
	printf(" 10. Get IF Count 1\n");
	printf(" 11. Set IF Count 2\n");
	printf(" 12. Get IF Count 2\n\n");
	printf(" 99. Test EXIT\n\n");
	printf("======================================================================\n\n");
}

void print_menu_main(void)
{
	print_title(FM_CMD_MENU);
	printf("  1. Initialise\n");
	printf("  2. Exit\n");
	printf("  3. Construct\n");
	printf("  4. Destruct\n\n");
	printf("  5. Sub test - Channel\n");
	printf("  6. Sub test - Mode\n");
	printf("  7. Sub test - RDS\n");
	printf("  8. Sub test - Status\n\n");
	printf(" 61. AgingTest - Scan\n");
	printf(" 62. Noise Scan\n");
	printf(" 63. Preset 89.1MHz\n\n");
	printf(" 99. Test EXIT\n\n");
	printf("======================================================================\n\n");
}

/*******************************************************************************
 *
 * SUB FUNCTIONS
 *
 ******************************************************************************/
int initialise(void)
{
	int ret;

	if (test_status == FM_RADIO_OFF) {
		printf(" NOT CREATED ... Please CONSTRUCT\n\n");
		return 0;
	} else if (test_status == FM_RADIO_ON) {
		printf(" ALREADY INITIALISED ...\n\n");
		return 0;
	}

	ret = pFMRadio->Initialise();
	if (ret) {
		printf(" INITIALISE error !!!\n\n");
		return -FM_CMD_MENU_INITIALISE;
	}

	test_status = FM_RADIO_ON;

	printf(" INITIALISED ... Start FM Radio TEST\n\n");
	return 0;
}

int construction(void)
{
	int input;

	if (pFMRadio) {
		printf("\n FM Radio HAL is already used, select next step.\n\n"); 
		printf("  1. Re-start\n");
		printf("  2. Exit\n\n");

		while (true) {
			printf(" Input your choice (1 or 2) : ");
			scanf("%d", &input);

			if (input == 2) {
				printf(" ALREADY USED !!!\n\n");
				return -FM_CMD_MENU_POWER_ON;
			}

			if (input != 1)
				continue;

			delete pFMRadio;
			pFMRadio = NULL;
			break;
		}
	}

	pFMRadio = new FmRadioController_slsi();
	if (!pFMRadio) {
		printf(" CREATE fail !!!\n\n");
		return -FM_CMD_MENU_POWER_ON;
	}

	return 0;
}

int destruction(void)
{
	if (!pFMRadio) {
		printf("\n FM Radio HAL was already removed\n\n");
		return 0;
	}

	delete pFMRadio;
	pFMRadio = NULL;

	test_status = FM_RADIO_OFF;

	printf(" STOP and REMOVED ...\n\n");

	return 0;
}

void set_channel(void)
{
	long channel;

	printf("\n Input Channel [input \"89100\" for 89.1MHz] : ");
	scanf("%ld", &channel);

	current_channel = channel;

	pFMRadio->TuneChannel(current_channel);

	printf("Channel: %.1fMHz\n\n", ((float)current_channel)/1000);
}

int get_channel(void)
{
	long channel;

	channel = pFMRadio->GetChannel();
	if (channel == FM_FAILURE) {
		printf("\n FAILED TO GET CHANNEL ...\n\n");
		return -(FM_CMD_CHANNEL_GET_CHANNEL+10);
	}

	current_channel = channel;
	printf("Channel: %.1fMHz\n\n", ((float)current_channel)/1000);

	return 0;
}

void sub_full_scan(void)
{
	long channel;

	stop_seek = false;
	if (current_band == 3)
		channel = 90000;
	else
		channel = 108000;

	pFMRadio->TuneChannel(channel);
	channel = pFMRadio->SeekUp();

	while (channel != FM_FAILURE) {
		current_channel = channel;
		printf("\n Channel: %.1fMHz", ((float)current_channel)/1000);
		if (stop_seek)
			return;
		channel = pFMRadio->SearchAll();
	}
	printf("\n");

	channel = pFMRadio->GetChannel();
	if (channel == FM_FAILURE) {
		printf("\n LAST GET CHANNEL FAIL ... please CHECK CURRENT CHANNEL\n\n");
		return;
	}
	current_channel = channel;
}

void *thread_seek_handler(void *arg)
{
	long channel;
	int testno;
//	int ret;

	if (arg == NULL) {
		printf("\n SEEK THREAD: Argument NULL !!!\n\n");
		goto thread_exit;
	}

	testno = *(int *)arg;
	if (testno < 3 || testno > 6) {
		printf("\n SEEK THREAD: Argument[%d] Error !!!\n\n", testno);
		goto thread_exit;
	}

	printf("\n SEEK TEST START ...\n");

	switch (testno) {
		case FM_CMD_CHANNEL_SEEK_UP:
			channel = pFMRadio->SeekUp();
			break;
		case FM_CMD_CHANNEL_SEEK_DOWN:
			channel = pFMRadio->SeekDown();
			break;
		case FM_CMD_CHANNEL_SEARCH_ALL:
			channel = pFMRadio->SearchAll();
			break;
		case FM_CMD_CHANNEL_FULL_SCAN:
			sub_full_scan();
			break;
		default:
			break;
	}

	switch (testno) {
		case FM_CMD_CHANNEL_SEEK_UP:
		case FM_CMD_CHANNEL_SEEK_DOWN:
		case FM_CMD_CHANNEL_SEARCH_ALL:
			if (channel == FM_FAILURE) {
				printf(" FAILED TO SEEK ...\n\n");
				goto thread_exit;
			}
			current_channel = channel;
			printf("\n Channel: %.1fMHz\n", ((float)current_channel)/1000);
			break;
		default:
			break;
	}

	printf(" SEEK TEST COMPLETION ...\n\n");

thread_exit:
	test_seek = false;
	pthread_exit(NULL);
	return NULL;
}

int seek_proc(int testcase)
{
	int testno = testcase;
	int ret;

	if (test_seek) {
		printf(" NOW SEEKING ...\n\n");
		return 0;
	}

	test_seek = true;
	ret = pthread_create(&thread_seek, NULL, thread_seek_handler, &testno);
	if (ret < 0) {
		test_seek = false;
		printf("\n SEEK: THREAD CREATE FAIL\n\n");
		return -(FM_CMD_CHANNEL_SEEK_UP+10);
	}

	return 0;
}

void seek_cancel(void)
{
	if (!test_seek) {
		printf(" NOW not SEEKING ...\n\n");
		return;
	}

	pFMRadio->SeekCancel();

	stop_seek = true;
}

void set_band(void)
{
	int band;

	printf("\n Band [1] : European/U.S. (87.5 - 108)MHz\n");
	printf(" Band [2] : Not supported (76   - 108)MHz\n");
	printf(" Band [3] : Japanese      (76   -  90)MHz\n");

	while (true) {
		printf(" Input Band : ");
		scanf("%d", &band);

		if (band == 1 || band == 3) {
			pFMRadio->SetBand(band);
			current_band = band;
			printf("\n BAND[%d] was SET ...\n\n", current_band);
			break;
		} else {
			printf(" NOT SUPPORTED ...\n\n");
		}
	}
}

void set_channel_spacing(void)
{
	int spacing;

	printf("\n Input Channel Spacing [5/10/20] (unit:10KHz) : ");
	scanf("%d", &spacing);

	pFMRadio->SetChannelSpacing(spacing);

	printf("\n CHANNEL SPACING [%dKHz] was SET ... \n", spacing*10);
}

int get_channel_spacing(void)
{
	int spacing;

	spacing = pFMRadio->GetChannelSpacing();
	if (spacing == FM_FAILURE) {
		printf("\n FAILED TO GET CHANNEL SPACING ...\n\n");
		return -(FM_CMD_MODE_GET_SPACING+20);
	}

	printf("\n CHANNEL SPACING : %dKHz\n\n", spacing*10);
	return 0;
}

void set_stereo(void)
{
	pFMRadio->SetStereo();
	tuner_mode = 1;

	printf("\n TUNER MODE STEREO was SET ...\n\n");
}

void set_mono(void)
{
	pFMRadio->SetMono();
	tuner_mode = 0;

	printf("\n TUNER MODE STEREO was MONO ...\n\n");
}

void mute_on(void)
{
	pFMRadio->MuteOn();
	mute_onoff = 1;

	printf("\n MUTE ON ...\n\n");
}

void mute_off(void)
{
	pFMRadio->MuteOff();
	mute_onoff = 0;

	printf("\n MUTE OFF ...\n\n");
}

void set_softmute(void)
{
	int mute;

	printf(" Input SoftMute (1:On / 0:Off) : ");
	scanf("%d", &mute);

	pFMRadio->setSoftmute((bool)mute);
	softmute_onoff = mute;

	printf("\n SOFT MUTE %s\n\n", mute ? "ON" : "OFF");
}

int get_softmute(void)
{
	int mute;

	mute = (int)pFMRadio->GetSoftMute();
	if (mute == FM_FAILURE) {
		printf("\n FAILED TO GET SOFT MUTE ...\n\n");
		return -(FM_CMD_MODE_GET_SOFTMUTE+20);
	}
	softmute_onoff = mute;

	printf("\n SOFT MUTE : %s\n\n", mute ? "ON" : "OFF");
	return 0;
}

void set_deconstant(void)
{
	int deconstant;

	printf("\n [0]:75us   [1]:50usec (Not Supported)   [2]:0us");
	printf("\n Input DeConstant : ");
	scanf("%d", &deconstant);

	pFMRadio->SetDeConstant((long)deconstant);

	printf("\n DECONSTAND[%d] was SET ...\n\n", deconstant);
}

void enable_RDS(void)
{
	pFMRadio->EnableRDS();
	printf("\n RDS was ENABLED ...\n\n");
}

void disable_RDS(void)
{
	pFMRadio->DisableRDS();
	printf("\n RDS was DISABLED ...\n\n");
}

void enable_DNS(void)
{
	pFMRadio->EnableDNS();
	printf("\n DNS was ENABLED ...\n\n");
}

void disable_DNS(void)
{
	pFMRadio->DisableDNS();
	printf("\n DNS was DISABLED ...\n\n");
}

void enable_AF(void)
{
	pFMRadio->EnableAF();
	printf("\n AF was ENABLED ...\n\n");
}

void disable_AF(void)
{
	pFMRadio->DisableAF();
	printf("\n AF was DISABLED ...\n\n");
}

void cancel_AF_switching(void)
{
	pFMRadio->CancelAfSwitchingProcess();
}

void set_AF_threshold(void)
{
	int value;

	printf("\n Input AF threshold : ");
	scanf("%d", &value);

	pFMRadio->SetAF_th(value);
	printf("\n AF THRESHOLD [%d] was SET ...\n\n", value);
}

void get_AF_threshold(void)
{
	int value;

	value = pFMRadio->GetAF_th();
	printf("\n AF THRESHOLD : %d\n\n", value);
}

void set_AF_valid_threshold(void)
{
	int value;

	printf("\n Input AF valid threshold : ");
	scanf(" %d", &value);

	pFMRadio->SetAFValid_th(value);
	printf("\n AF VALID THRESHOLD [%d] was SET ...\n\n", value);
}

void get_AF_valid_threshold(void)
{
	int value;

	value = pFMRadio->GetAFValid_th();
	printf("\n AF VALID THRESHOLD : %d\n\n", value);
}

int get_current_RSSI(void)
{
	long c_rssi;

	c_rssi = pFMRadio->GetCurrentRSSI();
	if (c_rssi == FM_FAILURE) {
		printf("\n FAILED TO GET CURRENT RSSI ...\n\n");
		return -(FM_CMD_STATUS_GET_CUR_RSSI+40);
	}

	printf("\n CURRENT RSSI : %d\n\n", (int)c_rssi);
	return 0;
}

int get_current_SNR(void)
{
	long c_snr;

	c_snr = pFMRadio->GetCurrentSNR();
	if (c_snr == FM_FAILURE) {
		printf("\n FAILED TO GET CURRENT SNR ...\n\n");
		return -(FM_CMD_STATUS_GET_CUR_SNR+40);
	}

	printf("\n CURRENT SNR : %ld\n\n", c_snr);
	return 0;
}

int get_soft_stereo_blend_coeff(void)
{
	long ssb_coeff;

	ssb_coeff = pFMRadio->GetSoftStereoBlendCoeff();
	if (ssb_coeff == FM_FAILURE) {
		printf("\n FAILED TO GET SOFT STEREO BLEND COEFF ...\n\n");
		return -(FM_CMD_STATUS_GET_SOFT_STEREO_BLEND_COEFF+40);
	}

	printf("\n SOFT STEREO BLEND COEFF : %ld\n\n", ssb_coeff);
	return 0;
}

void set_soft_stereo_blend_coeff(void)
{
	long ssb_coeff;

	printf(" Input Soft Stereo Blend Coeff : ");
	scanf("%ld", &ssb_coeff);

	pFMRadio->SetSoftStereoBlendCoeff(ssb_coeff);
	printf("\n SOFT STEREO BLEND COEFF [%ld] was SET ...\n\n", ssb_coeff);
}

int get_soft_mute_coeff(void)
{
	long mute_coeff;

	mute_coeff = pFMRadio->GetSoftMuteCoeff();
	if (mute_coeff == FM_FAILURE) {
		printf("\n FAILED TO GET SOFT MUTE COEFF ...\n\n");
		return -(FM_CMD_STATUS_GET_SOFT_MUTE_COEFF+40);
	}

	printf("\n SOFT MUTE COEFF : %ld\n\n", mute_coeff);
	return 0;
}

void set_soft_mute_coeff(void)
{
	long mute_coeff;

	printf(" Input Soft Mute Coeff : ");
	scanf("%ld", &mute_coeff);

	pFMRadio->SetSoftMuteCoeff(mute_coeff);
	printf("\n SOFT MITE COEFF [%ld] was SET ...\n\n", mute_coeff);
}

void set_RSSI_threshold(void)
{
	int threshold;

	printf(" Input RSSI Threshold : ");
	scanf("%d", &threshold);;

	pFMRadio->SetRSSI_th(threshold);
	printf("\n RSSI THRESHOLD [%d] was SET ...\n\n", threshold);
}

int get_RSSI_threshold(void)
{
	int threshold;

	threshold = pFMRadio->GetRSSI_th();
	if (threshold == FM_FAILURE) {
		printf("\n FAILED TO GET RSSI THRESHOLD ...\n\n");
		return -(FM_CMD_STATUS_GET_RSSI_TH+40);
	}

	printf("\n RSSI THRESHOLD : %d\n\n", threshold);
	return 0;
}

void set_IF_count1(void)
{
	int value;

	printf(" Input IF Count 1 : ");
	scanf(" %d", &value);

	pFMRadio->SetIFCount1(value);
	printf("\n IF COUNT 1 [%d] was SET ...\n\n", value);
}

int get_IF_count1(void)
{
	int value;

	value = pFMRadio->GetIFCount1();
	if (value == FM_FAILURE) {
		printf("\n FAILED TO GET IF COUNT 1 ...\n\n");
		return -(FM_CMD_STATUS_GET_IF_COUNT1+40);
	}

	printf("\n IF COUNT 1 : %d\n\n", value);
	return 0;
}

void set_IF_count2(void)
{
	int value;

	printf(" Input IF Count 2 : ");
	scanf(" %d", &value);

	pFMRadio->SetIFCount2(value);
	printf("\n IF COUNT 2 [%d] was SET ...\n\n", value);
}

int get_IF_count2(void)
{
	int value;

	value = pFMRadio->GetIFCount2();
	if (value == FM_FAILURE) {
		printf("\n FAILED TO GET IF COUNT 2 ...\n\n");
		return -(FM_CMD_STATUS_GET_IF_COUNT2+40);
	}

	printf("\n IF COUNT 2 : %d\n\n", value);
	return 0;
}

/*******************************************************************************
 *
 * NOT USED IN CURRENT
 *
 ******************************************************************************/
/*
void GetSeekMode()
{
	long seek_mode;

	printf("\n%s +++++++\n\n", __func__);

	seek_mode = pFMRadio->GetSeekMode();
	if (seek_mode == FM_FAILURE) {
		printf("%s failed to get seek mode\n", __func__);
		return;
	}

	printf("%s: seek mode: %ld\n", __func__, seek_mode);
	printf("%s: 0. STOP SEARCH MODE\n", __func__);
	printf("%s: 1. PRESET MODE\n", __func__);
	printf("%s: 2. AUTONOMOUS SEARCH MODE\n", __func__);
	printf("%s: 3. AF JUMP MODE\n", __func__);
	printf("%s: 4. AUTONOMOUS SEARCH MODE SKIP\n\n", __func__);
}

void SetSeekMode()
{
	long seek_mode;

	printf("\n%s: Seek Mode", __func__);
	printf("\n%s: 0. STOP SEARCH MODE", __func__);
	printf("\n%s: 1. PRESET MODE", __func__);
	printf("\n%s: 2. AUTONOMOUS SEARCH MODE", __func__);
	printf("\n%s: 3. AF JUMP MODE", __func__);
	printf("\n%s: 4. AUTONOMOUS SEARCH MODE SKIP", __func__);
	printf("\n%s: Select ...\n", __func__);
	scanf(" %ld", &seek_mode);

	pFMRadio->SetSeekMode(seek_mode);

	printf("\n%s -------\n\n", __func__);
}

void SetCurrentRSSI()
{
	long c_rssi;

	printf("\n%s: Input RSSI ...\n", __func__);
	scanf(" %ld", &c_rssi);

	pFMRadio->SetCurrentRSSI(c_rssi);

	printf("\n%s -------\n\n", __func__);
}
*/
/*******************************************************************************
 * Functions for RDS
 ******************************************************************************/
/*
void *rds_print_thread_handler(void *arg)
{
	int i, j;

	if (arg == NULL)
		printf("\n%s has not any arg\n", __func__);

	while (!thread_stop) {
		for (i = 0; i < TEST_DATA_SIZE; i++) {
			switch (print_mode) {
			case 1:
				printf("\n");
				for (j = 0; j < 12; j++) {
					printf("%02x ", test_read_data[i][j]);
				}
				break;

			case 2:
				printf("\n%04x %04x %04x %04x %02x %04x %04x %04x %04x",
					test_radio_data[i].rdsa, test_radio_data[i].rdsb,
					test_radio_data[i].rdsc, test_radio_data[i].rdsd,
					test_radio_data[i].curr_rssi,
					test_radio_data[i].blera, test_radio_data[i].blerb,
					test_radio_data[i].blerc, test_radio_data[i].blerd);
				break;

			case 3:
				printf("\n%04x %02x", test_piecc[i].PI, test_piecc[i].ECC);
				break;

			case 4:
				printf("\nRTPLUS");
				break;

			case 5:
				printf("\n%s,  %s,  %ld",
					final_rds_data[i].StationName, final_rds_data[i].RadioText,
					final_rds_data[i].Af_frequency);
				break;

			default:
				printf("\ndefault\n");
				break;
			}
		}
		printf("\n");
		usleep(1000000);
	}

	pthread_exit(NULL);
	return NULL;
}

void StartRDSData()
{
	int select;
	int ret;

	printf("\n%s: 1. read   data", __func__);
	printf("\n%s: 2. radio  data", __func__);
	printf("\n%s: 3. piecc  data", __func__);
	printf("\n%s: 4. rtplus data", __func__);
	printf("\n%s: 5. rds    data", __func__);
	printf("\n%s: Select data ...\n", __func__);
	scanf(" %d", &select);

	print_mode = select;
	thread_stop = false;
	ret = pthread_create(&rds_print_thread, NULL, rds_print_thread_handler, NULL);
	if (ret < 0) {
		printf("\nFailed to create thread\n\n");
		return;
	}

	printf("%s -------: thread creation success\n\n", __func__);
}

void StopRDSData()
{
	printf("\n%s +++++++\n\n", __func__);

	thread_stop = true;
	pthread_join(rds_print_thread, NULL);
	rds_print_thread = (pthread_t)NULL;

	printf("%s -------: thread termination success\n\n", __func__);
}
*/

void *thread_seekaging_handler(void *arg)
{
	unsigned long total_count, count;
	long channel;
	int input[2];
	int loop;
//	int i, j;

	if (arg == NULL) {
		printf("\n SEEKAGING THREAD: Argument NULL !!!\n\n");
		goto thread_exit;
	}

	input[0] = *(int *)arg;
	input[1] = *((int *)arg + 1);

	if (input[0] == 1) {
		loop = 1;
	} else if (input[0] == 2) {
		loop = 0;
		total_count = 100 * (unsigned long)input[1];
	} else {
		printf("\n SEEKAGING THREAD: Argument[%d] Error !!!\n\n", input[0]);
		goto thread_exit;
	}

	count = 0;
	while (true) {
		channel = pFMRadio->SeekUp();
		if (channel == FM_FAILURE) {
			printf("\n TEST FAIL ... Exit\n");
			goto thread_exit;
		}
		current_channel = channel;
		printf("\n%7ld: Found channel: %.1fMHz", ++count, ((float)current_channel)/1000);
		if (loop == 0 && count == total_count) {
			printf("\n TEST COMPLETE ...\n");
			print_sub_seekaging();
			break;
		}
		if (stop_seekaging)
			break;
	}

thread_exit:
	pthread_exit(NULL);
	return NULL;
}

int seekaging(void)
{
	int seekaging_arg[2];
	int input;
	int ret;

	if (test_status != FM_RADIO_ON) {
		printf("\n NOT INITIALISED ...\n");
		return 0;
	}

	print_sub_seekaging();

	test_seekaging = false;
	while (1) {
		printf(" Enter : ");
		scanf(" %d", &input);

		if (input == 1 || input == 2) {
			if (test_seekaging) {
				printf("\n ALREADY TESTED ...\n\n");
				continue;
			}
			test_seekaging = true;
			stop_seekaging = false;
			seekaging_arg[0] = input;
			if (input == 2) {
				while (true) {
					printf(" TEST COUNT (1-50000) : ");
					scanf("%d", &input);
					if (input > 0 && input <= 50000) {
						seekaging_arg[1] = input;
						break;
					}
					printf(" Wrong Input ... Please Input (1 - 50000)\n\n");
				}
			}
			ret = pthread_create(&thread_seekaging, NULL, thread_seekaging_handler, (void *)&seekaging_arg[0]);
			if (ret < 0) {
				printf("\n SEEKAGING: THREAD CREATE FAIL\n\n");
				return -FM_CMD_MENU_SEEKAGING;
			}
		} else if (input == 3) {
			if (test_seekaging) {
				stop_seekaging = true;
				pthread_join(thread_seekaging, NULL);
				thread_seekaging = (pthread_t)NULL;
				test_seekaging = false;
			}
			printf("\n SEEKAGING STOPPED ...\n\n");
			print_sub_seekaging();
		} else if (input == 99) {
			if (test_seekaging) {
				printf("\n NOW TESTING ... \n\n");
				continue;
			}
			break;
		}
	}

	return 0;
}

int noisescan(void)
{
	long channel, start_channel, end_channel;
	long c_rssi, c_snr;
	int spacing = pFMRadio->GetChannelSpacing() * 10;

	if (test_status != FM_RADIO_ON) {
		printf("\n NOT INITIALISED ...\n");
		return 0;
	}

	print_title(FM_CMD_MENU_NOISESCAN);
	if (current_band == 3)
		printf(" CURRENT BAND [3] : Japanese      [76   -  90]MHz [SPACING : %3dKHz]\n\n", spacing);
	else
		printf(" CURRENT BAND [1] : European/U.S. [87.5 - 108]MHz [SPACING : %3dKHz]\n\n", spacing);
	printf("======================================================================\n\n");

	if (current_band == 3) {
		start_channel = 76000;
		end_channel = 90000;
	} else {
		start_channel = 87500;
		end_channel = 108000;
	}

	for (channel = start_channel; channel <= end_channel; channel += spacing) {
		pFMRadio->TuneChannel(channel);
		c_rssi = pFMRadio->GetCurrentRSSI();
		if (c_rssi == FM_FAILURE) {
			printf("\n FAILED TO GET current RSSI ...\n");
			return -FM_CMD_MENU_NOISESCAN;
		}
		c_snr = pFMRadio->GetCurrentSNR();
		if (c_snr == FM_FAILURE) {
			printf("\n FAILED TO GET current SNR ...\n");
			return -FM_CMD_MENU_NOISESCAN;
		}
		printf(" Channel: %.1fMHz\tRSSI: %4d\tSNR: %3ld\n", ((float)channel)/1000, (int)c_rssi, c_snr);
		usleep(100000);
	}

	return 0;
}

int preset(void)
{
	int ret;

	ret = initialise();
	if (ret)
		return ret;

	current_channel = 89100;

	pFMRadio->TuneChannel(current_channel);

	printf(" Set Channel: %.1fMHz\n\n", ((float)current_channel)/1000);

	return 0;
}

int option(void)
{
	int input;
//	int ret;

	print_title(FM_CMD_MENU_OPTION);
	printf("  1. Menu Display [ON/OFF] : %s\n", print_option_display ? "ON" : "OFF");
	printf("  2. CLEAR SCREEN [ON/OFF] : %s\n\n", print_option_clear ? "ON" : "OFF");
	printf(" 99. EXIT\n\n");
	printf("======================================================================\n\n");

	while (true) {
		printf(" Enter : ");
		scanf("%d", &input);

		switch (input) {
			case 1:
				print_option_display = ~print_option_display;
				printf(" MENU DISPLAY %s ...\n\n", print_option_display ? "ON" : "OFF");
				break;

			case 2:
				print_option_clear = ~print_option_clear;
				printf(" CLEAR SCREEN %s ...\n\n", print_option_clear ? "ON" : "OFF");
				break;

			case FM_CMD_MENU_BACK:
				return 0;

			default:
				break;
		}
	}
}

/*******************************************************************************
 *
 * MENU FUNCTIONS
 *
 ******************************************************************************/
void init_variables(void)
{
//	long value_long;
//	int value_int;

	test_status = FM_RADIO_STOP;

	print_option_display = true;
	print_option_clear = false;
	current_band = 2;
	tuner_mode = 2;
	mute_onoff = 2;
	softmute_onoff = 2;
}

int execute_channel(void)
{
	int input;
	int ret;

	if (test_status != FM_RADIO_ON) {
		printf("\n NOT INITIALISED ...\n");
		return 0;
	}

	print_menu_channel();

	test_seek = false;
	while (true) {
		printf(" Enter : ");
		scanf("%d", &input);
		ret = 0;

		switch (input) {
			case FM_CMD_CHANNEL_SET_CHANNEL:
				set_channel();
				break;
			case FM_CMD_CHANNEL_GET_CHANNEL:
				ret = get_channel();
				break;

			case FM_CMD_CHANNEL_SEEK_UP:
				ret = seek_proc(FM_CMD_CHANNEL_SEEK_UP);
				break;
			case FM_CMD_CHANNEL_SEEK_DOWN:
				ret = seek_proc(FM_CMD_CHANNEL_SEEK_DOWN);
				break;
			case FM_CMD_CHANNEL_SEARCH_ALL:
				ret = seek_proc(FM_CMD_CHANNEL_SEARCH_ALL);
				break;

			case FM_CMD_CHANNEL_FULL_SCAN:
				ret = seek_proc(FM_CMD_CHANNEL_FULL_SCAN);
				break;
			case FM_CMD_CHANNEL_SEEK_CANCEL:
				seek_cancel();
				break;

			case FM_CMD_MENU_BACK:
				ret = 0;
				if (!test_seek)
					return ret;
				printf("\n NOW SEEKING ... \n\n");
				break;
			default:
				break;
		}

		if (ret)
			break;

		if (input == FM_CMD_CHANNEL_FULL_SCAN || print_option_display)
			print_menu_channel();
	}

	return ret;
}

int execute_mode(void)
{
	int input;
	int ret;

	if (test_status != FM_RADIO_ON) {
		printf("\n NOT INITIALISED ...\n");
		return 0;
	}

	print_menu_mode();

	while (true) {
		printf(" Enter : ");
		scanf("%d", &input);
		ret = 0;

		switch (input) {
			case FM_CMD_MODE_BAND:
				set_band();
				break;
			case FM_CMD_MODE_SET_SPACING:
				set_channel_spacing();
				break;
			case FM_CMD_MODE_GET_SPACING:
				ret = get_channel_spacing();
				break;
			case FM_CMD_MODE_STEREO:
				set_stereo();
				break;
			case FM_CMD_MODE_MONO:
				set_mono();
				break;
			case FM_CMD_MODE_MUTE_ON:
				mute_on();
				break;
			case FM_CMD_MODE_MUTE_OFF:
				mute_off();
				break;
			case FM_CMD_MODE_SET_SOFTMUTE:
				set_softmute();
				break;
			case FM_CMD_MODE_GET_SOFTMUTE:
				ret = get_softmute();
				break;
			case FM_CMD_MODE_DECONSTANT:
				set_deconstant();
				break;
			case FM_CMD_MENU_BACK:
				return 0;
			default:
				break;
		}

		if (ret)
			break;

		if (print_option_display)
			print_menu_mode();
	}

	return ret;
}

int execute_RDS(void)
{
	int input;

	if (test_status != FM_RADIO_ON) {
		printf("\n NOT INITIALISED ...\n");
		return 0;
	}

	print_menu_RDS();

	while (true) {
		printf(" Enter : ");
		scanf("%d", &input);

		switch (input) {
			case FM_CMD_RDS_RDS_ENABLE:
				enable_RDS();
				break;
			case FM_CMD_RDS_RDS_DISABLE:
				disable_RDS();
				break;
			case FM_CMD_RDS_DNS_ENABLE:
				enable_DNS();
				break;
			case FM_CMD_RDS_DNS_DISABLE:
				disable_DNS();
				break;
			case FM_CMD_RDS_AF_ENABLE:
				enable_AF();
				break;
			case FM_CMD_RDS_AF_DISABLE:
				disable_AF();
				break;
			case FM_CMD_RDS_CANCEL_AF_SWITCHING:
				cancel_AF_switching();
				break;
			case FM_CMD_RDS_SET_AFTH:
				set_AF_threshold();
				break;
			case FM_CMD_RDS_GET_AFTH:
				get_AF_threshold();
				break;
			case FM_CMD_RDS_SET_AFVALIDTH:
				set_AF_valid_threshold();
				break;
			case FM_CMD_RDS_GET_AFVALIDTH:
				get_AF_valid_threshold();
				break;
			case FM_CMD_MENU_BACK:
				return 0;
			default:
				break;
		}

		if (print_option_display)
			print_menu_RDS();
	}

	return 0;
}

int execute_status(void)
{
	int input;
	int ret;

	if (test_status != FM_RADIO_ON) {
		printf("\n NOT INITIALISED ...\n");
		return 0;
	}

	print_menu_status();

	while (true) {
		printf(" Enter : ");
		scanf("%d", &input);
		ret = 0;

		switch (input) {
			case FM_CMD_STATUS_GET_CUR_RSSI:
				ret = get_current_RSSI();
				break;
			case FM_CMD_STATUS_GET_CUR_SNR:
				ret = get_current_SNR();
				break;
			case FM_CMD_STATUS_GET_SOFT_STEREO_BLEND_COEFF:
				ret = get_soft_stereo_blend_coeff();
				break;
			case FM_CMD_STATUS_SET_SOFT_STEREO_BLEND_COEFF:
				set_soft_stereo_blend_coeff();
				break;
			case FM_CMD_STATUS_GET_SOFT_MUTE_COEFF:
				ret = get_soft_mute_coeff();
				break;
			case FM_CMD_STATUS_SET_SOFT_MUTE_COEFF:
				set_soft_mute_coeff();
				break;
			case FM_CMD_STATUS_SET_RSSI_TH:
				set_RSSI_threshold();
				break;
			case FM_CMD_STATUS_GET_RSSI_TH:
				ret = get_RSSI_threshold();
				break;
			case FM_CMD_STATUS_SET_IF_COUNT1:
				set_IF_count1();
				break;
			case FM_CMD_STATUS_GET_IF_COUNT1:
				ret = get_IF_count1();
				break;
			case FM_CMD_STATUS_SET_IF_COUNT2:
				set_IF_count2();
				break;
			case FM_CMD_STATUS_GET_IF_COUNT2:
				ret = get_IF_count2();
				break;
			case FM_CMD_MENU_BACK:
				return 0;
			default:
				break;
		}

		if (ret)
			break;

		if (print_option_display)
			print_menu_status();
	}

	return ret;
}

int execute_function(void)
{
	int input;
	int ret;

	print_menu_main();

	while (true) {
		printf(" Enter : ");
		scanf("%d", &input);

		switch (input) {
			case FM_CMD_MENU_INITIALISE:
				ret = initialise();
				break;

			case FM_CMD_MENU_POWER_ON:
				ret = construction();
				if (!ret) {
					printf(" CREATED ... please INITIALISE\n\n");
					init_variables();
				}
				break;

			case FM_CMD_MENU_POWER_OFF:
				ret = destruction();
				break;

			case FM_CMD_MENU_SEEKAGING:
				ret = seekaging();
				break;

			case FM_CMD_MENU_NOISESCAN:
				ret = noisescan();
				break;

			case FM_CMD_MENU_PRESET:
				ret = preset();
				break;

			case FM_CMD_MENU_CHANNEL:
				ret = execute_channel();
				break;

			case FM_CMD_MENU_MODE:
				ret = execute_mode();
				break;

			case FM_CMD_MENU_RDS:
				ret = execute_RDS();
				break;

			case FM_CMD_MENU_STATUS:
				ret = execute_status();
				break;

			case FM_CMD_MENU_OPTION:
				ret = option();
				break;

			case FM_CMD_MENU_EXIT:
			case FM_CMD_MENU_BACK:
				ret = destruction();
				return ret;

			default:
				break;
		}

		if (ret)
			break;

		switch (input) {
			case FM_CMD_MENU_SEEKAGING:
			case FM_CMD_MENU_NOISESCAN:
			case FM_CMD_MENU_PRESET:
			case FM_CMD_MENU_CHANNEL:
			case FM_CMD_MENU_MODE:
			case FM_CMD_MENU_RDS:
			case FM_CMD_MENU_STATUS:
			case FM_CMD_MENU_OPTION:
				if (print_option_display == false)
					print_menu_main();
				break;
			default:
				break;
		}

		if (print_option_display)
			print_menu_main();
	}

	return ret;
}

/*******************************************************************************
 *
 * MAIN FUNCTION
 *
 ******************************************************************************/
int main(void)
{
	int ret;

	ret = construction();
	if (ret)
		return ret;

	init_variables();

	return execute_function();
}
