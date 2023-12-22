#pragma once

#include <pthread.h>
#include <functional>
#include <iostream>

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
    // pthread_mutex_unlock(&prevHeadMutex);
    // if (Contains(item)) {
    //   std::cout << "contains " << item << std::endl;
    //   return false;
    // }
    // pthread_mutex_lock(&prevHeadMutex);
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
      //debug std::cout << "=====================FALSE cur = " << cur->data << ", item = " << item << std::endl;
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

  void Print() {
    if (Empty()) {
      std::cout << "set is empty" << std::endl;
      return;
    }
    Node* node = head;
    int count = 0;
    std::cout << "set data: ";
    while (node) {
      std::cout << node->data << " ";
      node = node->next;
      ++count;
    }
    std::cout << "\ttotal count of elements: " << count << std::endl;
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


///////////////////////////////////////////////////////////
// #pragma once

// #include <pthread.h>
// #include <functional>
// #include <iostream>

// template <typename T, typename Compare = std::less<T>>
// class FineSet {
// private:
//     class Node {
//     public:
//         Node* next = nullptr;
//         T data;
//         mutable pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//         Node(const T& data) : data(data) {}
//         ~Node() {
//             pthread_mutex_destroy(&mutex);
//         }
//     };

//     Node* head = nullptr;
//     Compare compare;
//     mutable pthread_mutex_t prevHeadMutex = PTHREAD_MUTEX_INITIALIZER;

//     bool is_equal(const T& lhs, const T& rhs) const {
//         return !compare(lhs, rhs) && !compare(rhs, lhs);
//     }

//     void lockNode(const Node* node) const {
//         pthread_mutex_lock(&(node->mutex));
//     }

//     void unlockNode(const Node* node) const {
//         pthread_mutex_unlock(&(node->mutex));
//     }

// public:
//     FineSet(const Compare& compare = Compare()) : compare(compare) {}

//     bool Add(const T& item) {
//         pthread_mutex_lock(&prevHeadMutex);

//         if (head == nullptr) {
//             head = new Node(item);
//             pthread_mutex_unlock(&prevHeadMutex);
//             return true;
//         }

// 		// ...

// 		if (!compare(head->data, item) && !is_equal(head->data, item)) {
// 			Node* tmp = head;
// 			head = new Node(item);
// 			head->next = tmp;
// 			pthread_mutex_unlock(&prevHeadMutex);
// 			return true;
// 		}

// 		lockNode(head);
// 		pthread_mutex_unlock(&prevHeadMutex);

// 		Node* cur = head;
// 		Node* prev = nullptr;

// 		while (cur->next && compare(cur->next->data, item)) {
// 			lockNode(cur->next);
// 			if (!compare(cur->next->data, item) && !is_equal(cur->next->data, item)) {
// 				unlockNode(cur->next);
// 				break;
// 			}
// 			if (prev)
// 				unlockNode(prev);
// 			else
// 				pthread_mutex_unlock(&prevHeadMutex);
// 			prev = cur;
// 			cur = cur->next;
// 		}

// // ...

//         // if (!compare(head->data, item) && !is_equal(head->data, item)) {
//         //     Node* tmp = head;
//         //     head = new Node(item);
//         //     head->next = tmp;
//         //     pthread_mutex_unlock(&prevHeadMutex);
//         //     return true;
//         // }

//         // lockNode(head);
//         // pthread_mutex_unlock(&prevHeadMutex);

//         // Node* cur = head;
//         // Node* prev = nullptr;

//         // while (cur->next && compare(cur->next->data, item)) {
//         //     lockNode(cur->next);
//         //     if (prev)
//         //         unlockNode(prev);
//         //     else
//         //         pthread_mutex_unlock(&prevHeadMutex);
//         //     prev = cur;
//         //     cur = cur->next;
//         // }

//         if (prev)
//             unlockNode(prev);
//         else
//             pthread_mutex_unlock(&prevHeadMutex);

//         if (is_equal(cur->data, item)) {
//             unlockNode(cur);
//             return false;
//         }

//         Node* tmp = cur->next;
//         cur->next = new Node(item);
//         cur->next->next = tmp;

//         unlockNode(cur);

//         return true;
//     }

//     bool Remove(const T& item) {
//         if (!head)
//             return false;

//         pthread_mutex_lock(&prevHeadMutex);
//         lockNode(head);

//         if (is_equal(head->data, item)) {
//             Node* tmp = head;
//             head = head->next;
//             unlockNode(tmp);
//             delete tmp;
//             pthread_mutex_unlock(&prevHeadMutex);
//             return true;
//         }

//         Node* prev = head;
//         Node* cur = head->next;

//         if (cur)
//             lockNode(cur);

//         pthread_mutex_unlock(&prevHeadMutex);

//         while (cur && compare(cur->data, item)) {
//             if (cur->next)
//                 lockNode(cur->next);
//             unlockNode(prev);
//             prev = cur;
//             cur = cur->next;
//         }

//         if (cur && is_equal(cur->data, item)) {
//             prev->next = cur->next;
//             unlockNode(cur);
//             delete cur;
//             unlockNode(prev);
//             return true;
//         }

//         if (cur)
//             unlockNode(cur);

//         unlockNode(prev);

//         return false;
//     }

//     bool Contains(const T& item) const {
//         pthread_mutex_lock(&prevHeadMutex);
//         lockNode(head);

//         const Node* prev = nullptr;
//         const Node* cur = head;

//         while (cur && compare(cur->data, item)) {
//             if (cur->next)
//                 lockNode(cur->next);
//             if (prev)
//                 unlockNode(prev);
//             else
//                 pthread_mutex_unlock(&prevHeadMutex);
//             prev = cur;
//             cur = cur->next;
//         }

//         bool res = cur && is_equal(cur->data, item);

//         if (cur)
//             unlockNode(cur);

//         if (prev)
//             unlockNode(prev);
//         else
//             pthread_mutex_unlock(&prevHeadMutex);

//         return res;
//     }

//     void Print() {
//         if (Empty()) {
//             std::cout << "set is empty" << std::endl;
//             return;
//         }

//         Node* node = head;
//         int count = 0;

//         std::cout << "set data: ";

//         while (node) {
//             std::cout << node->data << " ";
//             node = node->next;
//             ++count;
//         }

//         std::cout << "\ttotal count of elements: " << count << std::endl;
//     }

//     bool Empty() const {
//         return head == nullptr;
//     }

//     ~FineSet() {
//         Node* node = head;
//         Node* next;

//         while (node) {
//             next = node->next;
//             delete node;
//             node = next;
//         }
//     }
// };
