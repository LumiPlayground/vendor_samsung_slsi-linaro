/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or
 * distributed, transmitted, transcribed, stored in a retrieval system or
 * translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed to third parties
 * without the express written permission of Samsung Electronics.
 */
/*
    ROUTER protocol defines
*/
#ifndef _wfc_router_protocol_h
#define _wfc_router_protocol_h

/*
    Defines
*/
#define ROUTER_IP_ADDR_MAX_SIZE     16
#define ROUTER_RES_NOTIFY_SEQNUM    0xbb

/*
    Enums
*/
typedef enum {
    ROUTER_MSG_CONTROL = 0x01,
    ROUTER_MSG_DATA
} wfc_router_message_types_e_t;

typedef enum {
    ROUTER_CMD_ADD = 0x01,
    ROUTER_CMD_DELETE
} wfc_router_command_types_e_t;

typedef enum {
    ROUTER_RES_CMD_MASK = 0x80,
    ROUTER_RES_SOCKET_ERROR = 0xb1
} wfc_router_response_types_e_t;

typedef enum {
    ROUTER_RULE_FWD = 0x01,
    ROUTER_RULE_DROP
} wfc_router_rule_e_t;

typedef enum {
    ROUTER_IP_TYPE_IPv4 = 0x01,
    ROUTER_IP_TYPE_IPv6
} wfc_router_ip_type_e_t;

typedef enum {
    ROUTER_PROTO_TCP = 0x01,
    ROUTER_PROTO_UDP,
    ROUTER_PROTO_ESP
} wfc_router_protocol_e_t;

/*
    Messages
*/
#pragma pack(1)
typedef union {
    struct {
        unsigned short                      dst;
        unsigned short                      src;
    } ports;
    unsigned int                            spi;
} wfc_router_protocol_rule_parameters_t;

typedef struct {
    unsigned char                           rule;
    unsigned char                           protocol;
    wfc_router_protocol_rule_parameters_t   param;
} wfc_router_protocol_rule_descriptor_t, *p_wfc_router_protocol_rule_descriptor_t;

typedef struct {
    unsigned short                          type;
    unsigned char                           cmd;
    unsigned char                           seqnum;
    unsigned int                            index;
    unsigned char                           ip_type;
    unsigned char                           ip_addr[ROUTER_IP_ADDR_MAX_SIZE];
    unsigned char                           num_rules;
    wfc_router_protocol_rule_descriptor_t   rules[0];
} wfc_router_control_request_t, *p_wfc_router_control_request_t;

typedef struct {
    unsigned short                          type;
    unsigned char                           res;
    unsigned char                           seqnum;
    unsigned short                          err;
} wfc_router_control_response_t, *p_wfc_router_control_response_t;

typedef struct {
    unsigned short                          type;
    unsigned short                          length;
    unsigned char                           data[0];
} wfc_router_data_message_t, *p_wfc_router_data_message_t;
#pragma pack()

#endif
