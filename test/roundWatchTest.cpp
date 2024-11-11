#define CATCH_CONFIG_MAIN
#include <thread>

#include "catch.hpp"
#include "cppsl/time/roundWatch.hpp"

using namespace cppsl::time;

TEST_CASE("RoundWatch stores lap durations correctly", "[RoundWatch]") {
  RoundWatch<> watch;

  SECTION("Initial state") {
    REQUIRE(watch.Laps().empty());
  }

  SECTION("StoreLap stores the first lap duration") {
    watch.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    watch.StoreLap();
    REQUIRE(watch.Laps().size() == 1);
    REQUIRE(watch.Laps().front().total_time.count() >= 100);
  }

  SECTION("StoreLap stores multiple lap durations") {
    watch.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    watch.StoreLap();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    watch.StoreLap();
    REQUIRE(watch.Laps().size() == 2);
    REQUIRE(watch.Laps()[0].total_time.count() >= 100);
    REQUIRE(watch.Laps()[1].total_time.count() >= 300);
    REQUIRE(watch.Laps()[1].split_time.count() >= 200);
  }

  SECTION("Reset clears all lap durations") {
    watch.Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    watch.StoreLap();
    watch.Reset();
    REQUIRE(watch.Laps().empty());
  }
}
