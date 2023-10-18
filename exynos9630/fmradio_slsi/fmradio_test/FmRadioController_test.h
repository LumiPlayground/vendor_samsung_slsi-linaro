#ifndef __FM_HAL_APP_H__
#define __FM_HAL_APP_H__

#include <pthread.h>

/*******************************************************************************
 *
 * COMMAND CODE
 *
 ******************************************************************************/
enum fm_cmd_menu {
	FM_CMD_MENU = 0,

	FM_CMD_MENU_INITIALISE = 1,
	FM_CMD_MENU_EXIT,
	FM_CMD_MENU_POWER_ON,
	FM_CMD_MENU_POWER_OFF,

	FM_CMD_MENU_CHANNEL,
	FM_CMD_MENU_MODE,
	FM_CMD_MENU_RDS,
	FM_CMD_MENU_STATUS,

	FM_CMD_MENU_SEEKAGING = 61,
	FM_CMD_MENU_NOISESCAN,
	FM_CMD_MENU_PRESET,

	FM_CMD_MENU_OPTION = 98,
	FM_CMD_MENU_BACK,
};

enum fm_cmd_channel {
	FM_CMD_CHANNEL_SET_CHANNEL = 1,
	FM_CMD_CHANNEL_GET_CHANNEL,
	FM_CMD_CHANNEL_SEEK_UP,
	FM_CMD_CHANNEL_SEEK_DOWN,
	FM_CMD_CHANNEL_SEARCH_ALL,
	FM_CMD_CHANNEL_FULL_SCAN,
	FM_CMD_CHANNEL_SEEK_CANCEL,
};

enum fm_cmd_mode {
	FM_CMD_MODE_BAND = 1,
	FM_CMD_MODE_SET_SPACING,
	FM_CMD_MODE_GET_SPACING,
	FM_CMD_MODE_STEREO,
	FM_CMD_MODE_MONO,
	FM_CMD_MODE_MUTE_ON,
	FM_CMD_MODE_MUTE_OFF,
	FM_CMD_MODE_SET_SOFTMUTE,
	FM_CMD_MODE_GET_SOFTMUTE,
	FM_CMD_MODE_DECONSTANT,
};

enum fm_cmd_rds {
	FM_CMD_RDS_RDS_ENABLE = 1,
	FM_CMD_RDS_RDS_DISABLE,
	FM_CMD_RDS_DNS_ENABLE,
	FM_CMD_RDS_DNS_DISABLE,
	FM_CMD_RDS_AF_ENABLE,
	FM_CMD_RDS_AF_DISABLE,
	FM_CMD_RDS_CANCEL_AF_SWITCHING,
	FM_CMD_RDS_SET_AFTH,
	FM_CMD_RDS_GET_AFTH,
	FM_CMD_RDS_SET_AFVALIDTH,
	FM_CMD_RDS_GET_AFVALIDTH,
};

enum fm_cmd_status {
	FM_CMD_STATUS_GET_CUR_RSSI = 1,
	FM_CMD_STATUS_GET_CUR_SNR,
	FM_CMD_STATUS_GET_SOFT_STEREO_BLEND_COEFF,
	FM_CMD_STATUS_SET_SOFT_STEREO_BLEND_COEFF,
	FM_CMD_STATUS_GET_SOFT_MUTE_COEFF,
	FM_CMD_STATUS_SET_SOFT_MUTE_COEFF,
	FM_CMD_STATUS_SET_RSSI_TH,
	FM_CMD_STATUS_GET_RSSI_TH,
	FM_CMD_STATUS_SET_IF_COUNT1,
	FM_CMD_STATUS_GET_IF_COUNT1,
	FM_CMD_STATUS_SET_IF_COUNT2,
	FM_CMD_STATUS_GET_IF_COUNT2,
};

/*******************************************************************************
 *
 * Decalarations for RDS, DNS, and AF
 *
 ******************************************************************************/
typedef struct
{
	unsigned short rdsa;
	unsigned short rdsb;
	unsigned short rdsc;
	unsigned short rdsd;
	unsigned char  curr_rssi;
	unsigned int curr_channel;
	unsigned char blera;
	unsigned char blerb;
	unsigned char blerc;
	unsigned char blerd;
}radio_data_t;

struct PIECC_data
{
	unsigned short PI;
	unsigned char ECC;
};

#define PS_MAXIMUM_SIZE 8
#define RT_MAXIMUM_SIZE 64

struct Final_RDS_data
{
	char StationName[PS_MAXIMUM_SIZE+1], RadioText[RT_MAXIMUM_SIZE+1];
	long Af_frequency;
};

#define RTPLUS_TAG_MAXIMUM_SIZE 2

struct RTPlusTagInfo
{
	int contentType;
	int startPos;
	int additionalLen;
};

struct RTPlus_data
{
	bool bToggle;
	bool bRunning;
	unsigned char bValidated;
	RTPlusTagInfo RTPlusTag[RTPLUS_TAG_MAXIMUM_SIZE];
};

#define MAX_AF_NUM 25

typedef struct
{
	unsigned char curr_rssi;
	unsigned char curr_rssi_th;
	unsigned char curr_snr;
}rssi_snr_t;

typedef struct
{
	unsigned char AF[MAX_AF_NUM];
	unsigned char RSSI[MAX_AF_NUM];
	unsigned char AFCount;
}AF_List;

typedef struct
{
	char Text[RT_MAXIMUM_SIZE+1];
	unsigned char bChangeFlag;
	unsigned char bValidated;
	unsigned char iBytesReceived;
	unsigned char iLenght;
}RadioText;

typedef struct
{
	char Text[PS_MAXIMUM_SIZE+1];
	unsigned char iBytesReceived;
	unsigned char bValidated;
	unsigned char iLenght;
}ServiceName;

/*******************************************************************************
 *
 * Global variables for test
 *
 ******************************************************************************/
#define TEST_DATA_SIZE 10

extern unsigned char test_read_data[TEST_DATA_SIZE][12];
extern radio_data_t test_radio_data[TEST_DATA_SIZE];
extern struct PIECC_data test_piecc[TEST_DATA_SIZE];
extern struct RTPlus_data test_rtplus[TEST_DATA_SIZE];
extern struct Final_RDS_data final_rds_data[10];

extern RadioText RT_Final;
extern RadioText RT_Buffered;
extern ServiceName PS_Final;
extern ServiceName PS_Buffered;

extern unsigned char rt_flag;
extern unsigned char ps_flag;

extern unsigned int test_data_index;
extern unsigned int test_data_index_rtplus;
extern unsigned int test_data_index_rds;

#endif /* __FM_HAL_APP_H__ */
