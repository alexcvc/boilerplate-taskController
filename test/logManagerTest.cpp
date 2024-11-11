#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include "cppsl/log/logManager.hpp"

TEST_CASE("LogManager operations", "[LogManager]") {
  cppsl::log::LogManager logManager("test_logger");

  SECTION("Initial state") {
    REQUIRE(logManager.name() == "test_logger");
    REQUIRE(logManager.empty());
    REQUIRE(logManager.number_sinks() == 0);
  }

  SECTION("Add and remove sinks") {
    REQUIRE(logManager.add_console_sink(cppsl::log::LogManager::OutputLog::out, cppsl::log::LogManager::Colored::color, spdlog::level::info));
    REQUIRE_FALSE(logManager.empty());
    REQUIRE(logManager.number_sinks() == 1);

    logManager.removeSinks();
    REQUIRE(logManager.empty());
    REQUIRE(logManager.number_sinks() == 0);
  }

  SECTION("Logging levels") {
    logManager.set_level(spdlog::level::debug);
    REQUIRE(logManager.level() == spdlog::level::debug);

    logManager.set_level(spdlog::level::info);
    REQUIRE(logManager.level() == spdlog::level::info);
  }

  SECTION("Logger operations") {
    REQUIRE(logManager.openLogger(spdlog::level::info));
    logManager.info("This is an info message");
    logManager.debug("This is a debug message");
    logManager.error("This is an error message");
    logManager.closeLogger();
  }
}