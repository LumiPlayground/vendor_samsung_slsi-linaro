#ifndef __SEVA_UTILS_HPP
#define __SEVA_UTILS_HPP

#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "graph.hpp"
#include "ofi_kernel_desc.h"

/**
 * @file utils.hpp
 * @brief Utils class header file
 */

namespace seva {
namespace utils {
/**
 * @brief represents a Utils object.
 *
 * Used for utility functions like CreateDirectory
 */
class Utils {
  public:
    /**
     * @brief CreateDirectory
     * @param [in] path directory path which is included file name
     */
    static bool CreateDirectory(const char *path);
    /**
     * @brief GetSoC
     */
    static std::string GetSoC();

    static size_t Max(size_t a, size_t b);
    static bool UpdateInputsForKernelArgs(OfiKernelLibraryManager::KernelArgs& kernelArgs, const seva::graph::BufferList &buffers);
    static bool UpdateInputsRoiForKernelArgs(OfiKernelLibraryManager::KernelArgs& kernelArgs, const seva::graph::BufferList &buffers, bool &isSetRoi);
    static size_t CalculateTempBufferSize(seva::graph::Graph &graph);

  private:
    static const char *TAG;
};
}  // namespace utils
}  // namespace seva
#endif
