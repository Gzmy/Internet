#include "udp_client.h"

client::client(string _ip, int _port)
{
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr = inet_addr(_ip.c_str());
}

void client::clientInit()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        cerr << "socket error" << endl;
        exit(1);
    }
}

void client::recvData(string &out)
{
    char buf[MAX];
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    ssize_t s = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&peer, &len);
    if(s > 0){
        buf[s] = 0;
        out = buf;
    }
}

void client::sendData(const string &in)
{
    sendto(sock, in.c_str(), in.size(), 0, (struct sockaddr*)&server, sizeof(server));
}

client::~client()
{
    close(sock);
}
