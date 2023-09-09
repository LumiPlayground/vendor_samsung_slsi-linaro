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
 * modemstatemonitor.cpp
 *
 *  Created on: 2014. 11. 17.
 *      Author: sungwoo48.choi
 */


#include "modemstatemonitor.h"
#include "rfsservice.h"
#include "main.h"
#include <sys/poll.h>

ModemStateMonitor::ModemStateMonitor(CRfsService *pRfsSrv)
{
    m_pRfsSrv = pRfsSrv;
    m_pMonitorThread = NULL;
    mNvCheckOnCrash = true;
}

ModemStateMonitor::~ModemStateMonitor()
{

}

int ModemStateMonitor::Start()
{
    m_pMonitorThread = new Thread(this);
    if (m_pMonitorThread->Start() < 0) {
        ALOGE("[ModemStateMonitor::%s] thread errors", __FUNCTION__);
        return -1;
    }
    ALOGD("[ModemStateMonitor::%s] thread ok", __FUNCTION__);

    return 0;
}

INT32 ModemStateMonitor::GetModemStatus(INT32 fd)
{
    INT32 status = 0;
    ALOGE("[ModemStateMonitor::%s]", __FUNCTION__);

    if ( fd < 0 )
    {
        ALOGE("cannot get modem status - due to fd is invalid(%d)", fd);
        return -1;
    }

    status = ioctl(fd, IOCTL_MODEM_STATUS);

    return status;
}


void ModemStateMonitor::OnClose()
{
}

void ModemStateMonitor::Run()
{
    int32_t fd;
    struct pollfd pollfd;
    int ret = 0;
    int state = -1;

    fd = open("/dev/umts_boot0", O_RDWR | O_CLOEXEC);
    pollfd.fd = fd;
    pollfd.events = POLLHUP | POLLIN | POLLRDNORM;
    pollfd.revents = 0;

    ALOGD("[ModemStateMonitor::%s] Start modem state monitor!", __FUNCTION__);
    while(1)
    {
        pollfd.revents = 0;
        ret = poll(&pollfd, 1, -1);

        if((pollfd.revents & POLLHUP) || (pollfd.revents & POLLIN) || (pollfd.revents & POLLRDNORM))
        {
            ALOGD("[ModemStateMonitor::%s] receive poll event!!!", __FUNCTION__);
            const unsigned int interval = 2;
            while(true) {
                state = ioctl(fd, IOCTL_MODEM_STATUS);
                ALOGD("[ModemStateMonitor::%s] modem status [%d]", __FUNCTION__, state);

                switch (state) {
                case STATE_OFFLINE:
                case STATE_BOOTING: {
                    int spin = 300;
                    ALOGE("[ModemStateMonitor::%s] Set spin count %d. Start to monitor until modem online", __FUNCTION__, spin);
                    while(spin--) {
                        state = ioctl(fd, IOCTL_MODEM_STATUS);
                        if(state == STATE_ONLINE) {
                            ALOGE("Break out monitoring");
                            break;
                        }
                        usleep(100 * 1000);
                    }
                    if(spin < 0) {
                        ALOGE("[ModemStateMonitor::%s] Modem boot timeout", __FUNCTION__);
                    }
                    break;
                }
                case STATE_ONLINE:
                    break;
                case STATE_CRASH_EXIT:
                case STATE_CRASH_RESET: {
                    OnModemCrashOrReset();
                    // wait an intervale for the next polling to avoid busy state of MIF driver.
                    sleep(interval);

                    // update
                    // modem state can be STATE_ONLINE directly after sleep interval.
                    // no poll signal when a state become STATE_ONLINE.
                    state = ioctl(fd, IOCTL_MODEM_STATUS);
                    break;
                }
                default:
                    break;
                }  // end switch ~

                if(state == STATE_ONLINE) {
                    OnModemBootDone();
                }
                break;
            }
        }
        else {
            ALOGE("[ModemStateMonitor::%s] unknown poll event!", __FUNCTION__);
            usleep(200000);
        }
    }
    close(fd);
    ALOGV("[ModemStateMonitor::%s] Stop monitoring Modem Status ...", __FUNCTION__);
    return;
}

void ModemStateMonitor::OnModemBootDone()
{
    ALOGD("[ModemStateMonitor::%s] Modem is ONLINE", __FUNCTION__);

    // flag on for the next CP crash
    mNvCheckOnCrash = true;
}

void ModemStateMonitor::OnModemCrashOrReset()
{
    ALOGD("[ModemStateMonitor::%s] Modem  is STATE_CRASH_EXIT or STATE_CRASH_RESET", __FUNCTION__);
    if (mNvCheckOnCrash) {
        ALOGE("[ModemStateMonitor::%s] Start NV validation by CP crash", __FUNCTION__);
        m_pRfsSrv->CheckNvFiles();
        mNvCheckOnCrash = false;
    }
}

