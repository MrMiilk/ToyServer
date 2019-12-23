#include <iostream>
#include <string>
#include "protos/userReq.pb.h"

int main() {
  // protos::FileInfo fileInfo;
  // protos::UserInfo userInfo;
  protos::UserReq req;

  auto* req_f_info = req.mutable_fileinfo();
  req_f_info->set_filename("a.txt");
  req_f_info->set_path("/home/docs");
  req_f_info->set_size(32);
  req_f_info->set_tp(".txt");

  auto* req_usr_info = req.mutable_userinfo();
  req_usr_info->set_name("haha");
  req_usr_info->set_passwd("123456");

  req.set_tp(protos::UserReq::regist);
  std::string s;
  req.SerializeToString(&s);
  // ====================================================

  protos::UserReq reqParsed;
  if (!reqParsed.ParseFromString(s)) {
    printf("xx\n");
  } else {
    reqParsed.SerializeToString(&s);
  }

  std::cout << s << std::endl;
}