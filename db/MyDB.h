#ifndef _MYDB_H
#define _MYDB_H

#include </usr/include/mysql/mysql.h>
#include <string>
#include <vector>
#include "public.h"

// mysql编译
// g++ -o MyDB MyDB.cpp MyDB.h public.h -L/usr/lib/mysql -lmysqlclient

class MyDB
{
public:
  MyDB();  //构造函数初始化变量
  ~MyDB(); //析构函数
  bool initDB(string host, string user, string password,
              string db_name); //建立到运行host的一个MySQL数据库引擎的一个连接
  bool execSQL(string sql);    //执行mySQL语句并保存结果
  void showResult();           //打印结果
  list<string> getResult();    //获取结果
  // 基本操作
  void createDB(string dbname);        //创建数据库
  void deleteDB(string dbname);        //删除数据库
  void deleteAll(string tablename);    //删除表中全部数据
  void deleteTable(string tablename);  //删除一张表
  void useDB(string dbname);           //进入一个数据库
  void printResult(list<string> info); //输出列表内容

  // 文件(夹)结构体
  typedef struct
  {
    int fid;
    string filename;
    string username;
    string path;
    unsigned int findex;
    unsigned int size;
    string type;
    string time;
    int iflast;
  }File;

  // 用户信息表（存储全部用户信息）(不用改)
  // （用户名，密码，个人文件夹）
  void createUsrTab(); //创建用户信息表（Users）
  void addUsr(string username, string password,
              string foldername);                //添加用户，同时创建个人文件夹
  void deleteUsr(string username);               //删除用户，同时删除个人文件夹
  bool ifexistUsr(string username);              //用户是否已存在
  list<string> getUsrinfo(string username);      //获取用户信息
  string getPswd(string username);               //获取用户密码
  void updatePswd(string username, string pswd); //更改用户密码

  // 个人文件夹(fid(本文件id，仅仅为了标记)  属性（文件名，所属用户，文件路径，位置索引findex（int），大小（int），类型，时间，(是否是最后一片)iflast（int））)
  void createFolder(string foldername);  //创建该用户的文件夹
  void createFolAss(string foldername);  //创建该文件夹的关联表，表名（foldername_ass） (id(自增),fid(文件id),pid(其父节点的id))，规定 目录(type=dir(或者自己改),文件名/folder),文件(type,文件名：直接文件名)
  void deleteFolder(string foldername);  //删除该个人文件夹
  void deleteFolAss(string foldername);  //删除文件夹的关联表
  string getFoldername(string username); //通过用户名获取该用户个人文件夹名称
  void addUsrFile(string filename, string username, string path, int pid, int index,
                  int size, string type, string time, int iflast); //该用户添加一个文件（pid是父节点的fid）
  void deleteUsrFile(string username, int fid);                    // 删除用户文件夹内该文件(删除关联表内项目，文件表中对应项目也删除)
  list<string> getUsrFile(string username,
                          int id);              //获取该用户该文件（id还是name?）
  bool ifexistUsrFile(string username, int id); //该用户文件夹是否存在该文件
  list<string> getAllFile(string username);     //获取该个人文件夹内全部文件
  // 获取文件的父节点的fid（即该文件的pid）
  int getParent(string username, int fid);
  // 获取一个节点的所有子节点
  list<int> getSons(string username, int pid);
  // 根据fid获取文件名
  string getFilename(string username, int fid);
  // 用绝对路径获取fid
  int getFid(string username, string path);
  // 返回该用户的关联表
  vector<vector<int>> getAssTable(string username);
  // 返回该用户的关联表
  vector<File> getFileTable(string username);

private:
  MYSQL *connection; // MySQL对象
  MYSQL_RES *result; //结果集
  MYSQL_ROW row;     //行
  list<string> res;  //结果列表
};

#endif
