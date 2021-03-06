#ifndef _FILESQL_H_
#define _FILESQL_H_
#include "MyDB.h"
class FileSQL {
 private:
  MyDB db_;

 public:
  FileSQL(const std::string& host, const std::string& user,
          const std::string& password, const std::string& db_name)
      : db_() {
    db_.initDB(host, user, password, std::string());
    db_.createDB(db_name);
    db_.useDB(db_name);
    db_.createUsrTab();
  };
  int addUsrFile(const string& filename, const string& username,
                 const string& path, const int& size, const string& type,
                 const string& time, const int& findex = 0,
                 const int& iflast = 0)  // path是要插入的文件夹位置
  {
    printf("filename: %s\n", filename.c_str());
    printf("username: %s\n", username.c_str());
    printf("path: %s\n", path.c_str());
    int fid = db_.getFid(username, path);

    string Npath = path + filename;
    printf("%s\n", Npath);
    db_.addUsrFile(filename, username, Npath, fid, findex, size, type, time,
                   iflast);
    return db_.getFid(username, Npath);
  };
  void addRootFolder(const string& username) {
    db_.addUsrFile("/", username, "/", 0, 0, 0, "dir", "2019", 0);
  }
  void addFolder(string foldername, string username,
                 string path)  // path是要放入的绝对路径
  {
    int fid = db_.getFid(username, path);
    string Npath = path + foldername + '/';
    db_.addUsrFile(foldername, username, Npath, fid, 0, 0, "dir", " ", 0);
  };  //根据父文件夹绝对路径创建
  list<int> deletFolder(string username, int fid)  // path是删除文件夹的绝对路径
  {
    //!!!这里需要返还所有的子文件的fid,注意有目录
    list<int> files;
    files = db_.getSons(username, fid);
    db_.deleteSubFolder(username, fid);
    return files;
  }
  vector<int> getAssTable(const string& username) {
    vector<vector<int>> temp(db_.getAssTable(username));
    vector<int> ans;
    for (auto i = temp.cbegin(); i != temp.cend(); i++) {
      ans.insert(ans.end(), i->cbegin(), i->cend());
      ans.insert(ans.end(), -1);
    }
    return ans;
  }
  vector<MyDB::File> getFileTable(const string& username) {
    return db_.getFileTable(username);
  }
  void deletUsrFile(const string& username, const int& fid)  // path是绝对路径
  {
    //这里需要返还fid
    db_.deleteUsrFile(username,
                      fid);  //需要通知ftp,删除文件，并从映射里解除文件
  };
  bool FileExist(const string& username, const int& fid) {
    return db_.ifexistUsrFile(username, fid);
  }
  ~FileSQL() {}
};

#endif