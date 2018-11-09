#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comm.h"

int main()
{
    char arg[1024] = {0};
    if(getenv("METHOD")){
        if(strcasecmp(getenv("METHOD"), "GET") == 0){
            strcpy(arg, getenv("QUERY_STRING"));
        }else{
            char c;
            int i = 0;
            int len = atoi(getenv("CONTENT_LENGTH"));
            for(; i < len; i++){
                read(0, &c, 1);
                arg[i] = c;
            }
            arg[i] = 0;
        }
    }
    
    //arg
    //name=wangwu&sex=man&hobby=coding
    //const string name = "lisi";
    //const string sex = "woman";
    //const string hobby = "sleep";

    strtok(arg, "=&");
    string name = strtok(NULL, "=&");
    strtok(NULL, "=&");
    string sex = strtok(NULL, "=&");
    strtok(NULL, "=&");
    string hobby = strtok(NULL, "=&");


    MYSQL *myfd = connectMysql();
    insertmysql(myfd, name, sex, hobby)
    closeMysql(myfd);
}
