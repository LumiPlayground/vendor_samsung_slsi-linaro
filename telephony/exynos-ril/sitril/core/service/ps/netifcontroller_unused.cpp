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
 * netifcontroller_unused.cpp
 *
 *  Created on: 2020. 07.093.
 *      Author: jiwon.ss.kim
 *
 */

// These codes are preserved to be used future
// Not involved in build

// #define __INTENTIONAL_PRESERVED_EVEN_UNUSED__
// No more use procfs access
#define PROC_NET_IF_INET6       "/proc/net/if_inet6"
#define PROC_NET_IF_INET6_ROUTE "/proc/net/ipv6_route"
#define IN6_FMT "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
#define IN6_ADDR_EXP(__a, __op) \
             __op(__a[0]), __op(__a[1]), \
             __op(__a[2]), __op(__a[3]), \
             __op(__a[4]), __op(__a[5]), \
             __op(__a[6]), __op(__a[7]), \
             __op(__a[8]), __op(__a[9]), \
             __op(__a[10]), __op(__a[11]), \
             __op(__a[12]), __op(__a[13]), \
             __op(__a[14]), __op(__a[15])

static int clear_if_global_addr_v6(int skfd, char *ifname, int index)
{
    RilLogI("%s %s skfd:%d, ifname:%s, index:%d", TAG, __FUNCTION__, skfd, (ifname) ? ifname : "", index);
    //struct in6_ifreq ifr;
    //int res = 0;

    FILE *fp;
    char tmp_ifname[IF_NAMESIZE];
    unsigned int tmp_index, unused2, tmp_scope, unused3;
    unsigned int tmp_addr[16];
    unsigned char addr[16];

    if ((fp = fopen(PROC_NET_IF_INET6, "r")) == NULL) {
        RilLogE("%s(): Couldn't open proc file %s", __FUNCTION__, PROC_NET_IF_INET6);
        return -1;
    }

    while (fscanf(fp, IN6_FMT" %02x %02x %02x %02x %15s\n",
                  IN6_ADDR_EXP(tmp_addr, &), &tmp_index, &unused2,
                  &tmp_scope, &unused3, tmp_ifname) != EOF) {
        if (strcmp(ifname, tmp_ifname) == 0 && tmp_scope == IPV6_ADDR_LINKLOCAL) {
            for (int i = 0; i < 16; i++) {
                addr[i] = (__u8)tmp_addr[i];
            }

            RilLogV("%s(): remove ipv6 addr with index(%d)", __FUNCTION__, tmp_index);
            clear_if_addr_v6(skfd, ifname, index, addr);
        }
    }
    fclose(fp);
    fp = NULL;

    return 0;
}

static int clear_if_linklocal_addr_v6(int skfd, char *ifname, int index)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    //struct in6_ifreq ifr;
    //int res = 0;

    FILE *fp;
    char tmp_ifname[IF_NAMESIZE];
    unsigned int tmp_index, unused2, tmp_scope, unused3;
    unsigned int tmp_addr[16];
    unsigned char addr[16];

    if ((fp = fopen(PROC_NET_IF_INET6, "r")) == NULL) {
        RilLogE("%s(): Couldn't open proc file %s", __FUNCTION__, PROC_NET_IF_INET6);
        return -1;
    }

    while (fscanf(fp, IN6_FMT" %02x %02x %02x %02x %15s\n",
                  IN6_ADDR_EXP(tmp_addr, &), &tmp_index, &unused2,
                  &tmp_scope, &unused3, tmp_ifname) != EOF) {
        if (strcmp(ifname, tmp_ifname) == 0 && tmp_scope == IPV6_ADDR_GLOBAL) {
            for (int i = 0; i < 16; i++) {
                addr[i] = (__u8)tmp_addr[i];
            }

            RilLogV("%s(): remove ipv6 addr with index(%d)", __FUNCTION__, tmp_index);
            clear_if_addr_v6(skfd, ifname, index, addr);
        }
    }
    fclose(fp);
    fp = NULL;

    return 0;
}

static int clear_if_routes_v6(int skfd, char *ifname, int index, unsigned int fl, unsigned int pref)
{
    char tmp_ifname[64];
    unsigned int prefix, unused1, unused2, unused3, unused4, flags;
    unsigned int dst[16];
    unsigned int temp[16];
    unsigned int gw[16];
    struct in6_rtmsg rt;
    FILE *fp;
    int ifindex;
    char dst_str[64], gw_str[64];
    int i, result;

    if ((fp = fopen("/proc/net/ipv6_route", "r")) == NULL) {
        RilLogE("Failed to open route file.");
        return -1;
    }

    for (;;) {
        int nread =
                fscanf(fp,
                        IN6_FMT" %02x "IN6_FMT" %02x " IN6_FMT" \
                %08x %08x %08x %08x %15s\n",
                        IN6_ADDR_EXP(dst, &), &prefix, IN6_ADDR_EXP(temp, &),
                        &unused1, IN6_ADDR_EXP(gw, &), &unused2, &unused3,
                        &unused4, &flags, tmp_ifname);

        if (nread != (16 + 1 + 16 + 1 + 16 + 5)) {
            break;
        }

        if ((flags & (RTF_UP | fl)) != (RTF_UP | fl)
                || strcmp(tmp_ifname, ifname) != 0 || prefix != pref) {
            continue;
        }

        memset(&rt, 0, sizeof(rt));
        rt.rtmsg_dst_len = prefix;
        rt.rtmsg_ifindex = ifindex;
        rt.rtmsg_flags = flags;

        for (i = 0; i < 16; i++) {
            rt.rtmsg_gateway.s6_addr[i] = (__u8) gw[i];
            rt.rtmsg_dst.s6_addr[i] = (__u8) dst[i];
        }

        inet_ntop(AF_INET6, rt.rtmsg_gateway.s6_addr, gw_str, sizeof(gw_str));
        RilLogV("%s(): remove gw %s", __FUNCTION__, gw_str);

        if ((result = ioctl(skfd, SIOCDELRT, &rt)) < 0) {
            RilLogE("Failed to remove ipv6 route for %s to gw %s dst %s: %s",
                    tmp_ifname, gw_str,
                    inet_ntop(AF_INET6, rt.rtmsg_dst.s6_addr, dst_str, sizeof(dst_str)),
                    strerror(errno));
        }
    }

    fclose(fp);
    return 0;
}

/*
 * addr_gen_mode - INTEGER
 *     Defines how link-local and autoconf addresses are generated.
 *
 *     0: generate address based on EUI64 (default)
 *     1: do no generate a link-local address, use EUI64 for addresses generated
 *        from autoconf
 *     2: generate stable privacy addresses, using the secret from
 *        stable_secret (RFC7217)
 *     3: generate stable privacy addresses, using a random secret if unset
 */
static int set_if_addr_gen_mode(char *ifname, int mode)
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    if (mode < 0 || mode > 3)
    {
        RilLogE("Failed to set set_if_addr_gen_mode(%s, %d), Unsupported mode", ifname, mode);
        return -1;
    }
    set_if_ipv6conf_sysproc(ifname, "addr_gen_mode", mode);

    return 0;
}

/*
 * accept_ra - INTEGER
 *   Accept Router Advertisements; autoconfigure using them.
 *
 *   It also determines whether or not to transmit Router
 *   Solicitations. If and only if the functional setting is to
 *   accept Router Advertisements, Router Solicitations will be
 *   transmitted.
 *
 *   Possible values are:
 *       0 Do not accept Router Advertisements.
 *       1 Accept Router Advertisements if forwarding is disabled.
 *       2 Overrule forwarding behaviour. Accept Router Advertisements
 *         even if forwarding is enabled.
 *
 *   Functional default: enabled if local forwarding is disabled.
 *                       disabled if local forwarding is enabled.
 */
static int set_if_accept_ra(char *ifname, int mode)
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    if (mode < 0 || mode > 2)
    {
        RilLogE("Failed to set set_if_accept_ra(%s, %d), Unsupported mode", ifname, mode);
        return -1;
    }
    set_if_ipv6conf_sysproc(ifname, "accept_ra", mode);

    return 0;
}
