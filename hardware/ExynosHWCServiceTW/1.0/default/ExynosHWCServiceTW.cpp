#include "ExynosHWCServiceTW.h"
#include "IExynosHWC.h"
#include <log/log.h>
#include <binder/IServiceManager.h>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace ExynosHWCServiceTW {
namespace V1_0 {
namespace implementation {

// Methods from ::vendor::samsung_slsi::hardware::ExynosHWCServiceTW::V1_0::IExynosHWCServiceTW follow.

ExynosHWCServiceTW::ExynosHWCServiceTW() {
    clearExynosHWCServiceTW();
    createExynosHWCServiceTW();
}

ExynosHWCServiceTW::~ExynosHWCServiceTW() {
    if (mHwcService)
        IInterface::asBinder(mHwcService)->unlinkToDeath(mTWDeathObserver);
}

void ExynosHWCServiceTW::clearExynosHWCServiceTW() {
    mHwcService = NULL;
}

void ExynosHWCServiceTW::createExynosHWCServiceTW() {
    sp<IServiceManager> sm = defaultServiceManager();
    if (sm != NULL) {
        if (sm->checkService(String16("Exynos.HWCService"))) {
            mHwcService = interface_cast<IExynosHWCService>
                (sm->getService(String16("Exynos.HWCService")));
        } else {
            mHwcService = NULL;
        }

        if (mHwcService == NULL) {
            ALOGE("Can not get ExynosHWCService");
        } else {
            class TWDeathObserver : public IBinder::DeathRecipient {
                ExynosHWCServiceTW& mExynosHWCServiceTW;
                virtual void binderDied(const wp<IBinder>& token) {
                    ALOGW("ExynosHWCServiceTW remote (Exynos.HWCService) died [%p]", token.unsafe_get());
                    mExynosHWCServiceTW.clearExynosHWCServiceTW();
                }
            public:
                explicit TWDeathObserver(ExynosHWCServiceTW& get) : mExynosHWCServiceTW(get) {}
            };

            mTWDeathObserver = new TWDeathObserver(*const_cast<ExynosHWCServiceTW*>(this));
            IInterface::asBinder(mHwcService)->linkToDeath(mTWDeathObserver);
        }
    }
}
sp<IExynosHWCService> ExynosHWCServiceTW::getHwcService() {
    if (mHwcService != NULL)
        return mHwcService;
    else {
        sp<IServiceManager> sm = defaultServiceManager();
        if (sm != NULL) {
            mHwcService = interface_cast<IExynosHWCService>
                (sm->getService(String16("Exynos.HWCService")));

            return mHwcService;
        }
    }
    return NULL;
}

Return<int32_t> ExynosHWCServiceTW::setWFDOutputResolution(uint32_t width, uint32_t height) {
    int32_t ret = -1;
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = (int32_t)(mHwcService->setWFDOutputResolution(width, height));
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);
    return ret;
}

Return<int32_t> ExynosHWCServiceTW::setVDSGlesFormat(int32_t format) {
    int32_t ret = -1;
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = (int32_t)(mHwcService->setVDSGlesFormat(format));
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);
    return ret;
}

Return<int32_t> ExynosHWCServiceTW::setWFDMode(uint32_t mode) {
    ALOGW("%s :: doesn't support this function anymore (%u)", __func__, mode);
    return -1;
}

Return<int32_t> ExynosHWCServiceTW::getWFDMode() {
    int32_t ret = -1;
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = (int32_t)(mHwcService->getWFDMode());
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);
    return ret;
}

Return<void> ExynosHWCServiceTW::getWFDInfo(getWFDInfo_cb _hidl_cb) {
    int32_t ret = -1;
    WFDInfo info;

    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = (int32_t)(mHwcService->getWFDInfo(&info.state, &info.compositionType, &info.format, &info.usage, &info.width, &info.height));
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);
    _hidl_cb(ret, info);
    return Void();
}

Return<int32_t> ExynosHWCServiceTW::sendWFDCommand(int32_t cmd, int32_t ext1, int32_t ext2) {
    int32_t ret = -1;
#ifdef SUPPORT_WFD_COMMAND
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = (int32_t)(mHwcService->sendWFDCommand(cmd, ext1, ext2));
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);
#endif
    return ret;
}

Return<void> ExynosHWCServiceTW::setBootFinished() {
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        mHwcService->setBootFinished();
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);

    return Void();
}

Return<void> ExynosHWCServiceTW::setHWCDebug(int32_t debug) {
    ALOGW("%s :: doesn't support this function anymore (%d)", __func__, debug);
    return Void();
}

Return<int32_t> ExynosHWCServiceTW::setHWCCtl(uint32_t display, uint32_t ctrl, int32_t val) {
    ALOGW("%s :: doesn't support this function anymore (%u, %u, %d)",
            __func__, display, ctrl, val);
    return -1;
}

Return<uint32_t> ExynosHWCServiceTW::getHWCDebug() {
    uint32_t ret = 0;

    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = mHwcService->getHWCDebug();
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);

    return ret;
}

Return<int32_t> ExynosHWCServiceTW::setSecureVDSMode(uint32_t mode) {
    ALOGW("%s :: doesn't support this function anymore (%u)", __func__, mode);
    return -1;
}

Return<void> ExynosHWCServiceTW::enableMPP(uint32_t physicalType, uint32_t physicalIndex, uint32_t logicalIndex, uint32_t enable) {
    ALOGW("%s :: Can not get ExynosHWCService (%u, %u, %u, %u)", __func__,
            physicalType, physicalIndex, logicalIndex, enable);
    return Void();
}

Return<void> ExynosHWCServiceTW::setPresentationMode(uint32_t multiple_layerStack) {
    ALOGW("%s :: doesn't support this function anymore (%u)", __func__, multiple_layerStack);
    return Void();
}

Return<int32_t> ExynosHWCServiceTW::getExternalHdrCapabilities() {
    int32_t ret = -1;
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = mHwcService->getExternalHdrCapabilities();
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);

    return ret;
}

Return<void> ExynosHWCServiceTW::getCPUPerfInfo(int display, int config, getCPUPerfInfo_cb _hidl_cb) {
    int32_t ret = -1;
    CPUPerfInfo info;

#ifdef USE_CPU_PERF_MODE
    if (mHwcService == NULL)
        getHwcService();

    if (mHwcService != NULL)
        ret = (int32_t)mHwcService->getCPUPerfInfo(display, config, &info.cpuIDs, &info.min_clock);
    else
        ALOGE("%s :: Can not get ExynosHWCService", __func__);

    _hidl_cb(ret, info);
#else
    info.cpuIDs = 0;
    info.min_clock = 0;
    _hidl_cb(ret, info);
#endif
    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IExynosHWCServiceTW* HIDL_FETCH_IExynosHWCServiceTW(const char* /* name */) {
    return new ExynosHWCServiceTW();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace ExynosHWCServiceTW
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor
