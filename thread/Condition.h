#ifndef CONDITION_H_INCLUDED
#define CONDITION_H_INCLUDED

#include <assert.h>
#include <pthread.h>
#include "MutexLock.h"

class Condition {
 public:
  // cond_attr == nullptr
  explicit Condition(MutexLock& mutex) : mutex_(mutex) {
    pthread_cond_init(&cond_, nullptr);
  }
  Condition(const Condition&) = delete;
  Condition& operator=(const Condition&) = delete;
  ~Condition() { pthread_cond_destroy(&cond_); }
  void wait();
  void notify();
  void notifyAll();

 private:
  MutexLock& mutex_;
  pthread_cond_t cond_;
};

#endif  // CONDITION_H_INCLUDED