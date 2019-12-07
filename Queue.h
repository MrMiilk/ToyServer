#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include <queue>
#include "Condition.h"
#include "MutexLock.h"

template <typename data_Tp_>
class Queue {
 public:
  Queue() : mutex_(), cond_(mutex_), queue_() {}
  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  data_Tp_ get();
  data_Tp_ get_withlock();
  void put(data_Tp_);

 private:
  MutexLock mutex_;
  Condition cond_;
  std::queue<data_Tp_> queue_;
};

template <typename data_Tp_>
data_Tp_ Queue<data_Tp_>::get() {
  MutexLockGuard lock(mutex_);
  while (queue_.empty()) {
    cond_.wait();
  }
  data_Tp_ d = queue_.front();
  queue_.pop();
  return std::move(d);
}

template <typename data_Tp_>
data_Tp_ Queue<data_Tp_>::get_withlock() {
  MutexLockGuard lock(mutex_);
  data_Tp_ d;
  if (!queue_.empty()) {
    d = queue_.front();
    queue_.pop();
  }
  return std::move(d);
}

template <typename data_Tp_>
void Queue<data_Tp_>::put(data_Tp_ d) {
  MutexLockGuard lock(mutex_);
  queue_.push(d);
  cond_.notify();
}

#endif  // QUEUE_H_INCLUDED