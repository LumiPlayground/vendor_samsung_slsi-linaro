#ifndef __SEVA_TENSOR_HPP
#define __SEVA_TENSOR_HPP

#include "buffer.hpp"
#include "data.hpp"

/**
 * @file tensor.hpp
 * @brief Tensor class header file
 */

namespace seva {
namespace graph {
/**
 * @brief represents an image object.
 *
 * Can be used as input/output buffer of Node.
 */
class Tensor : public Buffer {
  public:
    Tensor(uint32_t width, uint32_t height, uint32_t channel, Data::Type type);

    Tensor(int fd, void *addr, uint32_t width, uint32_t height, uint32_t channel, Data::Type type);

    Tensor(int fd,
           void *addr,
           uint32_t offset,
           uint32_t size,
           uint32_t width,
           uint32_t height,
           uint32_t channel,
           Data::Type type);

    /**
     * @brief Virtual object constructor.
     */
    Tensor();
    /**
     * @brief Destructor.
     */
    ~Tensor();

    /**
     * @brief Get dimension information.
     * @return Image width in bytes.
     */
    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }
    uint32_t GetChannel() const { return mChannel; }

    Data::Type GetType() const { return mDataType; }

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
     * @brief Read tensor data from file.
     * @param [in] fileName image filename (.pgm or .rgb rawfile).
     * @return on Success, true is returned. On failure false is returned.
     */
    bool ReadFromFile(const char *fileName);

    /**
     * @brief Copy pixel data from given user buffer.
     * @param [in] buffer user provided buffer.
     * @param [in] size number of bytes buffer.
     * @return If succeeds, it returns true. If not, it return false.
     */
    bool ReadFromBuffer(void *buffer, size_t size);

    /**
     * @brief Make an raw file for Tensor data.
     * @param [in] fileName.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool WriteToFile(const char *fileName);

    /**
     * @brief Copy Tensor data to given user buffer.
     * @param [in] ptr user buffer.
     * @param [in] size buffer size.
     * @return On Success, true is returned. On failure false is returned.
     */
    bool WriteToBuffer(void *ptr, size_t size);

    /**
     * @brief Set Tensor metadata.
     * @return on Success, true is returned. On failure false is returned.
     */
    bool SetMetaData(uint32_t width, uint32_t height, uint32_t channel, Data::Type type);

    bool SetMetaData(uint32_t width, uint32_t height, uint32_t channel, bool isNeededReallocation = false);

    static std::shared_ptr<Tensor> MakeTensor(uint32_t width, uint32_t height, uint32_t channel, Data::Type type);

    static std::shared_ptr<Tensor>
    MakeTensor(int fd, void *addr, uint32_t width, uint32_t height, uint32_t channel, Data::Type type);

    static std::shared_ptr<Tensor> MakeTensor(int fd,
                                              void *addr,
                                              uint32_t offset,
                                              uint32_t size,
                                              uint32_t width,
                                              uint32_t height,
                                              uint32_t channel,
                                              Data::Type type);

    static std::shared_ptr<Tensor> MakeTensor();

  private:
    Tensor(const Tensor &) = delete;
    Tensor &operator=(const Tensor &) = delete;
    bool Initialize(uint32_t width, uint32_t height, uint32_t channel, Data::Type type, bool malloc=false);
    void SetBufferMetaData(uint32_t width, uint32_t height, uint32_t channel, bool malloc);
    void AllocateMemory(void);

  private:
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mChannel;
    Data::Type mDataType;
    static const char *TAG;
};
}  // namespace graph
}  // namespace seva
#endif
