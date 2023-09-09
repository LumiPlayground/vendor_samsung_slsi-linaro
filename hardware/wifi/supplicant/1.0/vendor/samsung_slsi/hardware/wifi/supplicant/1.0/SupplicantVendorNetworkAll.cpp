#define LOG_TAG "vendor.samsung_slsi.hardware.wifi.supplicant@1.0::SupplicantVendorNetwork"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hidl/manager/1.0/IServiceManager.h>
#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/BpHwSupplicantVendorNetwork.h>
#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/BnHwSupplicantVendorNetwork.h>
#include <vendor/samsung_slsi/hardware/wifi/supplicant/1.0/BsSupplicantVendorNetwork.h>
#include <android/hidl/base/1.0/BpHwBase.h>
#include <hidl/ServiceManagement.h>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace wifi {
namespace supplicant {
namespace V1_0 {

const char* ISupplicantVendorNetwork::descriptor("vendor.samsung_slsi.hardware.wifi.supplicant@1.0::ISupplicantVendorNetwork");

__attribute__((constructor)) static void static_constructor() {
    ::android::hardware::details::getBnConstructorMap().set(ISupplicantVendorNetwork::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hardware::IBinder> {
                return new BnHwSupplicantVendorNetwork(static_cast<ISupplicantVendorNetwork *>(iIntf));
            });
    ::android::hardware::details::getBsConstructorMap().set(ISupplicantVendorNetwork::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hidl::base::V1_0::IBase> {
                return new BsSupplicantVendorNetwork(static_cast<ISupplicantVendorNetwork *>(iIntf));
            });
}

__attribute__((destructor))static void static_destructor() {
    ::android::hardware::details::getBnConstructorMap().erase(ISupplicantVendorNetwork::descriptor);
    ::android::hardware::details::getBsConstructorMap().erase(ISupplicantVendorNetwork::descriptor);
}

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> ISupplicantVendorNetwork::interfaceChain(interfaceChain_cb _hidl_cb){
    _hidl_cb({
        ::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorNetwork::descriptor,
        ::android::hidl::base::V1_0::IBase::descriptor,
    });
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorNetwork::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorNetwork::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorNetwork::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorNetwork::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* 0000000000000000000000000000000000000000000000000000000000000000 */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorNetwork::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> ISupplicantVendorNetwork::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> ISupplicantVendorNetwork::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> ISupplicantVendorNetwork::getDebugInfo(getDebugInfo_cb _hidl_cb){
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

::android::hardware::Return<void> ISupplicantVendorNetwork::notifySyspropsChanged(){
    ::android::report_sysprop_change();
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> ISupplicantVendorNetwork::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorNetwork>> ISupplicantVendorNetwork::castFrom(const ::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorNetwork>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorNetwork>> ISupplicantVendorNetwork::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::hardware::details::castInterface<ISupplicantVendorNetwork, ::android::hidl::base::V1_0::IBase, BpHwSupplicantVendorNetwork>(
            parent, "vendor.samsung_slsi.hardware.wifi.supplicant@1.0::ISupplicantVendorNetwork", emitError);
}

BpHwSupplicantVendorNetwork::BpHwSupplicantVendorNetwork(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)
        : BpInterface<ISupplicantVendorNetwork>(_hidl_impl),
          ::android::hardware::details::HidlInstrumentor("vendor.samsung_slsi.hardware.wifi.supplicant@1.0", "ISupplicantVendorNetwork") {
}

void BpHwSupplicantVendorNetwork::onLastStrongRef(const void* id) {
    {
        std::unique_lock<std::mutex> lock(_hidl_mMutex);
        _hidl_mDeathRecipients.clear();
    }

    BpInterface<ISupplicantVendorNetwork>::onLastStrongRef(id);
}

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BpHwSupplicantVendorNetwork::interfaceChain(interfaceChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_debug(this, this, fd, options);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceDescriptor(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::getHashChain(getHashChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getHashChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::setHALInstrumentation(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_setHALInstrumentation(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwSupplicantVendorNetwork::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    ::android::hardware::ProcessState::self()->startThreadPool();
    ::android::hardware::hidl_binder_death_recipient *binder_recipient = new ::android::hardware::hidl_binder_death_recipient(recipient, cookie, this);
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    _hidl_mDeathRecipients.push_back(binder_recipient);
    return (remote()->linkToDeath(binder_recipient) == ::android::OK);
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::ping(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_ping(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getDebugInfo(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwSupplicantVendorNetwork::notifySyspropsChanged(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_notifySyspropsChanged(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwSupplicantVendorNetwork::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
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


BnHwSupplicantVendorNetwork::BnHwSupplicantVendorNetwork(const ::android::sp<ISupplicantVendorNetwork> &_hidl_impl)
        : ::android::hidl::base::V1_0::BnHwBase(_hidl_impl, "vendor.samsung_slsi.hardware.wifi.supplicant@1.0", "ISupplicantVendorNetwork") { 
            _hidl_mImpl = _hidl_impl;
            auto prio = ::android::hardware::getMinSchedulerPolicy(_hidl_impl);
            mSchedPolicy = prio.sched_policy;
            mSchedPriority = prio.prio;
            setRequestingSid(::android::hardware::getRequestingSid(_hidl_impl));
}

BnHwSupplicantVendorNetwork::~BnHwSupplicantVendorNetwork() {
    ::android::hardware::details::gBnMap->eraseIfEqual(_hidl_mImpl.get(), this);
}


// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BnHwSupplicantVendorNetwork::ping() {
    return ::android::hardware::Void();
}
::android::hardware::Return<void> BnHwSupplicantVendorNetwork::getDebugInfo(getDebugInfo_cb _hidl_cb) {
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

::android::status_t BnHwSupplicantVendorNetwork::onTransact(
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

BsSupplicantVendorNetwork::BsSupplicantVendorNetwork(const ::android::sp<::vendor::samsung_slsi::hardware::wifi::supplicant::V1_0::ISupplicantVendorNetwork> impl) : ::android::hardware::details::HidlInstrumentor("vendor.samsung_slsi.hardware.wifi.supplicant@1.0", "ISupplicantVendorNetwork"), mImpl(impl) {
    mOnewayQueue.start(3000 /* similar limit to binderized */);
}

::android::hardware::Return<void> BsSupplicantVendorNetwork::addOnewayTask(std::function<void(void)> fun) {
    if (!mOnewayQueue.push(fun)) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_TRANSACTION_FAILED,
                "Passthrough oneway function queue exceeds maximum size.");
    }
    return ::android::hardware::Status();
}

::android::sp<ISupplicantVendorNetwork> ISupplicantVendorNetwork::tryGetService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwSupplicantVendorNetwork>(serviceName, false, getStub);
}

::android::sp<ISupplicantVendorNetwork> ISupplicantVendorNetwork::getService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwSupplicantVendorNetwork>(serviceName, true, getStub);
}

::android::status_t ISupplicantVendorNetwork::registerAsService(const std::string &serviceName) {
    return ::android::hardware::details::registerAsServiceInternal(this, serviceName);
}

bool ISupplicantVendorNetwork::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm
            = ::android::hardware::defaultServiceManager();
    if (sm == nullptr) {
        return false;
    }
    ::android::hardware::Return<bool> success =
            sm->registerForNotifications("vendor.samsung_slsi.hardware.wifi.supplicant@1.0::ISupplicantVendorNetwork",
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
