#ifndef FTPSERVER_H_INCLUDED
#define FTPSERVER_H_INCLUDED
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "net/Epoll.h"
#include "net/Event.h"
#include "thread/Queue.h"
#include "net/Socket.h"
#include "net/TCPconn.h"
/* 管理和FTP servers 的连接 */
class FTPserver {
 public:
  FTPserver() : ep_(), servers_(), msgs_() {}

  void add(const std::vector<InetAddress>&);
  void send(const std::string&);
  void run();

 private:
  void onReadble(TCPconn::conn_sptr_t, const std::string&);
  Epoll ep_;    // 用于ftp监听
  std::set<std::shared_ptr<TCPconn>> servers_;
  Queue<std::string> msgs_;
};

#endif  // FTPSERVER_H_INCLUDED