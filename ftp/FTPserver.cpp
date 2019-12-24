#include "FTPserver.h"
#include "net/Epoll.h"
#include "net/Event.h"
#include "protos/MsgPaser.h"
#include "protos/ftp.pb.h"

// FTPserver 即为ftp部分的线程族 管理ftp的连接
// 自己监听读事件从而处理可能的关闭
// 使用队列接收向ftp 服务器发送的请求
void FTPserver::add(const std::vector<InetAddress>& ftp_addrs) {
  printf("connecting ftp ...\n");
  for (std::size_t i = 0; i < ftp_addrs.size(); ++i) {
    int s_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    const struct sockaddr_in& addr = ftp_addrs[i].get();
    if (::connect(s_fd, sock_func::addr_in2addr(&addr), sizeof(addr)) != 0) {
      // perror
      printf("connect to ftp client error\n");
      abort();
    }
    TCPconn* tmp = new TCPconn(s_fd);
    tmp->regist(&ep_);
    tmp->set_rd_cb(std::bind(&FTPserver::onReadble, this, std::placeholders::_1,
                             std::placeholders::_2));
    tmp->establish(true, true);
    servers_.emplace(tmp);
    // ==================================================
    protos::FtpQury aliveQ;
    aliveQ.set_tp(protos::FtpQury::CONNECT);
    aliveQ.set_key("Hello");
    // protos::FtpFileInfo* aliveQFInfo = aliveQ.mutable_fileinfo();
    // aliveQFInfo->set_name("None");
    // aliveQFInfo->set_path("/");
    // aliveQFInfo->set_fid(0);
    // aliveQFInfo->set_size(0);
    // aliveQ.mutable_userinfo()->set_name("hello");
    send(Parser::encode(aliveQ));  // 第一条验证消息
    // ==================================================
  }
}

void FTPserver::run() {
  std::vector<Event*> act_events;
  while (true) {
    ep_.poll(act_events);
    while (!act_events.empty()) {
      act_events.back()->handler();
      act_events.pop_back();
    }
  }
}

void FTPserver::send(const std::string& msg) {
  for (auto& s : servers_) {
    s->send(msg);
  }
}
void FTPserver::onReadble(TCPconn::conn_sptr_t ftp_cnn_sptr,
                          const std::string& msg) {
  // 处理连接关闭
  if (msg.size() == 0) {
    servers_.erase(ftp_cnn_sptr);
  }
  // 来自ftp server 的消息
  // 解析数据报并处理
  protos::FtpReq req = Parser::parseFtpReq(msg);
  if (req.tp() == protos::FtpReq::CONNECT) {
    printf("ftp connected... \n");
  }
  switch (req.tp()) {
    case protos::FtpReq::CONNECT:
      printf("ftp connected... \n");
      break;
    case protos::FtpReq::DELATED:
      printf("FIXME: delete from sql \n");
      break;
    default:
      break;
  }
}