#ifndef SQL_H_INCLUDED
#define SQL_H_INCLUDED
#include <list>
#include "MyDB.h"

class UserSQL {
 public:
  UserSQL(const std::string& host, const std::string& user,
          const std::string& password, const std::string& db_name)
      : db_() {
    db_.initDB(host, user, password, std::string());
    db_.createDB(db_name);
    db_.useDB(db_name);
    db_.createUsrTab();
  }

  void addUser(const std::string& username, const std::string& password,
               const std::string& foldername) {
    db_.addUsr(username, password, foldername);
  }

  void delteUser(const std::string& username) { db_.deleteUsr(username); }

  bool userExist(const std::string& username) {
    return db_.ifexistUsr(username);
  }

  std::list<std::string> userHome(const std::string& username) {
    return db_.getAllFile(username);
  }
  std::string getPwd(const std::string& username) {
    return db_.getPswd(username);
  }

 private:
  MyDB db_;
};

#endif  // SQL_H_INCLUDED