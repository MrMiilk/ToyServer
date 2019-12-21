#ifndef MSGPASER_H_INCLUDED
#define MSGPASER_H_INCLUDED

#include <string>
#include <vector>

class MsgPaser {
 public:
  static std::vector<std::string> parse(const std::string&);

 private:
  static const std::string spl_;
};

#endif  // MSGPASER_H_INCLUDED
