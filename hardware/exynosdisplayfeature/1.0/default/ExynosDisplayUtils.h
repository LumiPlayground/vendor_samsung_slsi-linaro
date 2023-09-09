#ifndef VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYUTILS_H
#define VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYUTILS_H

#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <math.h>
#include <sstream>
#include <iterator>
#include "ExynosDisplayResource.h"

using std::string;
using std::vector;

#define ARRSIZE(arr)	(sizeof(arr) / sizeof(arr[0]))

int setAps(const char* stream);
int setCgc(const char* stream);
int setCgc17_idx(const char* stream);
int setCgc17_enc(const char* stream);
int setCgc17_dec(const char* stream);
int setCgc17_con(const char* stream);
int setGamma(const char *stream);
int setHsc48_idx(const char *stream);
int setHsc48_lcg(const char *stream);
int setHsc(const char *stream);
int setDegamma(const char *stream);
int setGammaMat(const char *stream);
int setScl(const char *stream);
int setHDR(int id, struct IntToConstString *hdrIdToPathList, int listSize, const char* stream);
int existXML(const char *docname);
int parserXML(const char *docname, vector<string>& stream, const char *mode, const char *item);
int parserFactoryXMLText(const char *docname, vector<string>& stream, const char *mode, const char *item, int att0, int att1);
void printVector(vector<string> stream);

/* Back light */
int set_backlight(int bl_level);

/* PNG format file display */
int read_png_file(char *filename, struct png_info_ *output);
int print_png_file(struct png_info_ *input);
struct png_info_ {
	int width;
	int height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers = NULL;
};

/* PPM format file display */
typedef unsigned char	ppm_byte;
typedef ppm_byte*	ppm_bytep;
struct ppm_info_ {
	int width;
	int height;
	int max;
	int byte_per_pixel;
	ppm_bytep raw_data;
};
int read_ppm_file(char *filename, struct ppm_info_ *output);
int print_ppm_file(struct ppm_info_ *input);
int print_raw_data_RGBA10(int pattern, int RGBA);
int print_raw_data_RGBA8(int pattern, int RGBA);

/* Blank/Unblank function */
int set_blank(void);
int set_unblank(void);

/* string function */
template<typename Out>
void split(const string &s, char delim, Out result);
vector<string> split(const string &s, const char delim);
#endif  // VENDOR_SAMSUNG_SLSI_HARDWARE_EXYNOSDISPLAYFEATURE_V1_0_EXYNOSDISPLAYUTILS_H
