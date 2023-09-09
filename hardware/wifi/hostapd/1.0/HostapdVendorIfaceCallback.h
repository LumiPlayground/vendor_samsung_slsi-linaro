// FIXME: your file license if you have one

#pragma once

#include <vendor/samsung_slsi/hardware/wifi/hostapd/1.0/IHostapdVendorIfaceCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor::samsung_slsi::hardware::wifi::hostapd::implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct HostapdVendorIfaceCallback : public V1_0::IHostapdVendorIfaceCallback {
    // Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
    Return<void> onVendorDriverHang(const hidl_string& state, const hidl_string& msg) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IHostapdVendorIfaceCallback* HIDL_FETCH_IHostapdVendorIfaceCallback(const char* name);

}  // namespace vendor::samsung_slsi::hardware::wifi::hostapd::implementation
