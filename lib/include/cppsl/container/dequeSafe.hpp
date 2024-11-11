// SPDX-License-Identifier: MIT

/**
 * @file
 * @brief Contains a safe dual queue container using fixed size memory allocation and handling of elements at both ends in constant time.
 * @ingroup Container
 */

#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

namespace cppsl::container {

/**
 * @class DequeSafe
 * @brief A thread-safe implementation of deque container.
 *
 * The DequeSafe class implements a thread-safe version of the deque container,
 * which allows multiple threads to concurrently perform operations on the container
 * without causing any data race. It uses a mutex and condition variable to ensure
 * synchronized access to the underlying deque.
 *
 * @tparam TypeElement The type of elements stored in the deque.
 * @tparam TAlloc The allocator type for element construction, defaults to std::allocator<TypeElement>.
 */
template <typename TypeElement, typename TAlloc = std::allocator<TypeElement>>
class DequeSafe {
  mutable std::mutex m_semaphore;       ///< Mutex to protect the deque container
  std::condition_variable m_condition;  ///< Condition variable to notify waiting threads
  std::deque<TypeElement> m_container;  ///< The underlying deque container

 public:
  /// Default constructor
  DequeSafe() = default;

  /// Copy constructor
  DequeSafe(const DequeSafe& other) {
    std::lock_guard<std::mutex> lk(other.m_semaphore);
    m_container = other.m_container;
  }

  /**
   * @brief Removes all elements from the container.
   *
   * The clear function acquires a lock on the semaphore and removes all elements from the container.
   * After clearing the container, it is empty.
   */
  void clear() {
    std::lock_guard<std::mutex> lk(m_semaphore);
    m_container.clear();
  }

  /**
   * @brief Returns a reference to the first element in the deque.
   * @return A reference to the first element in the deque.
   */
  TypeElement& front() {
    std::lock_guard<std::mutex> lk(m_semaphore);
    return m_container.front();
  }

  /**
   * @brief Returns a const reference to the first element in the deque.
   * @return A const reference to the first element in the deque.
   */
  const TypeElement& front() const {
    std::lock_guard<std::mutex> lk(m_semaphore);
    return m_container.front();
  }

  /**
   * @brief Retrieves a reference to the last element of the container.
   * @return A reference to the last element in the container.
   */
  TypeElement& back() {
    std::lock_guard<std::mutex> lk(m_semaphore);
    return m_container.back();
  }

  /**
   * @brief Returns a const reference to the last element in the deque.
   * @return A const reference to the last element in the deque.
   */
  const TypeElement& back() const {
    std::lock_guard<std::mutex> lk(m_semaphore);
    return m_container.back();
  }

  /**
   * @brief Adds a new element to the front of the deque container.
   *
   * This function acquires a lock on the semaphore and adds the new value
   * to the front of the deque container. It then notifies one waiting thread
   * that the deque is no longer empty.
   *
   * @param new_value The value to be added to the deque.
   */
  [[maybe_unused]] void push_front(TypeElement new_value) {
    std::lock_guard<std::mutex> lk(m_semaphore);
    m_container.push_front(new_value);
    m_condition.notify_one();
  }

  /**
   * @brief Adds a new element to the end of the deque container.
   *
   * This function acquires a lock on the semaphore and adds the new value
   * to the end of the deque container. It then notifies one waiting thread
   * that the deque is no longer empty.
   *
   * @param new_value The value to be added to the deque.
   */
  void push_back(TypeElement new_value) {
    std::lock_guard<std::mutex> lk(m_semaphore);
    m_container.push_back(new_value);
    m_condition.notify_one();
  }

  /**
   * @brief Waits until the deque is not empty and pops the first element from the deque.
   *
   * This function acquires a lock on the semaphore and waits until the deque is not empty.
   * Once the deque is not empty, it pops the first element from the deque and assigns it
   * to the given value.
   *
   * @param value A reference to store the popped element.
   */
  [[maybe_unused]] bool wait_and_pop_front(TypeElement& value,
                                           std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
    std::unique_lock<std::mutex> lk(m_semaphore);
    m_condition.wait_for(lk, timeout, [this] {
      return !m_container.empty();
    });
    if (m_container.empty()) {
      return false;
    }
    value = m_container.front();
    m_container.pop_front();
    return true;
  }

  /**
   * @brief Waits until the deque is not empty and pops the last element.
   *
   * This function acquires a lock on the semaphore and waits until the deque is not empty.
   * Once the deque is not empty, it pops the last element from the deque and assigns it
   * to the given value.
   *
   * @param value A reference to store the popped element.
   */
  [[maybe_unused]] bool wait_and_pop_back(TypeElement& value,
                                          std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
    std::unique_lock<std::mutex> lk(m_semaphore);
    m_condition.wait_for(lk, timeout, [this] {
      return !m_container.empty();
    });
    if (m_container.empty()) {
      return false;
    }
    value = m_container.back();
    m_container.pop_back();
    return true;
  }

  /**
   * @brief Waits until the deque is not empty and pops the first element from the deque.
   *
   * This function acquires a lock on the semaphore and waits until the deque is not empty.
   * Once the deque is not empty, it pops the first element from the deque, creates a shared pointer
   * to that element, and returns it.
   *
   * @return A shared pointer to the first element of the deque.
   */
  [[maybe_unused]] std::shared_ptr<TypeElement> wait_and_pop_front(
      std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
    std::unique_lock<std::mutex> lk(m_semaphore);
    m_condition.wait_for(lk, timeout, [this] {
      return !m_container.empty();
    });
    std::shared_ptr<TypeElement> res(std::make_shared<TypeElement>(m_container.front()));
    m_container.pop_front();
    return res;
  }

  /**
   * @brief Waits until the deque is not empty and pops the last element from the deque.
   *
   * This function acquires a lock on the semaphore and waits until the deque is not empty.
   * Once the deque is not empty, it pops the last element from the deque, creates a shared pointer
   * to that element, and returns it.
   *
   * @return A shared pointer to the last element of the deque.
   */
  [[maybe_unused]] std::shared_ptr<TypeElement> wait_and_pop_back(
      std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
    std::unique_lock<std::mutex> lk(m_semaphore);
    m_condition.wait_for(lk, timeout, [this] {
      return !m_container.empty();
    });
    std::shared_ptr<TypeElement> res(std::make_shared<TypeElement>(m_container.back()));
    m_container.pop_back();
    return res;
  }

  /**
   * @brief Attempts to pop the last element from the deque if it is not empty.
   *
   * This function acquires a lock on the semaphore and checks if the deque is empty.
   * If it is not empty, it pops the last element from the deque and assigns it to the given value.
   * If the deque is empty, it returns false.
   *
   * @param value A reference to store the popped element.
   * @return True if an element was successfully popped, false otherwise.
   */
  [[maybe_unused]] bool try_pop_back(TypeElement& value) {
    std::lock_guard<std::mutex> lk(m_semaphore);
    if (m_container.empty()) {
      return false;
    }
    value = m_container.back();
    m_container.pop_back();
    return true;
  }

  /**
   * @brief Tries to pop the last element from the deque if it is not empty.
   * @return A shared pointer to the popped element, or nullptr if the deque is empty.
   */
  [[maybe_unused]] std::shared_ptr<TypeElement> try_pop_back() {
    std::lock_guard<std::mutex> lk(m_semaphore);
    if (m_container.empty()) {
      return std::shared_ptr<TypeElement>();
    }
    std::shared_ptr<TypeElement> res(std::make_shared<TypeElement>(m_container.back()));
    m_container.pop_back();
    return res;
  }

  /**
   * @brief Tries to pop the first element from the deque if it is not empty.
   * @param value The variable to store the popped element.
   * @return True if an element was successfully popped, false otherwise.
   */
  [[maybe_unused]] bool try_pop_front(TypeElement& value) {
    std::lock_guard<std::mutex> lk(m_semaphore);
    if (m_container.empty()) {
      return false;
    }
    value = m_container.front();
    m_container.pop_front();
    return true;
  }

  /**
   * @brief Tries to pop the first element from the deque if it is not empty.
   * @return A shared pointer to the popped element, or nullptr if the deque is empty.
   */
  [[maybe_unused]] std::shared_ptr<TypeElement> try_pop_front() {
    std::lock_guard<std::mutex> lk(m_semaphore);
    if (m_container.empty()) {
      return std::shared_ptr<TypeElement>();
    }
    std::shared_ptr<TypeElement> res(std::make_shared<TypeElement>(m_container.front()));
    m_container.pop_front();
    return res;
  }

  /**
   * @brief Checks if the container is empty.
   *
   * The empty function acquires a lock on the semaphore and checks if the container is empty.
   *
   * @return True if the container is empty, False otherwise.
   */
  bool empty() const {
    std::lock_guard<std::mutex> lk(m_semaphore);
    return m_container.empty();
  }

  /**
   * @brief Returns the size of the container.
   *
   * The size function acquires a lock on the semaphore and returns the current size of the container.
   *
   * @return The size of the container.
   */
  size_t size() const {
    std::lock_guard<std::mutex> lk(m_semaphore);
    return m_container.size();
  }
};

}  // namespace cppsl::container