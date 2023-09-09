// FIXME: your file license if you have one

#include "GeoTransService.h"

namespace vendor {
namespace samsung_slsi {
namespace hardware {
namespace geoTransService {
namespace V1_0 {
namespace implementation {

void* m_csc;
ExynosGDCInterface* m_gdc;

static native_handle_t* cloneCSCHndl[2];
static native_handle_t* cloneGDCHndl[2];

#define v4l2_fourcc(a,b,c,d) ((__u32) (a) | ((__u32) (b) << 8) | ((__u32) (c) << 16) | ((__u32) (d) << 24))
#define V4L2_PIX_FMT_NV21M   v4l2_fourcc('N', 'M', '2', '1') /* 21  Y/CrCb 4:2:0  */

void GeoTransService::serviceDied(uint64_t cookie, const android::wp<::android::hidl::base::V1_0::IBase>& who) {
    GEOTRANS_LOGD("%s GeoTransClient had died...", __FUNCTION__);

    (void)cookie;
    (void)who;
    if (NULL != m_csc) {
        csc_deinit(m_csc);
        m_csc = NULL;
    }
    if (NULL != m_gdc) {
        m_gdc->destroy();
        delete m_gdc;
        m_gdc = NULL;
    }
    // Service died, unlock mutexes
    csc_mutex.unlock();
    gdc_mutex.unlock();
}

// Methods from ::vendor::samsung_slsi::hardware::geoTransService::V1_0::IGeoTransService follow.
Return<int32_t> GeoTransService::runCSC(const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& dst, const hidl_handle& dstHndl, const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& src, const hidl_handle& srcHndl, const sp<::android::hidl::base::V1_0::IBase>& client) {

    // first lock mutex for csc
    csc_mutex.lock();
    if (client != nullptr) {
        mLastClient = client;
        mLastClient->linkToDeath(this, 0);
    }

    // Initialize CSC
    CSC_METHOD cscMethod = CSC_METHOD_HW;
    m_csc = csc_init(cscMethod);
    if (m_csc == NULL) {
        GEOTRANS_LOGE("ERR(%s[%d]):csc_init() fail", __FUNCTION__, __LINE__);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    csc_set_hw_property(m_csc, CSC_HW_PROPERTY_FIXED_NODE, (4)); // PICTURE_GSC_NODE_NUM

    // local variables
    int32_t srcFdArr[4];
    int32_t dstFdArr[4];
    int32_t ret = android::INVALID_OPERATION;

    // Get Fds for the input and output buffer
    cloneCSCHndl[0] = native_handle_clone(srcHndl.getNativeHandle());
    native_handle_t* srcHandle = cloneCSCHndl[0];
    if(!srcHandle) {
        GEOTRANS_LOGE("ERR(%s[%d]):native_handle_clone() fail for src buffer", __FUNCTION__, __LINE__);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    if(srcHandle->numFds < 1) {
        GEOTRANS_LOGE("ERR(%s[%d]):numFds(src)=%d", __FUNCTION__, __LINE__, srcHandle->numFds);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    cloneCSCHndl[1] = native_handle_clone(dstHndl.getNativeHandle());
    native_handle_t* dstHandle = cloneCSCHndl[1];
    if(!dstHandle) {
        GEOTRANS_LOGE("ERR(%s[%d]):native_handle_clone() fail for dst buffer", __FUNCTION__, __LINE__);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    if(dstHandle->numFds < 1) {
        GEOTRANS_LOGE("ERR(%s[%d]):numFds(dst)=%d", __FUNCTION__, __LINE__, dstHandle->numFds);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    // Set format for the input and output buffer
    ret = csc_set_src_format(m_csc,
            src.width, src.height, // full size
            src.fov.l, src.fov.t, ((src.fov.r - src.fov.l)& ~(2 - 1)), ((src.fov.b - src.fov.t)& ~(2 - 1)), // x, y, w, h
            srcHandle->numFds == 1 ? HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP :
            srcHandle->numFds == 2 ? V4L2_PIX_2_HAL_PIXEL_FORMAT(V4L2_PIX_FMT_NV21M) :
            HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP,
            0);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):csc_set_src_format() failed (HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP) (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = csc_set_dst_format(m_csc,
            dst.width, dst.height, // full size
            dst.fov.l, dst.fov.t, ((dst.fov.r - dst.fov.l)& ~(2 - 1)), ((dst.fov.b - dst.fov.t)& ~(2 - 1)), // x, y, w, h
            dstHandle->numFds == 1 ? HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP :
            dstHandle->numFds == 2 ? V4L2_PIX_2_HAL_PIXEL_FORMAT(V4L2_PIX_FMT_NV21M) :
            HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP,
            0);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):csc_set_dst_format() failed (HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP) (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    // Connect fds to the buffers
    srcFdArr[0] = srcHandle->data[0]; // fdY
    srcFdArr[1] = (srcHandle->numFds == 1) ? -1 : (srcHandle->numFds == 2) ? srcHandle->data[1] : -1; // fdUV
    ret = csc_set_src_buffer(m_csc, (void **)&srcFdArr, CSC_MEMORY_DMABUF);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):csc_set_src_buffer() failed (fdY=%d, fdUV=%d) (%d)", __FUNCTION__, __LINE__, srcFdArr[0], srcFdArr[1], ret);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    dstFdArr[0] = dstHandle->data[0]; // fdY
    dstFdArr[1] = (dstHandle->numFds == 1) ? -1 : (dstHandle->numFds == 2) ? dstHandle->data[1] : -1; // fdUV
    ret = csc_set_dst_buffer(m_csc, (void **)&dstFdArr, CSC_MEMORY_DMABUF);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):csc_set_dst_buffer() failed (fdY=%d, fdUV=%d) (%d)", __FUNCTION__, __LINE__, dstFdArr[0], dstFdArr[1], ret);
        // unlock mutex for csc
        csc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = csc_convert_with_rotation(m_csc, 0, 0, 0);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):csc_convert_with_rotation() failed [src(%dx%d, fdY=%d, fdUV=%d ) / dst(%dx%d, fdY=%d, fdUV=%d) (%d)",
            __FUNCTION__, __LINE__,
            src.width, src.height, srcFdArr[0], srcFdArr[1],
            dst.width, dst.height, dstFdArr[0], dstFdArr[1],
            (int)ret);
        ret = android::INVALID_OPERATION;
    }
    native_handle_close(cloneCSCHndl[0]);
    native_handle_close(cloneCSCHndl[1]);
    native_handle_delete(cloneCSCHndl[0]);
    native_handle_delete(cloneCSCHndl[1]);

    // Deinitialize CDC
    if (m_csc != NULL) {
        csc_deinit(m_csc);
    }
    m_csc = NULL;

    //unlink death recipient
    if (mLastClient != nullptr) {
        mLastClient->unlinkToDeath(this);
        mLastClient = nullptr;
    }
    // unlock mutex for csc
    csc_mutex.unlock();
    return int32_t {android::NO_ERROR};
}

Return<int32_t> GeoTransService::runGDC(const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& dst, const hidl_handle& dstHndl, const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::BufferData& src, const hidl_handle& srcHndl, const ::vendor::samsung_slsi::hardware::geoTransService::V1_0::GDCGrid& gridXY, const sp<::android::hidl::base::V1_0::IBase>& client) {

    // first lock mutex for gdc
    gdc_mutex.lock();
    if (client != nullptr) {
        mLastClient = client;
        mLastClient->linkToDeath(this, 0);
    }

    // Initialize GDC
    m_gdc = new ExynosGDCInterface();
    if (m_gdc->create() != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):m_gdc->create() fail", __FUNCTION__, __LINE__);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }

    // local variables
    int32_t ret = android::INVALID_OPERATION;
    int32_t grid_x[33][33];
    int32_t grid_y[33][33];
    memset(grid_x, 0x0, sizeof(int32_t) * 33 * 33);
    memset(grid_y, 0x0, sizeof(int32_t) * 33 * 33);
    struct ExynosGDCSizeParam srcSizeParam, dstSizeParam;
    struct ExynosGDCBuf srcBuf, dstBuf;

    // Get Fds for the input and output buffer
    cloneGDCHndl[0] = native_handle_clone(srcHndl.getNativeHandle());
    native_handle_t* srcHandle = cloneGDCHndl[0];
    if(!srcHandle) {
        GEOTRANS_LOGE("ERR(%s[%d]):native_handle_clone() fail for src buffer", __FUNCTION__, __LINE__);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    if(srcHandle->numFds < 1) {
        GEOTRANS_LOGE("ERR(%s[%d]):numFds(src)=%d", __FUNCTION__, __LINE__, srcHandle->numFds);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    cloneGDCHndl[1] = native_handle_clone(dstHndl.getNativeHandle());
    native_handle_t* dstHandle = cloneGDCHndl[1];
    if(!dstHandle) {
        GEOTRANS_LOGE("ERR(%s[%d]):native_handle_clone() fail for dst buffer", __FUNCTION__, __LINE__);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    if(dstHandle->numFds < 1) {
        GEOTRANS_LOGE("ERR(%s[%d]):numFds(dst)=%d", __FUNCTION__, __LINE__, dstHandle->numFds);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }

    // Set image size for the input and output
    ret = m_gdc->setSrcImageSize(src.width, src.height);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setSrcImageSize() failed for src(%dx%d) (%d)", __FUNCTION__, __LINE__, src.width, src.height, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->setDstImageSize(dst.width, dst.height);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setDstImageSize() failed for dst(%dx%d) (%d)", __FUNCTION__, __LINE__, dst.width, dst.height, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }

    // Set image format for the input and output
    ret = m_gdc->setSrcColorFormat(
            srcHandle->numFds == 1 ? HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP :
            srcHandle->numFds == 2 ? V4L2_PIX_2_HAL_PIXEL_FORMAT(V4L2_PIX_FMT_NV21M) :
            HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP, srcHandle->numFds);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setSrcColorFormat() failed (HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP) (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->setDstColorFormat(
            dstHandle->numFds == 1 ? HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP :
            dstHandle->numFds == 2 ? V4L2_PIX_2_HAL_PIXEL_FORMAT(V4L2_PIX_FMT_NV21M) :
            HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP, dstHandle->numFds);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setDstColorFormat() failed (HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP) (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }

    srcBuf.index = 0;
    if (srcHandle->numFds == 1) {
        srcBuf.planeCount = 1;
        srcBuf.planes[0].fd = srcHandle->data[0]; // fdY
        srcBuf.planes[0].length = src.width * src.height * 3 / 2;
    } else if (srcHandle->numFds == 2) {
        srcBuf.planeCount = 2;
        srcBuf.planes[0].fd = srcHandle->data[0]; // fdY
        srcBuf.planes[1].fd = srcHandle->data[1]; // fdUV
        srcBuf.planes[0].length = src.width * src.height;
        srcBuf.planes[1].length = src.width * src.height / 2;
    }
    // output crop width/height must be 4-aligned since GDC performs 4 ppc
    srcSizeParam.cropX = src.fov.l;
    srcSizeParam.cropY = src.fov.t;
    srcSizeParam.cropW = ((src.fov.r - src.fov.l)& ~(4 - 1));
    srcSizeParam.cropH = ((src.fov.b - src.fov.t)& ~(4 - 1));
    srcSizeParam.fullW = src.width;
    srcSizeParam.fullH = src.height;

    dstBuf.index = 0;
    if (dstHandle->numFds == 1) {
        dstBuf.planeCount = 1;
        dstBuf.planes[0].fd = dstHandle->data[0]; // fdY
        dstBuf.planes[0].length = dst.width * dst.height * 3 / 2;
    } else if (dstHandle->numFds == 2) {
        dstBuf.planeCount = 2;
        dstBuf.planes[0].fd = dstHandle->data[0]; // fdY
        dstBuf.planes[1].fd = dstHandle->data[1]; // fdUV
        dstBuf.planes[0].length = dst.width * dst.height;
        dstBuf.planes[1].length = dst.width * dst.height / 2;
    }
    // output crop width/height must be 4-aligned since GDC performs 4 ppc
    dstSizeParam.cropX = dst.fov.l;
    dstSizeParam.cropY = dst.fov.t;
    dstSizeParam.cropW = ((dst.fov.r - dst.fov.l)& ~(4 - 1));
    dstSizeParam.cropH = ((dst.fov.b - dst.fov.t)& ~(4 - 1));
    dstSizeParam.fullW = dst.width;
    dstSizeParam.fullH = dst.height;

    // Set grid table for GDC
    for(int i = 0; i < 33; i++) {
        for(int j = 0; j < 33; j++) {
			grid_x[i][j] = gridXY.gridX[i][j];
            grid_y[i][j] = gridXY.gridY[i][j];
        }
    }
    ret = m_gdc->setGridTable((int32_t*)grid_x, (int32_t*)grid_y, 33, 33);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setGridTable() failed (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->setInputBuffer(srcBuf);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setInputBuffer() failed (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->setOutputBuffer(dstBuf);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setOutputBuffer() failed (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->setInputSize(srcSizeParam);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setInputSize() failed (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->setOutputSize(dstSizeParam);
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d]):setOutputSize() failed (%d)", __FUNCTION__, __LINE__, ret);
        // unlock mutex for gdc
        gdc_mutex.unlock();
        return android::INVALID_OPERATION;
    }
    ret = m_gdc->runGDC();
    if (ret != android::NO_ERROR) {
        GEOTRANS_LOGE("ERR(%s[%d])runGDC() failed (%d)", __FUNCTION__, __LINE__, ret);
        ret = android::INVALID_OPERATION;
    }

    native_handle_close(cloneGDCHndl[0]);
    native_handle_close(cloneGDCHndl[1]);
    native_handle_delete(cloneGDCHndl[0]);
    native_handle_delete(cloneGDCHndl[1]);

    // Deinitialize GDC
    if (m_gdc != NULL) {
        m_gdc->destroy();
        delete m_gdc;
    }
    m_gdc = NULL;

    //unlink death recipient
    if (mLastClient != nullptr) {
        mLastClient->unlinkToDeath(this);
        mLastClient = nullptr;
    }
    // unlock mutex for gdc
    gdc_mutex.unlock();
    return int32_t {android::NO_ERROR};
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//IGeoTransService* HIDL_FETCH_IGeoTransService(const char* /* name */) {
    //return new GeoTransService();
//}
//
}  // namespace implementation
}  // namespace V1_0
}  // namespace geoTransService
}  // namespace hardware
}  // namespace samsung_slsi
}  // namespace vendor
