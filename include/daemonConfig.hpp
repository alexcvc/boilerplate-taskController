/* SPDX-License-Identifier: MIT */
//
// Copyright (c) 2024 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

#pragma once

#include <string>

namespace app {
/**
 * @struct DaemonConfig
 * @brief The DaemonConfig struct contains the configuration for a daemon.
 */
struct DaemonConfig {
  std::string pidFile;     ///< The path of the PID file
  bool isDaemon{false};    ///< Whether the process should run as a daemon
  std::string configFile;  ///< The path of the configuration file
};
}  // namespace app
