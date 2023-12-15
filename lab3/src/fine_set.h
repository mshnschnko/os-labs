#pragma once

#include <pthread.h>
#include <functional>

template <typename T, typename Compare = std::less<T>>
class FineSet{
private:

  class Node {
    public:
      Node* next = nullptr;
      T data;
      mutable pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
      Node(const T& data) : data(data) {}
      ~Node() {
        pthread_mutex_destroy(&mutex);
      }
  };

  Node* head = nullptr;
  Compare compare;
  mutable pthread_mutex_t prevHeadMutex = PTHREAD_MUTEX_INITIALIZER;

  bool is_equal(const T& lhs, const T& rhs) const {
    return !compare(lhs, rhs) && !compare(rhs, lhs);
  }

public:
  FineSet(const Compare& compare = Compare()) : compare(compare) {}

  bool Add(const T& item) {
    pthread_mutex_lock(&prevHeadMutex);

    if (head == nullptr) {
      head = new Node(item);
      pthread_mutex_unlock(&prevHeadMutex);
      return true;
    }

    if (!compare(head->data, item) && !is_equal(head->data, item)) {
      Node* tmp = head;
      head = new Node(item);
      head->next = tmp;
      pthread_mutex_unlock(&prevHeadMutex);
      return true;
    }

    pthread_mutex_lock(&head->mutex);
    Node* cur = head;
    Node* prev = nullptr;
    while (cur->next && compare(cur->next->data, item)) {
      pthread_mutex_lock(&cur->next->mutex);
      if (prev)
        pthread_mutex_unlock(&prev->mutex);
      else
        pthread_mutex_unlock(&prevHeadMutex);
      prev = cur;
      cur = cur->next;
    }

    if (prev)
      pthread_mutex_unlock(&prev->mutex);
    else
      pthread_mutex_unlock(&prevHeadMutex);

    if (is_equal(cur->data, item)) {
      pthread_mutex_unlock(&cur->mutex);
      return false;
    }

    Node* tmp = cur->next;
    cur->next = new Node(item);
    cur->next->next = tmp;

    pthread_mutex_unlock(&cur->mutex);

    return true;
  }

  bool Remove(const T& item) {
    if (!head)
      return false;

    pthread_mutex_lock(&prevHeadMutex);
    pthread_mutex_lock(&head->mutex);
    if (is_equal(head->data, item)) {
      Node* tmp = head;
      head = head->next;
      pthread_mutex_unlock(&tmp->mutex);
      delete tmp;
      pthread_mutex_unlock(&prevHeadMutex);
      return true;
    }

    Node* prev = head;
    Node* cur = head->next;
    if (cur)
      pthread_mutex_lock(&cur->mutex);

    pthread_mutex_unlock(&prevHeadMutex);

    while (cur && compare(cur->data, item)) {
      if (cur->next)
        pthread_mutex_lock(&cur->next->mutex);
      pthread_mutex_unlock(&prev->mutex);
      prev = cur;
      cur = cur->next;
    }

    if (cur && is_equal(cur->data, item)) {
      prev->next = cur->next;
      pthread_mutex_unlock(&cur->mutex);
      delete cur;
      pthread_mutex_unlock(&prev->mutex);
      return true;
    }

    if (cur)
      pthread_mutex_unlock(&cur->mutex);
    pthread_mutex_unlock(&prev->mutex);

    return false;
  }

  bool Contains(const T& item) const {
    pthread_mutex_lock(&prevHeadMutex);
    pthread_mutex_lock(&head->mutex);
    const Node* prev = nullptr;
    const Node* cur = head;
    while (cur && compare(cur->data, item)) {
      if (cur->next)
        pthread_mutex_lock(&cur->next->mutex);
      if (prev)
        pthread_mutex_unlock(&prev->mutex);
      else
        pthread_mutex_unlock(&prevHeadMutex);
      prev = cur;
      cur = cur->next;
    }

    bool res = cur && is_equal(cur->data, item);
    if (cur)
      pthread_mutex_unlock(&cur->mutex);

    if (prev)
      pthread_mutex_unlock(&prev->mutex);
    else
      pthread_mutex_unlock(&prevHeadMutex);

    return res;
  }

  bool Empty() const {
    return head == nullptr;
  }

  ~FineSet() {
    Node* node = head;
    Node* next;
    while (node) {
      next = node->next;
      delete node;
      node = next;
    }
  }
};