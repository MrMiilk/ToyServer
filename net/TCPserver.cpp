#include "TCPserver.h"
#include "Event.h"
#include "Socket.h"

TCPserver::TCPserver(int sockfd, Epoll* ep_fd)
    : sock_uptr_(new Socket(sockfd)),
      event_ptr_(new Event(sockfd, ep_fd)),
      conn_cb_() {
  event_ptr_->set_rd_cb(std::bind(&TCPserver::handle_rd, this));
}

void TCPserver::start(const InetAddress& addr) {
  sock_uptr_->set_reuse_addr();
  sock_uptr_->bind(addr);
  sock_uptr_->listen();
  event_ptr_->enable_rd();
  event_ptr_->update();
}

void TCPserver::handle_rd() {
  // may neet this after, ... logging..
  InetAddress addr;
  int sock_fd = sock_uptr_->accept(addr);
  if (conn_cb_) {
    conn_cb_(std::move(std::shared_ptr<TCPconn>(new TCPconn(sock_fd))));
  }
}

// int TCPserver::accept(InetAddress& addr) { return sock_uptr_->accept(addr); }