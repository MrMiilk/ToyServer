#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED
#include <pthread.h>
#include <functional>
#include <memory>
#include <string>

#include <assert.h>

class Thread {
 public:
  using thread_func_t = std::function<void()>;

  explicit Thread(thread_func_t, const std::string& name = std::string());
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;
  ~Thread();

  void start();
  void join();
  bool started() const { return started_; }
  const std::string& name() const { return name_; }

 private:
  bool started_;
  bool joined_;
  pthread_t pthread_id_;
  // tid
  std::string name_;
  thread_func_t func_;
};

#endif  // THREAD_H_INCLUDED