#include "Socket.h"

#include <fcntl.h>
#include <strings.h>
#include <unistd.h>

// moduo
namespace sock_func {
void setNonBlockAndCloseOnExec(int sockfd) {
  // non-block
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);

  // close-on-exec
  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  // FIXME: check
}
}  // namespace sock_func

static const in_addr_t INADDR_ANY_ = INADDR_ANY;

InetAddress::InetAddress(uint16_t port) {
  bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = sock_func::hostToNetwork32(INADDR_ANY_);
  addr_.sin_port = sock_func::hostToNetwork16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
  bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = sock_func::hostToNetwork16(port);
  // check return value
  inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}

Socket::~Socket() { ::close(socketfd_); }
