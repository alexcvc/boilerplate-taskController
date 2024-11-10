/*************************************************************************/ /**
 * @file
 * @brief  C++ wrapper class with  multiple loggers all sharing the same sink
 * aka categories
 *****************************************************************************/

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

// clang-format off
#include <cppsl/log/logManager.hpp>

#include <iostream>
#include <filesystem>

#include <spdlog/common.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/syslog_sink.h>

#include <cppsl/log/details/fallback_sink.hpp>
#include <cppsl/log/details/rsyslog_sink.hpp>
#include <fmt/format.h>
// clang-format on

namespace cppsl::log {

static const char* LOG_MANAGER_INITIALIZATION_FAILED = "Logging manager initialization failed";

/*************************************************************************/ /**
 * open logger with all added sinks
 * @param level default logging level if not set in sink
 * @return true if successfully, otherwise - false
 *****************************************************************************/
bool LogManager::openLogger(spdlog::level::level_enum level) {
  try {
    if (!m_logSp)
      throw spdlog::spdlog_ex("no logging manager instance created");

    m_logSp->set_level(level);

    // or you can even set multi_sink logger as default logger
    if (m_logSp->sinks().empty()) {
      if (!add_console_sink(OutputLog::err, Colored::color, level)) {
        throw spdlog::spdlog_ex("cannot add console sink");
      }
    }
    // register logger
    spdlog::register_logger(m_logSp);
    return true;

  } catch (const spdlog::spdlog_ex& ex) {
    std::cout << "Logging manager open failed: " << ex.what() << std::endl;
    return false;
  } catch (...) {
    std::cout << "Logging manager open unexpected failed" << std::endl;
    return false;
  }
}

/**
 * @brief Closes the logger by removing all sinks and dropping the logger.
 *
 * This method removes all sinks added to the logger and drops the logger. The logger can no longer be used after this method is called.
 *
 * @see removeSinks()
 * @see spdlog::drop()
 */
void LogManager::closeLogger() {
  removeSinks();
  spdlog::drop(m_name);
}

/**
 * Removes all sinks from the logger.
 *
 * This method clears all the sinks from the logger.
 * After calling this method, there will be no sinks connected to the logger, and all log messages will be discarded.
 */
void LogManager::removeSinks() {
  m_logSp->sinks().clear();
}

/**
 * Adds a basic file sink to the logger.
 *
 * This method adds a basic file sink to the logger. A basic file sink is used to
 * redirect log messages to a file.
 *
 * @param filename The name of the file to which the log messages should be written.
 * @param truncate Determines whether the file should be truncated if it already exists.
 * @param level The default logging level if not set in the sink.
 * @return True if the basic file sink was added successfully, false otherwise.
 *
 * @exception spdlog::spdlog_ex Thrown when the creation of the basic file sink fails.
 * @exception std::runtime_error Thrown when checking or creating the path for the file fails.
 */
bool LogManager::add_basic_file_sink(const std::string& filename, Truncate truncate,
                                     spdlog::level::level_enum level) noexcept {
  try {
    // create path
    if (!check_create_path(filename)) {
      throw std::runtime_error(fmt::format("cannot check or create path for: {}", filename));
    }
    auto sinkPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate == Truncate::by_open);
    push_sink_safe(sinkPtr, level);
    return true;
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << ex.what() << std::endl;
    return false;
  } catch (const std::runtime_error& ex) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << ex.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << std::endl;
    return false;
  }
}

/**
 * Adds a rotating file sink to the logger.
 *
 * This method adds a rotating file sink to the logger. A rotating file sink is used to
 * redirect log messages to a file that rotates when it reaches a maximum file size.
 *
 * @param filename The name of the file to log to.
 * @param max_file_size The maximum size of the log file before it rotates.
 * @param max_files The maximum number of rotated log files to keep.
 * @param level The default logging level if not set in the sink.
 * @return True if the rotating file sink was added successfully, false otherwise.
 *
 * @exception std::runtime_error Thrown when the path for the log file cannot be checked or created.
 * @exception spdlog::spdlog_ex Thrown when the creation of the rotating file sink fails.
 */
bool LogManager::add_rotation_file_sink(const std::string& filename, size_t max_file_size, size_t max_files,
                                        spdlog::level::level_enum level) noexcept {
  try {
    if (!check_create_path(filename)) {
      throw std::runtime_error(fmt::format("cannot check or create path for: {}", filename));
    }
    push_sink_safe(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, max_file_size, max_files), level);
    return true;
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << ex.what() << std::endl;
    return false;
  }
}

/**
 * Adds a daily file sink to the logger.
 *
 * This method adds a sink that writes log messages to a daily log file. The log file is rotated
 * daily at the specified hour and minute.
 *
 * @param filename The name of the log file.
 * @param hour The hour at which rotation occurs (0-23).
 * @param minute The minute at which rotation occurs (0-59).
 * @param level The default logging level if not set in the sink.
 * @return true if the daily file sink was added successfully, false otherwise.
 *
 * @exception std::runtime_error Thrown when checking or creating the path for the log file fails.
 * @exception spdlog::spdlog_ex Thrown when the creation of the daily file sink fails.
 */
bool LogManager::add_daily_file_sink(const std::string& filename, int hour, int minute,
                                     spdlog::level::level_enum level) noexcept {
  try {
    // create path
    if (!check_create_path(filename)) {
      throw std::runtime_error(fmt::format("cannot check or create path for: {}", filename));
    }
    push_sink_safe(std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, hour, minute), level);
    return true;
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << ex.what() << std::endl;
    return false;
  }
}

/*************************************************************************/ /**
 * @brief Add a console sink to the logger.
 *
 * This method adds a console sink to the logger. A console sink is used to
 * redirect log messages to the console.
 *
 * @param to_stderr Determines whether the log messages should be redirected to stderr.
 * @param colored Determines whether the log messages should be displayed in color.
 * @param level The default logging level if not set in the sink.
 * @return True if the console sink was added successfully, false otherwise.
 *
 * @exception spdlog::spdlog_ex Thrown when the creation of the console sink fails.
 *****************************************************************************/
bool LogManager::add_console_sink(OutputLog to_stderr, Colored colored, spdlog::level::level_enum level) noexcept {
  if (!initialize_console_sink(to_stderr, colored, level)) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << std::endl;
    return false;
  }
  return true;
}

/**
 * Initializes a console sink for logging.
 *
 * @param level The default logging level if not set in the sink.
 * @return True if the console sink was successfully initialized, otherwise returns false.
 */
bool LogManager::initialize_console_sink(OutputLog output, Colored colored_output, spdlog::level::level_enum level) {
  try {
    auto sinkPtr = create_console_sink(output, colored_output);
    push_sink_safe(sinkPtr, level);
    return true;
  } catch (const spdlog::spdlog_ex& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

/**
 * Creates a console sink for logging.
 *
 * @param level the logging level for the console sink
 *
 * @return true if the console sink is successfully created, false otherwise
 */
std::shared_ptr<spdlog::sinks::sink> LogManager::create_console_sink(OutputLog output,
                                                                     Colored colored_output) noexcept {
  if (colored_output == Colored::color) {
    if (output == OutputLog::err) {
      return std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    } else {
      return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }
  } else {
    if (output == OutputLog::err) {
      return std::make_shared<spdlog::sinks::stderr_sink_mt>();
    } else {
      return std::make_shared<spdlog::sinks::stdout_sink_mt>();
    }
  }
}

/**
 * Add syslog sink to the logger
 * @param level default logging level if not set in syslog sink
 * @return true if syslog sink is successfully added, otherwise - false
 */
bool LogManager::add_syslog_sink(const std::string& syslog_ident, int syslog_option, int syslog_facility,
                                 bool enable_formatting, log_level level) noexcept {
  try {
    auto sinkPtr = std::make_shared<spdlog::sinks::syslog_sink_mt>(syslog_ident, syslog_option, syslog_facility,
                                                                   enable_formatting);
    sinkPtr->set_level(level);
    push_sink_safe(sinkPtr, level);
    return true;

  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << ex.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << std::endl;
    return false;
  }
}

/**
 * Adds a new rsyslog sink to the logger
 * @param address The address of the rsyslog server
 * @param port The port number of the rsyslog server
 * @return true if the rsyslog sink was added successfully, false otherwise
 *****************************************************************************/
bool LogManager::add_rsyslog_sink(const std::string& ident, const std::string& rsyslog_ip, int syslog_facility,
                                  spdlog::level::level_enum lev, int port, bool enable_formatting,
                                  int log_buffer_max_size) noexcept {
  try {
    auto sinkPtr = std::make_shared<spdlog::sinks::rsyslog_sink_mt>(ident, rsyslog_ip, syslog_facility,
                                                                    log_buffer_max_size, port, enable_formatting);
    sinkPtr->set_pattern("[%Y-%m-%d %H:%M:%S:%e] [%n] [%l] [%P] %@ : %v");
    push_sink_safe(sinkPtr, lev);
    return true;
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << ex.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << LOG_MANAGER_INITIALIZATION_FAILED << ": " << std::endl;
    return false;
  }
}

/**
 * Checks if the specified path exists and creates it if it doesn't.
 *
 * @param path The path to check and create.
 * @return <code>true</code> if the path exists or is successfully created, <code>false</code> otherwise.
 */
bool LogManager::check_create_path(const std::string& filename) {
  // check parent folder
  auto folder = std::filesystem::path(filename).parent_path();
  if (folder.empty())
    folder.assign("./");
  else if (!std::filesystem::exists(folder)) {
    // check  create directory
    return std::filesystem::create_directories(folder);
  }
  return true;
}

/**
 * Pushes a logging sink onto the logger's stack safely.
 *
 * @param sink The logging sink to push onto the stack.
 *
 * @remarks If the sink is successfully pushed onto the stack, it will be added to the list of active logging sinks.
 * If the sink is not successfully pushed onto the stack, no changes will be made to the logger.
 *
 * @see pop_sink_safe
 */
void LogManager::push_sink_safe(std::shared_ptr<spdlog::sinks::sink> sinkPtr, spdlog::level::level_enum level) {
  if (sinkPtr) {
    sinkPtr->set_level(level);
    if (!m_logSp) {
      m_logSp = std::make_shared<spdlog::logger>(m_name, sinkPtr);
    } else {
      m_logSp->sinks().push_back(sinkPtr);
    }
  }
}
}  // namespace cppsl::log
