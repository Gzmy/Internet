#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
using namespace std;

#define MAX 1024

class client{
public:
    client(string _ip, int _port);
    void clientInit();
    void recvData(string &out);
    void sendData(const string &in);
    ~client();

private:
    int sock;
    struct sockaddr_in server;
};
