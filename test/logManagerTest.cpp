#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cppsl/log/logManager.hpp"

TEST_CASE("LogManager create manager", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->name() == "test");
  REQUIRE(logManPtr->numberSinks() == 0);
  REQUIRE(logManPtr->empty() == true);
}

TEST_CASE("LogManager add_basic_file_sink", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_basic_file_sink("test.log", cppsl::log::LogManager::Truncate::by_open, spdlog::level::info) ==
          true);
  REQUIRE(logManPtr->numberSinks() == 1);
}

TEST_CASE("LogManager add_rotation_file_sink", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_rotation_file_sink("test.log", 1048576, 3, spdlog::level::info) == true);
  REQUIRE(logManPtr->numberSinks() == 1);
}

TEST_CASE("LogManager add_daily_file_sink", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_daily_file_sink("test.log", 0, 0, spdlog::level::info) == true);
  REQUIRE(logManPtr->numberSinks() == 1);
}

TEST_CASE("LogManager add_console_sink", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::info) == true);
  REQUIRE(logManPtr->numberSinks() == 1);
}

TEST_CASE("LogManager add_rsyslog_sink", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_rsyslog_sink("testIdent", "127.0.0.1", 1, spdlog::level::info) == true);
  REQUIRE(logManPtr->numberSinks() == 1);
}

TEST_CASE("LogManager add_syslog_sink", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_syslog_sink("testIdent", 0, 1, true, spdlog::level::info) == true);
  REQUIRE(logManPtr->numberSinks() == 1);
}

TEST_CASE("LogManager openLogger and closeLogger", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::info) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::info) == true);
  logManPtr->info("Info message");
  logManPtr->closeLogger();
  REQUIRE(logManPtr->numberSinks() == 0);
}

TEST_CASE("LogManager set_level and level", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  logManPtr->set_level(spdlog::level::debug);
  REQUIRE(logManPtr->level() == spdlog::level::debug);
  logManPtr->closeLogger();
  REQUIRE(logManPtr->numberSinks() == 0);
}

TEST_CASE("LogManager false open empty log", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::debug) == true);
  logManPtr->set_level(spdlog::level::debug);
  REQUIRE_NOTHROW(logManPtr->openLogger(spdlog::level::debug));
  logManPtr->closeLogger();
  REQUIRE(logManPtr->numberSinks() == 0);
}

TEST_CASE("LogManager trace", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::trace) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::trace));
  REQUIRE_NOTHROW(logManPtr->trace("Trace message"));
  logManPtr->closeLogger();
  REQUIRE(logManPtr->numberSinks() == 0);
}

TEST_CASE("LogManager debug", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::debug) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::debug) == true);
  REQUIRE_NOTHROW(logManPtr->debug("Debug message"));
  logManPtr->closeLogger();
  REQUIRE(logManPtr->numberSinks() == 0);
}

TEST_CASE("LogManager info", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::info) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::info) == true);
  REQUIRE_NOTHROW(logManPtr->info("Info message"));
}

TEST_CASE("LogManager warn", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::warn) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::warn) == true);
  REQUIRE_NOTHROW(logManPtr->warn("Warn message"));
}

TEST_CASE("LogManager error", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::err) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::err) == true);
  REQUIRE_NOTHROW(logManPtr->error("Error message"));
}

TEST_CASE("LogManager critical", "[LogManager]") {
  auto logManPtr = cppsl::log::CreateSharedManager("test");
  REQUIRE(logManPtr->add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color,
                                      spdlog::level::critical) == true);
  REQUIRE(logManPtr->openLogger(spdlog::level::critical) == true);
  REQUIRE_NOTHROW(logManPtr->critical("Critical message"));
}