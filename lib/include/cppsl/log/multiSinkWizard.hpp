/* SPDX-License-Identifier: MIT */

/*************************************************************************/ /**
 * @file
 * @brief  C++ wrapper class with  multiple loggers all sharing the same sink
 * aka categories
 * @details This class maintains spdlog details as a private inside a class.
 * Other objects can not access to details directly, instead, they can only
 * invoke a list of public functions. This abstracts the spdlog in logger
 * container. One of the advantages is being able to apply the useful functions
 * with no modification.
 * All headers with skins are hidden in the cpp file. This brings significant
 * benefit in compiling and breaking the dependency of all files on spdlog.
 * We can use a logger targets with no confusion with mixing types and extra headers.
 * Each of sinks would have their own output target in any case. This manager contain:
 *  - one sink to console target;
 *  - one sink to file target;
 *  - one sink to daily file target;
 *  - one sink to rotate file target;
 *  - one sink to syslog target;
 *  - one sink to remote syslog target;
 *
 * @author  Alexander Sacharov <alexcvc@gmail.com>
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
// clang-format off
#include <cstdio>
#include <memory>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/fmt/fmt.h>
#include <fmt/core.h>

#include "spdlog/sinks/basic_file_sink.h"
// clang-format on

/**
 * @brief This namespace contains manager logger with skins.
 * C++ wrapper class with  multiple loggers all sharing the same sink aka categories
 */
namespace cppsl::log {

/**
 * @brief Class Log implements logger in  application
 */
class MultiSinkWizard {
  std::string m_name{"spdlog"};  ///< name of log manager
  std::vector<spdlog::sink_ptr> m_sinks;

 public:
  enum class Colored { color, black_white };
  enum class OutputLog : bool { err, out };
  enum class Truncate : bool { no, by_open };

  /**
   * @brief Returns the name of the logging manager.
   * @return const std::string& The name of the logging manager.
   */
  [[nodiscard]] const std::string& name() const {
    return m_name;
  }

  /**
   * @brief set name of logger
   * @param name - name of logger
   */
  void set_name(const std::string& name) {
    m_name = name;
  }

  /**
   * @brief Gets the number of sinks in the LogManager.
   * @return The number of sinks in the LogManager, or 0 if the LogManager has no log pointer.
   */
  [[nodiscard]] size_t size() const {
    return m_sinks.size();
  }

  /**
   *  @brief Checks if the logging manager is empty
   *  @return TRUE if the logging manager is empty, FALSE otherwise
   */
  [[nodiscard]] bool empty() const {
    return m_sinks.empty();
  }

  void set_default() const {
    if (!empty()) {
      // set multi-sink logger as default logger
      spdlog::set_default_logger(std::make_shared<spdlog::logger>(m_name, m_sinks.begin(), m_sinks.end()));
    }
  }

  //----------------------------------------------------------
  // Add various sinks with different parameters and levels
  //----------------------------------------------------------

  /**
   * add basic file sink + multi sinks.
   * @param filename - name of logfile
   * @param howToTruncate - if true - truncate by open
   * @param level - log level for this sink
   */
  [[nodiscard]] bool add_basic_file_sink(const std::string& filename, Truncate howToTruncate,
                                         spdlog::level::level_enum level) noexcept;

  /**
    * add rotation file sink.
    * @param filename - target log file name
    * @param max_file_size
    * @param max_files
    * @param level - log level for this sink
    */
  [[nodiscard]] bool add_rotation_file_sink(const std::string& filename, size_t max_file_size, size_t max_files,
                                            spdlog::level::level_enum level) noexcept;

  /**
    * Create file sink which create new file on the given time (default in midnight)
    * @param filename - target log file name
    * @param hour
    * @param minute
    * @param level - log level for this sink
    */
  [[nodiscard]] bool add_daily_file_sink(const std::string& filename, int hour, int minute,
                                         spdlog::level::level_enum level) noexcept;

  /**
    * Create and register no-/colored console sink
    * @param output
    * @param colored
    * @param level - log level for this sink
    */
  void add_console_sink(OutputLog output, Colored colored, spdlog::level::level_enum level) noexcept;

  /**
    * Create and register a rsyslog sinks to remote server
    * @param ident - The string pointed to by ident is prepended to every message.
    * @param rsyslog_ip - remote syslog server IP
    * @param syslog_facility - facility codes specifies what type of program is logging the message.
    * @param severity - severity of message
    * More information in https://man7.org/linux/man-pages/man3/syslog.3.html
    * @param lev - level of message
    * @param port - remote port of server
    * @param enable_formatting - default true. However the message format maybe changed.
    * @param log_buffer_max_size - buffer reserved in the message string. This increases performance when creating
    * log messages.
    */
  [[nodiscard]] bool add_rsyslog_sink(const std::string& ident, const std::string& rsyslog_ip, int syslog_facility,
                                      spdlog::level::level_enum lev, int port = 514, bool enable_formatting = true,
                                      int log_buffer_max_size = 1024 * 1024 * 16) noexcept;

  /**
    * Create and register a syslog sinks
    * @param syslog_ident- The string pointed to by ident is prepended to every message.
    * @param syslog_option - options. More information in https://man7.org/linux/man-pages/man3/syslog.3.html
    * @param syslog_facility - facility codes specifies what type of program is logging the message.
    * @param enable_formatting - default true. However the message format maybe changed.
    * @param level - log level for this sink
    */
  [[nodiscard]] bool add_syslog_sink(const std::string& syslog_ident, int syslog_option, int syslog_facility,
                                     bool enable_formatting, spdlog::level::level_enum level) noexcept;

  //----------------------------------------------------------
  // Logger startup, shutdown and drop all sinks
  //----------------------------------------------------------
  void push_sink_safe(spdlog::sink_ptr sinkPtr, spdlog::level::level_enum level);

 private:
  [[nodiscard]] bool check_create_path(const std::string& filename);
};

}  // namespace cppsl::log
