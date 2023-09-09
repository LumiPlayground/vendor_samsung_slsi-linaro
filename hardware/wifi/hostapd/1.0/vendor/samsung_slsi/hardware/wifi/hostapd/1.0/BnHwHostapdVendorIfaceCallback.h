#ifndef HIDL_GENERATED_VENDOR_SAMSUNG_SLSI_HARDWARE_WIFI_HOSTAPD_V1_0_BNHWHOSTAPDVENDORIFACECALLBACK_H
#define HIDL_GENERATED_VENDOR_SAMSUNG_SLSI_HARDWARE_WIFI_HOSTAPD_V1_0_BNHWHOSTAPDVENDORIFACECALLBACK_H

#include <vendor/samsung_slsi/hardware/wifi/hostapd/1.0/IHwHostapdVendorIfaceCallback.h>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace wifi {
namespace hostapd {
namespace V1_0 {

struct BnHwHostapdVendorIfaceCallback : public ::android::hidl::base::V1_0::BnHwBase {
    explicit BnHwHostapdVendorIfaceCallback(const ::android::sp<IHostapdVendorIfaceCallback> &_hidl_impl);
    explicit BnHwHostapdVendorIfaceCallback(const ::android::sp<IHostapdVendorIfaceCallback> &_hidl_impl, const std::string& HidlInstrumentor_package, const std::string& HidlInstrumentor_interface);

    virtual ~BnHwHostapdVendorIfaceCallback();

    ::android::status_t onTransact(
            uint32_t _hidl_code,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            uint32_t _hidl_flags = 0,
            TransactCallback _hidl_cb = nullptr) override;


    /**
     * The pure class is what this class wraps.
     */
    typedef IHostapdVendorIfaceCallback Pure;

    /**
     * Type tag for use in template logic that indicates this is a 'native' class.
     */
    typedef ::android::hardware::details::bnhw_tag _hidl_tag;

    ::android::sp<IHostapdVendorIfaceCallback> getImpl() { return _hidl_mImpl; }
    // Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
    static ::android::status_t _hidl_onVendorDriverHang(
            ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            TransactCallback _hidl_cb);



private:
    // Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    ::android::hardware::Return<void> ping();
    using getDebugInfo_cb = ::android::hidl::base::V1_0::IBase::getDebugInfo_cb;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb);

    ::android::sp<IHostapdVendorIfaceCallback> _hidl_mImpl;
};

}  // namespace V1_0
}  // namespace hostapd
}  // namespace wifi
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor

#endif  // HIDL_GENERATED_VENDOR_SAMSUNG_SLSI_HARDWARE_WIFI_HOSTAPD_V1_0_BNHWHOSTAPDVENDORIFACECALLBACK_H
