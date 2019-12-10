#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <assert.h>
#include <functional>
#include <memory>

class Epoll;
class Event {
 public:
  using callback_func_t = std::function<void()>;
  static const int NEW = 0;
  static const int ADDED = 1;
  static const int MOD = 2;
  static const int DEL = 3;
  static const int DELED = 3;

 public:
  explicit Event(int fd, Epoll* epoll_ptr)
      : epoll_ptr_(epoll_ptr),
        events_(0),
        revents_(0),
        read_cb_func_(nullptr),
        write_cb_func_(nullptr),
        fd_(fd),
        stau_(NEW) {
    assert(epoll_ptr_);
  }
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  ~Event() {
    // disable_rd();
    // disable_wr();
    events_ = NOEVENT;
    update();
  }

  /* 对读写状态的更新
   * 如果没有任何事件，Event会从epoll中删除 */
  void enable_rd() { events_ |= RDEVENT; }
  void enable_wr() { events_ |= WREVENT; }
  void disable_rd() { events_ &= ~RDEVENT; }
  void disable_wr() { events_ &= ~WREVENT; }
  bool rd_setted() const { return events_ & RDEVENT; }
  bool wr_setted() const { return events_ & WREVENT; }

  void set_rd_cb(const callback_func_t& func) { read_cb_func_ = func; }
  void set_wr_cb(const callback_func_t& func) { write_cb_func_ = func; }

  uint32_t events() const;
  void set_events(int evns = 0);
  int get_fd() const { return fd_; }
  int state() const { return stau_; }
  void added() { stau_ = ADDED; }
  void del() { stau_ = DELED; }

  void handler();

  void update();  // update event in epoll
 private:
  static const int RDEVENT;
  static const int WREVENT;
  static const int NOEVENT;

  Epoll* epoll_ptr_;
  int events_;
  int revents_;
  callback_func_t read_cb_func_;
  callback_func_t write_cb_func_;
  int fd_;
  int stau_;
};

#endif  // EVENT_H_INCLUDED