/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
 * tlvparser_test.cpp
 *
 *  Created on: 2022.01.21.
 */

#include <gtest/gtest.h>
#include "stk/tlvparser.h"

TEST(TlvParser, Address) {
    struct {
        BYTE data[10];
        int length;
        int TON;
        int NPI;
        const char* TONString;
        const char* NPIString;
        const char* number;
    } testVector[] = {
        { { 0x86, 0x07, 0x91, 0x19, 0x89, 0x54, 0x80, 0x56, 0x78 }, 9, 1, 1,
            "INTERNATIONAL_NUMBER", "TELEPHONY_NUMBERING_PLAN", "919845086587" },
        { { 0x86, 0x03, 0x81, 0x65, 0x87 }, 5, 0, 1,
            "TON_UNKNOWN", "TELEPHONY_NUMBERING_PLAN", "5678" },
    };

    for (auto test : testVector) {
        const int numOfConstructors = 2;

        Address address[numOfConstructors] = { Address(), Address(test.data, test.length) };
        address[0].Set(test.data, test.length);

        for (int i = 0; i < numOfConstructors; i++) {
            EXPECT_EQ(address[i].GetTON(), test.TON);
            EXPECT_EQ(address[i].GetNPI(), test.NPI);
            EXPECT_STREQ(address[i].GetTONString(), test.TONString);
            EXPECT_STREQ(address[i].GetNPIString(), test.NPIString);
            EXPECT_STREQ(address[i].GetDiallingNumber().c_str(), test.number);
        }
    }
}

TEST(TlvParser, DeviceIdentity) {
    struct {
        int len;
        BYTE data[4];
        const char *sourceString;
        const char *destString;
    } testVector[] = {
        { 4, { 0x82, 0x02, 0x81, 0x82 }, "UICC", "TERMINAL" },
        { 4, { 0x08, 0x02, 0x82, 0x81 }, "TERMINAL", "UICC" },
        { 4, { 0x08, 0x02, 0x81, 0x83 }, "UICC", "NETWORK" },
        { 4, { 0x08, 0x02, 0x01, 0x81 }, "KEYPAD", "UICC" },
        { 4, { 0x08, 0x02, 0x02, 0x81 }, "DISPLAY", "UICC" },
        { 4, { 0x08, 0x02, 0x03, 0x81 }, "EARPIECE", "UICC" },
        { 4, { 0x08, 0x02, 0x21, 0x81 }, "CHANNEL1", "UICC" },
        { 4, { 0x08, 0x02, 0x22, 0x81 }, "CHANNEL2", "UICC" },
        { 4, { 0x08, 0x02, 0x23, 0x81 }, "CHANNEL3", "UICC" },
        { 4, { 0x08, 0x02, 0x24, 0x81 }, "CHANNEL4", "UICC" },
        { 4, { 0x08, 0x02, 0x25, 0x81 }, "CHANNEL5", "UICC" },
        { 4, { 0x08, 0x02, 0x26, 0x81 }, "CHANNEL6", "UICC" },
        { 4, { 0x08, 0x02, 0x27, 0x81 }, "CHANNEL7", "UICC" },
    };

    for (auto test : testVector) {
        const int numOfConstructors = 2;

        DeviceIdentity deviceIdentity[numOfConstructors] = { DeviceIdentity(), DeviceIdentity(test.data, test.len) };
        deviceIdentity[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructors; i++) {
            EXPECT_EQ(deviceIdentity[i].GetSourceID(), test.data[2]);
            EXPECT_EQ(deviceIdentity[i].GetDestinationID(), test.data[3]);

            EXPECT_STREQ(deviceIdentity[i].GetDeviceIdString(true), test.sourceString);
            EXPECT_STREQ(deviceIdentity[i].GetDeviceIdString(false), test.destString);
        }
    }
}

TEST(TlvParser, Duration) {
    struct {
        int len;
        BYTE data[4];
        const char *timeUnitString;
    } testVector[] = {
        { 4, { 0x84, 0x02, 0x00, 0x01 }, "MINUTES" },
        { 4, { 0x84, 0x02, 0x01, 0x0A }, "SECONDS" },
        { 4, { 0x84, 0x02, 0x02, 0xF2 }, "TENTHS_OF_SECONDS" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        Duration duration[numOfConstructor] = { Duration(), Duration(test.data, test.len) };
        duration[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(duration[i].GetTimeUnit(), test.data[2]);
            EXPECT_EQ(duration[i].GetTimeInterval(), test.data[3]);

            EXPECT_STREQ(duration[i].GetTimeUnitString(), test.timeUnitString);
        }
    }
}

TEST(TlvParser, Result) {
    struct {
        int len;
        BYTE data[19];
        int result;
        const char* resultStr;
        bool isAdditionalInfoPresent;
        const char* additionalInfoStr;
    } testVector[] = {
        { 3, { 0x83, 0x01, 0x00 }, 0x00, "SUCCESS", false, "" },
        { 3, { 0x83, 0x01, 0x01 }, 0x01, "PERFORM_WITH_PARTIAL", false, "" },
        { 3, { 0x83, 0x01, 0x02 }, 0x02, "PERFORM_WITH_MISSING", false, "" },
        { 3, { 0x83, 0x01, 0x10 }, 0x10, "PROACTIVE_SESSION_TERMINATED", false, "" },
        { 3, { 0x83, 0x01, 0x11 }, 0x11, "BACKWARD_PROACTIVE_BY_USER", false, "" },
        { 3, { 0x83, 0x01, 0x12 }, 0x12, "NO_RESPONSE_FROM_USER", false, "" },
        { 3, { 0x83, 0x01, 0x20 }, 0x20, "TERMINAL_UNABLE_PROCESS_COMMAND", false, "" },
        { 3, { 0x83, 0x01, 0x21 }, 0x21, "NETWORK_UNABLE_PROCESS_COMMAND", false, "" },
        { 3, { 0x83, 0x01, 0x22 }, 0x22, "USER_UNACCEPT_PROACTIVE_COMMAND", false, "" },
        { 3, { 0x83, 0x01, 0x30 }, 0x30, "BEYOND_TERMINAL_CAPABILITY", false, "" },
        { 19, { 0x83, 0x11, 0x00, 0x0A, 0xA0, 0x0D, 0x04, 0x01, 0x21, 0x30, 0x08, 0x30, 0x06,
                  0x83, 0x01, 0x00, 0x84, 0x01, 0x07 },
            0x00, "SUCCESS", true, "0AA00D04012130083006830100840107" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        Result result[numOfConstructor] = { Result(), Result(test.data, test.len) };
        result[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(result[i].GetGeneralResult(), test.result);
            EXPECT_STREQ(result[i].GetResult(), test.resultStr);

            if (test.isAdditionalInfoPresent) {
                EXPECT_STREQ(result[i].GetAdditionalInfoString().c_str(), test.additionalInfoStr);
            }
        }
    }
}

TEST(TlvParser, AlphaIdentifier) {
    struct {
        int len;
        BYTE data[17];
        const char *alphaString;
    } testVector[] = {
        { 12, { 0x85, 0x0A, 0x43, 0x6C, 0x6F, 0x73, 0x65, 0x20, 0x49, 0x44, 0x20, 0x31 }, "Close ID 1" },
        { 17, { 0x85, 0x0f, 0x61, 0x69, 0x72, 0x74, 0x65, 0x6c, 0x20, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x73 }, "airtel services" },
        { 12, { 0x85, 0x0a, 0x49, 0x64, 0x65, 0x61, 0x20, 0x50, 0x6f, 0x77, 0x65, 0x72 }, "Idea Power" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        AlphaIdentifier alphaIdentifier[numOfConstructor] = { AlphaIdentifier(), AlphaIdentifier(test.data, test.len) };
        alphaIdentifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(alphaIdentifier[i].GetAlphaIdentifier(), test.alphaString);
        }
    }
}

TEST(TlvParser, ItemIdentifier) {
    struct {
        int len;
        BYTE data[19];
        int id;
        const char *identifier;
    } testVector[] = {
        { 19, { 0x8f, 0x11, 0x02, 0x50, 0x72, 0x65, 0x6d, 0x69, 0x75, 0x6d, 0x20, 0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x73 }, 2, "Premium Services" },
        { 13, { 0x8f, 0x0b, 0x03, 0x57, 0x68, 0x61, 0x74, 0x27, 0x73, 0x20, 0x4e, 0x65, 0x77 }, 3, "What's New" },
        { 15, { 0x8f, 0x0d, 0x04, 0x44, 0x69, 0x61, 0x6c, 0x65, 0x72, 0x20, 0x54, 0x6f, 0x6e, 0x65, 0x73 }, 4, "Dialer Tones" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ItemIdentifier itemIdentifier[numOfConstructor] = { ItemIdentifier(), ItemIdentifier(test.data, test.len) };
        itemIdentifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(itemIdentifier[i].GetId(), test.id);
            EXPECT_STREQ(itemIdentifier[i].GetItemIdentifier().c_str(), test.identifier);
        }
    }
}

TEST(TlvParser, ItemIDIdentifier) {
    struct {
        int len;
        BYTE data[3];
        int id;
    } testVector[] = {
        { 3, { 0x90, 0x01, 0x80 }, 0x80 },
        { 3, { 0x90, 0x01, 0x81 }, 0x81 },
        { 3, { 0x90, 0x01, 0x82 }, 0x82 },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ItemIDIdentifier itemIDIdentifier[numOfConstructor] = { ItemIDIdentifier(),
            ItemIDIdentifier(test.data, test.len) };
        itemIDIdentifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(itemIDIdentifier[i].GetId(), test.id);
        }
    }
}

TEST(TlvParser, IconIdentifier) {
    struct {
        int len;
        BYTE data[4];
        int iconQualifier;
        int iconIdentifier;
        const char *iconQualifierStr;
        const char *iconIdentifierStr;
    } testVector[] = {
        { 4, { 0x9E, 0x02, 0x00, 0x01 }, 0, 1, "Icon is self explanatory", "record 1 in EF_IMG" },
        { 4, { 0x9E, 0x02, 0x00, 0x02 }, 0, 2, "Icon is self explanatory", "record 2 in EF_IMG" },
        { 4, { 0x9E, 0x02, 0x01, 0x01 }, 1, 1, "Icon is not self explanatory", "record 1 in EF_IMG" },
        { 4, { 0x9E, 0x02, 0x01, 0x02 }, 1, 2, "Icon is not self explanatory", "record 2 in EF_IMG" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        IconIdentifier iconIdentifier[numOfConstructor] = { IconIdentifier(), IconIdentifier(test.data, test.len) };
        iconIdentifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(iconIdentifier[i].GetIconQualifier(), test.iconQualifier);
            EXPECT_EQ(iconIdentifier[i].GetIconIdentifier(), test.iconIdentifier);
            EXPECT_STREQ(iconIdentifier[i].GetIconQualifierString(), test.iconQualifierStr);
            EXPECT_STREQ(iconIdentifier[i].GetIconIdentifierString(), test.iconIdentifierStr);
        }
    }
}

TEST(TlvParser, TextString) {
    struct {
        int len;
        BYTE data[59];
        int codingScheme;
        const char* codingSchemeStr;
        int textStringLen;
        const char *textString;
    } testVector[] = {
        { 28, { 0x8D, 0x1A, 0x00, 0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 0xF7, 0x19,
                  0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99, 0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A },
            0x00, "GSM7BITS", 28, "USSD string received from SS" },
        { 31, { 0x8D, 0x1D, 0x04, 0x55, 0x53, 0x53, 0x44, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67,
                  0x20, 0x72, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x64, 0x20, 0x66, 0x72, 0x6F, 0x6D, 0x20, 0x53, 0x53 },
            0x04, "GSM8BITS", 28, "USSD string received from SS" },
        { 59, { 0x8D, 0x39, 0x08, 0x00, 0x55, 0x00, 0x53, 0x00, 0x53, 0x00, 0x44, 0x00, 0x20, 0x00,
                  0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20,
                  0x00, 0x72, 0x00, 0x65, 0x00, 0x63, 0x00, 0x65, 0x00, 0x69, 0x00, 0x76, 0x00,
                  0x65, 0x00, 0x64, 0x00, 0x20, 0x00, 0x66, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x6D,
                  0x00, 0x20, 0x00, 0x53, 0x00, 0x53 }, 0x08, "UCS2", 28, "USSD string received from SS" }
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        TextString textString[numOfConstructor] = { TextString(), TextString(test.data, test.len) };
        textString[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(textString[i].GetCodingScheme(), test.codingScheme);
            EXPECT_STREQ(textString[i].GetCodingString(), test.codingSchemeStr);

            EXPECT_EQ(textString[i].GetTextStringLength(), test.textStringLen);
            EXPECT_STREQ((char *) textString[i].GetTextString(), test.textString);
        }
    }
}

TEST(TlvParser, ChannelData) {
    struct {
        int len;
        BYTE data[12];
        const char* channelDataStr;
    } testVector[] = {
        { 10, { 0xB6, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }, "0001020304050607"  },
        { 12, { 0xB6, 0x0A, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 }, "00010203040506070809" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ChannelData channelData[numOfConstructor] = { ChannelData(), ChannelData(test.data, test.len) };
        channelData[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_STREQ(channelData[i].GetChannelDataString().c_str(), test.channelDataStr);
        }
     }
}

TEST(TlvParser, ChannelDataLength) {
    struct {
        int len;
        BYTE data[3];
    } testVector[] = {
        { 3, { 0xB7, 0x01, 0xFF }, },
        { 3, { 0xB7, 0x01, 0x10 }, },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ChannelDataLength channelDataLength[numOfConstructor] = { ChannelDataLength(), ChannelDataLength(test.data, test.len) };
        channelDataLength[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(channelDataLength[i].GetChannelDataLength(), test.data[2]);
        }
    }
}

TEST(TlvParser, BufferSize) {
    struct {
        int len;
        BYTE data[4];
        int bufferSize;
    } testVector[] = {
        { 4, { 0x39, 0x02, 0x03, 0xE8 }, 1000 },
        { 4, { 0x39, 0x02, 0x05, 0x78 }, 1400 },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        BufferSize bufferSize[numOfConstructor] = { BufferSize(), BufferSize(test.data, test.len) };
        bufferSize[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(bufferSize[i].GetBufferSize(), test.bufferSize);
        }
    }
}

TEST(TlvParser, TerminalInterfaceTransportLevel) {
    struct {
        int len;
        BYTE data[5];
        int transportType;
        int port;
        const char* transportTypeStr;
    } testVector[] = {
        { 5, { 0x3C, 0x03, 0x03, 0x0D, 0xBC }, 0x03, 3516, "TCP_SERVER" },
        { 5, { 0x3c, 0x03, 0x01, 0xAD, 0x9C }, 0x01, 44444, "UDP_CLIENT_REMOTE" },
        { 5, { 0x3c, 0x03, 0x02, 0xAD, 0x9C }, 0x02, 44444, "TCP_CLIENT_REMOTE" },
        { 5, { 0x3c, 0x03, 0x04, 0xAD, 0x9C }, 0x04, 44444, "UDP_CLIENT_LOCAL" },
        { 5, { 0x3C, 0x03, 0x05, 0x0D, 0xBC }, 0x05, 3516, "TCP_CLIENT_LOCAL" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        TerminalInterfaceTransportLevel terminalInterfaceTransportLevel[numOfConstructor] = {
            TerminalInterfaceTransportLevel(), TerminalInterfaceTransportLevel(test.data, test.len) };
        terminalInterfaceTransportLevel[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(terminalInterfaceTransportLevel[i].GetTransportType(), test.transportType);
            EXPECT_EQ(terminalInterfaceTransportLevel[i].GetPort(), test.port);
            EXPECT_STREQ(terminalInterfaceTransportLevel[i].GetTransportTypeString(),
                    test.transportTypeStr);
        }
    }
}

TEST(TlvParser, NetworkAccessName) {
    struct {
        int len;
        BYTE data[12];
        const char* networkAccessName;
    } testVector[] = {
        { 12, { 0x47, 0x0A, 0x06, 0x54, 0x65, 0x73, 0x74, 0x47, 0x70, 0x02, 0x72, 0x73 }, "TestGp.rs" },
        { 12, { 0x47, 0x0A, 0x06, 0x54, 0x65, 0x73, 0x74, 0x31, 0x32, 0x02, 0x72, 0x73 }, "Test12.rs" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        NetworkAccessName networkAccessName[numOfConstructor] = { NetworkAccessName(),
            NetworkAccessName(test.data, test.len) };
        networkAccessName[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_STREQ(networkAccessName[i].GetNetworkAccessName().c_str(), test.networkAccessName);
        }
    }
}

TEST(TlvParser, MeasurementQualifier) {
    struct {
        int len;
        BYTE data[3];
        const char* qualfierString;
    } testVector[] = {
        { 3, { 0x69, 0x01, 0x01 }, "UTRAN_INTRA_FREQUENCY" },
        { 3, { 0x69, 0x01, 0x02 }, "UTRAN_INTER_FREQUENCY" },
        { 3, { 0x69, 0x01, 0x03 }, "UTRAN_INTER_RAT_GERAN" },
        { 3, { 0x69, 0x01, 0x04 }, "UTRAN_INTER_RAT_EUTRAN" },
        { 3, { 0x69, 0x01, 0x05 }, "EUTRAN_INTRA_FREQUENCY" },
        { 3, { 0x69, 0x01, 0x06 }, "EUTRAN_INTER_FREQUENCY" },
        { 3, { 0x69, 0x01, 0x07 }, "EUTRAN_INTER_RAT_GERAN" },
        { 3, { 0x69, 0x01, 0x08 }, "EUTRAN_INTER_RAT_UTRAN" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        MeasurementQualifier measurementQualifier[numOfConstructor] = { MeasurementQualifier(),
            MeasurementQualifier(test.data, test.len) };
        measurementQualifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(measurementQualifier[i].GetQualifier(), test.data[2]);
            EXPECT_STREQ(measurementQualifier[i].GetQualifierString(),
                    test.qualfierString);
        }
    }
}

TEST(TlvParser, TextAttribute) {
    struct {
        int len;
        BYTE data[6];
    } testVector[] = {
        { 6, { 0xD0, 0x04, 0x00, 0x0B, 0x00, 0xB4 } },
        { 6, { 0xD0, 0x04, 0x00, 0x10, 0x00, 0xB4 } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        TextAttribute textAttribute[numOfConstructor] = { TextAttribute(), TextAttribute(test.data, test.len) };
        textAttribute[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(textAttribute[i].GetTextFormattingPosition(), test.data[2]);
            EXPECT_EQ(textAttribute[i].GetTextFormattingLength(), test.data[3]);
        }
    }
}

TEST(TlvParser, FrameIdentifier) {
    struct {
        int len;
        BYTE data[3];
    } testVector[] = {
        { 3, { 0x68, 0x01, 0x01 } },
        { 3, { 0x68, 0x01, 0x02 } },
        { 3, { 0x68, 0x01, 0x10 } },
        { 3, { 0x68, 0x01, 0xA0 } },
        { 3, { 0x68, 0x01, 0xFF } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        FrameIdentifier frameIdentifier[numOfConstructor] = { FrameIdentifier(), FrameIdentifier(test.data, test.len) };
        frameIdentifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(frameIdentifier[i].GetIdentifierOfFrame(), test.data[2]);
        }
    }
}

TEST(TlvParser, EventList) {
    struct {
        int len;
        BYTE data[8];
        int eventListLen;
        int events[6];
        std::string eventStr[6];
    } testVector[] = {
        { 3, { 0x99, 0x01, 0x00 }, 1, { 0x00 }, { "MT_CALL" } },
        { 4, { 0x99, 0x02, 0x01, 0x02 }, 2, { 0x01, 0x02 }, { "CALL_CONNECTED", "CALL_DISCONNECTED" }, },
        { 4, { 0x99, 0x02, 0x03, 0x04 }, 2, { 0x03, 0x04 }, { "LOCATION_STATUS", "USER_ACTIVITY"}, },
        { 3, { 0x99, 0x01, 0x05 }, 1, { 0x05 }, { "IDLE_SCREEN_AVAILABLE" } },
        { 3, { 0x99, 0x01, 0x06 }, 1, { 0x06 }, { "CARD_READER_STATUS" } },
        { 3, { 0x99, 0x01, 0x07 }, 1, { 0x07 }, { "LANGUAGE_SELECTION" } },
        { 3, { 0x99, 0x01, 0x08 }, 1, { 0x08 }, { "BROWSER_TERMINATION" } },
        { 4, { 0x99, 0x02, 0x09, 0x0A }, 2, { 0x09, 0x0A }, { "DATA_AVAILABLE_EVENT", "CHANNEL_STATUS_EVENT" } },
        { 8, { 0x99, 0x06, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 }, 6, { 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 },
            { "SINGLE_ACCESS_TECHNOLOGY_CHANGE", "DISPLAY_PARAMETERS_CHANGED", "LOCAL_CONNECTION",
            "NETWORK_SEARCH_MODE_CHANGE", "BROWSING_STATUS", "FRAMES_INFORMATION_CHANGE" } },
        { 3, { 0x99, 0x01, 0x11 }, 1, { 0x11 }, { "IWLAN_ACCESS_STATUS" } },
        { 7, { 0x99, 0x05, 0x12, 0x13, 0x14, 0x15, 0x16 }, 5, { 0x12, 0x13, 0x14, 0x15, 0x16 },
            { "NETWORK_REJECTION", "HCI_CONNECTIVITY_EVENT", "MULTIPLE_ACCESS_TECHNOLOGY_CHANGE",
            "CSG_CELL_SELECTION","CONTACTLESS_STATE_REQUEST" } },
        { 3, { 0x99, 0x01, 0x17 }, 1, { 0x17 }, { "IMS_REGISTRATION" } },
        { 3, { 0x99, 0x01, 0x1C }, 1, { 0x1C }, { "POLL_INTERVAL" } },
        { 3, { 0x99, 0x01, 0xFF }, 1, { 0xFF }, { "UNKNOWN_EVENT" } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        EventList eventList[numOfConstructor] = { EventList(), EventList(test.data, test.len) };
        eventList[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(eventList[i].GetEventListLength(), test.eventListLen);
            for (int j = 0; j < eventList[i].GetEventListLength(); j++) {
                EXPECT_EQ(eventList[i].GetEvent(j), test.events[j]);
                EXPECT_STREQ(eventList[i].GetEventString(j), test.eventStr[j].c_str());
            }
        }
    }
}

TEST(TlvParser, BrowserTerminationCause) {
    struct {
        int len;
        BYTE data[3];
        int cause;
        const char* causeStr;
    } testVector[] = {
        { 3, { 0x34, 0x01, 0x00 }, 0, "USER_TERMINATION" },
        { 3, { 0x34, 0x01, 0x01 }, 1, "ERROR_TERMINATION" },
        { 3, { 0x34, 0x01, 0x02 }, 2, "Unknown_Cause" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        BrowserTerminationCause browserTerminationCause[numOfConstructor] = { BrowserTerminationCause(),
            BrowserTerminationCause(test.data, test.len) };
        browserTerminationCause[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(browserTerminationCause[i].GetCause(), test.data[2]);
            EXPECT_STREQ(browserTerminationCause[i].GetCauseString(), test.causeStr);
        }
    }
}

TEST(TlvParser, Url) {
    struct {
        int len;
        BYTE data[20];
        const char* url;
    } testVector[] = {
        { 2, { 0x34, 0x00 }, "" },
        { 20, { 0x34, 0x12, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x78, 0x78, 0x78, 0x2E, 0x79, 0x79, 0x79, 0x2E,
                 0x7A, 0x7A, 0x7A }, "http://xxx.yyy.zzz" },
        { 17, { 0x34, 0x0F, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x61, 0x62, 0x63, 0x64, 0x2E, 0x63, 0x6F, 0x6D },
            "http://abcd.com" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        Url url[numOfConstructor] = { Url(), Url(test.data, test.len) };
        url[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_STREQ(url[i].GetUrl().c_str(), test.url);
        }
    }
}

TEST(TlvParser, Language) {
    struct {
        int len;
        BYTE data[9];
        const char* language;
    } testVector[] = {
        { 2, { 0x2D, 0x00 }, "" },
        { 9, { 0x2D, 0x07, 0x45, 0x6E, 0x67, 0x6c, 0x69, 0x73, 0x68 }, "English" },
        { 9, { 0x2D, 0x07, 0x53, 0x70, 0x61, 0x6E, 0x69, 0x73, 0x68 }, "Spanish" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        Language language[numOfConstructor] = { Language(), Language(test.data, test.len) };
        language[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_STREQ(language[i].GetLanguage().c_str(), test.language);
        }
    }
}

TEST(TlvParser, ResponseLength) {
    struct {
        int len;
        BYTE data[4];
    } testVector[] = {
        { 4, { 0x34, 0x02, 0x01, 0x15 } },
        { 4, { 0x34, 0x02, 0x10, 0x99 } },
        { 4, { 0x34, 0x02, 0x02, 0x05 } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ResponseLength responseLength[numOfConstructor] = { ResponseLength(), ResponseLength(test.data, test.len) };
        responseLength[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(responseLength[i].GetMinimum(), test.data[2]);
            EXPECT_EQ(responseLength[i].GetMaximum(), test.data[3]);
        }
    }
}

TEST(TlvParser, Tone) {
    struct {
        int len;
        BYTE data[3];
        const char* toneStr;
    } testVector[] = {
        { 3, { 0x0E, 0x01, 0x01 }, "DIAL_TONE" },
        { 3, { 0x0E, 0x01, 0x02 }, "CALLED_SUBSCRIBER_BUSY" },
        { 3, { 0x0E, 0x01, 0x03 }, "CONGESTION" },
        { 3, { 0x0E, 0x01, 0x04 }, "RADIO_PATH_ACKNOWLEDGE" },
        { 3, { 0x0E, 0x01, 0x05 }, "RADIO_PATH_NOT_AVAILABLE" },
        { 3, { 0x0E, 0x01, 0x06 }, "ERROR" },
        { 3, { 0x0E, 0x01, 0x07 }, "CALL_WAITING_TONE" },
        { 3, { 0x0E, 0x01, 0x08 }, "RINGING_TONE" },
        { 3, { 0x0E, 0x01, 0xFF }, "UNKNOWN_TONE" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        Tone tone[numOfConstructor] = { Tone(), Tone(test.data, test.len) };
        tone[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(tone[i].GetTone(), test.data[2]);
            EXPECT_STREQ(tone[i].GetToneString(), test.toneStr);
        }
    }
}

TEST(TlvParser, DefaultText) {
    struct {
        int len;
        BYTE data[59];
        int codingScheme;
        const char* codingSchemeStr;
        int textStringLen;
        const char *textString;
    } testVector[] = {
        { 28, { 0x17, 0x1A, 0x00, 0xD5, 0xE9, 0x94, 0x08, 0x9A, 0xD3, 0xE5, 0x69, 0xF7, 0x19,
                  0x24, 0x2F, 0x8F, 0xCB, 0x69, 0x7B, 0x99, 0x0C, 0x32, 0xCB, 0xDF, 0x6D, 0xD0, 0x74, 0x0A },
        0x00, "GSM7BITS", 28, "USSD string received from SS" },
        { 31, { 0x17, 0x1D, 0x04, 0x55, 0x53, 0x53, 0x44, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6E, 0x67,
                  0x20, 0x72, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x64, 0x20, 0x66, 0x72, 0x6F, 0x6D, 0x20, 0x53, 0x53 },
        0x04, "GSM8BITS", 28, "USSD string received from SS" },
        { 59, { 0x17, 0x39, 0x08, 0x00, 0x55, 0x00, 0x53, 0x00, 0x53, 0x00, 0x44, 0x00, 0x20, 0x00,
                  0x73, 0x00, 0x74, 0x00, 0x72, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20,
                  0x00, 0x72, 0x00, 0x65, 0x00, 0x63, 0x00, 0x65, 0x00, 0x69, 0x00, 0x76, 0x00,
                  0x65, 0x00, 0x64, 0x00, 0x20, 0x00, 0x66, 0x00, 0x72, 0x00, 0x6F, 0x00, 0x6D,
                  0x00, 0x20, 0x00, 0x53, 0x00, 0x53 }, 0x08, "UCS2", 28, "USSD string received from SS" }
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        DefaultText defaultText[numOfConstructor] = { DefaultText(), DefaultText(test.data, test.len) };
        defaultText[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(defaultText[i].GetCodingScheme(), test.codingScheme);
            EXPECT_STREQ(defaultText[i].GetCodingString(), test.codingSchemeStr);

            EXPECT_EQ(defaultText[i].GetDefaultTextLength(), test.textStringLen);
            EXPECT_STREQ((char *) defaultText[i].GetDefaultText(), test.textString);
        }
    }
}

TEST(TlvParser, ImmediateResponse) {
    struct {
        int len;
        BYTE data[2];
    } testVector[] = {
        { 2, { 0x2B, 0x00 } },
        { 2, { 0x2B, 0x00 } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ImmediateResponse immediateResponse[numOfConstructor] = { ImmediateResponse(),
            ImmediateResponse(test.data, test.len) };
        immediateResponse[0].Set(test.data, test.len);
    }
}

TEST(TlvParser, HelpRequest) {
    struct {
        int len;
        BYTE data[2];
    } testVector[] = {
        { 2, { 0x15, 0x00 } },
        { 2, { 0x15, 0x00 } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        HelpRequest helpRequest[numOfConstructor] = { HelpRequest(), HelpRequest(test.data, test.len) };
        helpRequest[0].Set(test.data, test.len);
    }
}

TEST(TlvParser, TimerIdentifier) {
    struct {
        int len;
        BYTE data[3];
        const char* timerStr;
    } testVector[] = {
        { 3, { 0x0F, 0x01, 0x01 }, "TIMER_1" },
        { 3, { 0x0F, 0x01, 0x02 }, "TIMER_2" },
        { 3, { 0x0F, 0x01, 0x03 }, "TIMER_3" },
        { 3, { 0x0F, 0x01, 0x04 }, "TIMER_4" },
        { 3, { 0x0F, 0x01, 0x05 }, "TIMER_5" },
        { 3, { 0x0F, 0x01, 0x06 }, "TIMER_6" },
        { 3, { 0x0F, 0x01, 0x07 }, "TIMER_7" },
        { 3, { 0x0F, 0x01, 0x08 }, "TIMER_8" },
        { 3, { 0x0F, 0x01, 0xFF }, "Unknown_Timer_Value" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        TimerIdentifier timerIdentifier[numOfConstructor] = { TimerIdentifier(),
            TimerIdentifier(test.data, test.len) };
        timerIdentifier[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(timerIdentifier[i].GetTimerIdentifier(), test.data[2]);
            EXPECT_STREQ(timerIdentifier[i].GetTimerString(), test.timerStr);
        }
    }
}

TEST(TlvParser, DTMFString) {
    struct {
        int len;
        BYTE data[7];
        const char* dtmfStr;
    } testVector[] = {
        { 7, { 0xAC, 0x05, 0x21, 0x43, 0x65, 0x87, 0x09 }, "1234567890" },
        { 4, { 0xAC, 0x02, 0x87, 0x09 }, "7890" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        DTMFString dtmfString[numOfConstructor] = { DTMFString(), DTMFString(test.data, test.len) };
        dtmfString[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_STREQ(dtmfString[i].GetDTMFIdentifier().c_str(), test.dtmfStr);
        }
    }
}

TEST(TlvParser, Subaddress) {
    struct {
        int len;
        BYTE data[10];
    } testVector[] = {
        { 8, { 0x08, 0x06, 0x05, 0x91, 0x56, 0x78, 0x90, 0x11 } },
        { 10, { 0x08, 0x08, 0x07, 0x91, 0x12, 0x56, 0x87, 0x82, 0x19, 0x18 } },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        Subaddress subaddress[numOfConstructor] = { Subaddress(), Subaddress(test.data, test.len) };
        subaddress[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(subaddress[i].GetAddressLength(), test.data[2]);

            BYTE *address = subaddress[i].GetAddress();
            for (int j = 0; j < subaddress[i].GetAddressLength(); j++) {
                EXPECT_EQ(address[j], test.data[j+3]);
            }
        }
    }
}

TEST(TlvParser, BearerDescription) {
    struct {
        int len;
        BYTE data[9];
        int type;
        const char* typeStr;
        const char* paramStr;
        int beaererLength;
    } testVector[] = {
        { 9, { 0x35, 0x07, 0x01, 0x03, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x01, "CSD", "030403041F02", 4 },
        { 9, { 0x35, 0x07, 0x02, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x02, "GPRS", "000403041F02", 7 },
        { 9, { 0x35, 0x07, 0x03, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x03, "DEFAULT_BEARER", "000403041F02", 1 },
        { 9, { 0x35, 0x07, 0x04, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x04, "LOCAL_LINK", "000403041F02", 1 },
        { 9, { 0x35, 0x07, 0x05, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x05, "BLUETOOTH", "000403041F02", 1 },
        { 9, { 0x35, 0x07, 0x06, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x06, "IRDA", "000403041F02", 1 },
        { 9, { 0x35, 0x07, 0x07, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x07, "RS232", "000403041F02", 1 },
        { 9, { 0x35, 0x07, 0x08, 0x00, 0x04, 0x03, 0x04, 0x1F, 0x02 }, 0x08, "CDMA2000", "000403041F02", 0 },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        BearerDescription bearerDescription[numOfConstructor] = { BearerDescription(),
            BearerDescription(test.data, test.len) };
        bearerDescription[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(bearerDescription[i].GetType(), test.type);
            EXPECT_STREQ(bearerDescription[i].GetTypeString().c_str(), test.typeStr);
            BYTE *param = bearerDescription[i].GetParameter();
            for (int j = 0; j < test.len-3; j++) {
                EXPECT_EQ(test.data[j+3], param[j]);
            }
            EXPECT_STREQ(bearerDescription[i].GetParamString().c_str(), test.paramStr);
            EXPECT_EQ(bearerDescription[i].GetBearerLength(test.type), test.beaererLength);
        }
    }
}

TEST(TlvParser, ChannelStatus) {
    struct {
        int len;
        BYTE data[4];
        int channelId;
        int serviceEstablished;
        int tcpStatus;
        int furtherInfo;
    } testVector[] = {
        { 4, { 0x38, 0x02, 0x81, 0x00 }, 0x01, 0x80, 0x80, 0x00 },
        { 4, { 0x38, 0x02, 0x00, 0x00 }, 0x00, 0x00, 0x00, 0x00 },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        ChannelStatus channelStatus[numOfConstructor] = { ChannelStatus(), ChannelStatus(test.data, test.len) };
        channelStatus[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(channelStatus[i].GetChannelId(), test.channelId);
            EXPECT_EQ(channelStatus[i].IsServiceEstablished(), test.serviceEstablished);
            EXPECT_EQ(channelStatus[i].GetTcpStatus(), test.tcpStatus);
            EXPECT_EQ(channelStatus[i].GetFurtherInfo(), test.furtherInfo);
        }
    }
}

TEST(TlvParser, CommandDetail_Refresh) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x00 }, 0x01, 0x01, 0x00, "REFRESH", "UICC Initialization and Full File Change Notification" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x01 }, 0x01, 0x01, 0x01, "REFRESH", "File Change Notification" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x02 }, 0x01, 0x01, 0x02, "REFRESH", "UICC Initialization and File Change Notification" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x03 }, 0x01, 0x01, 0x03, "REFRESH", "UICC Initialization" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x04 }, 0x01, 0x01, 0x04, "REFRESH", "UICC Reset" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x05 }, 0x01, 0x01, 0x05, "REFRESH", "UICC Application Reset, only applicable for a 3G platform" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x06 }, 0x01, 0x01, 0x06, "REFRESH", "UICC Session Reset, only applicable for a 3G platform" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x07 }, 0x01, 0x01, 0x07, "REFRESH", "Reserved by 3GPP('Steering of Roaming' REFRESH support)" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x08 }, 0x01, 0x01, 0x08, "REFRESH", "Reserved by 3GPP(Steering of Roaming for I-WLAN)" },
        { 5, { 0x81, 0x03, 0x01, 0x01, 0x09 }, 0x01, 0x01, 0x09, "REFRESH", "eUICC Profile State Change" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_SetUpCall) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x10, 0x00 }, 0x01, 0x10, 0x00, "SET_UP_CALL", "set up call, but only if not currently busy on another call" },
        { 5, { 0x81, 0x03, 0x01, 0x10, 0x01 }, 0x01, 0x10, 0x01, "SET_UP_CALL", "set up call, but only if not currently busy on another call, with redial" },
        { 5, { 0x81, 0x03, 0x01, 0x10, 0x02 }, 0x01, 0x10, 0x02, "SET_UP_CALL", "set up call, putting all other calls(if any) on hold" },
        { 5, { 0x81, 0x03, 0x01, 0x10, 0x03 }, 0x01, 0x10, 0x03, "SET_UP_CALL", "set up call, putting all other calls(if any) on hold, with redial" },
        { 5, { 0x81, 0x03, 0x01, 0x10, 0x04 }, 0x01, 0x10, 0x04, "SET_UP_CALL", "set up call, disconnecting all other calls(if any)" },
        { 5, { 0x81, 0x03, 0x01, 0x10, 0x05 }, 0x01, 0x10, 0x05, "SET_UP_CALL", "set up call, disconnecting all other calls(if any), with redial" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_SendSms) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x13, 0x00 }, 0x01, 0x13, 0x00, "SEND_SMS", "packing not required" },
        { 5, { 0x81, 0x03, 0x01, 0x13, 0x01 }, 0x01, 0x13, 0x01, "SEND_SMS", "SMS packing by the terminal required" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_PlayTone) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x20, 0x00 }, 0x01, 0x20, 0x00, "PLAY_TONE", "use of vibrate alert is up to the terminal" },
        { 5, { 0x81, 0x03, 0x01, 0x20, 0x01 }, 0x01, 0x20, 0x01, "PLAY_TONE", "vibrate alert, if available, with the tone" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_DisplayText) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x21, 0x08 }, 0x01, 0x21, 0x08, "DISPLAY_TEXT", "normal priority, clear message after a delay" },
        { 5, { 0x81, 0x03, 0x01, 0x21, 0x80 }, 0x01, 0x21, 0x80, "DISPLAY_TEXT", "normal priority, wait for user to clear message" },
        { 5, { 0x81, 0x03, 0x01, 0x21, 0x07 }, 0x01, 0x21, 0x07, "DISPLAY_TEXT", "high priority, clear message after a delay" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_GetInKey) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x22, 0x00 }, 0x01, 0x22, 0x00, "GET_INKEY",
            "digits (0 to 9, *, # and +) only, SMS default alphabet, character sets defined by bit 1 and bit 2 are enabled, user response shall be displayed. The terminal may allow alteration and/or confirmation, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x22, 0x7F }, 0x01, 0x22, 0x7F, "GET_INKEY", "alphabet set, UCS2 alphabet, character sets defined by bit 1 and bit 2 are disabled and the Yes/No response is requested, an immediate digit response (0 to 9, * and #) is requested, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x22, 0x80 }, 0x01, 0x22, 0x80, "GET_INKEY",
            "digits (0 to 9, *, # and +) only, SMS default alphabet, character sets defined by bit 1 and bit 2 are enabled, user response shall be displayed. The terminal may allow alteration and/or confirmation, help information available" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_GetInput) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x23, 0x00 }, 0x01, 0x23, 0x00, "GET_INPUT", "digits (0 to 9, *, # and +) only, SMS default alphabet, terminal may echo user input on the display, user input to be in unpacked format, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x23, 0x7F }, 0x01, 0x23, 0x7F, "GET_INPUT", "alphabet set, UCS2 alphabet, user input shall not be revealed in any way, user input to be in SMS packed format, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x23, 0x80 }, 0x01, 0x23, 0x80, "GET_INPUT", "digits (0 to 9, *, # and +) only, SMS default alphabet, terminal may echo user input on the display, user input to be in unpacked format, help information available" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_SelectItem) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x24, 0x00 }, 0x01, 0x24, 0x00, "SELECT_ITEM", "presentation type is not specified, presentation as a choice of data values if bit 1 is 1, no selection preference, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x24, 0x7F }, 0x01, 0x24, 0x7F, "SELECT_ITEM", "presentation type is specified in bit 2, presentation as choice of navigation options if bit 1 is 1, selection using soft key preferred, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x24, 0x80 }, 0x01, 0x24, 0x80, "SELECT_ITEM", "presentation type is not specified, presentation as a choice of data values if bit 1 is 1, no selection preference, help information available" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_SetupMenu) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x25, 0x08 }, 0x01, 0x25, 0x08, "SET_UP_MENU", "no selection preference, no help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x25, 0x80 }, 0x01, 0x25, 0x80, "SET_UP_MENU", "no selection preference, help information available" },
        { 5, { 0x81, 0x03, 0x01, 0x25, 0x07 }, 0x01, 0x25, 0x07, "SET_UP_MENU", "selection using soft key preferred, no help information available" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_ProvideLocalInformation) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x00 }, 0x01, 0x26, 0x00, "PROVIDE_LOCAL_INFORMATION", "Location Inforamtion according to current NAA" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x01 }, 0x01, 0x26, 0x01, "PROVIDE_LOCAL_INFORMATION", "IMEI of the terminal" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x02 }, 0x01, 0x26, 0x02, "PROVIDE_LOCAL_INFORMATION", "Network Measurment Results according to current NAA" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x03 }, 0x01, 0x26, 0x03, "PROVIDE_LOCAL_INFORMATION", "Date, Time and Time zone" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x04 }, 0x01, 0x26, 0x04, "PROVIDE_LOCAL_INFORMATION", "Language Setting" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x05 }, 0x01, 0x26, 0x05, "PROVIDE_LOCAL_INFORMATION", "Reserved for GSM" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x06 }, 0x01, 0x26, 0x06, "PROVIDE_LOCAL_INFORMATION", "Access Technology (Single Access Technology)" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x07 }, 0x01, 0x26, 0x07, "PROVIDE_LOCAL_INFORMATION", "ESN of the terminal" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x08 }, 0x01, 0x26, 0x08, "PROVIDE_LOCAL_INFORMATION", "IMEISV of the terminal" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x09 }, 0x01, 0x26, 0x09, "PROVIDE_LOCAL_INFORMATION", "Search Mode" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x0A }, 0x01, 0x26, 0x0A, "PROVIDE_LOCAL_INFORMATION", "Charge state of the battery" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x0B }, 0x01, 0x26, 0x0B, "PROVIDE_LOCAL_INFORMATION", "MEID of the terminal" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x0C }, 0x01, 0x26, 0x0C, "PROVIDE_LOCAL_INFORMATION", "reserved for 3GPP" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x0D }, 0x01, 0x26, 0x0D, "PROVIDE_LOCAL_INFORMATION", "Broadcast network information according to currnt broadcast network technology used" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x0E }, 0x01, 0x26, 0x0E, "PROVIDE_LOCAL_INFORMATION", "Multiple Access Technology" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x0F }, 0x01, 0x26, 0x0F, "PROVIDE_LOCAL_INFORMATION", "Local information for mutiple access technologies" },
        { 5, { 0x81, 0x03, 0x01, 0x26, 0x10 }, 0x01, 0x26, 0x10, "PROVIDE_LOCAL_INFORMATION", "Network Measurment results for multiple access technologies" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_GetReaderStatus) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x32, 0x00 }, 0x01, 0x32, 0x00, "GET_READER_STATUS", "Card Reader Status" },
        { 5, { 0x81, 0x03, 0x01, 0x32, 0x01 }, 0x01, 0x32, 0x01, "GET_READER_STATUS", "Card Reader Identifier" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_TimerManagement) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x27, 0x00 }, 0x01, 0x27, 0x00, "TIMER_MANAGEMENT", "start the timer" },
        { 5, { 0x81, 0x03, 0x01, 0x27, 0x01 }, 0x01, 0x27, 0x01, "TIMER_MANAGEMENT", "deactivate the timer" },
        { 5, { 0x81, 0x03, 0x01, 0x27, 0x02 }, 0x01, 0x27, 0x02, "TIMER_MANAGEMENT", "get current value of timer" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_LanguageNotification) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x34, 0x00 }, 0x01, 0x34, 0x00, "LANGUAGE_NOTIFICATION", "non-specific language notification" },
        { 5, { 0x81, 0x03, 0x01, 0x34, 0x01 }, 0x01, 0x34, 0x01, "LANGUAGE_NOTIFICATION", "specific language notification" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail_LaunchBrowser) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
        const char* qualifierStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x15, 0x00 }, 0x01, 0x15, 0x00, "LAUNCH_BROWSER", "launch browser if not already launched" },
        { 5, { 0x81, 0x03, 0x01, 0x15, 0x01 }, 0x01, 0x15, 0x01, "LAUNCH_BROWSER", "not used" },
        { 5, { 0x81, 0x03, 0x01, 0x15, 0x02 }, 0x01, 0x15, 0x02, "LAUNCH_BROWSER", "use the existing browser (the browser shall not use the active existing securd session)" },
        { 5, { 0x81, 0x03, 0x01, 0x15, 0x03 }, 0x01, 0x15, 0x03, "LAUNCH_BROWSER", "close the existing browser session and launch new browser session" },
        { 5, { 0x81, 0x03, 0x01, 0x15, 0x04 }, 0x01, 0x15, 0x04, "LAUNCH_BROWSER", "not used" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
            EXPECT_STREQ(commandDetail[i].GetQualifierString().c_str(), test.qualifierStr);
        }
    }
}

TEST(TlvParser, CommandDetail) {
    struct {
        int len;
        BYTE data[5];
        int comamndNumber;
        int commandType;
        int qualifier;
        const char* commandTypeStr;
    } testVector[] = {
        { 5, { 0x81, 0x03, 0x01, 0x02, 0x00 }, 0x01, 0x02, 0x00, "MORE_TIME" },
        { 5, { 0x81, 0x03, 0x01, 0x03, 0x02 }, 0x01, 0x03, 0x02, "POLL_INTERVAL" },
        { 5, { 0x81, 0x03, 0x01, 0x04, 0x03 }, 0x01, 0x04, 0x03, "POLLING_OFF" },
        { 5, { 0x81, 0x03, 0x01, 0x05, 0x03 }, 0x01, 0x05, 0x03, "SET_UP_EVENT_LIST" },
        { 5, { 0x81, 0x03, 0x01, 0x11, 0x04 }, 0x01, 0x11, 0x04, "SEND_SS" },
        { 5, { 0x81, 0x03, 0x01, 0x12, 0x04 }, 0x01, 0x12, 0x04, "SEND_USSD" },
        { 5, { 0x81, 0x03, 0x01, 0x14, 0x04 }, 0x01, 0x14, 0x04, "SEND_DTMF" },
        { 5, { 0x81, 0x03, 0x01, 0x16, 0x04 }, 0x01, 0x16, 0x04, "GEOGRAPHICAL_LOCATION_REQUEST" },
        { 5, { 0x81, 0x03, 0x01, 0x28, 0x04 }, 0x01, 0x28, 0x04, "SET_UP_IDLE_MODE_TEXT" },
        { 5, { 0x81, 0x03, 0x01, 0x30, 0x04 }, 0x01, 0x30, 0x04, "PERFORM_CARD_APDU" },
        { 5, { 0x81, 0x03, 0x01, 0x31, 0x04 }, 0x01, 0x31, 0x04, "POWER_ON_CARD" },
        { 5, { 0x81, 0x03, 0x01, 0x33, 0x04 }, 0x01, 0x33, 0x04, "RUN_AT_COMMAND" },
        { 5, { 0x81, 0x03, 0x01, 0x40, 0x04 }, 0x01, 0x40, 0x04, "OPEN_CHANNEL" },
        { 5, { 0x81, 0x03, 0x01, 0x41, 0x04 }, 0x01, 0x41, 0x04, "CLOSE_CHANNEL" },
        { 5, { 0x81, 0x03, 0x01, 0x42, 0x04 }, 0x01, 0x42, 0x04, "RECEIVE_DATA" },
        { 5, { 0x81, 0x03, 0x01, 0x43, 0x04 }, 0x01, 0x43, 0x04, "SEND_DATA" },
        { 5, { 0x81, 0x03, 0x01, 0x44, 0x04 }, 0x01, 0x44, 0x04, "GET_CHANNEL_STATUS" },
        { 5, { 0x81, 0x03, 0x01, 0x45, 0x04 }, 0x01, 0x45, 0x04, "SERVICE_SEARCH" },
        { 5, { 0x81, 0x03, 0x01, 0x46, 0x04 }, 0x01, 0x46, 0x04, "GET_SERVICE_INFORMATION" },
        { 5, { 0x81, 0x03, 0x01, 0x47, 0x04 }, 0x01, 0x47, 0x04, "DECLARE_SERVICE" },
        { 5, { 0x81, 0x03, 0x01, 0x50, 0x04 }, 0x01, 0x50, 0x04, "SET_FRAMES" },
        { 5, { 0x81, 0x03, 0x01, 0x51, 0x04 }, 0x01, 0x51, 0x04, "GET_FRAMES_STATUS" },
        { 5, { 0x81, 0x03, 0x01, 0x60, 0x04 }, 0x01, 0x60, 0x04, "RETRIVE_MULTIMEDIA_MESSAGE" },
        { 5, { 0x81, 0x03, 0x01, 0x61, 0x04 }, 0x01, 0x61, 0x04, "SUBMIT_MULTIMEDIA_MESSAGE" },
        { 5, { 0x81, 0x03, 0x01, 0x62, 0x04 }, 0x01, 0x62, 0x04, "DISPLAY_MULTIMEDIA_MESSAGE" },
        { 5, { 0x81, 0x03, 0x01, 0x70, 0x04 }, 0x01, 0x70, 0x04, "ACTIVATE" },
        { 5, { 0x81, 0x03, 0x01, 0x71, 0x04 }, 0x01, 0x71, 0x04, "CONTACTLESS_STATE_CHANGED" },
        { 5, { 0x81, 0x03, 0x01, 0x72, 0x04 }, 0x01, 0x72, 0x04, "COMMAND_CONTAINER" },
        { 5, { 0x81, 0x03, 0x01, 0x73, 0x04 }, 0x01, 0x73, 0x04, "ENCAPSULATED_SESSION_CONTROL" },
        { 5, { 0x81, 0x03, 0x01, 0x81, 0x04 }, 0x01, 0x81, 0x04, "END_PROACTIVE_UICC_SESSION" },
    };

    for (auto test : testVector) {
        const int numOfConstructor = 2;

        CommandDetail commandDetail[numOfConstructor] = { CommandDetail(), CommandDetail(test.data, test.len) };
        commandDetail[0].Set(test.data, test.len);

        for (int i = 0; i < numOfConstructor; i++) {
            EXPECT_EQ(commandDetail[i].GetCommandNumber(), test.comamndNumber);
            EXPECT_EQ(commandDetail[i].GetCommandType(), test.commandType);
            EXPECT_EQ(commandDetail[i].GetQualifier(), test.qualifier);
            EXPECT_STREQ(commandDetail[i].GetCommandTypeString(), test.commandTypeStr);
        }
    }
}
