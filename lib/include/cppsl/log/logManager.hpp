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

using log_level = spdlog::level::level_enum;
class LogManager;
using LogManagerPtr = std::shared_ptr<LogManager>;

template <typename Factory = LogManager>
auto CreateLoggingManager(std::string_view name) {
  return std::make_shared<Factory>(name);
}

/**
 * @brief Class Log implements logger in  application
 */
class LogManager final {
  std::string m_name{"logman"};             ///< name of log manager
  std::shared_ptr<spdlog::logger> m_logSp;  ///< manager handler

 public:
  enum class Colored { color, black_white };
  enum class OutputLog : bool { err, out };
  enum class Truncate : bool { no, by_open };

  /**
    * default constructor
    * @param name - logging name. It is useful, if you want to serve logger by name instead shared pointer
    */
  LogManager() {
    m_name = "logman";
    m_logSp = std::make_shared<spdlog::logger>(m_name);
  }

  explicit LogManager(std::string_view name) : m_name(name) {
    m_logSp = std::make_shared<spdlog::logger>(m_name);
  }

  // Copy constructor
  LogManager(const LogManager& other) : m_name(other.m_name), m_logSp(other.m_logSp) {}

  // Move constructor
  LogManager(LogManager&& other) noexcept : m_name(std::move(other.m_name)), m_logSp(std::move(other.m_logSp)) {}

  // Copy assignment operator
  LogManager& operator=(const LogManager& other) {
    if (this != &other) {
      m_name = other.m_name;
      m_logSp = other.m_logSp;
    }
    return *this;
  }

  // Move assignment operator
  LogManager& operator=(LogManager&& other) noexcept {
    if (this != &other) {
      m_name = std::move(other.m_name);
      m_logSp = std::move(other.m_logSp);
    }
    return *this;
  }

  /**
    * destructor
    */
  ~LogManager() noexcept {
    removeSinks();
  }

  /**
   * @brief Returns the name of the logging manager.
   * @return const std::string& The name of the logging manager.
   */
  [[nodiscard]] const std::string& name() const {
    return m_name;
  }

  const std::shared_ptr<spdlog::logger>& logPtr() const {
    return m_logSp;
  }

  void setMLogSp(const std::shared_ptr<spdlog::logger>& mLogSp) {
    m_logSp = mLogSp;
  }
  /**
   * @brief Gets the number of sinks in the LogManager.
   * @return The number of sinks in the LogManager, or 0 if the LogManager has no log pointer.
   */
  [[nodiscard]] size_t number_sinks() const {
    if (m_logSp == nullptr)
      return 0;
    return m_logSp->sinks().size();
  }

  /**
   *  @brief Checks if the logging manager is empty
   *  @return TRUE if the logging manager is empty, FALSE otherwise
   */
  [[nodiscard]] bool empty() const {
    if (m_logSp == nullptr)
      return true;
    return m_logSp->sinks().empty();
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
  [[nodiscard]] bool add_basic_file_sink(const std::string& filename, Truncate howToTruncate, log_level level) noexcept;

  /**
    * add rotation file sink.
    * @param filename - target log file name
    * @param max_file_size
    * @param max_files
    * @param level - log level for this sink
    */
  [[nodiscard]] bool add_rotation_file_sink(const std::string& filename, size_t max_file_size, size_t max_files,
                                            log_level level) noexcept;

  /**
    * Create file sink which create new file on the given time (default in midnight)
    * @param filename - target log file name
    * @param hour
    * @param minute
    * @param level - log level for this sink
    */
  [[nodiscard]] bool add_daily_file_sink(const std::string& filename, int hour, int minute, log_level level) noexcept;

  /**
    * Create and register no-/colored console sink
    * @param to_stderr
    * @param colored
    * @param level - log level for this sink
    */
  [[nodiscard]] bool add_console_sink(OutputLog to_stderr, Colored colored, log_level level) noexcept;

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
                                      log_level lev, int port = 514, bool enable_formatting = true,
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
                                     bool enable_formatting, log_level level) noexcept;

  //----------------------------------------------------------
  // Logger startup, shutdown and drop all sinks
  //----------------------------------------------------------
  /**
   * @brief Opens the logger.
   * @return True if the logger was successfully opened, false otherwise.
   */
  [[nodiscard]] bool openLogger(log_level level = spdlog::level::info);
  void push_sink_safe(spdlog::sink_ptr sinkPtr, spdlog::level::level_enum level);

  /**
   * @brief Closes the logger.
   */
  void closeLogger();

  /**
   * @brief Removes all the registered sinks from the log manager.
   */
  void removeSinks();

  /**
    * Interval based flush. This is implemented by a single worker thread that periodically
    * calls flush() on each logger.
    * @param interval_seconds - interval in seconds
    */
  [[maybe_unused]] static void flush_every(std::chrono::seconds& interval_seconds) noexcept {
    spdlog::flush_every(std::chrono::seconds(interval_seconds));
  }

  /**
   * set level only for logger (general and for next sinks default)
   * @param level - logging level
   */
  void set_level(spdlog::level::level_enum level) noexcept {
    if (m_logSp) {
      m_logSp->set_level(level);
    }
  }

  [[nodiscard]] spdlog::level::level_enum level() const noexcept {
    if (m_logSp) {
      return m_logSp->level();
    }
    return spdlog::level::off;
  }

 private:
  void drop_all() noexcept {
    if (m_logSp) {
      spdlog::drop(m_name);
    }
  }

  [[nodiscard]] bool check_create_path(const std::string& filename);

  [[nodiscard]] std::shared_ptr<spdlog::sinks::sink> create_console_sink(OutputLog to_stderr, Colored colored) noexcept;

  [[nodiscard]] bool initialize_console_sink(OutputLog to_stderr, Colored colored, log_level level);

 public:
  //----------------------------------------------------------
  // trace
  //----------------------------------------------------------
  template <typename... Args>
  [[maybe_unused]] inline void trace(fmt::format_string<Args...> fmt, Args&&... args) {
    if (m_logSp) {
      m_logSp->trace(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void trace(const T& msg) {
    if (m_logSp) {
      m_logSp->trace(msg);
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void trace_if(bool flag, fmt::format_string<Args...> fmt, Args&&... args) {
    if (flag) {
      if (m_logSp) {
        m_logSp->trace(fmt, std::forward<Args>(args)...);
      }
    }
  }

  template <typename T>
  [[maybe_unused]] inline void trace_if(bool flag, const T& msg) {
    if (flag) {
      if (m_logSp) {
        m_logSp->trace(msg);
      }
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void trace_name(const std::string& name, fmt::format_string<Args...> fmt, Args&&... args) {
    auto logPtr = spdlog::get(name);
    if (logPtr) {
      logPtr->trace(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void trace_name(const std::string& name, const T& msg) {
    std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
    if (logPtr) {
      // logging to multi-sink logger
      logPtr->trace(msg);
    }
  }

  //----------------------------------------------------------
  // debug
  //----------------------------------------------------------
  template <typename... Args>
  [[maybe_unused]] inline void debug(fmt::format_string<Args...> fmt, Args&&... args) {
    if (m_logSp) {
      m_logSp->debug(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void debug(const T& msg) {
    if (m_logSp) {
      m_logSp->debug(msg);
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void debug_if(bool flag, fmt::format_string<Args...> fmt, Args&&... args) {
    if (flag) {
      if (m_logSp) {
        m_logSp->debug(fmt, std::forward<Args>(args)...);
      }
    }
  }

  template <typename T>
  [[maybe_unused]] inline void debug_if(bool flag, const T& msg) {
    if (flag) {
      if (m_logSp) {
        m_logSp->debug(msg);
      }
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void debug_name(const std::string& name, fmt::format_string<Args...> fmt, Args&&... args) {
    auto logPtr = spdlog::get(name);
    if (logPtr) {
      logPtr->debug(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void debug_name(const std::string& name, const T& msg) {
    std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
    if (logPtr) {
      // logging to multi-sink logger
      logPtr->debug(msg);
    }
  }

  //----------------------------------------------------------
  // info
  //----------------------------------------------------------

  template <typename... Args>
  [[maybe_unused]] inline void info(fmt::format_string<Args...> fmt, Args&&... args) {
    if (m_logSp) {
      m_logSp->info(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void info(const T& msg) {
    if (m_logSp) {
      m_logSp->info(msg);
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void info_if(bool flag, fmt::format_string<Args...> fmt, Args&&... args) {
    if (flag) {
      if (m_logSp) {
        m_logSp->info(fmt, std::forward<Args>(args)...);
      }
    }
  }

  template <typename T>
  [[maybe_unused]] inline void info_if(bool flag, const T& msg) {
    if (flag) {
      if (m_logSp) {
        m_logSp->info(msg);
      }
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void info_name(const std::string& name, fmt::format_string<Args...> fmt, Args&&... args) {
    auto logPtr = spdlog::get(name);
    if (logPtr) {
      logPtr->info(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void info_name(const std::string& name, const T& msg) {
    std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
    if (logPtr) {
      // logging to multi-sink logger
      logPtr->info(msg);
    }
  }

  //----------------------------------------------------------
  // warn
  //----------------------------------------------------------
  template <typename... Args>
  [[maybe_unused]] inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
    if (m_logSp) {
      m_logSp->warn(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void warn(const T& msg) {
    if (m_logSp) {
      m_logSp->warn(msg);
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void warn_if(bool flag, fmt::format_string<Args...> fmt, Args&&... args) {
    if (flag) {
      if (m_logSp) {
        m_logSp->warn(fmt, std::forward<Args>(args)...);
      }
    }
  }

  template <typename T>
  [[maybe_unused]] inline void warn_if(bool flag, const T& msg) {
    if (flag) {
      if (m_logSp) {
        m_logSp->warn(msg);
      }
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void warn_name(const std::string& name, fmt::format_string<Args...> fmt, Args&&... args) {
    auto logPtr = spdlog::get(name);
    if (logPtr) {
      logPtr->warn(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void warn_name(const std::string& name, const T& msg) {
    std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
    if (logPtr) {
      // logging to multi-sink logger
      logPtr->warn(msg);
    }
  }

  //----------------------------------------------------------
  // error
  //----------------------------------------------------------
  template <typename... Args>
  [[maybe_unused]] inline void error(fmt::format_string<Args...> fmt, Args&&... args) {
    if (m_logSp) {
      m_logSp->error(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void error(const T& msg) {
    if (m_logSp) {
      m_logSp->error(msg);
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void error_if(bool flag, fmt::format_string<Args...> fmt, Args&&... args) {
    if (flag) {
      if (m_logSp) {
        m_logSp->error(fmt, std::forward<Args>(args)...);
      }
    }
  }

  template <typename T>
  [[maybe_unused]] inline void error_if(bool flag, const T& msg) {
    if (flag) {
      if (m_logSp) {
        m_logSp->error(msg);
      }
    }
  }

  template <typename... Args>
  [[maybe_unused]] inline void error_name(const std::string& name, fmt::format_string<Args...> fmt, Args&&... args) {
    auto logPtr = spdlog::get(name);
    if (logPtr) {
      logPtr->error(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void error_name(const std::string& name, const T& msg) {
    std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
    if (logPtr) {
      // logging to multi-sink logger
      logPtr->error(msg);
    }
  }

  //----------------------------------------------------------
  // critical
  //----------------------------------------------------------
  template <typename... Args>
  [[maybe_unused]] inline void critical(fmt::format_string<Args...> fmt, Args&&... args) {
    if (m_logSp) {
      m_logSp->critical(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  inline void critical(const T& msg) {
    if (m_logSp) {
      m_logSp->critical(msg);
    }
  }

  template <typename... Args>
  inline void critical_if(bool flag, fmt::format_string<Args...> fmt, Args&&... args) {
    if (flag) {
      if (m_logSp) {
        m_logSp->critical(fmt, std::forward<Args>(args)...);
      }
    }
  }

  template <typename T>
  inline void critical_if(bool flag, const T& msg) {
    if (flag) {
      if (m_logSp) {
        m_logSp->critical(msg);
      }
    }
  }

  template <typename... Args>
  inline void critical_name(const std::string& name, fmt::format_string<Args...> fmt, Args&&... args) {
    auto logPtr = spdlog::get(name);
    if (logPtr) {
      logPtr->critical(fmt, std::forward<Args>(args)...);
    }
  }

  template <typename T>
  [[maybe_unused]] inline void critical_name(const std::string& name, const T& msg) {
    std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
    if (logPtr) {
      // logging to multi-sink logger
      logPtr->critical(msg);
    }
  }
};

}  // namespace cppsl::log
