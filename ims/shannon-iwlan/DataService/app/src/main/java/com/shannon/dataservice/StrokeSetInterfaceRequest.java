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

public class StrokeSetInterfaceRequest extends StrokeRequestMessage {
    public StrokeSetInterfaceRequest(String name, String interfaceName) {
        super(RequestMessageType.STR_REQ_SET_INTERFACE);
        align(8);
        putString("name", name);
        putString("interface", interfaceName);
        align();
    }
}
