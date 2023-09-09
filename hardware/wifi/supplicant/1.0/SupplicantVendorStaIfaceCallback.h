// FIXME: your file license if you have one

#pragma once

#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/ISupplicantVendorStaIfaceCallback.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct SupplicantVendorStaIfaceCallback : public V1_0::ISupplicantVendorStaIfaceCallback {
    // Methods from ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorStaIfaceCallback follow.
    Return<void> onVendorDriverHang(const hidl_string& state, const hidl_string& msg) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" ISupplicantVendorStaIfaceCallback* HIDL_FETCH_ISupplicantVendorStaIfaceCallback(const char* name);

}  // namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation
