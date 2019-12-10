#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <memory>
#include <string>
#include "TCPconn.h"

class User {
 public:
  static const int NOLOGIN = 1;
  static const int LOGINED = 2;

  explicit User(TCPconn::conn_sptr_t link, int state = NOLOGIN)
      : name_(), tcp_link_(link), state_(state) {}
  // noncopyable

  void login() { state_ = LOGINED; }

 private:
  std::string name_;
  TCPconn::conn_sptr_t tcp_link_;
  int state_;
};

#endif  // USER_H_INCLUDED