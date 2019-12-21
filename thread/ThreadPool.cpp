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

void ThreadPool::run(Task task) {
  assert(running_);
  if (threads_.empty()) {
    task();
  } else {
    MutexLockGuard lock(mutex_);
    task_queue_.emplace(task);
    cond_.notify();
  }
}

ThreadPool::Task ThreadPool::take_() {
  Task task;
  MutexLockGuard lock(mutex_);
  while (task_queue_.empty() && running_) {
    cond_.wait();
  }
  if (!task_queue_.empty()) {
    task = task_queue_.front();
    task_queue_.pop();
  }
  return std::move(task);
}

void ThreadPool::run_() {
  // try
  while (running_) {
    Task t = take_();
    if (t) t();
  }
}