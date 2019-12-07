#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <functional>
#include <memory>
#include <queue>
#include "Condition.h"
#include "MutexLock.h"
#include "Thread.h"

class Task {
 public:
  using task_t = std::function<void(void* arg)>;
  explicit Task(task_t func, void* args = nullptr) : func_(func), args_(args) {}
  Task(const Task&) = delete;
  const Task& operator=(const Task&) = delete;

  void run() { func_(args_); }

 private:
  task_t func_;
  void* args_;
};

class ThreadPool {
 public:
  explicit ThreadPool();
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void start(int);
  void stop();
  void run(std::unique_ptr<Task>);

 private:
  Task* take_();
  void run_();

  MutexLock mutex_;
  Condition cond_;
  bool running_;
  std::queue<std::unique_ptr<Task>> task_queue_;
  std::vector<std::unique_ptr<Thread>> threads_;
};

#endif  // THREADPOOL_H_INCLUDED