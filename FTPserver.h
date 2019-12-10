#ifndef FTPSERVER_H_INCLUDED
#define FTPSERVER_H_INCLUDED
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "Epoll.h"
#include "Event.h"
#include "Queue.h"
#include "Socket.h"
#include "TCPconn.h"
/* 管理和FTP servers 的连接 */
class FTPserver {
 public:
  FTPserver() : servers_(), msgs_(), ep_() {}

  void add(const std::vector<InetAddress>&);
  void send(const std::string&);
  void run();


 private:
  void onReadble(TCPconn::conn_sptr_t, const std::string&);
  //   void onWritable(TCPconn::conn_sptr_t, const std::string&);
  
  std::set<std::shared_ptr<TCPconn>> servers_;
  Queue<std::string> msgs_;
  Epoll ep_;
};

#endif  // FTPSERVER_H_INCLUDED