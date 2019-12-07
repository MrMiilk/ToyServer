#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>

namespace sock_func {
// inline uint64_t hostToNetwork64(uint64_t host64) { return htobe64(host64); }

inline uint32_t hostToNetwork32(uint32_t host32) { return htonl(host32); }

inline uint16_t hostToNetwork16(uint16_t host16) { return htons(host16); }

inline const struct sockaddr* addr_in2addr(const struct sockaddr_in* addr) {
  return static_cast<const struct sockaddr*>(static_cast<const void*>(addr));
}

inline struct sockaddr* addr_in2addr(struct sockaddr_in* addr) {
  return static_cast<struct sockaddr*>(static_cast<void*>(addr));
}
}  // namespace sock_func

class InetAddress {
 public:
  InetAddress() : addr_() {}
  explicit InetAddress(uint16_t);
  InetAddress(const std::string&, uint16_t);
  InetAddress(const struct sockaddr_in& addr) : addr_(addr) {}

  const struct sockaddr_in& get() const { return addr_; }
  void set(const struct sockaddr_in& addr) { addr_ = addr; }

 private:
  struct sockaddr_in addr_;
};

class Socket {
 public:
  explicit Socket(int socketfd) : socketfd_(socketfd) {}
  Socket(int domain, int type, int protocol = 0)
      : socketfd_(socket(domain, type, protocol)) {}
  ~Socket();
  int fd() const { return socketfd_; }

  void bind(const InetAddress&);
  void listen();
  // non-blocking and close-on-exec
  int accept(InetAddress&);
  void set_reuse_addr(bool on = true);

 private:
  int socketfd_;
};

#endif  // SOCKET_H_INCLUDED