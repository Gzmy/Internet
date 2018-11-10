#include "comm.h"

MYSQL *connectMysql()
{
    MYSQL *myfd = mysql_init(NULL);

    if(mysql_real_connect(myfd, "127.0.0.1", "root", "zmy19980520", "web", 3306, NULL, 0) == NULL){
        cerr << "connect error" << endl;
    }else{
        cout << "connect success" << endl;
    }
    
    return myfd;
}

int insertmysql(MYSQL *myfd, const string& name, const string& sex, const string& hobby)
{
    //values("zhangsan", "man", "coding")
    string sql = "insert into conlist (name, email, telephone) values (\"";
    sql += name;
    sql += "\" , \"";
    sql += sex;
    sql += "\" , \"";
    sql += hobby;
    sql += "\")";
    
    cout << sql << endl;

    //char * query = "set name \'utf8mb4\'";
    //mysql_query(myfd, query);

    //return mysql_query(myfd, sql);
    return mysql_query(myfd, sql.c_str()); //转换C风格
}

void closeMysql(MYSQL *myfd)
{
    mysql_close(myfd);
}
