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
#include <gtest/gtest.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

extern "C" {
#include "main.c"
}

/* Socket Number */
#define RX_SOCK_FD 8
#define TX_SOCK_FD 9
#define DUMMY_SOCK_FD 5
#define TX_SOCK_ESP_FD 12

/* Rules */
#define FORWARD_RULE 0x01
#define DROP_RULE 0x02

/* Message Type */
#define MSG_CTRL 0x01
#define MSG_DATA 0x02

/* Command */
#define CMD_ADD 0x01
#define CMD_DELETE 0x02

/* IP address Type */
#define TYPE_IPv4 0x01
#define TYPE_IPv6 0x02

/* Rules */
#define RULE_FORWARD 0x01
#define RULE_DROP 0x02

/* Protocol */
#define PRT_TCP 0x01
#define PRT_UDP 0x02
#define PRT_ESP 0x03

void addRule(unsigned char ip_type, unsigned char ip_addr[], unsigned char rule, unsigned char protocol,
    unsigned char val1[], unsigned char val2[]);

void delRule(unsigned char ip_type, unsigned char ip_addr[], unsigned char rule, unsigned char protocol,
    unsigned char val1[], unsigned char val2[]);

void pkt_command(unsigned char meessage_type, unsigned char command_id, unsigned char ip_type, unsigned char ip_addr[], unsigned char rule, unsigned char protocol,
    unsigned char val1[], unsigned char val2[]);

void sendPacketToModemWithInterface(unsigned char *ip_adrr, int ip_type, unsigned char *WiFi_DataPacketTcp, int packetLen);

void setUp(unsigned char **ipv6_addr, unsigned char **ipv4_addr);
void tearDown(unsigned char **ipv6_addr, unsigned char **ipv4_addr);

/**
 * Verify whether or not the TCP packet(IPv6) was sent to the CP Modem.
 */
TEST(PacketRouterTest, TCP_SendDataToCPModemTest) {
    __DBG("[TEST] TCP_SendDataToCPModemTest");

    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char WiFi_DataPacketTcp[] = {
        0x00, 0x00, 0x00, 0x00, 0x66, 0x80, 0x00, 0x00, 0x00, 0x1c, 0x06, 0x32, 0xfd, 0x00, 0x97, 0x6a,
        0xd5, 0xd0, 0x19, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x13, 0xc4, 0xc0, 0x31,
        0x27, 0x1d, 0x1e, 0x4a, 0xa2, 0x41, 0xe8, 0xf9, 0x70, 0x12, 0x5f, 0x50, 0x39, 0x70, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xc4, 0x01, 0x03, 0x03, 0x0a
    };

    sendPacketToModemWithInterface(ipv6_addr, TYPE_IPv6, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));
    find_ifindex(find_interface(ipv6_addr));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the TCP packet(IPv6) was sent to the WiFi Network.
 */
TEST(PacketRouterTest, TCP_SendDataToWiFiNetworkTest) {
    __DBG("[TEST] TCP_SendDataToWiFiNetworkTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char CP_DataPacketTcp[] = {
        0x02, 0x00, 0x50, 0x00, 0x60, 0x0c, 0x96, 0x1a, 0x00, 0x28, 0x06, 0x40, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x20, 0x01, 0x18, 0x90,
        0x01, 0xf8, 0x22, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0xc0, 0x30, 0x13, 0xc4,
        0x35, 0xaa, 0xf1, 0xf1, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0xff, 0xff, 0xaa, 0x53, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xce, 0x01, 0x03, 0x03, 0x06, 0x04, 0x02, 0x08, 0x0a, 0x00, 0x00, 0x05, 0x51,
        0x00, 0x00, 0x00, 0x00
    };

    process_read_buffer(CP_DataPacketTcp, sizeof(CP_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the TCP packet(IPv4) was sent to the CP Modem.
 */
TEST(PacketRouterTest, TCP_SendDataToCPModemWithIPv4Test) {
    __DBG("[TEST] TCP_SendDataToCPModemWithIPv4Test");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv4, ipv4_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char WiFi_DataPacketTcp[] = {
        0x00, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, 0x00, 0x00, 0x1c, 0x06, 0x32, 0xfd, 0x00, 0x97, 0x6a,
        0xd5, 0xd0, 0x19, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x0B, 0x03, 0x1F,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xc4, 0xc0, 0x31,
        0x27, 0x1d, 0x1e, 0x4a, 0xa2, 0x41, 0xe8, 0xf9, 0x70, 0x12, 0x5f, 0x50, 0x39, 0x70, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xc4, 0x01, 0x03, 0x03, 0x0a
    };

    sendPacketToModemWithInterface(ipv4_addr, TYPE_IPv4, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv4, ipv4_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the TCP packet(IPv4) was sent to the WiFi Network.
 */
TEST(PacketRouterTest, TCP_SendDataToWiFiNetworkWithIPv4Test) {
    __DBG("[TEST] TCP_SendDataToWiFiNetworkWithIPv4Test");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv4, ipv4_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char CP_DataPacketTcp[] = {
        0x02, 0x00, 0x50, 0x00, 0x40, 0x0c, 0x96, 0x1a, 0x00, 0x28, 0x06, 0x40, 0x80, 0x06, 0x03, 0x1F,
        0x80, 0x0B, 0x03, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0xc0, 0x31, 0x13, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0xc0,
        0x31, 0x13, 0xc4, 0xf1, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0xff, 0xff, 0xaa, 0x53, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xce, 0x01, 0x03, 0x03, 0x06, 0x04, 0x02, 0x08, 0x0a, 0x00, 0x00, 0x05, 0x51,
        0x00, 0x00, 0x00, 0x00
    };

    process_read_buffer(CP_DataPacketTcp, sizeof(CP_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv4, ipv4_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify that the checksum of TCP packet(IPv6) exceeding the MTU size is checked.
 */
TEST(PacketRouterTest, TCP_SendDataToCPModemForChecksumTest) {
    __DBG("[TEST] TCP_SendDataToCPModemForChecksumTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char WiFi_DataPacketTcp[ROUTER_NET_BUFFER_MTU + 100] = {
        0x00, 0x00, 0x00, 0x00, 0x66, 0x80, 0x00, 0x00, 0x00, 0x1c, 0x06, 0x32, 0xfd, 0x00, 0x97, 0x6a,
        0xd5, 0xd0, 0x19, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x13, 0xc4, 0xc0, 0x31,
        0x27, 0x1d, 0x1e, 0x4a, 0xa2, 0x41, 0xe8, 0xf9, 0x70, 0x12, 0x5f, 0x50, 0x39, 0x70, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xc4, 0x01, 0x03, 0x03, 0x0a
    };

    sendPacketToModemWithInterface(ipv6_addr, TYPE_IPv6, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify that the checksum of TCP packet(IPv4) exceeding the MTU size is checked.
 */
TEST(PacketRouterTest, TCP_SendDataToCPModemForChecksumWithIPv4Test) {
    __DBG("[TEST] TCP_SendDataToCPModemForChecksumForIPv4Test");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv4, ipv4_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char WiFi_DataPacketTcp[ROUTER_NET_BUFFER_MTU + 100] = {
        0x00, 0x00, 0x06, 0x40, 0x13, 0xc4, 0xc0, 0x31, 0x00, 0x1c, 0x00, 0x00, 0xfd, 0x06, 0x97, 0x6a,
        0x13, 0xc4, 0xc0, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x0B, 0x03, 0x1F,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xc4, 0xc0, 0x31,
        0x27, 0x1d, 0x1e, 0x4a, 0xa2, 0x41, 0xe8, 0xf9, 0x70, 0x12, 0x5f, 0x50, 0x39, 0x70, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xc4, 0x01, 0x03, 0x03, 0x0a
    };

    sendPacketToModemWithInterface(ipv4_addr, TYPE_IPv4, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv4, ipv4_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * In the situation where the Drop Rule is applied,
 * Verify that the TCP packet(IPv6) is dropped.
 */
TEST(PacketRouterTest, TCP_SendDataToCPModemOnDropRuleTest) {
    __DBG("[TEST] TCP_SendDataToCPModemOnDropRuleTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};
    unsigned char all_port[] = {0x00, 0x00};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);
    addRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, dst_port, src_port);

    unsigned char WiFi_DataPacketTcp[] = {
        0x00, 0x00, 0x00, 0x00, 0x66, 0x80, 0x00, 0x00, 0x00, 0x1c, 0x06, 0x32, 0xfd, 0x00, 0x97, 0x6a,
        0xd5, 0xd0, 0x19, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, src_port[0], src_port[1], dst_port[0], dst_port[1],
        0x27, 0x1d, 0x1e, 0x4a, 0xa2, 0x41, 0xe8, 0xf9, 0x70, 0x12, 0x5f, 0x50, 0x39, 0x70, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xc4, 0x01, 0x03, 0x03, 0x0a
    };

    sendPacketToModemWithInterface(ipv6_addr, TYPE_IPv6, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the UDP packet(IPv6) was sent to the CP Modem.
 */
TEST(PacketRouterTest, UDP_SendDataToCPModemTest) {
    __DBG("[TEST] UDP_SendDataToCPModemTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);

    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    set_dummy_socket_for_test(DUMMY_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_UDP, dst_port, src_port);

    // TBU(Seongsik Kim): Sending data to a CP modem on UDP

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_UDP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the UDP packet(IPv6) was sent to the WiFi Network.
 */
TEST(PacketRouterTest, UDP_SendDataToWiFiNetworkTest) {
    __DBG("[TEST] UDP_SendDataToWiFiNetworkTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);

    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    set_dummy_socket_for_test(DUMMY_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_UDP, dst_port, src_port);

    // TBU(Seongsik Kim): Sending data to a WiFi network on UDP

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_UDP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the ESP packet(IPv6) was sent to the CP Modem.
 */
TEST(PacketRouterTest, ESP_SendDataToCPModemTest) {
    __DBG("[TEST] ESP_SendDataToCPModemTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_ESP_FD);
    unsigned char spi[] = {0x00, 0x00, 0x09, 0x04};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    unsigned char WiFi_DataPacketTcp[] = {
        0x02, 0x00, 0x6c, 0x00, 0x60, 0x05, 0xe7, 0x3b, 0x00, 0x44, 0x32, 0x40, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x20, 0x01, 0x18, 0x90,
	    0x01, 0xf8, 0x20, 0xa7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0xa9, 0x71, 0xf2,
	    0x00, 0x00, 0x00, 0x1f, 0x30, 0x39, 0x63, 0x64, 0x31, 0x65, 0x38, 0x61, 0x37, 0x39, 0x36, 0x62,
	    0x33, 0x35, 0x32, 0x66, 0x53, 0xf5, 0x05, 0x2c, 0x22, 0x79, 0xf4, 0x65, 0x2d, 0xe8, 0xd2, 0xb8,
	    0x32, 0xe1, 0xda, 0x41, 0x96, 0xf0, 0x2f, 0x0c, 0xfe, 0x3f, 0x28, 0xe7, 0x9b, 0x45, 0x8b, 0xea,
	    0x45, 0x5c, 0x3b, 0xc3, 0x24, 0xec, 0x9c, 0x0b, 0x28, 0x7a, 0x49, 0x9e, 0xde, 0x5a, 0x19, 0x28
    };

    sendPacketToModemWithInterface(ipv6_addr, TYPE_IPv6, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the ESP packet(IPv6) was sent to the WiFi Network.
 */
TEST(PacketRouterTest, ESP_SendDataToWiFiNetworkTest) {
    __DBG("[TEST] ESP_SendDataToWiFiNetworkTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_ESP_FD);
    unsigned char spi[] = {0x00, 0x00, 0x09, 0x04};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    unsigned char CP_DataPacketEsp[] = {
        0x02, 0x00, 0x7c, 0x00, 0x60, 0x0a, 0xbb, 0x97, 0x00, 0x54, 0x32, 0x40, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x20, 0x01, 0x18, 0x90,
        0x01, 0xf8, 0x21, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x8c, 0xff, 0x4c,
        0x00, 0x00, 0x00, 0x01, 0x33, 0x35, 0x32, 0x66, 0x30, 0x39, 0x63, 0x64, 0x31, 0x65, 0x38, 0x61,
        0x37, 0x39, 0x36, 0x62, 0x5f, 0x1f, 0x0f, 0x5d, 0xd7, 0x39, 0x28, 0xda, 0xc1, 0x62, 0xb2, 0xdd,
        0x9b, 0xe8, 0x59, 0x4d, 0xf6, 0xcb, 0xf4, 0x3f, 0xb6, 0xa5, 0x79, 0xcc, 0xf8, 0x53, 0x5c, 0xb4,
        0x7a, 0xc1, 0x32, 0xf7, 0x6d, 0x2e, 0x5b, 0xb4, 0x3f, 0x67, 0x3d, 0xf6, 0x95, 0xb8, 0x0b, 0xbd,
        0x21, 0x61, 0x22, 0x5e, 0xf9, 0xdb, 0x9e, 0x7f, 0xc6, 0xc2, 0x08, 0xf9, 0x1d, 0x2b, 0xc1, 0x26
    };

    process_read_buffer(CP_DataPacketEsp, sizeof(CP_DataPacketEsp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify whether or not the ESP packet(IPv6) was sent to the CP Modem over Forward All Rule.
 */
TEST(PacketRouterTest, ESP_SendDataToCPModemForwardAllTest) {
    __DBG("[TEST] ESP_SendDataToCPModemForwardAllTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_ESP_FD);
    unsigned char all_port[] = {0x00, 0x00};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, all_port, all_port);

    unsigned char WiFi_DataPacketTcp[] = {
        0x02, 0x00, 0x6c, 0x00, 0x60, 0x05, 0xe7, 0x3b, 0x00, 0x44, 0x32, 0x40, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x20, 0x01, 0x18, 0x90,
	    0x01, 0xf8, 0x20, 0xa7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0xa9, 0x71, 0xf2,
	    0x00, 0x00, 0x00, 0x1f, 0x30, 0x39, 0x63, 0x64, 0x31, 0x65, 0x38, 0x61, 0x37, 0x39, 0x36, 0x62,
	    0x33, 0x35, 0x32, 0x66, 0x53, 0xf5, 0x05, 0x2c, 0x22, 0x79, 0xf4, 0x65, 0x2d, 0xe8, 0xd2, 0xb8,
	    0x32, 0xe1, 0xda, 0x41, 0x96, 0xf0, 0x2f, 0x0c, 0xfe, 0x3f, 0x28, 0xe7, 0x9b, 0x45, 0x8b, 0xea,
	    0x45, 0x5c, 0x3b, 0xc3, 0x24, 0xec, 0x9c, 0x0b, 0x28, 0x7a, 0x49, 0x9e, 0xde, 0x5a, 0x19, 0x28
    };

    sendPacketToModemWithInterface(ipv6_addr, TYPE_IPv6, WiFi_DataPacketTcp, sizeof(WiFi_DataPacketTcp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, all_port, all_port);

    int result = -1;
    EXPECT_EQ(result, -1);
}

/**
 * Verify whether or not the ESP packet(IPv6) was sent to the WiFi Network over Forward All Rule.
 */
TEST(PacketRouterTest, ESP_SendDataToWiFiNetworkForwardAllTest) {
    __DBG("[TEST] ESP_SendDataToWiFiNetworkForwardAllTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_ESP_FD);
    unsigned char spi[] = {0x00, 0x00, 0x09, 0x04};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    unsigned char CP_DataPacketEsp[] = {
        0x02, 0x00, 0x7c, 0x00, 0x60, 0x0a, 0xbb, 0x97, 0x00, 0x54, 0x32, 0x40, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x20, 0x01, 0x18, 0x90,
        0x01, 0xf8, 0x21, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x8c, 0xff, 0x4c,
        0x00, 0x00, 0x00, 0x01, 0x33, 0x35, 0x32, 0x66, 0x30, 0x39, 0x63, 0x64, 0x31, 0x65, 0x38, 0x61,
        0x37, 0x39, 0x36, 0x62, 0x5f, 0x1f, 0x0f, 0x5d, 0xd7, 0x39, 0x28, 0xda, 0xc1, 0x62, 0xb2, 0xdd,
        0x9b, 0xe8, 0x59, 0x4d, 0xf6, 0xcb, 0xf4, 0x3f, 0xb6, 0xa5, 0x79, 0xcc, 0xf8, 0x53, 0x5c, 0xb4,
        0x7a, 0xc1, 0x32, 0xf7, 0x6d, 0x2e, 0x5b, 0xb4, 0x3f, 0x67, 0x3d, 0xf6, 0x95, 0xb8, 0x0b, 0xbd,
        0x21, 0x61, 0x22, 0x5e, 0xf9, 0xdb, 0x9e, 0x7f, 0xc6, 0xc2, 0x08, 0xf9, 0x1d, 0x2b, 0xc1, 0x26
    };

    process_read_buffer(CP_DataPacketEsp, sizeof(CP_DataPacketEsp) / sizeof(unsigned char));

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Precondition:
 * 1) The given port is 0x13c4 for source port and 0xc031 for destination port.
 * 2) A TCP Forward Rule for the given port is added.
 *
 * Test Purpose:
 * Verify that the TCP Drop Rule for the given port is added and deleted.
 */
TEST(PacketRouterTest, RuleVerify_AddAndDeleteDropRuleTest) {
    __DBG("[TEST] RuleVerify_AddAndDeleteRuleTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    addRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, dst_port, src_port);
    delRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, dst_port, src_port);

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Precondition:
 * 1) The given port is 0x13c4 for source port and 0xc031 for destination port.
 * 2) A TCP Forward Rule for all destination ports is added.
 *
 * Test Purpose:
 * Verify that the TCP Drop Rule for the given port is added and deleted.
 */
TEST(PacketRouterTest, RuleVerify_AddAndDeleteDropRuleOnAllowingAllPortTest) {
    __DBG("[TEST] RuleVerify_AddAndDeleteDropRuleOnAllowingAllPortTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};
    unsigned char all_port[] = {0x00, 0x00};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);

    addRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, dst_port, src_port);
    delRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, dst_port, src_port);

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Precondition:
 * 1) The given port is 0x13c4 for source port and 0xc031 for destination port.
 *
 * Test Purpose:
 * Verify that the TCP Forward Rule for the given port is deleted
 * through the delete all port Forward Rule.
 */
TEST(PacketRouterTest, RuleVerify_AddAndDeleteAllPortRuleTest) {
    __DBG("[TEST] RuleVerify_DeleteAllPortRuleTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};
    unsigned char all_port[] = {0x00, 0x00};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Precondition:
 * 1) The given port number is 0x13c4 for source port number and 0xc031 for destination port number.
 * 2) A TCP Forward Rule for all destination ports is added.
 *
 * Test Purpose:
 * Verify that the TCP Drop Rule for the given port is deleted when TCP Drop Rule for all ports is deleted.
 */
TEST(PacketRouterTest, RuleVerify_DeleteESPRuleWithPortsTest) {
    __DBG("[TEST] RuleVerify_DeleteESPRuleWithPortsTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};
    unsigned char all_port[] = {0x00, 0x00};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);

    addRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, dst_port, src_port);
    delRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_TCP, all_port, src_port);

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Precondition:
 * 1) The given SPI is 0x00000904
 * 2) A ESP Forward Rule for all SPI is added.
 *
 * Test Purpose:
 * Verify that the ESP Drop Rule for the given SPI is added and deleted.
 */
TEST(PacketRouterTest, RuleVerify_DeleteESPRuleWithSpiTest) {
    __DBG("[TEST] RuleVerify_DeleteESPRuleWithSpiTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char spi[] = {0x00, 0x00, 0x09, 0x04};
    unsigned char all_port[] = {0x00, 0x00};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, all_port, all_port);

    addRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_ESP, spi, spi + 2);
    delRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_ESP, spi, spi + 2);

    delRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_ESP, all_port, all_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * (Exception handling)
 * Verify that an exception is handled when an invalid command is entered.
 */
TEST(PacketRouterTest, InvalidCommandTest) {
    __DBG("[TEST] InvalidCommandTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    /** New Request **/
    /* Invalid MessageType*/
    pkt_command(0xFF, CMD_ADD, TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid Command*/
    pkt_command(MSG_CTRL, 0xFF, TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid IP type*/
    pkt_command(MSG_CTRL, CMD_ADD, 0xFF, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid Rule*/
    pkt_command(MSG_CTRL, CMD_ADD, TYPE_IPv6, ipv6_addr, 0xFF, PRT_TCP, dst_port, src_port);
    /* Invalid Protocol*/
    pkt_command(MSG_CTRL, CMD_ADD, TYPE_IPv6, ipv6_addr, FORWARD_RULE, 0xFF, dst_port, src_port);


    /** Modify Request **/
    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid MessageType*/
    pkt_command(0xFF, CMD_ADD, TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid Command*/
    pkt_command(MSG_CTRL, 0xFF, TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid IP type*/
    pkt_command(MSG_CTRL, CMD_ADD, 0xFF, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);
    /* Invalid Rule*/
    pkt_command(MSG_CTRL, CMD_ADD, TYPE_IPv6, ipv6_addr, 0xFF, PRT_TCP, dst_port, src_port);
    /* Invalid Protocol*/
    pkt_command(MSG_CTRL, CMD_ADD, TYPE_IPv6, ipv6_addr, FORWARD_RULE, 0xFF, dst_port, src_port);

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * (Exception handling)
 * Verify that an exception is handled when an invalid request of rule is entered.
 */
TEST(PacketRouterTest, InvalidRequestOfRuleTest) {
    __DBG("[TEST] InvalidRequestRuleTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char all_port[] = {0x00, 0x00};
    unsigned char spi[] = {0x00, 0x00, 0x09, 0x04};

    /* Invalid case: Removal of non-existent TCP rule*/
    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, all_port, all_port);

    /* Invalid case: Removal of non-existent ESP rule*/
    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    /* Invalid case: Drop and Foward Rule's SPI is same
     * (To change drop to forward, it is necessary to first remove the rule.)
     */
    addRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_ESP, spi, spi + 2);
    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    delRule(TYPE_IPv6, ipv6_addr, DROP_RULE, PRT_ESP, spi, spi + 2);
    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_ESP, spi, spi + 2);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify that the fragment packek is stored and used
 */
TEST(PacketRouterTest, CheckFragmentFunctionTest) {
    __DBG("[TEST] CheckFragmentFunctionTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    unsigned char WiFi_FragmentDataPacketTcp[] = {
        0x00, 0x00, 0x00, 0x00, 0x66, 0x80, 0x00, 0x00, 0x00, 0x1c, 0x06, 0x32, 0xfd, 0x00, 0x97, 0x6a,
        0xd5, 0xd0, 0x19, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x26, 0x07, 0xfc, 0x20,
        0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01, 0x13, 0xc4, 0xc0, 0x31,
        0x27, 0x1d, 0x1e, 0x4a, 0xa2, 0x41, 0xe8, 0xf9, 0x70, 0x12, 0x5f, 0x50, 0x39, 0x70, 0x00, 0x00,
        0x02, 0x04, 0x04, 0xc4, 0x01, 0x03, 0x03, 0x0a
    };

    struct wfc_router_interface* local_p_if = find_interface(ipv6_addr);
    p_wfc_router_data_message_t p_msg = (p_wfc_router_data_message_t)WiFi_FragmentDataPacketTcp;
    struct wfc_router_protocol* p_proto = find_protocol(local_p_if, get_protocol_from_packet(p_msg->data, local_p_if->ipv4));
    struct ip6_hdr* p_hdr = (struct ip6_hdr*)WiFi_FragmentDataPacketTcp;
    p_hdr->ip6_nxt = IPPROTO_FRAGMENT;

    is_first_fragment(WiFi_FragmentDataPacketTcp, 0);
    /* Fragment is stored */
    add_fragment(p_proto, WiFi_FragmentDataPacketTcp, 0);
    is_awaited_fragment(p_proto, WiFi_FragmentDataPacketTcp, 0);

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify that the Device Socket opens again on using 'restart_device_sockets' function
 */
TEST(PacketRouterTest, RestartDeviceSocketsTest) {
    __DBG("[TEST] RestartDeviceSocketsTest");
    unsigned char *ipv6_addr;
    unsigned char *ipv4_addr;

    setUp(&ipv6_addr, &ipv4_addr);
    set_rx_socket_for_test(RX_SOCK_FD);
    set_tx_socket_for_test(TX_SOCK_FD);
    unsigned char src_port[] = {0x13, 0xc4};
    unsigned char dst_port[] = {0xc0, 0x31};

    addRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    struct wfc_router_interface* local_p_if = find_interface(ipv6_addr);
    local_p_if->tx_bind_to_device = 1;
    restart_device_sockets(local_p_if);

    delRule(TYPE_IPv6, ipv6_addr, FORWARD_RULE, PRT_TCP, dst_port, src_port);

    int result = -1;
    EXPECT_EQ(result, -1);
    tearDown(&ipv6_addr, &ipv4_addr);
}

/**
 * Verify that the necessary functions (open netlink socket, netlink event receiver and io reader)
 * for executing the main function are executed correctly
 */
TEST(PacketRouterTest, MainFunctionTest) {
    __DBG("[TEST] MainFunctionTest");

    /* Initial process */
    LIST_INIT(&__router.interfaces);
    static wfc_router_t __router;

    netlink_open_socket();
    netlink_open_event_socket();
    pthread_create(&__router.nt_thread, NULL, nt_event_receiver, NULL);
    pthread_create(&__router.io_thread, NULL, io_reader, NULL);

    /* Exit process */
    netlink_close_socket();
    netlink_close_event_socket();

    int result = -1;
    EXPECT_EQ(result, -1);
}

/**
 * Verify netlink function execution inside the packet router
 */
TEST(PacketRouterTest, NetlinkMessageTest) {
    __DBG("[TEST] NetlinkMessageTest");

    unsigned int m_nlmsg_len = 480;

    struct nlmsghdr header = {
        m_nlmsg_len,
        (unsigned short) RTM_NEWADDR,
        (unsigned short) NLM_F_REQUEST,
        (unsigned int) 0,
        (unsigned int) 0
    };
    process_link_up(&header);

    header = {
        m_nlmsg_len,
        (unsigned short) RTM_DELLINK,
        (unsigned short) NLM_F_REQUEST,
        (unsigned int) 0,
        (unsigned int) 0
    };
    process_link_down_or_deleted(&header);

    header = {
        m_nlmsg_len,
        (unsigned short) RTM_DELADDR,
        (unsigned short) NLM_F_REQUEST,
        (unsigned int) 0,
        (unsigned int) 0
    };
    process_address_event(&header, "added to");

    header = {
        m_nlmsg_len,
        (unsigned short) RTM_DELADDR,
        (unsigned short) NLM_F_REQUEST,
        (unsigned int) 0,
        (unsigned int) 0
    };
    process_address_event(&header, "removed from");

    int result = -1;
    EXPECT_EQ(result, -1);
}

/**
 * Verify that the value for the protocol is properly converted to a string
 */
TEST(PacketRouterTest, ConvertProtocolToStringTest) {
    __DBG("[TEST] ConvertProtocolToStringTest");

    char *tmpStr;
    unsigned char errStr = '\0';
    tmpStr = convert_protocol_to_string(IPPROTO_TCP);
    EXPECT_STREQ(tmpStr, "TCP");
    tmpStr = convert_protocol_to_string(IPPROTO_UDP);
    EXPECT_STREQ(tmpStr, "UDP");
    tmpStr = convert_protocol_to_string(IPPROTO_ESP);
    EXPECT_STREQ(tmpStr, "ESP");
    tmpStr = convert_protocol_to_string(errStr);
    EXPECT_STREQ(tmpStr, "ERR");
}

/**
 * Excute a command to add a rule
 */
void addRule(unsigned char ip_type, unsigned char ip_addr[], unsigned char rule, unsigned char protocol,
    unsigned char val1[], unsigned char val2[]) {

    pkt_command(MSG_CTRL, CMD_ADD, ip_type, ip_addr, rule, protocol, val1, val2);
}

/**
 * Excute a command to delete a rule
 */
void delRule(unsigned char ip_type, unsigned char ip_addr[], unsigned char rule, unsigned char protocol,
    unsigned char val1[], unsigned char val2[]) {

    pkt_command(MSG_CTRL, CMD_DELETE, ip_type, ip_addr, rule, protocol, val1, val2);
}

/**
 * A function that creates a message from arguments and excutes it.
 */
void pkt_command(unsigned char meessage_type, unsigned char command_id, unsigned char ip_type, unsigned char ip_addr[], unsigned char rule, unsigned char protocol,
    unsigned char val1[], unsigned char val2[]) {

    unsigned char CommandFromCP[] = {
        meessage_type,  0x00,           command_id,        0x00,
        ip_type,        ip_addr[0],     ip_addr[1],     ip_addr[2],
        ip_addr[3],     ip_addr[4],     ip_addr[5],     ip_addr[6],
        ip_addr[7],     ip_addr[8],     ip_addr[9],     ip_addr[10],
        ip_addr[11],    ip_addr[12],    ip_addr[13],    ip_addr[14],
        ip_addr[15],    0x01,           rule,           protocol,
        val1[0],        val1[1],        val2[0],        val2[1]
    };

    process_read_buffer(CommandFromCP, sizeof(CommandFromCP) / sizeof(unsigned char));
}

void sendPacketToModemWithInterface(unsigned char *ip_adrr, int ip_type, unsigned char *WiFi_DataPacketTcp, int packetLen) {
    struct wfc_router_interface* local_p_if = find_interface(ip_adrr);
    if(ip_type == TYPE_IPv4) {
        local_p_if->ipv4 = 1;
    }
    compare_destination_ip_with_interface(WiFi_DataPacketTcp + ROUTER_NET_BUFFER_OFFSET, local_p_if);
    check_network_packet_and_forward_to_modem(local_p_if, WiFi_DataPacketTcp , packetLen);
}

/**
 * Before test, this function is called.
 * A function to set default values and environments.
 */
void setUp(unsigned char **ipv6_addr, unsigned char **ipv4_addr) {
    // IPv6 2607:fc20:0a65:1a82:0000:001c:b6df:c601
    unsigned char test_ipv6_addr[] = {
        0x26, 0x07, 0xfc, 0x20, 0x0a, 0x65, 0x1a, 0x82, 0x00, 0x00, 0x00, 0x1c, 0xb6, 0xdf, 0xc6, 0x01
    };

    // IPv4 128.11.3.31
    unsigned char test_ipv4_addr[] = {
        0x80, 0x0B, 0x03, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    *ipv6_addr = (unsigned char *)malloc(sizeof(test_ipv6_addr) / sizeof(unsigned char));
    *ipv4_addr = (unsigned char *)malloc(sizeof(test_ipv4_addr) / sizeof(unsigned char));

    memcpy(*ipv6_addr, test_ipv6_addr, sizeof(test_ipv6_addr) / sizeof(unsigned char));
    memcpy(*ipv4_addr, test_ipv4_addr, sizeof(test_ipv4_addr) / sizeof(unsigned char));

    set_tx_socket_for_test(0);
    set_rx_socket_for_test(0);
    set_dummy_socket_for_test(0);
}

/**
 * After test, this function is called.
 * A function to release allocated variables.
 */
void tearDown(unsigned char **ipv6_addr, unsigned char **ipv4_addr) {
    if(*ipv6_addr != nullptr) {
        free(*ipv6_addr);
    }
    if(*ipv4_addr != nullptr) {
        free(*ipv4_addr);
    }
}