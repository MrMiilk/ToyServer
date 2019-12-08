#ifndef TCPCONN_H_INCLUDED
#define TCPCONN_H_INCLUDED

#include <memory>

#define BUFSIZE 65535

class Socket;
class Event;
class Epoll;

class TCPconn : public std::enable_shared_from_this<TCPconn> {
 public:
  // using callback_func_t = Event::callback_func_t;
  using conn_sptr_t = std::shared_ptr<TCPconn>;
  using rd_cb_func_t =
      std::function<void(conn_sptr_t, const char*, std::size_t)>;
  using wr_cb_func_t =
      std::function<void(conn_sptr_t, const char*, std::size_t)>;
  // using

  // TCPconn(int domain, Epoll* epoll_ptr);
  TCPconn(int);
  TCPconn(const TCPconn&) = delete;
  TCPconn& operator=(const TCPconn&) = delete;

  // must call before establish
  void regist(Epoll* ep_fd);
  void establish(bool rd = true, bool wr = false);
  void disable_rd();
  void disable_wr();

  void set_rd_cb(const rd_cb_func_t& cb) { rd_cb_ = cb; }
  void set_wr_cb(const wr_cb_func_t& cb) { rd_cb_ = cb; }

 private:
  void handle_rd();
  void handle_wr();

  std::unique_ptr<Socket> sock_uptr_;
  std::unique_ptr<Event> event_ptr_;
  rd_cb_func_t rd_cb_;
  wr_cb_func_t wr_cb_;
};

#endif  // TCPCONN_H_INCLUDED