/* SPDX-License-Identifier: MIT */

/*************************************************************************/ /**
 * @file
 * @brief  contains base abstract class for loggable objects.
 * @ingroup C++ support library
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//
#include <memory>

#include <cppsl/log/LogManager.hpp>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------
namespace cppsl::log {

/// @brief LogAppender provides an interface for logging
///
/// @details
/// This class acts as a base class for any logging appenders.
/// It holds a pointer to a log appender instance.
class LoggableBase {
 protected:
  /// @brief Constructor
  ///
  /// @param appenderPtr A shared pointer to a log appender
  explicit LoggableBase(cppsl::log::LogManagerPtr appenderPtr) : m_logPtr(appenderPtr) {}

  /// @brief Destructor
  virtual ~LoggableBase() = default;

  /// @brief Pointer to the log appender
  cppsl::log::LogManagerPtr m_logPtr;
};

}  // namespace cppsl::log