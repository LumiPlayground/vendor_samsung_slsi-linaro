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
    ROUTER main
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <linux/tcp.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/wait.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>
#include <sys/uio.h>

#include "checksum.h"

#include "debug.h"
#include "protocol.h"
#include "router.h"

/*
    Defines
*/
#define ROUTER_IO_DEVICE            "/dev/umts_wfc1"
#define ROUTER_IPV4_ADDR_SIZE       4
#define ROUTER_IPV4_PREFIX_LEN      32
#define ROUTER_IPV6_ADDR_SIZE       16
#define ROUTER_IPV6_PREFIX_LEN      128
#define ROUTER_IO_BUFFER_MAX_SIZE   2048
#define ROUTER_NET_BUFFER_MAX_SIZE  4096
#define ROUTER_NET_BUFFER_OFFSET    sizeof(wfc_router_data_message_t)
#define ROUTER_NET_BUFFER_MTU       1500
#define GET_PROTOCOL_AND_MOVE       proto = *buf; buf += *(buf+1);
#define ROUTER_TCP_RST_FLAG         0x04

/*
    Forward declarations
*/
static void* socket_receiver(void* arg);
static void* io_reader(void* arg);

/*
    Variables
*/
static wfc_router_t __router;

#ifdef PKTRT_TEST
#undef __IS_VERBOSE
#define __IS_VERBOSE 1

static int g_tx_socket = 0;
static int g_rx_socket = 0;
static int g_dummy_socket = 0;

static void set_tx_socket_for_test(int sk) {
    g_tx_socket = sk;
}

static void set_rx_socket_for_test(int sk) {
    g_rx_socket = sk;
}

static void set_dummy_socket_for_test(int sk) {
    g_dummy_socket = sk;
}
#endif

void log_packet_data(char *type_behavior, int len_of_data, unsigned char* buf) {
    if(__IS_VERBOSE) {
        int i;
        char dump_hex[128] = {0};

        for (i = 0; i < 23; i++) {
            sprintf(dump_hex + (i * 3), "%02x ", buf[i]);
        }
        __VERBOSE("%s[%d]: %s", type_behavior, len_of_data, dump_hex);
    }
}

/*
    Network
*/
static char* convert_protocol_to_string(unsigned char protocol) {
    static char str_buf[16];

    switch (protocol) {
        case IPPROTO_TCP: strcpy(str_buf, "TCP"); break;
        case IPPROTO_UDP: strcpy(str_buf, "UDP"); break;
        case IPPROTO_ESP: strcpy(str_buf, "ESP"); break;
        default:
            /* should not be here */
            strcpy(str_buf, "ERR");
    }
    return str_buf;
}

static __inline__ int isIPv4_packet(unsigned char byte) {
    return ((byte & 0xf0) == 0x40);
}

static __inline__ int isIPv6_packet(unsigned char byte) {
    return ((byte & 0xf0) == 0x60);
}

static __inline__ unsigned char* get_source_ip_from_packet(unsigned char* buf) {
    if (isIPv4_packet(*buf)) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        return (unsigned char*)&p_hdr->saddr;
    } else if (isIPv6_packet(*buf)) {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        return p_hdr->ip6_src.in6_u.u6_addr8;
    } else {
        __ERROR("%s: Not IP header", __func__);
        return NULL;
    }
}

static __inline__ unsigned char* get_destination_ip_from_packet(unsigned char* buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        return (unsigned char*)&p_hdr->daddr;
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        return p_hdr->ip6_dst.in6_u.u6_addr8;
    }
}

static __inline__ unsigned int compare_destination_ip_with_interface(unsigned char* buf, wfc_router_interface_t* p_if) {
    int ipv4 = isIPv4_packet(*buf);
    if (ipv4 != p_if->ipv4) {
        __WARN("%s: IP version not matched", __func__);
        return 0;
    }
    /* compare destination IP and interface addresses, return 1 on matched */
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        return (memcmp(&p_hdr->daddr, p_if->ip, ROUTER_IPV4_ADDR_SIZE) == 0);
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        return (memcmp(p_hdr->ip6_dst.in6_u.u6_addr8, p_if->ip, ROUTER_IPV6_ADDR_SIZE) == 0);
    }
}

static __inline__ unsigned char get_protocol_from_packet(unsigned char* buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        return p_hdr->protocol;
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            proto = *buf;
            buf += sizeof(struct ip6_frag);
        }
        if (proto == IPPROTO_AH) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ESP) {
            return proto;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_MH) {
            GET_PROTOCOL_AND_MOVE;
        }
        return proto;
    }
}

static int get_ports_from_packet(unsigned char* buf, unsigned char protocol, int ipv4, unsigned short* p_dst, unsigned short* p_src) {
    *p_dst = *p_src = 0;

    /* move to next protocol */
    if (ipv4) {
        buf += ((*buf & 0x0f) * 4);
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            proto = *buf;
            buf += sizeof(struct ip6_frag);
        }
        if (proto == IPPROTO_AH) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_MH) {
            GET_PROTOCOL_AND_MOVE;
        }
    }
    /* get ports */
    switch (protocol) {
        case IPPROTO_TCP:
        {
            struct tcphdr* p_hdr = (struct tcphdr*)buf;
            *p_dst = p_hdr->dest;
            *p_src = p_hdr->source;
            break;
        }
        case IPPROTO_UDP:
        {
            struct udphdr* p_hdr = (struct udphdr*)buf;
            *p_dst = p_hdr->dest;
            *p_src = p_hdr->source;
            break;
        }
        default:
            __ERROR("%s: Bad protocol specified", __func__);
            return (-1);
    }
    return 0;
}

static int get_spi_from_packet(unsigned char* buf, int ipv4) {
    struct ip_esp_hdr* p_hdr;

    /* move to next protocol */
    if (ipv4) {
        buf += ((*buf & 0x0f) * 4);
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            proto = *buf;
            buf += sizeof(struct ip6_frag);
        }
        if (proto == IPPROTO_AH) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ESP) {
            /* nothing */
        }
    }
    p_hdr = (struct ip_esp_hdr*)buf;
    return p_hdr->spi;
}

/* IPv4 checksum */
static __inline__ uint16_t checksum_v4(const struct iphdr* ip, const void* data, uint16_t len) {
	/* header first */
	uint32_t sum = ipv4_pseudo_header_checksum(ip, len);
	/* packet data */
	sum = ip_checksum_add(sum, data, len);
	/* done */
	return ip_checksum_finish(sum);
}

/* IPv6 checksum */
static __inline__ uint16_t checksum_v6(const struct ip6_hdr* ip6, uint8_t protocol, const void* data, uint32_t len) {
	/* header first */
	uint32_t sum = ipv6_pseudo_header_checksum(ip6, len, protocol);
	/* packet data */
	sum = ip_checksum_add(sum, data, len);
	/* done */
	return ip_checksum_finish(sum);
}

static void calculate_tcp_checksum(unsigned char* buf, int len, unsigned char protocol, int ipv4) {
    if ((protocol == IPPROTO_TCP) && (len > ROUTER_NET_BUFFER_MTU)) {
        if (ipv4) {
            unsigned int iphdr_len = ((*buf & 0x0f) * 4);
            struct tcphdr* p_tcphdr = (struct tcphdr*)(buf + iphdr_len);
            /* reset */
            p_tcphdr->check = 0;
            /* calculate */
            unsigned short tcp_len = len - iphdr_len;
            p_tcphdr->check = checksum_v4((const struct iphdr*)buf, (const void*)p_tcphdr, tcp_len);
            __DBG("%s: TCPv4 checksum=%#04x, length = %d, total = %d", __func__, ntohs(p_tcphdr->check), tcp_len, len);
        } else {
            struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
            unsigned char proto = p_hdr->ip6_nxt;

            buf += sizeof(struct ip6_hdr);
            if (proto == IPPROTO_HOPOPTS) {
                GET_PROTOCOL_AND_MOVE;
            }
            if (proto == IPPROTO_DSTOPTS) {
                GET_PROTOCOL_AND_MOVE;
            }
            if (proto == IPPROTO_ROUTING) {
                GET_PROTOCOL_AND_MOVE;
            }
            if (proto == IPPROTO_FRAGMENT) {
                proto = *buf;
                buf += sizeof(struct ip6_frag);
            }
            if (proto == IPPROTO_AH) {
                GET_PROTOCOL_AND_MOVE;
            }
            if (proto == IPPROTO_DSTOPTS) {
                GET_PROTOCOL_AND_MOVE;
            }
            if (proto == IPPROTO_MH) {
                GET_PROTOCOL_AND_MOVE;
            }
            if (proto == IPPROTO_TCP) {
                struct tcphdr* p_tcphdr = (struct tcphdr*)buf;
                /* reset */
                p_tcphdr->check = 0;
                /* calculate */
                unsigned int tcp_len = len - ((unsigned char*)p_tcphdr - (unsigned char*)p_hdr);
                p_tcphdr->check = checksum_v6(p_hdr, IPPROTO_TCP, (const void*)p_tcphdr, tcp_len);
                __DBG("%s: TCPv6 checksum=%#04x, length = %d, total = %d", __func__, ntohs(p_tcphdr->check), tcp_len, len);
            }
        }
    }
}

static int fill_sa_from_packet(unsigned char* pkt_buf, unsigned char* addr_buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)pkt_buf;
        struct sockaddr_in* p_sa = (struct sockaddr_in*)addr_buf;

        p_sa->sin_family = AF_INET;
        p_sa->sin_port = 0;
        memcpy(&p_sa->sin_addr, &p_hdr->daddr, ROUTER_IPV4_ADDR_SIZE);
        return sizeof(struct sockaddr_in);
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)pkt_buf;
        struct sockaddr_in6* p_sa = (struct sockaddr_in6*)addr_buf;

        p_sa->sin6_family = AF_INET6;
        p_sa->sin6_port = 0;
        p_sa->sin6_flowinfo = 0;
        p_sa->sin6_scope_id = 0; /* possible need to set scope id to specific adapter */
        memcpy(&p_sa->sin6_addr, &p_hdr->ip6_dst, ROUTER_IPV6_ADDR_SIZE);
        return sizeof(struct sockaddr_in6);
    }
}

static int is_first_fragment(unsigned char* buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        __DBG("%s: IPv4 offs=%#04x", __func__, ntohs(p_hdr->frag_off) & IP_OFFMASK);
        if ((ntohs(p_hdr->frag_off) & IP_OFFMASK) == 0) {
            return 1;
        }
        return 0;
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            struct ip6_frag* hdr = (struct ip6_frag*)buf;
            __DBG("%s: IPv6 offs=%#04x", __func__, hdr->ip6f_offlg & IP6F_OFF_MASK);
            if ((hdr->ip6f_offlg & IP6F_OFF_MASK) == 0) {
                return 1;
            } else {
                return 0;
            }
        }
        return 1;
    }
}

static int is_fragmented_packet(unsigned char* buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        if (((ntohs(p_hdr->frag_off) & IP_OFFMASK) != 0) ||
            ((ntohs(p_hdr->frag_off) & IP_MF) != 0)) {
            return 1;
        }
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            return 1;
        }
    }
    return 0;
}

static int is_last_fragment(unsigned char* buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        if ((ntohs(p_hdr->frag_off) & IP_MF) == 0) {
            return 1;
        }
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            struct ip6_frag* hdr = (struct ip6_frag*)buf;
            if ((hdr->ip6f_offlg & IP6F_MORE_FRAG) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

static void save_packet_id_to_fragment(struct wfc_router_packet_fragment* p_frg, unsigned char* buf, int ipv4) {
    if (ipv4) {
        struct iphdr* p_hdr = (struct iphdr*)buf;
        p_frg->id.v4 = p_hdr->id;
    } else {
        struct ip6_hdr* p_hdr = (struct ip6_hdr*)buf;
        unsigned char proto = p_hdr->ip6_nxt;

        buf += sizeof(struct ip6_hdr);
        if (proto == IPPROTO_HOPOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_DSTOPTS) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_ROUTING) {
            GET_PROTOCOL_AND_MOVE;
        }
        if (proto == IPPROTO_FRAGMENT) {
            struct ip6_frag* hdr = (struct ip6_frag*)buf;
            p_frg->id.v6 = hdr->ip6f_ident;
        }
    }
}

static __inline__ int starts_with(const char *src, const char *find) {
    return (strlen(src) >= strlen(find)) && (strncmp(src, find, strlen(find)) == 0);
}

static int find_ifindex(struct wfc_router_interface* p_if) {
    int ifindex = -1;
    char str_addr[INET6_ADDRSTRLEN];
    struct ifaddrs *ifap, *ifa;

    memset(p_if->ifname, 0, IF_NAMESIZE);

    ifindex = p_if->ifindex;
    if (ifindex > 0) {
        if_indextoname(ifindex, p_if->ifname);
        __INFO("%s: Interface: %s, address: %s, ifindex: %d ... Found\n", __func__, p_if->ifname, p_if->ip_str, ifindex);
        return ifindex;
    }

    if (getifaddrs(&ifap) < 0) {
        __ERROR("%s: Failed to get interfaces (%d)", __func__, errno);
        return (-1);
    }
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        if (starts_with(ifa->ifa_name, "rmnet")) {
            continue;
        }
        if (p_if->ipv4 && (AF_INET == ifa->ifa_addr->sa_family)) {
            struct sockaddr_in* psa = (struct sockaddr_in*)ifa->ifa_addr;
            int index = if_nametoindex(ifa->ifa_name);
            if (!memcmp(p_if->ip, &psa->sin_addr, ROUTER_IPV4_ADDR_SIZE)) {
                __INFO("%s: Interface: %s, address: %s, ifindex: %d ... Found\n", __func__, ifa->ifa_name, p_if->ip_str, index);
                strncpy(p_if->ifname, ifa->ifa_name, IF_NAMESIZE - 1);
                ifindex = index;
                break;
            } else {
                inet_ntop(AF_INET, (void*)&psa->sin_addr, str_addr, INET6_ADDRSTRLEN);
                __DBG("%s: Interface: %s, address: %s, ifindex: %d, next: %p\n", __func__, ifa->ifa_name, str_addr, index, ifa->ifa_next);
            }
        } else if (!p_if->ipv4 && (AF_INET6 == ifa->ifa_addr->sa_family)) {
           struct sockaddr_in6* psa = (struct sockaddr_in6*)ifa->ifa_addr;
           int index = if_nametoindex(ifa->ifa_name);
           if (!memcmp(p_if->ip, &psa->sin6_addr, ROUTER_IPV6_ADDR_SIZE)) {
                __INFO("%s: Interface: %s, address: %s, ifindex: %d ... Found\n", __func__, ifa->ifa_name, p_if->ip_str, index);
                strncpy(p_if->ifname, ifa->ifa_name, IF_NAMESIZE - 1);
                ifindex = index;
                break;
           } else {
                inet_ntop(AF_INET6, (void*)&psa->sin6_addr, str_addr, INET6_ADDRSTRLEN);
                __DBG("%s: Interface: %s, address: %s, ifindex: %d, next: %p\n", __func__, ifa->ifa_name, str_addr, index, ifa->ifa_next);
           }
        }
    }
    freeifaddrs(ifap);
    return ifindex;
}

static int open_raw_socket_tx(struct wfc_router_interface* p_if, unsigned char protocol, int* p_err) {
    int sk = -1;
    int enable = 1;
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    struct sockaddr* psa;
    int len;
    int err = 0;

#ifdef PKTRT_TEST
    return g_tx_socket;
#else
    sk = socket(p_if->ipv4 ? AF_INET : AF_INET6, SOCK_RAW, protocol);
    if (sk < 0) {
        *p_err = errno;
        __ERROR("%s: Failed to open socket for %d (%d and %d)", __func__, protocol, sk, errno);
        return sk;
    }
    err = setsockopt(sk, p_if->ipv4 ? IPPROTO_IP : IPPROTO_IPV6, p_if->ipv4 ? IP_HDRINCL : IPV6_HDRINCL, &enable, sizeof(enable));
    if (err < 0) {
        *p_err = errno;
        __ERROR("%s: Failed to set options for socket (%d and %d)", __func__, err, errno);

        close(sk);
        return err;
    }
    if (p_if->tx_bind_to_device) {
        struct ifreq ifr;

        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, p_if->ifname, sizeof(ifr.ifr_name) - 1);

        err = setsockopt(sk, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr));
        if (err < 0) {
            *p_err = errno;
            __ERROR("%s: Failed to bind socket to device (%d and %d)", __func__, err, errno);

            close(sk);
            return err;
        }
    }
    if (p_if->ipv4) {
        sa4.sin_family = AF_INET;
        sa4.sin_port = 0;
        memcpy(&sa4.sin_addr, p_if->ip, ROUTER_IPV4_ADDR_SIZE);

        psa = (struct sockaddr*)&sa4;
        len = sizeof(struct sockaddr_in);
    } else {
        sa6.sin6_family = AF_INET6;
        sa6.sin6_port = 0;
        sa6.sin6_flowinfo = 0;
        sa6.sin6_scope_id = 0;
        memcpy(&sa6.sin6_addr, p_if->ip, ROUTER_IPV6_ADDR_SIZE);

        psa = (struct sockaddr*)&sa6;
        len = sizeof(struct sockaddr_in6);
    }
    err = bind(sk, psa, len);
    if (err < 0) {
        *p_err = errno;
        __ERROR("%s: Failed to bind socket (%d and %d)", __func__, err, errno);

        close(sk);
        return err;
    }

    __INFO("%s: TX AF_INET/AF_INET6 Socket %d opened successfully for interface=%s, ip=%s and protocol=%s. Bound=%s", __func__,
        sk, p_if->ifname, p_if->ip_str, convert_protocol_to_string(protocol), p_if->tx_bind_to_device ? "true" : "false");
    return sk;
#endif
}

static int is_not_xfrm_device(const char* name)
{
    int index = 0;

    if (sscanf(name, "epdg%d", &index) > 0) {
        return (index < 10);
    }
    return 0;
}

static int open_raw_socket_rx(struct wfc_router_interface* p_if, int* p_err) {
    int sk = -1;
    struct sockaddr_ll saddr;
    struct sockaddr* psa;
    int len;
    int err = 0;

#ifdef PKTRT_TEST
    return g_rx_socket;
#else
    int ifindex = find_ifindex(p_if);
    if (ifindex < 0) {
        __ERROR("%s: Failed to find_ifindex(%d)", __func__, ifindex);
        return sk;
    }
    p_if->ifindex = ifindex;

    sk = socket(AF_PACKET, SOCK_DGRAM, p_if->ipv4 ? htons(ETH_P_IP) : htons(ETH_P_IPV6));
    if (sk < 0) {
        *p_err = errno;
        __ERROR("%s: Failed to open socket for %s (%d and %d)", __func__, p_if->ip_str, sk, errno);
        return sk;
    }

    saddr.sll_family = AF_PACKET;
    saddr.sll_protocol = p_if->ipv4 ? htons(ETH_P_IP) : htons(ETH_P_IPV6);
    if (is_not_xfrm_device(p_if->ifname)) {
        snprintf(p_if->ifname, IF_NAMESIZE, "i%d", ifindex);
        saddr.sll_ifindex = 0;
    }
    else {
        saddr.sll_ifindex = ifindex;
        p_if->tx_bind_to_device = 1;
    }
    psa = (struct sockaddr*)&saddr;
    len = sizeof(struct sockaddr_ll);
    err = bind(sk, psa, len);
    if (err < 0) {
        *p_err = errno;
        __ERROR("%s: Failed to bind socket (%d and %d)", __func__, err, errno);

        close(sk);
        return err;
    }

    __INFO("%s: RX AF_PACKET Socket %d opened successfully for interface=%s, ip=%s", __func__, sk, p_if->ifname, p_if->ip_str);
    return sk;
#endif
}

static void send_netlink_route_command(int cmd, int ifindex, unsigned char* src, unsigned char* dst, int ipv4) {
#define NETLINK_BUFFER_SIZE 1024

    unsigned char buf[NETLINK_BUFFER_SIZE];
    struct nlmsghdr* hdr;
    struct rtmsg* msg;
    struct rtattr* attr;
    int addrlen = ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE;

    memset(buf, 0, NETLINK_BUFFER_SIZE);

    hdr = (struct nlmsghdr*)buf;
    hdr->nlmsg_type   = cmd;
    hdr->nlmsg_flags  = NLM_F_REQUEST | (cmd == RTM_NEWROUTE ? (NLM_F_CREATE | NLM_F_EXCL) : 0);
    hdr->nlmsg_len    = NLMSG_LENGTH(sizeof(struct rtmsg));
    msg = (struct rtmsg *)NLMSG_DATA(hdr);
    msg->rtm_family   = ipv4 ? AF_INET : AF_INET6;
    msg->rtm_dst_len  = ipv4 ? ROUTER_IPV4_PREFIX_LEN : ROUTER_IPV6_PREFIX_LEN;
    msg->rtm_table    = RT_TABLE_LOCAL;
    msg->rtm_protocol = RTPROT_STATIC;
    msg->rtm_scope    = RT_SCOPE_UNIVERSE;
    msg->rtm_type     = RTN_UNICAST;

#define RTA_ADD_ATTR(_a, _d, _l)\
    attr = (struct rtattr*)(buf + NLMSG_ALIGN(hdr->nlmsg_len));\
    attr->rta_type = _a;\
    attr->rta_len  = RTA_LENGTH(_l);\
    memcpy(RTA_DATA(attr), (void*)_d, _l);\
    hdr->nlmsg_len = NLMSG_ALIGN(hdr->nlmsg_len) + attr->rta_len;

    // destination
    RTA_ADD_ATTR(RTA_DST, dst, addrlen);
    // source
    RTA_ADD_ATTR(RTA_PREFSRC, src, addrlen);
    // interface
    RTA_ADD_ATTR(RTA_OIF, &ifindex, sizeof(uint32_t));

    if (__IS_INFO) {
        char str_src[INET6_ADDRSTRLEN];
        char str_dst[INET6_ADDRSTRLEN];
        if (ipv4) {
            inet_ntop(AF_INET, (void*)src, str_src, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET, (void*)dst, str_dst, INET6_ADDRSTRLEN);
        } else {
            inet_ntop(AF_INET6, (void*)src, str_src, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, (void*)dst, str_dst, INET6_ADDRSTRLEN);
        }
        __INFO("%s: %s route from %s to %s on %d", __func__, cmd == RTM_NEWROUTE ? "Create" : "Delete", str_src, str_dst, ifindex);
    }
    if (write(__router.nt_sock, buf, NLMSG_ALIGN(hdr->nlmsg_len)) < 0) {
        __WARN("%s: failed to write to netlink socket %d", __func__, errno);
    }
}

static __inline__ unsigned char convert_protocol_to_network(unsigned char protocol) {
    switch (protocol) {
        case ROUTER_PROTO_TCP: return IPPROTO_TCP;
        case ROUTER_PROTO_UDP: return IPPROTO_UDP;
        case ROUTER_PROTO_ESP: return IPPROTO_ESP;
        default:
            /* should not be here */
            __WARN("%s: invalid protocol %d", __func__, protocol);
    }
    return 0;
}

/*
    List
*/
static struct wfc_router_interface* find_interface(unsigned char* ip) {
    if (ip != NULL) {
        struct wfc_router_interface* np = NULL;
        LIST_FOREACH(np, &__router.interfaces, entries) {
            if (!memcmp(np->ip, ip, np->ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE)) {
                __VERBOSE("%s found %p", __func__, np);
                return np;
            }
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_iptables_rule* find_iptables_rule(struct wfc_router_interface* p_if, unsigned char* ip) {
    struct wfc_router_iptables_rule* np = NULL;
    LIST_FOREACH(np, &p_if->iptables, entries) {
        if (!memcmp(np->ip, ip, p_if->ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE)) {
            __VERBOSE("%s found %p", __func__, np);
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_rttables_rule* find_rttables_rule(struct wfc_router_interface* p_if, unsigned char* ip) {
    struct wfc_router_rttables_rule* np = NULL;
    LIST_FOREACH(np, &p_if->rttables, entries) {
        if (!memcmp(np->ip, ip, p_if->ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE)) {
            __VERBOSE("%s found %p", __func__, np);
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_protocol* find_protocol(struct wfc_router_interface* p_if, unsigned char protocol) {
    struct wfc_router_protocol* np = NULL;
    LIST_FOREACH(np, &p_if->protocols, entries) {
        if (np->protocol == protocol) {
            __VERBOSE("%s found %p", __func__, np);
            return np;
        }
    }
    return NULL;
}

static __inline__ int find_socket(struct wfc_router_interface* p_if, unsigned char protocol) {
    struct wfc_router_protocol* np = NULL;
    LIST_FOREACH(np, &p_if->protocols, entries) {
        if (np->protocol == protocol) {
            __VERBOSE("%s found np(%p), protocol=%s, sk_tx=%d", __func__, np, convert_protocol_to_string(np->protocol), np->sk_tx);
            return np->sk_tx;
        }
    }
    return (-1);
}

static __inline__ struct wfc_router_protocol_rule* find_forward_rule_by_ports_full_match(struct wfc_router_protocol* p_proto, unsigned short dst, unsigned short src) {
    struct wfc_router_protocol_rule* np = NULL;
    LIST_FOREACH(np, &p_proto->fwd, entries) {
        if ((np->prp.ports.dst == dst) && (np->prp.ports.src == src)) {
            __VERBOSE("%s found %p", __func__, np);
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_protocol_rule* find_forward_rule_by_ports(struct wfc_router_protocol* p_proto, unsigned short dst, unsigned short src) {
    struct wfc_router_protocol_rule* np = NULL;

    __VERBOSE("%s find in %p: dst=%d, src=%d", __func__, p_proto, ntohs(dst), ntohs(src));

    LIST_FOREACH(np, &p_proto->fwd, entries) {
        __VERBOSE("%s %p dst = %d", __func__, np, ntohs(np->prp.ports.dst));
        if (np->prp.ports.dst == dst) {
            if (np->prp.ports.dst) {
                /* not zero, check source */
                __VERBOSE("%s %p src = %d", __func__, np, ntohs(np->prp.ports.src));
                if (np->prp.ports.src && (np->prp.ports.src != src)) {
                    /* source not zero and not matched */
                    continue;
                }
            }
            __DBG("%s found %p %s, dst=%d, src=%d", __func__, np, convert_protocol_to_string(p_proto->protocol), ntohs(np->prp.ports.dst), ntohs(np->prp.ports.src));
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_protocol_rule* find_forward_rule_by_spi(struct wfc_router_protocol* p_proto, unsigned int spi) {
    struct wfc_router_protocol_rule* np = NULL;
    LIST_FOREACH(np, &p_proto->fwd, entries) {
        if (np->prp.spi == spi) {
            __DBG("%s found %p ESP, spi=%#x", __func__, np, ntohl(spi));
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_protocol_rule* find_drop_rule_by_ports_full_match(struct wfc_router_protocol* p_proto, unsigned short dst, unsigned short src) {
    struct wfc_router_protocol_rule* np = NULL;
    LIST_FOREACH(np, &p_proto->drop, entries) {
        if ((np->prp.ports.dst == dst) && (np->prp.ports.src == src)) {
            __DBG("%s found %p %s, dst=%d, src=%d", __func__, np, convert_protocol_to_string(p_proto->protocol), ntohs(np->prp.ports.dst), ntohs(np->prp.ports.src));
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_protocol_rule* find_drop_rule_by_ports(struct wfc_router_protocol* p_proto, unsigned short dst, unsigned short src) {
    struct wfc_router_protocol_rule* np = NULL;

    __VERBOSE("%s find in %p, dst=%d, src=%d", __func__, p_proto, ntohs(dst), ntohs(src));

    LIST_FOREACH(np, &p_proto->drop, entries) {
        __VERBOSE("%s %p dst = %d", __func__, np, ntohs(np->prp.ports.dst));
        if (np->prp.ports.dst == dst) {
            if (np->prp.ports.dst) {
                /* not zero, check source */
                __VERBOSE("%s %p src = %d", __func__, np, ntohs(np->prp.ports.src));
                if (np->prp.ports.src && (np->prp.ports.src != src)) {
                    /* source not zero and not matched */
                    continue;
                }
            }
            __DBG("%s found %p %s, dst=%d, src=%d", __func__, np, convert_protocol_to_string(p_proto->protocol), ntohs(np->prp.ports.dst), ntohs(np->prp.ports.src));
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_protocol_rule* find_drop_rule_by_spi(struct wfc_router_protocol* p_proto, unsigned int spi) {
    struct wfc_router_protocol_rule* np = NULL;
    LIST_FOREACH(np, &p_proto->drop, entries) {
        if (np->prp.spi == spi) {
            __DBG("%s found %p ESP, spi=%#x", __func__, np, ntohl(spi));
            return np;
        }
    }
    return NULL;
}

static __inline__ struct wfc_router_packet_fragment* find_fragment(struct wfc_router_protocol* p_proto, struct wfc_router_packet_fragment* p_frg, int ipv4) {
    struct wfc_router_packet_fragment* np = NULL;
    LIST_FOREACH(np, &p_proto->fragments, entries) {
        if (ipv4) {
            if (np->id.v4 == p_frg->id.v4) {
                return np;
            }
        } else {
            if (np->id.v6 == p_frg->id.v6) {
                return np;
            }
        }
    }
    return NULL;
}

static void execute_iptables_command(const char* cmd, const char* action, const char* chain, unsigned char* dst, int ipv4) {
    pid_t pid;
    int link[2];
    const char* path = "/system/bin/";
    char path_cmd[512] = {0};
    char ip_str[INET6_ADDRSTRLEN] = {0};

    snprintf(path_cmd, sizeof(path_cmd), "%s%s", path, cmd);
    inet_ntop(ipv4 ? AF_INET : AF_INET6, (void*)dst, ip_str, INET6_ADDRSTRLEN);
    __INFO("execute %s -w %s %s -p tcp --tcp-flags RST RST -d %s -j DROP", cmd, action, chain, ip_str);

    if (pipe(link) < 0) {
        __ERROR("Failed to open pipe: %s", strerror(errno));
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* CHILD */
        dup2(link[1], STDOUT_FILENO);
        dup2(link[1], STDERR_FILENO);
        close(link[0]);
        close(link[1]);

        int err = execl(path_cmd, cmd, "-w", action, chain, "-p", "tcp", "--tcp-flags", "RST", "RST", "-d", ip_str, "-j", "DROP", (char*)NULL);
        if (err < 0) {
            fprintf(stderr, "Failed to exec: %s", strerror(errno));
        }
        _exit(0);
    } else {
        close(link[1]);
        if (pid < 0) {
            __ERROR("Failed to fork: %s", strerror(errno));
            close(link[0]);
        } else {
            char buffer[1024] = {0};
            int bytes = 0;
            while (0 != (bytes = read(link[0], buffer, 1024))) {
                __INFO("Exec output: %s", buffer);
                memset(buffer, 0, 1024);
            }
            wait(NULL);
            close(link[0]);
        }
    }
}

// ipXtables -D OUTPUT -p tcp --tcp-flags RST RST -d %s -j DROP
static void iptables_delete_entry(unsigned char* dst) {
    execute_iptables_command("iptables-wrapper-1.0", "-D", "oem_out", dst, 1);
}

static void ip6tables_delete_entry(unsigned char* dst) {
    execute_iptables_command("ip6tables-wrapper-1.0", "-D", "oem_out", dst, 0);
}

static void delete_iptables_rules(struct wfc_router_interface* p_if) {
    struct wfc_router_iptables_rule* np = LIST_FIRST(&p_if->iptables);
    while (np != NULL) {
        struct wfc_router_iptables_rule* next = LIST_NEXT(np, entries);
        LIST_REMOVE(np, entries);
        if (p_if->ipv4) {
            iptables_delete_entry(np->ip);
        } else {
            ip6tables_delete_entry(np->ip);
        }
        free(np);
        np = next;
    }
}

// ipXtables -A OUTPUT -p tcp --tcp-flags RST RST -d %s -j DROP
static int iptables_append_entry(unsigned char* dst) {
    execute_iptables_command("iptables-wrapper-1.0", "-A", "oem_out", dst, 1);
    return 0;
}

static int ip6tables_append_entry(unsigned char* dst) {
    execute_iptables_command("ip6tables-wrapper-1.0", "-A", "oem_out", dst, 0);
    return 0;
}

static void make_iptables_rule(struct wfc_router_interface* p_if, unsigned char* ip) {
    struct wfc_router_iptables_rule* p_iptbl = find_iptables_rule(p_if, ip);
    if (p_iptbl == NULL) {
        int err = 0;

        p_iptbl = (struct wfc_router_iptables_rule*)malloc(sizeof(struct wfc_router_iptables_rule));
        if (p_iptbl == NULL) {
            return;
        }
        if (p_if->ipv4) {
            err = iptables_append_entry(ip);
        } else {
            err = ip6tables_append_entry(ip);
        }
        if (err != 0) {
            free(p_iptbl);
        } else {
            memcpy(p_iptbl->ip, ip, p_if->ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE);
            LIST_INSERT_HEAD(&p_if->iptables, p_iptbl, entries);
        }
    }
}

static void delete_rttables_rules(struct wfc_router_interface* p_if) {
    struct wfc_router_rttables_rule* np = LIST_FIRST(&p_if->rttables);
    while (np != NULL) {
        struct wfc_router_rttables_rule* next = LIST_NEXT(np, entries);
        LIST_REMOVE(np, entries);

        send_netlink_route_command(RTM_DELROUTE, p_if->ifindex, p_if->ip, np->ip, p_if->ipv4);

        free(np);
        np = next;
    }
}

static void make_rttables_rule(struct wfc_router_interface* p_if, unsigned char* ip) {
    struct wfc_router_rttables_rule* rule = find_rttables_rule(p_if, ip);
    if (rule == NULL) {
        rule = (struct wfc_router_rttables_rule*)malloc(sizeof(struct wfc_router_rttables_rule));
        if (rule == NULL) {
            return;
        }
        memcpy(rule->ip, ip, p_if->ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE);
        LIST_INSERT_HEAD(&p_if->rttables, rule, entries);

        send_netlink_route_command(RTM_NEWROUTE, p_if->ifindex, p_if->ip, ip, p_if->ipv4);
    }
}

static int open_dummy_socket(struct wfc_router_interface* p_if, int sin_port, int* p_err) {
    int sk = -1;
    int enable = 1;
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    struct sockaddr* psa;
    int len;
    int err = 0;

#ifdef PKTRT_TEST
    return g_dummy_socket;
#else
    do {
        sk = socket(p_if->ipv4 ? AF_INET : AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        if (sk < 0) {
            *p_err = errno;
            __ERROR("%s: Failed to open dummy socket for %s (%d and %d)", __func__, p_if->ip_str, sk, errno);
            break;
        }

        err = setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
        if (err < 0) {
            *p_err = errno;
            __ERROR("%s: Failed to set options for socket (%d and %d)", __func__, err, errno);
            close(sk);
            break;
        }

        if (p_if->ipv4) {
            memset((char *)&sa4, 0, sizeof(sa4));

            sa4.sin_family = AF_INET;
            sa4.sin_port = sin_port;
            memcpy(&sa4.sin_addr, p_if->ip, ROUTER_IPV4_ADDR_SIZE);

            psa = (struct sockaddr*)&sa4;
            len = sizeof(struct sockaddr_in);
        } else {
            memset((char *)&sa6, 0, sizeof(sa6));

            sa6.sin6_family = AF_INET6;
            sa6.sin6_port = sin_port;
            memcpy(&sa6.sin6_addr, p_if->ip, ROUTER_IPV6_ADDR_SIZE);

            psa = (struct sockaddr*)&sa6;
            len = sizeof(struct sockaddr_in6);
        }

        err = bind(sk, psa, len);
        if (err < 0) {
            *p_err = errno;
            __ERROR("%s: Failed to bind dummy socket (sk:%d, addr:%s, port:%d), err:%d and errno:%d", __func__, sk, p_if->ip_str, sin_port, err, errno);
            close(sk);
            break;
        }
    } while(0);

    if (*p_err) sk = -1;

    __INFO("%s dummy socket %d opened for %s, port %d", __func__, sk, p_if->ip_str, ntohs(sin_port));
    return sk;
#endif
}

static void close_dummy_socket(struct wfc_router_protocol_rule* p_rule) {
    if (p_rule->dummy_sk > 0) {
        close(p_rule->dummy_sk);

        __INFO("%s dummy socket %d closed", __func__, p_rule->dummy_sk);
        p_rule->dummy_sk = -1;
    }
}

static int add_rule_with_ports(struct wfc_router_protocol* p_proto, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    int err = 0;
    switch (p_dsc->rule) {
        case ROUTER_RULE_FWD:
        {
            struct wfc_router_protocol_rule* p_rule = NULL;

            /* check if super rule exists */
            if (p_proto->fwd_all) {
                __WARN("%s: Forwarding super rule exists for %d protocol, not required to add new", __func__, p_proto->protocol);
            }
            /* check if it already exists */
            p_rule = find_forward_rule_by_ports(p_proto, p_dsc->param.ports.dst, p_dsc->param.ports.src);
            if (p_rule != NULL) {
                __ERROR("%s: Forward rule with ports %d, %d already exists", __func__, ntohs(p_dsc->param.ports.dst), ntohs(p_dsc->param.ports.src));
                return EEXIST;
            }
            /* check if it conflicts */
            p_rule = find_drop_rule_by_ports(p_proto, p_dsc->param.ports.dst, p_dsc->param.ports.src);
            if (p_rule != NULL) {
                __ERROR("%s: Forward rule with ports %d, %d conflicts with drop rule", __func__, ntohs(p_dsc->param.ports.dst), ntohs(p_dsc->param.ports.src));
                return ENOTUNIQ;
            }

            /* create rule */
            p_rule = (struct wfc_router_protocol_rule *)malloc(sizeof(struct wfc_router_protocol_rule));
            if (p_rule == NULL) {
                err = errno;

                __ERROR("%s: Failed to allocate memory for forward rule", __func__);
                return err;
            }
            p_rule->prp.ports.dst = p_dsc->param.ports.dst;
            p_rule->prp.ports.src = p_rule->prp.ports.dst ? p_dsc->param.ports.src : 0;
            p_rule->dummy_sk = -1;
            LIST_INSERT_HEAD(&p_proto->fwd, p_rule, entries);

            if ((IPPROTO_UDP == p_proto->protocol) && p_rule->prp.ports.dst) {
                /* create dummy socket */
                p_rule->dummy_sk = open_dummy_socket(p_proto->parent, p_rule->prp.ports.dst, &err);
            }

            /* set forward all rule flag */
            if (p_rule->prp.ports.dst == 0) {
                p_proto->fwd_all++;
            }
            __INFO("%s: Created forward rule for ports : dst=%d, src=%d", __func__, ntohs(p_rule->prp.ports.dst), ntohs(p_rule->prp.ports.src));
            break;
        }
        case ROUTER_RULE_DROP:
        {
            struct wfc_router_protocol_rule* p_rule = NULL;

            /* check for invalid request */
            if (p_dsc->param.ports.dst == 0) {
                __ERROR("%s: Invalid rule, DROP ALL", __func__);
                return EINVAL;
            }
            /* check if it already exists */
            p_rule = find_drop_rule_by_ports(p_proto, p_dsc->param.ports.dst, p_dsc->param.ports.src);
            if (p_rule != NULL) {
                __ERROR("%s: Drop rule with ports %d, %d already exists", __func__, ntohs(p_dsc->param.ports.dst), ntohs(p_dsc->param.ports.src));
                return EEXIST;
            }
            /* check if it conflicts */
            p_rule = find_forward_rule_by_ports(p_proto, p_dsc->param.ports.dst, p_dsc->param.ports.src);
            if (p_rule != NULL) {
                __ERROR("%s: Drop rule with ports %d, %d conflicts with forward rule", __func__, ntohs(p_dsc->param.ports.dst), ntohs(p_dsc->param.ports.src));
                return ENOTUNIQ;
            }

            /* create rule */
            p_rule = (struct wfc_router_protocol_rule *)malloc(sizeof(struct wfc_router_protocol_rule));
            if (p_rule == NULL) {
                err = errno;

                __ERROR("%s: Failed to allocate memory for drop rule", __func__);
                return err;
            }
            p_rule->prp.ports.dst = p_dsc->param.ports.dst;
            p_rule->prp.ports.src = p_dsc->param.ports.src;
            LIST_INSERT_HEAD(&p_proto->drop, p_rule, entries);

            __INFO("%s: Created drop rule for ports : dst=%d, src=%d", __func__, ntohs(p_rule->prp.ports.dst), ntohs(p_rule->prp.ports.src));
            break;
        }
        default:
            __ERROR("%s: Invalid rule command %d", __func__, p_dsc->rule);
            return EINVAL;
    }
    return err;
}

static int add_rule_with_spi(struct wfc_router_protocol* p_proto, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    int err = 0;
    switch (p_dsc->rule) {
        case ROUTER_RULE_FWD:
        {
            struct wfc_router_protocol_rule* p_rule = NULL;

            /* check if super rule exists */
            if (p_proto->fwd_all) {
                __WARN("%s: Forwarding super rule exists for %d protocol, not required to add new", __func__, p_proto->protocol);
            }
            /* check if it already exists */
            p_rule = find_forward_rule_by_spi(p_proto, p_dsc->param.spi);
            if (p_rule != NULL) {
                __ERROR("%s: Forward rule with SPI %x already exists", __func__, ntohl(p_dsc->param.spi));
                return EEXIST;
            }
            /* check if it conflicts */
            p_rule = find_drop_rule_by_spi(p_proto, p_dsc->param.spi);
            if (p_rule != NULL) {
                __ERROR("%s: Forward rule with SPI %x conflicts with drop rule", __func__, ntohl(p_dsc->param.spi));
                return ENOTUNIQ;
            }

            /* create rule */
            p_rule = (struct wfc_router_protocol_rule *)malloc(sizeof(struct wfc_router_protocol_rule));
            if (p_rule == NULL) {
                err = errno;

                __ERROR("%s: Failed to allocate memory for forward rule", __func__);
                return err;
            }
            p_rule->prp.spi = p_dsc->param.spi;
            LIST_INSERT_HEAD(&p_proto->fwd, p_rule, entries);

            /* set forward all rule flag */
            if (p_rule->prp.spi == 0) {
                p_proto->fwd_all++;
            }
            __INFO("%s: Created forward rule for SPI %#x", __func__, ntohl(p_rule->prp.spi));
            break;
        }
        case ROUTER_RULE_DROP:
        {
            struct wfc_router_protocol_rule* p_rule = NULL;

            /* check for invalid request */
            if (p_dsc->param.spi == 0) {
                __ERROR("%s: Invalid rule, DROP ALL", __func__);
                return EINVAL;
            }
            /* check if it already exists */
            p_rule = find_drop_rule_by_spi(p_proto, p_dsc->param.spi);
            if (p_rule != NULL) {
                __ERROR("%s: Drop rule with SPI %x already exists", __func__, ntohl(p_dsc->param.spi));
                return EEXIST;
            }
            /* check if it conflicts */
            p_rule = find_forward_rule_by_spi(p_proto, p_dsc->param.spi);
            if (p_rule != NULL) {
                __ERROR("%s: Drop rule with SPI %x conflicts with forward rule", __func__, ntohl(p_dsc->param.spi));
                return ENOTUNIQ;
            }

            /* create rule */
            p_rule = (struct wfc_router_protocol_rule *)malloc(sizeof(struct wfc_router_protocol_rule));
            if (p_rule == NULL) {
                err = errno;

                __ERROR("%s: Failed to allocate memory for drop rule", __func__);
                return err;
            }
            p_rule->prp.spi = p_dsc->param.spi;
            LIST_INSERT_HEAD(&p_proto->drop, p_rule, entries);

            __INFO("%s: Created drop rule for SPI %#x", __func__, ntohl(p_rule->prp.spi));
            break;
        }
        default:
            __ERROR("%s: Invalid rule command %d", __func__, p_dsc->rule);
            return EINVAL;
    }
    return err;
}

static __inline__ int add_rule(struct wfc_router_protocol* p_proto, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    __DBG("%s to %p", __func__, p_proto);

    if (p_proto->protocol == IPPROTO_ESP) {
        return add_rule_with_spi(p_proto, p_dsc);
    }
    return add_rule_with_ports(p_proto, p_dsc);
}

static __inline__ void delete_list_rules(struct wfc_router_protocol_rule* first, int close_socket)
{
    struct wfc_router_protocol_rule* np = first;
    while (np != NULL) {
        struct wfc_router_protocol_rule* next = LIST_NEXT(np, entries);
        LIST_REMOVE(np, entries);

        if (close_socket) close_dummy_socket(np);

        free(np);
        np = next;
    }
}

static int delete_rule_with_ports(struct wfc_router_protocol* p_proto, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    int err = 0;
    switch (p_dsc->rule) {
        case ROUTER_RULE_FWD:
        {
            if (p_dsc->param.ports.dst) {
                /* specific ports */
                struct wfc_router_protocol_rule* p_rule = find_forward_rule_by_ports_full_match(p_proto, p_dsc->param.ports.dst, p_dsc->param.ports.src);
                if (p_rule == NULL) {
                    __ERROR("%s: Forward rule with ports %d, %d not exists", __func__, ntohs(p_dsc->param.ports.dst), ntohs(p_dsc->param.ports.src));
                    return ENOENT;
                }
                close_dummy_socket(p_rule);

                /* remove rule */
                LIST_REMOVE(p_rule, entries);
                __INFO("%s: Deleted forward rule for ports : dst=%d, src=%d", __func__, ntohs(p_rule->prp.ports.dst), ntohs(p_rule->prp.ports.src));

                free(p_rule);
            } else {
                /* delete all */
                p_proto->fwd_all = 0;
                delete_list_rules(LIST_FIRST(&p_proto->fwd), 1);
                __INFO("%s: Deleted all forward rules", __func__);
            }
            break;
        }
        case ROUTER_RULE_DROP:
        {
            if (p_dsc->param.ports.dst) {
                /* specific ports */
                struct wfc_router_protocol_rule* p_rule = find_drop_rule_by_ports_full_match(p_proto, p_dsc->param.ports.dst, p_dsc->param.ports.src);
                if (p_rule == NULL) {
                    __ERROR("%s: Drop rule with ports %d, %d not exists", __func__, ntohs(p_dsc->param.ports.dst), ntohs(p_dsc->param.ports.src));
                    return ENOENT;
                }

                /* remove rule */
                LIST_REMOVE(p_rule, entries);
                __INFO("%s: Deleted drop rule for ports : dst=%d, src=%d", __func__, ntohs(p_rule->prp.ports.dst), ntohs(p_rule->prp.ports.src));

                free(p_rule);
            } else {
                /* delete all */
                delete_list_rules(LIST_FIRST(&p_proto->drop), 0);
                __INFO("%s: Deleted all drop rules", __func__);
            }
            break;
        }
        default:
            __ERROR("%s: Invalid rule command %d", __func__, p_dsc->rule);
            return EINVAL;
    }
    return err;
}

static int delete_rule_with_spi(struct wfc_router_protocol* p_proto, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    int err = 0;
    switch (p_dsc->rule) {
        case ROUTER_RULE_FWD:
        {
            if (p_dsc->param.spi) {
                /* specific value */
                struct wfc_router_protocol_rule* p_rule = find_forward_rule_by_spi(p_proto, p_dsc->param.spi);
                if (p_rule == NULL) {
                    __ERROR("%s: Forward rule with SPI %x not exists", __func__, ntohl(p_dsc->param.spi));
                    return ENOENT;
                }

                /* remove rule */
                LIST_REMOVE(p_rule, entries);
                __INFO("%s: Deleted forward rule for SPI %#x", __func__, ntohl(p_rule->prp.spi));

                free(p_rule);
            } else {
                /* delete all */
                p_proto->fwd_all = 0;
                delete_list_rules(LIST_FIRST(&p_proto->fwd), 0);
                __INFO("%s: Deleted all forward rules", __func__);
            }
            break;
        }
        case ROUTER_RULE_DROP:
        {
            if (p_dsc->param.spi) {
                /* specific value */
                struct wfc_router_protocol_rule* p_rule = find_drop_rule_by_spi(p_proto, p_dsc->param.spi);
                if (p_rule == NULL) {
                    __ERROR("%s: Drop rule with SPI %x not exists", __func__, ntohl(p_dsc->param.spi));
                    return ENOENT;
                }

                /* remove rule */
                LIST_REMOVE(p_rule, entries);
                __INFO("%s: Deleted drop rule for SPI %#x", __func__, ntohl(p_rule->prp.spi));

                free(p_rule);
            } else {
                /* delete all */
                delete_list_rules(LIST_FIRST(&p_proto->drop), 0);
                __INFO("%s: Deleted all drop rules", __func__);
            }
            break;
        }
        default:
            __ERROR("%s: Invalid rule command %d", __func__, p_dsc->rule);
            return EINVAL;
    }
    return err;
}

static __inline__ int delete_rule(struct wfc_router_protocol* p_proto, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    __DBG("%s from %p", __func__, p_proto);

    if (p_proto->protocol == IPPROTO_ESP) {
        return delete_rule_with_spi(p_proto, p_dsc);
    }
    return delete_rule_with_ports(p_proto, p_dsc);
}

static int add_protocol(struct wfc_router_interface* p_if, p_wfc_router_protocol_rule_descriptor_t p_dsc) {
    int err = 0;

    struct wfc_router_protocol* p_proto = (struct wfc_router_protocol *)malloc(sizeof(struct wfc_router_protocol));
    if (p_proto == NULL) {
        err = errno;

        __ERROR("%s: Failed to allocate memory", __func__);
        return err;
    }

    /* fill it up */
    p_proto->fwd_all = 0;
    LIST_INIT(&p_proto->fwd);
    LIST_INIT(&p_proto->drop);
    LIST_INIT(&p_proto->fragments);

    p_proto->parent   = p_if;
    p_proto->protocol = convert_protocol_to_network(p_dsc->protocol);
    if (p_proto->protocol == 0) {
        free(p_proto);
        return EINVAL;
    }

    /* Create Socket for interface */
    p_proto->sk_tx = open_raw_socket_tx(p_if, p_proto->protocol, &err);
    if (p_proto->sk_tx < 0) {
        __ERROR("%s: Failed to open RAW TX socket", __func__);
        free(p_proto);
        return err;
    }

    err = add_rule(p_proto, p_dsc);
    if (err != 0) {
        close(p_proto->sk_tx);
        free(p_proto);
    } else {
        LIST_INSERT_HEAD(&p_if->protocols, p_proto, entries);
        __INFO("%s: Protocol %s (%p) for interface=%s is added", __func__, convert_protocol_to_string(p_proto->protocol), p_proto, p_if->ifname);
    }
    return err;
}

static void delete_protocol(struct wfc_router_protocol* p_proto) {
    /* remove from interface */
    LIST_REMOVE(p_proto, entries);

    /* close socket */
    close(p_proto->sk_tx);

    /* delete all forward rules */
    delete_list_rules(LIST_FIRST(&p_proto->fwd), 1);
    /* delete all drop rules */
    delete_list_rules(LIST_FIRST(&p_proto->drop), 0);
    /* delete all fragments */
    struct wfc_router_packet_fragment* np = LIST_FIRST(&p_proto->fragments);
    while (np != NULL) {
        struct wfc_router_packet_fragment* next = LIST_NEXT(np, entries);
        LIST_REMOVE(np, entries);

        free(np);
        np = next;
    }

    __INFO("%s: Protocol %s (%p) is removed", __func__, convert_protocol_to_string(p_proto->protocol), p_proto);
    free(p_proto);
}

static void add_fragment(struct wfc_router_protocol* p_proto, unsigned char* buf, int ipv4) {
    if (is_fragmented_packet(buf, ipv4)) {
        struct wfc_router_packet_fragment* p_frg = (struct wfc_router_packet_fragment*)malloc(sizeof(struct wfc_router_packet_fragment));
        if (p_frg == NULL) {
            return;
        }
        save_packet_id_to_fragment(p_frg, buf, ipv4);
        LIST_INSERT_HEAD(&p_proto->fragments, p_frg, entries);
    }
}

static int is_awaited_fragment(struct wfc_router_protocol* p_proto, unsigned char* buf, int ipv4) {
    struct wfc_router_packet_fragment frg;
    struct wfc_router_packet_fragment* p_frg = NULL;

    save_packet_id_to_fragment(&frg, buf, ipv4);

    p_frg = find_fragment(p_proto, &frg, ipv4);
    if (p_frg != NULL) {
        if (is_last_fragment(buf, ipv4)) {
            LIST_REMOVE(p_frg, entries);
            free(p_frg);
        }
        return 1;
    }
    return 0;
}

/*
    Misc
*/
static __inline__ int is_valid_new_request(p_wfc_router_control_request_t p_req) {
    /* DELETE not valid for first time, check add only */
    if (p_req->cmd == ROUTER_CMD_ADD) {
        /* check IP types */
        if ((p_req->ip_type == ROUTER_IP_TYPE_IPv4) ||
            (p_req->ip_type == ROUTER_IP_TYPE_IPv6)) {
            int i;
            for (i = 0; i < p_req->num_rules; i++) {
                /* FORWARD rule must be present */
                if (p_req->rules[i].rule == ROUTER_RULE_FWD) {
                    /* check protocols specified */
                    if ((p_req->rules[i].protocol >= ROUTER_PROTO_TCP) &&
                        (p_req->rules[i].protocol <= ROUTER_PROTO_ESP)) {
                        return 1;
                    }
                }
            }
        }
    }
    __WARN("%s: invalid request", __func__);
    return 0;
}

static __inline__ int is_valid_modify_request(p_wfc_router_control_request_t p_req) {
    int i;

    /* basic checks */
    if ((p_req->cmd != ROUTER_CMD_ADD) &&
        (p_req->cmd != ROUTER_CMD_DELETE)) {
        __ERROR("%s: invalid command %d", __func__, p_req->cmd);
        return 0;
    }
    if ((p_req->ip_type != ROUTER_IP_TYPE_IPv4) &&
        (p_req->ip_type != ROUTER_IP_TYPE_IPv6)) {
        __ERROR("%s: invalid IP type %d", __func__, p_req->ip_type);
        return 0;
    }
    for (i = 0; i < p_req->num_rules; i++) {
        if ((p_req->rules[i].rule != ROUTER_RULE_FWD) &&
            (p_req->rules[i].rule != ROUTER_RULE_DROP)) {
            __ERROR("%s: invalid rule type %d", __func__, p_req->rules[i].rule);
            return 0;
        }
        switch (p_req->rules[i].protocol) {
            case ROUTER_PROTO_TCP:
            case ROUTER_PROTO_UDP:
            case ROUTER_PROTO_ESP: break;
            default:
                __ERROR("%s: invalid protocol %d", __func__, p_req->rules[i].protocol);
                return 0;
        }
    }
    return 1;
}

/*
    Check received network packet and forward to modem if needed
*/
static void check_network_packet_and_forward_to_modem(struct wfc_router_interface* p_if, unsigned char* buf, int len) {
    int err = 0;
    struct wfc_router_protocol_rule* p_rule = NULL;
    p_wfc_router_data_message_t p_msg = (p_wfc_router_data_message_t)buf;
    struct wfc_router_protocol* p_proto = find_protocol(p_if, get_protocol_from_packet(p_msg->data, p_if->ipv4));
    if (p_proto == NULL)
        /* drop */
        return;

    if (is_first_fragment(p_msg->data, p_if->ipv4)) {
        /* check rules */
        if (p_proto->protocol == IPPROTO_ESP) {
            unsigned int spi = get_spi_from_packet(buf + ROUTER_NET_BUFFER_OFFSET, p_proto->parent->ipv4);

            /* if all enabled, check for exceptions */
            if (p_proto->fwd_all) {
                p_rule = find_drop_rule_by_spi(p_proto, spi);
                if (p_rule != NULL) {
                    /* drop */
                    __WARN("%s: Drop by rule(0x%p) for ESP", __func__, p_rule);
                    return;
                }
            } else {
                p_rule = find_forward_rule_by_spi(p_proto, spi);
                if (p_rule == NULL) {
                    /* drop */
                    __WARN("%s: Drop by no rule for ESP", __func__);
                    return;
                }
            }
        } else {
            unsigned short dst, src;

            err = get_ports_from_packet(buf + ROUTER_NET_BUFFER_OFFSET, p_proto->protocol, p_proto->parent->ipv4, &dst, &src);
            if (err < 0) {
                __ERROR("%s: Failed to extract ports from network packet", __func__);
                return;
            }
            /* if all enabled, check for exceptions */
            if (p_proto->fwd_all) {
                p_rule = find_drop_rule_by_ports(p_proto, dst, src);
                if (p_rule != NULL) {
                    /* drop */
                    __WARN("%s: Drop by rule(0x%p)", __func__, p_rule);
                    return;
                }
            } else {
                p_rule = find_forward_rule_by_ports(p_proto, dst, src);
                if (p_rule == NULL) {
                    /* drop */
                    __WARN("%s: Drop by no rule", __func__);
                    return;
                }
            }
        }
        add_fragment(p_proto, p_msg->data, p_if->ipv4);
    } else {
        if (!is_awaited_fragment(p_proto, p_msg->data, p_if->ipv4)) {
            if (p_proto->protocol == IPPROTO_ESP) {
                /*
                    unexpected ESP packet received
                    forward it anyway because it happened in field
                */
                __WARN("%s: Unexpected ESP fragment", __func__);
            } else {
                /* drop */
                __WARN("%s: Drop by unexpected fragment", __func__);
                return;
            }
        }
    }

    /* fill the header */
    p_msg->type   = ROUTER_MSG_DATA;
    p_msg->length = len;

    /* recalculate checksum for big packets */
    calculate_tcp_checksum(buf + ROUTER_NET_BUFFER_OFFSET, len, p_proto->protocol, p_if->ipv4);
    /* send */
    pthread_mutex_lock(&__router.io_lock);
    __INFO("%s: write %d bytes", __func__, len);
    err = write(__router.io_dev, buf, len + sizeof(wfc_router_data_message_t));
    pthread_mutex_unlock(&__router.io_lock);
    if (err < 0) {
        __WARN("%s: Failed to send packet to modem (%d)", __func__, err);
    }
}

/*
    Forward packet to network
*/
static int forward_data_packet_to_network(unsigned char* buf, int len) {
    struct wfc_router_interface* p_if = NULL;
    p_wfc_router_data_message_t p_pkt = (p_wfc_router_data_message_t)buf;

    if ((unsigned long)len < (p_pkt->length + sizeof(wfc_router_data_message_t))) {
        __ERROR("%s: Input buffer is too short (%d for %d)", __func__, len, p_pkt->length);
        return (-1);
    }

    log_packet_data("DATA_SEND", len, buf);

    /* find interface it belongs to */
    p_if = find_interface(get_source_ip_from_packet(p_pkt->data));
    if (p_if != NULL) {
        /* find protocol socket */
        int sk = find_socket(p_if, get_protocol_from_packet(p_pkt->data, p_if->ipv4));
        if (sk > 0) {
            unsigned char addr_buf[sizeof(struct sockaddr_in6)];
            struct sockaddr* p_sa = (struct sockaddr*)addr_buf;
            int sa_len = fill_sa_from_packet(p_pkt->data, addr_buf, p_if->ipv4);

            make_iptables_rule(p_if, get_destination_ip_from_packet(p_pkt->data, p_if->ipv4));
            if (p_if->tx_bind_to_device == 0) {
                make_rttables_rule(p_if, get_destination_ip_from_packet(p_pkt->data, p_if->ipv4));
            }

            __INFO("%s: sendto(sk:%d, length:%d)", __func__, sk, p_pkt->length);
            int err = sendto(sk, p_pkt->data, p_pkt->length, 0, p_sa, sa_len);
            if (err < 0) {
                __WARN("%s: Failed to send packet to network (%d, %d)", __func__, err, errno);
            }
        }
    } else {
        __ERROR("%s: p_if is null", __func__);
    }

    return (p_pkt->length + sizeof(wfc_router_data_message_t));
}

/*
    Send control response to modem
*/
static void send_control_response(p_wfc_router_control_request_t p_req, unsigned short err) {
    wfc_router_control_response_t res;

    __DBG("%s: send %d", __func__, err);

    res.type    = ROUTER_MSG_CONTROL;
    res.res     = ROUTER_RES_CMD_MASK | p_req->cmd;
    res.seqnum  = p_req->seqnum;
    res.err     = err;

    write(__router.io_dev, &res, sizeof(wfc_router_control_response_t));
}

/*
    Process rules
*/
static int process_add_command(struct wfc_router_interface* p_if, p_wfc_router_control_request_t p_req) {
    int err = 0;
    int i;

    /* add rules */
    __DBG("%s: add %d rules", __func__, p_req->num_rules);

    for (i = 0; i < p_req->num_rules; i++) {
        struct wfc_router_protocol* p_proto = NULL;

        p_proto = find_protocol(p_if, convert_protocol_to_network(p_req->rules[i].protocol));
        if (p_proto == NULL) {
            err = add_protocol(p_if, &p_req->rules[i]);
        } else {
            err = add_rule(p_proto, &p_req->rules[i]);
        }
        if (err != 0)
            break;
    }
    return err;
}

static int process_delete_command(struct wfc_router_interface* p_if, p_wfc_router_control_request_t p_req) {
    int err = 0;

    /* delete rules */
    __DBG("%s: delete %d rules", __func__, p_req->num_rules);

    if (p_req->num_rules) {
        int i;
        for (i = 0; i < p_req->num_rules; i++) {
            struct wfc_router_protocol* p_proto = NULL;

            p_proto = find_protocol(p_if, convert_protocol_to_network(p_req->rules[i].protocol));
            if (p_proto != NULL) {
                err = delete_rule(p_proto, &p_req->rules[i]);
                if (err == 0) {
                    if (LIST_EMPTY(&p_proto->fwd)) {
                        __INFO("%s: no forward rules, delete protocol", __func__);
                        delete_protocol(p_proto);
                    }
                }
            } else {
                __ERROR("%s: protocol not found", __func__);
                err = ENOENT;
            }
            if (err != 0)
                return err;
        }
    } else {
        struct wfc_router_protocol* np = LIST_FIRST(&p_if->protocols);
        while (np != NULL) {
            struct wfc_router_protocol* next = LIST_NEXT(np, entries);
            delete_protocol(np);
            np = next;
        }
    }
    return err;
}

static void close_rx_socket(struct wfc_router_interface* p_if) {
    if (p_if->sk_rx > 0) {
        shutdown(p_if->sk_rx, SHUT_RD);
        close(p_if->sk_rx);
        p_if->sk_rx = -1;
    }
}

static void delete_interface(struct wfc_router_interface* p_if) {
    if (NULL != p_if) {
        close_rx_socket(p_if);
        LIST_REMOVE(p_if, entries);
        __INFO("%s: interface successfully deleted (%p)", __func__, p_if);
        free(p_if);
    }
}

static int start_rx(struct wfc_router_interface* p_if, int *p_err) {
    int err = -1;
    do {
        /* Create Socket for interface */
        p_if->sk_rx = open_raw_socket_rx(p_if, p_err);
        if (p_if->sk_rx < 0) {
            __ERROR("%s: Failed to open RX socket", __func__);
            break;
        }

        /* Create Thread to receive socket*/
        err = pthread_create(&p_if->sk_thread, NULL, socket_receiver, p_if);
        if (err < 0) {
            *p_err = errno;
            __ERROR("%s: Failed to start socket receiver thread (%d, %d)", __func__, err, errno);
            close_rx_socket(p_if);
        }
    } while (0);

    return err;
}

static struct wfc_router_interface* create_interface(p_wfc_router_control_request_t p_req,  int *p_err) {
    struct wfc_router_interface* p_if = NULL;
    int err = -1;

    do {
        p_if = (struct wfc_router_interface *)malloc(sizeof(struct wfc_router_interface));
        if (NULL == p_if) {
            __ERROR("%s: Memory allocation failed", __func__);
            *p_err = errno;
            return NULL;
        }

        /* fill up interface structure */
        LIST_INIT(&p_if->protocols);
        LIST_INIT(&p_if->iptables);
        LIST_INIT(&p_if->rttables);
        p_if->ifindex = p_req->index;
        p_if->ipv4 = (p_req->ip_type == ROUTER_IP_TYPE_IPv4);
        memcpy(p_if->ip, p_req->ip_addr, p_if->ipv4 ? ROUTER_IPV4_ADDR_SIZE : ROUTER_IPV6_ADDR_SIZE);
        if (p_if->ipv4)
            inet_ntop(AF_INET, (void*)p_if->ip, p_if->ip_str, INET6_ADDRSTRLEN);
        else
            inet_ntop(AF_INET6, (void*)p_if->ip, p_if->ip_str, INET6_ADDRSTRLEN);
        __INFO("%s: interface created for %s", __func__, p_if->ip_str);

        p_if->tx_bind_to_device = 0;

        /* Create RX process */
        err = start_rx(p_if, p_err);
        if (err < 0) {
            break;
        }

        /* add interface to router */
        LIST_INSERT_HEAD(&__router.interfaces, p_if, entries);
        __DBG("%s: interface successfully added (%p)", __func__, p_if);

        *p_err = err = 0;
    } while(0);

    if (err) {
        close_rx_socket(p_if);
        free(p_if);
        return NULL;
    }
    return p_if;
}

/*
    Process control request
*/
static int handle_control_request(unsigned char* buf, int len) {
    struct wfc_router_interface* p_if = NULL;
    p_wfc_router_control_request_t p_req = (p_wfc_router_control_request_t)buf;

    if ((unsigned long)len < sizeof(wfc_router_control_request_t)) {
        __ERROR("%s: Input buffer is too short (%d for min length %zu)", __func__, len, sizeof(wfc_router_control_request_t));
        return (-1);
    }
    if ((unsigned long)len < (sizeof(wfc_router_control_request_t) + (p_req->num_rules * sizeof(wfc_router_protocol_rule_descriptor_t)))) {
        __ERROR("%s: Input buffer is too short (%d for %zu)", __func__, len,
            sizeof(wfc_router_control_request_t) + (p_req->num_rules * sizeof(wfc_router_protocol_rule_descriptor_t)));
        return (-1);
    }
    /* find interface */
    p_if = find_interface(p_req->ip_addr);
    if (p_if == NULL) {
        /* new */
        __INFO("%s: new interface create request", __func__);

        if (is_valid_new_request(p_req)) {
            int err = 0;

            p_if = create_interface(p_req, &err);
            if (p_if != NULL) {
                /* create protocols or rules */
                err = process_add_command(p_if, p_req);
            }
            send_control_response(p_req, err);
        } else {
            send_control_response(p_req, EBADR);
        }
    } else {
        /* modify */
        __INFO("%s: modify request for %p", __func__, p_if);

        if (is_valid_modify_request(p_req)) {
            int err = 0;

            switch (p_req->cmd) {
                case ROUTER_CMD_ADD:
                {
                    err = process_add_command(p_if, p_req);
                    break;
                }
                case ROUTER_CMD_DELETE:
                {
                    err = process_delete_command(p_if, p_req);

                    if (LIST_EMPTY(&p_if->protocols)) {
                        delete_iptables_rules(p_if);
                        delete_rttables_rules(p_if);

                        __INFO("%s: no protocols, delete interface", __func__);
                        delete_interface(p_if);
                    }

                    break;
                }
                default:
                    /* should not be here */
                    __WARN("%s: invalid command %d", __func__, p_req->cmd);
            }
            send_control_response(p_req, err);
        } else {
            send_control_response(p_req, EBADR);
        }
    }

    return (sizeof(wfc_router_control_request_t) + (p_req->num_rules * sizeof(wfc_router_protocol_rule_descriptor_t)));
}

/*
    Process I/O read buffer
*/
static void process_read_buffer(unsigned char* buf, int len) {
    do {
        int err = 0;

        p_wfc_router_data_message_t p_msg = (p_wfc_router_data_message_t)buf;
        switch (p_msg->type) {
            case ROUTER_MSG_CONTROL:
                err = handle_control_request(buf, len);
                break;
            case ROUTER_MSG_DATA:
                err = forward_data_packet_to_network(buf, len);
                break;
            default:
                __ERROR("%s: Invalid message type %d", __func__, p_msg->type);
                return;
        }
        if (err < 0) {
            return;
        }
        buf += err;
        len -= err;
    } while (len > 0);
}

/*
    Socket receiver handler
*/
static void* socket_receiver(void* arg) {
    int dummy;
    int err = -1;
    struct wfc_router_interface* p_if = (struct wfc_router_interface*)arg;
    unsigned char* buf = (unsigned char *)malloc(ROUTER_NET_BUFFER_MAX_SIZE);
    if (buf == NULL) {
        __ERROR("%s: Failed to allocate memory for buffer", __func__);
        pthread_exit(&err);
    }

    __INFO("%s started for interface=%s, IP=%s", __func__, p_if->ifname, p_if->ip_str);
    /* run it for a while */
    dummy = p_if->ifindex;
    while (1) {
        err = recvfrom(p_if->sk_rx, buf + ROUTER_NET_BUFFER_OFFSET, ROUTER_NET_BUFFER_MAX_SIZE - ROUTER_NET_BUFFER_OFFSET, 0, NULL, 0);
        if (err > 0) {
            int matched = compare_destination_ip_with_interface(buf + ROUTER_NET_BUFFER_OFFSET, p_if);
            if (__IS_DBG) {
                char str_addr[INET6_ADDRSTRLEN];
                unsigned char* pkt_buf = buf + ROUTER_NET_BUFFER_OFFSET;
                int ipv4 = isIPv4_packet(*pkt_buf);
                if (ipv4) {
                    inet_ntop(AF_INET, get_destination_ip_from_packet(pkt_buf, ipv4), str_addr, INET6_ADDRSTRLEN);
                } else {
                    inet_ntop(AF_INET6, get_destination_ip_from_packet(pkt_buf, ipv4), str_addr, INET6_ADDRSTRLEN);
                }
                __DBG("%s: %d bytes received from %d for %s. Matched: %s", __func__, err, p_if->sk_rx, str_addr, matched ? "YES" : "NO");
            }
            /* check packets belogs to our interface only */
            if (matched) {
                log_packet_data("DATA_RECV", err, buf);
                check_network_packet_and_forward_to_modem(p_if, buf, err);
            }
        } else {
            __WARN("%s: Failed to read from socket %d", __func__, err);
            break;
        }
    }

    __INFO("%s stopped for ifindex=%d", __func__, dummy);
    free(buf);
    pthread_exit(&err);
}

/*
    I/O device reader handler
*/
static void* io_reader(void* __unused arg) {
    int err = -1;
    unsigned char* buf = (unsigned char *)malloc(ROUTER_IO_BUFFER_MAX_SIZE);
    if (buf == NULL) {
        __FATAL("%s: Failed to allocate memory for buffer", __func__);
        pthread_exit(&err);
    }
    __DBG("%s started", __func__);

    /* FOREVER */
    while (1) {
        struct pollfd pfd;

        pfd.fd = __router.io_dev;
        pfd.events = POLLIN;
        pfd.revents = 0;

#ifndef PKTRT_TEST
        err = poll(&pfd, 1, -1);
#else
        err = -1;
#endif
        if (err > 0) {
            if (pfd.revents & POLLIN) {
                err = read(__router.io_dev, buf, ROUTER_IO_BUFFER_MAX_SIZE);
                if (err > 0) {
                    process_read_buffer(buf, err);
                } else {
                    __FATAL("%s: Failed to read from I/O device (%d, %d)", __func__, err, errno);
                    break;
                }
            }
            if (pfd.revents & ~pfd.events) {
                __ERROR("%s: Poll returns %d", __func__, pfd.revents);
            }
        } else {
            __FATAL("%s: Failed to poll I/O device (%d, %d)", __func__, err, errno);
            break;
        }
    }

    __WARN("%s stopped", __func__);
    free(buf);
    pthread_exit(&err);
}

static int netlink_open_socket() {
    int err;
    struct sockaddr_nl sa = { .nl_family = AF_NETLINK, };

    __router.nt_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (__router.nt_sock < 0) {
        __FATAL("%s: Failed to open netlink socket", __func__);
        return __router.nt_sock;
    }
    err = bind(__router.nt_sock, (struct sockaddr*)&sa, sizeof(sa));
    if (err < 0) {
        __FATAL("%s: Failed to bind netlink socket", __func__);
        return err;
    }
    return 0;
}

static void netlink_close_socket() {
    close(__router.nt_sock);
}

static int netlink_open_event_socket() {
    int err;
    struct sockaddr_nl sa = {
        .nl_family = AF_NETLINK,
        .nl_groups = RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_LINK, };

    __router.nt_evt_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (__router.nt_evt_sock < 0) {
        __FATAL("%s: Failed to open netlink event socket", __func__);
        return __router.nt_evt_sock;
    }
    err = bind(__router.nt_evt_sock, (struct sockaddr*)&sa, sizeof(sa));
    if (err < 0) {
        __FATAL("%s: Failed to bind netlink event socket", __func__);
        return err;
    }
    return 0;
}

static void netlink_close_event_socket() {
    close(__router.nt_evt_sock);
}

static char* get_link_name(struct rtattr* attr, size_t len) {
    char* name = NULL;
	while (RTA_OK(attr, len)) {
		switch (attr->rta_type)
		{
			case IFLA_IFNAME:
				name = (char *)RTA_DATA(attr);
				break;
		}
		attr = RTA_NEXT(attr, len);
	}
    return name;
}

static void restart_device_sockets(struct wfc_router_interface* p_if) {
    int err;
    int rebind = p_if->tx_bind_to_device;
    struct wfc_router_protocol* np = NULL;

    p_if->tx_bind_to_device = 0;
    if (start_rx(p_if, &err) < 0) {
        __ERROR("%s: failed to restart RX for %s", __func__, p_if->ip_str);
        return;
    }
    if (rebind) {
        LIST_FOREACH(np, &p_if->protocols, entries) {
            if (np->sk_tx > 0) {
                close(np->sk_tx);
                np->sk_tx = -1;
            }
            np->sk_tx = open_raw_socket_tx(p_if, np->protocol, &err);
            if (np->sk_tx < 0) {
                __ERROR("%s: failed to reopen TX for %s, device %s", __func__, convert_protocol_to_string(np->protocol), p_if->ifname);
            }
        }
    } else {
        delete_rttables_rules(p_if);
    }
}

static __inline__ int str_equals(const char *first, const char *second) {
    return (strlen(first) == strlen(second)) && (strncmp(first, second, strlen(second)) == 0);
}

static void process_link_up(struct nlmsghdr* header) {
    char ifname[IF_NAMESIZE];
    struct wfc_router_interface* np = NULL;
    struct ifinfomsg* msg = (struct ifinfomsg *)NLMSG_DATA(header);
    char* name = get_link_name(IFLA_RTA(msg), IFLA_PAYLOAD(header));
    if (name == NULL) {
        if_indextoname(msg->ifi_index, ifname);
        name = ifname;
    }
    __INFO("%s: %s is activated", __func__, name);

    LIST_FOREACH(np, &__router.interfaces, entries) {
        if (str_equals(name, np->ifname) && (np->sk_rx < 0)) {
            __INFO("%s: re-start RX/TX for %s", __func__, np->ip_str);
            restart_device_sockets(np);
        }
    }
}

static void process_link_down_or_deleted(struct nlmsghdr* header) {
    char ifname[IF_NAMESIZE];
    struct wfc_router_interface* np = NULL;
    struct ifinfomsg* msg = (struct ifinfomsg *)NLMSG_DATA(header);
    char* name = get_link_name(IFLA_RTA(msg), IFLA_PAYLOAD(header));
    if (name == NULL) {
        if_indextoname(msg->ifi_index, ifname);
        name = ifname;
    }
    __INFO("%s: %s is deactivated", __func__, name);

    LIST_FOREACH(np, &__router.interfaces, entries) {
        if (str_equals(name, np->ifname) && (np->sk_rx > 0)) {
            __INFO("%s: stop RX for %s", __func__, np->ip_str);
            close_rx_socket(np);
        }
    }
}

static void process_address_event(struct nlmsghdr* header, const char* action) {
    char ifname[IF_NAMESIZE];
    char str_addr[INET6_ADDRSTRLEN];

    struct ifaddrmsg* msg = (struct ifaddrmsg *)NLMSG_DATA(header);
    struct rtattr *attr = IFA_RTA(msg);
    size_t len = IFA_PAYLOAD(header);
    unsigned char* addr = NULL;
    int addr_len = 0;

	while (RTA_OK(attr, len)) {
		switch (attr->rta_type)
		{
			case IFA_ADDRESS:
				addr = (unsigned char *)RTA_DATA(attr);
                addr_len = RTA_PAYLOAD(attr);
				break;
		}
		attr = RTA_NEXT(attr, len);
	}
    if (addr == NULL) {
        return;
    }
    if_indextoname(msg->ifa_index, ifname);
    if (addr_len > ROUTER_IPV4_ADDR_SIZE) {
        inet_ntop(AF_INET6, addr, str_addr, INET6_ADDRSTRLEN);
    } else {
        inet_ntop(AF_INET, addr, str_addr, INET6_ADDRSTRLEN);
    }
    __INFO("%s: %s %s %s", __func__, str_addr, action, ifname);
}

/*
    Netlink event handler
*/
static void* nt_event_receiver(void* __unused arg) {
    int err = -1;
    unsigned char* buf = (unsigned char *)malloc(ROUTER_IO_BUFFER_MAX_SIZE);
    if (buf == NULL) {
        __FATAL("%s: Failed to allocate memory for buffer", __func__);
        pthread_exit(&err);
    }
    __DBG("%s started", __func__);

    /* FOREVER */
    while (1) {
        struct sockaddr_nl addr;
        socklen_t addr_len = sizeof(addr);
        struct nlmsghdr *hdr = (struct nlmsghdr*)buf;
#ifndef PKTRT_TEST
        err = recvfrom(__router.nt_evt_sock, buf, ROUTER_IO_BUFFER_MAX_SIZE, 0, (struct sockaddr*)&addr, &addr_len);
#else
        err = -1;
#endif
        if (err > 0) {
            __DBG("%s: %d bytes received from netlink", __func__, err);

            if (addr.nl_pid != 0) {
                /* not from kernel, ignore */
                continue;
            }
            while (NLMSG_OK(hdr, err)) {
                switch (hdr->nlmsg_type) {
                    case RTM_NEWADDR:
                        process_address_event(hdr, "added to");
                        break;
                    case RTM_DELADDR:
                        process_address_event(hdr, "removed from");
                        break;
                    case RTM_NEWLINK:
                    {
                        struct ifinfomsg* msg = (struct ifinfomsg *)NLMSG_DATA(hdr);
                        if (msg->ifi_flags & IFF_UP) {
                            process_link_up(hdr);
                            break;
                        }
                    }
                    case RTM_DELLINK:
                        process_link_down_or_deleted(hdr);
                        break;
                    default:;
                }
                hdr = NLMSG_NEXT(hdr, err);
            }
        } else {
            __FATAL("%s: Failed to read netlink events (%d, %d)", __func__, err, errno);
            break;
        }
    }

    __DBG("%s stopped", __func__);
    free(buf);
    pthread_exit(&err);
}

#ifndef PKTRT_TEST
int main() {
    int err = 0;

    __DBG("%s starting", WFC_ROUTER_LOG_TAG);

    /* init local structure */
    LIST_INIT(&__router.interfaces);

    err = pthread_mutex_init(&__router.io_lock, NULL);
    if (err < 0) {
        __FATAL("%s: Failed to init mutex (%d, %d)", __func__, err, errno);
        return err;
    }
    __router.io_dev = open(ROUTER_IO_DEVICE, O_RDWR | O_SYNC);
    __INFO("%s is opened", ROUTER_IO_DEVICE);
    if (__router.io_dev < 0) {
        __FATAL("%s: Failed to open device (%d, %d)", __func__, __router.io_dev, errno);
        pthread_mutex_destroy(&__router.io_lock);
        return __router.io_dev;
    }
    err = netlink_open_socket();
    if (err < 0) {
        __FATAL("%s: Failed to open netlink socket (%d, %d)", __func__, err, errno);
        pthread_mutex_destroy(&__router.io_lock);
        close(__router.io_dev);
        return err;
    }
    err = netlink_open_event_socket();
    if (err < 0) {
        __FATAL("%s: Failed to open netlink event socket (%d, %d)", __func__, err, errno);
        pthread_mutex_destroy(&__router.io_lock);
        close(__router.io_dev);
        netlink_close_socket();
        return err;
    }
    err = pthread_create(&__router.nt_thread, NULL, nt_event_receiver, NULL);
    if (err < 0) {
        __FATAL("%s: Failed to start netlink event thread (%d, %d)", __func__, err, errno);
        pthread_mutex_destroy(&__router.io_lock);
        close(__router.io_dev);
        netlink_close_socket();
        netlink_close_event_socket();
        return err;
    }
    err = pthread_create(&__router.io_thread, NULL, io_reader, NULL);
    if (err < 0) {
        __FATAL("%s: Failed to start I/O reader thread (%d, %d)", __func__, err, errno);
        pthread_mutex_destroy(&__router.io_lock);
        close(__router.io_dev);
        netlink_close_socket();
        netlink_close_event_socket();
        return err;
    }

    /* wait here */
    pthread_join(__router.io_thread, NULL);

    /* exit */
    pthread_mutex_destroy(&__router.io_lock);
    close(__router.io_dev);
    netlink_close_socket();
    netlink_close_event_socket();
    return 0;
}
#endif
