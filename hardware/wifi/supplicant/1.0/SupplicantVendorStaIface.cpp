// FIXME: your file license if you have one

#include "SupplicantVendorStaIface.h"

namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation {

// Methods from ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorStaIface follow.
Return<void> SupplicantVendorStaIface::registerVendorCallback(const sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorStaIfaceCallback>& callback, registerVendorCallback_cb _hidl_cb) {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//ISupplicantVendorStaIface* HIDL_FETCH_ISupplicantVendorStaIface(const char* /* name */) {
    //return new SupplicantVendorStaIface();
//}
//
}  // namespace vendor::samsung_slsi::hardware::wifi::supplicant::implementation
