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
  // FIXME check
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

Socket::~Socket() {
  ::close(socketfd_);
}

void Socket::bind(const InetAddress& addrin) {
  const struct sockaddr_in& addr = addrin.get();
  if (::bind(socketfd_, sock_func::addr_in2addr(&addr), sizeof(addr))) {
    // perror
  }
}

void Socket::listen() {
  if (::listen(socketfd_, SOMAXCONN)) {
    // perror
  }
}

int Socket::accept(InetAddress& addr_net) {
  struct sockaddr_in addr_in;
  bzero(&addr_in, sizeof(addr_in));
  socklen_t addr_len = sizeof(addr_in);
  int resfd = ::accept(socketfd_, sock_func::addr_in2addr(&addr_in), &addr_len);
  sock_func::setNonBlockAndCloseOnExec(resfd);
  if (resfd >= 0) addr_net.set(addr_in);
  // else perror(nullptr);
  return resfd;
}

void Socket::set_reuse_addr(bool on) {
  int opt = on ? 1 : 0;
  ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}