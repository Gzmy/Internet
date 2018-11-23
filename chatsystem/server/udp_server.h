#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "data_pool.h"
using namespace std;

#define MAX 1024

class server{
public:
    server(int _port);
    void serverInit();
    void recvData(string &out);
    void sendData(const string &in, struct sockaddr_in &peer);
    void broadcastData();
    ~server();

private:
    int sock;
    int port;
    dataPool pool;
};
