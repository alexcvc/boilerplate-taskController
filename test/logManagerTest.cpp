#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cppsl/log/logManager.hpp"

TEST_CASE("LogManager default constructor", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.name() == "logman");
  REQUIRE(logManager.number_sinks() == 0);
  REQUIRE(logManager.empty() == true);
}

TEST_CASE("LogManager parameterized constructor", "[LogManager]") {
  cppsl::log::LogManager logManager("testLogger");
  REQUIRE(logManager.name() == "testLogger");
  REQUIRE(logManager.number_sinks() == 0);
  REQUIRE(logManager.empty() == true);
}

TEST_CASE("LogManager add_basic_file_sink", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.add_basic_file_sink("test.log", cppsl::log::LogManager::Truncate::by_open, spdlog::level::info) ==
          true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("LogManager add_rotation_file_sink", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.add_rotation_file_sink("test.log", 1048576, 3, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("LogManager add_daily_file_sink", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.add_daily_file_sink("test.log", 0, 0, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("LogManager add_console_sink", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("LogManager add_rsyslog_sink", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.add_rsyslog_sink("testIdent", "127.0.0.1", 1, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("LogManager add_syslog_sink", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.add_syslog_sink("testIdent", 0, 1, true, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("LogManager openLogger and closeLogger", "[LogManager]") {
  cppsl::log::LogManager logManager;
  REQUIRE(logManager.openLogger(spdlog::level::info) == true);
  logManager.closeLogger();
  REQUIRE(logManager.number_sinks() == 0);
}

TEST_CASE("LogManager set_level and level", "[LogManager]") {
  cppsl::log::LogManager logManager;
  logManager.set_level(spdlog::level::debug);
  REQUIRE(logManager.level() == spdlog::level::debug);
}

TEST_CASE("LogManager false open empty log", "[LogManager]") {
  cppsl::log::LogManager logManager;
  logManager.set_level(spdlog::level::debug);
  REQUIRE_FALSE(logManager.openLogger(spdlog::level::debug));
}

TEST_CASE("LogManager trace", "[LogManager]") {
  cppsl::log::LogManager logManager{"testLogger"};
  REQUIRE(logManager.add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::trace) == true);
  REQUIRE(logManager.openLogger(spdlog::level::trace) == true);
  REQUIRE_NOTHROW(logManager.trace("Trace message"));
}

TEST_CASE("LogManager debug", "[LogManager]") {
  cppsl::log::LogManager logManager{"testLogger"};
  logManager.openLogger(spdlog::level::debug);
  REQUIRE_NOTHROW(logManager.debug("Debug message"));
}

TEST_CASE("LogManager info", "[LogManager]") {
  cppsl::log::LogManager logManager;
  logManager.openLogger(spdlog::level::info);
  REQUIRE_NOTHROW(logManager.info("Info message"));
}

TEST_CASE("LogManager warn", "[LogManager]") {
  cppsl::log::LogManager logManager;
  logManager.openLogger(spdlog::level::warn);
  REQUIRE_NOTHROW(logManager.warn("Warn message"));
}

TEST_CASE("LogManager error", "[LogManager]") {
  cppsl::log::LogManager logManager;
  logManager.openLogger(spdlog::level::err);
  REQUIRE_NOTHROW(logManager.error("Error message"));
}

TEST_CASE("LogManager critical", "[LogManager]") {
  cppsl::log::LogManager logManager;
  auto ret = logManager.openLogger(spdlog::level::critical);
  REQUIRE_NOTHROW(logManager.critical("Critical message"));
}