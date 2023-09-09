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
    ROUTER internal definitions
*/
#ifndef _wfc_router_h
#define _wfc_router_h

#include <pthread.h>
#include <sys/queue.h>

/*
    Structures
*/
typedef struct wfc_router_packet_fragment {
    union {
        unsigned short                              v4;
        unsigned int                                v6;
    }id;
    LIST_ENTRY(wfc_router_packet_fragment)          entries;
} wfc_router_packet_fragment_t;

typedef struct wfc_router_protocol_rule {
    wfc_router_protocol_rule_parameters_t           prp;        /* Filter parameters */
    int                                             dummy_sk;   /* Dummy socket for ICMP Unreachable */
    LIST_ENTRY(wfc_router_protocol_rule)            entries;
} wfc_router_protocol_rule_t;

typedef struct wfc_router_protocol {
    unsigned char                                   protocol;   /* Protocol */
    int                                             sk_tx;      /* TX Socket */
    struct wfc_router_interface*                    parent;     /* Parent interface */
    int                                             fwd_all;    /* Forward all flag */
    LIST_HEAD(fwd_list, wfc_router_protocol_rule)   fwd;        /* Forward rules */
    LIST_HEAD(drop_list, wfc_router_protocol_rule)  drop;       /* Drop rules */
    LIST_HEAD(frag_list, wfc_router_packet_fragment) fragments; /* Fragmented packets IDs */
    LIST_ENTRY(wfc_router_protocol)                 entries;
} wfc_router_protocol_t;

typedef struct wfc_router_iptables_rule {
    unsigned char                  ip[ROUTER_IP_ADDR_MAX_SIZE]; /* Destination IP address */
    LIST_ENTRY(wfc_router_iptables_rule)            entries;
} wfc_router_iptables_rule_t;

typedef struct wfc_router_rttables_rule {
    unsigned char                  ip[ROUTER_IP_ADDR_MAX_SIZE]; /* Destination IP address */
    LIST_ENTRY(wfc_router_rttables_rule)            entries;
} wfc_router_rttables_rule_t;

typedef struct wfc_router_interface {
    int                            ifindex;                     /* Interface Index */
    char                           ifname[IF_NAMESIZE];         /* Interface name */
    int                            ipv4;                        /* IP address type: true for IPv4 */
    unsigned char                  ip[ROUTER_IP_ADDR_MAX_SIZE]; /* IP address */
    char                           ip_str[INET6_ADDRSTRLEN];    /* IP address String*/
    int                                             sk_rx;      /* RX Socket */
    pthread_t                                       sk_thread;  /* RX Socket read thread */
    int                                     tx_bind_to_device;  /* Bind TX socket to current device */
    LIST_HEAD(iptbl_list, wfc_router_iptables_rule) iptables;   /* Blocking rules for TCP RST */
    LIST_HEAD(rttbl_list, wfc_router_rttables_rule) rttables;   /* Routing rules */
    LIST_HEAD(proto_list, wfc_router_protocol)      protocols;  /* Enabled protocols */
    LIST_ENTRY(wfc_router_interface)                entries;
} wfc_router_interface_t;

typedef struct {
    int                                             io_dev;     /* I/O defice FD */
    pthread_t                                       io_thread;  /* I/O device read thread */
    pthread_mutex_t                                 io_lock;    /* I/O device write lock */
    int                                             nt_sock;    /* Netlink socket */
    int                                             nt_evt_sock;/* Netlink events socket */
    pthread_t                                       nt_thread;  /* Netlink events read thread */
    LIST_HEAD(if_head, wfc_router_interface)        interfaces; /* Created interfaces */
} wfc_router_t;

#endif
