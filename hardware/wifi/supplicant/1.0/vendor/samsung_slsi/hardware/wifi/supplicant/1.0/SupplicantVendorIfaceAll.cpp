#define LOG_TAG "vendor.samsung_slsi.hardware.wifi.supplicant@1.0::SupplicantVendorIface"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hidl/manager/1.0/IServiceManager.h>
#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/BpHwSupplicantVendorIface.h>
#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/BnHwSupplicantVendorIface.h>
#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/BsSupplicantVendorIface.h>
#include <android/hidl/base/1.0/BpHwBase.h>
#include <hidl/ServiceManagement.h>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace wifi {
namespace supplicant {
namespace V1_0 {

const char* ISupplicantVendorIface::descriptor("vendor.samsung_slsi.hardware.wifi.supplicant@1.0::ISupplicantVendorIface");

__attribute__((constructor)) static void static_constructor() {
    ::android::hardware::details::getBnConstructorMap().set(ISupplicantVendorIface::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hardware::IBinder> {
                return new BnHwSupplicantVendorIface(static_cast<ISupplicantVendorIface *>(iIntf));
            });
    ::android::hardware::details::getBsConstructorMap().set(ISupplicantVendorIface::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hidl::base::V1_0::IBase> {
                return new BsSupplicantVendorIface(static_cast<ISupplicantVendorIface *>(iIntf));
            });
}

__attribute__((destructor))static void static_destructor() {
    ::android::hardware::details::getBnConstructorMap().erase(ISupplicantVendorIface::descriptor);
    ::android::hardware::details::getBsConstructorMap().erase(ISupplicantVendorIface::descriptor);
}

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> ISupplicantVendorIface::interfaceChain(interfaceChain_cb _hidl_cb){
    _hidl_cb({
        ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorIface::descriptor,
        ::android::hidl::base::V1_0::IBase::descriptor,
    });
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorIface::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorIface::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorIface::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorIface::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* 0000000000000000000000000000000000000000000000000000000000000000 */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorIface::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> ISupplicantVendorIface::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> ISupplicantVendorIface::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorIface::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = -1;
    info.ptr = 0;
    info.arch = 
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorIface::notifySyspropsChanged(){
    ::android::report_sysprop_change();
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> ISupplicantVendorIface::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorIface>> ISupplicantVendorIface::castFrom(const ::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorIface>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorIface>> ISupplicantVendorIface::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::hardware::details::castInterface<ISupplicantVendorIface, ::android::hidl::base::V1_0::IBase, BpHwSupplicantVendorIface>(
            parent, "vendor.samsung_slsi.hardware.wifi.supplicant@1.0::ISupplicantVendorIface", emitError);
}

BpHwSupplicantVendorIface::BpHwSupplicantVendorIface(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)
        : BpInterface<ISupplicantVendorIface>(_hidl_impl),
          ::android::hardware::details::HidlInstrumentor("vendor.samsung_slsi.hardware.wifi.supplicant@1.0", "ISupplicantVendorIface") {
}

void BpHwSupplicantVendorIface::onLastStrongRef(const void* id) {
    {
        std::unique_lock<std::mutex> lock(_hidl_mMutex);
        _hidl_mDeathRecipients.clear();
    }

    BpInterface<ISupplicantVendorIface>::onLastStrongRef(id);
}

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BpHwSupplicantVendorIface::interfaceChain(interfaceChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_debug(this, this, fd, options);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceDescriptor(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::getHashChain(getHashChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getHashChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::setHALInstrumentation(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_setHALInstrumentation(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwSupplicantVendorIface::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    ::android::hardware::ProcessState::self()->startThreadPool();
    ::android::hardware::hidl_binder_death_recipient *binder_recipient = new ::android::hardware::hidl_binder_death_recipient(recipient, cookie, this);
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    _hidl_mDeathRecipients.push_back(binder_recipient);
    return (remote()->linkToDeath(binder_recipient) == ::android::OK);
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::ping(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_ping(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getDebugInfo(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorIface::notifySyspropsChanged(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_notifySyspropsChanged(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwSupplicantVendorIface::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    for (auto it = _hidl_mDeathRecipients.rbegin();it != _hidl_mDeathRecipients.rend();++it) {
        if ((*it)->getRecipient() == recipient) {
            ::android::status_t status = remote()->unlinkToDeath(*it);
            _hidl_mDeathRecipients.erase(it.base()-1);
            return status == ::android::OK;
        }
    }
    return false;
}


BnHwSupplicantVendorIface::BnHwSupplicantVendorIface(const ::android::sp<ISupplicantVendorIface> &_hidl_impl)
        : ::android::hidl::base::V1_0::BnHwBase(_hidl_impl, "vendor.samsung_slsi.hardware.wifi.supplicant@1.0", "ISupplicantVendorIface") { 
            _hidl_mImpl = _hidl_impl;
            auto prio = ::android::hardware::getMinSchedulerPolicy(_hidl_impl);
            mSchedPolicy = prio.sched_policy;
            mSchedPriority = prio.prio;
            setRequestingSid(::android::hardware::getRequestingSid(_hidl_impl));
}

BnHwSupplicantVendorIface::~BnHwSupplicantVendorIface() {
    ::android::hardware::details::gBnMap->eraseIfEqual(_hidl_mImpl.get(), this);
}


// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BnHwSupplicantVendorIface::ping() {
    return ::android::hardware::Void();
}
::android::hardware::Return<void> BnHwSupplicantVendorIface::getDebugInfo(getDebugInfo_cb _hidl_cb) {
    ::android::hidl::base::V1_0::DebugInfo info = {};
    info.pid = ::android::hardware::details::getPidIfSharable();
    info.ptr = ::android::hardware::details::debuggable()? reinterpret_cast<uint64_t>(this) : 0;
    info.arch = 
    #if defined(__LP64__)
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_64BIT
    #else
    ::android::hidl::base::V1_0::DebugInfo::Architecture::IS_32BIT
    #endif
    ;
    _hidl_cb(info);
    return ::android::hardware::Void();
}

::android::status_t BnHwSupplicantVendorIface::onTransact(
        uint32_t _hidl_code,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        uint32_t _hidl_flags,
        TransactCallback _hidl_cb) {
    ::android::status_t _hidl_err = ::android::OK;

    switch (_hidl_code) {
        default:
        {
            return ::android::hidl::base::V1_0::BnHwBase::onTransact(
                    _hidl_code, _hidl_data, _hidl_reply, _hidl_flags, _hidl_cb);
        }
    }

    if (_hidl_err == ::android::UNEXPECTED_NULL) {
        _hidl_err = ::android::hardware::writeToParcel(
                ::android::hardware::Status::fromExceptionCode(::android::hardware::Status::EX_NULL_POINTER),
                _hidl_reply);
    }return _hidl_err;
}

BsSupplicantVendorIface::BsSupplicantVendorIface(const ::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorIface> impl) : ::android::hardware::details::HidlInstrumentor("vendor.samsung_slsi.hardware.wifi.supplicant@1.0", "ISupplicantVendorIface"), mImpl(impl) {
    mOnewayQueue.start(3000 /* similar limit to binderized */);
}

::android::hardware::Return<void> BsSupplicantVendorIface::addOnewayTask(std::function<void(void)> fun) {
    if (!mOnewayQueue.push(fun)) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_TRANSACTION_FAILED,
                "Passthrough oneway function queue exceeds maximum size.");
    }
    return ::android::hardware::Status();
}

::android::sp<ISupplicantVendorIface> ISupplicantVendorIface::tryGetService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwSupplicantVendorIface>(serviceName, false, getStub);
}

::android::sp<ISupplicantVendorIface> ISupplicantVendorIface::getService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwSupplicantVendorIface>(serviceName, true, getStub);
}

::android::status_t ISupplicantVendorIface::registerAsService(const std::string &serviceName) {
    return ::android::hardware::details::registerAsServiceInternal(this, serviceName);
}

bool ISupplicantVendorIface::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm
            = ::android::hardware::defaultServiceManager();
    if (sm == nullptr) {
        return false;
    }
    ::android::hardware::Return<bool> success =
            sm->registerForNotifications("vendor.samsung_slsi.hardware.wifi.supplicant@1.0::ISupplicantVendorIface",
                    serviceName, notification);
    return success.isOk() && success;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V1_0
}  // namespace supplicant
}  // namespace wifi
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor
