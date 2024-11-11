/* SPDX-License-Identifier: MIT */

/*************************************************************************/ /**
 * \file
 * @brief  contains utilities, functions, templates
 * @ingroup C++ support library
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <bit>
#include <cstddef>
#include <cstdint>

//----------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------

namespace cppsl::utility {

/**
 * @class bytesSwap
 * @brief A class for swapping bytes based on the endianness
 *
 * This class provides a static method for swapping bytes in a value based on the endianness of the machine,
 * as well as an enum class for specifying the swap type.
 */
class bytesSwap {
 public:
  enum class SwapType {
    BE,  ///< Swap bytes on big-endian machines to maintain compatibility
    LE,  ///< Swap bytes on little-endian machines to maintain compatibility
    AX,  ///< Always swap bytes regardless of platform endian
    NX   ///< Never swap bytes, preserve original byte order
  };

  /**
  * @brief Swaps bytes of a value based on endianness requirements
  * @tparam T Trivially copyable type (integral, floating-point, or standard layout)
  * @param val Value to swap
  * @param swapType Endianness conversion type
  * @return Byte-swapped value if required by SwapType
  * @throws std::invalid_argument if T is not suitable for byte swapping
  */
  template static std::enable_if_t, T > static T Swap(T val, SwapType swapType) {
    static_assert(std::is_trivially_copyable_v, "T must be trivially copyable for byte swapping");

    if (swapType == SwapType::NX) {
      return val;
    }

    T ret = val;
    // Ensure properly aligned access
    std::aligned_storage_t storage;
    std::memcpy(&storage, &ret, sizeof(T));
    auto ptr = reinterpret_cast(&storage);
    const auto size = sizeof(T);

    if constexpr (std::endian::native == std::endian::big) {
      if (swapType == SwapType::BE || swapType == SwapType::AX)
        swapBytes(ptr, size);
    } else if constexpr (std::endian::native == std::endian::little) {
      if (swapType == SwapType::LE || swapType == SwapType::AX)
        swapBytes(ptr, size);
    }

    std::memcpy(&ret, &storage, sizeof(T));
    return ret;
  }

  /**
   * @fn swapBytes
   * @brief A helper method to swap bytes
   * Swaps the bytes in the given buffer.
   *
   * This function swaps the bytes in the given buffer of the specified size.
   *
   * @param ptr The pointer to the buffer of bytes to be swapped.
   * @param size The size of the buffer in bytes.
   * @return None.
   */
  static void swapBytes(uint8_t* ptr, size_t size) {
    for (size_t i = 0; i < size / 2; ++i) {
      std::swap(ptr[i], ptr[size - i - 1]);
    }
  }
};

}  // namespace cppsl::utility
