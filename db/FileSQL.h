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
  ~FileSQL(){}
  int addUsrFile(string filename, string username, string path, int size,
                 string type, string time, int findex = 0,
                 int iflast = 0)  // path是要插入的文件夹位置
  {
    int fid = db_.getFid(username, path);
    string Npath = path + filename;
    db_.addUsrFile(filename, username, Npath, fid, findex, size, type, time,
                   iflast);
    return db_.getFid(username, Npath);
  };
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
  void deletUsrFile(string username, int fid)  // path是绝对路径
  {
    //这里需要返还fid
    db_.deleteUsrFile(username,
                      fid);  //需要通知ftp,删除文件，并从映射里解除文件
  };
  bool FileExist(string username, int fid) {
    return db_.ifexistUsrFile(username, fid);
  }
};

#endif