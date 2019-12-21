#include "MutexLock.h"

void MutexLock::lock() {
  pthread_mutex_lock(&lock_);
  // holder_ = CurrentThread::tid();
}

void MutexLock::unlock() {
  holder_ = 0;
  pthread_mutex_unlock(&lock_);
}
