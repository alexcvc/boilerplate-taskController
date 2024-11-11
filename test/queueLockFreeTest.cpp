#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "cppsl/container/queueLockFree.hpp"

TEST_CASE("queueLockFree operations", "[queueLockFree]") {
  cppsl::container::queueLockFree<int> queue;

  SECTION("Initial state") {
    REQUIRE(queue.empty());
    REQUIRE(queue.front() == nullptr);
    REQUIRE(queue.back() == nullptr);
  }

  SECTION("Push and pop operations") {
    queue.push(1);
    queue.push(2);

    REQUIRE_FALSE(queue.empty());
    REQUIRE(queue.front() != nullptr);
    REQUIRE(queue.back() != nullptr);

    auto front = queue.try_pop();
    REQUIRE(front != nullptr);
    REQUIRE(*front == 1);

    front = queue.try_pop();
    REQUIRE(front != nullptr);
    REQUIRE(*front == 2);

    REQUIRE(queue.empty());
  }

  SECTION("Try pop on empty queue") {
    auto front = queue.try_pop();
    REQUIRE(front == nullptr);
  }

  SECTION("Pop on empty queue") {
    auto front = queue.pop();
    REQUIRE(front == nullptr);
  }
}