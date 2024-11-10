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
    BE,  // swap for big endian machines
    LE,  // swap for little endian machines
    AX,  // swap always, for f... strange float cases
    NX   // swap never, for f... strange float cases
  };

  /**
   * @brief A class for swapping bytes based on the endianness
   *
   * This class provides a static method for swapping bytes in a value based on the endianness of the machine,
   * as well as an enum class for specifying the swap type.
   */
  template <typename T>
  static T Swap(T val, SwapType swapType) {
    if (swapType == SwapType::NX) {
      return val;
    }

    T ret = val;
    auto ptr = reinterpret_cast<uint8_t*>(&ret);
    const auto size = sizeof(T);

    if constexpr (std::endian::native == std::endian::big) {
      if (swapType == SwapType::BE || swapType == SwapType::AX)
        swapBytes(ptr, size);
    } else if constexpr (std::endian::native == std::endian::little) {
      if (swapType == SwapType::LE || swapType == SwapType::AX)
        swapBytes(ptr, size);
    }

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
