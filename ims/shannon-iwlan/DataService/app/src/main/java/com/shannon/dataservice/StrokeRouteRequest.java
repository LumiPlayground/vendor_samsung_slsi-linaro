/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
package com.shannon.dataservice;

public class StrokeRouteRequest extends StrokeRequestMessage {
    public StrokeRouteRequest(RequestMessageType request, String source, String destination, String adapter) {
        super(request);
        align(8);
        putString("src", source);
        putString("dst", destination);
        putString("interface", adapter);
        align();
    }
}
