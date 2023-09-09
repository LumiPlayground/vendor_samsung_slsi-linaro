#pragma once

#include <stdio.h>

#define MAX_MODEL_NUM 5

#define VRA_BIN_START_SIGNATURE 0x35792468
#define VRA_BIN_END_SIGNATURE   0xeca8db97

#define VRA_NET_HDR_VERSION 1

#define VRA_MAX_CNN_HW_PARTS      4
#define VRA_MAX_CNN_HW_WORK_AREAS 7

// Each bin file describing CNN network includes the following structure:
// VRA_NetworkHdrStr, data (according to offsets described in header), end signature
#define VAL_4_BYTES 4
#define VAL_2_BYTES 2

#define VRA_MAX_OUTPUTS 15
#define VRA_NUM_OUTPUTS 5

#define SIZE_OF_BIN   100
#define SIZE_OF_INST  10
#define SIZE_OF_CONST 50
#define SIZE_OF_TMP   40

#define ENGINE_TIMEOUT_SEC 10

enum VraError {
    VRA_OK,
    VRA_ERR,
    VRA_ERR_NO_AVAILABLE_MODEL_ID,
    VRA_ERR_INIT_MODEL,
    VRA_ERR_ENGINE_TIMEOUT,
    VRA_ERR_HDR_START_SIGNATURE,
    VRA_ERR_HDR_END_SIGNATURE,
    VRA_ERR_TOTAL_FILE_SIZE,
    VRA_ERR_INPUT_FORMAT,
    VRA_ERR_HDR_VERSION,
    VRA_ERR_NN_TYPE,
    VRA_ERR_WRONG_OFFSET,
    VRA_ERR_EXEC_ERROR,
    VRA_ERR_UNEXPECTED,
    VRA_ERR_NN_BUSY,
    VRA_ERR_MEM_LEAK,
    VRA_ERR_NULL_MODEL_PARAM
};

enum VraInputFormatType {
    VRA_FORMAT_YUV_422_3P,
    VRA_FORMAT_BGR_1P,
    VRA_FORMAT_BGR_3P,
    NUM_VRA_FORMATS
};

typedef enum {
    VRA_CNN_NFD_V3,
    NUM_CNN_TYPES
} VRA_CnnType;

typedef enum {
    VRA_CLOCK_STATIC, // Use the highest min clock level among open models
    VRA_CLOCK_DYNAMIC, // Each model operates at the minimum clock level set in the configuration for each model
} VRA_ClockMode;
