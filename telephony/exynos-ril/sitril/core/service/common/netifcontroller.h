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
 * netifcontroller.h
 *
 *  Created on: 2014. 10. 23.
 *      Author: sungwoo48.choi
 *
 *  reference code : kernel/exynos/Documentation/networking/ifenslave.c
 */

#ifndef    __NETIF_CONTROLLER_H__
#define    __NETIF_CONTROLLER_H__

#include "rildef.h"
#include <netinet/in.h>
#include <linux/netlink.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
const sockaddr_nl KERNEL_NLADDR = {AF_NETLINK, 0, 0, 0};

const uint16_t NETLINK_REQUEST_FLAGS = NLM_F_REQUEST | NLM_F_ACK;
const uint16_t NETLINK_ROUTE_CREATE_FLAGS = NETLINK_REQUEST_FLAGS | NLM_F_CREATE | NLM_F_EXCL;
const uint16_t NETLINK_ROUTE_REPLACE_FLAGS = NETLINK_REQUEST_FLAGS | NLM_F_REPLACE;

class PdpContext;

class NetIfController {
    DECLARE_MODULE_TAG()
private:
    PdpContext *m_pPdpContext;
    char    m_ifname[256];
    int     m_skfd;
    int     m_skfdv6;

public:
    NetIfController(PdpContext *pPdpContext);
    NetIfController(const NetIfController&) = delete;
    NetIfController& operator=(NetIfController const&) = delete;
    ~NetIfController();

public:
    bool ChangeAddress();
    bool BringUp();
    bool TearDown();
    void Reset();
    void SetPdpContext(PdpContext *pPdpContext);
    bool UpdateIfAddr(int action, uint8_t family, const char *addr, unsigned int ifa_valid, unsigned int ifa_prefered);

private:
    bool needToIpv6Rs(DataCall *dc);
    bool configureIpv4Addr(DataCall *dc);
    bool configureIpv6Addr(DataCall *dc);
    void doEmulateIpv4v6ForTest(DataCall *dc);
    void processIpv6LinkLocal(DataCall *dc);
    static const bool skip_set_ipv6_linklocaladdr = true;
    static const bool emulate_ipv4v6_for_test = false; // For on SKT-live test(IPV6 only network), Only enable when dev testing
    bool checkSockFd();
    //Netlink support part
    int openNetlinkSocket(int protocol);
    int sendNetlinkRequest(uint16_t action, uint16_t flags, iovec* iov, int iovlen);
    int recvNetlinkAck(int sock);

    // static
public:
    static int GetIfSock();
    static int GetIfSockV6();
    static short GetIfFlags(int skfd, char *ifname);
    static bool SetIfFlags(int skfd, char *ifname, short flags);
    static bool SetIfUp(int skfd, char *ifname, short flags);
    static bool SetIfDown(int skfd, char *ifname, short flags);
    static bool SetIfDormant(int skfd, char *ifname, short flags, bool enable);
    static bool GetLastError();
    static int GetIfIndex(int skfd, const char *ifname);
    static bool SetIfAddr(int skfd, char *ifname, unsigned char *addr);
    static bool ClearIfAddr(int skfd, char *ifname);
    static bool SetIfAddrIpv6(int skfd, char *ifname, int index, unsigned char *addr);
    static bool SetIfAddrIpv6(int skfd, char *ifname, unsigned char *addr);
    bool ClearIfIpv6Address(int skfd, char *ifname, bool isLinkLocal);
    bool ClearIfGlobalAddress(int skfd, char *ifname);
    bool ClearIfLinkLocalAddress(int skfd, char *ifname);
    vector<in6_addr> get_if_addrn_v6(const char *ifname);
    static int SetIfMaxRsCount(const char *ifname, int count);
    static int GetIfMaxRsCount(const char *ifname);
    static int SetIfRsDelay(const char *ifname, int delay);
    static int GetIfRsDelay(const char *ifname);
    static int SetIfRsInterval(const char *ifname, int interval);
    static int GetIfRsInterval(const char *ifname);
};

#endif // __NET_CONTROLLER_H__
