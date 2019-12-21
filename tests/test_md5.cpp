#include <iostream>
#include "md5.h"
int main() {
  std::string test("hello world");
  std::cout << MD5(test) << std::endl;
}