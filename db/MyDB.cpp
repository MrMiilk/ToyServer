#include "MyDB.h"
#include <iostream>
using namespace std;

// 类被创建，最先运行构造函数初始化变量
MyDB::MyDB()
{
  // 分配或初始化适合mysql_real_connect()的一个MYSQL对象。
  if ((connection = mysql_init(NULL)) == NULL)
  {
    cout << "Errorr" << mysql_error(connection) << endl;
    abort();
  }
}
// 析构函数,它会在每次删除所创建的对象时执行。
MyDB::~MyDB()
{
  if (connection != NULL)
  {
    mysql_close(connection);
  }

  if (result != NULL)
  {
    mysql_free_result(result);
  }
}

// mysql_real_connect()试图建立到运行host的一个MySQL数据库引擎的一个连接
bool MyDB::initDB(string host, string user, string password, string db_name)
{
  if ((connection = mysql_real_connect(connection, host.c_str(), user.c_str(),
                                       password.c_str(), db_name.c_str(), 0,
                                       NULL, 0)) == NULL)
  {
    cout << "Mysql Connecting Error" << mysql_error(connection) << endl;
    // abort();
    return false;
  }
  return true;
}

// 执行指向空终止的字符串query的SQL查询
bool MyDB::execSQL(string sql)
{
  int i, j;
  if (!res.empty())
    res.clear();
  if (mysql_query(connection, sql.c_str()))
  {
    cout << "Query Error " << mysql_error(connection) << endl;
    exit(1);
  }
  else
  {
    result = mysql_use_result(connection); //获取结果集

    if (mysql_field_count(connection) > 0)
    {
      //获取下一行
      while (1)
      {
        row = mysql_fetch_row(result);
        if (row <= 0)
        {
          break;
        }
        for (j = 0; j < mysql_num_fields(result); ++j) //结果集列数
        {
          // cout << row[j] << " ";
          res.push_back(row[j]);
          // cout << "push_back" << endl;
        }
      }
    }
    //释放结果集的内存
    mysql_free_result(result);
  }
  return true;
}

list<string> MyDB::getResult()
{
  return res; // list<string> res    结果列表
}

void MyDB::showResult()
{
  cout << "showResult():";
  cout << res.size() << endl;
  list<string>::iterator it; //列表迭代器
  for (it = res.begin(); it != res.end(); ++it)
  {
    cout << *it << endl;
  }
}

// 输出列表内容
void MyDB::printResult(list<string> info)
{
  cout << "printResult():";
  cout << info.size() << endl;
  list<string>::iterator it; //列表迭代器
  for (it = info.begin(); it != info.end(); ++it)
  {
    cout << *it << endl;
  }
}

// 创建数据库
void MyDB::createDB(string dbname)
{
  string s = "create database if not exists " + dbname;
  execSQL(s);
}

// 删除数据库
void MyDB::deleteDB(string dbname)
{
  string s = "drop database " + dbname;
  execSQL(s);
}

// 删除表中全部数据
void MyDB::deleteAll(string tablename)
{
  string s = "delete from " + tablename;
  execSQL(s);
}

// 删除一张表
void MyDB::deleteTable(string tablename)
{
  string s = "drop table " + tablename;
  execSQL(s);
}

// 使用数据库
void MyDB::useDB(string dbname)
{
  string s = "use " + dbname;
  execSQL(s);
}

// 创建用户信息表(用户名，密码，个人文件夹)
void MyDB::createUsrTab()
{
  string s =
      "create table if not exists Users(username char(32) not null primary "
      "key,password char(32) not null,foldername char(32) not "
      "null)engine=innodb default charset=UTF8";
  execSQL(s);
}

// 添加用户，同时创建个人文件夹
void MyDB::addUsr(string username, string password, string foldername)
{
  string s = "insert into Users values('" + username + "','" + password +
             "','" + foldername + "')";
  execSQL(s);
  createFolAss(foldername);
  createFolder(foldername);
}

// 删除用户，同时删除个人文件夹和关联表
void MyDB::deleteUsr(string username)
{
  string foldername = getFoldername(username);
  string s = "delete from Users where username='" + username + "'";
  execSQL(s);
  deleteFolAss(foldername);
  deleteFolder(foldername);
}

// 用户是否存在
bool MyDB::ifexistUsr(string username)
{
  list<string> usrInfo = getUsrinfo(username);
  if (usrInfo.empty())
    return false;
  else
    return true;
}

// 获取用户信息
list<string> MyDB::getUsrinfo(string username)
{
  string s = "select * from Users where username='" + username + "'";
  execSQL(s);
  return res;
}

// 获取用户密码
string MyDB::getPswd(string username)
{
  string s = "select password from Users where username='" + username + "'";
  execSQL(s);
  return res.back();
}

// 更改用户密码
void MyDB::updatePswd(string username, string pswd)
{
  string s = "update Users set password='" + pswd + "' where username='" +
             username + "'";
  execSQL(s);
}

// 个人文件夹表(fid(本文件id，仅仅为了标记)  属性（文件名，所属用户，文件路径，位置索引findex（int），大小（int），类型，时间，(是否是最后一片)iflast（int））)
void MyDB::createFolder(string foldername)
{
  string s = "create table if not exists " + foldername +
             "(fid int not null,filename char(32) not null,username "
             "char(32) not null,path char(32) not null,findex int unsigned not null,size int unsigned not null,type "
             "char(16) not null,time char(16) not null,iflast int not null,foreign key (fid) references " +
             foldername + "_ass(fid) on delete cascade on update cascade)engine=innodb default "
                          "charset=UTF8";
  execSQL(s);
}

//创建该文件夹的关联表，表名（foldername_ass） (id(自增),fid(文件id),pid(其父节点的id))
void MyDB::createFolAss(string foldername)
{
  string tableName = foldername + "_ass";
  string s = "create table if not exists " + tableName + "(fid int primary key not null,pid int not null)engine=innodb default charset=UTF8";
  execSQL(s);
}

// 删除关联表
void MyDB::deleteFolAss(string foldername)
{
  string s = "drop table " + foldername + "_ass";
  execSQL(s);
}

// 删除个人文件夹
void MyDB::deleteFolder(string foldername)
{
  string s = "drop table " + foldername;
  execSQL(s);
  deleteFolAss(foldername);
}

// 获取该用户个人文件夹名称
string MyDB::getFoldername(string username)
{
  string s = "select * from Users where username='" + username + "'";
  execSQL(s);
  return res.back();
}

// 为该用户文件夹增添一个文件(id(自增，与项目无关)，属性（文件名，所属用户，位置索引（int），大小（int），类型，时间，iflast（int））)，输入父亲节点id
// 该文件夹的关联表，表名（foldername_ass） (id(自增),fid(文件id),pid(其父节点的id))
void MyDB::addUsrFile(string filename, string username, string path, int pid, int findex, int size, string type, string time, int iflast)
{
  string foldername = getFoldername(username);

  //  给关联表增加一条
  // select max(id) from foldername
  // 先加入根目录，pid=0，fid=1
  int fid;
  string s = "";
  if (pid == 0)
  {
    fid = 1;
  }
  else
  {
    s = "select max(fid) from " + foldername;
    execSQL(s);
    fid = stoi(*res.begin()) + 1; //文件id
  }
  s = "insert into " + foldername + "_ass values('" + to_string(fid) + "','" + to_string(pid) + "')";
  execSQL(s);

  // 给用户文件夹增添一条
  s = "insert into " + foldername + " values('" + to_string(fid) + "','" + filename + "','" +
      username + "','" + path + "','" + to_string(findex) + "','" +
      to_string(size) + "','" + type + "','" + time + "','" + to_string(iflast) + "')";
  execSQL(s);
}

// 删除用户文件夹内该文件(删除关联表内项目，文件表中对应项目也删除)
void MyDB::deleteUsrFile(string username, int fid)
{
  string foldername = getFoldername(username);
  string s = "delete from " + foldername + "_ass where fid= '" + to_string(fid) + "'";
  execSQL(s);
}

// 获取文件信息
list<string> MyDB::getUsrFile(string username, int fid)
{
  string foldername = getFoldername(username);
  string s =
      "select * from " + foldername + " where fid='" + to_string(fid) + "'";
  execSQL(s);
  return res;
}

// 该用户文件夹是否存在该文件
bool MyDB::ifexistUsrFile(string username, int fid)
{
  string foldername = getFoldername(username);
  list<string> usrFile = getUsrFile(username, fid);
  if (usrFile.empty())
    return false;
  else
    return true;
}

// 获取父节点id
int MyDB::getParent(string username, int fid)
{
  string foldername = getFoldername(username);
  string s = "select pid from " + foldername + "_ass where fid='" + to_string(fid) + "'";
  execSQL(s);
  return stoi(*res.begin());
}

// 获取一个节点的所有子节点
list<int> MyDB::getSons(string username, int pid)
{
  string foldename = getFoldername(username);
  string s = "select fid from " + foldename + "_ass where pid='" + to_string(pid) + "'";
  execSQL(s);
  list<int> sons;
  while (!res.empty())
  {
    sons.push_back(stoi(*res.begin()));
    res.pop_front();
  }
  return sons;
}
// 根据fid获取文件名
string MyDB::getFilename(string username, int fid)
{
  string foldername = getFoldername(username);
  string s = "select filename from " + foldername + " where fid='" + to_string(fid) + "'";
  execSQL(s);
  return *res.begin();
}

// 用绝对路径获取fid
int MyDB::getFid(string username, string path)
{
  string foldername = getFoldername(username);
  string s = "select fid from " + foldername + " where path='" + path + "'";
  execSQL(s);
  if (!res.empty())
    return stoi(*res.begin());
  else
    return -1;
}

// 返回文件夹全部文件
list<string> MyDB::getAllFile(string username)
{
  string foldername = getFoldername(username);
  string s = "select * from " + foldername;
  execSQL(s);
  return res;
}

// 返回该用户的关联表
vector<vector<int>> MyDB::getAssTable(string username)
{
  string foldername = getFoldername(username);
  string s = "select * from " + foldername + "_ass";
  execSQL(s);
  vector<vector<int>> assTable; //((fid,pid),(fid,pid))
  vector<int> row;              //(fid,pid)
  list<string>::iterator it;    //列表迭代器
  for (it = res.begin(); it != res.end(); ++it)
  {
    row.push_back(stoi(*it));
    ++it;
    row.push_back(stoi(*it));
    assTable.push_back(row);
    row.clear();
  }
  return assTable;
}

// 返回该用户的文件表
vector<MyDB::File> MyDB::getFileTable(string username)
{
  string foldername = getFoldername(username);
  string s = "select * from " + foldername;
  execSQL(s);
  vector<MyDB::File> Files;
  MyDB::File file;
  list<string>::iterator it;
  for (it = res.begin(); it != res.end(); ++it)
  {
    file.fid = stoi(*it);
    ++it;
    file.filename = *it;
    ++it;
    file.username = *it;
    ++it;
    file.path = *it;
    ++it;
    file.findex = stoi(*it);
    ++it;
    file.size = stoi(*it);
    ++it;
    file.type = *it;
    ++it;
    file.time = *it;
    ++it;
    file.iflast = stoi(*it);

    Files.push_back(file);
  }
  return Files;
}

// 删除子文件夹,fid是该子文件夹的fid
void MyDB::deleteSubFolder(string username, int fid)
{
  list<int> sons = getSons(username, fid);
  list<int>::iterator it;
  for (it = sons.begin(); it != sons.end(); ++it)
  {
    if (!getSons(username, *it).empty())
    {
      deleteSubFolder(username, *it);
    }
    deleteUsrFile(username,*it);
  }
  deleteUsrFile(username,fid);
}

// 测试用函数
// 打印关联表
void printasstable(vector<vector<int>> asstable)
{
  vector<vector<int>>::iterator it;
  for (it = asstable.begin(); it != asstable.end(); ++it)
  {
    // printf("%d,%d\n",(*it)[0],(*it)[1]);
    cout << (*it)[0] << "," << (*it)[1] << endl;
  }
}
// 打印文件表
void printfiletable(vector<MyDB::File> filetable)
{
  MyDB::File file;
  vector<MyDB::File>::iterator it;
  for (it = filetable.begin(); it != filetable.end(); ++it)
  {
    file = *it;
    cout << file.fid << "," << file.filename << "," << file.username << "," << file.path << "," << file.findex << "," << file.size << "," << file.type << "," << file.time << "," << file.iflast << endl;
  }
}

// int main()
// {
//   MyDB db;
//   db.initDB("localhost", "root", "12580", "mysql");
//   db.deleteDB("HelloPan");
//   db.createDB("HelloPan");
//   db.useDB("HelloPan");
//   db.createUsrTab();
//   db.addUsr("HSB", "123456", "HSBFolder");
//   db.updatePswd("HSB", "098765");
//   db.addUsrFile("/", "HSB", "/", 0, 123, 234, "dir", "2019", 0); //增加根目录
//   db.addUsrFile("hsb", "HSB", "/hsb/", 1, 123, 234, "dir", "2019", 0); //新建子目录
//   db.addUsrFile("hhh", "HSB", "/hhh/", 1, 123, 234, "dir", "2019", 0); //新建子目录
//   string path = "/hsb/";
//   int pid = db.getFid("HSB", path);
//   db.addUsrFile("hsb_1", "HSB", "/hsb/hsb_1", pid, 123, 234, "dir", "2019", 0); //新建文件
//   db.deleteUsrFile("HSB",4);
//   db.printResult(db.getUsrFile("HSB",3));
//   return 0;
// }

/*构造函数具有多个字段 X、Y、Z 等需要进行初始化
    C::C( double a, double b, double c): X(a), Y(b), Z(c)
    {
    ....
    }
*/
