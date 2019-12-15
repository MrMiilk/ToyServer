#include "MyDB.h"
#include <iostream>
using namespace std;

// 类被创建，最先运行构造函数初始化变量
MyDB::MyDB() {
  // 分配或初始化适合mysql_real_connect()的一个MYSQL对象。
  if ((connection = mysql_init(NULL)) == NULL) {
    cout << "Errorr" << mysql_error(connection) << endl;
    abort();
  }
}
// 析构函数,它会在每次删除所创建的对象时执行。
MyDB::~MyDB() {
  if (connection != NULL) {
    mysql_close(connection);
  }

  if (result != NULL) {
    mysql_free_result(result);
  }
}

// mysql_real_connect()试图建立到运行host的一个MySQL数据库引擎的一个连接
bool MyDB::initDB(string host, string user, string password, string db_name) {
  if ((connection = mysql_real_connect(connection, host.c_str(), user.c_str(),
                                       password.c_str(), db_name.c_str(), 0,
                                       NULL, 0)) == NULL) {
    cout << "Mysql Connecting Error" << mysql_error(connection) << endl;
    // abort();
    return false;
  }
  return true;
}

// 执行指向空终止的字符串query的SQL查询
bool MyDB::execSQL(string sql) {
  int i, j;
  if (mysql_query(connection, sql.c_str())) {
    cout << "Query Error" << mysql_error(connection) << endl;
    abort();
  } else {
    result = mysql_use_result(connection);  //获取结果集

    // mysql_use_result()  和  mysql_store_result()的差别

    // mysql_use_result()初始化一个结果集合的检索，但不真正将结果集合读入客户

    // mysql_store_result()读取一个到客户的查询的全部结果，分配一个MYSQL_RES结构，并且把结果放进这个结构中。
    // result = mysql_store_result(connection);
    /*while(row = mysql_fetch_row(result))
    {
        for(i=0;i<mysql_num_fields(result);++i)
        {
            cout<< row[i]<<" ";
        }
        cout<<endl;
    }*/
    res.clear();
    for (i = 0; i < mysql_field_count(connection); ++i)  //结果集的列数
    {
      row = mysql_fetch_row(result);  //检索一个结果集合的下一行
      if (row <= 0) break;
      for (j = 0; j < mysql_num_fields(result); ++j)  //返回列的数量
      {
        //cout << row[j] << " ";
        res.push_back(row[j]);  // list<stri用完结果集合，你必须调用mysql_free_result()。如果成功则一个MYSQL_RES结果结构ng>
                                // res
        //cout << "push_back" << endl;
      }
      //cout << endl;
    }
    //释放结果集的内存
    // 用完结果集合，必须调用mysql_free_result()。如果成功则一个MYSQL_RES结果结构
    mysql_free_result(result);
  }
  return true;
}

list<string> MyDB::getResult() {
  return res;  // list<string> res    结果列表
}

void MyDB::showResult() {
  cout << "showResult():";
  cout << res.size() << endl;
  list<string>::iterator it;  //列表迭代器
  for (it = res.begin(); it != res.end(); ++it) {
    cout << *it << endl;
  }
}

// 输出列表内容
void MyDB::printResult(list<string> info) {
  cout << "printResult():";
  cout << info.size() << endl;
  list<string>::iterator it;  //列表迭代器
  for (it = info.begin(); it != info.end(); ++it) {
    cout << *it << endl;
  }
}

// 创建数据库
void MyDB::createDB(string dbname) {
  string s = "create database if not exists " + dbname;
  execSQL(s);
}

// 删除数据库
void MyDB::deleteDB(string dbname) {
  string s = "drop database " + dbname;
  execSQL(s);
}

// 删除表中全部数据
void MyDB::deleteAll(string tablename) {
  string s = "delete from " + tablename;
  execSQL(s);
}

// 删除一张表
void MyDB::deleteTable(string tablename) {
  string s = "drop table " + tablename;
  execSQL(s);
}

// 使用数据库
void MyDB::useDB(string dbname) {
  string s = "use " + dbname;
  execSQL(s);
}

// 创建用户信息表(用户名，密码，个人文件夹)
void MyDB::createUsrTab() {
  string s =
      "create table if not exists Users(username char(32) not null primary "
      "key,password char(32) not null,foldername char(32) not "
      "null)engine=innodb default charset=UTF8";
  execSQL(s);
}

// 添加用户，同时创建个人文件夹
void MyDB::addUsr(string username, string password, string foldername) {
  string s = "insert into Users values('" + username + "','" + password +
             "','" + foldername + "')";
  execSQL(s);
  createFolder(foldername);
}

// 删除用户，同时删除个人文件夹
void MyDB::deleteUsr(string username) {
  string foldername = getFoldername(username);
  string s = "delete from Users where username='" + username + "'";
  execSQL(s);
  deleteFolder(foldername);
}

// 用户是否存在
bool MyDB::ifexistUsr(string username) {
  list<string> usrInfo = getUsrinfo(username);
  if (usrInfo.empty())
    return false;
  else
    return true;
}

// 获取用户信息
list<string> MyDB::getUsrinfo(string username) {
  string s = "select * from Users where username='" + username + "'";
  execSQL(s);
  return res;
}

// 获取用户密码
string MyDB::getPswd(string username)
{
  string s = "select password from Users where username='" +username+"'";
  execSQL(s);
  return res.back();
}

// 更改用户密码
void MyDB::updatePswd(string username, string pswd) {
  string s = "update Users set password='" + pswd + "' where username='" +
             username + "'";
  execSQL(s);
}

// 创建个人文件夹(文件标识符，文件名，所属用户，路径，大小，类型，时间)
void MyDB::createFolder(string foldername) {
  string s = "create table if not exists " + foldername +
             "(id int not null primary key,filename char(32) not null,username "
             "char(32) not null,path char(32) not null,size int not null,type "
             "char(16) not null,time char(16) not null)engine=innodb default "
             "charset=UTF8";
  execSQL(s);
}

// 删除个人文件夹
void MyDB::deleteFolder(string foldername) {
  string s = "drop table " + foldername;
  execSQL(s);
}

// 获取该用户个人文件夹名称
string MyDB::getFoldername(string username) {
  string s = "select * from Users where username='" + username + "'";
  execSQL(s);
  return res.back();
}

// 为该用户文件夹增添一个文件(文件标识符，文件名，所属用户，路径，大小，类型，时间)
void MyDB::addUsrFile(int id, string filename, string username, string path,
                      int size, string type, string time) {
  string foldername = getFoldername(username);
  string s = "insert into " + foldername + " values('" + to_string(id) + "','" +
             filename + "','" + username + "','" + path + "','" +
             to_string(size) + "','" + type + "','" + time + "')";
  execSQL(s);
}

// 删除用户文件夹内该文件
void MyDB::deleteUsrFile(string username, int id) {
  string foldername = getFoldername(username);
  string s = "delete from " + foldername + " where id= '" + to_string(id) + "'";
  execSQL(s);
}

// 获取文件信息
list<string> MyDB::getUsrFile(string username, int id) {
  string foldername = getFoldername(username);
  string s =
      "select * from " + foldername + " where id='" + to_string(id) + "'";
  execSQL(s);
  return res;
}

// 该用户文件夹是否存在该文件
bool MyDB::ifexistUsrFile(string username, int id) {
  string foldername = getFoldername(username);
  list<string> usrFile = getUsrFile(username, id);
  if (usrFile.empty())
    return false;
  else
    return true;
}

// 返回文件夹全部文件
list<string> MyDB::getAllFile(string username)
{
  string foldername = getFoldername(username);
  string s = "select * from "+foldername;
  execSQL(s);
  return res;
}

// int main()
// {
//     MyDB db;
//     db.initDB("localhost", "root", "hsb19990812", "mysql");

//     db.createDB("HelloPan");
//     db.useDB("HelloPan");
//     db.createUsrTab();
//     db.addUsr("HSB", "123456", "HSB_files");
//     db.updatePswd("HSB", "098765");
//     db.addUsr("hhh", "1111", "hhh_files");
//     db.deleteUsr("hhh");
//     db.printResult(db.getUsrinfo("HSB"));

//     if (db.ifexistUsr("hhh"))
//         printf("exist\n");
//     else
//         printf("not exist\n");

//     db.addUsrFile(10, "10_file", "HSB", "/a/b/10_file", 100, "txt", "10:00");
//     db.addUsrFile(11, "11_file", "HSB", "/a/b/10_file", 100, "txt", "11:00");

//     if (db.ifexistUsrFile("HSB", 10))
//         printf("file exist\n");
//     else
//         printf("file not exist\n");

//     db.deleteUsrFile("HSB", 11);
//     db.printResult(db.getUsrFile("HSB", 10));

//     return 0;
// }

/*构造函数具有多个字段 X、Y、Z 等需要进行初始化
    C::C( double a, double b, double c): X(a), Y(b), Z(c)
    {
    ....
    }
*/
