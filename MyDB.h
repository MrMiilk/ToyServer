#ifndef _MYDB_H
#define _MYDB_H

#include </usr/include/mysql/mysql.h>
#include <string>
#include "public.h"

// mysql编译
// g++ -o MyDB MyDB.cpp MyDB.h public.h -L/usr/lib/mysql -lmysqlclient

class MyDB {
 public:
  MyDB();   //构造函数初始化变量
  ~MyDB();  //析构函数
  bool initDB(string host, string user, string password,
              string db_name);  //建立到运行host的一个MySQL数据库引擎的一个连接
  bool execSQL(string sql);  //执行mySQL语句并保存结果
  void showResult();         //打印结果
  list<string> getResult();  //获取结果
  // 基本操作
  void createDB(string dbname);         //创建数据库
  void deleteDB(string dbname);         //删除数据库
  void deleteAll(string tablename);     //删除表中全部数据
  void deleteTable(string tablename);   //删除一张表
  void useDB(string dbname);            //进入一个数据库
  void printResult(list<string> info);  //输出列表内容

  // 用户信息表（存储全部用户信息）
  // （用户名，密码，个人文件夹）
  void createUsrTab();  //创建用户信息表（Users）
  void addUsr(string username, string password,
              string foldername);   //添加用户，同时创建个人文件夹
  void deleteUsr(string username);  //删除用户，同时删除个人文件夹
  bool ifexistUsr(string username);               //用户是否已存在
  list<string> getUsrinfo(string username);       //获取用户信息
  string getPswd(string username);//获取用户密码
  void updatePswd(string username, string pswd);  //更改用户密码

  // 个人文件夹(文件标识符，文件名，所属用户，路径，大小，类型，时间)
  void createFolder(string foldername);   //创建该用户的文件夹
  void deleteFolder(string foldername);   //删除该个人文件夹
  string getFoldername(string username);  //通过用户名获取该用户个人文件夹名称
  void addUsrFile(int id, string filename, string username, string path,
                  int size, string type, string time);  //该用户添加一个文件
  void deleteUsrFile(string username,
                     int id);  //该用户删除一个文件（id还是name?）
  list<string> getUsrFile(string username,
                          int id);  //获取该用户该文件（id还是name?）
  bool ifexistUsrFile(string username, int id);  //该用户文件夹是否存在该文件
  list<string> getAllFile(string username);//获取该个人文件夹内全部文件

 private:
  MYSQL *connection;  // MySQL对象
  MYSQL_RES *result;  //结果集
  MYSQL_ROW row;      //行
  list<string> res;   //结果列表
};

#endif
