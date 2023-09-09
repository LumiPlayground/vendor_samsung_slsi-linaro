#define LOG_TAG "ExynosDisplayUtils-Hal"
#include "ExynosDisplayUtils.h"
#include <utils/Log.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

using std::string;
using std::vector;
using std::stringstream;

#define SYSFS_APS_PATH "/sys/class/dqe/dqe/aps"
#define SYSFS_CGC_PATH "/sys/class/dqe/dqe/cgc"
#define SYSFS_GAMMA_PATH "/sys/class/dqe/dqe/gamma"
#define SYSFS_HSC_PATH "/sys/class/dqe/dqe/hsc"
#define SYSFS_BACKLIGHT_0 "/sys/class/panel/panel/brightness"
#define SYSFS_BACKLIGHT_1 "/devices/virtual/backlight/panel_0/brightness"
#define SYSFS_BACKLIGHT_2 "/sys/class/backlight/panel_0/brightness"

#define SYSFS_CGC17_IDX_PATH "/sys/class/dqe/dqe/cgc17_idx"
#define SYSFS_CGC17_ENC_PATH "/sys/class/dqe/dqe/cgc17_enc"
#define SYSFS_CGC17_DEC_PATH "/sys/class/dqe/dqe/cgc17_dec"
#define SYSFS_CGC17_CON_PATH "/sys/class/dqe/dqe/cgc17_con"

#define SYSFS_HSC48_IDX_PATH "/sys/class/dqe/dqe/hsc48_idx"
#define SYSFS_HSC48_LCG_PATH "/sys/class/dqe/dqe/hsc48_lcg"

#define SYSFS_DEGAMMA_PATH "/sys/class/dqe/dqe/degamma"
#define SYSFS_GAMMA_MAT_PATH "/sys/class/dqe/dqe/gamma_matrix"
#define SYSFS_SCL_PATH "/sys/class/dqe/dqe/scl"

void printVector(vector<string> stream)
{
	for (vector<int>::size_type  i = 0; i < stream.size(); i++) {
		ALOGD("[%2d]: %s\n", (int)i, stream[i].c_str());
	}
}

int setAps(const char* stream)
{
    FILE* fp;

    /*ALOGD("setAps() %s\n", stream);*/

    fp = fopen(SYSFS_APS_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_APS_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("aps write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setCgc(const char* stream)
{
    FILE* fp;

    /*ALOGD("setCgc() %s\n", stream);*/

    fp = fopen(SYSFS_CGC_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_CGC_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("cgc write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setCgc17_idx(const char* stream)
{
    FILE* fp;

    /*ALOGD("setCgc17_idx() %s\n", stream);*/

    fp = fopen(SYSFS_CGC17_IDX_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_CGC17_IDX_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("cgc17_idx write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setCgc17_enc(const char* stream)
{
    FILE* fp;

    /*ALOGD("setCgc17_enc() %s\n", stream);*/

    fp = fopen(SYSFS_CGC17_ENC_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_CGC17_ENC_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("cgc17_enc write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setCgc17_dec(const char* stream)
{
    FILE* fp;

    /*ALOGD("setCgc17_dec() %s\n", stream);*/

    fp = fopen(SYSFS_CGC17_DEC_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_CGC17_DEC_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("cgc17_dec write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setCgc17_con(const char* stream)
{
    FILE* fp;

    /*ALOGD("setCgc17_con() %s\n", stream);*/

    fp = fopen(SYSFS_CGC17_CON_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_CGC17_CON_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("cgc17_con write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setGamma(const char *stream)
{
    FILE* fp;

    /*ALOGD("setGamma() %s\n", stream);*/

    fp = fopen(SYSFS_GAMMA_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_GAMMA_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("gamma write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setHsc48_idx(const char *stream)
{
    FILE* fp;

    /*ALOGD("setHsc48_idx() %s\n", stream);*/

    fp = fopen(SYSFS_HSC48_IDX_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_HSC48_IDX_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("hsc48_idx write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setHsc48_lcg(const char *stream)
{
    FILE* fp;

    /*ALOGD("setHsc48_lcg() %s\n", stream);*/

    fp = fopen(SYSFS_HSC48_LCG_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_HSC48_LCG_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("hsc48_lcg write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setHsc(const char *stream)
{
    FILE* fp;

    /*ALOGD("setHsc() %s\n", stream);*/

    fp = fopen(SYSFS_HSC_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_HSC_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("hsc write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setDegamma(const char *stream)
{
    FILE* fp;

    /*ALOGD("setDegamma() %s\n", stream);*/

    fp = fopen(SYSFS_DEGAMMA_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_DEGAMMA_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("degamma write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setGammaMat(const char *stream)
{
    FILE* fp;

    /*ALOGD("setGammaMat() %s\n", stream);*/

    fp = fopen(SYSFS_GAMMA_MAT_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_GAMMA_MAT_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("gamma mat write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int setScl(const char *stream)
{
    FILE* fp;

    /*ALOGD("setScl() %s\n", stream);*/

    fp = fopen(SYSFS_SCL_PATH, "w");
    if (fp == NULL) {
        ALOGE("open file error: %s", SYSFS_SCL_PATH);
        return -1;
    }

    if (fputs(stream, fp) < 0) {
        ALOGE("scl write error");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int existXML(const char *docname) {
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(docname);
    if (doc == NULL) {
        ALOGE("can not parse document: %s\n", docname);
        return -1;
    }

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        ALOGE("empty document: %s\n", docname);
        xmlFreeDoc(doc);
        return -1;
    }

    xmlFreeDoc(doc);
    return 0;
}

int parserXML(const char *docname, vector<string>& stream, const char *mode, const char *item) {
    xmlDocPtr doc;
    xmlNodePtr cur, node;
    xmlChar *key;

    /*ALOGD("xml_path = %s, mode = %s, item = %s\n", docname, mode, item);*/

    doc = xmlParseFile(docname);
    if (doc == NULL) {
        ALOGE("can not parse document: %s\n", docname);
        return -1;
    }

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        ALOGE("empty document: %s\n", docname);
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *)"Calib_Data")) {
        ALOGE("document of the wrong type, root node != Calib_Data\n");
        xmlFreeDoc(doc);
        return -1;
    }

    cur = cur->xmlChildrenNode;

    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"mode"))) {

            xmlChar *mode_name;

            mode_name = xmlGetProp(cur, (const xmlChar *)"name");

            if ((!xmlStrcmp(mode_name, (const xmlChar *)mode))) {

                node = cur->xmlChildrenNode;

                if ((!xmlStrcmp((const xmlChar *)item, (const xmlChar *)"dqe")))
                {
                    while (node != NULL) {
                        if ((!xmlStrcmp(node->name, (const xmlChar *)"aps"))) {
                            key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                            /*ALOGD("%s dqe cgc: %s\n", mode_name, key);*/
                            stream.push_back((char *)key);
                            xmlFree(key);
                        }

                        if ((!xmlStrcmp(node->name, (const xmlChar *)"cgc"))) {
                            key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                            /*ALOGD("%s dqe cgc: %s\n", mode_name, key);*/
                            stream.push_back((char *)key);
                            xmlFree(key);
                        }

                        if ((!xmlStrcmp(node->name, (const xmlChar *)"gamma"))) {
                            key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                            /*ALOGD("%s dqe gamma: %s\n", mode_name, key);*/
                            stream.push_back((char *)key);
                            xmlFree(key);
                        }

                        if ((!xmlStrcmp(node->name, (const xmlChar *)"hsc"))) {
                            key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                            /*ALOGD("%s dqe hsc: %s\n", mode_name, key);*/
                            stream.push_back((char *)key);
                            xmlFree(key);
                        }
                        node = node->next;
                    }
                }
                else
                {
                    while (node != NULL) {
                        if ((!xmlStrcmp(node->name, (const xmlChar *)item))) {
                            key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                            /*ALOGD("%s %s: %s\n", mode_name, item, key);*/
                            stream.push_back((char *)key);
                            xmlFree(key);
                        }
                        node = node->next;
                    }
                }
            }
            xmlFree(mode_name);
        }
        cur = cur->next;
    }
    xmlFreeDoc(doc);

    if ((int)stream.size() == 0) {
        ALOGE("no data document\n");
        return -1;
    }

    return 0;
}

int parserFactoryXMLText(const char *docname, vector<string>& stream, const char *mode, const char *item, int att0, int att1) {
    xmlDocPtr doc;
    xmlNodePtr cur, node;
    xmlChar *key;

    /*ALOGD("xml_path = %s, mode = %s, item = %s, att0 = %d, att1 = %d\n", docname, mode, item, att0, att1);*/

    doc = xmlParseFile(docname);
    if (doc == NULL) {
        ALOGE("can not parse document: %s\n", docname);
        return -1;
    }

    cur = xmlDocGetRootElement(doc);
    if (cur == NULL) {
        ALOGE("empty document: %s\n", docname);
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *)"Calib_Data")) {
        ALOGE("document of the wrong type, root node != Calib_Data\n");
        xmlFreeDoc(doc);
        return -1;
    }

    cur = cur->xmlChildrenNode;

    while (cur != NULL) {

        if ((!xmlStrcmp(cur->name, (const xmlChar *)"mode"))) {

            xmlChar *mode_name = xmlGetProp(cur, (const xmlChar *)"name");

            if ((!xmlStrcmp(mode_name, (const xmlChar *)mode))) {

                node = cur->xmlChildrenNode;

                while (node != NULL) {

                    if ((!xmlStrcmp((const xmlChar *)item, (const xmlChar *)"cgc17_enc")))
                    {

                        if ((!xmlStrcmp(node->name, (const xmlChar *)"cgc17_enc"))) {
                            xmlChar *attr0Value = xmlGetProp(node, (const xmlChar *)"att0");
                            xmlChar *attr1Value = xmlGetProp(node, (const xmlChar *)"att1");
                            int attr0Num = std::atoi((char *)attr0Value);
                            int attr1Num = std::atoi((char *)attr1Value);

                            if ((attr0Num == att0) && (attr1Num == att1)) {
                                key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                                /*ALOGD("%s %s %d %d: %s\n", mode_name, item, attr0Num, attr1Num, key);*/
                                stream.push_back((char *)key);
                                xmlFree(key);
                            }
                            xmlFree(attr0Value);
                            xmlFree(attr1Value);
                        }

                    }
                    else if ((!xmlStrcmp((const xmlChar *)item, (const xmlChar *)"hsc48_lcg")))
                    {

                        if ((!xmlStrcmp(node->name, (const xmlChar *)"hsc48_lcg"))) {
                            xmlChar *attr0Value = xmlGetProp(node, (const xmlChar *)"att0");
                            int attr0Num = std::atoi((char *)attr0Value);

                            if (attr0Num == att0) {
                                key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                                /*ALOGD("%s %s %d %s\n", mode_name, item, attr0Num, key);*/
                                stream.push_back((char *)key);
                                xmlFree(key);
                            }
                            xmlFree(attr0Value);
                        }

                    } else {

                            if ((!xmlStrcmp(node->name, (const xmlChar *)item))) {
                                key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
                                /*ALOGD("%s %s: %s\n", mode_name, item, key);*/
                                stream.push_back((char *)key);
                                xmlFree(key);
                            }
                    }

                  node = node->next;
                }

            }
            xmlFree(mode_name);
        }

        cur = cur->next;
    }
    xmlFreeDoc(doc);

    if ((int)stream.size() == 0) {
        ALOGE("no data document\n");
        return -1;
    }

    return 0;
}

int g_fd;

int read_png_file(char *filename, struct png_info_ *output) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		ALOGD("file(%s) open fail\n", filename);
		return -1;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) abort();

	png_infop info = png_create_info_struct(png);
	if(!info) abort();

	if(setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_read_info(png, info);

	output->width      = png_get_image_width(png, info);
	output->height     = png_get_image_height(png, info);
	output->color_type = png_get_color_type(png, info);
	output->bit_depth  = png_get_bit_depth(png, info);

	// Read any output->color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(output->bit_depth == 16)
		png_set_strip_16(png);

	if(output->color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(output->color_type == PNG_COLOR_TYPE_GRAY && output->bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These output->color_type don't have an alpha channel then fill it with 0xff.
	if(output->color_type == PNG_COLOR_TYPE_RGB ||
	output->color_type == PNG_COLOR_TYPE_GRAY ||
	output->color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if(output->color_type == PNG_COLOR_TYPE_GRAY ||
	output->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	if (output->row_pointers) abort();

	output->row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * output->height);
	for(int y = 0; y < output->height; y++) {
		output->row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
	}

	png_read_image(png, output->row_pointers);

	fclose(fp);

	png_destroy_read_struct(&png, &info, NULL);

	return 0;
}

static inline int get_hdr_win_config_png(struct decon_win_config *config,
					int(*buffer)[NUM_COLOR],
					int lcd_width, int lcd_height,
					int img_width, int img_height)
{
	config[DEDICATED_WIN].state = decon_win_config::DECON_WIN_STATE_BUFFER;
	config[DEDICATED_WIN].acq_fence = -1;
	config[DEDICATED_WIN].plane_alpha = 255;
	config[DEDICATED_WIN].blending = DECON_BLENDING_NONE;
	config[DEDICATED_WIN].format = DECON_PIXEL_FORMAT_RGBA_8888;
	config[DEDICATED_WIN].dpp_parm.eq_mode = CSC_BT_709;
	config[DEDICATED_WIN].dpp_parm.rot = DPP_ROT_NORMAL;
	config[DEDICATED_WIN].dpp_parm.hdr_std = DPP_HDR_OFF;

	config[DEDICATED_WIN].src.x = 0;
	config[DEDICATED_WIN].src.y = 0;
	config[DEDICATED_WIN].src.w = img_width;
	config[DEDICATED_WIN].src.h = img_height;
	config[DEDICATED_WIN].src.f_w = img_width;
	config[DEDICATED_WIN].src.f_h = img_height;
	config[DEDICATED_WIN].dst.x = 0;
	config[DEDICATED_WIN].dst.y = 0;
	config[DEDICATED_WIN].dst.w = img_width;
	config[DEDICATED_WIN].dst.h = img_height;
	config[DEDICATED_WIN].dst.f_w = lcd_width;
	config[DEDICATED_WIN].dst.f_h = lcd_height;

	config[DEDICATED_WIN].fd_idma[0] = buffer[DEDICATED_WIN][0];
	config[DEDICATED_WIN].idma_type = IDMA_VGS0;

	return 0;
}

int print_png_file(struct png_info_ *input) {
	struct decon_win_config_data win_data;
	struct decon_win_config *config = win_data.config;
	struct fb_var_screeninfo info;
	int client;
	int buffer[MAX_DECON_WIN][NUM_COLOR];

	char *buf_addr[MAX_DECON_WIN][NUM_COLOR];
	unsigned int *addr[MAX_DECON_WIN][NUM_COLOR];

	int i, j, k, ret;

	int img_width;
	int img_height;
	int img_size;
	int lcd_width, lcd_height;

	/* Device Node open & Ion Client Create */
	if (g_fd <= 0) {
		ALOGE("g_fd not open yet\n");
		ret = -1;
		goto err_g_fd;
	}

	client = exynos_ion_open();

	/* Getting Screen Info from the Machine */
	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &info);
	if (ret < 0) {
		ALOGE("failed to FBIOGET_VSCREENINFO\n");
		goto err_ion_open;
	}
	ALOGD("= Window Control =============================\n");
	ALOGD("FBIOGET_VSCREENINFO : xres = %d , yres = %d\n",
					info.xres, info.yres);
	lcd_width = info.xres;
	lcd_height = info.yres;
	img_width = input->width;
	img_height = input->height;
	img_size = img_width * img_height * 4;

	/* buffer allocation (total number : 8) */
	ALOGD("Buffer allocating");
	for (i = 0; i < MAX_DECON_WIN; i++) {
		for (j = 0; j < NUM_COLOR; j++) {
			buffer[i][j] = exynos_ion_alloc(client, img_size,
					EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
			if (buffer[i][j] < 0) {
				ALOGE("ion buffer[%d] allocation is failed\n", i);
				goto err_ion_open;
			}

			buf_addr[i][j] = (char *)mmap(NULL, img_size,
					PROT_READ | PROT_WRITE, MAP_SHARED,
					buffer[i][j], 0);
			if (buf_addr[i][j] == NULL) {
				ALOGE("buffer[%d] mapping is failed\n", i);
				goto err_ion;
			}

			addr[i][j] = (unsigned int *)buf_addr[i][j];
		}
		ALOGD("...win(%d)", i);
	}
	ALOGD("\n");

	/* Fill in the buffer with colors (RGB) */
	ALOGD("memory copying dst(0x%lx) size(0x%x)...",
			(unsigned long)addr[DEDICATED_WIN][0], img_size);
	for (i = 0, k = 0; i < img_height; i++) {
		png_bytep row = input->row_pointers[i];
		for (j = 0; j < img_width * 4; j++, k++) {
			buf_addr[DEDICATED_WIN][0][k] = row[j];
		}
	}

	memset(&win_data, 0, sizeof(win_data));

	ALOGD("win_config parameter configuration");
	get_hdr_win_config_png(config, buffer,
			lcd_width, lcd_height,
			img_width, img_height);

	/* call S5CFB_WIN_CONFIG ioctl */
	ALOGD("win_config ioctl");
	ret = ioctl(g_fd, S3CFB_WIN_CONFIG, &win_data);
	if (ret < 0) {
		ALOGE("failed to S3CFB_WIN_CONFIG ioctl ret = %d\n",
			ret);
		goto err;
	}
	ALOGD("sleep");
	usleep(30000);

err:
err_ion:
	/* munmap and buffer free */
	ALOGD("munmap and buffer free");
	for (i = 0; i < MAX_DECON_WIN; i++) {
		for (j = 0; j < NUM_COLOR; j++) {
			ret = munmap(buf_addr[i][j], img_size);
			exynos_ion_close(buffer[i][j]);
			if (ret < 0) {
				ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
								i, ret);
				goto err_ion_open;
			}
		}
	}

err_ion_open:
	ALOGD("exynos_ion_close");
	exynos_ion_close(client);
err_g_fd:
	free(input->row_pointers);

	return ret;
}

int read_ppm_file(char *filename, struct ppm_info_ *output)
{
	int img_size;
	char magic[2];
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		ALOGD("file(%s) open fail\n", filename);
		return -1;
	}
	ALOGD("ppm_file : filename (%s)", filename);

	fscanf(fp, "%c%c\n", &magic[0], &magic[1]);
	if (magic[0] != 'P' || magic[1] != '6') {
		ALOGD("ppm_file : wrong magic number (%c%c)", magic[0], magic[1]);
		abort();
	}
	ALOGD("ppm_file : magic number (%c%c)", magic[0], magic[1]);

	fscanf(fp, "%d %d\n", &output->width, &output->height);
	ALOGD("ppm_file : width(%d), height(%d)", output->width, output->height);

	fscanf(fp, "%d\n", &output->max);
	if (output->max != 65535) abort();
	ALOGD("ppm_file : max(%d)", output->max);

	output->byte_per_pixel = 6;
	ALOGD("ppm_file : byte_per_pixel(%d)", output->byte_per_pixel);

	img_size = sizeof(ppm_byte) * output->byte_per_pixel *
		output->width * output->height;
	ALOGD("ppm_file : image size(%d)", img_size);
	output->raw_data = (ppm_bytep)malloc(img_size);
	ALOGD("ppm_file : raw_data(0x%lx)", (unsigned long)output->raw_data);

	fread(output->raw_data, sizeof(unsigned char), img_size, fp);

	fclose(fp);
	return 0;
}

static inline int get_hdr_win_config_ppm(struct decon_win_config *config,
					int(*buffer)[NUM_COLOR],
					int lcd_width, int lcd_height,
					int img_width, int img_height)
{
	config[DEDICATED_WIN].state = decon_win_config::DECON_WIN_STATE_BUFFER;
	config[DEDICATED_WIN].acq_fence = -1;
	config[DEDICATED_WIN].plane_alpha = 255;
	config[DEDICATED_WIN].blending = DECON_BLENDING_NONE;
	config[DEDICATED_WIN].format = DECON_PIXEL_FORMAT_RGBA_1010102;
	config[DEDICATED_WIN].dpp_parm.eq_mode = CSC_BT_709;
	config[DEDICATED_WIN].dpp_parm.rot = DPP_ROT_NORMAL;
	config[DEDICATED_WIN].dpp_parm.hdr_std = DPP_HDR_OFF;

	config[DEDICATED_WIN].src.x = 0;
	config[DEDICATED_WIN].src.y = 0;
	config[DEDICATED_WIN].src.w = img_width;
	config[DEDICATED_WIN].src.h = img_height;
	config[DEDICATED_WIN].src.f_w = img_width;
	config[DEDICATED_WIN].src.f_h = img_height;
	config[DEDICATED_WIN].dst.x = 0;
	config[DEDICATED_WIN].dst.y = 0;
	config[DEDICATED_WIN].dst.w = img_width;
	config[DEDICATED_WIN].dst.h = img_height;
	config[DEDICATED_WIN].dst.f_w = lcd_width;
	config[DEDICATED_WIN].dst.f_h = lcd_height;

	config[DEDICATED_WIN].fd_idma[0] = buffer[DEDICATED_WIN][0];
	config[DEDICATED_WIN].idma_type = IDMA_VGS0;

	/* meta data */
	config[DEDICATED_WIN].fd_idma[1] = buffer[DEDICATED_WIN][1];

	return 0;
}

/*
 * conversion from ppm 10bit format to dpu dma 10bit format
 * ppm format 6byte -> dpu dma format(RGBA1010102) 4byte
 * each pixel 2byte shifts 6 bit becomes 10bit
 * alpha 2bit => b11
 */
static void convert_ppm_to_dma_RGBA10(ppm_bytep dst, ppm_bytep src,
				int width, int height)
{
	int i;
	int *tmp_dst = (int*) dst;
	int r, r1, r2;
	int g, g1, g2;
	int b, b1, b2;
	int a = 0x3;

	for (i = 0, a = 0x3; i < width * height; i++) {
		r1 = src[i*6], r2 = src[i*6+1];
		g1 = src[i*6+2], g2 = src[i*6+3];
		b1 = src[i*6+4], b2 = src[i*6+5];
		r = ((r1 << 8) | r2) >> 6;
		g = ((g1 << 8) | g2) >> 6;
		b = ((b1 << 8) | b2) >> 6;
		tmp_dst[i] = ((r << 22)| (g << 12) | (b << 2) | a);
	}
}

int set_backlight(int bl_level)
{
	FILE* fp;

	/*ALOGD("setHsc() %s\n", stream);*/

	fp = fopen(SYSFS_BACKLIGHT_0 , "w");
	if (fp == NULL) {
		fp = fopen(SYSFS_BACKLIGHT_1 , "w");
		if (fp == NULL) {
			fp = fopen(SYSFS_BACKLIGHT_2 , "w");
			if(fp == NULL) {
				ALOGE("open file error: %s, %s, %s",
						SYSFS_BACKLIGHT_2,
						SYSFS_BACKLIGHT_1,
						SYSFS_BACKLIGHT_0);
				return -1;
			}
		}
	}

	if (fprintf(fp, "%d", bl_level) < 0) {
		ALOGE("set backlight error");
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

int print_ppm_file(struct ppm_info_ *input)
{
	struct decon_win_config_data win_data;
	struct decon_win_config *config = win_data.config;
	struct fb_var_screeninfo info;
	int client;
	int buffer[MAX_DECON_WIN][NUM_COLOR];

	char *buf_addr[MAX_DECON_WIN][NUM_COLOR];
	unsigned int *addr[MAX_DECON_WIN][NUM_COLOR];

	int i = 0, ret;

	int img_width;
	int img_height;
	int img_size;
	int lcd_width, lcd_height;

	if (g_fd <= 0) {
		ALOGE("g_fd not open yet\n");
		ret = -1;
		goto err_g_fd;
	}

	client = exynos_ion_open();

	/* Getting Screen Info from the Machine */
	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &info);
	if (ret < 0) {
		ALOGE("failed to FBIOGET_VSCREENINFO\n");
		goto err_ion_open;
	}
	ALOGD("= Window Control =============================\n");
	ALOGD("FBIOGET_VSCREENINFO : xres = %d , yres = %d\n",
					info.xres, info.yres);
	lcd_width = info.xres;
	lcd_height = info.yres;
	img_width = input->width;
	img_height = input->height;
	img_size = img_width * img_height * 4;

	/* buffer allocation (total number : 8) */
	ALOGD("Buffer allocating");
	buffer[DEDICATED_WIN][0] = exynos_ion_alloc(client, img_size,
			EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
	if (buffer[DEDICATED_WIN][0] < 0) {
		ALOGE("ion buffer[%d] allocation is failed\n", i);
		goto err_ion_open;
	}

	buf_addr[DEDICATED_WIN][0] = (char *)mmap(NULL, img_size,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			buffer[DEDICATED_WIN][0], 0);
	if (buf_addr[DEDICATED_WIN][0] == NULL) {
		ALOGE("buffer[%d] mapping is failed\n", i);
		goto err_ion;
	}

	addr[DEDICATED_WIN][0] = (unsigned int *)buf_addr[DEDICATED_WIN][0];
	ALOGD("...win(%d)", DEDICATED_WIN);
	ALOGD("\n");

	/* meta data allocation */
	ALOGD("Meta data buffer allocating");
	buffer[DEDICATED_WIN][1] = exynos_ion_alloc(client, 128,
			EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
	if (buffer[DEDICATED_WIN][1] < 0) {
		ALOGE("ion buffer[%d] allocation is failed\n", i);
		goto err_ion_open;
	}

	buf_addr[DEDICATED_WIN][1] = (char *)mmap(NULL, 128,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			buffer[DEDICATED_WIN][1], 0);
	if (buf_addr[DEDICATED_WIN][1] == NULL) {
		ALOGE("buffer[%d] mapping is failed\n", i);
		goto err_ion;
	}

	addr[DEDICATED_WIN][1] = (unsigned int *)buf_addr[DEDICATED_WIN][1];
	ALOGD("...win(%d) - meta data", DEDICATED_WIN);
	ALOGD("\n");

	/* Fill in the buffer with colors (RGB) */
	convert_ppm_to_dma_RGBA10((ppm_bytep)buf_addr[DEDICATED_WIN][0],
			(ppm_bytep)input->raw_data, input->width, input->height);

	memset(&win_data, 0, sizeof(win_data));

	ALOGD("win_config parameter configuration");
	get_hdr_win_config_ppm(config, buffer,
			lcd_width, lcd_height,
			img_width, img_height);

	/* call S5CFB_WIN_CONFIG ioctl */
	ALOGD("win_config ioctl");
	ret = ioctl(g_fd, S3CFB_WIN_CONFIG, &win_data);
	if (ret < 0) {
		ALOGE("failed to S3CFB_WIN_CONFIG ioctl ret = %d\n",
			ret);
		goto err;
	}
	ALOGD("sleep");
	usleep(30000);

err:
	/* munmap and buffer free */
	ALOGD("munmap and buffer free");
	ret = munmap(buf_addr[DEDICATED_WIN][0], img_size);
	if (ret < 0) {
		ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
						DEDICATED_WIN, ret);
		goto err_ion;
	}
	exynos_ion_close(buffer[DEDICATED_WIN][0]);
err_ion:
	ret = munmap(buf_addr[DEDICATED_WIN][1], 128);
	if (ret < 0) {
		ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
				DEDICATED_WIN, ret);
		goto err_ion_open;
	}
	exynos_ion_close(buffer[DEDICATED_WIN][1]);
err_ion_open:
	ALOGD("exynos_ion_close");
	exynos_ion_close(client);
err_g_fd:
	free(input->raw_data);

	return ret;
}

int set_blank(void)
{
	int ret = 0;

	if (g_fd <= 0) {
		ret = -1;
		goto err;
	}

	/* unblank */
	ALOGD("unblank");
	ret = ioctl(g_fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	if (ret < 0) {
		ALOGE("failed to FB_BLANK_POWERDOWN\n");
		ret = -1;
		goto err_blank;
	}

err_blank:
	close(g_fd);
	g_fd = 0;
err:
	return ret;
}

int set_unblank(void)
{
	int ret = 0;

	/* Device Node open & Ion Client Create */
	g_fd = open(DEVNAME_FB0, O_RDWR);
	if (g_fd < 0) {
		ALOGE("failed to open framebuffer : %s, %d\n",
						DEVNAME_FB0, g_fd);
		goto err;
	}

	/* unblank */
	ALOGD("unblank");
	ret = ioctl(g_fd, FBIOBLANK, FB_BLANK_UNBLANK);
	if (ret < 0) {
		ALOGE("failed to FB_BLANK_UNBLANK\n");
		ret = -1;
		goto err_blank;
	}

	return ret;

err_blank:
	close(g_fd);
err:
	return ret;
}

static void fill_pattern(char* dst, int pattern, int RGBA,
				int width, int height)
{
	int i, x, y;
	int *tmp_dst = (int*) dst;
	double ratio, root_of_ratio;
	int start_x, start_y, width_r, height_r;
	int corner_w, corner_h, corner_sx, corner_sy;

	switch(pattern) {
		case 0:
			ratio = 1.0;
			break;
		case 1:
			ratio = 0.02;
			break;
		case 2:
			ratio = 0.05;
			break;
		case 3:
			ratio = 0.1;
			break;
		case 4:
			ratio = 0.2;
			break;
		case 5:
			ratio = 0.3;
			break;
		case 6:
			ratio = 0.4;
			break;
		case 7: /* corner */
			ratio = 0.025;
			root_of_ratio = sqrt(ratio);
			corner_w = (int)(width * root_of_ratio);
			corner_sx = width - corner_w;
			corner_h = (int)(height * root_of_ratio);
			corner_sy = height - corner_h;

			for (y = 0; y < corner_h; y++) {
				for (x = 0; x < corner_w; x++) {
					i = (y * width) + x;
					tmp_dst[i] = RGBA;
				}
			}
			for (y = corner_sy; y < height; y++) {
				for (x = 0; x < corner_w; x++) {
					i = (y * width) + x;
					tmp_dst[i] = RGBA;
				}
			}
			for (y = 0; y < corner_h; y++) {
				for (x = corner_sx; x < width; x++) {
					i = (y * width) + x;
					tmp_dst[i] = RGBA;
				}
			}
			for (y = corner_sy; y < height; y++) {
				for (x = corner_sx; x < width; x++) {
					i = (y * width) + x;
					tmp_dst[i] = RGBA;
				}
			}
			return;
		default:
			ratio = 1.0;
			break;
	}
	root_of_ratio = sqrt(ratio);
	width_r = (int)(width * root_of_ratio);
	height_r = (int)(height * root_of_ratio);
	start_x = (width - width_r) / 2;
	start_y = (height - height_r) / 2;
	for (y = start_y; y < start_y + height_r; y++) {
		for(x = start_x; x < start_x + width_r; x++) {
			i = (y * width) + x;
			tmp_dst[i] = RGBA;
		}
	}
	return;
}

static inline int get_hdr_win_config_raw(struct decon_win_config *config,
					int(*buffer)[NUM_COLOR],
					int lcd_width, int lcd_height,
					int img_width, int img_height)
{
	config[DEDICATED_WIN].state = decon_win_config::DECON_WIN_STATE_BUFFER;
	config[DEDICATED_WIN].acq_fence = -1;
	config[DEDICATED_WIN].plane_alpha = 255;
	config[DEDICATED_WIN].blending = DECON_BLENDING_NONE;
	config[DEDICATED_WIN].format = DECON_PIXEL_FORMAT_RGBA_1010102;
	config[DEDICATED_WIN].dpp_parm.eq_mode = CSC_BT_709;
	config[DEDICATED_WIN].dpp_parm.rot = DPP_ROT_NORMAL;
	config[DEDICATED_WIN].dpp_parm.hdr_std = DPP_HDR_OFF;

	config[DEDICATED_WIN].src.x = 0;
	config[DEDICATED_WIN].src.y = 0;
	config[DEDICATED_WIN].src.w = img_width;
	config[DEDICATED_WIN].src.h = img_height;
	config[DEDICATED_WIN].src.f_w = img_width;
	config[DEDICATED_WIN].src.f_h = img_height;
	config[DEDICATED_WIN].dst.x = 0;
	config[DEDICATED_WIN].dst.y = 0;
	config[DEDICATED_WIN].dst.w = img_width;
	config[DEDICATED_WIN].dst.h = img_height;
	config[DEDICATED_WIN].dst.f_w = lcd_width;
	config[DEDICATED_WIN].dst.f_h = lcd_height;

	config[DEDICATED_WIN].fd_idma[0] = buffer[DEDICATED_WIN][0];
	config[DEDICATED_WIN].idma_type = IDMA_VGS0;

	/* meta data */
	config[DEDICATED_WIN].fd_idma[1] = buffer[DEDICATED_WIN][1];
	return 0;
}

int print_raw_data_RGBA10(int pattern, int RGBA)
{
	struct decon_win_config_data win_data;
	struct decon_win_config *config = win_data.config;
	struct fb_var_screeninfo info;
	int client;
	int buffer[MAX_DECON_WIN][NUM_COLOR];

	char *buf_addr[MAX_DECON_WIN][NUM_COLOR];
	unsigned int *addr[MAX_DECON_WIN][NUM_COLOR];

	int i = 0, ret;

	int img_width;
	int img_height;
	int img_size;
	int lcd_width, lcd_height;

	if (g_fd <= 0) {
		ALOGE("g_fd not open yet\n");
		ret = -1;
		goto err_g_fd;
	}

	client = exynos_ion_open();

	/* Getting Screen Info from the Machine */
	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &info);
	if (ret < 0) {
		ALOGE("failed to FBIOGET_VSCREENINFO\n");
		goto err_ion_open;
	}
	ALOGD("= Window Control =============================\n");
	ALOGD("FBIOGET_VSCREENINFO : xres = %d , yres = %d\n",
					info.xres, info.yres);
	lcd_width = info.xres;
	lcd_height = info.yres;
	img_width = lcd_width;
	img_height = lcd_height;
	img_size = img_width * img_height * 4;

	/* buffer allocation (total number : 8) */
	ALOGD("Buffer allocating");
	buffer[DEDICATED_WIN][0] = exynos_ion_alloc(client, img_size,
			EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
	if (buffer[DEDICATED_WIN][0] < 0) {
		ALOGE("ion buffer[%d] allocation is failed\n", i);
		goto err_ion_open;
	}

	buf_addr[DEDICATED_WIN][0] = (char *)mmap(NULL, img_size,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			buffer[DEDICATED_WIN][0], 0);
	if (buf_addr[DEDICATED_WIN][0] == NULL) {
		ALOGE("buffer[%d] mapping is failed\n", i);
		goto err_ion;
	}

	addr[DEDICATED_WIN][0] = (unsigned int *)buf_addr[DEDICATED_WIN][0];
	ALOGD("...win(%d)", DEDICATED_WIN);
	ALOGD("\n");

	/* meta data allocation */
	ALOGD("Meta data buffer allocating");
	buffer[DEDICATED_WIN][1] = exynos_ion_alloc(client, 128,
			EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
	if (buffer[DEDICATED_WIN][1] < 0) {
		ALOGE("ion buffer[%d] allocation is failed\n", i);
		goto err_ion_open;
	}

	buf_addr[DEDICATED_WIN][1] = (char *)mmap(NULL, 128,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			buffer[DEDICATED_WIN][1], 0);
	if (buf_addr[DEDICATED_WIN][1] == NULL) {
		ALOGE("buffer[%d] mapping is failed\n", i);
		goto err_ion;
	}

	addr[DEDICATED_WIN][1] = (unsigned int *)buf_addr[DEDICATED_WIN][1];
	ALOGD("...win(%d) - meta data", DEDICATED_WIN);
	ALOGD("\n");

	/* Fill in the buffer with colors (RGB) */
	fill_pattern(buf_addr[DEDICATED_WIN][0], pattern,
			RGBA, lcd_width, lcd_height);

	memset(&win_data, 0, sizeof(win_data));

	ALOGD("win_config parameter configuration");
	get_hdr_win_config_raw(config, buffer,
			lcd_width, lcd_height,
			img_width, img_height);

	/* call S5CFB_WIN_CONFIG ioctl */
	ALOGD("win_config ioctl");
	ret = ioctl(g_fd, S3CFB_WIN_CONFIG, &win_data);
	if (ret < 0) {
		ALOGE("failed to S3CFB_WIN_CONFIG ioctl ret = %d\n",
			ret);
		goto err;
	}
	ALOGD("sleep");
	usleep(30000);

err:
	/* munmap and buffer free */
	ALOGD("munmap and buffer free");
	ret = munmap(buf_addr[DEDICATED_WIN][0], img_size);
	if (ret < 0) {
		ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
						DEDICATED_WIN, ret);
		goto err_ion;
	}
	exynos_ion_close(buffer[DEDICATED_WIN][0]);
err_ion:
	ret = munmap(buf_addr[DEDICATED_WIN][1], 128);
	if (ret < 0) {
		ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
				DEDICATED_WIN, ret);
		goto err_ion_open;
	}
	exynos_ion_close(buffer[DEDICATED_WIN][1]);
err_ion_open:
	ALOGD("exynos_ion_close");
	exynos_ion_close(client);
err_g_fd:

	return ret;
}

static inline int get_hdr_win_config_raw8(struct decon_win_config *config,
					int(*buffer)[NUM_COLOR],
					int lcd_width, int lcd_height,
					int img_width, int img_height)
{
	config[DEDICATED_WIN].state = decon_win_config::DECON_WIN_STATE_BUFFER;
	config[DEDICATED_WIN].acq_fence = -1;
	config[DEDICATED_WIN].plane_alpha = 255;
	config[DEDICATED_WIN].blending = DECON_BLENDING_NONE;
	config[DEDICATED_WIN].format = DECON_PIXEL_FORMAT_RGBA_8888;
	config[DEDICATED_WIN].dpp_parm.eq_mode = CSC_BT_709;
	config[DEDICATED_WIN].dpp_parm.rot = DPP_ROT_NORMAL;
	config[DEDICATED_WIN].dpp_parm.hdr_std = DPP_HDR_OFF;

	config[DEDICATED_WIN].src.x = 0;
	config[DEDICATED_WIN].src.y = 0;
	config[DEDICATED_WIN].src.w = img_width;
	config[DEDICATED_WIN].src.h = img_height;
	config[DEDICATED_WIN].src.f_w = img_width;
	config[DEDICATED_WIN].src.f_h = img_height;
	config[DEDICATED_WIN].dst.x = 0;
	config[DEDICATED_WIN].dst.y = 0;
	config[DEDICATED_WIN].dst.w = img_width;
	config[DEDICATED_WIN].dst.h = img_height;
	config[DEDICATED_WIN].dst.f_w = lcd_width;
	config[DEDICATED_WIN].dst.f_h = lcd_height;

	config[DEDICATED_WIN].fd_idma[0] = buffer[DEDICATED_WIN][0];
	config[DEDICATED_WIN].idma_type = IDMA_VGS0;

	/* meta data */
	config[DEDICATED_WIN].fd_idma[1] = buffer[DEDICATED_WIN][1];
	return 0;
}

int print_raw_data_RGBA8(int pattern, int RGBA)
{
	struct decon_win_config_data win_data;
	struct decon_win_config *config = win_data.config;
	struct fb_var_screeninfo info;
	int client;
	int buffer[MAX_DECON_WIN][NUM_COLOR];

	char *buf_addr[MAX_DECON_WIN][NUM_COLOR];
	unsigned int *addr[MAX_DECON_WIN][NUM_COLOR];

	int ret;

	int img_width;
	int img_height;
	int img_size;
	int lcd_width, lcd_height;

	if (g_fd <= 0) {
		ALOGE("g_fd not open yet\n");
		ret = -1;
		goto err_g_fd;
	}

	client = exynos_ion_open();

	/* Getting Screen Info from the Machine */
	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &info);
	if (ret < 0) {
		ALOGE("failed to FBIOGET_VSCREENINFO\n");
		goto err_ion_open;
	}
	ALOGD("= Window Control =============================\n");
	ALOGD("FBIOGET_VSCREENINFO : xres = %d , yres = %d\n",
					info.xres, info.yres);
	lcd_width = info.xres;
	lcd_height = info.yres;
	img_width = lcd_width;
	img_height = lcd_height;
	img_size = img_width * img_height * 4;

	/* buffer allocation (total number : 8) */
	ALOGD("Buffer allocating");
	buffer[DEDICATED_WIN][0] = exynos_ion_alloc(client, img_size,
			EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
	if (buffer[DEDICATED_WIN][0] < 0) {
		ALOGE("ion buffer[%d] allocation is failed\n", DEDICATED_WIN);
		goto err_ion_open;
	}

	buf_addr[DEDICATED_WIN][0] = (char *)mmap(NULL, img_size,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			buffer[DEDICATED_WIN][0], 0);
	if (buf_addr[DEDICATED_WIN][0] == NULL) {
		ALOGE("buffer[%d] mapping is failed\n", DEDICATED_WIN);
		goto err_ion;
	}

	addr[DEDICATED_WIN][0] = (unsigned int *)buf_addr[DEDICATED_WIN][0];
	ALOGD("...win(%d)", DEDICATED_WIN);
	ALOGD("\n");

	/* meta data allocation */
	ALOGD("Meta data buffer allocating");
	buffer[DEDICATED_WIN][1] = exynos_ion_alloc(client, 128,
			EXYNOS_ION_HEAP_SYSTEM_MASK, 0);
	if (buffer[DEDICATED_WIN][1] < 0) {
		ALOGE("ion buffer[%d] allocation is failed\n", DEDICATED_WIN);
		goto err_ion_open;
	}

	buf_addr[DEDICATED_WIN][1] = (char *)mmap(NULL, 128,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			buffer[DEDICATED_WIN][1], 0);
	if (buf_addr[DEDICATED_WIN][1] == NULL) {
		ALOGE("buffer[%d] mapping is failed\n", DEDICATED_WIN);
		goto err_ion;
	}

	addr[DEDICATED_WIN][1] = (unsigned int *)buf_addr[DEDICATED_WIN][1];
	ALOGD("...win(%d) - meta data", DEDICATED_WIN);
	ALOGD("\n");

	/* Fill in the buffer with colors (RGB) */
	fill_pattern(buf_addr[DEDICATED_WIN][0], pattern,
			RGBA, lcd_width, lcd_height);

	memset(&win_data, 0, sizeof(win_data));

	ALOGD("win_config parameter configuration");
	get_hdr_win_config_raw8(config, buffer,
			lcd_width, lcd_height,
			img_width, img_height);

	/* call S5CFB_WIN_CONFIG ioctl */
	ALOGD("win_config ioctl");
	ret = ioctl(g_fd, S3CFB_WIN_CONFIG, &win_data);
	if (ret < 0) {
		ALOGE("failed to S3CFB_WIN_CONFIG ioctl ret = %d\n",
			ret);
		goto err;
	}
	ALOGD("sleep");
	usleep(30000);

err:
	/* munmap and buffer free */
	ALOGD("munmap and buffer free");
	ret = munmap(buf_addr[DEDICATED_WIN][0], img_size);
	if (ret < 0) {
		ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
						DEDICATED_WIN, ret);
		goto err_ion;
	}
	exynos_ion_close(buffer[DEDICATED_WIN][0]);
err_ion:
	ret = munmap(buf_addr[DEDICATED_WIN][1], 128);
	if (ret < 0) {
		ALOGE("failed to ion_unmap buf[%d] ret = %d\n",
				DEDICATED_WIN, ret);
		goto err_ion_open;
	}
	exynos_ion_close(buffer[DEDICATED_WIN][1]);
err_ion_open:
	ALOGD("exynos_ion_close");
	exynos_ion_close(client);
err_g_fd:

	return ret;
}

int setHDR(int id, struct IntToConstString *hdrIdToPathList, int listSize, const char* stream)
{
    int fd;
    const char *hdrPath;
    int i;

    /*ALOGD("setHDR() %s\n", stream);*/
    for (i = 0; i < listSize; i++) {
	    if (hdrIdToPathList[i].integer == id) {
		    hdrPath = hdrIdToPathList[i].string;
		    break;
	    }
    }
    if (i == listSize) {
        ALOGE("hdr id(%d) not supported on this project", id);
        return -1;
    }

    fd = open(hdrPath, O_WRONLY);
    if (fd < 0) {
        ALOGE("open file error: %s", hdrPath);
        return -1;
    }

    if (write(fd, stream, sizeof(struct dpp_hdr_params)) < 0) {
        ALOGE("hdr write error");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

template<typename Out>
void split(const string &s, char delim, Out result)
{
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim))
		*(result++) = item;
}

vector<string> split(const string &s, const char delim)
{
	vector<string> elems;
	split(s, delim, back_inserter(elems));

	return elems;
}
