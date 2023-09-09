/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define LOG_TAG "WLBTLOGDBG"
#include <utils/Log.h>

#include "filedir.h"

int makedir(const char* path) {
    char fullpath[255] = {0};
    char subdirpath[255] = {0};
    char* subdir;
    int skipcount = 2 /*Skip "/dir1" and "/dir1/dir2" */;

    if (path[0] != '/') {
        fullpath[0] = '/';
        strncpy(fullpath+1, path, sizeof(fullpath)-2);
    } else {
        strncpy(fullpath, path, sizeof(fullpath)-1);
    }
    if (fullpath[strlen(fullpath)-1] != '/') {
        fullpath[strlen(fullpath)] = '/';
        fullpath[strlen(fullpath)+1] = 0;
    }
    subdir = fullpath+1;
    while ((subdir = strstr(subdir, "/"))) {
        if (skipcount > 0) {
            skipcount--;
            subdir = subdir+1;
            continue;
        }
        if (subdir - fullpath >= sizeof(subdirpath))
            return -1;
        strncpy(subdirpath, fullpath, subdir - fullpath);
        mkdir(subdirpath, 0777);
        chmod(subdirpath, 0777);
        subdir = subdir+1;
    }
    return 0;
}

int create_dir(const char* file) {
    char dir_path[255];
    char* output_dir_end = NULL;
    int str_len;

    memset(dir_path, 0, sizeof(dir_path));
    str_len = strlen(file);
    /* [Static Analysis] CID:249773 */
    strncpy(dir_path, file, sizeof(dir_path)-1);
    for (int i = str_len -1 ; i > -1 ; i--) {
        if (output_dir_end == NULL && dir_path[i] == '/') {
            output_dir_end = dir_path + i;
            break;
        }
    }
    if (output_dir_end == NULL)
        return -1;
    *output_dir_end = 0;
    return makedir(dir_path);
}

FILE* create_file(const char* file) {
    int fd = -1;
    FILE* fp = NULL;

    fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if (fd == -1)
        return NULL;
    close(fd);
    chmod(file, 0666);
    fp = fopen(file, "w");
    return fp;
}
