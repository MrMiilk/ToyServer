#ifndef EPOLL_H_INCLUDED
#define EPOLL_H_INCLUDED
#include <sys/epoll.h>
#include <unistd.h>
#include <map>
#include <memory>
#include <vector>

class Event;

class Epoll {
 public:
  Epoll()
      : epfd_(::epoll_create1(EPOLL_CLOEXEC)),
        events_(INITEVENTSIZE),
        events_map_() {}
  ~Epoll() { close(epfd_); }
  void update(Event*);  // call by Event
  //
  void poll(std::vector<Event*>&, int timeout = -1);

 private:
  void add_(Event*);
  void remove_(Event*);  // remove Event from epoll_fd
  void update_(int, Event*);
  static const int INITEVENTSIZE = 16;

  using event_map_t = std::map<int, Event*>;
  int epfd_;
  // 借助vector在内存中的排列和数组一致
  std::vector<struct epoll_event> events_;
  event_map_t events_map_;
};

#endif  // EPOLL_H_INCLUDED