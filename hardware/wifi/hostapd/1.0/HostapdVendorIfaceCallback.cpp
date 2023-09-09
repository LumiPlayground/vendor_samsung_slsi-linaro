// FIXME: your file license if you have one

#include "HostapdVendorIfaceCallback.h"

namespace vendor::samsung_slsi::hardware::wifi::hostapd::implementation {

// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
Return<void> HostapdVendorIfaceCallback::onVendorDriverHang(const hidl_string& state, const hidl_string& msg) {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//IHostapdVendorIfaceCallback* HIDL_FETCH_IHostapdVendorIfaceCallback(const char* /* name */) {
    //return new HostapdVendorIfaceCallback();
//}
//
}  // namespace vendor::samsung_slsi::hardware::wifi::hostapd::implementation
