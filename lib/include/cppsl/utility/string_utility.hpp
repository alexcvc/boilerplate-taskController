/* SPDX-License-Identifier: MIT */

/*************************************************************************/ /**
 * \file
 * @brief  contains other util-functions.
 * @ingroup C++ support library
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl::utility {

/**
 * @brief Converts a hexadecimal character to its corresponding byte value.
 * @param input The hexadecimal character to be converted.
 * @return The byte value of the input hexadecimal character.
 * @throws std::range_error If the input character is not a valid hexadecimal character.
 *
 * @example
 * ```cpp
 * char hexChar = 'A';
 * std::uint8_t byte = HexToByte(hexChar);
 * // byte is now 10
 * ```
 */
[[maybe_unused]] constexpr std::uint8_t HexToByte(char input) {
  if (input >= '0' && input <= '9') {
    return static_cast<std::uint8_t>(input - '0');
  }
  if (input >= 'A' && input <= 'F') {
    return static_cast<std::uint8_t>(input - 'A' + 10);
  }
  if (input >= 'a' && input <= 'f') {
    return static_cast<std::uint8_t>(input - 'a' + 10);
  }
  throw std::range_error(std::string("cannot convert HEX character ") + std::to_string(input));
}

/**
 * @brief Converts two hexadecimal characters to a single byte value.
 * @param High The character representing the high nibble of the byte.
 * @param Low The character representing the low nibble of the byte.
 * @return The combined byte value of the high and low nibbles.
 * @throws std::range_error If either input character is not a valid hexadecimal character.
 *
 * @example
 * ```cpp
 * char highNibble = 'A';
 * char lowNibble = '7';
 * std::uint8_t byte = HexCharsToByte(highNibble, lowNibble);
 * // byte is now 167
 * ```
 */
[[maybe_unused]] constexpr std::uint8_t HexCharsToByte(char High, char Low) {
  return (HexToByte(High) << 4) | (HexToByte(Low));
}

/**
 * @brief Converts a hexadecimal string to a sequence of bytes.
 *
 * This function takes a null-terminated hexadecimal string and converts it to a sequence of bytes.
 * Each pair of consecutive characters in the string represents a single byte in hexadecimal format.
 * The resulting byte sequence is stored in an output array of type TypeOutput.
 * The length of the output array should be at least half the length of the input string,
 * to accommodate the conversion of each pair of characters into a byte.
 *
 * @tparam TypeOutput The type of the output array. It must be an array type.
 * @tparam Length The length of the input string, including the null terminator.
 * @tparam Index A parameter pack of compile-time indices used for expanding the function parameter pack.
 * @param Input The null-terminated input hexadecimal string.
 * @param seq A std::index_sequence used for expanding the function parameter pack.
 * @return The resulting byte sequence stored in the output array.
 *
 * @example
 * ```
 * const char* hexString = "48656C6C6F"; // "Hello" in hexadecimal
 * std::array<std::uint8_t, 5> bytes = HexStringToBytes<std::array<std::uint8_t, 5>>(hexString, std::make_index_sequence<10>{});
 * // bytes is now {72, 101, 108, 108, 111}
 * ```
 *
 * @remark The input string must consist only of valid hexadecimal characters (0-9, A-F, a-f).
 * If any input character is not a valid hexadecimal character, a std::range_error exception is thrown.
 * The output array must have enough capacity to store the resulting byte sequence.
 *
 * @see HexCharsToByte
 * @see HexToByte
 */
template <typename TypeOutput, std::size_t Length, std::size_t... Index>
constexpr TypeOutput HexStringToBytes(const char (&Input)[Length], const std::index_sequence<Index...>&) {
  return TypeOutput{HexCharsToByte(Input[(Index * 2)], Input[((Index * 2) + 1)])...};
}

/**
 * @brief Converts a hex string to byte representation.
 *
 * This function converts a null-terminated hex string to a byte representation. The byte representation is of a specified output type (`TypeOutput`).
 * The length of the input string (`Length`) must be an even number.
 *
 * @tparam TypeOutput The output type of the byte representation.
 * @tparam Length The length of the input string.
 * @param Input The null-terminated hex string to convert.
 * @return The byte representation of the hex string.
 */
template <typename TypeOutput, std::size_t Length>
constexpr TypeOutput HexStringToBytes(const char (&Input)[Length]) {
  return HexStringToBytes<TypeOutput>(Input, std::make_index_sequence<(Length / 2)>{});
}

/**
 * @brief Checks if a value is within a specified range.
 *
 * This template function checks if the given value is within the specified range.
 * The range is defined by the lower bound (low) and the upper bound (high) values.
 * The function returns true if the value is within the range, and false otherwise.
 *
 * @tparam T The type of the value and the range bounds.
 * @param value The value to check if it is within the range.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return True if the value is within the range, false otherwise.
 */
template <typename T>
[[maybe_unused]] bool IsInRange(const T& value, const T& low, const T& high) noexcept {
  return value >= low && high >= value;
}

/**
 * @brief Checks if a value is within a specified range using comparator function
 *
 * This function compares a given value with a low and high limit using a provided comparator.
 * It returns true if the value is not less than the low limit and is less than the high limit according to the comparator,
 * otherwise it returns false.
 *
 * @tparam T The type of the value being checked.
 * @tparam R The type of the low and high limits.
 * @tparam Comparator The type of the comparator function.
 * @param value The value being checked.
 * @param low The lower limit of the range.
 * @param high The upper limit of the range.
 * @param comp The comparator function used to compare the value with the limits.
 * @return Returns true if the value is within the range, false otherwise.
 */
template <typename T, typename R, typename Comparator>
[[maybe_unused]] bool IsInRange(const T& value, const R& low, const R& high, Comparator comp) {
  return !comp(value, low) && comp(value, high);
}

/**
 * Converts a vector of unsigned characters to a string.
 *
 * The function takes a vector of unsigned characters as input and converts it to a string by
 * mapping each character to a corresponding character in the string. The resulting string is
 * then returned.
 *
 * @param bytes A vector containing the unsigned characters to convert to a string.
 * @return The string representation of the input vector of unsigned characters.
 */
[[maybe_unused]] static std::string ByteVectorToString(const std::vector<unsigned char>& bytes) {
  std::string str;
  std::transform(std::begin(bytes), std::end(bytes), std::back_inserter(str), [](uint8_t c) {
    return char(c);
  });
  return str;
}

/**
 * @brief Compares two strings case-insensitively.
 *
 * This function compares two strings case-insensitively by converting
 * each character to uppercase using std::toupper, and then comparing
 * the resulting characters. If the sizes of the strings are different,
 * the function returns false immediately. Otherwise, it compares each
 * character of the strings one by one, stopping the comparison and
 * returning false if any characters are not equal in the case-insensitive
 * sense. If all characters are equal, the function returns true.
 *
 * @param str1 The first string to be compared.
 * @param str2 The second string to be compared.
 * @return True if the strings are case-insensitively equal, false otherwise.
 *
 * @note The function treats Unicode case-folding equivalences as equal.
 * @note The function is case-insensitive, but it assumes that the strings
 * contain only ASCII characters. If the strings contain non-ASCII
 * characters, the behavior is undefined.
 * @note The function is marked as [[maybe_unused]] to suppress compiler
 * warnings about unused functions.
 */
[[maybe_unused]] static bool StrCmpNoCase(const std::string& str1, const std::string& str2) {
  if (str1.size() != str2.size()) {
    return false;
  }
  for (size_t i = 0; i < str1.size(); ++i) {
    if (std::toupper(str1[i]) != std::toupper(str2[i])) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Safely compares two null-terminated C-style strings.
 *
 * @param str1 First string pointer.
 * @param str2 Second string pointer.
 * @return An integer less than, equal to, or greater than zero if str1 is found,
 *         respectively, to be less than, to match, or be greater than str2.
 */
template <typename T>
[[maybe_unused]] int StrCmpCSafe(T* str1, T* str2) noexcept {
  if (str1 == nullptr && str2 == nullptr) {
    return 0;
  } else if (str1 == nullptr) {
    return -1;
  } else if (str2 == nullptr) {
    return 1;
  } else {
    return std::strcmp(str1, str2);
  }
}

/**
 * @brief Copies a C-style string and returns a unique_ptr to the copy.
 *
 * This function takes a C-style string, `str`, as input and creates a copy of it. The copy is then
 * stored in a unique_ptr and returned. If the input string is null, a null unique_ptr is returned.
 *
 * @tparam CharT The character type of the string.
 * @param str The input C-style string to be copied.
 * @return std::unique_ptr<CharT[]> A unique_ptr pointing to the copy of the input string.
 *
 * @remarks The function assumes that the input string is null-terminated.
 *          The resulting copy will also be null-terminated.
 *          The function allocates memory dynamically, so the caller is responsible for ensuring
 *          proper deallocation of the memory when it is no longer needed.
 *          If the input string is null, a null unique_ptr will be returned.
 *
 * @see std::unique_ptr
 * @see std::char_traits
 * @see std::copy
 */
template <typename CharT>
[[maybe_unused]] std::unique_ptr<CharT[]> StrDupSafeUnique(const CharT* str) {
  if (!str) {
    return nullptr;  // Handle null string input
  }

  size_t len = std::char_traits<CharT>::length(str);
  std::unique_ptr<CharT[]> copy(new CharT[len + 1]);

  std::copy(str, str + len, copy.get());
  copy[len] = CharT();  // Null-terminate the new string

  return copy;
}

/**
 * @brief Safely duplicates a string.
 *
 * This function duplicates a character string by allocating memory for a new string and copying the contents of the
 * input string into it. It handles null pointer input and memory allocation failure by returning nullptr. The new
 * string is null-terminated.
 *
 * @tparam CharT typename of the input string characters.
 * @param str The input string to be duplicated.
 * @return CharT* A pointer to the duplicated string. It is the responsibility of the caller to free the allocated memory.
 *
 * @note The function uses the `std::char_traits<CharT>::length` function to calculate the length of the input string.
 * @note The function uses the `std::copy` function to copy the contents of the input string into the new string.
 * @note The new string is null-terminated by assigning a null character (`CharT()`) at the end.
 * @note If the input string is null, the function returns nullptr.
 * @note If memory allocation fails, the function returns nullptr.
 */
template <typename CharT>
[[maybe_unused]] CharT* StrDupSafe(const CharT* str) noexcept {
  if (!str) {
    return nullptr;  // Handle null string input
  }

  size_t len = std::char_traits<CharT>::length(str);
  size_t totalLen = len + 1;

  try {
    auto copiedStr = new CharT[totalLen];
    std::copy(str, str + len, copiedStr);
    copiedStr[len] = CharT();  // Null-terminate the new string
    return copiedStr;
  } catch (const std::bad_alloc&) {
    return nullptr;  // Handle memory allocation failure
  }
}

/**
 * @brief Trims leading whitespaces from the input string.
 * @param str The input string from which leading whitespaces should be removed.
 * @return A new string with leading whitespaces removed.
 */
[[nodiscard, maybe_unused]] static std::string TrimLeadingWhitespaces(const std::string& str) {
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
    return std::isspace(ch);
  });
  return {start, str.end()};
}

/**
 * @brief Trims trailing whitespaces from the given string.
 * @param str The input string to be trimmed.
 * @return A new string with trailing whitespaces removed.
 */
[[nodiscard, maybe_unused]] static std::string TrimTrailingWhitespaces(const std::string& str) {
  auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
               return !std::isspace(ch);
             }).base();
  return {str.begin(), end};
}

/**
 * @brief Trims leading and trailing whitespaces from a string.
 * @param str The input string that needs to be trimmed.
 * @return A new string with leading and trailing whitespaces removed.
 */
[[nodiscard, maybe_unused]] static std::string TrimWhitespace(const std::string& str) {
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
    return std::isspace(ch);
  });

  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
               return std::isspace(ch);
             }).base();

  return (start < end) ? std::string(start, end) : std::string();
}

/**
 * Tokenizes the given string using the specified delimiters.
 *
 * @param str The input string to be tokenized.
 * @return A vector of tokens extracted from the input string.
 * @example
 * std::string sAddr=
 *    "$CHK:SameVal:Off=3:BIT=5:VAL=0; $CHK:InBand:Off=104:INT8:MIN=-64:MAX=64:RISE=2:LOWER=1; "
 *    "$ACK:AckAny:Off=104:INT8:LATCH:VAL=0; $ERR:SameVal:Off=3:BIT=4:VAL=1}";
 *     std::vector<std::string> tokens = SplitString(input, "; \t\n\r");
 *
 * // Print the tokens
 * for (const auto& token : tokens) {
 *    std::cout << "[" << token << "]" << std::endl;
 * }
 *
 * Output:
 *  [$CHK:SameVal:Off=3:BIT=7:VAL=1]
 *  [$CHK:SameVal:Off=3:BIT=5:VAL=0]
 *  [$ACK:AckAny:Off=104:INT8:LATCH:VAL=0]
 *  [$ERR:SameVal:Off=3:BIT=4:VAL=1]
 */
[[nodiscard, maybe_unused]] static std::vector<std::string> SplitString(const std::string& str,
                                                                        const std::string& delimiters) {
  std::vector<std::string> tokens;
  size_t start = 0;
  size_t end = 0;

  while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
    if (end != start) {
      tokens.push_back(str.substr(start, end - start));
    }
    start = end + 1;
  }

  if (start < str.size()) {
    tokens.push_back(str.substr(start));
  }

  return tokens;
}

}  // namespace cppsl::utility
