#include "MsgPaser.h"

namespace Parser {

protos::UserReq parseUserMsg(const std::string& msg) {
  protos::UserReq req;
  if (!req.ParseFromString(msg)) {
    printf("%s\n", msg.c_str());
    printf("prseUserMsg: ParseFromString error\n");
  }
  return req;
}

protos::FtpQury parseFtpQury(const std::string& msg) {
  protos::FtpQury req;
  if (!req.ParseFromString(msg)) {
    printf("%s\n", msg.c_str());
    printf("parseFtpReq: ParseFromString error\n");
  }
  return req;
}

protos::FtpReq parseFtpReq(const std::string& msg) {
  protos::FtpReq req;
  if (!req.ParseFromString(msg)) {
    printf("%s\n", msg.c_str());
    printf("parseFtpReq: ParseFromString error\n");
  }
  return req;
}

}  // namespace Parser