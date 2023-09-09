// FIXME: your file license if you have one

#include "SupplicantVendor.h"

namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation {

// Methods from ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendor follow.
Return<void> SupplicantVendor::getVendorInterface(const ::android::hardware::wifi::supplicant::V1_0::ISupplicant::IfaceInfo& ifaceInfo, getVendorInterface_cb _hidl_cb) {
    // TODO implement
    return Void();
}

Return<void> SupplicantVendor::listVendorInterfaces(listVendorInterfaces_cb _hidl_cb) {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//ISupplicantVendor* HIDL_FETCH_ISupplicantVendor(const char* /* name */) {
    //return new SupplicantVendor();
//}
//
}  // namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation
