#include "CliInfi.h"

const CliInfo_t& CliManager::get(const std::string& key) {
  MutexLockGuard lock(mutex_);
  CliInfo_t info;
  req_sto_t::iterator iter = requests_.find(key);
  if (iter != requests_.end()) {
    // 在请求中有
    
  }  // else
  return info;
}

void CliManager::insert(const CliInfo_t& info) {
  MutexLockGuard lock(mutex_);
  //
}