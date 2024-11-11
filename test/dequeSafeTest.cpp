#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cppsl/container/dequeSafe.hpp"

TEST_CASE("DequeSafe operations", "[DequeSafe]") {
  cppsl::container::DequeSafe<int> deque;

  SECTION("Initial state") {
    REQUIRE(deque.empty());
    REQUIRE(deque.size() == 0);
  }

  SECTION("Push and pop operations") {
    deque.push_back(1);
    deque.push_front(2);

    REQUIRE(deque.size() == 2);
    REQUIRE(deque.front() == 2);
    REQUIRE(deque.back() == 1);

    int value;
    deque.wait_and_pop_front(value);
    REQUIRE(value == 2);
    REQUIRE(deque.size() == 1);

    deque.wait_and_pop_back(value);
    REQUIRE(value == 1);
    REQUIRE(deque.empty());
  }

  SECTION("Try pop operations") {
    deque.push_back(3);
    deque.push_front(4);

    int value;
    REQUIRE(deque.try_pop_front(value));
    REQUIRE(value == 4);
    REQUIRE(deque.size() == 1);

    REQUIRE(deque.try_pop_back(value));
    REQUIRE(value == 3);
    REQUIRE(deque.empty());
  }

  SECTION("Clear operation") {
    deque.push_back(5);
    deque.push_front(6);
    deque.clear();

    REQUIRE(deque.empty());
    REQUIRE(deque.size() == 0);
  }

  SECTION("Thread safety") {
    std::vector<std::thread> threads;
    const int iterations = 1000;
    std::atomic<int> sum{0};

    // Producer thread
    threads.emplace_back([&]() {
      for (int i = 0; i < iterations; ++i) {
        deque.push_back(i);
      }
    });

    // Consumer thread
    threads.emplace_back([&]() {
      int value;
      for (int i = 0; i < iterations; ++i) {
        deque.wait_and_pop_front(value);
        sum += value;
      }
    });

    for (auto& t : threads)
      t.join();
    REQUIRE(deque.empty());
  }
}
