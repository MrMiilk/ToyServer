#ifndef RSACODER_H_INCLUDED
#define RSACODER_H_INCLUDED

#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

namespace encrypt_ {

class RSAEncoder {
 public:
  explicit RSAEncoder(const std::string& pem_file) : pRSAPublicKey(RSA_new()) {
    if (pem_file.empty() || pem_file.empty()) {
      /* error */
    }
    FILE* hPubKeyFile = fopen(pem_file.c_str(), "rb");
    if (hPubKeyFile == nullptr) { /* error */
    }
    if (PEM_read_RSA_PUBKEY(hPubKeyFile, &pRSAPublicKey, 0, 0) == NULL) {
      /* error */
    }
    fclose(hPubKeyFile);
  }
  ~RSAEncoder() {
    RSA_free(pRSAPublicKey);
    CRYPTO_cleanup_all_ex_data();
  }

  std::string encode(const std::string& data) const {
    std::string strRet;
    int nLen = RSA_size(pRSAPublicKey);
    char* pEncode = new char[nLen + 1];
    int ret = RSA_public_encrypt(
        data.length(), (const unsigned char*)data.c_str(),
        (unsigned char*)pEncode, pRSAPublicKey, RSA_PKCS1_PADDING);
    if (ret >= 0) {
      strRet = std::string(pEncode, ret);
    }
    delete[] pEncode;
    return std::move(strRet);
  }

 private:
  RSA* pRSAPublicKey;
};

class RSADecoder {
 public:
  explicit RSADecoder(const std::string& pem_file) : pRSAPrivateKey(RSA_new()) {
    if (pem_file.empty() || pem_file.empty()) {
      /* error */
    }
    FILE* hPriKeyFile = fopen(pem_file.c_str(), "rb");
    if (hPriKeyFile == nullptr) { /* error */
    }
    if (PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPrivateKey, 0, 0) == NULL) {
      /* error */
    }
    fclose(hPriKeyFile);
  }

  ~RSADecoder() {
    RSA_free(pRSAPrivateKey);
    CRYPTO_cleanup_all_ex_data();
  }

  std::string decode(const std::string& data) const {
    std::string strRet;
    int nLen = RSA_size(pRSAPrivateKey);
    char* pDecode = new char[nLen + 1];
    int ret = RSA_private_decrypt(
        data.length(), (const unsigned char*)data.c_str(),
        (unsigned char*)pDecode, pRSAPrivateKey, RSA_PKCS1_PADDING);
    if (ret >= 0) {
      strRet = std::string(pDecode, ret);
    }
    delete[] pDecode;
    return std::move(strRet);
  }

 private:
  RSA* pRSAPrivateKey;
};

}  // namespace encrypt_

#endif  // RSACODER_H_INCLUDED