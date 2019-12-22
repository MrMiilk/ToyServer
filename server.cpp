/*
 * sql password: 123456
 */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include "db/SQL.h"
#include "encrypt/RSA_.h"
#include "encrypt/md5.h"
#include "ftp/FTPserver.h"
#include "net/Epoll.h"
#include "net/Event.h"
#include "net/Socket.h"
#include "net/TCPconn.h"
#include "net/TCPserver.h"
#include "protos/MsgPaser.h"
#include "thread/Queue.h"
#include "thread/Thread.h"
#include "thread/ThreadPool.h"

#include <iostream>

using TCPconn_sptr_t = TCPserver::TCPconn_sptr_t;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void server_thr();
void cli_thr();
void ftp_thr();
void accept_cli(TCPconn_sptr_t server_ptr);
void cli_add_to_epoll(std::set<TCPconn_sptr_t>* conns, Epoll* epfd_ptr);
void cli_regist(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq);
void cli_login(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq);

// request type
enum RequestType { RG = 1, LG = 2, FILE_GET = 10 };

// ThreadPool thr_pool;
int new_cli_fd;
// Queue<std::unique_ptr<struct thr_info_t>> queue;
Queue<TCPconn_sptr_t> TCPConnQueue;
ThreadPool thread_pool;
FTPserver ftp_server;
UserSQL userSql("127.0.0.1", "root", "123456", "Users");
encrypt_::RSADecoder rsaDecoder("./base/rsa_private_key.pem");

// 增加非对称加密
// 增加加密解密线程池
int main() {
  new_cli_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
  Thread t1(server_thr);
  Thread t2(cli_thr);
  Thread t3(cli_thr);
  Thread t4(ftp_thr);

  t1.start();
  t2.start();
  t3.start();
  t4.start();
  thread_pool.start(3);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
}

// put into queue
void accept_cli(TCPconn_sptr_t server_ptr) {
  uint64_t i = 1;
  TCPConnQueue.put(server_ptr);
  write(new_cli_fd, &i, sizeof(uint64_t));
}

void userMsgHandle(TCPconn_sptr_t conn_sptr, const std::string& msg) {
  auto userReq = Parser::parseUserMsg(msg);
  switch (userReq.tp()) {
    case protos::UserReq::regist:
      cli_regist(conn_sptr, userReq);
      break;
    case protos::UserReq::login:
      cli_login(conn_sptr, userReq);
      break;
    case protos::UserReq::getFile:
      break;
    case protos::UserReq::uploadFile:
      break;
  }
}

void cli_regist(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  if (!userSql.userExist(userReq.userinfo().name())) {
    // 解密
    std::string passwd(rsaDecoder.decode(userReq.userinfo().passwd()));
    // userReq.userinfo.name 作为根目录名
    userSql.addUser(userReq.userinfo().name(), passwd, userReq.userinfo().name());
    conn_sptr->send("successful");
  } else {
    conn_sptr->send(std::move("user exist"));
  }
}

void cli_login(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  // 解密
  // 数据库
  // auto msgBag = MsgPaser::parse(msg);
  // if (!userSql.userExist(msgBag[1])) {
  //   conn_sptr->send("没有这个用户");
  // } else {
  //   if (userSql.getPwd(msgBag[1]).compare(msgBag[2]) == 0) {
  //     conn_sptr->send("logined");
  //     auto file_list = userSql.userHome(msgBag[1]);
  //     std::string send_msg;
  //     for (const auto& m : file_list) {
  //       send_msg += m;
  //     }
  //     conn_sptr->send(send_msg);
  //   } else {
  //     conn_sptr->send("pwd error");
  //   }
  // }
  if (!userSql.userExist(userReq.userinfo().name())) {
    conn_sptr->send("没有这个用户");
  } else {
    // 解密
    std::string passwd(rsaDecoder.decode(userReq.userinfo().passwd()));
    if (userSql.getPwd(userReq.userinfo().name()).compare(passwd) == 0) {
      // FIXME: 返回序列化的两个表
      conn_sptr->send("login success");
    } else {
      conn_sptr->send("pwd error");
    }
  }
}

void cli_get_file(TCPconn_sptr_t conn_sptr, const std::string& msg) {
  // 从消息中解码
  // 从数据库获取如何请求ftp
  // 向ftp发送通知
  ftp_server.send(std::move(msg));
  // 回复客户端如何请求ftp
}

// 客户端请求分发
void cli_msg_process(std::set<TCPconn_sptr_t>* conns, TCPconn_sptr_t conn_sptr,
                     const std::string& msg) {
  if (msg.size() == 0) {
    // close by client
    conns->erase(conn_sptr);
  } else {
    // handle
    thread_pool.run(std::bind(userMsgHandle, conn_sptr, std::move(msg)));
  }
}
// 新用户连接
void cli_add_to_epoll(std::set<TCPconn_sptr_t>* conns, Epoll* epfd_ptr) {
  // get info from queue
  // create new TCP
  uint64_t i = 0;
  TCPconn_sptr_t conn_sptr(TCPConnQueue.get_withlock());
  // 只有获取到queue中的内容才能读
  // FIXME: eventfd多线程安全?
  if (conn_sptr == nullptr) return;
  // read from new_cli_fd
  // FIXME: try-catch NOBLOCK
  read(new_cli_fd, &i, sizeof(uint64_t));
  conn_sptr->regist(epfd_ptr);
  conn_sptr->set_rd_cb(std::bind(cli_msg_process, conns, _1, _2));
  conn_sptr->enable_rd();
  // 应当自己管理TCPconn的生命周期
  conns->insert(std::move(conn_sptr));
}

void ftp_thr() {
  std::vector<InetAddress> ftp_addr({InetAddress("193.112.153.150", 10003)});
  ftp_server.add(ftp_addr);
  ftp_server.run();
}

void cli_thr() {
  Epoll cli_epoll;
  Event client_event(new_cli_fd, &cli_epoll);
  std::set<TCPconn_sptr_t> connections;
  client_event.set_rd_cb(std::bind(cli_add_to_epoll, &connections, &cli_epoll));
  client_event.enable_rd();
  client_event.update();

  std::vector<Event*> act_events;
  while (true) {
    cli_epoll.poll(act_events);
    while (!act_events.empty()) {
      act_events.back()->handler();
      act_events.pop_back();
    }
  }
}

void server_thr() {
  InetAddress server_addr("0.0.0.0", 8808);

  Epoll server_epoll;

  TCPserver server_tcp_conn(socket(AF_INET, SOCK_STREAM, 0), &server_epoll);
  server_tcp_conn.rg_conn_cb(accept_cli);
  server_tcp_conn.start(server_addr);

  std::vector<Event*> act_events;
  while (true) {
    server_epoll.poll(act_events);
    while (!act_events.empty()) {
      act_events.back()->handler();
      act_events.pop_back();
    }
  }
}