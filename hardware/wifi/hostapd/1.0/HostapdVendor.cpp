// FIXME: your file license if you have one

#include "HostapdVendor.h"

namespace vendor::samsung_slsi::hardware::wifi::hostapd::implementation {

// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendor follow.
Return<void> HostapdVendor::registerVendorCallback(const hidl_string& ifaceName, const sp<::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback>& callback, registerVendorCallback_cb _hidl_cb) {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//IHostapdVendor* HIDL_FETCH_IHostapdVendor(const char* /* name */) {
    //return new HostapdVendor();
//}
//
}  // namespace vendor::samsung_slsi::hardware::wifi::hostapd::implementation
