/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include "PsIpv6Handler.h"
#include "rillog.h"
#include "psservice.h"
#include "PsUtil.h"
#include "netifcontroller.h"
#include "pdpcontext.h"
#include "netlink.h"
#include <map>
#include <unistd.h>

typedef struct _timer_param
{
    void *callback_handler;
    int cid;
} timer_param;

PsIpv6Handler::PsIpv6Handler()
{
    m_pPsService = NULL;
    m_pUtil = NULL;
    map_timer_id.clear();
}

PsIpv6Handler::~PsIpv6Handler()
{
}

void PsIpv6Handler::Init(PsService* pPsService)
{
    if(pPsService != NULL)
        m_pPsService = pPsService;
}

void PsIpv6Handler::SetRSRATimeoutTimer(int cid, const char *ifname, std::function<void(int)> cb_func)
{
    struct sigaction sigv;
    struct sigevent sigx;
    struct itimerspec val;
    timer_t temp_t_id;

    sigemptyset(&sigv.sa_mask);
    sigv.sa_flags = SA_SIGINFO;
    //sigv.sa_handler = (sighandler_t)&PsIpv6Handler::TimerHandler_wrapper;
    sigv.sa_sigaction = &PsIpv6Handler::TimerHandler_wrapper;

    if (sigaction(SIGUSR1, &sigv, 0) == -1) {
        RilLog("sigaciton error");
    }

    m_pCallback = cb_func;

    sigx.sigev_notify       = SIGEV_SIGNAL;
    sigx.sigev_signo        = SIGUSR1;
    //sigx.sigev_value.sival_int = gettid();
    timer_param * t_param = new(timer_param);
    sigx.sigev_value.sival_ptr = (void *) t_param;
    t_param->callback_handler = (void *) this;
    t_param->cid = cid;
    //sigx.sigev_notify_function = &PsService::TimerHandler_wrapper;
    //sigx.sigev_notify_attributes = cid;
    sigx.sigev_notify_thread_id = gettid();

    if (map_timer_id.find(cid) == map_timer_id.end()){
        if(timer_create(CLOCK_REALTIME, &sigx, &temp_t_id) == -1){
            RilLogE("timer_create() error");
        }
        else{
            //RilLogV("timer is created with %x", temp_t_id);
            map_timer_id.insert( map<int, timer_t>::value_type(cid, temp_t_id) );
        }
    }
    else{
        temp_t_id = map_timer_id.find(cid)->second;
        RilLog("Reset Timer : %p", temp_t_id);
    }

    // Read Kernel RS Parameters
    int rs_retry = NetIfController::GetIfMaxRsCount(ifname);
    int rs_interval = NetIfController::GetIfRsInterval(ifname);
    int rs_delay = NetIfController::GetIfRsDelay(ifname);
    int rs_timeout = (rs_retry+1) * rs_interval + rs_delay;

    clock_gettime(CLOCK_REALTIME, &val.it_value);
    val.it_value.tv_sec += rs_timeout;
    RilLog("RSRATimeout Timer(%p) will go off at : %s", temp_t_id, ctime(&val.it_value.tv_sec));
    val.it_value.tv_sec = rs_timeout;
    val.it_value.tv_nsec = 0;
    val.it_interval.tv_sec = 0;
    val.it_interval.tv_nsec = 0;

    if(timer_settime(temp_t_id, 0/*TIMER_ABSTIME*/, &val, NULL) == -1){
        RilLog("timer_settimer() error");
    }
}

void PsIpv6Handler::RSRATimeoutCallback(int sigNo, siginfo_t *evp, int cid)
{
    time_t tim = time(0);
    //If Already Completed just ignore event.
    //If not yet Completed
    //Sending RS/RA Fail Event, so complete SETUP_DATA_CALL
    timer_t t_id;
    t_id = map_timer_id.find(cid)->second;
    RilLog("Catched RSRA Timeout t_id:%p, tid:%d, pid:%d, sigNo: %d cid:%d @ %s", t_id, gettid(), evp->si_pid, sigNo, cid, ctime(&tim));
    if(timer_delete(t_id) == -1){
        RilLog("timer_delete() error");
    }
    else
    {
        t_id = 0;
        map_timer_id.erase(cid);
    }
    if(!(cid>0 && cid<9)){
        RilLog("Incorrect CID:%d, Timeout will not be processed", cid);
        return ;
    }

    if(m_pCallback)
        m_pCallback(cid);
    //m_pPsService->RSRATimeoutCallback(cid);
}

void PsIpv6Handler::TimerHandler_wrapper(int sigNo, siginfo_t *evp, void * /* uc */)
{
    if(evp->si_code != SI_TIMER && sigNo != SIGUSR1){
        RilLog("non SI_TIMER signal is received");
        return;
    }
    sigval_t val = evp->si_value;

    timer_param *object = (timer_param *) val.sival_ptr;
    ((PsIpv6Handler *) object->callback_handler)->RSRATimeoutCallback(sigNo, evp, object->cid);
    delete object;
}

int PsIpv6Handler::OnSetupDataCallIPv6Configured(Message *pMsg, const int halVer)
{
    RilLogI("[%s] %s", m_pPsService->GetServiceName(), __FUNCTION__);

    if (pMsg == NULL) {
        RilLogE("pMsg is NULL");
        return -1;
    }

    RilDataRaw *rildata = static_cast<RilDataRaw *>(pMsg->GetUserData());
    if (rildata == NULL) {
        RilLogE("rildata is NULL");
        return -1;
    }

    Ipv6Prefix *ipv6prefix = (Ipv6Prefix *)rildata->GetData();
    if (ipv6prefix == NULL) {
        RilLogE("ipv6prefix is NULL");
        return -1;
    }

     RilLogI("%s(): index=%d from NETLINK", __FUNCTION__, ipv6prefix->index);
     int skfd = NetIfController::GetIfSockV6();
    if ( skfd < 0 )
    {
        RilLogE("socket is minus");
        return -1;
    }

    for (int i = 0; i < m_pPsService->GetPdpContextSize(); i++) {
        PdpContext *pPdpContext = m_pPsService->GetPdpContextByIndex(i);
        if ( pPdpContext == NULL )
        {
            continue;
        }
        int index = NetIfController::GetIfIndex(skfd, pPdpContext->GetInterfaceName());
        RilLogV("%s(): ifname=%s index=%d", __FUNCTION__, pPdpContext->GetInterfaceName(), index);
        if (ipv6prefix->index == index) {
            if (pPdpContext->GetState() == PDP_CONTEXT_IPV6_CONFIGURING
                || (isDeferToGenerateIpv6GlobalAddress) ? (pPdpContext->GetState() == PDP_CONTEXT_CONNECTED) : false
                )
            {
                bool bNeedtoCompleteSetupDataCall = (pPdpContext->GetState() == PDP_CONTEXT_IPV6_CONFIGURING);
                RilLogV("PDP Context{cid=%d,state=%d}, needComplete:%d", pPdpContext->GetCID(), pPdpContext->GetState(), bNeedtoCompleteSetupDataCall);
                DataCall *dc = pPdpContext->GetDataCallInfo();
                if (dc != NULL && dc->ipv6.valid) {
                    memcpy(dc->ipv6.addr, ipv6prefix->prefix.s6_addr, 8);
                    memcpy(dc->ipv6.gw, ipv6prefix->gateway_addr.s6_addr, 16);
                    // Make Gateway address with route source address, needed?
                    //memcpy(dc->ipv6.gw, ipv6prefix->prefix.s6_addr, 8);
                    pPdpContext->OnActivated(dc);

                    int cid = dc->cid;
                    // Here we will change status with 0x1
                    if(!bNeedtoCompleteSetupDataCall && dc->status==PDP_FAIL_NONE)
                        dc->status = PDP_SUCCESS_IPV6_RENEW_PREFIX;
                    int status = dc->status;
                    int active = dc->active;

                    int mtu = (halVer == HAL_VERSION_CODE(1, 6) ? dc->ipv6_mtu_size: dc->ipv4_mtu_size);
                    int pco = dc->pco;

                    RilLogV("===================================================");
                    RilLogV("  OnSetupDataCallIPv6Configured");
                    RilLogV("  CID=%d", cid);
                    RilLogV("  Status=0x%X", status);
                    RilLogV("  Active=%s(%d)", active ? "ACTIVE" : "INACTIVE", active);
                    RilLogV("  MTU=%d, PCO=%d", mtu, pco);
                    RilLogV("===================================================");
                    // print IP information again
                    PsUtil::PrintAddressInfo(dc);

                    if(bNeedtoCompleteSetupDataCall)
                    {
                        // response the result
                        m_pPsService->OnSetupDataCallComplete(RIL_E_SUCCESS, pPdpContext, NULL);
                    }
                    else // PDP_CONTEXT_CONNECTED
                    {
                        // Prefix Reconfiguration Case
                        // notify data call list only
                        m_pPsService->OnNotifyDataCallList();
                        // Recover original status
                        dc->status=PDP_FAIL_NONE;
                    }
                }
            }
            else
            {
                RilLogW("Unexpected PDP Context state");
                RilLogW("PDP Context{cid=%d,state=%d,active=%d}", pPdpContext->GetCID(), pPdpContext->GetState(), pPdpContext->GetActive());
            }
            break;
        }
    } // end for i ~
    close(skfd);

    return 0;
}

bool PsIpv6Handler::DeferToGenerateIpv6GlobalAddress(PdpContext *pPdpContext, int &state)
{
    if(isDeferToGenerateIpv6GlobalAddress) {
        DataCall *dc = pPdpContext->GetDataCallInfo();
        ApnSetting* pApnSetting = pPdpContext->GetApnSetting();
        if( NULL != pApnSetting && pApnSetting->CanHandleType(APN_TYPE_BIT_DEFAULT) &&
            dc->ipv4.valid && dc->ipv6.valid )
        {
            // No need to wait, New Global IPv6 will be reported with DataCallList Changed.
            RilLogV("[%s] %s() Return Immediately with IPv6 Link-local address", m_pPsService->GetServiceName(), __FUNCTION__);
            // We are in already ACTIVE_AND_LINKUP
            //pPdpContext->OnActivated(dc);
            // Force CONNECTED STATE
            pPdpContext->SetState(PDP_CONTEXT_CONNECTED);
            state = PDP_CONTEXT_CONNECTED;
        }
        return true;
    }
    else
        return false;
}

