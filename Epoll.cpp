#include "Epoll.h"
#include <assert.h>
#include <strings.h>
#include "Event.h"

void Epoll::poll(std::vector<Event*>& act_events, int timeout) {
  int num_prepared =
      ::epoll_wait(epfd_, &*events_.begin(), events_.size(), timeout);
  for (int i = 0; i < num_prepared; ++i) {
    Event* evn_ptr = static_cast<Event*>(events_[i].data.ptr);
    evn_ptr->set_events(events_[i].events);
    int fd = evn_ptr->get_fd();
    act_events.push_back(events_map_[fd]);
  }
  if(num_prepared == 0) {return;}
  else if(num_prepared < 0) {/* error */}
  else if (num_prepared == events_.size()) {
    events_.resize(2 * events_.size());
  }
}

void Epoll::update(Event* event_ptr) {
  if (event_ptr->state() == Event::NEW) {
    add_(event_ptr);
    event_ptr->added();
  } else if (event_ptr->state() == Event::ADDED) {
    update_(EPOLL_CTL_MOD, event_ptr);
  } else if (event_ptr->state() == Event::DEL) {
    remove_(event_ptr);
    event_ptr->del();
  }
}

void Epoll::add_(Event* event_ptr) {
  update_(EPOLL_CTL_ADD, event_ptr);
  events_map_[event_ptr->get_fd()] = event_ptr;
  event_ptr->added();
}

void Epoll::remove_(Event* event_ptr) {
  update_(EPOLL_CTL_DEL, event_ptr);
  events_map_.erase(event_ptr->get_fd());
}

void Epoll::update_(int opr, Event* event_ptr) {
  struct epoll_event evn;
  bzero(&evn, sizeof(evn));
  evn.events = event_ptr->events();
  evn.data.ptr = event_ptr;
  if (::epoll_ctl(epfd_, opr, event_ptr->get_fd(), &evn) < 0) {
    // perror
    assert(0);
  }
}