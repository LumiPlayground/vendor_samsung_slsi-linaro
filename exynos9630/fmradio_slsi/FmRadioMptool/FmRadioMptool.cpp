#include <cstring>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>

#include "FmRadioController_slsi.h"
#include "FmRadioMptool.h"

/*******************************************************************************
 *
 * Global Variables
 *
 ******************************************************************************/
static FmRadioController_slsi * pFMRadio = NULL;

static long current_freq;

/*******************************************************************************
 *
 * MP tool api
 *
 ******************************************************************************/
 
int PowerOn(void)
{
	int ret;
	
	pFMRadio = new FmRadioController_slsi();
	if (!pFMRadio) {
		printf("\n CREATE fail !!!\n\n");
		return -1;
	}

	ret = pFMRadio->Initialise();
	if (ret) {
		printf(" INITIALISE error !!!\n\n");
		return -1;
	}

	printf("\n INITIALISED ... Start FM Radio TEST\n\n");
	
	return 0;
}

int PowerOff(void)
{
	if (!pFMRadio) {
		printf("\n FM Radio HAL was already removed\n\n");
		return 0;
	}

	delete pFMRadio;
	pFMRadio = NULL;

	printf(" STOP and REMOVED ...\n\n");
	
	return 0;
}

int SetTune(long freqkhz)
{
	printf("\n Set frequency %.1fMHz\n\n", (float)freqkhz / 1000);

	pFMRadio->TuneChannel(freqkhz);

	if(freqkhz == pFMRadio->GetChannel()) {
		current_freq = freqkhz;
		return 0;
	}
	else
		return -1;
}

int GetRSSI(void)
{
	long c_rssi;

	c_rssi = pFMRadio->GetCurrentRSSI();
	if (c_rssi == FM_FAILURE) {
		printf("\n FAILED TO GET CURRENT RSSI ...\n\n");
		return -1;
	}

	printf("\n CURRENT RSSI : %d\n\n", (int)c_rssi);
	return (int)c_rssi;
}