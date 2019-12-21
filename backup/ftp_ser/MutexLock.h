#ifndef MUTEXLOCK_H_INCLUDED
#define MUTEXLOCK_H_INCLUDED
#include <assert.h>
#include <pthread.h>

class MutexLock {
  friend class MutexLockGuard;
  friend class Condition;

 public:
  // mutexattr == nullptr
  MutexLock() : holder_(0) { pthread_mutex_init(&lock_, nullptr); }
  MutexLock(const MutexLock&) = delete;
  MutexLock& operator=(const MutexLock&) = delete;
  ~MutexLock() {
    assert(holder_ == 0);
    pthread_mutex_destroy(&lock_);
  }
  // bool isLockByThisThread() const { return holder_ == CurrentThread::tid(); }

 private:
  void lock();
  void unlock();
  pthread_mutex_t* getPThreadMutex() { return &lock_; }

 private:
  pthread_mutex_t lock_;
  pid_t holder_;
};

class MutexLockGuard {
 public:
  explicit MutexLockGuard(MutexLock& mutex) : mutex_(mutex) { mutex_.lock(); }
  MutexLockGuard(const MutexLockGuard&) = delete;
  MutexLockGuard& operator=(const MutexLockGuard&) = delete;
  ~MutexLockGuard() { mutex_.unlock(); }

 private:
  MutexLock& mutex_;
};
#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")

#endif  // MUTEXLOCK_H_INCLUDED