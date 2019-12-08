#include <strings.h>
#include <sys/eventfd.h>
#include <time.h>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include "Epoll.h"
#include "Event.h"
#include "Queue.h"
#include "Socket.h"
#include "TCPconn.h"
#include "TCPserver.h"
#include "Thread.h"
#include "ThreadPool.h"

using TCPconn_sptr_t = TCPserver::TCPconn_sptr_t;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void server_thr();
void accept_cli(std::shared_ptr<TCPconn> server_ptr);
void cli_thr();
void cli_add_to_epoll(std::set<TCPconn_sptr_t>* conns, Epoll* epfd_ptr);
void cli_msg_process(std::set<TCPconn_sptr_t>* conns, TCPconn_sptr_t conn_sptr,
                     const char* buf, std::size_t sz);

// ThreadPool thr_pool;
int event_fd;
// Queue<std::unique_ptr<struct thr_info_t>> queue;
Queue<std::shared_ptr<TCPconn>> queue;

// 增加非对称加密
// 增加加密解密线程池
int main() {
  event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);  //
  Thread t1(server_thr, "1");
  Thread t2(cli_thr, "2");
  Thread t3(cli_thr, "3");
  // Thread t4(cli_thr);

  t1.start();
  t2.start();
  t3.start();
  // t4.start();
  t1.join();
  t2.join();
  t3.join();
  // t4.join();
}

// FIXME: replace std::shared_ptr<TCPconn> with using ...
void accept_cli(std::shared_ptr<TCPconn> server_ptr) {  // server TCP
  uint64_t i = 1;
  // put into queue
  queue.put(server_ptr);
  write(event_fd, &i, sizeof(uint64_t));
  // printf("new clli\n");
}

void cli_msg_process(std::set<TCPconn_sptr_t>* conns, TCPconn_sptr_t conn_sptr,
                     const char* buf, std::size_t sz) {
  if (buf == nullptr) {
    // close by client
    // printf("conn_sptr.count: %ld\n", conn_sptr.use_count());
    // printf("conns.size: %d\n", conns->size());
    conns->erase(conn_sptr);
    // printf("conns.size: %d\n", conns->size());
    // printf("conn_sptr.count: %ld\n", conn_sptr.use_count());
    // sleep(1);
  } else {
    // printf("%s\n", buf);
  }
  // data process
  // maybe regist wr
  // wr函数的conn应该在这里提供，这样能处理半关闭??
}

void cli_add_to_epoll(std::set<TCPconn_sptr_t>* conns, Epoll* epfd_ptr) {
  // get info from queue
  // create new TCP
  uint64_t i = 0;
  std::shared_ptr<TCPconn> conn_sptr(queue.get_withlock());
  // 只有获取到queue中的内容才能读
  // FIXME: eventfd多线程安全?
  if (conn_sptr == nullptr) return;
  // read from event_fd
  // FIXME: try-catch NOBLOCK
  read(event_fd, &i, sizeof(uint64_t));
  // printf("new cli accept, i = %lu\n", i);
  conn_sptr->regist(epfd_ptr);
  conn_sptr->set_rd_cb(std::bind(cli_msg_process, conns, _1, _2, _3));
  conn_sptr->establish();
  // 应当自己管理TCPconn的生命周期
  conns->insert(std::move(conn_sptr));
  // printf("cli_add_to_epoll, conns.size: %d\n", conns->size());
}

void cli_thr() {
  Epoll cli_epoll;
  Event client_event(event_fd, &cli_epoll);
  std::set<TCPconn_sptr_t> connections;
  std::vector<Socket> socket_list;
  client_event.set_rd_cb(std::bind(cli_add_to_epoll, &connections, &cli_epoll));
  client_event.enable_rd();
  client_event.update();

  std::vector<Event*> act_events;
  while (true) {
    cli_epoll.poll(act_events);
    while (!act_events.empty()) {
      // printf("cli_thr, conns.size1: %d\n", connections.size());
      // printf("new cli or new info\n");
      act_events.back()->handler();
      // printf("cli_thr, conns.size2: %d\n", connections.size());
      act_events.pop_back();
    }
  }
}

void server_thr() {
  InetAddress server_addr("0.0.0.0", 8808);
  //   Socket server_socket(AF_INET, SOCK_STREAM);
  //   server_socket.set_reuse_addr();
  //   server_socket.bind(server_addr);
  //   server_socket.listen();

  Epoll server_epoll;
  //   Event server_accept_event(server_socket.fd(), &server_epoll);
  //   server_accept_event.set_rd_cb(std::bind(accept_cli, &server_socket));
  //   server_accept_event.enable_rd();`

  TCPserver server_tcp_conn(socket(AF_INET, SOCK_STREAM, 0), &server_epoll);
  // server_tcp_conn.set_rd_cb(std::bind(accept_cli, &server_tcp_conn));
  server_tcp_conn.rg_conn_cb(accept_cli);
  server_tcp_conn.start(server_addr);

  std::vector<Event*> act_events;
  while (true) {
    server_epoll.poll(act_events);
    while (!act_events.empty()) {
      // printf("new cli\n");
      // FIXME: try-catch, raw pointer maybe illegal
      act_events.back()->handler();
      act_events.pop_back();
    }
  }
}