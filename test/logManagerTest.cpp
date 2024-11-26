#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cppsl/log/multiSinkWizard.hpp"

TEST_CASE("MultiSinkWizard default constructor", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.name() == "logman");
  REQUIRE(logManager.number_sinks() == 0);
  REQUIRE(logManager.empty() == true);
}

TEST_CASE("MultiSinkWizard parameterized constructor", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager("testLogger");
  REQUIRE(logManager.name() == "testLogger");
  REQUIRE(logManager.number_sinks() == 0);
  REQUIRE(logManager.empty() == true);
}

TEST_CASE("MultiSinkWizard add_basic_file_sink", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.add_basic_file_sink("test.log", cppsl::log::MultiSinkWizard::Truncate::by_open,
                                         spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("MultiSinkWizard add_rotation_file_sink", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.add_rotation_file_sink("test.log", 1048576, 3, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("MultiSinkWizard add_daily_file_sink", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.add_daily_file_sink("test.log", 0, 0, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("MultiSinkWizard add_console_sink", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.add_console_sink(cppsl::log::MultiSinkWizard::OutputLog::out,
                                      cppsl::log::MultiSinkWizard::Colored::color, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("MultiSinkWizard add_rsyslog_sink", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.add_rsyslog_sink("testIdent", "127.0.0.1", 1, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("MultiSinkWizard add_syslog_sink", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.add_syslog_sink("testIdent", 0, 1, true, spdlog::level::info) == true);
  REQUIRE(logManager.number_sinks() == 1);
}

TEST_CASE("MultiSinkWizard openLogger and closeLogger", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  REQUIRE(logManager.openLogger(spdlog::level::info) == true);
  logManager.closeLogger();
  REQUIRE(logManager.number_sinks() == 0);
}

TEST_CASE("MultiSinkWizard set_level and level", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  logManager.set_level(spdlog::level::debug);
  REQUIRE(logManager.level() == spdlog::level::debug);
}

TEST_CASE("MultiSinkWizard false open empty log", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  logManager.set_level(spdlog::level::debug);
  REQUIRE_FALSE(logManager.openLogger(spdlog::level::debug));
}

TEST_CASE("MultiSinkWizard trace", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager{"testLogger"};
  REQUIRE(logManager.add_console_sink(cppsl::log::MultiSinkWizard::OutputLog::out,
                                      cppsl::log::MultiSinkWizard::Colored::color, spdlog::level::trace) == true);
  REQUIRE(logManager.openLogger(spdlog::level::trace) == true);
  REQUIRE_NOTHROW(logManager.trace("Trace message"));
}

TEST_CASE("MultiSinkWizard debug", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager{"testLogger"};
  logManager.openLogger(spdlog::level::debug);
  REQUIRE_NOTHROW(logManager.debug("Debug message"));
}

TEST_CASE("MultiSinkWizard info", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  logManager.openLogger(spdlog::level::info);
  REQUIRE_NOTHROW(logManager.info("Info message"));
}

TEST_CASE("MultiSinkWizard warn", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  logManager.openLogger(spdlog::level::warn);
  REQUIRE_NOTHROW(logManager.warn("Warn message"));
}

TEST_CASE("MultiSinkWizard error", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  logManager.openLogger(spdlog::level::err);
  REQUIRE_NOTHROW(logManager.error("Error message"));
}

TEST_CASE("MultiSinkWizard critical", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard logManager;
  auto ret = logManager.openLogger(spdlog::level::critical);
  REQUIRE_NOTHROW(logManager.critical("Critical message"));
}