// FIXME: your file license if you have one

#pragma once

#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/ISupplicantVendorStaIface.h>
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

struct SupplicantVendorStaIface : public V1_0::ISupplicantVendorStaIface {
    // Methods from ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorStaIface follow.
    Return<void> registerVendorCallback(const sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorStaIfaceCallback>& callback, registerVendorCallback_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" ISupplicantVendorStaIface* HIDL_FETCH_ISupplicantVendorStaIface(const char* name);

}  // namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation
