#include "TCPconn.h"
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include "Event.h"
#include "Socket.h"

TCPconn::TCPconn(int sockfd)
    : msgs_(),
      sock_uptr_(new Socket(sockfd)),
      event_ptr_(),
      rd_cb_(),
      wr_cb_() {}
TCPconn::TCPconn(std::unique_ptr<Socket> sock_uptr)
    : msgs_(),
      sock_uptr_(sock_uptr.release()),
      event_ptr_(),
      rd_cb_(),
      wr_cb_() {}

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

void TCPconn::disable_rd() {
  event_ptr_->disable_rd();
  event_ptr_->update();
}
void TCPconn::disable_wr() {
  event_ptr_->disable_wr();
  event_ptr_->update();
}

void TCPconn::enable_rd() {
  event_ptr_->enable_rd();
  event_ptr_->update();
}
void TCPconn::enable_wr() {
  event_ptr_->enable_wr();
  event_ptr_->update();
}

// add read and pass to rd_cb_
void TCPconn::handle_rd() {
  char buf[BUFSIZE];

  // bzero(buf, BUFSIZE);
  ssize_t rd_sz = ::read(sock_uptr_->fd(), buf, BUFSIZE);
  if (rd_sz > 0) {
    rd_cb_(shared_from_this(), std::move(std::string(buf, rd_sz)));
    // set wr or save
  } else if (rd_sz == 0) {
    // closed by client
    rd_cb_(shared_from_this(), {});
  }  // else, error
}
void TCPconn::handle_wr() {
  ssize_t sz = 0;
  if (event_ptr_->wr_setted()) {
    std::string msg = msgs_.get();  // block
    sz = ::write(sock_uptr_->fd(), msg.c_str(), msg.size());
    if (sz == msg.size()) {
      // send all
      // call handler
      disable_wr();
    } else {
      // 出错 或者部分发送
    }
  }
}