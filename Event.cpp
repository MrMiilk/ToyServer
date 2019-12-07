#include "Event.h"

#include <sys/epoll.h>
#include "Epoll.h"

const int Event::RDEVENT = EPOLLIN | EPOLLPRI;
const int Event::WREVENT = EPOLLOUT;
const int Event::NOEVENT = 0;

uint32_t Event::events() const { return events_; }

void Event::set_events(int evns) { revents_ = evns; }

void Event::update() { epoll_ptr_->update(this); }

void Event::handler() {
  if ((revents_ & RDEVENT)) {
    assert(read_cb_func_);
    read_cb_func_();
  }
  if (revents_ & WREVENT) {
    assert(write_cb_func_);
    write_cb_func_();
  }
}