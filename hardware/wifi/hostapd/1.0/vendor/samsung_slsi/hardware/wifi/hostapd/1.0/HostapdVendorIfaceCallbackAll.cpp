#define LOG_TAG "vendor.samsung_slsi.hardware.wifi.hostapd@1.0::HostapdVendorIfaceCallback"

#include <log/log.h>
#include <cutils/trace.h>
#include <hidl/HidlTransportSupport.h>

#include <hidl/Static.h>
#include <hwbinder/ProcessState.h>
#include <utils/Trace.h>
#include <android/hidl/manager/1.0/IServiceManager.h>
#include <vendor/samsung_slsi/hardware/wifi/hostapd/1.0/BpHwHostapdVendorIfaceCallback.h>
#include <vendor/samsung_slsi/hardware/wifi/hostapd/1.0/BnHwHostapdVendorIfaceCallback.h>
#include <vendor/samsung_slsi/hardware/wifi/hostapd/1.0/BsHostapdVendorIfaceCallback.h>
#include <android/hidl/base/1.0/BpHwBase.h>
#include <hidl/ServiceManagement.h>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace wifi {
namespace hostapd {
namespace V1_0 {

const char* IHostapdVendorIfaceCallback::descriptor("vendor.samsung_slsi.hardware.wifi.hostapd@1.0::IHostapdVendorIfaceCallback");

__attribute__((constructor)) static void static_constructor() {
    ::android::hardware::details::getBnConstructorMap().set(IHostapdVendorIfaceCallback::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hardware::IBinder> {
                return new BnHwHostapdVendorIfaceCallback(static_cast<IHostapdVendorIfaceCallback *>(iIntf));
            });
    ::android::hardware::details::getBsConstructorMap().set(IHostapdVendorIfaceCallback::descriptor,
            [](void *iIntf) -> ::android::sp<::android::hidl::base::V1_0::IBase> {
                return new BsHostapdVendorIfaceCallback(static_cast<IHostapdVendorIfaceCallback *>(iIntf));
            });
}

__attribute__((destructor))static void static_destructor() {
    ::android::hardware::details::getBnConstructorMap().erase(IHostapdVendorIfaceCallback::descriptor);
    ::android::hardware::details::getBsConstructorMap().erase(IHostapdVendorIfaceCallback::descriptor);
}

// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
// no default implementation for: ::android::hardware::Return<void> IHostapdVendorIfaceCallback::onVendorDriverHang(const ::android::hardware::hidl_string& state, const ::android::hardware::hidl_string& msg)

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> IHostapdVendorIfaceCallback::interfaceChain(interfaceChain_cb _hidl_cb){
    _hidl_cb({
        ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback::descriptor,
        ::android::hidl::base::V1_0::IBase::descriptor,
    });
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IHostapdVendorIfaceCallback::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    (void)fd;
    (void)options;
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IHostapdVendorIfaceCallback::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    _hidl_cb(::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback::descriptor);
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IHostapdVendorIfaceCallback::getHashChain(getHashChain_cb _hidl_cb){
    _hidl_cb({
        (uint8_t[32]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* 0000000000000000000000000000000000000000000000000000000000000000 */,
        (uint8_t[32]){236,127,215,158,208,45,250,133,188,73,148,38,173,174,62,190,35,239,5,36,243,205,105,87,19,147,36,184,59,24,202,76} /* ec7fd79ed02dfa85bc499426adae3ebe23ef0524f3cd6957139324b83b18ca4c */});
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IHostapdVendorIfaceCallback::setHALInstrumentation(){
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IHostapdVendorIfaceCallback::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    (void)cookie;
    return (recipient != nullptr);
}

::android::hardware::Return<void> IHostapdVendorIfaceCallback::ping(){
    return ::android::hardware::Void();
}

::android::hardware::Return<void> IHostapdVendorIfaceCallback::getDebugInfo(getDebugInfo_cb _hidl_cb){
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

::android::hardware::Return<void> IHostapdVendorIfaceCallback::notifySyspropsChanged(){
    ::android::report_sysprop_change();
    return ::android::hardware::Void();
}

::android::hardware::Return<bool> IHostapdVendorIfaceCallback::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
    return (recipient != nullptr);
}


::android::hardware::Return<::android::sp<::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback>> IHostapdVendorIfaceCallback::castFrom(const ::android::sp<::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback>& parent, bool /* emitError */) {
    return parent;
}

::android::hardware::Return<::android::sp<::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback>> IHostapdVendorIfaceCallback::castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError) {
    return ::android::hardware::details::castInterface<IHostapdVendorIfaceCallback, ::android::hidl::base::V1_0::IBase, BpHwHostapdVendorIfaceCallback>(
            parent, "vendor.samsung_slsi.hardware.wifi.hostapd@1.0::IHostapdVendorIfaceCallback", emitError);
}

BpHwHostapdVendorIfaceCallback::BpHwHostapdVendorIfaceCallback(const ::android::sp<::android::hardware::IBinder> &_hidl_impl)
        : BpInterface<IHostapdVendorIfaceCallback>(_hidl_impl),
          ::android::hardware::details::HidlInstrumentor("vendor.samsung_slsi.hardware.wifi.hostapd@1.0", "IHostapdVendorIfaceCallback") {
}

void BpHwHostapdVendorIfaceCallback::onLastStrongRef(const void* id) {
    {
        std::unique_lock<std::mutex> lock(_hidl_mMutex);
        _hidl_mDeathRecipients.clear();
    }

    BpInterface<IHostapdVendorIfaceCallback>::onLastStrongRef(id);
}
// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::_hidl_onVendorDriverHang(::android::hardware::IInterface *_hidl_this, ::android::hardware::details::HidlInstrumentor *_hidl_this_instrumentor, const ::android::hardware::hidl_string& state, const ::android::hardware::hidl_string& msg) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this_instrumentor->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this_instrumentor->getInstrumentationCallbacks();
    #else
    (void) _hidl_this_instrumentor;
    #endif // __ANDROID_DEBUGGABLE__
    ::android::ScopedTrace PASTE(___tracer, __LINE__) (ATRACE_TAG_HAL, "HIDL::IHostapdVendorIfaceCallback::onVendorDriverHang::client");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)&state);
        _hidl_args.push_back((void *)&msg);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_ENTRY, "vendor.samsung_slsi.hardware.wifi.hostapd", "1.0", "IHostapdVendorIfaceCallback", "onVendorDriverHang", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Parcel _hidl_data;
    ::android::hardware::Parcel _hidl_reply;
    ::android::status_t _hidl_err;
    ::android::status_t _hidl_transact_err;
    ::android::hardware::Status _hidl_status;

    _hidl_err = _hidl_data.writeInterfaceToken(BpHwHostapdVendorIfaceCallback::descriptor);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_state_parent;

    _hidl_err = _hidl_data.writeBuffer(&state, sizeof(state), &_hidl_state_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            state,
            &_hidl_data,
            _hidl_state_parent,
            0 /* parentOffset */);

    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    size_t _hidl_msg_parent;

    _hidl_err = _hidl_data.writeBuffer(&msg, sizeof(msg), &_hidl_msg_parent);
    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_err = ::android::hardware::writeEmbeddedToParcel(
            msg,
            &_hidl_data,
            _hidl_msg_parent,
            0 /* parentOffset */);

    if (_hidl_err != ::android::OK) { goto _hidl_error; }

    _hidl_transact_err = ::android::hardware::IInterface::asBinder(_hidl_this)->transact(1 /* onVendorDriverHang */, _hidl_data, &_hidl_reply, 1u /* oneway */);
    if (_hidl_transact_err != ::android::OK) 
    {
        _hidl_err = _hidl_transact_err;
        goto _hidl_error;
    }

    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::CLIENT_API_EXIT, "vendor.samsung_slsi.hardware.wifi.hostapd", "1.0", "IHostapdVendorIfaceCallback", "onVendorDriverHang", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    return ::android::hardware::Return<void>();

_hidl_error:
    _hidl_status.setFromStatusT(_hidl_err);
    return ::android::hardware::Return<void>(_hidl_status);
}


// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::onVendorDriverHang(const ::android::hardware::hidl_string& state, const ::android::hardware::hidl_string& msg){
    ::android::hardware::Return<void>  _hidl_out = ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::BpHwHostapdVendorIfaceCallback::_hidl_onVendorDriverHang(this, this, state, msg);

    return _hidl_out;
}


// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::interfaceChain(interfaceChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_debug(this, this, fd, options);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::interfaceDescriptor(interfaceDescriptor_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_interfaceDescriptor(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::getHashChain(getHashChain_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getHashChain(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::setHALInstrumentation(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_setHALInstrumentation(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwHostapdVendorIfaceCallback::linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie){
    ::android::hardware::ProcessState::self()->startThreadPool();
    ::android::hardware::hidl_binder_death_recipient *binder_recipient = new ::android::hardware::hidl_binder_death_recipient(recipient, cookie, this);
    std::unique_lock<std::mutex> lock(_hidl_mMutex);
    _hidl_mDeathRecipients.push_back(binder_recipient);
    return (remote()->linkToDeath(binder_recipient) == ::android::OK);
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::ping(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_ping(this, this);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::getDebugInfo(getDebugInfo_cb _hidl_cb){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_getDebugInfo(this, this, _hidl_cb);

    return _hidl_out;
}

::android::hardware::Return<void> BpHwHostapdVendorIfaceCallback::notifySyspropsChanged(){
    ::android::hardware::Return<void>  _hidl_out = ::android::hidl::base::V1_0::BpHwBase::_hidl_notifySyspropsChanged(this, this);

    return _hidl_out;
}

::android::hardware::Return<bool> BpHwHostapdVendorIfaceCallback::unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient){
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


BnHwHostapdVendorIfaceCallback::BnHwHostapdVendorIfaceCallback(const ::android::sp<IHostapdVendorIfaceCallback> &_hidl_impl)
        : ::android::hidl::base::V1_0::BnHwBase(_hidl_impl, "vendor.samsung_slsi.hardware.wifi.hostapd@1.0", "IHostapdVendorIfaceCallback") { 
            _hidl_mImpl = _hidl_impl;
            auto prio = ::android::hardware::getMinSchedulerPolicy(_hidl_impl);
            mSchedPolicy = prio.sched_policy;
            mSchedPriority = prio.prio;
            setRequestingSid(::android::hardware::getRequestingSid(_hidl_impl));
}

BnHwHostapdVendorIfaceCallback::~BnHwHostapdVendorIfaceCallback() {
    ::android::hardware::details::gBnMap->eraseIfEqual(_hidl_mImpl.get(), this);
}

// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.
::android::status_t BnHwHostapdVendorIfaceCallback::_hidl_onVendorDriverHang(
        ::android::hidl::base::V1_0::BnHwBase* _hidl_this,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        TransactCallback _hidl_cb) {
    #ifdef __ANDROID_DEBUGGABLE__
    bool mEnableInstrumentation = _hidl_this->isInstrumentationEnabled();
    const auto &mInstrumentationCallbacks = _hidl_this->getInstrumentationCallbacks();
    #endif // __ANDROID_DEBUGGABLE__

    ::android::status_t _hidl_err = ::android::OK;
    if (!_hidl_data.enforceInterface(BnHwHostapdVendorIfaceCallback::Pure::descriptor)) {
        _hidl_err = ::android::BAD_TYPE;
        return _hidl_err;
    }

    const ::android::hardware::hidl_string* state;
    const ::android::hardware::hidl_string* msg;

    size_t _hidl_state_parent;

    _hidl_err = _hidl_data.readBuffer(sizeof(*state), &_hidl_state_parent,  reinterpret_cast<const void **>(&state));

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_string &>(*state),
            _hidl_data,
            _hidl_state_parent,
            0 /* parentOffset */);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    size_t _hidl_msg_parent;

    _hidl_err = _hidl_data.readBuffer(sizeof(*msg), &_hidl_msg_parent,  reinterpret_cast<const void **>(&msg));

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    _hidl_err = ::android::hardware::readEmbeddedFromParcel(
            const_cast<::android::hardware::hidl_string &>(*msg),
            _hidl_data,
            _hidl_msg_parent,
            0 /* parentOffset */);

    if (_hidl_err != ::android::OK) { return _hidl_err; }

    atrace_begin(ATRACE_TAG_HAL, "HIDL::IHostapdVendorIfaceCallback::onVendorDriverHang::server");
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        _hidl_args.push_back((void *)state);
        _hidl_args.push_back((void *)msg);
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_ENTRY, "vendor.samsung_slsi.hardware.wifi.hostapd", "1.0", "IHostapdVendorIfaceCallback", "onVendorDriverHang", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    ::android::hardware::Return<void> _hidl_ret = static_cast<IHostapdVendorIfaceCallback*>(_hidl_this->getImpl().get())->onVendorDriverHang(*state, *msg);

    (void) _hidl_cb;

    atrace_end(ATRACE_TAG_HAL);
    #ifdef __ANDROID_DEBUGGABLE__
    if (UNLIKELY(mEnableInstrumentation)) {
        std::vector<void *> _hidl_args;
        for (const auto &callback: mInstrumentationCallbacks) {
            callback(InstrumentationEvent::SERVER_API_EXIT, "vendor.samsung_slsi.hardware.wifi.hostapd", "1.0", "IHostapdVendorIfaceCallback", "onVendorDriverHang", &_hidl_args);
        }
    }
    #endif // __ANDROID_DEBUGGABLE__

    _hidl_ret.assertOk();
    ::android::hardware::writeToParcel(::android::hardware::Status::ok(), _hidl_reply);

    return _hidl_err;
}


// Methods from ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback follow.

// Methods from ::android::hidl::base::V1_0::IBase follow.
::android::hardware::Return<void> BnHwHostapdVendorIfaceCallback::ping() {
    return ::android::hardware::Void();
}
::android::hardware::Return<void> BnHwHostapdVendorIfaceCallback::getDebugInfo(getDebugInfo_cb _hidl_cb) {
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

::android::status_t BnHwHostapdVendorIfaceCallback::onTransact(
        uint32_t _hidl_code,
        const ::android::hardware::Parcel &_hidl_data,
        ::android::hardware::Parcel *_hidl_reply,
        uint32_t _hidl_flags,
        TransactCallback _hidl_cb) {
    ::android::status_t _hidl_err = ::android::OK;

    switch (_hidl_code) {
        case 1 /* onVendorDriverHang */:
        {
            _hidl_err = ::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::BnHwHostapdVendorIfaceCallback::_hidl_onVendorDriverHang(this, _hidl_data, _hidl_reply, _hidl_cb);
            break;
        }

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

BsHostapdVendorIfaceCallback::BsHostapdVendorIfaceCallback(const ::android::sp<::vendor::samsung_slsi::hardware::wifi::hostapd::V1_0::IHostapdVendorIfaceCallback> impl) : ::android::hardware::details::HidlInstrumentor("vendor.samsung_slsi.hardware.wifi.hostapd@1.0", "IHostapdVendorIfaceCallback"), mImpl(impl) {
    mOnewayQueue.start(3000 /* similar limit to binderized */);
}

::android::hardware::Return<void> BsHostapdVendorIfaceCallback::addOnewayTask(std::function<void(void)> fun) {
    if (!mOnewayQueue.push(fun)) {
        return ::android::hardware::Status::fromExceptionCode(
                ::android::hardware::Status::EX_TRANSACTION_FAILED,
                "Passthrough oneway function queue exceeds maximum size.");
    }
    return ::android::hardware::Status();
}

::android::sp<IHostapdVendorIfaceCallback> IHostapdVendorIfaceCallback::tryGetService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwHostapdVendorIfaceCallback>(serviceName, false, getStub);
}

::android::sp<IHostapdVendorIfaceCallback> IHostapdVendorIfaceCallback::getService(const std::string &serviceName, const bool getStub) {
    return ::android::hardware::details::getServiceInternal<BpHwHostapdVendorIfaceCallback>(serviceName, true, getStub);
}

::android::status_t IHostapdVendorIfaceCallback::registerAsService(const std::string &serviceName) {
    return ::android::hardware::details::registerAsServiceInternal(this, serviceName);
}

bool IHostapdVendorIfaceCallback::registerForNotifications(
        const std::string &serviceName,
        const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification) {
    const ::android::sp<::android::hidl::manager::V1_0::IServiceManager> sm
            = ::android::hardware::defaultServiceManager();
    if (sm == nullptr) {
        return false;
    }
    ::android::hardware::Return<bool> success =
            sm->registerForNotifications("vendor.samsung_slsi.hardware.wifi.hostapd@1.0::IHostapdVendorIfaceCallback",
                    serviceName, notification);
    return success.isOk() && success;
}

static_assert(sizeof(::android::hardware::MQDescriptor<char, ::android::hardware::kSynchronizedReadWrite>) == 32, "wrong size");
static_assert(sizeof(::android::hardware::hidl_handle) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_memory) == 40, "wrong size");
static_assert(sizeof(::android::hardware::hidl_string) == 16, "wrong size");
static_assert(sizeof(::android::hardware::hidl_vec<char>) == 16, "wrong size");

}  // namespace V1_0
}  // namespace hostapd
}  // namespace wifi
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor
