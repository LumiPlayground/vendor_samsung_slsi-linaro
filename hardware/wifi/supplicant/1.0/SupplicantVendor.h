// FIXME: your file license if you have one

#pragma once

#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/ISupplicantVendor.h>
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

struct SupplicantVendor : public V1_0::ISupplicantVendor {
    // Methods from ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendor follow.
    Return<void> getVendorInterface(const ::android::hardware::wifi::supplicant::V1_0::ISupplicant::IfaceInfo& ifaceInfo, getVendorInterface_cb _hidl_cb) override;
    Return<void> listVendorInterfaces(listVendorInterfaces_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" ISupplicantVendor* HIDL_FETCH_ISupplicantVendor(const char* name);

}  // namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation
