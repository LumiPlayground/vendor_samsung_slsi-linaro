/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include <chrono>
#include <unistd.h>
#include <cutils/properties.h>
#include <sys/wait.h>
#include <random>
#include <cstring>

#ifndef HAL_VERSION_CODE
#define HAL_VERSION_CODE(m,n)   (((m) & 0xF) << 4 | ((n) & 0xF))
#endif

using std::string;

int INTrand(bool /* timeseed */) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, INT_MAX);
    return dis(gen);

}

/*
vendor/etc/vintf # lshal  -Si | grep radio
alive  android.hardware.radio.config@1.0::IRadioConfig/default
alive  android.hardware.radio.config@1.1::IRadioConfig/default
alive  android.hardware.radio@1.0::IRadio/slot1
alive  android.hardware.radio@1.0::ISap/slot1
alive  android.hardware.radio@1.1::IRadio/slot1
alive  android.hardware.radio@1.1::ISap/slot1
alive  android.hardware.radio@1.2::IRadio/slot1
alive  android.hardware.radio@1.2::ISap/slot1
alive  android.hardware.radio@1.3::IRadio/slot1
alive  android.hardware.radio@1.4::IRadio/slot1
alive  android.hardware.radio@1.5::IRadio/slot1
alive  android.hardware.radio@1.6::IRadio/slot1
alive  vendor.samsung_slsi.telephony.hardware.radio@1.0::IOemSamsungslsi/slot1
alive  vendor.samsung_slsi.telephony.hardware.radio@1.1::IOemSamsungslsi/slot1
alive  vendor.samsung_slsi.telephony.hardware.radioExternal@1.0::IOemSlsiRadioExternal/rilExternal
alive  vendor.samsung_slsi.telephony.hardware.radioExternal@1.1::IOemSlsiRadioExternal/rilExternal
*/

#define die(e) do {fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while(0);

#define HIDL_VERSION "vendor.rild.test.hidlversion"
#define AIDL_VERSION "vendor.rild.test.aidlversion"

static int getRunningHALInfo(std::string halCmd, std::string halOpt, std::string &out) {
    int link[2] = {};
    pid_t pid = -1;
    char foo[4096] = {};
    int ret = 0;

    if (pipe(link)== -1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if (pid == 0) {
        while ((dup2 (link[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {};
        close(link[0]);
        close(link[1]);

        char *cmd;
        char *opt;
        cmd = (char *)halCmd.c_str();
        opt = (char *)halOpt.c_str();
        char *args[] = {cmd, opt, NULL};
        char *envp[] = {NULL};
        execve(cmd, args, envp);
        die("execve");
    } else {
        ssize_t nbytes = 0;
        close(link[1]);
        while(0 != (nbytes = read(link[0], foo, sizeof(foo)))) {
            if(nbytes == -1) {
                if (errno == EINTR) {
                    continue;
                } else {
                    printf("read Errno:%d", errno);
                    exit(1);
                }
            }
            out.append(foo, nbytes);
            memset(foo, 0, sizeof(foo));
        }
        ret = nbytes;
        wait(NULL);
    }
    return ret;
}

// find maximum supported Radio HAL version
static int getRunningRadioHidlVersion(std::string &out) {
    out.reserve(65535);
    getRunningHALInfo(string("/bin/lshal"), string("-Si"), out);
    size_t pos = 0;
    size_t live_pos = 0;
    // We does not check other interface of HIDL
    // These are alive HIDL with AIDL IRadio HAL 2.0
    // android.hardware.radio@1.0::ISap/slot1
    // android.hardware.radio@1.0::ISap/slot2
    // android.hardware.radio@1.1::ISap/slot1
    // android.hardware.radio@1.1::ISap/slot2
    // android.hardware.radio@1.2::ISap/slot1
    // android.hardware.radio@1.2::ISap/slot2
    // vendor.samsung_slsi.telephony.hardware.radio*
    if ((pos = out.find("android.hardware.radio@1.6::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 6);
    }
    if ((pos = out.find("android.hardware.radio@1.5::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 5);
    }
    if ((pos = out.find("android.hardware.radio@1.4::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 4);
    }
    if ((pos = out.find("android.hardware.radio@1.3::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 3);
    }
    if ((pos = out.find("android.hardware.radio@1.2::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 2);
    }
    if ((pos = out.find("android.hardware.radio@1.1::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 1);
    }
    if ((pos = out.find("android.hardware.radio@1.0::IRadio")) != string::npos) {
        if ((live_pos = out.find("alive", pos-7)) == (pos-7)) return HAL_VERSION_CODE(1, 0);
    }
    return 0;
}

// TBD : Need to find proper Radio HAL AIDL Dumpsys name
static int getRunningRadioAidlVersion(std::string &out) {
    out.reserve(65535);
    getRunningHALInfo(string("/bin/service"), string("list"), out);
    size_t pos = 0;
    if ((pos = out.find("android.hardware.radio.data.IRadioData/slot1:")) != string::npos) {
        return HAL_VERSION_CODE(2, 0);
    }
    return 0;
}

int getRunningRadioHalVersion(std::string &out) {
    int aidl = 0, hidl = 0;
    char aidlp[PROPERTY_VALUE_MAX] = {};
    char hidlp[PROPERTY_VALUE_MAX] = {};
    property_get(HIDL_VERSION, hidlp, NULL);
    property_get(AIDL_VERSION, aidlp, NULL);
    hidl = strtol(hidlp, NULL, 10);
    aidl = strtol(aidlp, NULL, 10);
    if (aidl) return aidl;
    if (hidl) return hidl;
    aidl = getRunningRadioAidlVersion(out);
    hidl = getRunningRadioHidlVersion(out);
    if (aidl) {
        property_set(AIDL_VERSION, std::to_string(aidl).c_str());
        return aidl;
    }
    if (hidl) {
        property_set(HIDL_VERSION, std::to_string(hidl).c_str());
        return hidl;
    }
    return 0;
}
