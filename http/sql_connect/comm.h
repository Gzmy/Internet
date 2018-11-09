#pragma once

#include <iostream>
#include <mysql.h>
#include <string>
using namespace std;

MYSQL *connectMysql();
int insertmysql(MYSQL *myfd, const string& name, \
                const string& sex, const string& hobby);
void closeMysql(MYSQL *myfd);
