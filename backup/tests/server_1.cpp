#include <strings.h>
#include <sys/eventfd.h>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include "Epoll.h"
#include "Event.h"
#include "Queue.h"
#include "Socket.h"
#include "Thread.h"
#include "ThreadPool.h"

class thr_info_t {
 public:
  InetAddress addr;
  int fd;
};
void server_thr();
void accept_cli(Socket* sockfd_ptr);
void cli_thr();
void cli_add_to_epoll(Epoll* epfd_ptr);
void cli_read(int fd, Event* ev_ptr);

// ThreadPool thr_pool;
int event_fd;
Queue<std::unique_ptr<struct thr_info_t>> queue;

// 调整各个类，特别是指针管理的类与指针
// event_fd 改为每次读取减1
// 使用多线程接受数据

// 增加非对称加密
// 增加加密解密线程池
int main() {
  event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
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

void accept_cli(Socket* sockfd_ptr) {  // server TCP
  // put into queue
  uint64_t i = 1;
  thr_info_t* client_info = new thr_info_t;
  client_info->fd = sockfd_ptr->accept(client_info->addr);
  queue.put(std::unique_ptr<struct thr_info_t>(client_info));
  write(event_fd, &i, sizeof(uint64_t));
}

void cli_read(int fd, Event* ev_ptr) {
  char buf[BUFSIZ];
  bzero(buf, BUFSIZ);
  int n = read(fd, buf, BUFSIZ);
  if (n == 0) {
    // close
    ev_ptr->disable_rd();
    close(fd);
    // printf("close a cli\n");
  } else {
    // printf("receved:%s\n", buf);
  }
}

void cli_add_to_epoll(Epoll* epfd_ptr) {
  // get info from queue
  // create new TCP
  uint64_t i;
  std::unique_ptr<struct thr_info_t> thr_info_ptr = queue.get_withlock();
  if (thr_info_ptr == nullptr) return;
  read(event_fd, &i, sizeof(uint64_t));
  // FIXME：内存溢出
  Event* client_rd_event = new Event(thr_info_ptr->fd, epfd_ptr);
  client_rd_event->set_rd_cb(
      std::bind(cli_read, thr_info_ptr->fd, client_rd_event));
  client_rd_event->enable_rd();
}

void cli_thr() {
  Epoll cli_epoll;
  Event client_event(event_fd, &cli_epoll);
  std::vector<Socket> socket_list;
  client_event.set_rd_cb(std::bind(cli_add_to_epoll, &cli_epoll));
  client_event.enable_rd();

  std::vector<std::shared_ptr<Event>> act_events;
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
  Socket server_socket(AF_INET, SOCK_STREAM);
  server_socket.set_reuse_addr();
  server_socket.bind(server_addr);
  server_socket.listen();

  Epoll server_epoll;
  Event server_accept_event(server_socket.fd(), &server_epoll);
  server_accept_event.set_rd_cb(std::bind(accept_cli, &server_socket));
  server_accept_event.enable_rd();

  std::vector<std::shared_ptr<Event>> act_events;
  while (true) {
    server_epoll.poll(act_events);
    while (!act_events.empty()) {
      // printf("new cli\n");
      act_events.back()->handler();
      act_events.pop_back();
    }
  }
}