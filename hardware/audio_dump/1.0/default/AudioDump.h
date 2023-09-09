#ifndef VENDOR_SAMSUNG_SLSI_HARDWARE_AUDIODUMP_V1_0_AUDIODUMP_H
#define VENDOR_SAMSUNG_SLSI_HARDWARE_AUDIODUMP_V1_0_AUDIODUMP_H

#include <vendor/samsung_slsi/hardware/audio_dump/1.0/IAudioDump.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace audio_dump {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct AudioDump : public IAudioDump {
public:
    AudioDump();
    Return<void> startDump(uint32_t status) override;
    Return<void> stopDump() override;
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IAudioDump* HIDL_FETCH_IAudioDump(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace audio_dump
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor

#endif  // VENDOR_SAMSUNG_SLSI_HARDWARE_AUDIODUMP_V1_0_AUDIODUMP_H
