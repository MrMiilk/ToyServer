#include "ThreadPool.h"
#include <algorithm>

ThreadPool::ThreadPool()
    : mutex_(), cond_(mutex_), running_(false), task_queue_(), threads_() {}

void ThreadPool::start(int numThreads) {
  assert(!running_);
  assert(threads_.size() == 0);
  for (int i = 0; i < numThreads; ++i) {
    threads_.emplace_back(
        new Thread(std::bind(&ThreadPool::run_, this)));
    threads_[i]->start();
  }
  running_ = true;
}

void ThreadPool::stop() {
  assert(running_);
  running_ = false;
  cond_.notifyAll();
  std::for_each(threads_.begin(), threads_.end(),
                std::bind(&Thread::join, std::placeholders::_1));
}

void ThreadPool::run(std::unique_ptr<Task> task_uptr) {
  assert(running_);
  if (threads_.empty()) {
    task_uptr->run();
  } else {
    MutexLockGuard lock(mutex_);
    task_queue_.emplace(task_uptr.release());
    cond_.notify();
  }
}

Task* ThreadPool::take_() {
  Task* task = nullptr;
  MutexLockGuard lock(mutex_);
  while (task_queue_.empty() && running_) {
    cond_.wait();
  }
  if (!task_queue_.empty()) {
    task = task_queue_.front().release();
    task_queue_.pop();
  }
  return task;
}

void ThreadPool::run_() {
  // try
  while (running_) {
    Task* t = take_();
    if (t) t->run();
  }
}