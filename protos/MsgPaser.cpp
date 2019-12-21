#include "MsgPaser.h"

namespace Parser {

protos::UserReq parseUserMsg(const std::string& msg) {
  protos::UserReq req;
  if (!req.ParseFromString(msg)) {
    printf("prseUserMsg: ParseFromString error");
  }
  return req;
}

protos::FtpQury parseFtpQury(const std::string& msg) {
  protos::FtpQury req;
  if (!req.ParseFromString(msg)) {
    printf("parseFtpReq: ParseFromString error");
  }
  return req;
}

}  // namespace Parser