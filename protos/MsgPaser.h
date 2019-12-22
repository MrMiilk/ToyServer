#ifndef MSGPASER_H_INCLUDED
#define MSGPASER_H_INCLUDED

#include <string>
#include "ftp.pb.h"
#include "userReq.pb.h"

namespace Parser {

protos::UserReq parseUserMsg(const std::string& msg);

protos::FtpQury parseFtpQury(const std::string& msg);

template <typename T>
std::string encode(const T& req) {
  std::string msg;
  req.SerializeToString(&msg);
  return msg;
}

}  // namespace Parser

#endif  // MSGPASER_H_INCLUDED
