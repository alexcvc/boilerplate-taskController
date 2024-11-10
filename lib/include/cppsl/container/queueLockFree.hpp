/* SPDX-License-Identifier: MIT */

/*************************************************************************/ /**
 * @file
 * @brief   contains lock free queue shared pointers.
 * @author  Alexander Sacharov <alexcvc@gmail.com>
 * @ingroup C++ support library
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <atomic>
#include <memory>

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::container {

/**
 * @class queueLockFree
 * @brief A lock-free queue data structure implementation.
 *
 * This class provides a lock-free queue data structure, where multiple threads can
 * simultaneously push and pop elements without encountering any races or deadlocks.
 *
 * @tparam T The type of the elements stored in the queue.
 */
template <typename T>
class queueLockFree {
  /**
     * @brief A node used in the queueLockFree class.
     * This class represents a node in the lock-free queue data structure.
     * It contains a shared pointer to the data and a pointer to the next node.
     */
  struct node {
    std::shared_ptr<T> m_dataSp;
    node* m_next{nullptr};
  };

  /// atomic head and tail of queue
  std::atomic<node*> m_head;
  std::atomic<node*> m_tail;

 public:
  /**
     * @brief A lock-free queue data structure implementation.
     * @tparam T The type of the elements stored in the queue.
     */
  queueLockFree() : m_head(new node), m_tail(m_head.load()) {}

  /**
     * @brief A lock-free queue data structure implementation.
     * @tparam T The type of the elements stored in the queue.
     */
  ~queueLockFree() {
    while (node* const old_head = m_head.load()) {
      m_head.store(old_head->m_next);
      delete old_head;
    }
  }
  queueLockFree(const queueLockFree& other) = delete;
  queueLockFree& operator=(const queueLockFree& other) = delete;

  /**
   * @brief Checks if the queue is empty.
   * @return bool True if the queue is empty, false otherwise.
   */
  [[nodiscard]] bool empty() {
    node* const old_head = m_head.load();
    if (old_head == m_tail.load()) {
      return true;
    }
    return false;
  }

  /**
   * @brief Returns a pointer to the first node at the head of the lock-free queue.
   * @return const node* A pointer to the first node at the head of the queue if successful,
   * or nullptr if the queue is empty.
   */
  [[nodiscard]] const node* front() const {
    node* const head = m_head.load();
    if (head == m_tail.load()) {
      return nullptr;
    }
    return head;
  }

  /**
   * @brief Returns a pointer to the last element in the lock-free queue.
   * @return const node* A pointer to the last element in the queue if successful,
   * or nullptr if the queue is empty.
   */
  [[nodiscard]] const node* back() const {
    node* const tail = m_tail.load();
    if (tail == m_head.load()) {
      return nullptr;
    }
    return tail;
  }

  /**
    * @brief Try to pop an element from the front of the lock-free queue.
    *
    * This function tries to pop an element from the front of the lock-free queue.
    * If the queue is empty, it returns an empty shared pointer.
    *
    * @tparam T The type of the elements stored in the queue.
    *
    * @return std::shared_ptr<T> A shared pointer to the popped element if successful,
    * or an empty shared pointer if the queue is empty.
    */
  [[nodiscard]] std::shared_ptr<T> try_pop() {
    std::unique_ptr<node> old_head(pop_head());
    return old_head ? old_head->m_dataSp : std::shared_ptr<T>();
  }

  /**
   * @brief Pop an element from the front of the lock-free queue.
   * @tparam T The type of the elements stored in the queue.
   * @return std::shared_ptr<T> A shared pointer to the popped element.
   */
  [[nodiscard]] std::shared_ptr<T> pop() {
    node* old_head = pop_head();
    if (!old_head) {
      return std::shared_ptr<T>();
    }
    std::shared_ptr<T> const res(old_head->m_dataSp);
    delete old_head;
    return res;
  }

  /**
   * @brief Pushes a new value into the lock-free queue.
   *
   * This function pushes a new value into the lock-free queue. It creates a new node with
   * the given value, and updates the tail of the queue to point to this new node.
   *
   * @tparam T The type of the value to be pushed into the queue.
   * @param new_value The new value to be pushed into the queue.
   */
  void push(T new_value) {
    std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
    node* p = new node;
    node* const old_tail = m_tail.load();
    old_tail->m_dataSp.swap(new_data);
    old_tail->m_next = p;
    m_tail.store(p);
  }

 private:
  /**
     * @brief Removes and returns the first node from the head of the lock-free queue.
     * @return A pointer to the first node on success, or nullptr if the queue is empty.
     */
  node* pop_head() {
    node* const old_head = m_head.load();
    if (old_head == m_tail.load()) {
      return nullptr;
    }
    m_head.store(old_head->m_next);
    return old_head;
  }
};

}  //namespace cppsl::container
