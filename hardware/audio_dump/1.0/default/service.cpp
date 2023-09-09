#define LOG_TAG "vendor.samsung_slsi.hardware.audio_dump@1.0-service"

#include <utils/Log.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/samsung_slsi/hardware/audio_dump/1.0/IAudioDump.h>
#include "AudioDump.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using vendor::samsung_slsi::hardware::audio_dump::V1_0::IAudioDump;
using vendor::samsung_slsi::hardware::audio_dump::V1_0::implementation::AudioDump;
using android::sp;
using android::status_t;
using android::OK;

int main() {
    configureRpcThreadpool(1, true);

    sp<IAudioDump> audioDump = new AudioDump();
    status_t status = audioDump->registerAsService();
    if (status == OK) {
        ALOGI("Default service is ready.");
    } else {
        ALOGE("Could not register IAudioDump v1.0.");
        return 1;
    }
    joinRpcThreadpool();

    return 1;
}
