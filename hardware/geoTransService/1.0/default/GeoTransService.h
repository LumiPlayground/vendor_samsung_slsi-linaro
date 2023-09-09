// FIXME: your file license if you have one

#pragma once

#include <vendor/samsung_slsi/hardware/geoTransService/1.0/IGeoTransService.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "ExynosGDCTypes.h"
#include "csc.h"
#include "ExynosGDCInterface.h"
#include "exynos_v4l2.h"
#include "exynos_format.h"

#include <android/log.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "GEOTRANS_SERVICE"
#define GEOTRANS_LOGE(str,...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s:%d " str, __func__, __LINE__, ##__VA_ARGS__))
#define GEOTRANS_LOGD(str,...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s:%d " str, __func__, __LINE__, ##__VA_ARGS__))

#include <mutex>

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace geoTransService {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_handle;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct GeoTransService : public IGeoTransService, public ::android::hardware::hidl_death_recipient {
    // Methods from ::vendor::samsung_slsi::hardware::geoTransService::V1_0::IGeoTransService follow.
    Return<int32_t> runCSC(const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& dst, const hidl_handle& dstHndl, const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& src, const hidl_handle& srcHndl, const sp<::android::hidl::base::V1_0::IBase>& client) override;
    Return<int32_t> runGDC(const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& dst, const hidl_handle& dstHndl, const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& src, const hidl_handle& srcHndl, const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::GDCGrid& gridXY, const sp<::android::hidl::base::V1_0::IBase>& client) override;

    // From hidl_death_recipient
    void serviceDied(uint64_t cookie, const android::wp<::android::hidl::base::V1_0::IBase>& who) override;
    ::android::sp<::android::hidl::base::V1_0::IBase> mLastClient;
    std::mutex csc_mutex;
    std::mutex gdc_mutex;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
};

// FIXME: most likely delete, this is only for passthrough implementations
// extern "C" IGeoTransService* HIDL_FETCH_IGeoTransService(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace geoTransService
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor
