// FIXME: your file license if you have one

#include "SupplicantVendorStaIfaceCallback.h"

namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation {

// Methods from ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorStaIfaceCallback follow.
Return<void> SupplicantVendorStaIfaceCallback::onVendorDriverHang(const hidl_string& state, const hidl_string& msg) {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//ISupplicantVendorStaIfaceCallback* HIDL_FETCH_ISupplicantVendorStaIfaceCallback(const char* /* name */) {
    //return new SupplicantVendorStaIfaceCallback();
//}
//
}  // namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation
