#include "TCPconn.h"
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include "Event.h"
#include "Socket.h"

TCPconn::TCPconn(int sockfd) : sock_uptr_(new Socket(sockfd)), event_ptr_() {}

void TCPconn::regist(Epoll* ep_fd) {
  event_ptr_.reset(new Event(sock_uptr_->fd(), ep_fd));
  event_ptr_->set_rd_cb(std::bind(&TCPconn::handle_rd, this));
  event_ptr_->set_wr_cb(std::bind(&TCPconn::handle_wr, this));
}

void TCPconn::establish(bool rd, bool wr) {
  assert(event_ptr_);
  if (rd) {
    event_ptr_->enable_rd();
  }
  if (wr) {
    event_ptr_->enable_wr();
  }
  event_ptr_->update();
}

void TCPconn::disable_rd() { event_ptr_->disable_rd(); }
void TCPconn::disable_wr() { event_ptr_->disable_wr(); }

// add read and pass to rd_cb_
void TCPconn::handle_rd() {
  char buf[BUFSIZE];

  // bzero(buf, BUFSIZE);
  std::size_t rd_sz = ::read(sock_uptr_->fd(), buf, BUFSIZE);
  if (rd_sz > 0) {
    rd_cb_(shared_from_this(), buf, rd_sz);
    // set wr or save
  } else if (rd_sz == 0) {
    // closed by client
    rd_cb_(shared_from_this(), nullptr, rd_sz);
  }  // else, error
}
void TCPconn::handle_wr() {
  // wr_cb_();
}