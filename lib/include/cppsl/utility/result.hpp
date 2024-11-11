/* SPDX-License-Identifier: MIT */

/*************************************************************************/ /**
 * \file
 * @brief  contains result templates useful in new projects.
 * @ingroup C++ support library
 *****************************************************************************/

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <optional>
#include <variant>

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl {

/**
 * @class ResultOptVal
 * @brief A class that represents a result using std::optional.
 *
 * The ResultOptVal class provides methods for checking if the result is valid or invalid,
 * retrieving the result value, and getting a default value if no result is present.
 *
 * It is important to note that this class does not inherit from a standard library class to avoid
 * incorrect behavior.
 * It is marked as final so that it cannot be used as a base class. Instead, it contains an std::optional
 * member for storing the result value.
 *
 * @tparam T The type of the result value.
 */
template <typename T>
class ResultOptVal final {
 public:
  /**
   * @brief Constructor to initialize with a result value.
   *
   * @param t The result value to initialize with.
   */
  constexpr explicit ResultOptVal(const T& t) noexcept : opt_result_value_{t} {}

  /**
   * @brief Default constructor.
   */
  explicit constexpr ResultOptVal() noexcept = default;

  /**
   * @brief Check if the result value is valid.
   *
   * @return true if the result value is valid, false otherwise.
   */
  [[nodiscard]] constexpr bool IsValid() const noexcept {
    return opt_result_value_.has_value();
  }

  /**
   * @brief Check if the result value is invalid.
   *
   * @return true if the result value is invalid, false otherwise.
   */
  [[nodiscard]] constexpr bool IsInvalid() const noexcept {
    return !IsValid();
  }

  /**
   * @brief Retrieves the result value.
   *
   * @return The result value.
   */
  [[nodiscard]] constexpr T GetValue() const {
    return opt_result_value_.value();
  }

  /**
   * @brief Retrieves the result value or a default value if no result is present.
   *
   * @return The result value, or a default-constructed value of type `T` if no result is present.
   */
  [[nodiscard]] T GetValueOrDefault() const noexcept {
    return opt_result_value_.value_or(T());
  }

 private:
  std::optional<T> opt_result_value_;
};
;

/**
 * @class ResultVarVal
 * @brief A class that represents a result variable value.
 *
 * The ResultVarVal class is a template class that represents a result variable value. It is derived from std::variant<std::monostate, T>, where T is the type of the value.
 *
 * The ResultVarVal class provides constructors and member functions to determine the validity of the value, retrieve the value, and check if the object holds a valid value.
 *
 * Example Usage:
 *
 * @code
 *
 * // Create a ResultVarVal object with an integer value
 * ResultVarVal<int> result(10);
 *
 * // Check if the result is valid
 * if(result.valid()) {
 *     ...
 * }
 *
 * // Retrieve the value
 * int value = result.get();
 *
 * @endcode
 *
 * @tparam T The type of the value
 */
template <typename T>
struct ResultVarVal : protected std::variant<std::monostate, T> {
  explicit constexpr ResultVarVal() noexcept = default;

  constexpr explicit ResultVarVal(const T& t) noexcept : std::variant<std::monostate, T>{t} {}

  /**
   * @brief Checks if the ResultVarVal object holds a valid value.
   * @return Returns true if the ResultVarVal object holds a valid value, false otherwise.
   */
  [[nodiscard]] constexpr bool valid() const noexcept {
    return holds_value();
  }

  /**
    * @brief Checks if the ResultVarVal object is valid.
    * @return True if the object is invalid, false otherwise.
    */
  [[nodiscard]] constexpr bool invalid() const noexcept {
    return !holds_value();
  }

  /**
    * Retrieves the value of the ResultVarVal object.
    * @tparam T The type of the value
    * @return The stored value if valid, otherwise a default-constructed value of type T
    */
  constexpr auto get() const noexcept -> T {
    return (holds_value() ? std::get<T>(*this) : T());
  }

 private:
  [[nodiscard]] constexpr bool holds_value() const noexcept {
    return std::holds_alternative<T>(*this);
  }
};

/**
 * @class ResultOptional
 *
 * @brief Result based on std::optional
 *
 * The ResultOptional class is a templated class that is based on std::optional. It provides a result type that either contains
 * a valid value of type T or is empty. This class is designed to be a wrapper around std::optional and adds some
 * convenience methods for checking the validity of the result and accessing the value.
 *
 * Usage:
 * ```
 * ResultOptional<int> result(42);
 * if (result.valid()) {
 *     int value = result.get();
 *     // Do something with value
 * } else {
 *     // Handle error case
 * }
 * ```
 *
 * @tparam T The type of the value contained in the result
 */
template <typename T>
struct ResultOptional : protected std::optional<T> {
  explicit constexpr ResultOptional() noexcept = default;

  constexpr explicit ResultOptional(T const& t) noexcept : std::optional<T>{t} {}

  [[nodiscard]] constexpr bool isValid() const noexcept {
    return hasValue();
  }

  [[nodiscard]] constexpr bool isInvalid() const noexcept {
    return !isValid();
  }

  [[nodiscard]] constexpr auto value() const noexcept -> T {
    return std::optional<T>::value_or(T());
  }

 private:
  [[nodiscard]] constexpr bool hasValue() const noexcept {
    return std::optional<T>::has_value();
  }
};
}  // namespace cppsl
