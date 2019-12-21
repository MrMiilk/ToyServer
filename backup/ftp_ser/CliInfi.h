#ifndef CLIINFO_H_INCLUDED
#define CLIINFO_H_INCLUDED

#include <map>
#include <string>
#include <vector>
#include "MutexLock.h"

typedef struct CliInfo_ {
  std::string name;
  std::string filename;
  std::string fileLocal;
  std::string key;
  // 增加一个时间字段 定期对超时请求进行清理
} CliInfo_t;

class CliManager {
 public:
  CliManager() : mutex_(), requests_() {}

  void insert(const CliInfo_t&);
  const CliInfo_t& get(const std::string& key);

 private:
  using req_sto_t = std::map<std::string, std::vector<CliInfo_t>>;
  MutexLock mutex_;
  req_sto_t requests_;
};

#endif  // CLIINFO_H_INCLUDED