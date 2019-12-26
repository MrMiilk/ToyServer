#ifndef TCPCONN_H_INCLUDED
#define TCPCONN_H_INCLUDED

#include <memory>
#include <functional>
#include"thread/Queue.h"

#define BUFSIZE 65535

class Socket;
class Event;
class Epoll;

class TCPconn : public std::enable_shared_from_this<TCPconn> {
 public:
  // using callback_func_t = Event::callback_func_t;
  using conn_sptr_t = std::shared_ptr<TCPconn>;
  using rd_cb_func_t = std::function<void(conn_sptr_t, const std::string&)>;
  using wr_cb_func_t = std::function<void(conn_sptr_t, const std::string&)>;
  // using

  // TCPconn(int domain, Epoll* epoll_ptr);
  TCPconn(int);
  TCPconn(std::unique_ptr<Socket>);
  TCPconn(const TCPconn&) = delete;
  TCPconn& operator=(const TCPconn&) = delete;

  // must call before establish
  void regist(Epoll* ep_fd);
  void establish(bool rd = true, bool wr = false);
  void disable_rd();
  void disable_wr();
  void enable_rd();
  void enable_wr();

  void set_rd_cb(const rd_cb_func_t& cb) { rd_cb_ = cb; }
  // TODO: call when write complete
  // void set_wr_cb(const wr_cb_func_t& cb) { rd_cb_ = cb; }
  void send(const std::string& msg) {
    msgs_.put(msg);
    enable_wr();
  }
  // void set_context(void* c) { context_ = c; }
  // void* get_context() { return context_; }

 private:
  void handle_rd();
  void handle_wr();

  // void* context_;
  Queue<std::string> msgs_;
  std::unique_ptr<Socket> sock_uptr_;
  std::unique_ptr<Event> event_ptr_;
  rd_cb_func_t rd_cb_;
  wr_cb_func_t wr_cb_;
};

#endif  // TCPCONN_H_INCLUDED