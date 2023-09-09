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
 * netifcontroller.cpp
 *
 *  Created on: 2014. 10. 23.
 *      Author: sungwoo48.choi
 *
 *  reference code : kernel/exynos/Documentation/networking/ifenslave.c
 */

#include "netifcontroller.h"
#include <cerrno>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <ifaddrs.h>

#include "pdpcontext.h"
#include "rillog.h"
#include <unistd.h>
#include <fcntl.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_NETIF, CRilLog::E_RIL_INFO_LOG, format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_NETIF, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_NETIF, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_NETIF, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

#define MAX_FILE_LEN 64
#ifndef IPV6_ADDR_LINKLOCAL
#define IPV6_ADDR_LINKLOCAL     0x0020U
#endif

#ifndef IPV6_ADDR_GLOBAL
#define IPV6_ADDR_GLOBAL        0x0000U
#endif

static unsigned char nullIpv4[MAX_IPV4_ADDR_LEN] = { 0, };
static unsigned char nullIpv6[MAX_IPV6_ADDR_LEN] = { 0, };

static bool debug = true;
IMPLEMENT_MODULE_TAG(NetIfController, NetIfController)
static const char *TAG = "[NetIfController]";

static int last_errno = 0;
static int get_if_sock();
static short get_if_flags(int skfd, char *ifname);
static int set_if_flags(int skfd, char *ifname, short flags);
static int set_if_up(int skfd, char *ifname, short flags);
static int set_if_down(int skfd, char *ifname, short flags);
static int set_if_addr(int skfd, char *ifname, unsigned char *addr);
static int clear_if_addr(int skfd, char *ifname);

static int get_if_sock()
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    int skfd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (skfd < 0) {
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
    }

    return skfd;
}

static short get_if_flags(int skfd, char *ifname)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    struct ifreq ifr = {};
    int res = 0;

    SECURELIB::strncpy(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, SECURELIB::strlen(ifname));
    res = ioctl(skfd, SIOCGIFFLAGS, &ifr);
    if (res < 0) {
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
        return 0;
    }
    RilLogI("%s %s returns %x", TAG, __FUNCTION__, ifr.ifr_flags);

    return ifr.ifr_flags;
}

static int set_if_flags(int skfd, char *ifname, short flags)
{
    RilLogI("%s %s %x", TAG, __FUNCTION__, flags);
    struct ifreq ifr = {};
    int res = 0;

    ifr.ifr_flags = flags;
    SECURELIB::strncpy(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, SECURELIB::strlen(ifname));

    res = ioctl(skfd, SIOCSIFFLAGS, &ifr);
    if (res < 0) {
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
    }

    return res;
}

static int set_if_up(int skfd, char *ifname, short flags)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    return set_if_flags(skfd, ifname, flags | IFF_UP);
}

static int set_if_down(int skfd, char *ifname, short flags)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    return set_if_flags(skfd, ifname, flags & ~IFF_UP);
}

static int set_if_dormant(int skfd, char *ifname, short flags)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    return set_if_flags(skfd, ifname, flags | IFF_DORMANT);
}

static int clear_if_dormant(int skfd, char *ifname, short flags)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    return set_if_flags(skfd, ifname, flags & ~IFF_DORMANT);
}

static int set_if_addr(int skfd, char *ifname, unsigned char *addr)
{
    RilLogI("%s %s %s %d", TAG, __FUNCTION__, (ifname) ? ifname : "", skfd );
    struct ifreq ifr = {};
    int res = 0;
    SECURELIB::strncpy(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, SECURELIB::strlen(ifname));
    ifr.ifr_addr.sa_family = AF_INET;
    memset(ifr.ifr_addr.sa_data, 0, sizeof(ifr.ifr_addr.sa_data));
    ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = *((in_addr_t *)addr);

    res = ioctl(skfd, SIOCSIFADDR, &ifr);
    if (res < 0) {
        if (errno == EEXIST) {
            // ignore
            return 0;
        }
        last_errno = errno;
        RilLogE("%s(SIOCSIFADDR) %s error %d", TAG, __FUNCTION__, last_errno);
        if(last_errno == 13)
        {
            RilLogE("%s(SIOCSIFADDR) %s uid %d, tid %d, pid %d, gid %d, egid%d", TAG, __FUNCTION__, getuid(), gettid(), getpid(), getgid(), getegid());
        }
    }
    else {
        // This will trigger Kernel main routing insertion,
        // but useless from AOSP N version, each interface's routing table will hold own default
        /*
        unsigned int mask = 24;
        mask = htonl(~(0) << (32 - mask));
        ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = *((in_addr_t *)&mask);
        if (ioctl(skfd, SIOCSIFNETMASK, &ifr) < 0) {
            if (errno == EEXIST) {
                // ignore
            }
            last_errno = errno;
            RilLogE("%s(SIOCSIFNETMASK) %s error %d", TAG, __FUNCTION__, last_errno);
        }
        */
    }

    return res;
}

static int clear_if_addr(int skfd, char *ifname)
{
    RilLogI("%s %s %s %d", TAG, __FUNCTION__, (ifname) ? ifname : "", skfd);

    return set_if_addr(skfd, ifname, nullIpv4);
}

static int set_if_ipv6conf_sysproc(const char *ifname, const char *prop_name, int value)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    char filepath[MAX_FILE_LEN];

    snprintf(filepath, MAX_FILE_LEN, "/proc/sys/net/ipv6/conf/%s/%s", ifname, prop_name);

    int fd = open(filepath, O_WRONLY | O_CLOEXEC);
    if (fd == -1) {
        RilLog("%s(%s,%s,%d) Failed to open %s due to %s",
                __func__, ifname, prop_name, value, filepath, strerror(errno));
        return -1;
    }

    char c = value + '0';
    int n = write(fd, &c, sizeof(c));
    close(fd);

    if (n < 1) {
        RilLog("Failed to write for %s(%s,%s,%d)", __func__, ifname, prop_name, value);
        return -1;
    }
    RilLogV("%s for %s is set to %d.", prop_name, ifname, value);
    return 0;
}


static int set_if_enabled_IPv6(char *ifname, bool enabled)
{
    RilLogI("%s %s %s %d", TAG, __FUNCTION__, (ifname) ? ifname : "", enabled);

    // this prop need inverted value
    set_if_ipv6conf_sysproc(ifname, "disable_ipv6", (enabled) ? 0 : 1);

    return 0;
}

static int set_if_enabled_auto_conf_v6(char *ifname, bool enabled)
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    set_if_ipv6conf_sysproc(ifname, "autoconf", (enabled) ? 1 : 0);

    return 0;
}

static int set_if_enabled_dad_v6(char *ifname, bool enabled)
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    set_if_ipv6conf_sysproc(ifname, "accept_dad", (enabled) ? 1 : 0);

    return 0;
}

static int set_if_max_rs_count_v6(const char *ifname, int count)
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    set_if_ipv6conf_sysproc(ifname, "router_solicitations", count);

    return 0;
}

static int set_if_max_dad_count_v6(const char *ifname, int count)
{
    RilLogI("%s %s", TAG, __FUNCTION__);

    set_if_ipv6conf_sysproc(ifname, "dad_transmits", count);

    return 0;
}

const char* rs_propname[3] = { "router_solicitations",
                               "router_solicitation_delay",
                               "router_solicitation_interval" };

typedef enum { RS_MAXCOUNT=0, RS_DELAY, RS_INTERVAL } _rs_propname;

static int get_if_ipv6conf_sysproc(const char *ifname, const char *prop_name)
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    char filepath[MAX_FILE_LEN];

    snprintf(filepath, MAX_FILE_LEN, "/proc/sys/net/ipv6/conf/%s/%s", ifname, prop_name);

    int fd = open(filepath, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        RilLog("%s(%s,%s) Failed to open %s due to %s",
                __func__, ifname, prop_name, filepath, strerror(errno));
        return -1;
    }

    char c[20] = {0};  // 64bit integer max is about 9*10^18
    int n = read(fd, &c, sizeof(c) - 1);
    close(fd);
    c[19] = '\0';

    if (n < 0) {
        RilLog("Failed to %s(%s)", __func__, ifname);
        return -1;
    }
    n = stoi(c);
    RilLogV("%s for %s is %d.", prop_name, ifname, n);
    return n;
}

static int get_if_index(int skfd, const char *ifname)
{
    RilLogI("%s %s skfd:%d, ifname:%s", TAG, __FUNCTION__, skfd, (ifname) ? ifname : "");
    struct ifreq ifr = {};
    SECURELIB::strncpy(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, SECURELIB::strlen(ifname));
    if (ioctl(skfd, SIOCGIFINDEX, &ifr) < 0) {
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
        return -1;
    }
    RilLogV("%s %s returns index:%d", TAG, __FUNCTION__, ifr.ifr_ifindex);
    return ifr.ifr_ifindex;
}


static int get_if_sock_v6()
{
    RilLogI("%s %s", TAG, __FUNCTION__);
    int skfd = socket(AF_INET6, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (skfd < 0) {
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
    } else {

    }
    RilLogV("%s %s success return skfd:%d", TAG, __FUNCTION__, skfd);
    return skfd;
}

enum ADDR_OPER { SET_ADDR = 0, CLEAR_ADDR = 1 };

static int modify_if_addr_v6(int skfd, char *ifname, int index, unsigned char *addr, ADDR_OPER oper) {
    RilLogI("%s %s skfd:%d, ifname:%s, index:%d oper:%d", TAG, __FUNCTION__, skfd, (ifname) ? ifname : "", index, oper);
    struct in6_ifreq ifr = { { {{0}} }, 0, 0};

    memcpy((void*)(ifr.ifr6_addr.s6_addr), (void *)addr, 16);
    ifr.ifr6_ifindex = index;
    ifr.ifr6_prefixlen = 64;
    return ioctl(skfd, (oper == SET_ADDR) ? SIOCSIFADDR : SIOCDIFADDR, &ifr);
}

static int set_if_addr_v6(int skfd, char *ifname, int index, unsigned char *addr)
{
    int res = modify_if_addr_v6(skfd, ifname, index, addr, SET_ADDR);
    if (res < 0) {
        if (errno == EEXIST) {
            RilLogW("%s %s wanring %d address is already exist", TAG, __FUNCTION__, last_errno);
            // ignore
            return 0;
        }
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
    }

    return res;
}

static int clear_if_addr_v6(int skfd, char *ifname, int index, unsigned char *addr)
{
    int res = modify_if_addr_v6(skfd, ifname, index, addr, CLEAR_ADDR);
    if (res < 0) {
        last_errno = errno;
        RilLogE("%s %s error %d", TAG, __FUNCTION__, last_errno);
    }

    return res;
}

// Avoids "non-constant-expression cannot be narrowed from type 'unsigned int' to 'unsigned short'"
// warnings when using RTA_LENGTH(x) inside static initializers (even when x is already uint16_t).
static constexpr uint16_t U16_RTA_LENGTH(uint16_t x) {
    return RTA_LENGTH(x);
}


/****************************************
 * Wrapper functions
 ****************************************/
int NetIfController::GetIfSock()
{
    return get_if_sock();
}

int NetIfController::GetIfSockV6()
{
    return get_if_sock_v6();
}

short NetIfController::GetIfFlags(int skfd, char *ifname)
{
    return get_if_flags(skfd, ifname);
}

bool NetIfController::SetIfFlags(int skfd, char *ifname, short flags)
{
    if (set_if_flags(skfd, ifname, flags) < 0)
        return false;
    return true;
}

bool NetIfController::SetIfUp(int skfd, char *ifname, short flags)
{
    if (set_if_up(skfd, ifname, flags) < 0)
        return false;
    return true;
}

bool NetIfController::SetIfDown(int skfd, char *ifname, short flags)
{
    if (set_if_down(skfd, ifname, flags) < 0)
        return false;
    return true;
}

bool NetIfController::SetIfDormant(int skfd, char *ifname, short flags, bool enable)
{
    if (enable) {
        if (set_if_dormant(skfd, ifname, flags) < 0) return false;
    } else {
        if (clear_if_dormant(skfd, ifname, flags) < 0) return false;
    }
    return true;
}

bool NetIfController::SetIfAddr(int skfd, char *ifname, unsigned char *addr)
{
    if (set_if_addr(skfd, ifname, addr) < 0)
        return false;
    return true;
}

bool NetIfController::ClearIfAddr(int skfd, char *ifname)
{
    if (clear_if_addr(skfd, ifname) < 0)
        return false;
    return true;
}

bool NetIfController::GetLastError()
{
    return last_errno;
}

int NetIfController::GetIfIndex(int skfd, const char *ifname)
{
    return get_if_index(skfd, ifname);
}

bool NetIfController::SetIfAddrIpv6(int skfd, char *ifname, int index, unsigned char *addr)
{
    if (set_if_addr_v6(skfd, ifname, index, addr) < 0)
        return false;
    return true;
}

bool NetIfController::SetIfAddrIpv6(int skfd, char *ifname, unsigned char *addr)
{
    int index = get_if_index(skfd, ifname);
    if (index < 0) {
        return false;
    }

    if (set_if_addr_v6(skfd, ifname, index, addr) < 0)
        return false;
    return true;
}

vector<in6_addr> NetIfController::get_if_addrn_v6(const char *ifname)
{
    RilLogI("[%s] %s ifname:%s", TAG, __FUNCTION__, (ifname) ? ifname : "");

    vector<in6_addr> addresses;

    struct ifaddrs *ifa=NULL, *ifEntry=NULL;
    int res = 0;

    res = getifaddrs(&ifa);
    if(res==0){
        for(ifEntry = ifa; ifEntry != NULL; ifEntry = ifEntry->ifa_next) {
            if(ifEntry->ifa_addr == NULL)
                continue;
            if(strcmp(ifEntry->ifa_name, ifname))
                continue;

            if(ifEntry->ifa_addr->sa_family == AF_INET6) {
                struct sockaddr_in6 *in =  (struct sockaddr_in6 *)ifEntry->ifa_addr;
                addresses.push_back(in->sin6_addr);
            }
        }
    }
    freeifaddrs(ifa);

    return addresses;
}

bool NetIfController::ClearIfIpv6Address(int skfd, char *ifname, bool isLinkLocal)
{
    int index = GetIfIndex(skfd, ifname);
    if (index >= 0) {
        RilLogV("ifname=%s", ifname);
        vector<in6_addr> alladdr = get_if_addrn_v6(ifname);

        for(in6_addr item : alladdr) {
            if(!isLinkLocal ^ (item.s6_addr[0] == 0xfe && item.s6_addr[1] == 0x80)) {
                continue;
            }
            RilLogV("%s(): remove ipv6 addr with index(%d)", __FUNCTION__, index);
            clear_if_addr_v6(skfd, ifname, index, item.s6_addr);
        }

        return true;
    }
    return false;
}

bool NetIfController::ClearIfGlobalAddress(int skfd, char *ifname)
{
    return ClearIfIpv6Address(skfd, ifname, false);
}

bool NetIfController::ClearIfLinkLocalAddress(int skfd, char *ifname)
{
    return ClearIfIpv6Address(skfd, ifname, true);
}

int NetIfController::SetIfMaxRsCount(const char *ifname, int count)
{
    return set_if_max_rs_count_v6(ifname, count);
}

int NetIfController::GetIfMaxRsCount(const char *ifname)
{
    return get_if_ipv6conf_sysproc(ifname, rs_propname[RS_MAXCOUNT]);
}

int NetIfController::SetIfRsDelay(const char * /* ifname */, int /* delay */)
{
    // TBD
    return 0;
}

int NetIfController::GetIfRsDelay(const char *ifname)
{
    return get_if_ipv6conf_sysproc(ifname, rs_propname[RS_DELAY]);
}

int NetIfController::SetIfRsInterval(const char * /* ifname */, int /* interval */)
{
    // TBD
    return 0;
}

int NetIfController::GetIfRsInterval(const char *ifname)
{
    return get_if_ipv6conf_sysproc(ifname, rs_propname[RS_INTERVAL]);
}

/****************************************
 * NetIfController
 ****************************************/
NetIfController::NetIfController(PdpContext *pPdpContext) : m_skfd(-1), m_skfdv6(-1)
{
    SetPdpContext(pPdpContext);
}

NetIfController::~NetIfController()
{
    SetPdpContext(NULL);
}

void NetIfController::SetPdpContext(PdpContext *pPdpContext)
{
    m_pPdpContext = pPdpContext;
    Reset();
}

// IN RFC4193, fc00::/7 is unique local address,
// This address range can be assigned as IP by Network
bool NetIfController::needToIpv6Rs(DataCall *dc)
{
    if ( dc != NULL && dc->ipv6.valid && dc->ipv6.addr[0] == 0xFE && dc->ipv6.addr[1] == 0x80 )  return true;
    else return false;
}

bool NetIfController::configureIpv4Addr(DataCall *dc)
{
    bool ipset = false;

    // IPv4 configuration
    if (dc->ipv4.valid) {
        if (debug) RilLogV("[%s] %s IPv4 address is valid", TAG, __FUNCTION__);
        if (memcmp(nullIpv4, dc->ipv4.addr, sizeof(nullIpv4)) != 0) {
            ipset = SetIfAddr(m_skfd, m_ifname, dc->ipv4.addr);
            if (ipset)
                RilLogV("[%s] %s SetIfAddr success", TAG, __FUNCTION__);
        }
        else {
            RilLogW("[%s] %s IPv4 is valid but null address", TAG, __FUNCTION__);
        }
    }
    else {
        if (debug) RilLogV("[%s] %s IPv4 address is not valid", TAG, __FUNCTION__);
        ipset = ClearIfAddr(m_skfd, m_ifname);
        if (ipset){
            RilLogV("[%s] %s ClearIfAddr success", TAG, __FUNCTION__);
            ipset=false;
        }
    }
    return ipset;
}

bool NetIfController::configureIpv6Addr(DataCall *dc)
{
    bool ipv6set = false;
    // IPv6 configuration
    if (dc->ipv6.valid) {
        if (debug) RilLogV("[%s] %s IPv6 address is valid", TAG, __FUNCTION__);
        if (memcmp(nullIpv6, dc->ipv6.addr, sizeof(nullIpv6)) != 0) {
            // SetIfAddrIpv6 requires disable_ipv6=0
            set_if_enabled_IPv6(m_ifname, true);

            short flags = GetIfFlags(m_skfdv6, m_ifname);
            if (!SetIfUp(m_skfdv6, m_ifname, flags)) {
                if (debug) RilLogW("[%s] %s fail to SetIfUp for skfdv6:%d", TAG, __FUNCTION__, m_skfdv6);
            }

            processIpv6LinkLocal(dc);

            // This trigger addrconf_dad_completed -> ndisc_send_rs in kernel, requires IF_UP(vnet_open: rmnet*)
            ipv6set = SetIfAddrIpv6(m_skfdv6, m_ifname, dc->ipv6.addr);
            if (ipv6set)
                RilLogV("%s %s set_if_addr_v6 success", TAG, __FUNCTION__);

            set_if_enabled_auto_conf_v6(m_ifname, false);
            if (needToIpv6Rs(dc)) {
                set_if_max_rs_count_v6(m_ifname, 3);
            } else {
                // Ensure not to send RS
                set_if_max_rs_count_v6(m_ifname, 0);
            }
            set_if_enabled_dad_v6(m_ifname, false);
            set_if_max_dad_count_v6(m_ifname, 0);
        }
        else {
            RilLogW("[%s] %s IPv6 is valid but null address", TAG, __FUNCTION__);
        }
    }
    else {
        if (debug) RilLogW("[%s] %s IPv6 address is not valid", TAG, __FUNCTION__);
        set_if_enabled_IPv6(m_ifname, false);
    }
    return ipv6set;
}

// This method does not care about Interface Up/Down
bool NetIfController::ChangeAddress()
{
    RilLogI("[%s] %s", TAG, __FUNCTION__);

    if (!checkSockFd()) return false;

    DataCall *dc = m_pPdpContext->GetDataCallInfo();
    if (dc != NULL && dc->active >= ACTIVE_AND_LINKDOWN) {
        bool ipset = false;
        ipset = configureIpv4Addr(dc);

        bool ipv6set = false;
        ipv6set = configureIpv6Addr(dc);
    }
    return true;
}

bool NetIfController::BringUp()
{
    RilLogI("[%s] %s", TAG, __FUNCTION__);

    if (!checkSockFd()) return false;

    DataCall *dc = m_pPdpContext->GetDataCallInfo();
    if (dc != NULL && dc->active >= ACTIVE_AND_LINKDOWN) {

        // For RIL Reset case or any abnormal case, initialze interface
        TearDown();

        doEmulateIpv4v6ForTest(dc);

        bool ipset = false;
        ipset = configureIpv4Addr(dc);

        bool ipv6set = false;
        ipv6set = configureIpv6Addr(dc);

        if (ipset){
            short flags = GetIfFlags(m_skfd, m_ifname);
            if (!SetIfUp(m_skfd, m_ifname, flags)) {
                if (debug) RilLogW("[%s] %s fail to SetIfUp for skfd:%d", TAG, __FUNCTION__, m_skfd);
            }
        }

        // Strict Fail Policy : If we get valid ip, we should set IP as Interface address
        bool needToSetIpAddr = (dc->ipv4.valid && !ipset) || (dc->ipv6.valid && !ipv6set);
        // Loose Fail Policy : If we can set at least one valid ip address, it's ok
        //bool needToSetIpAddr = (dc->ipv4.valid && !ipset) && (dc->ipv6.valid && !ipv6set);
        if (needToSetIpAddr) {
            if (debug) RilLogW("[%s] %s fail to SetIfAddr", TAG, __FUNCTION__);
            TearDown();
            return false;
        }
    }

    return true;
}

bool NetIfController::TearDown()
{
    RilLogI("[%s] %s", TAG, __FUNCTION__);

    if (!checkSockFd()) return false;

    DataCall *dc = m_pPdpContext->GetDataCallInfo();
    if (dc != NULL && dc->active >= ACTIVE_AND_LINKDOWN) {
        // clear addresses : this will trigger SOCK_DESTROY
        if (!ClearIfAddr(m_skfd, m_ifname)) {
            if (debug) RilLogW("[%s] %s fail to ClearIfAddr", TAG, __FUNCTION__);
        }
        //int index = GetIfIndex(m_skfdv6, m_ifname);
        //if (index >= 0) {
        ClearIfGlobalAddress(m_skfdv6, m_ifname);
        ClearIfLinkLocalAddress(m_skfdv6, m_ifname);
        //clear_if_routes_v6(m_skfdv6, m_ifname, index, 0, 0);
        //clear_if_routes_v6(m_skfdv6, m_ifname, index, RTF_GATEWAY|RTF_ADDRCONF, 0);
        //}
        RilLogW("[%s] %s after ClearIfAddr : %s", TAG, __FUNCTION__, m_ifname);

        short flags = GetIfFlags(m_skfd, m_ifname);
        if (SetIfDown(m_skfd, m_ifname, flags)) {
            return true;
        }
        else {
            if (debug) RilLogW("[%s] %s fail to SetIfDown", TAG, __FUNCTION__);
        }
        return true;
    }

    return false;
}

void NetIfController::Reset()
{
    RilLogI("[%s] %s", TAG, __FUNCTION__);

    if (m_skfd > 0) {
        close(m_skfd);
    }

    if (m_skfdv6 > 0) {
        close(m_skfdv6);
    }

    m_skfd = m_skfdv6 = -1;
    memset(m_ifname, 0, sizeof(m_ifname));

    if (m_pPdpContext != NULL) {
        SECURELIB::strncpy(m_ifname, sizeof(m_ifname), m_pPdpContext->GetInterfaceName(), SECURELIB::strlen(m_pPdpContext->GetInterfaceName()));

        m_skfd = GetIfSock();
        m_skfdv6 = get_if_sock_v6();
    }
    RilLogV("[%s] %s success m_skfd:%d, m_skfdv6:%d", TAG, __FUNCTION__, m_skfd, m_skfdv6);
}

void NetIfController::doEmulateIpv4v6ForTest(DataCall *dc)
{
    if ( dc == NULL ) return;
    if (emulate_ipv4v6_for_test) {
        static bool emulation_ipv4=false;
        if(emulation_ipv4 || (!dc->ipv4.valid && dc->ipv6.valid))
        {
            if(needToIpv6Rs(dc)){
                emulation_ipv4=true;
                dc->ipv4.valid=true;
                inet_pton(AF_INET, "192.168.0.224", (void *)&dc->ipv4.addr);
                RilLogV("emulated ipv4:%lu\n", (unsigned long)dc->ipv4.addr);
            }
            else{
                emulation_ipv4=false;
                dc->ipv4.valid=false;
                memset(&dc->ipv4.addr, 0, sizeof(dc->ipv4.addr));
                RilLogV("emulated ipv4:%lu\n", (unsigned long)dc->ipv4.addr);
            }
        }
    }
}

void NetIfController::processIpv6LinkLocal(DataCall *dc)
{

    if (skip_set_ipv6_linklocaladdr) {
        if (debug) RilLogW("[%s] %s no need to set Link Local", TAG, __FUNCTION__);
        return;
    } else {
        // Available Options not to send RS
        // Disable link Local Address gen mode
        //set_if_addr_gen_mode(m_ifname, 1);
        // Disable accept RA. This will disable ND, RS sending
        //set_if_accept_ra(m_ifname, 0);
        // This is required when autoconf is disabled and addr_gen_mode none
        // To use specific Interface ID
        if (!needToIpv6Rs(dc)) {
            // Need to add Link-Local Address first
            if (debug) RilLogW("[%s] %s Set Link-Local address from Global address skfdv6:%d", TAG, __FUNCTION__, m_skfdv6);
            unsigned char temp_ipv6addr[16];
            memset(&temp_ipv6addr[0], 0, 8);
            temp_ipv6addr[0] = 0xFE;
            temp_ipv6addr[1] = 0x80;
            memcpy(&temp_ipv6addr[8], &dc->ipv6.addr[8], sizeof(char)*8);

            if(!SetIfAddrIpv6(m_skfdv6, m_ifname, temp_ipv6addr))
                RilLogE("[%s] %s Set Link-Local address from Global address skfdv6:%d failed", TAG, __FUNCTION__, m_skfdv6);
        }
    }
}

bool NetIfController::checkSockFd()
{
    if (m_skfd <= 0 || m_skfdv6 <= 0) {
        if (debug) RilLogW("[%s] %s invalid socket, may be need to reset m_skfd:%d, m_skfdv6:%d, will return false", TAG, __FUNCTION__, m_skfd, m_skfdv6);
        return false;
    }
    return true;
}

// lifetime means preferred lifetime
bool NetIfController::UpdateIfAddr(int action, uint8_t family, const char *addr, unsigned int ifa_valid, unsigned int ifa_prefered)
{
    int index = get_if_index(m_skfdv6, m_ifname);
    if (index < 0) {
        return false;
    }
    uint8_t flags = (uint8_t)GetIfFlags(m_skfd, m_ifname);

    RilLogI("%s %s skfd:%d, ifname:%s, index:%d, flags:%d, action:%d, family:%u, ifa_valid:%u, ifa_prefered:%u",
            TAG, __FUNCTION__, m_skfdv6, m_ifname, index, flags, action, family, ifa_valid, ifa_prefered);

    char szAddr[100] = {0, };
    inet_ntop(family, addr, szAddr, (socklen_t)sizeof(szAddr));
    RilLogI("addr:%s", szAddr);

    int rtm_action = RTM_NEWADDR;
    unsigned int preferred = ifa_prefered;
    unsigned int valid = ifa_valid;
    uint16_t netlinkflags = NETLINK_REQUEST_FLAGS;

    switch(action) {
      case IP_CHANGE:
          netlinkflags |= NLM_F_REPLACE;
          [[fallthrough]];
      case IP_ADD:
          flags |= (IFA_F_PERMANENT & ~IFA_F_SECONDARY);
          break;
      case IP_UPDATE:
          netlinkflags |= NLM_F_REPLACE;
          if (preferred == 0)
            flags |= (IFA_F_SECONDARY | IFA_F_DEPRECATED);
          break;
      case IP_DELETE:
          netlinkflags |= NLM_F_REPLACE;
          preferred = 0;
          valid = 1;
          break;
    }

    ifaddrmsg addrmsg = {
        .ifa_family = family,
        .ifa_prefixlen = 32,
        .ifa_flags = flags,
        .ifa_scope = RT_SCOPE_UNIVERSE,
        .ifa_index = (uint32_t)index,   // IFA_F_SECONDARY or IFA_F_PERMANENT
    };

    struct ifa_cacheinfo cacheinfo = {
        .ifa_prefered = preferred,
        .ifa_valid = valid,
        .cstamp = 0,
        .tstamp = 0
    };

    if (addrmsg.ifa_family == AF_INET6) {
        addrmsg.ifa_prefixlen = 128;
    }

    size_t addrlen = (family == AF_INET) ? 4 : 16;
    rtattr rtaLocal = { U16_RTA_LENGTH(addrlen), IFA_LOCAL };
    rtattr rtaAddress = { U16_RTA_LENGTH(addrlen), IFA_ADDRESS };
    rtattr rtaCacheinfo = { U16_RTA_LENGTH(sizeof(cacheinfo)), IFA_CACHEINFO };

    iovec iov[] = {
        { nullptr,              0},
        { &addrmsg,             sizeof(addrmsg) },
        { &rtaLocal,            sizeof(rtaLocal) },
        { (void *)addr,                addrlen},
        { &rtaAddress,          sizeof(rtaAddress) },
        { (void *)addr,                addrlen},
        { &rtaCacheinfo,        sizeof(rtaCacheinfo) },
        { &cacheinfo,           sizeof(cacheinfo) },
    };

    int ret = sendNetlinkRequest(rtm_action, netlinkflags, iov, ARRAY_SIZE(iov));
    if (ret) {
        RilLogE("Error on RTM_NEWADDR: %s", strerror(-ret));
        return false;
    }
    return true;
}

int NetIfController::openNetlinkSocket(int protocol) {
    int sock = socket(AF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, protocol);
    if (sock == -1) {
        return -errno;
    }
    if (connect(sock, reinterpret_cast<const sockaddr*>(&KERNEL_NLADDR),
                sizeof(KERNEL_NLADDR)) == -1) {
        close(sock);
        return -errno;
    }
    return sock;
}

int NetIfController::sendNetlinkRequest(uint16_t action, uint16_t flags, iovec* iov, int iovlen) {
    nlmsghdr nlmsg = {
        .nlmsg_type = action,
        .nlmsg_flags = flags,
    };
    iov[0].iov_base = &nlmsg;
    iov[0].iov_len = sizeof(nlmsg);
    for (int i = 0; i < iovlen; ++i) {
        nlmsg.nlmsg_len += iov[i].iov_len;
    }

    int sock = openNetlinkSocket(NETLINK_ROUTE);
    if (sock < 0) {
        return sock;
    }

    int ret = 0;

    if (writev(sock, iov, iovlen) == -1) {
        ret = -errno;
        RilLogE("netlink socket connect/writev failed (%s)", strerror(-ret));
        close(sock);
        return ret;
    }

    if (flags & NLM_F_ACK) {
        ret = recvNetlinkAck(sock);
    }

    close(sock);

    return ret;
}

int NetIfController::recvNetlinkAck(int sock) {
    struct {
        nlmsghdr msg;
        nlmsgerr err;
    } response;

    int ret = recv(sock, &response, sizeof(response), 0);

    if (ret == -1) {
        ret = -errno;
        RilLogE("netlink recv failed (%s)", strerror(-ret));
        return ret;
    }

    if (ret != sizeof(response)) {
        RilLogE("bad netlink response message size (%d != %zu)", ret, sizeof(response));
        return -EBADMSG;
    }

    return response.err.error;  // Netlink errors are negative errno.
}
