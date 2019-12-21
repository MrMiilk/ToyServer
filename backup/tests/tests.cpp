#include <iostream>
#include <string>
#include "RSA_.h"

int main() {
  RSAEncoder encoder("./rsa_public_key.pem");
  std::string text_encoded = encoder.encode("test");
  text_encoded = encoder.encode("test3");

  RSADecoder decoder("./rsa_private_key.pem");
  std::string res = decoder.decode(text_encoded);
  std::cout << res << std::endl;
}