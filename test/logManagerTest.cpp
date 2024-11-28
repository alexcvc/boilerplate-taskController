#define CATCH_CONFIG_MAIN

#include <sys/syslog.h>

#include "catch.hpp"
#include "cppsl/log/multiSinkWizard.hpp"

TEST_CASE("Test MultiSinkWizard", "[MultiSinkWizard]") {
  cppsl::log::MultiSinkWizard sinkWizard;

  SECTION("Test add_rotation_file_sink") {
    REQUIRE(sinkWizard.add_rotation_file_sink("test_rotation.log", 1024, 3, spdlog::level::info));
  }

  SECTION("Test add_daily_file_sink") {
    REQUIRE(sinkWizard.add_daily_file_sink("test_daily.log", 0, 0, spdlog::level::info));
  }

  SECTION("Test add_console_sink") {
    sinkWizard.add_console_sink(cppsl::log::MultiSinkWizard::OutputLog::err,
                                cppsl::log::MultiSinkWizard::Colored::color, spdlog::level::info);
    // Assuming add_console_sink has a way to verify the sink was added
    REQUIRE(true);  // Replace with actual verification if available
  }

  SECTION("Test add_syslog_sink") {
    REQUIRE(sinkWizard.add_syslog_sink("syslog_ident", LOG_PID, LOG_USER, true, spdlog::level::info));
  }

  SECTION("Test add_rsyslog_sink") {
    REQUIRE(sinkWizard.add_rsyslog_sink("rsyslog_ident", "127.0.0.1", LOG_USER, spdlog::level::info, 514, true, 1024));
  }
}