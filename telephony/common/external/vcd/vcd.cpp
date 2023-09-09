/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

#include <cutils/properties.h>
#include <sys/prctl.h>
#include <sys/capability.h>
#include <cutils/android_filesystem_config.h>


#include "vcd.h"
#include "vcd_socket.h"
#include "rilclienthelper.h"
#include "utils.h"
#include "HostAgentManager.h"
#include "ModemProxy.h"
#include "base/log.h"

#define PATH_MODEM            "/dev/umts_router"
#define PATH_ATC              "/dev/umts_atc0"
#define PATH_USB              "/dev/ttyGS0"
#define PATH_USB2             "/dev/ttyGS2"
#define PATH_SERIAL           "/dev/ttySAC1"
#define UART_PATH_UARTSEL     "/sys/class/sec/switch/uart_sel"
#define COMMAND_LINE_FILE     "/proc/cmdline"
#define KERNEL_CONSOLE_CHECK  "console=ttySAC1,115200n8"
#define MAX_BUF               65536
#define OK_STRING             "OK\n"
#define AT_STRING             "AT\r\n"
#define BOOT_COMPLETE_STRING  "BOOT COMPLETE\n"
#define IOCTL_MODEM_STATUS    _IO('o', 0x27)
#define PROPERTY_DUN_SETTINGS "persist.vendor.radio.dun_settings"

enum modem_state {
    STATE_OFFLINE,
    STATE_CRASH_RESET,          /* silent reset */
    STATE_CRASH_EXIT,           /* cp ramdump */
    STATE_BOOTING,
    STATE_ONLINE,
};

static int32_t g_usb_fd2 = -1;
static int32_t g_modem_fd = -1;
static int32_t g_atc_fd = -1;
static int32_t g_serial_fd = -1;
static int32_t g_serial_decision_check = -1;
static int32_t g_board_type_check = 0;
static int32_t g_cp_at_ok_check = 0;
static char g_dun_mode[100] = {0, };
RilClientHelper *g_pRilClientHelper = NULL;
HANDLE g_client;
FILE *infile;

static int s_fdWakeupRead2;
static int s_fdWakeupWrite2;

// static void OnUnsolicitedResponse(unsigned int msgId, void* data, size_t length, unsigned int channel);

void close_modem(void)
{
    if (g_modem_fd != -1) {
        close(g_modem_fd);
        g_modem_fd = -1;
    }
}

void close_atc(void)
{
    if (g_atc_fd != -1) {
        close(g_atc_fd);
        g_atc_fd = -1;
    }
}

void close_usb2(void)
{
    if (g_usb_fd2 != -1) {
        int ret = close(g_usb_fd2);
        if (ret < 0) {
            ALOGD("%s close fd2=%d ret=%d errno=%d", __FUNCTION__, g_usb_fd2, ret, errno);
        }
        g_usb_fd2 = -1;
    }
}

void close_serial(void)
{
     if (g_serial_fd != -1) {
        close(g_serial_fd);
        g_serial_fd = -1;
    }
}

void close_interface(void)
{
    close_usb2();
    close_modem();
    close_atc();
    close_serial();
}

int32_t init_usb2(void)
{
    struct termios usb_termios;

    while(1) {
        g_usb_fd2 = open(PATH_USB2, O_RDWR | O_CLOEXEC);
        if (g_usb_fd2 < 0) {
            sleep(1);
            continue;
        } else {
            ALOGD("%s : Success to open %s, fd = %d", __func__, PATH_USB2, g_usb_fd2);
            break;
        }
    }

    //set params
    memset((char *)&usb_termios, 0, sizeof(struct termios));

    if (tcgetattr(g_usb_fd2, &usb_termios) < 0) {
        ALOGE("%s : Fail to get attributes from %s, error = %d", __func__, PATH_USB2, errno);
        return -1;
    }

    cfmakeraw(&usb_termios);

    usb_termios.c_iflag &= ~ICRNL;
    usb_termios.c_iflag &= ~INLCR;
    usb_termios.c_oflag &= ~OCRNL;
    usb_termios.c_oflag &= ~ONLCR;
    usb_termios.c_lflag &= ~ICANON;
    usb_termios.c_lflag &= ~ECHO;

    if (tcsetattr(g_usb_fd2, TCSANOW, &usb_termios) < 0) {
        ALOGE("%s : Fail to set attributes to %s, error = %d", __func__, PATH_USB2, errno);
        return -1;
    }
    return 0;
}

int32_t init_modem(void)
{
    while(1) {
        g_modem_fd = open(PATH_MODEM, O_RDWR | O_CLOEXEC);
        if (g_modem_fd < 0) {
            ALOGE("%s : Fail to open %s, errno = %d", __func__, PATH_MODEM, errno);
            sleep(1);
            continue;
        } else {
            break;
        }
    }
    LOGV("%s fd=%d", PATH_MODEM, g_modem_fd);
    return 0;
}

int get_modem_fd(void)
{
    return g_modem_fd;
}

int32_t init_atc(void)
{
    while(1) {
        g_atc_fd = open(PATH_ATC, O_RDWR | O_CLOEXEC);
        if (g_atc_fd < 0) {
            sleep(1);
            continue;
        } else {
            ALOGD("%s : Success to open %s, fd = %d", __func__, PATH_ATC, g_atc_fd);
            break;
        }
    }
    return 0;
}


int32_t init_serial(void)
{
    if(g_board_type_check==1)       //board is mochagw01
    {
    if(g_serial_decision_check == -1 || g_serial_decision_check == 1)   // re-init allowed only when in console mode
    {
        struct termios newtio;

        char cmdlinebuf[200];

        int readcnt =0;
        memset(cmdlinebuf, 0, 200);

        // FILE *cmdline_file = NULL;

        readcnt = GetSysFS(COMMAND_LINE_FILE , cmdlinebuf);

        if(g_serial_decision_check == -1)   //only first time show log
        {
            ALOGD("%s : COMMAND_LINE_FILE open = %s", __func__, cmdlinebuf);
            ALOGD("%s : COMMAND_LINE_FILE readcnt = %d", __func__, readcnt);
        }

        if(readcnt>0)
        {
            int ii=0;
            char *confind;
            for(;ii<readcnt -24; ii++)
            {
                confind= &cmdlinebuf[ii];
                if(commandcmp(confind,KERNEL_CONSOLE_CHECK,24)==0)
                {
                    ALOGD("%s : KERNEL_CONSOLE_CHECK found at index = %d", __func__, ii);
                    g_serial_decision_check = 0;
                    break;
                }
                g_serial_decision_check = 1;    //kernel is not using console , do init console
            }
        }

        if(g_serial_decision_check==1)
        {
             ALOGD("%s : KERNEL_CONSOLE_CHECK not found !! do console initialization", __func__);

            while(1) {

                g_serial_fd = open( PATH_SERIAL, O_RDWR | O_NOCTTY | O_CLOEXEC);
                if (g_serial_fd < 0) {
                    ALOGE("%s : Fail to open %s, errno = %d", __func__, PATH_SERIAL, errno);
                    sleep(1);
                    continue;
                } else {
                    break;
                }
            }
            ALOGD("%s : g_serial_fd open = %d", __func__, g_serial_fd);

            memset((char *)&newtio, 0, sizeof(newtio));
            if (tcgetattr(g_serial_fd, &newtio) < 0) {
                ALOGE("%s : Fail to get attributes from %s, error = %d", __func__, PATH_SERIAL, errno);
                return -1;
            }

            cfmakeraw(&newtio);

            newtio.c_iflag = IGNPAR; // non-parity
            newtio.c_oflag = 0;
            newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts
            newtio.c_cflag |= B115200;

            //set input mode (non-canonical, no echo,.....)
            newtio.c_lflag = 0;
            newtio.c_cc[VTIME] = 10; // timeout
            newtio.c_cc[VMIN] = 32;

             if (tcsetattr(g_serial_fd, TCSANOW, &newtio) < 0) {
                ALOGE("%s : Fail to set attributes to %s, error = %d", __func__, PATH_SERIAL, errno);
                return -1;
                }
            }
        }
    }
    return 0;
}

int32_t init_interface(void)
{
    if (init_serial() < 0) {
        return -1;
    }
    return 0;
}

int32_t write_to_interface(int32_t fd, const char* buffer, int32_t buf_len)
{
    int32_t write_len = 0;
    int32_t len = 0;

    if (buffer == NULL || buf_len <= 0) {
        ALOGD("%s : invalid parameter\n", __FUNCTION__);
        return -1;
    }

    do {
        if ((len = write(fd, buffer + write_len, (buf_len - write_len))) < 0) {
            ALOGE("%s : Fail to write", __func__);
            if (errno == EINTR || errno == EAGAIN)
                continue;
            return -1;
        }
        write_len += len;
    } while (write_len < buf_len);
    return write_len;
}

void *run_modem_monitor(void * /* arg */)
{
    char buffer[MAX_BUF];

    while(1) {
        init_modem();
        int fd = get_modem_fd();
        while (true) {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            int n = select(fd + 1, &rfds, NULL, NULL, NULL);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                ALOGE("%s : Fail to monitor %s, errno = %d", __func__, PATH_MODEM, errno);
                break;
            }

            if (FD_ISSET(fd, &rfds)) {
                int r = read(fd, buffer, MAX_BUF - 1);
                if (r < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    LOGE("%s : Fail to read %s, errno = %d", __func__, PATH_MODEM, errno);
                    break;
                } else if (r == 0) {// status unpluged
                    LOGE("%s : No data on %s", __func__, PATH_MODEM);
                    break;
                }

                LOGV("%s : fd=%d %d byte(s)", __func__, fd, r);
                char *checkok = &buffer[0];
                buffer[r] = 0;
                if(strstr (checkok, "OK") != NULL && g_cp_at_ok_check==0)
                {
                    LOGD("%s :Success OK found", __func__);
                    g_cp_at_ok_check =1;
                }

                PrintATCommands("From Modem:", buffer, r);
                HostAgent *activeAgent = HostAgentManager::GetInstance()->GetActiveAgent();
                if (activeAgent != NULL) {
                    activeAgent->Write(buffer, r);
                }
            } // end while ~ (reader)
        } // end while ~ (reader)
        close_modem();
    } // end while ~
    return NULL;
}

void *run_atc_monitor(void * /* arg */)
{
    int32_t n;
    fd_set rfds;
    char buffer[MAX_BUF];

    while(1) {
        if (g_atc_fd < 0) {
            init_atc();
        }
        int32_t fd = g_atc_fd;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        n = select(fd + 1, &rfds, NULL, NULL, NULL);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("%s : Fail to monitor %s, errno = %d", __func__, PATH_ATC, errno);
            close_atc();
            continue;
        }

        if (FD_ISSET(fd, &rfds)) {
            n = read(fd, &buffer, MAX_BUF - 1);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                ALOGE("%s : Fail to read %s, errno = %d", __func__, PATH_ATC, errno);
                close_atc();
                continue;
            } else if (n == 0) {// status unpluged
                ALOGE("%s : No data on %s", __func__, PATH_ATC);
                close_atc();
                continue;
            }

            ALOGD("%s : Read %s n = %d", __func__, PATH_ATC, n);
            char *checkok = &buffer[0];
            buffer[n] = 0;
            if(strstr (checkok, "OK") != NULL && g_cp_at_ok_check==0)
            {
                ALOGD("%s :Success OK found", __func__);
                g_cp_at_ok_check =1;
            }

            if (n > 0) {
                HexDump(buffer, n);
                PrintATCommands("To USB2", buffer, n);
                write_to_interface(g_usb_fd2, buffer, n);
            }
        }
    }
    return NULL;
}

void *run_usb2_monitor(void * /* arg */)
{
    int32_t n;
    fd_set rfds;
    char buffer[MAX_BUF];
    int32_t fd;

    while(1) {
        if (g_usb_fd2 < 0) {
            init_usb2();
        }

        fd = g_usb_fd2;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        FD_SET(s_fdWakeupRead2, &rfds);
        int maxFd = fd;
        if (maxFd < s_fdWakeupRead2)
            maxFd = s_fdWakeupRead2;

        n = select(maxFd + 1, &rfds, NULL, NULL, NULL);

        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("%s : Fail to monitor %s, errno = %d", __func__, PATH_USB2, errno);
            close_usb2();
            continue;
        }

        if (FD_ISSET(fd, &rfds)) {
            memset(buffer, '\0', MAX_BUF);
            n = read(fd, &buffer, MAX_BUF - 1);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                ALOGE("%s : Fail to read %s, errno = %d", __func__, PATH_USB2, errno);
                close_usb2();
                continue;

            } else if (n == 0) { // status unpluged
                ALOGE("%s : No data on %s", __func__, PATH_USB2);
                close_usb2();
                continue;
            }

            PrintATCommands("From USB2", buffer, n);

            // route to ATC
            if (write_to_interface(g_atc_fd, (char *)buffer, n) > 0) {
                PrintATCommands("To ATC", buffer, n);
            }
        }
        else if (FD_ISSET(s_fdWakeupRead2, &rfds)) {
            ALOGE("%s : wakeup", __FUNCTION__);
            n = read(s_fdWakeupRead2, &buffer, 1);
            close_usb2();
        }
    }
    return NULL;
}

// test end
void start_modem_monitor_thread(void)
{
    pthread_t thread_id_modem = 0;
    //run thread
    pthread_create(&thread_id_modem, NULL, run_modem_monitor, &g_modem_fd);
}

void start_atc_monitor_thread(void)
{
    pthread_t thread_id_atc = 0;
    //run thread
    pthread_create(&thread_id_atc, NULL, run_atc_monitor, &g_atc_fd);
}

void start_usb2_monitor_thread(void)
{
    int fdSet[2];
    int ret2pipe = pipe2(fdSet, O_CLOEXEC);
    if (ret2pipe == 0) {
        s_fdWakeupRead2 = fdSet[0];
        s_fdWakeupWrite2 = fdSet[1];
    }

    pthread_t thread_id_usb2 = 0;
    //run thread
    pthread_create(&thread_id_usb2, NULL, run_usb2_monitor, &g_usb_fd2);
}

void start_monitor_thread(void)
{
    start_modem_monitor_thread();
    start_atc_monitor_thread();
    start_usb2_monitor_thread();
}

void switch_user(void)
{
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    if (setuid(AID_SYSTEM) != 0) {
        ALOGE("setuid() failed : %s", strerror(errno));
    }

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;

    cap.effective = cap.permitted = ( 1 << CAP_SYS_BOOT) | (1 << CAP_NET_ADMIN) |\
                    (1 << CAP_SYS_ADMIN) | (1 << CAP_NET_RAW) | (1<<CAP_DAC_OVERRIDE);

    cap.inheritable = 0;
    capset(&header, &cap);
}

int32_t main(void)
{
    ALOGE("%s : enter ", __func__);

    // get property for checking dun mode
    property_get(PROPERTY_DUN_SETTINGS, g_dun_mode, "0");
    ALOGD("dun mode = %c", g_dun_mode[0]);
    if (g_pRilClientHelper == NULL) {
        g_pRilClientHelper = RilClientHelper::GetInstance();
    }

    /*
    if (g_pRilClientHelper != NULL) {
        for (int i = 0; i < 10; ++i) {
            g_client = g_pRilClientHelper->Open();
            if (g_client != 0) {
                ALOGD("g_pRilClientHelper is opened");
                break;
            }
            else usleep(200000);
        }
        g_pRilClientHelper->RegisterUnsolicitedResponseHandler(g_client, OnUnsolicitedResponse);
    }
    */

    HostAgentManager::Init();

    start_monitor_thread();

    while(1) {
        sleep(INT_MAX);
    }
    close_interface();
    return 0;
}

#if 0  // unused currently
void OnUnsolicitedResponse(unsigned int msgId, void* data, size_t length, unsigned int channel)
{
    if (msgId == RILC_UNSOL_PIN_CONTROL)
    {
        char *res = (char *)data;
        ALOGD("%s() msgId = %d, signal = %d, status = %d", __FUNCTION__, msgId, res[0], res[1]);
        /*
                * data[0] - signal
                * 0x00 = PIN_CTRL_SIGNAL_NONE, 0x01 = PIN_CTRL_SIGNAL_DCD, 0x02 = PIN_CTRL_SIGNAL_DTR
                * data[1] = status
                * 0x00 = PIN_CTRL_STATUS_OFF, 0x01 = PIN_CTRL_STATUS_ON
                */
        if (res[0] == 1 && res[1] == 0)
        {
            ALOGD("%s close", __FUNCTION__);

            HostAgent *activeAgent = HostAgentManager::GetInstance()->GetActiveAgent();
            if (activeAgent != NULL) {
                activeAgent->Reset();
            }

            int ret;
            do {
                ret = write (s_fdWakeupWrite2, " ", 1);
            } while (ret < 0 && errno == EINTR);
        }
    }
}
#endif
