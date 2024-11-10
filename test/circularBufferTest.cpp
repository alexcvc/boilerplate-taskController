#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "cppsl/buffer/circularBuffer.hpp"

using namespace cppsl::buffer;

TEST_CASE("CircularBuffer operations", "[CircularBuffer]") {
  CircularBuffer<int> buffer(8);

  SECTION("Initial state") {
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.capacity() == 8);
  }

  SECTION("Push and pop operations") {
    REQUIRE(buffer.push(1));
    REQUIRE(buffer.push(2));
    REQUIRE(buffer.push(3));
    REQUIRE(buffer.size() == 3);
    REQUIRE_FALSE(buffer.empty());

    int value;
    REQUIRE(buffer.pop(value));
    REQUIRE(value == 1);
    REQUIRE(buffer.pop(value));
    REQUIRE(value == 2);
    REQUIRE(buffer.pop(value));
    REQUIRE(value == 3);
    REQUIRE(buffer.empty());
  }

  SECTION("Buffer full and empty states") {
    for (int i = 0; i < 8; ++i) {
      if (i < 7)
        REQUIRE(buffer.push(i));
      else
        REQUIRE_FALSE(buffer.push(i));
    }
    REQUIRE(buffer.full());
    REQUIRE_FALSE(buffer.push(9));

    int value;
    for (int i = 0; i < 8; ++i) {
      if (buffer.size()) {
        REQUIRE(buffer.pop(value));
        REQUIRE(value == i);
      } else {
        REQUIRE_FALSE(buffer.pop(value));
      }
    }
    REQUIRE(buffer.empty());
  }

  SECTION("Clear buffer") {
    REQUIRE(buffer.push(1));
    REQUIRE(buffer.push(2));
    REQUIRE(buffer.push(3));
    REQUIRE(buffer.push(4));
    REQUIRE(buffer.push(5));
    REQUIRE(buffer.push(6));
    REQUIRE(buffer.push(7));
    REQUIRE(buffer.push(8) == false);
    buffer.clear();
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
  }
}