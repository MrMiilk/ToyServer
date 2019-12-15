#include "MsgPaser.h"
#include <string.h>
#include <strings.h>
#include <iostream>

const std::string MsgPaser::spl_("%#%");

namespace {
enum RequestType { RG = 1, LG = 2, FILE_GET = 10 };
}

std::vector<std::string> MsgPaser::parse(const std::string& msg) {
  std::vector<std::string> msgs;
  std::size_t sz1 = 0, sz2;
  //   sz2 = msg.find(spl_);
  //   char tag[16];
  //   strncpy(tag, msg.c_str(), sz2);
  //   tag[sz2] = 0;
  //   int tp_r = atoi(tag);
  //   switch (tp_r) {
  //     case RequestType::RG:  // 注册
  //       msgs.emplace_back("RG");
  //       break;
  //     case RequestType::LG:  // 登录
  //       msgs.emplace_back("LG");
  //       break;
  //   }
  //   sz1 = sz2 + spl_.size();
  while ((sz2 = msg.find(spl_, sz1)) != std::string::npos) {
    msgs.push_back(msg.substr(sz1, sz2 - sz1));
    sz1 = sz2 + spl_.size();
  }
  msgs.emplace_back(msg.substr(sz1, msg.size()));
  return msgs;
}