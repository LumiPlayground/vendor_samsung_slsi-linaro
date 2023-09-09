#ifndef __SEVA_IMAGE_HPP
#define __SEVA_IMAGE_HPP

#include <memory>

#include "buffer.hpp"

/**
 * @file image.hpp
 * @brief Image class header file
 */

namespace seva {
namespace graph {
/**
 * @brief represents an image object.
 *
 * Can be used as input/output buffer of Node.
 */
class Image : public Buffer {
  public:
    /**
     * @brief represents a rectangular object.
     *
     * Normally, used for representing image's valid region.
     * A rectangular can be defined startx, starty, endx, endy value
     */
    class Rect {
      public:
        /**
         * @brief Constructor.
         */
        Rect() : mStartX(0), mStartY(0), mEndX(0), mEndY(0), mChanged(false) {}
        /**
         * @brief Constructor.
         *
         * @param [in] startX start x of valid image.
         * @param [in] startY start y of valid image.
         * @param [in] endX end x of valid image.
         * @param [in] endY end y of valid image.
         */
        Rect(size_t startX, size_t startY, size_t endX, size_t endY) {
            SetStartX(startX);
            SetStartY(startY);
            SetEndX(endX);
            SetEndY(endY);
        }
        /**
         * @brief Set startx of rectangular.
         * @param [in] x the starting coordinates of X.
         */
        void SetStartX(size_t x, bool changedByUser = true) {
            mStartX = x;

            if (changedByUser == true) {
                SetChanged(true);
            }
        }
        /**
         * @brief Set starty of rectangular.
         * @param [in] y the starting coordinates of Y.
         */
        void SetStartY(size_t y, bool changedByUser = true) {
            mStartY = y;

            if (changedByUser == true) {
                SetChanged(true);
            }
        }
        /**
         * @brief Set endx of rectangular.
         * @param [in] x the end coordinates of X.
         */
        void SetEndX(size_t x, bool changedByUser = true) {
            mEndX = x;

            if (changedByUser == true) {
                SetChanged(true);
            }
        }
        /**
         * @brief Set endy of rectangular.
         * @param [in] y the end coordinates of Y.
         */
        void SetEndY(size_t y, bool changedByUser = true) {
            mEndY = y;

            if (changedByUser == true) {
                SetChanged(true);
            }
        }
        /**
         * @brief Get startx of rectangular.
         * @return startx
         */
        size_t GetStartX(void) { return mStartX; }
        /**
         * @brief Get starty.
         * @return starty
         */
        size_t GetStartY(void) { return mStartY; }
        /**
         * @brief Get endx.
         * @return endx
         */
        size_t GetEndX(void) { return mEndX; }
        /**
         * @brief Get endy.
         * @return endy
         */
        size_t GetEndY(void) { return mEndY; }
        /**
         * @cond
         * internal
         */
        /* Used only internally */
        void SetChanged(bool changed) { mChanged = changed; }
        bool GetChanged(void) { return mChanged; }
        /* Used only internally */
        /**
         * @endcond
         */

      private:
        size_t mStartX;
        size_t mStartY;
        size_t mEndX;
        size_t mEndY;
        bool mChanged;
    };

    enum class Format : int {
        NONE = 0,
        U8,
        U16,
        U32,
        S8,
        S16,
        S32,
        F8,
        F16,
        F32,
        RGBX888,
        RGB888,
        BGRX888,
        BGR888,
        LUV888,
        YUV422_1P,
        YUV422_2P,
        YUV422_UV,
        YUV420_NV21,
        YUV420_NV12,
        YUV420_NV21_UV,
        YUV420_NV12_UV,
        UNKNOWN,
    };

    enum class Channel : int {
        NONE = 0,
        /*! \brief Use to extract the RED channel, no matter the byte or packing order. */
        R,
        /*! \brief Use to extract the GREEN channel, no matter the byte or packing order. */
        G,
        /*! \brief Use to extract the BLUE channel, no matter the byte or packing order. */
        B,
        /*! \brief Use to extract the ALPHA channel, no matter the byte or packing order. */
        X,
        /*! \brief Use to extract the LUMA channel, no matter the byte or packing order. */
        Y,
        /*! \brief Use to extract the Cb/U channel, no matter the byte or packing order. */
        U,
        /*! \brief Use to extract the Cr/V/Value channel, no matter the byte or packing order. */
        V,
        /*! \brief Use to extract chroma channel. */
        UV,
        /*! \brief ALL Channels for YUV Image format. */
        ALL,
    };

  public:
    /**
     * @brief Constructor.
     *
     * @param [in] width image width.
     * @param [in] height image height.
     * @param [in] format image format.
     */
    Image(uint32_t width, uint32_t height, Format format);

    /**
     * @brief Constructor.
     *
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @param [in] width image width.
     * @param [in] height image height.
     * @param [in] format image format.
     */
    Image(int fd, void *addr, uint32_t width, uint32_t height, Format format);

    /**
     * @brief Constructor.
     *
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @param [in] offset offset from start virtual address for ion buffer.
     * @param [in] size size of ion buffer.
     * @param [in] width image width.
     * @param [in] height image height.
     * @param [in] format image format.
     */
    Image(int fd, void *addr, uint32_t offset, uint32_t size, uint32_t width, uint32_t height, Format format);

    /**
     * @brief Constructor.
     *
     * @param [in] format image format.
     */
    Image(Format format);

    /**
     * @brief Virtual object constructor.
     */
    Image();

    /**
     * @brief Destructor.
     */
    ~Image();

    /**
     * @brief Get width.
     * @return Image width in bytes.
     */
    uint32_t GetWidth() const { return mRealWidth; }

    /**
     * @brief Get height.
     * @return Image height in bytes.
     */
    uint32_t GetHeight() const { return mRealHeight; }

    /**
     * @brief Get format.
     * @return Image format.
     */
    Format GetFormat() const { return mFormat; }

    /**
     * @brief Get valid region.
     * @param [out] region image valid region.
     */
    void GetValidRegion(Image::Rect *region);
    /**
     * @brief Set valid region.
     * @param [in] region image valid region.
     */
    bool SetValidRegion(Image::Rect *region, bool changedByUser = true);

    /**
     * @brief set fd.
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool SetFd(int fd, void *addr);

    /**
     * @brief set fd.
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @param [in] offset offset from start virtual address for ion buffer.
     * @param [in] size size of ion buffer.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool SetFd(int fd, void *addr, uint32_t offset, uint32_t size);

    /**
     * @brief Read image from fd.
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool ReadFromFd(int fd, void *addr);

    /**
     * @brief Read image from fd.
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @param [in] offset offset from start virtual address for ion buffer.
     * @param [in] size size of ion buffer.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool ReadFromFd(int fd, void *addr, uint32_t offset, uint32_t size);

    /**
     * @brief Read image from file.
     * @param [in] fileName image filename (.pgm or .rgb rawfile).
     * @return on Success, true is returned. On failure false is returned.
     */
    bool ReadFromFile(const char *fileName);

    /**
     * @brief Read image from file.
     * @param [in] fileName image filename (yuv rawfile).
     * @param [in] format image file format.
     * @param [in] w image width.
     * @param [in] h image height.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool ReadFromFile(const char *fileName, Format format, int w, int h);

    /**
     * @brief Copy pixel data from given user buffer.
     * @param [in] buffer user provided buffer.
     * @param [in] size number of bytes buffer.
     * @return If succeeds, it returns true. If not, it return false
     */
    bool ReadFromBuffer(void *buffer, size_t size);

    /**
     * @brief Make an image file.
     * @param [in] fileName image filename (.pgm or .rgb rawfile).
     * @return on Success, true is returned. On failure false is returned.
     */
    bool WriteToFile(const char *fileName);

    /**
     * @brief Copy Image data to given user buffer.
     * @param [in] ptr user buffer.
     * @param [in] size buffer size.
     * @return On Success, true is returned. On failure, false is returned.
     */
    bool WriteToBuffer(void *ptr, size_t size);

    /**
     * @brief Set image metadata.
     * @param [in] width image width
     * @param [in] height image height.
     * @param [in] format image format.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool SetMetaData(uint32_t width, uint32_t height, Format format);

    /**
     * @brief Set image real width and height.
     * @param [in] width image width
     * @param [in] height image height.
     */
    bool SetMetaData(uint32_t width, uint32_t height, bool isNeededReallocation = false);

    /**
     * @brief Get the image's pixel data buffer pointer.
     * returns image pixel data buffer pointer.
     */
    void *GetImageBuffer();

    /**
     * @brief Get the image's pixel data buffer pointer for given plane number.
     * returns image pixel data buffer pointer.
     */
    void *GetImagePlaneBuffer(int plane);

    /**
     * @cond
     * internal
     */
    /* Used only internally */
    void DumpImageInfo(void);
    Channel GetChannel(void) { return mChannel; }
    void SetRectChanged(bool changed) { mValidRegion.SetChanged(changed); }
    bool GetRectChanged(void) { return mValidRegion.GetChanged(); }
    /* Used only internally */
    /**
     * @endcond
     */

    /**
     * @brief Create Image class instance with std::shared_ptr
     * @param [in] width image width.
     * @param [in] height image height.
     * @param [in] format image format.
     * @return std::shared_ptr<Image>
     */
    static std::shared_ptr<Image> MakeImage(uint32_t width, uint32_t height, Format format);

    /**
     * @brief Create Image class instance with std::shared_ptr
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @param [in] width image width.
     * @param [in] height image height.
     * @param [in] format image format.
     * @return std::shared_ptr<Image>
     */
    static std::shared_ptr<Image> MakeImage(int fd, void *addr, uint32_t width, uint32_t height, Format format);

    /**
     * @brief Create Image class instance with std::shared_ptr
     * @param [in] fd file descriptor for ion buffer.
     * @param [in] addr virtual address for ion buffer.
     * @param [in] offset offset from start virtual address for ion buffer.
     * @param [in] size size of ion buffer.
     * @param [in] width image width.
     * @param [in] height image height.
     * @param [in] format image format.
     * @return std::shared_ptr<Image>
     */
    static std::shared_ptr<Image>
    MakeImage(int fd, void *addr, uint32_t offset, uint32_t size, uint32_t width, uint32_t height, Format format);

    /**
     * @brief Create Virtual Image class instance with std::shared_ptr
     * @param [in] format image format.
     * @return std::shared_ptr<Image> that is virtual.
     */
    static std::shared_ptr<Image> MakeImage(Format format);

    /**
     * @brief Create Virtual Image class instance with std::shared_ptr
     * @return std::shared_ptr<Image> that is virtual.
     */
    static std::shared_ptr<Image> MakeImage();

    void AllocateMemoryFromFd(void);

  private:
    friend class RingBuffer;
    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;
    bool Initialize(uint32_t width, uint32_t height, Image::Format format, bool malloc=false);
    void SetBufferMetaData(uint32_t plane, size_t channelSize, size_t channel, size_t width, size_t height);
    static std::shared_ptr<Image> CloneMeta(Image &source);
    size_t GetImageSize();
    size_t GetImageSize(int w, int h);

  private:
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mRealWidth;
    uint32_t mRealHeight;
    Format mFormat;
    Rect mValidRegion;
    Channel mChannel;
    static const char *TAG;
};
}  // namespace graph
}  // namespace seva
#endif
