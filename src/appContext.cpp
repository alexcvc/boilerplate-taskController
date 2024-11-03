/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2024 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

#include "appContext.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <regex>
#include <thread>

#include <fmt/chrono.h>

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, enums, unions, variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Definitions
//----------------------------------------------------------------------------

/*************************************************************************/ /**
 * @brief Validates the path of the error file.
 *
 * This method checks if the specified error file exists. If the file does not exist,
 * it will be created and the error message will be written to it. If the file exists,
 * the method will return true to indicate that the error file was incremented successfully.
 *
 * @param path The path of the error file.
 * @param desc The error message to be written to the file.
 * @return The success status of the operation. Returns true if the error file was incremented successfully
 *         or already exists, false otherwise.
 ******************************************************************************/
bool app::AppContext::ValidatePath(const std::string& path, const std::string& desc) const {
  std::cout << "Application context: Validating path: " << path << std::endl;

  if (!path.empty()) {
    if (!std::filesystem::exists(path)) {
      std::cerr << desc << " \"" << path << "\" doesn't exist" << std::endl;
      return false;
    }
  }
  return true;
}

/*************************************************************************/ /**
 * @brief Validates the configuration of the daemon.
 * @param config The configuration of the daemon to validate.
 * @return An optional boolean value. If the configuration is valid, the method
 * returns an optional that contains the boolean value 'true'.
 * If the configuration is not valid, the method returns an optional that contains
 * the boolean value 'false'.
 * If an error occurs during the validation process, the method returns an empty optional.
 ******************************************************************************/
std::optional<bool> app::AppContext::ValidateConfig(const app::DaemonConfig& config) {
  int errorCount{0};

  std::cout << "Application context: Validating the configuration" << std::endl;

  m_configFile = config.configFile;

  if (!ValidatePath(m_configFile, "Configuration file")) {
    errorCount++;
  }

  if (errorCount > 0)
    return false;

  return true;
}

/*************************************************************************/ /**
* @brief Processes the reconfiguration of the daemon.
*
* This method is responsible for handling the reconfiguration of the
* application. It performs all necessary tasks related to the reconfiguration
* process. The method returns an optional boolean value to indicate the success
* or failure of the reconfiguration.
*
* @return An optional boolean value. If the reconfiguration is successful,
* the method returns an optional that contains the boolean value 'true'.
* If the reconfiguration is not successful, the method returns an optional that
* contains the boolean value 'false'. If an error occurs during the reconfiguration
* process, the method returns an empty optional.
 ******************************************************************************/
std::optional<bool> app::AppContext::ProcessReconfigure() {
  std::cout << "Application context: Reconfiguring the application" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return true;
}

/*************************************************************************/ /**
 * @brief Process the restart of the application context.
 * @return An optional boolean value indicating if the process restart was successful.
 *         Returns std::nullopt if failed, otherwise returns true.
 ******************************************************************************/
std::optional<bool> app::AppContext::ProcessRestart() {
  std ::cout << "Application context: Restarting the application" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return true;
}

/*************************************************************************/ /**
 * @brief Process everything after USER1 signal
 * @return An optional boolean value indicating if the process was successful.
 *         Returns std::nullopt if failed, otherwise returns true.
 ******************************************************************************/
std::optional<bool> app::AppContext::ProcessSignalUser1() {
  std::cout << "Application context: get and process the USER1 signal" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return true;
}

/*************************************************************************/ /**
 * @brief Process everything after USER2 signal
 * @return An optional boolean value indicating if the process was successful.
 *         Returns std::nullopt if failed, otherwise returns true.
 ******************************************************************************/
std::optional<bool> app::AppContext::ProcessSignalUser2() {
  std::cout << "Application context: get and process the USER2 signal" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return true;
}

/*************************************************************************/ /**
 * \brief Process the start of the application context.
 * \return An optional boolean value indicating if the process start was successful.
 *         Returns std::nullopt if failed, otherwise returns true.
 ******************************************************************************/
std::optional<bool> app::AppContext::ProcessStart() {
  std ::cout << "Application context: Start the application" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return true;
}

/*************************************************************************/ /**
 * @brief Performs a graceful shutdown of the application.
 * @return An optional boolean value indicating the success of the shutdown process.
 *         The optional value will be empty if the shutdown process encountered an error.
 ******************************************************************************/
std::optional<bool> app::AppContext::ProcessShutdown() {
  std ::cout << "Application context: Shutting down the application" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return true;
}

/*************************************************************************/ /**
 * @brief processing the context.
 * @param min_duration minimum duration until next processing.
 * @return The earlier timeout until next process.
 ******************************************************************************/
std::chrono::milliseconds app::AppContext::ProcessExecuting(const std::chrono::milliseconds& min_duration) {
  std::cout << "Processing the context. Minimal duration: " << min_duration.count() << " ms" << std::endl;
  return min_duration > std::chrono::milliseconds(5000) ? std::chrono::milliseconds(1000)
                                                        : min_duration + std::chrono::milliseconds(1000);
}
