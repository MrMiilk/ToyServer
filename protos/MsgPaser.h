#ifndef MSGPASER_H_INCLUDED
#define MSGPASER_H_INCLUDED

#include <string>
#include <vector>
#include "userReq.pb.h"

namespace Parser {

protos::UserReq prseUserMsg(const std::string& msg) {
  protos::UserReq req;
  if (!req.ParseFromString(msg)) {
    printf("prseUserMsg: ParseFromString error");
  }
  return req;
}

const std::string& encodeUserMsg(const protos::UserReq& req) {
  std::string msg;
  req.SerializeToString(&msg);
  return msg;
}



}  // namespace Parser

#endif  // MSGPASER_H_INCLUDED
