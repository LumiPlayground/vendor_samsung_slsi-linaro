/*
 * Copyright (c) SAMSUNG Electronics Co., Ltd. All rights reserved.
 * This software is a confidential stuff and proprietary of
 * SAMSUNG Electronics Co., Ltd.
 * So you shall not disclose this software other company or persons
 * without permission of SAMSUNG and shall use this software only
 * in accordance with the license agreement of SAMSUNG.
 */
#pragma once

#include <cstdint>

#include "VraTypes.h"

struct VraEngineParams {
    uint32_t vraVersion;  // currently only VRA version 60 (VRAv6.0) is supported.
    uint32_t timeOut;
    VRA_ClockMode clockMode;
    uint32_t reserved[8] = {0};
};

struct VraModelParams {
    const char *modelPath = nullptr;
    bool dequantizeOutputs = false;
    uint32_t clockLevel = 0;
    uint32_t reserved[8] = {0};
};

struct VraBuffer {  //Buffer for user space
    void *ptr = nullptr;
    uint32_t sizeInBytes = 0;
    //The following fields are valid for output buffers only:
    uint32_t numOfchannels = 0;
    int8_t *fractionalLengthArray = nullptr;
};

/*
Initializes the VRA engine.
*/
VraError InitEngine(const VraEngineParams *params);

/*
Terminates the VRA engine.
*/
VraError TerminateEngine();

/*
Opens a VRA model using the given model binary file, and return the assigned model ID.
Parameters:
- modelId - returned model ID assigned to the opened model.
- modelPath - model binary file.
- dequantizeOutputs - if true, the returned network outputs are de-quantized.
*/
VraError OpenModel(uint32_t &modelId, const char *modelPath, bool dequantizeOutputs);

/*
Opens a VRA model using the given model binary file, and return the assigned model ID.
Parameters:
- modelId - returned model ID assigned to the opened model.
- VraModelParams - informations to execute model on VRA HW.
*/
VraError OpenModel(uint32_t &modelId, VraModelParams *params);

/*
Closes the given VRA model and frees its buffers.
*/
void CloseModel(uint32_t modelId);

/*
Allocates input and output buffers for the given model, and returns an array with the allocated buffers.
Parameters:
- modelId - model ID to allocate the buffers for.
- outputBuffs - returned array of allocated output buffers.
- inputBuf - returned array of allocated input buffers.
- numOfOutputs - returned number of allocated output buffers
- numOfInputs - returned number of allocated input buffers
*/
void AllocateNetworkBuffers(
        uint32_t modelId,
        VraBuffer **outputBuffs,
        VraBuffer **inputBuf,
        uint32_t &numOfOutputs,
        uint32_t &numOfInputs);

/*
Executes the given model.
Uses the pre-allocated input buffers as model input.
The model outputs are written to the pre-allocated output buffers.
*/
VraError ExecuteModel(uint32_t modelId);

/*
Dumps HW debug data to the log.
*/
void DebugDump();
