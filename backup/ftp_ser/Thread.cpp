#include "Thread.h"

namespace {
class Thread_ {
 public:
  using thread_func_t = Thread::thread_func_t;
  Thread_(thread_func_t func) : func_(func) {}

  void run() { func_(); }

 private:
  thread_func_t func_;
  // name and tid
};

void* run_thread(void* arg) {
  Thread_* thr = static_cast<Thread_*>(arg);

  thr->run();
  delete thr;
  return nullptr;
}
}  // namespace

Thread::Thread(thread_func_t func, const std::string& name)
    : started_(false),
      joined_(false),
      pthread_id_(0),
      name_(name),
      func_(func) {}
Thread::~Thread() {
  if (!joined_) {
    pthread_detach(pthread_id_);
  }
}

void Thread::start() {
  assert(!started_);
  started_ = true;

  Thread_* thr = new Thread_(func_);
  if (pthread_create(&pthread_id_, nullptr, run_thread, thr)) {
    started_ = false;
    delete thr;
    abort();
  }
}

void Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  pthread_join(pthread_id_, nullptr);
}