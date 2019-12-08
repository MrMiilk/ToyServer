#ifndef TCPSERVER_H_INCLUDED
#define TCPSERVER_H_INCLUDED

#include <memory>
#include "TCPconn.h"

class InetAddress;
class Socket;
class Event;

class TCPserver {
 public:
  using TCPconn_sptr_t = TCPconn::conn_sptr_t;
  using conn_cb_func_t = std::function<void(std::shared_ptr<TCPconn>)>;

  TCPserver(int, Epoll*);
  TCPserver(const TCPserver&) = delete;
  TCPserver& operator=(const TCPserver&) = delete;

  void rg_conn_cb(const conn_cb_func_t& cb) { conn_cb_ = cb; }
  void start(const InetAddress&);
  // int accept(InetAddress&);

 private:
  void handle_rd();  // new connection
  // void handle_wr();

  std::unique_ptr<Socket> sock_uptr_;
  std::unique_ptr<Event> event_ptr_;
  conn_cb_func_t conn_cb_;
  // callback_func_t wr_cb_;
};

#endif  // TCPSERVER_H_INCLUDED