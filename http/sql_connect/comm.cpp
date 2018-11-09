#include "comm.h"

MYSQL *connectMysql()
{
    MYSQL *myfd = mysql_init(NULL);

    if(mysql_real_connect(myfd, "127.0.0.1", "root", "passwd", "35_class", 3306, NULL, 0) == NULL){
        cerr << "connect error" << endl;
    }else{
        cout << "connect success" << endl;
    }
    
    return myfd;
}

int insertmysql(MYSQL *myfd, const string& name, const string& sex, const string& hobby)
{
    //values("zhangsan", "man", "coding")
    string sql = "insert into table-name (name, sex, hobby) values (\"";
    sql += name;
    sql += " \" , \" ";
    sql += sex;
    sql += " \" , \" ";
    sql += hobby;
    sql += "\")";
    
    cout << sql << endl;

    return mysql_query(myfd, sql.c_str()); //转换C风格
}

void closeMysql(MYSQL *myfd)
{
    mysql_close(myfd);
}
