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
#include <utility>
#include <vector>
#include "db/FileSQL.h"
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
FileSQL fileSql("127.0.0.1", "root", "123456", "Users");
encrypt_::RSADecoder rsaDecoder("./base/rsa_private_key.pem");
std::vector<std::pair<std::string, uint32_t>> ftpAddrs{
    {"193.112.153.150", 10003}};

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

void cli_regist(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  try {
    if (!userSql.userExist(userReq.userinfo().name())) {
      // 解密
      std::string passwd(rsaDecoder.decode(userReq.userinfo().passwd()));
      // userReq.userinfo.name 作为根目录名
      userSql.addUser(userReq.userinfo().name(), passwd,
                      userReq.userinfo().name());
      fileSql.addRootFolder(userReq.userinfo().name());
      conn_sptr->send("successful");
    } else {
      conn_sptr->send(std::move("user exist"));
    }
  } catch (std::exception e) {
    printf("cli_upload fail, %s\n", e.what());
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
  try {
    protos::UserQury query;
    if (!userSql.userExist(userReq.userinfo().name())) {
      conn_sptr->send("没有这个用户");
    } else {
      // 解密
      std::string passwd(rsaDecoder.decode(userReq.userinfo().passwd()));
      if (userSql.getPwd(userReq.userinfo().name()).compare(passwd) == 0) {
        // FIXME: 返回序列化的两个表
        // printf(">>> bef\n");
        auto folders = fileSql.getFileTable(userReq.userinfo().name());
        auto table = fileSql.getAssTable(userReq.userinfo().name());
        // printf(">>>aft\n");
        for (const auto& f : folders) {
          auto folder = query.add_folder();
          folder->set_fid(f.fid);
          folder->set_filename(f.filename);
          folder->set_username(f.username);
          folder->set_path(f.path);
          folder->set_findex(f.findex);
          folder->set_sz(f.size);
          folder->set_tp(f.type);
          folder->set_time(f.time);
          folder->set_iflast(f.iflast);
        }
        auto qTb = query.mutable_filetable();
        for (int f : table) {
          qTb->add_entry(f);
        }
        query.set_tp(protos::UserQury::LOGIN);
        query.set_success(true);
        conn_sptr->send(Parser::encode(query));
      } else {
        conn_sptr->send("pwd error");
      }
    }
  } catch (std::exception e) {
    printf("cli_upload fail, %s\n", e.what());
  }
}

void cli_download(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  try {
    // 通知ftp
    protos::FtpQury ftpQury;
    ftpQury.set_tp(protos::FtpQury::DOWNLOAD);
    ftpQury.set_key(
        encrypt_::MD5(userReq.userinfo().name() + userReq.fileinfo().time()));
    auto f_info_ptr = ftpQury.mutable_fileinfo();
    f_info_ptr->set_name(userReq.fileinfo().filename());
    f_info_ptr->set_path(userReq.fileinfo().path());
    f_info_ptr->set_fid(userReq.fileinfo().fid());
    f_info_ptr->set_size(userReq.fileinfo().size());
    ftpQury.mutable_userinfo()->set_name(userReq.userinfo().name());
    // construct msg
    ftp_server.send(Parser::encode(ftpQury));
    // 回复客户端如何请求ftp
    protos::UserQury userQury;
    // construct msg
    userQury.set_tp(protos::UserQury::DOWNLOAD);
    for (const auto& addr : ftpAddrs) {
      auto ftps = userQury.add_ftps();
      ftps->set_ip(addr.first);
      ftps->set_port(addr.second);
    }
    conn_sptr->send(Parser::encode(userQury));
  } catch (std::exception e) {
    printf("cli_upload fail, %s\n", e.what());
  }
}

// 数据库查询fid
void cli_upload(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  std::string msg_;
  userReq.SerializeToString(&msg_);
  printf("filename: %s\n", userReq.fileinfo().filename().c_str());
  printf("msg_: %s\n", msg_.c_str());
  try {
    // 数据库
    int fid = fileSql.addUsrFile(
        userReq.fileinfo().filename(), userReq.userinfo().name(),
        userReq.fileinfo().path(), userReq.fileinfo().size(),
        userReq.fileinfo().tp(), userReq.fileinfo().time());
    // 通知ftp
    protos::FtpQury ftpQury;
    ftpQury.set_tp(protos::FtpQury::UPLOAD);
    ftpQury.set_key(
        encrypt_::MD5(userReq.userinfo().name() + userReq.fileinfo().time()));
    auto f_info_ptr = ftpQury.mutable_fileinfo();
    f_info_ptr->set_name(userReq.fileinfo().filename());
    f_info_ptr->set_path(userReq.fileinfo().path());
    f_info_ptr->set_fid(fid);
    f_info_ptr->set_size(userReq.fileinfo().size());
    ftpQury.mutable_userinfo()->set_name(userReq.userinfo().name());
    ftp_server.send(Parser::encode(ftpQury));
    // 回复客户端如何请求ftp
    protos::UserQury userQury;
    // construct msg
    userQury.set_tp(protos::UserQury::UPLOAD);
    for (const auto& addr : ftpAddrs) {
      auto ftps = userQury.add_ftps();
      ftps->set_ip(addr.first);
      ftps->set_port(addr.second);
      ftps->set_nonce(
          encrypt_::MD5(userReq.userinfo().name() + userReq.fileinfo().time()));
    }
    conn_sptr->send(Parser::encode(userQury));
  } catch (std::exception e) {
    printf("cli_upload fail, %s\n", e.what());
  }
}

void cli_mkdir(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  // 数据库添加folder
  protos::UserQury userQury;
  userQury.set_tp(protos::UserQury::ELSE);
  try {
    fileSql.addFolder(userReq.fileinfo().filename(), userReq.userinfo().name(),
                      userReq.fileinfo().path());
    // 回复客户端
    userQury.set_success(true);
  } catch (std::exception e) {
    printf("cli_mkdir fail, %s\n", e.what());
    userQury.set_success(false);
  }
  conn_sptr->send(Parser::encode(userQury));
}

void cli_delFile(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  protos::UserQury userQury;
  userQury.set_tp(protos::UserQury::ELSE);
  try {
    // 数据库删除
    fileSql.deletUsrFile(userReq.userinfo().name(), userReq.fileinfo().fid());
    // 通知ftp
    protos::FtpQury ftpQury;
    ftpQury.set_tp(protos::FtpQury::DELTEDFILES);
    ftpQury.mutable_userinfo()->set_name(userReq.userinfo().name());
    ftpQury.mutable_filelist()->add_fid(userReq.fileinfo().fid());
    ftp_server.send(Parser::encode(ftpQury));
    userQury.set_success(true);
  } catch (std::exception e) {
    printf("cli_delFile fail, %s\n", e.what());
    userQury.set_success(false);
  }
  conn_sptr->send(Parser::encode(userQury));
}

void cli_del_Folder(TCPconn_sptr_t conn_sptr, const protos::UserReq& userReq) {
  protos::UserQury userQury;
  protos::FtpQury ftpQury;
  userQury.set_tp(protos::UserQury::ELSE);
  try {
    auto file2del = fileSql.deletFolder(userReq.fileinfo().filename(),
                                        userReq.fileinfo().fid());
    ftpQury.set_tp(protos::FtpQury::DELTEDFILES);
    ftpQury.mutable_userinfo()->set_name(userReq.userinfo().name());
    for (int f : file2del) {
      ftpQury.mutable_filelist()->add_fid(f);
    }
    ftp_server.send(Parser::encode(ftpQury));
    // 回复客户端
    userQury.set_success(true);
  } catch (std::exception e) {
    printf("cli_del_Folder fail, %s\n", e.what());
    userQury.set_success(false);
  }
  conn_sptr->send(Parser::encode(userQury));
}

void userMsgHandle(TCPconn_sptr_t conn_sptr, const std::string& msg) {
  auto userReq = Parser::parseUserMsg(msg);
  switch (userReq.tp()) {
    case protos::UserReq::REGIST:
      cli_regist(conn_sptr, userReq);
      break;
    case protos::UserReq::LOGIN:
      cli_login(conn_sptr, userReq);
      break;
    case protos::UserReq::DOENLOAD:
      cli_download(conn_sptr, userReq);
      break;
    case protos::UserReq::UPLOAD:
      cli_upload(conn_sptr, userReq);
      break;
    case protos::UserReq::MKDIR:
      cli_mkdir(conn_sptr, userReq);
      break;
    case protos::UserReq::DELFILE:
      cli_delFile(conn_sptr, userReq);
      break;
    case protos::UserReq::DELFOLDER:
      cli_del_Folder(conn_sptr, userReq);
      break;
    default:
      break;
  }
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
  std::vector<InetAddress> ftp_addrs;
  for (const auto& addr : ftpAddrs) {
    ftp_addrs.emplace_back(InetAddress(addr.first, addr.second));
  }
  ftp_server.add(ftp_addrs);
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