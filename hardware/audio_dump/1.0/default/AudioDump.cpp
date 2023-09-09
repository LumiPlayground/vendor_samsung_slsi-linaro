#define LOG_TAG "AudioDump"
#define LOG_NDEBUG 0

#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fnmatch.h>
#include <unistd.h>
#include <log/log.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <android/hardware/audio/7.0/IDevicesFactory.h>
#include <android/hardware/audio/7.0/IDevice.h>
#include <android/hardware/audio/7.0/types.h>

#include "audio_abox2.h"
#include "AudioDump.h"

#define DUMP_VERSION "1.3"

using android::hardware::audio::V7_0::IDevicesFactory;
using android::hardware::audio::V7_0::IDevice;
using android::hardware::audio::V7_0::Result;
using android::hardware::audio::V7_0::ParameterValue;
using ::android::hardware::hidl_vec;
using android::sp;

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace audio_dump {
namespace V1_0 {
namespace implementation {

enum {
    NONE_DBG = -1,
    AUDIO_DBG = 0,
    GENERAL_DBG = 1,
    SILENT_DBG = 2,
    AUDIO_DUMP_COUNT
};

static const char *dump_mode_name[AUDIO_DUMP_COUNT] = {
    [AUDIO_DBG]     = "AUDIO_LOGGING",
    [GENERAL_DBG]   = "GENERAL_LOGGING",
    [SILENT_DBG]    = "SILENT_LOGGING",
};

// General defines
#define AUDIO_PARAM_AUDIO_LOGGING       "audiologging"
#define AUDIO_PARAM_GENERAL_LOGGING     "generallogging"
const char LOGGING_START[] = "start";
const char LOGGING_STOP[] = "stop";

#define OUT_TARGET_PATH                 "/data/vendor/log/abox/"
#define ABOX_LOG_PATH                   "/proc/abox/"
#define ABOX_DUMP_PATH                  "/proc/abox/dump/"

#define SILENT_LOG_DUMP_NAME            "log"
#define OUT_SILENT_LOG_DUMP_NAME        "abox-silentlog"
#define OUT_SILENT_LOG_EXTENSION        ".adm"
#define OUT_PCM_DUMP_EXTENSION          ".raw"
#define ABOX_LOG_DUMP_NAME              "log-00"
#define CIRCULAR_HX_DUMP_NAME           "hx/dump"
#define OUT_CIRCULAR_HX_DUMP_NAME       "abox_hxdump"

#define BUFFER_SIZE                     4096
#define AUDIO_DUMP_START                1
#define AUDIO_DUMP_STOP                 0
#define MAX_DUMP_COUNT                  1

#ifdef SUPPORT_VTS_LOGGING
#define VTS_LOG_PATH                    "/proc/vts/"
#define VTS_DUMP_PATH                   "/proc/vts/dump/"
#define VTS_LOG_DUMP_NAME               "vts-log"
#define VTS_NODE_NAME_SZ                9
#define VTS_PCM_DUMP_CNT                2

const char supported_vts_dump[VTS_PCM_DUMP_CNT][VTS_NODE_NAME_SZ] = {"vts_tri", "vts_rec"};
#endif

struct Dump_Info {
    std::string src_name;
    std::string dst_name;
    std::string src_path;
    std::string dst_path;
    std::string dstout_type;
    pthread_t tid;
    int src_fd;
    int dst_fd;
    int num;
};

struct Dump_Info pcm_dump[PCM_DUMP_CNT];
struct Dump_Info aboxlog_dump;
struct Dump_Info silent_dump;
struct Dump_Info circular_dump;

#ifdef SUPPORT_VTS_LOGGING
struct Dump_Info vts_pcm_dump[VTS_PCM_DUMP_CNT];
struct Dump_Info vtslog_dump;
#endif
static bool pcm_dump_stat;
uint32_t en_logging = NONE_DBG;
char time_str[32];

void thread_exit_handler(int sig)
{
    ALOGI("%s: tid(%lu)  sig(%d)", __func__, pthread_self(), sig);
    pthread_exit(0);
}

void remove_old_dump(std::string dst_name)
{
    struct dirent **list;
    std::string pattern;
    int n, m;

    ALOGD("%s(%s, %s)", __func__, OUT_TARGET_PATH, dst_name.c_str());

    pattern = dst_name + "*";

    n = scandir(OUT_TARGET_PATH, &list, NULL, alphasort);
    if (n < 0) {
        ALOGE("%s: scandir failed: %s", __func__, strerror(errno));
        return;
    }
    m = 0;

    while (n--) {
        if (!fnmatch(pattern.c_str(), list[n]->d_name, FNM_FILE_NAME)) {
            if (++m >= MAX_DUMP_COUNT) {
                char *tgt;

                if (asprintf(&tgt, "%s/%s", OUT_TARGET_PATH, list[n]->d_name) != -1) {
                    remove(tgt);
                    free(tgt);
                }
            }
        }
        free(list[n]);
    }
    free(list);
}

static void save_circular_dump(struct Dump_Info *pdump_info)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes = 0;

    do {
        bytes = read(pdump_info->src_fd, buffer, BUFFER_SIZE);
        if (bytes > 0) {
            if ((write(pdump_info->dst_fd, buffer, bytes)) < 0) {
                ALOGE("%s: ERROR: Failed to write the %s file. (%s)",
                       __func__, pdump_info->dst_name.c_str(), strerror(errno));
            }
        }
    } while(bytes > 0);

    ALOGI("%s: finish General dump copy filename=(%s)", __func__, pdump_info->dst_name.c_str());

    return;
}

static void *save_audio_dump(void *info)
{
    struct Dump_Info *pdump_info = (struct Dump_Info *)info;
    char buffer[BUFFER_SIZE];
    ssize_t bytes = 0;
    pthread_t tid = pthread_self();
    struct sigaction actions;

    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = thread_exit_handler;
    sigaction(SIGINT, &actions, NULL);

    do {
        if (pcm_dump_stat == AUDIO_DUMP_STOP)
            break;

        bytes = read(pdump_info->src_fd, buffer, BUFFER_SIZE);
        if (bytes > 0) {
            if ((write(pdump_info->dst_fd, buffer, bytes)) < 0) {
                ALOGE("%s: ERROR: thread=(%lu): Failed to write the %s file. (%s)",
                       __func__, tid, pdump_info->dst_name.c_str(), strerror(errno));
            }
        }
    } while(1);

    ALOGI("%s: finish copy with thread=(%lu) filename=(%s)", __func__, tid, pdump_info->dst_name.c_str());

    return NULL;
}

int32_t open_dump_fd(struct Dump_Info *pdump_info)
{
    std::string src_filename;
    std::string dst_filename;
    int result = 0;

    pdump_info->src_fd = -1;
    pdump_info->dst_fd = -1;

    src_filename = pdump_info->src_path + pdump_info->src_name;
    ALOGI("%s: src path filename=(%s)", __func__, src_filename.c_str());

    pdump_info->src_fd = open(src_filename.c_str(), O_RDONLY);
    if (pdump_info->src_fd < 0) {
        ALOGE("%s: ERROR: failed to open %s file. (%s)",
               __func__, src_filename.c_str(), strerror(errno));
        goto err;
    }

    dst_filename = pdump_info->dst_path + pdump_info->dst_name;
    dst_filename = dst_filename + "_" + time_str + pdump_info->dstout_type;
    ALOGI("%s: dst path filename=(%s)", __func__, dst_filename.c_str());

    pdump_info->dst_fd = open(dst_filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (pdump_info->dst_fd < 0) {
        ALOGE("%s: ERROR: failed to open %s file. (%s)", __func__, dst_filename.c_str(), strerror(errno));
        goto err;
    }

    result = chmod(dst_filename.c_str(), 0660);
    if(result != 0)
        ALOGE("failed to change mode for %s, error = %s\n", dst_filename.c_str(), strerror(errno));

    fchmod(pdump_info->dst_fd, 0755);

    return 0;

err:
    if (pdump_info->src_fd >= 0)
        close(pdump_info->src_fd);

    if (pdump_info->dst_fd >= 0)
        close(pdump_info->dst_fd);

    return -1;
}

void close_dump_fd(struct Dump_Info *pdump_info)
{
    if (pdump_info->src_fd >= 0) {
        close(pdump_info->src_fd);
        pdump_info->src_fd = -1;
    }

    if (pdump_info->dst_fd >= 0) {
        close(pdump_info->dst_fd);
        pdump_info->dst_fd = -1;
    }

    ALOGI("%s: exit", __func__);

    return;
}

uint32_t set_dump_status_tohal(uint32_t mode, const char *status)
{
    sp<IDevicesFactory> audio_hidl = IDevicesFactory::getService();
    sp<IDevice> audiohal_handle = nullptr;
    hidl_vec<ParameterValue> value;

    value.resize(1);

    switch (mode) {
    case AUDIO_DBG:
    case SILENT_DBG:
        value[0].key = AUDIO_PARAM_AUDIO_LOGGING;
        break;
    case GENERAL_DBG:
        value[0].key = AUDIO_PARAM_GENERAL_LOGGING;
        break;
    default:
        ALOGE("%s: mode[%d] is not defined", __func__, mode);
        goto err;
    }

    value[0].value = status;

    if (audio_hidl == nullptr) {
        ALOGE("%s: failed to get Audio hidl service", __func__);
        goto err;
    } else {
        audio_hidl->openDevice("primary", [&](const Result &ret, sp<IDevice> dev_handle) {
                                              audiohal_handle = dev_handle;
                                          });
        if (audiohal_handle == nullptr) {
            ALOGE("%s: failed to get Audiohal handle", __func__);
            goto err;
        } else {
            if (audiohal_handle->setParameters(NULL, value) != Result::OK) {
                ALOGE("%s: failed to send param to Audiohal", __func__);
                goto err;
            }
        }
    }

    return 0;
err:
    ALOGE("%s: failed to set dump status to Audiohal", __func__);
    return -1;
}

void start_audio_dump(uint32_t mode)
{
    int i;
    time_t t;
    struct tm *timeptr;

    ALOGI("%s: enter", __func__);

    t = time(NULL);
    timeptr = localtime(&t);
    if (strftime(time_str, sizeof(time_str), "%Y%m%d-%H%M%S", timeptr) == 0) {
        ALOGE("%s: Failed to get the formatted time for current time", __func__);
        time_str[0] = '\0';
    }

    set_dump_status_tohal(mode, LOGGING_START);

    switch (mode) {
    case AUDIO_DBG:
        for (i = 0; i < PCM_DUMP_CNT; i++) {
            remove_old_dump(pcm_dump[i].dst_name);
            if (open_dump_fd(&pcm_dump[i]) == 0) {
                if (pthread_create(&pcm_dump[i].tid, NULL, save_audio_dump, (void *)&pcm_dump[i]) != 0)
                    ALOGE("%s: fail to create the pcm_dump[%d] copy thread  (%s)", __func__, i, strerror(errno));
            }
        }

        remove_old_dump(aboxlog_dump.dst_name);
        if (open_dump_fd(&aboxlog_dump) == 0) {
            if (pthread_create(&aboxlog_dump.tid, NULL, save_audio_dump, (void *)&aboxlog_dump) != 0)
                ALOGE("%s: fail to create the A-Box Log dump copy thread (%s)", __func__, strerror(errno));
        }

#ifdef SUPPORT_VTS_LOGGING
        for (i = 0; i < VTS_PCM_DUMP_CNT; i++) {
            remove_old_dump(vts_pcm_dump[i].dst_name);
            if (open_dump_fd(&vts_pcm_dump[i]) == 0) {
                if (pthread_create(&vts_pcm_dump[i].tid, NULL, save_audio_dump, (void *)&vts_pcm_dump[i]) != 0)
                    ALOGE("%s: fail to create the vts_pcm_dump[%d] copy thread  (%s)", __func__, i, strerror(errno));
            }
        }

        remove_old_dump(vtslog_dump.dst_name);
        if (open_dump_fd(&vtslog_dump) == 0) {
            if (pthread_create(&vtslog_dump.tid, NULL, save_audio_dump, (void *)&vtslog_dump) != 0)
                ALOGE("%s: fail to create the A-Box Log dump copy thread (%s)", __func__, strerror(errno));
        }
#endif
        break;
    case GENERAL_DBG:
        for (i = 0; i < PCM_DUMP_CNT; i++) {
            remove_old_dump(pcm_dump[i].dst_name);
            if (open_dump_fd(&pcm_dump[i]) == 0) {
                ALOGE("%s: failed opem pcm_dump[%d] nodes (%s)", __func__, i, strerror(errno));
            }
        }
        break;
    case SILENT_DBG:
        remove_old_dump(silent_dump.dst_name);
        if (open_dump_fd(&silent_dump) == 0) {
            if (pthread_create(&silent_dump.tid, NULL, save_audio_dump, (void *)&silent_dump) != 0)
                ALOGE("%s: fail to create the Silent dump copy thread (%s)", __func__, strerror(errno));
        }

        remove_old_dump(aboxlog_dump.dst_name);
        if (open_dump_fd(&aboxlog_dump) == 0) {
            if (pthread_create(&aboxlog_dump.tid, NULL, save_audio_dump, (void *)&aboxlog_dump) != 0)
                ALOGE("%s: fail to create the A-Box Log dump copy thread (%s)", __func__, strerror(errno));
        }
        break;
    default:
        ALOGE("%s: error: mode(%d) is not defined!", __func__, mode);
        break;
    }

    return;
}

void stop_audio_dump(uint32_t mode)
{
    int i;

    ALOGI("%s: enter", __func__);

    set_dump_status_tohal(mode, LOGGING_STOP);

    switch (mode) {
    case AUDIO_DBG:
        for (i = 0; i < PCM_DUMP_CNT; i++) {
            close_dump_fd(&pcm_dump[i]);
            pthread_kill(pcm_dump[i].tid, SIGINT);
            pthread_join(pcm_dump[i].tid, NULL);
        }
        ALOGI("%s: finish all A-Box pcm dump threads", __func__);

        close_dump_fd(&aboxlog_dump);
        pthread_kill(aboxlog_dump.tid, SIGINT);
        pthread_join(aboxlog_dump.tid, NULL);
        ALOGI("%s: finish A-Box log dump thread", __func__);

#ifdef SUPPORT_VTS_LOGGING
        for (i = 0; i < VTS_PCM_DUMP_CNT; i++) {
            close_dump_fd(&vts_pcm_dump[i]);
            pthread_kill(vts_pcm_dump[i].tid, SIGINT);
            pthread_join(vts_pcm_dump[i].tid, NULL);
        }
        ALOGI("%s: finish all VTS pcm dump threads", __func__);

        close_dump_fd(&vtslog_dump);
        pthread_kill(vtslog_dump.tid, SIGINT);
        pthread_join(vtslog_dump.tid, NULL);
        ALOGI("%s: finish VTS log dump thread", __func__);
#endif
        break;
    case GENERAL_DBG:
        for (i = 0; i < PCM_DUMP_CNT; i++) {
            close_dump_fd(&pcm_dump[i]);
        }
        /* capture circular dump once stopped */
        if (open_dump_fd(&circular_dump) == 0) {
            save_circular_dump(&circular_dump);
            ALOGI("%s: finish General circular dump", __func__);
            close_dump_fd(&circular_dump);
        } else {
            ALOGI("%s: failed to capture General circular dump", __func__);
        }
        break;
    case SILENT_DBG:
        close_dump_fd(&silent_dump);
        pthread_kill(silent_dump.tid, SIGINT);
        pthread_join(silent_dump.tid, NULL);
        ALOGI("%s: finish silent dump thread", __func__);

        close_dump_fd(&aboxlog_dump);
        pthread_kill(aboxlog_dump.tid, SIGINT);
        pthread_join(aboxlog_dump.tid, NULL);
        ALOGI("%s: finish A-Box log dump thread", __func__);
        break;
    default:
        ALOGE("%s: error: mode(%d) is not defined!", __func__, mode);
        break;
    }

    return;
}

// Methods from ::vendor::samsung_slsi::hardware::audio_dump::V1_0::IAudioDump follow.
Return<void> AudioDump::startDump(uint32_t status)
{
    ALOGI("%s: enter with %s(%u)", __func__,
            (status < AUDIO_DUMP_COUNT ? dump_mode_name[status] : "INVALID"), status);

    if (pcm_dump_stat == AUDIO_DUMP_START) {
        ALOGE("%s: ignore AUDIO_DUMP_START msg, already AudioDump start!", __func__);
        return Void();
    }

    pcm_dump_stat = AUDIO_DUMP_START;

    switch (status) {
    case AUDIO_DBG:
    case SILENT_DBG:
    case GENERAL_DBG:
        start_audio_dump(status);
        break;
    default:
        ALOGE("%s: error: mode(%d) is not defined!", __func__, status);
        goto exit;
    }

    en_logging = status;
    ALOGI("%s: mode(%d) is enabled", __func__, status);

exit:
    return Void();
}

Return<void> AudioDump::stopDump()
{
    ALOGI("%s: enter", __func__);

    if (pcm_dump_stat == AUDIO_DUMP_STOP) {
        ALOGE("%s: ignore AUDIO_DUMP_STOP msg, already AudioDump stop!", __func__);
        return Void();
    }

    pcm_dump_stat = AUDIO_DUMP_STOP;

    switch (en_logging) {
    case AUDIO_DBG:
    case SILENT_DBG:
    case GENERAL_DBG:
        stop_audio_dump(en_logging);
        break;
    default:
        ALOGE("%s: error: there is no enabled logging", __func__);
        break;
    }

    en_logging = NONE_DBG;

    return Void();
}

AudioDump::AudioDump()
{
    int i;

    ALOGI("%s: enter: version is %s", __func__, DUMP_VERSION);

    // initialize pcm dump structure aray
    for (i = 0; i < PCM_DUMP_CNT; i++) {
        pcm_dump[i].src_name = supported_dump[i];
        pcm_dump[i].dst_name = supported_dump[i];
        pcm_dump[i].src_path = ABOX_DUMP_PATH;
        pcm_dump[i].dst_path = OUT_TARGET_PATH;
        pcm_dump[i].dstout_type = OUT_PCM_DUMP_EXTENSION;
        pcm_dump[i].num = i;
    }

    // initialize A-Box log dump structure
    aboxlog_dump.src_name = ABOX_LOG_DUMP_NAME;
    aboxlog_dump.dst_name = ABOX_LOG_DUMP_NAME;
    aboxlog_dump.src_path = ABOX_LOG_PATH;
    aboxlog_dump.dst_path = OUT_TARGET_PATH;
    aboxlog_dump.dstout_type = "";

#ifdef SUPPORT_VTS_LOGGING
    // initialize VTS pcm dump structure aray
    for (i = 0; i < VTS_PCM_DUMP_CNT; i++) {
        vts_pcm_dump[i].src_name = supported_vts_dump[i];
        vts_pcm_dump[i].dst_name = supported_vts_dump[i];
        vts_pcm_dump[i].src_path = VTS_DUMP_PATH;
        vts_pcm_dump[i].dst_path = OUT_TARGET_PATH;
        vts_pcm_dump[i].dstout_type = OUT_PCM_DUMP_EXTENSION;
        vts_pcm_dump[i].num = i;
    }

    // initialize VTS log dump structure
    vtslog_dump.src_name = VTS_LOG_DUMP_NAME;
    vtslog_dump.dst_name = VTS_LOG_DUMP_NAME;
    vtslog_dump.src_path = VTS_LOG_PATH;
    vtslog_dump.dst_path = OUT_TARGET_PATH;
    vtslog_dump.dstout_type = "";
#endif

    // initialize circular dump structure
    circular_dump.src_name = CIRCULAR_HX_DUMP_NAME;
    circular_dump.dst_name = OUT_CIRCULAR_HX_DUMP_NAME;
    circular_dump.src_path = ABOX_LOG_PATH;
    circular_dump.dst_path = OUT_TARGET_PATH;
    circular_dump.dstout_type = "";

    // initialize silent dump structure
    silent_dump.src_name = SILENT_LOG_DUMP_NAME;
    silent_dump.dst_name = OUT_SILENT_LOG_DUMP_NAME;
    silent_dump.src_path = ABOX_DUMP_PATH;
    silent_dump.dst_path = OUT_TARGET_PATH;
    silent_dump.dstout_type = OUT_SILENT_LOG_EXTENSION;
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

//IAudioDump* HIDL_FETCH_IAudioDump(const char* /* name */) {
    //return new AudioDump();
//}
//
}  // namespace implementation
}  // namespace V1_0
}  // namespace audio_dump
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor
